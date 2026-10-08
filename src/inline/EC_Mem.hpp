#pragma once
#include "types/System.hpp"
#include "types/SystemImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/WindowsSdk.hpp"

namespace EC_Mem {
    inline void FreeEC(void* Data) {
        WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Data);
    }

    inline void FreeFromHeapEC(std::uint32_t Heap, void* Data) {
        WindowsSdk::HeapFree(Heap, 0u, Data);
    }

    // Explicit Pascal implementations of the native stack-ABI scalar accessors.
    // Typed pointer accesses preserve the stored bits, including unaligned values.
    // Integer(@PRecord(ByteOffset).Field) in callers derives a relative field offset
    // without reading memory, preserving these native calls and their argument order.
    inline void* AddPointerOffset(void* Data, std::int32_t ByteOffset) {
        return pas::pointer_add(static_cast<std::uint8_t*>(Data), ByteOffset);
    }

    inline void WriteByteEC(void* Dest, std::uint8_t Value) {
        *static_cast<std::uint8_t*>(Dest) = Value;
    }

    inline void WriteWordEC(void* Dest, std::uint16_t Value) {
        pas::store_unaligned<std::uint16_t>(static_cast<WindowsSdk::PWORD>(Dest), Value);
    }

    inline void WriteIntegerEC(void* Dest, std::int32_t Value) {
        pas::store_unaligned<std::int32_t>(static_cast<WindowsSdk::PInteger>(Dest), Value);
    }

    inline void WriteInt32EC(void* Dest, std::int32_t Value) {
        pas::store_unaligned<std::int32_t>(static_cast<WindowsSdk::PInteger>(Dest), Value);
    }

    inline void WriteSingleEC(void* Dest, float Value) {
        pas::store_unaligned<float>(static_cast<WindowsSdk::PSingle>(Dest), Value);
    }

    inline void WriteDoubleEC(void* Dest, double Value) {
        pas::store_unaligned<double>(static_cast<PDoubleEC>(Dest), Value);
    }

    inline std::uint8_t ReadByteEC(void* Source) {
        return *static_cast<std::uint8_t*>(Source);
    }

    inline char16_t ReadWideCharEC(void* Source) {
        return pas::load_unaligned<char16_t>(static_cast<char16_t*>(Source));
    }

    inline std::uint16_t ReadWordEC(void* Source) {
        return pas::load_unaligned<std::uint16_t>(static_cast<WindowsSdk::PWORD>(Source));
    }

    inline std::uint32_t ReadDWordEC(void* Source) {
        return pas::load_unaligned<std::uint32_t>(static_cast<System::PCardinal>(Source));
    }

    inline std::int32_t ReadIntegerEC(void* Source) {
        return pas::load_unaligned<std::int32_t>(static_cast<WindowsSdk::PInteger>(Source));
    }

    inline float ReadSingleEC(void* Source) {
        return pas::load_unaligned<float>(static_cast<WindowsSdk::PSingle>(Source));
    }

    inline double ReadDoubleEC(void* Source) {
        return pas::load_unaligned<double>(static_cast<PDoubleEC>(Source));
    }

} // namespace EC_Mem
