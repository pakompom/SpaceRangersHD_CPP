#pragma once
#include "types/GI_PSWeapon16Esodafer.hpp"

namespace GI_PSWeapon16Esodafer {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer, FirstParticle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer, LastParticle) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer, Colors) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer, ProjectionBounds) == 322);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer, OriginalLength) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer, LengthScale) == 352);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon16Esodafer::TEsodaferParticle) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TEsodaferParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TEsodaferParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TEsodaferParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TEsodaferParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TEsodaferParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TEsodaferParticle, Velocity) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon16Esodafer::TEsodaferParticle, State) == 28);

} // namespace GI_PSWeapon16Esodafer
