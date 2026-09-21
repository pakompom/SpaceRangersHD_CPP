#pragma once
#include "types/EC_CacheGAI.hpp"
#include "types/GR_gi.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheGAI {
    TCGaiEC* AcquireCachedGai(EC_Cache::TCacheControlEC* Control);

    // Native directory indexing adds the two header dwords separately.
    std::uint32_t ReadGaiSequenceOffset(GR_gi::PGaiSequenceTableHeader Table, std::int32_t Index);

    // CachedFrameOrigins has Header.FrameCount entries.
    void TCGaiEC_Create(TCGaiEC* Self);

    void TCGaiEC_Destroy(TCGaiEC* Self);

} // namespace EC_CacheGAI
