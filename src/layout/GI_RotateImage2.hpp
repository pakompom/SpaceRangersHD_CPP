#pragma once
#include "types/GI_RotateImage2.hpp"

namespace GI_RotateImage2 {
    static_assert(sizeof(void*) != 4 || sizeof(GI_RotateImage2::TRotateImage2GI) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage2::TRotateImage2GI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage2::TRotateImage2GI, RotationCache) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage2::TRotateImage2GI, RotatedImage) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage2::TRotateImage2GI, RenderedAngle) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage2::TRotateImage2GI, Angle) == 301);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage2::TRotateImage2GI, Alpha) == 302);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RotateImage2::TRotateImage2GI, ImageDirty) == 303);

} // namespace GI_RotateImage2
