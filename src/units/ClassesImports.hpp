#pragma once
#include "runtime_support.hpp"
#include "types/Types.hpp"

namespace ClassesImports {
    inline Types::TPoint Point(std::int32_t X, std::int32_t Y);

    inline Types::TRect Rect(std::int32_t Left, std::int32_t Top, std::int32_t Right, std::int32_t Bottom);

} // namespace ClassesImports

#include "inline/ClassesImports.hpp"
