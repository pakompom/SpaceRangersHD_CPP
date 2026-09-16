#pragma once
#include "types/GI_StatusBar.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_StatusBar {
    void TStatusBarGI_Create(TStatusBarGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TStatusBarGI_Destroy(TStatusBarGI* Self);

} // namespace GI_StatusBar
