#pragma once
#include "types/GI_PSWeapon11Desintegrator.hpp"

namespace GI_PSWeapon11Desintegrator {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, Wavelength) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, PhaseMask) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, FirstParticle) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, LastParticle) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, PendingSparkSteps) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, Color) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, ProjectionBounds) == 330);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, LengthScale) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator, OriginalLength) == 360);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon11Desintegrator::TDesintegratorParticle) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, Velocity) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, State) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, RemainingTicks) == 30);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon11Desintegrator::TDesintegratorParticle, BaseAlpha) == 32);

} // namespace GI_PSWeapon11Desintegrator
