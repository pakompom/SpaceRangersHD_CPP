#pragma once
#include "types/SysUtils.hpp"

namespace SysUtils {
    extern std::uint8_t DecimalSeparator;

    inline pas::AnsiString IntToStr(std::int32_t Value);

    inline pas::AnsiString Int64ToStr(std::int64_t Value);

    // Parses signed decimal or hexadecimal text; raises a conversion exception on invalid input.
    inline std::int32_t StrToInt(pas::AnsiString Value);

    // Zero length returns True.
    inline std::uint8_t CompareMem(void* Buffer1, void* Buffer2, std::int32_t ByteCount);

    // DCC32 MAP SysUtils.StrToInt64. Source rtl/sys/SysUtils.pas:5035.
    inline std::int64_t StrToInt64(const pas::AnsiString& S);

} // namespace SysUtils

#include "inline/SysUtils.hpp"
