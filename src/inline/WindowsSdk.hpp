#pragma once
#include "types/SystemImports.hpp"
#include "units/WindowsSdk.hpp"

namespace WindowsSdk {
    inline void* PAS_STDCALL HeapAlloc(THandle hHeap, std::uint32_t dwFlags, std::uint32_t dwBytes) {
        return pas::win::heap_alloc(hHeap, dwFlags, dwBytes);
    }

    inline void* PAS_STDCALL HeapReAlloc(THandle hHeap, std::uint32_t dwFlags, void* lpMem, std::uint32_t dwBytes) {
        return pas::win::heap_realloc(hHeap, dwFlags, lpMem, dwBytes);
    }

    inline BOOL PAS_STDCALL HeapFree(THandle hHeap, std::uint32_t dwFlags, void* lpMem) {
        return pas::win::heap_free(hHeap, dwFlags, lpMem);
    }

    inline THandle PAS_STDCALL GetProcessHeap() {
        return pas::win::process_heap();
    }

} // namespace WindowsSdk
