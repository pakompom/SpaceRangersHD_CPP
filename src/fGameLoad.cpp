#include "layout/fGameLoad.hpp"
#include "types/EC_Buf.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Thread.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/fGameLoad.hpp"
#include "units/fLoad.hpp"
#include "units/fPanelLoad.hpp"

namespace fGameLoad {
    void TThreadGameLoad_Execute(TThreadGameLoad* Self) {
        Self->Succeeded = false;
        Self->Succeeded = aSaveLoad::LoadGameFromFile(static_cast<pas::WideString>(GlobalsV::PendingLoadFileName));
    }

    void TfGameLoad_Create(TfGameLoad* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfGameLoad_Destroy(TfGameLoad* Self) {
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfGameLoad::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fGameLoad... "_a);
        ViewportRect = ClassesImports::Rect(GR_Main::ExtraScreenWidth / 2, GR_Main::ExtraScreenHeight / 2, ViewportRect.Left + GR_Main::ExtraScreenWidth / 2, ViewportRect.Top + GR_Main::ExtraScreenHeight / 2);
        GetByName(u"PanelLoad"sv)->Parent->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        LoadPanel->InitializeLayout(this);
    }

    void TfGameLoad::OnOpen() {
        LoadPanel->OnOpen();
        LoadPanel->Show();
        if (GR_Main::MusicManager->CategoryOverride == u"") {
            GR_Main::MusicManager->RequestFadeOut();
        }
        TargetProgress = 0.0f;
        DisplayedProgress = 0.0f;
        LoadingComplete = false;
        if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
            pas::free(aSaveLoad::MemorySnapshotBuffer);
        }
        aSaveLoad::MemorySnapshotBuffer = nullptr;
        GlobalsV::MemorySnapshotActive = false;
        if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
            pas::free(aGalaxy::Galaxy);
        }
        aGalaxy::Galaxy = nullptr;
        AssetPreloadStarted = false;
        LoadThread = pas::construct_call<TThreadGameLoad>(EC_Thread::TThreadEC_Create);
        LoadThread->SetPriority(2);
        LoadThread->Start();
        ProgressTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfGameLoad::UpdateLoadingProgress>(this), 0);
        LoadPanel->SetProgress(0.0f);
    }

    void TfGameLoad::OnClose() {
        pas::WideString Category{};
        LoadPanel->OnClose();
        if (LoadThread != nullptr) {
            pas::free(LoadThread);
            LoadThread = nullptr;
        }
        if (ProgressTimer != nullptr) {
            CancelCallbackTimer(ProgressTimer);
            ProgressTimer = nullptr;
        }
        if (GR_Main::MusicManager->CategoryOverride == u"") {
            if (aPlayer::GetPlayer() == nullptr) {
                GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
            } else if (aPlayer::GetPlayer()->IsOnPlanet()) {
                if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                    if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                        GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId)].InternalName, u"Pirate"}));
                    } else {
                        GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
                    }
                } else {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
                }
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                if (pas::in_range(aPlayer::GetPlayer()->DockedTo->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                    Category = aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey;
                    if (Category != u"" && GR_Main::MainDataConfig->GetBlock(u"Music"sv)->CountBlocks(Category) > 0) {
                        GR_Main::MusicManager->PlayCategory(aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey);
                    } else {
                        GR_Main::MusicManager->PlayCategory(aPlayer::GetPlayer()->DockedTo->GetTypeNameKey());
                    }
                } else if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId)].InternalName, u"Pirate"}));
                } else {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
                }
            } else if (aPlayer::GetPlayer()->InNormalSpace()) {
                if (GlobalsV::MusicInSpaceEnabled) {
                    if (aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 20) {
                        Globals::StarMapScreen->BattleMusicSelected = true;
                        GR_Main::MusicManager->PlayCategory(u"Destroyer"_wref.get());
                    } else {
                        Globals::StarMapScreen->BattleMusicSelected = false;
                        GR_Main::MusicManager->PlayCategory(u"StarMap"_wref.get());
                    }
                } else {
                    GR_Main::MusicManager->RequestFadeOut();
                }
            }
            SysUtilsImports::Sleep(100u);
            GR_Main::MusicManager->RequestFadeOut();
        }
        aGalaxy::Galaxy->CheckIntegrityChecksumAndSetStatus(555);
    }

    std::uint8_t TfGameLoad::IsLoading() {
        return LoadThread != nullptr && LoadThread->IsRunning();
    }

    void TfGameLoad::UpdateLoadingProgress(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        pas::List* Loads{};
        EC_BlockPar::TBlockParEC* Block{};
        if (LoadThread->IsRunning()) {
            if (GlobalsV::LoadingFilmCount < 0 && Globals::ActiveLoadBuffer != nullptr) {
                TargetProgress = pas::real_divide(Globals::ActiveLoadBuffer->Position, Globals::ActiveLoadBuffer->DataSize) * 0.5L;
            }
        } else if (!LoadingComplete) {
            if (!LoadThread->Succeeded) {
                if (GR_Main::SelectedMods != GR_Main::LoadedSaveModSet) {
                    if (([&] {
                        const pas::WideString& paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormSaveManager.QueryReloadMods"_wref.get());
                        GI_MessageLoop::TMessageLoopGI* innermostScreenLoop = Globals::GetInnermostScreenLoop();
                        return GI_MessageBox::ShowMessageBoxGI(innermostScreenLoop, paramByPathOrMarker, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
                    }()) == GI_MessageBox::mbgResultOK) {
                        Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                        Block->AddParam(u"CurrentMod"_wref.get(), GR_Main::LoadedSaveModSet);
                        Block->SaveTextFile(pas::literal_pointer(u"Mods\\ModCFG.txt"), true, false);
                        pas::free(Block);
                        GlobalsV::RequestedScreenId = GlobalsV::screenNone;
                        GlobalsV::PostLoadScreenId = GlobalsV::screenGameLoad;
                        Globals::ReloadModsRequested = true;
                        RequestClose(1);
                        return;
                    }
                } else {
                    GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormSaveManager.LoadError"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgError, 0, 0, 0);
                }
                GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
                GR_Main::ApplyEditableSaveOnLoad = false;
                RequestClose(1);
                return;
            }
            if (!AssetPreloadStarted) {
                Loads = pas::make_object<pas::List>();
                Globals::StarMapScreen->ResumeMode = fStarMap::smrOrders;
                if (GlobalsV::ScreenUsesCompositeLoadAssets(GlobalsV::RequestedScreenId)) {
                    fLoad::QueueSpaceLoadingAssets(Loads, RootUiObject);
                }
                if (pas::list_count(Loads) < 1) {
                    pas::free(Loads);
                    TargetProgress = 1.0f;
                    LoadingComplete = true;
                    AssetPreloadStarted = true;
                } else {
                    Globals::CacheLoader->SetPendingLoads(Loads, true);
                    AssetPreloadStarted = true;
                }
            } else if (!Globals::CacheLoader->IsRunning()) {
                LoadingComplete = true;
                TargetProgress = 1.0f;
            } else {
                TargetProgress = pas::real_divide(Globals::CacheLoader->CompletedLoadCount, Globals::CacheLoader->TotalLoadCount) * 0.5L + 0.5L;
            }
        }
        if (DisplayedProgress < TargetProgress) {
            DisplayedProgress = pas::real_min<pas::Extended>(0.005L + DisplayedProgress, static_cast<pas::Extended>(TargetProgress));
            LoadPanel->SetProgress(DisplayedProgress);
        }
        if (LoadingComplete && DisplayedProgress >= 0.999L) {
            RequestClose(1);
        }
    }

    void TfGameLoad::SelectMusic() {
    }

    void TfGameLoad::p_destroy() {
        fGameLoad::TfGameLoad_Destroy(this);
    }

    void TThreadGameLoad::virtual_TThreadEC_Execute() {
        fGameLoad::TThreadGameLoad_Execute(this);
    }

} // namespace fGameLoad
