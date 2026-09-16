#pragma once
#include "types/GI_StarFieldImg.hpp"

namespace GI_StarFieldImg {
    static_assert(sizeof(void*) != 4 || sizeof(GI_StarFieldImg::TStarFieldImgGI) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, Stars) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, StarCount) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, Capacity) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, ReservedDirty) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, FocusPoint) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, ViewPosition) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, TargetHeading) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, CurrentHeading) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, TargetFocusDistance) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, CurrentFocusDistance) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, MotionTicks) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImgGI, AnimationTimer) == 340);
    static_assert(sizeof(void*) != 4 || sizeof(GI_StarFieldImg::TStarFieldImageGI) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, TemplateIndex) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, FrameIndex) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, LastFrame) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, FramePosition) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, FrameStep) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, Position) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, Velocity) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, Acceleration) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, Direction) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, ImageSize) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, ImageOffset) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldImg::TStarFieldImageGI, PixelPosition) == 68);

} // namespace GI_StarFieldImg
