#pragma once
#include "types/ab_Ship.hpp"

namespace ab_Ship {
    static_assert(sizeof(void*) != 4 || sizeof(ab_Ship::TabShip) == 736);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, Visual) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, VisualDiameter) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, OffscreenMarker) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, OffscreenLabel) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, Enemies) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, InitialEnemies) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, TrackedShips) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, TurnSpeed) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, TurnInput) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, WeaponCount) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, Weapons) == 264);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, PrimaryWeapon) == 504);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, SecondaryWeapon) == 508);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, LastPrimaryWeapon) == 512);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, LastPrimaryFireTick) == 516);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, LastSecondaryWeapon) == 520);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, LastSecondaryFireTick) == 524);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, BonusTicks) == 528);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, RevealTicks) == 560);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, OuterAvoidanceDistance) == 568);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, MiddleAvoidanceDistance) == 576);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, InnerAvoidanceDistance) == 584);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, NextObstacleScanTick) == 592);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, ObstacleDistances) == 600);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, ObstacleLevels) == 664);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, EncounterTag) == 696);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, TickCounter) == 700);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, ConvertedFromGameShip) == 704);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, RandomRewardsDisabled) == 705);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, SpawnGraphKey) == 708);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, ScriptLabel) == 712);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, RewardObject) == 716);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, Team) == 720);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, HealthScalePercent) == 724);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, DamageScalePercent) == 728);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Ship::TabShip, HasFiredWeapon) == 732);

} // namespace ab_Ship
