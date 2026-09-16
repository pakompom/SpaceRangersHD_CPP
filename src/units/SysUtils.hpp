#pragma once
#include "types/SysUtils.hpp"

namespace SysUtils {
    extern std::uint8_t DecimalSeparator;

    inline pas::AnsiString IntToStr(std::int32_t Value);

    inline pas::AnsiString Int64ToStr(std::int64_t Value);

    inline std::int32_t StrToInt(pas::AnsiString Value);

    inline std::uint8_t CompareMem(void* Buffer1, void* Buffer2, std::int32_t ByteCount);

    inline std::int64_t StrToInt64(const pas::AnsiString& S);

} // namespace SysUtils

#include "inline/SysUtils.hpp"
