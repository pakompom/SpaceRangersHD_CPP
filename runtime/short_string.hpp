#pragma once
#include "runtime.hpp"

namespace pas {
// The only short-string type used by this game: one length byte and 255 bytes.
struct ShortString {
  std::uint8_t count{};
  std::array<std::uint8_t, 255> bytes{};
  ShortString() = default;
  explicit ShortString(const std::uint8_t* source) {
    if (source) while (count < 255 && source[count]) {
      bytes[count] = source[count];
      ++count;
    }
  }
  template<std::size_t N> ShortString(const char (&source)[N]) {
    count = std::uint8_t(std::min<std::size_t>(N-1, 255));
    std::memcpy(bytes.data(), source, count);
  }
  explicit ShortString(const AnsiString& source) {
    count = std::uint8_t(std::min<std::int32_t>(source.length(), 255));
    std::memcpy(bytes.data(), source.c_str(), count);
  }
  explicit ShortString(const WideString& source) : ShortString(AnsiString(source)) {}
  std::int32_t length() const { return count; }
  operator AnsiString() const { return AnsiString(reinterpret_cast<const char*>(bytes.data()), count); }
  explicit operator WideString() const { return WideString(AnsiString(*this)); }
};
static_assert(sizeof(ShortString) == 256);
template<class T> ShortString class_name(ClassRef<T> value) {
  if (!value.info) throw std::runtime_error("ClassName on nil class reference");
  return ShortString(reinterpret_cast<const std::uint8_t*>(value.info->name));
}
}
