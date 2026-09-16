#pragma once
#include "types/ab_W18.hpp"

namespace ab_W18 {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W18::TabW18) == 204);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W18::TabW18, Damage) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W18::TabW18, Image) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W18::TabW18, Exploding) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W18::TabW18, ExpireTick) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W18::TabW18, OrbitAngle) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W18::TabW18, AngleCorrection) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W18::TabW18, OrbitRadius) == 200);

} // namespace ab_W18
