#pragma once
#include "types/aGalaxy.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "units/aNormalShip.hpp"
#include "units/aShip.hpp"
#include "units/aTransport.hpp"

namespace aTransport {
    inline void TTransport_ResolveLoadedReferences(TTransport* Self, aGalaxy::TGalaxy* Galaxy) {
        aNormalShip::TNormalShip_ResolveLoadedReferences(Self, Galaxy);
    }

    // AI ownership check only; does not test travel range.
    inline std::uint8_t TTransport_CanQueueReachablePlanet(TTransport* Self, aPlanet::TPlanet* Planet) {
        return Planet->OwnerId != aGalaxyStruct::oiDominator;
    }

    inline std::uint8_t TTransport_TrustsAttackRequester(TTransport* Self, aShip::TShip* Ship) {
        return aShip::TShip_RelationToShip(Self, Ship) >= aGalaxyStruct::RelationNormalMin;
    }

} // namespace aTransport
