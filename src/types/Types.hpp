#pragma once
#include "runtime_support.hpp"

namespace Types {
    struct TPoint;

    struct TRect;

    #pragma pack(push, 1)
    struct TPoint {
        std::int32_t X;
        std::int32_t Y;
    };
    #pragma pack(pop)

    // Delphi 2007 RTL variant views share these sixteen bytes.
    #pragma pack(push, 1)
    struct TRect {
        std::int32_t Left;
        std::int32_t Top;
        std::int32_t Right;
        std::int32_t Bottom;
    };
    #pragma pack(pop)

    using PRect = TRect*;

    using PPoint = TPoint*;

} // namespace Types
