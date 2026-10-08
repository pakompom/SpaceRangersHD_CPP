#pragma once
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"

namespace GI_Panel {
    inline void TPanelGI_Destroy(TPanelGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

} // namespace GI_Panel
