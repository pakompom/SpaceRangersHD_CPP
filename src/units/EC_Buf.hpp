#pragma once
#include "types/EC_Buf.hpp"

namespace EC_Buf {
    void TBufEC_Create(TBufEC* Self);

    void TBufEC_Destroy(TBufEC* Self);

    inline std::uint8_t TBufEC_GetByte(TBufEC* Self);

    inline std::uint16_t TBufEC_GetWord(TBufEC* Self);

    inline std::uint32_t TBufEC_GetUInt32(TBufEC* Self);

    inline std::int32_t TBufEC_GetInt32(TBufEC* Self);

    inline float TBufEC_GetSingle(TBufEC* Self);

    inline std::uint8_t TBufEC_GetBoolean(TBufEC* Self);

} // namespace EC_Buf

#include "inline/EC_Buf.hpp"
