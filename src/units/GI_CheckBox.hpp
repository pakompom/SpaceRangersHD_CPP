#pragma once
#include "types/GI_CheckBox.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_CheckBox {
    void TCheckBoxGI_Create(TCheckBoxGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TCheckBoxGI_Destroy(TCheckBoxGI* Self);

} // namespace GI_CheckBox
