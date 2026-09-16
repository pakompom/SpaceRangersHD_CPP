#pragma once
#include "types/ab_W02.hpp"

namespace ab_W02 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W02::TabW02) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W02::TabW02, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W02::TabW02, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W02::TabW02, Phase) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W02::TabW02, ExpireTick) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W02::TabW02, ArmTick) == 192);

} // namespace ab_W02
