#pragma once
#include "types/ab_W08.hpp"

namespace ab_W08 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W08::TabW08) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W08::TabW08, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W08::TabW08, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W08::TabW08, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W08::TabW08, Generation) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W08::TabW08, ExpireTick) == 192);

} // namespace ab_W08
