#pragma once
#include "types/SE_GAIEffect.hpp"
#include "types/Types.hpp"

namespace SE_GAIEffect {
    void TGAIEffectSE_Create(TGAIEffectSE* Self, const pas::WideString& GraphKey, Types::TPoint UnusedPosition);

    void TGAIEffectSE_Destroy(TGAIEffectSE* Self);

} // namespace SE_GAIEffect
