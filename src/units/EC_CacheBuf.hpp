#pragma once
#include "types/EC_CacheBuf.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheBuf {
    TCBufEC* AcquireOrCreateBuffer(EC_Cache::TCacheControlEC* Control);

    void TCBufEC_Create(TCBufEC* Self);

    void TCBufEC_Destroy(TCBufEC* Self);

} // namespace EC_CacheBuf
