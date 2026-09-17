#pragma once
#include "types/GI_PanelScrollBar.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PanelScrollBar {
    void TPanelScrollBarGI_Create(TPanelScrollBarGI* Self, GI_MessageLoop::TObjectGI* Owner);

    // Frees both scrollbars, including when parented outside this panel.
    void TPanelScrollBarGI_Destroy(TPanelScrollBarGI* Self);

    void TPanelScrollBarGI_LoadFromConfigPath(TPanelScrollBarGI* Self, const pas::WideString& Path);

} // namespace GI_PanelScrollBar
