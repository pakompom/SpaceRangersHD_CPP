#pragma once
#include <bit>
#include <type_traits>

namespace pas {
// Delphi 2007 steps the index past the final iteration, wrapping its storage
// width. Remember completion before that step so INT_MAX still terminates.
template <class T, bool Down> class ForRange {
  T first, last;
  bool started = false;
  template<class Ordinal> static void advance(Ordinal& value) {
    if constexpr (std::is_enum_v<Ordinal>) {
      auto ordinal = static_cast<std::underlying_type_t<Ordinal>>(value);
      advance(ordinal);
      value = static_cast<Ordinal>(ordinal);
    } else {
      using Unsigned = std::make_unsigned_t<Ordinal>;
      value = std::bit_cast<Ordinal>(static_cast<Unsigned>(static_cast<Unsigned>(value) + (Down ? -1 : 1)));
    }
  }

public:
  ForRange(T from, T to) : first(from), last(to) {}
  bool next(T &value) {
    if (!started) {
      started = true;
      // DCC evaluates both bounds but leaves the index alone for an empty loop.
      if (Down ? first < last : first > last) return false;
      value = first;
      return true;
    }
    const bool finished = value == last;
    advance(value);
    return !finished;
  }
};
template <class T> auto for_to(T first, T last) {
  static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
  return ForRange<T, false>(first, last);
}
template <class T> auto for_downto(T first, T last) {
  static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
  return ForRange<T, true>(first, last);
}
} // namespace pas
