#include "layout/aScript.hpp"
#include "types/BreakMessageGIException.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_XviD.hpp"
#include "types/GR_Music.hpp"
#include "types/ThreadCalc.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/aWarrior.hpp"
#include "types/ab_MainForm.hpp"
#include "types/fHangar.hpp"
#include "types/fPanelMain.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fShip2.hpp"
#include "types/fStarMap.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBuf.hpp"
#include "units/EC_Ether.hpp"
#include "units/EC_Expression.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/Robot.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aScriptFun.hpp"
#include "units/aShip.hpp"

namespace aScript {
    aScript::TScript* CurrentScript{};

    // Owns PScriptABRequest records.
    pas::List* QueuedArcadeBattles{};

    // Owns PScriptPBRequest records.
    pas::List* QueuedPlanetaryBattles{};

    // // Owns PQueuedTextQuest records.
    pas::List* QueuedTextQuests{};

    // Owns PScriptVDRequest records.
    pas::List* QueuedVideos{};

    aScript::TScript* ResumingScript = nullptr;

    aScript::TScript* StagedArcadeShipScript = nullptr;

    // Direct stores in TryShowQueuedArcadeBattle establish ownership.
    std::uint8_t ScriptArcadeReturnScreenId = 0;

    std::uint8_t ScriptTakeoffRequested = false;

    // Owns ships waiting for an arcade request.
    aMyFunction::TObjectList* StagedArcadeShips{};

    std::uint8_t ScriptEndTurnRequested = false;

    EC_Expression::TVarArrayEC* ScriptFunctionScope{};

    EC_Expression::TCodeProcessEC* ScriptProcess{};

    // Record entries are freed explicitly before clearing.
    aMyFunction::TObjectList* ScriptDialogOverrides{};

    aScript::TLibraryCache* ScriptLibraryCache = nullptr;

    // Named artifact action code via TArtefact.GetActionCode.
    aScript::TScriptCache* ArtefactScriptCache = nullptr;

    // Item-type artifact action code; item types 8 and 9 use the named artifact cache instead.
    aScript::TScriptCache* ArtefactKindScriptCache = nullptr;

    aScript::TScriptCache* UselessItemScriptCache = nullptr;

    aScript::TScriptCache* CustomShipInfoScriptCache = nullptr;

    aScript::TScriptGICache* GameplayUiScriptCache = nullptr;

    aScript::TScriptState* CurrentScriptState = nullptr;

    aMyFunction::TObjectList* ScriptDialogInjections{};

    aMyFunction::TObjectList* ScriptDialogBlocks{};

    aScript::TScriptThread* ScriptRequestThread{};

    void LogScriptCallHistory() {
        std::int32_t I{};
        std::int32_t Position{};
        EC_Expression::TVarEC* Cell{};
        std::int32_t Count = EC_Expression::ScriptCallTraceCount;
        Position = (EC_Expression::ScriptCallTracePosition - 1 + 20) % 20;
        Cell = EC_Expression::ScriptCallTrace[Position];
        while (Cell != nullptr && Cell->RealVType() == EC_Expression::vkFunction) {
            --Count;
            if (Count <= 0) {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Non-function error at beginning or after return from user function ", Cell->Name})));
                return;
            }
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in user function ", Cell->Name})));
            Position = (Position - 1 + 20) % 20;
            Cell = EC_Expression::ScriptCallTrace[Position];
        }
        GR_Main::AppendLogLineThreadSafe("function call history:"_a);
        Position = (EC_Expression::ScriptCallTracePosition - EC_Expression::ScriptCallTraceCount + 20) % 20;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(I); ) {
            if (EC_Expression::ScriptCallTrace[Position] != nullptr) {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(EC_Expression::ScriptCallTrace[Position]->Name));
            }
            Position = (Position + 1) % 20;
        }
        if (aGalaxy::Galaxy != nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"current turn is ", EC_Str::IntToWideString(aGalaxy::Galaxy->CurrentTurn)})));
        }
    }

    void ClearPendingScriptRequests() {
        ResumingScript = nullptr;
        while (pas::list_count(QueuedTextQuests) > 0) {
            pas::dispose(pas::list_at<TScriptTQRequest>(QueuedTextQuests, 0));
            pas::list_delete(QueuedTextQuests, 0);
        }
        while (pas::list_count(QueuedArcadeBattles) > 0) {
            pas::free(pas::list_at<TScriptABRequest>(QueuedArcadeBattles, 0)->Ships);
            pas::dispose(pas::list_at<TScriptABRequest>(QueuedArcadeBattles, 0));
            pas::list_delete(QueuedArcadeBattles, 0);
        }
        while (pas::list_count(QueuedPlanetaryBattles) > 0) {
            pas::dispose(pas::list_at<TScriptPBRequest>(QueuedPlanetaryBattles, 0));
            pas::list_delete(QueuedPlanetaryBattles, 0);
        }
        while (pas::list_count(QueuedVideos) > 0) {
            pas::dispose(pas::list_at<TScriptVDRequest>(QueuedVideos, 0));
            pas::list_delete(QueuedVideos, 0);
        }
        ScriptTakeoffRequested = false;
        ScriptEndTurnRequested = false;
        pas::list_clear(StagedArcadeShips);
        StagedArcadeShipScript = nullptr;
    }

    std::uint8_t HasPendingScriptRequests() {
        return pas::list_count(QueuedArcadeBattles) > 0 || pas::list_count(QueuedTextQuests) > 0 || pas::list_count(QueuedPlanetaryBattles) > 0 || pas::list_count(QueuedVideos) > 0 || ScriptTakeoffRequested || ScriptEndTurnRequested;
    }

    std::uint8_t TryShowQueuedArcadeBattle() {
        PScriptABRequest Request{};
        std::uint8_t Result = false;
        if (pas::list_count(QueuedArcadeBattles) > 0) {
            Request = pas::list_at<TScriptABRequest>(QueuedArcadeBattles, 0);
            Globals::ArcadeBattleScreen->SelectedMapName = Request->MapName;
            switch (GlobalsV::CurrentScreenId) {
                case GlobalsV::screenEquipmentShop:
                case GlobalsV::screenGovernment:
                case GlobalsV::screenInfo:
                case GlobalsV::screenGoodsShop: {
                    if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                        ScriptArcadeReturnScreenId = GlobalsV::screenPlanet;
                    } else {
                        ScriptArcadeReturnScreenId = GlobalsV::screenRuinsTalk;
                    }
                    break;
                }
                default: ScriptArcadeReturnScreenId = GlobalsV::CurrentScreenId; break;
            }
            GlobalsV::RequestedScreenId = GlobalsV::screenArcadeBattle;
            reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
            return true;
        }
        return Result;
    }

    std::uint8_t TryShowQueuedTextQuest() {
        std::uint8_t Result = false;
        if (pas::list_count(QueuedTextQuests) > 0) {
            Globals::StandaloneQuestMode = false;
            switch (GlobalsV::CurrentScreenId) {
                case GlobalsV::screenEquipmentShop:
                case GlobalsV::screenGovernment:
                case GlobalsV::screenInfo:
                case GlobalsV::screenGoodsShop: {
                    if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                        GlobalsV::QuestReturnScreenId = GlobalsV::screenPlanet;
                    } else {
                        GlobalsV::QuestReturnScreenId = GlobalsV::screenRuinsTalk;
                    }
                    break;
                }
                default: GlobalsV::QuestReturnScreenId = GlobalsV::CurrentScreenId; break;
            }
            GlobalsV::RequestedScreenId = GlobalsV::screenPlanetQuest;
            reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
            return true;
        }
        return Result;
    }

    std::uint8_t TryRunQueuedPlanetaryBattle() {
        PScriptPBRequest Request{};
        std::int32_t Status{};
        std::uint8_t Failed{};
        std::uint8_t Result = false;
        if (pas::list_count(QueuedPlanetaryBattles) > 0) {
            Status = 0;
            Request = pas::list_at<TScriptPBRequest>(QueuedPlanetaryBattles, 0);
            try {
                Failed = false;
                Status = Robot::FRun(Request->MapName, Request->StartText, Request->SuccessText, Request->FailureText, Request->PlaceText);
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    Failed = true;
                } else {
                    throw;
                }
            }
            if (Failed) {
                if (([&] {
                    const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"FormGov.BattlePlanetQuestCrashed"_wref.get());
                    GI_MessageLoop::TMessageLoopGI* cpp_arg = reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]);
                    return GI_MessageBox::ShowMessageBoxGI(cpp_arg, localizedColorText, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0);
                }()) == GI_MessageBox::mbgResultOK) {
                    Status = 3;
                } else {
                    pas::raise(pas::make_exception<pas::Exception>("Error in Matrix.dll"_a));
                }
            }
            if (Status == 0) {
                return Result;
            }
            if (Status == 1) {
                GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
                return Result;
            }
            if (Status == 3) {
                Status = 2;
            } else {
                Status = 3;
            }
            GlobalsV::RequestedScreenId = GlobalsV::CurrentScreenId;
            reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
            aScript::CompleteQueuedPlanetaryBattle(Status);
            return true;
        }
        return Result;
    }

    std::uint8_t TryShowQueuedVideo() {
        PScriptVDRequest Request{};
        GI_XviD::TxvidGI* Video{};
        std::uint8_t Result = false;
        if (pas::list_count(QueuedVideos) > 0) {
            Request = pas::list_at<TScriptVDRequest>(QueuedVideos, 0);
            if (static_cast<std::uint8_t>(Globals::SkipVideo ^ 1) && reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]) == Globals::RuinsTalkScreen) {
                Video = pas::checked_cast<GI_XviD::TxvidGI*>(reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->GetByName(u"Film"_wref.get()));
                Video->SetActive(true);
                if (Video->ImageOpen(Request->Video, false)) {
                    if (GlobalsV::MusicEnabled) {
                        GR_Main::MusicManager->StopImmediately();
                        while (GR_Main::MusicManager->IsPlaying()) {
                            SysUtilsImports::Sleep(1u);
                        }
                    }
                    if (GlobalsV::MusicEnabled) {
                        GR_Main::MusicManager->PlayCategory(Request->Soundtrack);
                        while (!GR_Main::MusicManager->IsPlaying()) {
                            SysUtilsImports::Sleep(1u);
                        }
                    }
                    Globals::RuinsTalkScreen->ScriptVideoStartedAt = MMSystem::timeGetTime();
                    if (Globals::RuinsTalkScreen->ScriptVideoTimer != nullptr) {
                        Globals::RuinsTalkScreen->CancelCallbackTimer(Globals::RuinsTalkScreen->ScriptVideoTimer);
                        Globals::RuinsTalkScreen->ScriptVideoTimer = nullptr;
                    }
                    Globals::RuinsTalkScreen->ScriptVideoTimer = Globals::RuinsTalkScreen->ScheduleCallbackTimer(5, 5, pas::bind_method<&fRuinsTalk::TfRuinsTalk::AdvanceScriptVideo>(Globals::RuinsTalkScreen), 0);
                } else {
                    aScript::CompleteQueuedVideo(3);
                }
            } else {
                aScript::CompleteQueuedVideo(3);
            }
            return true;
        }
        return Result;
    }

    std::uint8_t TryDispatchScriptTakeoff() {
        std::uint8_t Result = false;
        if (ScriptTakeoffRequested) {
            ScriptTakeoffRequested = false;
            if (aPlayer::GetPlayer()->CurrentPlanet != nullptr || aPlayer::GetPlayer()->DockedTo != nullptr) {
                aPlayer::GetPlayer()->OrderTakeoff();
                aGalaxy::Galaxy->PrimeIntegrityChecksum(212);
                if (aPlayer::GetPlayer()->Order == aShip::soTakeoff) {
                    fHangar::TfHangar::TryTakeOff();
                    ScriptEndTurnRequested = false;
                    reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
                    return true;
                }
            }
        }
        return Result;
    }

    std::uint8_t TryDispatchScriptEndTurn() {
        std::uint8_t Result = false;
        if (ScriptEndTurnRequested) {
            ScriptEndTurnRequested = false;
            if (GlobalsV::CurrentScreenId == GlobalsV::screenRuinsTalk) {
                reinterpret_cast<fPanelMain::TMessageLoopGIWithMainPanel*>(reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]))->MainPanel->EndTurnClicked(nullptr);
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenPlanet) {
                reinterpret_cast<fPanelMain::TMessageLoopGIWithMainPanel*>(reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]))->MainPanel->EndTurnClicked(nullptr);
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenPlanetNO) {
                reinterpret_cast<fPanelMain::TMessageLoopGIWithMainPanel*>(reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]))->MainPanel->EndTurnClicked(nullptr);
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenStarMap) {
                reinterpret_cast<fStarMap::TfStarMap*>(reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]))->EndTurnAfterOpen = true;
            }
            return true;
        }
        return Result;
    }

    // Moves the first request belonging to ResumingScript to its queue's front.
    std::uint8_t TryDispatchResumingScriptRequest() {
        std::int32_t I{};
        PQueuedTextQuest Quest{};
        PQueuedTextQuest Arcade{};
        PQueuedTextQuest Planetary{};
        PQueuedTextQuest Video{};
        std::uint8_t Result = false;
        if (ResumingScript == nullptr) {
            return Result;
        }
        // Native code uses the text-quest record view for all four queues. The
        // $0C comparison is therefore not the Script field of the other records;
        // for video requests it even reads beyond their declared $0C allocation.
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(QueuedVideos) - 1); cpp_range.next(I); ) {
            Video = pas::list_at<TScriptTQRequest>(QueuedVideos, I);
            if (Video->Script == ResumingScript) {
                if (I > 0) {
                    pas::list_delete(QueuedVideos, I);
                    pas::list_insert(QueuedVideos, 0, static_cast<void*>(Video));
                }
                aScript::TryShowQueuedVideo();
                return true;
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(QueuedTextQuests) - 1); cpp_range_2.next(I); ) {
            Quest = pas::list_at<TScriptTQRequest>(QueuedTextQuests, I);
            if (Quest->Script == ResumingScript) {
                if (I > 0) {
                    pas::list_delete(QueuedTextQuests, I);
                    pas::list_insert(QueuedTextQuests, 0, static_cast<void*>(Quest));
                }
                aScript::TryShowQueuedTextQuest();
                return true;
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(QueuedArcadeBattles) - 1); cpp_range_3.next(I); ) {
            Arcade = pas::list_at<TScriptTQRequest>(QueuedArcadeBattles, I);
            if (Arcade->Script == ResumingScript) {
                if (I > 0) {
                    pas::list_delete(QueuedArcadeBattles, I);
                    pas::list_insert(QueuedArcadeBattles, 0, static_cast<void*>(Arcade));
                }
                aScript::TryShowQueuedArcadeBattle();
                return true;
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(QueuedPlanetaryBattles) - 1); cpp_range_4.next(I); ) {
            Planetary = pas::list_at<TScriptTQRequest>(QueuedPlanetaryBattles, I);
            if (Planetary->Script == ResumingScript) {
                if (I > 0) {
                    pas::list_delete(QueuedPlanetaryBattles, I);
                    pas::list_insert(QueuedPlanetaryBattles, 0, static_cast<void*>(Planetary));
                }
                aScript::TryRunQueuedPlanetaryBattle();
                return true;
            }
        }
        return Result;
    }

    std::uint8_t DispatchPendingScriptRequests() {
        std::uint8_t Result = true;
        if (Globals::TurnCalculationThread != nullptr && Globals::TurnCalculationThread->IsRunning()) {
            Globals::TurnCalculationThread->WaitForIdle(WindowsSdk::INFINITE);
        }
        if (aScript::TryDispatchResumingScriptRequest()) {
            return Result;
        }
        if (aScript::TryShowQueuedArcadeBattle()) {
            return Result;
        }
        if (aScript::TryShowQueuedTextQuest()) {
            return Result;
        }
        if (aScript::TryRunQueuedPlanetaryBattle()) {
            return Result;
        }
        if (aScript::TryShowQueuedVideo()) {
            return Result;
        }
        if (aScript::TryDispatchScriptTakeoff()) {
            return Result;
        }
        aScript::TryDispatchScriptEndTurn();
        return false;
    }

    void StartScriptRequestThread() {
        if (ScriptRequestThread != nullptr) {
            if (!ScriptRequestThread->IsRunning()) {
                ScriptRequestThread->Start();
            }
        }
    }

    // Requires a nonempty queue. Clears ResumingScript when no requests remain; preserves GABStatus changed by the script.
    void CompleteQueuedArcadeBattle(std::int32_t Status) {
        PScriptABRequest Request = pas::list_at<TScriptABRequest>(QueuedArcadeBattles, 0);
        pas::list_delete(QueuedArcadeBattles, 0);
        ResumingScript = Request->Script;
        if (Request->Script != nullptr) {
            Request->Script->InitCode->LocalVar->GetVar(u"GABStatus"_wref.get())->SetInt(Status);
            Request->Script->RunTurnCode();
            if (Request->Script->InitCode->LocalVar->GetVar(u"GABStatus"_wref.get())->GetInt() == Status) {
                Request->Script->InitCode->LocalVar->GetVar(u"GABStatus"_wref.get())->SetInt(0);
            }
        }
        pas::list_clear(Request->Ships);
        pas::free(Request->Ships);
        pas::dispose(Request);
        if (!aScript::HasPendingScriptRequests()) {
            ResumingScript = nullptr;
        }
    }

    // Requires a nonempty queue. Clears ResumingScript when no requests remain; preserves GQuestStatus changed by the script.
    void CompleteQueuedTextQuest(TScriptQuestStatus Status) {
        PQueuedTextQuest Request = pas::list_at<TScriptTQRequest>(QueuedTextQuests, 0);
        pas::list_delete(QueuedTextQuests, 0);
        ResumingScript = Request->Script;
        if (Request->Script != nullptr) {
            Request->Script->InitCode->LocalVar->GetVar(u"GQuestStatus"_wref.get())->SetInt(static_cast<std::int32_t>(Status));
            Request->Script->RunTurnCode();
            if (Request->Script->InitCode->LocalVar->GetVar(u"GQuestStatus"_wref.get())->GetInt() == static_cast<std::int32_t>(Status)) {
                Request->Script->InitCode->LocalVar->GetVar(u"GQuestStatus"_wref.get())->SetInt(0);
            }
        }
        pas::dispose(Request);
        if (!aScript::HasPendingScriptRequests()) {
            ResumingScript = nullptr;
        }
    }

    // Requires a nonempty queue. Clears ResumingScript when no requests remain; preserves GRobotStatus changed by the script.
    void CompleteQueuedPlanetaryBattle(std::int32_t Status) {
        PScriptPBRequest Request = pas::list_at<TScriptPBRequest>(QueuedPlanetaryBattles, 0);
        pas::list_delete(QueuedPlanetaryBattles, 0);
        ResumingScript = Request->Script;
        if (Request->Script != nullptr) {
            Request->Script->InitCode->LocalVar->GetVar(u"GRobotStatus"_wref.get())->SetInt(Status);
            Request->Script->RunTurnCode();
            if (Request->Script->InitCode->LocalVar->GetVar(u"GRobotStatus"_wref.get())->GetInt() == Status) {
                Request->Script->InitCode->LocalVar->GetVar(u"GRobotStatus"_wref.get())->SetInt(0);
            }
        }
        pas::dispose(Request);
        if (!aScript::HasPendingScriptRequests()) {
            ResumingScript = nullptr;
        }
    }

    // Requires a nonempty queue when a player exists. Clears ResumingScript when no requests remain; preserves GVideoStatus changed by the script.
    void CompleteQueuedVideo(std::int32_t Status) {
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        PScriptVDRequest Request = pas::list_at<TScriptVDRequest>(QueuedVideos, 0);
        pas::list_delete(QueuedVideos, 0);
        ResumingScript = Request->Script;
        if (Request->Script != nullptr) {
            Request->Script->InitCode->LocalVar->GetVar(u"GVideoStatus"_wref.get())->SetInt(Status);
            Request->Script->RunTurnCode();
            if (Request->Script->InitCode->LocalVar->GetVar(u"GVideoStatus"_wref.get())->GetInt() == Status) {
                Request->Script->InitCode->LocalVar->GetVar(u"GVideoStatus"_wref.get())->SetInt(0);
            }
        }
        pas::dispose(Request);
        if (!aScript::HasPendingScriptRequests()) {
            ResumingScript = nullptr;
        }
    }

    void LogScriptStepCount(std::int32_t ExpressionCount) {
        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({aScript::GetScriptContextDescription(), u" code expressions count ", EC_Str::IntToWideString(ExpressionCount)})));
        if (ExpressionCount > 200000) {
            aScript::LogScriptCallHistory();
        }
    }

    void InitializeScriptEngine() {
        ScriptProcess = pas::construct_call<EC_Expression::TCodeProcessEC>(EC_Expression::TCodeProcessEC_Create);
        CurrentScript = nullptr;
        ScriptFunctionScope = pas::construct_call<EC_Expression::TVarArrayEC>(EC_Expression::TVarArrayEC_Create);
        aScriptFun::InitializeScriptBuiltinsAndConstants(ScriptFunctionScope);
        EC_Expression::SetScriptStepCallback(EC_Expression::TScriptStepCallback(LogScriptStepCount), 100000);
    }

    void FinalizeScriptEngine() {
        if (ScriptFunctionScope != nullptr) {
            pas::free(ScriptFunctionScope);
            ScriptFunctionScope = nullptr;
        }
        if (ScriptProcess != nullptr) {
            pas::free(ScriptProcess);
            ScriptProcess = nullptr;
        }
    }

    void RunGlobalScriptsForContext(aGalaxy::TStar* Star, std::int32_t RunFrom) {
        Globals::TScriptTemplUnit* Template{};
        std::int32_t I{};
        Globals::SharedScriptVariables->GetVar(u"GRunFrom"_wref.get())->SetInt(RunFrom);
        Globals::SharedScriptVariables->GetVar(u"GRunStar"_wref.get())->SetDword(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Star)));
        pas::List* Candidates = pas::make_object<pas::List>();
        Globals::CollectInactiveScriptTemplates(Candidates);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Candidates) - 1); cpp_range.next(I); ) {
            Template = pas::list_at<Globals::TScriptTemplUnit>(Candidates, I);
            Globals::ScriptTemplateStartRequested = false;
            try {
                Template->ConditionCode->Run(ScriptProcess);
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
                } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                    aScript::LogScriptCallHistory();
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in global code of script ", Template->FileName}))));
                } else {
                    throw;
                }
            }
            if (Globals::ScriptTemplateStartRequested) {
                aScript::TryStartScriptInstanceFromTemplate(Star, nullptr, pas::list_indexof(Globals::ScriptTemplates, reinterpret_cast<void*>(Template)));
            }
        }
        pas::free(Candidates);
    }

    std::uint8_t TryStartScriptByName(aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet, pas::WideString Name) {
        std::int32_t Index = Globals::FindScriptTemplateIndex(Name);
        if (Index < 0) {
            return false;
        }
        return aScript::TryStartScriptInstanceFromTemplate(AnchorStar, AnchorPlanet, Index);
    }

    std::uint8_t TryStartScriptInstanceFromTemplate(aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet, std::int32_t TemplateIndex) {
        Globals::TScriptTemplUnit* Template = pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, TemplateIndex);
        TScript* Script = pas::construct_call<TScript>(TScript_Create);
        pas::list_add(aGalaxy::Galaxy->Scripts, reinterpret_cast<void*>(Script));
        Template->ActiveScriptIndex = pas::list_count(aGalaxy::Galaxy->Scripts) - 1;
        Script->ClassId = Template->ConfigValue;
        if (Script->LoadFromFile(Template->FileName, AnchorStar, AnchorPlanet, true)) {
            Template->LastTurn = aGalaxy::Galaxy->CurrentTurn;
            ++Template->UseCount;
            return true;
        }
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Scripts, reinterpret_cast<void*>(Script));
        if (Index >= 0) {
            pas::list_delete(aGalaxy::Galaxy->Scripts, Index);
            pas::free(Script);
        }
        Template->ActiveScriptIndex = -1;
        return false;
    }

    // Replaces and frees Script on success; retains it on failure. Requires an existing Galaxy.Scripts entry.
    std::uint8_t TryRestartScript(TScript* Script, aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet) {
        std::int32_t I{};
        Globals::TMessagePlayer* Message{};
        std::uint8_t Result = false;
        if (Script == nullptr) {
            return Result;
        }
        std::int32_t Index = Globals::FindScriptTemplateIndex(Script->ScriptFileName);
        if (Index < 0) {
            return Result;
        }
        Globals::TScriptTemplUnit* Template = pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, Index);
        I = pas::list_indexof(aGalaxy::Galaxy->Scripts, reinterpret_cast<void*>(Script));
        TScript* NewScript = pas::construct_call<TScript>(TScript_Create);
        pas::list_put(aGalaxy::Galaxy->Scripts, I, reinterpret_cast<void*>(NewScript));
        NewScript->ClassId = Template->ConfigValue;
        if (NewScript->LoadFromFile(Template->FileName, AnchorStar, AnchorPlanet, true)) {
            Template->LastTurn = aGalaxy::Galaxy->CurrentTurn;
            ++Template->UseCount;
            Result = true;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Script->EtherIds->GetCount() - 1); cpp_range.next(I); ) {
                Message = Globals::FindPlayerBubbleByKey(Script->EtherIds->GetTextAt(I), false);
                if (Message != nullptr && Message->Kind == 3) {
                    Message->Kind = 5;
                    Message->WasRead = false;
                }
            }
            pas::free(Script);
        } else {
            pas::list_put(aGalaxy::Galaxy->Scripts, I, reinterpret_cast<void*>(Script));
            pas::free(NewScript);
        }
        return Result;
    }

    void CompileScriptTemplateCondition(std::int32_t TemplateIndex) {
        Globals::TScriptTemplUnit* Template{};
        EC_CacheBuf::TCBufEC* CachedBuffer{};
        EC_Buf::TBufEC* Buffer{};
        EC_Expression::TCodeAnalyzerEC* Analyzer{};
        pas::WideString ErrorText{};
        std::uint32_t Version{};
        Template = pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, TemplateIndex);
        EC_CacheBuf::TCBufControlEC* Control = nullptr;
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(Template->FileName);
                CachedBuffer = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                Buffer = CachedBuffer->Buffer;
                Version = EC_Buf::TBufEC_GetUInt32(Buffer);
                if (Version < 5 || Version > 8) {
                    GR_Main::RaiseWideMessage(u"Script file incorrect version"_wref.get());
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                EC_Buf::TBufEC_GetUInt32(Buffer);
                Globals::GlobalScriptVariables->AppendFromBuffer(Buffer);
                Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
                Analyzer->Tokenize(Buffer->ReadWideString(), 0);
                Analyzer->RemoveComments();
                Analyzer->RemoveNewlines();
                Analyzer->RemoveWhitespace();
                Analyzer->ValidateDelimiters();
                Template->ConditionCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
                pas::free(Analyzer);
                if (ErrorText != u"") {
                    GR_Main::RaiseWideMessage(pas::concat_wide({u"ScriptFirstLoad.Compiler. Error=", ErrorText}));
                }
                {
                    EC_Expression::TVarEC* add = Template->ConditionCode->LocalVar->Add(u"GScriptName"_wref.get(), EC_Expression::vkString);
                    const pas::WideString& name = Template->Name;
                    add->SetString(name);
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            if (Control != nullptr) {
                Control->Release();
                pas::free(Control);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return;
            }
        }
    }

    std::uint8_t IsStarProtectedByScript(aGalaxy::TStar* Star) {
        std::int32_t I{};
        std::int32_t J{};
        TScript* Script{};
        TScriptStar* Binding{};
        if (Star->NoComeKling) {
            return true;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range.next(I); ) {
            Script = pas::list_at<TScript>(aGalaxy::Galaxy->Scripts, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Script->Stars) - 1); cpp_range_2.next(J); ) {
                Binding = pas::list_at<TScriptStar>(Script->Stars, J);
                if (Binding->ProtectStar && Binding->Star == Star) {
                    return true;
                }
            }
        }
        return false;
    }

    std::uint8_t ScriptDefinitionBit(std::uint32_t Value, std::int32_t BitIndex) {
        return pas::shr(Value, BitIndex) & 1;
    }

    aGalaxyStruct::TOwnerMask DecodeScriptRaceMask(std::uint32_t Value) {
        aGalaxyStruct::TOwnerMask Result{};
        if (!aScript::ScriptDefinitionBit(Value, 0)) {
            Result = pas::constant_set<aGalaxyStruct::TOwnerMask>({{0, 4}});
            return Result;
        }
        Result = pas::constant_set<aGalaxyStruct::TOwnerMask>({});
        if (aScript::ScriptDefinitionBit(Value, 1)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}});
        }
        if (aScript::ScriptDefinitionBit(Value, 2)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}});
        }
        if (aScript::ScriptDefinitionBit(Value, 3)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{2}});
        }
        if (aScript::ScriptDefinitionBit(Value, 4)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{3}});
        }
        if (aScript::ScriptDefinitionBit(Value, 5)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{4}});
        }
        return Result;
    }

    // Bit 9 also selects the player's current owner ID.
    aGalaxyStruct::TOwnerMask DecodeScriptOwnerMask(std::uint32_t Value) {
        aGalaxyStruct::TOwnerMask Result{};
        if (!aScript::ScriptDefinitionBit(Value, 0)) {
            Result = pas::constant_set<aGalaxyStruct::TOwnerMask>({{0, 7}});
            return Result;
        }
        Result = pas::constant_set<aGalaxyStruct::TOwnerMask>({});
        if (aScript::ScriptDefinitionBit(Value, 1)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}});
        }
        if (aScript::ScriptDefinitionBit(Value, 2)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}});
        }
        if (aScript::ScriptDefinitionBit(Value, 3)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{2}});
        }
        if (aScript::ScriptDefinitionBit(Value, 4)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{3}});
        }
        if (aScript::ScriptDefinitionBit(Value, 5)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{4}});
        }
        if (aScript::ScriptDefinitionBit(Value, 6)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{5}});
        }
        if (aScript::ScriptDefinitionBit(Value, 7)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{6}});
        }
        if (aScript::ScriptDefinitionBit(Value, 8)) {
            Result = Result + pas::constant_set<aGalaxyStruct::TOwnerMask>({{7}});
        }
        if (aScript::ScriptDefinitionBit(Value, 9) && aPlayer::GetPlayer() != nullptr) {
            Result = Result + pas::make_set<aGalaxyStruct::TOwnerMask>({{static_cast<std::int32_t>(aPlayer::GetPlayer()->OwnerId)}});
        }
        return Result;
    }

    TScriptEconomyMask DecodeScriptEconomyMask(std::uint32_t Value) {
        TScriptEconomyMask Result{};
        if (!aScript::ScriptDefinitionBit(Value, 0)) {
            Result = pas::constant_set<TScriptEconomyMask>({{0, 2}});
            return Result;
        }
        Result = pas::constant_set<TScriptEconomyMask>({});
        if (aScript::ScriptDefinitionBit(Value, 1)) {
            Result = Result + pas::constant_set<TScriptEconomyMask>({{0}});
        }
        if (aScript::ScriptDefinitionBit(Value, 2)) {
            Result = Result + pas::constant_set<TScriptEconomyMask>({{2}});
        }
        if (aScript::ScriptDefinitionBit(Value, 3)) {
            Result = Result + pas::constant_set<TScriptEconomyMask>({{1}});
        }
        return Result;
    }

    TScriptGovernmentMask DecodeScriptGovernmentMask(std::uint32_t Value) {
        TScriptGovernmentMask Result{};
        if (!aScript::ScriptDefinitionBit(Value, 0)) {
            Result = pas::constant_set<TScriptGovernmentMask>({{0, 4}});
            return Result;
        }
        Result = pas::constant_set<TScriptGovernmentMask>({});
        if (aScript::ScriptDefinitionBit(Value, 1)) {
            Result = Result + pas::constant_set<TScriptGovernmentMask>({{0}});
        }
        if (aScript::ScriptDefinitionBit(Value, 2)) {
            Result = Result + pas::constant_set<TScriptGovernmentMask>({{1}});
        }
        if (aScript::ScriptDefinitionBit(Value, 3)) {
            Result = Result + pas::constant_set<TScriptGovernmentMask>({{2}});
        }
        if (aScript::ScriptDefinitionBit(Value, 4)) {
            Result = Result + pas::constant_set<TScriptGovernmentMask>({{3}});
        }
        if (aScript::ScriptDefinitionBit(Value, 5)) {
            Result = Result + pas::constant_set<TScriptGovernmentMask>({{4}});
        }
        return Result;
    }

    TScriptShipTypeMask DecodeScriptShipTypeMask(std::uint32_t Value) {
        TScriptShipTypeMask Result{};
        std::int32_t I{};
        if (!aScript::ScriptDefinitionBit(Value, 0)) {
            Result = pas::constant_set<TScriptShipTypeMask>({{0, 8}});
            return Result;
        }
        Result = pas::constant_set<TScriptShipTypeMask>({});
        if (aScript::ScriptDefinitionBit(Value, 1)) {
            Result = Result + pas::constant_set<TScriptShipTypeMask>({{0}});
        }
        if (aScript::ScriptDefinitionBit(Value, 2)) {
            Result = Result + pas::constant_set<TScriptShipTypeMask>({{1}});
        }
        if (aScript::ScriptDefinitionBit(Value, 3)) {
            Result = Result + pas::constant_set<TScriptShipTypeMask>({{2}});
        }
        if (aScript::ScriptDefinitionBit(Value, 4)) {
            Result = Result + pas::constant_set<TScriptShipTypeMask>({{3}});
        }
        if (aScript::ScriptDefinitionBit(Value, 5)) {
            Result = Result + pas::constant_set<TScriptShipTypeMask>({{4}});
        }
        if (aScript::ScriptDefinitionBit(Value, 6)) {
            Result = Result + pas::constant_set<TScriptShipTypeMask>({{5}});
        }
        if (aScript::ScriptDefinitionBit(Value, 25)) {
            Result = Result + pas::constant_set<TScriptShipTypeMask>({{7}});
        }
        for (I = 7; I <= 24; ++I) {
            if (aScript::ScriptDefinitionBit(Value, I)) {
                Result = Result + pas::constant_set<TScriptShipTypeMask>({{6}});
                break;
            }
        }
        return Result;
    }

    aGalaxy::TDominatorSeriesMask DecodeScriptDominatorMask(std::uint32_t Value, std::uint8_t KlingType) {
        aGalaxy::TDominatorSeriesMask Result{};
        if (!aScript::ScriptDefinitionBit(Value, 0)) {
            Result = pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0, 2}});
            return Result;
        }
        Result = pas::constant_set<aGalaxy::TDominatorSeriesMask>({});
        switch (KlingType) {
            case 0: {
                if (aScript::ScriptDefinitionBit(Value, 7)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0}});
                }
                if (aScript::ScriptDefinitionBit(Value, 13)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{1}});
                }
                if (aScript::ScriptDefinitionBit(Value, 19)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{2}});
                }
                break;
            }
            case 1: {
                if (aScript::ScriptDefinitionBit(Value, 8)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0}});
                }
                if (aScript::ScriptDefinitionBit(Value, 14)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{1}});
                }
                if (aScript::ScriptDefinitionBit(Value, 20)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{2}});
                }
                break;
            }
            case 2: {
                if (aScript::ScriptDefinitionBit(Value, 9)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0}});
                }
                if (aScript::ScriptDefinitionBit(Value, 15)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{1}});
                }
                if (aScript::ScriptDefinitionBit(Value, 21)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{2}});
                }
                break;
            }
            case 3: {
                if (aScript::ScriptDefinitionBit(Value, 10)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0}});
                }
                if (aScript::ScriptDefinitionBit(Value, 16)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{1}});
                }
                if (aScript::ScriptDefinitionBit(Value, 22)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{2}});
                }
                break;
            }
            case 4: {
                if (aScript::ScriptDefinitionBit(Value, 11)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0}});
                }
                if (aScript::ScriptDefinitionBit(Value, 17)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{1}});
                }
                if (aScript::ScriptDefinitionBit(Value, 23)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{2}});
                }
                break;
            }
            case 5: {
                if (aScript::ScriptDefinitionBit(Value, 12)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0}});
                }
                if (aScript::ScriptDefinitionBit(Value, 18)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{1}});
                }
                if (aScript::ScriptDefinitionBit(Value, 24)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{2}});
                }
                break;
            }
            case 6: {
                if (aScript::ScriptDefinitionBit(Value, 26)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0}});
                }
                if (aScript::ScriptDefinitionBit(Value, 28)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{1}});
                }
                if (aScript::ScriptDefinitionBit(Value, 30)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{2}});
                }
                break;
            }
            case 7: {
                if (aScript::ScriptDefinitionBit(Value, 27)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0}});
                }
                if (aScript::ScriptDefinitionBit(Value, 29)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{1}});
                }
                if (aScript::ScriptDefinitionBit(Value, 31)) {
                    Result = Result + pas::constant_set<aGalaxy::TDominatorSeriesMask>({{2}});
                }
                break;
            }
        }
        return Result;
    }

    // Values outside 0..7 become owner 6.
    std::uint8_t DecodeScriptItemOwner(std::int32_t Value) {
        if (Value == 0) {
            return 0;
        } else if (Value == 1) {
            return 1;
        } else if (Value == 2) {
            return 2;
        } else if (Value == 3) {
            return 3;
        } else if (Value == 4) {
            return 4;
        } else if (Value == 5) {
            return 5;
        } else if (Value == 6) {
            return 6;
        } else if (Value == 7) {
            return 7;
        } else {
            return 6;
        }
    }

    // Values outside 0..4 become hostile.
    aGalaxyStruct::TRelationLevel DecodeScriptRelationLevel(std::int32_t Value) {
        if (Value == 0) {
            return aGalaxyStruct::rlHostile;
        } else if (Value == 1) {
            return aGalaxyStruct::rlBad;
        } else if (Value == 2) {
            return aGalaxyStruct::rlNormal;
        } else if (Value == 3) {
            return aGalaxyStruct::rlGood;
        } else if (Value == 4) {
            return aGalaxyStruct::rlExcellent;
        } else {
            return aGalaxyStruct::rlHostile;
        }
    }

    // DominatorMasks requires eight entries indexed by TKlingType. StationNames is a comma-separated filter when ship-type bit 8 is set.
    std::uint8_t ScriptShipMatchesType(aShip::TShip* Ship, TScriptShipTypeMask ShipTypeMask, pas::WideString StationNames, pas::OpenArray<aGalaxy::TDominatorSeriesMask> DominatorMasks) {
        auto cpp_array_copy = pas::copy_open_array(DominatorMasks);
        DominatorMasks = pas::open_array(cpp_array_copy);
        pas::WideString Name{};
        std::int32_t I{};
        std::int32_t Count{};
        std::uint8_t Result = false;
        if (!pas::contains(ShipTypeMask, aConst::ShipToHullType(Ship))) {
            return Result;
        }
        if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr && pas::contains(ShipTypeMask, 8)) {
            Name = pas::WideString();
            if (!pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                return Result;
            }
            if (reinterpret_cast<aRuins::TRuins*>(Ship)->NoLanding) {
                return Result;
            }
            if (Ship->TypeNameOverrideKey != u"") {
                Name = Ship->TypeNameOverrideKey;
            } else {
                Name = aConst::ShipTypeNames[Ship->TypeId].Name;
            }
            Count = EC_Str::CountDelimitedPartsW(StationNames, u","_wref.get());
            I = 0;
            while (I < Count) {
                if (EC_Str::ExtractDelimitedPartW(StationNames, I, u","_wref.get()) == Name) {
                    break;
                }
                ++I;
            }
            if (I >= Count) {
                return Result;
            }
        }
        if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) {
            const aGalaxy::TDominatorSeriesMask& cpp_set = DominatorMasks[pas::checked_cast<aKling::TKling*>(Ship)->KlingType];
            std::uint8_t cpp_element = static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Ship)->DominatorSeries);
            if (!pas::contains(cpp_set, cpp_element)) {
                return Result;
            }
        }
        return true;
    }

    // Caller owns the list; ship references are borrowed.
    pas::List* CollectScriptCandidateShips(aGalaxy::TStar* Star) {
        aShip::TShip* Ship{};
        aShip::TShip* OtherShip{};
        aPlanet::TPlanet* Planet{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::uint8_t Strict{};
        pas::List* Candidates = pas::make_object<pas::List>();
        std::int32_t ShipCount = pas::list_count(Star->Ships);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
            if (Ship->LiberationGroup != nullptr) {
                continue;
            }
            Strict = aPlayer::GetPlayer() != Ship && (!(pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) || pas::checked_cast<aKling::TKling*>(Ship)->KlingType != aGalaxyStruct::ktBoss);
            if (Ship->ScriptShip != nullptr) {
                continue;
            }
            if (Ship->AbsoluteScriptOrder > 0) {
                continue;
            }
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip != nullptr) {
                continue;
            }
            if (Strict && Ship->InHyperspace) {
                continue;
            }
            if (Strict && !(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) && Ship->EnemyShip != nullptr && Ship->EnemyShip->CurrentStar == Ship->CurrentStar) {
                continue;
            }
            if (Strict && !(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) && Ship->PartnerShip != nullptr) {
                continue;
            }
            if (Strict && !(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) && Ship->GetHull()->HullPoints < Ship->GetHull()->Weight / 2) {
                continue;
            }
            if (Strict && Ship->InNormalSpace() && pas::sqr(static_cast<pas::Extended>(Star->SafeRadius)) > aMyFunction::PointDistanceSquared(Ship->Position, EC_Struct::MakePointF(0.0f, 0.0f))) {
                continue;
            }
            if (Strict) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range_2.next(J); ) {
                    OtherShip = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (OtherShip == Ship) {
                        continue;
                    }
                    if (OtherShip->EnemyShip == Ship || OtherShip->PartnerShip == Ship) {
                        break;
                    }
                }
                if (J < ShipCount) {
                    continue;
                }
            }
            pas::list_add(Candidates, reinterpret_cast<void*>(Ship));
        }
        std::int32_t PlanetCount = pas::list_count(Star->Planets);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, PlanetCount - 1); cpp_range_3.next(K); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, K);
            ShipCount = pas::list_count(Planet->Warriors);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range_4.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Planet->Warriors, I);
                if (Ship->ScriptShip != nullptr) {
                    continue;
                }
                if (Ship->AbsoluteScriptOrder > 0) {
                    continue;
                }
                if (pas::list_indexof(Candidates, reinterpret_cast<void*>(Ship)) >= 0) {
                    continue;
                }
                if (Ship->GetHull()->HullPoints < Ship->GetHull()->Weight / 2) {
                    continue;
                }
                pas::list_add(Candidates, reinterpret_cast<void*>(Ship));
            }
        }
        return Candidates;
    }

    // Returns the first match or nil; leaves Candidates unchanged.
    aShip::TShip* FindScriptGroupCandidate(pas::List* Candidates, TScriptGroup* Group) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        EC_Struct::TPointF Point{};
        float DistanceSquared{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Candidates) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Candidates, I);
            if (Ship->LiberationGroup != nullptr) {
                continue;
            }
            if (!pas::contains(Group->OwnerMask, Ship->OwnerId)) {
                continue;
            }
            if (!aScript::ScriptShipMatchesType(Ship, Group->ShipTypeMask, Group->StationNames, pas::open_array(Group->DominatorMasks))) {
                continue;
            }
            if (aPlayer::GetPlayer() == Ship) {
                continue;
            }
            if (aKling::BlazerShip != Ship && aKling::KellerShip != Ship && aKling::TerronShip != Ship) {
                if (Ship->Speed < Group->MinSpeed) {
                    continue;
                }
                if (Ship->Speed > Group->MaxSpeed) {
                    continue;
                }
            }
            if (Group->WeaponRequirement == 1 && Ship->WeaponCount <= 0) {
                continue;
            }
            if (Group->WeaponRequirement == 2 && Ship->WeaponCount > 0) {
                continue;
            }
            if (Group->MinCargoHookLevel > 0) {
                if (Ship->GetCargoHook() == nullptr) {
                    continue;
                }
                if (Ship->GetCargoHook()->GetLevel() < Group->MinCargoHookLevel) {
                    continue;
                }
            }
            if (Ship->CargoFreeSpace < Group->MinFreeCargoSpace) {
                continue;
            }
            if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
                if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader] < Group->MinTraderStatus) {
                    continue;
                }
                if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader] > Group->MaxTraderStatus) {
                    continue;
                }
                if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior] < Group->MinWarriorStatus) {
                    continue;
                }
                if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior] > Group->MaxWarriorStatus) {
                    continue;
                }
                if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate] < Group->MinPirateStatus) {
                    continue;
                }
                if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate] > Group->MaxPirateStatus) {
                    continue;
                }
            }
            if (Group->MaxDistanceFromPlanet < 10000 && Ship->CurrentPlanet != Group->Planet) {
                Point = Group->Planet->GetPosition();
                if (Ship->CurrentPlanet != nullptr) {
                    DistanceSquared = aMyFunction::PointDistanceSquared(Point, Ship->CurrentPlanet->GetPosition());
                } else {
                    DistanceSquared = aMyFunction::PointDistanceSquared(Point, Ship->Position);
                }
                if (static_cast<long double>(DistanceSquared) > pas::sqr(Group->MaxDistanceFromPlanet)) {
                    continue;
                }
            }
            if (Group->MinStrength != 0.0L || Group->MaxStrength != 0.0L) {
                if (Ship->StrengthInBestRanger < Group->MinStrength) {
                    continue;
                }
                if (Ship->StrengthInBestRanger > Group->MaxStrength) {
                    continue;
                }
            }
            return Ship;
        }
        return nullptr;
    }

    // The player may have multiple bindings; ordinary ships have one.
    TScriptShip* GetScriptShipBindingForContext(aShip::TShip* Ship, TScript* Script) {
        std::int32_t I{};
        TScriptShip* Result = nullptr;
        if (pas::class_cast_if<aPlayer::TPlayer*>(Ship) != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(reinterpret_cast<aPlayer::TPlayer*>(Ship)->ScriptShipBindings) - 1); cpp_range.next(I); ) {
                Result = pas::list_at<TScriptShip>(reinterpret_cast<aPlayer::TPlayer*>(Ship)->ScriptShipBindings, I);
                if (Result->Script == Script) {
                    break;
                }
                Result = nullptr;
            }
            return Result;
        }
        return reinterpret_cast<TScriptShip*>(Ship->ScriptShip);
    }

    void ClearScriptDialogRules() {
        std::int32_t I{};
        std::int32_t Count{};
        // Native untyped Dispose: these records have no managed-field finalization.
        if (ScriptDialogOverrides != nullptr) {
            Count = pas::list_count(ScriptDialogOverrides);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                pas::dispose(pas::list_get(ScriptDialogOverrides, I));
            }
            pas::list_clear(ScriptDialogOverrides);
        } else {
            ScriptDialogOverrides = pas::make_object<aMyFunction::TObjectList>();
        }
        if (ScriptDialogInjections != nullptr) {
            Count = pas::list_count(ScriptDialogInjections);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                pas::dispose(pas::list_get(ScriptDialogInjections, I));
            }
            pas::list_clear(ScriptDialogInjections);
        } else {
            ScriptDialogInjections = pas::make_object<aMyFunction::TObjectList>();
        }
        if (ScriptDialogBlocks != nullptr) {
            Count = pas::list_count(ScriptDialogBlocks);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
                pas::dispose(pas::list_get(ScriptDialogBlocks, I));
            }
            pas::list_clear(ScriptDialogBlocks);
        } else {
            ScriptDialogBlocks = pas::make_object<aMyFunction::TObjectList>();
        }
    }

    // Nil Scope temporarily clears CurrentScript. Compiled code is freed after execution.
    void ExecuteScriptText(pas::WideString SourceText, EC_Expression::TVarArrayEC* Scope) {
        EC_Expression::TVarArrayEC* ScriptScope{};
        EC_Expression::TCodeEC* Code = aScript::CompileScriptText(SourceText);
        TScript* SavedScript = CurrentScript;
        if (Scope == nullptr) {
            CurrentScript = nullptr;
        }
        Code->LinkAll(Globals::SharedScriptVariables, false);
        Code->LinkAll(ScriptFunctionScope, false);
        Code->ScriptFunLinked = true;
        if (Scope != nullptr) {
            Code->LinkAll(Scope, false);
        }
        if (CurrentScript != nullptr) {
            ScriptScope = CurrentScript->InitCode->LocalVar;
            if (ScriptScope != Scope) {
                Code->LinkAll(ScriptScope, false);
            }
        }
        try {
            Code->Run(ScriptProcess);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                GR_Main::AppendLogLineThreadSafe("Error while executing code from string: "_a);
                if (SourceText.length() <= 256) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(SourceText));
                } else {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({pas::copy(SourceText, 1, 256), u" ..."})));
                }
                throw;
            } else {
                throw;
            }
        }
        CurrentScript = SavedScript;
        pas::free(Code);
    }

    // Caller owns the returned code.
    EC_Expression::TCodeEC* CompileScriptText(pas::WideString SourceText) {
        pas::WideString ErrorText{};
        pas::WideString DelimiterError{};
        EC_Expression::TCodeAnalyzerEC* Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
        Analyzer->Tokenize(SourceText, 0);
        Analyzer->RemoveComments();
        Analyzer->RemoveNewlines();
        Analyzer->RemoveWhitespace();
        DelimiterError = Analyzer->ValidateDelimiters();
        EC_Expression::TCodeEC* Result = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
        Result->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
        if (ErrorText != u"") {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Compiler. Error=", ErrorText})));
            GR_Main::AppendLogLineThreadSafe(pas::AnsiString());
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(SourceText));
            GR_Main::AppendLogLineThreadSafe(pas::AnsiString());
            GR_Main::RaiseWideMessage(pas::concat_wide({u"Compiler. Error=", ErrorText}));
        }
        pas::free(Analyzer);
        return Result;
    }

    void RunScriptCode(pas::WideString ContextName, EC_Expression::TCodeEC* Code, EC_Expression::TCodeEC* ParentCode) {
        Code->LinkAll(Globals::SharedScriptVariables, false);
        if (!Code->ScriptFunLinked) {
            Code->LinkAll(ScriptFunctionScope, false);
            Code->ScriptFunLinked = true;
        }
        if (ParentCode != nullptr) {
            Code->LinkAll(ParentCode->LocalVar, false);
        }
        try {
            Code->Run(ScriptProcess);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                GR_Main::AppendLogLineThreadSafe("Error while executing code from string: "_a);
                if (ContextName.length() <= 128) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(ContextName));
                } else {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({pas::copy(ContextName, 1, 128), u" ..."})));
                }
                pas::raise(pas::make_exception<pas::Exception>(pas::AnsiString()));
            } else {
                throw;
            }
        }
    }

    TScriptCacheUnit* GetCachedActionCode(TScriptCache*& Cache, pas::WideString Name, EC_BlockPar::TBlockParEC* Config) {
        TScriptCacheUnit* Result = nullptr;
        if (Config == nullptr) {
            return Result;
        }
        if (Cache == nullptr) {
            Cache = pas::construct_call<TScriptCache>(TScriptCache_Create);
        }
        return Cache->GetOrCompile(Name, Config);
    }

    // Runs with CurrentScript nil. VirtualKey=0 leaves KEY and KEYMOD unchanged.
    void ExecuteGameplayUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t VirtualKey) {
        pas::WideString Text{};
        EC_Expression::TVarEC* Cell{};
        std::uint32_t KeyModifiers{};
        if (Block == nullptr) {
            return;
        }
        if (GameplayUiScriptCache == nullptr) {
            GameplayUiScriptCache = pas::construct_call<TScriptGICache>(TScriptGICache_Create);
        }
        TScriptGICacheUnit* Entry = GameplayUiScriptCache->GetOrCompile(Block);
        if (Entry == nullptr) {
            return;
        }
        EC_Expression::TCodeEC* Code = Entry->Code;
        Code->LinkAll(Globals::SharedScriptVariables, false);
        Code->LinkAll(ScriptFunctionScope, false);
        Code->ScriptFunLinked = true;
        TScript* SavedScript = CurrentScript;
        CurrentScript = nullptr;
        try {
            if (VirtualKey != 0) {
                Cell = Code->LocalVar->GetVarNE(u"KEY"_wref.get());
                if (Cell != nullptr) {
                    Cell->SetDword(VirtualKey);
                } else {
                    Code->LocalVar->Add(u"KEY"_wref.get(), EC_Expression::vkDword)->SetDword(VirtualKey);
                }
                {
                    std::int32_t cpp_left_2 = GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) & 127;
                    std::int32_t cpp_left = cpp_left_2 + (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) & 127) * 2;
                    KeyModifiers = cpp_left + (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) & 127) * 4;
                }
                Cell = Code->LocalVar->GetVarNE(u"KEYMOD"_wref.get());
                if (Cell != nullptr) {
                    Cell->SetDword(KeyModifiers);
                } else {
                    Code->LocalVar->Add(u"KEYMOD"_wref.get(), EC_Expression::vkDword)->SetDword(KeyModifiers);
                }
            }
            Code->Run(ScriptProcess);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                GR_Main::AppendLogLineThreadSafe("Error while executing GI code:"_a);
                Text = Entry->SourceText;
                if (Text.length() <= 256) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(Text));
                } else {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({pas::copy(Text, 1, 256), u" ..."})));
                }
                throw;
            } else {
                throw;
            }
        }
        CurrentScript = SavedScript;
    }

    // When CurrentScript is nil, only Snapshot.Script is written.
    void ScriptSnap(TScriptContextSnapshot& Snapshot) {
        Snapshot.Script = CurrentScript;
        if (CurrentScript != nullptr) {
            Snapshot.CurrentShip = CurrentScript->CurrentShip;
            Snapshot.EndState = CurrentScript->InitCode->LocalVar->GetVar(u"EndState"_wref.get())->GetInt() != 0;
        }
    }

    void ScriptUnSnap(TScriptContextSnapshot Snapshot) {
        CurrentScript = Snapshot.Script;
        if (CurrentScript != nullptr) {
            CurrentScript->CurrentShip = Snapshot.CurrentShip;
            CurrentScript->InitCode->LocalVar->GetVar(u"CurShip"_wref.get())->SetDword(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Snapshot.CurrentShip)));
            CurrentScript->InitCode->LocalVar->GetVar(u"EndState"_wref.get())->SetInt(Snapshot.EndState);
        }
    }

    // Returns ScriptItemActParam, initially zero.
    std::int32_t RunItemUseCode(aItem::TItem* Item, aShip::TShip* Ship) {
        TScript* Script{};
        pas::WideString Text{};
        std::int32_t Result = 0;
        if (Item == nullptr) {
            return Result;
        }
        Globals::ScriptUseItem = Item;
        TScriptItem* Binding = pas::checked_cast<TScriptItem*>(Item->ScriptItem);
        TScript* SavedScript = CurrentScript;
        CurrentScript = nullptr;
        EC_Expression::TCodeEC* ParentCode = nullptr;
        if (Binding != nullptr) {
            Text = Binding->OnUseText;
        } else {
            Text = pas::WideString();
        }
        if (Text == u"") {
            if (pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
                Text = pas::checked_cast<aItem::TUselessItem*>(Item)->GetOnUseCodeText();
            } else if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                Text = pas::checked_cast<aItem::TArtefact*>(Item)->GetOnUseCodeText();
            }
        }
        if (Text != u"" && Binding != nullptr && Binding->Script != nullptr) {
            Script = Binding->Script;
            CurrentScript = Script;
            Script->CurrentShip = Ship;
            Script->InitCode->LocalVar->GetVar(u"CurShip"_wref.get())->SetDword(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Ship)));
            ParentCode = Script->InitCode;
        }
        if (Text != u"") {
            pas::list_add(Globals::ScriptActionParamStack, nullptr);
            pas::list_add(Globals::ScriptActionShipStack, reinterpret_cast<void*>(Ship));
            if (ParentCode != nullptr) {
                aScript::ExecuteScriptText(Text, ParentCode->LocalVar);
            } else {
                aScript::ExecuteScriptText(Text, nullptr);
            }
            Result = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1)));
            pas::list_delete(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1);
            pas::list_delete(Globals::ScriptActionShipStack, pas::list_count(Globals::ScriptActionShipStack) - 1);
        }
        CurrentScript = SavedScript;
        Globals::ScriptUseItem = nullptr;
        return Result;
    }

    // Uses artifact or useless-item configuration code. Object slots can carry event-specific integers.
    std::int32_t RunItemConfigActionCode(aItem::TItem* Item, std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param) {
        TScriptItem* Binding{};
        EC_Expression::TCodeEC* ParentCode{};
        TScriptContextSnapshot Snapshot{};
        std::int32_t Result = Param;
        TScriptCacheUnit* Entry = nullptr;
        if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
            Entry = static_cast<TScriptCacheUnit*>(pas::checked_cast<aItem::TArtefact*>(Item)->GetActionCode());
        }
        if (pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
            Entry = static_cast<TScriptCacheUnit*>(pas::checked_cast<aItem::TUselessItem*>(Item)->GetActionCode());
        }
        if (Entry != nullptr) {
            if (pas::contains(Entry->ActionTypeMask, ActionType)) {
                if (ActionType != aConst::satOnStep || pas::contains(Entry->StepTypeMask, Param)) {
                    Binding = reinterpret_cast<TScriptItem*>(Item->ScriptItem);
                    ParentCode = nullptr;
                    try {
                        pas::list_add(Globals::ScriptItemContextStack, reinterpret_cast<void*>(Item));
                        pas::list_add(Globals::ScriptItemInfoContextStack, nullptr);
                        pas::list_add(Globals::ScriptActionTypeStack, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(ActionType))));
                        pas::list_add(Globals::ScriptActionObject1Stack, reinterpret_cast<void*>(Object1));
                        pas::list_add(Globals::ScriptActionObject2Stack, reinterpret_cast<void*>(Object2));
                        pas::list_add(Globals::ScriptActionParamStack, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Param))));
                        pas::list_add(Globals::ScriptActionShipStack, reinterpret_cast<void*>(Ship));
                        aScript::ScriptSnap(Snapshot);
                        if (Binding != nullptr && Binding->Script != nullptr) {
                            Binding->Script->PublishCurrentShip(Ship);
                            ParentCode = CurrentScript->InitCode;
                        }
                        aScript::RunScriptCode(Entry->SourceText, Entry->Code, ParentCode);
                        aScript::ScriptUnSnap(Snapshot);
                        Result = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1)));
                        pas::list_delete(Globals::ScriptActionTypeStack, pas::list_count(Globals::ScriptActionTypeStack) - 1);
                        pas::list_delete(Globals::ScriptActionObject1Stack, pas::list_count(Globals::ScriptActionObject1Stack) - 1);
                        pas::list_delete(Globals::ScriptActionObject2Stack, pas::list_count(Globals::ScriptActionObject2Stack) - 1);
                        pas::list_delete(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1);
                        pas::list_delete(Globals::ScriptActionShipStack, pas::list_count(Globals::ScriptActionShipStack) - 1);
                        pas::list_delete(Globals::ScriptItemInfoContextStack, pas::list_count(Globals::ScriptItemInfoContextStack) - 1);
                        pas::list_delete(Globals::ScriptItemContextStack, pas::list_count(Globals::ScriptItemContextStack) - 1);
                    } catch (...) {
                        auto cpp_exception = pas::caught_object();
                        if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                            GR_Main::AppendLogLineThreadSafe("Error in item actcode, item="_a);
                            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(Item->GetDisplayName()));
                            if (Item->ScriptItem != nullptr) {
                                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"script name - ", reinterpret_cast<TScriptItem*>(Item->ScriptItem)->Name})));
                            }
                            throw;
                        } else {
                            throw;
                        }
                    }
                }
            }
        }
        return Result;
    }

    // Returns the event parameter after script changes. Object slots can carry event-specific integers.
    std::int32_t RunCustomShipInfoActionCode(aShip::PCustomShipInfo Info, std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param) {
        EC_BlockPar::TBlockParEC* Config{};
        TScriptContextSnapshot Snapshot{};
        std::int32_t Result = Param;
        TScriptCacheUnit* Entry = static_cast<TScriptCacheUnit*>(Info->ActionCode);
        if (!Info->ActionCodeInitialized) {
            Info->ActionCodeInitialized = true;
            Config = GR_Main::LanguageDataConfig->GetBlock(u"ShipInfo"_wref.get())->GetBlock(u"AddInfo"_wref.get())->GetBlock(u"CustomInfos"_wref.get())->GetBlock(Info->TypeName);
            try {
                Entry = aScript::GetCachedActionCode(CustomShipInfoScriptCache, Info->TypeName, Config);
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in actcode, info=", Info->TypeName})));
                    aScript::LogScriptCallHistory();
                    throw;
                } else {
                    throw;
                }
            }
            Info->ActionCode = Entry;
        }
        if (Entry != nullptr) {
            if (pas::contains(Entry->ActionTypeMask, ActionType)) {
                if (ActionType != aConst::satOnStep || pas::contains(Entry->StepTypeMask, Param)) {
                    pas::list_add(Globals::ScriptItemContextStack, nullptr);
                    pas::list_add(Globals::ScriptItemInfoContextStack, static_cast<void*>(Info));
                    pas::list_add(Globals::ScriptActionTypeStack, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(ActionType))));
                    pas::list_add(Globals::ScriptActionObject1Stack, reinterpret_cast<void*>(Object1));
                    pas::list_add(Globals::ScriptActionObject2Stack, reinterpret_cast<void*>(Object2));
                    pas::list_add(Globals::ScriptActionParamStack, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Param))));
                    pas::list_add(Globals::ScriptActionShipStack, reinterpret_cast<void*>(Ship));
                    try {
                        aScript::ScriptSnap(Snapshot);
                        aScript::RunScriptCode(Entry->SourceText, Entry->Code, nullptr);
                        aScript::ScriptUnSnap(Snapshot);
                    } catch (...) {
                        auto cpp_exception_2 = pas::caught_object();
                        if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception_2)) {
                            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in actcode, info=", Info->TypeName})));
                            aScript::LogScriptCallHistory();
                            throw;
                        } else {
                            throw;
                        }
                    }
                    Result = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1)));
                    pas::list_delete(Globals::ScriptActionTypeStack, pas::list_count(Globals::ScriptActionTypeStack) - 1);
                    pas::list_delete(Globals::ScriptActionObject1Stack, pas::list_count(Globals::ScriptActionObject1Stack) - 1);
                    pas::list_delete(Globals::ScriptActionObject2Stack, pas::list_count(Globals::ScriptActionObject2Stack) - 1);
                    pas::list_delete(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1);
                    pas::list_delete(Globals::ScriptActionShipStack, pas::list_count(Globals::ScriptActionShipStack) - 1);
                    pas::list_delete(Globals::ScriptItemInfoContextStack, pas::list_count(Globals::ScriptItemInfoContextStack) - 1);
                    pas::list_delete(Globals::ScriptItemContextStack, pas::list_count(Globals::ScriptItemContextStack) - 1);
                }
            }
        }
        return Result;
    }

    pas::WideString GetScriptContextDescription() {
        pas::WideString Result{};
        aShip::PCustomShipInfo Info{};
        aItem::TItem* Item{};
        TScriptItem* Binding{};
        if (pas::list_count(Globals::ScriptItemContextStack) > 0) {
            Info = pas::list_at<aShip::TCustomShipInfo>(Globals::ScriptItemInfoContextStack, pas::list_count(Globals::ScriptItemInfoContextStack) - 1);
            if (Info != nullptr) {
                return Info->TypeName;
            }
            Item = pas::list_at<aItem::TItem>(Globals::ScriptItemContextStack, pas::list_count(Globals::ScriptItemInfoContextStack) - 1);
            if (Item != nullptr) {
                Binding = reinterpret_cast<TScriptItem*>(Item->ScriptItem);
                if (Binding != nullptr) {
                    if (Binding->Script != nullptr) {
                        return pas::concat_wide({Item->GetDisplayName(), u" (", Binding->Name, u", ", Binding->Script->ScriptFileName, u")"});
                    }
                    return pas::concat_wide({Item->GetDisplayName(), u" (", Binding->Name, u", unknown script)"});
                }
                return Item->GetDisplayName();
            }
            Result = u"unknown actcode"_w;
        }
        if (CurrentScript != nullptr) {
            if (CurrentScriptState != nullptr) {
                return pas::concat_wide({CurrentScriptState->Name, u" (", CurrentScript->ScriptFileName, u")"});
            }
            return CurrentScript->ScriptFileName;
        } else if (Globals::ScriptUseItem != nullptr) {
            Binding = reinterpret_cast<TScriptItem*>(Globals::ScriptUseItem->ScriptItem);
            if (Binding != nullptr) {
                if (Binding->Script != nullptr) {
                    return pas::concat_wide({Globals::ScriptUseItem->GetDisplayName(), u" (", Binding->Name, u", ", Binding->Script->ScriptFileName, u")"});
                }
                return pas::concat_wide({Globals::ScriptUseItem->GetDisplayName(), u" (", Binding->Name, u", unknown script)"});
            }
            return Globals::ScriptUseItem->GetDisplayName();
        } else {
            return u"unknown source"_w;
        }
    }

    void TScriptThread_Create(TScriptThread* Self) {
        EC_Thread::TThreadEC_Create(Self);
    }

    void TScriptThread_Execute(TScriptThread* Self) {
        if (Globals::TurnCalculationThread != nullptr && Globals::TurnCalculationThread->IsRunning()) {
            Globals::TurnCalculationThread->WaitForIdle(WindowsSdk::INFINITE);
        }
        if (aScript::HasPendingScriptRequests()) {
            if (GlobalsV::CurrentScreenId == GlobalsV::screenShip) {
                GlobalsV::ShipReturnScreenId = GlobalsV::screenStarMap;
                reinterpret_cast<fShip2::TfShip2*>(reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]))->CloseClicked(nullptr);
                return;
            }
            if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
            } else if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                    GlobalsV::RequestedScreenId = GlobalsV::screenPlanetNO;
                } else {
                    GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
                }
            } else {
                GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
            }
            reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
        }
    }

    void TScriptStar_Create(TScriptStar* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Constraints = nullptr;
        Self->Planets = nullptr;
        Self->ShipRequirements = nullptr;
    }

    void TScriptStar_Destroy(TScriptStar* Self) {
        Self->ShipRequirements = nullptr;
        Self->Planets = nullptr;
        Self->Constraints = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TScriptConstellation_Create(TScriptConstellation* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TScriptConstellation_Destroy(TScriptConstellation* Self) {
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TScriptShip_Create(TScriptShip* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TScriptShip_Destroy(TScriptShip* Self) {
        if (Self->Ship != nullptr) {
            Self->Ship->ScriptShip = nullptr;
            Self->Ship = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    TScriptGroup* TScriptShip::GetGroup() {
        return pas::list_at<TScriptGroup>(Script->Groups, GroupIndex);
    }

    void TScriptPlace_Create(TScriptPlace* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TScriptPlace_Destroy(TScriptPlace* Self) {
        EC_Struct::TObjectEx_Destroy(Self);
    }

    EC_Struct::TPointF TScriptPlace::GetPoint() {
        EC_Struct::TPointF Result{};
        float Distance{};
        float Angle{};
        std::int32_t I{};
        std::int32_t Count{};
        TScriptShip* Binding{};
        EC_Struct::TPointF Center{};
        if (PlaceKind == spkPolar) {
            Angle = aMyFunction::HeadingDegreesToRadians(AngleOffset);
            Distance = pas::real_divide(OriginStar->MapDiameter, 2.0L) * DistanceScale;
            Result.X = System::Sin(Angle) * Distance;
            Result.Y = System::Cos(Angle) * -Distance;
        } else if (PlaceKind == spkPlanetPosition) {
            Result = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(TargetValue)))->GetPosition();
        } else if (PlaceKind == spkDockedPlanet) {
            Result = EC_Struct::MakePointF(0.0f, 0.0f);
        } else if (PlaceKind == spkStarDirection) {
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(static_cast<long double>(aMyFunction::PointBearingDegrees(OriginStar->Position, reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(TargetValue)))->Position)) + AngleOffset));
            Distance = pas::real_divide(OriginStar->MapDiameter, 2.0L) * DistanceScale;
            Result.X = System::Sin(Angle) * Distance;
            Result.Y = System::Cos(Angle) * -Distance;
        } else if (PlaceKind == spkScriptItem) {
            if (reinterpret_cast<TScriptItem*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(TargetValue)))->Item == nullptr) {
                Result = EC_Struct::MakePointF(0.0f, 0.0f);
            } else {
                Result = reinterpret_cast<TScriptItem*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(TargetValue)))->Item->Position;
            }
        } else if (PlaceKind == spkGroupCentroid) {
            Count = 0;
            Center = EC_Struct::MakePointF(0.0f, 0.0f);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Script->Ships) - 1); cpp_range.next(I); ) {
                Binding = pas::list_at<TScriptShip>(Script->Ships, I);
                if (Binding->GroupIndex == static_cast<std::int32_t>(TargetValue)) {
                    Center = EC_Struct::AddPointsF(Center, Binding->Ship->Position);
                    ++Count;
                }
            }
            if (Count < 1) {
                Result = EC_Struct::MakePointF(0.0f, 0.0f);
            } else {
                Center.X = pas::real_divide(Center.X, Count);
                Center.Y = pas::real_divide(Center.Y, Count);
                Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(static_cast<long double>(aMyFunction::PointBearingDegrees(Center, EC_Struct::MakePointF(0.0f, 0.0f))) + AngleOffset));
                Distance = pas::real_divide(OriginStar->MapDiameter, 2.0L) * DistanceScale;
                Result.X = System::Sin(Angle) * Distance + Center.X;
                Result.Y = Center.Y - System::Cos(Angle) * Distance;
            }
        } else if (PlaceKind == spkCoordinates) {
            {
                float pasfloat = reinterpret_cast<EC_Expression::TVarEC*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(TargetValue)))->GetFloat();
                float pasfloat_2 = TargetValue2->GetFloat();
                Center = EC_Struct::MakePointF(pasfloat, pasfloat_2);
            }
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(static_cast<long double>(aMyFunction::PointBearingDegrees(Center, EC_Struct::MakePointF(0.0f, 0.0f))) + AngleOffset));
            Distance = pas::real_divide(OriginStar->MapDiameter, 2.0L) * DistanceScale;
            Result.X = System::Sin(Angle) * Distance + Center.X;
            Result.Y = Center.Y - System::Cos(Angle) * Distance;
        }
        if (pas::is_one_of<spkScriptItem, spkGroupCentroid>(PlaceKind)) {
            if (static_cast<long double>(Result.X) * Result.X + static_cast<long double>(Result.Y) * Result.Y < 1.0E-4L) {
                Angle = aMyFunction::HeadingDegreesToRadians(0.0);
                Result.X = System::Sin(Angle) * (OriginStar->SafeRadius * 1.5L);
                Result.Y = System::Cos(Angle) * (OriginStar->SafeRadius * 1.5L);
            } else {
                while (static_cast<long double>(Result.X) * Result.X + static_cast<long double>(Result.Y) * Result.Y < 2.0L * OriginStar->SafeRadius * OriginStar->SafeRadius) {
                    Result.X = Result.X * 1.05L;
                    Result.Y = Result.Y * 1.05L;
                }
            }
        }
        return Result;
    }

    EC_Struct::TPointF TScriptPlace::GetRandomPoint(std::uint32_t Seed) {
        EC_Struct::TPointF Result{};
        float Angle{};
        Result = GetPoint();
        if (Radius != 0) {
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 360, Seed));
            Result.X = Result.X + System::Sin(Angle) * (Radius * 0.9L);
            Result.Y = Result.Y + -System::Cos(Angle) * (Radius * 0.9L);
            if (pas::is_one_of<spkScriptItem, spkGroupCentroid>(PlaceKind) && static_cast<long double>(Result.X) * Result.X + static_cast<long double>(Result.Y) * Result.Y < 2.0L * OriginStar->SafeRadius * OriginStar->SafeRadius) {
                Result.X = Result.X - System::Sin(Angle) * (Radius * 1.8L);
                Result.Y = Result.Y - -System::Cos(Angle) * (Radius * 1.8L);
            }
        }
        return Result;
    }

    // Kind 2 requires docking at the bound planet; other kinds require normal space.
    std::uint8_t TScriptPlace::ShipInPlace(aShip::TShip* Ship) {
        if (PlaceKind == spkDockedPlanet) {
            return Ship->CurrentPlanet == reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(TargetValue)));
        } else if (!Ship->InNormalSpace()) {
            return false;
        } else {
            return static_cast<long double>(aMyFunction::PointDistanceSquared(GetPoint(), Ship->Position)) <= pas::sqr(Radius);
        }
    }

    void TScriptItem_Create(TScriptItem* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->ActionCode = nullptr;
        Self->ActionCodeInitialized = false;
        Self->Script = nullptr;
    }

    void TScriptItem_Destroy(TScriptItem* Self) {
        if (Self->Item != nullptr) {
            Self->Item->ScriptItem = nullptr;
            Self->Item = nullptr;
        }
        if (Self->ActionCode != nullptr) {
            pas::free(Self->ActionCode);
        }
        Self->ActionCode = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TScriptGroup_Create(TScriptGroup* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TScriptGroup_Destroy(TScriptGroup* Self) {
        if (Self->Ships != nullptr) {
            pas::free(Self->Ships);
            Self->Ships = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // EntryCode runs before CurShip/EndState refresh; StateCode sees the new context.
    void TScriptState_Create(TScriptState* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->StateCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
        Self->ActionCode = nullptr;
    }

    void TScriptState_Destroy(TScriptState* Self) {
        Self->EnemyGroupNames = nullptr;
        Self->EnemyGroupIndices = nullptr;
        if (Self->StateCode != nullptr) {
            pas::free(Self->StateCode);
            Self->StateCode = nullptr;
        }
        if (Self->AuxiliaryCode != nullptr) {
            pas::free(Self->AuxiliaryCode);
            Self->AuxiliaryCode = nullptr;
        }
        if (Self->ActionCode != nullptr) {
            pas::free(Self->ActionCode);
            Self->ActionCode = nullptr;
        }
        if (Self->EntryCode != nullptr) {
            pas::free(Self->EntryCode);
            Self->EntryCode = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TScriptDialog_Create(TScriptDialog* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Code = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
    }

    void TScriptDialog_Destroy(TScriptDialog* Self) {
        if (Self->Code != nullptr) {
            pas::free(Self->Code);
            Self->Code = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TScriptDialogMsg_Create(TScriptDialogMsg* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Code = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
    }

    void TScriptDialogMsg_Destroy(TScriptDialogMsg* Self) {
        if (Self->Code != nullptr) {
            pas::free(Self->Code);
            Self->Code = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TScriptDialogAnswer_Create(TScriptDialogAnswer* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->AnswerCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
        Self->ActionCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
    }

    void TScriptDialogAnswer_Destroy(TScriptDialogAnswer* Self) {
        if (Self->AnswerCode != nullptr) {
            pas::free(Self->AnswerCode);
            Self->AnswerCode = nullptr;
        }
        if (Self->ActionCode != nullptr) {
            pas::free(Self->ActionCode);
            Self->ActionCode = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TScript_Create(TScript* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->InitCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
        Self->TurnCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
        Self->AuxiliaryCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
        Self->Constellations = pas::make_object<pas::List>();
        Self->Stars = pas::make_object<pas::List>();
        Self->Places = pas::make_object<pas::List>();
        Self->Items = pas::make_object<pas::List>();
        Self->Groups = pas::make_object<pas::List>();
        Self->Ships = pas::make_object<pas::List>();
        Self->States = pas::make_object<pas::List>();
        Self->Dialogs = pas::make_object<pas::List>();
        Self->DialogMessages = pas::make_object<pas::List>();
        Self->DialogAnswers = pas::make_object<pas::List>();
        Self->Ether = pas::construct_call<EC_Ether::TEther>(EC_Ether::TEther_Create);
        Self->EtherIds = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
        Self->CurrentAnswer = -1;
    }

    void TScript_Destroy(TScript* Self) {
        if (CurrentScript == Self) {
            CurrentScript = nullptr;
        }
        Self->Clear();
        if (Self->Constellations != nullptr) {
            pas::free(Self->Constellations);
            Self->Constellations = nullptr;
        }
        if (Self->Groups != nullptr) {
            pas::free(Self->Groups);
            Self->Groups = nullptr;
        }
        if (Self->Items != nullptr) {
            pas::free(Self->Items);
            Self->Items = nullptr;
        }
        if (Self->Places != nullptr) {
            pas::free(Self->Places);
            Self->Places = nullptr;
        }
        if (Self->Stars != nullptr) {
            pas::free(Self->Stars);
            Self->Stars = nullptr;
        }
        if (Self->States != nullptr) {
            pas::free(Self->States);
            Self->States = nullptr;
        }
        if (Self->Ships != nullptr) {
            pas::free(Self->Ships);
            Self->Ships = nullptr;
        }
        if (Self->Dialogs != nullptr) {
            pas::free(Self->Dialogs);
            Self->Dialogs = nullptr;
        }
        if (Self->DialogMessages != nullptr) {
            pas::free(Self->DialogMessages);
            Self->DialogMessages = nullptr;
        }
        if (Self->DialogAnswers != nullptr) {
            pas::free(Self->DialogAnswers);
            Self->DialogAnswers = nullptr;
        }
        if (Self->TurnCode != nullptr) {
            pas::free(Self->TurnCode);
            Self->TurnCode = nullptr;
        }
        if (Self->AuxiliaryCode != nullptr) {
            pas::free(Self->AuxiliaryCode);
            Self->AuxiliaryCode = nullptr;
        }
        if (Self->InitCode != nullptr) {
            pas::free(Self->InitCode);
            Self->InitCode = nullptr;
        }
        if (Self->Ether != nullptr) {
            pas::free(Self->Ether);
            Self->Ether = nullptr;
        }
        if (Self->EtherIds != nullptr) {
            pas::free(Self->EtherIds);
            Self->EtherIds = nullptr;
        }
        Self->GroupRelations = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Frees owned entries but retains list and code containers.
    void TScript::Clear() {
        std::int32_t I{};
        std::int32_t StateCount{};
        TScriptState* State{};
        TScriptShip* Binding{};
        TScriptDialog* Dialog{};
        if (States != nullptr) {
            StateCount = pas::list_count(States);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, StateCount - 1); cpp_range.next(I); ) {
                State = pas::list_at<TScriptState>(States, I);
                pas::free(State);
            }
            pas::list_clear(States);
        }
        if (Groups != nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Groups) - 1); cpp_range_2.next(I); ) {
                pas::free(pas::list_at<pas::Object>(Groups, I));
            }
            pas::list_clear(Groups);
        }
        if (Items != nullptr) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Items) - 1); cpp_range_3.next(I); ) {
                pas::free(pas::list_at<pas::Object>(Items, I));
            }
            pas::list_clear(Items);
        }
        if (Places != nullptr) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Places) - 1); cpp_range_4.next(I); ) {
                pas::free(pas::list_at<pas::Object>(Places, I));
            }
            pas::list_clear(Places);
        }
        if (Stars != nullptr) {
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_5.next(I); ) {
                pas::free(pas::list_at<pas::Object>(Stars, I));
            }
            pas::list_clear(Stars);
        }
        if (Constellations != nullptr) {
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_6.next(I); ) {
                pas::free(pas::list_at<pas::Object>(Constellations, I));
            }
            pas::list_clear(Constellations);
        }
        if (Ships != nullptr) {
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_7.next(I); ) {
                Binding = pas::list_at<TScriptShip>(Ships, I);
                pas::free(Binding);
            }
            pas::list_clear(Ships);
        }
        if (Dialogs != nullptr) {
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Dialogs) - 1); cpp_range_8.next(I); ) {
                Dialog = pas::list_at<TScriptDialog>(Dialogs, I);
                pas::free(Dialog);
            }
            pas::list_clear(Dialogs);
        }
        if (DialogMessages != nullptr) {
            for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(DialogMessages) - 1); cpp_range_9.next(I); ) {
                pas::free(pas::list_at<pas::Object>(DialogMessages, I));
            }
            pas::list_clear(DialogMessages);
        }
        if (DialogAnswers != nullptr) {
            for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, pas::list_count(DialogAnswers) - 1); cpp_range_10.next(I); ) {
                pas::free(pas::list_at<pas::Object>(DialogAnswers, I));
            }
            pas::list_clear(DialogAnswers);
        }
        if (Ether != nullptr) {
            Ether->Clear();
        }
        if (InitCode != nullptr) {
            InitCode->Clear();
        }
        if (TurnCode != nullptr) {
            TurnCode->Clear();
        }
        if (AuxiliaryCode != nullptr) {
            AuxiliaryCode->Clear();
        }
        if (EtherIds != nullptr) {
            EtherIds->Clear();
        }
        GroupRelations = nullptr;
    }

    // Changes the global CurrentScript context.
    void TScript::PublishShipContext(TScriptShip* Binding) {
        CurrentShip = Binding->Ship;
        CurrentScript = this;
        {
            EC_Expression::TVarEC* var = InitCode->LocalVar->GetVar(u"EndState"_wref.get());
            std::int32_t cpp_arg = Binding->EndState;
            var->SetInt(cpp_arg);
        }
        {
            EC_Expression::TVarEC* var_2 = InitCode->LocalVar->GetVar(u"CurShip"_wref.get());
            std::uint32_t currentShip = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(CurrentShip));
            var_2->SetDword(currentShip);
        }
    }

    void TScript::PublishCurrentShip(aShip::TShip* Ship) {
        CurrentShip = Ship;
        CurrentScript = this;
        {
            EC_Expression::TVarEC* var = InitCode->LocalVar->GetVar(u"CurShip"_wref.get());
            std::uint32_t currentShip = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(CurrentShip));
            var->SetDword(currentShip);
        }
    }

    // Raises when absent.
    TScriptStar* TScript::GetStar(pas::WideString Name) {
        std::int32_t I{};
        TScriptStar* Star{};
        std::int32_t Count = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TScriptStar>(Stars, I);
            if (Name == Star->Name) {
                return Star;
            }
        }
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error.Script. Not found star =", Name}))));
    }

    // Raises when absent.
    PScriptPlanetBinding TScript::GetPlanetBinding(pas::WideString Name) {
        std::int32_t I{};
        std::int32_t J{};
        TScriptStar* Star{};
        std::int32_t Count = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TScriptStar>(Stars, I);
            if (Star->Planets != nullptr) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Star->Planets.length() - 1); cpp_range_2.next(J); ) {
                    if (Name == Star->Planets[J].Name) {
                        return &Star->Planets[J];
                    }
                }
            }
        }
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error.Script. Not found planet =", Name}))));
    }

    // Raises when absent.
    TScriptItem* TScript::GetItem(pas::WideString Name) {
        TScriptItem* Item{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Items) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<TScriptItem>(Items, I);
            if (Item->Name == Name) {
                return Item;
            }
        }
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error.Script. Not found item =", Name}))));
    }

    void TScript::RunShipState(TScriptShip* Binding) {
        TScript* SavedScript{};
        TScriptState* SavedState{};
        TScriptState* State = Binding->State;
        try {
            if (State != nullptr && State->StateCode != nullptr) {
                SavedScript = CurrentScript;
                CurrentScript = this;
                PublishShipContext(Binding);
                SavedState = CurrentScriptState;
                CurrentScriptState = State;
                State->StateCode->Run(ScriptProcess);
                CurrentScriptState = SavedState;
                CurrentScript = SavedScript;
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in state code of script ", ScriptFileName, u" state #", pas::wide_int_to_str(pas::list_indexof(States, reinterpret_cast<void*>(State))), u"(", State->Name, u")"}))));
            } else {
                throw;
            }
        }
    }

    void TScript::RunTurnCode() {
        try {
            CurrentScript = this;
            TurnCode->Run(ScriptProcess);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in turn code of script ", ScriptFileName}))));
            } else {
                throw;
            }
        }
    }

    void TScript::RunAuxiliaryCode() {
        try {
            CurrentScript = this;
            AuxiliaryCode->Run(ScriptProcess);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in dialog code of script ", ScriptFileName}))));
            } else {
                throw;
            }
        }
    }

    void TScript::CallDialog(std::int32_t Index) {
        if (Index < 0 || Index >= pas::list_count(Dialogs)) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error.Script.CallDialog ", EC_Str::IntToWideString(Index), u" ", ScriptFileName}))));
        }
        CurrentScript = this;
        CurrentDialog = Index;
        SkipGreeting = false;
        Globals::ScriptDialogIndex = -1;
        try {
            pas::list_at<TScriptDialog>(Dialogs, Index)->Code->Run(ScriptProcess);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in call dialog code of script ", ScriptFileName, u" (", pas::list_at<TScriptDialog>(Dialogs, Index)->Name, u")"}))));
            } else {
                throw;
            }
        }
    }

    void TScript::CallDialogByVariable(pas::WideString Name) {
        EC_Expression::TVarEC* Cell = InitCode->LocalVar->GetVarNE(Name);
        if (Cell == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Dialog ", Name, u" not found in script ", ScriptFileName}))));
        }
        CallDialog(Cell->GetInt());
    }

    void TScript::CallDialogMessage(std::int32_t Index) {
        if (Index < 0 || Index >= pas::list_count(DialogMessages)) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error.Script.CallDialogMsg ", ScriptFileName}))));
        }
        try {
            pas::list_at<TScriptDialogMsg>(DialogMessages, Index)->Code->Run(ScriptProcess);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in call dialog message code of script ", ScriptFileName, u" (message ", EC_Str::IntToWideString(Index), u")"}))));
            } else {
                throw;
            }
        }
    }

    void TScript::BuildDialogAnswer(std::int32_t Index) {
        if (Index < 0 || Index >= pas::list_count(DialogAnswers)) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error.Script.CallDialogAnswerAnswer ", ScriptFileName}))));
        }
        CurrentAnswer = Index;
        try {
            pas::list_at<TScriptDialogAnswer>(DialogAnswers, Index)->AnswerCode->Run(ScriptProcess);
            CurrentAnswer = -1;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in call answer code of script ", ScriptFileName, u" (answer ", EC_Str::IntToWideString(Index), u")"}))));
            } else {
                throw;
            }
        }
    }

    // Index -1 is ignored.
    void TScript::ExecuteDialogAnswer(std::int32_t Index) {
        if (Index == -1) {
            return;
        }
        if (Index < 0 || Index >= pas::list_count(DialogAnswers)) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error.Script.CallDialogAnswerCode ", ScriptFileName}))));
        }
        try {
            pas::list_at<TScriptDialogAnswer>(DialogAnswers, Index)->ActionCode->Run(ScriptProcess);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
            } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                aScript::LogScriptCallHistory();
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in add answer code of script ", ScriptFileName, u" (answer ", EC_Str::IntToWideString(Index), u")"}))));
            } else {
                throw;
            }
        }
    }

    // The player may have multiple script bindings; ordinary ships have one.
    void TScript::BindShip(std::int32_t GroupIndex, aShip::TShip* Ship) {
        TScriptShip* Binding = pas::construct_call<TScriptShip>(TScriptShip_Create);
        pas::list_add(Ships, reinterpret_cast<void*>(Binding));
        Binding->GroupIndex = GroupIndex;
        Binding->Script = this;
        Binding->Ship = Ship;
        if (pas::class_cast_if<aPlayer::TPlayer*>(Ship) != nullptr) {
            pas::list_add(reinterpret_cast<aPlayer::TPlayer*>(Ship)->ScriptShipBindings, reinterpret_cast<void*>(Binding));
        } else {
            Ship->ScriptShip = Binding;
        }
        if (pas::class_cast_if<aWarrior::TWarrior*>(Ship) != nullptr && pas::list_indexof(Ship->CurrentStar->Ships, reinterpret_cast<void*>(Ship)) == -1) {
            pas::list_add(Ship->CurrentStar->Ships, reinterpret_cast<void*>(Ship));
        }
    }

    void TScript::UnbindShip(aShip::TShip* Ship) {
        std::int32_t I{};
        std::int32_t Index{};
        TScriptShip* Binding{};
        if (pas::class_cast_if<aPlayer::TPlayer*>(Ship) != nullptr) {
            I = 0;
            while (I < pas::list_count(reinterpret_cast<aPlayer::TPlayer*>(Ship)->ScriptShipBindings)) {
                Index = pas::list_indexof(Ships, pas::list_get(reinterpret_cast<aPlayer::TPlayer*>(Ship)->ScriptShipBindings, I));
                if (Index >= 0) {
                    Binding = pas::list_at<TScriptShip>(Ships, Index);
                    pas::free(Binding);
                    pas::list_delete(Ships, Index);
                    pas::list_delete(reinterpret_cast<aPlayer::TPlayer*>(Ship)->ScriptShipBindings, I);
                } else {
                    ++I;
                }
            }
        } else {
            Index = pas::list_indexof(Ships, reinterpret_cast<void*>(Ship->ScriptShip));
            if (Index >= 0) {
                Binding = pas::list_at<TScriptShip>(Ships, Index);
                pas::free(Binding);
                pas::list_delete(Ships, Index);
            }
            Ship->ScriptShip = nullptr;
        }
    }

    void TScript::ClearShipBindings() {
        std::int32_t I{};
        std::int32_t Index{};
        TScriptShip* Binding{};
        aShip::TShip* Ship{};
        if (aPlayer::GetPlayer() != nullptr) {
            const std::int32_t cpp_first = pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Index = pas::list_indexof(Ships, pas::list_get(aPlayer::GetPlayer()->ScriptShipBindings, I));
                    if (Index >= 0) {
                        Binding = pas::list_at<TScriptShip>(Ships, Index);
                        pas::free(Binding);
                        pas::list_delete(Ships, Index);
                        pas::list_delete(aPlayer::GetPlayer()->ScriptShipBindings, I);
                    }
                }
            }
        }
        {
            const std::int32_t cpp_first_2 = pas::list_count(Ships) - 1;
            if (cpp_first_2 >= 0) {
                for (I = cpp_first_2; I >= 0; --I) {
                    Binding = pas::list_at<TScriptShip>(Ships, I);
                    Ship = Binding->Ship;
                    pas::free(Binding);
                    pas::list_delete(Ships, I);
                    Ship->ScriptShip = nullptr;
                }
            }
        }
    }

    void TScript::ChangeState(TScriptShip* Binding, std::int32_t StateIndex) {
        if (Binding == nullptr) {
            return;
        }
        Binding->State = pas::list_at<TScriptState>(States, StateIndex);
        if (Binding->State->EntryCode != nullptr) {
            Binding->State->EntryCode->Run(ScriptProcess);
        }
        if (!(pas::class_cast_if<aPlayer::TPlayer*>(Binding->Ship) != nullptr)) {
            Binding->Ship->InitializeScriptStateOrders();
        }
        if (Binding->State->StateCode != nullptr) {
            PublishShipContext(Binding);
            Binding->State->StateCode->Run(ScriptProcess);
        }
    }

    // Changes source ships' relations to ranger members of the target group.
    void TScript::SetGroupRelation(std::int32_t SourceGroup, std::int32_t TargetGroup, aGalaxyStruct::TRelationLevel Level) {
        TScriptShip* Source{};
        TScriptShip* Target{};
        std::int32_t I{};
        std::int32_t J{};
        if (SourceGroup == TargetGroup) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Source = pas::list_at<TScriptShip>(Ships, I);
            if (Source->GroupIndex == SourceGroup) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_2.next(J); ) {
                    Target = pas::list_at<TScriptShip>(Ships, J);
                    if (Target->GroupIndex == TargetGroup && pas::class_cast_if<aRanger::TRanger*>(Target->Ship) != nullptr) {
                        Source->Ship->SetStoredRangerRelationLevel(reinterpret_cast<aRanger::TRanger*>(Target->Ship), Level);
                    }
                }
            }
        }
    }

    void TScript::SetPlanetRelation(std::int32_t GroupIndex, aPlanet::TPlanet* Planet, aGalaxyStruct::TRelationLevel Level) {
        TScriptShip* Binding{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Binding = pas::list_at<TScriptShip>(Ships, I);
            if (Binding->GroupIndex == GroupIndex && pas::class_cast_if<aRanger::TRanger*>(Binding->Ship) != nullptr) {
                Planet->SetRelationLevelToRanger(reinterpret_cast<aRanger::TRanger*>(Binding->Ship), Level);
            }
        }
    }

    // Recursively assigns remaining stars and their planets; earlier star bindings must already exist.
    std::uint8_t TScript::TryBindStars(std::int32_t StarIndex) {
        TScriptStar* Binding{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t I{};
        std::int32_t Count{};
        std::int32_t DistanceSquared{};
        float Bearing{};
        pas::List* Candidates{};
        aGalaxy::THole* Hole{};
        TScriptConstellation* ConstellationBinding{};
        aGalaxy::TConstellation* Constellation{};
        float MinOrbitSquared{};
        float MaxOrbitSquared{};
        float OrbitSquared{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        PScriptShipRequirement Requirement{};
        pas::WideString Indent{};
        TScriptStarConstraint Constraint{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarIndex - 1); cpp_range.next(J); ) {
            Indent = pas::concat_wide({Indent, u"    "});
        }
        std::uint8_t Result = false;
        Binding = pas::list_at<TScriptStar>(Stars, StarIndex);
        aGalaxy::TStar* Star = Binding->Star;
        if (Binding->RejectHostilePresence && Star->HasHostilePresenceForScriptBinding()) {
            return Result;
        }
        if (StarIndex < 2) {
            Bearing = 0.0f;
        } else {
            EC_Struct::TPointF position = pas::list_at<TScriptStar>(Stars, 1)->Star->Position;
            EC_Struct::TPointF position_2 = pas::list_at<TScriptStar>(Stars, 0)->Star->Position;
            Bearing = aMyFunction::PointBearingDegrees(position_2, position);
        }
        {
            const std::int32_t cpp_last = Binding->Constraints.length() - 1;
            if (0 <= cpp_last) {
                for (J = 0; J <= cpp_last; ++J) {
                    Constraint = Binding->Constraints[J];
                    if (Constraint.RequireBlackHole) {
                        Count = pas::list_count(aGalaxy::Galaxy->Holes);
                        K = 0;
                        while (K < Count) {
                            Hole = pas::list_at<aGalaxy::THole>(aGalaxy::Galaxy->Holes, K);
                            if (Hole->Star1 == Star && Hole->Star2 == Constraint.OtherStar->Star) {
                                break;
                            }
                            if (Hole->Star1 == Constraint.OtherStar->Star && Hole->Star2 == Star) {
                                break;
                            }
                            ++K;
                        }
                        if (K >= Count) {
                            return Result;
                        }
                    }
                    if (Constraint.MinDistance > 0 || Constraint.MaxDistance < 150) {
                        DistanceSquared = System::Round(aMyFunction::PointDistanceSquared(Star->Position, Constraint.OtherStar->Star->Position));
                        if (DistanceSquared < pas::sqr(Constraint.MinDistance) || DistanceSquared > pas::sqr(Constraint.MaxDistance)) {
                            return Result;
                        }
                    }
                }
            }
        }
        if (Binding->Planets != nullptr) {
            Count = Binding->Planets.length() - 1 + 1;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(K); ) {
                if (StarIndex == 0 && K == 0 && AnchorPlanet != nullptr) {
                    Binding->Planets[0].Planet = AnchorPlanet;
                } else {
                    if (Star->Status.CustomFaction != u"") {
                        return Result;
                    }
                    MinOrbitSquared = pas::sqr(([&] {
                        pas::Extended cpp_left = pas::real_divide(Binding->Planets[K].MinOrbitPercent, 1.0E+2L);
                        return cpp_left * pas::real_divide(Star->MapDiameter, 2.0L);
                    }()));
                    MaxOrbitSquared = pas::sqr(([&] {
                        pas::Extended cpp_left_2 = pas::real_divide(Binding->Planets[K].MaxOrbitPercent, 1.0E+2L);
                        return cpp_left_2 * pas::real_divide(Star->MapDiameter, 2.0L);
                    }()));
                    Binding->Planets[K].Planet = nullptr;
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_3.next(J); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                        if (Planet->NoLanding) {
                            continue;
                        }
                        if (!pas::contains(Binding->Planets[K].RaceMask, Planet->RaceId)) {
                            continue;
                        }
                        if (!pas::contains(Binding->Planets[K].OwnerMask, Planet->OwnerId)) {
                            continue;
                        }
                        if (!pas::contains(Binding->Planets[K].EconomyMask, static_cast<std::uint8_t>(Planet->Economy))) {
                            continue;
                        }
                        if (!pas::contains(Binding->Planets[K].GovernmentMask, static_cast<std::uint8_t>(Planet->Government))) {
                            continue;
                        }
                        I = 0;
                        while (I < K) {
                            if (Binding->Planets[I].Planet == Planet) {
                                break;
                            }
                            ++I;
                        }
                        if (I < K) {
                            continue;
                        }
                        OrbitSquared = aMyFunction::PointDistanceSquared(Planet->GetPosition(), EC_Struct::MakePointF(0.0f, 0.0f));
                        if (OrbitSquared < MinOrbitSquared) {
                            continue;
                        }
                        // Native search stops at the first eligible orbit beyond the upper bound.
                        if (OrbitSquared > MaxOrbitSquared) {
                            break;
                        }
                        Binding->Planets[K].Planet = Planet;
                        break;
                    }
                    if (Binding->Planets[K].Planet == nullptr) {
                        return Result;
                    }
                }
            }
        }
        if (Binding->ShipRequirements != nullptr) {
            Candidates = aScript::CollectScriptCandidateShips(Star);
            Count = Binding->ShipRequirements.length() - 1 + 1;
            // The original early rejection leaves Candidates allocated here.
            if (pas::list_count(Candidates) < Count) {
                return Result;
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(K); ) {
                Requirement = &Binding->ShipRequirements[K];
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Requirement->Count - 1); cpp_range_5.next(J); ) {
                    I = 0;
                    while (I < pas::list_count(Candidates)) {
                        Ship = pas::list_at<aShip::TShip>(Candidates, I);
                        ++I;
                        if (Requirement->PlayerOnly && aPlayer::GetPlayer() != Ship) {
                            continue;
                        }
                        if (Ship->LiberationGroup != nullptr) {
                            continue;
                        }
                        if (!pas::contains(Requirement->OwnerMask, Ship->OwnerId)) {
                            continue;
                        }
                        if (!aScript::ScriptShipMatchesType(Ship, Requirement->ShipTypeMask, Requirement->StationNames, pas::open_array(pas::load_unaligned<TScriptDominatorMasks>(&Requirement->DominatorMasks)))) {
                            continue;
                        }
                        if (Ship->Speed < Requirement->MinSpeed) {
                            continue;
                        }
                        if (Ship->Speed > Requirement->MaxSpeed) {
                            continue;
                        }
                        if (Requirement->WeaponRequirement == 1 && Ship->WeaponCount <= 0) {
                            continue;
                        }
                        if (Requirement->WeaponRequirement == 2 && Ship->WeaponCount > 0) {
                            continue;
                        }
                        if (Requirement->MinCargoHookLevel > 0) {
                            if (Ship->GetCargoHook() == nullptr) {
                                continue;
                            }
                            if (Ship->GetCargoHook()->GetLevel() < Requirement->MinCargoHookLevel) {
                                continue;
                            }
                        }
                        if (Ship->CargoFreeSpace < Requirement->MinFreeCargoSpace && aPlayer::GetPlayer() != Ship) {
                            continue;
                        }
                        if (Ship->CargoFreeSpace < Requirement->MinFreeCargoSpace && Requirement->MinFreeCargoSpace > 0) {
                            continue;
                        }
                        if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
                            if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader] < Requirement->MinTraderStatus) {
                                continue;
                            }
                            if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader] > Requirement->MaxTraderStatus) {
                                continue;
                            }
                            if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior] < Requirement->MinWarriorStatus) {
                                continue;
                            }
                            if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior] > Requirement->MaxWarriorStatus) {
                                continue;
                            }
                            if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate] < Requirement->MinPirateStatus) {
                                continue;
                            }
                            if (reinterpret_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate] > Requirement->MaxPirateStatus) {
                                continue;
                            }
                        }
                        if (Requirement->MinStrength != 0.0L || Requirement->MaxStrength != 0.0L) {
                            if (Ship->StrengthInBestRanger < Requirement->MinStrength) {
                                continue;
                            }
                            if (Ship->StrengthInBestRanger > Requirement->MaxStrength) {
                                continue;
                            }
                        }
                        --I;
                        break;
                    }
                    if (I >= pas::list_count(Candidates)) {
                        pas::free(Candidates);
                        return Result;
                    }
                    pas::list_delete(Candidates, I);
                }
            }
            pas::free(Candidates);
        }
        ++StarIndex;
        if (StarIndex >= pas::list_count(Stars)) {
            return true;
        }
        Binding = pas::list_at<TScriptStar>(Stars, StarIndex);
        if (Binding->ConstellationIndex == -1) {
            Count = pas::list_count(aGalaxy::Galaxy->Stars);
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(K); ) {
                Star = pas::list_at<TScriptStar>(Stars, 0)->Star->StarDistances[K].Star;
                if (Binding->RejectHostilePresence && Star->HasHostilePresenceForScriptBinding()) {
                    continue;
                }
                J = 0;
                while (J < StarIndex) {
                    if (pas::list_at<TScriptStar>(Stars, J)->Star == Star) {
                        break;
                    }
                    ++J;
                }
                if (J < StarIndex) {
                    continue;
                }
                Binding->Star = Star;
                Result = TryBindStars(StarIndex);
                if (Result) {
                    return Result;
                }
            }
        } else if (Binding->ConstellationIndex >= 0 && pas::list_at<TScriptConstellation>(Constellations, Binding->ConstellationIndex)->Constellation == nullptr) {
            Count = pas::list_count(aGalaxy::Galaxy->Stars);
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(K); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, K);
                if (Binding->RejectHostilePresence && Star->HasHostilePresenceForScriptBinding()) {
                    continue;
                }
                J = 0;
                while (J < StarIndex) {
                    if (pas::list_at<TScriptStar>(Stars, J)->Star == Star) {
                        break;
                    }
                    ++J;
                }
                if (J < StarIndex) {
                    continue;
                }
                J = 0;
                while (J < pas::list_count(Constellations)) {
                    ConstellationBinding = pas::list_at<TScriptConstellation>(Constellations, J);
                    if (ConstellationBinding->Constellation != nullptr) {
                        if (ConstellationBinding->Constellation == Star->Constellation) {
                            break;
                        }
                    }
                    ++J;
                }
                if (J < pas::list_count(Constellations)) {
                    continue;
                }
                ConstellationBinding = pas::list_at<TScriptConstellation>(Constellations, Binding->ConstellationIndex);
                ConstellationBinding->Constellation = Star->Constellation;
                Binding->Star = Star;
                Result = TryBindStars(StarIndex);
                if (Result) {
                    return Result;
                }
                ConstellationBinding->Constellation = nullptr;
            }
        } else if (Binding->ConstellationIndex >= 0) {
            if (pas::list_at<TScriptConstellation>(Constellations, Binding->ConstellationIndex)->Constellation != nullptr) {
                Constellation = pas::list_at<TScriptConstellation>(Constellations, Binding->ConstellationIndex)->Constellation;
                Count = pas::list_count(Constellation->Stars);
                for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(K); ) {
                    Star = pas::list_at<aGalaxy::TStar>(Constellation->Stars, K);
                    if (Binding->RejectHostilePresence && Star->HasHostilePresenceForScriptBinding()) {
                        continue;
                    }
                    J = 0;
                    while (J < StarIndex) {
                        if (pas::list_at<TScriptStar>(Stars, J)->Star == Star) {
                            break;
                        }
                        ++J;
                    }
                    if (J < StarIndex) {
                        continue;
                    }
                    Binding->Star = Star;
                    Result = TryBindStars(StarIndex);
                    if (Result) {
                        return Result;
                    }
                }
            }
        }
        return false;
    }

    // Accepts script-definition versions 5 through 8.
    std::uint8_t TScript::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* FirstPlanet, std::uint8_t CreateObjects) {
        pas::List* Candidates{};
        std::int32_t I{};
        std::int32_t Count{};
        std::int32_t J{};
        std::int32_t SubCount{};
        std::int32_t K{};
        float Balance{};
        float Radius{};
        // The native frame retains and finalizes an otherwise unused WideString at EBP-$38.
        pas::WideString Text{};
        pas::WideString UnusedText{};
        pas::WideString ErrorText{};
        TScriptConstellation* Constellation{};
        TScriptStar* Star{};
        PScriptPlanetBinding PlanetBinding{};
        TScriptPlace* Place{};
        TScriptItem* ScriptItem{};
        TScriptGroup* Group{};
        aShip::TShip* Ship{};
        TScriptState* State{};
        TScriptShip* Binding{};
        TScriptDialog* Dialog{};
        TScriptDialogMsg* DialogMessage{};
        TScriptDialogAnswer* DialogAnswer{};
        aItem::TItem* Item{};
        aItem::TItem* OtherItem{};
        aPlanet::TPlanet* Planet{};
        std::uint32_t Mask{};
        std::uint8_t KlingType{};
        // Nested helper; captures Self at ParentFrame-4. Caller removes ParentFrame.
        auto CompileStateActionCode = [&](TScriptState* State) -> void {
            pas::WideString SourceText{};
            pas::WideString ActionTypes{};
            pas::WideString StepTypes{};
            std::int32_t I{};
            std::int32_t Count{};
            std::uint32_t Step{};
            std::uint8_t Action{};
            if (State->OnActionText.read(1) == u'[') {
                I = EC_Str::FindTextPosW(u"]"_wref.get(), State->OnActionText);
                SourceText = EC_Str::CopyWideStringUnchecked(State->OnActionText, I + 1, State->OnActionText.length() - I);
                ActionTypes = EC_Str::CopyWideStringUnchecked(State->OnActionText, 2, I - 2);
                StepTypes = EC_Str::ExtractDelimitedPartW(ActionTypes, 1, u"|"_wref.get());
                ActionTypes = EC_Str::ExtractDelimitedPartW(ActionTypes, 0, u"|"_wref.get());
                State->ActionCode = aScript::CompileScriptText(SourceText);
                if (ActionTypes == u"" && StepTypes == u"" || ActionTypes == u"Any") {
                    State->ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep, aConst::satOnDeath}});
                } else if (ActionTypes == u"") {
                    State->ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep}});
                } else {
                    if (StepTypes != u"") {
                        State->ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep}});
                    } else {
                        State->ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({});
                    }
                    ActionTypes = pas::concat_wide({u",", ActionTypes, u","});
                    for (Action = static_cast<std::uint8_t>(0); Action <= static_cast<std::uint8_t>(61); ++Action) {
                        if (pas::pos(pas::concat_wide({u",", aConst::ScriptActionTypeNames[Action], u","}), ActionTypes) > 0) {
                            pas::include_at(&State->ActionTypeMask, Action);
                        }
                    }
                }
                if (StepTypes == u"" || StepTypes == u"Any") {
                    State->StepTypeMask = pas::constant_set<TScriptStepTypeSet>({{0, 11}});
                } else {
                    State->StepTypeMask = pas::constant_set<TScriptStepTypeSet>({});
                    Count = EC_Str::CountDelimitedPartsW(StepTypes, u","_wref.get());
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                        Step = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(StepTypes, I, u","_wref.get()));
                        if (pas::in_range(Step, 0, 11)) {
                            pas::include_at(&State->StepTypeMask, Step);
                        }
                    }
                }
            } else {
                State->ActionCode = aScript::CompileScriptText(State->OnActionText);
                State->ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep, aConst::satOnDeath}});
                State->StepTypeMask = pas::constant_set<TScriptStepTypeSet>({{0, 11}});
            }
            State->ActionCode->LinkAll(ScriptFunctionScope, false);
            State->ActionCode->LinkAll(Globals::SharedScriptVariables, false);
            State->ActionCode->LinkAll(this->InitCode->LocalVar, false);
            State->ActionCode->ScriptFunLinked = true;
        };
        Clear();
        AnchorPlanet = FirstPlanet;
        std::uint8_t Result = false;
        std::uint32_t Version = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (Version < 5 || Version > 8) {
            GR_Main::RaiseWideMessage(u"Script file incorrect version"_wref.get());
            return Result;
        }
        {
            std::int32_t word = EC_Buf::TBufEC_GetWord(Buffer);
            EC_Buf::TBufEC* buffer = Buffer;
            buffer->SetPosition(word);
        }
        CurrentScript = this;
        InitCode->LocalVar->AppendFromBuffer(Buffer);
        Text = pas::concat_wide({u"ScriptLibs.", EC_Str::ExtractDelimitedPartW(ScriptFileName, 1, u"."_wref.get())});
        if (GR_Main::GameDataConfig->CountParamsByPath(Text) > 0) {
            if (ScriptLibraryCache == nullptr) {
                ScriptLibraryCache = pas::construct_call<TLibraryCache>(TLibraryCache_Create);
            }
            Text = GR_Main::GameDataConfig->GetParamByPath(Text);
            Count = EC_Str::CountDelimitedPartsW(Text, u","_wref.get());
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                TLibraryHandler* lib = ScriptLibraryCache->GetLib(EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get()));
                EC_Expression::TVarArrayEC* localVar = InitCode->LocalVar;
                lib->InitAllFunctions(localVar);
            }
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Constellation = pas::construct_call<TScriptConstellation>(TScriptConstellation_Create);
            pas::list_add(Constellations, reinterpret_cast<void*>(Constellation));
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Star = pas::construct_call<TScriptStar>(TScriptStar_Create);
            pas::list_add(Stars, reinterpret_cast<void*>(Star));
            Star->Name = Buffer->ReadWideString();
            Star->ConstellationIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            if (Version < 7) {
                EC_Buf::TBufEC_GetBoolean(Buffer);
            }
            Star->RejectHostilePresence = EC_Buf::TBufEC_GetBoolean(Buffer);
            Star->ProtectStar = EC_Buf::TBufEC_GetBoolean(Buffer);
            SubCount = EC_Buf::TBufEC_GetInt32(Buffer);
            if (SubCount > 0) {
                Star->Constraints.set_length(SubCount);
                if (Version >= 7) {
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_4.next(J); ) {
                        {
                            std::int32_t int32 = EC_Buf::TBufEC_GetInt32(Buffer);
                            pas::List* stars = Stars;
                            Star->Constraints[J].OtherStar = pas::list_at<TScriptStar>(stars, int32);
                        }
                        Star->Constraints[J].MinDistance = EC_Buf::TBufEC_GetInt32(Buffer);
                        Star->Constraints[J].MaxDistance = EC_Buf::TBufEC_GetInt32(Buffer);
                        Star->Constraints[J].RequireBlackHole = EC_Buf::TBufEC_GetBoolean(Buffer);
                    }
                } else {
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_5.next(J); ) {
                        {
                            std::int32_t int32_2 = EC_Buf::TBufEC_GetInt32(Buffer);
                            pas::List* stars_2 = Stars;
                            Star->Constraints[J].OtherStar = pas::list_at<TScriptStar>(stars_2, int32_2);
                        }
                        EC_Buf::TBufEC_GetInt32(Buffer);
                        Star->Constraints[J].MinDistance = EC_Buf::TBufEC_GetInt32(Buffer);
                        Star->Constraints[J].MaxDistance = EC_Buf::TBufEC_GetInt32(Buffer);
                        EC_Buf::TBufEC_GetInt32(Buffer);
                        EC_Buf::TBufEC_GetInt32(Buffer);
                        EC_Buf::TBufEC_GetInt32(Buffer);
                        Star->Constraints[J].RequireBlackHole = EC_Buf::TBufEC_GetBoolean(Buffer);
                    }
                }
            }
            SubCount = EC_Buf::TBufEC_GetInt32(Buffer);
            if (SubCount > 0) {
                Star->Planets.set_length(SubCount);
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_6.next(J); ) {
                    Star->Planets[J].Name = Buffer->ReadWideString();
                    Star->Planets[J].RaceMask = aScript::DecodeScriptRaceMask(EC_Buf::TBufEC_GetUInt32(Buffer));
                    Star->Planets[J].OwnerMask = aScript::DecodeScriptOwnerMask(EC_Buf::TBufEC_GetUInt32(Buffer));
                    Star->Planets[J].EconomyMask = aScript::DecodeScriptEconomyMask(EC_Buf::TBufEC_GetUInt32(Buffer));
                    Star->Planets[J].GovernmentMask = aScript::DecodeScriptGovernmentMask(EC_Buf::TBufEC_GetUInt32(Buffer));
                    Star->Planets[J].MinOrbitPercent = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->Planets[J].MaxOrbitPercent = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->Planets[J].DefinitionText = Buffer->ReadWideString();
                }
            }
            SubCount = EC_Buf::TBufEC_GetInt32(Buffer);
            if (SubCount > 0) {
                Star->ShipRequirements.set_length(SubCount);
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_7.next(J); ) {
                    Star->ShipRequirements[J].Count = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].OwnerMask = aScript::DecodeScriptOwnerMask(EC_Buf::TBufEC_GetUInt32(Buffer));
                    Mask = EC_Buf::TBufEC_GetUInt32(Buffer);
                    Star->ShipRequirements[J].ShipTypeMask = aScript::DecodeScriptShipTypeMask(Mask);
                    for (KlingType = static_cast<std::uint8_t>(0); KlingType <= static_cast<std::uint8_t>(7); ++KlingType) {
                        Star->ShipRequirements[J].DominatorMasks[KlingType] = aScript::DecodeScriptDominatorMask(Mask, KlingType);
                    }
                    Star->ShipRequirements[J].PlayerOnly = EC_Buf::TBufEC_GetBoolean(Buffer);
                    Star->ShipRequirements[J].MinSpeed = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].MaxSpeed = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].WeaponRequirement = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].MinCargoHookLevel = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].MinFreeCargoSpace = EC_Buf::TBufEC_GetInt32(Buffer);
                    if (Version < 7) {
                        EC_Buf::TBufEC_GetInt32(Buffer);
                        EC_Buf::TBufEC_GetInt32(Buffer);
                    }
                    Star->ShipRequirements[J].MinTraderStatus = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].MaxTraderStatus = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].MinWarriorStatus = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].MaxWarriorStatus = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].MinPirateStatus = EC_Buf::TBufEC_GetInt32(Buffer);
                    Star->ShipRequirements[J].MaxPirateStatus = EC_Buf::TBufEC_GetInt32(Buffer);
                    if (Version < 7) {
                        EC_Buf::TBufEC_GetInt32(Buffer);
                        EC_Buf::TBufEC_GetInt32(Buffer);
                    }
                    Star->ShipRequirements[J].MinStrength = EC_Buf::TBufEC_GetSingle(Buffer);
                    Star->ShipRequirements[J].MaxStrength = EC_Buf::TBufEC_GetSingle(Buffer);
                    Star->ShipRequirements[J].StationNames = EC_Str::TrimWideString(Buffer->ReadWideString());
                    if (Star->ShipRequirements[J].StationNames != u"") {
                        Star->ShipRequirements[J].ShipTypeMask = Star->ShipRequirements[J].ShipTypeMask + pas::constant_set<TScriptShipTypeMask>({{8}});
                    }
                }
            }
        }
        if (CreateObjects) {
            Star = pas::list_at<TScriptStar>(Stars, 0);
            Star->Star = AnchorStar;
            if (Star->ConstellationIndex >= 0) {
                Constellation = pas::list_at<TScriptConstellation>(Constellations, Star->ConstellationIndex);
                Constellation->Constellation = AnchorStar->Constellation;
            }
            if (!TryBindStars(0)) {
                return Result;
            }
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_8.next(I); ) {
                Star = pas::list_at<TScriptStar>(Stars, I);
                {
                    EC_Expression::TVarEC* var = InitCode->LocalVar->GetVar(Star->Name);
                    std::uint32_t star = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Star->Star));
                    var->SetDword(star);
                }
                if (Star->Planets != nullptr) {
                    for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Star->Planets.length() - 1); cpp_range_9.next(J); ) {
                        PlanetBinding = &Star->Planets[J];
                        {
                            EC_Expression::TVarEC* var_2 = InitCode->LocalVar->GetVar(PlanetBinding->Name);
                            std::uint32_t planet = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(PlanetBinding->Planet));
                            var_2->SetDword(planet);
                        }
                    }
                }
            }
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_10.next(I); ) {
            Place = pas::construct_call<TScriptPlace>(TScriptPlace_Create);
            pas::list_add(Places, reinterpret_cast<void*>(Place));
            Place->Script = this;
            Place->Name = Buffer->ReadWideString();
            Place->OriginVarName = Buffer->ReadWideString();
            Place->PlaceKind = EC_Buf::TBufEC_GetInt32(Buffer);
            if (Place->PlaceKind == spkPolar) {
                Place->AngleOffset = EC_Buf::TBufEC_GetSingle(Buffer);
                Place->DistanceScale = EC_Buf::TBufEC_GetSingle(Buffer);
                Place->Radius = EC_Buf::TBufEC_GetInt32(Buffer);
            } else if (Place->PlaceKind == spkPlanetPosition) {
                Place->TargetVarName = Buffer->ReadWideString();
                Place->Radius = EC_Buf::TBufEC_GetInt32(Buffer);
            } else if (Place->PlaceKind == spkDockedPlanet) {
                Place->TargetVarName = Buffer->ReadWideString();
            } else if (Place->PlaceKind == spkStarDirection) {
                Place->TargetVarName = Buffer->ReadWideString();
                Place->DistanceScale = EC_Buf::TBufEC_GetSingle(Buffer);
                Place->Radius = EC_Buf::TBufEC_GetInt32(Buffer);
                Place->AngleOffset = EC_Buf::TBufEC_GetSingle(Buffer);
            } else if (Place->PlaceKind == spkScriptItem) {
                Place->TargetVarName = Buffer->ReadWideString();
                Place->Radius = EC_Buf::TBufEC_GetInt32(Buffer);
            } else if (Place->PlaceKind == spkGroupCentroid) {
                Place->TargetVarName = Buffer->ReadWideString();
                Place->DistanceScale = EC_Buf::TBufEC_GetSingle(Buffer);
                Place->Radius = EC_Buf::TBufEC_GetInt32(Buffer);
                Place->AngleOffset = EC_Buf::TBufEC_GetSingle(Buffer);
            } else if (Place->PlaceKind == spkCoordinates) {
                Place->TargetVarName = Buffer->ReadWideString();
                Place->TargetVarName2 = Buffer->ReadWideString();
                Place->Radius = EC_Buf::TBufEC_GetInt32(Buffer);
            } else {
                GR_Main::RaiseWideMessage(u"Script.Place.Type"_wref.get());
            }
            if (CreateObjects) {
                EC_Expression::TVarEC* var_3 = InitCode->LocalVar->GetVar(Place->Name);
                std::uint32_t place = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Place));
                var_3->SetDword(place);
            }
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_11.next(I); ) {
            ScriptItem = pas::construct_call<TScriptItem>(TScriptItem_Create);
            ScriptItem->Script = this;
            pas::list_add(Items, reinterpret_cast<void*>(ScriptItem));
            ScriptItem->Name = Buffer->ReadWideString();
            ScriptItem->LocationVarName = Buffer->ReadWideString();
            ScriptItem->DefinitionKind = EC_Buf::TBufEC_GetInt32(Buffer);
            ScriptItem->DefinitionType = EC_Buf::TBufEC_GetInt32(Buffer);
            ScriptItem->Weight = EC_Buf::TBufEC_GetInt32(Buffer);
            ScriptItem->Level = EC_Buf::TBufEC_GetInt32(Buffer);
            ScriptItem->DefinitionValue1C = EC_Buf::TBufEC_GetInt32(Buffer);
            ScriptItem->OwnerId = aScript::DecodeScriptItemOwner(EC_Buf::TBufEC_GetInt32(Buffer));
            ScriptItem->ConfigName = Buffer->ReadWideString();
            ScriptItem->CanSell = false;
            ScriptItem->Data[1] = 0;
            ScriptItem->Data[2] = 0;
            ScriptItem->Data[3] = 0;
            ScriptItem->TextData1 = pas::WideString();
            ScriptItem->TextData2 = pas::WideString();
            ScriptItem->TextData3 = pas::WideString();
            ScriptItem->OnUseText = pas::WideString();
            ScriptItem->OnActionText = pas::WideString();
            {
                EC_Expression::TVarEC* var_4 = InitCode->LocalVar->GetVar(ScriptItem->Name);
                std::uint32_t scriptItem = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ScriptItem));
                var_4->SetDword(scriptItem);
            }
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_12.next(I); ) {
            Group = pas::construct_call<TScriptGroup>(TScriptGroup_Create);
            pas::list_add(Groups, reinterpret_cast<void*>(Group));
            Group->Name = Buffer->ReadWideString();
            Group->PlanetVarName = Buffer->ReadWideString();
            Group->InitialStateIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->OwnerMask = aScript::DecodeScriptOwnerMask(EC_Buf::TBufEC_GetUInt32(Buffer));
            Mask = EC_Buf::TBufEC_GetUInt32(Buffer);
            Group->ShipTypeMask = aScript::DecodeScriptShipTypeMask(Mask);
            for (KlingType = static_cast<std::uint8_t>(0); KlingType <= static_cast<std::uint8_t>(7); ++KlingType) {
                Group->DominatorMasks[KlingType] = aScript::DecodeScriptDominatorMask(Mask, KlingType);
            }
            Group->MinCount = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MaxCount = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MinSpeed = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MaxSpeed = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->WeaponRequirement = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MinCargoHookLevel = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MinFreeCargoSpace = EC_Buf::TBufEC_GetInt32(Buffer);
            if (Version < 7) {
                EC_Buf::TBufEC_GetInt32(Buffer);
            }
            Group->IncludePlayer = EC_Buf::TBufEC_GetBoolean(Buffer);
            if (Version < 7) {
                EC_Buf::TBufEC_GetInt32(Buffer);
                EC_Buf::TBufEC_GetInt32(Buffer);
                EC_Buf::TBufEC_GetInt32(Buffer);
                EC_Buf::TBufEC_GetInt32(Buffer);
            }
            Group->MinTraderStatus = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MaxTraderStatus = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MinWarriorStatus = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MaxWarriorStatus = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MinPirateStatus = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MaxPirateStatus = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->MaxDistanceFromPlanet = EC_Buf::TBufEC_GetInt32(Buffer);
            Group->DefinitionText = Buffer->ReadWideString();
            Group->MinStrength = EC_Buf::TBufEC_GetSingle(Buffer);
            Group->MaxStrength = EC_Buf::TBufEC_GetSingle(Buffer);
            Group->StationNames = EC_Str::TrimWideString(Buffer->ReadWideString());
            if (Group->StationNames != u"") {
                Group->ShipTypeMask = Group->ShipTypeMask + pas::constant_set<TScriptShipTypeMask>({{8}});
            }
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        if (Count > 0) {
            GroupRelations.set_length(Count);
            for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_13.next(I); ) {
                GroupRelations[I].Group1 = EC_Buf::TBufEC_GetInt32(Buffer);
                GroupRelations[I].Group2 = EC_Buf::TBufEC_GetInt32(Buffer);
                GroupRelations[I].Relation1To2 = EC_Buf::TBufEC_GetInt32(Buffer);
                GroupRelations[I].Relation2To1 = EC_Buf::TBufEC_GetInt32(Buffer);
                GroupRelations[I].MinCombatBalance = EC_Buf::TBufEC_GetSingle(Buffer);
                GroupRelations[I].MaxCombatBalance = EC_Buf::TBufEC_GetSingle(Buffer);
            }
        }
        if (CreateObjects) {
            for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, pas::list_count(Groups) - 1); cpp_range_14.next(I); ) {
                Group = pas::list_at<TScriptGroup>(Groups, I);
                Group->Ships = pas::make_object<pas::List>();
                Group->Planet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(InitCode->LocalVar->GetVar(Group->PlanetVarName)->GetDword())));
                Candidates = aScript::CollectScriptCandidateShips(Group->Planet->CurrentStar);
                SubCount = Group->MinCount;
                if (Group->IncludePlayer) {
                    --SubCount;
                }
                for (auto cpp_range_15 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_15.next(J); ) {
                    Ship = aScript::FindScriptGroupCandidate(Candidates, Group);
                    if (Ship == nullptr) {
                        Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Group->Planet->GenerateShipForScriptGroup(Group)));
                        if (Ship == nullptr) {
                            pas::free(Candidates);
                            return Result;
                        }
                    }
                    BindShip(I, Ship);
                    pas::list_add(Group->Ships, reinterpret_cast<void*>(Ship));
                    if (pas::list_indexof(Candidates, reinterpret_cast<void*>(Ship)) >= 0) {
                        pas::list_delete(Candidates, pas::list_indexof(Candidates, reinterpret_cast<void*>(Ship)));
                    }
                }
                if (Group->IncludePlayer) {
                    BindShip(I, aPlayer::GetPlayer());
                }
                pas::free(Candidates);
            }
            while (true) {
                I = 0;
                Group = nullptr;
                while (I <= GroupRelations.length() - 1) {
                    if (GroupRelations[I].MinCombatBalance > 0.0L || GroupRelations[I].MaxCombatBalance < 1.0E+3L) {
                        Balance = ([&] {
                            pas::List* ships = pas::list_at<TScriptGroup>(Groups, GroupRelations[I].Group2)->Ships;
                            pas::List* ships_2 = pas::list_at<TScriptGroup>(Groups, GroupRelations[I].Group1)->Ships;
                            return aShip::CompareShipGroupsStrength(ships_2, ships);
                        }());
                        if (Balance < GroupRelations[I].MinCombatBalance) {
                            Group = pas::list_at<TScriptGroup>(Groups, GroupRelations[I].Group1);
                            if (pas::list_count(Group->Ships) < Group->MaxCount) {
                                break;
                            }
                        } else if (Balance > GroupRelations[I].MaxCombatBalance) {
                            Group = pas::list_at<TScriptGroup>(Groups, GroupRelations[I].Group2);
                            if (pas::list_count(Group->Ships) < Group->MaxCount) {
                                break;
                            }
                        }
                        Group = nullptr;
                    }
                    ++I;
                }
                if (Group == nullptr) {
                    break;
                }
                Candidates = aScript::CollectScriptCandidateShips(Group->Planet->CurrentStar);
                Ship = aScript::FindScriptGroupCandidate(Candidates, Group);
                if (Ship == nullptr) {
                    Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Group->Planet->GenerateShipForScriptGroup(Group)));
                    if (Ship == nullptr) {
                        pas::free(Candidates);
                        return Result;
                    }
                }
                BindShip(pas::list_indexof(Groups, reinterpret_cast<void*>(Group)), Ship);
                pas::list_add(Group->Ships, reinterpret_cast<void*>(Ship));
                pas::free(Candidates);
            }
            {
                const std::int32_t cpp_last = GroupRelations.length() - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        if (GroupRelations[I].MinCombatBalance > 0.0L || GroupRelations[I].MaxCombatBalance < 1.0E+3L) {
                            Balance = ([&] {
                                pas::List* ships_3 = pas::list_at<TScriptGroup>(Groups, GroupRelations[I].Group2)->Ships;
                                pas::List* ships_4 = pas::list_at<TScriptGroup>(Groups, GroupRelations[I].Group1)->Ships;
                                return aShip::CompareShipGroupsStrength(ships_4, ships_3);
                            }());
                            if (Balance < GroupRelations[I].MinCombatBalance || Balance > GroupRelations[I].MaxCombatBalance) {
                                return Result;
                            }
                        }
                    }
                }
            }
        }
        if (CreateObjects) {
            const std::int32_t cpp_last_2 = GroupRelations.length() - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    if (GroupRelations[I].Relation1To2 != 5) {
                        SetGroupRelation(GroupRelations[I].Group1, GroupRelations[I].Group2, aScript::DecodeScriptRelationLevel(GroupRelations[I].Relation1To2));
                    }
                    if (GroupRelations[I].Relation2To1 != 5) {
                        SetGroupRelation(GroupRelations[I].Group2, GroupRelations[I].Group1, aScript::DecodeScriptRelationLevel(GroupRelations[I].Relation2To1));
                    }
                }
            }
        }
        EC_Expression::TCodeAnalyzerEC* Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
        Analyzer->Tokenize(Buffer->ReadWideString(), 0);
        Analyzer->RemoveComments();
        Analyzer->RemoveNewlines();
        Analyzer->RemoveWhitespace();
        Analyzer->ValidateDelimiters();
        InitCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
        pas::free(Analyzer);
        if (ErrorText != u"") {
            GR_Main::RaiseWideMessage(pas::concat_wide({u"CodeInit.Compiler. Error=", ErrorText}));
        }
        InitCode->LocalVar->Add(u"EndState"_wref.get(), EC_Expression::vkInt)->SetInt(0);
        InitCode->LocalVar->Add(u"CurShip"_wref.get(), EC_Expression::vkDword)->SetInt(0);
        InitCode->LocalVar->Add(u"GABStatus"_wref.get(), EC_Expression::vkInt)->SetInt(0);
        InitCode->LocalVar->Add(u"GQuestStatus"_wref.get(), EC_Expression::vkInt)->SetInt(0);
        InitCode->LocalVar->Add(u"GRobotStatus"_wref.get(), EC_Expression::vkInt)->SetInt(0);
        InitCode->LocalVar->Add(u"GVideoStatus"_wref.get(), EC_Expression::vkInt)->SetInt(0);
        InitCode->LocalVar->Add(u"GAnswerData"_wref.get(), EC_Expression::vkDword)->SetInt(0);
        InitCode->LinkAll(ScriptFunctionScope, false);
        InitCode->LinkAll(Globals::SharedScriptVariables, false);
        InitCode->LinkAll(InitCode->LocalVar, false);
        InitCode->ScriptFunLinked = true;
        if (CreateObjects) {
            try {
                InitCode->Run(ScriptProcess);
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
                } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                    aScript::LogScriptCallHistory();
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in init code of script ", ScriptFileName}))));
                } else {
                    throw;
                }
            }
        }
        Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
        Analyzer->Tokenize(Buffer->ReadWideString(), 0);
        Analyzer->RemoveComments();
        Analyzer->RemoveNewlines();
        Analyzer->RemoveWhitespace();
        Analyzer->ValidateDelimiters();
        TurnCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
        pas::free(Analyzer);
        if (ErrorText != u"") {
            GR_Main::RaiseWideMessage(pas::concat_wide({u"CodeTurn.Compiler. Error=", ErrorText}));
        }
        TurnCode->LinkAll(ScriptFunctionScope, false);
        TurnCode->LinkAll(Globals::SharedScriptVariables, false);
        TurnCode->LinkAll(InitCode->LocalVar, false);
        TurnCode->ScriptFunLinked = true;
        if (Version > 5) {
            Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
            Analyzer->Tokenize(Buffer->ReadWideString(), 0);
            Analyzer->RemoveComments();
            Analyzer->RemoveNewlines();
            Analyzer->RemoveWhitespace();
            Analyzer->ValidateDelimiters();
            AuxiliaryCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
            pas::free(Analyzer);
            if (ErrorText != u"") {
                GR_Main::RaiseWideMessage(pas::concat_wide({u"CodeTurn.Compiler. Error=", ErrorText}));
            }
            AuxiliaryCode->LinkAll(ScriptFunctionScope, false);
            AuxiliaryCode->LinkAll(Globals::SharedScriptVariables, false);
            AuxiliaryCode->LinkAll(InitCode->LocalVar, false);
            AuxiliaryCode->ScriptFunLinked = true;
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_16 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_16.next(I); ) {
            State = pas::construct_call<TScriptState>(TScriptState_Create);
            pas::list_add(States, reinterpret_cast<void*>(State));
            State->Name = Buffer->ReadWideString();
            State->StateKind = EC_Buf::TBufEC_GetInt32(Buffer);
            if (State->StateKind != sskIdle && State->StateKind != sskNormalAI) {
                State->TargetVarName = Buffer->ReadWideString();
            }
            SubCount = EC_Buf::TBufEC_GetInt32(Buffer);
            if (SubCount > 0) {
                State->EnemyGroupNames.set_length(SubCount);
                State->EnemyGroupIndices.set_length(SubCount);
                for (auto cpp_range_17 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_17.next(J); ) {
                    State->EnemyGroupNames[J] = Buffer->ReadWideString();
                }
            }
            State->PickupItemVarName = Buffer->ReadWideString();
            if (State->PickupItemVarName != u"") {
                State->PickupItem = reinterpret_cast<TScriptItem*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(InitCode->LocalVar->GetVar(State->PickupItemVarName)->GetDword())));
            }
            State->PickUpNearbyItems = EC_Buf::TBufEC_GetBoolean(Buffer);
            State->AuxiliaryText = Buffer->ReadWideString();
            if (State->AuxiliaryText != u"" && InitCode->LocalVar->GetVarNE(State->AuxiliaryText) == nullptr) {
                State->AuxiliaryCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
                Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
                Analyzer->Tokenize(State->AuxiliaryText, 0);
                Analyzer->RemoveComments();
                Analyzer->RemoveNewlines();
                Analyzer->RemoveWhitespace();
                Analyzer->ValidateDelimiters();
                State->AuxiliaryCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
                pas::free(Analyzer);
                if (ErrorText != u"") {
                    GR_Main::RaiseWideMessage(pas::concat_wide({u"StateCodeText.Compiler. Error=", ErrorText, u" State=", State->Name}));
                }
                State->AuxiliaryCode->LinkAll(ScriptFunctionScope, false);
                State->AuxiliaryCode->LinkAll(Globals::SharedScriptVariables, false);
                State->AuxiliaryCode->LinkAll(InitCode->LocalVar, false);
                State->AuxiliaryCode->ScriptFunLinked = true;
            }
            State->OnActionText = Buffer->ReadWideString();
            if (State->OnActionText != u"" && State->OnActionText.length() < 32 && InitCode->LocalVar->GetVarNE(State->AuxiliaryText) != nullptr) {
                State->OnActionText = pas::WideString();
            }
            if (State->OnActionText != u"") {
                CompileStateActionCode(State);
            }
            Text = Buffer->ReadWideString();
            if (Text != u"") {
                State->EntryCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
                Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
                Analyzer->Tokenize(Text, 0);
                Analyzer->RemoveComments();
                Analyzer->RemoveNewlines();
                Analyzer->RemoveWhitespace();
                Analyzer->ValidateDelimiters();
                State->EntryCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
                pas::free(Analyzer);
                if (ErrorText != u"") {
                    GR_Main::RaiseWideMessage(pas::concat_wide({u"StateCodeEther.Compiler. Error=", ErrorText, u" State=", State->Name}));
                }
                State->EntryCode->LinkAll(ScriptFunctionScope, false);
                State->EntryCode->LinkAll(Globals::SharedScriptVariables, false);
                State->EntryCode->LinkAll(InitCode->LocalVar, false);
                State->EntryCode->ScriptFunLinked = true;
            }
            Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
            Analyzer->Tokenize(Buffer->ReadWideString(), 0);
            Analyzer->RemoveComments();
            Analyzer->RemoveNewlines();
            Analyzer->RemoveWhitespace();
            Analyzer->ValidateDelimiters();
            State->StateCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
            pas::free(Analyzer);
            if (ErrorText != u"") {
                GR_Main::RaiseWideMessage(pas::concat_wide({u"StateTurn.Compiler. Error=", ErrorText, u" State=", State->Name}));
            }
            State->StateCode->LinkAll(ScriptFunctionScope, false);
            State->StateCode->LinkAll(Globals::SharedScriptVariables, false);
            State->StateCode->LinkAll(InitCode->LocalVar, false);
            State->StateCode->ScriptFunLinked = true;
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_18 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_18.next(I); ) {
            Dialog = pas::construct_call<TScriptDialog>(TScriptDialog_Create);
            pas::list_add(Dialogs, reinterpret_cast<void*>(Dialog));
            Dialog->Name = Buffer->ReadWideString();
            Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
            Analyzer->Tokenize(Buffer->ReadWideString(), 0);
            Analyzer->RemoveComments();
            Analyzer->RemoveNewlines();
            Analyzer->RemoveWhitespace();
            Analyzer->ValidateDelimiters();
            Dialog->Code->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
            pas::free(Analyzer);
            if (ErrorText != u"") {
                GR_Main::RaiseWideMessage(pas::concat_wide({u"Dialog.Code.Compiler. Error=", ErrorText, u" State=", Dialog->Name}));
            }
            Dialog->Code->LinkAll(ScriptFunctionScope, false);
            Dialog->Code->LinkAll(Globals::SharedScriptVariables, false);
            Dialog->Code->LinkAll(InitCode->LocalVar, false);
            Dialog->Code->ScriptFunLinked = true;
            InitCode->LocalVar->GetVar(Dialog->Name)->SetInt(I);
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_19 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_19.next(I); ) {
            DialogMessage = pas::construct_call<TScriptDialogMsg>(TScriptDialogMsg_Create);
            pas::list_add(DialogMessages, reinterpret_cast<void*>(DialogMessage));
            DialogMessage->Name = Buffer->ReadWideString();
            Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
            Analyzer->Tokenize(Buffer->ReadWideString(), 0);
            Analyzer->RemoveComments();
            Analyzer->RemoveNewlines();
            Analyzer->RemoveWhitespace();
            Analyzer->ValidateDelimiters();
            DialogMessage->Code->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
            pas::free(Analyzer);
            if (ErrorText != u"") {
                GR_Main::RaiseWideMessage(pas::concat_wide({u"DialogMsg.Code.Compiler. Error=", ErrorText, u" State=", DialogMessage->Name}));
            }
            DialogMessage->Code->LinkAll(ScriptFunctionScope, false);
            DialogMessage->Code->LinkAll(Globals::SharedScriptVariables, false);
            DialogMessage->Code->LinkAll(InitCode->LocalVar, false);
            DialogMessage->Code->ScriptFunLinked = true;
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_20 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_20.next(I); ) {
            DialogAnswer = pas::construct_call<TScriptDialogAnswer>(TScriptDialogAnswer_Create);
            pas::list_add(DialogAnswers, reinterpret_cast<void*>(DialogAnswer));
            DialogAnswer->Name = Buffer->ReadWideString();
            Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
            Analyzer->Tokenize(Buffer->ReadWideString(), 0);
            Analyzer->RemoveComments();
            Analyzer->RemoveNewlines();
            Analyzer->RemoveWhitespace();
            Analyzer->ValidateDelimiters();
            DialogAnswer->AnswerCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
            pas::free(Analyzer);
            if (ErrorText != u"") {
                GR_Main::RaiseWideMessage(pas::concat_wide({u"DialogAnswer.CodeAnswer.Compiler. Error=", ErrorText, u" State=", DialogAnswer->Name}));
            }
            DialogAnswer->AnswerCode->LinkAll(ScriptFunctionScope, false);
            DialogAnswer->AnswerCode->LinkAll(Globals::SharedScriptVariables, false);
            DialogAnswer->AnswerCode->LinkAll(InitCode->LocalVar, false);
            DialogAnswer->AnswerCode->ScriptFunLinked = true;
            Analyzer = pas::construct_call<EC_Expression::TCodeAnalyzerEC>(EC_Expression::TCodeAnalyzerEC_Create);
            Analyzer->Tokenize(Buffer->ReadWideString(), 0);
            Analyzer->RemoveComments();
            Analyzer->RemoveNewlines();
            Analyzer->RemoveWhitespace();
            Analyzer->ValidateDelimiters();
            DialogAnswer->ActionCode->Compile(Analyzer, nullptr, nullptr, nullptr, nullptr, ErrorText);
            pas::free(Analyzer);
            if (ErrorText != u"") {
                GR_Main::RaiseWideMessage(pas::concat_wide({u"DialogAnswer.Code.Compiler. Error=", ErrorText, u" State=", DialogAnswer->Name}));
            }
            DialogAnswer->ActionCode->LinkAll(ScriptFunctionScope, false);
            DialogAnswer->ActionCode->LinkAll(Globals::SharedScriptVariables, false);
            DialogAnswer->ActionCode->LinkAll(InitCode->LocalVar, false);
            DialogAnswer->ActionCode->ScriptFunLinked = true;
        }
        if (CreateObjects) {
            for (auto cpp_range_21 = pas::for_to<std::int32_t>(0, pas::list_count(States) - 1); cpp_range_21.next(I); ) {
                State = pas::list_at<TScriptState>(States, I);
                if (State->TargetVarName != u"") {
                    State->TargetValue = InitCode->LocalVar->GetVar(State->TargetVarName)->GetDword();
                }
                if (State->EnemyGroupIndices != nullptr) {
                    for (auto cpp_range_22 = pas::for_to<std::int32_t>(0, State->EnemyGroupIndices.length() - 1); cpp_range_22.next(J); ) {
                        State->EnemyGroupIndices[J] = InitCode->LocalVar->GetVar(State->EnemyGroupNames[J])->GetInt();
                    }
                }
            }
            for (auto cpp_range_23 = pas::for_to<std::int32_t>(0, pas::list_count(Places) - 1); cpp_range_23.next(I); ) {
                Place = pas::list_at<TScriptPlace>(Places, I);
                if (Place->OriginVarName != u"") {
                    Place->OriginStar = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(InitCode->LocalVar->GetVar(Place->OriginVarName)->GetDword())));
                }
                if (Place->PlaceKind == spkCoordinates) {
                    if (Place->TargetVarName != u"") {
                        Place->TargetValue = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(InitCode->LocalVar->GetVar(Place->TargetVarName)));
                    }
                    if (Place->TargetVarName2 != u"") {
                        Place->TargetValue2 = InitCode->LocalVar->GetVar(Place->TargetVarName2);
                    }
                } else if (Place->TargetVarName != u"") {
                    Place->TargetValue = InitCode->LocalVar->GetVar(Place->TargetVarName)->GetDword();
                }
            }
            for (auto cpp_range_24 = pas::for_to<std::int32_t>(0, pas::list_count(Items) - 1); cpp_range_24.next(I); ) {
                ScriptItem = pas::list_at<TScriptItem>(Items, I);
                if (ScriptItem->Name == u"") {
                    continue;
                }
                Item = nullptr;
                if (ScriptItem->DefinitionKind == 0 && ScriptItem->DefinitionType == 0) {
                    Item = pas::construct_call<aItem::TFuelTanks>(aItem::TEquipment_Create);
                    pas::checked_cast<aItem::TFuelTanks*>(Item)->Init(ScriptItem->Weight, ScriptItem->Level, ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 0 && ScriptItem->DefinitionType == 1) {
                    Item = pas::construct_call<aItem::TEngine>(aItem::TEquipment_Create);
                    pas::checked_cast<aItem::TEngine*>(Item)->Init(ScriptItem->Weight, ScriptItem->Level, ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 0 && ScriptItem->DefinitionType == 2) {
                    Item = pas::construct_call<aItem::TRadar>(aItem::TEquipment_Create);
                    pas::checked_cast<aItem::TRadar*>(Item)->Init(ScriptItem->Weight, ScriptItem->Level, ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 0 && ScriptItem->DefinitionType == 3) {
                    Item = pas::construct_call<aItem::TScaner>(aItem::TEquipment_Create);
                    pas::checked_cast<aItem::TScaner*>(Item)->Init(ScriptItem->Weight, ScriptItem->Level, ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 0 && ScriptItem->DefinitionType == 4) {
                    Item = pas::construct_call<aItem::TRepairRobot>(aItem::TEquipment_Create);
                    pas::checked_cast<aItem::TRepairRobot*>(Item)->Init(ScriptItem->Weight, ScriptItem->Level, ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 0 && ScriptItem->DefinitionType == 5) {
                    Item = pas::construct_call<aItem::TCargoHook>(aItem::TCargoHook_Create);
                    pas::checked_cast<aItem::TCargoHook*>(Item)->Init(ScriptItem->Weight, ScriptItem->Level, ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 0 && ScriptItem->DefinitionType == 6) {
                    Item = pas::construct_call<aItem::TDefGenerator>(aItem::TEquipment_Create);
                    pas::checked_cast<aItem::TDefGenerator*>(Item)->Init(ScriptItem->Weight, ScriptItem->Level, ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 0) {
                    GR_Main::RaiseWideMessage(u"Script unknow item type"_wref.get());
                } else if (ScriptItem->DefinitionKind == 1 && ScriptItem->DefinitionType >= 0 && ScriptItem->DefinitionType < aConst::CountItemTypesInMask(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}))) {
                    Item = pas::construct_call<aItem::TWeapon>(aItem::TEquipment_Create);
                    pas::checked_cast<aItem::TWeapon*>(Item)->Init(static_cast<aConst::TItemType>(aConst::GetItemTypeFromMask(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}), ScriptItem->DefinitionType + 1)), ScriptItem->Weight, ScriptItem->Level, ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 1) {
                    GR_Main::RaiseWideMessage(u"Script unknow item type"_wref.get());
                } else if (ScriptItem->DefinitionKind == 2 && ScriptItem->DefinitionType >= 0 && ScriptItem->DefinitionType <= 8) {
                    Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                    pas::checked_cast<aItem::TGoods*>(Item)->Init(static_cast<aConst::TItemType>(static_cast<std::uint8_t>(ScriptItem->DefinitionType) - static_cast<std::uint8_t>(ScriptItem->DefinitionType > 4)), ScriptItem->Weight);
                    pas::checked_cast<aItem::TGoods*>(Item)->NaturalFlag = ScriptItem->DefinitionType == 5;
                } else if (ScriptItem->DefinitionKind == 2 && ScriptItem->DefinitionType == 9) {
                    GR_Main::RaiseWideMessage(u"Script. Protoplasm not support"_wref.get());
                } else if (ScriptItem->DefinitionKind == 2) {
                    GR_Main::RaiseWideMessage(u"Script unknow item type"_wref.get());
                } else if (ScriptItem->DefinitionKind == 3 && ScriptItem->DefinitionType >= 0 && ScriptItem->DefinitionType < aConst::CountItemTypesInMask(pas::constant_set<aConst::TItemTypeSelection>({{10, 41}}))) {
                    Item = aItem::CreateConfiguredArtefactByItemType(static_cast<aConst::TItemType>(aConst::GetItemTypeFromMask(pas::constant_set<aConst::TItemTypeSelection>({{10, 41}}), ScriptItem->DefinitionType + 1)), ScriptItem->OwnerId);
                } else if (ScriptItem->DefinitionKind == 3) {
                    GR_Main::RaiseWideMessage(u"Script unknow item type"_wref.get());
                } else if (ScriptItem->DefinitionKind == 4) {
                    Item = pas::construct_call<aItem::TUselessItem>(aItem::TUselessItem_Create);
                    pas::checked_cast<aItem::TUselessItem*>(Item)->Init(ScriptItem->ConfigName, aGalaxyStruct::dsBlazer, 0u, false);
                } else if (ScriptItem->DefinitionKind == 5) {
                    continue;
                } else {
                    GR_Main::RaiseWideMessage(u"Script unknow item type"_wref.get());
                }
                Item->ScriptItem = ScriptItem;
                if (InitCode->LocalVar->GetVar(ScriptItem->LocationVarName)->GetDword() < 255) {
                    Group = ([&] {
                        std::int32_t dword = static_cast<std::int32_t>(InitCode->LocalVar->GetVar(ScriptItem->LocationVarName)->GetDword());
                        pas::List* groups = Groups;
                        return pas::list_at<TScriptGroup>(groups, dword);
                    }());
                    J = 0;
                    while (J < pas::list_count(Group->Ships)) {
                        Ship = pas::list_at<aShip::TShip>(Group->Ships, J);
                        if (Ship->CargoFreeSpace >= Item->Weight) {
                            break;
                        }
                        ++J;
                    }
                    if (J < pas::list_count(Group->Ships)) {
                        Ship = pas::list_at<aShip::TShip>(Group->Ships, J);
                        if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr) {
                            Ship->CargoGoods[Item->ItemType].Count += reinterpret_cast<aItem::TGoods*>(Item)->Quantity;
                            pas::free(Item);
                            Item = nullptr;
                        } else if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                            pas::list_add(Ship->Artefacts, reinterpret_cast<void*>(Item));
                            if (pas::class_cast_if<aItem::TArtefactTranclucator*>(Item) != nullptr) {
                                pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(pas::checked_cast<aItem::TArtefactTranclucator*>(Item)->Ship))->OwnerShip = Ship;
                            }
                        } else {
                            pas::list_add(Ship->Inventory, reinterpret_cast<void*>(Item));
                            Ship->EquipItem(pas::checked_cast<aItem::TEquipment*>(Item));
                        }
                        Ship->RefreshDerivedStats(true);
                    } else if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr) {
                        for (auto cpp_range_25 = pas::for_to<std::int32_t>(0, pas::list_count(Group->Ships) - 1); cpp_range_25.next(J); ) {
                            Ship = pas::list_at<aShip::TShip>(Group->Ships, J);
                            if (Ship->CargoFreeSpace < reinterpret_cast<aItem::TGoods*>(Item)->Quantity) {
                                Ship->CargoGoods[Item->ItemType].Count += Ship->CargoFreeSpace;
                                // Native code adds free space to the remaining quantity here.
                                reinterpret_cast<aItem::TGoods*>(Item)->Quantity += Ship->CargoFreeSpace;
                                Ship->RefreshDerivedStats(true);
                            } else {
                                Ship->CargoGoods[Item->ItemType].Count += reinterpret_cast<aItem::TGoods*>(Item)->Quantity;
                                reinterpret_cast<aItem::TGoods*>(Item)->Quantity = 0;
                                Ship->RefreshDerivedStats(true);
                                break;
                            }
                        }
                        if (reinterpret_cast<aItem::TGoods*>(Item)->Quantity > 0) {
                            Ship = pas::list_at<aShip::TShip>(Group->Ships, 0);
                            Ship->CargoGoods[Item->ItemType].Count += reinterpret_cast<aItem::TGoods*>(Item)->Quantity;
                        }
                        pas::free(Item);
                        Item = nullptr;
                    } else {
                        Ship = pas::list_at<aShip::TShip>(Group->Ships, 0);
                        if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                            pas::list_add(Ship->Artefacts, reinterpret_cast<void*>(Item));
                        } else {
                            pas::list_add(Ship->Inventory, reinterpret_cast<void*>(Item));
                            Ship->EquipItem(pas::checked_cast<aItem::TEquipment*>(Item));
                        }
                        Ship->RefreshDerivedStats(true);
                    }
                } else if (pas::class_cast_if<aPlanet::TPlanet*>(reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(InitCode->LocalVar->GetVar(ScriptItem->LocationVarName)->GetDword())))) != nullptr) {
                    Planet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(InitCode->LocalVar->GetVar(ScriptItem->LocationVarName)->GetDword())));
                    pas::list_add(Planet->EquipmentShop, reinterpret_cast<void*>(Item));
                } else {
                    Place = reinterpret_cast<TScriptPlace*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(InitCode->LocalVar->GetVar(ScriptItem->LocationVarName)->GetDword())));
                    if (Place->PlaceKind != spkPolar && Place->PlaceKind != spkPlanetPosition && Place->PlaceKind != spkStarDirection && Place->PlaceKind != spkGroupCentroid) {
                        GR_Main::RaiseWideMessage(u"Script error place type"_wref.get());
                    }
                    Item->Position = Place->GetPoint();
                    for (K = 0; K <= 3; ++K) {
                        SubCount = pas::list_count(Place->OriginStar->Items);
                        J = 0;
                        while (J < SubCount) {
                            OtherItem = pas::list_at<aItem::TItem>(Place->OriginStar->Items, J);
                            if (static_cast<long double>(aMyFunction::PointDistanceSquared(Item->Position, OtherItem->Position)) < 16 - 2 * K) {
                                break;
                            }
                            ++J;
                        }
                        if (J >= SubCount) {
                            break;
                        }
                        Balance = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 360, aGalaxy::Galaxy->GenerationSeed * aGalaxy::Galaxy->CurrentTurn * (I + K + 1)));
                        Radius = aMyFunction::SeededRandomIntRange(0, Place->Radius, aGalaxy::Galaxy->GenerationSeed * aGalaxy::Galaxy->CurrentTurn * (I + K + 1 + 457) * 341);
                        Item->Position = Place->GetPoint();
                        Item->Position.X = Item->Position.X + System::Sin(Balance) * Radius;
                        Item->Position.Y = Item->Position.Y - System::Cos(Balance) * Radius;
                    }
                    pas::list_add(Place->OriginStar->Items, reinterpret_cast<void*>(Item));
                }
                ScriptItem->Item = Item;
            }
            {
                const std::int32_t cpp_first = pas::list_count(Ships) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        Binding = pas::list_at<TScriptShip>(Ships, I);
                        ChangeState(Binding, pas::list_at<TScriptGroup>(Groups, Binding->GroupIndex)->InitialStateIndex);
                    }
                }
            }
            for (auto cpp_range_26 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_26.next(I); ) {
                Star = pas::list_at<TScriptStar>(Stars, I);
                if (Star->ProtectStar) {
                    aGalaxy::Galaxy->CancelEnemyJumpsToStar(Star->Star);
                }
            }
        }
        return true;
    }

    std::uint8_t TScript::LoadFromFile(pas::WideString FileName, aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* FirstPlanet, std::uint8_t CreateObjects) {
        std::uint8_t Result{};
        EC_CacheBuf::TCBufEC* CachedBuffer{};
        EC_CacheBuf::TCBufControlEC* Control = nullptr;
        ScriptFileName = FileName;
        std::uint8_t Failed = true;
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(FileName);
                CachedBuffer = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                Result = LoadFromBuffer(CachedBuffer->Buffer, AnchorStar, FirstPlanet, CreateObjects);
                Failed = false;
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Control != nullptr) {
                Control->Release();
                Control->EvictData(pas::class_ref<EC_CacheBuf::TCBufEC>());
                pas::free(Control);
            }
            if (Failed) {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to load script: ", FileName})));
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        return Result;
    }

    // Compiled instructions are excluded.
    void TScript::SaveState(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        std::int32_t J{};
        TScriptShip* Binding{};
        EC_Expression::TVarEC* Cell{};
        EC_Expression::TVarKind Kind{};
        TScriptStar* Star{};
        TScriptItem* ScriptItem{};
        Buffer->AddWideStringZ(ScriptFileName);
        Ether->SaveToBuffer(Buffer);
        std::int32_t Count = InitCode->LocalVar->Count;
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Cell = EC_Expression::TVarArrayEC_GetItem(InitCode->LocalVar, I);
            Kind = Cell->Kind;
            if (Kind == EC_Expression::vkEmpty) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
            } else if (Kind == EC_Expression::vkInt) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddIntegerValue(Cell->GetInt());
            } else if (Kind == EC_Expression::vkDword) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddDWord(Cell->GetDword());
            } else if (Kind == EC_Expression::vkFloat) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddDouble(Cell->GetFloat());
            } else if (Kind == EC_Expression::vkString) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddWideStringZ(Cell->GetString());
            } else if (Kind == EC_Expression::vkArray) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Cell->GetArray()->SaveToBuffer(Buffer);
            } else if (Kind == EC_Expression::vkLibraryFun && Cell->GetString() != u"") {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddWideStringZ(Cell->GetString());
            } else {
                Buffer->AddWideStringZ(Cell->Name);
                Kind = EC_Expression::vkEmpty;
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
            }
        }
        Count = TurnCode->LocalVar->Count;
        Buffer->AddWideChar(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Cell = EC_Expression::TVarArrayEC_GetItem(TurnCode->LocalVar, I);
            Kind = Cell->Kind;
            if (Kind == EC_Expression::vkEmpty) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
            } else if (Kind == EC_Expression::vkInt) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddIntegerValue(Cell->GetInt());
            } else if (Kind == EC_Expression::vkDword) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddDWord(Cell->GetDword());
            } else if (Kind == EC_Expression::vkFloat) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddDouble(Cell->GetFloat());
            } else if (Kind == EC_Expression::vkString) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddWideStringZ(Cell->GetString());
            } else if (Kind == EC_Expression::vkArray) {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Cell->GetArray()->SaveToBuffer(Buffer);
            } else if (Kind == EC_Expression::vkLibraryFun && Cell->GetString() != u"") {
                Buffer->AddWideStringZ(Cell->Name);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
                Buffer->AddWideStringZ(Cell->GetString());
            } else {
                Buffer->AddWideStringZ(Cell->Name);
                Kind = EC_Expression::vkEmpty;
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
            }
        }
        Buffer->AddIntegerValue(pas::list_count(Stars));
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_3.next(I); ) {
            Star = pas::list_at<TScriptStar>(Stars, I);
            Buffer->AddWideStringZ(Star->Name);
            Buffer->AddDWord(Star->Star->Id);
            if (Star->Planets == nullptr) {
                Buffer->AddIntegerValue(0);
            } else {
                Buffer->AddIntegerValue(Star->Planets.length() - 1 + 1);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Star->Planets.length() - 1); cpp_range_4.next(J); ) {
                    Buffer->AddWideStringZ(Star->Planets[J].Name);
                    Buffer->AddDWord(Star->Planets[J].Planet->Id);
                }
            }
            Buffer->AddIntegerValue(0);
        }
        Count = 0;
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Items) - 1); cpp_range_5.next(I); ) {
            ScriptItem = pas::list_at<TScriptItem>(Items, I);
            if (ScriptItem->Name != u"" || ScriptItem->Item != nullptr) {
                ++Count;
            }
        }
        Buffer->AddIntegerValue(Count);
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Items) - 1); cpp_range_6.next(I); ) {
            ScriptItem = pas::list_at<TScriptItem>(Items, I);
            if (ScriptItem->Name != u"" || ScriptItem->Item != nullptr) {
                Buffer->AddWideStringZ(ScriptItem->Name);
                Buffer->AddBoolean(ScriptItem->CanSell);
                Buffer->AddIntegerValue(ScriptItem->Data[1]);
                Buffer->AddIntegerValue(ScriptItem->Data[2]);
                Buffer->AddIntegerValue(ScriptItem->Data[3]);
                Buffer->AddWideStringZ(ScriptItem->TextData1);
                Buffer->AddWideStringZ(ScriptItem->TextData2);
                Buffer->AddWideStringZ(ScriptItem->TextData3);
                Buffer->AddWideStringZ(ScriptItem->OnUseText);
                Buffer->AddWideStringZ(ScriptItem->OnActionText);
                if (ScriptItem->Item == nullptr) {
                    Buffer->AddDWord(0u);
                } else {
                    Buffer->AddDWord(ScriptItem->Item->Id);
                }
            }
        }
        Count = pas::list_count(Ships);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(I); ) {
            Binding = pas::list_at<TScriptShip>(Ships, I);
            Buffer->AddIntegerValue(Binding->GroupIndex);
            Buffer->AddDWord(Binding->Ship->Id);
            Buffer->AddDWord(Binding->Data[0]);
            Buffer->AddDWord(Binding->Data[1]);
            Buffer->AddDWord(Binding->Data[2]);
            Buffer->AddDWord(Binding->Data[3]);
            Buffer->AddIntegerValue(pas::list_indexof(States, reinterpret_cast<void*>(Binding->State)));
            Buffer->AddWideStringZ(Binding->StateText);
            Buffer->AddBoolean(Binding->Hit);
            Buffer->AddBoolean(Binding->HitPlayer);
        }
        Buffer->AddWideChar(EtherIds->GetCount());
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, EtherIds->GetCount() - 1); cpp_range_8.next(I); ) {
            Buffer->AddWideStringZ(EtherIds->GetTextAt(I));
        }
    }

    // Requires the original script-definition file. Resolves star, planet and item IDs through Galaxy; ship IDs are deferred.
    void TScript::LoadState(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t SubCount{};
        std::int32_t StateIndex{};
        TScriptShip* Binding{};
        pas::WideString Name{};
        EC_Expression::TVarEC* Cell{};
        EC_Expression::TVarKind Kind{};
        TScriptStar* Star{};
        PScriptPlanetBinding PlanetBinding{};
        TScriptItem* ScriptItem{};
        if (!([&] {
            pas::WideString readWideString = Buffer->ReadWideString();
            TScript* self = this;
            return self->LoadFromFile(std::move(readWideString), nullptr, nullptr, false);
        }())) {
            pas::raise(pas::make_exception<pas::Exception>("Error. Script.GameLoad"_a));
        }
        Ether->LoadFromBuffer(Buffer);
        std::uint8_t TemporaryCell = false;
        std::int32_t Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Name = Buffer->ReadWideString();
            Cell = InitCode->LocalVar->GetVarNE(Name);
            if (Cell == nullptr) {
                Cell = pas::construct_call<EC_Expression::TVarEC>(EC_Expression::TVarEC_Create, EC_Expression::vkEmpty);
                TemporaryCell = true;
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning.Script.GameLoad variable not found: ", Name, u" (", ScriptFileName, u")"})));
            }
            Kind = static_cast<EC_Expression::TVarKind>(EC_Buf::TBufEC_GetByte(Buffer));
            // Definition-owned dialog/group handles must not be overwritten by saved pointers.
            if (Kind == EC_Expression::vkDword && static_cast<std::uint8_t>(TemporaryCell ^ 1)) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Dialogs) - 1); cpp_range_2.next(J); ) {
                    if (pas::list_at<TScriptDialog>(Dialogs, J)->Name == Name) {
                        Cell = pas::construct_call<EC_Expression::TVarEC>(EC_Expression::TVarEC_Create, EC_Expression::vkEmpty);
                        TemporaryCell = true;
                        break;
                    }
                }
                if (!TemporaryCell) {
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Groups) - 1); cpp_range_3.next(J); ) {
                        if (pas::list_at<TScriptGroup>(Groups, J)->Name == Name) {
                            Cell = pas::construct_call<EC_Expression::TVarEC>(EC_Expression::TVarEC_Create, EC_Expression::vkEmpty);
                            TemporaryCell = true;
                            break;
                        }
                    }
                }
            }
            if (Kind == EC_Expression::vkLibraryFun && static_cast<std::uint8_t>(TemporaryCell ^ 1)) {
                Cell = pas::construct_call<EC_Expression::TVarEC>(EC_Expression::TVarEC_Create, EC_Expression::vkEmpty);
                TemporaryCell = true;
            }
            if (!(Kind == EC_Expression::vkEmpty)) {
                if (Kind == EC_Expression::vkInt) {
                    Cell->SetInt(EC_Buf::TBufEC_GetInt32(Buffer));
                } else if (Kind == EC_Expression::vkDword) {
                    Cell->SetDword(EC_Buf::TBufEC_GetUInt32(Buffer));
                } else if (Kind == EC_Expression::vkFloat) {
                    Cell->SetFloat(Buffer->GetDouble());
                } else if (Kind == EC_Expression::vkString) {
                    Cell->SetString(Buffer->ReadWideString());
                } else if (Kind == EC_Expression::vkArray) {
                    Cell->SetArray(pas::construct_call<EC_Expression::TVarArrayEC>(EC_Expression::TVarArrayEC_Create));
                    Cell->GetArray()->LoadFromBuffer(Buffer);
                } else if (Kind == EC_Expression::vkLibraryFun) {
                    Cell->ConvertToKind(EC_Expression::vkLibraryFun);
                    Name = Buffer->ReadWideString();
                    Cell->SetString(Name);
                    if (ScriptLibraryCache == nullptr) {
                        ScriptLibraryCache = pas::construct_call<TLibraryCache>(TLibraryCache_Create);
                    }
                    try {
                        ScriptLibraryCache->InitFunction(Cell);
                    } catch (...) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to load function ", Name})));
                        Cell->ConvertToKind(EC_Expression::vkEmpty);
                    }
                } else {
                    if (TemporaryCell) {
                        pas::free(Cell);
                    }
                    pas::raise(pas::make_exception<pas::Exception>("Error. Script. Unknown variable format."_a));
                }
            }
            if (TemporaryCell) {
                pas::free(Cell);
                TemporaryCell = false;
            }
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
            Name = Buffer->ReadWideString();
            Cell = TurnCode->LocalVar->GetVarNE(Name);
            if (Cell == nullptr) {
                Cell = pas::construct_call<EC_Expression::TVarEC>(EC_Expression::TVarEC_Create, EC_Expression::vkEmpty);
                TemporaryCell = true;
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning.Script.GameLoad variable not found: ", Name, u" (", ScriptFileName, u")"})));
            }
            Kind = static_cast<EC_Expression::TVarKind>(EC_Buf::TBufEC_GetByte(Buffer));
            if (Kind == EC_Expression::vkLibraryFun && static_cast<std::uint8_t>(TemporaryCell ^ 1)) {
                Cell = pas::construct_call<EC_Expression::TVarEC>(EC_Expression::TVarEC_Create, EC_Expression::vkEmpty);
                TemporaryCell = true;
            }
            if (!(Kind == EC_Expression::vkEmpty)) {
                if (Kind == EC_Expression::vkInt) {
                    Cell->SetInt(EC_Buf::TBufEC_GetInt32(Buffer));
                } else if (Kind == EC_Expression::vkDword) {
                    Cell->SetDword(EC_Buf::TBufEC_GetUInt32(Buffer));
                } else if (Kind == EC_Expression::vkFloat) {
                    Cell->SetFloat(Buffer->GetDouble());
                } else if (Kind == EC_Expression::vkString) {
                    Cell->SetString(Buffer->ReadWideString());
                } else if (Kind == EC_Expression::vkArray) {
                    Cell->SetArray(pas::construct_call<EC_Expression::TVarArrayEC>(EC_Expression::TVarArrayEC_Create));
                    Cell->GetArray()->LoadFromBuffer(Buffer);
                } else if (Kind == EC_Expression::vkLibraryFun) {
                    Cell->ConvertToKind(EC_Expression::vkLibraryFun);
                    Name = Buffer->ReadWideString();
                    Cell->SetString(Name);
                    if (ScriptLibraryCache == nullptr) {
                        ScriptLibraryCache = pas::construct_call<TLibraryCache>(TLibraryCache_Create);
                    }
                    try {
                        ScriptLibraryCache->InitFunction(Cell);
                    } catch (...) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to load function ", Name})));
                        Cell->ConvertToKind(EC_Expression::vkEmpty);
                    }
                } else {
                    if (TemporaryCell) {
                        pas::free(Cell);
                    }
                    pas::raise(pas::make_exception<pas::Exception>("Error. Script. Unknown variable format."_a));
                }
            }
            if (TemporaryCell) {
                pas::free(Cell);
                TemporaryCell = false;
            }
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
            Name = Buffer->ReadWideString();
            Star = GetStar(Name);
            Star->Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(EC_Buf::TBufEC_GetUInt32(Buffer))));
            {
                EC_Expression::TVarEC* var = InitCode->LocalVar->GetVar(Star->Name);
                std::uint32_t star = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Star->Star));
                var->SetDword(star);
            }
            SubCount = EC_Buf::TBufEC_GetInt32(Buffer);
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_6.next(K); ) {
                Name = Buffer->ReadWideString();
                PlanetBinding = GetPlanetBinding(Name);
                PlanetBinding->Planet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(EC_Buf::TBufEC_GetUInt32(Buffer), true)));
                {
                    EC_Expression::TVarEC* var_2 = InitCode->LocalVar->GetVar(PlanetBinding->Name);
                    std::uint32_t planet = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(PlanetBinding->Planet));
                    var_2->SetDword(planet);
                }
            }
            EC_Buf::TBufEC_GetInt32(Buffer);
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(I); ) {
            Name = Buffer->ReadWideString();
            if (Name == u"") {
                ScriptItem = pas::construct_call<TScriptItem>(TScriptItem_Create);
                ScriptItem->Script = this;
                pas::list_add(Items, reinterpret_cast<void*>(ScriptItem));
            } else {
                ScriptItem = GetItem(Name);
            }
            if (GlobalsV::LoadedSaveVersion >= 162) {
                ScriptItem->CanSell = EC_Buf::TBufEC_GetBoolean(Buffer);
            }
            if (GlobalsV::LoadedSaveVersion >= 123) {
                ScriptItem->Data[1] = EC_Buf::TBufEC_GetInt32(Buffer);
                ScriptItem->Data[2] = EC_Buf::TBufEC_GetInt32(Buffer);
                ScriptItem->Data[3] = EC_Buf::TBufEC_GetInt32(Buffer);
                ScriptItem->TextData1 = Buffer->ReadWideString();
                ScriptItem->TextData2 = Buffer->ReadWideString();
                ScriptItem->TextData3 = Buffer->ReadWideString();
            }
            if (GlobalsV::LoadedSaveVersion >= 69) {
                ScriptItem->OnUseText = Buffer->ReadWideString();
            } else {
                ScriptItem->OnUseText = pas::WideString();
            }
            if (GlobalsV::LoadedSaveVersion >= 87) {
                ScriptItem->OnActionText = Buffer->ReadWideString();
            } else {
                ScriptItem->OnActionText = pas::WideString();
            }
            if (Name != u"") {
                InitCode->LocalVar->GetVar(Name)->SetDword(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ScriptItem)));
            }
            try {
                ScriptItem->Item = pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(Galaxy->IdToItem(EC_Buf::TBufEC_GetUInt32(Buffer), true)));
            } catch (...) {
                auto cpp_exception_3 = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception_3)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to load script item <", Name, u"> for script ", ScriptFileName}))));
                } else {
                    throw;
                }
            }
            if (ScriptItem->Item != nullptr) {
                ScriptItem->Item->ScriptItem = ScriptItem;
            }
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(I); ) {
            Binding = pas::construct_call<TScriptShip>(TScriptShip_Create);
            pas::list_add(Ships, reinterpret_cast<void*>(Binding));
            Binding->Script = this;
            Binding->GroupIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            // Kept as an ID until ResolveLoadedReferences runs after ship loading.
            Binding->Ship = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            Binding->Data[0] = EC_Buf::TBufEC_GetUInt32(Buffer);
            Binding->Data[1] = EC_Buf::TBufEC_GetUInt32(Buffer);
            Binding->Data[2] = EC_Buf::TBufEC_GetUInt32(Buffer);
            Binding->Data[3] = EC_Buf::TBufEC_GetUInt32(Buffer);
            StateIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            if (StateIndex >= 0) {
                Binding->State = pas::list_at<TScriptState>(States, StateIndex);
            } else {
                Binding->State = nullptr;
            }
            if (GlobalsV::LoadedSaveVersion >= 149) {
                Binding->StateText = Buffer->ReadWideString();
                if (GlobalsV::LoadedSaveVersion < 151) {
                    EC_Buf::TBufEC_GetByte(Buffer);
                }
            } else {
                Binding->StateText = pas::WideString();
            }
            Binding->Hit = EC_Buf::TBufEC_GetBoolean(Buffer);
            Binding->HitPlayer = EC_Buf::TBufEC_GetBoolean(Buffer);
        }
        EtherIds->Clear();
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_9.next(I); ) {
            const pas::WideString& readWideString_2 = Buffer->ReadWideString();
            EC_Str::TStringsEC* etherIds = EtherIds;
            etherIds->Add(readWideString_2);
        }
    }

    void TScript::BindImportedFunctions() {
        std::int32_t I{};
        EC_Expression::TVarEC* Cell{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, InitCode->LocalVar->Count - 1); cpp_range.next(I); ) {
            Cell = EC_Expression::TVarArrayEC_GetItemByNameOrder(InitCode->LocalVar, I);
            if (Cell->RealVType() == EC_Expression::vkLibraryFun && Cell->GetString() != u"") {
                if (ScriptLibraryCache == nullptr) {
                    ScriptLibraryCache = pas::construct_call<TLibraryCache>(TLibraryCache_Create);
                }
                try {
                    ScriptLibraryCache->InitFunction(Cell);
                } catch (...) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to load function ", Cell->Name})));
                    Cell->ConvertToKind(EC_Expression::vkEmpty);
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, TurnCode->LocalVar->Count - 1); cpp_range_2.next(I); ) {
            Cell = EC_Expression::TVarArrayEC_GetItemByNameOrder(TurnCode->LocalVar, I);
            if (Cell->RealVType() == EC_Expression::vkLibraryFun && Cell->GetString() != u"") {
                if (ScriptLibraryCache == nullptr) {
                    ScriptLibraryCache = pas::construct_call<TLibraryCache>(TLibraryCache_Create);
                }
                try {
                    ScriptLibraryCache->InitFunction(Cell);
                } catch (...) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to load function ", Cell->Name})));
                    Cell->ConvertToKind(EC_Expression::vkEmpty);
                }
            }
        }
    }

    // Resolves saved ship IDs and restores ship, place, and state bindings after LoadState.
    void TScript::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        std::int32_t J{};
        TScriptShip* Binding{};
        TScriptState* State{};
        TScriptPlace* Place{};
        std::int32_t Count = pas::list_count(Ships);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Binding = pas::list_at<TScriptShip>(Ships, I);
            Binding->Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Binding->Ship)), true)));
            if (pas::class_cast_if<aPlayer::TPlayer*>(Binding->Ship) != nullptr) {
                pas::list_add(reinterpret_cast<aPlayer::TPlayer*>(Binding->Ship)->ScriptShipBindings, reinterpret_cast<void*>(Binding));
            } else {
                Binding->Ship->ScriptShip = Binding;
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Places) - 1); cpp_range_2.next(I); ) {
            Place = pas::list_at<TScriptPlace>(Places, I);
            {
                EC_Expression::TVarEC* var = InitCode->LocalVar->GetVar(Place->Name);
                std::uint32_t place = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Place));
                var->SetDword(place);
            }
            if (Place->OriginVarName != u"") {
                Place->OriginStar = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(InitCode->LocalVar->GetVar(Place->OriginVarName)->GetDword())));
            }
            if (Place->PlaceKind == spkCoordinates) {
                if (Place->TargetVarName != u"") {
                    Place->TargetValue = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(InitCode->LocalVar->GetVar(Place->TargetVarName)));
                }
                if (Place->TargetVarName2 != u"") {
                    Place->TargetValue2 = InitCode->LocalVar->GetVar(Place->TargetVarName2);
                }
            } else if (Place->TargetVarName != u"") {
                Place->TargetValue = InitCode->LocalVar->GetVar(Place->TargetVarName)->GetDword();
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(States) - 1); cpp_range_3.next(I); ) {
            State = pas::list_at<TScriptState>(States, I);
            if (State->TargetVarName != u"") {
                State->TargetValue = InitCode->LocalVar->GetVar(State->TargetVarName)->GetDword();
            }
            if (State->EnemyGroupIndices != nullptr) {
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, State->EnemyGroupIndices.length() - 1); cpp_range_4.next(J); ) {
                    State->EnemyGroupIndices[J] = InitCode->LocalVar->GetVar(State->EnemyGroupNames[J])->GetInt();
                }
            }
        }
    }

    void TLibraryHandler_Create(TLibraryHandler* Self, pas::WideString LibraryName, std::uint32_t ModuleHandle, EC_BlockPar::TBlockParEC* DefinitionBlock) {
        pas::object_create(Self);
        Self->LibraryName = std::move(LibraryName);
        Self->ModuleHandle = ModuleHandle;
        Self->DefinitionBlock = DefinitionBlock;
    }

    void TLibraryHandler_Destroy(TLibraryHandler* Self) {
        if (Self->ModuleHandle != 0) {
            WindowsImports::FreeLibrary(Self->ModuleHandle);
        }
        pas::object_destroy(Self);
    }

    void TLibraryHandler::InitFunction(EC_Expression::TVarEC* Cell) {
        pas::AnsiString cpp_text{};
        pas::WideString Name{};
        pas::WideString Text{};
        pas::WideString Kind{};
        std::int32_t I{};
        pas::DynArray<std::uint32_t> Signature{};
        Text = Cell->GetString();
        Name = EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get());
        Text = DefinitionBlock->GetParam(Name);
        std::int32_t Count = EC_Str::CountDelimitedPartsW(Text, u","_wref.get());
        if (Count < 2) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to init library function ", Name, u" from ", LibraryName}))));
        }
        void* Proc = ([&] {
            std::uint8_t* cpp_arg = (cpp_text = static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())), cpp_text.pchar());
            std::uint32_t moduleHandle = ModuleHandle;
            return WindowsImports::GetProcAddress(moduleHandle, cpp_arg);
        }());
        if (Proc == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to find library function ", Name, u" in ", LibraryName}))));
        }
        Signature.set_length(Count);
        Kind = EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get());
        if (Kind == u"int") {
            Signature[0] = EC_Expression::lvInt;
        } else if (Kind == u"dword") {
            Signature[0] = EC_Expression::lvDword;
        } else if (Kind == u"float") {
            Signature[0] = EC_Expression::lvFloat;
        } else if (Kind == u"str") {
            Signature[0] = EC_Expression::lvString;
        } else {
            Signature[0] = EC_Expression::lvVoid;
        }
        Signature[1] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Proc));
        for (auto cpp_range = pas::for_to<std::int32_t>(2, Count - 1); cpp_range.next(I); ) {
            Kind = EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get());
            if (Kind == u"int") {
                Signature[I] = EC_Expression::lvInt;
            } else if (Kind == u"dword") {
                Signature[I] = EC_Expression::lvDword;
            } else if (Kind == u"float") {
                Signature[I] = EC_Expression::lvFloat;
            } else if (Kind == u"str") {
                Signature[I] = EC_Expression::lvString;
            } else if (Kind == u"ref") {
                Signature[I] = EC_Expression::lvRef;
            } else if (Kind == u"code") {
                Signature[I] = EC_Expression::lvCode;
            } else {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to init library function ", Name, u" from ", LibraryName, u" - unknown type ", Kind}))));
            }
        }
        Cell->SetLibrarySignature(pas::open_array(Signature));
        Signature.set_length(0);
    }

    void TLibraryHandler::InitAllFunctions(EC_Expression::TVarArrayEC* Scope) {
        std::int32_t I{};
        pas::WideString Name{};
        EC_Expression::TVarEC* Cell{};
        std::int32_t Count = DefinitionBlock->GetParamCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Name = DefinitionBlock->GetParamName(I);
            if (Name == u"Path") {
                continue;
            }
            Cell = Scope->GetVarNE(Name);
            if (Cell == nullptr) {
                Cell = pas::construct_call<EC_Expression::TVarEC>(EC_Expression::TVarEC_Create, EC_Expression::vkLibraryFun);
                Cell->Name = Name;
                Scope->AddItem(Cell);
            }
            if (Cell->RealVType() == EC_Expression::vkLibraryFun) {
                Cell->SetString(pas::concat_wide({LibraryName, u",", Name}));
                InitFunction(Cell);
            }
        }
    }

    void TLibraryCache_Create(TLibraryCache* Self) {
        pas::object_create(Self);
        Self->Libraries = pas::make_object<aMyFunction::TObjectList>();
    }

    void TLibraryCache_Destroy(TLibraryCache* Self) {
        pas::free(Self->Libraries);
        pas::object_destroy(Self);
    }

    TLibraryHandler* TLibraryCache::GetLib(pas::WideString Name) {
        TLibraryHandler* Result{};
        std::int32_t Middle{};
        TLibraryHandler* Entry{};
        // Nested helper; captures the library name at ParentFrame-4. Returns nil when its ScriptLibs block is absent.
        auto LoadHandler = [&]() -> TLibraryHandler* {
            pas::WideString cpp_text{};
            TLibraryHandler* Result = nullptr;
            EC_BlockPar::TBlockParEC* Definition = GR_Main::GameDataConfig->FindBlockByPath(pas::concat_wide({u"ScriptLibs.", Name}));
            if (Definition == nullptr) {
                return Result;
            }
            std::uint32_t Module = WindowsSdk::LoadLibraryW((cpp_text = Definition->GetParam(u"Path"_wref.get()), cpp_text.pchar()));
            if (Module == 0) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Failed to load library ", Name}))));
            }
            return pas::construct_call<TLibraryHandler>(TLibraryHandler_Create, Name, Module, Definition);
        };
        if (pas::list_count(Libraries) < 1) {
            Result = LoadHandler();
            pas::list_add(Libraries, reinterpret_cast<void*>(Result));
            return Result;
        }
        std::int32_t LowIndex = 0;
        Entry = pas::list_at<TLibraryHandler>(Libraries, 0);
        std::int32_t Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->LibraryName.pchar());
        if (Comparison == 0) {
            return Entry;
        }
        if (Comparison < 0) {
            Result = LoadHandler();
            pas::list_insert(Libraries, 0, reinterpret_cast<void*>(Result));
            return Result;
        }
        std::int32_t HighIndex = pas::list_count(Libraries) - 1;
        Entry = pas::list_at<TLibraryHandler>(Libraries, HighIndex);
        Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->LibraryName.pchar());
        if (Comparison == 0) {
            return Entry;
        }
        if (Comparison > 0) {
            Result = LoadHandler();
            pas::list_add(Libraries, reinterpret_cast<void*>(Result));
            return Result;
        }
        while (true) {
            if (HighIndex - LowIndex < 2) {
                Result = LoadHandler();
                pas::list_insert(Libraries, HighIndex, reinterpret_cast<void*>(Result));
                return Result;
            }
            Middle = (LowIndex + HighIndex) / 2;
            Entry = pas::list_at<TLibraryHandler>(Libraries, Middle);
            Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->LibraryName.pchar());
            if (Comparison == 0) {
                return Entry;
            }
            if (Comparison < 0) {
                HighIndex = Middle;
            } else {
                LowIndex = Middle;
            }
        }
        return Result;
    }

    void TLibraryCache::InitFunction(EC_Expression::TVarEC* Cell) {
        TLibraryHandler* lib = GetLib(EC_Str::ExtractDelimitedPartW(Cell->GetString(), 0, u","_wref.get()));
        EC_Expression::TVarEC* cell = Cell;
        lib->InitFunction(cell);
    }

    void TScriptCache_Create(TScriptCache* Self) {
        pas::object_create(Self);
        Self->Entries = pas::make_object<aMyFunction::TObjectList>();
    }

    void TScriptGICache_Create(TScriptGICache* Self) {
        pas::object_create(Self);
        Self->Entries = pas::make_object<aMyFunction::TObjectList>();
    }

    void TScriptCache_Destroy(TScriptCache* Self) {
        pas::free(Self->Entries);
        Self->Entries = nullptr;
        pas::object_destroy(Self);
    }

    void TScriptGICache_Destroy(TScriptGICache* Self) {
        pas::free(Self->Entries);
        Self->Entries = nullptr;
        pas::object_destroy(Self);
    }

    // Returns nil for empty source.
    TScriptGICacheUnit* TScriptGICache::GetOrCompile(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t Middle{};
        TScriptGICacheUnit* Entry{};
        // Nested helper; captures Block at ParentFrame-4.
        auto CreateUiCacheEntry = [&]() -> TScriptGICacheUnit* {
            pas::WideString Text{};
            TScriptGICacheUnit* Result = nullptr;
            if (Block == nullptr) {
                return Result;
            }
            Text = Block->ConcatenateValues();
            if (Text == u"") {
                return Result;
            }
            Result = pas::construct_call<TScriptGICacheUnit>(TScriptGICacheUnit_Create);
            Result->Initialize(Block, Text);
            return Result;
        };
        TScriptGICacheUnit* Result = nullptr;
        if (pas::list_count(Entries) < 1) {
            Entry = CreateUiCacheEntry();
            if (Entry != nullptr) {
                pas::list_add(Entries, reinterpret_cast<void*>(Entry));
                return Entry;
            }
            return Result;
        }
        std::uint32_t Key = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Block));
        std::int32_t LowIndex = 0;
        Entry = pas::list_at<TScriptGICacheUnit>(Entries, 0);
        if (Entry->Block == Block) {
            return Entry;
        }
        if (Key < static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Entry->Block))) {
            Entry = CreateUiCacheEntry();
            if (Entry != nullptr) {
                pas::list_insert(Entries, 0, reinterpret_cast<void*>(Entry));
                return Entry;
            }
            return Result;
        }
        std::int32_t HighIndex = pas::list_count(Entries) - 1;
        Entry = pas::list_at<TScriptGICacheUnit>(Entries, HighIndex);
        if (Entry->Block == Block) {
            return Entry;
        }
        if (Key > static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Entry->Block))) {
            Entry = CreateUiCacheEntry();
            if (Entry != nullptr) {
                pas::list_add(Entries, reinterpret_cast<void*>(Entry));
                return Entry;
            }
            return Result;
        }
        while (true) {
            if (HighIndex - LowIndex < 2) {
                Entry = CreateUiCacheEntry();
                if (Entry != nullptr) {
                    pas::list_insert(Entries, HighIndex, reinterpret_cast<void*>(Entry));
                    return Entry;
                }
                return Result;
            }
            Middle = (LowIndex + HighIndex) / 2;
            Entry = pas::list_at<TScriptGICacheUnit>(Entries, Middle);
            if (Entry->Block == Block) {
                return Entry;
            }
            if (Key < static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Entry->Block))) {
                HighIndex = Middle;
            } else {
                LowIndex = Middle;
            }
        }
        return Result;
    }

    // Returns nil for absent or empty OnActCode.
    TScriptCacheUnit* TScriptCache::GetOrCompile(pas::WideString Name, EC_BlockPar::TBlockParEC* Config) {
        std::int32_t Middle{};
        TScriptCacheUnit* Entry{};
        EC_BlockPar::TBlockParEC* SourceBlock{};
        // Nested helper; captures source block, config, and name at ParentFrame-4, -8, and -12. Caller owns a non-nil result.
        auto CreateEntry = [&]() -> TScriptCacheUnit* {
            pas::WideString Text{};
            pas::WideString ActionTypes{};
            pas::WideString StepTypes{};
            TScriptCacheUnit* Result = nullptr;
            Text = SourceBlock->ConcatenateValues();
            if (Text == u"") {
                return Result;
            }
            Result = pas::construct_call<TScriptCacheUnit>(TScriptCacheUnit_Create);
            if (Config->CountParams(u"OnActCodeTypes"_wref.get()) > 0) {
                ActionTypes = Config->GetParam(u"OnActCodeTypes"_wref.get());
            } else {
                ActionTypes = pas::WideString();
            }
            if (Config->CountParams(u"OnActStepTypes"_wref.get()) > 0) {
                StepTypes = Config->GetParam(u"OnActStepTypes"_wref.get());
            } else {
                StepTypes = pas::WideString();
            }
            Result->Initialize(Name, Text, ActionTypes, StepTypes);
            return Result;
        };
        TScriptCacheUnit* Result = nullptr;
        SourceBlock = Config->FindBlock(u"OnActCode"_wref.get());
        if (SourceBlock == nullptr) {
            return Result;
        }
        if (pas::list_count(Entries) < 1) {
            Entry = CreateEntry();
            if (Entry != nullptr) {
                pas::list_add(Entries, reinterpret_cast<void*>(Entry));
                return Entry;
            }
            return Result;
        }
        std::int32_t LowIndex = 0;
        Entry = pas::list_at<TScriptCacheUnit>(Entries, 0);
        std::int32_t Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->Name.pchar());
        if (Comparison == 0) {
            return Entry;
        }
        if (Comparison < 0) {
            Entry = CreateEntry();
            if (Entry != nullptr) {
                pas::list_insert(Entries, 0, reinterpret_cast<void*>(Entry));
                return Entry;
            }
            return Result;
        }
        std::int32_t HighIndex = pas::list_count(Entries) - 1;
        Entry = pas::list_at<TScriptCacheUnit>(Entries, HighIndex);
        Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->Name.pchar());
        if (Comparison == 0) {
            return Entry;
        }
        if (Comparison > 0) {
            Entry = CreateEntry();
            if (Entry != nullptr) {
                pas::list_add(Entries, reinterpret_cast<void*>(Entry));
                return Entry;
            }
            return Result;
        }
        while (true) {
            if (HighIndex - LowIndex < 2) {
                Entry = CreateEntry();
                if (Entry != nullptr) {
                    pas::list_insert(Entries, HighIndex, reinterpret_cast<void*>(Entry));
                    return Entry;
                }
                return Result;
            }
            Middle = (LowIndex + HighIndex) / 2;
            Entry = pas::list_at<TScriptCacheUnit>(Entries, Middle);
            Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->Name.pchar());
            if (Comparison == 0) {
                return Entry;
            }
            if (Comparison < 0) {
                HighIndex = Middle;
            } else {
                LowIndex = Middle;
            }
        }
        return Result;
    }

    void TScriptCacheUnit_Create(TScriptCacheUnit* Self) {
        pas::object_create(Self);
        Self->Code = nullptr;
    }

    void TScriptCacheUnit_Destroy(TScriptCacheUnit* Self) {
        if (Self->Code != nullptr) {
            pas::free(Self->Code);
        }
        Self->Code = nullptr;
        pas::object_destroy(Self);
    }

    void TScriptCacheUnit::Initialize(pas::WideString Name, pas::WideString SourceText, pas::WideString ActionTypes, pas::WideString StepTypes) {
        std::uint8_t Action{};
        std::int32_t I{};
        std::uint32_t Step{};
        std::int32_t Count{};
        this->Name = std::move(Name);
        this->SourceText = SourceText;
        Code = aScript::CompileScriptText(SourceText);
        if (ActionTypes == u"" && StepTypes == u"" || ActionTypes == u"Any") {
            ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep, aConst::satOnDeath}});
        } else if (ActionTypes == u"") {
            ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep}});
        } else {
            if (StepTypes != u"") {
                ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep}});
            } else {
                ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({});
            }
            ActionTypes = pas::concat_wide({u",", ActionTypes, u","});
            for (Action = static_cast<std::uint8_t>(0); Action <= static_cast<std::uint8_t>(61); ++Action) {
                if (pas::pos(pas::concat_wide({u",", aConst::ScriptActionTypeNames[Action], u","}), ActionTypes) > 0) {
                    pas::include_at(&ActionTypeMask, Action);
                }
            }
        }
        if (StepTypes == u"" || StepTypes == u"Any") {
            StepTypeMask = pas::constant_set<TScriptStepTypeSet>({{0, 11}});
        } else {
            StepTypeMask = pas::constant_set<TScriptStepTypeSet>({});
            Count = EC_Str::CountDelimitedPartsW(StepTypes, u","_wref.get());
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Step = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(StepTypes, I, u","_wref.get()));
                if (pas::in_range(Step, 0, 11)) {
                    pas::include_at(&StepTypeMask, Step);
                }
            }
        }
    }

    void TScriptGICacheUnit_Create(TScriptGICacheUnit* Self) {
        pas::object_create(Self);
        Self->Code = nullptr;
    }

    void TScriptGICacheUnit_Destroy(TScriptGICacheUnit* Self) {
        if (Self->Code != nullptr) {
            pas::free(Self->Code);
        }
        Self->Code = nullptr;
        pas::object_destroy(Self);
    }

    void TScriptGICacheUnit::Initialize(EC_BlockPar::TBlockParEC* Block, pas::WideString SourceText) {
        this->Block = Block;
        this->SourceText = SourceText;
        Code = aScript::CompileScriptText(SourceText);
    }

    // Requires nonempty OnActionText and an empty ActionCode slot.
    void TScriptItem::CompileActionCode() {
        std::int32_t I{};
        std::uint32_t Step{};
        std::int32_t Count{};
        std::uint8_t Action{};
        pas::WideString SourceText{};
        pas::WideString ActionTypes{};
        pas::WideString StepTypes{};
        ActionCodeInitialized = true;
        if (OnActionText.read(1) == u'[') {
            I = EC_Str::FindTextPosW(u"]"_wref.get(), OnActionText);
            SourceText = EC_Str::CopyWideStringUnchecked(OnActionText, I + 1, OnActionText.length() - I);
            ActionTypes = EC_Str::CopyWideStringUnchecked(OnActionText, 2, I - 2);
            StepTypes = EC_Str::ExtractDelimitedPartW(ActionTypes, 1, u"|"_wref.get());
            ActionTypes = EC_Str::ExtractDelimitedPartW(ActionTypes, 0, u"|"_wref.get());
            ActionCode = aScript::CompileScriptText(SourceText);
            ActionCode->LinkAll(ScriptFunctionScope, false);
            ActionCode->LinkAll(Globals::SharedScriptVariables, false);
            if (Script != nullptr) {
                ActionCode->LinkAll(Script->InitCode->LocalVar, false);
            }
            ActionCode->ScriptFunLinked = true;
            if (ActionTypes == u"" && StepTypes == u"" || ActionTypes == u"Any") {
                ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep, aConst::satOnDeath}});
            } else if (ActionTypes == u"") {
                ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep}});
            } else {
                if (StepTypes != u"") {
                    ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep}});
                } else {
                    ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({});
                }
                ActionTypes = pas::concat_wide({u",", ActionTypes, u","});
                for (Action = static_cast<std::uint8_t>(0); Action <= static_cast<std::uint8_t>(61); ++Action) {
                    if (pas::pos(pas::concat_wide({u",", aConst::ScriptActionTypeNames[Action], u","}), ActionTypes) > 0) {
                        pas::include_at(&ActionTypeMask, Action);
                    }
                }
            }
            if (StepTypes == u"" || StepTypes == u"Any") {
                StepTypeMask = pas::constant_set<TScriptStepTypeSet>({{0, 11}});
            } else {
                StepTypeMask = pas::constant_set<TScriptStepTypeSet>({});
                Count = EC_Str::CountDelimitedPartsW(StepTypes, u","_wref.get());
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                    Step = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(StepTypes, I, u","_wref.get()));
                    if (pas::in_range(Step, 0, 11)) {
                        pas::include_at(&StepTypeMask, Step);
                    }
                }
            }
        } else {
            ActionCode = aScript::CompileScriptText(OnActionText);
            ActionTypeMask = pas::constant_set<TScriptActionTypeSet>({{aConst::satOnStep, aConst::satOnDeath}});
            StepTypeMask = pas::constant_set<TScriptStepTypeSet>({{0, 11}});
        }
    }

    // Returns the event parameter after script changes. Object slots can carry event-specific integers.
    std::int32_t TScriptItem::RunActionCode(std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param) {
        TScriptContextSnapshot Snapshot{};
        std::int32_t Result = Param;
        if (ActionCode != nullptr && static_cast<std::uint8_t>(ActionCodeInitialized ^ 1)) {
            pas::free(ActionCode);
            ActionCode = nullptr;
        }
        if (OnActionText != u"") {
            try {
                if (ActionCode == nullptr) {
                    CompileActionCode();
                }
                if (!pas::contains(ActionTypeMask, ActionType)) {
                    return Result;
                }
                if (ActionType == aConst::satOnStep && static_cast<std::uint8_t>(pas::contains(StepTypeMask, Param) ^ 1)) {
                    return Result;
                }
                if (Script != nullptr) {
                    aScript::ScriptSnap(Snapshot);
                    Script->PublishCurrentShip(Ship);
                    pas::list_add(Globals::ScriptItemContextStack, reinterpret_cast<void*>(Item));
                    pas::list_add(Globals::ScriptItemInfoContextStack, nullptr);
                    pas::list_add(Globals::ScriptActionTypeStack, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(ActionType))));
                    pas::list_add(Globals::ScriptActionObject1Stack, reinterpret_cast<void*>(Object1));
                    pas::list_add(Globals::ScriptActionObject2Stack, reinterpret_cast<void*>(Object2));
                    pas::list_add(Globals::ScriptActionParamStack, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Param))));
                    pas::list_add(Globals::ScriptActionShipStack, reinterpret_cast<void*>(Ship));
                    aScript::RunScriptCode(OnActionText, ActionCode, Script->InitCode);
                    aScript::ScriptUnSnap(Snapshot);
                    Result = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1)));
                    pas::list_delete(Globals::ScriptActionTypeStack, pas::list_count(Globals::ScriptActionTypeStack) - 1);
                    pas::list_delete(Globals::ScriptActionObject1Stack, pas::list_count(Globals::ScriptActionObject1Stack) - 1);
                    pas::list_delete(Globals::ScriptActionObject2Stack, pas::list_count(Globals::ScriptActionObject2Stack) - 1);
                    pas::list_delete(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1);
                    pas::list_delete(Globals::ScriptActionShipStack, pas::list_count(Globals::ScriptActionShipStack) - 1);
                    pas::list_delete(Globals::ScriptItemInfoContextStack, pas::list_count(Globals::ScriptItemInfoContextStack) - 1);
                    pas::list_delete(Globals::ScriptItemContextStack, pas::list_count(Globals::ScriptItemContextStack) - 1);
                    return Result;
                }
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    GR_Main::AppendLogLineThreadSafe("Error in script item actcode, item="_a);
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({Item->GetDisplayName(), u", script name - ", Name})));
                    throw;
                } else {
                    throw;
                }
            }
        }
        return Result;
    }

    // Returns the event parameter after script changes. Object slots can carry event-specific integers.
    std::int32_t TScriptShip::RunActionCode(std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param) {
        TScriptContextSnapshot Snapshot{};
        std::int32_t Result = Param;
        if (State != nullptr && State->OnActionText != u"") {
            if (pas::contains(State->ActionTypeMask, ActionType)) {
                if (ActionType != aConst::satOnStep || pas::contains(State->StepTypeMask, Param)) {
                    try {
                        aScript::ScriptSnap(Snapshot);
                        Script->PublishCurrentShip(Ship);
                        pas::list_add(Globals::ScriptItemContextStack, nullptr);
                        pas::list_add(Globals::ScriptItemInfoContextStack, nullptr);
                        pas::list_add(Globals::ScriptActionTypeStack, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(ActionType))));
                        pas::list_add(Globals::ScriptActionObject1Stack, reinterpret_cast<void*>(Object1));
                        pas::list_add(Globals::ScriptActionObject2Stack, reinterpret_cast<void*>(Object2));
                        pas::list_add(Globals::ScriptActionParamStack, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Param))));
                        pas::list_add(Globals::ScriptActionShipStack, reinterpret_cast<void*>(Ship));
                        aScript::RunScriptCode(State->OnActionText, State->ActionCode, Script->InitCode);
                        aScript::ScriptUnSnap(Snapshot);
                        Result = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1)));
                        pas::list_delete(Globals::ScriptActionTypeStack, pas::list_count(Globals::ScriptActionTypeStack) - 1);
                        pas::list_delete(Globals::ScriptActionObject1Stack, pas::list_count(Globals::ScriptActionObject1Stack) - 1);
                        pas::list_delete(Globals::ScriptActionObject2Stack, pas::list_count(Globals::ScriptActionObject2Stack) - 1);
                        pas::list_delete(Globals::ScriptActionParamStack, pas::list_count(Globals::ScriptActionParamStack) - 1);
                        pas::list_delete(Globals::ScriptActionShipStack, pas::list_count(Globals::ScriptActionShipStack) - 1);
                        pas::list_delete(Globals::ScriptItemInfoContextStack, pas::list_count(Globals::ScriptItemInfoContextStack) - 1);
                        pas::list_delete(Globals::ScriptItemContextStack, pas::list_count(Globals::ScriptItemContextStack) - 1);
                        // Native Exit leaves the compiler's dormant normal SEH unlink.
                        return Result;
                    } catch (...) {
                        auto cpp_exception = pas::caught_object();
                        if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                            GR_Main::AppendLogLineThreadSafe("Error in script ship actcode"_a);
                            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({State->Name, u", script - ", Script->ScriptFileName})));
                            throw;
                        } else {
                            throw;
                        }
                    }
                }
            }
        }
        return Result;
    }

    pas::WideString TScriptItem::FormatDataText(pas::WideString Text, pas::WideString ColorTag) {
        pas::WideString Result{};
        Result = std::move(Text);
        aMyFunction::ReplaceTextToken(Result, u"<Data1>"_w, pas::wide_int_to_str(Data[1]), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<Data2>"_w, pas::wide_int_to_str(Data[2]), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<Data3>"_w, pas::wide_int_to_str(Data[3]), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<TextData1>"_w, TextData1, ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<TextData2>"_w, TextData2, ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<TextData3>"_w, TextData3, ColorTag);
        return Result;
    }

    void TScriptStar::p_destroy() {
        aScript::TScriptStar_Destroy(this);
    }

    void TScriptConstellation::p_destroy() {
        aScript::TScriptConstellation_Destroy(this);
    }

    void TScriptShip::p_destroy() {
        aScript::TScriptShip_Destroy(this);
    }

    void TScriptPlace::p_destroy() {
        aScript::TScriptPlace_Destroy(this);
    }

    void TScriptItem::p_destroy() {
        aScript::TScriptItem_Destroy(this);
    }

    void TScriptGroup::p_destroy() {
        aScript::TScriptGroup_Destroy(this);
    }

    void TScriptState::p_destroy() {
        aScript::TScriptState_Destroy(this);
    }

    void TScriptDialog::p_destroy() {
        aScript::TScriptDialog_Destroy(this);
    }

    void TScriptDialogMsg::p_destroy() {
        aScript::TScriptDialogMsg_Destroy(this);
    }

    void TScriptDialogAnswer::p_destroy() {
        aScript::TScriptDialogAnswer_Destroy(this);
    }

    void TScript::p_destroy() {
        aScript::TScript_Destroy(this);
    }

    void TScriptCacheUnit::p_destroy() {
        aScript::TScriptCacheUnit_Destroy(this);
    }

    void TScriptCache::p_destroy() {
        aScript::TScriptCache_Destroy(this);
    }

    void TScriptGICacheUnit::p_destroy() {
        aScript::TScriptGICacheUnit_Destroy(this);
    }

    void TScriptGICache::p_destroy() {
        aScript::TScriptGICache_Destroy(this);
    }

    void TLibraryHandler::p_destroy() {
        aScript::TLibraryHandler_Destroy(this);
    }

    void TLibraryCache::p_destroy() {
        aScript::TLibraryCache_Destroy(this);
    }

    void TScriptThread::virtual_TThreadEC_Execute() {
        aScript::TScriptThread_Execute(this);
    }

} // namespace aScript
