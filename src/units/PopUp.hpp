#pragma once
#include "types/PopUp.hpp"

namespace PopUp {
    // Created.
    extern PopUp::TfPopUpController* PopupController;

    void TfPopUpController_Create(TfPopUpController* Self);

    // Frees queued cells without finalizing their strings; does not call inherited Destroy.
    void TfPopUpController_Destroy(TfPopUpController* Self);

} // namespace PopUp
