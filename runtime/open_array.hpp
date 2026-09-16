#pragma once
#include "runtime.hpp"
#include <vector>

namespace pas {
template<class T> struct OpenArray {
  T* elements;
  Integer count;
  Integer length() const { return count; }
  T* data() const { return elements; }
  T& operator[](Integer index) const { return elements[index]; }
};
template<class T> OpenArray<T> open_array(const DynArray<T>& array) {
  return {array.data(), array.length()};
}
template<class T, Integer L, Integer H> OpenArray<T> open_array(Array<T, L, H>& array) {
  return {array.elements, H - L + 1};
}
template<class T, Integer L, Integer H> OpenArray<T> open_array(const Array<T, L, H>& array) {
  return {const_cast<T*>(array.elements), H - L + 1};
}
template<class T> OpenArray<T> open_array(std::vector<T>& array) {
  return {array.data(), static_cast<Integer>(array.size())};
}
template<class T> OpenArray<T> open_array(OpenArray<T> array) { return array; }
// Delphi copies value open-array elements onto its stack. Keep typical game
// callback argument lists on the stack too; large lists retain normal ownership.
template<class T> class OpenArrayCopy {
  // Only the copied prefix is read. Managed elements still default-construct;
  // scalar/pointer slots need no zeroing before they are overwritten.
  std::array<T, 16> small_;
  std::vector<T> large_;
  Integer count_;
public:
  explicit OpenArrayCopy(OpenArray<T> array) : count_(array.count) {
    if (count_ <= Integer(small_.size())) {
      if (count_) {
        // This private destination cannot overlap the incoming array.
        if constexpr (std::is_trivially_copyable_v<T>)
          std::memcpy(small_.data(), array.elements, std::size_t(count_) * sizeof(T));
        else
          std::copy_n(array.elements, count_, small_.data());
      }
    } else {
      large_.assign(array.elements, array.elements + count_);
    }
  }
  OpenArrayCopy(const OpenArrayCopy&) = delete;
  OpenArray<T> view() { return {count_ <= Integer(small_.size()) ? small_.data() : large_.data(), count_}; }
};
template<class T> OpenArray<T> open_array(OpenArrayCopy<T>& array) { return array.view(); }
template<class T> OpenArrayCopy<T> copy_open_array(OpenArray<T> array) { return OpenArrayCopy<T>(array); }
}
