#pragma once
#include "types/GI_RotateImage5.hpp"

namespace GI_RotateImage5 {
    static_assert(sizeof(void*) != 4 || sizeof(GI_RotateImage5::TRotateImage5GI) == 8660);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, RotationCache) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, RotatedImage) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, RenderedAngle) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, RenderedFrameIndex) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, FrameIndex) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, Angle) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, Alpha) == 313);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, ImageDirty) == 314);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, Unknown13C) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, Vertices) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, FrameTexture) == 432);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, TopLeftX) == 436);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, TopLeftY) == 1464);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, TopRightX) == 2492);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, TopRightY) == 3520);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, BottomRightX) == 4548);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, BottomRightY) == 5576);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, BottomLeftX) == 6604);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage5::TRotateImage5GI, BottomLeftY) == 7632);

} // namespace GI_RotateImage5
