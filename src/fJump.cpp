#include "layout/fJump.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_XviD.hpp"
#include "types/GR_Music.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aRuins.hpp"
#include "types/aShip.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/aCalc.hpp"
#include "units/aGalaxy.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/fJump.hpp"
#include "units/fLoad.hpp"
#include "units/fPanelLoad.hpp"

namespace fJump {
    void BeginTravel(TfJump* Self);

    void TfJump_Create(TfJump* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfJump_Destroy(TfJump* Self) {
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfJump::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fJump... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* cpp_with = GetByName(u""_wref.get());
            cpp_with->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            cpp_with->FindByNameRecursive(u"Film"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        RestoreOrdersOnArrival = false;
    }

    void TfJump::OnOpen() {
        pas::WideString MovieConfig{};
        pas::WideString MoviePath{};
        EC_Cache::EvictStarAndBackgroundCaches();
        GR_DX::ReleaseAllTextureSurfaces();
        if (!aPlayer::GetPlayer()->IsDockedToShip()) {
            SetCursorActive(true);
            LoadPanel->OnOpen();
            aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 2);
            LoadingStarted = false;
            NoPendingLoads = false;
            if (aPlayer::GetPlayer()->InHyperspace || aPlayer::GetPlayer()->IsDockedToShip()) {
                TransitionTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfJump::AdvanceTravel>(this), 0);
            } else {
                AdvanceLoading(nullptr, 0);
            }
            Progress = 0.0f;
            LoadPanel->SetProgress(0.0f);
            LoadPanel->Show();
            Present();
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            SetCursorActive(false);
            LoadPanel->OnOpen();
            aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 2);
            LoadingStarted = false;
            NoPendingLoads = false;
            Progress = 0.0f;
            Present();
            if (Globals::SkipVideo) {
                fJump::BeginTravel(this);
                return;
            }
            LoadPanel->SetProgress(1.0f);
            LoadPanel->Hide();
            if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase)) {
                MovieConfig = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormRuins.WB.HyperJumpVideo"_wref.get());
            } else if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion)) {
                MovieConfig = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormRuins.CB.HyperJumpVideo"_wref.get());
            } else {
                fJump::BeginTravel(this);
                return;
            }
            MoviePath = EC_Str::ExtractDelimitedPartW(MovieConfig, 0, u","_wref.get());
            {
                GI_XviD::TxvidGI* Film = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"_wref.get()));
                Film->SetActive(true);
                if (Film->ImageOpen(MoviePath, true)) {
                    if (GlobalsV::MusicEnabled && EC_Str::CountDelimitedPartsW(MovieConfig, u","_wref.get()) > 1) {
                        GR_Main::MusicManager->StopImmediately();
                        while (GR_Main::MusicManager->IsPlaying()) {
                            SysUtilsImports::Sleep(1u);
                        }
                        GR_Main::MusicManager->PlayCategory(EC_Str::ExtractDelimitedPartW(MovieConfig, 1, u","_wref.get()));
                        while (!GR_Main::MusicManager->IsPlaying()) {
                            SysUtilsImports::Sleep(1u);
                        }
                    }
                    MovieStartTick = MMSystem::timeGetTime();
                    if (MovieTimer != nullptr) {
                        CancelCallbackTimer(MovieTimer);
                        MovieTimer = nullptr;
                    }
                    MovieTimer = ScheduleCallbackTimer(5, 5, pas::bind_method<&TfJump::AdvanceMovie>(this), 0);
                } else {
                    Film->SetActive(false);
                    fJump::BeginTravel(this);
                }
            }
        }
    }

    void BeginTravel(TfJump* Self) {
        if (Self->TransitionTimer != nullptr) {
            Self->CancelCallbackTimer(Self->TransitionTimer);
            Self->TransitionTimer = nullptr;
        }
        Self->TransitionTimer = Self->ScheduleCallbackTimer(20, 20, pas::bind_method<&TfJump::AdvanceTravel>(Self), 0);
        Self->LoadPanel->SetProgress(0.0f);
        Self->LoadPanel->Show();
    }

    void TfJump::OnClose() {
        StopMovie();
        {
            GI_XviD::TxvidGI* Film = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"_wref.get()));
            Film->ImageClose();
            Film->SetActive(false);
        }
        LoadPanel->OnClose();
        if (TransitionTimer != nullptr) {
            CancelCallbackTimer(TransitionTimer);
            TransitionTimer = nullptr;
        }
    }

    void TfJump::AdvanceTravel(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        aGalaxy::TStar* PreviousStar{};
        if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->GetHull()->HullPoints <= 0) {
            if (aPlayer::GetPlayer() != nullptr) {
                aGalaxy::Galaxy->ScoreScreenDismissed = 1;
            }
            while (aPlayer::GetPlayer() != nullptr) {
                SysUtilsImports::Sleep(1u);
            }
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
            RequestClose(1);
            return;
        }
        Progress = Progress + 0.008L;
        if (Progress > 0.49L) {
            Progress = 0.5f;
        }
        LoadPanel->SetProgress(Progress);
        if (aCalc::IsTurnCalculationRunningUI() || pas::in_set<1, 1, 3, 3>(aCalc::TurnCalculationPhase)) {
            return;
        }
        if (aCalc::TurnCalculationPhase == ThreadCalc::tcpGalaxyFinished) {
            aCalc::QueuePlayerStarTurnCalculation();
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip()) {
            if (aPlayer::GetPlayer()->DockedTo->Order != aShip::soTeleport && (pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar == aPlayer::GetPlayer()->DockedTo->CurrentStar || pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar == nullptr)) {
                aCalc::QueueGalaxyTurnCalculation();
                AdvanceLoading(nullptr, 0);
                return;
            }
            aGalaxy::Galaxy->ClearJumpGates();
            PreviousStar = aGalaxy::PlayerStar;
            aGalaxy::PlayerStar = aPlayer::GetPlayer()->CurrentStar;
            aGalaxy::PlayerStar->RebuildShipMovementPaths();
            PreviousStar->RebuildShipMovementPaths();
            if ((static_cast<std::uint32_t>(aPlayer::GetPlayer()->OrderStateData) & 0x0000ffff) == 1) {
                Globals::PruneExpiredPersistentPlayerMessages();
                aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 3);
            }
            aGalaxy::Galaxy->GenerateSpaceBackground(aPlayer::GetPlayer()->CurrentStar->BackgroundImage);
            aCalc::QueueGalaxyTurnCalculation();
            Present();
        } else {
            if (static_cast<std::uint8_t>(pas::in_set<3, 4, 7, 7>(aPlayer::GetPlayer()->Order) ^ 1) || aPlayer::GetPlayer()->Order == aShip::soJumpHole && aPlayer::GetPlayer()->OrderStateData == -65536) {
                aCalc::QueueGalaxyTurnCalculation();
                Globals::StarMapScreen->SetMapCenterManually(EC_Struct::TruncatePointF(aPlayer::GetPlayer()->Position));
                Globals::StarMapScreen->ResumeMode = fStarMap::smrTurnFilm;
                AdvanceLoading(nullptr, 0);
                return;
            }
            aGalaxy::Galaxy->ClearJumpGates();
            PreviousStar = aGalaxy::PlayerStar;
            aGalaxy::PlayerStar = aPlayer::GetPlayer()->CurrentStar;
            aGalaxy::PlayerStar->RebuildShipMovementPaths();
            PreviousStar->RebuildShipMovementPaths();
            if ((static_cast<std::uint32_t>(aPlayer::GetPlayer()->OrderStateData) & 0x0000ffff) == 1) {
                Globals::PruneExpiredPersistentPlayerMessages();
                aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 3);
            }
            aGalaxy::Galaxy->GenerateSpaceBackground(aPlayer::GetPlayer()->CurrentStar->BackgroundImage);
            aCalc::QueueGalaxyTurnCalculation();
            Present();
        }
    }

    void TfJump::AdvanceLoading(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        pas::List* Loads{};
        if (!LoadingStarted) {
            LoadingStarted = true;
            if (TransitionTimer != nullptr) {
                CancelCallbackTimer(TransitionTimer);
                TransitionTimer = nullptr;
            }
            Loads = pas::make_object<pas::List>();
            fLoad::QueueSpaceLoadingAssets(Loads, RootUiObject);
            if (pas::list_count(Loads) > 0) {
                Globals::CacheLoader->SetPendingLoads(Loads, true);
                TransitionTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfJump::AdvanceLoading>(this), 0);
            } else {
                TransitionTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfJump::AdvanceLoading>(this), 0);
                NoPendingLoads = true;
                pas::free(Loads);
            }
        } else {
            if (NoPendingLoads) {
                Progress = Progress + 0.008L;
            } else {
                Progress = pas::real_min<pas::Extended>(Progress + 0.008L, pas::real_divide(Globals::CacheLoader->CompletedLoadCount, Globals::CacheLoader->TotalLoadCount) * 0.5L + 0.5L);
            }
            if (Progress > 0.99L) {
                Progress = 1.0f;
            }
            LoadPanel->SetProgress(Progress);
            if ((NoPendingLoads || static_cast<std::uint8_t>(Globals::CacheLoader->IsRunning() ^ 1)) && Progress >= 1.0L) {
                if (TransitionTimer != nullptr) {
                    CancelCallbackTimer(TransitionTimer);
                    TransitionTimer = nullptr;
                }
                if (aPlayer::GetPlayer()->IsDockedToShip()) {
                    Globals::RuinsTalkScreen->ShowArrivalVideo = true;
                    GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
                } else {
                    GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
                    if (RestoreOrdersOnArrival) {
                        Globals::StarMapScreen->ResumeMode = fStarMap::smrOrders;
                        Globals::SpaceViewPosition = aPlayer::GetPlayer()->Position;
                        RestoreOrdersOnArrival = false;
                    }
                }
                RequestClose(1);
            }
        }
    }

    void TfJump::AdvanceMovie(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (([&] {
            std::uint32_t cpp_left = MMSystem::timeGetTime();
            double cpp_arg = cpp_left - MovieStartTick;
            GI_XviD::TxvidGI* cpp_arg_2 = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"_wref.get()));
            return cpp_arg_2->SetPlaybackTime(cpp_arg);
        }())) {
            StopMovie();
        }
    }

    std::uint8_t TfJump::StopMovie() {
        std::uint8_t Result = MovieTimer != nullptr;
        if (GlobalsV::MusicEnabled && Result) {
            GR_Main::MusicManager->StopImmediately();
        }
        if (MovieTimer != nullptr) {
            CancelCallbackTimer(MovieTimer);
            MovieTimer = nullptr;
        }
        InvalidateViewport();
        if (TransitionTimer != nullptr) {
            CancelCallbackTimer(TransitionTimer);
            TransitionTimer = nullptr;
        }
        TransitionTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfJump::AdvanceTravel>(this), 0);
        return Result;
    }

    void TfJump::SelectMusic() {
    }

    void TfJump::p_destroy() {
        fJump::TfJump_Destroy(this);
    }

} // namespace fJump
