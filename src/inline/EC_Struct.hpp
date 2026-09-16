#pragma once
#include "types/Types.hpp"
#include "units/EC_Struct.hpp"

namespace EC_Struct {
    inline std::uint8_t IntersectRects(Types::TRect& Intersection, const Types::TRect& First, const Types::TRect& Second) {
        return pas::intersect_rects(Intersection, First, Second);
    }

} // namespace EC_Struct
