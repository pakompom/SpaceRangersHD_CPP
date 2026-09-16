#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"

namespace PopUp {
    struct TfPopUpController;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfPopUpController : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TfPopUpController, GI_MessageLoop::TObjectGI, "TfPopUpController", 316)
        void p_destroy() override;
        GI_MessageLoop::TObjectGI* CreatePopup(pas::WideString Text, pas::WideString ImagePath);
        void AdvancePopups(std::uint32_t Tick);
        void QueueNotification(pas::WideString Text, pas::WideString ImagePath);
        std::uint32_t LastTick;
        std::uint32_t PauseRemaining;
        std::uint8_t MovingUp;
        std::uint8_t cpp_padding[3];
        float MotionRemainder;
        pas::CriticalSection* QueueLock;
        pas::List* TextQueue;
        pas::List* ImageQueue;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace PopUp
