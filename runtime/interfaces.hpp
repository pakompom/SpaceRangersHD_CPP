#pragma once
#include "callables.hpp"
#include <cstdint>
#include <cstring>
#include <utility>

namespace pas {
template <class Function> Function com_method(void *self, std::size_t slot) {
  if (!self) throw std::runtime_error("COM call through nil interface");
  void **table;
  std::memcpy(&table, self, sizeof(table));
  return reinterpret_cast<Function>(table[slot]);
}
template <std::size_t Slot, class R, class... A>
R com_invoke(void *self, A... args) {
  if (!self)
    throw std::runtime_error("COM call through nil interface");
  void **table;
  std::memcpy(&table, self, sizeof(table));
  using Function = R(PAS_STDCALL *)(void *, A...);
  auto function = reinterpret_cast<Function>(table[Slot]);
  return function(self, std::forward<A>(args)...);
}
inline void com_add_ref(void *p) {
  if (p)
    com_invoke<1, std::int32_t>(p);
}
inline void com_release(void *p) {
  if (p)
    com_invoke<2, std::int32_t>(p);
}
template <class Tag> struct ComView {
  void *value;
  ComView(std::nullptr_t) : value(nullptr) {}
  explicit ComView(void *p) : value(p) {}
  template<class T> ComView(const T& source) : value(source.get()) {}
  void *get() const { return value; }
  friend bool operator==(ComView a, std::nullptr_t) { return !a.value; }

};
template <class Tag> class ComPtr {
  void *value = nullptr;
  void assign(void *source) {
    // _IntfCopy reads the old destination after AddRef, then installs the
    // source before Release. Self-assignment still performs both calls.
    com_add_ref(source);
    auto *old = std::exchange(value, source);
    com_release(old);
  }

public:
  ComPtr() = default;
  ComPtr(std::nullptr_t) {}
  ComPtr(const ComPtr &other) { assign(other.value); }
  template <class U> ComPtr(const ComPtr<U> &other) { assign(other.get()); }
  template <class U> ComPtr(ComView<U> other) { assign(other.get()); }
  ComPtr(ComPtr &&other) noexcept
      : value(std::exchange(other.value, nullptr)) {}
  ~ComPtr() { clear(); }
  ComPtr &operator=(const ComPtr &other) {
    assign(other.value);
    return *this;
  }
  template <class U> ComPtr &operator=(const ComPtr<U> &other) {
    assign(other.get());
    return *this;
  }
  template <class U> ComPtr &operator=(ComView<U> other) {
    assign(other.get());
    return *this;
  }
  ComPtr &operator=(std::nullptr_t) {
    clear();
    return *this;
  }
  void *get() const { return value; }
  void clear() { com_release(std::exchange(value, nullptr)); }
  void **address() { return &value; }
  void **out_address() {
    clear();
    return &value;
  }
  void capture(ComPtr &&result) {
    com_release(std::exchange(value, std::exchange(result.value, nullptr)));
  }
  friend bool operator==(const ComPtr &a, std::nullptr_t) { return !a.value; }
};
template <class A, class B> bool operator==(ComView<A> a, ComView<B> b) {
  return a.get() == b.get();
}
template <class A, class B>
bool operator==(const ComPtr<A> &a, const ComPtr<B> &b) {
  return a.get() == b.get();
}
template <class A, class B> bool operator==(const ComPtr<A> &a, ComView<B> b) {
  return a.get() == b.get();
}
template <class T> bool assigned(const ComPtr<T> &value) {
  return value.get() != nullptr;
}
template <class T> bool assigned(ComView<T> value) {
  return value.get() != nullptr;
}
template <class T> ComPtr<T> &out_interface(ComPtr<T> &value) {
  value.clear();
  return value;
}
inline void* interface_pointer(std::nullptr_t) { return nullptr; }
template <class T> void* interface_pointer(const T& value) { return value.get(); }
template <class T>
const ComPtr<T> &capture_interface(ComPtr<T> &target, ComPtr<T> &&value) {
  target.capture(std::move(value));
  return target;
}
// Capture the value without AddRef while evaluating caller arguments. The
// translated callee's by-value parameter acquires its reference at entry.
template <class Ptr> struct ComArgument {
  void *value;
  ComArgument(std::nullptr_t) : value(nullptr) {}
  template <class T> ComArgument(const T &source) : value(source.get()) {}
  operator Ptr() const { return Ptr(ComView<void>{value}); }
};
} // namespace pas
