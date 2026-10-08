#pragma once
#include "extended.hpp"
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

// Small PC24/nearest kernels for audited RNG and quest boundaries. These are
// not a general x87 implementation. Keep the original arithmetic for other modes.
namespace pas::legacy_numeric {

// Round a normal binary64 value to 24 significant bits without shrinking its
// exponent range to binary32. Callers establish that prior arithmetic cannot
// double-round across a PC24 midpoint. Zero and nonfinite values pass through.
inline double round24(double value) {
  auto bits = std::bit_cast<std::uint64_t>(value);
  if ((bits & 0x7fffffffffffffffULL) == 0 ||
      (bits & 0x7ff0000000000000ULL) == 0x7ff0000000000000ULL)
    return value;
  constexpr std::uint64_t mask = (1ULL << 29) - 1;
  const auto tail = bits & mask;
  auto rounded = bits & ~mask;
  if (tail > (1ULL << 28) ||
      (tail == (1ULL << 28) && ((bits >> 29) & 1)))
    rounded += 1ULL << 29;
  return std::bit_cast<double>(rounded);
}

inline double random_bound(double value) {
  if (!std::isfinite(value)) return value;
  // At this scale, both signs round to exactly one after the +1 operation.
  if (std::abs(value) < 0x1p-40) return 1;
  // Every such product is outside the subsequent signed Int64 conversion.
  if (std::abs(value) > 0x1p54)
    return std::copysign(std::numeric_limits<double>::infinity(), value);
  const auto bits = std::bit_cast<std::uint64_t>(value);
  const auto significand = (bits & 0x000fffffffffffffULL) | (1ULL << 52);
  // Exact 53-bit operand * 1000 fits in 63 bits. Round this product directly,
  // avoiding a binary64 multiply's possible rounding onto a PC24 midpoint.
  const std::uint64_t product = significand * 1000;
  const int shift = 64 - std::countl_zero(product) - 24;
  auto mantissa = product >> shift;
  const auto tail = product & ((1ULL << shift) - 1);
  const auto half = 1ULL << (shift - 1);
  mantissa += tail > half || (tail == half && (mantissa & 1));
  double scaled = std::ldexp(double(mantissa),
      int((bits >> 52) & 0x7ff) - 1023 - 52 + shift);
  if (std::signbit(value)) scaled = -scaled;
  // Two 24-bit operands (the product and 1) do not have the mixed-width
  // midpoint problem above. Retain the separate multiply/add rounding points.
  return round24(scaled + 1.0);
}

// Exact PC24 quotient for signed 32-bit inputs, including large denominators
// for which the RNG's simpler double-then-float proof does not apply.
inline double signed_quotient(std::int32_t numerator, std::int32_t denominator) {
  const bool negative = (numerator < 0) != (denominator < 0);
  const std::uint64_t n = numerator < 0 ? -std::int64_t(numerator) : numerator;
  const std::uint64_t d = denominator < 0 ? -std::int64_t(denominator) : denominator;
  if (!n) return negative ? -0.0 : 0.0;
  int exponent = std::countl_zero(d) - std::countl_zero(n);
  if (exponent >= 0 ? n < (d << exponent) : (n << -exponent) < d)
    --exponent;
  const int shift = 23 - exponent;
  const auto dividend = shift >= 0 ? n << shift : n;
  const auto divisor = shift >= 0 ? d : d << -shift;
  auto mantissa = dividend / divisor;
  const auto remainder = dividend % divisor;
  mantissa += remainder * 2 > divisor ||
              (remainder * 2 == divisor && (mantissa & 1));
  const double result = std::ldexp(double(mantissa), exponent - 23);
  return negative ? -result : result;
}

// TCPVariant calls this only within +/-2e9. Its stored input has at most the
// original 64 significant bits. Compare the distance to the PC24 midpoint
// before adding epsilon: binary64 addition would lose the positive bias for
// inputs such as 16777217. Scaling and the nearby subtraction are exact.
inline long double quest_value(long double value) {
  if (std::abs(value) < 0.25L) return 0; // final nearest integer is zero
  int exponent;
  std::frexp(value, &exponent);
  const auto step = std::ldexp(1.0L, exponent - 24);
  const auto lower = std::ldexp(std::floor(std::ldexp(value, 24 - exponent)),
                                exponent - 24);
  const auto distance = (lower + step * 0.5L) - value;
  // In this domain the input/midpoint lattice is coarser than the difference
  // between the binary64 and binary80 encodings of 1e-11.
  return distance > 1.0e-11L ? lower : lower + step;
}

// Preserve the field's low bits before a binary64 host conversion. This is only
// the audited finite +/-2e9 quest boundary, not general binary80 arithmetic.
#if !SRHD_NATIVE_REALS
inline bool quest_value_binary80(const Extended& value, long double& result) {
  std::uint64_t significand;
  std::uint16_t encoded;
  std::memcpy(&significand, value.bytes, 8);
  std::memcpy(&encoded, value.bytes + 8, 2);
  const unsigned stored_exponent = encoded & 0x7fff;
  if (stored_exponent == 0x7fff ||
      (stored_exponent && !(significand >> 63)))
    return false;
  const int exponent = int(stored_exponent) - 16383;
  if (exponent > 30 ||
      (exponent == 30 && significand > (std::uint64_t(2000000000) << 33)))
    return false;
  if (exponent < -2) { result = 0; return true; }

  // An input lattice unit is 2^(E-63). The binary80 encoding of 1e-11 has
  // the same integer-unit floor as exact 1/10^11 throughout E=-2..30.
  // For the two shifts above 63, split 2^64's quotient/remainder to avoid
  // requiring a wider integer type on the Win32 target.
  constexpr std::uint64_t divisor = 100000000000ULL;
  const unsigned shift = unsigned(63 - exponent);
  const auto bias_units = shift < 64 ? (1ULL << shift) / divisor :
      (std::numeric_limits<std::uint64_t>::max() / divisor) * (1ULL << (shift - 64)) +
      ((std::numeric_limits<std::uint64_t>::max() % divisor + 1) *
       (1ULL << (shift - 64))) / divisor;
  constexpr std::uint64_t half = 1ULL << 39;
  const auto tail = significand & ((1ULL << 40) - 1);
  auto rounded = significand >> 40;
  const bool negative = (encoded & 0x8000) != 0;
  if (negative ? (tail > half && tail - half > bias_units) :
                 (tail >= half || half - tail <= bias_units))
    ++rounded;
  result = std::ldexp(static_cast<long double>(rounded), exponent - 23);
  if (negative) result = -result;
  return true;
}
#endif
} // namespace pas::legacy_numeric
