#pragma once
#include "runtime.hpp"

namespace pas {
// Manual semantic port of EC_Str.CompareWideChars. Nil is distinct from a
// nonnil empty string; nonnil inputs compare unsigned UTF-16 until the first 0.
inline Integer compare_wide_chars(const char16_t* left, const char16_t* right) {
  if (!left) return right ? -1 : 0;
  if (!right) return 1;
  auto* a=reinterpret_cast<const std::uint8_t*>(left);
  auto* b=reinterpret_cast<const std::uint8_t*>(right);
  for (;;) {
    auto x=load_unaligned<char16_t>(a), y=load_unaligned<char16_t>(b);
    if (x!=y) return x<y ? -1 : 1;
    if (!x) return 0;
    a+=2; b+=2;
  }
}
}
