#pragma once
#include <cfloat>
#include <cstring>
#include <type_traits>

namespace pas {
#if LDBL_MANT_DIG == 64
// Delphi stores ten bytes. The i686 C++ ABI pads long double to twelve.
// Arithmetic still uses ordinary long double; memcpy lowers to x87 loads/stores.
struct alignas(2) Extended {
  unsigned char bytes[10]{};
  Extended() = default;
  Extended(long double value) { std::memcpy(bytes, &value, 10); }
  operator long double() const {
    long double value{};
    std::memcpy(&value, bytes, 10);
    return value;
  }
};
static_assert(sizeof(Extended) == 10);
static_assert(std::is_trivially_copyable_v<Extended>);
inline long double abs(Extended value) {
  const auto real = static_cast<long double>(value);
  return real < 0 ? -real : real;
}
#else
// Native ARM64 checks do not validate x87 precision or Win32 storage layouts.
using Extended = long double;
#endif
template<class T> inline constexpr bool is_real_v =
    std::is_floating_point_v<T> || std::is_same_v<T, Extended>;
}
