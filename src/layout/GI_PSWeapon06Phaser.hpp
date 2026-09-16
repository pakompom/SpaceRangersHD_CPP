#pragma once
#include "types/GI_PSWeapon06Phaser.hpp"

namespace GI_PSWeapon06Phaser {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon06Phaser::TPSWeapon06Phaser) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPSWeapon06Phaser, Particles) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPSWeapon06Phaser, ParticleCount) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPSWeapon06Phaser, ParticleCapacity) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPSWeapon06Phaser, OriginalLength) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPSWeapon06Phaser, PaletteIndex) == 320);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon06Phaser::TPhaserParticle) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Next) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Kind) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Position) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Incoming) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Displacement) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Reflected) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Color) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Alpha) == 34);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, Phase) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, PhaseStep) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon06Phaser::TPhaserParticle, PhaseCountdown) == 44);

} // namespace GI_PSWeapon06Phaser
