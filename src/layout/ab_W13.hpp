#pragma once
#include "types/ab_W13.hpp"

namespace ab_W13 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W13::TabW13) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W13::TabW13, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W13::TabW13, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W13::TabW13, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W13::TabW13, Generation) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W13::TabW13, ExpireTick) == 192);

} // namespace ab_W13
