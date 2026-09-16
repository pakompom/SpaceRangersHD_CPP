#pragma once
#include "types/ab_W06.hpp"

namespace ab_W06 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W06::TabW06) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W06::TabW06, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W06::TabW06, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W06::TabW06, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W06::TabW06, ExpireTick) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W06::TabW06, TurnSpeed) == 192);

} // namespace ab_W06
