#pragma once
#include "types/SysUtilsImports.hpp"

namespace SysUtilsImports {
    static_assert(sizeof(void*) != 4 || sizeof(SysUtilsImports::EAccessViolation) == 16);

} // namespace SysUtilsImports
