#pragma once
#include "units/EC_Buf.hpp"
#include "units/EC_Struct.hpp"

namespace EC_Buf {
    inline void TBufEC_Create(TBufEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace EC_Buf
