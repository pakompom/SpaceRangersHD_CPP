#pragma once
#include "runtime_support.hpp"

namespace EC_Mem {
    // Uses the process heap; allocation failure can evict texture caches before raising.
    void* AllocEC(std::int32_t ByteCount);

    void* AllocClearEC(std::int32_t ByteCount);

    // Nonpositive sizes free Data and return nil; allocation failure can evict texture caches.
    void* ReAllocREC(void* Data, std::int32_t ByteCount);

    void FreeEC(void* Data);

    void FreeFromHeapEC(std::uint32_t Heap, void* Data);

    // The diagnostics retain AllocEC/AllocClearEC/ReAllocREC for these explicit-heap variants.
    // Raises on allocation failure; does not evict caches.
    void* AllocFromHeapEC(std::uint32_t Heap, std::int32_t ByteCount);

    // Raises on allocation failure; does not evict caches.
    void* AllocClearFromHeapEC(std::uint32_t Heap, std::int32_t ByteCount);

    // Nonpositive sizes free Data and return nil. Raises on allocation failure; does not evict caches.
    void* ReAllocFromHeapREC(std::uint32_t Heap, void* Data, std::int32_t ByteCount);

    // These stack-ABI accessors are handwritten assembly in the native unit.
    inline void* AddPointerOffset(void* Data, std::int32_t ByteOffset);

    inline void WriteByteEC(void* Dest, std::uint8_t Value);

    inline void WriteWordEC(void* Dest, std::uint16_t Value);

    inline void WriteIntegerEC(void* Dest, std::int32_t Value);

    inline void WriteInt32EC(void* Dest, std::int32_t Value);

    inline void WriteSingleEC(void* Dest, float Value);

    inline void WriteDoubleEC(void* Dest, double Value);

    inline std::uint8_t ReadByteEC(void* Source);

    inline char16_t ReadWideCharEC(void* Source);

    inline std::uint16_t ReadWordEC(void* Source);

    inline std::uint32_t ReadDWordEC(void* Source);

    inline std::int32_t ReadIntegerEC(void* Source);

    inline float ReadSingleEC(void* Source);

    inline double ReadDoubleEC(void* Source);

} // namespace EC_Mem

#include "inline/EC_Mem.hpp"
