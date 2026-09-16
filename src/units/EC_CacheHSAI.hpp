#pragma once
#include "types/EC_CacheHSAI.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheHSAI {
    TCHSAIEC* AcquireCachedHSAI(EC_Cache::TCacheControlEC* Control);

    void TCHSAIEC_Create(TCHSAIEC* Self);

    void TCHSAIEC_Destroy(TCHSAIEC* Self);

} // namespace EC_CacheHSAI
