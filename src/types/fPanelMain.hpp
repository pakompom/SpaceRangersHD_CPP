#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_GraphButton {
    struct TGraphButtonGI;

} // namespace GI_GraphButton

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace fPanelMain {
    struct TfPanelMain;

    struct TMessageLoopGIWithMainPanel;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMessageLoopGIWithMainPanel : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TMessageLoopGIWithMainPanel, GI_MessageLoop::TMessageLoopGI, "TMessageLoopGIWithMainPanel", 212)
        void p_destroy() override;
        TfPanelMain* MainPanel;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfPanelMain : EC_Struct::TObjectEx {
        PAS_CLASS_META(TfPanelMain, EC_Struct::TObjectEx, "TfPanelMain", 136)
        void p_destroy() override;
        void InitializeLayout(GI_MessageLoop::TMessageLoopGI* Screen);
        void OnOpen();
        void OnClose();
        void Show();
        void Hide();
        void RefreshMoneyAndCargo();
        void RefreshDate();
        void SetDateRange(std::int32_t FirstTurn, std::int32_t LastTurn);
        void StartDateAnimation(std::int32_t IntervalMs);
        void AdvanceDateAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void QuestClicked(GI_MessageLoop::TObjectGI* Sender);
        void GalaxyClicked(GI_MessageLoop::TObjectGI* Sender);
        void JournalClicked(GI_MessageLoop::TObjectGI* Sender);
        void MenuClicked(GI_MessageLoop::TObjectGI* Sender);
        void TryAutoTurnSave();
        void QuickSave();
        void QuickLoad(std::int32_t SlotIndex);
        void RefreshStatusTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void PulseUnreadMessages(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void RefreshEndTurnButton();
        void DisableNavigationButtons();
        void EnableNavigationButtons();
        void RebuildMessageButtons(std::uint8_t SkipLock);
        void ClearMessageButtons();
        std::uint8_t RemoveDismissibleMessages(pas::WideString Key);
        void MessageMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void MessageMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void DeleteMessage(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point, std::uint8_t SkipLock);
        void MessageRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void AdvanceMessageDeletion(GI_MessageLoop::TObjectGI* Sender);
        void FinishMessageDeletion(GI_MessageLoop::TObjectGI* Sender);
        void SlideMessagesIn();
        void SlideMessagesOut();
        void AdvanceMessageSlide(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void MessageClicked(GI_MessageLoop::TObjectGI* Sender);
        static void PlayUnreadMessageSounds();
        void FlashMoneyWarning();
        void AdvanceMoneyWarning(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void FlashCargoWarning();
        void AdvanceCargoWarning(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible);
        void ShowHelpText(pas::WideString Text, std::uint8_t Visible);
        void ProcessKeyDown(std::uint32_t Key);
        static void PostMouseMove();
        GI_MessageLoop::TMessageLoopGI* Screen;
        GI_MessageLoop::PCallbackTimerGI StatusTimer;
        GI_MessageLoop::PCallbackTimerGI MessagePulseTimer;
        GI_MessageLoop::PCallbackTimerGI MessageSlideTimer;
        GI_Label::TLabelGI* HelpLabel;
        std::uint32_t DisplayedShipId;
        std::uint32_t DisplayedPlanetId;
        GI_Panel::TPanelGI* MessagePanel;
        GI_Image::TImageGI* BackgroundImage;
        GI_GraphButton::TGraphButtonGI* ShipButton;
        GI_GraphButton::TGraphButtonGI* GalaxyButton;
        GI_GraphButton::TGraphButtonGI* QuestButton;
        GI_GraphButton::TGraphButtonGI* EndTurnButton;
        GI_GraphButton::TGraphButtonGI* MenuButton;
        GI_Label::TLabelGI* DateLabel;
        GI_Image::TImageGI* DateNextImage;
        GI_Label::TLabelGI* NextDateLabel;
        std::uint8_t NavigationLocked;
        std::uint8_t cpp_padding[3];
        std::int32_t MessagePulseStep;
        std::int32_t MessageSlideDirection;
        std::int32_t MessagePanelRestTop;
        std::int32_t DisplayedTurn;
        float DateSlideProgress;
        std::int32_t TargetTurn;
        GI_MessageLoop::PCallbackTimerGI DateTimer;
        std::int32_t DateTimerIntervalMs;
        pas::List* AuxiliaryItems;
        std::uint8_t MoneyWarningActive;
        std::uint8_t cpp_padding_2[3];
        std::int32_t MoneyWarningTicks;
        GI_MessageLoop::PCallbackTimerGI MoneyWarningTimer;
        std::uint8_t CargoWarningActive;
        std::uint8_t cpp_padding_3[3];
        std::int32_t CargoWarningTicks;
        GI_MessageLoop::PCallbackTimerGI CargoWarningTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fPanelMain
