#pragma once
#include "legacy_numeric.hpp"
#include "pc24_contract.hpp"
#include <bit>
#include <cmath>
#include <cstdint>
namespace bezier24 {
// PC24 arithmetic with binary80-like exponent freedom, specialized to finite
// Bezier factors. Normalized mantissas keep every hardware operation normal.
struct Wide {
  float m = 0;
  int e = 0;
  Wide() = default;
  Wide(double v) {
    const auto b = std::bit_cast<std::uint64_t>(v);
    const int be = (b >> 52) & 2047;
    if (be && be != 2047) {
      m = float(
          std::bit_cast<double>((b & 0x800fffffffffffffULL) | (1023ULL << 52)));
      e = be - 1023;
    } else {
      int x = 0;
      m = float(std::frexp(v, &x));
      e = x;
    }
  }
  static Wide normalize(float m, int e) {
    const auto b = std::bit_cast<std::uint32_t>(m);
    const int be = (b >> 23) & 255;
    Wide r;
    if (be && be != 255) {
      r.m = std::bit_cast<float>((b & 0x807fffffu) | (127u << 23));
      r.e = e + be - 127;
    } else {
      r.m = m;
      r.e = 0;
    }
    return r;
  }
  double value() const {
    const auto b = std::bit_cast<std::uint64_t>(double(m));
    const int be = (b >> 52) & 2047;
    const int target = be + e;
    if (be && be != 2047 && target > 0 && target < 2047)
      return std::bit_cast<double>((b & 0x800fffffffffffffULL) |
                                   (std::uint64_t(target) << 52));
    return std::ldexp(double(m), e);
  }
  bool single_range() const { return m == 0 || (e >= -126 && e <= 127); }
  float single() const { return float(value()); }
};
inline Wide mul(Wide a, Wide b) {
  return Wide::normalize(a.m * b.m, a.e + b.e);
}
inline Wide div(Wide a, Wide b) {
  return Wide::normalize(a.m / b.m, a.e - b.e);
}
inline Wide power(Wide b, int e) {
  unsigned n = e < 0 ? 0u - unsigned(e) : unsigned(e);
  Wide r(1);
  while (n) {
    if (n & 1)
      r = mul(r, b);
    n >>= 1;
    if (n)
      b = mul(b, b);
  }
  return e < 0 ? div(Wide(1), r) : r;
}
inline double rounded(double v) {
  const auto e = (std::bit_cast<std::uint64_t>(v) >> 52) & 2047;
  if (e >= 897 && e <= 1150)
    return float(v);
  return pas::legacy_numeric::round24(v);
}
inline double add_product(double a, Wide w, float c) {
  const Wide p = mul(w, Wide(c));
  // Once both operands are representable as normal Single, hardware performs
  // the exact PC24 addition; retain double only for early vanishing tails.
  const auto ab = std::bit_cast<std::uint64_t>(a) & 0x7fffffffffffffffULL;
  if (p.single_range() && (!ab || ((ab >> 52) >= 897 && (ab >> 52) <= 1150)))
    return float(a) + p.single();
  return rounded(a + p.value());
}
} // namespace bezier24
