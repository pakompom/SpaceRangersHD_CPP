#pragma once
#include "types/GI_PSWeapon03Lezka.hpp"

namespace GI_PSWeapon03Lezka {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon03Lezka::TPSWeapon03Lezka) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TPSWeapon03Lezka, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TPSWeapon03Lezka, FirstParticle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TPSWeapon03Lezka, LastParticle) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TPSWeapon03Lezka, ProjectionBounds) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TPSWeapon03Lezka, LengthScale) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TPSWeapon03Lezka, OriginalLength) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TPSWeapon03Lezka, PaletteIndex) == 352);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon03Lezka::TLezkaParticle) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, MaximumAlpha) == 19);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, AlphaStep) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, Velocity) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon03Lezka::TLezkaParticle, State) == 32);

} // namespace GI_PSWeapon03Lezka
