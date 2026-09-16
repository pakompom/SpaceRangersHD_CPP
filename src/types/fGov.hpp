#pragma once
#include "runtime_support.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aRanger.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fPanelPlanet {
    struct TfPanelPlanet;

} // namespace fPanelPlanet

namespace fGov {
    struct TfGov;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfGov : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfGov, fPanelMain::TMessageLoopGIWithMainPanel, "TfGov", 316)
        void p_destroy() override;
        void InitializeLayout() override;
        // Reviewed compiler-layout difference: native reserves one extra, unreferenced
        // dword at EBP-$F4, before its managed-string temporaries, and emits an extra
        // push ECX in the prologue. Rebuilt temporaries from $F8 onward are four bytes
        // nearer EBP. Calls, branches, constants and field accesses agree throughout.
        // Native diagnostic name: TfGov.BeforeRun.
        void OnOpen() override;
        void OnClose() override;
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void RequestAnimationRestart();
        void PortraitAnimationComplete(GI_MessageLoop::TObjectGI* Sender);
        void UpdatePortraitAnimation(std::uint8_t Talking);
        void RememberChoiceScroll();
        void ClearDialogChoices();
        void AddChoice(pas::WideString Text, std::int32_t Value, GI_MessageLoop::TDialogChoiceEventGI Callback);
        static void ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void RestartTextPresentation(std::uint8_t RestartAnimation);
        void AdvanceTextPresentation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        static GI_MessageLoop::TObjectGI* CreateDialogObject(GI_Label::TLabelGI* LabelControl, EC_CacheFont::PFontObjectEC Item);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void AddMessageClicked(GI_MessageLoop::TObjectGI* Sender);
        void SelectMusic() override;
        void RefreshGovernmentDialog();
        // DL flag: true suppresses selecting/appending response text from the script-choice list; script execution and choice construction still run. Callers pass 0 or 1.
        void BuildGovernmentChoices(std::uint8_t SkipScriptResponseText);
        void AddBuiltinGovernmentChoices();
        void ContinueScriptDialog();
        void AddScriptTakeoffChoice(pas::WideString Caption);
        void AddScriptPlanetChoice(pas::WideString Caption);
        void AddScriptGoodsChoice(pas::WideString Caption);
        void AddScriptShopChoice(pas::WideString Caption);
        void AddScriptHangarChoice(pas::WideString Caption);
        void AddScriptNewsExitChoice(pas::WideString Caption);
        void BuildQuestOfferChoices();
        void StartScriptMessage(std::int32_t Action);
        void RunScriptAnswer(std::int32_t Answer);
        void RunScriptAnswerKeepingScroll(std::int32_t Answer);
        void RunScriptTakeoffAnswer(std::int32_t Answer);
        void RunScriptPlanetAnswer(std::int32_t Answer);
        void RunScriptGoodsAnswer(std::int32_t Answer);
        void RunScriptShopAnswer(std::int32_t Answer);
        void RunScriptHangarAnswer(std::int32_t Answer);
        void RunScriptNewsExitAnswer(std::int32_t Answer);
        void EnterPrison(std::int32_t Action);
        void ContinueAfterPrison(std::int32_t Action);
        void ShowBribeOffer(std::int32_t Action);
        void PayBribe(std::int32_t Action);
        void DeclineBribe(std::int32_t Action);
        void RequestQuest(std::int32_t Action);
        void MakeQuestEasier(std::int32_t Action);
        void MakeQuestHarder(std::int32_t Action);
        void AcceptQuest(std::int32_t Action);
        void RejectQuest(std::int32_t Action);
        void PermanentlyDeclineQuest(std::int32_t Action);
        void DeclinePlanetBattle(std::int32_t Action);
        void ConfirmDeclineAllPlanetBattles(std::int32_t Action);
        void DeclineAllPlanetBattles(std::int32_t Action);
        void CancelDeclineAllPlanetBattles(std::int32_t Action);
        void ShowPlanetBattleSupport(std::int32_t Action);
        void StartPlanetBattleWithoutSupport(std::int32_t Action);
        void StartPlanetBattleWithReinforcements(std::int32_t Action);
        void StartPlanetBattleWithBombardment(std::int32_t Action);
        void DeclineBattleAndLeave(std::int32_t Action);
        void ChoosePrisonInsteadOfBattle(std::int32_t Action);
        void ShowMapOffer(std::int32_t Action);
        void BuyMap(std::int32_t Action);
        void DeclineMapOffer(std::int32_t Action);
        void ShowPrisonBail(std::int32_t Action);
        void PayPrisonBail(std::int32_t Action);
        void CancelPrisonBail(std::int32_t Action);
        void ReturnToPlanet(std::int32_t Action);
        void ExitGovernment(std::int32_t Action);
        void RunInjectedAnswer(std::int32_t Answer);
        void RunInjectedAnswerKeepingScroll(std::int32_t Answer);
        void RunScriptRestartAnswer(std::int32_t Answer);
        void AddScriptRestartChoice(pas::WideString Caption);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        fPanelPlanet::TfPanelPlanet* PlanetPanel;
        fPanelLoad::TfPanelLoad* LoadPanel;
        pas::WideString DialogText;
        std::int32_t FormattedTextLength;
        GI_MessageLoop::PCallbackTimerGI DialogRefreshTimer;
        std::int32_t NextChoiceTop;
        std::uint8_t AnimationRestartRequested;
        std::uint8_t cpp_padding[3];
        aRanger::TQuest QuestOffer;
        // -1..1.
        std::int32_t QuestNegotiationLevel;
        std::int32_t QuestRewardStep;
        std::int32_t QuestDurationStep;
        // Script names with borrowed TScript data.
        EC_Str::TStringsEC* ScriptDialogNames;
        std::int32_t ScriptDialogCursor;
        std::int32_t PlanetBattleMapId;
        // 1=launch/exit to menu, 2=loss, 3=win, 4=cancel; launch is consumed on reopening.
        std::int32_t PendingTransition;
        std::uint8_t UseHdPortrait;
        std::uint8_t UseClassicPortrait;
        std::uint8_t cpp_padding_2[2];
        GI_MessageLoop::TObjectGI* PortraitPanel;
        std::int32_t SavedChoiceScroll;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fGov
