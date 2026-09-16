#pragma once
#include "types/EC_CacheGAI.hpp"

namespace EC_CacheGAI {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheGAI::TCGaiControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheGAI::TCGaiEC) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGAI::TCGaiEC, RawGaiData) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGAI::TCGaiEC, Header) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGAI::TCGaiEC, DecodedFrameGi) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGAI::TCGaiEC, SequenceTableData) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGAI::TCGaiEC, SkipPalettedColorCacheBuild) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGAI::TCGaiEC, FrameSurfaceCache) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGAI::TCGaiEC, CachedFrameOrigins) == 56);

} // namespace EC_CacheGAI
