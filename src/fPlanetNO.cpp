#include "layout/fPlanetNO.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_Music.hpp"
#include "types/SE_Planet.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "types/aTranclucator.hpp"
#include "types/fPlanetQuest.hpp"
#include "types/fSaveManager.hpp"
#include "types/fShip2.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Sound.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fPlanetNO.hpp"

namespace fPlanetNO {
    const float ProbeTrajectoryHitRadiusSquared = 4.0E+2f;

    void TfPlanetNO_Create(TfPlanetNO* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfPlanetNO_Destroy(TfPlanetNO* Self) {
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfPlanetNO::InitializeLayout() {
        std::int32_t I{};
        std::uint8_t LargeBackground{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fPlanetNO... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* PlanetBG = MainPanel->FindByNameRecursive(u"PlanetBG"_wref.get());
                LargeBackground = PlanetBG->ClientSize.X > 1024 || PlanetBG->ClientSize.Y > 768;
                if (LargeBackground) {
                    PlanetBG->SetPosition(ClassesImports::Point(0, 0));
                    PlanetBG->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                } else {
                    PlanetBG->SetPosition(ClassesImports::Point(PlanetBG->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PlanetBG->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
                }
            }
            {
                GI_Image::TImageGI* CockpitImage = pas::checked_cast<GI_Image::TImageGI*>(MainPanel->FindByNameRecursive(u"CockpitImage"_wref.get()));
                CockpitImage->SetPosition(ClassesImports::Point(CockpitImage->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, CockpitImage->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
                CockpitImage->SetImagePath(CockpitImage->GetImagePath());
                CockpitImage->SetActive(static_cast<std::uint8_t>(LargeBackground ^ 1));
            }
            {
                GI_MessageLoop::TObjectGI* PanelResearch = MainPanel->FindByNameRecursive(u"PanelResearch"_wref.get());
                PanelResearch->SetPosition(ClassesImports::Point(PanelResearch->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelResearch->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* ButResearch_Parent = MainPanel->FindByNameRecursive(u"ButResearch"_wref.get())->Parent;
                ButResearch_Parent->SetPosition(ClassesImports::Point(ButResearch_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth, ButResearch_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* PanelInfo = MainPanel->FindByNameRecursive(u"PanelInfo"_wref.get());
                PanelInfo->SetPosition(ClassesImports::Point(PanelInfo->LocalPosition.X + GR_Main::ExtraScreenWidth, PanelInfo->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* QuestInfo = MainPanel->FindByNameRecursive(u"QuestInfo"_wref.get());
                QuestInfo->SetPosition(ClassesImports::Point(QuestInfo->LocalPosition.X + GR_Main::ExtraScreenWidth, QuestInfo->LocalPosition.Y + GR_Main::ExtraScreenWidth));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        {
            GI_MessageLoop::TObjectGI* MainPanel_2 = GetByName(u"MainPanel"_wref.get());
            MainPanel_2->MouseMoveCallback = pas::bind_method<&TfPlanetNO::MainPanelMouseMove>(this);
            MainPanel_2->RightButtonDownCallback = pas::bind_method<&TfPlanetNO::MainPanelRightButtonDown>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButTakeoff"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::TakeoffClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::EndTurnClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::ShipClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Gal"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::GalaxyClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Quest"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::QuestClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButResearch"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::ToggleResearchPanel>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::ToggleResearchPanel>(this);
        {
            GI_MessageLoop::TObjectGI* PanelPath = GetByName(u"PanelPath"_wref.get());
            PanelPath->MouseMoveCallback = pas::bind_method<&TfPlanetNO::ResearchMapMouseMove>(this);
            PanelPath->MouseLeaveCallback = pas::bind_method<&TfPlanetNO::ResearchMapMouseLeave>(this);
            PanelPath->LeftButtonDownCallback = pas::bind_method<&TfPlanetNO::ResearchMapMouseDown>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButLeft"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::ScrollSatellitePageLeft>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRight"_wref.get()))->UpCallback = pas::bind_method<&TfPlanetNO::ScrollSatellitePageRight>(this);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 5); cpp_range.next(I); ) {
            SatelliteInventorySlots[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot_", SysUtils::IntToStr(I), "i"}))));
            SatelliteInventorySlots[I]->UserValue = I;
            SatelliteInventorySlots[I]->LeftButtonDownCallback = pas::bind_method<&TfPlanetNO::SatelliteInventoryMouseDown>(this);
        }
        ItemInfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"PII"_wref.get()));
        ItemInfoImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"_wref.get()));
        ItemInfoNameLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"_wref.get()));
        ItemInfoTextLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"_wref.get()));
        ItemInfoSizeLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"_wref.get()));
        ItemInfoCostLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"_wref.get()));
        ItemInfoRaceIcon = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"_wref.get()));
    }

    void TfPlanetNO::OnOpen() {
        SelectMusic();
        MainPanel->OnOpen();
        fPanelLoad::TfPanelLoad_OnOpen(LoadPanel);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfPlanetNO::MainPanelKeyDown>(this);
        {
            GI_Image::TImageGI* PlanetBG = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PlanetBG"_wref.get()));
            PlanetBG->SetImagePath(pas::concat_wide({u"GI,Bm.PlanetBG.", aPlayer::GetPlayer()->CurrentPlanet->Graphic->BackgroundGraph}));
        }
        SelectedTrajectoryIndex = -1;
        MainPanel->RebuildMessageButtons(false);
        RefreshPlanetInfo();
        RefreshTextQuestPrompt();
        CloseResearchPanel();
        aScript::DispatchPendingScriptRequests();
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(111);
    }

    void TfPlanetNO::OnClose() {
        if (GlobalsV::RequestedScreenId != GlobalsV::screenLoad) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(112);
        }
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        CloseResearchPanel();
        MainPanel->OnClose();
        LoadPanel->OnClose();
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->RefreshStorageBubbles();
        }
        MainPanel->RefreshMoneyAndCargo();
        MainPanel->RebuildMessageButtons(false);
    }

    void TfPlanetNO::TakeoffClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        if (aCalc::TurnCalculationPhase == ThreadCalc::tcpGalaxyRunning || aCalc::TurnCalculationPhase == ThreadCalc::tcpPlayerStarRunning || aScript::HasPendingScriptRequests()) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(129);
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(129);
        if (HeldSatellite != nullptr && HeldSatelliteOrigin == 0 && aPlayer::GetPlayer()->GetCargoFreeSpace() < HeldSatellite->Weight) {
            GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"FormRuins.ShipOvercharging"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
            return;
        }
        if (!aPlayer::GetPlayer()->HasPositiveSpeed()) {
            if (aPlayer::GetPlayer()->GetCargoFreeSpace() < 0) {
                GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"FormRuins.ShipOvercharging"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
            } else if (aPlayer::GetPlayer()->GetEngine() == nullptr) {
                GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"FormRuins.NotEngine"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
            } else if (aPlayer::GetPlayer()->GetFuelTanks() == nullptr) {
                GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"FormRuins.NotFuelTank"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
            }
            return;
        }
        if (aPlayer::GetPlayer()->HasSatelliteOnPlanet(aPlayer::GetPlayer()->CurrentPlanet) && GI_MessageBox::ShowMessageBoxGI(this, ([&] {
            auto name = pas::borrow(aPlayer::GetPlayer()->CurrentPlanet->Name);
            pas::WideString paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormPlanetNO.SatelliteInPlanet"_wref.get());
            return aMyFunction::FormatText1(std::move(paramByPathOrMarker), u"<color=255,240,100>"_w, u"<Name>"_w, name.get());
        }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(113);
        CloseResearchPanel();
        ReturnHeldSatellite();
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        Globals::PruneExpiredPersistentPlayerMessages();
        aPlayer::GetPlayer()->OrderTakeoff();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range.next(I); ) {
            aScript::TScript_RunTurnCode(pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, I));
        }
        Globals::StarMapScreen->SetMapCenterManually(EC_Struct::TruncatePointF(aPlayer::GetPlayer()->Position));
        aGalaxy::PlayerStar->RefreshSpaceObjectPositions();
        aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 1);
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(3)) {
            aGalaxy::Galaxy->EnableDominatorSurfaces();
        } else {
            aGalaxy::Galaxy->DisableDominatorSurfaces();
        }
        aCalc::CalculatePlayerStarTurnAndWait();
        if (!GR_Main::ExitScreenLoop) {
            if (aPlayer::GetPlayer() == nullptr) {
                GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
                RequestClose(1);
            } else {
                aCalc::CalculateGalaxyTurnAndWait();
                Globals::StarMapWeaponPanelOpen = false;
                Globals::StarMapScreen->ResumeMode = fStarMap::smrTurnFilm;
                Globals::ScreenLoadMode = 2;
                GlobalsV::PostLoadScreenId = GlobalsV::screenStarMap;
                GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
                LoadPanel->SelectBackgroundStyle(0);
                LoadPanel->RefreshBackgroundImages();
                LoadPanel->StartClosingShutters();
            }
        }
    }

    void TfPlanetNO::StartTextQuest(GI_MessageLoop::TObjectGI* Sender) {
        if (LoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (Sender->UserValue != 0 && GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"FormGov.QuestCertificate.NotCertificateAttention"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(114);
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        Globals::StandaloneQuestMode = false;
        GlobalsV::QuestReturnScreenId = GlobalsV::FormToId(this);
        GlobalsV::RequestedScreenId = GlobalsV::screenPlanetQuest;
        RequestClose(1);
    }

    void TfPlanetNO::RefreshPlanetInfo() {
        GI_Window::TWindowGI* Window = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"PanelInfo"_wref.get()));
        {
            GI_Label::TLabelGI* PanelInfo_Name = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PanelInfo_Name"_wref.get()));
            PanelInfo_Name->SetText(([&] {
                auto infoNameColorTag = pas::borrow(aMyFunction::InfoNameColorTag);
                pas::WideString localizedText = aConst::LocalizedText(u"Planet.Civil.Info.TextNamePlanet"_wref.get());
                pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                return aMyFunction::ReplaceColoredToken(std::move(localizedText), u"<Planet>"_w, std::move(name), infoNameColorTag.get());
            }()));
        }
        {
            GI_Label::TLabelGI* PanelInfo_Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PanelInfo_Text"_wref.get()));
            PanelInfo_Text->SetText(aPlayer::GetPlayer()->CurrentPlanet->GetInfoText(false));
            Window->SetSize(ClassesImports::Point(PanelInfo_Text->ClientSize.X + Window->WorkSubRect.Left + Window->WorkSubRect.Right, PanelInfo_Text->ClientSize.Y + Window->WorkSubRect.Top + Window->WorkSubRect.Bottom));
            Window->UpdateAutoGeometry();
            Window->SetActive(true);
            PanelInfo_Text->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&Window->WorkSubRect, 0)));
        }
        {
            GI_GraphBuf::TGraphBufGI* PanelInfo_Image = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"PanelInfo_Image"_wref.get()));
            PanelInfo_Image->SourceHasPerPixelAlpha = true;
            aPlayer::GetPlayer()->CurrentPlanet->Graphic->RenderToBuffer(this, PanelInfo_Image->GraphBuf, false);
            if (static_cast<std::uint32_t>(PanelInfo_Image->GraphBuf->Width) >= static_cast<std::uint32_t>(PanelInfo_Image->GraphBuf->Height)) {
                PanelInfo_Image->GraphBuf->RescaleRgba(PanelInfo_Image->ClientSize.X, System::Round(pas::real_divide(PanelInfo_Image->ClientSize.X, static_cast<std::uint32_t>(PanelInfo_Image->GraphBuf->Width)) * static_cast<std::uint32_t>(PanelInfo_Image->GraphBuf->Height)), 5);
            } else {
                PanelInfo_Image->GraphBuf->RescaleRgba(System::Round(pas::real_divide(PanelInfo_Image->ClientSize.Y, static_cast<std::uint32_t>(PanelInfo_Image->GraphBuf->Height)) * static_cast<std::uint32_t>(PanelInfo_Image->GraphBuf->Width)), PanelInfo_Image->ClientSize.Y, 5);
            }
            PanelInfo_Image->SetImageKindX(GI_Main::ikxCenter);
            PanelInfo_Image->SetImageKindY(GI_Main::ikyCenter);
        }
        {
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PanelInfo_Text"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PanelInfo_Name"_wref.get()));
            fShip2::TfShip2::LayoutItemInfo(Window, cpp_arg_2, cpp_arg, true, true, 0);
        }
        {
            GI_Label::TLabelGI* PanelInfo_Name_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PanelInfo_Name"_wref.get()));
            PanelInfo_Name_2->SetSize(ClassesImports::Point(Window->ClientSize.X - PanelInfo_Name_2->LocalPosition.X - Window->WorkSubRect.Right, PanelInfo_Name_2->ClientSize.Y));
        }
        Window->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - 10 - Window->ClientSize.X, 10));
    }

    void TfPlanetNO::RefreshTextQuestPrompt() {
        std::int32_t QuestId{};
        std::int32_t I{};
        aRanger::PQuest Quest{};
        pas::WideString Text{};
        QuestId = -1;
        GI_Window::TWindowGI* Window = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"QuestInfo"_wref.get()));
        Quest = nullptr;
        Window->SetActive(false);
        if (aPlayer::GetPlayer()->CurrentPlanet->TextQuestId > -1 && pas::list_count(aPlayer::GetPlayer()->Quests) > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Quests) - 1); cpp_range.next(I); ) {
                Quest = pas::list_at<aRanger::TQuest>(aPlayer::GetPlayer()->Quests, I);
                if (Quest->QuestType == aGalaxyStruct::qtPlanetQuest && pas::class_cast_if<aPlanet::TPlanet*>(Quest->ObjectiveTarget) != nullptr && aPlayer::GetPlayer()->CurrentPlanet == pas::checked_cast<aPlanet::TPlanet*>(Quest->ObjectiveTarget) && ([&] {
                    const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(Quest->QuestNumber));
                    EC_BlockPar::TBlockParEC* blockByPath = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.PlanetQuest"_wref.get());
                    return blockByPath->CountParams(intToStr);
                }()) > 0) {
                    QuestId = Quest->QuestNumber;
                    if (Quest->QuestNumber < 10000 || GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"_wref.get())->CountBlocks(u"PlanetQuestLic"_wref.get()) > 0 && ([&] {
                        pas::WideString cpp_string = ([&] {
                            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(Quest->QuestNumber));
                            EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"_wref.get())->GetBlock(u"PlanetQuestLic"_wref.get());
                            return block->GetParamOrMarker(intToStr_2);
                        }());
                        pas::WideString cpp_string_2 = fPlanetQuest::TfPlanetQuest::GetQuestContentHash(Quest->QuestNumber);
                        return cpp_string == cpp_string_2;
                    }())) {
                        Window->FindByNameRecursive(u"QuestInfo_Run"_wref.get())->UserValue = 0;
                    } else {
                        Window->FindByNameRecursive(u"QuestInfo_Run"_wref.get())->UserValue = 1;
                    }
                    Window->SetActive(true);
                    break;
                }
            }
        }
        if (Window->Active) {
            {
                GI_Label::TLabelGI* QuestInfo_Name = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"QuestInfo_Name"_wref.get()));
                QuestInfo_Name->SetText(aConst::LocalizedText(u"PlanetQuest.StartText.QuestCaption"_wref.get()));
            }
            {
                GI_Label::TLabelGI* QuestInfo_Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"QuestInfo_Text"_wref.get()));
                Text = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.StartText.", SysUtils::IntToStr(QuestId)})));
                if (Text == u"") {
                    Text = aConst::LocalizedColorText(u"PlanetQuest.StartText.QuestExtern"_wref.get());
                }
                if (Quest != nullptr) {
                    aMyFunction::ReplaceTextToken(Text, u"<CurPlanet>"_w, pas::checked_cast<aPlanet::TPlanet*>(Quest->ObjectiveTarget)->Name, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Text, u"<CurStar>"_w, pas::checked_cast<aPlanet::TPlanet*>(Quest->ObjectiveTarget)->CurrentStar->Name, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, Quest->Planet->Name, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Text, u"<FromStar>"_w, Quest->Planet->CurrentStar->Name, u"<color=255,240,100>"_w);
                }
                QuestInfo_Text->SetText(Text);
                Window->SetSize(ClassesImports::Point(QuestInfo_Text->ClientSize.X + Window->WorkSubRect.Left + Window->WorkSubRect.Right, QuestInfo_Text->ClientSize.Y + Window->WorkSubRect.Top + Window->WorkSubRect.Bottom));
                Window->UpdateAutoGeometry();
                Window->SetActive(true);
                QuestInfo_Text->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&Window->WorkSubRect, 0)));
            }
            {
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"QuestInfo_Text"_wref.get()));
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"QuestInfo_Name"_wref.get()));
                fShip2::TfShip2::LayoutItemInfo(Window, cpp_arg_2, cpp_arg, true, true, 0);
            }
            {
                GI_GraphButton::TGraphButtonGI* QuestInfo_Run = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"QuestInfo_Run"_wref.get()));
                QuestInfo_Run->UpCallback = pas::bind_method<&TfPlanetNO::StartTextQuest>(this);
                Window->SetSize(ClassesImports::Point(Window->ClientSize.X, GR_Main::GiScalePixels(5) + (QuestInfo_Run->ClientSize.Y + Window->ClientSize.Y)));
                Window->UpdateAutoGeometry();
                QuestInfo_Run->SetPosition(ClassesImports::Point(Window->ClientSize.X / 2 - QuestInfo_Run->ClientSize.X / 2, Window->ClientSize.Y - GR_Main::GiScalePixels(10) - QuestInfo_Run->ClientSize.Y));
            }
            {
                GI_Label::TLabelGI* QuestInfo_Name_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"QuestInfo_Name"_wref.get()));
                QuestInfo_Name_2->SetSize(ClassesImports::Point(Window->ClientSize.X - QuestInfo_Name_2->LocalPosition.X - Window->WorkSubRect.Right, QuestInfo_Name_2->ClientSize.Y));
            }
            Window->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - 10 - Window->ClientSize.X, GR_Main::GameScreenHeight - GR_Main::GiScalePixels(90) - Window->ClientSize.Y));
        }
    }

    void TfPlanetNO::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (LoadPanel->IsAnimatingShutters()) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(115);
        if (ResearchPanelVisible) {
            ReturnHeldSatellite();
            HideItemInfoPopup(nullptr, 0);
        }
        MainPanel->EndTurnClicked(Sender);
        RefreshPlanetInfo();
        RefreshTextQuestPrompt();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(116);
        MainPanel->RebuildMessageButtons(false);
        if (ResearchPanelVisible) {
            RefreshResearchPanel();
            if (NewSurfaceLootDiscovered) {
                GR_Main::SoundManager->PlaySound(u"Sound.ProbeExplore"_wref.get());
            }
        }
    }

    void TfPlanetNO::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ResearchPanelVisible) {
            ReturnHeldSatellite();
            HideItemInfoPopup(nullptr, 0);
        }
        MainPanel->ShipClicked(Sender);
        if (ExitCode == 0 && ResearchPanelVisible) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(777);
            aShip::TShip_AssignSatelliteIndicesFromHoldOrder(aPlayer::GetPlayer());
            aGalaxy::Galaxy->PrimeIntegrityChecksum(774);
            RefreshResearchPanel();
        }
    }

    void TfPlanetNO::GalaxyClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ResearchPanelVisible) {
            ReturnHeldSatellite();
            HideItemInfoPopup(nullptr, 0);
        }
        MainPanel->GalaxyClicked(Sender);
        if (ResearchPanelVisible) {
            RefreshResearchPanel();
        }
    }

    void TfPlanetNO::QuestClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ResearchPanelVisible) {
            ReturnHeldSatellite();
            HideItemInfoPopup(nullptr, 0);
        }
        MainPanel->QuestClicked(Sender);
        if (ResearchPanelVisible) {
            RefreshResearchPanel();
        }
    }

    void TfPlanetNO::ToggleResearchPanel(GI_MessageLoop::TObjectGI* Sender) {
        if (GetByName(u"PanelResearch"_wref.get())->Active) {
            CloseResearchPanel();
        } else {
            OpenResearchPanel();
        }
    }

    void TfPlanetNO::OpenResearchPanel() {
        std::int32_t I{};
        std::int32_t Parts{};
        SatelliteInventoryPageStart = 0;
        SatellitePanelNeedsLayout = true;
        aGalaxy::Galaxy->CheckIntegrityChecksum(117);
        aPlayer::GetPlayer()->RepairDuplicateSatelliteTrajectoryIndices();
        aPlayer::GetPlayer()->CompactSatelliteTrajectoryIndices();
        aShip::TShip_AssignSatelliteIndicesFromHoldOrder(aPlayer::GetPlayer());
        aGalaxy::Galaxy->PrimeIntegrityChecksum(118);
        ResearchPanelVisible = true;
        for (I = 0; I <= 5; ++I) {
            if (TrajectoryPointCounts[I] == 0) {
                BuildTrajectory(I);
            }
        }
        GetByName(u"PanelResearch"_wref.get())->SetActive(true);
        RefreshResearchPanel();
        HoveredItem = nullptr;
        HoveringSurfaceLoot = false;
        {
            GI_GraphBuf::TGraphBufGI* PlanetImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"PlanetImage"_wref.get()));
            Parts = EC_Str::CountDelimitedPartsW(aPlayer::GetPlayer()->CurrentPlanet->Graphic->ImagePath, u"."_wref.get());
            PlanetImage->LoadBitmapPathAsRgb(pas::concat_wide({u"Bm.PUMaps.", EC_Str::ExtractDelimitedPartW(aPlayer::GetPlayer()->CurrentPlanet->Graphic->ImagePath, Parts - 1, u"."_wref.get()), u"?RGB"}));
            if (GR_Main::GiResourceVariant() == 1) {
                std::int32_t round = System::Round(pas::real_divide(static_cast<std::uint32_t>(PlanetImage->GraphBuf->Height) * 800, 1024.0L));
                std::int32_t round_2 = System::Round(pas::real_divide(static_cast<std::uint32_t>(PlanetImage->GraphBuf->Width) * 800, 1024.0L));
                PlanetImage->GraphBuf->RescaleRgb(round_2, round);
            }
            PlanetImage->GraphBuf->ConvertRgbTo565();
        }
        if (SatelliteMovementTimer != nullptr) {
            CancelCallbackTimer(SatelliteMovementTimer);
            SatelliteMovementTimer = nullptr;
        }
        SatelliteMovementTimer = ScheduleCallbackTimer(30, 30, pas::bind_method<&TfPlanetNO::AdvanceSatelliteMarkers>(this), 0);
        ProbeSignalCount = 0;
        if (ProbeSignalTimer != nullptr) {
            CancelCallbackTimer(ProbeSignalTimer);
            ProbeSignalTimer = nullptr;
        }
        ProbeSignalTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfPlanetNO::UpdateProbeSignalSound>(this), 0);
        {
            GI_GAI::TgaiGI* Scan = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"Scan"_wref.get()));
            Scan->SetSize(Scan->GetContentSize());
            Scan->SequenceIndex = 0;
            Scan->UpdateAutoGeometry();
        }
    }

    // Returns a held probe to its origin list before destroying research controls.
    void TfPlanetNO::CloseResearchPanel() {
        if (ProbeSignalSound != nullptr) {
            pas::free(ProbeSignalSound);
            ProbeSignalSound = nullptr;
        }
        if (SatelliteMovementTimer != nullptr) {
            CancelCallbackTimer(SatelliteMovementTimer);
            SatelliteMovementTimer = nullptr;
        }
        if (ProbeSignalTimer != nullptr) {
            CancelCallbackTimer(ProbeSignalTimer);
            ProbeSignalTimer = nullptr;
        }
        ReturnHeldSatellite();
        GetByName(u"PanelResearch"_wref.get())->SetActive(false);
        ResearchMapMouseLeave(nullptr);
        GetByName(u"PanelSatellite"_wref.get())->FreeOwnedChildren();
        GetByName(u"PanelItems"_wref.get())->FreeOwnedChildren();
        HoveredItem = nullptr;
        HoveringSurfaceLoot = false;
        HideItemInfoPopup(nullptr, 0);
        ResearchPanelVisible = false;
    }

    // Requires index 0..5 and an empty point count. Builds at most 256 points from connected image markers.
    void TfPlanetNO::BuildTrajectory(std::int32_t TrajectoryIndex) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t NeighborX{};
        std::int32_t NeighborY{};
        std::int32_t SumX{};
        std::int32_t SumY{};
        System::PCardinal NeighborPixel{};
        std::uint8_t* NeighborVisited{};
        PProbeMarkerPixel ReadNode{};
        PProbeMarkerPixel WriteNode{};
        std::int32_t Count{};
        std::int32_t ReadCount{};
        float InverseLength{};
        TProbeTrajectoryPoint Swap{};
        GR_GraphBuf::TGraphBufGR* Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(TrajectoryIndex + 1)}), Buffer);
        std::uint8_t* Visited = static_cast<std::uint8_t*>(EC_Mem::AllocClearEC(Buffer->Width * Buffer->Height));
        PProbeMarkerPixel Queue = static_cast<PProbeMarkerPixel>(EC_Mem::AllocClearEC(256 * static_cast<std::int32_t>(sizeof(TProbeMarkerPixel))));
        std::int32_t OffsetX = GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(TrajectoryIndex)})))->LocalPosition.X;
        std::int32_t OffsetY = GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(TrajectoryIndex)})))->LocalPosition.Y;
        std::uint8_t* CursorVisited = Visited;
        System::PCardinal Pixel = static_cast<System::PCardinal>(Buffer->GetPixels());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Buffer->Height - 1); cpp_range.next(Y); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Buffer->Width - 1); cpp_range_2.next(X); ) {
                if (pas::load_unaligned<std::uint32_t>(Pixel) >> 24 > 32 && *CursorVisited == 0) {
                    if (TrajectoryPointCounts[TrajectoryIndex] >= 256) {
                        GR_Main::RaiseWideMessage(u"BuildPath.1"_wref.get());
                    }
                    SumX = X;
                    SumY = Y;
                    ReadNode = Queue;
                    ReadNode->X = X;
                    ReadNode->Y = Y;
                    ReadNode->Pixel = Pixel;
                    ReadNode->Visited = CursorVisited;
                    *CursorVisited = 1;
                    ReadCount = 0;
                    Count = 1;
                    WriteNode = reinterpret_cast<PProbeMarkerPixel>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ReadNode)) + static_cast<std::int32_t>(sizeof(TProbeMarkerPixel)))));
                    pas::store_unaligned<std::uint32_t>(Pixel, 0xffffffffu);
                    while (ReadCount < Count) {
                        for (I = 0; I <= 3; ++I) {
                            NeighborX = ReadNode->X;
                            NeighborY = ReadNode->Y;
                            NeighborVisited = ReadNode->Visited;
                            NeighborPixel = ReadNode->Pixel;
                            switch (I) {
                                case 0: {
                                    ++NeighborX;
                                    if (Buffer->Width <= NeighborX) {
                                        continue;
                                    }
                                    NeighborVisited = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(NeighborVisited)) + 1)));
                                    NeighborPixel = reinterpret_cast<System::PCardinal>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(NeighborPixel)) + 4)));
                                    break;
                                }
                                case 1: {
                                    --NeighborX;
                                    if (NeighborX < 0) {
                                        continue;
                                    }
                                    NeighborVisited = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(NeighborVisited)) - 1)));
                                    NeighborPixel = reinterpret_cast<System::PCardinal>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(NeighborPixel)) - 4)));
                                    break;
                                }
                                case 2: {
                                    ++NeighborY;
                                    if (Buffer->Height <= NeighborY) {
                                        continue;
                                    }
                                    NeighborVisited = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(NeighborVisited)) + static_cast<std::uint32_t>(Buffer->Width))));
                                    NeighborPixel = reinterpret_cast<System::PCardinal>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(NeighborPixel)) + static_cast<std::uint32_t>(Buffer->PitchBytes))));
                                    break;
                                }
                                case 3: {
                                    --NeighborY;
                                    if (NeighborY < 0) {
                                        continue;
                                    }
                                    NeighborVisited = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(NeighborVisited)) - static_cast<std::uint32_t>(Buffer->Width))));
                                    NeighborPixel = reinterpret_cast<System::PCardinal>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(NeighborPixel)) - static_cast<std::uint32_t>(Buffer->PitchBytes))));
                                    break;
                                }
                            }
                            if (pas::load_unaligned<std::uint32_t>(NeighborPixel) >> 24 > 32 && *NeighborVisited == 0) {
                                if (Count >= 256) {
                                    GR_Main::RaiseWideMessage(u"BuildPath.2"_wref.get());
                                }
                                SumX += NeighborX;
                                SumY += NeighborY;
                                WriteNode->X = NeighborX;
                                WriteNode->Y = NeighborY;
                                WriteNode->Visited = NeighborVisited;
                                WriteNode->Pixel = NeighborPixel;
                                WriteNode = reinterpret_cast<PProbeMarkerPixel>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(WriteNode)) + static_cast<std::int32_t>(sizeof(TProbeMarkerPixel)))));
                                *NeighborVisited = 1;
                                ++Count;
                                pas::store_unaligned<std::uint32_t>(NeighborPixel, 0xff800000u);
                            }
                        }
                        ReadNode = reinterpret_cast<PProbeMarkerPixel>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ReadNode)) + static_cast<std::int32_t>(sizeof(TProbeMarkerPixel)))));
                        ++ReadCount;
                    }
                    Trajectories[TrajectoryIndex][TrajectoryPointCounts[TrajectoryIndex]].Position.X = pas::real_divide(SumX, Count) + OffsetX;
                    Trajectories[TrajectoryIndex][TrajectoryPointCounts[TrajectoryIndex]].Position.Y = pas::real_divide(SumY, Count) + OffsetY;
                    ++TrajectoryPointCounts[TrajectoryIndex];
                }
                Pixel = reinterpret_cast<System::PCardinal>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Pixel)) + 4)));
                CursorVisited = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(CursorVisited)) + 1)));
            }
            Pixel = reinterpret_cast<System::PCardinal>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Pixel)) + static_cast<std::uint32_t>(Buffer->PitchBytes - 4 * Buffer->Width))));
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, TrajectoryPointCounts[TrajectoryIndex] - 2); cpp_range_3.next(I); ) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(I + 1, TrajectoryPointCounts[TrajectoryIndex] - 1); cpp_range_4.next(J); ) {
                if (Trajectories[TrajectoryIndex][J].Position.X < Trajectories[TrajectoryIndex][I].Position.X) {
                    Swap = Trajectories[TrajectoryIndex][J];
                    Trajectories[TrajectoryIndex][J] = Trajectories[TrajectoryIndex][I];
                    Trajectories[TrajectoryIndex][I] = Swap;
                }
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, TrajectoryPointCounts[TrajectoryIndex] - 2); cpp_range_5.next(I); ) {
            Trajectories[TrajectoryIndex][I].Direction.X = static_cast<long double>(Trajectories[TrajectoryIndex][I + 1].Position.X) - Trajectories[TrajectoryIndex][I].Position.X;
            Trajectories[TrajectoryIndex][I].Direction.Y = static_cast<long double>(Trajectories[TrajectoryIndex][I + 1].Position.Y) - Trajectories[TrajectoryIndex][I].Position.Y;
            InverseLength = pas::real_divide(1.0L, System::Sqrt(pas::sqr(static_cast<pas::Extended>(Trajectories[TrajectoryIndex][I].Direction.X)) + pas::sqr(static_cast<pas::Extended>(Trajectories[TrajectoryIndex][I].Direction.Y))));
            Trajectories[TrajectoryIndex][I].Direction.X = static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.X) * InverseLength;
            Trajectories[TrajectoryIndex][I].Direction.Y = static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.Y) * InverseLength;
        }
        EC_Mem::FreeEC(Queue);
        EC_Mem::FreeEC(Visited);
        pas::free(Buffer);
    }

    // Requires a nonempty trajectory.
    WindowsSdk::TPoint TfPlanetNO::GetRandomTrajectoryPoint(std::int32_t TrajectoryIndex) {
        WindowsSdk::TPoint Result{};
        std::int32_t I = aMyFunction::RandomIntRange(0, TrajectoryPointCounts[TrajectoryIndex] - 1);
        Result.X = System::Round(Trajectories[TrajectoryIndex][I].Position.X);
        Result.Y = System::Round(Trajectories[TrajectoryIndex][I].Position.Y);
        return Result;
    }

    // Requires a trajectory spanning a positive X range. Result is untouched if no segment matches.
    WindowsSdk::TPoint TfPlanetNO::ProjectPointOntoTrajectory(std::int32_t TrajectoryIndex, WindowsSdk::TPoint Point) {
        WindowsSdk::TPoint Result{};
        std::int32_t I{};
        float Distance{};
        float DeltaX{};
        float DeltaY{};
        float X = Point.X;
        while (X >= Trajectories[TrajectoryIndex][TrajectoryPointCounts[TrajectoryIndex] - 1].Position.X) {
            X = X - (static_cast<long double>(Trajectories[TrajectoryIndex][TrajectoryPointCounts[TrajectoryIndex] - 1].Position.X) - Trajectories[TrajectoryIndex][0].Position.X);
        }
        if (X < Trajectories[TrajectoryIndex][0].Position.X) {
            X = Trajectories[TrajectoryIndex][0].Position.X;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TrajectoryPointCounts[TrajectoryIndex] - 1); cpp_range.next(I); ) {
            if (X >= Trajectories[TrajectoryIndex][I].Position.X && X < Trajectories[TrajectoryIndex][I + 1].Position.X) {
                DeltaX = static_cast<long double>(X) - Trajectories[TrajectoryIndex][I].Position.X;
                DeltaY = static_cast<long double>(Point.Y) - Trajectories[TrajectoryIndex][I].Position.Y;
                Distance = static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.X) * DeltaX + static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.Y) * DeltaY;
                if (Distance < 0.0L) {
                    Distance = 0.0f;
                }
                Result.X = System::Round(Trajectories[TrajectoryIndex][I].Position.X + static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.X) * Distance);
                Result.Y = System::Round(Trajectories[TrajectoryIndex][I].Position.Y + static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.Y) * Distance);
                return Result;
            }
        }
        return Result;
    }

    // Advances four pixels along the projected segment; Result is untouched if no segment matches.
    WindowsSdk::TPoint TfPlanetNO::AdvanceTrajectoryPoint(std::int32_t TrajectoryIndex, WindowsSdk::TPoint Point) {
        WindowsSdk::TPoint Result{};
        std::int32_t I{};
        float Distance{};
        float DeltaX{};
        float DeltaY{};
        float X = Point.X;
        while (X > Trajectories[TrajectoryIndex][TrajectoryPointCounts[TrajectoryIndex] - 1].Position.X) {
            X = X - (static_cast<long double>(Trajectories[TrajectoryIndex][TrajectoryPointCounts[TrajectoryIndex] - 1].Position.X) - Trajectories[TrajectoryIndex][0].Position.X);
        }
        if (X < Trajectories[TrajectoryIndex][0].Position.X) {
            X = Trajectories[TrajectoryIndex][0].Position.X;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TrajectoryPointCounts[TrajectoryIndex] - 1); cpp_range.next(I); ) {
            if (X >= Trajectories[TrajectoryIndex][I].Position.X && X < Trajectories[TrajectoryIndex][I + 1].Position.X) {
                DeltaX = static_cast<long double>(X) - Trajectories[TrajectoryIndex][I].Position.X;
                DeltaY = static_cast<long double>(Point.Y) - Trajectories[TrajectoryIndex][I].Position.Y;
                Distance = static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.X) * DeltaX + static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.Y) * DeltaY;
                if (Distance < 0.0L) {
                    Distance = 0.0f;
                }
                Distance = Distance + 4.0L;
                Result.X = System::Round(Trajectories[TrajectoryIndex][I].Position.X + static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.X) * Distance);
                Result.Y = System::Round(Trajectories[TrajectoryIndex][I].Position.Y + static_cast<long double>(Trajectories[TrajectoryIndex][I].Direction.Y) * Distance);
                Result = ProjectPointOntoTrajectory(TrajectoryIndex, Result);
                return Result;
            }
        }
        return Result;
    }

    // Returns -1 on a miss; searches only the current planet's available probe orbits.
    std::int32_t TfPlanetNO::FindTrajectoryAtCursor() {
        std::int32_t I{};
        {
            const std::int32_t cpp_last = aPlayer::GetPlayer()->CurrentPlanet->ProbeOrbitCount - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (IsCursorOverTrajectory(I)) {
                        return I;
                    }
                }
            }
        }
        return -1;
    }

    std::uint8_t TfPlanetNO::IsCursorOverTrajectory(std::int32_t TrajectoryIndex) {
        std::int32_t I{};
        WindowsSdk::TPoint Point{};
        Point = GetByName(u"PanelPath"_wref.get())->ToLocalPoint(GetCursorPoint());
        float X = Point.X;
        float Y = Point.Y;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TrajectoryPointCounts[TrajectoryIndex] - 2); cpp_range.next(I); ) {
            if (pas::sqr(static_cast<long double>(Trajectories[TrajectoryIndex][I].Position.X) - X) + pas::sqr(static_cast<long double>(Trajectories[TrajectoryIndex][I].Position.Y) - Y) < ProbeTrajectoryHitRadiusSquared) {
                return true;
            }
        }
        return false;
    }

    void TfPlanetNO::ResearchMapMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t I{};
        aPlanet::PPlanetSurfaceLootEntry Entry{};
        WindowsSdk::TPoint Cell{};
        if (aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries != nullptr) {
            Cell = GetByName(u"PanelItems"_wref.get())->ToLocalPoint(Point);
            Cell.X = pas::idiv(Cell.X, GR_Main::GiScalePixelsEx(36, 28));
            Cell.Y = pas::idiv(Cell.Y, GR_Main::GiScalePixelsEx(36, 28));
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<aPlanet::TPlanetSurfaceLootEntry>(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries, I);
                if (Entry->GridX == Cell.X && Entry->GridY == Cell.Y && (Entry->TerrainKind == aPlanet::ptWater && aPlayer::GetPlayer()->CurrentPlanet->WaterExplored >= Entry->SurfaceTileIndex || Entry->TerrainKind == aPlanet::ptLand && aPlayer::GetPlayer()->CurrentPlanet->LandExplored >= Entry->SurfaceTileIndex || Entry->TerrainKind == aPlanet::ptHill && aPlayer::GetPlayer()->CurrentPlanet->HillExplored >= Entry->SurfaceTileIndex) && static_cast<std::uint8_t>(Entry->Unavailable ^ 1)) {
                    UpdateItemInfoPopup(Entry->Item);
                    HoveringSurfaceLoot = true;
                    if (HeldSatellite == nullptr && static_cast<std::uint8_t>(IsCursorImageSelected(u"Take"_wref.get()) ^ 1)) {
                        SetCursorByName(u"Take"_wref.get());
                    }
                    if (SelectedTrajectoryIndex >= 0) {
                        {
                            GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(SelectedTrajectoryIndex)}))));
                            if (TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex) != nullptr && (TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex)->BrokenFlag != 0 || (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSatelliteExplorationTurns(TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex))) == 0)) {
                                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(SelectedTrajectoryIndex + 1), u"B"}));
                            } else {
                                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(SelectedTrajectoryIndex + 1)}));
                            }
                        }
                        SelectedTrajectoryIndex = -1;
                    }
                    return;
                }
            }
        }
        HoveringSurfaceLoot = false;
        if (SelectedTrajectoryIndex < 0) {
            UpdateItemInfoPopup(nullptr);
        }
        if (SelectedTrajectoryIndex < 0 || static_cast<std::uint8_t>(IsCursorOverTrajectory(SelectedTrajectoryIndex) ^ 1)) {
            I = FindTrajectoryAtCursor();
            if (I != SelectedTrajectoryIndex) {
                ResearchMapMouseLeave(Sender);
                SelectedTrajectoryIndex = I;
                if (SelectedTrajectoryIndex >= 0) {
                    GI_Image::TImageGI* cpp_with_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(SelectedTrajectoryIndex)}))));
                    cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(SelectedTrajectoryIndex + 1), u"A"}));
                }
                UpdateItemInfoPopup(TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex));
                UpdateActionCursor(false);
            }
        }
    }

    void TfPlanetNO::ResearchMapMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (SelectedTrajectoryIndex >= 0) {
            {
                GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(SelectedTrajectoryIndex)}))));
                if (TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex) != nullptr && (TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex)->BrokenFlag != 0 || (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSatelliteExplorationTurns(TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex))) == 0)) {
                    cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(SelectedTrajectoryIndex + 1), u"B"}));
                } else {
                    cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(SelectedTrajectoryIndex + 1)}));
                }
            }
            SelectedTrajectoryIndex = -1;
        }
        UpdateItemInfoPopup(nullptr);
        UpdateActionCursor(false);
    }

    void TfPlanetNO::RefreshResearchPanel() {
        std::int32_t I{};
        aItem::TSatellite* InventorySatellite{};
        aItem::TSatellite* Satellite{};
        std::int32_t WaterRate{};
        std::int32_t LandRate{};
        std::int32_t HillRate{};
        GI_MessageLoop::TObjectGI* OldChild{};
        aPlanet::PPlanetSurfaceLootEntry Entry{};
        std::uint8_t Undiscovered{};
        NewSurfaceLootDiscovered = false;
        {
            GI_GAI::TgaiGI* Scan = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"Scan"_wref.get()));
            Scan->SetActive(TfPlanetNO::CountDeployedSatellites() > 0);
            if (Scan->Active) {
                Scan->RestartPlayback();
            }
        }
        {
            const pas::WideString& replaceColoredToken = ([&] {
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormPlanetNO.Caption"_wref.get());
                pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Name>"_w, std::move(name), pas::WideString());
            }());
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Caption"_wref.get()));
            cpp_arg->SetText(replaceColoredToken);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 5); cpp_range.next(I); ) {
            GI_Image::TImageGI* cpp_with_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(I)}))));
            cpp_with_2->SetActive(aPlayer::GetPlayer()->CurrentPlanet->ProbeOrbitCount > I);
            if (cpp_with_2->Active) {
                if (SelectedTrajectoryIndex == I) {
                    cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(I + 1), u"A"}));
                } else if (TfPlanetNO::FindDeployedSatellite(I) != nullptr && (TfPlanetNO::FindDeployedSatellite(I)->BrokenFlag != 0 || (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSatelliteExplorationTurns(TfPlanetNO::FindDeployedSatellite(I))) == 0)) {
                    cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(I + 1), u"B"}));
                } else {
                    cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormUnknown2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(I + 1)}));
                }
            }
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(119);
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelItems"_wref.get()));
        GI_Image::TImageGI* Child = reinterpret_cast<GI_Image::TImageGI*>(Panel->FirstChild);
        while (Child != nullptr) {
            OldChild = Child;
            Child = reinterpret_cast<GI_Image::TImageGI*>(Child->NextSibling);
            Entry = reinterpret_cast<aPlanet::PPlanetSurfaceLootEntry>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(OldChild->UserValue)));
            if (aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries == nullptr || pas::list_indexof(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries, static_cast<void*>(Entry)) < 0) {
                OldChild->Invalidate();
                pas::free(OldChild);
            }
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries != nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries) - 1); cpp_range_2.next(I); ) {
                Entry = pas::list_at<aPlanet::TPlanetSurfaceLootEntry>(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries, I);
                Undiscovered = false;
                if ((Entry->TerrainKind == aPlanet::ptWater && aPlayer::GetPlayer()->CurrentPlanet->WaterExplored >= Entry->SurfaceTileIndex || Entry->TerrainKind == aPlanet::ptLand && aPlayer::GetPlayer()->CurrentPlanet->LandExplored >= Entry->SurfaceTileIndex || Entry->TerrainKind == aPlanet::ptHill && aPlayer::GetPlayer()->CurrentPlanet->HillExplored >= Entry->SurfaceTileIndex) && static_cast<std::uint8_t>(Entry->Unavailable ^ 1) || aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0 && pas::class_cast_if<aItem::TEquipmentWithActCode*>(Entry->Item) != nullptr && reinterpret_cast<aItem::TEquipmentWithActCode*>(Entry->Item)->DisplayAsArtefact) {
                    if (!(Entry->TerrainKind == aPlanet::ptWater && aPlayer::GetPlayer()->CurrentPlanet->WaterExplored >= Entry->SurfaceTileIndex || Entry->TerrainKind == aPlanet::ptLand && aPlayer::GetPlayer()->CurrentPlanet->LandExplored >= Entry->SurfaceTileIndex || Entry->TerrainKind == aPlanet::ptHill && aPlayer::GetPlayer()->CurrentPlanet->HillExplored >= Entry->SurfaceTileIndex)) {
                        Undiscovered = true;
                    }
                    Child = reinterpret_cast<GI_Image::TImageGI*>(Panel->FirstChild);
                    while (Child != nullptr) {
                        if (reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Child->UserValue))) == Entry) {
                            break;
                        }
                        Child = reinterpret_cast<GI_Image::TImageGI*>(Child->NextSibling);
                    }
                    if (Child == nullptr) {
                        GI_Image::TImageGI* cpp_with_3 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                        cpp_with_3->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Entry));
                        if (!Undiscovered) {
                            if (pas::class_cast_if<aItem::TGoods*>(Entry->Item) != nullptr) {
                                cpp_with_3->SetImagePath(pas::concat_wide({u"GI,", Entry->Item->virtual_TItem_GetBitmapResourceName()}));
                            } else {
                                cpp_with_3->SetImagePath(pas::concat_wide({u"GI,", Entry->Item->virtual_TItem_GetBitmapResourceName(), u"s"}));
                            }
                        } else {
                            cpp_with_3->SetImagePath(pas::concat_wide({u"GI,", Entry->Item->virtual_TItem_GetBitmapResourceName(), u"ab"}));
                        }
                        cpp_with_3->SetSize(cpp_with_3->GetContentSize());
                        cpp_with_3->SetOrigin(EC_Struct::HalfPoint(cpp_with_3->ClientSize));
                        cpp_with_3->SetPosition(ClassesImports::Point(Entry->GridX * GR_Main::GiScalePixelsEx(36, 28) + GR_Main::GiScalePixelsEx(36, 28) / 2, Entry->GridY * GR_Main::GiScalePixelsEx(36, 28) + GR_Main::GiScalePixelsEx(36, 28) / 2));
                    } else if (!(pas::class_cast_if<aItem::TGoods*>(Entry->Item) != nullptr)) {
                        if (([&] {
                            pas::WideString cpp_string = Child->GetImagePath();
                            pas::WideString cpp_string_2 = pas::concat_wide({u"GI,", Entry->Item->virtual_TItem_GetBitmapResourceName(), u"s"});
                            return cpp_string != cpp_string_2;
                        }()) && static_cast<std::uint8_t>(Undiscovered ^ 1)) {
                            Child->SetImagePath(pas::concat_wide({u"GI,", Entry->Item->virtual_TItem_GetBitmapResourceName(), u"s"}));
                            Child->SetSize(Child->GetContentSize());
                            Child->SetOrigin(EC_Struct::HalfPoint(Child->ClientSize));
                            Child->SetPosition(ClassesImports::Point(Entry->GridX * GR_Main::GiScalePixelsEx(36, 28) + GR_Main::GiScalePixelsEx(36, 28) / 2, Entry->GridY * GR_Main::GiScalePixelsEx(36, 28) + GR_Main::GiScalePixelsEx(36, 28) / 2));
                            NewSurfaceLootDiscovered = true;
                        }
                    }
                }
            }
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(120);
        Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelSatellite"_wref.get()));
        Child = reinterpret_cast<GI_Image::TImageGI*>(Panel->FirstChild);
        while (Child != nullptr) {
            OldChild = Child;
            Child = reinterpret_cast<GI_Image::TImageGI*>(Child->NextSibling);
            Satellite = reinterpret_cast<aItem::TSatellite*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(OldChild->UserValue)));
            if (pas::list_indexof(aPlayer::GetPlayer()->Satellites, reinterpret_cast<void*>(Satellite)) < 0 || aPlayer::GetPlayer()->CurrentPlanet != Satellite->TargetPlanet) {
                OldChild->Invalidate();
                pas::free(OldChild);
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Satellites) - 1); cpp_range_3.next(I); ) {
            Satellite = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I);
            if (aPlayer::GetPlayer()->CurrentPlanet == Satellite->TargetPlanet) {
                Child = reinterpret_cast<GI_Image::TImageGI*>(Panel->FirstChild);
                while (Child != nullptr) {
                    if (reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Child->UserValue))) == Satellite) {
                        break;
                    }
                    Child = reinterpret_cast<GI_Image::TImageGI*>(Child->NextSibling);
                }
                if (Child == nullptr) {
                    if (!GlobalsV::AnimItem) {
                        GI_Image::TImageGI* cpp_with_4 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                        cpp_with_4->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Satellite));
                        cpp_with_4->SetImagePath(pas::concat_wide({u"GI,", Satellite->virtual_TItem_GetBitmapResourceName(), u"s"}));
                        cpp_with_4->SetSize(cpp_with_4->GetContentSize());
                        cpp_with_4->SetOrigin(EC_Struct::HalfPoint(cpp_with_4->ClientSize));
                        if (SatellitePanelNeedsLayout) {
                            cpp_with_4->SetPosition(GetRandomTrajectoryPoint(Satellite->TrajectoryIndex));
                        } else {
                            cpp_with_4->SetPosition(ProjectPointOntoTrajectory(Satellite->TrajectoryIndex, Panel->ToLocalPoint(GetCursorPoint())));
                        }
                    } else {
                        GI_GAI::TgaiGI* cpp_with_5 = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Panel);
                        cpp_with_5->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Satellite));
                        cpp_with_5->SetImagePath(pas::concat_wide({Satellite->virtual_TItem_GetBitmapResourceName(), u"a"}));
                        cpp_with_5->SequenceIndex = 0;
                        cpp_with_5->UpdateAutoGeometry();
                        cpp_with_5->SetSize(cpp_with_5->GetContentSize());
                        cpp_with_5->SetOrigin(EC_Struct::HalfPoint(cpp_with_5->ClientSize));
                        cpp_with_5->RestartPlayback();
                        if (SatellitePanelNeedsLayout) {
                            cpp_with_5->SetPosition(GetRandomTrajectoryPoint(Satellite->TrajectoryIndex));
                        } else {
                            cpp_with_5->SetPosition(ProjectPointOntoTrajectory(Satellite->TrajectoryIndex, Panel->ToLocalPoint(GetCursorPoint())));
                        }
                    }
                }
            }
        }
        SatellitePanelNeedsLayout = false;
        aGalaxy::Galaxy->CheckIntegrityChecksum(1117);
        aPlayer::GetPlayer()->RepairDuplicateSatelliteTrajectoryIndices();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(1118);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 5); cpp_range_4.next(I); ) {
            InventorySatellite = aPlayer::GetPlayer()->FindSatelliteByTrajectoryIndex(SatelliteInventoryPageStart + I);
            {
                GI_Image::TImageGI* cpp_with_6 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot_", SysUtils::IntToStr(I), "i"}))));
                if (InventorySatellite == nullptr) {
                    cpp_with_6->SetImagePath(pas::WideString());
                } else {
                    cpp_with_6->SetImagePath(pas::concat_wide({u"GI,", InventorySatellite->virtual_TItem_GetBitmapResourceName(), u"s"}));
                    cpp_with_6->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_6->SetImageKindY(GI_Main::ikyCenter);
                }
            }
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButLeft"_wref.get()))->SetDisabled(SatelliteInventoryPageStart <= 0);
        {
            GI_GraphButton::TGraphButtonGI* cpp_arg_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButRight"_wref.get()));
            std::uint8_t cpp_arg_3 = aPlayer::GetPlayer()->GetSatelliteTrajectoryIndexLimit() < SatelliteInventoryPageStart + 6;
            cpp_arg_2->SetDisabled(cpp_arg_3);
        }
        {
            const pas::WideString& replaceColoredToken_2 = ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->CurrentPlanet->WaterTiles);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormPlanetNO.Space"_wref.get());
                return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<val>"_w, std::move(intToStr), u"<color=0,50,200>"_w);
            }());
            GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"WaterSpace"_wref.get()));
            cpp_arg_4->SetText(replaceColoredToken_2);
        }
        {
            const pas::WideString& replaceColoredToken_3 = ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(aPlayer::GetPlayer()->CurrentPlanet->LandTiles);
                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormPlanetNO.Space"_wref.get());
                return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_3), u"<val>"_w, std::move(intToStr_2), u"<color=0,50,200>"_w);
            }());
            GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LandSpace"_wref.get()));
            cpp_arg_5->SetText(replaceColoredToken_3);
        }
        {
            const pas::WideString& replaceColoredToken_4 = ([&] {
                pas::WideString intToStr_3 = pas::wide_int_to_str(aPlayer::GetPlayer()->CurrentPlanet->HillTiles);
                pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormPlanetNO.Space"_wref.get());
                return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_4), u"<val>"_w, std::move(intToStr_3), u"<color=0,50,200>"_w);
            }());
            GI_Label::TLabelGI* cpp_arg_6 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"HillSpace"_wref.get()));
            cpp_arg_6->SetText(replaceColoredToken_4);
        }
        {
            const pas::WideString& replaceColoredToken_5 = ([&] {
                pas::WideString intToStr_4 = pas::wide_int_to_str(aPlayer::GetPlayer()->CurrentPlanet->WaterExplored);
                pas::WideString localizedColorText_5 = aConst::LocalizedColorText(u"FormPlanetNO.Complate"_wref.get());
                return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_5), u"<val>"_w, std::move(intToStr_4), u"<color=0,50,200>"_w);
            }());
            GI_Label::TLabelGI* cpp_arg_7 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"WaterComplate"_wref.get()));
            cpp_arg_7->SetText(replaceColoredToken_5);
        }
        {
            const pas::WideString& replaceColoredToken_6 = ([&] {
                pas::WideString intToStr_5 = pas::wide_int_to_str(aPlayer::GetPlayer()->CurrentPlanet->LandExplored);
                pas::WideString localizedColorText_6 = aConst::LocalizedColorText(u"FormPlanetNO.Complate"_wref.get());
                return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_6), u"<val>"_w, std::move(intToStr_5), u"<color=0,50,200>"_w);
            }());
            GI_Label::TLabelGI* cpp_arg_8 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LandComplate"_wref.get()));
            cpp_arg_8->SetText(replaceColoredToken_6);
        }
        {
            const pas::WideString& replaceColoredToken_7 = ([&] {
                pas::WideString intToStr_6 = pas::wide_int_to_str(aPlayer::GetPlayer()->CurrentPlanet->HillExplored);
                pas::WideString localizedColorText_7 = aConst::LocalizedColorText(u"FormPlanetNO.Complate"_wref.get());
                return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_7), u"<val>"_w, std::move(intToStr_6), u"<color=0,50,200>"_w);
            }());
            GI_Label::TLabelGI* cpp_arg_9 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"HillComplate"_wref.get()));
            cpp_arg_9->SetText(replaceColoredToken_7);
        }
        WaterRate = 0;
        LandRate = 0;
        HillRate = 0;
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Satellites) - 1); cpp_range_5.next(I); ) {
            Satellite = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I);
            if (aPlayer::GetPlayer()->CurrentPlanet == Satellite->TargetPlanet && Satellite->BrokenFlag == 0) {
                WaterRate = std::min<std::int32_t>(aPlayer::GetPlayer()->CurrentPlanet->WaterTiles - aPlayer::GetPlayer()->CurrentPlanet->WaterExplored, WaterRate + Satellite->WaterExplorationRate);
                LandRate = std::min<std::int32_t>(aPlayer::GetPlayer()->CurrentPlanet->LandTiles - aPlayer::GetPlayer()->CurrentPlanet->LandExplored, LandRate + Satellite->LandExplorationRate);
                HillRate = std::min<std::int32_t>(aPlayer::GetPlayer()->CurrentPlanet->HillTiles - aPlayer::GetPlayer()->CurrentPlanet->HillExplored, HillRate + Satellite->HillExplorationRate);
            }
        }
        {
            GI_Label::TLabelGI* WaterTimeLeft = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"WaterTimeLeft"_wref.get()));
            WaterTimeLeft->SetActive(WaterRate > 0);
            if (WaterTimeLeft->Active) {
                WaterRate = std::min<std::int32_t>(999, MathImports::Ceil(pas::real_divide(aPlayer::GetPlayer()->CurrentPlanet->WaterTiles - aPlayer::GetPlayer()->CurrentPlanet->WaterExplored, WaterRate)));
                WaterTimeLeft->SetText(([&] {
                    pas::WideString intToStr_7 = pas::wide_int_to_str(WaterRate);
                    pas::WideString localizedColorText_8 = aConst::LocalizedColorText(u"FormPlanetNO.TimeLeft"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_8), u"<val>"_w, std::move(intToStr_7), u"<color=0,50,200>"_w);
                }()));
            }
        }
        {
            GI_Label::TLabelGI* LandTimeLeft = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LandTimeLeft"_wref.get()));
            LandTimeLeft->SetActive(LandRate > 0);
            if (LandTimeLeft->Active) {
                LandRate = std::min<std::int32_t>(999, MathImports::Ceil(pas::real_divide(aPlayer::GetPlayer()->CurrentPlanet->LandTiles - aPlayer::GetPlayer()->CurrentPlanet->LandExplored, LandRate)));
                LandTimeLeft->SetText(([&] {
                    pas::WideString intToStr_8 = pas::wide_int_to_str(LandRate);
                    pas::WideString localizedColorText_9 = aConst::LocalizedColorText(u"FormPlanetNO.TimeLeft"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_9), u"<val>"_w, std::move(intToStr_8), u"<color=0,50,200>"_w);
                }()));
            }
        }
        {
            GI_Label::TLabelGI* HillTimeLeft = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"HillTimeLeft"_wref.get()));
            HillTimeLeft->SetActive(HillRate > 0);
            if (HillTimeLeft->Active) {
                HillRate = std::min<std::int32_t>(999, MathImports::Ceil(pas::real_divide(aPlayer::GetPlayer()->CurrentPlanet->HillTiles - aPlayer::GetPlayer()->CurrentPlanet->HillExplored, HillRate)));
                HillTimeLeft->SetText(([&] {
                    pas::WideString intToStr_9 = pas::wide_int_to_str(HillRate);
                    pas::WideString localizedColorText_10 = aConst::LocalizedColorText(u"FormPlanetNO.TimeLeft"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_10), u"<val>"_w, std::move(intToStr_9), u"<color=0,50,200>"_w);
                }()));
            }
        }
        GetByName(u"Light1"_wref.get())->SetActive(aPlayer::GetPlayer()->CurrentPlanet->WaterExplored >= aPlayer::GetPlayer()->CurrentPlanet->WaterTiles);
        GetByName(u"Light2"_wref.get())->SetActive(aPlayer::GetPlayer()->CurrentPlanet->LandExplored >= aPlayer::GetPlayer()->CurrentPlanet->LandTiles);
        GetByName(u"Light3"_wref.get())->SetActive(aPlayer::GetPlayer()->CurrentPlanet->HillExplored >= aPlayer::GetPlayer()->CurrentPlanet->HillTiles);
    }

    void TfPlanetNO::ScrollSatellitePageLeft(GI_MessageLoop::TObjectGI* Sender) {
        if (SatelliteInventoryPageStart > 0) {
            --SatelliteInventoryPageStart;
            RefreshResearchPanel();
            GR_Main::PostMouseMoveMessage();
        }
    }

    void TfPlanetNO::ScrollSatellitePageRight(GI_MessageLoop::TObjectGI* Sender) {
        if (SatelliteInventoryPageStart + 6 <= aPlayer::GetPlayer()->GetSatelliteTrajectoryIndexLimit()) {
            ++SatelliteInventoryPageStart;
            RefreshResearchPanel();
            GR_Main::PostMouseMoveMessage();
        }
    }

    void TfPlanetNO::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            ScrollSatellitePageLeft(nullptr);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            ScrollSatellitePageRight(nullptr);
        }
    }

    void TfPlanetNO::MainPanelMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::uint8_t HideInfo{};
        std::uint8_t ForceHand{};
        if (ResearchPanelVisible && SelectedTrajectoryIndex < 0 && static_cast<std::uint8_t>(HoveringSurfaceLoot ^ 1)) {
            HideInfo = true;
            ForceHand = false;
            for (I = 0; I <= 5; ++I) {
                if (SatelliteInventorySlots[I]->ContainsPoint(Point)) {
                    Item = aPlayer::GetPlayer()->FindSatelliteByTrajectoryIndex(SatelliteInventoryPageStart + I);
                    if (Item != nullptr) {
                        HideInfo = false;
                        ForceHand = true;
                        UpdateItemInfoPopup(Item);
                    }
                    break;
                }
            }
            if (HideInfo) {
                UpdateItemInfoPopup(nullptr);
            }
            UpdateActionCursor(ForceHand);
        }
    }

    void TfPlanetNO::SatelliteInventoryMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        aGalaxy::Galaxy->CheckIntegrityChecksum(121);
        aItem::TSatellite* Item = aPlayer::GetPlayer()->FindSatelliteByTrajectoryIndex(SatelliteInventoryPageStart + Sender->UserValue);
        if (HeldSatellite == nullptr && Item != nullptr) {
            HeldSatellite = Item;
            HeldSatelliteOrigin = 0;
            pas::list_delete(aPlayer::GetPlayer()->Inventory, pas::list_indexof(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item)));
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            UpdateActionCursor(false);
            RefreshResearchPanel();
            GR_Main::SoundManager->PlaySound(u"Sound.SlotGet"_wref.get());
            GR_Main::PostMouseMoveMessage();
        } else if (HeldSatellite != nullptr) {
            if (Item != nullptr) {
                aPlayer::GetPlayer()->InsertSatelliteTrajectoryIndex(SatelliteInventoryPageStart + Sender->UserValue);
            }
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(HeldSatellite));
            pas::checked_cast<aItem::TSatellite*>(static_cast<pas::Object*>(HeldSatellite))->TrajectoryIndex = SatelliteInventoryPageStart + Sender->UserValue;
            pas::checked_cast<aItem::TSatellite*>(static_cast<pas::Object*>(HeldSatellite))->TargetPlanet = nullptr;
            HeldSatellite = nullptr;
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            aPlayer::GetPlayer()->RemoveEmptySatelliteTrajectoryIndex(SatelliteInventoryPageStart + Sender->UserValue + 1);
            aShip::TShip_ArrangeHoldSatellitesByTrajectoryIndex(aPlayer::GetPlayer());
            UpdateActionCursor(false);
            RefreshResearchPanel();
            GR_Main::PostMouseMoveMessage();
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(122);
        aPlayer::GetPlayer()->RefreshStorageBubbles();
        MainPanel->RefreshMoneyAndCargo();
        MainPanel->RebuildMessageButtons(false);
    }

    // Collects accessible surface loot or exchanges the held probe with the selected orbit.
    void TfPlanetNO::ResearchMapMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t I{};
        aPlanet::PPlanetSurfaceLootEntry Entry{};
        WindowsSdk::TPoint Cell{};
        aItem::TSatellite* Item{};
        if (aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<aPlanet::TPlanetSurfaceLootEntry>(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries, I);
                if (Entry->TerrainKind == aPlanet::ptWater && aPlayer::GetPlayer()->CurrentPlanet->WaterExplored >= Entry->SurfaceTileIndex || Entry->TerrainKind == aPlanet::ptLand && aPlayer::GetPlayer()->CurrentPlanet->LandExplored >= Entry->SurfaceTileIndex || Entry->TerrainKind == aPlanet::ptHill && aPlayer::GetPlayer()->CurrentPlanet->HillExplored >= Entry->SurfaceTileIndex) {
                    if (!Entry->Unavailable) {
                        Cell = GetByName(u"PanelItems"_wref.get())->ToLocalPoint(Point);
                        Cell.X = pas::idiv(Cell.X, GR_Main::GiScalePixelsEx(36, 28));
                        Cell.Y = pas::idiv(Cell.Y, GR_Main::GiScalePixelsEx(36, 28));
                        if (Entry->GridX == Cell.X && Entry->GridY == Cell.Y) {
                            if (aPlayer::GetPlayer()->GetCargoFreeSpace() < Entry->Item->Weight) {
                                MainPanel->FlashCargoWarning();
                                return;
                            }
                            GR_Main::SoundManager->PlaySound(u"Sound.PlanetGet"_wref.get());
                            aGalaxy::Galaxy->CheckIntegrityChecksum(123);
                            if (pas::class_cast_if<aItem::TArtefactTranclucator*>(Entry->Item) != nullptr) {
                                pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(pas::checked_cast<aItem::TArtefactTranclucator*>(Entry->Item)->Ship))->OwnerShip = aPlayer::GetPlayer();
                            }
                            if (pas::class_cast_if<aItem::TGoods*>(Entry->Item) != nullptr) {
                                aPlayer::GetPlayer()->CargoGoods[Entry->Item->ItemType].Count += pas::checked_cast<aItem::TGoods*>(Entry->Item)->Quantity;
                                pas::free(Entry->Item);
                            } else if (pas::class_cast_if<aItem::TArtefact*>(Entry->Item) != nullptr) {
                                pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Entry->Item));
                            } else {
                                pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Entry->Item));
                            }
                            aPlayer::GetPlayer()->RefreshDerivedStats(true);
                            pas::list_delete(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries, I);
                            if (pas::list_count(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries) <= 0) {
                                pas::free(aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries);
                                aPlayer::GetPlayer()->CurrentPlanet->SurfaceLootEntries = nullptr;
                            }
                            Entry->Item = nullptr;
                            pas::dispose(Entry);
                            aGalaxy::Galaxy->PrimeIntegrityChecksum(124);
                            UpdateActionCursor(false);
                            RefreshResearchPanel();
                            GR_Main::PostMouseMoveMessage();
                            break;
                        }
                    }
                }
            }
        }
        if (SelectedTrajectoryIndex >= 0 && (HeldSatellite != nullptr || TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex) != nullptr)) {
            HeldSatelliteOrigin = 1;
            aGalaxy::Galaxy->CheckIntegrityChecksum(125);
            Item = TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex);
            if (Item != nullptr) {
                pas::list_delete(aPlayer::GetPlayer()->Satellites, pas::list_indexof(aPlayer::GetPlayer()->Satellites, reinterpret_cast<void*>(Item)));
            }
            if (HeldSatellite != nullptr) {
                GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
                pas::list_add(aPlayer::GetPlayer()->Satellites, reinterpret_cast<void*>(HeldSatellite));
                if (Item != nullptr) {
                    Item->TrajectoryIndex = pas::checked_cast<aItem::TSatellite*>(static_cast<pas::Object*>(HeldSatellite))->TrajectoryIndex;
                    HeldSatelliteOrigin = 0;
                }
                pas::checked_cast<aItem::TSatellite*>(static_cast<pas::Object*>(HeldSatellite))->TrajectoryIndex = SelectedTrajectoryIndex;
                pas::checked_cast<aItem::TSatellite*>(static_cast<pas::Object*>(HeldSatellite))->TargetPlanet = aPlayer::GetPlayer()->CurrentPlanet;
                HeldSatellite = nullptr;
            } else if (Item != nullptr) {
                GR_Main::SoundManager->PlaySound(u"Sound.SlotGet"_wref.get());
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(126);
            if (Item != nullptr) {
                HeldSatellite = Item;
            }
            SelectedTrajectoryIndex = -1;
            UpdateActionCursor(false);
            RefreshResearchPanel();
            GR_Main::PostMouseMoveMessage();
            aPlayer::GetPlayer()->RefreshStorageBubbles();
            MainPanel->RefreshMoneyAndCargo();
            MainPanel->RebuildMessageButtons(false);
        }
    }

    void TfPlanetNO::MainPanelRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (ResearchPanelVisible) {
            ReturnHeldSatellite();
        }
    }

    void TfPlanetNO::UpdateActionCursor(std::uint8_t ForceHand) {
        if (HeldSatellite != nullptr) {
            const pas::WideString& cpp_arg = pas::concat_wide({u"GI,", HeldSatellite->virtual_TItem_GetBitmapResourceName(), u"s"});
            GI_MessageLoop::TMessageLoopGI* self = this;
            self->SetCursorImage(cpp_arg, ClassesImports::Point(16, 16));
        } else if (SelectedTrajectoryIndex >= 0 && TfPlanetNO::FindDeployedSatellite(SelectedTrajectoryIndex) != nullptr) {
            if (!IsCursorImageSelected(u"Take"_wref.get())) {
                GR_Main::SoundManager->PlaySound(u"Sound.ProbeEnter"_wref.get());
                SetCursorByName(u"Take"_wref.get());
            }
        } else if (ForceHand || HoveredItem != nullptr) {
            if (!IsCursorImageSelected(u"Take"_wref.get())) {
                SetCursorByName(u"Take"_wref.get());
            }
        } else if (!IsCursorImageSelected(u"Main"_wref.get())) {
            SetCursorByName(u"Main"_wref.get());
        }
        GetByName(u"Glow"_wref.get())->SetActive(HeldSatellite != nullptr);
    }

    void TfPlanetNO::ReturnHeldSatellite() {
        if (HeldSatellite == nullptr) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(127);
        if (HeldSatelliteOrigin == 0) {
            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(HeldSatellite));
            HeldSatellite = nullptr;
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            UpdateActionCursor(false);
            RefreshResearchPanel();
            GR_Main::PostMouseMoveMessage();
        } else {
            pas::list_add(aPlayer::GetPlayer()->Satellites, reinterpret_cast<void*>(HeldSatellite));
            HeldSatellite = nullptr;
            SatellitePanelNeedsLayout = true;
            SelectedTrajectoryIndex = -1;
            UpdateActionCursor(false);
            RefreshResearchPanel();
            GR_Main::PostMouseMoveMessage();
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(128);
    }

    // Borrowed probe on the current planet, or nil; -1 always returns nil.
    aItem::TSatellite* TfPlanetNO::FindDeployedSatellite(std::int32_t TrajectoryIndex) {
        aItem::TSatellite* Result{};
        std::int32_t I{};
        if (TrajectoryIndex == -1) {
            return nullptr;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Satellites) - 1); cpp_range.next(I); ) {
            Result = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I);
            if (aPlayer::GetPlayer()->CurrentPlanet == Result->TargetPlanet && Result->TrajectoryIndex == TrajectoryIndex) {
                return Result;
            }
        }
        return nullptr;
    }

    std::int32_t TfPlanetNO::CountDeployedSatellites() {
        std::int32_t I{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Satellites) - 1); cpp_range.next(I); ) {
            if (pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I)->TargetPlanet == aPlayer::GetPlayer()->CurrentPlanet) {
                ++Result;
            }
        }
        return Result;
    }

    void TfPlanetNO::AdvanceSatelliteMarkers(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        aItem::TSatellite* Satellite{};
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelSatellite"_wref.get()));
        GI_MessageLoop::TObjectGI* Control = Panel->FirstChild;
        while (Control != nullptr) {
            Satellite = reinterpret_cast<aItem::TSatellite*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Control->UserValue)));
            if (pas::list_indexof(aPlayer::GetPlayer()->Satellites, reinterpret_cast<void*>(Satellite)) >= 0) {
                Control->SetPosition(AdvanceTrajectoryPoint(Satellite->TrajectoryIndex, Control->LocalPosition));
            }
            Control = Control->NextSibling;
        }
    }

    void TfPlanetNO::UpdateProbeSignalSound(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Count = TfPlanetNO::CountDeployedSatellites();
        if (ProbeSignalCount != Count && ProbeSignalSound != nullptr) {
            ProbeSignalSound->SetVolume(pas::real_max<pas::Extended>(0.0L, ProbeSignalSound->Volume - 0.02L));
            if (ProbeSignalSound->Volume <= 0.0L) {
                pas::free(ProbeSignalSound);
                ProbeSignalSound = nullptr;
            }
        }
        if (ProbeSignalSound == nullptr) {
            ProbeSignalCount = Count;
        }
        if (ProbeSignalSound == nullptr && ProbeSignalCount > 0) {
            ProbeSignalSound = pas::construct_call<GR_Sound::TSoundBufferControl>(GR_Sound::TSoundBufferControl_Create);
            ProbeSignalSound->Configure(static_cast<pas::WideString>(pas::concat_ansi({"Sound.ProbeSignal", SysUtils::IntToStr(ProbeSignalCount)})), 0, true);
            ProbeSignalSound->SetVolume(0.01f);
        }
        if (ProbeSignalCount > 0 && ProbeSignalCount == Count && ProbeSignalSound != nullptr && ProbeSignalSound->Volume < 1.0L) {
            ProbeSignalSound->SetVolume(pas::real_min<pas::Extended>(1.0L, ProbeSignalSound->Volume + 0.02L));
        }
    }

    // The explicit script receiver value preserves native argument evaluation order.
    // Borrows Item; nil schedules a delayed hide.
    void TfPlanetNO::UpdateItemInfoPopup(aItem::TItem* Item) {
        static const pas::Set<0, 255> DurableTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        aItem::TEquipment* Equipment{};
        std::int32_t BarWidth{};
        std::int32_t CapWidth{};
        std::int32_t MinimumWidth{};
        if (Item != HoveredItem) {
            HoveredItem = Item;
            if (Item == nullptr) {
                if (ItemInfoHideTimer != nullptr) {
                    CancelCallbackTimer(ItemInfoHideTimer);
                    ItemInfoHideTimer = nullptr;
                }
                ItemInfoHideTimer = ScheduleCallbackTimer(300, 99999, pas::bind_method<&TfPlanetNO::HideItemInfoPopup>(this), 0);
            } else {
                if (ItemInfoHideTimer != nullptr) {
                    CancelCallbackTimer(ItemInfoHideTimer);
                    ItemInfoHideTimer = nullptr;
                }
                if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr) {
                    ShowGoodsInfoPopup(pas::checked_cast<aItem::TGoods*>(Item));
                } else {
                    if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
                        if (Item->ScriptItem != nullptr) {
                            reinterpret_cast<aScript::TScriptItem*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item->ScriptItem)) + 0)))->RunActionCode(aConst::satOnShowingItemInfo, nullptr, aPlayer::GetPlayer()->CurrentPlanet, nullptr, 0);
                        }
                        if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
                            aScript::RunItemConfigActionCode(Item, aConst::satOnShowingItemInfo, nullptr, aPlayer::GetPlayer()->CurrentPlanet, nullptr, 0);
                        }
                    }
                    Equipment = pas::checked_cast<aItem::TEquipment*>(Item);
                    ItemInfoWindow->SetActive(true);
                    {
                        GI_Image::TImageGI* cpp_with = ItemInfoImage;
                        cpp_with->SetImagePath(pas::concat_wide({u"GI,", Equipment->virtual_TItem_GetBitmapResourceName(), u"s"}));
                        cpp_with->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with->SetImageKindY(GI_Main::ikyCenter);
                        {
                            Types::TPoint visualCenter = cpp_with->GetVisualCenter();
                            Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                            cpp_with->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                        }
                    }
                    {
                        const pas::WideString& wrapTextInColor = ([&] {
                            pas::WideString displayName = Equipment->GetDisplayName();
                            pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(std::move(displayName), std::move(infoNameColorTag));
                        }());
                        GI_Label::TLabelGI* itemInfoNameLabel = ItemInfoNameLabel;
                        itemInfoNameLabel->SetText(wrapTextInColor);
                    }
                    {
                        const pas::WideString& infoText = Equipment->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, aPlayer::GetPlayer());
                        GI_Label::TLabelGI* itemInfoTextLabel = ItemInfoTextLabel;
                        itemInfoTextLabel->SetText(infoText);
                    }
                    ItemInfoSizeLabel->SetText(pas::wide_int_to_str(Equipment->Weight));
                    ItemInfoCostLabel->SetText(pas::wide_int_to_str(Equipment->Cost));
                    {
                        GI_Image::TImageGI* cpp_with_2 = ItemInfoRaceIcon;
                        cpp_with_2->SetImagePath(aConst::GetFactionEmblemPath(aItem::TItem_GetOwnerConfigName(Equipment)));
                        cpp_with_2->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_2->SetImageKindY(GI_Main::ikyCenter);
                    }
                    if (static_cast<std::uint8_t>(pas::contains(DurableTypes, static_cast<std::uint8_t>(Equipment->ItemType)) ^ 1) && Equipment->ItemType != aConst::t_Hull) {
                        {
                            GI_Image::TImageGI* InfoDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"_wref.get()));
                            InfoDurable->Parent->Parent->SetActive(false);
                        }
                        MinimumWidth = 0;
                    } else {
                        if (pas::class_cast_if<aItem::THull*>(Equipment) != nullptr) {
                            pas::Extended cpp_right = pas::real_max<float>(0.1f, Equipment->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})));
                            BarWidth = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(Equipment->Weight, aConst::HullBaseSize), cpp_right)) * 64.0L);
                        } else {
                            BarWidth = System::Round(pas::real_divide(64.0L, pas::real_max<float>(0.1f, Equipment->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})))));
                        }
                        BarWidth = std::min<std::int32_t>(192, std::max<std::int32_t>(32, BarWidth));
                        {
                            GI_Image::TImageGI* InfoDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableLeft"_wref.get()));
                            CapWidth = InfoDurableLeft->GetContentSize().X;
                            MinimumWidth = 2 * CapWidth + BarWidth + InfoDurableLeft->LocalPosition.X + InfoDurableLeft->Parent->LocalPosition.X + 2 * InfoDurableLeft->Parent->Parent->LocalPosition.X;
                        }
                        {
                            GI_Image::TImageGI* InfoDurable_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"_wref.get()));
                            InfoDurable_2->Parent->Parent->SetActive(true);
                            InfoDurable_2->Parent->Parent->SetSize(ClassesImports::Point(2 * CapWidth + BarWidth, InfoDurable_2->Parent->Parent->ClientSize.Y));
                            InfoDurable_2->Parent->SetSize(ClassesImports::Point(BarWidth + 2, InfoDurable_2->Parent->Parent->ClientSize.Y));
                            if (Equipment->ItemType == aConst::t_Hull) {
                                pas::Extended cpp_left_2 = pas::checked_cast<aItem::THull*>(Equipment)->HullPoints;
                                std::int64_t cpp_left = System::Round(pas::real_divide(cpp_left_2, pas::checked_cast<aItem::THull*>(Equipment)->Weight) * BarWidth);
                                std::int32_t cpp_arg = cpp_left - (InfoDurable_2->GetContentSize().X - 5);
                                std::int32_t y = InfoDurable_2->LocalPosition.Y;
                                InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg, y));
                            } else {
                                std::int64_t cpp_left_3 = System::Round(BarWidth * pas::real_divide(Equipment->ConditionPercent, 1.0E+2L));
                                std::int32_t cpp_arg_2 = cpp_left_3 - (InfoDurable_2->GetContentSize().X - 5);
                                std::int32_t y_2 = InfoDurable_2->LocalPosition.Y;
                                InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_2, y_2));
                            }
                        }
                        {
                            GI_Image::TImageGI* InfoDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableRight"_wref.get()));
                            {
                                std::int32_t cpp_arg_3 = BarWidth + CapWidth - InfoDurableRight->GetContentSize().X;
                                std::int32_t y_3 = InfoDurableRight->LocalPosition.Y;
                                InfoDurableRight->SetPosition(ClassesImports::Point(cpp_arg_3, y_3));
                            }
                            InfoDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoDurableRight->Parent->LocalPosition.Y));
                            InfoDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableRight->Parent->ClientSize.Y));
                        }
                        {
                            GI_Image::TImageGI* InfoDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableBack"_wref.get()));
                            {
                                std::int32_t cpp_arg_4 = BarWidth + 1 - InfoDurableBack->GetContentSize().X;
                                std::int32_t y_4 = InfoDurableBack->LocalPosition.Y;
                                InfoDurableBack->SetPosition(ClassesImports::Point(cpp_arg_4, y_4));
                            }
                            InfoDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableBack->Parent->ClientSize.Y));
                        }
                    }
                    fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, ItemInfoNameLabel, ItemInfoTextLabel, true, true, MinimumWidth);
                    ItemInfoSizeLabel->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemSizeLabelPosition.Y));
                    ItemInfoCostLabel->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemPriceLabelPosition.Y));
                    ItemInfoRaceIcon->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
                }
            }
        }
    }

    void TfPlanetNO::ShowGoodsInfoPopup(aItem::TGoods* Item) {
        GetByName(u"PII"_wref.get())->SetActive(true);
        {
            GI_Image::TImageGI* InfoImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"_wref.get()));
            InfoImage->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(Item->ItemType)}));
            InfoImage->SetImageKindX(GI_Main::ikxCenter);
            InfoImage->SetImageKindY(GI_Main::ikyCenter);
            {
                Types::TPoint visualCenter = InfoImage->GetVisualCenter();
                Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                InfoImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
            }
        }
        {
            const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(aConst::GoodsMarket[Item->ItemType].DisplayName, aMyFunction::InfoNameColorTag);
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"_wref.get()));
            cpp_arg->SetText(wrapTextInColor);
        }
        {
            const pas::WideString& localizedText = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Text.", SysUtils::IntToStr(Item->ItemType + 1)})));
            GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"_wref.get()));
            cpp_arg_2->SetText(localizedText);
        }
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Item->Quantity);
            GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"_wref.get()));
            cpp_arg_3->SetText(intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Item->Cost);
            GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"_wref.get()));
            cpp_arg_4->SetText(intToStr_2);
        }
        {
            GI_Image::TImageGI* EmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"_wref.get()));
            EmRace->SetImagePath(aConst::GetFactionEmblemPath(aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace) & 0x0000007f].InternalName));
            EmRace->SetImageKindX(GI_Main::ikxCenter);
            EmRace->SetImageKindY(GI_Main::ikyCenter);
        }
        GetByName(u"InfoDurable"_wref.get())->Parent->Parent->SetActive(false);
        fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, ItemInfoNameLabel, ItemInfoTextLabel, true, true, 0);
        ItemInfoSizeLabel->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemSizeLabelPosition.Y));
        ItemInfoCostLabel->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemPriceLabelPosition.Y));
        ItemInfoRaceIcon->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
    }

    void TfPlanetNO::HideItemInfoPopup(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        HoveredItem = nullptr;
        if (ItemInfoHideTimer != nullptr) {
            CancelCallbackTimer(ItemInfoHideTimer);
            ItemInfoHideTimer = nullptr;
        }
        GetByName(u"PII"_wref.get())->SetActive(false);
    }

    void TfPlanetNO::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        GI_MessageLoop::TObjectGI* QuestButton{};
        if (LoadPanel->IsAnimatingShutters() || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            return;
        }
        if (Key == WindowsSdk::VK_SPACE) {
            if (GetByName(u"PM_EndTurn"_wref.get())->Active) {
                EndTurnClicked(nullptr);
            }
        } else if (Key == 'F') {
            TakeoffClicked(nullptr);
        } else if (Key == 'E') {
            ToggleResearchPanel(nullptr);
        } else if (Key == WindowsSdk::VK_LEFT) {
            if (ResearchPanelVisible) {
                ScrollSatellitePageLeft(nullptr);
            }
        } else if (Key == WindowsSdk::VK_RIGHT) {
            if (ResearchPanelVisible) {
                ScrollSatellitePageRight(nullptr);
            }
        } else if (Key == WindowsSdk::VK_ESCAPE) {
            if (ResearchPanelVisible) {
                if (HeldSatellite != nullptr) {
                    ReturnHeldSatellite();
                } else {
                    CloseResearchPanel();
                }
            } else {
                MainPanel->MenuClicked(nullptr);
            }
        } else if (Key == 'M') {
            GalaxyClicked(nullptr);
        } else if (Key == 'S') {
            ShipClicked(nullptr);
        } else if (Key == 'R') {
            QuestClicked(nullptr);
        } else if (Key == 'Q') {
            QuestButton = GetByName(u"QuestInfo_Run"_wref.get());
            if (QuestButton->Active) {
                StartTextQuest(QuestButton);
            }
        } else {
            MainPanel->ProcessKeyDown(Key);
        }
    }

    void TfPlanetNO::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10003);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20003);
        }
    }

    void TfPlanetNO::SelectMusic() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->GetShutterDirection() == -1) {
            return;
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId) & 0x0000007f].InternalName, u"Pirate"}));
                } else {
                    GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
                }
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
            }
        }
    }

    void TfPlanetNO::p_destroy() {
        fPlanetNO::TfPlanetNO_Destroy(this);
    }

} // namespace fPlanetNO
