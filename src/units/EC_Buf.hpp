#pragma once
#include "types/EC_Buf.hpp"

namespace EC_Buf {
    inline void TBufEC_Create(TBufEC* Self);

    void TBufEC_Destroy(TBufEC* Self);

} // namespace EC_Buf

#include "inline/EC_Buf.hpp"
