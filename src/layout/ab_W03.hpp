#pragma once
#include "types/ab_W03.hpp"

namespace ab_W03 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W03::TabW03) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W03::TabW03, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W03::TabW03, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W03::TabW03, Exploding) == 184);

} // namespace ab_W03
