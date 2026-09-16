#pragma once
#include "types/EC_Cache.hpp"

namespace EC_Cache {
    static_assert(sizeof(void*) != 4 || sizeof(EC_Cache::TCacheControlEC) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheControlEC, PrevBoundControl) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheControlEC, NextBoundControl) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheControlEC, BoundData) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheControlEC, CacheKey) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheControlEC, RetainCount) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Cache::TCacheDataEC) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheDataEC, PrevData) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheDataEC, NextData) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheDataEC, FirstBoundControl) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheDataEC, LastBoundControl) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheDataEC, CacheKey) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheDataEC, ResidentBytes) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheDataEC, LoadCompleteEvent) == 28);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Cache::TCacheEC) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheEC, CacheLock) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheEC, MostRecentData) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheEC, LeastRecentData) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheEC, DataRoot) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheEC, ResidentBytes) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Cache::TCacheEC, ResidentByteLimit) == 24);

} // namespace EC_Cache
