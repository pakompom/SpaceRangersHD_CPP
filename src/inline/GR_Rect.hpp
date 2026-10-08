#pragma once
#include "units/EC_Struct.hpp"
#include "units/GR_Rect.hpp"

namespace GR_Rect {
    inline void TArrayRectGR_Create(TArrayRectGR* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace GR_Rect
