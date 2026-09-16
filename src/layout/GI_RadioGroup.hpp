#pragma once
#include "types/GI_RadioGroup.hpp"

namespace GI_RadioGroup {
    static_assert(sizeof(void*) != 4 || sizeof(GI_RadioGroup::TRadioGroupGI) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_RadioGroup::TRadioGroupGI, SelectionChangedCallback) == 288);

} // namespace GI_RadioGroup
