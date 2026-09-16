#pragma once
#include "types/GI_StatusBar.hpp"

namespace GI_StatusBar {
    static_assert(sizeof(void*) != 4 || sizeof(GI_StatusBar::TStatusBarGI) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StatusBar::TStatusBarGI, Minimum) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StatusBar::TStatusBarGI, Maximum) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StatusBar::TStatusBarGI, Value) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StatusBar::TStatusBarGI, LeftImage) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StatusBar::TStatusBarGI, CenterImage) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_StatusBar::TStatusBarGI, RightImage) == 352);

} // namespace GI_StatusBar
