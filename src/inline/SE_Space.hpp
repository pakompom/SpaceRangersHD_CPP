#pragma once
#include "units/EC_Struct.hpp"
#include "units/SE_Space.hpp"

namespace SE_Space {
    inline void TObjectSE_CreateEmpty(TObjectSE* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace SE_Space
