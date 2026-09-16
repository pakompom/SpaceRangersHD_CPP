#pragma once
#include "types/GI_SimpleButton.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_SimpleButton {
    void TSimpleButtonGI_Create(TSimpleButtonGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TSimpleButtonGI_Destroy(TSimpleButtonGI* Self);

} // namespace GI_SimpleButton
