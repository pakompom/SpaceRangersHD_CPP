#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"

namespace fGameEnd {
    struct TfGameEnd;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfGameEnd : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfGameEnd, GI_MessageLoop::TMessageLoopGI, "TfGameEnd", 228)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void ScrollBackground(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ScrollEndingText(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ContinueClicked(GI_MessageLoop::TObjectGI* Sender);
        void LoadClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Show);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void SelectMusic() override;
        GI_MessageLoop::PCallbackTimerGI TextScrollTimer;
        std::int32_t TextPanelTop;
        std::int32_t TextPanelHeight;
        GI_MessageLoop::PCallbackTimerGI BackgroundTimer;
        std::int32_t BackgroundScrollOffset;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fGameEnd
