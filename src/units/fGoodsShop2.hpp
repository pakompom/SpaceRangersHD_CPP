#pragma once
#include "types/aGalaxyStruct.hpp"
#include "types/fGoodsShop2.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fGoodsShop2 {
    extern const aGalaxyStruct::TGoodsTextOrder ShopGoodsOrder;

    extern pas::WideString OutOfStockColor;

    std::uint8_t RunGoodsShop(GI_MessageLoop::TMessageLoopGI* ParentLoop);

    void TfGoodsShop2_Create(TfGoodsShop2* Self);

    void TfGoodsShop2_Destroy(TfGoodsShop2* Self);

} // namespace fGoodsShop2
