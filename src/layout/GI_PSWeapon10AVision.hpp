#pragma once
#include "types/GI_PSWeapon10AVision.hpp"

namespace GI_PSWeapon10AVision {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon10AVision::TPSWeapon10AVision) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TPSWeapon10AVision, Unknown130) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TPSWeapon10AVision, Particles) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TPSWeapon10AVision, ParticleCount) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TPSWeapon10AVision, ParticleCapacity) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TPSWeapon10AVision, OriginalLength) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TPSWeapon10AVision, Colors) == 324);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon10AVision::TAVisionParticle) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TAVisionParticle, Kind) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TAVisionParticle, Position) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TAVisionParticle, Color) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TAVisionParticle, Alpha) == 14);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TAVisionParticle, Velocity) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TAVisionParticle, FadeInTicks) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TAVisionParticle, InitialFadeInTicks) == 25);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon10AVision::TAVisionParticle, FadeOutThreshold) == 26);

} // namespace GI_PSWeapon10AVision
