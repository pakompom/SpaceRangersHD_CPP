#pragma once
#include "types/fPanelPlanet.hpp"

namespace fPanelPlanet {
    static_assert(sizeof(void*) != 4 || sizeof(fPanelPlanet::TfPanelPlanet) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelPlanet::TfPanelPlanet, Screen) == 4);

} // namespace fPanelPlanet
