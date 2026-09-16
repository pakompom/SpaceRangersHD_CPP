#pragma once
#include "types/aGalaxyStruct.hpp"
#include "types/fGoodsShop2.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fGoodsShop2 {
    // Native unit-local copy of the goods presentation order.
    extern const aGalaxyStruct::TGoodsTextOrder ShopGoodsOrder;

    // Native style-derived out-of-stock price markup.
    extern pas::WideString OutOfStockColor;

    // Native modal wrapper used while talking to another ship.
    std::uint8_t RunGoodsShop(GI_MessageLoop::TMessageLoopGI* ParentLoop);

    void TfGoodsShop2_Create(TfGoodsShop2* Self);

    void TfGoodsShop2_Destroy(TfGoodsShop2* Self);

} // namespace fGoodsShop2
