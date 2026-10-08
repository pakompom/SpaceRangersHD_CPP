#pragma once
#include "runtime.hpp"
#if SRHD_NATIVE_REALS
#include "native_reals.hpp"
#else
#include "legacy_numeric.hpp"
#include <cfenv>
// Default Win32 Delphi evaluates reals with the x87 64-bit significand. The game
// selects 24-bit precision in the renderer and simulation/generation workers.
// Non-x87 hosts emulate only the audited boundaries below, not every operation.
#include <bit>
#include <cmath>
#include <cstdint>
namespace pas {
inline thread_local std::uint16_t fpu_control_word = 0x1332;
// x87 reads reserved bit 6 as one, even when it was clear in FLDCW's operand.
inline std::uint16_t get_fpu_control() { return fpu_control_word | 0x40; }
inline bool legacy_pc24_nearest() { return (fpu_control_word & 0x0f00) == 0; }
inline long double random_real(Cardinal *seed) {
#pragma STDC FENV_ACCESS ON
  const auto value = next_random(seed);
  if (legacy_pc24_nearest())
    return legacy_numeric::round24(double(value) * 0x1p-32);
  return static_cast<long double>(value) * 0x1p-32L;
}
inline long double real_arctan2(long double y, long double x) {
#pragma STDC FENV_ACCESS ON
  return std::atan2(y, x);
}
inline void set_fpu_control(std::uint16_t value) {
  fpu_control_word = value;
#if defined(__i386__) || defined(__x86_64__)
  // C++ exceptions represent Delphi floating errors. Hardware traps stay masked
  // so they cannot escape through a foreign SEH boundary.
  const std::uint16_t native = value | 0x3f;
  asm volatile("fnclex; fldcw %0" : : "m"(native));
#if defined(SRHD_PORTABLE_REALS) && SRHD_PORTABLE_REALS
  const int rounding[] = {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO};
  std::fesetround(rounding[(value >> 10) & 3]);
#endif
#else
  const int rounding[] = {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO};
  std::fesetround(rounding[(value >> 10) & 3]);
  std::feclearexcept(FE_ALL_EXCEPT);
#endif
}
long double real_divide(long double left, long double right);
inline std::int64_t real_integer(long double value, bool truncate) {
#pragma STDC FENV_ACCESS ON
  value = truncate ? std::trunc(value) : std::nearbyint(value);
  if (!std::isfinite(value) || value < -9223372036854775808.0L || value >= 9223372036854775808.0L) {
    if (!(fpu_control_word & 1)) raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
    return std::numeric_limits<std::int64_t>::min();
  }
  return std::int64_t(value);
}
std::int64_t real_trunc(long double value);
std::int64_t real_round(long double value);
namespace detail {
std::int64_t extended_integer(const Extended& value, bool truncate);
#if LDBL_MANT_DIG == 53 && !(defined(__i386__) || defined(__x86_64__))
inline bool zero_integer_operand(const Extended& value) {
  std::uint64_t significand;
  std::uint16_t encoded;
  std::memcpy(&significand, value.bytes, 8);
  std::memcpy(&encoded, value.bytes + 8, 2);
  return !significand && !(encoded & 0x7fff);
}
inline bool binary64_integer_operand(const Extended& value) {
  std::uint64_t significand;
  std::uint16_t encoded;
  std::memcpy(&significand, value.bytes, 8);
  std::memcpy(&encoded, value.bytes + 8, 2);
  const unsigned exponent = encoded & 0x7fff;
  return exponent - 0x3c01u <= 0x7fdu
      && (significand & 0x80000000000007ffull) == 0x8000000000000000ull;
}
#endif
template<bool Truncate> inline std::int64_t extended_integer_dispatch(const Extended& value) {
#if LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__))
  // Unsupported encodings need the raw validation when errors are enabled.
  if (!(fpu_control_word & 1) && !(value.bytes[7] & 0x80)
      && (value.bytes[8] || (value.bytes[9] & 0x7f)))
    return extended_integer(value, Truncate);
#elif LDBL_MANT_DIG == 53 && !(defined(__i386__) || defined(__x86_64__))
  // Exactly representable normal operands can use the native scalar helper.
  // Retained binary80 low bits, subnormals and exceptional encodings keep the
  // full converter out of the ordinary path.
  if (zero_integer_operand(value)) return 0;
  if (!binary64_integer_operand(value)) return extended_integer(value, Truncate);
#else
  return extended_integer(value, Truncate);
#endif
  if constexpr (Truncate) return real_trunc(static_cast<long double>(value));
  else return real_round(static_cast<long double>(value));
}
}
inline std::int64_t real_round(const Extended& value) {
  return detail::extended_integer_dispatch<false>(value);
}
inline std::int64_t real_trunc(const Extended& value) {
  return detail::extended_integer_dispatch<true>(value);
}
inline long double real_sqrt(long double value) {
#pragma STDC FENV_ACCESS ON
  if (value < 0 && !(fpu_control_word & 1)) raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
  return std::sqrt(value);
}
// Extended saved-inline selection. The scalar overloads below have the
// opposite unordered policy; all overloads choose the second operand on ties.
template<class T> T real_min(T a, T b) { return a < b ? a : b; }
template<class T> T real_max(T a, T b) { return a <= b ? b : a; }
template<class T> T real_min_scalar(T a, T b) { return quiet_scalar_nan(b <= a ? b : a); }
template<class T> T real_max_scalar(T a, T b) { return quiet_scalar_nan(b < a ? a : b); }
inline long double real_sin(long double value) {
#pragma STDC FENV_ACCESS ON
  if (std::isinf(value) && !(fpu_control_word & 1))
    raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
#if LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__))
  // Delphi 2007 System._Sin uses the x87 primitive, including its range limit.
  asm volatile("fsin" : "+t"(value));
  return value;
#else
  return std::sin(value);
#endif
}
inline long double real_cos(long double value) {
#pragma STDC FENV_ACCESS ON
  if (std::isinf(value) && !(fpu_control_word & 1))
    raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
#if LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__))
  // Keep the selected RTL's result, rather than libm's different range reduction.
  asm volatile("fcos" : "+t"(value));
  return value;
#else
  return std::cos(value);
#endif
}
inline long double real_ln(long double value) {
#pragma STDC FENV_ACCESS ON
  if (value < 0 && !(fpu_control_word & 1))
    raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
  if (value == 0 && !(fpu_control_word & 4))
    raise(make_exception<ZeroDivide>(AnsiString("Floating point division by zero")));
  return std::log(value);
}
inline long double real_exp(long double value) {
#pragma STDC FENV_ACCESS ON
  const long double result = std::exp(value);
  if (std::isinf(result) && std::isfinite(value) && !(fpu_control_word & 8))
    raise(make_exception<Overflow>(AnsiString("Floating point overflow")));
  return result;
}
inline long double real_arctan(long double value) { return std::atan(value); }
inline long double real_frac(long double value) {
  if (std::isinf(value)) {
    if (!(fpu_control_word & 1))
      raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
    return std::numeric_limits<long double>::quiet_NaN();
  }
  long double whole;
  return std::modf(value, &whole);
}
inline long double int_power(long double base, std::int32_t exponent) {
#pragma STDC FENV_ACCESS ON
  auto remaining = exponent < 0 ? 0U - std::uint32_t(exponent) : std::uint32_t(exponent);
  long double result = 1;
  while (remaining) {
    if (remaining & 1) result *= base;
    remaining >>= 1;
    if (remaining) base *= base;
  }
  return exponent < 0 ? real_divide(1, result) : result;
}
inline long double real_power(long double base, long double exponent) {
#pragma STDC FENV_ACCESS ON
  if (std::fabs(exponent) <= 2147483647.0L && std::trunc(exponent) == exponent)
    return int_power(base, std::int32_t(exponent));
  // The selected Math.Power returns zero for a zero base on its noninteger path.
  if (base == 0) return base;
  if (base < 0 && !(fpu_control_word & 1))
    raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
  return std::exp(exponent * std::log(base));
}
inline bool is_nan(float value) {
  return (std::bit_cast<std::uint32_t>(value) & 0x7fffffffU) > 0x7f800000U;
}
inline bool is_nan(double value) {
  return (std::bit_cast<std::uint64_t>(value) & 0x7fffffffffffffffULL) > 0x7ff0000000000000ULL;
}

// Explicit ports of the game's RNG/quest arithmetic. Keep other control modes
// on the original expressions; these helpers are not general PC24 lowering.
inline std::int64_t legacy_random_bound(double value) {
#pragma STDC FENV_ACCESS ON
  if (legacy_pc24_nearest()) return real_trunc(legacy_numeric::random_bound(value));
  return real_trunc(static_cast<long double>(value) * 1000.0L + 1.0L);
}
inline double legacy_random_milli(std::int32_t value) {
#pragma STDC FENV_ACCESS ON
  if (legacy_pc24_nearest()) return legacy_numeric::round24(double(value) / 1000.0);
  return real_divide(value, 1000.0L);
}
inline double legacy_seed_fraction(std::uint32_t seed) {
#pragma STDC FENV_ACCESS ON
  if (legacy_pc24_nearest()) {
    // UInt32/10011001 cannot double-round across a PC24 midpoint. The seed
    // stays an exact integer until division, including its upper 32-bit values.
    const double quotient = legacy_numeric::round24(double(seed) / 10011001.0);
    return quotient - std::trunc(quotient);
  }
  return real_frac(real_divide(seed, 10011001.0L));
}
inline double legacy_fractional_quotient(std::int32_t numerator, std::int32_t denominator) {
#pragma STDC FENV_ACCESS ON
  if (legacy_pc24_nearest() && denominator != 0) {
    const double quotient = legacy_numeric::signed_quotient(numerator, denominator);
    return quotient - std::trunc(quotient);
  }
  return real_frac(real_divide(numerator, denominator));
}
inline std::int64_t legacy_quest_round(long double value) {
#pragma STDC FENV_ACCESS ON
#if !(LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__)))
  if (legacy_pc24_nearest() && std::isfinite(value) && std::abs(value) <= 2.0e9L)
    return real_round(legacy_numeric::quest_value(value));
#endif
  return real_round(value + 1.0e-11L);
}
inline std::int64_t legacy_quest_round(const Extended& value) {
#pragma STDC FENV_ACCESS ON
#if !(LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__)))
  long double rounded;
  if (legacy_pc24_nearest() && legacy_numeric::quest_value_binary80(value, rounded))
    return real_round(rounded);
#endif
  return legacy_quest_round(static_cast<long double>(value));
}
}
#endif
