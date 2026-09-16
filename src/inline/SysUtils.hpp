#pragma once
#include "units/SysUtils.hpp"

namespace SysUtils {
    inline pas::AnsiString IntToStr(std::int32_t Value) {
        return pas::int_to_str(Value);
    }

    inline pas::AnsiString Int64ToStr(std::int64_t Value) {
        return pas::int_to_str(Value);
    }

    // Parses signed decimal or hexadecimal text; raises a conversion exception on invalid input.
    inline std::int32_t StrToInt(pas::AnsiString Value) {
        return pas::str_to_int(Value);
    }

    // Zero length returns True.
    inline std::uint8_t CompareMem(void* Buffer1, void* Buffer2, std::int32_t ByteCount) {
        return pas::compare_memory(Buffer1, Buffer2, ByteCount);
    }

    // DCC32 MAP SysUtils.StrToInt64. Source rtl/sys/SysUtils.pas:5035.
    inline std::int64_t StrToInt64(const pas::AnsiString& S) {
        return pas::str_to_int64(S);
    }

} // namespace SysUtils
