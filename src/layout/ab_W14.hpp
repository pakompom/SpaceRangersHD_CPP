#pragma once
#include "types/ab_W14.hpp"

namespace ab_W14 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W14::TabW14) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W14::TabW14, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W14::TabW14, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W14::TabW14, Phase) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W14::TabW14, ExpireTick) == 188);

} // namespace ab_W14
