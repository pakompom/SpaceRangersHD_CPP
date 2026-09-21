#pragma once
#include "types/GI_SpaceImg.hpp"

namespace GI_SpaceImg {
    static_assert(sizeof(void*) != 4 || sizeof(GI_SpaceImg::TSpaceImgGI) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImgGI, ImageCount) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImgGI, Images) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImgGI, ViewDirty) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImgGI, ViewPosition) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImgGI, AnimationTimer) == 308);
    static_assert(sizeof(void*) != 4 || sizeof(GI_SpaceImg::TSpaceImageGI) == 136);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, TemplateIndex) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, FrameIndex) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, FrameTicks) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, X) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, Y) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, Depth) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, InverseDepth) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, OrbitCenter) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, Unknown38) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, ImageSize) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, ImageOffset) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, PixelPosition) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, OrbitStepDegrees) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, SavedTemplateIndex) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, OrbitAngleRadians) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SpaceImg::TSpaceImageGI, OrbitRadius) == 128);

} // namespace GI_SpaceImg
