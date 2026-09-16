#pragma once
#include "types/ab_W11.hpp"

namespace ab_W11 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W11::TabW11) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W11::TabW11, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W11::TabW11, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W11::TabW11, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W11::TabW11, LastTrailPosition) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W11::TabW11, ExpireTick) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W11::TabW11, TurnSpeed) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W11::TabW11, TrailImages) == 224);

} // namespace ab_W11
