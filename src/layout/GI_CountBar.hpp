#pragma once
#include "types/GI_CountBar.hpp"

namespace GI_CountBar {
    static_assert(sizeof(void*) != 4 || sizeof(GI_CountBar::TCountBarGI) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, Minimum) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, Maximum) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, Position) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, Orientation) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, Step) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, DecreaseButton) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, IncreaseButton) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, AfterThumbImage) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, BeforeThumbImage) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, ThumbButton) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, MarkerImage) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, PositionChangedCallback) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CountBar::TCountBarGI, RepeatTimer) == 344);

} // namespace GI_CountBar
