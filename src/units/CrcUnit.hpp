#pragma once
#include "types/CrcUnit.hpp"

namespace CrcUnit {
    // Reflected CRC-32 polynomial $EDB88320; native table.
    extern const CrcUnit::TCrc32Table Crc32Table;

    inline std::uint32_t ComputeCrc32(void* BufferPtr, std::int32_t ByteCount);

    std::uint32_t InitCrc32();

    inline std::uint32_t UpdateCrc32(std::uint32_t State, void* BufferPtr, std::int32_t ByteCount);

    std::uint32_t FinishCrc32(std::uint32_t State);

    inline std::uint32_t InvertCrc32(std::uint32_t State);

    // Assembly loop requires nonnegative ByteCount; consumes bytes with LODSB.
    inline std::uint32_t UpdateCrc32Bytes(std::uint32_t State, void* BufferPtr, std::int32_t ByteCount);

    std::uint32_t ExtendCrc32(std::uint32_t Crc, void* BufferPtr, std::int32_t ByteCount);

    // Writes four bytes whose appended CRC transforms CurrentCrc into TargetCrc.
    void WriteCrc32Correction(std::uint32_t CurrentCrc, std::uint32_t TargetCrc, void* Dest);

} // namespace CrcUnit

#include "inline/CrcUnit.hpp"
