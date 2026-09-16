#pragma once
#include "types/EC_CacheGI.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheGI {
    TCGiEC* AcquireCachedGi(EC_Cache::TCacheControlEC* Control);

    void TCGiEC_Create(TCGiEC* Self);

    void TCGiEC_Destroy(TCGiEC* Self);

} // namespace EC_CacheGI
