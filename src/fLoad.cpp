#include "layout/fLoad.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Cache.hpp"
#include "types/EC_CacheGAI.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_XviD.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/ab_MainForm.hpp"
#include "types/fAbout.hpp"
#include "types/fAchievements.hpp"
#include "types/fCfgSettings.hpp"
#include "types/fEquipmentShop.hpp"
#include "types/fFilm.hpp"
#include "types/fGalaxy2.hpp"
#include "types/fGameEnd.hpp"
#include "types/fGameLoad.hpp"
#include "types/fGameMenu.hpp"
#include "types/fGameSettings2.hpp"
#include "types/fGoodsShop2.hpp"
#include "types/fGov.hpp"
#include "types/fHangar.hpp"
#include "types/fInfo.hpp"
#include "types/fIntroduction.hpp"
#include "types/fJournal.hpp"
#include "types/fJump.hpp"
#include "types/fLoadAB.hpp"
#include "types/fLoadQuest.hpp"
#include "types/fLoadRobot.hpp"
#include "types/fMainForm.hpp"
#include "types/fPlanet.hpp"
#include "types/fPlanetNO.hpp"
#include "types/fPlanetQuest.hpp"
#include "types/fRating2.hpp"
#include "types/fRewards.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fSaveManager.hpp"
#include "types/fScaner.hpp"
#include "types/fScore.hpp"
#include "types/fSelectFace.hpp"
#include "types/fShip2.hpp"
#include "types/fStarMap.hpp"
#include "types/fTalk.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/SE_Gate.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aGalaxy.hpp"
#include "units/ab_Object.hpp"
#include "units/fLoad.hpp"
#include "units/fPanelLoad.hpp"

namespace fLoad {
    std::uint8_t IntroFinished = false;

    std::uint8_t IntroPlaying{};

    void QueueCommonLoadingAssets(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        if (Globals::ScreenLoadMode != 4) {
            fLoad::QueueConfiguredLoadingAssets(PendingLoads, u"LoadGame"_w);
            fLoad::RemoveDuplicateCacheLoads(PendingLoads);
        }
        if (GlobalsV::AnimMenuShip) {
            GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"_wref.get(), u"Bm.FormMain3.2ShipA1"_wref.get());
            GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"_wref.get(), u"Bm.FormMain3.2ShipA2"_wref.get());
            GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"_wref.get(), u"Bm.FormMain3.2ShipA3"_wref.get());
        }
        if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1600) {
            if (GlobalsV::AnimMenuShip) {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"_wref.get(), u"Bm.FormMain3.AnimGaalShip01A"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"_wref.get(), u"Bm.FormMain3.AnimGaalShip02A"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"_wref.get(), u"Bm.FormMain3.AnimGaalShip03A"_wref.get());
            }
            GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GI"_wref.get(), u"Bm.FormMain3.AnimGaalShip01"_wref.get());
            GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GI"_wref.get(), u"Bm.FormMain3.AnimGaalShip02"_wref.get());
            GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GI"_wref.get(), u"Bm.FormMain3.AnimGaalShip03"_wref.get());
        }
        GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GI"_wref.get(), u"Bm.FormMain3.2Ship1"_wref.get());
        GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GI"_wref.get(), u"Bm.FormMain3.2Ship2"_wref.get());
        GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GI"_wref.get(), u"Bm.FormMain3.2Ship3"_wref.get());
        GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GI"_wref.get(), u"Bm.FormMain2.2AnimCaption"_wref.get());
        GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GI"_wref.get(), u"Bm.FormMain3.2BG"_wref.get());
        static_cast<void>(Globals::ScreenLoadMode == 4);
    }

    void QueueSpaceLoadingAssets(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        Globals::TSputnikTempl* Template{};
        std::int32_t I{};
        std::int32_t Count{};
        aGalaxy::PlayerStar->QueueSpaceImageLoads(PendingLoads, Owner);
        if (GlobalsV::SputnikShow) {
            Count = pas::list_count(GlobalsV::SatelliteRenderTemplates);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Template = pas::list_at<Globals::TSputnikTempl>(GlobalsV::SatelliteRenderTemplates, I);
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"PlanetTempl"_wref.get(), Template->MaskName);
            }
        }
        SE_Gate::TGateSE* Gate = pas::construct_call<SE_Gate::TGateSE>(SE_Gate::TGateSE_Create, u"Gate"_w, ClassesImports::Point(0, 0));
        Gate->QueueImageLoad(PendingLoads, Owner);
        pas::free(Gate);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, GlobalsV::SpaceImageTemplates.length() - 1); cpp_range_2.next(I); ) {
            if (reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[I].CacheControl) != nullptr) {
                reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[I].CacheControl)->QueueLoadIfMissing(PendingLoads);
            }
        }
        {
            const pas::WideString& backgroundImagePath = aGalaxy::PlayerStar->GetBackgroundImagePath(I);
            EC_Cache::TCacheEC* globalCache = GR_Main::GlobalCache;
            globalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"_wref.get(), backgroundImagePath);
        }
        fLoad::QueueConfiguredLoadingAssets(PendingLoads, u"Space"_w);
        if (GlobalsV::SoundEnabled) {
            fLoad::QueueConfiguredLoadingAssets(PendingLoads, u"SpaceSound"_w);
        }
        fLoad::RemoveDuplicateCacheLoads(PendingLoads);
    }

    void QueueHyperspaceLoadingAssets(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        aGalaxy::PlayerStar->QueueHyperspaceShipImageLoads(PendingLoads, Owner);
        fLoad::RemoveDuplicateCacheLoads(PendingLoads);
    }

    void QueueArcadeLoadingAssets(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        ab_Object::ab_Object_QueueImageLoads(PendingLoads, Owner);
        fLoad::QueueConfiguredLoadingAssets(PendingLoads, u"AB"_w);
        fLoad::RemoveDuplicateCacheLoads(PendingLoads);
    }

    void RemoveDuplicateCacheLoads(pas::List* PendingLoads) {
        std::int32_t J{};
        EC_Cache::TCacheControlEC* First{};
        EC_Cache::TCacheControlEC* Second{};
        std::int32_t I = 0;
        while (pas::list_count(PendingLoads) - 1 > I) {
            First = pas::list_at<EC_Cache::TCacheControlEC>(PendingLoads, I);
            J = I + 1;
            while (pas::list_count(PendingLoads) > J) {
                Second = pas::list_at<EC_Cache::TCacheControlEC>(PendingLoads, J);
                if (([&] {
                    pas::AnsiString cpp_string = static_cast<pas::AnsiString>(pas::class_name(pas::class_type(First)));
                    pas::AnsiString cpp_string_2 = static_cast<pas::AnsiString>(pas::class_name(pas::class_type(Second)));
                    return cpp_string == cpp_string_2;
                }()) && First->CacheKey == Second->CacheKey) {
                    pas::free(Second);
                    pas::list_delete(PendingLoads, J);
                } else {
                    ++J;
                }
            }
            ++I;
        }
    }

    void QueueConfiguredLoadingAssets(pas::List* PendingLoads, pas::WideString Path) {
        std::int32_t I{};
        EC_BlockPar::TBlockParEC* Block{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"Load.", Path}));
        std::int32_t Count = Block->GetBlockCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            fLoad::QueueConfiguredLoadingAssets(PendingLoads, pas::concat_wide({Path, u".", Block->GetBlockNameByIndex(I)}));
        }
        Count = Block->GetParamCount();
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            const pas::WideString& paramValue = Block->GetParamValue(I);
            const pas::WideString& paramName = Block->GetParamName(I);
            GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, paramName, paramValue);
        }
    }

    void LoadPendingAssets(pas::List* PendingLoads) {
        std::int32_t I{};
        EC_Cache::TCacheControlEC* Control{};
        std::int32_t Count = pas::list_count(PendingLoads);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Control = pas::list_at<EC_Cache::TCacheControlEC>(PendingLoads, I);
            Control->AcquireData();
            Control->Release();
            pas::free(Control);
        }
        pas::list_clear(PendingLoads);
    }

    void TCacheLoader_Execute(TCacheLoader* Self) {
        std::int32_t I{};
        std::int32_t Count{};
        EC_Cache::TCacheControlEC* Control{};
        if (Self->PendingLoads != nullptr) {
            Count = pas::list_count(Self->PendingLoads);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                while ((Self->Flag18 || aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->Destroying) && static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && static_cast<std::uint8_t>(Self->IsStopRequested() ^ 1)) {
                    SysUtilsImports::Sleep(100u);
                }
                Control = pas::list_at<EC_Cache::TCacheControlEC>(Self->PendingLoads, I);
                if (static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && static_cast<std::uint8_t>(Self->IsStopRequested() ^ 1)) {
                    Control->AcquireData();
                    Control->Release();
                }
                pas::free(Control);
                ++Self->CompletedLoadCount;
            }
            pas::free(Self->PendingLoads);
            Self->PendingLoads = nullptr;
        }
    }

    // Takes ownership; waits for the previous run before replacing the list.
    void TCacheLoader::SetPendingLoads(pas::List* Loads, std::uint8_t StartImmediately) {
        if (IsRunning()) {
            WaitForIdle(WindowsSdk::INFINITE);
        }
        PendingLoads = Loads;
        CompletedLoadCount = 0;
        TotalLoadCount = pas::list_count(PendingLoads);
        SetPriority(1);
        if (StartImmediately) {
            Start();
        }
    }

    void TfLoad_Create(TfLoad* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfLoad_Destroy(TfLoad* Self) {
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfLoad::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fLoad... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GI_MessageLoop::TObjectGI* Root = GetByName(u""_wref.get());
        Root->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Root->FindByNameRecursive(u"IntroRect"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        {
            GI_GAI::TgaiGI* Intro = pas::checked_cast<GI_GAI::TgaiGI*>(Root->FindByNameRecursive(u"Intro"_wref.get()));
            Intro->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        }
        Root->FindByNameRecursive(u"Film"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        LoadPanel->InitializeLayout(this);
    }

    void TfLoad::OnOpen() {
        IntroSkipRequest = 0;
        fPanelLoad::TfPanelLoad_OnOpen(LoadPanel);
        ContentPanel->KeyDownCallback = pas::bind_method<&TfLoad::IntroKeyDown>(this);
        ContentPanel->LeftButtonDownCallback = pas::bind_method<&TfLoad::IntroMouseDown>(this);
        ContentPanel->RightButtonDownCallback = pas::bind_method<&TfLoad::IntroMouseDown>(this);
        if (Globals::SkipIntro) {
            IntroFinished = true;
        }
        if (IntroFinished && GR_Main::MusicManager->CategoryOverride == u"") {
            GR_Main::MusicManager->RequestFadeOut();
        }
        LoadProgress = 0.0f;
        DisplayedProgress = 0.0f;
        LoadingFinished = false;
        SetCursorActive(false);
        pas::List* Loads = pas::make_object<pas::List>();
        if (Globals::ScreenLoadMode == 0 || Globals::ScreenLoadMode == 4) {
            fLoad::QueueCommonLoadingAssets(Loads, RootUiObject);
        } else if (Globals::ScreenLoadMode == 2) {
            fLoad::QueueSpaceLoadingAssets(Loads, RootUiObject);
        } else if (Globals::ScreenLoadMode == 3) {
            fLoad::QueueCommonLoadingAssets(Loads, RootUiObject);
            fLoad::QueueSpaceLoadingAssets(Loads, RootUiObject);
        }
        if (pas::list_count(Loads) > 0) {
            Globals::CacheLoader->SetPendingLoads(Loads, false);
            ProgressTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfLoad::UpdateLoadingProgress>(this), 0);
        } else {
            pas::free(Loads);
            RequestClose(1);
        }
        if (IntroFinished) {
            Globals::CacheLoader->Start();
            LoadPanel->SetProgress(0.0f);
            LoadPanel->Show();
        } else {
            IntroPlaying = true;
            Globals::CacheLoader->Start();
            IntroSkipRequest = 0;
            IntroTimer = nullptr;
            IntroConfig = GR_Main::MainDataConfig->GetBlock(u"Intro"_wref.get());
            StartIntroItem(1);
        }
        GR_Main::CheckPlatformModules();
    }

    void TfLoad::OnClose() {
        {
            GI_XviD::TxvidGI* Film = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"_wref.get()));
            Film->ImageClose();
        }
        LoadPanel->OnClose();
        if (Globals::CacheLoader->IsRunning()) {
            Globals::CacheLoader->WaitForIdle(WindowsSdk::INFINITE);
        }
        if (ProgressTimer != nullptr) {
            CancelCallbackTimer(ProgressTimer);
            ProgressTimer = nullptr;
        }
        if (IntroTimer != nullptr) {
            CancelCallbackTimer(IntroTimer);
            IntroTimer = nullptr;
        }
        if (Globals::ScreenLoadMode == 0 || Globals::ScreenLoadMode == 3) {
            Globals::MainMenuScreen->InitializeLayout();
            Globals::PlanetQuestScreen->InitializeLayout();
            Globals::LoadScreen->InitializeLayout();
            Globals::NewGameScreen->InitializeLayout();
            Globals::IntroductionScreen->InitializeLayout();
            Globals::HangarScreen->InitializeLayout();
            Globals::PlanetScreen->InitializeLayout();
            Globals::UninhabitedPlanetScreen->InitializeLayout();
            Globals::RuinsTalkScreen->InitializeLayout();
            Globals::ArcadeBattleScreen->InitializeLayout();
            Globals::EquipmentShopScreen->InitializeLayout();
            Globals::GoodsShopScreen->InitializeLayout();
            Globals::GovernmentScreen->InitializeLayout();
            Globals::InfoScreen->InitializeLayout();
            Globals::RangerRatingScreen->InitializeLayout();
            Globals::RewardsScreen->InitializeLayout();
            Globals::ShipScreen->InitializeLayout();
            Globals::ScannerScreen->InitializeLayout();
            Globals::StarMapScreen->InitializeLayout();
            Globals::FilmScreen->InitializeLayout();
            Globals::GalaxyScreen->InitializeLayout();
            Globals::JumpScreen->InitializeLayout();
            Globals::SaveManagerScreen->InitializeLayout();
            Globals::GameLoadScreen->InitializeLayout();
            Globals::GameMenuScreen->InitializeLayout();
            Globals::SettingsScreen->InitializeLayout();
            Globals::GameEndScreen->InitializeLayout();
            Globals::AboutScreen->InitializeLayout();
            Globals::ScoreScreen->InitializeLayout();
            Globals::SpaceObjectUiLoop->InitializeLayout();
            Globals::TalkScreen->InitializeLayout();
            Globals::SelectFaceScreen->InitializeLayout();
            Globals::JournalScreen->InitializeLayout();
            Globals::LoadRobotScreen->InitializeLayout();
            Globals::LoadQuestScreen->InitializeLayout();
            Globals::LoadArcadeScreen->InitializeLayout();
            Globals::AchievementsScreen->InitializeLayout();
        }
        IntroFinished = true;
        GlobalsV::RequestedScreenId = GlobalsV::PostLoadScreenId;
        GlobalsV::PostLoadScreenId = GlobalsV::screenNone;
    }

    void TfLoad::UpdateLoadingProgress(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (!LoadingFinished) {
            if (Globals::CacheLoader->IsRunning()) {
                LoadProgress = pas::real_divide(Globals::CacheLoader->CompletedLoadCount, Globals::CacheLoader->TotalLoadCount);
            } else {
                LoadingFinished = true;
                LoadProgress = 1.0f;
            }
        }
        if (DisplayedProgress < LoadProgress) {
            DisplayedProgress = pas::real_min<pas::Extended>(0.05L + DisplayedProgress, static_cast<pas::Extended>(LoadProgress));
            LoadPanel->SetProgress(DisplayedProgress);
        }
        if (!LoadingFinished) {
            return;
        }
        if (0.999L > DisplayedProgress) {
            return;
        }
        if (!IntroPlaying) {
            LoadPanel->SetProgress(1.0f);
            Present();
            RequestClose(1);
        }
    }

    void TfLoad::IntroMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        IntroSkipRequest = 1;
    }

    void TfLoad::IntroKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key != 0) {
            IntroSkipRequest = 1;
            if (Key == WindowsSdk::VK_ESCAPE) {
                ++IntroSkipRequest;
            }
        }
    }

    void TfLoad::StartIntroItem(std::int32_t Index) {
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString ImagePath{};
        Block = IntroConfig->FindBlock(pas::wide_int_to_str(Index));
        if (Block == nullptr || IntroSkipRequest > 1) {
            InvalidateViewport();
            IntroPlaying = false;
            return;
        }
        IntroItemIndex = Index;
        if (Block->CountParams(u"Image"_wref.get()) == 0) {
            StartIntroItem(Index + 1);
            return;
        }
        ImagePath = Block->GetParam(u"Image"_wref.get());
        if (EC_Str::LowerCaseWideString(EC_Str::TrimWideString(EC_Str::ExtractFileExtNoDotW(ImagePath))) == u"vdo") {
            IntroImageKind = 0;
            {
                GI_XviD::TxvidGI* Film = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"_wref.get()));
                if (!Film->ImageOpen(ImagePath, false)) {
                    StartIntroItem(Index + 1);
                    return;
                }
            }
            if (Block->CountParams(u"Frames"_wref.get()) > 0) {
                IntroVideoFrameCount = EC_Str::ExtractDigitsToIntW(Block->GetParam(u"Frames"_wref.get()));
            }
        } else {
            IntroImageKind = 1;
            {
                GI_GAI::TgaiGI* Intro = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"Intro"_wref.get()));
                Intro->SetImagePath(ImagePath);
                Intro->SetPosition(ClassesImports::Point(0, 0));
                Intro->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                if (Block->CountParams(u"Size"_wref.get()) > 0) {
                    ImagePath = Block->GetParam(u"Size"_wref.get());
                    if (EC_Str::CountDelimitedPartsW(ImagePath, u","_wref.get()) > 1) {
                        Intro->SetSize(([&] {
                            std::int32_t extractDigitsToIntW = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(ImagePath, 1, u","_wref.get()));
                            std::int32_t extractDigitsToIntW_2 = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(ImagePath, 0, u","_wref.get()));
                            return ClassesImports::Point(extractDigitsToIntW_2, extractDigitsToIntW);
                        }()));
                        Intro->SetPosition(ClassesImports::Point((GR_Main::GameScreenWidth - Intro->ClientSize.X) / 2, (GR_Main::GameScreenHeight - Intro->ClientSize.Y) / 2));
                        if (Block->CountParams(u"Sme"_wref.get()) > 0) {
                            ImagePath = Block->GetParam(u"Sme"_wref.get());
                            if (EC_Str::CountDelimitedPartsW(ImagePath, u","_wref.get()) > 1) {
                                Intro->SetPosition(([&] {
                                    std::int32_t cpp_arg = Intro->LocalPosition.Y + EC_Str::ExtractSignedDigitsToIntW(EC_Str::ExtractDelimitedPartW(ImagePath, 1, u","_wref.get()));
                                    std::int32_t cpp_arg_2 = Intro->LocalPosition.X + EC_Str::ExtractSignedDigitsToIntW(EC_Str::ExtractDelimitedPartW(ImagePath, 0, u","_wref.get()));
                                    return ClassesImports::Point(cpp_arg_2, cpp_arg);
                                }()));
                            }
                        }
                    }
                }
                if (Block->CountParams(u"Frames"_wref.get()) > 0) {
                    Intro->LoadFrameSequenceFromText(pas::concat_wide({u"[80,0-", Block->GetParam(u"Frames"_wref.get()), u"]"}));
                }
                Intro->PrimeImageCaches();
                Intro->SetActive(true);
                Intro->StopAutoPlayback();
                Intro->SetSequenceFrame(0);
            }
        }
        if (Block->CountParams(u"Time"_wref.get()) > 0) {
            IntroDurationMs = EC_Str::ExtractDigitsToIntW(Block->GetParam(u"Time"_wref.get()));
        }
        if (GlobalsV::MusicEnabled) {
            if (Block->CountParams(u"Sound"_wref.get()) > 0) {
                GR_Main::MusicManager->PlayCategory(Block->GetParam(u"Sound"_wref.get()));
                while (!GR_Main::MusicManager->IsPlaying()) {
                    SysUtilsImports::Sleep(1u);
                }
            }
        }
        IntroStartedAt = MMSystem::timeGetTime();
        if (IntroTimer != nullptr) {
            CancelCallbackTimer(IntroTimer);
            IntroTimer = nullptr;
        }
        IntroSkipRequest = 0;
        IntroTimer = ScheduleCallbackTimer(5, 5, pas::bind_method<&TfLoad::UpdateIntro>(this), 0);
    }

    void TfLoad::UpdateIntro(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::uint32_t cpp_left_2 = MMSystem::timeGetTime();
        pas::Extended cpp_left = cpp_left_2 - IntroStartedAt;
        double Fraction = pas::real_divide(cpp_left, IntroDurationMs);
        if (Fraction > 1.0L) {
            Fraction = 1.0;
        }
        if (Fraction >= 1.0L || IntroSkipRequest > 0) {
            if (IntroTimer != nullptr) {
                CancelCallbackTimer(IntroTimer);
                IntroTimer = nullptr;
            }
            if (GlobalsV::MusicEnabled) {
                GR_Main::MusicManager->StopImmediately();
                while (GR_Main::MusicManager->IsPlaying()) {
                    SysUtilsImports::Sleep(1u);
                }
            }
            {
                GI_XviD::TxvidGI* Film = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"_wref.get()));
                Film->ImageClose();
            }
            {
                GI_GAI::TgaiGI* Intro = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"Intro"_wref.get()));
                Intro->SetImagePath(u""_wref.get());
            }
            StartIntroItem(IntroItemIndex + 1);
            return;
        }
        if (IntroImageKind == 0) {
            GI_XviD::TxvidGI* Film_2 = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"_wref.get()));
            Film_2->SetFramePosition(System::Round(static_cast<long double>(IntroVideoFrameCount - 1) * Fraction));
        } else {
            GI_GAI::TgaiGI* Intro_2 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"Intro"_wref.get()));
            Intro_2->SetFramePosition(System::Round(static_cast<long double>(Intro_2->SequenceFrameCount - 1) * Fraction), true);
        }
    }

    void TfLoad::p_destroy() {
        fLoad::TfLoad_Destroy(this);
    }

    void TCacheLoader::virtual_TThreadEC_Execute() {
        fLoad::TCacheLoader_Execute(this);
    }

} // namespace fLoad
