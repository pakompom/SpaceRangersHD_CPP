#pragma once
#include "types/GI_Tail.hpp"

namespace GI_Tail {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Tail::TTailSegmentGI) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailSegmentGI, Active) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailSegmentGI, FrameIndex) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailSegmentGI, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailSegmentGI, Velocity) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailSegmentGI, PixelPosition) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(GI_Tail::TTailGI) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, FrameCount) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, SegmentCapacity) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, Segments) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, ImageSize) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, LastSegmentIndex) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, EmitterPosition) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, SegmentVelocity) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, FrameTimer) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, MoveTimer) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, EmitTimer) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, EmitIntervalMs) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Tail::TTailGI, Emitting) == 348);

} // namespace GI_Tail
