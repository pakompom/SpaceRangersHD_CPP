#pragma once
#include "types/GI_RadialEffect.hpp"

namespace GI_RadialEffect {
    static_assert(sizeof(void*) != 4 || sizeof(GI_RadialEffect::TPSRadEffectGI) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, FirstParticle) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, LastParticle) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, Color) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, Radius) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, Alpha) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, PaletteIndex) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, ProjectionBounds) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, LatticeOffset) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TPSRadEffectGI, PositionInitialized) == 352);
    static_assert(sizeof(void*) != 4 || sizeof(GI_RadialEffect::TRadiationParticle) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TRadiationParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TRadiationParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TRadiationParticle, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TRadiationParticle, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TRadiationParticle, Alpha) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadialEffect::TRadiationParticle, EdgeDistance) == 32);

} // namespace GI_RadialEffect
