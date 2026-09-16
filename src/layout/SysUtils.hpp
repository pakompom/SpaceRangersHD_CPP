#pragma once
#include "types/SysUtils.hpp"

namespace SysUtils {
    static_assert(sizeof(void*) != 4 || sizeof(SysUtils::TSearchRec) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(SysUtils::TSearchRec, Time) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SysUtils::TSearchRec, Size) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SysUtils::TSearchRec, Attr) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SysUtils::TSearchRec, Name) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SysUtils::TSearchRec, ExcludeAttr) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SysUtils::TSearchRec, FindHandle) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(SysUtils::TSearchRec, FindData) == 32);

} // namespace SysUtils
