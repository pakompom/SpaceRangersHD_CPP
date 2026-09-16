#pragma once
// Narrow Windows host surface. The game target calls the real Win32 APIs;
// native sanitizer probes use a checked process-heap adapter.
#include "runtime.hpp"
#ifndef _WIN32
#include <chrono>
#include <mutex>
#include <unordered_map>
#endif

namespace pas::win {
#ifdef _WIN32
extern "C" {
__declspec(dllimport) void *__stdcall GetProcessHeap();
__declspec(dllimport) void *__stdcall HeapAlloc(void *, unsigned long,
                                                std::size_t);
__declspec(dllimport) void *__stdcall HeapReAlloc(void *, unsigned long, void *,
                                                  std::size_t);
__declspec(dllimport) int __stdcall HeapFree(void *, unsigned long, void *);
__declspec(dllimport) std::uint32_t __stdcall timeGetTime();
__declspec(dllimport) int __stdcall GlobalMemoryStatusEx(void *);
using ImportAddress = std::intptr_t(PAS_STDCALL *)();
__declspec(dllimport) void *PAS_STDCALL LoadLibraryA(const char *);
__declspec(dllimport) ImportAddress PAS_STDCALL GetProcAddress(void *,
                                                               const char *);
__declspec(dllimport) unsigned long PAS_STDCALL GetLastError();
}
inline ImportAddress resolve_import(const char *library, const char *name) {
  // Keep a reference for process lifetime, like the source's static imports.
  auto module = LoadLibraryA(library);
  if (!module) {
    const auto error = GetLastError();
    raise_os_error(AnsiString((std::string("LoadLibraryA(") + library +
                             ") failed: " + std::to_string(error)).c_str()), error);
  }
  auto code = GetProcAddress(module, name);
  if (!code) {
    const auto error = GetLastError();
    auto selector = reinterpret_cast<std::uintptr_t>(name);
    raise_os_error(AnsiString((std::string("GetProcAddress(") + library + ", " +
        (selector <= 65535 ? "#" + std::to_string(selector) : name) +
        ") failed: " + std::to_string(error)).c_str()), error);
  }
  return code;
}
inline Cardinal time_get_time() { return timeGetTime(); }
inline std::int32_t memory_status(void *status) {
  return GlobalMemoryStatusEx(status);
}
inline void *heap_handle(Cardinal heap) {
  return reinterpret_cast<void *>(std::uintptr_t(heap));
}
inline Cardinal process_heap() {
  return Cardinal(reinterpret_cast<std::uintptr_t>(GetProcessHeap()));
}
inline void *heap_alloc(Cardinal heap, Cardinal flags, Cardinal bytes) {
  return HeapAlloc(heap_handle(heap), flags, bytes);
}
inline void *heap_realloc(Cardinal heap, Cardinal flags, void *data,
                          Cardinal bytes) {
  return HeapReAlloc(heap_handle(heap), flags, data, bytes);
}
inline bool heap_free(Cardinal heap, Cardinal flags, void *data) {
  return HeapFree(heap_handle(heap), flags, data) != 0;
}
#else
inline auto resolve_import(const char *library, const char *) -> void (*)() {
  throw std::runtime_error(std::string("DLL import requires a Win32 host: ") +
                           library);
}
inline Cardinal time_get_time() {
  return Cardinal(std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now().time_since_epoch())
                      .count());
}
inline std::int32_t memory_status(void *) {
  throw std::runtime_error("GlobalMemoryStatusEx requires a Win32 host");
}
inline std::mutex heap_mutex;
inline std::unordered_map<void *, std::size_t> heap_sizes;
inline Cardinal process_heap() { return 1; }
inline void check_heap(Cardinal heap, Cardinal flags) {
  if (heap != process_heap() || (flags & ~Cardinal(8)))
    throw std::runtime_error("native heap probe supports process heap, flags "
                             "0/HEAP_ZERO_MEMORY only");
}
inline void *heap_alloc(Cardinal heap, Cardinal flags, Cardinal bytes) {
  check_heap(heap, flags);
  std::lock_guard lock(heap_mutex);
  void *p = std::malloc(std::max<std::size_t>(bytes, 1));
  if (p) {
    if (flags & 8)
      std::memset(p, 0, bytes);
    heap_sizes.emplace(p, bytes);
  }
  return p;
}
inline void *heap_realloc(Cardinal heap, Cardinal flags, void *data,
                          Cardinal bytes) {
  check_heap(heap, flags);
  std::lock_guard lock(heap_mutex);
  auto found = heap_sizes.find(data);
  if (found == heap_sizes.end())
    throw std::runtime_error("invalid native heap realloc");
  auto old_size = found->second;
  void *p = std::realloc(data, std::max<std::size_t>(bytes, 1));
  if (p) {
    heap_sizes.erase(found);
    heap_sizes.emplace(p, bytes);
    if ((flags & 8) && bytes > old_size)
      std::memset(static_cast<std::uint8_t *>(p) + old_size, 0,
                  bytes - old_size);
  }
  return p;
}
inline bool heap_free(Cardinal heap, Cardinal flags, void *data) {
  check_heap(heap, flags);
  std::lock_guard lock(heap_mutex);
  if (!data)
    return true;
  if (!heap_sizes.erase(data))
    throw std::runtime_error("invalid native heap free");
  std::free(data);
  return true;
}
#endif
template <class Function>
Function load_import(const char *library, const char *name) {
  return reinterpret_cast<Function>(resolve_import(library, name));
}
template <class Function>
Function load_import(const char *library, std::uint16_t ordinal) {
  return load_import<Function>(
      library, reinterpret_cast<const char *>(std::uintptr_t(ordinal)));
}
} // namespace pas::win
