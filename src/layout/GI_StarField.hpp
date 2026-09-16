#pragma once
#include "types/GI_StarField.hpp"

namespace GI_StarField {
    static_assert(sizeof(void*) != 4 || sizeof(GI_StarField::TStarFieldGI) == 400);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, BackgroundCache) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, Stars) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, ViewPosition) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, Unknown150) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, ViewDirty) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, Pixels) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, PixelCapacity) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, PixelCount) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, PreviousPixels) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, PreviousPixelCount) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, BackgroundScale) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, PreviousBackgroundBounds) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldGI, BackgroundBounds) == 384);
    static_assert(sizeof(void*) != 4 || sizeof(GI_StarField::TStarFieldList) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldList, Points) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldList, Count) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldList, Capacity) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(GI_StarField::TStarFieldPoint) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPoint, X) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPoint, Y) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPoint, Depth) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPoint, InverseDepth) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPoint, Color) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(GI_StarField::TStarFieldPixel) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPixel, ByteOffset) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPixel, Position) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPixel, Color) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StarField::TStarFieldPixel, SavedPixel) == 14);

} // namespace GI_StarField
