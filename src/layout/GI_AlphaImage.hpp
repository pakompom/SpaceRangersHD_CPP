#pragma once
#include "types/GI_AlphaImage.hpp"

namespace GI_AlphaImage {
    static_assert(sizeof(void*) != 4 || sizeof(GI_AlphaImage::TAlphaImageGI) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_AlphaImage::TAlphaImageGI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_AlphaImage::TAlphaImageGI, ImageKindX) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_AlphaImage::TAlphaImageGI, ImageKindY) == 293);

} // namespace GI_AlphaImage
