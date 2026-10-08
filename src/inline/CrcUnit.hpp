#pragma once
#include "units/CrcUnit.hpp"

namespace CrcUnit {
    inline std::uint32_t ComputeCrc32(void* BufferPtr, std::int32_t ByteCount) {
        return ~CrcUnit::UpdateCrc32(0xffffffffu, BufferPtr, ByteCount);
    }

    inline std::uint32_t InitCrc32() {
        return 0xffffffffu;
    }

    inline std::uint32_t FinishCrc32(std::uint32_t State) {
        return ~State;
    }

} // namespace CrcUnit
