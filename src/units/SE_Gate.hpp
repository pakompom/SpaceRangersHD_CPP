#pragma once
#include "types/SE_Gate.hpp"
#include "types/Types.hpp"

namespace SE_Gate {
    void TGateSE_Create(TGateSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition);

    void TGateSE_Destroy(TGateSE* Self);

    void TGateEffectSE_Create(TGateEffectSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition);

    void TGateEffectSE_Destroy(TGateEffectSE* Self);

} // namespace SE_Gate
