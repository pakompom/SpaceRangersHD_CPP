#pragma once
#include "types/GI_RadioGroup.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RadioGroup {
    void TRadioGroupGI_Create(TRadioGroupGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TRadioGroupGI_Destroy(TRadioGroupGI* Self);

} // namespace GI_RadioGroup
