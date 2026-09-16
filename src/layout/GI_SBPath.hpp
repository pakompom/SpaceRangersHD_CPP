#pragma once
#include "types/GI_SBPath.hpp"

namespace GI_SBPath {
    static_assert(sizeof(void*) != 4 || sizeof(GI_SBPath::TSBPathGI) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, PointCount) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, Points) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, Minimum) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, Maximum) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, Position) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, Dragging) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, ThumbImage) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, HitRadius) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SBPath::TSBPathGI, ChangeCallback) == 320);

} // namespace GI_SBPath
