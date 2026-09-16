#pragma once
#include "types/fEquipmentShop.hpp"

namespace fEquipmentShop {
    static_assert(sizeof(void*) != 4 || sizeof(fEquipmentShop::TShopSlot) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TShopSlot, GridPoint) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TShopSlot, Item) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TShopSlot, SlotImage) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TShopSlot, BorderImage) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TShopSlot, TypeOverlayImage) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TShopSlot, ItemIconImage) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TShopSlot, ItemAnimation) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TShopSlot, MicroModuleImage) == 36);
    static_assert(sizeof(void*) != 4 || sizeof(fEquipmentShop::TfEquipmentShop) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, PlanetPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, StationPanel) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, LoadPanel) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, ItemInfoAnchor) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, ItemInfoWindow) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, UnknownEC) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, ContentColumnCount) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, TargetScrollX) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, ScrollTimer) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, ItemInfoTimer) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, OpenPreviewTimer) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, PreviewSlot) == 260);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, HullSizeOffset) == 264);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, HullPriceOffset) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(fEquipmentShop::TfEquipmentShop, HullRaceOffset) == 280);

} // namespace fEquipmentShop
