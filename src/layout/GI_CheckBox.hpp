#pragma once
#include "types/GI_CheckBox.hpp"

namespace GI_CheckBox {
    static_assert(sizeof(void*) != 4 || sizeof(GI_CheckBox::TCheckBoxGI) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CheckBox::TCheckBoxGI, CheckedImage) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CheckBox::TCheckBoxGI, UncheckedImage) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CheckBox::TCheckBoxGI, Checked) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_CheckBox::TCheckBoxGI, ChangedCallback) == 304);

} // namespace GI_CheckBox
