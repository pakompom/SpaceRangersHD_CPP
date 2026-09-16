#pragma once
#include "types/EC_CachePlanetTempl.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CachePlanetTempl {
    TCPlanetTemplEC* AcquireOrCreatePlanetTemplate(EC_Cache::TCacheControlEC* Control);

    void TCPlanetTemplEC_Create(TCPlanetTemplEC* Self);

    void TCPlanetTemplEC_Destroy(TCPlanetTemplEC* Self);

} // namespace EC_CachePlanetTempl
