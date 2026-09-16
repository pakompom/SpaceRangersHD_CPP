#pragma once
#include "types/aNormalShip.hpp"

namespace aGalaxy {
    struct TStar;

} // namespace aGalaxy

namespace aNormalShip {
    void ProcessSystemLiberationRewards(TNormalShip* SourceShip, aGalaxy::TStar* Star);

    void TNormalShip_Create(TNormalShip* Self);

    void TNormalShip_Destroy(TNormalShip* Self);

} // namespace aNormalShip
