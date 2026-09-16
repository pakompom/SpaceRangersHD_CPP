#pragma once
#include "types/aPlanet.hpp"

namespace aPlanet {
    extern std::int32_t EconomicEventChance;

    extern aPlanet::TDominatorSpawnWeightTable DominatorSpawnWeights;

    extern aPlanet::TPlanet* MainPiratePlanet;

    void TSputnik_Create(TSputnik* Self);

    void TSputnik_Destroy(TSputnik* Self);

    void TPlanet_Create(TPlanet* Self);

    void TPlanet_Destroy(TPlanet* Self);

} // namespace aPlanet
