#pragma once
#include "types/EC_CacheSound.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheSound {
    TCSoundEC* AcquireCachedSound(EC_Cache::TCacheControlEC* Control);

    void TCSoundEC_Create(TCSoundEC* Self);

    void TCSoundEC_Destroy(TCSoundEC* Self);

} // namespace EC_CacheSound
