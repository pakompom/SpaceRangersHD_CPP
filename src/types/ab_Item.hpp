#pragma once
#include "runtime_support.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_Zone.hpp"

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aItem {
    struct TItem;

} // namespace aItem

namespace ab_Item {
    struct TabItem;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabItem : ab_Object::TabObject {
        PAS_CLASS_META(TabItem, ab_Object::TabObject, "TabItem", 196)
        void p_destroy() override;
        void SetItem(aItem::TItem* Value);
        void SetBonus(std::int32_t Kind, std::uint8_t Hidden, ab_Zone::PabZone Zone);
        void AttachVisual();
        void DetachVisual();
        void UpdateState() override;
        void Advance() override;
        void UpdateVisuals() override;
        aItem::TItem* Item;
        std::int32_t BonusKind;
        std::uint8_t HiddenBonus;
        std::uint8_t cpp_padding[3];
        SE_Space::TObjectSE* Visual;
        ab_Zone::PabZone SpawnZone;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_Item
