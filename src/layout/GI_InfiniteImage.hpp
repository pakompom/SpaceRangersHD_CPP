#pragma once
#include "types/GI_InfiniteImage.hpp"

namespace GI_InfiniteImage {
    static_assert(sizeof(void*) != 4 || sizeof(GI_InfiniteImage::TInfiniteImageGI) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_InfiniteImage::TInfiniteImageGI, ImageCache) == 288);

} // namespace GI_InfiniteImage
