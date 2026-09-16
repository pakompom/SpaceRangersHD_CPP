#pragma once
#include "types/ab_W01.hpp"

namespace ab_W01 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W01::TabW01) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, ValueB0) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, Damage) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, Image) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, Exploding) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, TrailDistance) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, LastTrailPosition) == 200);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, ExpireTick) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, TurnSpeed) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W01::TabW01, TrailImages) == 232);

} // namespace ab_W01
