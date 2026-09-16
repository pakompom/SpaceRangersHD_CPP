#pragma once
#include "types/EC_CacheGI.hpp"

namespace EC_CacheGI {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheGI::TCGiControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheGI::TCGiEC) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGI::TCGiEC, Image) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGI::TCGiEC, SurfaceCache) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGI::TCGiEC, UsesTiledSurfaces) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGI::TCGiEC, TileOrigins) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheGI::TCGiEC, TileCount) == 48);

} // namespace EC_CacheGI
