#pragma once
#include "types/GI_CountBar.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_CountBar {
    void TCountBarGI_Create(TCountBarGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TCountBarGI_Destroy(TCountBarGI* Self);

} // namespace GI_CountBar
