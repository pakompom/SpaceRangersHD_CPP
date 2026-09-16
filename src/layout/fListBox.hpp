#pragma once
#include "types/fListBox.hpp"

namespace fListBox {
    static_assert(sizeof(void*) != 4 || sizeof(fListBox::TfListBox) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fListBox::TfListBox, SelectedIndex) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fListBox::TfListBox, AcceptButton) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fListBox::TfListBox, ScrollPanel) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fListBox::TfListBox, SelectedControl) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fListBox::TfListBox, Caption) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fListBox::TfListBox, Items) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fListBox::TfListBox, OffsetX) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fListBox::TfListBox, OffsetY) == 236);

} // namespace fListBox
