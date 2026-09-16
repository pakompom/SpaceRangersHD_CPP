#pragma once
#include "types/EC_CacheTBitmap.hpp"

namespace EC_CacheTBitmap {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheTBitmap::TCTBitmapControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheTBitmap::TCTBitmapEC) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheTBitmap::TCTBitmapEC, TransBuffer) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheTBitmap::TCTBitmapEC, PixelSize) == 36);

} // namespace EC_CacheTBitmap
