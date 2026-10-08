#pragma once
#include "runtime_support.hpp"

namespace Types {
    struct TPoint;

    struct TRect;

} // namespace Types

namespace TypesImports {
    inline std::uint8_t PtInRect(const Types::TRect& Rect, const Types::TPoint& P);

} // namespace TypesImports

#include "inline/TypesImports.hpp"
