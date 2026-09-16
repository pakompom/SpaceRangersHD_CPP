#pragma once
#include "types/GI_PSWeapon02FragCannon.hpp"

namespace GI_PSWeapon02FragCannon {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon02FragCannon::TPSWeapon02FragCannon) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TPSWeapon02FragCannon, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TPSWeapon02FragCannon, FirstParticle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TPSWeapon02FragCannon, LastParticle) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TPSWeapon02FragCannon, PrimaryColor) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TPSWeapon02FragCannon, SecondaryColor) == 318);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TPSWeapon02FragCannon, ProjectionBounds) == 320);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon02FragCannon::TFragCannonParticle) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TFragCannonParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TFragCannonParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TFragCannonParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TFragCannonParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TFragCannonParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TFragCannonParticle, Velocity) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TFragCannonParticle, State) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon02FragCannon::TFragCannonParticle, Unknown1E) == 30);

} // namespace GI_PSWeapon02FragCannon
