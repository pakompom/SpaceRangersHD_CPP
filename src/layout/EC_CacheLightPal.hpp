#pragma once
#include "types/EC_CacheLightPal.hpp"

namespace EC_CacheLightPal {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheLightPal::TCLightPalControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheLightPal::TCLightPalEC) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheLightPal::TCLightPalEC, PaletteData) == 32);

} // namespace EC_CacheLightPal
