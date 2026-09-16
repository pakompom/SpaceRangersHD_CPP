#pragma once
#include "types/EC_CacheRotateBuf.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheRotateBuf {
    TCRotateBufEC* AcquireOrCreateRotateBuf(EC_Cache::TCacheControlEC* Control);

    void TCRotateBufEC_Create(TCRotateBufEC* Self);

    void TCRotateBufEC_Destroy(TCRotateBufEC* Self);

} // namespace EC_CacheRotateBuf
