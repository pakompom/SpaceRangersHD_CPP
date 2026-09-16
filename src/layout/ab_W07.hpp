#pragma once
#include "types/ab_W07.hpp"

namespace ab_W07 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W07::TabW07) == 204);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W07::TabW07, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W07::TabW07, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W07::TabW07, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W07::TabW07, TrailDistance) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W07::TabW07, ExpireTick) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W07::TabW07, TurnSpeed) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W07::TabW07, TrailImages) == 200);

} // namespace ab_W07
