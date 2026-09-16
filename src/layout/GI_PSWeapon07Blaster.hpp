#pragma once
#include "types/GI_PSWeapon07Blaster.hpp"

namespace GI_PSWeapon07Blaster {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon07Blaster::TPSWeapon07Blaster) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TPSWeapon07Blaster, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TPSWeapon07Blaster, FirstParticle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TPSWeapon07Blaster, LastParticle) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TPSWeapon07Blaster, PrimaryColor) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TPSWeapon07Blaster, SecondaryColor) == 318);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TPSWeapon07Blaster, ProjectionBounds) == 320);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon07Blaster::TBlasterParticle) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, Velocity) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, State) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, Unknown1E) == 30);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon07Blaster::TBlasterParticle, BaseX) == 36);

} // namespace GI_PSWeapon07Blaster
