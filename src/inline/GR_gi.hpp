#pragma once
#include "units/EC_Struct.hpp"
#include "units/GR_gi.hpp"

namespace GR_gi {
    inline void TgiGR_Create(TgiGR* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace GR_gi
