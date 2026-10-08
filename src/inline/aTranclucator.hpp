#pragma once
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "units/aTranclucator.hpp"

namespace aTranclucator {
    inline std::uint8_t TTranclucator_CanQueueReachablePlanet(TTranclucator* Self, aPlanet::TPlanet* Planet) {
        return false;
    }

    inline std::uint8_t TTranclucator_RecomputeFearState(TTranclucator* Self) {
        return false;
    }

    inline std::uint8_t TTranclucator_AcceptsRansomDemandFrom(TTranclucator* Self, aShip::TShip* Ship) {
        return false;
    }

    inline std::uint8_t TTranclucator_TrustsAttackRequester(TTranclucator* Self, aShip::TShip* Ship) {
        return Ship == Self->OwnerShip;
    }

} // namespace aTranclucator
