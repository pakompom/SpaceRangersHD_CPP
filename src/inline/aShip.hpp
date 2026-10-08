#pragma once
#include "types/aItem.hpp"
#include "units/aShip.hpp"

namespace aShip {
    // Same result as GetWeaponRange in this binary.
    inline std::int32_t TShip_GetWeaponActionRange(TShip* Self, aItem::TWeapon* Weapon) {
        return aShip::TShip_GetWeaponRange(Self, Weapon);
    }

    // Base implementation always returns false.
    inline std::uint8_t TShip_CanDock(TShip* Self, TShip* Ship) {
        return false;
    }

} // namespace aShip
