#pragma once
#include "types/ab_Object.hpp"

namespace ab_Object {
    static_assert(sizeof(void*) != 4 || sizeof(ab_Object::TabObject) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, State) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, Mass) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, Thrust) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, Velocity) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, MaxSpeed) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, SpeedScale) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, DistanceTravelled) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, CollisionRadius) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, Collidable) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, ZoneRadius) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, DeletionPending) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, WallCollisionEnabled) == 113);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, GravityEnabled) == 114);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, ZoneDamageEnabled) == 115);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, Active) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, SourceObject) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, InitialRandomSeed) == 124);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, RandomState) == 128);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, SoundDelay) == 132);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, SoundPath) == 136);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, SoundGroup) == 140);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, Sound) == 144);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, WeaponDamageScale) == 148);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, AmmoRechargeScale) == 152);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, MovementScale) == 156);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, GravityScale) == 160);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, RegenerationRate) == 164);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, DamageTakenScale) == 168);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Object::TabObject, LuckScale) == 172);

} // namespace ab_Object
