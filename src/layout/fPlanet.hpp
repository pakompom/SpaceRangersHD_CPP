#pragma once
#include "types/fPlanet.hpp"

namespace fPlanet {
    static_assert(sizeof(void*) != 4 || sizeof(fPlanet::TfPlanet) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanet::TfPlanet, PlanetPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanet::TfPlanet, LoadPanel) == 216);

} // namespace fPlanet
