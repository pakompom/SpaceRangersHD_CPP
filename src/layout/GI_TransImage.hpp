#pragma once
#include "types/GI_TransImage.hpp"

namespace GI_TransImage {
    static_assert(sizeof(void*) != 4 || sizeof(GI_TransImage::TTransImageGI) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TransImage::TTransImageGI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TransImage::TTransImageGI, ImageKindX) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TransImage::TTransImageGI, ImageKindY) == 293);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TransImage::TTransImageGI, HalfAlpha) == 294);

} // namespace GI_TransImage
