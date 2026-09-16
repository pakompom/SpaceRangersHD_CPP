#pragma once
#include "types/GI_PSWeapon08ECutter.hpp"

namespace GI_PSWeapon08ECutter {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon08ECutter::TPSWeapon08ECutter) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TPSWeapon08ECutter, Particles) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TPSWeapon08ECutter, ParticleCount) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TPSWeapon08ECutter, ParticleCapacity) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TPSWeapon08ECutter, OriginalLength) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TPSWeapon08ECutter, Colors) == 320);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon08ECutter::TECutterParticle) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TECutterParticle, Kind) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TECutterParticle, Position) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TECutterParticle, Color) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TECutterParticle, Alpha) == 14);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TECutterParticle, Velocity) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon08ECutter::TECutterParticle, Unknown1A) == 26);

} // namespace GI_PSWeapon08ECutter
