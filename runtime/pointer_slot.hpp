#pragma once
#include <cstddef>
#include <cstring>

namespace pas {
// Managed Delphi values may live at byte-aligned offsets in packed records.
// Keep their owning pointer byte-aligned too; arithmetic uses an ordinary pointer.
// Fixed-size memcpy compiles to a native pointer load/store on our targets.
template<class T> class PointerSlot {
  unsigned char bytes_[sizeof(T*)]{};
public:
  PointerSlot() = default;
  PointerSlot(T* value) { *this = value; }
  T* get() const {
    T* value;
    std::memcpy(&value, bytes_, sizeof(value));
    return value;
  }
  operator T*() const { return get(); }
  PointerSlot& operator=(T* value) {
    std::memcpy(bytes_, &value, sizeof(value));
    return *this;
  }
  T& operator[](std::ptrdiff_t index) const { return get()[index]; }
};
static_assert(alignof(PointerSlot<char>) == 1);
static_assert(sizeof(PointerSlot<char>) == sizeof(char*));
}
