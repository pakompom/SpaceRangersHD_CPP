#pragma once
#include "types/ab_Global.hpp"

namespace ab_Global {
    static_assert(sizeof(void*) != 4 || sizeof(ab_Global::TSphericalBearingState) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Global::TSphericalBearingState, LongitudeDegrees) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Global::TSphericalBearingState, PolarAngleDegrees) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Global::TSphericalBearingState, BearingDegrees) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(ab_Global::TSphericalBearingDistance) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Global::TSphericalBearingDistance, BearingDeltaDegrees) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Global::TSphericalBearingDistance, Distance) == 8);

} // namespace ab_Global
