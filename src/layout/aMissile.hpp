#pragma once
#include "types/aMissile.hpp"

namespace aMissile {
    static_assert(sizeof(void*) != 4 || sizeof(aMissile::TMissile) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, Graphic) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, Id) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, WeaponId) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, ItemType) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, TechLevel) == 17);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, MinDamage) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, MaxDamage) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, MicroModuleIndex) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, SpecialModuleIndex) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, Position) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, Direction) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, Speed) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, MaximumSpeed) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, CurrentStar) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, OwnerShip) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, Target) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, PreviousTarget) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, ShotIndex) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, TurnDirection) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, SourceHeading) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, FlightTicks) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, DestroyQueued) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, FilmObject) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, SavedTargetKind) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, SavedPreviousTargetKind) == 97);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, LastTargetPosition) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, LastTargetDistance) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TMissile, OvershootTicks) == 112);
    static_assert(sizeof(void*) != 4 || sizeof(aMissile::TCustomMissile) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(aMissile::TCustomMissile, WeaponInfo) == 116);

} // namespace aMissile
