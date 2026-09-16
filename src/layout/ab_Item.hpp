#pragma once
#include "types/ab_Item.hpp"

namespace ab_Item {
    static_assert(sizeof(void*) != 4 || sizeof(ab_Item::TabItem) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Item::TabItem, Item) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Item::TabItem, BonusKind) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Item::TabItem, HiddenBonus) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Item::TabItem, Visual) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Item::TabItem, SpawnZone) == 192);

} // namespace ab_Item
