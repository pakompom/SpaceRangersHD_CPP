#pragma once
#include "types/EC_CacheAlphaBitmap.hpp"

namespace EC_CacheAlphaBitmap {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheAlphaBitmap::TCAlphaBitmapControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheAlphaBitmap::TCAlphaBitmapEC) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheAlphaBitmap::TCAlphaBitmapEC, TransBuf16) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheAlphaBitmap::TCAlphaBitmapEC, TransAlphaBuf16) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheAlphaBitmap::TCAlphaBitmapEC, AlphaBuf) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheAlphaBitmap::TCAlphaBitmapEC, PixelSize) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheAlphaBitmap::TCAlphaBitmapEC, SurfaceCache) == 52);

} // namespace EC_CacheAlphaBitmap
