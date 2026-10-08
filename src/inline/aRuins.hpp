#pragma once
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "units/aRuins.hpp"

namespace aRuins {
    // Always false.
    inline std::uint8_t TRuins_CanQueueReachablePlanet(TRuins* Self, aPlanet::TPlanet* Planet) {
        return false;
    }

    // Returns false without changing InFear.
    inline std::uint8_t TRuins_RecomputeFearState(TRuins* Self) {
        return false;
    }

    inline std::uint8_t TRuins_AcceptsRansomDemandFrom(TRuins* Self, aShip::TShip* Ship) {
        return false;
    }

    inline std::uint8_t TRuins_TrustsAttackRequester(TRuins* Self, aShip::TShip* Ship) {
        return true;
    }

} // namespace aRuins
