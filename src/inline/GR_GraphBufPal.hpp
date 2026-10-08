#pragma once
#include "units/EC_Struct.hpp"
#include "units/GR_GraphBufPal.hpp"

namespace GR_GraphBufPal {
    inline void TGraphBufPalGR_Create(TGraphBufPalGR* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace GR_GraphBufPal
