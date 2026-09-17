#pragma once
#include "types/PopUp.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace PopUp {
    // Created.
    extern PopUp::TfPopUpController* PopupController;

    void TfPopUpController_Create(TfPopUpController* Self);

    // Frees queued cells without finalizing their strings; does not call inherited Destroy.
    void TfPopUpController_Destroy(TfPopUpController* Self);

    GI_MessageLoop::TObjectGI* TfPopUpController_CreatePopup(TfPopUpController* Self, pas::WideString Text, pas::WideString ImagePath);

    // Drains queued notifications, advances their vertical animation, and retires off-screen controls.
    void TfPopUpController_AdvancePopups(TfPopUpController* Self, std::uint32_t Tick);

} // namespace PopUp
