#pragma once
#include "types/EC_CacheTBitmap.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheTBitmap {
    TCTBitmapEC* AcquireCachedTransBitmap(EC_Cache::TCacheControlEC* Control);

    void TCTBitmapEC_Create(TCTBitmapEC* Self);

    void TCTBitmapEC_Destroy(TCTBitmapEC* Self);

} // namespace EC_CacheTBitmap
