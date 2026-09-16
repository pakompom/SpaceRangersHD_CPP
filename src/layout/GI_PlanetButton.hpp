#pragma once
#include "types/GI_PlanetButton.hpp"

namespace GI_PlanetButton {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PlanetButton::TPlanetButtonGI) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PlanetButton::TPlanetButtonGI, NormalPlanet) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PlanetButton::TPlanetButtonGI, HoverPlanet) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PlanetButton::TPlanetButtonGI, TextLabel) == 328);

} // namespace GI_PlanetButton
