#pragma once
#include "units/EC_Buf.hpp"
#include "units/GR_Main.hpp"

namespace EC_Buf {
    inline std::uint8_t TBufEC_GetByte(TBufEC* Self) {
        Self->EnsureReadable(static_cast<std::int32_t>(sizeof(std::uint8_t)));
        return pas::read_and_advance<std::uint8_t>(Self->Data, &Self->Position);
    }

    inline std::uint16_t TBufEC_GetWord(TBufEC* Self) {
        Self->EnsureReadable(static_cast<std::int32_t>(sizeof(std::uint16_t)));
        return pas::read_and_advance<std::uint16_t>(Self->Data, &Self->Position);
    }

    inline std::uint32_t TBufEC_GetUInt32(TBufEC* Self) {
        Self->EnsureReadable(static_cast<std::int32_t>(sizeof(std::uint32_t)));
        return pas::read_and_advance<std::uint32_t>(Self->Data, &Self->Position);
    }

    inline std::int32_t TBufEC_GetInt32(TBufEC* Self) {
        Self->EnsureReadable(static_cast<std::int32_t>(sizeof(std::int32_t)));
        return pas::read_and_advance<std::int32_t>(Self->Data, &Self->Position);
    }

    inline float TBufEC_GetSingle(TBufEC* Self) {
        Self->EnsureReadable(static_cast<std::int32_t>(sizeof(float)));
        float Result = pas::read_and_advance<float>(Self->Data, &Self->Position);
        if (pas::is_nan(Result)) {
            Result = 0.0f;
            GR_Main::AppendLogLineThreadSafe("Warning! NaN encountered, replaced with zero."_a);
        }
        return Result;
    }

    inline std::uint8_t TBufEC_GetBoolean(TBufEC* Self) {
        Self->EnsureReadable(static_cast<std::int32_t>(sizeof(std::uint8_t)));
        return pas::read_and_advance<std::uint8_t>(Self->Data, &Self->Position);
    }

} // namespace EC_Buf
