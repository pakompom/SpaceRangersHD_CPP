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
        void FreeItems();
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPolarPoint {
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

} // namespace aMyFunction
