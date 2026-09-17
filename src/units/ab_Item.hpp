#pragma once
#include "types/ab_Item.hpp"
#include "types/ab_Zone.hpp"

namespace aItem {
    struct TItem;

} // namespace aItem

namespace ab_Object {
    struct TabObject;

} // namespace ab_Object

namespace ab_Item {
    void ab_Item_Update();

    void ab_Item_Drop(ab_Object::TabObject* Origin, aItem::TItem* Item, std::int32_t MinDistance, std::int32_t MaxDistance);

    TabItem* ab_Item_FindNearestBonus(ab_Zone::PabZone Origin);

    TabItem* ab_Item_FindBonusRoute(ab_Zone::PabZone Origin, ab_Zone::PabZone& Zone);

    TabItem* ab_Item_FindRepairRoute(ab_Zone::PabZone Origin, ab_Zone::PabZone& Zone);

    void TabItem_Create(TabItem* Self);

    void TabItem_Destroy(TabItem* Self);

} // namespace ab_Item
