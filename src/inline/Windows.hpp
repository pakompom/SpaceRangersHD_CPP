#pragma once
#include "units/Windows.hpp"

namespace Windows {
    inline void FillMemory(void* Dest, std::uint32_t ByteCount, std::uint8_t Value) {
        pas::fill_memory(Dest, ByteCount, Value);
    }

    inline void MoveMemory(void* Dest, void* Source, std::uint32_t ByteCount) {
        pas::copy_memory(Dest, Source, ByteCount);
    }

    inline void CopyMemory(void* Dest, void* Source, std::uint32_t ByteCount) {
        pas::copy_memory(Dest, Source, ByteCount);
    }

} // namespace Windows
