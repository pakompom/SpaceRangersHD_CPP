#pragma once
#include "types/EC_CacheBitmap.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheBitmap {
    // Cache paths pass the option after '?' to LoadFromConfigBuffer.
    extern const pas::WideString RgbaImagePathSuffix;

    TCBitmapEC* AcquireOrCreateBitmap(EC_Cache::TCacheControlEC* Control);

    void TCBitmapEC_Create(TCBitmapEC* Self);

    void TCBitmapEC_Destroy(TCBitmapEC* Self);

} // namespace EC_CacheBitmap
