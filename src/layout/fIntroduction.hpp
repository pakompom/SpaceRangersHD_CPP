#pragma once
#include "types/fIntroduction.hpp"

namespace fIntroduction {
    static_assert(sizeof(void*) != 4 || sizeof(fIntroduction::TfIntroduction) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fIntroduction::TfIntroduction, GenerationProgressTimer) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fIntroduction::TfIntroduction, TextScrollTimer) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fIntroduction::TfIntroduction, BackgroundTimer) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fIntroduction::TfIntroduction, BackgroundScrollOffset) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fIntroduction::TfIntroduction, TextPanelTop) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fIntroduction::TfIntroduction, TextPanelHeight) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fIntroduction::TfIntroduction, ProgressPulsePhase) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fIntroduction::TfIntroduction, ContinueBlinkTimer) == 236);

} // namespace fIntroduction
