#pragma once
#include "types/fGameEnd.hpp"

namespace fGameEnd {
    static_assert(sizeof(void*) != 4 || sizeof(fGameEnd::TfGameEnd) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fGameEnd::TfGameEnd, TextScrollTimer) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fGameEnd::TfGameEnd, TextPanelTop) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fGameEnd::TfGameEnd, TextPanelHeight) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fGameEnd::TfGameEnd, BackgroundTimer) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fGameEnd::TfGameEnd, BackgroundScrollOffset) == 224);

} // namespace fGameEnd
