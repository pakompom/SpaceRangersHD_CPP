#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace fAbout {
    struct TfAbout;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfAbout : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfAbout, GI_MessageLoop::TMessageLoopGI, "TfAbout", 228)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void ClearCredits();
        void AddCreditLine(pas::WideString Text, std::uint8_t Red, std::uint8_t Green, std::uint8_t Blue);
        void AddCreditSeparator();
        void AddCreditSpacing(std::int32_t Height);
        void ScrollCredits(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void CloseMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, Types::TPoint Point);
        void CloseKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void SelectMusic() override;
        GI_MessageLoop::PCallbackTimerGI ScrollTimer;
        GI_Panel::TPanelGI* ViewportPanel;
        GI_Panel::TPanelGI* CreditsPanel;
        std::int32_t CreditsHeight;
        std::uint8_t FirstMusicSelection;
        std::uint8_t ReturnToScores;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fAbout
