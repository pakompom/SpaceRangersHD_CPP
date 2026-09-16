#pragma once
#include "types/EC_CachePlanetTempl.hpp"

namespace EC_CachePlanetTempl {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CachePlanetTempl::TCPlanetTemplControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CachePlanetTempl::TCPlanetTemplEC) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CachePlanetTempl::TCPlanetTemplEC, TemplateData) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CachePlanetTempl::TCPlanetTemplEC, ImageHeight) == 36);

} // namespace EC_CachePlanetTempl
