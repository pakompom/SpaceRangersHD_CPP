#pragma once
#include <cfloat>
#include <bit>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

namespace pas {
// Delphi's Single/Double Min/Max results pass through an x87 load/store,
// which quiets a selected signaling NaN without changing its sign or payload.
// Extended selection instead copies its original ten bytes unchanged.
template<class T> inline T quiet_scalar_nan(T value) {
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>);
  using Bits = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
  auto bits = std::bit_cast<Bits>(value);
  constexpr Bits infinity = Bits(sizeof(T) == 4 ? 0x7f800000ULL : 0x7ff0000000000000ULL);
  constexpr Bits magnitude = ~(Bits(1) << (sizeof(T) * 8 - 1));
  if ((bits & magnitude) > infinity)
    bits |= Bits(sizeof(T) == 4 ? 0x00400000ULL : 0x0008000000000000ULL);
  return std::bit_cast<T>(bits);
}
#ifndef SRHD_NATIVE_REALS
#define SRHD_NATIVE_REALS 0
#endif
#if SRHD_NATIVE_REALS
using Real = double;
using Extended = double;
#else
using Real = long double;
static_assert(std::endian::native == std::endian::little,
              "Delphi binary80 storage requires a little-endian host");
// Delphi fields and stream payloads always hold little-endian binary80. Host
// arithmetic remains long double, so a binary64 host does not emulate x87's
// precision or exponent range merely by preserving this storage format.
// Non-x87 host conversions round to nearest-even independently of FPU mode;
// they preserve field bytes, rather than emulating the x87 exception/status ABI.
struct alignas(2) Extended {
  unsigned char bytes[10]{};
  Extended() = default;
  static constexpr Extended from_bits(std::uint64_t significand, std::uint16_t exponent) {
    Extended result;
    for (unsigned i = 0; i < 8; ++i) result.bytes[i] = significand >> (8 * i);
    result.bytes[8] = exponent;
    result.bytes[9] = exponent >> 8;
    return result;
  }
  // Integer loads retain their full significand independently of host real
  // width, arithmetic precision control and rounding mode, just as FILD does.
  template<class T> requires (std::is_integral_v<T> && sizeof(T) <= sizeof(std::uint64_t))
  Extended(T value) {
    bool negative = false;
    if constexpr (std::is_signed_v<T>) negative = value < 0;
    const auto bits = std::uint64_t(value);
    const auto magnitude = negative ? std::uint64_t(0) - bits : bits;
    std::uint64_t significand = 0;
    std::uint16_t encoded = 0;
    if (magnitude) {
      const int leading = std::countl_zero(magnitude);
      significand = magnitude << leading;
      encoded = std::uint16_t(16383 + 63 - leading) | (negative ? 0x8000 : 0);
    }
    std::memcpy(bytes, &significand, 8);
    std::memcpy(bytes + 8, &encoded, 2);
  }
#if LDBL_MANT_DIG == 64
  // The i686 C++ ABI pads long double to twelve bytes. Copy its payload only.
  Extended(long double value) { std::memcpy(bytes, &value, 10); }
  operator long double() const {
    long double value{};
    std::memcpy(&value, bytes, 10);
    return value;
  }
#elif LDBL_MANT_DIG == 53
  static_assert(sizeof(long double) == sizeof(double));
  Extended(long double value) {
    const auto bits = std::bit_cast<std::uint64_t>(double(value));
    const auto fraction = bits & 0x000fffffffffffffull;
    const unsigned exponent = (bits >> 52) & 0x7ff;
    std::uint16_t encoded = std::uint16_t((bits >> 48) & 0x8000);
    std::uint64_t significand = 0;
    if (exponent == 0x7ff) {
      encoded |= 0x7fff;
      significand = 0x8000000000000000ull | (fraction << 11);
    } else if (exponent) {
      encoded |= std::uint16_t(int(exponent) - 1023 + 16383);
      significand = (fraction | (1ull << 52)) << 11;
    } else if (fraction) {
      const int leading = std::countl_zero(fraction);
      encoded |= std::uint16_t(16383 - 1074 + 63 - leading);
      significand = fraction << leading;
    }
    std::memcpy(bytes, &significand, 8);
    std::memcpy(bytes + 8, &encoded, 2);
  }
  static std::uint64_t round_right(std::uint64_t bits, unsigned shift) {
    if (shift > 64) return 0;
    if (shift == 64) return bits > (1ull << 63);
    if (!shift) return bits;
    const auto result = bits >> shift;
    const auto remainder = bits & ((1ull << shift) - 1);
    const auto half = 1ull << (shift - 1);
    return result + (remainder > half || (remainder == half && (result & 1)));
  }
  operator long double() const {
    std::uint64_t significand;
    std::uint16_t encoded;
    std::memcpy(&significand, bytes, 8);
    std::memcpy(&encoded, bytes + 8, 2);
    const auto sign = std::uint64_t(encoded & 0x8000) << 48;
    const unsigned stored_exponent = encoded & 0x7fff;
    // Ordinary finite values already have their explicit integer bit set.
    // Rebase the exponent and round the eleven discarded bits directly, with
    // mantissa carry naturally advancing the exponent (including to infinity).
    // Keep all subnormal, unsupported and special encodings on the full path.
    if (stored_exponent - 0x3c01u <= 0x7fdu && (significand >> 63)) {
      const auto fraction = (significand >> 11) & 0x000fffffffffffffull;
      const auto carry = ((significand & 0x7ff) + 0x3ff + (fraction & 1)) >> 11;
      const auto magnitude = (std::uint64_t(stored_exponent - 0x3c00u) << 52)
                           + fraction + carry;
      return std::bit_cast<double>(sign | magnitude);
    }
    return decode_binary64_slow(significand, encoded);
  }
private:
  // Keep normalization and unusual encodings out of the ordinary load path.
  // Preserve nearest-even conversion for subnormals and extreme exponents.
#if defined(__GNUC__) || defined(__clang__)
  __attribute__((noinline))
#elif defined(_MSC_VER)
  __declspec(noinline)
#endif
  static long double decode_binary64_slow(std::uint64_t significand, std::uint16_t encoded) {
    const auto sign = std::uint64_t(encoded & 0x8000) << 48;
    const unsigned stored_exponent = encoded & 0x7fff;
    if (stored_exponent && !(significand >> 63))
      return std::bit_cast<double>(sign | 0x7ff8000000000000ull);
    if (stored_exponent == 0x7fff) {
      auto fraction = (significand & 0x7fffffffffffffffull) >> 11;
      if (significand & 0x7fffffffffffffffull) fraction |= 1ull << 51;
      return std::bit_cast<double>(sign | 0x7ff0000000000000ull | fraction);
    }
    if (!significand) return std::bit_cast<double>(sign);
    const int leading = std::countl_zero(significand);
    significand <<= leading;
    int exponent = int(stored_exponent ? stored_exponent : 1) - 16383 - leading;
    // Explicit integer-bit storage includes pseudo-denormals; normalizing it
    // also makes the binary64 subnormal/normal rounding boundary straightforward.
    if (exponent < -1022)
      return std::bit_cast<double>(sign | round_right(significand, unsigned(-1011 - exponent)));
    auto rounded = round_right(significand, 11);
    if (rounded == (1ull << 53)) { rounded >>= 1; ++exponent; }
    if (exponent > 1023) return std::bit_cast<double>(sign | 0x7ff0000000000000ull);
    return std::bit_cast<double>(sign | (std::uint64_t(exponent + 1023) << 52) |
                                 (rounded & 0x000fffffffffffffull));
  }
public:
#elif LDBL_MANT_DIG == 113
  static_assert(sizeof(long double) == 16);
  Extended(long double value) {
    using Bits = unsigned __int128;
    Bits bits;
    std::memcpy(&bits, &value, 16);
    auto exponent = std::uint16_t((bits >> 112) & 0x7fff);
    const auto sign = std::uint16_t((bits >> 112) & 0x8000);
    const Bits fraction = bits & ((Bits(1) << 112) - 1);
    std::uint64_t significand;
    if (exponent == 0x7fff) {
      significand = (1ull << 63) | (fraction ? 1ull << 62 : 0);
    } else {
      const Bits full = fraction | (exponent ? Bits(1) << 112 : 0);
      const Bits tail = full & ((Bits(1) << 49) - 1);
      Bits rounded = full >> 49;
      // Round a binary128 host value to the field's 64-bit significand.
      if (tail > (Bits(1) << 48) || (tail == (Bits(1) << 48) && (rounded & 1)))
        ++rounded;
      if (rounded >> 64) { rounded >>= 1; ++exponent; }
      else if (!exponent && (rounded >> 63)) exponent = 1;
      significand = std::uint64_t(rounded);
    }
    const std::uint16_t sign_exponent = sign | exponent;
    std::memcpy(bytes, &significand, 8);
    std::memcpy(bytes + 8, &sign_exponent, 2);
  }
  operator long double() const {
    using Bits = unsigned __int128;
    std::uint64_t significand;
    std::uint16_t sign_exponent;
    std::memcpy(&significand, bytes, 8);
    std::memcpy(&sign_exponent, bytes + 8, 2);
    unsigned exponent = sign_exponent & 0x7fff;
    if (exponent && !(significand >> 63))
      return std::numeric_limits<long double>::quiet_NaN();
    if (!exponent && (significand >> 63)) exponent = 1;
    const Bits bits = (Bits(sign_exponent & 0x8000) << 112) | (Bits(exponent) << 112) |
                      (Bits(significand & 0x7fffffffffffffffull) << 49);
    long double value;
    std::memcpy(&value, &bits, 16);
    return value;
  }
#else
#error Unsupported host long double representation for Delphi binary80 storage
#endif
};
static_assert(sizeof(Extended) == 10 && alignof(Extended) == 2);
static_assert(std::is_trivially_copyable_v<Extended>);
inline long double abs(Extended value) {
  const auto real = static_cast<long double>(value);
  return real < 0 ? -real : real;
}
#endif
template<class T> inline constexpr bool is_real_v =
    std::is_floating_point_v<T> || std::is_same_v<T, Extended>;
}
