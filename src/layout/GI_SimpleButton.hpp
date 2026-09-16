#pragma once
#include "types/GI_SimpleButton.hpp"

namespace GI_SimpleButton {
    static_assert(sizeof(void*) != 4 || sizeof(GI_SimpleButton::TSimpleButtonGI) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SimpleButton::TSimpleButtonGI, CurrentImage) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SimpleButton::TSimpleButtonGI, NormalImage) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_SimpleButton::TSimpleButtonGI, ActiveImage) == 296);

} // namespace GI_SimpleButton
