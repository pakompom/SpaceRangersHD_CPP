#pragma once
#include "runtime_support.hpp"

namespace BlockParException {
    struct EBlockPar;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct EBlockPar : pas::Exception {
        PAS_CLASS_META(EBlockPar, pas::Exception, "EBlockPar", 16)
        // False suppresses the reporting flag in ExceptionInfo.
        std::uint8_t IsReportable();
        std::uint8_t Reportable;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace BlockParException
