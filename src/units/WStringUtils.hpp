#pragma once
#include "types/WStringUtils.hpp"

namespace WStringUtils {
    PStartupWideString AllocateStartupWideString(std::int32_t Length);

    std::uint8_t FreeStartupWideString(pas::Var<PStartupWideString> Text);

    PStartupWideString TruncateStartupWideString(pas::Var<PStartupWideString> Text);

} // namespace WStringUtils
