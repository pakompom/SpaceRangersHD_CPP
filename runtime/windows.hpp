#pragma once
// Narrow Windows host surface. The game target calls the real Win32 APIs;
// Native hosts use checked heap handles with per-allocation ownership.
#include "runtime.hpp"
#ifndef _WIN32
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#endif

namespace pas::win {
#ifdef _WIN32
extern "C" {
__declspec(dllimport) void *__stdcall GetProcessHeap();
__declspec(dllimport) void *__stdcall HeapCreate(unsigned long, std::size_t,
                                                 std::size_t);
__declspec(dllimport) int __stdcall HeapDestroy(void *);
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
__declspec(dllimport) unsigned long PAS_STDCALL FormatMessageA(
    unsigned long, const void *, unsigned long, unsigned long, char *, unsigned long, void *);
}
[[noreturn]] inline void raise_last_os_error() {
  // Capture before formatting/allocation changes this thread's last-error slot.
  // Delphi's EOSError.ErrorCode is signed storage of the original DWORD bits.
  const auto code = std::bit_cast<Integer>(Cardinal(GetLastError()));
  if (!code)
    raise_os_error(AnsiString("A call to an OS function failed"), code);
  char buffer[256];
  auto length = FormatMessageA(0x3200, nullptr, Cardinal(code), 0, buffer,
                              sizeof(buffer), nullptr);
  // SysErrorMessage removes CR/LF, other trailing controls/spaces and periods.
  while (length && (static_cast<unsigned char>(buffer[length - 1]) <= 32 ||
                    buffer[length - 1] == '.'))
    --length;
  raise_os_error(AnsiString("System Error.  Code: ") + int_to_str(code) +
                     AnsiString(".\r\n") + AnsiString(buffer, Integer(length)), code);
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
inline Cardinal heap_create(Cardinal flags, Cardinal initial, Cardinal maximum) {
  return Cardinal(reinterpret_cast<std::uintptr_t>(HeapCreate(flags, initial, maximum)));
}
inline bool heap_destroy(Cardinal heap) {
  return HeapDestroy(heap_handle(heap)) != 0;
}
inline void *heap_alloc(Cardinal heap, Cardinal flags, std::size_t bytes) {
  return HeapAlloc(heap_handle(heap), flags, bytes);
}
inline void *heap_realloc(Cardinal heap, Cardinal flags, void *data,
                          std::size_t bytes) {
  return HeapReAlloc(heap_handle(heap), flags, data, bytes);
}
inline bool heap_free(Cardinal heap, Cardinal flags, void *data) {
  return HeapFree(heap_handle(heap), flags, data) != 0;
}
#else
[[noreturn]] inline void raise_last_os_error() {
  throw std::runtime_error("RaiseLastOSError requires a Win32 host");
}
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
struct HeapAllocation {
  std::size_t size;
  Cardinal heap;
};
inline std::unordered_map<void *, HeapAllocation> heap_sizes;
inline std::unordered_set<Cardinal> private_heaps;
inline Cardinal next_heap = 2;
inline Cardinal process_heap() { return 1; }
// Call while holding heap_mutex. NO_SERIALIZE is safe to accept: the adapter
// still synchronizes its ownership bookkeeping, independently of that hint.
inline void check_heap(Cardinal heap, Cardinal flags, Cardinal supported) {
  if (heap != process_heap() && !private_heaps.contains(heap))
    throw std::runtime_error("invalid native heap handle");
  if (flags & ~supported)
    throw std::runtime_error("unsupported native heap flags");
}
inline Cardinal heap_create(Cardinal flags, Cardinal, Cardinal maximum) {
  // Initial reservation is an allocation hint. Fixed-capacity and executable
  // heaps require host reservation semantics that this adapter does not supply.
  if ((flags & ~Cardinal(1)) || maximum)
    throw std::runtime_error("native HeapCreate supports growable, nonexecutable heaps only");
  std::lock_guard lock(heap_mutex);
  if (!next_heap) return 0; // Do not reuse a token held by a stale caller.
  const auto heap = next_heap;
  try { private_heaps.insert(heap); }
  catch (const std::bad_alloc &) { return 0; }
  ++next_heap;
  return heap;
}
inline bool heap_destroy(Cardinal heap) {
  std::lock_guard lock(heap_mutex);
  if (heap == process_heap())
    throw std::runtime_error("cannot destroy native process heap");
  check_heap(heap, 0, 0);
  for (auto it = heap_sizes.begin(); it != heap_sizes.end();) {
    if (it->second.heap == heap) {
      std::free(it->first);
      it = heap_sizes.erase(it);
    } else ++it;
  }
  private_heaps.erase(heap);
  return true;
}
inline void *heap_alloc(Cardinal heap, Cardinal flags, std::size_t bytes) {
  std::lock_guard lock(heap_mutex);
  check_heap(heap, flags, 1 | 8);
  if (bytes > std::size_t(std::numeric_limits<std::ptrdiff_t>::max())) return nullptr;
  void *p = std::malloc(std::max<std::size_t>(bytes, 1));
  if (p) {
    std::unique_ptr<void, decltype(&std::free)> owner(p, &std::free);
    if (flags & 8)
      std::memset(p, 0, bytes);
    // Failed bookkeeping releases the block rather than leaking an allocation
    // that no caller can subsequently free through this adapter.
    try { heap_sizes.emplace(p, HeapAllocation{bytes, heap}); }
    catch (const std::bad_alloc &) { return nullptr; }
    owner.release();
  }
  return p;
}
inline void *heap_realloc(Cardinal heap, Cardinal flags, void *data,
                          std::size_t bytes) {
  std::lock_guard lock(heap_mutex);
  check_heap(heap, flags, 1 | 8);
  auto found = heap_sizes.find(data);
  if (found == heap_sizes.end() || found->second.heap != heap)
    throw std::runtime_error("invalid native heap realloc");
  if (bytes > std::size_t(std::numeric_limits<std::ptrdiff_t>::max())) return nullptr;
  auto old_size = found->second.size;
  // Reuse the existing node: restoring the previous map size needs neither a
  // new node nor rehashing. Nothing can allocate after realloc moves the block.
  auto allocation = heap_sizes.extract(found);
  void *p = std::realloc(data, std::max<std::size_t>(bytes, 1));
  if (p) {
    allocation.key() = p;
    allocation.mapped().size = bytes;
    heap_sizes.insert(std::move(allocation));
    if ((flags & 8) && bytes > old_size)
      std::memset(static_cast<std::uint8_t *>(p) + old_size, 0,
                  bytes - old_size);
  } else heap_sizes.insert(std::move(allocation));
  return p;
}
inline bool heap_free(Cardinal heap, Cardinal flags, void *data) {
  std::lock_guard lock(heap_mutex);
  check_heap(heap, flags, 1);
  if (!data)
    return true;
  auto found = heap_sizes.find(data);
  if (found == heap_sizes.end() || found->second.heap != heap)
    throw std::runtime_error("invalid native heap free");
  heap_sizes.erase(found);
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
