#pragma once
#include "runtime.hpp"
#include <cfenv>
// Default Win32 Delphi evaluates reals with the x87 64-bit significand. The game
// selects 24-bit precision in the renderer. The two worker-thread setup seams
// and general strict compiler floating-environment access remain to be ported.
// The shared runtime selects startup precision. Floating exception identities
// remain a separate RTL surface; native arm64 cannot validate x87 precision.
#include <bit>
#include <cmath>
#include <cstdint>
namespace pas {
inline thread_local std::uint16_t fpu_control_word = 0x1332;
// x87 reads reserved bit 6 as one, even when it was clear in FLDCW's operand.
inline std::uint16_t get_fpu_control() { return fpu_control_word | 0x40; }
inline long double random_real(Cardinal *seed) {
#pragma STDC FENV_ACCESS ON
  return static_cast<long double>(next_random(seed)) * 0x1p-32L;
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
inline long double real_sqrt(long double value) {
#pragma STDC FENV_ACCESS ON
  if (value < 0 && !(fpu_control_word & 1)) raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
  return std::sqrt(value);
}
// The selected RTL chooses its second operand for ties and unordered values.
// std::min/max choose the first, which differs for signed zero and NaN.
template<class T> T real_min(T a, T b) { return a < b ? a : b; }
template<class T> T real_max(T a, T b) { return a > b ? a : b; }
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
}
