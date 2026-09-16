#pragma once
#include "types/ab_W16.hpp"

namespace ab_W16 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W16::TabW16) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W16::TabW16, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W16::TabW16, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W16::TabW16, Phase) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W16::TabW16, ExpireTick) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W16::TabW16, ParentProjectile) == 192);

} // namespace ab_W16
