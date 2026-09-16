#pragma once
#include "types/SE_GAIEffect.hpp"

namespace SE_GAIEffect {
    static_assert(sizeof(void*) != 4 || sizeof(SE_GAIEffect::TGAIEffectSE) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_GAIEffect::TGAIEffectSE, SoundPath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_GAIEffect::TGAIEffectSE, ImagePosition) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_GAIEffect::TGAIEffectSE, DurationScale) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_GAIEffect::TGAIEffectSE, ImagePath) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_GAIEffect::TGAIEffectSE, Animation) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_GAIEffect::TGAIEffectSE, StepsPerFrame) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_GAIEffect::TGAIEffectSE, StepIndex) == 104);

} // namespace SE_GAIEffect
