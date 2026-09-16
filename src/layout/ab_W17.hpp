#pragma once
#include "types/ab_W17.hpp"

namespace ab_W17 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W17::TabW17) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W17::TabW17, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W17::TabW17, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W17::TabW17, Phase) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W17::TabW17, ExpireTick) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W17::TabW17, Partner) == 192);

} // namespace ab_W17
