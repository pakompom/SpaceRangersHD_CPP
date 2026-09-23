#pragma once
#include "runtime_support.hpp"

namespace aMyFunction {
    struct TPolarPoint;

    struct TPolarRadiansPoint;

    struct TObjectList;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TObjectList : pas::List {
        PAS_CLASS_META(TObjectList, pas::List, "TObjectList", 16)
        void p_destroy() override;
        // Inherited Clear/Delete do not free objects.
        void FreeItems();
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Natural Double alignment is visible in TPlanet.PredictPosition locals.
    #pragma pack(push, 1)
    struct TPolarPoint {
        // Clockwise from the negative Y axis.
        double AngleDegrees;
        double Radius;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TPolarRadiansPoint {
        double AngleRadians;
        double Radius;
    };
    #pragma pack(pop)

    // Preserve the native angle approximations and random endpoint quantization.
    inline constexpr long double GamePi = 3.1415926L;

    inline constexpr long double GameTwoPi = 6.2831852L;

    inline constexpr std::int32_t RandomFloatResolution = 1000;

} // namespace aMyFunction
