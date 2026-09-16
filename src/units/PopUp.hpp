#pragma once
#include "types/PopUp.hpp"

namespace PopUp {
    extern PopUp::TfPopUpController* PopupController;

    void TfPopUpController_Create(TfPopUpController* Self);

    void TfPopUpController_Destroy(TfPopUpController* Self);

} // namespace PopUp
