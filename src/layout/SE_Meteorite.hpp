#pragma once
#include "types/SE_Meteorite.hpp"

namespace SE_Meteorite {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Meteorite::TMeteoriteSE) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Meteorite::TMeteoriteSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Meteorite::TMeteoriteSE, TimerInterval) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Meteorite::TMeteoriteSE, Speed) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Meteorite::TMeteoriteSE, Angle) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Meteorite::TMeteoriteSE, Animation) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Meteorite::TMeteoriteSE, MoveTimer) == 96);

} // namespace SE_Meteorite
