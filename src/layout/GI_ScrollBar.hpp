#pragma once
#include "types/GI_ScrollBar.hpp"

namespace GI_ScrollBar {
    static_assert(sizeof(void*) != 4 || sizeof(GI_ScrollBar::TScrollBarGI) == 472);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, Minimum) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, Maximum) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, Position) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, LargeChange) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, SmallChange) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, PageSize) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, Orientation) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, CalculationMode) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, UpImages) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, BeforeThumbBarImages) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, ThumbTopImages) == 376);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, ThumbCenterImages) == 388);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, ThumbBottomImages) == 400);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, AfterThumbBarImages) == 412);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, DownImages) == 424);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, RepeatTimer) == 436);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, PressedRegion) == 440);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, HoveredRegion) == 444);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, DragStartPosition) == 448);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, MinimumLabel) == 452);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, MaximumLabel) == 456);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, PositionLabel) == 460);
    static_assert(sizeof(void*) != 4 || offsetof(GI_ScrollBar::TScrollBarGI, PositionChangedCallback) == 464);

} // namespace GI_ScrollBar
