#pragma once
#include "types/GI_PSWeapon13IMHO.hpp"

namespace GI_PSWeapon13IMHO {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon13IMHO::TPSWeapon13IMHO) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TPSWeapon13IMHO, Particles) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TPSWeapon13IMHO, ParticleCount) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TPSWeapon13IMHO, ParticleCapacity) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TPSWeapon13IMHO, OriginalLength) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TPSWeapon13IMHO, Colors) == 320);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon13IMHO::TIMHOParticle) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TIMHOParticle, Kind) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TIMHOParticle, Position) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TIMHOParticle, Color) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TIMHOParticle, Alpha) == 14);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TIMHOParticle, Velocity) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon13IMHO::TIMHOParticle, Unknown1A) == 26);

} // namespace GI_PSWeapon13IMHO
