#pragma once
#include "units/GR_DX.hpp"

namespace GR_DX {
    inline std::uint32_t Color565ToArgb(std::uint32_t Color) {
        return Color << 3 & 0x000000f8 | (Color >> 3 & 0x000000fc) << 8 | (Color >> 8 & 0x000000f8) << 16 | 0xff000000u;
    }

    inline std::uint32_t ColorWithAlpha(std::uint32_t Color, std::uint32_t Alpha) {
        return Color & 0x00ffffff | (Alpha & 0x000000ff) << 24;
    }

} // namespace GR_DX
