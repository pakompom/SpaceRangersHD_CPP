#pragma once
#include "types/GI_SpaceCircle.hpp"

namespace GI_SpaceCircle {
    static_assert(sizeof(void*) != 4 || sizeof(GI_SpaceCircle::TSpaceCircleGI) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, SegmentCount) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, Segments) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, PreviousLineCount) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, PreviousLines) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, Center) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, Radius) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, Color) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, GeometryDirty) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, DrawnSegmentCount) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, DeactivateAfterFrame) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, AnimationTimer) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleGI, SavedPixels) == 336);
    static_assert(sizeof(void*) != 4 || sizeof(GI_SpaceCircle::TSpaceCircleSegmentGI) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleSegmentGI, First) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleSegmentGI, Last) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleSegmentGI, ClipResult) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleSegmentGI, PixelFirst) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleSegmentGI, PixelLast) == 28);
    static_assert(sizeof(void*) != 4 || sizeof(GI_SpaceCircle::TSpaceCircleSavedLineGI) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleSavedLineGI, First) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceCircle::TSpaceCircleSavedLineGI, Last) == 8);

} // namespace GI_SpaceCircle
