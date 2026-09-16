#pragma once
#include "types/EC_Buf.hpp"

namespace EC_Buf {
    static_assert(sizeof(void*) != 4 || sizeof(EC_Buf::TBufEC) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Buf::TBufEC, DataSize) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Buf::TBufEC, Capacity) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Buf::TBufEC, Position) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Buf::TBufEC, Data) == 16);

} // namespace EC_Buf
