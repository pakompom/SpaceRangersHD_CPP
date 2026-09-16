#pragma once
#include "types/GI_TextButton.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_TextButton {
    void TTextButtonGI_Create(TTextButtonGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TTextButtonGI_Destroy(TTextButtonGI* Self);

} // namespace GI_TextButton
