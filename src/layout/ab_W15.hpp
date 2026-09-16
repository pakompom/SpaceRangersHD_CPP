#pragma once
#include "types/ab_W15.hpp"

namespace ab_W15 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W15::TabW15) == 200);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W15::TabW15, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W15::TabW15, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W15::TabW15, Phase) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W15::TabW15, TurnSpeed) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W15::TabW15, TurnBias) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W15::TabW15, ExpireTick) == 196);

} // namespace ab_W15
