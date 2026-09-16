#pragma once
#include "types/EC_CachePalBitmap.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CachePalBitmap {
    TCPalBitmapEC* AcquireOrCreatePalBitmap(EC_Cache::TCacheControlEC* Control);

    void TCPalBitmapEC_Create(TCPalBitmapEC* Self);

    void TCPalBitmapEC_Destroy(TCPalBitmapEC* Self);

} // namespace EC_CachePalBitmap
