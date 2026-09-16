#pragma once
#include "runtime_support.hpp"

namespace BreakMessageGIException {
    struct EBreakMessageGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct EBreakMessageGI : pas::Abort {
        PAS_CLASS_META(EBreakMessageGI, pas::Abort, "EBreakMessageGI", 12)
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace BreakMessageGIException
