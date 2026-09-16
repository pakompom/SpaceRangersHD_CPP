#pragma once
#include "types/ab_W09.hpp"

namespace ab_W09 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W09::TabW09) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W09::TabW09, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W09::TabW09, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W09::TabW09, Phase) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W09::TabW09, ExpireTick) == 188);

} // namespace ab_W09
