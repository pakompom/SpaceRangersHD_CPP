#pragma once
#include "types/EC_CacheHSAI.hpp"

namespace EC_CacheHSAI {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheHSAI::TCHSAIControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheHSAI::TCHSAIEC) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::TCHSAIEC, BlobData) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::TCHSAIEC, Header) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::TCHSAIEC, Width) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::TCHSAIEC, Height) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::TCHSAIEC, FrameSurfaceCache) == 48);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheHSAI::THSAIHeaderEC) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::THSAIHeaderEC, Width) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::THSAIHeaderEC, Height) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::THSAIHeaderEC, PitchBytes) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::THSAIHeaderEC, FrameCount) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::THSAIHeaderEC, FrameStride) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheHSAI::THSAIHeaderEC, PalettePresent) == 48);

} // namespace EC_CacheHSAI
