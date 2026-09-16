#pragma once
#include "types/Windows_group.hpp"

namespace Windows {
    inline void FillMemory(void* Dest, std::uint32_t ByteCount, std::uint8_t Value);

    inline void MoveMemory(void* Dest, void* Source, std::uint32_t ByteCount);

    inline void CopyMemory(void* Dest, void* Source, std::uint32_t ByteCount);

} // namespace Windows

#include "inline/Windows.hpp"
