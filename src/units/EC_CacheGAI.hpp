#pragma once
#include "types/EC_CacheGAI.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheGAI {
    TCGaiEC* AcquireCachedGai(EC_Cache::TCacheControlEC* Control);

    // CachedFrameOrigins has Header.FrameCount entries.
    void TCGaiEC_Create(TCGaiEC* Self);

    void TCGaiEC_Destroy(TCGaiEC* Self);

} // namespace EC_CacheGAI
