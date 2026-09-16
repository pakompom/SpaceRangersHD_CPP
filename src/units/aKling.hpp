#pragma once
#include "types/aKling.hpp"

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aKling {
    // Maximum-size index followed by minimum-size index.
    extern pas::Array<pas::Array<std::int32_t, 0, 1>, 0, 7> DominatorEquipmentSizeIndices;

    extern pas::Array<std::int32_t, 1, 7> DominatorWeaponDistributionByTier;

    extern pas::Array<pas::Array<pas::Array<std::int32_t, 50, 64>, 0, 7>, 1, 4> DominatorWeaponWeights;

    // Control threshold and paired equipment/weapon generation bounds.
    extern pas::Array<pas::Array<std::int32_t, 1, 19>, 1, 7> DominatorGenerationTuning;

    extern aKling::TKling* BlazerShip;

    extern aKling::TKling* KellerShip;

    extern aKling::TKling* TerronShip;

    // PIECECREATOR target selected during new-game generation and persisted with the galaxy.
    extern std::uint32_t PieceCreatorTargetStarId;

    extern aPlanet::TPlanet* DominatorSpawnPlanet;

    void TKling_Destroy(TKling* Self);

} // namespace aKling
