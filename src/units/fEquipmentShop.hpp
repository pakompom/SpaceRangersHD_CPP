#pragma once
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fEquipmentShop.hpp"

namespace aItem {
    struct TItem;

} // namespace aItem

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aRuins {
    struct TRuins;

} // namespace aRuins

namespace fEquipmentShop {
    extern std::int32_t ShopGridRowCount;

    extern std::int32_t ShopVisibleColumnCount;

    extern pas::List* TemporaryShopSlots;

    extern aPlanet::TPlanet* TemporaryShopPlanet;

    extern aRuins::TRuins* TemporaryShopStation;

    extern pas::Array<pas::WideString, 0, 2> ShopDominatorImagePrefixes;

    // Native no-op. Sole caller passes nil; original parameter meaning is unresolved.
    void TemporaryShopStockHook(void* Argument);

    // Transfers ownership of market inventory into the temporary slots.
    void BuildTemporaryShopSlotGrid();

    // Returns ownership of remaining items to the original market.
    void RestoreTemporaryShopStock();

    // Frees remaining items without returning them to the market.
    void ClearTemporaryShopSlotGrid();

    TShopSlot* FindShopSlotByGridPoint(WindowsSdk::TPoint Point);

    TShopSlot* FindShopSlotByItem(aItem::TItem* Item);

    pas::WideString GetShopItemIconName(aItem::TItem* Item);

    // Owns Item while the location's shop list is detached, and frees remaining controls.
    void TShopSlot_Create(TShopSlot* Self);

    void TShopSlot_Destroy(TShopSlot* Self);

    void TfEquipmentShop_Create(TfEquipmentShop* Self);

    void TfEquipmentShop_Destroy(TfEquipmentShop* Self);

} // namespace fEquipmentShop
