#pragma once
#include "types/aGalaxy.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "units/aKling.hpp"
#include "units/aShip.hpp"

namespace aKling {
    inline void TKling_ResolveLoadedReferences(TKling* Self, aGalaxy::TGalaxy* Galaxy) {
        aShip::TShip_ResolveLoadedReferences(Self, Galaxy);
    }

    // AI ownership check only; does not test travel range.
    inline std::uint8_t TKling_CanQueueReachablePlanet(TKling* Self, aPlanet::TPlanet* Planet) {
        return Planet->OwnerId == aGalaxyStruct::oiDominator;
    }

    inline std::uint8_t TKling_RecomputeFearState(TKling* Self) {
        return false;
    }

    inline std::uint8_t TKling_AcceptsRansomDemandFrom(TKling* Self, aShip::TShip* Ship) {
        return false;
    }

    inline std::uint8_t TKling_TrustsAttackRequester(TKling* Self, aShip::TShip* Ship) {
        return false;
    }

} // namespace aKling
