#pragma once
#include "types/GI_ShrLight.hpp"

namespace GI_ShrLight {
    static_assert(sizeof(void*) != 4 || sizeof(GI_ShrLight::TShrLightGI) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ShrLight::TShrLightGI, Kind) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ShrLight::TShrLightGI, LightShift) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ShrLight::TShrLightGI, LightBuffer) == 296);

} // namespace GI_ShrLight
