#pragma once
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "units/aShip.hpp"
#include "units/aWarrior.hpp"

namespace aWarrior {
    // AI ownership check only; does not test travel range.
    inline std::uint8_t TWarrior_CanQueueReachablePlanet(TWarrior* Self, aPlanet::TPlanet* Planet) {
        return Planet->OwnerId != aGalaxyStruct::oiDominator && Planet->OwnerId != aGalaxyStruct::oiPirate;
    }

    inline std::uint8_t TWarrior_TrustsAttackRequester(TWarrior* Self, aShip::TShip* Ship) {
        return aShip::TShip_RelationToShip(Self, Ship) >= aGalaxyStruct::RelationNormalMin;
    }

} // namespace aWarrior
