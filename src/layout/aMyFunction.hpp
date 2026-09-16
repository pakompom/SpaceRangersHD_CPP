#pragma once
#include "types/aMyFunction.hpp"

namespace aMyFunction {
    static_assert(sizeof(void*) != 4 || sizeof(aMyFunction::TObjectList) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(aMyFunction::TPolarPoint) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aMyFunction::TPolarPoint, AngleDegrees) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aMyFunction::TPolarPoint, Radius) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(aMyFunction::TPolarRadiansPoint) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aMyFunction::TPolarRadiansPoint, AngleRadians) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aMyFunction::TPolarRadiansPoint, Radius) == 8);

} // namespace aMyFunction
