#pragma once
#include "types/GI_RotateImageGAI.hpp"

namespace GI_RotateImageGAI {
    static_assert(sizeof(void*) != 4 || sizeof(GI_RotateImageGAI::TRotateImageGaiGI) == 452);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, RotationCache) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, RotatedImage) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, RenderedAngle) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, Angle) == 301);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, Alpha) == 302);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, ImageDirty) == 303);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, RenderedFrameIndex) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, FrameIndex) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, FrameCount) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, FrameIndexTable) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, FrameDelayTable) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, AnimationIndex) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, ImageSize) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, Vertices) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImageGAI::TRotateImageGaiGI, FrameTexture) == 448);

} // namespace GI_RotateImageGAI
