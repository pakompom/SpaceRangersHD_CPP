#pragma once
#include "types/EC_Cache.hpp"

namespace EC_Cache {
    void EvictMainMenuShipCachesWhenAddressSpaceHigh();

    void EvictRuinsAndGovernmentCaches();

    void EvictStarAndBackgroundCaches();

    void EvictBlockChildrenFromCache(pas::WideString BlockPath, TCacheDataClass CacheDataClass);

    void TCacheControlEC_Create(TCacheControlEC* Self);

    void TCacheControlEC_Destroy(TCacheControlEC* Self);

    void TCacheDataEC_Create(TCacheDataEC* Self);

    void TCacheDataEC_Destroy(TCacheDataEC* Self);

    void TCacheEC_Create(TCacheEC* Self);

    void TCacheEC_Destroy(TCacheEC* Self);

} // namespace EC_Cache
