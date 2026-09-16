#pragma once
#include "types/SE_Ship2.hpp"
#include "types/Types.hpp"

namespace SE_Ship2 {
    void TShip2AnimSE_Destroy(TShip2AnimSE* Self);

    void TShip2SE_CreateEmpty(TShip2SE* Self);

    void TShip2SE_Create(TShip2SE* Self, const pas::WideString& GraphKey, Types::TPoint UnusedPosition);

    void TShip2SE_Destroy(TShip2SE* Self);

} // namespace SE_Ship2
