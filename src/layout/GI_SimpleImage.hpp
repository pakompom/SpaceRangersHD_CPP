#pragma once
#include "types/GI_SimpleImage.hpp"

namespace GI_SimpleImage {
    static_assert(sizeof(void*) != 4 || sizeof(GI_SimpleImage::TSimpleImageGI) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SimpleImage::TSimpleImageGI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SimpleImage::TSimpleImageGI, ImageKindX) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SimpleImage::TSimpleImageGI, ImageKindY) == 293);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SimpleImage::TSimpleImageGI, HalfAlpha) == 294);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SimpleImage::TSimpleImageGI, SourceRGBA) == 295);

} // namespace GI_SimpleImage
