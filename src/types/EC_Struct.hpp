#pragma once
#include "runtime_support.hpp"

namespace EC_Struct {
    struct TPointF;

    struct TVector3D;

    struct TObjectEx;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TObjectEx : pas::Object {
        PAS_CLASS_META(TObjectEx, pas::Object, "TObjectEx", 4)
        void p_destroy() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TVector3D {
        double X;
        double Y;
        double Z;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TPointF {
        float X;
        float Y;
    };
    #pragma pack(pop)

    using PPointF = TPointF*;

} // namespace EC_Struct
