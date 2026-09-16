#pragma once
#include "types/GI_ScrollBar.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_ScrollBar {
    void TScrollBarGI_Create(TScrollBarGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TScrollBarGI_Destroy(TScrollBarGI* Self);

} // namespace GI_ScrollBar
