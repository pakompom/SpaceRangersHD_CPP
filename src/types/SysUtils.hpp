#pragma once
#include "runtime_support.hpp"
#include "types/Windows_group.hpp"

namespace SysUtils {
    struct TSearchRec;

    struct TSearchRec {
        std::int32_t Time;
        std::uint8_t cpp_padding[4];
        std::int64_t Size;
        std::int32_t Attr;
        pas::AnsiString Name;
        std::int32_t ExcludeAttr;
        std::uint32_t FindHandle;
        Windows::TWin32FindDataA FindData;
    };

} // namespace SysUtils
