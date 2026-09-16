#pragma once
#include "types/GI_PSEyes.hpp"

namespace GI_PSEyes {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSEyes::TPSEyesGI) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, HalfWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, SegmentLength) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, Dispersion) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, StartingAlpha) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, FirstParticle) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, LastParticle) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, FirstLine) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, LastLine) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, ProjectionBounds) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, PrimaryColor) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, SecondaryColor) == 354);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TPSEyesGI, BeamTicks) == 356);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSEyes::TEyesParticle) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, Origin) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, Position) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, Color) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, Alpha) == 26);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, Velocity) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, State) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesParticle, Countdown) == 40);
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSEyes::TEyesLine) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesLine, Next) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesLine, First) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesLine, Last) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesLine, Color) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSEyes::TEyesLine, Alpha) == 27);

} // namespace GI_PSEyes
