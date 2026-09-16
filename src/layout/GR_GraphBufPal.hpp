#pragma once
#include "types/GR_GraphBufPal.hpp"

namespace GR_GraphBufPal {
    static_assert(sizeof(void*) != 4 || sizeof(GR_GraphBufPal::TGraphBufPalGR) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBufPal::TGraphBufPalGR, Width) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBufPal::TGraphBufPalGR, Height) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBufPal::TGraphBufPalGR, PitchBytes) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBufPal::TGraphBufPalGR, BytesPerPixel) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBufPal::TGraphBufPalGR, Pixels) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBufPal::TGraphBufPalGR, PaletteCount) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBufPal::TGraphBufPalGR, Palette) == 28);

} // namespace GR_GraphBufPal
