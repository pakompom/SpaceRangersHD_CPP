#pragma once
#include "types/GI_ScrollBar.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_ScrollBar {
    void TScrollBarGI_Create(TScrollBarGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TScrollBarGI_Destroy(TScrollBarGI* Self);

    void TScrollBarGI_LoadFromConfigPath(TScrollBarGI* Self, const pas::WideString& Path);

} // namespace GI_ScrollBar
