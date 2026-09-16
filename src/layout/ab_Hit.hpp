#pragma once
#include "types/ab_Hit.hpp"

namespace ab_Hit {
    static_assert(sizeof(void*) != 4 || sizeof(ab_Hit::TabHit) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Hit::TabHit, Health) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Hit::TabHit, MaxHealth) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Hit::TabHit, DisruptUntilTick) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Hit::TabHit, EffectOriginSpread) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Hit::TabHit, TurnSpeedScale) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Hit::TabHit, Effects) == 200);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Hit::TabHit, StateCC) == 204);

} // namespace ab_Hit
