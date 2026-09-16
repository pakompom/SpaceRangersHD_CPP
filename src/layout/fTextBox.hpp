#pragma once
#include "types/fTextBox.hpp"

namespace fTextBox {
    static_assert(sizeof(void*) != 4 || sizeof(fTextBox::TfTextBox) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fTextBox::TfTextBox, Caption) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fTextBox::TfTextBox, Edit) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fTextBox::TfTextBox, Value) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fTextBox::TfTextBox, AcceptButton) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fTextBox::TfTextBox, MaximumLength) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fTextBox::TfTextBox, OffsetX) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fTextBox::TfTextBox, OffsetY) == 232);

} // namespace fTextBox
