#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fJump {
    struct TfJump;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfJump : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfJump, GI_MessageLoop::TMessageLoopGI, "TfJump", 236)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void AdvanceTravel(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void AdvanceLoading(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void AdvanceMovie(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        std::uint8_t StopMovie();
        void SelectMusic() override;
        GI_MessageLoop::PCallbackTimerGI TransitionTimer;
        std::uint8_t LoadingStarted;
        std::uint8_t NoPendingLoads;
        std::uint8_t cpp_padding[2];
        float Progress;
        fPanelLoad::TfPanelLoad* LoadPanel;
        std::uint32_t MovieStartTick;
        GI_MessageLoop::PCallbackTimerGI MovieTimer;
        std::uint8_t RestoreOrdersOnArrival;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fJump
