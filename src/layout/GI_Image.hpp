#pragma once
#include "types/GI_Image.hpp"

namespace GI_Image {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Image::TImageGI) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, SimpleImageControl) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, TransImageControl) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, AlphaImageControl) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, GiImageControl) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, AnimImageControl) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, GaiImageControl) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, GraphBufControl) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, ImagePath) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Image::TImageGI, AutoUpdateFlags) == 320);

} // namespace GI_Image
