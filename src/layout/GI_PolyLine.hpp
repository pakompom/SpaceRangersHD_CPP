#pragma once
#include "types/GI_PolyLine.hpp"

namespace GI_PolyLine {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PolyLine::TPolyLineGI) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, FirstSegment) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, LastSegment) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, AnimationPhase) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, AnimationTimer) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, FrameDrawing) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, ShadowCircle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, AutoRebuildBounds) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, NormalizeBounds) == 313);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineGI, SegmentHeap) == 316);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PolyLine::TPolyLineSegmentGI) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, Next) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, First) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, Last) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, UserData) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, Animated) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, PixelCount) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, PixelCapacity) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, PixelFirst) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, PixelLast) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, SavedPixels) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, Visible) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, PreviousFirst) == 61);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, PreviousLast) == 69);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, PreviousPixels) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, PreviouslyVisible) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, ClippedColor) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, ClippedEndColor) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, Color) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, EndColor) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PolyLine::TPolyLineSegmentGI, Kind) == 104);

} // namespace GI_PolyLine
