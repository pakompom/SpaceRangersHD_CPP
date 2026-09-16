#pragma once
#include "types/GI_PSWeapon17Kafacitor.hpp"

namespace GI_PSWeapon17Kafacitor {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor, FirstParticle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor, LastParticle) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor, PrimaryColor) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor, SecondaryColor) == 318);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor, ProjectionBounds) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor, OriginalLength) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor, LengthScale) == 344);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon17Kafacitor::TKafacitorParticle) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TKafacitorParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TKafacitorParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TKafacitorParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TKafacitorParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TKafacitorParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TKafacitorParticle, Velocity) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon17Kafacitor::TKafacitorParticle, State) == 28);

} // namespace GI_PSWeapon17Kafacitor
