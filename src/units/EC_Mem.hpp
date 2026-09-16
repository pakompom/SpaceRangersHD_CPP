#pragma once
#include "runtime_support.hpp"

namespace EC_Mem {
    void* AllocEC(std::int32_t ByteCount);

    void* AllocClearEC(std::int32_t ByteCount);

    void* ReAllocREC(void* Data, std::int32_t ByteCount);

    void FreeEC(void* Data);

    void FreeFromHeapEC(std::uint32_t Heap, void* Data);

    void* AllocFromHeapEC(std::uint32_t Heap, std::int32_t ByteCount);

    void* AllocClearFromHeapEC(std::uint32_t Heap, std::int32_t ByteCount);

    void* ReAllocFromHeapREC(std::uint32_t Heap, void* Data, std::int32_t ByteCount);

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
