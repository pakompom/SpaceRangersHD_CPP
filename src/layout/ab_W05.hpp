#pragma once
#include "types/ab_W05.hpp"

namespace ab_W05 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W05::TabW05) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W05::TabW05, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W05::TabW05, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W05::TabW05, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W05::TabW05, TrailDistance) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W05::TabW05, LastTrailPosition) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W05::TabW05, ExpireTick) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W05::TabW05, TurnDelta) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W05::TabW05, TrailImages) == 224);

} // namespace ab_W05
