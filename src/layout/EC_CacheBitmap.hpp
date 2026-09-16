#pragma once
#include "types/EC_CacheBitmap.hpp"

namespace EC_CacheBitmap {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheBitmap::TCBitmapControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheBitmap::TCBitmapEC) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheBitmap::TCBitmapEC, Bitmap) == 32);

} // namespace EC_CacheBitmap
