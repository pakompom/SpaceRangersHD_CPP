#pragma once
#include "types/GI_PanelScrollBar.hpp"

namespace GI_PanelScrollBar {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PanelScrollBar::TPanelScrollBarGI) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PanelScrollBar::TPanelScrollBarGI, HorizontalScrollBar) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PanelScrollBar::TPanelScrollBarGI, VerticalScrollBar) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PanelScrollBar::TPanelScrollBarGI, AutoHorizontalPlacement) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PanelScrollBar::TPanelScrollBarGI, AutoVerticalPlacement) == 329);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PanelScrollBar::TPanelScrollBarGI, HorizontalScrollBarRect) == 330);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PanelScrollBar::TPanelScrollBarGI, VerticalScrollBarRect) == 346);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PanelScrollBar::TPanelScrollBarGI, ScrollbarsOutside) == 362);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PanelScrollBar::TPanelScrollBarGI, UnlimitedWorld) == 363);

} // namespace GI_PanelScrollBar
