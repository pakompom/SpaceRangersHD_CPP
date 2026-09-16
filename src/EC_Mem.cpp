#include "types/SystemImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/WindowsSdk.hpp"

namespace EC_Mem {
    // Uses the process heap; allocation failure can evict texture caches before raising.
    void* AllocEC(std::int32_t ByteCount) {
        void* Memory = WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, ByteCount);
        if (Memory == nullptr) {
            GR_Main::AppendLogTextThreadSafe("Failed to allocate memory, trying to free some textures... "_a);
            GR_DX::EvictTextureCaches(true);
            Memory = WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, ByteCount);
            if (Memory != nullptr) {
                GR_Main::AppendLogLineThreadSafe("success"_a);
            } else {
                GR_Main::AppendLogLineThreadSafe("fail"_a);
                GR_Main::LogMemoryUsage();
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"AllocEC. size=", SysUtils::IntToStr(ByteCount)})));
            }
        }
        return Memory;
    }

    void* AllocClearEC(std::int32_t ByteCount) {
        void* Memory = WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), WindowsSdk::HEAP_ZERO_MEMORY, ByteCount);
        if (Memory == nullptr) {
            GR_Main::AppendLogTextThreadSafe("Failed to allocate memory, trying to free some textures... "_a);
            GR_DX::EvictTextureCaches(true);
            Memory = WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), WindowsSdk::HEAP_ZERO_MEMORY, ByteCount);
            if (Memory != nullptr) {
                GR_Main::AppendLogLineThreadSafe("success"_a);
            } else {
                GR_Main::AppendLogLineThreadSafe("fail"_a);
                GR_Main::LogMemoryUsage();
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"AllocClearEC. size=", SysUtils::IntToStr(ByteCount)})));
            }
        }
        return Memory;
    }

    // Nonpositive sizes free Data and return nil; allocation failure can evict texture caches.
    void* ReAllocREC(void* Data, std::int32_t ByteCount) {
        void* Memory{};
        if (ByteCount <= 0 && Data != nullptr) {
            WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Data);
            Memory = nullptr;
        } else if (ByteCount <= 0) {
            Memory = nullptr;
        } else if (ByteCount > 0 && Data != nullptr) {
            Memory = WindowsSdk::HeapReAlloc(WindowsSdk::GetProcessHeap(), 0u, Data, ByteCount);
            if (Memory == nullptr) {
                GR_Main::AppendLogTextThreadSafe("Failed to allocate memory, trying to free some textures... "_a);
                GR_DX::EvictTextureCaches(true);
                Memory = WindowsSdk::HeapReAlloc(WindowsSdk::GetProcessHeap(), 0u, Data, ByteCount);
                if (Memory != nullptr) {
                    GR_Main::AppendLogLineThreadSafe("success"_a);
                } else {
                    GR_Main::AppendLogLineThreadSafe("fail"_a);
                    GR_Main::LogMemoryUsage();
                    pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"ReAllocREC. size=", SysUtils::IntToStr(ByteCount)})));
                }
            }
        } else {
            Memory = WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, ByteCount);
            if (Memory == nullptr) {
                GR_Main::AppendLogTextThreadSafe("Failed to allocate memory, trying to free some textures... "_a);
                GR_DX::EvictTextureCaches(true);
                Memory = WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, ByteCount);
                if (Memory != nullptr) {
                    GR_Main::AppendLogLineThreadSafe("success"_a);
                } else {
                    GR_Main::AppendLogLineThreadSafe("fail"_a);
                    GR_Main::LogMemoryUsage();
                    pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"ReAllocREC. size=", SysUtils::IntToStr(ByteCount)})));
                }
            }
        }
        return Memory;
    }

    void FreeEC(void* Data) {
        WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Data);
    }

    void FreeFromHeapEC(std::uint32_t Heap, void* Data) {
        WindowsSdk::HeapFree(Heap, 0u, Data);
    }

    // The diagnostics retain AllocEC/AllocClearEC/ReAllocREC for these explicit-heap variants.
    // Raises on allocation failure; does not evict caches.
    void* AllocFromHeapEC(std::uint32_t Heap, std::int32_t ByteCount) {
        void* Memory = WindowsSdk::HeapAlloc(Heap, 0u, ByteCount);
        if (Memory == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"AllocEC. size=", SysUtils::IntToStr(ByteCount)})));
        }
        return Memory;
    }

    // Raises on allocation failure; does not evict caches.
    void* AllocClearFromHeapEC(std::uint32_t Heap, std::int32_t ByteCount) {
        void* Memory = WindowsSdk::HeapAlloc(Heap, WindowsSdk::HEAP_ZERO_MEMORY, ByteCount);
        if (Memory == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"AllocClearEC. size=", SysUtils::IntToStr(ByteCount)})));
        }
        return Memory;
    }

    // Nonpositive sizes free Data and return nil. Raises on allocation failure; does not evict caches.
    void* ReAllocFromHeapREC(std::uint32_t Heap, void* Data, std::int32_t ByteCount) {
        if (ByteCount <= 0 && Data != nullptr) {
            WindowsSdk::HeapFree(Heap, 0u, Data);
            Data = nullptr;
        } else if (ByteCount <= 0) {
            Data = nullptr;
        } else if (ByteCount > 0 && Data != nullptr) {
            Data = WindowsSdk::HeapReAlloc(Heap, 0u, Data, ByteCount);
            if (Data == nullptr) {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"ReAllocREC. size=", SysUtils::IntToStr(ByteCount)})));
            }
        } else {
            Data = WindowsSdk::HeapAlloc(Heap, 0u, ByteCount);
            if (Data == nullptr) {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"ReAllocREC. size=", SysUtils::IntToStr(ByteCount)})));
            }
        }
        return Data;
    }

} // namespace EC_Mem
