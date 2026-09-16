#include "layout/fPanelMain.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_Sound.hpp"
#include "types/MessagesSdk.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/aEFilm.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fGov.hpp"
#include "types/fSaveManager.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fJournal.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fRating2.hpp"
#include "units/fShip2.hpp"

namespace fPanelMain {
    std::uint32_t CurrentDateColor{};

    std::uint32_t AdvancingDateColor{};

    void TfPanelMain_Create(TfPanelMain* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->NavigationLocked = false;
        Self->AuxiliaryItems = pas::make_object<pas::List>();
    }

    void TfPanelMain_Destroy(TfPanelMain* Self) {
        pas::free(Self->AuxiliaryItems);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TfPanelMain::InitializeLayout(GI_MessageLoop::TMessageLoopGI* Screen) {
        this->Screen = Screen;
        GR_Main::AppendLogTextThreadSafe("fPanelMain... "_a);
        {
            GI_MessageLoop::TObjectGI* PanelMain = this->Screen->GetByName(u"PanelMain"_wref.get());
            PanelMain->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* PM_PanelMsg = PanelMain->FindByNameRecursive(u"PM_PanelMsg"_wref.get());
                PM_PanelMsg->SetSize(ClassesImports::Point(PM_PanelMsg->ClientSize.X + GR_Main::ExtraScreenWidth, PM_PanelMsg->ClientSize.Y));
                PM_PanelMsg->SetPosition(ClassesImports::Point(PM_PanelMsg->LocalPosition.X, PM_PanelMsg->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* PM_WinMsg = PanelMain->FindByNameRecursive(u"PM_WinMsg"_wref.get());
                PM_WinMsg->SetSize(ClassesImports::Point(PM_WinMsg->ClientSize.X + GR_Main::ExtraScreenWidth, PM_WinMsg->ClientSize.Y));
                PM_WinMsg->SetPosition(ClassesImports::Point(PM_WinMsg->LocalPosition.X, PM_WinMsg->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* PM_Ship_Parent = PanelMain->FindByNameRecursive(u"PM_Ship"_wref.get())->Parent;
                PM_Ship_Parent->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, PM_Ship_Parent->ClientSize.Y));
                PM_Ship_Parent->SetPosition(ClassesImports::Point(PM_Ship_Parent->LocalPosition.X, PM_Ship_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                {
                    GI_Image::TImageGI* PM_ImageBG = pas::checked_cast<GI_Image::TImageGI*>(PM_Ship_Parent->FindByNameRecursive(u"PM_ImageBG"_wref.get()));
                    PM_ImageBG->SetPosition(ClassesImports::Point(0, 0));
                    PM_ImageBG->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, PM_ImageBG->ClientSize.Y));
                }
                {
                    GI_MessageLoop::TObjectGI* PM_Ship = PM_Ship_Parent->FindByNameRecursive(u"PM_Ship"_wref.get());
                    PM_Ship->SetPosition(ClassesImports::Point(PM_Ship->LocalPosition.X + GR_Main::ExtraScreenWidth, PM_Ship->LocalPosition.Y));
                }
                {
                    GI_MessageLoop::TObjectGI* PM_Gal = PM_Ship_Parent->FindByNameRecursive(u"PM_Gal"_wref.get());
                    PM_Gal->SetPosition(ClassesImports::Point(PM_Gal->LocalPosition.X + GR_Main::ExtraScreenWidth, PM_Gal->LocalPosition.Y));
                }
                {
                    GI_MessageLoop::TObjectGI* PM_Quest = PM_Ship_Parent->FindByNameRecursive(u"PM_Quest"_wref.get());
                    PM_Quest->SetPosition(ClassesImports::Point(PM_Quest->LocalPosition.X + GR_Main::ExtraScreenWidth, PM_Quest->LocalPosition.Y));
                }
                {
                    GI_GraphButton::TGraphButtonGI* PM_EndTurn = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(PM_Ship_Parent->FindByNameRecursive(u"PM_EndTurn"_wref.get()));
                    if (GR_Main::GiResourceVariant() == 2) {
                        PM_EndTurn->SetPosition(ClassesImports::Point(899 + GR_Main::ExtraScreenWidth, 33));
                    } else {
                        PM_EndTurn->SetPosition(ClassesImports::Point(702 + GR_Main::ExtraScreenWidth, 25));
                    }
                }
                {
                    GI_GraphButton::TGraphButtonGI* PM_Break = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(PM_Ship_Parent->FindByNameRecursive(u"PM_Break"_wref.get()));
                    if (GR_Main::GiResourceVariant() == 2) {
                        PM_Break->SetPosition(ClassesImports::Point(899 + GR_Main::ExtraScreenWidth, 33));
                    } else {
                        PM_Break->SetPosition(ClassesImports::Point(702 + GR_Main::ExtraScreenWidth, 25));
                    }
                }
                {
                    GI_MessageLoop::TObjectGI* PM_Logo = PM_Ship_Parent->FindByNameRecursive(u"PM_Logo"_wref.get());
                    if (GR_Main::GiResourceVariant() == 2) {
                        PM_Logo->SetPosition(ClassesImports::Point(0, 42));
                    } else {
                        PM_Logo->SetPosition(ClassesImports::Point(3, 32));
                    }
                }
                {
                    GI_MessageLoop::TObjectGI* PM_WarningSpace = PM_Ship_Parent->FindByNameRecursive(u"PM_WarningSpace"_wref.get());
                    PM_WarningSpace->SetPosition(ClassesImports::Point(PM_WarningSpace->LocalPosition.X + GR_Main::ExtraScreenWidth, PM_WarningSpace->LocalPosition.Y));
                }
                {
                    GI_MessageLoop::TObjectGI* PM_WarningMoney = PM_Ship_Parent->FindByNameRecursive(u"PM_WarningMoney"_wref.get());
                    PM_WarningMoney->SetPosition(ClassesImports::Point(PM_WarningMoney->LocalPosition.X + GR_Main::ExtraScreenWidth, PM_WarningMoney->LocalPosition.Y));
                }
                {
                    GI_MessageLoop::TObjectGI* PM_Money = PM_Ship_Parent->FindByNameRecursive(u"PM_Money"_wref.get());
                    PM_Money->SetPosition(ClassesImports::Point(PM_Money->LocalPosition.X + GR_Main::ExtraScreenWidth, PM_Money->LocalPosition.Y));
                }
                {
                    GI_MessageLoop::TObjectGI* PM_FreeSpace = PM_Ship_Parent->FindByNameRecursive(u"PM_FreeSpace"_wref.get());
                    PM_FreeSpace->SetPosition(ClassesImports::Point(PM_FreeSpace->LocalPosition.X + GR_Main::ExtraScreenWidth, PM_FreeSpace->LocalPosition.Y));
                }
                {
                    GI_MessageLoop::TObjectGI* PM_Help = PM_Ship_Parent->FindByNameRecursive(u"PM_Help"_wref.get());
                    PM_Help->SetSize(ClassesImports::Point(PM_Help->ClientSize.X + GR_Main::ExtraScreenWidth, PM_Help->ClientSize.Y));
                }
                {
                    GI_MessageLoop::TObjectGI* PM_PanelDate = PM_Ship_Parent->FindByNameRecursive(u"PM_PanelDate"_wref.get());
                    PM_PanelDate->SetPosition(ClassesImports::Point(PM_PanelDate->LocalPosition.X + GR_Main::ExtraScreenWidth, PM_PanelDate->LocalPosition.Y));
                }
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        MessagePanel = pas::checked_cast<GI_Panel::TPanelGI*>(this->Screen->GetByName(u"PM_PanelMsg"_wref.get()));
        MessagePanelRestTop = MessagePanel->LocalPosition.Y;
        HelpLabel = pas::checked_cast<GI_Label::TLabelGI*>(this->Screen->GetByName(u"PM_Help"_wref.get()));
        BackgroundImage = pas::checked_cast<GI_Image::TImageGI*>(this->Screen->GetByName(u"PM_ImageBG"_wref.get()));
        ShipButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PM_Ship"_wref.get()));
        GalaxyButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PM_Gal"_wref.get()));
        QuestButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PM_Quest"_wref.get()));
        EndTurnButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PM_EndTurn"_wref.get()));
        MenuButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PM_Logo"_wref.get()));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PM_EndTurn"_wref.get()))->UpCallback = pas::bind_method<&TfPanelMain::EndTurnClicked>(this);
        ShipButton->UpCallback = pas::bind_method<&TfPanelMain::ShipClicked>(this);
        QuestButton->UpCallback = pas::bind_method<&TfPanelMain::QuestClicked>(this);
        GalaxyButton->UpCallback = pas::bind_method<&TfPanelMain::GalaxyClicked>(this);
        MenuButton->UpCallback = pas::bind_method<&TfPanelMain::MenuClicked>(this);
        DateLabel = pas::checked_cast<GI_Label::TLabelGI*>(this->Screen->GetByName(u"PM_Date"_wref.get()));
        DateNextImage = pas::checked_cast<GI_Image::TImageGI*>(this->Screen->GetByName(u"PM_DateNextImage"_wref.get()));
        NextDateLabel = pas::checked_cast<GI_Label::TLabelGI*>(this->Screen->GetByName(u"PM_DateNew"_wref.get()));
        this->Screen->SetHelpCallback(pas::bind_method<&TfPanelMain::ShowControlHelp>(this));
        CurrentDateColor = GR_Main::GetStyleColorGI(u"PanelMain.TextColor"_w, 200, 240, 255);
        AdvancingDateColor = GR_Main::GetStyleColorGI(u"PanelMain.DateTransitionColor"_w, 6, 166, 198);
    }

    void TfPanelMain::OnOpen() {
        NavigationLocked = false;
        if (MessageSlideTimer != nullptr) {
            Screen->CancelCallbackTimer(MessageSlideTimer);
            MessageSlideTimer = nullptr;
        }
        if (MessagePulseTimer != nullptr) {
            Screen->CancelCallbackTimer(MessagePulseTimer);
            MessagePulseTimer = nullptr;
        }
        if (StatusTimer != nullptr) {
            Screen->CancelCallbackTimer(StatusTimer);
            StatusTimer = nullptr;
        }
        if (DateTimer != nullptr) {
            Screen->CancelCallbackTimer(DateTimer);
            DateTimer = nullptr;
        }
        StatusTimer = Screen->ScheduleCallbackTimer(200, 200, pas::bind_method<&TfPanelMain::RefreshStatusTimer>(this), 0);
        MessagePanel->SetPosition(ClassesImports::Point(MessagePanel->LocalPosition.X, MessagePanelRestTop));
        Screen->GetByName(u"PM_Help"_wref.get())->SetActive(false);
        EnableNavigationButtons();
        DisplayedShipId = 0u;
        DisplayedPlanetId = 0u;
        DisplayedTurn = aGalaxy::Galaxy->CurrentTurn;
        DateSlideProgress = 1.0f;
        TargetTurn = aGalaxy::Galaxy->CurrentTurn;
        pas::list_clear(AuxiliaryItems);
        CargoWarningActive = false;
        MoneyWarningActive = false;
        RefreshMoneyAndCargo();
    }

    void TfPanelMain::OnClose() {
        NavigationLocked = false;
        if (StatusTimer != nullptr) {
            Screen->CancelCallbackTimer(StatusTimer);
            StatusTimer = nullptr;
        }
        if (MessagePulseTimer != nullptr) {
            Screen->CancelCallbackTimer(MessagePulseTimer);
            MessagePulseTimer = nullptr;
        }
        if (MessageSlideTimer != nullptr) {
            Screen->CancelCallbackTimer(MessageSlideTimer);
            MessageSlideTimer = nullptr;
        }
        if (DateTimer != nullptr) {
            Screen->CancelCallbackTimer(DateTimer);
            DateTimer = nullptr;
        }
        if (CargoWarningTimer != nullptr) {
            Screen->CancelCallbackTimer(CargoWarningTimer);
            CargoWarningTimer = nullptr;
        }
        if (MoneyWarningTimer != nullptr) {
            Screen->CancelCallbackTimer(MoneyWarningTimer);
            MoneyWarningTimer = nullptr;
        }
        pas::list_clear(AuxiliaryItems);
    }

    void TfPanelMain::Show() {
        Screen->GetByName(u"PanelMain"_wref.get())->SetActive(true);
        Screen->GetByName(u"PM_Help"_wref.get())->SetActive(false);
        EnableNavigationButtons();
    }

    void TfPanelMain::Hide() {
        Screen->GetByName(u"PanelMain"_wref.get())->SetActive(false);
    }

    void TfPanelMain::RefreshMoneyAndCargo() {
        std::int32_t FreeSpace{};
        if (GlobalsV::CurrentScreenId != GlobalsV::screenStarMap || Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
            TargetTurn = aGalaxy::Galaxy->CurrentTurn;
            if (DisplayedTurn < TargetTurn && DateSlideProgress >= 1.0L) {
                DateSlideProgress = 0.0f;
            }
            RefreshDate();
        }
        if (aPlayer::GetPlayer() != nullptr) {
            if (MoneyWarningActive && (MoneyWarningTicks & 1) == 0) {
                Screen->GetByName(u"PM_WarningMoney"_wref.get())->SetActive(true);
                {
                    std::uint32_t packRgbBytes = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 128, 61);
                    GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_Money"_wref.get()));
                    cpp_arg->SetTextColor(packRgbBytes);
                }
            } else {
                Screen->GetByName(u"PM_WarningMoney"_wref.get())->SetActive(false);
                pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_Money"_wref.get()))->SetTextColor(CurrentDateColor);
            }
            {
                const pas::WideString& intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->Money);
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_Money"_wref.get()));
                cpp_arg_2->SetText(intToStr);
            }
            FreeSpace = aPlayer::GetPlayer()->GetCargoFreeSpace();
            if (CargoWarningActive && (CargoWarningTicks & 1) == 0 || FreeSpace < 0 && static_cast<std::uint8_t>(CargoWarningActive ^ 1)) {
                Screen->GetByName(u"PM_WarningSpace"_wref.get())->SetActive(true);
                {
                    std::uint32_t packRgbBytes_2 = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 128, 61);
                    GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_FreeSpace"_wref.get()));
                    cpp_arg_3->SetTextColor(packRgbBytes_2);
                }
            } else {
                Screen->GetByName(u"PM_WarningSpace"_wref.get())->SetActive(false);
                pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_FreeSpace"_wref.get()))->SetTextColor(CurrentDateColor);
            }
            {
                const pas::WideString& intToStr_2 = pas::wide_int_to_str(FreeSpace);
                GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_FreeSpace"_wref.get()));
                cpp_arg_4->SetText(intToStr_2);
            }
        } else {
            pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_Money"_wref.get()))->SetText(u""_wref.get());
            pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_FreeSpace"_wref.get()))->SetText(u""_wref.get());
        }
    }

    void TfPanelMain::RefreshDate() {
        {
            const pas::WideString& formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(DisplayedTurn);
            GI_Label::TLabelGI* dateLabel = DateLabel;
            dateLabel->SetText(formatTurnDate);
        }
        if (DisplayedTurn < TargetTurn) {
            const pas::WideString& formatTurnDate_2 = aGalaxy::Galaxy->FormatTurnDate(DisplayedTurn + 1);
            GI_Label::TLabelGI* nextDateLabel = NextDateLabel;
            nextDateLabel->SetText(formatTurnDate_2);
        } else {
            const pas::WideString& formatTurnDate_3 = aGalaxy::Galaxy->FormatTurnDate(DisplayedTurn);
            GI_Label::TLabelGI* nextDateLabel_2 = NextDateLabel;
            nextDateLabel_2->SetText(formatTurnDate_3);
        }
        std::int32_t X = -System::Round(static_cast<long double>(DateLabel->ClientSize.X + DateNextImage->ClientSize.X) * DateSlideProgress);
        DateLabel->SetPosition(ClassesImports::Point(X, 0));
        DateNextImage->SetPosition(ClassesImports::Point(X + DateLabel->ClientSize.X, DateNextImage->LocalPosition.Y));
        NextDateLabel->SetPosition(ClassesImports::Point(X + DateLabel->ClientSize.X + DateNextImage->ClientSize.X, 0));
        if (DisplayedTurn >= TargetTurn || DisplayedTurn >= TargetTurn - 1 && DateSlideProgress >= 1.0L) {
            DateLabel->SetTextColor(CurrentDateColor);
            NextDateLabel->SetTextColor(CurrentDateColor);
        } else {
            DateLabel->SetTextColor(AdvancingDateColor);
            NextDateLabel->SetTextColor(AdvancingDateColor);
        }
    }

    void TfPanelMain::SetDateRange(std::int32_t FirstTurn, std::int32_t LastTurn) {
        DisplayedTurn = FirstTurn;
        TargetTurn = LastTurn;
        if (DisplayedTurn > TargetTurn) {
            DisplayedTurn = TargetTurn;
        }
        if (DisplayedTurn != TargetTurn) {
            DateSlideProgress = 0.0f;
        } else {
            DateSlideProgress = 1.0f;
        }
        RefreshDate();
    }

    void TfPanelMain::StartDateAnimation(std::int32_t IntervalMs) {
        if (DisplayedTurn < TargetTurn && (DisplayedTurn < TargetTurn - 1 || DateSlideProgress < 1.0L)) {
            if (DateTimer != nullptr) {
                Screen->CancelCallbackTimer(DateTimer);
                DateTimer = nullptr;
            }
            DateTimerIntervalMs = IntervalMs;
            DateTimer = Screen->ScheduleCallbackTimer(IntervalMs, IntervalMs, pas::bind_method<&TfPanelMain::AdvanceDateAnimation>(this), 0);
        }
    }

    void TfPanelMain::AdvanceDateAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        DateSlideProgress = (TargetTurn - DisplayedTurn) * 0.01L * (TargetTurn - DisplayedTurn) + DateSlideProgress;
        if (DateSlideProgress >= 1.0L) {
            ++DisplayedTurn;
            if (TargetTurn - DisplayedTurn > 300) {
                DisplayedTurn = TargetTurn - 10;
            }
            if (DisplayedTurn >= TargetTurn) {
                DateSlideProgress = 1.0f;
                if (DateTimer != nullptr) {
                    Screen->CancelCallbackTimer(DateTimer);
                    DateTimer = nullptr;
                    aScript::StartScriptRequestThread();
                }
            } else {
                DateSlideProgress = 0.0f;
            }
        }
        RefreshDate();
    }

    void TfPanelMain::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
            return;
        }
        if (aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->RuinsMode != 0) {
            return;
        }
        if (Screen->ParentLoop != nullptr) {
            return;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
            if (Screen != Globals::GovernmentScreen) {
                GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
                Screen->RequestClose(1);
            }
            return;
        }
        if (!aCalc::IsTurnCalculationRunningUI()) {
            TryAutoTurnSave();
            GR_Main::SoundManager->PlaySound(u"Sound.Turn"_wref.get());
            Globals::PruneExpiredPersistentPlayerMessages();
            aCalc::CalculatePlayerStarTurnAndWait();
            if (GR_Main::ExitScreenLoop) {
                return;
            }
            if (aPlayer::GetPlayer() != nullptr) {
                ++aGalaxy::Galaxy->CurrentTurn;
                RefreshEndTurnButton();
                StartDateAnimation(10);
                --aGalaxy::Galaxy->CurrentTurn;
                aCalc::CalculateGalaxyTurnAndWait();
            }
        }
        if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
            Event->AddTextData(u"PlanetCaptured"_w);
            GlobalsV::GameEndReason = 2;
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
            Screen->RequestClose(1);
        } else {
            GR_Main::PostMouseMoveMessage();
            if (aPlayer::GetPlayer()->PendingDockDialogue > 0 && aPlayer::GetPlayer()->DockedTo != nullptr) {
                GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
            } else if (aPlayer::GetPlayer()->PendingDockDialogue > 0 && aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
            }
        }
    }

    void TfPanelMain::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (Screen->ParentLoop != nullptr && Sender == nullptr) {
            return;
        }
        if (Screen->ExitCode != 0) {
            return;
        }
        Screen->GetByName(u"PM_WinMsg"_wref.get())->SetActive(false);
        Screen->SetCursorActive(false);
        Screen->Present();
        GR_Main::CaptureScreenBackground(true, 0);
        Screen->SetCursorActive(true);
        std::uint8_t Changed = false;
        Globals::ShipScreen->PlayTransitionSounds = true;
        aGalaxy::Galaxy->CheckIntegrityChecksum(109);
        while (true) {
            ClearMessageButtons();
            fShip2::RunShipEquipment(Screen);
            RebuildMessageButtons(false);
            TfPanelMain::PostMouseMove();
            if (Globals::ShipScreen->Flag3BC) {
                Changed = true;
            }
            RefreshMoneyAndCargo();
            RebuildMessageButtons(false);
            if (!Globals::ShipScreen->FlagD4) {
                break;
            }
            Screen->SetCursorActive(false);
            GR_Main::FullFrameRedrawRequested = true;
            Screen->InvalidateViewport();
            Screen->DrawQueuedUpdateRects();
            GR_Main::CaptureScreenBackground(true, 0);
            Screen->SetCursorActive(true);
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(110);
        Globals::ShipScreen->Flag3BC = Changed;
        if (aPlayer::GetPlayer()->IsOnPlanet() && Screen != Globals::GovernmentScreen) {
            if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
                GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
                Screen->RequestClose(1);
            }
        }
    }

    void TfPanelMain::QuestClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (Screen->ExitCode != 0) {
            return;
        }
        if (Screen->ParentLoop != nullptr && Sender == nullptr) {
            return;
        }
        Screen->GetByName(u"PM_WinMsg"_wref.get())->SetActive(false);
        Screen->SetCursorActive(false);
        Screen->Present();
        GR_Main::CaptureScreenBackground(true, 0);
        Screen->SetCursorActive(true);
        aGalaxy::Galaxy->CheckIntegrityChecksum(107);
        ClearMessageButtons();
        fRating2::ShowRangerRating(Screen);
        RebuildMessageButtons(false);
        TfPanelMain::PostMouseMove();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(108);
    }

    void TfPanelMain::GalaxyClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (Screen->ParentLoop != nullptr && Sender == nullptr) {
            return;
        }
        if (Screen->ExitCode != 0) {
            return;
        }
        Screen->GetByName(u"PM_WinMsg"_wref.get())->SetActive(false);
        Screen->SetCursorActive(false);
        Screen->Present();
        GR_Main::CaptureScreenBackground(true, 0);
        Screen->SetCursorActive(true);
        Globals::GalaxyScreen->ViewMode = 1;
        aGalaxy::Galaxy->CheckIntegrityChecksum(140);
        ClearMessageButtons();
        fGalaxy2::RunGalaxyMap(Screen);
        RebuildMessageButtons(false);
        TfPanelMain::PostMouseMove();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(141);
    }

    void TfPanelMain::JournalClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (Screen->ExitCode != 0) {
            return;
        }
        if (Screen->ParentLoop != nullptr && Sender == nullptr) {
            return;
        }
        Screen->GetByName(u"PM_WinMsg"_wref.get())->SetActive(false);
        Screen->SetCursorActive(false);
        Screen->Present();
        GR_Main::CaptureScreenBackground(true, 0);
        Screen->SetCursorActive(true);
        aGalaxy::Galaxy->CheckIntegrityChecksum(102);
        ClearMessageButtons();
        fJournal::RunJournal(Screen);
        RebuildMessageButtons(false);
        TfPanelMain::PostMouseMove();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(103);
    }

    void TfPanelMain::MenuClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (Screen->ParentLoop != nullptr) {
            return;
        }
        Screen->SetCursorActive(false);
        Screen->Present();
        GR_Main::CaptureScreenBackground(true, 0);
        GR_Main::CaptureSavePreview();
        aGalaxy::Galaxy->CheckIntegrityChecksum(98);
        fGalaxy2::CaptureGalaxyPreview(Screen);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(99);
        Screen->SetCursorActive(true);
        GlobalsV::GameMenuReturnScreenId = GlobalsV::FormToId(Screen);
        GlobalsV::RequestedScreenId = GlobalsV::screenGameMenu;
        Screen->RequestClose(1);
    }

    void TfPanelMain::TryAutoTurnSave() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aGalaxy::Galaxy->IronWill) {
            return;
        }
        if (GlobalsV::TurnSaveStep == 0) {
            return;
        }
        if (pas::imod(aGalaxy::Galaxy->CurrentTurn, GlobalsV::TurnSaveStep) != 0) {
            return;
        }
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            return;
        }
        Screen->SetCursorActive(false);
        Screen->Present();
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(Screen);
        Screen->SetCursorActive(false);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(Screen);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetTurnSavePath(), u"TurnSave"_w);
        Screen->SetCursorActive(true);
    }

    void TfPanelMain::QuickSave() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (Globals::GetInnermostScreenLoop()->ParentLoop != nullptr) {
            return;
        }
        if (aGalaxy::Galaxy->IronWill) {
            {
                const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"FormGameSet2.IronWillText"_wref.get());
                GI_MessageLoop::TMessageLoopGI* screen = Screen;
                GI_MessageBox::ShowMessageBoxGI(screen, localizedColorText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
            }
            GR_Main::FullFrameRedrawRequested = true;
            return;
        }
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            return;
        }
        Screen->SetCursorActive(false);
        Screen->Present();
        GR_Main::CaptureSavePreview();
        aGalaxy::Galaxy->CheckIntegrityChecksum(132);
        fGalaxy2::CaptureGalaxyPreview(Screen);
        Screen->SetCursorActive(false);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(Screen);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetQuickSavePath(1), u"QuickSave"_w);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(133);
        Screen->SetCursorActive(true);
    }

    void TfPanelMain::QuickLoad(std::int32_t SlotIndex) {
        pas::WideString Text{};
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (Globals::GetInnermostScreenLoop()->ParentLoop != nullptr) {
            return;
        }
        Screen->SetCursorActive(false);
        if (Globals::SaveManagerScreen->QuickSaveExists(SlotIndex)) {
            if (GlobalsV::QuickSaveExtraSlots > 0 || SlotIndex > 1) {
                Text = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(SlotIndex);
                    pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"FormSaveManager.QueryQuickN"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(lookupLocalizedTextByKey), u"<Num>"_w, std::move(intToStr), u"<color=255,240,100>"_w);
                }());
            } else {
                Text = GR_Main::LookupLocalizedTextByKey(u"FormSaveManager.QueryQuick"_wref.get());
            }
            if (GI_MessageBox::ShowMessageBoxGI(Screen, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                GlobalsV::PendingLoadFileName = static_cast<pas::AnsiString>(fSaveManager::TfSaveManager::GetQuickSavePath(SlotIndex));
                GlobalsV::RequestedScreenId = GlobalsV::screenGameLoad;
                Screen->RequestClose(1);
                return;
            }
        } else if (GlobalsV::QuickSaveExtraSlots + 1 >= SlotIndex) {
            if (GlobalsV::QuickSaveExtraSlots > 0) {
                Text = ([&] {
                    pas::WideString intToStr_2 = pas::wide_int_to_str(SlotIndex);
                    pas::WideString lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(u"FormSaveManager.QuickNotExistN"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(lookupLocalizedTextByKey_2), u"<Num>"_w, std::move(intToStr_2), u"<color=255,240,100>"_w);
                }());
            } else {
                Text = GR_Main::LookupLocalizedTextByKey(u"FormSaveManager.QuickNotExist"_wref.get());
            }
            GI_MessageBox::ShowMessageBoxGI(Screen, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
            GR_Main::FullFrameRedrawRequested = true;
        }
        Screen->SetCursorActive(true);
        Screen->Present();
    }

    void TfPanelMain::RefreshStatusTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        RefreshEndTurnButton();
    }

    void TfPanelMain::PulseUnreadMessages(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_MessageLoop::TObjectGI* Control{};
        std::uint8_t Finished{};
        Globals::TMessagePlayer* MessageEntry{};
        std::uint8_t Alpha{};
        pas::critical_enter(Globals::PersistentPlayerMessageLock);
        std::int32_t Stage = 0;
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Globals::PersistentPlayerMessageLock);
            };
            ++MessagePulseStep;
            Finished = true;
            if (MessagePanel->Active == true) {
                Stage = 1;
                MessagePulseStep = MessagePulseStep % 32;
                if (MessagePulseStep < 16) {
                    Alpha = 255 - MessagePulseStep * 8;
                } else {
                    Alpha = 127 + (MessagePulseStep - 16) * 8;
                }
                Control = MessagePanel->FirstChild;
                while (Control != nullptr) {
                    Stage = 2;
                    MessageEntry = reinterpret_cast<Globals::TMessagePlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Control->UserValue)));
                    if (Globals::IsPersistentPlayerMessageQueued(MessageEntry, true) && MessageEntry->Button == Control) {
                        Stage = 3;
                        if (pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                            Stage = 4;
                            if (pas::in_set<0, 0, 6, 6>(MessageEntry->Kind) && static_cast<std::uint8_t>(MessageEntry->WasRead ^ 1)) {
                                Stage = 5;
                                {
                                    GI_GraphButton::TGraphButtonGI* cpp_with = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control);
                                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.MsgPlayer.", GR_Main::GiResourceSuffix(), MessageEntry->GetNormalImageName()}), cpp_with->ImageNormal->GraphBufControl->GraphBuf);
                                    Stage = 6;
                                    GR_GraphBuf::TGraphBufGR_ScaleAlpha(cpp_with->ImageNormal->GraphBufControl->GraphBuf, ClassesImports::Rect(0, 0, cpp_with->ImageNormal->GraphBufControl->GraphBuf->Width, cpp_with->ImageNormal->GraphBufControl->GraphBuf->Height), Alpha);
                                    Stage = 7;
                                    Finished = false;
                                    cpp_with->Invalidate();
                                    Stage = 8;
                                }
                            } else if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control)->ImageNormal != nullptr && pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control)->ImageNormal->GraphBufControl != nullptr) {
                                Stage = 9;
                                {
                                    pas::WideString cpp_arg = pas::concat_wide({u"GI,Bm.MsgPlayer.", GR_Main::GiResourceSuffix(), MessageEntry->GetNormalImageName()});
                                    GI_Image::TImageGI* imageNormal = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control)->ImageNormal;
                                    imageNormal->SetImagePath(std::move(cpp_arg));
                                }
                                Stage = 10;
                                pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control)->Invalidate();
                            }
                        }
                    }
                    Stage = 11;
                    Control = Control->NextSibling;
                }
            }
            Stage = 12;
            if (Finished && MessagePulseTimer != nullptr) {
                Screen->CancelCallbackTimer(MessagePulseTimer);
                MessagePulseTimer = nullptr;
            }
        }
    }

    void TfPanelMain::RefreshEndTurnButton() {
        GI_GraphButton::TGraphButtonGI* Button = EndTurnButton;
        if (aCalc::IsTurnCalculationRunningUI()) {
            Button->SetDisabled(true);
        } else {
            Button->SetDisabled(false);
        }
        RefreshMoneyAndCargo();
    }

    void TfPanelMain::DisableNavigationButtons() {
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Screen->GetByName(u"PM_Ship"_wref.get()))->SetDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Screen->GetByName(u"PM_Gal"_wref.get()))->SetDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Screen->GetByName(u"PM_Quest"_wref.get()))->SetDisabled(true);
        EndTurnButton->SetActive(false);
        GI_MessageLoop::TObjectGI* Control = Screen->GetByName(u"PM_Break"_wref.get());
        Control->HelpCallback = pas::bind_method<&TfPanelMain::ShowControlHelp>(this);
        Control->SetActive(true);
        MenuButton->SetDisabled(true);
    }

    void TfPanelMain::EnableNavigationButtons() {
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Screen->GetByName(u"PM_Ship"_wref.get()))->SetDisabled(false);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Screen->GetByName(u"PM_Gal"_wref.get()))->SetDisabled(false);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Screen->GetByName(u"PM_Quest"_wref.get()))->SetDisabled(false);
        EndTurnButton->SetActive(true);
        GI_MessageLoop::TObjectGI* Control = Screen->GetByName(u"PM_Break"_wref.get());
        Control->HelpCallback = nullptr;
        Control->SetActive(false);
        MenuButton->SetDisabled(false);
        GR_Main::PostMouseMoveMessage();
    }

    void TfPanelMain::RebuildMessageButtons(std::uint8_t SkipLock) {
        GI_Panel::TPanelGI* Panel{};
        Globals::TMessagePlayer* MessageEntry{};
        GI_GraphButton::TGraphButtonGI* Button{};
        std::int32_t Count{};
        std::int32_t MaxCount{};
        std::int32_t X{};
        ClearMessageButtons();
        if (!SkipLock) {
            pas::critical_enter(Globals::PersistentPlayerMessageLock);
        }
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                Screen->GetByName(u"PM_Help"_wref.get())->SetActive(false);
                if (Globals::FirstPersistentPlayerMessage == nullptr) {
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                DisplayedShipId = 0u;
                DisplayedPlanetId = 0u;
                Panel = pas::checked_cast<GI_Panel::TPanelGI*>(Screen->GetByName(u"PM_PanelMsg"_wref.get()));
                Panel->SetActive(true);
                Screen->GetByName(u"PM_WinMsg"_wref.get())->SetActive(false);
                MessageEntry = Globals::LastPersistentPlayerMessage;
                Count = 0;
                MaxCount = (Panel->ClientSize.X - 44) / 22;
                while (MessageEntry != nullptr && Count < MaxCount) {
                    ++Count;
                    MessageEntry = MessageEntry->Prev;
                }
                if (MessageEntry == nullptr) {
                    MessageEntry = Globals::FirstPersistentPlayerMessage;
                }
                X = 0;
                while (MessageEntry != nullptr) {
                    Button = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, Panel);
                    Button->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(MessageEntry));
                    MessageEntry->Button = Button;
                    Button->MouseEnterCallback = pas::bind_method<&TfPanelMain::MessageMouseEnter>(this);
                    Button->MouseLeaveCallback = pas::bind_method<&TfPanelMain::MessageMouseLeave>(this);
                    Button->RightButtonDownCallback = pas::bind_method<&TfPanelMain::MessageRightButtonDown>(this);
                    Button->UpCallback = pas::bind_method<&TfPanelMain::MessageClicked>(this);
                    Button->EnterSound = u"Sound.ButtonInfoEnter"_w;
                    Button->LeaveSound = u"Sound.ButtonInfoLeave"_w;
                    Button->ClickSound = u"Sound.ButtonInfoClick"_w;
                    if (MessageEntry->Kind == 1 && aPlayer::GetPlayer() != nullptr && (static_cast<std::int32_t>(MessageEntry->Targets[0].ShipId) == aPlayer::GetPlayer()->Id || static_cast<std::int32_t>(MessageEntry->Targets[1].ShipId) == aPlayer::GetPlayer()->Id || static_cast<std::int32_t>(MessageEntry->Targets[2].ShipId) == aPlayer::GetPlayer()->Id)) {
                        MessageEntry->Kind = 10;
                    }
                    if (pas::in_set<0, 0, 6, 6>(MessageEntry->Kind) && static_cast<std::uint8_t>(MessageEntry->WasRead ^ 1)) {
                        Button->SetImageNormalPath(u"GraphBuf"_wref.get());
                        Button->ImageNormal->GraphBufControl->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.MsgPlayer.", GR_Main::GiResourceSuffix(), MessageEntry->GetNormalImageName()}), Button->ImageNormal->GraphBufControl->GraphBuf);
                        {
                            Types::TPoint contentSize = Button->ImageNormal->GetContentSize();
                            GI_Image::TImageGI* imageNormal = Button->ImageNormal;
                            imageNormal->SetSize(contentSize);
                        }
                        if (MessagePulseTimer == nullptr) {
                            MessagePulseTimer = Screen->ScheduleCallbackTimer(40, 40, pas::bind_method<&TfPanelMain::PulseUnreadMessages>(this), 0);
                        }
                    } else {
                        Button->SetImageNormalPath(pas::concat_wide({u"GI,Bm.MsgPlayer.", GR_Main::GiResourceSuffix(), MessageEntry->GetNormalImageName()}));
                    }
                    Button->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.MsgPlayer.", GR_Main::GiResourceSuffix(), MessageEntry->GetActiveImageName()}));
                    Button->SetImageDownPath(pas::concat_wide({u"GI,Bm.MsgPlayer.", GR_Main::GiResourceSuffix(), MessageEntry->GetPressedImageName()}));
                    Button->HitKind = GI_GraphButton::gbhRect;
                    Button->SetSize(Button->GetMaxStateImageSize());
                    Button->SetPosition(ClassesImports::Point(X, Panel->ClientSize.Y / 2 - Button->ClientSize.Y / 2));
                    X = X + Button->ClientSize.X + 2;
                    Button->UpdateStateImagePlacement();
                    Button->UpdateStateVisuals();
                    MessageEntry = MessageEntry->Next;
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            if (!SkipLock) {
                pas::critical_leave(Globals::PersistentPlayerMessageLock);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return;
            }
        }
        TfPanelMain::PlayUnreadMessageSounds();
        SlideMessagesIn();
        TfPanelMain::PostMouseMove();
    }

    void TfPanelMain::ClearMessageButtons() {
        if (MessagePulseTimer != nullptr) {
            Screen->CancelCallbackTimer(MessagePulseTimer);
            MessagePulseTimer = nullptr;
        }
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(Screen->GetByName(u"PM_PanelMsg"_wref.get()));
        Panel->SetActive(false);
        Panel->FreeOwnedChildren();
        Screen->GetByName(u"PM_WinMsg"_wref.get())->SetActive(false);
        Globals::TMessagePlayer* MessageEntry = Globals::LastPersistentPlayerMessage;
        while (MessageEntry != nullptr) {
            MessageEntry->Button = nullptr;
            MessageEntry = MessageEntry->Prev;
        }
    }

    std::uint8_t TfPanelMain::RemoveDismissibleMessages(pas::WideString Key) {
        std::uint8_t Result = false;
        if (Globals::RemovePlayerMessagesExceptKinds(Key, pas::constant_set<Globals::TPlayerMessageKindSet>({{3}, {9}}), false)) {
            RebuildMessageButtons(false);
            GR_Main::SoundManager->PlaySound(u"Sound.DelMsg"_wref.get());
            return true;
        }
        return Result;
    }

    void TfPanelMain::MessageMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        Globals::TMessagePlayer* MessageEntry{};
        GI_Panel::TPanelGI* Panel{};
        GI_Window::TWindowGI* Window{};
        GI_Label::TLabelGI* LabelControl{};
        pas::critical_enter(Globals::PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Globals::PersistentPlayerMessageLock);
            };
            MessageEntry = reinterpret_cast<Globals::TMessagePlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
            if (!Globals::IsPersistentPlayerMessageQueued(MessageEntry, true)) {
                return;
            }
            Panel = pas::checked_cast<GI_Panel::TPanelGI*>(Screen->GetByName(u"PM_PanelMsg"_wref.get()));
            Window = pas::checked_cast<GI_Window::TWindowGI*>(Screen->GetByName(u"PM_WinMsg"_wref.get()));
            LabelControl = pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"PM_LabelMsg"_wref.get()));
            LabelControl->SetTextAlignY(GI_Main::tayAuto);
            LabelControl->SetText(MessageEntry->Text);
            LabelControl->SetTextAlignY(GI_Main::tayCenterEx);
            Window->SetSize(ClassesImports::Point(LabelControl->ClientSize.X + Window->WorkSubRect.Left + Window->WorkSubRect.Right, LabelControl->ClientSize.Y + Window->WorkSubRect.Top + Window->WorkSubRect.Bottom));
            Window->UpdateAutoGeometry();
            Window->SetPosition(ClassesImports::Point(Window->LocalPosition.X, Panel->LocalPosition.Y - Window->ClientSize.Y - 5 - 5));
            Window->SetActive(true);
            LabelControl->SetSize(ClassesImports::Point(Window->ClientSize.X - Window->WorkSubRect.Left - Window->WorkSubRect.Right, Window->ClientSize.Y - Window->WorkSubRect.Top - Window->WorkSubRect.Bottom));
            if (!MessageEntry->WasRead) {
                MessageEntry->WasRead = true;
                if (MessageEntry->Kind == 6) {
                    MessageEntry->Turn = aGalaxy::Galaxy->CurrentTurn;
                }
            }
            LabelControl->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&Window->WorkSubRect, 0)));
            Window->SetPosition(ClassesImports::Point(Sender->HitTestBounds.Left + Sender->ClientSize.X / 2, Sender->HitTestBounds.Top - Window->ClientSize.Y - 5));
        }
    }

    void TfPanelMain::MessageMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        Screen->GetByName(u"PM_WinMsg"_wref.get())->SetActive(false);
    }

    void TfPanelMain::DeleteMessage(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point, std::uint8_t SkipLock) {
        Globals::TMessagePlayer* MessageEntry{};
        GI_MessageLoop::TObjectGI* Control{};
        GI_GAI::TgaiGI* Animation{};
        if (!SkipLock) {
            pas::critical_enter(Globals::PersistentPlayerMessageLock);
        }
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                MessageEntry = reinterpret_cast<Globals::TMessagePlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
                if (!Globals::IsPersistentPlayerMessageQueued(MessageEntry, true)) {
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                if (pas::in_set<3, 3, 9, 9>(MessageEntry->Kind)) {
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                Control = MessagePanel->FirstChild;
                while (Control != nullptr) {
                    if (pas::class_cast_if<GI_GAI::TgaiGI*>(Control) != nullptr && Control->Active) {
                        break;
                    }
                    Control = Control->NextSibling;
                }
                if (Control != nullptr) {
                    if (pas::list_indexof(AuxiliaryItems, reinterpret_cast<void*>(MessageEntry)) < 0) {
                        pas::list_add(AuxiliaryItems, reinterpret_cast<void*>(MessageEntry));
                    }
                } else {
                    Screen->GetByName(u"PM_WinMsg"_wref.get())->SetActive(false);
                    GR_Main::SoundManager->PlaySound(u"Sound.DelMsg"_wref.get());
                    Sender->SetActive(false);
                    Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, MessagePanel);
                    Animation->SetImagePath(u"Bm.PanelMain2.MsgDel"_wref.get());
                    Animation->SequenceIndex = 0;
                    Animation->UpdateAutoGeometry();
                    Animation->SetSize(Animation->GetContentSize());
                    Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
                    Animation->SetPosition(EC_Struct::AddPoints(Sender->LocalPosition, EC_Struct::HalfPoint(Sender->ClientSize)));
                    Animation->RestartPlayback();
                    Animation->FrameAdvancedCallback = pas::bind_method<&TfPanelMain::AdvanceMessageDeletion>(this);
                    Animation->CycleCompleteCallback = pas::bind_method<&TfPanelMain::FinishMessageDeletion>(this);
                    Animation->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(MessageEntry));
                    Animation->UserIndex = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Sender));
                    Control = MessagePanel->FirstChild;
                    while (Control != nullptr) {
                        Control->UserState = Control->LocalPosition.X;
                        Control = Control->NextSibling;
                    }
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            if (!SkipLock) {
                pas::critical_leave(Globals::PersistentPlayerMessageLock);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return;
            }
        }
    }

    void TfPanelMain::MessageRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        DeleteMessage(Sender, KeyState, Point, false);
        GI_Main::BreakUiMessage();
    }

    void TfPanelMain::AdvanceMessageDeletion(GI_MessageLoop::TObjectGI* Sender) {
        GI_MessageLoop::TObjectGI* DeletedButton = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
        pas::Extended cpp_left = pas::checked_cast<GI_GAI::TgaiGI*>(Sender)->SequenceFrame;
        double Progress = pas::real_divide(cpp_left, pas::checked_cast<GI_GAI::TgaiGI*>(Sender)->SequenceFrameCount);
        GI_MessageLoop::TObjectGI* Control = MessagePanel->FirstChild;
        while (Control != nullptr) {
            if (pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                Control->SetPosition(ClassesImports::Point(Control->UserState, Control->LocalPosition.Y));
            }
            Control = Control->NextSibling;
        }
        if (([&] {
            std::int32_t cpp_left_2 = (Screen->GetByName(u"PM_PanelMsg"_wref.get())->ClientSize.X - 22) / 22;
            return cpp_left_2 < Globals::CountPersistentPlayerMessages();
        }())) {
            Control = DeletedButton->PrevSibling;
            while (Control != nullptr) {
                if (pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                    Control->SetPosition(ClassesImports::Point(Control->LocalPosition.X + System::Round(static_cast<long double>(DeletedButton->ClientSize.X + 2) * Progress), Control->LocalPosition.Y));
                }
                Control = Control->PrevSibling;
            }
        } else {
            Control = DeletedButton->NextSibling;
            while (Control != nullptr) {
                if (pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                    Control->SetPosition(ClassesImports::Point(Control->LocalPosition.X - System::Round(static_cast<long double>(DeletedButton->ClientSize.X + 2) * Progress), Control->LocalPosition.Y));
                }
                Control = Control->NextSibling;
            }
        }
        TfPanelMain::PostMouseMove();
    }

    void TfPanelMain::FinishMessageDeletion(GI_MessageLoop::TObjectGI* Sender) {
        Globals::TMessagePlayer* MessageEntry{};
        GI_MessageLoop::TObjectGI* Control{};
        pas::critical_enter(Globals::PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Globals::PersistentPlayerMessageLock);
            };
            if (!Globals::IsPersistentPlayerMessageQueued(reinterpret_cast<Globals::TMessagePlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue))), true)) {
                return;
            }
            Globals::RemovePersistentPlayerMessage(reinterpret_cast<Globals::TMessagePlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue))), true);
            {
                GI_GAI::TgaiGI* cpp_with = pas::checked_cast<GI_GAI::TgaiGI*>(Sender);
                pas::free(Sender);
            }
            RebuildMessageButtons(true);
            if (pas::list_count(AuxiliaryItems) > 0) {
                MessageEntry = pas::list_at<Globals::TMessagePlayer>(AuxiliaryItems, 0);
                Control = MessagePanel->FirstChild;
                while (Control != nullptr) {
                    if (reinterpret_cast<Globals::TMessagePlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Control->UserValue))) == MessageEntry) {
                        break;
                    }
                    Control = Control->NextSibling;
                }
                if (Control != nullptr) {
                    pas::list_delete(AuxiliaryItems, 0);
                    try {
                        DeleteMessage(Control, 0u, ClassesImports::Point(0, 0), true);
                    } catch (...) {
                    }
                    return;
                }
            }
        }
    }

    void TfPanelMain::SlideMessagesIn() {
        MessageSlideDirection = -1;
        if (MessageSlideTimer != nullptr) {
            Screen->CancelCallbackTimer(MessageSlideTimer);
            MessageSlideTimer = nullptr;
        }
        if (MessagePanel->LocalPosition.Y > MessagePanelRestTop) {
            MessageSlideTimer = Screen->ScheduleCallbackTimer(10, 10, pas::bind_method<&TfPanelMain::AdvanceMessageSlide>(this), 0);
        }
    }

    void TfPanelMain::SlideMessagesOut() {
        MessageSlideDirection = 1;
        if (MessageSlideTimer != nullptr) {
            Screen->CancelCallbackTimer(MessageSlideTimer);
            MessageSlideTimer = nullptr;
        }
        if (MessagePanel->LocalPosition.Y < GR_Main::GameScreenHeight - 5) {
            MessageSlideTimer = Screen->ScheduleCallbackTimer(10, 10, pas::bind_method<&TfPanelMain::AdvanceMessageSlide>(this), 0);
        }
    }

    void TfPanelMain::AdvanceMessageSlide(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (MessageSlideDirection < 0) {
            if (MessagePanel->LocalPosition.Y <= MessagePanelRestTop) {
                if (MessageSlideTimer != nullptr) {
                    Screen->CancelCallbackTimer(MessageSlideTimer);
                    MessageSlideTimer = nullptr;
                }
            } else {
                MessagePanel->SetPosition(ClassesImports::Point(MessagePanel->LocalPosition.X, MessagePanel->LocalPosition.Y + MessageSlideDirection));
            }
        } else if (MessagePanel->LocalPosition.Y >= GR_Main::GameScreenHeight - 5) {
            if (MessageSlideTimer != nullptr) {
                Screen->CancelCallbackTimer(MessageSlideTimer);
                MessageSlideTimer = nullptr;
            }
        } else {
            MessagePanel->SetPosition(ClassesImports::Point(MessagePanel->LocalPosition.X, MessagePanel->LocalPosition.Y + MessageSlideDirection));
        }
    }

    void TfPanelMain::MessageClicked(GI_MessageLoop::TObjectGI* Sender) {
        Globals::TMessagePlayer* MessageEntry{};
        aShip::TShip* Ship{};
        aPlanet::TPlanet* Planet{};
        std::uint32_t ShipId{};
        std::uint32_t PlanetId{};
        std::int32_t Attempts{};
        aEFilm::TEFilmObj* FilmObject{};
        if (GlobalsV::CurrentScreenId != GlobalsV::screenStarMap || Globals::StarMapScreen->Mode != fStarMap::smmOrders && Globals::StarMapScreen->Mode != fStarMap::smmTurnFilm) {
            return;
        }
        pas::critical_enter(Globals::PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Globals::PersistentPlayerMessageLock);
            };
            MessageEntry = reinterpret_cast<Globals::TMessagePlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
            if (!Globals::IsPersistentPlayerMessageQueued(MessageEntry, true)) {
                return;
            }
            Attempts = 0;
            while (Attempts < 3) {
                ShipId = 0u;
                PlanetId = 0u;
                if (DisplayedShipId == 0 && DisplayedPlanetId == 0) {
                    ShipId = MessageEntry->Targets[0].ShipId;
                    if (ShipId < 1) {
                        PlanetId = MessageEntry->Targets[0].PlanetId;
                    }
                    if (ShipId < 1 && PlanetId < 1) {
                        return;
                    }
                } else {
                    if (MessageEntry->Targets[0].ShipId == DisplayedShipId) {
                        ShipId = MessageEntry->Targets[1].ShipId;
                    } else if (MessageEntry->Targets[1].ShipId == DisplayedShipId) {
                        ShipId = MessageEntry->Targets[2].ShipId;
                    } else if (MessageEntry->Targets[2].ShipId == DisplayedShipId) {
                        ShipId = MessageEntry->Targets[0].ShipId;
                    }
                    if (MessageEntry->Targets[0].PlanetId == DisplayedPlanetId) {
                        PlanetId = MessageEntry->Targets[1].PlanetId;
                    } else if (MessageEntry->Targets[1].PlanetId == DisplayedPlanetId) {
                        PlanetId = MessageEntry->Targets[2].PlanetId;
                    } else if (MessageEntry->Targets[2].PlanetId == DisplayedPlanetId) {
                        PlanetId = MessageEntry->Targets[0].PlanetId;
                    }
                    if (ShipId < 1 && PlanetId < 1) {
                        ShipId = MessageEntry->Targets[0].ShipId;
                        if (ShipId < 1) {
                            PlanetId = MessageEntry->Targets[0].PlanetId;
                        }
                    }
                    if (ShipId < 1 && PlanetId < 1) {
                        return;
                    }
                }
                DisplayedShipId = ShipId;
                DisplayedPlanetId = PlanetId;
                if (DisplayedShipId != 0) {
                    if (Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
                        Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(aGalaxy::Galaxy->IdToShip(DisplayedShipId, false)));
                        if (Ship != nullptr && Ship->InNormalSpace() && Ship->CurrentStar == aPlayer::GetPlayer()->CurrentStar) {
                            Globals::StarMapScreen->SetMapCenterManually(EC_Struct::TruncatePointF(Ship->Position));
                            Globals::StarMapScreen->AddMapAnimation(Ship->Position, pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 0);
                            Globals::StarMapScreen->AddMapAnimation(Ship->Position, pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 200);
                            Globals::StarMapScreen->AddMapAnimation(Ship->Position, pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 400);
                            break;
                        }
                    } else {
                        FilmObject = Globals::SecondaryFilm->FindObjectById(u"Ship2"_wref.get(), DisplayedShipId);
                        if (FilmObject != nullptr && FilmObject->SceneObject != nullptr) {
                            Globals::StarMapScreen->SetMapCenterManually(EC_Struct::TruncatePointF(FilmObject->SceneObject->Position));
                            break;
                        }
                    }
                } else if (DisplayedPlanetId != 0) {
                    if (Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
                        Planet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(aGalaxy::Galaxy->IdToPlanet(DisplayedPlanetId, false)));
                        if (Planet != nullptr && Planet->CurrentStar == aPlayer::GetPlayer()->CurrentStar) {
                            Globals::StarMapScreen->SetMapCenterManually(EC_Struct::TruncatePointF(Planet->GetPosition()));
                            {
                                EC_Struct::TPointF position = Planet->GetPosition();
                                pas::WideString cpp_arg = pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"});
                                Globals::StarMapScreen->AddMapAnimation(position, std::move(cpp_arg), 0);
                            }
                            {
                                EC_Struct::TPointF position_2 = Planet->GetPosition();
                                pas::WideString cpp_arg_2 = pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"});
                                Globals::StarMapScreen->AddMapAnimation(position_2, std::move(cpp_arg_2), 200);
                            }
                            {
                                EC_Struct::TPointF position_3 = Planet->GetPosition();
                                pas::WideString cpp_arg_3 = pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"});
                                Globals::StarMapScreen->AddMapAnimation(position_3, std::move(cpp_arg_3), 400);
                            }
                            break;
                        }
                    } else {
                        FilmObject = Globals::SecondaryFilm->FindObjectById(u"Planet"_wref.get(), DisplayedPlanetId);
                        if (FilmObject != nullptr && FilmObject->SceneObject != nullptr) {
                            Globals::StarMapScreen->SetMapCenterManually(EC_Struct::TruncatePointF(FilmObject->SceneObject->Position));
                            break;
                        }
                    }
                }
                ++Attempts;
            }
        }
    }

    void TfPanelMain::PlayUnreadMessageSounds() {
        Globals::TMessagePlayer* MessageEntry{};
        std::uint8_t PlayedNew = false;
        std::uint8_t PlayedLiberation = false;
        std::uint8_t PlayedQuestOk = false;
        std::uint8_t PlayedQuestCancel = false;
        pas::critical_enter(Globals::PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Globals::PersistentPlayerMessageLock);
            };
            MessageEntry = Globals::FirstPersistentPlayerMessage;
            while (MessageEntry != nullptr) {
                if (!MessageEntry->NotificationSoundPlayed) {
                    if (pas::in_set<0, 6, 8, 8>(MessageEntry->Kind)) {
                        if (static_cast<std::uint8_t>(PlayedQuestOk ^ 1) && MessageEntry->Kind == 4) {
                            PlayedQuestOk = true;
                            GR_Main::SoundManager->PlaySound(u"Sound.QuestOk"_wref.get());
                        } else if (static_cast<std::uint8_t>(PlayedQuestCancel ^ 1) && MessageEntry->Kind == 5) {
                            PlayedQuestCancel = true;
                            GR_Main::SoundManager->PlaySound(u"Sound.QuestCancel"_wref.get());
                        } else if (static_cast<std::uint8_t>(PlayedNew ^ 1) && MessageEntry->NotificationSoundKind == 0) {
                            PlayedNew = true;
                            GR_Main::SoundManager->PlaySound(u"Sound.NewMsg"_wref.get());
                        } else if (static_cast<std::uint8_t>(PlayedLiberation ^ 1) && MessageEntry->NotificationSoundKind == 1) {
                            PlayedLiberation = true;
                            GR_Main::SoundManager->PlaySound(u"Sound.LiberationSystem"_wref.get());
                        }
                    }
                }
                MessageEntry = MessageEntry->Next;
            }
            MessageEntry = Globals::FirstPersistentPlayerMessage;
            while (MessageEntry != nullptr) {
                MessageEntry->NotificationSoundPlayed = true;
                MessageEntry = MessageEntry->Next;
            }
        }
    }

    void TfPanelMain::FlashMoneyWarning() {
        if (MoneyWarningTimer != nullptr) {
            Screen->CancelCallbackTimer(MoneyWarningTimer);
            MoneyWarningTimer = nullptr;
        }
        MoneyWarningTimer = Screen->ScheduleCallbackTimer(100, 100, pas::bind_method<&TfPanelMain::AdvanceMoneyWarning>(this), 0);
        MoneyWarningActive = true;
        MoneyWarningTicks = 6;
        RefreshMoneyAndCargo();
    }

    void TfPanelMain::AdvanceMoneyWarning(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        --MoneyWarningTicks;
        if (MoneyWarningTicks <= 0) {
            if (MoneyWarningTimer != nullptr) {
                Screen->CancelCallbackTimer(MoneyWarningTimer);
                MoneyWarningTimer = nullptr;
            }
            MoneyWarningActive = false;
        }
        RefreshMoneyAndCargo();
    }

    void TfPanelMain::FlashCargoWarning() {
        if (CargoWarningTimer != nullptr) {
            Screen->CancelCallbackTimer(CargoWarningTimer);
            CargoWarningTimer = nullptr;
        }
        CargoWarningTimer = Screen->ScheduleCallbackTimer(100, 100, pas::bind_method<&TfPanelMain::AdvanceCargoWarning>(this), 0);
        CargoWarningActive = true;
        CargoWarningTicks = 6;
        RefreshMoneyAndCargo();
    }

    void TfPanelMain::AdvanceCargoWarning(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        --CargoWarningTicks;
        if (CargoWarningTicks <= 0) {
            if (CargoWarningTimer != nullptr) {
                Screen->CancelCallbackTimer(CargoWarningTimer);
                CargoWarningTimer = nullptr;
            }
            CargoWarningActive = false;
        }
        RefreshMoneyAndCargo();
    }

    void TfPanelMain::ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible) {
        GI_Label::TLabelGI* LabelControl = HelpLabel;
        if (Sender == nullptr || Sender->HelpText == u"") {
            Visible = false;
        }
        if (Visible) {
            SlideMessagesOut();
        } else {
            SlideMessagesIn();
        }
        LabelControl->SetActive(Visible);
        if (Sender == nullptr) {
            LabelControl->SetText(u""_wref.get());
        } else {
            LabelControl->SetText(Sender->HelpText);
        }
    }

    void TfPanelMain::ShowHelpText(pas::WideString Text, std::uint8_t Visible) {
        GI_Label::TLabelGI* LabelControl = HelpLabel;
        if (Visible) {
            SlideMessagesOut();
        } else {
            SlideMessagesIn();
        }
        LabelControl->SetActive(Visible);
        LabelControl->SetText(Text);
    }

    void TfPanelMain::ProcessKeyDown(std::uint32_t Key) {
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (NavigationLocked) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aCalc::IsTurnCalculationRunningUI()) {
            return;
        }
        if (Key == WindowsSdk::VK_F2) {
            if (aGalaxy::Galaxy->IronWill) {
                const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"FormGameSet2.IronWillText"_wref.get());
                GI_MessageLoop::TMessageLoopGI* screen = Screen;
                GI_MessageBox::ShowMessageBoxGI(screen, localizedColorText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
            } else if (aGalaxy::Galaxy->SpecialSimulationMode == 0) {
                GR_Main::CaptureSavePreview();
                aGalaxy::Galaxy->CheckIntegrityChecksum(100);
                fGalaxy2::CaptureGalaxyPreview(Screen);
                aGalaxy::Galaxy->PrimeIntegrityChecksum(101);
                GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(Screen);
                Globals::SaveManagerMode = fSaveManager::smmSave;
                GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
                Screen->RequestClose(1);
            }
        } else if (Key == WindowsSdk::VK_F3) {
            GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(Screen);
            Globals::SaveManagerMode = fSaveManager::smmLoad;
            GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
            Screen->RequestClose(1);
        } else if (Key == WindowsSdk::VK_SPACE) {
            EndTurnClicked(nullptr);
        } else if (Key == 'M') {
            GalaxyClicked(nullptr);
        } else if (Key == 'S') {
            ShipClicked(nullptr);
        } else if (Key == 'R') {
            QuestClicked(nullptr);
        } else if (Key == WindowsSdk::VK_F1) {
            JournalClicked(nullptr);
        } else if (Key == WindowsSdk::VK_ESCAPE) {
            MenuClicked(nullptr);
        } else if (Key == WindowsSdk::VK_F5) {
            QuickSave();
        } else if (Key == WindowsSdk::VK_F6) {
            QuickLoad(3);
        } else if (Key == WindowsSdk::VK_F7) {
            QuickLoad(2);
        } else if (Key == WindowsSdk::VK_F8) {
            QuickLoad(1);
        } else if (Key == WindowsSdk::VK_F11) {
            if (!RemoveDismissibleMessages(u"GOODS"_w)) {
                RemoveDismissibleMessages(pas::WideString());
            }
        }
    }

    void TfPanelMain::PostMouseMove() {
        WindowsSdk::TPoint Point{};
        WindowsSdk::GetCursorPos(Point);
        WindowsSdk::ScreenToClient(GR_Main::MainWindowHandle, Point);
        WindowsSdk::PostMessage(GR_Main::MainWindowHandle, MessagesSdk::WM_MOUSEMOVE, 0, static_cast<std::int16_t>(Point.X) | pas::shl(static_cast<std::int32_t>(static_cast<std::int16_t>(Point.Y)), 16));
    }

    void TMessageLoopGIWithMainPanel_Create(TMessageLoopGIWithMainPanel* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->MainPanel = pas::construct_call<TfPanelMain>(TfPanelMain_Create);
    }

    void TMessageLoopGIWithMainPanel_Destroy(TMessageLoopGIWithMainPanel* Self) {
        if (Self->MainPanel != nullptr) {
            pas::free(Self->MainPanel);
            Self->MainPanel = nullptr;
        }
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfPanelMain::p_destroy() {
        fPanelMain::TfPanelMain_Destroy(this);
    }

    void TMessageLoopGIWithMainPanel::p_destroy() {
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(this);
    }

} // namespace fPanelMain
