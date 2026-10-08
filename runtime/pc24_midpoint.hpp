#pragma once
#include "pc24_contract.hpp"
#include <bit>
#include <cmath>
#include <cstdint>
namespace native_pc24 {
// Only mixed wide operand sites need this guard. Ordinary float expressions
// already round once in hardware. FMA recovers the residual at a float tie.
inline float add(double a, double b) {
  double v = a + b;
  if ((std::bit_cast<std::uint64_t>(v) & 0x1fffffffULL) == 0x10000000ULL &&
      std::isfinite(v)) {
    const double z = v - a;
    const double error = (a - (v - z)) + (b - z);
    if (error != 0)
      v = std::nextafter(v, error > 0 ? INFINITY : -INFINITY);
  }
  return float(v);
}
inline float multiply(double a, double b) {
  double v = a * b;
  if ((std::bit_cast<std::uint64_t>(v) & 0x1fffffffULL) == 0x10000000ULL &&
      std::isfinite(v)) {
    const double error = std::fma(a, b, -v);
    if (error != 0)
      v = std::nextafter(v, error > 0 ? INFINITY : -INFINITY);
  }
  return float(v);
}
// hi+tail represents the original binary80 literal exactly. Keep both
// products until rounding; wider Double inputs must not be narrowed first.
// This shares the hardware profile's normal binary32 gameplay domain.
inline float multiply_split(double a, double hi, double tail) {
  const double p = a * hi;
  const double q = a * tail;
  double v = p + q;
  if ((std::bit_cast<std::uint64_t>(v) & 0x1fffffffULL) == 0x10000000ULL &&
      std::isfinite(v)) {
    const double z = v - p;
    double x = (p - (v - z)) + (q - z);
    double y = std::fma(a, hi, -p);
    double w = std::fma(a, tail, -q);
    // Add the largest residuals first so cancellation retains the sign.
    if (std::abs(x) < std::abs(y)) { const double t = x; x = y; y = t; }
    if (std::abs(y) < std::abs(w)) { const double t = y; y = w; w = t; }
    if (std::abs(x) < std::abs(y)) { const double t = x; x = y; y = t; }
    const double error = (x + y) + w;
    if (error != 0)
      v = std::nextafter(v, error > 0 ? INFINITY : -INFINITY);
  }
  return float(v);
}
inline float divide(double a, double b) {
  double v = a / b;
  if ((std::bit_cast<std::uint64_t>(v) & 0x1fffffffULL) == 0x10000000ULL &&
      std::isfinite(v)) {
    double error = std::fma(-v, b, a) / b;
    if (error != 0)
      v = std::nextafter(v, error > 0 ? INFINITY : -INFINITY);
  }
  return float(v);
}
} // namespace native_pc24
