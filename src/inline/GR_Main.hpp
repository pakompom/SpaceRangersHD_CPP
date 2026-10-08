#pragma once
#include "units/GR_Main.hpp"
#include "units/WindowsSdk.hpp"

namespace GR_Main {
    // Tests bit 15 of GetAsyncKeyState.
    inline std::uint8_t IsVirtualKeyDown(std::int32_t Key) {
        return (WindowsSdk::GetAsyncKeyState(Key) & 0x00008000) == 0x00008000;
    }

    // Always returns 2; variant 1 retains the legacy quest-picture downscaling branch.
    inline std::int32_t GiResourceVariant() {
        return 2;
    }

    // 'libogg-0', 'libvorbis-0', 'libvorbisfile', 'matrixgame',
    // 'okgf', 'steam_ach', 'steam_api', 'xvidcore', 'zlib'.
    // Differences from the 1024x768 UI baseline; may be negative.
    // Identity function in this binary.
    inline std::int32_t GiScalePixels(std::int32_t Value) {
        return Value;
    }

    // Returns Value; AlternateValue is unused in this binary.
    inline std::int32_t GiScalePixelsEx(std::int32_t Value, std::int32_t AlternateValue) {
        return Value;
    }

} // namespace GR_Main
