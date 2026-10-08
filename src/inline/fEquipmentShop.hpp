#pragma once
#include "units/EC_Struct.hpp"
#include "units/fEquipmentShop.hpp"

namespace fEquipmentShop {
    // Owns Item while the location's shop list is detached, and frees remaining controls.
    inline void TShopSlot_Create(TShopSlot* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace fEquipmentShop
