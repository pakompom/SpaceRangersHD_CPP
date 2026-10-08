#pragma once
#include "units/GI_MessageLoop.hpp"
#include "units/GI_RadioGroup.hpp"

namespace GI_RadioGroup {
    inline void TRadioGroupGI_Create(TRadioGroupGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
    }

} // namespace GI_RadioGroup
