#pragma once
#include "runtime.hpp"
#ifndef _WIN32
#include <mutex>
#endif
namespace pas {
#ifdef _WIN32
extern "C" {
__declspec(dllimport) void __stdcall InitializeCriticalSection(void *);
__declspec(dllimport) void __stdcall DeleteCriticalSection(void *);
__declspec(dllimport) void __stdcall EnterCriticalSection(void *);
__declspec(dllimport) void __stdcall LeaveCriticalSection(void *) noexcept;
}
#endif
struct CriticalSection : Object {
  PAS_CLASS_META(CriticalSection, Object, "TCriticalSection", 28)
#ifdef _WIN32
  alignas(4) std::byte section[24]{};
  void p_create() { InitializeCriticalSection(section); }
  void p_destroy() override { DeleteCriticalSection(section); }
  void p_enter() { EnterCriticalSection(section); }
  void p_leave() noexcept { LeaveCriticalSection(section); }
#else
  std::recursive_mutex section;
  void p_create() {}
  void p_enter() { section.lock(); }
  void p_leave() noexcept { section.unlock(); }
#endif
};
inline void critical_create(CriticalSection *self) { self->p_create(); }
inline void critical_destroy(CriticalSection *self) { self->p_destroy(); }
inline void critical_enter(CriticalSection *self) { self->p_enter(); }
inline void critical_leave(CriticalSection *self) noexcept { self->p_leave(); }
template<class T> T *make_critical_section() {
  return construct_call<T>(critical_create);
}
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(CriticalSection) == 28, "Delphi TCriticalSection size");
#endif
} // namespace pas
