#pragma once
#include "types/GI_Frame.hpp"

namespace GI_Frame {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Frame::TFrameGI) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Frame::TFrameGI, Kind) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Frame::TFrameGI, Color) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Frame::TFrameGI, FillColor) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Frame::TFrameGI, Fill) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Frame::TFrameGI, FillAlpha) == 301);

} // namespace GI_Frame
