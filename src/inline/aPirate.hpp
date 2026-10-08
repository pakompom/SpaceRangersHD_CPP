#pragma once
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "units/aPirate.hpp"
#include "units/aShip.hpp"

namespace aPirate {
    // AI ownership check only; does not test travel range.
    inline std::uint8_t TPirate_CanQueueReachablePlanet(TPirate* Self, aPlanet::TPlanet* Planet) {
        return Planet->OwnerId != aGalaxyStruct::oiDominator && (Self->PirateType == 0 || Planet->OwnerId ==
                    Self->OwnerId);
    }

    inline std::uint8_t TPirate_TrustsAttackRequester(TPirate* Self, aShip::TShip* Ship) {
        return aShip::TShip_RelationToShip(Self, Ship) >= aGalaxyStruct::RelationNormalMin;
    }

} // namespace aPirate
