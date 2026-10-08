#pragma once
#include "runtime.hpp"
#include "legacy_numeric.hpp"
#include "pc24_midpoint.hpp"
#include <cmath>

// Fixed PC24/nearest arithmetic with binary64 storage. Generated expressions
// preserve their individual rounding boundaries; mixed-width operations and
// RNG/quest conversions use the correction helpers below. This is not a full
// x87 emulator: control-word changes and floating traps are not implemented.
namespace pas {
inline std::uint16_t get_fpu_control() { return 0x137f; }
inline void set_fpu_control(std::uint16_t) {}
inline double random_real(Cardinal* seed) { return float(double(next_random(seed)) * 0x1p-32); }
inline double real_divide(double a, double b) { return static_cast<float>(a / b); }
inline std::int64_t real_trunc(double value) { return pas::trunc(value); }
inline std::int64_t real_round(double value) { return pas::round(value); }
inline double real_sqrt(double value) { return static_cast<float>(std::sqrt(value)); }
inline double real_sin(double value) { return std::sin(value); }
inline double real_cos(double value) { return std::cos(value); }
inline double real_ln(double value) { return std::log(value); }
inline double real_exp(double value) { return std::exp(value); }
inline double real_arctan(double value) { return std::atan(value); }
inline double real_arctan2(double y, double x) { return std::atan2(y, x); }
inline double real_frac(double value) { double whole; return std::modf(value, &whole); }
inline double int_power(double base, std::int32_t exponent) {
  auto remaining = exponent < 0 ? 0u - std::uint32_t(exponent) : std::uint32_t(exponent);
  double result = 1;
  while (remaining) {
    if (remaining & 1) result = static_cast<float>(result * base);
    remaining >>= 1;
    if (remaining) base = static_cast<float>(base * base);
  }
  return exponent < 0 ? static_cast<float>(1.0 / result) : result;
}
inline double real_power(double base, double exponent) {
  if (std::abs(exponent) <= 2147483647.0 && std::trunc(exponent) == exponent)
    return int_power(base, std::int32_t(exponent));
  return std::pow(base, exponent);
}
template<class T> T real_min(T a, T b) { return a < b ? a : b; }
template<class T> T real_max(T a, T b) { return a <= b ? b : a; }
// Source overload selection must survive Extended's native double alias.
template<class T> T real_min_scalar(T a, T b) { return quiet_scalar_nan(b <= a ? b : a); }
template<class T> T real_max_scalar(T a, T b) { return quiet_scalar_nan(b < a ? a : b); }
inline bool is_nan(float value) { return std::isnan(value); }
inline bool is_nan(double value) { return std::isnan(value); }
inline std::int64_t legacy_random_bound(double value) {
  return real_trunc(legacy_numeric::random_bound(value));
}
inline double legacy_random_milli(std::int32_t value) {
  return legacy_numeric::round24(double(value) / 1000.0);
}
inline double legacy_seed_fraction(std::uint32_t seed) {
  // UInt32/10011001 cannot double-round across a PC24 midpoint.
  const double quotient = legacy_numeric::round24(double(seed) / 10011001.0);
  return quotient - std::trunc(quotient);
}
inline double legacy_fractional_quotient(std::int32_t a, std::int32_t b) {
  if (!b) return real_frac(double(a) / b);
  const double quotient = legacy_numeric::signed_quotient(a, b);
  return quotient - std::trunc(quotient);
}
inline std::int64_t legacy_quest_round(double value) { return real_round(native_pc24::add(value, 1.0e-11)); }
}
