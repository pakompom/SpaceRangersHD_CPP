#pragma once
#include "types/aPlanet.hpp"

namespace aPlanet {
    // Compared against an inclusive 0..100 roll for each economic event.
    extern std::int32_t EconomicEventChance;

    // Five 32-byte rows, eight Integer weights in TKlingType order; SpawnWeightedDominatorShip selects row 1..5 from faction control.
    extern aPlanet::TDominatorSpawnWeightTable DominatorSpawnWeights;

    // Script.PlanetPirateClan; borrowed reference, retained after PirateWin(3).
    extern aPlanet::TPlanet* MainPiratePlanet;

    void TSputnik_Create(TSputnik* Self);

    void TSputnik_Destroy(TSputnik* Self);

    void TPlanet_Create(TPlanet* Self);

    void TPlanet_Destroy(TPlanet* Self);

} // namespace aPlanet
