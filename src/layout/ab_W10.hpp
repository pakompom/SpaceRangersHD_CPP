#pragma once
#include "types/ab_W10.hpp"

namespace ab_W10 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W10::TabW10) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W10::TabW10, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W10::TabW10, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W10::TabW10, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W10::TabW10, TrailDistance) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W10::TabW10, LastTrailPosition) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W10::TabW10, ExpireTick) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W10::TabW10, TurnSpeed) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W10::TabW10, TrailImages) == 224);

} // namespace ab_W10
