#pragma once
#include "types/WStringUtils.hpp"

namespace WStringUtils {
    PStartupWideString AllocateStartupWideString(std::int32_t Length);

    // Does not clear the disposed pointer.
    std::uint8_t FreeStartupWideString(pas::Var<PStartupWideString> Text);

    // Shrinks a caller-provided WideString to its first zero. Requires a valid pointer and a terminator within the buffer.
    PStartupWideString TruncateStartupWideString(pas::Var<PStartupWideString> Text);

} // namespace WStringUtils
