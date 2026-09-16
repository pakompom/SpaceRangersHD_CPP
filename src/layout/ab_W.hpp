#pragma once
#include "types/ab_W.hpp"

namespace ab_W {
    static_assert(sizeof(void*) != 4 || sizeof(ab_W::TabWeapon) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, Kind) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, ItemType) == 1);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, SlotData) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, Ammo) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, MaxAmmo) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, RechargePerTick) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, AmmoCost) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, LastFireTick) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, FireIntervalTicks) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, Damage) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ab_W::TabWeapon, Range) == 40);

} // namespace ab_W
