#include "layout/fFilm.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GI_SpaceImg.hpp"
#include "types/GI_StarField.hpp"
#include "types/GI_StarFieldImg.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Rect.hpp"
#include "types/SE_Asteroid.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SE_Sputnik.hpp"
#include "types/SE_Star.hpp"
#include "types/SE_Weapon.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aAsteroid.hpp"
#include "types/aItem.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fFilmFile.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"
#include "units/aEFilm.hpp"
#include "units/aEFilmEnd.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/fFilm.hpp"

namespace fFilm {
    // Loads FilmScreen.PreloadHistoryIndex into PreloadedFilm and supplies its separately stored Turn.
    void TfFilmLoader_Execute(TfFilmLoader* Self) {
        {
            fFilmFile::PFilmHistoryEntry entry = Globals::FilmHistory->GetEntry(Globals::FilmScreen->PreloadHistoryIndex);
            aEFilm::TEFilm* preloadedFilm = Globals::FilmScreen->PreloadedFilm;
            fFilmFile::TFilmFile* filmHistory = Globals::FilmHistory;
            filmHistory->LoadFilm(entry, preloadedFilm);
        }
        Globals::FilmScreen->PreloadedFilm->Turn = Globals::FilmHistory->GetEntry(Globals::FilmScreen->PreloadHistoryIndex)->Turn;
    }

    void TfFilm::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fFilm... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GI_MessageLoop::TObjectGI* Main = GetByName(u"MainPanel"sv);
        Main->Parent->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Main->SetPosition(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
        Main->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
        Main->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* Map = Main->FindByNameRecursive(u"MapPanel"sv);
        Map->SetPosition(ClassesImports::Point(Map->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Map->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
        GI_MessageLoop::TObjectGI* Center = Main->FindByNameRecursive(u"CenterShip"sv);
        Center->SetPosition(ClassesImports::Point(Center->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Center->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
        GI_MessageLoop::TObjectGI* Shade = Main->FindByNameRecursive(u"MapPanelA"sv);
        Shade->SetPosition(ClassesImports::Point(Shade->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Shade->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
        GI_MessageLoop::TObjectGI* ShadeNext = Shade->NextSibling;
        ShadeNext->SetPosition(ClassesImports::Point(ShadeNext->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, ShadeNext->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
        GI_MessageLoop::TObjectGI* Fps = Main->FindByNameRecursive(u"FPS"sv);
        Fps->SetPosition(ClassesImports::Point(Fps->LocalPosition.X, Fps->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
        GI_MessageLoop::TObjectGI* FilmPanel = Main->FindByNameRecursive(u"PanelFilm"sv);
        FilmPanel->SetPosition(ClassesImports::Point(FilmPanel->LocalPosition.X, FilmPanel->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
        GI_MessageLoop::TObjectGI* SpaceImages = Main->FindByNameRecursive(u"SpaceImg"sv);
        SpaceImages->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
        SpaceImages->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* Stars = Main->FindByNameRecursive(u"StarField"sv);
        Stars->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
        Stars->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* StarImages = Main->FindByNameRecursive(u"StarFieldImg"sv);
        StarImages->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
        StarImages->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* StarM = Main->FindByNameRecursive(u"StarFieldM"sv);
        StarM->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
        StarM->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        CenterShipButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"CenterShip"sv));
        CenterShipButton->DownCallback = pas::bind_method<&TfFilm::CenterShipClicked>(this);
        SpacePanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"sv));
        MapPanel = GetByName(u"MapPanel"sv);
        FrameSlider = pas::checked_cast<GI_ScrollBar::TScrollBarGI*>(GetByName(u"SBFrame"sv));
        SpeedSlider = pas::checked_cast<GI_ScrollBar::TScrollBarGI*>(GetByName(u"SBSpeed"sv));
        PlayButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PF_Play"sv));
        StopButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PF_Stop"sv));
        TurnSlider = pas::checked_cast<GI_ScrollBar::TScrollBarGI*>(GetByName(u"PF_SBTurn"sv));
        DateLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PF_Date"sv));
        SpacePanel->ScrollType = GI_Panel::pstSimple;
        GI_GraphBuf::TGraphBufGI* Graph = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(MapPanel);
        Graph->BindExternalGraphBuf(GR_Main::RenderScratchBuffer);
    }

    void TfFilm::OnOpen() {
        GetByName(u"FPS"sv)->SetActive(GR_Main::ShowFrameRate);
        GI_StarFieldImg::TStarFieldImgGI* Stars = pas::checked_cast<GI_StarFieldImg::TStarFieldImgGI*>(GetByName(u"StarFieldImg"sv));
        Stars->SetActive(GlobalsV::Wind >= 2);
        if (Stars->StarCount <= 0) {
            Stars->SeedStars();
        }
        Stars->CopyStarsFrom(pas::checked_cast<GI_StarFieldImg::TStarFieldImgGI*>(Globals::StarMapScreen->GetByName(u"StarFieldImg"sv)));
        {
            GI_MessageLoop::TObjectGI* findControlByPath = FindControlByPath(u"StarFieldM"_wref.get());
            std::uint8_t cpp_arg = GlobalsV::Wind >= 1;
            findControlByPath->SetActive(cpp_arg);
        }
        UpdateRectsEnabled = false;
        SetViewOffset(EC_Struct::TruncatePointF(Globals::SpaceViewPosition));
        UpdateRectsEnabled = true;
        PreloadHistoryIndex = -1;
        if (Loader != nullptr) {
            pas::free(Loader);
            Loader = nullptr;
        }
        Loader = pas::construct_call<TfFilmLoader>(EC_Thread::TThreadEC_Create);
        Loader->SetPriority(1);
        PanTimer = ScheduleCallbackTimer(GlobalsV::ScrollTime, GlobalsV::ScrollTime, pas::bind_method<&TfFilm::PanView>(this), 0);
        ContentPanel->KeyDownCallback = pas::bind_method<&TfFilm::KeyDown>(this);
        ContentPanel->KeyUpCallback = pas::bind_method<&TfFilm::KeyUp>(this);
        CurrentFilm = pas::construct_call<aEFilm::TEFilm>(aEFilm::TEFilm_Create);
        PreloadedFilm = pas::construct_call<aEFilm::TEFilm>(aEFilm::TEFilm_Create);
        FrameSlider->PositionChangedCallback = pas::bind_method<&TfFilm::FrameSliderChanged>(this);
        SpeedSlider->PositionChangedCallback = pas::bind_method<&TfFilm::SpeedSliderChanged>(this);
        SpeedSlider->SetRange(0, 100);
        SetFrameInterval(18, true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PF_Exit"sv))->UpCallback = pas::bind_method<&TfFilm::ExitClicked>(this);
        PlayButton->UpCallback = pas::bind_method<&TfFilm::PlayStopClicked>(this);
        StopButton->UpCallback = pas::bind_method<&TfFilm::PlayStopClicked>(this);
        {
            std::int32_t cpp_arg_2 = Globals::FilmHistory->GetCount() - 1;
            GI_ScrollBar::TScrollBarGI* turnSlider = TurnSlider;
            turnSlider->SetRange(0, cpp_arg_2);
        }
        TurnSlider->PositionChangedCallback = pas::bind_method<&TfFilm::TurnSliderChanged>(this);
        GetByName(u"MapPanelA"sv)->SetActive(aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(1));
        SelectHistoryEntry(Globals::FilmHistory->GetCount() - 1, true);
        CopyLiveVisualStateToFilm();
        AdvanceOneStep();
        StartPlayback();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(133);
    }

    void TfFilm::OnClose() {
        aGalaxy::Galaxy->CheckIntegrityChecksum(134);
        {
            GI_StarFieldImg::TStarFieldImgGI* cpp_arg = pas::checked_cast<GI_StarFieldImg::TStarFieldImgGI*>(GetByName(u"StarFieldImg"sv));
            GI_StarFieldImg::TStarFieldImgGI* cpp_arg_2 = pas::checked_cast<GI_StarFieldImg::TStarFieldImgGI*>(Globals::StarMapScreen->GetByName(u"StarFieldImg"sv));
            cpp_arg_2->CopyStarsFrom(cpp_arg);
        }
        fStarMap::TfStarMap::SaveSpaceImageState(pas::checked_cast<GI_SpaceImg::TSpaceImgGI*>(GetByName(u"SpaceImg"sv)));
        if (Globals::TrailingFilmEffects != nullptr) {
            pas::free(Globals::TrailingFilmEffects);
            Globals::TrailingFilmEffects = nullptr;
        }
        TfFilm::ReleaseFilmSceneObjects(CurrentFilm, true);
        if (Loader != nullptr) {
            pas::free(Loader);
            Loader = nullptr;
        }
        if (CurrentFilm != nullptr) {
            pas::free(CurrentFilm);
            CurrentFilm = nullptr;
        }
        if (PreloadedFilm != nullptr) {
            pas::free(PreloadedFilm);
            PreloadedFilm = nullptr;
        }
        if (PanTimer != nullptr) {
            CancelCallbackTimer(PanTimer);
            PanTimer = nullptr;
        }
        Globals::SpaceProcess->CloseSpace();
    }

    WindowsSdk::TPoint TfFilm::GetViewOffset() {
        WindowsSdk::TPoint Result{};
        if (SpacePanel == nullptr) {
            SpacePanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"sv));
        }
        Result = SpacePanel->ScrollOffset;
        return Result;
    }

    // Disables automatic camera following.
    void TfFilm::SetViewOffset(WindowsSdk::TPoint Offset) {
        if (SpacePanel == nullptr) {
            SpacePanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"sv));
        }
        SpacePanel->SetScrollOffset(Offset);
        if (Globals::SpaceProcess->Space != nullptr) {
            Globals::SpaceProcess->Space->MapScrollChanged(nullptr);
        }
        Globals::FilmCameraFollow = false;
    }

    // Ignored while automatic camera following is disabled.
    void TfFilm::FollowViewOffset(WindowsSdk::TPoint Offset) {
        if (Globals::FilmCameraFollow) {
            if (SpacePanel == nullptr) {
                SpacePanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"sv));
            }
            SpacePanel->SetScrollOffset(Offset);
            if (Globals::SpaceProcess->Space != nullptr) {
                Globals::SpaceProcess->Space->MapScrollChanged(nullptr);
            }
        }
    }

    void TfFilm::PanView(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        WindowsSdk::TPoint Offset{};
        WindowsSdk::TPoint OldOffset{};
        OldOffset = GetViewOffset();
        Offset = OldOffset;
        if (PanLeft) {
            Offset.X -= GlobalsV::ScrollStep;
        }
        if (PanRight) {
            Offset.X += GlobalsV::ScrollStep;
        }
        if (PanUp) {
            Offset.Y -= GlobalsV::ScrollStep;
        }
        if (PanDown) {
            Offset.Y += GlobalsV::ScrollStep;
        }
        std::int16_t X = GetCursorPoint().X;
        std::int16_t Y = GetCursorPoint().Y;
        if (X < GlobalsV::ScrollSense) {
            Offset.X -= GlobalsV::ScrollStep;
        }
        if (X > GR_Main::GameScreenWidth - GlobalsV::ScrollSense - 1) {
            Offset.X += GlobalsV::ScrollStep;
        }
        if (Y < GlobalsV::ScrollSense) {
            Offset.Y -= GlobalsV::ScrollStep;
        }
        if (Y > GR_Main::GameScreenHeight - GlobalsV::ScrollSense - 1) {
            Offset.Y += GlobalsV::ScrollStep;
        }
        if (OldOffset.X != Offset.X || OldOffset.Y != Offset.Y) {
            SetViewOffset(Offset);
        }
    }

    void TfFilm::KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Key == WindowsSdk::VK_LEFT) {
                PanLeft = true;
            } else if (Key == WindowsSdk::VK_RIGHT) {
                PanRight = true;
            } else if (Key == WindowsSdk::VK_UP) {
                PanUp = true;
            } else if (Key == WindowsSdk::VK_DOWN) {
                PanDown = true;
            } else if (Key == 'C') {
                CenterShipClicked(nullptr);
            } else if (Key == WindowsSdk::VK_SPACE) {
                PlayStopClicked(nullptr);
            } else if (Key == WindowsSdk::VK_ESCAPE) {
                ExitClicked(nullptr);
            }
        }
    }

    void TfFilm::KeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_LEFT) {
            PanLeft = false;
        } else if (Key == WindowsSdk::VK_RIGHT) {
            PanRight = false;
        } else if (Key == WindowsSdk::VK_UP) {
            PanUp = false;
        } else if (Key == WindowsSdk::VK_DOWN) {
            PanDown = false;
        }
    }

    void TfFilm::CopyLiveVisualStateToFilm() {
        std::int32_t Index{};
        std::int32_t SatelliteIndex{};
        std::int32_t SatelliteCount{};
        aPlanet::TPlanet* Planet{};
        aPlanet::TSputnik* Satellite{};
        aAsteroid::TAsteroid* Asteroid{};
        aEFilm::TEFilmObj* Obj = CurrentFilm->FindObject(pas::view(SE_Process::ClassSEtoName(aGalaxy::PlayerStar->Graphic)), pas::view(aGalaxy::PlayerStar->Graphic->GraphKey), aGalaxy::PlayerStar->Id);
        if (Obj != nullptr && Obj->SceneObject != nullptr) {
            pas::checked_cast<SE_Star::TStarSE*>(Obj->SceneObject)->SetSequenceFrameIndex(reinterpret_cast<SE_Star::TStarSE*>(aGalaxy::PlayerStar->Graphic)->GetSequenceFrameIndex());
        }
        std::int32_t Count = pas::list_count(aGalaxy::PlayerStar->Planets);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::PlayerStar->Planets, Index);
            Obj = CurrentFilm->FindObject(pas::view(SE_Process::ClassSEtoName(Planet->Graphic)), pas::view(Planet->Graphic->GraphKey), Planet->Id);
            if (Obj != nullptr && Obj->SceneObject != nullptr) {
                pas::checked_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->SetSurfaceMapOffset(Planet->Graphic->SurfaceMapOffset);
                pas::checked_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->SetCloud1MapOffset(Planet->Graphic->Cloud1MapOffset);
                pas::checked_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->SetCloud2MapOffset(Planet->Graphic->Cloud2MapOffset);
                pas::checked_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->SetCloud3MapOffset(Planet->Graphic->Cloud3MapOffset);
            }
            SatelliteCount = pas::list_count(Planet->Satellites);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SatelliteCount - 1); cpp_range_2.next(SatelliteIndex); ) {
                Satellite = pas::list_at<aPlanet::TSputnik>(Planet->Satellites, SatelliteIndex);
                Obj = CurrentFilm->FindObject(pas::view(SE_Process::ClassSEtoName(Satellite->Graphic)), pas::view(Satellite->Graphic->GraphKey), Satellite->Id);
                if (Obj != nullptr && Obj->SceneObject != nullptr) {
                    pas::checked_cast<SE_Sputnik::TSputnikSE*>(Obj->SceneObject)->SurfaceMapOffset = Satellite->Graphic->SurfaceMapOffset;
                    static_cast<SE_Sputnik::TSputnikSE*>(Obj->SceneObject)->OrbitAngle = Satellite->Graphic->OrbitAngle;
                }
            }
        }
        Count = pas::list_count(aGalaxy::PlayerStar->Asteroids);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
            Asteroid = pas::list_at<aAsteroid::TAsteroid>(aGalaxy::PlayerStar->Asteroids, Index);
            Obj = CurrentFilm->FindObject(pas::view(SE_Process::ClassSEtoName(Asteroid->GraphObject)), pas::view(Asteroid->GraphObject->GraphKey), Asteroid->Id);
            if (Obj != nullptr && Obj->SceneObject != nullptr) {
                pas::checked_cast<SE_Asteroid::TAsteroidSE*>(Obj->SceneObject)->SetSequenceFrameIndex(reinterpret_cast<SE_Asteroid::TAsteroidSE*>(Asteroid->GraphObject)->GetSequenceFrameIndex());
            }
        }
    }

    void TfFilm::CopyFilmVisualStateToLive() {
        std::int32_t Index{};
        std::int32_t SatelliteIndex{};
        std::int32_t SatelliteCount{};
        aPlanet::TPlanet* Planet{};
        aPlanet::TSputnik* Satellite{};
        aAsteroid::TAsteroid* Asteroid{};
        aEFilm::TEFilmObj* Obj = CurrentFilm->FindObject(pas::view(SE_Process::ClassSEtoName(aGalaxy::PlayerStar->Graphic)), pas::view(aGalaxy::PlayerStar->Graphic->GraphKey), aGalaxy::PlayerStar->Id);
        if (Obj != nullptr && Obj->SceneObject != nullptr) {
            reinterpret_cast<SE_Star::TStarSE*>(aGalaxy::PlayerStar->Graphic)->SetSequenceFrameIndex(pas::checked_cast<SE_Star::TStarSE*>(Obj->SceneObject)->GetSequenceFrameIndex());
        }
        std::int32_t Count = pas::list_count(aGalaxy::PlayerStar->Planets);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::PlayerStar->Planets, Index);
            Obj = CurrentFilm->FindObject(pas::view(SE_Process::ClassSEtoName(Planet->Graphic)), pas::view(Planet->Graphic->GraphKey), Planet->Id);
            if (Obj != nullptr && Obj->SceneObject != nullptr) {
                Planet->Graphic->SetSurfaceMapOffset(pas::checked_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->SurfaceMapOffset);
                Planet->Graphic->SetCloud1MapOffset(pas::checked_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->Cloud1MapOffset);
                Planet->Graphic->SetCloud2MapOffset(pas::checked_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->Cloud2MapOffset);
                Planet->Graphic->SetCloud3MapOffset(pas::checked_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->Cloud3MapOffset);
            }
            SatelliteCount = pas::list_count(Planet->Satellites);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SatelliteCount - 1); cpp_range_2.next(SatelliteIndex); ) {
                Satellite = pas::list_at<aPlanet::TSputnik>(Planet->Satellites, SatelliteIndex);
                Obj = CurrentFilm->FindObject(pas::view(SE_Process::ClassSEtoName(Satellite->Graphic)), pas::view(Satellite->Graphic->GraphKey), Satellite->Id);
                if (Obj != nullptr && Obj->SceneObject != nullptr) {
                    Satellite->Graphic->SurfaceMapOffset = pas::checked_cast<SE_Sputnik::TSputnikSE*>(Obj->SceneObject)->SurfaceMapOffset;
                    Satellite->Graphic->OrbitAngle = static_cast<SE_Sputnik::TSputnikSE*>(Obj->SceneObject)->OrbitAngle;
                }
            }
        }
        Count = pas::list_count(aGalaxy::PlayerStar->Asteroids);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
            Asteroid = pas::list_at<aAsteroid::TAsteroid>(aGalaxy::PlayerStar->Asteroids, Index);
            Obj = CurrentFilm->FindObject(pas::view(SE_Process::ClassSEtoName(Asteroid->GraphObject)), pas::view(Asteroid->GraphObject->GraphKey), Asteroid->Id);
            if (Obj != nullptr && Obj->SceneObject != nullptr) {
                reinterpret_cast<SE_Asteroid::TAsteroidSE*>(Asteroid->GraphObject)->SetSequenceFrameIndex(pas::checked_cast<SE_Asteroid::TAsteroidSE*>(Obj->SceneObject)->GetSequenceFrameIndex());
            }
        }
    }

    void TfFilm::ExitClicked(GI_MessageLoop::TObjectGI* Sender) {
        CopyFilmVisualStateToLive();
        GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
        RequestClose(1);
    }

    void TfFilm::CenterShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        SetViewOffset(EC_Struct::TruncatePointF(CameraTarget));
    }

    // Waits for the loader, swaps film buffers, resets the command cursor, then preloads the following entry. Requires a valid index and nonempty command stream.
    void TfFilm::SelectHistoryEntry(std::int32_t Index, std::uint8_t InitialLoad) {
        if (Loader->IsRunning()) {
            Loader->WaitForIdle(WindowsSdk::INFINITE);
        }
        if (PreloadHistoryIndex != Index) {
            PreloadHistoryIndex = Index;
            Loader->Start();
            Loader->WaitForIdle(WindowsSdk::INFINITE);
        }
        if (CurrentHistoryIndex >= PreloadHistoryIndex && Globals::TrailingFilmEffects != nullptr) {
            pas::free(Globals::TrailingFilmEffects);
            Globals::TrailingFilmEffects = nullptr;
        }
        ReuseSceneObjectsForPreloadedFilm();
        TfFilm::ReleaseFilmSceneObjects(CurrentFilm, true);
        TfFilm::CreateFilmSceneObjects(PreloadedFilm);
        aEFilm::TEFilm* SwapFilm = CurrentFilm;
        CurrentFilm = PreloadedFilm;
        PreloadedFilm = SwapFilm;
        CurrentHistoryIndex = Index;
        PreloadHistoryIndex = 0;
        if (!InitialLoad) {
            fStarMap::TfStarMap::SaveSpaceImageState(pas::checked_cast<GI_SpaceImg::TSpaceImgGI*>(GetByName(u"SpaceImg"sv)));
        }
        {
            GI_SpaceImg::TSpaceImgGI* cpp_arg = pas::checked_cast<GI_SpaceImg::TSpaceImgGI*>(GetByName(u"SpaceImg"sv));
            GI_StarField::TStarFieldGI* cpp_arg_2 = pas::checked_cast<GI_StarField::TStarFieldGI*>(GetByName(u"StarField"sv));
            fStarMap::TfStarMap::BuildSpaceBackground(cpp_arg_2, cpp_arg, CurrentFilm->StarGenerationSeed, CurrentFilm->BackgroundImage);
        }
        GR_Main::FullFrameRedrawRequested = true;
        InvalidateViewport();
        PreloadedFilm->StarGenerationSeed = 0u;
        TurnSlider->SetPositionInternal(Index);
        {
            const pas::WideString& formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(CurrentFilm->Turn);
            GI_Label::TLabelGI* dateLabel = DateLabel;
            dateLabel->SetText(formatTurnDate);
        }
        if (aPlayer::GetPlayer() != nullptr) {
            Globals::SpaceProcess->RadarCenter = EC_Struct::MakePointF(0.0f, 0.0f);
            Globals::SpaceProcess->RadarRange = CurrentFilm->RadarRange;
            Globals::SpaceProcess->ActionRange = CurrentFilm->RadarRange;
            Globals::SpaceProcess->ActionColor = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
        }
        Globals::SpaceProcess->SystemRadius = CurrentFilm->MapDiameter / 2;
        Globals::SpaceProcess->PopulateAmbientObjects(CurrentFilm->MapDiameter / 2, CurrentFilm->BackgroundImage, CurrentFilm->StarGenerationSeed);
        Globals::SpaceProcess->OpenSpace(SpacePanel, this);
        Globals::SpaceProcess->Space->MinimapScale = 1.0E-7;
        Globals::SpaceProcess->BindMinimap(MapPanel);
        Globals::SpaceProcess->Space->MinimapScale = pas::real_divide(MapPanel->ClientSize.X, CurrentFilm->MapDiameter);
        if (aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->IsHealthEffectActive(1)) {
                Globals::SpaceProcess->Space->AlphaShift = 2;
            }
        }
        Globals::SpaceProcess->Space->CreateMinimapViewport();
        StepIndex = 0;
        NextCommand = CurrentFilm->FirstCommand;
        FrameSlider->SetRange(1, CurrentFilm->LastCommand->StepIndex);
        PreloadHistoryIndex = Index + 1;
        {
            std::int32_t cpp_left = Globals::FilmHistory->GetCount();
            if (cpp_left <= PreloadHistoryIndex) {
                PreloadHistoryIndex = Globals::FilmHistory->GetCount() - 1;
            }
        }
        Loader->Start();
        Globals::MinimapFrameCounter = 0;
    }

    void TfFilm::CreateFilmSceneObjects(aEFilm::TEFilm* Film) {
        aEFilm::TEFilmObj* Obj{};
        Obj = Film->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject == nullptr) {
                SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(pas::view(Obj->KindName), Obj->GraphKey, ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> sceneObject = pas::Var<SE_Space::TObjectSE*>(&Obj->SceneObject);
                SE_Space::RetainSpaceObject(sceneObject, createSpaceObjectByName);
            }
            Obj = Obj->Next;
        }
    }

    void TfFilm::ReleaseFilmSceneObjects(aEFilm::TEFilm* Film, std::uint8_t ReleaseTrailingReferences) {
        aEFilm::TEFilmObj* Obj{};
        aEFilmEnd::PEFilmEndEntry Entry{};
        aEFilmEnd::PEFilmEndEntry NextEntry{};
        Obj = Film->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject != nullptr) {
                Obj->SceneObject->DetachFromSpace();
                if (ReleaseTrailingReferences) {
                    if (Globals::TrailingFilmEffects != nullptr) {
                        NextEntry = Globals::TrailingFilmEffects->FirstEntry;
                        while (NextEntry != nullptr) {
                            Entry = NextEntry;
                            NextEntry = NextEntry->Next;
                            if (Entry->RelatedObject1 == Obj->SceneObject) {
                                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1));
                            }
                            if (Entry->RelatedObject2 == Obj->SceneObject) {
                                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject2));
                            }
                            if (pas::class_cast_if<SE_Weapon::TWeaponSE*>(Entry->SceneObject) != nullptr && (pas::checked_cast<SE_Weapon::TWeaponSE*>(Entry->SceneObject)->SourceObject == Obj->SceneObject || pas::checked_cast<SE_Weapon::TWeaponSE*>(Entry->SceneObject)->TargetObject == Obj->SceneObject)) {
                                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1));
                                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject2));
                                Globals::TrailingFilmEffects->RemoveEntry(Entry);
                            }
                        }
                    }
                }
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj->SceneObject));
            }
            Obj = Obj->Next;
        }
    }

    void TfFilm::ReuseSceneObjectsForPreloadedFilm() {
        aEFilm::TEFilmObj* NewObj{};
        aEFilm::TEFilmObj* OldObj{};
        NewObj = PreloadedFilm->FirstObject;
        while (NewObj != nullptr) {
            if (NewObj->SceneObject == nullptr) {
                OldObj = CurrentFilm->FindObject(pas::view(NewObj->KindName), pas::view(NewObj->GraphKey), NewObj->ObjectId);
                if (OldObj != nullptr) {
                    if (OldObj->SceneObject != nullptr) {
                        SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&NewObj->SceneObject), OldObj->SceneObject);
                        if (!(pas::class_cast_if<SE_Ship2::TShip2SE*>(OldObj->SceneObject) != nullptr) && OldObj->SceneObject->GraphKey != u"Ruins.WB" || CurrentHistoryIndex != PreloadHistoryIndex - 1) {
                            NewObj->SceneObject->DetachFromSpace();
                        }
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&OldObj->SceneObject));
                        if (SE_Weapon::TWeaponSE* weaponSE = pas::class_cast_if<SE_Weapon::TWeaponSE*>(NewObj->SceneObject)) {
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&weaponSE->SourceObject));
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&pas::checked_cast<SE_Weapon::TWeaponSE*>(NewObj->SceneObject)->TargetObject));
                        }
                    }
                }
            }
            NewObj = NewObj->Next;
        }
        if (CurrentHistoryIndex == PreloadHistoryIndex - 1) {
            OldObj = CurrentFilm->FirstObject;
            while (OldObj != nullptr) {
                if (OldObj->SceneObject != nullptr && OldObj->SceneObject->IsAttachedToSpace() && OldObj->SceneObject->GraphKey == u"Ruins.WB") {
                    NewObj = PreloadedFilm->AllocateObject();
                    NewObj->ObjectId = OldObj->ObjectId;
                    SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&NewObj->SceneObject), OldObj->SceneObject);
                    NewObj->KindName = OldObj->KindName;
                    NewObj->GraphKey = OldObj->GraphKey;
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&OldObj->SceneObject));
                }
                OldObj = OldObj->Next;
            }
        }
    }

    void TfFilm::AdvancePausedEffects(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (Globals::TrailingFilmEffects != nullptr) {
            Globals::TrailingFilmEffects->AdvanceEffects();
            if (Globals::TrailingFilmEffects->FirstEntry == nullptr) {
                pas::free(Globals::TrailingFilmEffects);
                Globals::TrailingFilmEffects = nullptr;
            }
        }
        Globals::SpaceProcess->Space->AdvanceTimers();
        Globals::SpaceProcess->Space->AdvanceObjects();
    }

    void TfFilm::SetFrameInterval(std::int32_t IntervalMs, std::uint8_t UpdateSlider) {
        FrameIntervalMs = IntervalMs;
        if (Playing) {
            PausePlayback();
            StartPlayback();
        } else {
            if (EffectsTimer != nullptr) {
                CancelCallbackTimer(EffectsTimer);
                EffectsTimer = nullptr;
            }
            EffectsTimer = ScheduleCallbackTimer(FrameIntervalMs, FrameIntervalMs, pas::bind_static_method<&TfFilm::AdvancePausedEffects>(this), 0);
        }
        if (UpdateSlider) {
            SpeedSlider->SetPositionInternal(100 - System::Round(pas::real_divide(5 - FrameIntervalMs, -95.0L) * 1.0E+2L));
        }
    }

    void TfFilm::SpeedSliderChanged(GI_MessageLoop::TObjectGI* Sender) {
        SetFrameInterval(System::Round(pas::real_divide(100 - SpeedSlider->Position, 1.0E+2L) * 95.0L + 5.0L), false);
    }

    // Backward seeking reloads the recording and executes commands forward to the requested step.
    void TfFilm::FrameSliderChanged(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Position = FrameSlider->Position;
        if (Playing) {
            PausePlayback();
        }
        if (Position != StepIndex) {
            if (Position < StepIndex) {
                SelectHistoryEntry(CurrentHistoryIndex, false);
                AdvanceOneStep();
            }
            Globals::FilmSoundEffectsEnabled = false;
            while (Position > StepIndex) {
                AdvanceOneStep();
            }
            Globals::FilmSoundEffectsEnabled = true;
        }
    }

    void TfFilm::PlayStopClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Playing) {
            PausePlayback();
        } else {
            Globals::FilmCameraFollow = true;
            if (NextCommand == nullptr) {
                SelectHistoryEntry(Globals::FilmHistory->GetCount() - 1, false);
                AdvanceOneStep();
            }
            StartPlayback();
        }
    }

    void TfFilm::TurnSliderChanged(GI_MessageLoop::TObjectGI* Sender) {
        if (Playing) {
            PausePlayback();
        }
        Globals::FilmCameraFollow = true;
        SelectHistoryEntry(TurnSlider->Position, false);
        AdvanceOneStep();
    }

    void TfFilm::StartPlayback() {
        if (!Playing) {
            if (EffectsTimer != nullptr) {
                CancelCallbackTimer(EffectsTimer);
                EffectsTimer = nullptr;
            }
            PlaybackTimer = ScheduleCallbackTimer(FrameIntervalMs, FrameIntervalMs, pas::bind_method<&TfFilm::AdvancePlayback>(this), 0);
            PlayButton->SetActive(false);
            StopButton->SetActive(true);
            Playing = true;
        }
    }

    // Trailing effects continue on a separate timer.
    void TfFilm::PausePlayback() {
        if (Playing) {
            if (PlaybackTimer != nullptr) {
                CancelCallbackTimer(PlaybackTimer);
                PlaybackTimer = nullptr;
            }
            SpacePanel->SetDragScrollingEnabled(true);
            if (EffectsTimer != nullptr) {
                CancelCallbackTimer(EffectsTimer);
                EffectsTimer = nullptr;
            }
            EffectsTimer = ScheduleCallbackTimer(FrameIntervalMs, FrameIntervalMs, pas::bind_static_method<&TfFilm::AdvancePausedEffects>(this), 0);
            PlayButton->SetActive(true);
            StopButton->SetActive(false);
            Playing = false;
        }
    }

    // Automatically advances to the following retained recording when this one ends.
    void TfFilm::AdvancePlayback(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        AdvanceOneStep();
        if (NextCommand == nullptr) {
            PausePlayback();
            {
                std::int32_t cpp_right = Globals::FilmHistory->GetCount() - 1;
                if (CurrentHistoryIndex < cpp_right) {
                    SelectHistoryEntry(CurrentHistoryIndex + 1, false);
                    AdvanceOneStep();
                    StartPlayback();
                }
            }
        }
    }

    // Requires NextCommand <> nil.
    void TfFilm::AdvanceOneStep() {
        if (Globals::TrailingFilmEffects != nullptr) {
            Globals::TrailingFilmEffects->AdvanceEffects();
            if (Globals::TrailingFilmEffects->FirstEntry == nullptr) {
                pas::free(Globals::TrailingFilmEffects);
                Globals::TrailingFilmEffects = nullptr;
            }
        }
        Globals::SpaceProcess->Space->AdvanceTimers();
        if (NextCommand->Kind == aEFilm::efcBeginTrailingEffects) {
            if (Globals::TrailingFilmEffects != nullptr) {
                pas::free(Globals::TrailingFilmEffects);
                Globals::TrailingFilmEffects = nullptr;
            }
            Globals::TrailingFilmEffects = pas::construct_call<aEFilmEnd::TEFilmEnd>(aEFilmEnd::TEFilmEnd_Create);
            Globals::TrailingFilmEffects->TakeTrailingEffects(CurrentFilm);
            ++StepIndex;
            FrameSlider->SetPositionInternal(StepIndex);
            NextCommand = NextCommand->Next;
        } else {
            ++StepIndex;
            FrameSlider->SetPositionInternal(StepIndex);
            while (NextCommand != nullptr) {
                if (NextCommand->Kind == aEFilm::efcBeginTrailingEffects) {
                    break;
                }
                if (NextCommand->StepIndex >= StepIndex) {
                    break;
                }
                CurrentFilm->ExecuteCommand(Globals::SpaceProcess, NextCommand, true);
                NextCommand = NextCommand->Next;
            }
        }
    }

    void TfFilm::InvalidateAnimatedControls() {
        UpdateRectsEnabled = true;
        SpacePanel->InvalidateChildren(true);
        CursorControl->Invalidate();
        UpdateRectsEnabled = false;
    }

    void TfFilm::DrawFrame() {
        if (Globals::MinimapFrameCounter % 16 == 0) {
            Globals::SpaceProcess->Space->DrawMinimap();
        }
        ++Globals::MinimapFrameCounter;
        InvalidateAnimatedControls();
        GI_StarField::TStarFieldGI* StarField = pas::checked_cast<GI_StarField::TStarFieldGI*>(GetByName(u"StarField"sv));
        StarField->UpdateBackgroundBounds();
        if (GlobalsV::SkipSavedPixelRestore || GR_Main::FullFrameRedrawRequested) {
            UpdateRects->Clear();
            UpdateRectsEnabled = true;
            InvalidateViewport();
            UpdateRectsEnabled = false;
        }
        GR_Main::FullFrameRedrawRequested = false;
        GI_MessageLoop::TMessageLoopGI_RestoreSavedPixels16(this);
        ErasePreviousFrame();
        GR_Rect::TRectGR* RectNode = UpdateRects->FirstRect;
        while (RectNode != nullptr) {
            StarField->DrawBackground(RectNode->Bounds);
            RectNode = RectNode->Next;
        }
        PrepareFrameDraw();
        DrawQueuedControlRects();
        if (!GR_Main::BeginFramePresentation()) {
            GlobalsV::RequestedScreenId = GlobalsV::screenNone;
            GlobalsV::PostLoadScreenId = GlobalsV::FormToId(this);
            RequestClose(1);
        } else {
            FinishQueuedDraw();
            CommitFrameDraw();
            ResetSecondaryPixelCount();
            GR_Main::EndFramePresentation();
            InvalidateAnimatedControls();
        }
    }

    void TfFilm::SelectMusic() {
        if (GlobalsV::MusicInSpaceEnabled) {
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 20) {
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

    void TfFilmLoader::virtual_TThreadEC_Execute() {
        fFilm::TfFilmLoader_Execute(this);
    }

} // namespace fFilm
