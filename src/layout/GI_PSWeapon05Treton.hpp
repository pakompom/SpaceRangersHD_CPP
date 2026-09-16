#pragma once
#include "types/GI_PSWeapon05Treton.hpp"

namespace GI_PSWeapon05Treton {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon05Treton::TPSWeapon05Treton) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TPSWeapon05Treton, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TPSWeapon05Treton, FirstParticle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TPSWeapon05Treton, LastParticle) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TPSWeapon05Treton, PrimaryColor) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TPSWeapon05Treton, SecondaryColor) == 318);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TPSWeapon05Treton, ProjectionBounds) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TPSWeapon05Treton, LengthScale) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TPSWeapon05Treton, OriginalLength) == 344);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon05Treton::TTretonParticle) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, MaximumAlpha) == 19);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, Velocity) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, Countdown) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon05Treton::TTretonParticle, State) == 29);

} // namespace GI_PSWeapon05Treton
