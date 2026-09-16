#include "layout/fPlanet.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/SE_Planet.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "types/aShip.hpp"
#include "types/fPlanetQuest.hpp"
#include "types/fSaveManager.hpp"
#include "types/fShip2.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fPanelPlanet.hpp"
#include "units/fPlanet.hpp"

namespace fPlanet {
    void TfPlanet_Create(TfPlanet* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->PlanetPanel = pas::construct_call<fPanelPlanet::TfPanelPlanet>(fPanelPlanet::TfPanelPlanet_Create);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfPlanet_Destroy(TfPlanet* Self) {
        if (Self->PlanetPanel != nullptr) {
            pas::free(Self->PlanetPanel);
            Self->PlanetPanel = nullptr;
        }
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfPlanet::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        PlanetPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fPlanet... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GI_MessageLoop::TObjectGI* Panel = GetByName(u"MainPanel"_wref.get());
        Panel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Panel->FindByNameRecursive(u"BGCity"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* Info = Panel->FindByNameRecursive(u"PanelInfo"_wref.get());
        Info->SetPosition(ClassesImports::Point(Info->LocalPosition.X + GR_Main::ExtraScreenWidth, Info->LocalPosition.Y));
        GI_MessageLoop::TObjectGI* Quest = Panel->FindByNameRecursive(u"QuestInfo"_wref.get());
        Quest->SetPosition(ClassesImports::Point(Quest->LocalPosition.X + GR_Main::ExtraScreenWidth, Quest->LocalPosition.Y + GR_Main::ExtraScreenWidth));
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"_wref.get()))->UpCallback = pas::bind_method<&TfPlanet::EndTurnClicked>(this);
    }

    void TfPlanet::OnOpen() {
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
        }
        MainPanel->OnOpen();
        fPanelPlanet::TfPanelPlanet::OnOpen();
        LoadPanel->OnOpen();
        if (aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
            SoundSection = 0;
        } else {
            SoundSection = aPlayer::GetPlayer()->CurrentPlanet->RaceId + 1;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet != fEquipmentShop::TemporaryShopPlanet) {
            SelectMusic();
            if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                fEquipmentShop::RestoreTemporaryShopStock();
            }
            aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 0);
            Globals::PruneExpiredPersistentPlayerMessages();
            fEquipmentShop::BuildTemporaryShopSlotGrid();
        }
        aGalaxy::Galaxy->ReleaseItemGraphics();
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfPlanet::MainPanelKeyDown>(this);
        {
            pas::WideString governmentBackgroundGraph = aPlayer::GetPlayer()->CurrentPlanet->GetGovernmentBackgroundGraph();
            GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity"_wref.get()));
            cpp_arg->SetImagePath(std::move(governmentBackgroundGraph));
        }
        RefreshPlanetInfo();
        RefreshTextQuestPrompt();
        if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
            Event->AddTextData(u"PlanetCaptured"_w);
            GlobalsV::GameEndReason = 2;
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
            RequestClose(1);
            return;
        }
        if (aPlayer::GetPlayer()->PendingLiberationCeremonyPlanet == aPlayer::GetPlayer()->CurrentPlanet) {
            GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
            RequestClose(1);
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
            GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
            RequestClose(1);
            return;
        }
        if (aScript::DispatchPendingScriptRequests()) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(94);
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet != nullptr && (aPlayer::GetPlayer()->PendingDockDialogue > 0 || aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet)) {
            GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
            RequestClose(1);
            return;
        }
        if (GR_Main::CCInterface->GetResourceChecksumFailed() && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
            GR_Main::CCInterface->SetTamperDetected(true);
        }
        MainPanel->RebuildMessageButtons(false);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(94);
    }

    void TfPlanet::OnClose() {
        aGalaxy::Galaxy->CheckIntegrityChecksum(95);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        MainPanel->OnClose();
        fPanelPlanet::TfPanelPlanet::OnClose();
        LoadPanel->OnClose();
    }

    void TfPlanet::RefreshPlanetInfo() {
        GI_Window::TWindowGI* Window = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"PanelInfo"_wref.get()));
        {
            GI_Label::TLabelGI* PanelInfo_Name = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PanelInfo_Name"_wref.get()));
            if (aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                PanelInfo_Name->SetText(aPlayer::GetPlayer()->CurrentPlanet->Name);
            } else {
                PanelInfo_Name->SetText(([&] {
                    auto infoNameColorTag = pas::borrow(aMyFunction::InfoNameColorTag);
                    pas::WideString localizedText = aConst::LocalizedText(u"Planet.Civil.Info.TextNamePlanet"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedText), u"<Planet>"_w, std::move(name), infoNameColorTag.get());
                }()));
            }
        }
        {
            GI_Label::TLabelGI* PanelInfo_Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PanelInfo_Text"_wref.get()));
            PanelInfo_Text->SetText(aPlayer::GetPlayer()->CurrentPlanet->GetCivilInfoText());
            Window->SetSize(ClassesImports::Point(PanelInfo_Text->ClientSize.X + Window->WorkSubRect.Left + Window->WorkSubRect.Right, PanelInfo_Text->ClientSize.Y + Window->WorkSubRect.Top + Window->WorkSubRect.Bottom));
            Window->UpdateAutoGeometry();
            Window->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - 10 - Window->ClientSize.X, 10));
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
            fShip2::TfShip2::LayoutItemInfo(Window, cpp_arg_2, cpp_arg, true, false, 0);
        }
        {
            GI_Image::TImageGI* PanelInfo_Race = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PanelInfo_Race"_wref.get()));
            PanelInfo_Race->SetImagePath(aConst::GetFactionEmblemPath(aPlayer::GetPlayer()->CurrentPlanet->GetFactionResourceName()));
            PanelInfo_Race->SetImageKindX(GI_Main::ikxCenter);
            PanelInfo_Race->SetImageKindY(GI_Main::ikyCenter);
            PanelInfo_Race->SetPosition(ClassesImports::Point(Window->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, Window->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
        }
        {
            GI_Label::TLabelGI* PanelInfo_Text_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PanelInfo_Text"_wref.get()));
            PanelInfo_Text_2->SetTextAlignX(GI_Main::taxLeft);
        }
    }

    void TfPlanet::StartTextQuest(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender->UserValue != 0 && GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"FormGov.QuestCertificate.NotCertificateAttention"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(162);
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        Globals::StandaloneQuestMode = false;
        GlobalsV::QuestReturnScreenId = GlobalsV::FormToId(this);
        GlobalsV::RequestedScreenId = GlobalsV::screenPlanetQuest;
        RequestClose(1);
    }

    void TfPlanet::RefreshTextQuestPrompt() {
        std::int32_t QuestNumber{};
        std::int32_t I{};
        aRanger::PQuest Quest{};
        pas::WideString Text{};
        QuestNumber = -1;
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
                    QuestNumber = Quest->QuestNumber;
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
        if (!Window->Active) {
            return;
        }
        {
            GI_Label::TLabelGI* QuestInfo_Name = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"QuestInfo_Name"_wref.get()));
            QuestInfo_Name->SetText(aConst::LocalizedText(u"PlanetQuest.StartText.QuestCaption"_wref.get()));
        }
        {
            GI_Label::TLabelGI* QuestInfo_Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"QuestInfo_Text"_wref.get()));
            Text = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.StartText.", SysUtils::IntToStr(QuestNumber)})));
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
            QuestInfo_Run->UpCallback = pas::bind_method<&TfPlanet::StartTextQuest>(this);
            Window->SetSize(ClassesImports::Point(Window->ClientSize.X, QuestInfo_Run->ClientSize.Y + Window->ClientSize.Y + GR_Main::GiScalePixels(5)));
            Window->UpdateAutoGeometry();
            QuestInfo_Run->SetPosition(ClassesImports::Point(Window->ClientSize.X / 2 - QuestInfo_Run->ClientSize.X / 2, Window->ClientSize.Y - GR_Main::GiScalePixels(10) - QuestInfo_Run->ClientSize.Y));
        }
        {
            GI_Label::TLabelGI* QuestInfo_Name_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"QuestInfo_Name"_wref.get()));
            QuestInfo_Name_2->SetSize(ClassesImports::Point(Window->ClientSize.X - QuestInfo_Name_2->LocalPosition.X - Window->WorkSubRect.Right, QuestInfo_Name_2->ClientSize.Y));
        }
        Window->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - 10 - Window->ClientSize.X, GR_Main::GameScreenHeight - GR_Main::GiScalePixels(90) - Window->ClientSize.Y));
    }

    void TfPlanet::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        aGalaxy::Galaxy->CheckIntegrityChecksum(96);
        fEquipmentShop::RestoreTemporaryShopStock();
        MainPanel->EndTurnClicked(Sender);
        RefreshPlanetInfo();
        RefreshTextQuestPrompt();
        MainPanel->RebuildMessageButtons(false);
        if (ExitCode == 0) {
            fEquipmentShop::BuildTemporaryShopSlotGrid();
            aGalaxy::Galaxy->PrimeIntegrityChecksum(97);
        }
    }

    void TfPlanet::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        GI_MessageLoop::TObjectGI* Button{};
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            return;
        }
        if (Key == WindowsSdk::VK_SPACE) {
            if (GetByName(u"PM_EndTurn"_wref.get())->Active) {
                EndTurnClicked(nullptr);
            }
        } else if (Key == 'Q') {
            Button = GetByName(u"QuestInfo_Run"_wref.get());
            if (Button->Active) {
                StartTextQuest(Button);
            }
        } else {
            MainPanel->ProcessKeyDown(Key);
            PlanetPanel->ProcessKeyDown(Key);
        }
    }

    void TfPlanet::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (MainPanel->NavigationLocked) {
            return;
        }
        if (GR_Main::ExitScreenLoop) {
            return;
        }
        if (pas::in_set<0, 0, 2, 2, 4, 4, 6, 6>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10002);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20002);
        }
    }

    void TfPlanet::SelectMusic() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->GetShutterDirection() == -1) {
            return;
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
            return;
        }
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

    void TfPlanet::p_destroy() {
        fPlanet::TfPlanet_Destroy(this);
    }

} // namespace fPlanet
