#pragma once
#include "runtime_support.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace aGalaxy {
    struct TStar;

} // namespace aGalaxy

namespace aItem {
    struct TEquipment;

    struct TItem;

} // namespace aItem

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fPanelRuins {
    struct TfPanelRuins;

} // namespace fPanelRuins

namespace fRuinsTalk {
    struct TDominionTravelQuote;

    struct TConstructionEquipment;

    struct TResearchItemSortKey;

    struct TfRuinsTalk;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfRuinsTalk : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfRuinsTalk, fPanelMain::TMessageLoopGIWithMainPanel, "TfRuinsTalk", 268)
        void p_destroy() override;
        void InitializeLayout() override;
        void LayoutStationPortrait(GI_MessageLoop::TObjectGI* Panel);
        void HideStationTransientControl();
        // Native diagnostic name: TfRuinsTalk.BeforeRun.
        void OnOpen() override;
        void OnClose() override;
        // Returns inventory count; unused sorted slots are -1.
        std::int32_t SortResearchItems(std::uint8_t Series);
        static std::uint8_t IsResearchItemQuestLetter(aItem::TItem* Item);
        std::int32_t CountResearchRemains(std::uint8_t Series, std::int32_t Count);
        std::int32_t CountResearchEquipment(std::int32_t Count);
        void BuildResearchItemChoices(std::uint8_t Series, pas::WideString& Text);
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void RememberChoiceScroll();
        // Native diagnostic name: TfRuinsTalk.A_Start.
        void ClearChoices();
        // Text filters may disable or suppress the choice. Invokes the method callback with Value in EDX.
        void AddChoice(pas::WideString Text, std::int32_t Value, GI_MessageLoop::TDialogChoiceEventGI Callback);
        static void ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        // Clears the dialogue panel state and restarts its ten-millisecond presentation timer.
        void RestartTextPresentation();
        void AdvanceTextPresentation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ResetPortraitCycle();
        void PortraitCycleComplete(GI_MessageLoop::TObjectGI* Sender);
        void SelectPortraitAnimation(std::uint8_t Alternate);
        static GI_MessageLoop::TObjectGI* CreateChoiceBullet(GI_Label::TLabelGI* LabelControl, EC_CacheFont::PFontObjectEC Item);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void AddMessageClicked(GI_MessageLoop::TObjectGI* Sender);
        // Advances Film over the native 138-second interval.
        void AdvanceScriptVideo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        std::uint8_t StopScriptVideo(std::uint8_t Unused);
        void ProcessWindowMessage(std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) override;
        pas::Method<void(std::uint32_t, std::uint32_t, std::int32_t)> bind_ProcessWindowMessage() override;
        void SelectMusic() override;
        // Rebuilds the station dialogue root; receiver-only entry verified at native prologue and CheatNextRank.
        void I_Start();
        std::uint8_t ShowDominatorVictoryDialog();
        void ContinueDominatorVictoryDialog(std::int32_t Action);
        // Rebuilds choices; KeepText preserves the current greeting and skips rank rewards.
        void M_Main(std::uint8_t KeepText);
        static void CloseHullMode(std::int32_t Action);
        void ToggleImpulseShields(std::int32_t Action);
        void ShowBridgeBlackHoleDialog(std::int32_t Action);
        void SelectBridgeBlackHoleDestination(std::int32_t Action);
        void ShowInterceptorDialog(std::int32_t Action);
        void ShowActiveInterceptors(std::int32_t Action);
        void RecallInterceptorsFromTarget(std::int32_t Action);
        void RecallAllInterceptors(std::int32_t Action);
        void ShowInterceptorPassDialog(std::int32_t Action);
        void IncreaseInterceptorPassCount(std::int32_t Action);
        void DecreaseInterceptorPassCount(std::int32_t Action);
        void ShowInterceptorTargetDialog(std::int32_t Action);
        void SelectInterceptorTarget(std::int32_t Action);
        void ClearInterceptorTarget(std::int32_t Action);
        void ShowInterceptorStrategyDialog(std::int32_t Action);
        void SelectInterceptorStrategy(std::int32_t Action);
        void ShowBridgeHelp(std::int32_t Action);
        void ShowBridgeHelpAnswer(std::int32_t Action);
        void BuildBuiltinServiceOptions();
        void ContinueScriptDialog();
        void AddScriptTakeoffChoice(pas::WideString Caption);
        void AddScriptNewsExitChoice(pas::WideString Caption);
        void AddScriptHangarChoice(pas::WideString Caption);
        void AddScriptGoodsChoice(pas::WideString Caption);
        void AddScriptGameEndChoice(pas::WideString Caption);
        // Choice callback value carries a TScript pointer.
        void SelectScriptDialog(std::int32_t ScriptValue);
        void RunScriptAnswer(std::int32_t Answer);
        void RunScriptAnswerKeepingScroll(std::int32_t Answer);
        void RunScriptTakeoff(std::int32_t Answer);
        void RunScriptNewsExit(std::int32_t Answer);
        void RunScriptHangar(std::int32_t Answer);
        void OpenHangar(std::int32_t Action);
        void RunScriptGoods(std::int32_t Answer);
        void RunScriptGameEnd(std::int32_t Answer);
        void ReturnToMain(std::int32_t Action);
        // Zero requests a quote; a nonzero quote is charged and enables sponsorship. Sponsored stations open the equipment-refit screen.
        void OpenStationModernization(std::int32_t QuotedCost);
        void DeclineStationModernization(std::int32_t Action);
        // Deposits every carried node stack.
        void DepositNodesAtRangerCenter(std::int32_t Action);
        void ShowRangerCenterTakeNodeDialog(std::int32_t Action);
        void BuyRangerCenterMicroModule(std::int32_t Action);
        void DeclineRangerCenterNodeDeposit(std::int32_t Action);
        void ShowRangerCenterGiveNodeDialog(std::int32_t Action);
        void ExchangeMicroModuleForNodes(std::int32_t Action);
        void DeclineRangerCenterNodeReward(std::int32_t Action);
        void ShowRangerCenterNodeInfo(std::int32_t Action);
        void ShowRangerCenterNodeInfoContinuation(std::int32_t Action);
        void ShowRangerCenterRatingAnswer(std::int32_t Action);
        void ShowRangerCenterPirateClanAnswer(std::int32_t Action);
        void ShowRangerCenterBestRangerAnswer(std::int32_t Action);
        void ShowPirateBaseNationalityDialog(std::int32_t Action);
        void AcceptPirateBaseNationality(std::int32_t Action);
        void DeclinePirateBaseNationality(std::int32_t Action);
        void ShowPirateBaseSideChangeDialog(std::int32_t Action);
        // Recalculates the fee at acceptance time.
        void AcceptPirateBaseSideChange(std::int32_t Action);
        void DeclinePirateBaseSideChange(std::int32_t Action);
        void ShowPirateBaseNodeDialog(std::int32_t Action);
        void BuyPirateBaseNodes(std::int32_t Action);
        void DeclinePirateBaseNodes(std::int32_t Action);
        void ShowPirateBaseProgramDialog(std::int32_t Action);
        void BuyPirateBaseProgram(std::int32_t Action);
        void DeclinePirateBaseProgram(std::int32_t Action);
        void ShowPirateBaseRepairDialog(std::int32_t Action);
        void AcceptPirateBaseRepair(std::int32_t Action);
        void DeclinePirateBaseRepair(std::int32_t Action);
        void ShowPirateBaseSubCrackDialog(std::int32_t Action);
        void ConfirmPirateBaseSubCrack(std::int32_t Action);
        void BuyPirateBaseSubCrack(std::int32_t Action);
        void BuyPirateBaseSubCrackHalfPrice(std::int32_t Action);
        void DeclinePirateBaseSubCrack(std::int32_t Action);
        void ShowPirateBaseChameleonDialog(std::int32_t Action);
        void BuyPirateBaseChameleon(std::int32_t Action);
        void DeclinePirateBaseChameleon(std::int32_t Action);
        void I_WarWithKlingAndPirates(std::int32_t Action);
        // Does not promote the player.
        void ShowMilitaryBaseNextRankDialog(std::int32_t Action);
        void ShowMilitaryBaseRepairDialog(std::int32_t Action);
        void ShowMilitaryBaseRepairQuote(std::int32_t Action);
        void AcceptMilitaryBaseRepair(std::int32_t Action);
        void DeclineMilitaryBaseRepair(std::int32_t Action);
        void ShowMilitaryBaseProgramsDialog(std::int32_t Action);
        void AcceptMilitaryBasePrograms(std::int32_t Action);
        void ShowMilitaryBaseWarOperationDialog(std::int32_t Action);
        void AcceptMilitaryBaseWarOperation(std::int32_t Action);
        void DeclineMilitaryBaseWarOperation(std::int32_t Action);
        void ShowMilitaryBaseTravelDialog(std::int32_t Action);
        void ConfirmMilitaryBaseTravel(std::int32_t Action);
        void DepartWithStation(std::int32_t Action);
        void ShowMilitaryBaseArrivalDialog(std::int32_t Action);
        void DeclineMilitaryBaseTravel(std::int32_t Action);
        void ShowMilitaryBaseArrivalInfo(std::int32_t Action);
        void ShowMilitaryBaseArrivalQuestions(std::int32_t Action);
        void ShowScienceBaseImprovementDialog(std::int32_t Action);
        void ShowScienceBaseImprovementItems(std::int32_t Action);
        void DeclineScienceBaseImprovement(std::int32_t Action);
        void ShowScienceBaseImprovementQuote(std::int32_t Action);
        void SelectScienceBaseImprovementKind(std::int32_t Action);
        void AcceptScienceBaseImprovement(std::int32_t Action);
        void DeclineScienceBaseRepeatImprovement(std::int32_t Action);
        void ShowScienceBaseRepairDialog(std::int32_t Action);
        void ShowScienceBaseRepairQuote(std::int32_t Action);
        void AcceptScienceBaseRepair(std::int32_t Action);
        void DeclineScienceBaseRepair(std::int32_t Action);
        void ShowScienceBaseSatelliteOfferDialog(std::int32_t Refresh);
        void ShowSatelliteInstructions(std::int32_t Action);
        // Transfers the existing SatelliteOffer into inventory.
        void BuyScienceBaseSatellite(std::int32_t Action);
        void DeclineScienceBaseSatellite(std::int32_t Action);
        void ShowScienceBaseResearchDialog(std::int32_t Action);
        void SelectScienceBaseResearchSection(std::int32_t Action);
        void SellResearchRemains(std::int32_t Action);
        void SellResearchEquipment(std::int32_t Action);
        void SellResearchItem(std::int32_t Action);
        void DeclineScienceBaseResearch(std::int32_t Action);
        void BuyScienceBaseResearchProgram(std::int32_t Action);
        void AcceptScienceBaseResearchProgram(std::int32_t Action);
        void DeclineScienceBaseResearchProgram(std::int32_t Action);
        void ShowScienceBaseHistoryDialog(std::int32_t Action);
        void ShowBusinessCenterDebtDialog(std::int32_t Action);
        // Quotes 1/2/3 use the large/medium/small principal, with 20/15/10 percent interest included in DebtAmount.
        void AcceptBusinessCenterDebtQuote(std::int32_t Quote);
        void DeclineBusinessCenterDebtDialog(std::int32_t Action);
        // Requires the menu's prior affordability check.
        void RepayBusinessCenterDebt(std::int32_t Action);
        void ShowBusinessCenterDepositDialog(std::int32_t Action);
        // Quotes 1/2/3 select large/medium/small amounts; a new deposit resets accrued days.
        void AcceptBusinessCenterDepositQuote(std::int32_t Quote);
        void DeclineBusinessCenterDepositDialog(std::int32_t Action);
        void WithdrawBusinessCenterDeposit(std::int32_t Action);
        void ShowBusinessCenterMedicalPolicyDialog(std::int32_t Refresh);
        // Policy duration is 1825 ticks.
        void BuyBusinessCenterMedicalPolicy(std::int32_t Action);
        void ShowBusinessCenterPolicyDetails(std::int32_t Action);
        void DeclineBusinessCenterPolicy(std::int32_t Action);
        void ShowBusinessCenterInvestmentDialog(std::int32_t Action);
        void AcceptBusinessCenterInvestment(std::int32_t Action);
        void DeclineBusinessCenterInvestment(std::int32_t Action);
        void ShowBusinessCenterTradeDialog(std::int32_t Action);
        void BuyBusinessCenterTradeAdvice(std::int32_t Action);
        void DeclineBusinessCenterTradeAdvice(std::int32_t Action);
        void ShowMedicalCenterIllnessTreatmentDialog(std::int32_t Refresh);
        // Treats disease indexes 1..12; valid insurance halves the fee outside pirate-owned systems.
        void TreatSelectedDiseaseAtMedicalCenter(std::int32_t DiseaseIndex);
        // Trusts QuotedCost from the menu.
        void TreatAllDiseasesAtMedicalCenter(std::int32_t QuotedCost);
        void DeclineMedicalCenterTreatment(std::int32_t Action);
        void LeaveMedicalCenterTreatment(std::int32_t Action);
        void ShowMedicalCenterStimulantDialog(std::int32_t Action);
        // Stimulants use effect indices 13..24; valid insurance halves the fee outside pirate-owned systems.
        void BuySelectedStimulantAtMedicalCenter(std::int32_t StimulantIndex);
        void DeclineMedicalCenterStimulants(std::int32_t Action);
        void ShowStationSpecialShipDialog(std::int32_t Action);
        void DeclineStationSpecialShip(std::int32_t Action);
        void BuyStationSpecialShip(std::int32_t Action);
        void RunInjectedDialog(std::int32_t Action);
        void RunInjectedDialogKeepingScroll(std::int32_t Action);
        void RunScriptRestart(std::int32_t Answer);
        void AddScriptRestartChoice(pas::WideString Caption);
        static void CloseRuinsMode(GI_MessageLoop::TObjectGI* Sender);
        std::int32_t BuildConstructionItemChoices(aConst::TItemType Kind);
        void ShowDominionShipConstructionDialog(std::int32_t Action);
        void ConfirmDominionConstructionLimit(std::int32_t Action);
        void DeclineDominionShipConstruction(std::int32_t Action);
        void SelectConstructionHeldItem(std::int32_t Action);
        void SelectConstructionStoredItem(std::int32_t Action);
        void SelectConstructionShopItem(std::int32_t Action);
        void SkipConstructionItem(std::int32_t Action);
        void AppendConstructionItemList();
        void ContinueDominionConstruction(pas::WideString PreviousItem);
        void PickConstructionWeapon(std::int32_t Action);
        void PickConstructionRadar(std::int32_t Action);
        void PickConstructionScanner(std::int32_t Action);
        void PickConstructionRepairRobot(std::int32_t Action);
        void PickConstructionDefGenerator(std::int32_t Action);
        void CompleteDominionConstruction(std::int32_t Action);
        void ShowDominionImprovementDialog(std::int32_t Action);
        void ShowDominionImprovementItems(std::int32_t Action);
        void DeclineDominionImprovement(std::int32_t Action);
        void ShowDominionImprovementQuote(std::int32_t Action);
        void AcceptDominionImprovement(std::int32_t Action);
        void DeclineDominionRepeatImprovement(std::int32_t Action);
        void ShowDominionPirateLicenseDialog(std::int32_t Action);
        void BuyDominionPirateLicense(std::int32_t Action);
        void DeclineDominionPirateLicense(std::int32_t Action);
        std::uint8_t CheckDominionServiceStanding(std::uint8_t RequiredRank, pas::WideString Prefix, float CreditCost);
        static void SpendDominionServiceCredit(float CreditCost);
        std::uint8_t CheckDominionAvailable();
        void ShowDominionTravelDialog(std::int32_t Action);
        void ConfirmDominionTravel(std::int32_t Action);
        void DeclineDominionTravel(std::int32_t Action);
        void AcceptDominionTravel(std::int32_t Action);
        void DeclineDominionTravelConfirmation(std::int32_t Action);
        void ShowDominionCancelTravelDialog(std::int32_t Action);
        void CancelDominionTravel(std::int32_t Action);
        void DeclineDominionCancelTravel(std::int32_t Action);
        void ShowDominionRelocationDialog(std::int32_t Action);
        void AcceptDominionRelocation(std::int32_t Action);
        void DeclineDominionRelocation(std::int32_t Action);
        void BuildDominionWarOptions();
        void I_CBWarWithKlingAndCoalition(std::int32_t Action);
        void ShowDominionCaptureAnswer(std::int32_t Action);
        void ShowDominionRanksAnswer(std::int32_t Action);
        void ShowDominionWarOperationDialog(std::int32_t Action);
        void AcceptDominionWarOperation(std::int32_t Action);
        void DeclineDominionWarOperation(std::int32_t Action);
        void ShowDominionAmbushDialog(std::int32_t Action);
        void AcceptDominionAmbush(std::int32_t Action);
        void DeclineDominionAmbush(std::int32_t Action);
        void ShowDominionAssaultDialog(std::int32_t Action);
        void AcceptDominionAssault(std::int32_t Action);
        void DeclineDominionAssault(std::int32_t Action);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        // Owned.
        fPanelRuins::TfPanelRuins* StationPanel;
        // Owned.
        fPanelLoad::TfPanelLoad* LoadPanel;
        pas::WideString DialogText;
        std::int32_t PresentedTextLength;
        GI_MessageLoop::PCallbackTimerGI TextPresentationTimer;
        // Accumulated dialogue-choice row height.
        std::int32_t ChoiceHeight;
        std::uint8_t NextPortraitCycleAlternate;
        // Set after docked hyperspace travel; OnOpen tests this together with SkipVideo.
        std::uint8_t ShowArrivalVideo;
        // Copied from the docked ship on entry.
        aGalaxyStruct::TOwnerId StationOwner;
        // Copied from the docked ship on entry.
        std::uint8_t StationType;
        // timeGetTime timestamp used by the queued-video callback.
        std::uint32_t ScriptVideoStartedAt;
        GI_MessageLoop::PCallbackTimerGI ScriptVideoTimer;
        pas::DynArray<std::uint8_t> ResearchItemVisited;
        // Negative entries terminate the sorted sale list.
        pas::DynArray<std::int32_t> ResearchItemIndexes;
        // Viewport is at least 1280x960; permits HD portraits or the table layout.
        std::uint8_t LargePortraitLayout;
        // Large layout with UseTablesForGov; shows the table and standard portrait animations.
        std::uint8_t PortraitTableVisible;
        std::uint8_t cpp_padding[2];
        GI_MessageLoop::TObjectGI* StationTransientControl;
        std::int32_t SavedChoiceScroll;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Four seeded draws are merged by destination.
    #pragma pack(push, 1)
    struct TDominionTravelQuote {
        aGalaxy::TStar* Star;
        std::int32_t DrawCount;
        std::int32_t Cost;
    };
    #pragma pack(pop)

    // Borrowed item selection and source: 0=hold, 1=storage, 2=shop.
    #pragma pack(push, 1)
    struct TConstructionEquipment {
        aItem::TEquipment* Item;
        std::uint8_t Source;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    // Native sort helpers.
    #pragma pack(push, 1)
    struct TResearchItemSortKey {
        std::uint8_t Priority;
        std::uint8_t cpp_padding[3];
        std::int32_t Cost;
        std::int32_t Weight;
    };
    #pragma pack(pop)

} // namespace fRuinsTalk
