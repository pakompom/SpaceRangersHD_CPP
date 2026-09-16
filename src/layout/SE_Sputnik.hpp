#pragma once
#include "types/SE_Sputnik.hpp"

namespace SE_Sputnik {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Sputnik::TSputnikSE) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, DepthOrder) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, OrbitCenter) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, OrbitInclination) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, OrbitRotation) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, OrbitAngleStep) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, OrbitTimerInterval) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, OrbitRadius) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, MinDisplayRadius) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, MaxDisplayRadius) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, RotationTimerInterval) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, SurfaceMapStep) == 124);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, OrbitAngle) == 128);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, SurfaceMapOffset) == 132);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, DisplayRadius) == 136);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, LightAngle) == 140);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, InclinationCos) == 144);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, InclinationSin) == 148);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, RotationCos) == 152);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, RotationSin) == 156);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, MinOrbitDepth) == 160);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, MaxOrbitDepth) == 164);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, PlanetControl) == 168);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, OrbitTimer) == 172);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Sputnik::TSputnikSE, RotationTimer) == 176);

} // namespace SE_Sputnik
