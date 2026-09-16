#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"

namespace fIntroduction {
    struct TfIntroduction;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfIntroduction : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfIntroduction, GI_MessageLoop::TMessageLoopGI, "TfIntroduction", 240)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void UpdateGenerationProgress(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void BlinkContinueButton(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ScrollIntroductionText(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ScrollBackground(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ContinueMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void ContinueClicked(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void SelectMusic() override;
        GI_MessageLoop::PCallbackTimerGI GenerationProgressTimer;
        GI_MessageLoop::PCallbackTimerGI TextScrollTimer;
        GI_MessageLoop::PCallbackTimerGI BackgroundTimer;
        std::int32_t BackgroundScrollOffset;
        std::int32_t TextPanelTop;
        std::int32_t TextPanelHeight;
        float ProgressPulsePhase;
        GI_MessageLoop::PCallbackTimerGI ContinueBlinkTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fIntroduction
