#pragma once
#include "types/GI_StarFieldM.hpp"

namespace GI_StarFieldM {
    static_assert(sizeof(void*) != 4 || sizeof(GI_StarFieldM::TStarFieldMGI) == 376);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, Stars) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, StarCount) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, Capacity) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, FocusPoint) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, ViewPosition) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, TargetHeading) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, CurrentHeading) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, TargetFocusDistance) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, CurrentFocusDistance) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, MotionTicks) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, AnimationTimer) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TStarFieldMGI, ColorTable) == 372);
    static_assert(sizeof(void*) != 4 || sizeof(GI_StarFieldM::TMovingStarPixel) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, ByteOffset) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, PreviousByteOffset) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, SavedPixel) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, Position) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, Velocity) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, Acceleration) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, Direction) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, PixelPosition) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, PaletteIndex) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, Color) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, ColorPosition) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarFieldM::TMovingStarPixel, ColorStep) == 72);

} // namespace GI_StarFieldM
