#pragma once
#include "types/EC_CacheAlphaBitmap.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheAlphaBitmap {
    TCAlphaBitmapEC* AcquireOrCreateAlphaBitmap(EC_Cache::TCacheControlEC* Control);

    void TCAlphaBitmapEC_Create(TCAlphaBitmapEC* Self);

    void TCAlphaBitmapEC_Destroy(TCAlphaBitmapEC* Self);

} // namespace EC_CacheAlphaBitmap
