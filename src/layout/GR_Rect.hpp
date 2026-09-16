#pragma once
#include "types/GR_Rect.hpp"

namespace GR_Rect {
    static_assert(sizeof(void*) != 4 || sizeof(GR_Rect::TArrayRectGR) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Rect::TArrayRectGR, FirstRect) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Rect::TArrayRectGR, LastRect) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(GR_Rect::TRectGR) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Rect::TRectGR, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Rect::TRectGR, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Rect::TRectGR, Bounds) == 12);

} // namespace GR_Rect
