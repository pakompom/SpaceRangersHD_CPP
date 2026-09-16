#pragma once
#include "types/EC_Struct.hpp"

namespace EC_Struct {
    static_assert(sizeof(void*) != 4 || sizeof(EC_Struct::TObjectEx) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Struct::TVector3D) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Struct::TVector3D, X) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Struct::TVector3D, Y) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Struct::TVector3D, Z) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Struct::TPointF) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Struct::TPointF, X) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Struct::TPointF, Y) == 4);

} // namespace EC_Struct
