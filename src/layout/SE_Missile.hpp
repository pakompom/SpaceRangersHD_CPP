#pragma once
#include "types/SE_Missile.hpp"

namespace SE_Missile {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Missile::TMissileSE) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Missile::TMissileSE, Angle) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Missile::TMissileSE, ImageScale) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Missile::TMissileSE, Image) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Missile::TMissileSE, AnimationTimer) == 88);

} // namespace SE_Missile
