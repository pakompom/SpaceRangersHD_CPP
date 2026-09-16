#pragma once
#include "types/ab_Zone.hpp"

namespace ab_Zone {
    static_assert(sizeof(void*) != 4 || sizeof(ab_Zone::TabZone) == 144);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, RouteIndex) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Longitude) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, PolarAngle) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, RadiusDegrees) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Radius) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Position) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, RouteDistance) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Routes) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, WorldLines) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, WorldImage) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Name) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, BarrierHealth) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, GravityStrength) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, DamagePerTick) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, BonusFlags) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, BonusRespawnClass) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, NextBonusTick) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Kind) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZone, Segments) == 124);
    static_assert(sizeof(void*) != 4 || sizeof(ab_Zone::TabZoneLink) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZoneLink, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZoneLink, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZoneLink, First) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZoneLink, Last) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZoneLink, Distance) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZoneLink, BarrierLinkMode) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZoneLink, WorldLine) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Zone::TabZoneLink, Segments) == 32);

} // namespace ab_Zone
