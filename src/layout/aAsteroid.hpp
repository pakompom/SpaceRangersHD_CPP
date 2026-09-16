#pragma once
#include "types/aAsteroid.hpp"

namespace aAsteroid {
    static_assert(sizeof(void*) != 4 || sizeof(aAsteroid::TAsteroid) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, Id) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, CurrentStar) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, Position) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, PhysicsPosition) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, Velocity) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, Mass) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, GravityForceFactor) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, InverseMass) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, MineralCount) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, GraphObject) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(aAsteroid::TAsteroid, FilmObject) == 56);

} // namespace aAsteroid
