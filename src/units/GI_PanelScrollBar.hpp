#pragma once
#include "types/GI_PanelScrollBar.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PanelScrollBar {
    void TPanelScrollBarGI_Create(TPanelScrollBarGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TPanelScrollBarGI_Destroy(TPanelScrollBarGI* Self);

} // namespace GI_PanelScrollBar
