#pragma once
#include "types/fPlanetNO.hpp"

namespace fPlanetNO {
    extern const float ProbeTrajectoryHitRadiusSquared;

    void TfPlanetNO_Create(TfPlanetNO* Self);

    void TfPlanetNO_Destroy(TfPlanetNO* Self);

} // namespace fPlanetNO
