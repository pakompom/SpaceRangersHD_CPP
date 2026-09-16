#pragma once
#include "types/EC_Cache.hpp"

namespace EC_Cache {
    // The threshold is 0x30000000 bytes of used virtual address space.
    void EvictMainMenuShipCachesWhenAddressSpaceHigh();

    void EvictRuinsAndGovernmentCaches();

    void EvictStarAndBackgroundCaches();

    // Requires an exact cache-data class match.
    void EvictBlockChildrenFromCache(pas::WideString BlockPath, TCacheDataClass CacheDataClass);

    void TCacheControlEC_Create(TCacheControlEC* Self);

    void TCacheControlEC_Destroy(TCacheControlEC* Self);

    void TCacheDataEC_Create(TCacheDataEC* Self);

    // Detaches all controls without freeing them.
    void TCacheDataEC_Destroy(TCacheDataEC* Self);

    void TCacheEC_Create(TCacheEC* Self);

    void TCacheEC_Destroy(TCacheEC* Self);

} // namespace EC_Cache
