#pragma once
#include "types/EC_CacheBuf.hpp"

namespace EC_CacheBuf {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheBuf::TCBufControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheBuf::TCBufEC) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheBuf::TCBufEC, Buffer) == 32);

} // namespace EC_CacheBuf
