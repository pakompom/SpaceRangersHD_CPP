#pragma once
#include "types/GI_Panel.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Panel {
    void TPanelGI_Create(TPanelGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TPanelGI_Destroy(TPanelGI* Self);

    void TPanelGI_LoadFromConfigPath(TPanelGI* Self, const pas::WideString& Path);

} // namespace GI_Panel
