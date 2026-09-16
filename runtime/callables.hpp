#pragma once
// Source-level callbacks: one code pointer, or the Delphi code/Self pair.
#include <cstddef>
#include <stdexcept>
#include <utility>

#if defined(_WIN32) && defined(__i386__)
#define PAS_STDCALL __attribute__((stdcall))
#define PAS_CDECL __attribute__((cdecl))
#else
#define PAS_STDCALL
#define PAS_CDECL
#endif

namespace pas {
enum class CallbackAbi { native, stdcall };
template <class Signature, CallbackAbi Abi> struct ProcedureCode;
template <class R, class... A>
struct ProcedureCode<R(A...), CallbackAbi::native> {
  using type = R (*)(A...);
};
template <class R, class... A>
struct ProcedureCode<R(A...), CallbackAbi::stdcall> {
  using type = R(PAS_STDCALL *)(A...);
};

template <class Signature, CallbackAbi Abi = CallbackAbi::native> struct Proc;
template <class Signature> using StdcallProc = Proc<Signature, CallbackAbi::stdcall>;
template <class R, class... A, CallbackAbi Abi> struct Proc<R(A...), Abi> {
  using Code = typename ProcedureCode<R(A...), Abi>::type;
  Code code = nullptr;
  Proc() = default;
  Proc(std::nullptr_t) {}
  Proc(Code fn) : code(fn) {}
  R operator()(A... args) const {
    if (!code)
      throw std::runtime_error("call through nil procedure value");
    return code(std::forward<A>(args)...);
  }
  friend bool operator==(Proc, Proc) = default;
};

struct MethodBits {
  void *code = nullptr;
  void *data = nullptr;
};
template <class Signature> struct Method;
template <class R, class... A> struct Method<R(A...)> {
  using Code = R (*)(void *, A...);
  Code code = nullptr;
  void *data = nullptr;
  Method() = default;
  Method(std::nullptr_t) {}
  Method(Code fn, void *self) : code(fn), data(self) {}
  explicit Method(MethodBits bits)
      : code(reinterpret_cast<Code>(bits.code)), data(bits.data) {}
  R operator()(A... args) const {
    if (!code)
      throw std::runtime_error("call through nil method value");
    return code(data, std::forward<A>(args)...);
  }
  friend bool operator==(Method, Method) = default;
};
template <class S> MethodBits method_bits(Method<S> value) {
  return {reinterpret_cast<void *>(value.code), value.data};
}
template <class S, CallbackAbi A> bool assigned(Proc<S, A> value) {
  return value.code != nullptr;
}
template <class S> bool assigned(Method<S> value) {
  return value.code != nullptr;
}
template <class C> void *callback_address(C value) {
  return reinterpret_cast<void *>(value.code);
}
template <class C> C callback_from_address(void* address) {
  return C{reinterpret_cast<typename C::Code>(address)};
}
template <class C, class... A>
decltype(auto) invoke_callback(C value, A &&...args) {
  return value(std::forward<A>(args)...);
}

// One adapter per source method keeps TMethod.Code identity stable across sites.
// Virtual callbacks use a selected-implementation bridge, not a virtual member
// pointer: their target is frozen when the method value is formed.
template <auto Function> struct MethodAdapter;
template <class R, class T, class... A, R (*Function)(T *, A...)>
struct MethodAdapter<Function> {
  static R invoke(void *self, A... args) {
    return Function(static_cast<T *>(self), std::forward<A>(args)...);
  }
  static Method<R(A...)> bind(T *self) { return {&invoke, self}; }
};
template <class R, class T, class... A, R (T::*Function)(A...)>
struct MethodAdapter<Function> {
  static R invoke(void *self, A... args) {
    return (static_cast<T *>(self)->*Function)(std::forward<A>(args)...);
  }
  static Method<R(A...)> bind(T *self) { return {&invoke, self}; }
};
template <auto Function, class T> auto bind_method(T *self) {
  return MethodAdapter<Function>::bind(self);
}
template <auto Function> void *method_address() {
  return reinterpret_cast<void *>(&MethodAdapter<Function>::invoke);
}
template <auto Function> struct StaticMethodAdapter;
template <class R, class... A, R (*Function)(A...)>
struct StaticMethodAdapter<Function> {
  static R invoke(void *, A... args) { return Function(std::forward<A>(args)...); }
  static Method<R(A...)> bind(void *self) { return {&invoke, self}; }
};
template <auto Function, class T> auto bind_static_method(T *self) {
  return StaticMethodAdapter<Function>::bind(self);
}
template <auto Function> void *static_method_address() {
  return reinterpret_cast<void *>(&StaticMethodAdapter<Function>::invoke);
}
} // namespace pas
