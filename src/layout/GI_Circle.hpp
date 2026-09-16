#pragma once
#include "types/GI_Circle.hpp"

namespace GI_Circle {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Circle::TCircleGI) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, Kind) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, Color) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, FillColor) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, Center) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, Radius) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, ShrLightInner) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, ShrLightOuter) == 313);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, LightBufferDirty) == 314);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Circle::TCircleGI, LightBuffer) == 316);

} // namespace GI_Circle
