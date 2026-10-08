#pragma once
#include "units/aItem.hpp"

namespace aItem {
    inline void TCargoHook_Create(TCargoHook* Self) {
        aItem::TEquipment_Create(Self);
    }

} // namespace aItem
