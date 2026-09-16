#pragma once
#include "runtime_support.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace aScript {
    struct TScript;

} // namespace aScript

namespace fTalk {
    struct TfTalkA;

    struct TfTalk;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfTalk : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfTalk, GI_MessageLoop::TMessageLoopGI, "TfTalk", 308)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RememberChoiceScroll();
        void EnableCloseButton();
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void ClearChoices(std::uint8_t AllowClose);
        // ExtraValue is stored in the choice object at $1C; its wider meaning remains unresolved.
        void AddChoice(pas::WideString Text, std::int32_t Value, GI_MessageLoop::TDialogChoiceEventGI Callback, std::int32_t ExtraValue);
        GI_MessageLoop::TObjectGI* CreateDialogObject(GI_Label::TLabelGI* LabelControl, EC_CacheFont::PFontObjectEC Item);
        void CenterEmbeddedObject(GI_MessageLoop::TObjectGI* Sender);
        void CenterShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainPanelMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainPanelMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        static void ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void RestartTextPresentation();
        void AdvanceTextPresentation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void AddDialogEffect(EC_Struct::TPointF Position, pas::WideString ImagePath, std::int32_t DelayMs);
        void ClearDialogEffects();
        static void DialogEffectComplete(GI_MessageLoop::TObjectGI* Sender);
        void ProcessCallbackTimers() override;
        void ApplyMapSelection(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void AddMessageClicked(GI_MessageLoop::TObjectGI* Sender);
        void MinimapScrolled();
        void FlushMinimapRefresh(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void AdvanceSlide(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void UpdateSlidePosition();
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void SelectMusic() override;
        void BuildStandardChoices(std::uint8_t KeepGreeting);
        void BuildBuiltinChoices();
        void AddScriptExitChoice(pas::WideString Caption);
        void CodeMsgOut(std::uint8_t KeepGreeting);
        void RunScriptAnswer(std::int32_t Answer);
        void RunScriptAnswerKeepingScroll(std::int32_t Answer);
        void FastExit(std::int32_t Answer);
        void RunScriptExitAnswer(std::int32_t Action);
        void ShowGreeting(std::int32_t Action);
        void AcceptScriptedConversation(std::int32_t Action);
        void ReturnToMap(std::int32_t Action);
        void ShowTrade(std::int32_t Action);
        void OpenTrade(std::int32_t Action);
        void CancelTrade(std::int32_t Action);
        void ShowMoneyDemand(std::int32_t Action);
        void DemandMoney(std::int32_t Action);
        void HalveMoneyDemand(std::int32_t Action);
        void DoubleMoneyDemand(std::int32_t Action);
        void DemandCargo(std::int32_t Action);
        void ShowTruceOffer(std::int32_t Action);
        void AcceptTruceOffer(std::int32_t Action);
        void HalveTruceOffer(std::int32_t Action);
        void DoubleTruceOffer(std::int32_t Action);
        void ShowAttackTargets(std::int32_t Action);
        void RequestAttackTarget(std::int32_t Action);
        void AcceptJointAttack(std::int32_t Action);
        void RequestProtection(std::int32_t Action);
        void RequestPreserveItems(std::int32_t Action);
        void ShowPartnerOffer(std::int32_t Action);
        void AcceptPartnerOffer(std::int32_t Action);
        void HalvePartnerOffer(std::int32_t Action);
        void DoublePartnerOffer(std::int32_t Action);
        void OrderPartnerFollow(std::int32_t Action);
        void OrderPartnerLand(std::int32_t Action);
        void OrderPartnerJump(std::int32_t Action);
        void OrderPartnerDropCargo(std::int32_t Action);
        void ShowPartnerFinances(std::int32_t Action);
        void ShowPartnerGift(std::int32_t Action);
        void GivePartnerGift(std::int32_t Action);
        void HalvePartnerGift(std::int32_t Action);
        void DoublePartnerGift(std::int32_t Action);
        void ApplyOrderToAllPartners(std::int32_t Action);
        void ExitPartnerConversation(std::int32_t Action);
        void OrderTranclucatorFollow(std::int32_t Action);
        void OrderTranclucatorReturn(std::int32_t Action);
        void OrderTranclucatorSeekItems(std::int32_t Action);
        void CancelTranclucatorSeekItems(std::int32_t Action);
        void ShowTranclucatorOptions(std::int32_t Action);
        void OrderTranclucatorDropCargo(std::int32_t Action);
        void OrderTranclucatorLand(std::int32_t Action);
        void OrderTranclucatorStoreCargo(std::int32_t Action);
        void AddTranclucatorGroupChoice();
        void ApplyOrderToAllTranclucators(std::int32_t Action);
        void ShowPiratePartnerOffer(std::int32_t Action);
        void AcceptPiratePartnerOffer(std::int32_t Action);
        void HalvePiratePartnerOffer(std::int32_t Action);
        void DoublePiratePartnerOffer(std::int32_t Action);
        void ShowPirateAttackTargets(std::int32_t Action);
        void OrderPiratePartnerAttack(std::int32_t Action);
        void OrderPiratePartnerFollow(std::int32_t Action);
        void OrderPiratePartnerLand(std::int32_t Action);
        void OrderPiratePartnerJump(std::int32_t Action);
        void ShowPiratePartnerFinances(std::int32_t Action);
        void ShowPiratePartnerGift(std::int32_t Action);
        void GivePiratePartnerGift(std::int32_t Action);
        void HalvePiratePartnerGift(std::int32_t Action);
        void DoublePiratePartnerGift(std::int32_t Action);
        void ShowPartnerDismissal(std::int32_t Action);
        void ShipDismissAct(std::int32_t Action);
        void RunDominatorProgram(std::int32_t Action);
        void ShowDominatorGreeting(std::int32_t Action);
        void ShowDominatorPeace(std::int32_t Action);
        void ShowDominatorGoods(std::int32_t Action);
        void ShowDominatorCommand(std::int32_t Action);
        std::uint8_t AddImmediateAttackChoices();
        static pas::WideString GetShipGreeting();
        void RunInjectedAnswer(std::int32_t Action);
        void RunInjectedAnswerKeepingScroll(std::int32_t Action);
        void StartScriptMessage(aScript::TScript* Script);
        void BuildMilitarySupportChoices();
        void ShowMilitarySupport(std::int32_t Action);
        void CancelMilitarySupport(std::int32_t Action);
        void DeclineMilitarySupport(std::int32_t Action);
        void ShowMilitaryHullRepair(std::int32_t Action);
        void AcceptMilitaryHullRepair(std::int32_t Action);
        void ShowMilitaryEquipmentRepair(std::int32_t Action);
        void AcceptMilitaryEquipmentRepair(std::int32_t Action);
        void ShowMilitaryRemains(std::int32_t Action);
        void SellAllMilitaryRemains(std::int32_t Action);
        void SellIndividualMilitaryRemains(std::int32_t Action);
        void ShowMilitaryBuff(std::int32_t Action);
        void AcceptMilitaryBuff(std::int32_t Action);
        void DiscussOldHull(std::int32_t Action);
        void RunScriptRestartAnswer(std::int32_t Action);
        void AddScriptRestartChoice(pas::WideString Caption);
        GI_Panel::TPanelGI* MainPanel;
        GI_Panel::TPanelGI* DialogPanel;
        pas::WideString DialogText;
        std::int32_t PresentedTextLength;
        GI_MessageLoop::PCallbackTimerGI TextPresentationTimer;
        std::int32_t ChoiceHeight;
        std::uint8_t cpp_padding[8];
        std::uint8_t SkipShipScriptAdvance;
        std::uint8_t ChoiceMousePressed;
        std::uint8_t cpp_padding_2[2];
        pas::Object* RequestedMapCenter;
        pas::Object* CurrentMapCenter;
        GI_MessageLoop::PCallbackTimerGI MapSelectionTimer;
        pas::Object* RequestedMapHover;
        std::uint32_t CurrentFilmObjectId;
        pas::Object* CurrentMapHover;
        GI_MessageLoop::PCallbackTimerGI MinimapRefreshTimer;
        std::uint8_t MinimapEnabled;
        std::uint8_t MapDragging;
        WindowsSdk::TPoint MapDragPoint;
        std::uint8_t cpp_padding_3[2];
        GI_MessageLoop::PCallbackTimerGI SlideTimer;
        float SlideProgress;
        std::int32_t DialogPanelLeft;
        // Nonzero suppresses parent star-map presentation during modal transitions; other uses unresolved.
        std::int32_t Flag128;
        // Set after the star-map goods-trading modal returns; remaining readers need recovery.
        std::uint8_t Flag12C;
        std::uint8_t cpp_padding_4[3];
        std::int32_t SavedChoiceScroll;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TDialogTextChoiceEvent = pas::Method<void(pas::WideString)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfTalkA : EC_Struct::TObjectEx {
        PAS_CLASS_META(TfTalkA, EC_Struct::TObjectEx, "TfTalkA", 36)
        void p_destroy() override;
        // No managed cleanup at this offset.
        std::int32_t Reserved04;
        GI_MessageLoop::TDialogChoiceEventGI Callback;
        TDialogTextChoiceEvent FallbackCallback;
        std::int32_t Value;
        std::int32_t ExtraValue;
        // Native cleanup table owns this string.
        pas::WideString FallbackText;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fTalk
