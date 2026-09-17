#pragma once
#include "types/GI_GraphButton.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_GraphButton {
    void TGraphButtonGI_Create(TGraphButtonGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TGraphButtonGI_Destroy(TGraphButtonGI* Self);

    void TGraphButtonGI_LoadFromConfigPath(TGraphButtonGI* Self, const pas::WideString& Path);

} // namespace GI_GraphButton
