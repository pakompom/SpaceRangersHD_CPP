#pragma once
#include "units/CrcUnit.hpp"

namespace CrcUnit {
    inline std::uint32_t ComputeCrc32(void* BufferPtr, std::int32_t ByteCount) {
        return ~pas::crc32_update(0xffffffffu, BufferPtr, pas::crc32_signed_span(BufferPtr, ByteCount), Crc32Table.elements);
    }

    inline std::uint32_t UpdateCrc32(std::uint32_t State, void* BufferPtr, std::int32_t ByteCount) {
        return pas::crc32_update(State, BufferPtr, pas::crc32_signed_span(BufferPtr, ByteCount), Crc32Table.elements);
    }

    inline std::uint32_t InvertCrc32(std::uint32_t State) {
        return ~State;
    }

    // Assembly loop requires nonnegative ByteCount; consumes bytes with LODSB.
    inline std::uint32_t UpdateCrc32Bytes(std::uint32_t State, void* BufferPtr, std::int32_t ByteCount) {
        return pas::crc32_update(State, BufferPtr, ByteCount, Crc32Table.elements);
    }

} // namespace CrcUnit
