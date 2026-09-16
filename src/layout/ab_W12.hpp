#pragma once
#include "types/ab_W12.hpp"

namespace ab_W12 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W12::TabW12) == 204);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W12::TabW12, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W12::TabW12, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W12::TabW12, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W12::TabW12, ExpireTick) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W12::TabW12, OrbitAngle) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W12::TabW12, OrbitRadius) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W12::TabW12, TrailImages) == 200);

} // namespace ab_W12
