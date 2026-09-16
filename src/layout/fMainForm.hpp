#pragma once
#include "types/fMainForm.hpp"

namespace fMainForm {
    static_assert(sizeof(void*) != 4 || sizeof(fMainForm::TfMainForm) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fMainForm::TfMainForm, BackgroundTimer) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fMainForm::TfMainForm, BackgroundScrollOffset) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fMainForm::TfMainForm, LastMenuShipAnimation) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fMainForm::TfMainForm, LastGaalShipAnimation) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fMainForm::TfMainForm, MenuTextState) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fMainForm::TfMainForm, PopupState) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fMainForm::TfMainForm, LoadPanel) == 240);

} // namespace fMainForm
