#pragma once
#include "runtime_support.hpp"

namespace SysUtilsImports {
    struct EAccessViolation;

    using TByteArray = pas::Array<std::uint8_t, 0, 32767>;

    using PByteArray = TByteArray*;

    enum TFloatFormat : std::uint8_t {
        ffGeneral = 0,
        ffExponent = 1,
        ffFixed = 2,
        ffNumber = 3,
        ffCurrency = 4,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct EAccessViolation : pas::ExternalError {
        PAS_CLASS_META(EAccessViolation, pas::ExternalError, "EAccessViolation", 16)
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    inline constexpr std::int32_t faHidden = 2;

    inline constexpr std::int32_t faSysFile = 4;

    inline constexpr std::int32_t faDirectory = 16;

    inline constexpr std::int32_t faAnyFile = 63;

} // namespace SysUtilsImports
