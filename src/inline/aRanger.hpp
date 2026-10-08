#pragma once
#include "types/aGalaxyStruct.hpp"
#include "units/aRanger.hpp"
#include "units/aShip.hpp"

namespace aRanger {
    // Relation of at least 30.
    inline std::uint8_t TRanger_TrustsAttackRequester(TRanger* Self, aShip::TShip* Ship) {
        return aShip::TShip_RelationToShip(Self, Ship) >= aGalaxyStruct::RelationNormalMin;
    }

} // namespace aRanger
