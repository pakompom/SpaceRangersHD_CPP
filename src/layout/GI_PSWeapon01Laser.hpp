#pragma once
#include "types/GI_PSWeapon01Laser.hpp"

namespace GI_PSWeapon01Laser {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon01Laser::TPSWeapon01Laser) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TPSWeapon01Laser, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TPSWeapon01Laser, FirstParticle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TPSWeapon01Laser, LastParticle) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TPSWeapon01Laser, ProjectionBounds) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TPSWeapon01Laser, OriginalLength) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TPSWeapon01Laser, LengthScale) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TPSWeapon01Laser, PaletteIndex) == 352);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon01Laser::TBeamLaserParticle) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TBeamLaserParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TBeamLaserParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TBeamLaserParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TBeamLaserParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TBeamLaserParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TBeamLaserParticle, Velocity) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon01Laser::TBeamLaserParticle, State) == 28);

} // namespace GI_PSWeapon01Laser
