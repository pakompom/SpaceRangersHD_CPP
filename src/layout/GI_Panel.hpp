#pragma once
#include "types/GI_Panel.hpp"

namespace GI_Panel {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Panel::TPanelGI) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Panel::TPanelGI, DragScrollingEnabled) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Panel::TPanelGI, ScrollType) == 289);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Panel::TPanelGI, Dragging) == 290);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Panel::TPanelGI, LastDragPoint) == 291);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Panel::TPanelGI, ScrollChangedCallback) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Panel::TPanelGI, ScrollAxis) == 312);

} // namespace GI_Panel
