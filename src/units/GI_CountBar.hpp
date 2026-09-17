#pragma once
#include "types/GI_CountBar.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_CountBar {
    void TCountBarGI_Create(TCountBarGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TCountBarGI_Destroy(TCountBarGI* Self);

    void TCountBarGI_LoadFromConfigPath(TCountBarGI* Self, const pas::WideString& Path);

} // namespace GI_CountBar
