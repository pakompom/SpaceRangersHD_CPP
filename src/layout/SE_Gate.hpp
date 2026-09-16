#pragma once
#include "types/SE_Gate.hpp"

namespace SE_Gate {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Gate::TGateSE) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, Angle) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, State) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, StateStep) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, LabelText) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, Image) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, TextLabel) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, TextRed) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, TextGreen) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, TextBlue) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateSE, TickCount) == 112);
    static_assert(sizeof(void*) != 4 || sizeof(SE_Gate::TGateEffectSE) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateEffectSE, Angle) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateEffectSE, StateStep) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateEffectSE, Image) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Gate::TGateEffectSE, TickCount) == 88);

} // namespace SE_Gate
