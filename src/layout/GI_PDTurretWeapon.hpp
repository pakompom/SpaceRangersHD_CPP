#pragma once
#include "types/GI_PDTurretWeapon.hpp"

namespace GI_PDTurretWeapon {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PDTurretWeapon::TPSPDWeaponGI) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPSPDWeaponGI, Particles) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPSPDWeaponGI, ParticleCount) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPSPDWeaponGI, ParticleCapacity) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPSPDWeaponGI, OriginalLength) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPSPDWeaponGI, ParticleColor) == 320);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PDTurretWeapon::TPDWeaponParticle) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPDWeaponParticle, Kind) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPDWeaponParticle, Position) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPDWeaponParticle, Color) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPDWeaponParticle, Alpha) == 14);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPDWeaponParticle, Velocity) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PDTurretWeapon::TPDWeaponParticle, Unknown1A) == 26);

} // namespace GI_PDTurretWeapon
