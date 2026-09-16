#pragma once
#include "types/EC_CacheRotateBuf.hpp"

namespace EC_CacheRotateBuf {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheRotateBuf::TCRotateBufControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheRotateBuf::TCRotateBufEC) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheRotateBuf::TCRotateBufEC, Buffer) == 32);

} // namespace EC_CacheRotateBuf
