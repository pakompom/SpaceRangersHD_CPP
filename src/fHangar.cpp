#include "layout/fHangar.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Data.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SE_Space.hpp"
#include "types/SE_Star.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aPirate.hpp"
#include "types/aPlanet.hpp"
#include "types/aRuins.hpp"
#include "types/aShip.hpp"
#include "types/aTranclucator.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fSaveManager.hpp"
#include "types/fShip2.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/ThreadCalc.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fHangar.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fPanelPlanet.hpp"
#include "units/fPanelRuins.hpp"

namespace fHangar {
    const pas::Array<pas::Array<float, 0, 7>, 0, 2> HangarDominatorPortraitScales = pas::Array<pas::Array<float, 0, 7>, 0, 2>{{pas::Array<float, 0, 7>{{1.0f, 1.1f, 1.1f, 0.7f, 0.7f, 0.3f, 1.0f, 0.5f}}, pas::Array<float, 0, 7>{{1.0f, 1.2f, 0.9f, 0.9f, 0.8f, 0.7f, 1.0f, 0.5f}}, pas::Array<float, 0, 7>{{1.0f, 1.1f, 0.9f, 0.7f, 0.6f, 0.5f, 1.0f, 0.5f}}}};

    void TfHangar_Create(TfHangar* Self) {
        std::int32_t I{};
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->PlanetPanel = pas::construct_call<fPanelPlanet::TfPanelPlanet>(fPanelPlanet::TfPanelPlanet_Create);
        Self->StationPanel = pas::construct_call<fPanelRuins::TfPanelRuins>(fPanelRuins::TfPanelRuins_Create);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
        for (I = 0; I <= 8; ++I) {
            Self->ShipSlots[I].ImageBuffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        }
        Self->SelectedShip = nullptr;
    }

    void TfHangar_Destroy(TfHangar* Self) {
        std::int32_t I{};
        if (Self->StationPanel != nullptr) {
            pas::free(Self->StationPanel);
            Self->StationPanel = nullptr;
        }
        if (Self->PlanetPanel != nullptr) {
            pas::free(Self->PlanetPanel);
            Self->PlanetPanel = nullptr;
        }
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        for (I = 0; I <= 8; ++I) {
            if (Self->ShipSlots[I].ImageBuffer != nullptr) {
                pas::free(Self->ShipSlots[I].ImageBuffer);
                Self->ShipSlots[I].ImageBuffer = nullptr;
            }
        }
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfHangar::InitializeLayout() {
        std::int32_t I{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        PlanetPanel->InitializeLayout(this);
        StationPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fHangar... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* AnimOpen = MainPanel->FindByNameRecursive(u"AnimOpen"_wref.get());
                AnimOpen->SetPosition(ClassesImports::Point(AnimOpen->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, AnimOpen->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* AnimRnd = MainPanel->FindByNameRecursive(u"AnimRnd"_wref.get());
                AnimRnd->SetPosition(ClassesImports::Point(AnimRnd->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, AnimRnd->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* AnimRepair = MainPanel->FindByNameRecursive(u"AnimRepair"_wref.get());
                AnimRepair->SetPosition(ClassesImports::Point(AnimRepair->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, AnimRepair->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* AnimFuel = MainPanel->FindByNameRecursive(u"AnimFuel"_wref.get());
                AnimFuel->SetPosition(ClassesImports::Point(AnimFuel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, AnimFuel->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship0 = MainPanel->FindByNameRecursive(u"Ship0"_wref.get());
                Ship0->SetPosition(ClassesImports::Point(Ship0->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship0->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship1 = MainPanel->FindByNameRecursive(u"Ship1"_wref.get());
                Ship1->SetPosition(ClassesImports::Point(Ship1->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship1->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship2 = MainPanel->FindByNameRecursive(u"Ship2"_wref.get());
                Ship2->SetPosition(ClassesImports::Point(Ship2->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship2->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship3 = MainPanel->FindByNameRecursive(u"Ship3"_wref.get());
                Ship3->SetPosition(ClassesImports::Point(Ship3->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship3->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship4 = MainPanel->FindByNameRecursive(u"Ship4"_wref.get());
                Ship4->SetPosition(ClassesImports::Point(Ship4->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship4->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship5 = MainPanel->FindByNameRecursive(u"Ship5"_wref.get());
                Ship5->SetPosition(ClassesImports::Point(Ship5->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship5->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship6 = MainPanel->FindByNameRecursive(u"Ship6"_wref.get());
                Ship6->SetPosition(ClassesImports::Point(Ship6->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship6->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship7 = MainPanel->FindByNameRecursive(u"Ship7"_wref.get());
                Ship7->SetPosition(ClassesImports::Point(Ship7->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship7->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ship8 = MainPanel->FindByNameRecursive(u"Ship8"_wref.get());
                Ship8->SetPosition(ClassesImports::Point(Ship8->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ship8->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            MainPanel->FindByNameRecursive(u"BGCity2"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGCity"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* OpenImage = MainPanel->FindByNameRecursive(u"OpenImage"_wref.get());
                OpenImage->SetPosition(ClassesImports::Point(OpenImage->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, OpenImage->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PanelUp = MainPanel->FindByNameRecursive(u"PanelUp"_wref.get());
                PanelUp->SetPosition(ClassesImports::Point(PanelUp->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelUp->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PanelDown = MainPanel->FindByNameRecursive(u"PanelDown"_wref.get());
                PanelDown->SetPosition(ClassesImports::Point(PanelDown->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelDown->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        ShipInfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"InfoShip"_wref.get()));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 8); cpp_range.next(I); ) {
            ShipSlots[I].ImageControl = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Ship", SysUtils::IntToStr(I)}))));
        }
        GetByName(u"MainPanel"_wref.get())->MouseMoveCallback = pas::bind_method<&TfHangar::MainMouseMove>(this);
        GetByName(u"MainPanel"_wref.get())->RightButtonDownCallback = pas::bind_method<&TfHangar::MainRightButtonDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"_wref.get()))->UpCallback = pas::bind_method<&TfHangar::EndTurnClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"_wref.get()))->UpCallback = pas::bind_method<&TfHangar::ShipClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRepair"_wref.get()))->UpCallback = pas::bind_method<&TfHangar::RepairHullClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRefuel"_wref.get()))->UpCallback = pas::bind_method<&TfHangar::RefuelClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButTakeOff"_wref.get()))->UpCallback = pas::bind_method<&TfHangar::TakeOffClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"_wref.get()))->UpCallback = pas::bind_method<&fPanelPlanet::TfPanelPlanet::PlanetClicked>(PlanetPanel);
    }

    void TfHangar::OnOpen() {
        std::int32_t I{};
        pas::WideString Path{};
        HoveredShip = nullptr;
        LoadPanel->OnOpen();
        if (AmbientAnimationTimer != nullptr) {
            CancelCallbackTimer(AmbientAnimationTimer);
            AmbientAnimationTimer = nullptr;
        }
        if (DockedShipsTimer != nullptr) {
            CancelCallbackTimer(DockedShipsTimer);
            DockedShipsTimer = nullptr;
        }
        DockedShipsTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfHangar::AnimateDockedShips>(this), 0);
        GetByName(u"AnimRepair"_wref.get())->SetActive(false);
        GetByName(u"AnimFuel"_wref.get())->SetActive(false);
        SelectMusic();
        MainPanel->OnOpen();
        MainPanel->NavigationLocked = false;
        MainPanel->Show();
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            fPanelPlanet::TfPanelPlanet::OnOpen();
            PlanetPanel->Show();
            StationPanel->Hide();
        } else {
            PlanetPanel->Hide();
            StationPanel->OnOpen();
            StationPanel->Show();
        }
        TakeOffPending = false;
        {
            GI_GraphButton::TGraphButtonGI* ButClose = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"_wref.get()));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                ButClose->UpCallback = pas::bind_method<&fPanelPlanet::TfPanelPlanet::PlanetClicked>(PlanetPanel);
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                ButClose->UpCallback = pas::bind_method<&fPanelRuins::TfPanelRuins::ServicesClicked>(StationPanel);
            }
        }
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfHangar::MainKeyDown>(this);
        {
            GI_Image::TImageGI* BGCity2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity2"_wref.get()));
            BGCity2->SetActive(aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase));
            if (BGCity2->Active) {
                BGCity2->SetImagePath(pas::concat_wide({u"GAI,", aPlayer::GetPlayer()->CurrentStar->GetBackgroundImagePath(I)}));
                BGCity2->GaiImageControl->LoadFrameSequenceFromText(u"[50,0-0]"_wref.get());
                BGCity2->SetImageKindX(GI_Main::ikxCenter);
                BGCity2->SetImageKindY(GI_Main::ikyCenter);
            }
        }
        {
            GI_Image::TImageGI* BGCity = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity"_wref.get()));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                BGCity->SetActive(true);
                {
                    pas::WideString governmentBackgroundGraph = aPlayer::GetPlayer()->CurrentPlanet->GetGovernmentBackgroundGraph();
                    GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(BGCity->FindByNameRecursive(u"BGCity"_wref.get()));
                    cpp_arg->SetImagePath(std::move(governmentBackgroundGraph));
                }
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                BGCity->SetActive(true);
                if (aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey != u"") {
                    Path = pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey, u"bg"});
                    if (GR_Main::CacheDataRoot->FileExistsByPath(Path)) {
                        BGCity->SetImagePath(pas::concat_wide({u"GI,", Path}));
                    } else {
                        BGCity->SetImagePath(pas::concat_wide({u"GI,Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[aPlayer::GetPlayer()->DockedTo->TypeId].Name, u"bg"}));
                    }
                } else {
                    BGCity->SetImagePath(pas::concat_wide({u"GI,Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[aPlayer::GetPlayer()->DockedTo->TypeId].Name, u"bg"}));
                }
            } else {
                BGCity->SetActive(false);
            }
        }
        if (GlobalsV::AnimHangar) {
            {
                GI_GAI::TgaiGI* AnimOpen = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimOpen"_wref.get()));
                AnimOpen->SetActive(true);
                AnimOpen->SetSequenceFrame(0);
                AnimOpen->RestartPlayback();
                AnimOpen->CycleCompleteCallback = pas::bind_method<&TfHangar::AmbientAnimationComplete>(this);
            }
            {
                GI_GAI::TgaiGI* AnimRnd = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimRnd"_wref.get()));
                AnimRnd->SetActive(false);
                AnimRnd->StopAutoPlayback();
                AnimRnd->CycleCompleteCallback = pas::bind_method<&TfHangar::AmbientAnimationComplete>(this);
            }
            DrawQueuedUpdateRects();
            GR_Main::SoundManager->PlaySound(u"Sound.HangarOpen"_wref.get());
            GetByName(u"OpenImage"_wref.get())->SetActive(false);
        } else {
            GetByName(u"AnimOpen"_wref.get())->SetActive(false);
            GetByName(u"AnimRnd"_wref.get())->SetActive(false);
            GetByName(u"OpenImage"_wref.get())->SetActive(true);
        }
        GetByName(u"PanelUp"_wref.get())->SetActive(true);
        GetByName(u"PanelDown"_wref.get())->SetActive(true);
        {
            GI_Image::TImageGI* FaceI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"FaceI"_wref.get()));
            FaceI->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), u"Dispatcheri"}));
            FaceI->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
            FaceI->SetImageKindX(GI_Main::ikxCenter);
            FaceI->SetImageKindY(GI_Main::ikyCenter);
            FaceI->SetActive(true);
        }
        {
            GI_GAI::TgaiGI* FaceA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
            FaceA->UserValue = 0;
            FaceA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            FaceA->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), u"Dispatchera"}));
            FaceA->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
            FaceA->SequenceIndex = 0;
            FaceA->UpdateAutoGeometry();
            FaceA->SetSequenceFrame(aMyFunction::RandomIntRange(0, FaceA->SequenceFrameCount - 1));
            FaceA->SetImageKindX(GI_Main::ikxCenter);
            FaceA->SetImageKindY(GI_Main::ikyCenter);
            FaceA->SetActive(true);
            FaceA->CycleCompleteCallback = pas::bind_method<&TfHangar::DispatcherAnimationComplete>(this);
            FaceA->RestartPlayback();
        }
        {
            GI_GAI::TgaiGI* FaceA_2 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
            FaceA_2->FrameAdvancedCallback = pas::bind_method<&TfHangar::CaptureDispatcherMirror>(this);
        }
        GetByName(u"FaceGB"_wref.get())->SetActive(false);
        {
            GI_Image::TImageGI* CaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"_wref.get()));
            CaptainI->SetImagePath(pas::concat_wide({u"GI,", aPlayer::GetPlayer()->GetCaptainPortraitResourceBase(), u"i"}));
            CaptainI->SetImageKindX(GI_Main::ikxCenter);
            CaptainI->SetImageKindY(GI_Main::ikyCenter);
            CaptainI->SetActive(true);
        }
        {
            GI_GAI::TgaiGI* CaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"_wref.get()));
            CaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            CaptainA->SetImagePath(pas::concat_wide({aPlayer::GetPlayer()->GetCaptainPortraitResourceBase(), u"a"}));
            CaptainA->SequenceIndex = 0;
            CaptainA->UpdateAutoGeometry();
            CaptainA->SetImageKindX(GI_Main::ikxCenter);
            CaptainA->SetImageKindY(GI_Main::ikyCenter);
            CaptainA->SetActive(true);
            CaptainA->RestartPlayback();
        }
        CaptureDispatcherMirror(nullptr);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 8); cpp_range.next(I); ) {
            ShipSlots[I].AnimationState = 0;
            ShipSlots[I].Opacity = 0;
            GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Ship", SysUtils::IntToStr(I)})))->SetActive(false);
        }
        RefreshDockedShips();
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(210);
        RefreshServiceButtons();
        MainPanel->RebuildMessageButtons(false);
    }

    void TfHangar::OnClose() {
        std::int32_t I{};
        aGalaxy::Galaxy->CheckIntegrityChecksum(211);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        ShipInfoWindow->SetActive(false);
        HoveredShip = nullptr;
        if (ShipInfoHideTimer != nullptr) {
            CancelCallbackTimer(ShipInfoHideTimer);
            ShipInfoHideTimer = nullptr;
        }
        LoadPanel->OnClose();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 8); cpp_range.next(I); ) {
            pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Ship", SysUtils::IntToStr(I)}))))->GraphBuf->Clear();
            if (ShipSlots[I].ImageBuffer != nullptr) {
                ShipSlots[I].ImageBuffer->Clear();
            }
        }
        if (AmbientAnimationTimer != nullptr) {
            CancelCallbackTimer(AmbientAnimationTimer);
            AmbientAnimationTimer = nullptr;
        }
        if (DockedShipsTimer != nullptr) {
            CancelCallbackTimer(DockedShipsTimer);
            DockedShipsTimer = nullptr;
        }
        if (GlobalsV::RequestedScreenId != GlobalsV::screenScanner) {
            GR_Main::SoundManager->StopUncontrolledSounds();
        }
        MainPanel->OnClose();
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsOnPlanet()) {
            fPanelPlanet::TfPanelPlanet::OnClose();
        } else {
            fPanelRuins::TfPanelRuins::OnClose();
        }
    }

    void TfHangar::CaptureDispatcherMirror(GI_MessageLoop::TObjectGI* Sender) {
        WindowsSdk::TPoint Position{};
        if (GlobalsV::HardwareRenderingEnabled) {
            return;
        }
        std::uint8_t WasActive = ShipInfoWindow->Active;
        ShipInfoWindow->SetActive(false);
        {
            GI_GraphBuf::TGraphBufGI* FaceGB = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"FaceGB"_wref.get()));
            FaceGB->SetActive(false);
            if (!GR_Main::ShowSystemMouse) {
                SetCursorActive(false);
            }
            DrawQueuedUpdateRects();
            if (!GR_Main::ShowSystemMouse) {
                SetCursorActive(true);
            }
            Position = FaceGB->ToAbsolutePoint(ClassesImports::Point(0, 0));
            FaceGB->GraphBuf->AllocateNative(FaceGB->ClientSize.X, FaceGB->ClientSize.Y);
            {
                std::uint32_t cpp_left = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(GR_Main::ScreenRenderBuffer->GetPixels())) + static_cast<std::uint32_t>(Position.X * 2);
                void* cpp_arg = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(cpp_left + static_cast<std::uint32_t>(Position.Y * GR_Main::ScreenRenderBuffer->PitchBytes))));
                std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                std::int32_t x = FaceGB->ClientSize.X;
                std::int32_t y = FaceGB->ClientSize.Y;
                void* pixels = FaceGB->GraphBuf->GetPixels();
                std::int32_t pitchBytes_2 = FaceGB->GraphBuf->PitchBytes;
                GR_Main::Ex_OKGR_Copy_XY_XY_WORD(pixels, pitchBytes_2, 0, 0, cpp_arg, pitchBytes, 0, 0, x, y);
            }
            FaceGB->GraphBuf->FlipHorizontal16();
            FaceGB->SetActive(true);
        }
        ShipInfoWindow->SetActive(WasActive);
    }

    void TfHangar::DispatcherAnimationComplete(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Alternate = 0;
        if (Sender->UserValue != 0) {
            Alternate = 0;
        } else if (aMyFunction::RandomIntRange(0, 2) == 0) {
            Alternate = 1;
        }
        {
            GI_Image::TImageGI* FaceI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"FaceI"_wref.get()));
            if (Alternate == 0) {
                FaceI->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), u"Dispatcheri"}));
            } else {
                FaceI->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), u"Dispatcher2i"}));
            }
            FaceI->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
            FaceI->SetImageKindX(GI_Main::ikxCenter);
            FaceI->SetImageKindY(GI_Main::ikyCenter);
            FaceI->SetActive(true);
        }
        {
            GI_GAI::TgaiGI* FaceA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
            FaceA->UserValue = Alternate;
            FaceA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            if (Alternate == 0) {
                FaceA->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), u"Dispatchera"}));
            } else {
                FaceA->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), u"Dispatcher2a"}));
            }
            FaceA->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
            FaceA->SequenceIndex = 0;
            FaceA->UpdateAutoGeometry();
            FaceA->SetSequenceFrame(0);
            FaceA->SetImageKindX(GI_Main::ikxCenter);
            FaceA->SetImageKindY(GI_Main::ikyCenter);
            FaceA->SetActive(true);
            FaceA->CycleCompleteCallback = pas::bind_method<&TfHangar::DispatcherAnimationComplete>(this);
            FaceA->RestartPlayback();
        }
    }

    // Orders player takeoff and runs campaign turn/transitions when accepted. Self is unused.
    std::uint8_t TfHangar::TryTakeOff() {
        std::uint8_t Result{};
        std::int32_t I{};
        aGalaxy::Galaxy->CheckIntegrityChecksum(212);
        Globals::PruneExpiredPersistentPlayerMessages();
        aPlayer::GetPlayer()->OrderTakeoff();
        if (aPlayer::GetPlayer()->Order != aShip::soTakeoff) {
            Result = false;
            aGalaxy::Galaxy->PrimeIntegrityChecksum(212);
            return Result;
        }
        Result = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range.next(I); ) {
            pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, I)->RunTurnCode();
        }
        Globals::StarMapWeaponPanelOpen = false;
        Globals::FilmCameraFollow = true;
        aGalaxy::PlayerStar->RefreshSpaceObjectPositions();
        fEquipmentShop::RestoreTemporaryShopStock();
        aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 1);
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(3)) {
            aGalaxy::Galaxy->EnableDominatorSurfaces();
        } else {
            aGalaxy::Galaxy->DisableDominatorSurfaces();
        }
        aCalc::CalculatePlayerStarTurnAndWait();
        if (GR_Main::ExitScreenLoop) {
            return Result;
        }
        if (aPlayer::GetPlayer() == nullptr) {
            GlobalsV::GameEndReason = 2;
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
            reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
            return Result;
        }
        aCalc::CalculateGalaxyTurnAndWait();
        Globals::StarMapScreen->ResumeMode = fStarMap::smrTurnFilm;
        Globals::ScreenLoadMode = 2;
        GlobalsV::PostLoadScreenId = GlobalsV::screenStarMap;
        GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
        return Result;
    }

    void TfHangar::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion) && aPlayer::GetPlayer()->DockedTo->Order == aShip::soTeleport && static_cast<std::uint32_t>(aPlayer::GetPlayer()->DockedTo->OrderStateData) > 0 && static_cast<std::uint8_t>(aPlayer::GetPlayer()->DockedTo->InHyperspace ^ 1)) {
            Globals::RuinsTalkScreen->DepartWithStation(1);
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion) && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != aPlayer::GetPlayer()->CurrentStar && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate <= aGalaxy::Galaxy->CurrentTurn) {
            Globals::RuinsTalkScreen->DepartWithStation(1);
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase) && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != aPlayer::GetPlayer()->CurrentStar && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate <= aGalaxy::Galaxy->CurrentTurn) {
            if (aPlayer::GetPlayer()->Speed <= 0) {
                Globals::RuinsTalkScreen->DepartWithStation(1);
            } else {
                StationPanel->TakeOffForStationTravel();
            }
            return;
        }
        if (LoadPanel->IsAnimatingShutters()) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(213);
        fEquipmentShop::RestoreTemporaryShopStock();
        MainPanel->EndTurnClicked(Sender);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(227);
        RefreshServiceButtons();
        MainPanel->RebuildMessageButtons(false);
        if (ExitCode == 0) {
            fEquipmentShop::BuildTemporaryShopSlotGrid();
            aGalaxy::Galaxy->PrimeIntegrityChecksum(214);
            RefreshDockedShips();
        }
    }

    void TfHangar::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (IsServiceButtonDown()) {
            return;
        }
        ShipInfoWindow->SetActive(false);
        Globals::ShipScreen->ShipToInspect = SelectedShip;
        MainPanel->ShipClicked(Sender);
        Globals::ShipScreen->ShipToInspect = nullptr;
        SelectedShip = nullptr;
        MainPanel->RebuildMessageButtons(false);
        MainPanel->RefreshMoneyAndCargo();
        if (Globals::ShipScreen->Flag3BC) {
            ShipSlots[0].AnimationState = 0;
            ShipSlots[0].Opacity = 0;
            GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Ship", SysUtils::IntToStr(0)})))->SetActive(false);
            RefreshDockedShips();
        }
        RefreshServiceButtons();
    }

    void TfHangar::BeginTakeOff() {
        MainPanel->NavigationLocked = true;
        if (!TfHangar::TryTakeOff()) {
            MainPanel->NavigationLocked = false;
            return;
        }
        LoadPanel->SelectBackgroundStyle(0);
        LoadPanel->RefreshBackgroundImages();
        LoadPanel->StartClosingShutters();
    }

    void TfHangar::MainKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (ExitCode != 0 || LoadPanel->IsAnimatingShutters() || IsServiceButtonDown() || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            return;
        }
        if (Key == WindowsSdk::VK_SPACE) {
            if (GetByName(u"PM_EndTurn"_wref.get())->Active) {
                EndTurnClicked(nullptr);
            }
        } else if (Key == 'F') {
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButTakeOff"_wref.get()))->Disabled) {
                TakeOffClicked(nullptr);
            }
        } else if (Key == 'A') {
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRepair"_wref.get()))->Disabled) {
                RepairHullClicked(nullptr);
            }
        } else if (Key == 'B') {
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRefuel"_wref.get()))->Disabled) {
                RefuelClicked(nullptr);
            }
        } else if (Key == 'S') {
            SelectedShip = aPlayer::GetPlayer();
            ShipClicked(nullptr);
        } else {
            MainPanel->ProcessKeyDown(Key);
            if (aPlayer::GetPlayer()->IsDockedToShip()) {
                StationPanel->ProcessKeyDown(Key);
            } else if (aPlayer::GetPlayer()->IsOnPlanet()) {
                PlanetPanel->ProcessKeyDown(Key);
            }
        }
    }

    void TfHangar::SelectMusic() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->GetShutterDirection() == -1 || TakeOffPending) {
            return;
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
            return;
        }
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId) & 0x0000007f].InternalName, u"Pirate"}));
                } else {
                    GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
                }
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
            }
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
                return;
            }
            if (pas::in_set<7, 7, 12, 12>(aPlayer::GetPlayer()->DockedTo->TypeId)) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace) & 0x0000007f].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace) & 0x0000007f].InternalName}));
            }
        }
    }

    std::uint8_t TfHangar::IsServiceButtonDown() {
        std::uint8_t Result = true;
        if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButTakeOff"_wref.get()))->Down || pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRepair"_wref.get()))->Down || pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRefuel"_wref.get()))->Down) {
            return Result;
        }
        return false;
    }

    void TfHangar::RefreshServiceButtons() {
        {
            GI_GraphButton::TGraphButtonGI* ButRepair = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRepair"_wref.get()));
            ButRepair->SetDisabled(aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->GetHull()->HullPoints >= aPlayer::GetPlayer()->GetHull()->Weight);
            if (ButRepair->Disabled) {
                ButRepair->HelpText = aConst::LocalizedColorText(u"Help.ButRepair"_wref.get());
            } else {
                ButRepair->HelpText = pas::concat_wide({aConst::LocalizedColorText(u"Help.ButRepair"_wref.get()), u" ", ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->GetHull()->CalculateRepairCost());
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormHangar.HullStatus.Cost"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
                }())});
            }
        }
        {
            GI_GraphButton::TGraphButtonGI* ButRefuel = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRefuel"_wref.get()));
            ButRefuel->SetDisabled(aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->GetFullRefuelCost() <= 0);
            if (ButRefuel->Disabled) {
                ButRefuel->HelpText = aConst::LocalizedColorText(u"Help.ButRefuel"_wref.get());
            } else {
                ButRefuel->HelpText = pas::concat_wide({aConst::LocalizedColorText(u"Help.ButRefuel"_wref.get()), u" ", ([&] {
                    pas::WideString intToStr_2 = pas::wide_int_to_str(aPlayer::GetPlayer()->GetFullRefuelCost());
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.Cost"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2));
                }())});
            }
        }
        {
            GI_GraphButton::TGraphButtonGI* cpp_arg = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButTakeOff"_wref.get()));
            std::uint8_t cpp_arg_2 = static_cast<std::uint8_t>(RefreshTakeOffStatus() ^ 1);
            cpp_arg->SetDisabled(cpp_arg_2);
        }
    }

    // Insufficient funds buy a proportional partial repair.
    void TfHangar::RepairHullClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (LoadPanel->IsAnimatingShutters() || MainPanel->NavigationLocked || aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer()->Money <= 0) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(215);
            aPlayer::GetPlayer()->SetMoney(0);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(216);
            GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->GetHull()->CalculateRepairCost());
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormHangar.HullStatus.NotMoney"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
            }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
            MainPanel->FlashMoneyWarning();
            return;
        }
        if (aPlayer::GetPlayer()->GetHull()->CalculateRepairCost() > aPlayer::GetPlayer()->Money) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(217);
            aPlayer::GetPlayer()->GetHull()->HullPoints += System::Round(pas::real_divide(aPlayer::GetPlayer()->Money, aPlayer::GetPlayer()->GetHull()->CalculateRepairCost()) * (aPlayer::GetPlayer()->GetHull()->Weight - aPlayer::GetPlayer()->GetHull()->HullPoints));
            aPlayer::GetPlayer()->SetMoney(0);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(218);
            GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
        } else {
            aGalaxy::Galaxy->CheckIntegrityChecksum(219);
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - aPlayer::GetPlayer()->GetHull()->CalculateRepairCost());
            aPlayer::GetPlayer()->GetHull()->HullPoints = aPlayer::GetPlayer()->GetHull()->Weight;
            aGalaxy::Galaxy->PrimeIntegrityChecksum(220);
            GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
        }
        {
            GI_GAI::TgaiGI* AnimRepair = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimRepair"_wref.get()));
            AnimRepair->SetActive(true);
            AnimRepair->SetSequenceFrame(0);
            AnimRepair->RestartPlayback();
            AnimRepair->CycleCompleteCallback = pas::bind_static_method<&TfHangar::StopAnimation>(this);
        }
        RefreshServiceButtons();
        SetHoveredControl(nullptr);
        GR_Main::PostMouseMoveMessage();
    }

    // Requires enough money to fill the tank completely.
    void TfHangar::RefuelClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (LoadPanel->IsAnimatingShutters() || MainPanel->NavigationLocked || aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer()->GetFullRefuelCost() > aPlayer::GetPlayer()->Money) {
            GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->GetFullRefuelCost());
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.NotMoney"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
            }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
            MainPanel->FlashMoneyWarning();
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(221);
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - aPlayer::GetPlayer()->GetFullRefuelCost());
        aPlayer::GetPlayer()->GetFuelTanks()->Fuel = aPlayer::GetPlayer()->GetFuelTanks()->Capacity;
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(222);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        {
            GI_GAI::TgaiGI* AnimFuel = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimFuel"_wref.get()));
            AnimFuel->SetActive(true);
            AnimFuel->SetSequenceFrame(0);
            AnimFuel->RestartPlayback();
            AnimFuel->CycleCompleteCallback = pas::bind_static_method<&TfHangar::StopAnimation>(this);
        }
        RefreshServiceButtons();
        SetHoveredControl(nullptr);
        GR_Main::PostMouseMoveMessage();
    }

    void TfHangar::TakeOffClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (LoadPanel->IsAnimatingShutters() || MainPanel->NavigationLocked || aScript::HasPendingScriptRequests() || aCalc::TurnCalculationPhase == ThreadCalc::tcpGalaxyRunning || aCalc::TurnCalculationPhase == ThreadCalc::tcpPlayerStarRunning) {
            return;
        }
        GR_Main::CaptureSavePreview();
        aGalaxy::Galaxy->CheckIntegrityChecksum(223);
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(223);
        aRanger::PlayerAutomaticControl = false;
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRepair"_wref.get()))->SetDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRefuel"_wref.get()))->SetDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButTakeOff"_wref.get()))->SetDisabled(true);
        TakeOffPending = true;
        if (GR_Main::MusicManager->CategoryOverride == u"") {
            GR_Main::MusicManager->RequestFadeOut();
        }
        EC_Cache::EvictRuinsAndGovernmentCaches();
        GR_DX::ReleaseAllTextureSurfaces();
        BeginTakeOff();
    }

    void TfHangar::StopAnimation(GI_MessageLoop::TObjectGI* Sender) {
        GI_GAI::TgaiGI* cpp_with = pas::checked_cast<GI_GAI::TgaiGI*>(Sender);
        cpp_with->SetActive(false);
        cpp_with->StopAutoPlayback();
    }

    // Refreshes hull, fuel and engine warnings and reports whether takeoff is allowed.
    std::uint8_t TfHangar::RefreshTakeOffStatus() {
        pas::WideString TotalText{};
        pas::WideString Text{};
        std::uint8_t Warning{};
        if (aPlayer::GetPlayer() == nullptr) {
            return false;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(225);
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(226);
        std::uint8_t Result = true;
        TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Good"_wref.get());
        {
            aPlayer::TPlayer* cpp_with = aPlayer::GetPlayer();
            Text = pas::WideString();
            Warning = false;
            if (cpp_with->GetHull()->HullPoints >= cpp_with->GetHull()->Weight) {
                Text = aConst::LocalizedColorText(u"FormHangar.HullStatus.Ok"_wref.get());
            } else {
                Text = aConst::LocalizedColorText(u"FormHangar.HullStatus.NeedRepair"_wref.get());
                Warning = true;
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Text1"_wref.get()))->SetText(Text);
            GetByName(u"Light1"_wref.get())->SetActive(Warning);
            Text = pas::WideString();
            Warning = false;
            if (cpp_with->GetFuelTanks() == nullptr) {
                Text = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.Non"_wref.get());
                TotalText = aConst::LocalizedText(u"FormHangar.TotalStatus.Bad"_wref.get());
                Result = false;
                Warning = true;
            } else if (!cpp_with->CanUseEquipmentTech(cpp_with->GetFuelTanks())) {
                Text = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.CanNotUse"_wref.get());
                TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Bad"_wref.get());
                Result = false;
                Warning = true;
            } else if (cpp_with->GetFuelTanks()->Fuel == 0) {
                Text = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.Empty"_wref.get());
                TotalText = aConst::LocalizedText(u"FormHangar.TotalStatus.Bad"_wref.get());
                Result = false;
                Warning = true;
            } else if (cpp_with->GetFuelTanks()->BrokenFlag != 0) {
                Text = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.NeedRepair"_wref.get());
                TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Nearly"_wref.get());
                Warning = true;
            } else if (cpp_with->GetFuelTanks()->ConditionPercent < 2.0E+1L) {
                Text = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.SmallDuration"_wref.get());
                TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Nearly"_wref.get());
                Warning = true;
            } else if (cpp_with->GetFuelTanks()->ConditionPercent < 5.0E+1L) {
                Text = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.AverageDuration"_wref.get());
                TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Nearly"_wref.get());
                Warning = true;
            } else if (cpp_with->GetFuelTanks()->Fuel < cpp_with->GetFuelTanks()->Capacity) {
                Text = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.NeedFuel"_wref.get());
            } else {
                Text = aConst::LocalizedColorText(u"FormHangar.FuelTankStatus.Ok"_wref.get());
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Text2"_wref.get()))->SetText(Text);
            GetByName(u"Light2"_wref.get())->SetActive(Warning);
            Text = pas::WideString();
            Warning = false;
            if (cpp_with->GetEngine() == nullptr) {
                Text = aConst::LocalizedColorText(u"FormHangar.EngineStatus.Non"_wref.get());
                TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Bad"_wref.get());
                Result = false;
                Warning = true;
            } else if (static_cast<std::uint8_t>(cpp_with->CanUseEquipmentTech(cpp_with->GetEngine()) ^ 1) || cpp_with->CalculateEngineSpeed(cpp_with->GetEngine(), false) <= 0) {
                Text = aConst::LocalizedColorText(u"FormHangar.EngineStatus.CanNotUse"_wref.get());
                TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Bad"_wref.get());
                Result = false;
                Warning = true;
            } else {
                if (cpp_with->GetEngine()->BrokenFlag != 0) {
                    Text = aConst::LocalizedColorText(u"FormHangar.EngineStatus.NeedRepair"_wref.get());
                    if (Result) {
                        TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Nearly"_wref.get());
                    }
                    Warning = true;
                } else if (cpp_with->GetEngine()->ConditionPercent < 2.0E+1L) {
                    Text = aConst::LocalizedColorText(u"FormHangar.EngineStatus.SmallDuration"_wref.get());
                    if (Result) {
                        TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Nearly"_wref.get());
                    }
                    Warning = true;
                } else if (cpp_with->GetEngine()->ConditionPercent < 5.0E+1L) {
                    Text = aConst::LocalizedColorText(u"FormHangar.EngineStatus.AverageDuration"_wref.get());
                    if (Result) {
                        TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.Nearly"_wref.get());
                    }
                    Warning = true;
                } else {
                    Text = aConst::LocalizedColorText(u"FormHangar.EngineStatus.Ok"_wref.get());
                }
                if (aPlayer::GetPlayer()->DockedTo != nullptr && aPlayer::GetPlayer()->DockedTo->InHyperspace) {
                    TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.RuinInHyperSpace"_wref.get());
                    Result = false;
                }
                if (cpp_with->CargoFreeSpace < 0) {
                    TotalText = aConst::LocalizedColorText(u"FormHangar.TotalStatus.ShipOvercharging"_wref.get());
                    Result = false;
                }
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Text3"_wref.get()))->SetText(Text);
            GetByName(u"Light3"_wref.get())->SetActive(Warning);
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Text4"_wref.get()))->SetText(TotalText);
            GetByName(u"Light4"_wref.get())->SetActive(static_cast<std::uint8_t>(Result ^ 1));
            return Result;
        }
    }

    void TfHangar::AmbientAnimationComplete(GI_MessageLoop::TObjectGI* Sender) {
        if (!GlobalsV::AnimHangar) {
            return;
        }
        {
            GI_GAI::TgaiGI* AnimOpen = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimOpen"_wref.get()));
            AnimOpen->SetActive(true);
            AnimOpen->StopAutoPlayback();
            AnimOpen->SetSequenceFrame(AnimOpen->SequenceFrameCount - 1);
        }
        {
            GI_GAI::TgaiGI* AnimRnd = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimRnd"_wref.get()));
            AnimRnd->SetActive(false);
            AnimRnd->StopAutoPlayback();
        }
        if (AmbientAnimationTimer != nullptr) {
            CancelCallbackTimer(AmbientAnimationTimer);
            AmbientAnimationTimer = nullptr;
        }
        AmbientAnimationTimer = ScheduleCallbackTimer(aMyFunction::RandomIntRange(2000, 4000), 1, pas::bind_method<&TfHangar::StartAmbientAnimation>(this), 0);
    }

    void TfHangar::StartAmbientAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (AmbientAnimationTimer != nullptr) {
            CancelCallbackTimer(AmbientAnimationTimer);
            AmbientAnimationTimer = nullptr;
        }
        if (GlobalsV::AnimHangar) {
            {
                GI_GAI::TgaiGI* AnimOpen = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimOpen"_wref.get()));
                AnimOpen->SetActive(false);
                AnimOpen->StopAutoPlayback();
            }
            {
                GI_GAI::TgaiGI* AnimRnd = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimRnd"_wref.get()));
                AnimRnd->SetActive(true);
                AnimRnd->SetSequenceFrame(0);
                AnimRnd->RestartPlayback();
            }
        }
    }

    float TfHangar::GetShipPortraitScale(aShip::TShip* Ship) {
        if (Ship->ChameleonActive) {
            return HangarDominatorPortraitScales[Ship->ChameleonSeries][Ship->ChameleonVisualType];
        } else if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) {
            return *([&] {
                auto cpp_index = pas::checked_cast<aKling::TKling*>(Ship)->KlingType;
                auto* cpp_array = &HangarDominatorPortraitScales[pas::checked_cast<aKling::TKling*>(Ship)->DominatorSeries];
                return &(*cpp_array)[cpp_index];
            }());
        } else {
            return 1.0f;
        }
    }

    void TfHangar::LoadDockedShipImage(std::int32_t Index, pas::WideString ImagePath, std::uint8_t LargeHull, float Scale) {
        std::int32_t Width{};
        {
            GI_GraphBuf::TGraphBufGI* cpp_with = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Ship", SysUtils::IntToStr(Index)}))));
            cpp_with->SetActive(true);
            cpp_with->SourceHasPerPixelAlpha = true;
            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(ImagePath, 1, u","_wref.get()), ShipSlots[Index].ImageBuffer);
            if (Index <= 0) {
                Width = GR_Main::GiScalePixels(128);
            } else if (Index <= 3) {
                Width = GR_Main::GiScalePixels(100);
            } else {
                Width = GR_Main::GiScalePixels(80);
            }
            if (LargeHull) {
                Width += Width / 2;
            }
            Width = System::Round(static_cast<long double>(Width) * Scale);
            if (ShipSlots[Index].ImageBuffer->Width > Width || ShipSlots[Index].ImageBuffer->Height > Width) {
                ShipSlots[Index].ImageBuffer->RescaleRgba(Width, System::Round(pas::real_divide(Width * ShipSlots[Index].ImageBuffer->Height, ShipSlots[Index].ImageBuffer->Width)), 5);
            }
            cpp_with->SetOrigin(ShipSlots[Index].ImageBuffer->GetPixelCentroid());
            cpp_with->SetSize(ClassesImports::Point(ShipSlots[Index].ImageBuffer->Width, ShipSlots[Index].ImageBuffer->Height));
            cpp_with->SetImageKindX(GI_Main::ikxLeft);
            cpp_with->SetImageKindY(GI_Main::ikyTop);
        }
    }

    void TfHangar::SetDockedShipOpacity(std::int32_t Index, std::uint8_t Alpha) {
        GI_GraphBuf::TGraphBufGI* cpp_with = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Ship", SysUtils::IntToStr(Index)}))));
        cpp_with->GraphBuf->AllocateRgbaTight(ShipSlots[Index].ImageBuffer->Width, ShipSlots[Index].ImageBuffer->Height);
        GR_GraphBuf::TGraphBufGR_CopyRect32(cpp_with->GraphBuf, ClassesImports::Point(0, 0), ShipSlots[Index].ImageBuffer, ClassesImports::Rect(0, 0, ShipSlots[Index].ImageBuffer->Width, ShipSlots[Index].ImageBuffer->Height));
        GR_GraphBuf::TGraphBufGR_ScaleAlpha(cpp_with->GraphBuf, ClassesImports::Rect(0, 0, ShipSlots[Index].ImageBuffer->Width, ShipSlots[Index].ImageBuffer->Height), Alpha);
        cpp_with->Invalidate();
    }

    void TfHangar::AnimateDockedShips(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 8); cpp_range.next(I); ) {
            if (ShipSlots[I].AnimationState == 1) {
                ShipSlots[I].Opacity += 10;
                if (ShipSlots[I].Opacity >= 255) {
                    ShipSlots[I].Opacity = 255;
                    ShipSlots[I].AnimationState = 2;
                }
                SetDockedShipOpacity(I, ShipSlots[I].Opacity);
            } else if (ShipSlots[I].AnimationState == 3) {
                ShipSlots[I].Opacity -= 10;
                if (ShipSlots[I].Opacity <= 0) {
                    ShipSlots[I].Opacity = 0;
                    ShipSlots[I].AnimationState = 0;
                    pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Ship", SysUtils::IntToStr(I)}))))->SetActive(false);
                }
                SetDockedShipOpacity(I, ShipSlots[I].Opacity);
            }
        }
        RefreshDockedShips();
    }

    void TfHangar::RefreshDockedShips() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t Swap{};
        aShip::TShip* Ship{};
        pas::WideString Path{};
        std::uint8_t LargeHull{};
        pas::Array<std::int32_t, 0, 8> SlotOrder{};
        if (ThreadCalc::IsTurnCalculationRunning()) {
            ThreadCalc::WaitForTurnCalculation();
        }
        for (I = 0; I <= 8; ++I) {
            if (ShipSlots[I].AnimationState == 2) {
                Ship = static_cast<aShip::TShip*>(aGalaxy::Galaxy->IdToShip(ShipSlots[I].ShipId, false));
                if (Ship == nullptr || !(aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet == Ship->CurrentPlanet) && !(aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo == Ship->DockedTo)) {
                    ShipSlots[I].AnimationState = 3;
                    ShipSlots[I].Opacity = 255;
                }
            }
        }
        for (I = 1; I <= 8; ++I) {
            if (ShipSlots[I].AnimationState == 0 && ShipSlots[I].ShipId != 0) {
                Ship = static_cast<aShip::TShip*>(aGalaxy::Galaxy->IdToShip(ShipSlots[I].ShipId, false));
                if (Ship != nullptr) {
                    if (!(aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet == Ship->CurrentPlanet || aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo == Ship->DockedTo)) {
                        Ship = nullptr;
                    } else {
                        K = 0;
                        while (K <= 8) {
                            if (ShipSlots[K].AnimationState != 0 && ShipSlots[K].ShipId == Ship->Id) {
                                Ship = nullptr;
                                break;
                            }
                            ++K;
                        }
                    }
                }
                if (Ship == nullptr) {
                    continue;
                }
                Path = pas::WideString();
                LargeHull = false;
                Path = Ship->GetShipPortraitImagePath();
                if (Path == u"") {
                    continue;
                }
                ShipSlots[I].ShipId = Ship->Id;
                ShipSlots[I].AnimationState = 1;
                ShipSlots[I].Opacity = 0;
                LoadDockedShipImage(I, Path, LargeHull, TfHangar::GetShipPortraitScale(Ship));
                SetDockedShipOpacity(I, ShipSlots[I].Opacity);
            }
        }
        for (I = 0; I <= 8; ++I) {
            SlotOrder[I] = I;
        }
        for (I = 0; I <= 10; ++I) {
            J = aMyFunction::RandomIntRange(1, 8);
            K = aMyFunction::RandomIntRange(1, 8);
            Swap = SlotOrder[J];
            SlotOrder[J] = SlotOrder[K];
            SlotOrder[K] = Swap;
        }
        for (I = 0; I <= 8; ++I) {
            if (ShipSlots[SlotOrder[I]].AnimationState == 0) {
                Ship = nullptr;
                if (SlotOrder[I] == 0) {
                    Ship = aPlayer::GetPlayer();
                } else if (Ship == nullptr) {
                    if (aPlayer::GetPlayer()->IsOnPlanet()) {
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(J); ) {
                            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, J);
                            if (aPlayer::GetPlayer() == Ship) {
                                Ship = nullptr;
                            } else if (aPlayer::GetPlayer()->CurrentPlanet != Ship->CurrentPlanet) {
                                Ship = nullptr;
                            } else {
                                K = 0;
                                while (K <= 8) {
                                    if (ShipSlots[K].AnimationState != 0 && ShipSlots[K].ShipId == Ship->Id) {
                                        break;
                                    }
                                    ++K;
                                }
                                if (K > 8) {
                                    break;
                                }
                                Ship = nullptr;
                            }
                        }
                        if (Ship == nullptr) {
                            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentPlanet->Warriors) - 1); cpp_range_2.next(J); ) {
                                Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentPlanet->Warriors, J);
                                if (aPlayer::GetPlayer()->CurrentPlanet != Ship->CurrentPlanet) {
                                    Ship = nullptr;
                                } else {
                                    K = 0;
                                    while (K <= 8) {
                                        if (ShipSlots[K].AnimationState != 0 && ShipSlots[K].ShipId == Ship->Id) {
                                            break;
                                        }
                                        ++K;
                                    }
                                    if (K > 8) {
                                        break;
                                    }
                                    Ship = nullptr;
                                }
                            }
                        }
                    } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range_3.next(J); ) {
                            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, J);
                            if (aPlayer::GetPlayer() == Ship) {
                                Ship = nullptr;
                            } else if (aPlayer::GetPlayer()->DockedTo != Ship->DockedTo) {
                                Ship = nullptr;
                            } else {
                                K = 0;
                                while (K <= 8) {
                                    if (ShipSlots[K].AnimationState != 0 && ShipSlots[K].ShipId == Ship->Id) {
                                        break;
                                    }
                                    ++K;
                                }
                                if (K > 8) {
                                    break;
                                }
                                Ship = nullptr;
                            }
                        }
                    }
                }
                if (Ship == nullptr) {
                    continue;
                }
                Path = pas::WideString();
                LargeHull = false;
                Path = Ship->GetShipPortraitImagePath();
                if (Path == u"") {
                    continue;
                }
                ShipSlots[SlotOrder[I]].ShipId = Ship->Id;
                ShipSlots[SlotOrder[I]].AnimationState = 1;
                ShipSlots[SlotOrder[I]].Opacity = 0;
                LoadDockedShipImage(SlotOrder[I], Path, LargeHull, TfHangar::GetShipPortraitScale(Ship));
                // Native indexes the opacity by I here, before mapping through SlotOrder.
                SetDockedShipOpacity(SlotOrder[I], ShipSlots[I].Opacity);
            }
        }
    }

    void TfHangar::MainMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        WindowsSdk::TPoint CursorPoint{};
        if (aGalaxy::Galaxy == nullptr || aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->IsHullDestroyed()) {
            return;
        }
        CursorPoint = GetCursorPoint();
        for (I = 0; I <= 8; ++I) {
            Ship = static_cast<aShip::TShip*>(aGalaxy::Galaxy->IdToShip(ShipSlots[I].ShipId, false));
            if (Ship != nullptr && ShipSlots[I].ImageControl->HitTestPixel(CursorPoint)) {
                ShowShipInfo(Ship);
                if (GlobalsV::DynamicTipsPos) {
                    GI_GraphBuf::TGraphBufGI* cpp_with = ShipSlots[I].ImageControl;
                    ShipInfoWindow->SetPosition(ClassesImports::Point(cpp_with->HitTestBounds.Left + cpp_with->ClientSize.X / 2 - ShipInfoWindow->ClientSize.X / 2, cpp_with->HitTestBounds.Top + cpp_with->ClientSize.Y));
                } else {
                    ShipInfoWindow->SetPosition(ClassesImports::Point(10, 10));
                }
                return;
            }
        }
        HoveredShip = nullptr;
        if (ShipInfoHideTimer != nullptr) {
            CancelCallbackTimer(ShipInfoHideTimer);
            ShipInfoHideTimer = nullptr;
        }
        ShipInfoHideTimer = ScheduleCallbackTimer(30, 30, pas::bind_method<&TfHangar::HideShipInfo>(this), 0);
    }

    void TfHangar::MainRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        WindowsSdk::TPoint CursorPoint{};
        CursorPoint = GetCursorPoint();
        for (I = 0; I <= 8; ++I) {
            Ship = static_cast<aShip::TShip*>(aGalaxy::Galaxy->IdToShip(ShipSlots[I].ShipId, false));
            if (Ship != nullptr && ShipSlots[I].ImageControl->HitTestPixel(CursorPoint) && Ship->TypeId != aGalaxyStruct::stKling && static_cast<std::uint8_t>(Ship->NoScan ^ 1)) {
                if (aPlayer::GetPlayer() == Ship) {
                    SelectedShip = aPlayer::GetPlayer();
                    ShipClicked(nullptr);
                } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr && static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == aPlayer::GetPlayer()) {
                    if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                        aGalaxy::Galaxy->CheckIntegrityChecksum(227);
                        pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->ConvertToStoredArtefact();
                        aGalaxy::Galaxy->PrimeIntegrityChecksum(228);
                        RefreshDockedShips();
                    } else {
                        SelectedShip = Ship;
                        ShipClicked(nullptr);
                    }
                } else {
                    GR_Main::SoundManager->PlaySound(u"Sound.Scan"_wref.get());
                    SetCursorActive(false);
                    Present();
                    GR_Main::CaptureScreenBackground(true, 0);
                    SetCursorActive(true);
                    Globals::ScannerTarget = Ship;
                    GlobalsV::ScannerReturnScreenId = GlobalsV::FormToId(this);
                    GlobalsV::RequestedScreenId = GlobalsV::screenScanner;
                    RequestClose(1);
                }
                GI_Main::BreakUiMessage();
                break;
            }
        }
    }

    void TfHangar::HideShipInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (ShipInfoHideTimer != nullptr) {
            CancelCallbackTimer(ShipInfoHideTimer);
            ShipInfoHideTimer = nullptr;
        }
        ShipInfoWindow->SetActive(false);
        HoveredShip = nullptr;
    }

    void TfHangar::ShowShipInfo(aShip::TShip* Ship) {
        pas::WideString Text{};
        pas::WideString Path{};
        pas::WideString ColorTag{};
        std::int32_t CapWidth{};
        std::int32_t MinimumWidth{};
        if (ShipInfoHideTimer != nullptr) {
            CancelCallbackTimer(ShipInfoHideTimer);
            ShipInfoHideTimer = nullptr;
        }
        if (HoveredShip == Ship) {
            return;
        }
        ShipInfoWindow->SetActive(true);
        Ship->ScriptItemsAct(aConst::satOnShowingShipInfo, nullptr, nullptr, 0);
        if (aPlayer::GetPlayer() != Ship) {
            {
                const pas::WideString& wrapTextInColor = ([&] {
                    pas::WideString fullName = Ship->GetFullName(u" "_wref.get());
                    pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                    return aMyFunction::WrapTextInColor(std::move(fullName), std::move(infoNameColorTag));
                }());
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()));
                cpp_arg->SetText(wrapTextInColor);
            }
            if (Ship != nullptr && aPlayer::GetPlayer() == Ship->PartnerShip) {
                const pas::WideString& cpp_arg_2 = pas::concat_wide({pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()))->GetText(), u"\r\n", aMyFunction::WrapTextInColor(GR_Main::LookupLocalizedTextByKey(u"FormInfo.Partner"_wref.get()), u"<color=255,240,100>"_w)});
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()));
                cpp_arg_3->SetText(cpp_arg_2);
            }
            if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr && static_cast<aKling::TKling*>(Ship)->ActiveProgramAppliedTurn > 0 && pas::in_range(static_cast<aKling::TKling*>(Ship)->ActiveProgramId, 6, 11)) {
                const pas::WideString& cpp_arg_4 = pas::concat_wide({pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()))->GetText(), u"\r\n", aMyFunction::WrapTextInColor(aConst::LocalizedText(pas::concat_wide({u"Programms.", aConst::ProgramNames[pas::checked_cast<aKling::TKling*>(Ship)->ActiveProgramId], u".AddToShipInfo"})), u"<color=255,0,0>"_w)});
                GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()));
                cpp_arg_5->SetText(cpp_arg_4);
            }
            if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr && static_cast<std::uint32_t>(static_cast<aRanger::TRanger*>(Ship)->PrisonTermRemaining) > 0) {
                const pas::WideString& cpp_arg_6 = pas::concat_wide({pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()))->GetText(), u"\r\n", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormHangar.Prison"_wref.get()), u"<color=255,0,0>"_w)});
                GI_Label::TLabelGI* cpp_arg_7 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()));
                cpp_arg_7->SetText(cpp_arg_6);
            } else if (pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr && static_cast<aPirate::TPirate*>(Ship)->PrisonTermRemaining > 0) {
                const pas::WideString& cpp_arg_8 = pas::concat_wide({pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()))->GetText(), u"\r\n", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormHangar.Prison"_wref.get()), u"<color=255,0,0>"_w)});
                GI_Label::TLabelGI* cpp_arg_9 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()));
                cpp_arg_9->SetText(cpp_arg_8);
            }
        } else {
            const pas::WideString& wrapTextInColor_2 = ([&] {
                pas::WideString fullName_2 = Ship->GetFullName(u" "_wref.get());
                pas::WideString infoNameColorTag_2 = aMyFunction::InfoNameColorTag;
                return aMyFunction::WrapTextInColor(std::move(fullName_2), std::move(infoNameColorTag_2));
            }());
            GI_Label::TLabelGI* cpp_arg_10 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()));
            cpp_arg_10->SetText(wrapTextInColor_2);
        }
        if (Ship->GetFactionNameKey() != u"None") {
            GI_Image::TImageGI* InfoShipEmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipEmRace"_wref.get()));
            InfoShipEmRace->SetImagePath(aConst::GetFactionEmblemPath(Ship->GetFactionNameKey()));
            InfoShipEmRace->SetImageKindX(GI_Main::ikxRight);
            InfoShipEmRace->SetImageKindY(GI_Main::ikyBottom);
            InfoShipEmRace->SetActive(true);
        } else {
            GetByName(u"InfoShipEmRace"_wref.get())->SetActive(false);
        }
        if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Ship->Graphic) != nullptr) {
            GI_GraphBuf::TGraphBufGI* InfoShipImage2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoShipImage2"_wref.get()));
            Path = Ship->GetShipPortraitImagePath();
            InfoShipImage2->SetActive(Path != u"");
            if (InfoShipImage2->Active) {
                InfoShipImage2->SourceHasPerPixelAlpha = true;
                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(Path, 1, u","_wref.get()), InfoShipImage2->GraphBuf);
                if (InfoShipImage2->ClientSize.X < InfoShipImage2->GraphBuf->Width || InfoShipImage2->ClientSize.Y < InfoShipImage2->GraphBuf->Height) {
                    if (static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)) {
                        InfoShipImage2->GraphBuf->RescaleRgba(InfoShipImage2->ClientSize.X, System::Round(pas::real_divide(InfoShipImage2->ClientSize.X, static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)), 5);
                    } else {
                        InfoShipImage2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoShipImage2->ClientSize.Y, static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width)), InfoShipImage2->ClientSize.Y, 5);
                    }
                }
                InfoShipImage2->SetImageKindX(GI_Main::ikxCenter);
                InfoShipImage2->SetImageKindY(GI_Main::ikyCenter);
                {
                    Types::TPoint visualCenter = InfoShipImage2->GetVisualCenter();
                    Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                    InfoShipImage2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                }
            }
        } else {
            GI_GraphBuf::TGraphBufGI* InfoShipImage2_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoShipImage2"_wref.get()));
            InfoShipImage2_2->SetActive(true);
            InfoShipImage2_2->SourceHasPerPixelAlpha = true;
            if (aKling::TerronShip == Ship && aGalaxy::Galaxy->TerronToStarTurn >= 0x40000000) {
                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(reinterpret_cast<SE_Star::TStarSE*>(aKling::TerronShip->CurrentStar->Graphic)->StaticImagePath, 1, u","_wref.get()), InfoShipImage2_2->GraphBuf);
            } else {
                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ruins::TRuinsSE*>(Ship->Graphic)->StaticImagePath, 1, u","_wref.get()), InfoShipImage2_2->GraphBuf);
            }
            if (InfoShipImage2_2->ClientSize.X < InfoShipImage2_2->GraphBuf->Width || InfoShipImage2_2->ClientSize.Y < InfoShipImage2_2->GraphBuf->Height) {
                if (static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)) {
                    InfoShipImage2_2->GraphBuf->RescaleRgba(InfoShipImage2_2->ClientSize.X, System::Round(pas::real_divide(InfoShipImage2_2->ClientSize.X, static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)), 5);
                } else {
                    InfoShipImage2_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoShipImage2_2->ClientSize.Y, static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width)), InfoShipImage2_2->ClientSize.Y, 5);
                }
            }
            {
                Types::TPoint visualCenter_2 = InfoShipImage2_2->GetVisualCenter();
                Types::TPoint itemImageCenter_2 = Globals::ShipScreen->ItemImageCenter;
                InfoShipImage2_2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_2, visualCenter_2));
            }
        }
        if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) {
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"_wref.get()))->SetActive(false);
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"_wref.get()))->SetActive(false);
        } else {
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"_wref.get()))->SetActive(true);
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"_wref.get()))->SetActive(true);
            if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
                const pas::WideString& characterName = pas::checked_cast<aRanger::TRanger*>(Ship)->GetCharacterName();
                GI_Label::TLabelGI* cpp_arg_11 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"_wref.get()));
                cpp_arg_11->SetText(characterName);
            } else {
                const pas::WideString& localizedTypeName = Ship->GetLocalizedTypeName();
                GI_Label::TLabelGI* cpp_arg_12 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"_wref.get()));
                cpp_arg_12->SetText(localizedTypeName);
            }
        }
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Ship->CalculateSpeed());
            GI_Label::TLabelGI* cpp_arg_13 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSpeed"_wref.get()));
            cpp_arg_13->SetText(intToStr);
        }
        {
            const pas::WideString& wrapTextInColor_3 = aMyFunction::WrapTextInColor(u"???"_w, pas::WideString());
            GI_Label::TLabelGI* cpp_arg_14 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDamage"_wref.get()));
            cpp_arg_14->SetText(wrapTextInColor_3);
        }
        if (Ship->GetHull()->HullPoints <= pas::real_divide(Ship->GetHull()->Weight, 2.0L)) {
            ColorTag = u"<color=255,166,0>"_w;
        } else {
            ColorTag = pas::WideString();
        }
        if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship) || aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == Ship->PartnerShip || Ship->TypeId == aGalaxyStruct::stTranclucator) {
            Text = pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(Ship->GetHull()->HullPoints), ColorTag), u"/", pas::wide_int_to_str(Ship->GetHull()->Weight)});
            if (aPlayer::GetPlayer()->HasScannerArtefact(Ship)) {
                {
                    const pas::WideString& weaponDamageSummary = Ship->GetWeaponDamageSummary();
                    GI_Label::TLabelGI* cpp_arg_15 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDamage"_wref.get()));
                    cpp_arg_15->SetText(weaponDamageSummary);
                }
                Text = pas::concat_wide({Text, u" + ", aMyFunction::WrapTextInColor(Ship->GetRepairPointsSummary(), pas::WideString())});
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"_wref.get()))->SetText(Text);
        } else {
            const pas::WideString& wrapTextInColor_4 = aMyFunction::WrapTextInColor(u"???"_w, ColorTag);
            GI_Label::TLabelGI* cpp_arg_16 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"_wref.get()));
            cpp_arg_16->SetText(wrapTextInColor_4);
        }
        Text = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Ship->GetDefensePercent() & 0x0000007f), "%"}));
        if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship) || aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == Ship->PartnerShip || Ship->TypeId == aGalaxyStruct::stTranclucator) {
            Text = pas::concat_wide({Text, u" + ", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Ship->GetArmor()), pas::WideString())});
            if (aPlayer::GetPlayer()->HasScannerArtefact(Ship)) {
                Text = pas::concat_wide({Ship->GetManeuverabilitySummary(), Text});
            }
        }
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDef"_wref.get()))->SetText(Text);
        {
            const pas::WideString& relationLevelTextToShip = Ship->GetRelationLevelTextToShip(aPlayer::GetPlayer());
            GI_Label::TLabelGI* cpp_arg_17 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipRel"_wref.get()));
            cpp_arg_17->SetText(relationLevelTextToShip);
        }
        if (aPlayer::GetPlayer() != Ship && !(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0 && aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship)) {
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"_wref.get()))->SetActive(true);
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"_wref.get()))->SetActive(true);
            {
                const pas::WideString& cpp_arg_18 = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(aPlayer::GetPlayer()->GetWinChancePercent(Ship) & 0x0000007f), "%"}));
                GI_Label::TLabelGI* cpp_arg_19 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"_wref.get()));
                cpp_arg_19->SetText(cpp_arg_18);
            }
        } else {
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"_wref.get()))->SetActive(false);
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"_wref.get()))->SetActive(false);
        }
        pas::Extended cpp_right = pas::real_max<float>(0.1f, Ship->GetHull()->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})));
        std::int32_t BarWidth = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(Ship->GetHull()->Weight, aConst::HullBaseSize), cpp_right)) * 64.0L);
        BarWidth = std::min<std::int32_t>(192, std::max<std::int32_t>(32, BarWidth));
        {
            GI_Image::TImageGI* InfoShipDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableLeft"_wref.get()));
            CapWidth = InfoShipDurableLeft->GetContentSize().X;
            MinimumWidth = 2 * CapWidth + BarWidth + InfoShipDurableLeft->LocalPosition.X + InfoShipDurableLeft->Parent->LocalPosition.X + 2 * InfoShipDurableLeft->Parent->Parent->LocalPosition.X;
        }
        {
            GI_Image::TImageGI* InfoShipDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurable"_wref.get()));
            if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship) || aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == Ship->PartnerShip || Ship->TypeId == aGalaxyStruct::stTranclucator) {
                std::int64_t cpp_left = System::Round(pas::real_divide(Ship->GetHull()->HullPoints, Ship->GetHull()->Weight) * BarWidth);
                std::int32_t cpp_arg_20 = cpp_left - (InfoShipDurable->GetContentSize().X - 5);
                std::int32_t y = InfoShipDurable->LocalPosition.Y;
                InfoShipDurable->SetPosition(ClassesImports::Point(cpp_arg_20, y));
            } else {
                MinimumWidth = MinimumWidth - BarWidth + 64;
                BarWidth = 64;
                {
                    std::int32_t cpp_arg_21 = BarWidth - (InfoShipDurable->GetContentSize().X - 5);
                    std::int32_t y_2 = InfoShipDurable->LocalPosition.Y;
                    InfoShipDurable->SetPosition(ClassesImports::Point(cpp_arg_21, y_2));
                }
            }
            InfoShipDurable->Parent->Parent->SetActive(true);
            InfoShipDurable->Parent->Parent->SetSize(ClassesImports::Point(2 * CapWidth + BarWidth, InfoShipDurable->Parent->Parent->ClientSize.Y));
            InfoShipDurable->Parent->SetSize(ClassesImports::Point(BarWidth + 2, InfoShipDurable->Parent->Parent->ClientSize.Y));
        }
        {
            GI_Image::TImageGI* InfoShipDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableRight"_wref.get()));
            {
                std::int32_t cpp_arg_22 = BarWidth + CapWidth - InfoShipDurableRight->GetContentSize().X;
                std::int32_t y_3 = InfoShipDurableRight->LocalPosition.Y;
                InfoShipDurableRight->SetPosition(ClassesImports::Point(cpp_arg_22, y_3));
            }
            InfoShipDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoShipDurableRight->Parent->LocalPosition.Y));
            InfoShipDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoShipDurableRight->Parent->ClientSize.Y));
        }
        {
            GI_Image::TImageGI* InfoShipDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableBack"_wref.get()));
            {
                std::int32_t cpp_arg_23 = BarWidth + 1 - InfoShipDurableBack->GetContentSize().X;
                std::int32_t y_4 = InfoShipDurableBack->LocalPosition.Y;
                InfoShipDurableBack->SetPosition(ClassesImports::Point(cpp_arg_23, y_4));
            }
            InfoShipDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoShipDurableBack->Parent->ClientSize.Y));
        }
        GI_Label::TLabelGI* DamageCaption = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISDamage"_wref.get()));
        GI_Label::TLabelGI* DamageText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDamage"_wref.get()));
        if (aPlayer::GetPlayer()->HasScannerArtefact(Ship)) {
            DamageCaption->SetActive(true);
            DamageText->SetActive(true);
        } else {
            DamageCaption->SetActive(false);
            DamageText->SetActive(false);
            DamageCaption = nullptr;
            DamageText = nullptr;
        }
        {
            GI_Label::TLabelGI* cpp_arg_24 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_25 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_26 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISSpeed"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_27 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSpeed"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_28 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISSize"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_29 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_30 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISDef"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_31 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDef"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_32 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISRel"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_33 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipRel"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_34 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_35 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"_wref.get()));
            GI_MessageLoop::TObjectGI* byName = GetByName(u"InfoShipEmRace"_wref.get());
            GI_Label::TLabelGI* cpp_arg_36 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"_wref.get()));
            Globals::ShipScreen->LayoutObjectInfo(ShipInfoWindow, cpp_arg_36, cpp_arg_24, cpp_arg_25, cpp_arg_26, cpp_arg_27, cpp_arg_28, cpp_arg_29, cpp_arg_30, cpp_arg_31, DamageCaption, DamageText, cpp_arg_32, cpp_arg_33, cpp_arg_34, cpp_arg_35, nullptr, nullptr, byName, true, MinimumWidth);
        }
        HoveredShip = Ship;
    }

    void TfHangar::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(LoadPanel->IsAnimatingShutters() ^ 1) && static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10000);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20000);
        }
    }

    void TfHangar::p_destroy() {
        fHangar::TfHangar_Destroy(this);
    }

} // namespace fHangar
