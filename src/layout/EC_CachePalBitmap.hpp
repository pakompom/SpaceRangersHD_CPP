#pragma once
#include "types/EC_CachePalBitmap.hpp"

namespace EC_CachePalBitmap {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CachePalBitmap::TCPalBitmapControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CachePalBitmap::TCPalBitmapEC) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CachePalBitmap::TCPalBitmapEC, Bitmap) == 32);

} // namespace EC_CachePalBitmap
