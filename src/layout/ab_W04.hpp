#pragma once
#include "types/ab_W04.hpp"

namespace ab_W04 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W04::TabW04) == 200);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W04::TabW04, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W04::TabW04, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W04::TabW04, Phase) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W04::TabW04, ExpireTick) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W04::TabW04, AimTick) == 192);

} // namespace ab_W04
