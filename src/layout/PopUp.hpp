#pragma once
#include "types/PopUp.hpp"

namespace PopUp {
    static_assert(sizeof(void*) != 4 || sizeof(PopUp::TfPopUpController) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(PopUp::TfPopUpController, LastTick) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(PopUp::TfPopUpController, PauseRemaining) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(PopUp::TfPopUpController, MovingUp) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(PopUp::TfPopUpController, MotionRemainder) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(PopUp::TfPopUpController, QueueLock) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(PopUp::TfPopUpController, TextQueue) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(PopUp::TfPopUpController, ImageQueue) == 312);

} // namespace PopUp
