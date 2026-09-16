#pragma once
#include "types/fGoodsShop2.hpp"

namespace fGoodsShop2 {
    static_assert(sizeof(void*) != 4 || sizeof(fGoodsShop2::TGoodsShopTradeRow) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TGoodsShopTradeRow, Count) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TGoodsShopTradeRow, MaximumPrice) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TGoodsShopTradeRow, PurchasePrice) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TGoodsShopTradeRow, BaseSalePrice) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(fGoodsShop2::TfGoodsShop2) == 404);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, PlanetPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, StationPanel) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, LoadPanel) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, DraggedGoodsIndex) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, NameFaceHeight) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, FaceCaptionHeight) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, FlagEC) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, TradeRows) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, PartnerCargoLimit) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, PartnerMoneyLimit) == 372);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, MoneyWarningActive) == 376);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, MoneyWarningTicks) == 380);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, MoneyWarningTimer) == 384);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, CargoWarningActive) == 388);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, CargoWarningTicks) == 392);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, CargoWarningTimer) == 396);
    static_assert(sizeof(void*) != 4 || offsetof(fGoodsShop2::TfGoodsShop2, AmbientSound) == 400);

} // namespace fGoodsShop2
