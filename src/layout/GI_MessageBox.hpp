#pragma once
#include "types/GI_MessageBox.hpp"

namespace GI_MessageBox {
    static_assert(sizeof(void*) != 4 || sizeof(GI_MessageBox::TMessageBoxGI) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MessageBox::TMessageBoxGI, MessageText) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MessageBox::TMessageBoxGI, Options) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MessageBox::TMessageBoxGI, UnusedOption) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MessageBox::TMessageBoxGI, OffsetX) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MessageBox::TMessageBoxGI, OffsetY) == 224);

} // namespace GI_MessageBox
