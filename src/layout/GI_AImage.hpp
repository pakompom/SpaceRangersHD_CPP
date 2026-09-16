#pragma once
#include "types/GI_AImage.hpp"

namespace GI_AImage {
    static_assert(sizeof(void*) != 4 || sizeof(GI_AImage::TAImageGI) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_AImage::TAImageGI, FrameTimer) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_AImage::TAImageGI, ImageKindX) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_AImage::TAImageGI, ImageKindY) == 293);
    static_assert(sizeof(void*) != 4 || offsetof(GI_AImage::TAImageGI, HalfAlpha) == 294);
    static_assert(sizeof(void*) != 4 || offsetof(GI_AImage::TAImageGI, CurrentFrame) == 296);

} // namespace GI_AImage
