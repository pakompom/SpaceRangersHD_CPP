#pragma once
#include "types/GI_RotateImage.hpp"

namespace GI_RotateImage {
    static_assert(sizeof(void*) != 4 || sizeof(GI_RotateImage::TRotateImageGI) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage::TRotateImageGI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage::TRotateImageGI, RotationCache) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage::TRotateImageGI, Angle) == 296);

} // namespace GI_RotateImage
