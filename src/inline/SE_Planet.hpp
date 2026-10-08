#pragma once
#include "units/SE_Planet.hpp"
#include "units/SE_Space.hpp"

namespace SE_Planet {
    inline void TPlanetSE_Create(TPlanetSE* Self) {
        SE_Space::TObjectSE_CreateEmpty(Self);
    }

} // namespace SE_Planet
