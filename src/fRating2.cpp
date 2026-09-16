#include "layout/fRating2.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aNormalShip.hpp"
#include "types/aPirate.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "types/aShip.hpp"
#include "types/fShip2.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/fPanelMain.hpp"
#include "units/fRating2.hpp"
#include "units/fRewards.hpp"

namespace fRating2 {
    // Native calls pass only columns 1, 2 or 3.
    const pas::Array<std::int32_t, 1, 3> CareerHintColumns = pas::Array<std::int32_t, 1, 3>{{10, 75, 105}};

    const pas::Array<std::int32_t, 1, 3> DominatorHintColumns = pas::Array<std::int32_t, 1, 3>{{10, 75, 95}};

    // Runs the registered rating screen modally; true for normal close.
    std::uint8_t ShowRangerRating(GI_MessageLoop::TMessageLoopGI* Parent) {
        std::uint8_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        Parent->RootUiObject->NativeHook50();
        Parent->CaptureCursorState(&State);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        Globals::RangerRatingScreen->ParentLoop = Parent;
        Parent->ChildLoop = Globals::RangerRatingScreen;
        if (Globals::RangerRatingScreen->Run() == 1) {
            Result = true;
        } else {
            Result = false;
        }
        Globals::RangerRatingScreen->ParentLoop = nullptr;
        Parent->ChildLoop = nullptr;
        Parent->InvalidateViewport();
        Parent->RestoreCursorState(&State);
        Parent->UpdateCursorPosition();
        Parent->RootUiObject->NativeHook48();
        Parent->Present();
        GR_Main::PostMouseMoveMessage();
        return Result;
    }

    void TfRating2_Create(TfRating2* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
    }

    void TfRating2_Destroy(TfRating2* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfRating2::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* ButClose_Parent = MainPanel->FindByNameRecursive(u"ButClose"_wref.get())->Parent;
                ButClose_Parent->SetPosition(ClassesImports::Point(ButClose_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, ButClose_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
        RewardWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"RewardWnd"_wref.get()));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"_wref.get()))->UpCallback = pas::bind_method<&TfRating2::CloseClicked>(this);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfRating2::KeyDown>(this);
        GetByName(u"MainPanel"_wref.get())->LeftButtonDownCallback = pas::bind_method<&TfRating2::BackgroundMouseDown>(this);
        GetByName(u"MainPanel"_wref.get())->LeftButtonUpCallback = pas::bind_method<&TfRating2::BackgroundMouseUp>(this);
        TablePanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PTable"_wref.get()));
        TablePanel->VerticalScrollBar->SetPageSize(TablePanel->ClientSize.Y);
        TablePanel->VerticalScrollBar->SetLargeChange(TablePanel->ClientSize.Y);
    }

    void TfRating2::OnOpen() {
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(true, 0);
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
        }
        MainPanel->OnOpen();
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Gal"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Quest"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Logo"_wref.get()))->SetHitTestDisabled(true);
        BackgroundClickStarted = true;
        SortColumn = rrscExperience;
        SelectedIndex = -1;
        SelectedRangerId = aPlayer::GetPlayer()->Id;
        {
            GI_MessageLoop::TObjectGI* SortName = GetByName(u"SortName"_wref.get());
            SortName->LeftButtonUpCallback = pas::bind_method<&TfRating2::SortHeaderMouseUp>(this);
            SortName->UserValue = 1;
        }
        {
            GI_MessageLoop::TObjectGI* SortCharacter = GetByName(u"SortCharacter"_wref.get());
            SortCharacter->UserValue = 5;
        }
        {
            GI_MessageLoop::TObjectGI* SortRace = GetByName(u"SortRace"_wref.get());
            SortRace->LeftButtonUpCallback = pas::bind_method<&TfRating2::SortHeaderMouseUp>(this);
            SortRace->UserValue = 3;
        }
        {
            GI_MessageLoop::TObjectGI* SortRank = GetByName(u"SortRank"_wref.get());
            SortRank->LeftButtonUpCallback = pas::bind_method<&TfRating2::SortHeaderMouseUp>(this);
            SortRank->UserValue = 4;
        }
        {
            GI_MessageLoop::TObjectGI* SortScore = GetByName(u"SortScore"_wref.get());
            SortScore->LeftButtonUpCallback = pas::bind_method<&TfRating2::SortHeaderMouseUp>(this);
            SortScore->UserValue = 2;
        }
        HideHint();
        RefreshFeaturedRangers();
        RebuildTable();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(1113);
        MainPanel->RebuildMessageButtons(false);
    }

    void TfRating2::OnClose() {
        GI_MessageLoop::TMessageLoopGI::OnClose();
        aGalaxy::Galaxy->CheckIntegrityChecksum(1114);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        ClearRows();
        MainPanel->OnClose();
    }

    void TfRating2::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        RequestClose(1);
    }

    void TfRating2::KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Key == 'R') {
                CloseClicked(nullptr);
            } else if (Key == WindowsSdk::VK_UP || Key == WindowsSdk::VK_LEFT) {
                if (SelectedIndex > 0) {
                    SelectRow(SelectedIndex - 1);
                }
            } else if (Key == WindowsSdk::VK_DOWN || Key == WindowsSdk::VK_RIGHT) {
                if (SelectedIndex < Rows.length() - 1) {
                    SelectRow(SelectedIndex + 1);
                }
            } else if (Key == WindowsSdk::VK_HOME) {
                SelectRow(0);
            } else if (Key == WindowsSdk::VK_END) {
                SelectRow(Rows.length() - 1);
            } else if (Key == WindowsSdk::VK_PRIOR) {
                TablePanel->VerticalScrollBar->SetPosition_2(TablePanel->VerticalScrollBar->Position - TablePanel->VerticalScrollBar->LargeChange);
            } else if (Key == WindowsSdk::VK_NEXT) {
                TablePanel->VerticalScrollBar->SetPosition_2(TablePanel->VerticalScrollBar->Position + TablePanel->VerticalScrollBar->LargeChange);
            } else if (Key == WindowsSdk::VK_ESCAPE) {
                RequestClose(1);
            } else if (Key == WindowsSdk::VK_F11) {
                if (!MainPanel->RemoveDismissibleMessages(u"GOODS"_w)) {
                    MainPanel->RemoveDismissibleMessages(pas::WideString());
                }
            }
        }
    }

    void TfRating2::BackgroundMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (GlobalsV::ClickAutoCloseForm) {
            BackgroundClickStarted = false;
            if (static_cast<std::uint8_t>(pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImagePanel"_wref.get()))->ContainsPoint(Point) ^ 1) && static_cast<std::uint8_t>(GetByName(u"PM_PanelMsg"_wref.get())->ContainsPoint(Point) ^ 1)) {
                BackgroundClickStarted = true;
            }
        }
    }

    void TfRating2::BackgroundMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (GlobalsV::ClickAutoCloseForm) {
            if (static_cast<std::uint8_t>(pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImagePanel"_wref.get()))->ContainsPoint(Point) ^ 1) && static_cast<std::uint8_t>(GetByName(u"PM_PanelMsg"_wref.get())->ContainsPoint(Point) ^ 1) && BackgroundClickStarted) {
                CloseClicked(nullptr);
            }
        }
    }

    void TfRating2::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            TablePanel->VerticalScrollBar->SetPosition_2(TablePanel->VerticalScrollBar->Position - TablePanel->VerticalScrollBar->SmallChange);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            TablePanel->VerticalScrollBar->SetPosition_2(TablePanel->VerticalScrollBar->Position + TablePanel->VerticalScrollBar->SmallChange);
        }
    }

    void TfRating2::AwardsMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (GlobalsV::UiRuntimeFlag) {
            RewardWindow->SetActive(false);
            Globals::AwardSubject = static_cast<pas::Object*>(aGalaxy::Galaxy->IdToShip(Sender->UserValue, true));
            if (Globals::AwardSubject != nullptr) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(335);
                if (!fRewards::RunRewards(this, true)) {
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(336);
                    RequestClose(2);
                } else {
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(336);
                }
            }
        }
    }

    void TfRating2::AwardsMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::uint8_t AwardId{};
        std::int32_t Size = GR_Main::GiScalePixels(20);
        std::int32_t Step = Size / 2;
        aRanger::TRanger* Ranger = Rows[Sender->UserState].Ranger;
        std::int32_t Index = std::max<std::int32_t>(0, pas::list_count(Ranger->AwardIds) - 8) + pas::idiv(Sender->ToLocalPoint(Point).X, Step);
        if (Sender->ToLocalPoint(Point).Y < Sender->ClientSize.Y - Size) {
            HideHint();
        } else {
            if (Index >= pas::list_count(Ranger->AwardIds)) {
                Index = pas::list_count(Ranger->AwardIds) - 1;
            }
            AwardId = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ranger->AwardIds, Index)));
            ShowAwardHint(Ranger, AwardId);
        }
    }

    void TfRating2::HintMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        HideHint();
    }

    void TfRating2::ShowPartnershipHint(GI_MessageLoop::TObjectGI* Sender) {
        WindowsSdk::TPoint Cursor{};
        std::int32_t I{};
        pas::WideString Text{};
        pas::WideString PartnerInfo{};
        pas::WideString Names{};
        aRanger::TRanger* Ally{};
        aPirate::TPirate* Pirate{};
        // Nested in TfRating2.ShowPartnershipHint; caller supplies its parent frame.
        auto FormatRangerWingmenHint = [&](aRanger::TRanger* Ranger) -> pas::WideString {
            pas::WideString Result{};
            std::int32_t I{};
            pas::WideString Names{};
            aRanger::TRanger* Ally{};
            if (Ranger->CountWingmen() == 1) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
                    Ally = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                    if (aPlayer::GetPlayer() != Ally && Ally->PartnerShip == Ranger) {
                        Result = ([&] {
                            pas::WideString name = Ally->GetName();
                            pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + Ally->PartnershipDaysRemaining);
                            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRating.PartnerBossOneText"_wref.get());
                            return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name), u"<Date>"_w, std::move(formatTurnDate));
                        }());
                    }
                }
                return Result;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range_2.next(I); ) {
                Ally = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                if (aPlayer::GetPlayer() != Ally && Ally->PartnerShip == Ranger) {
                    if (Names != u"") {
                        Names = pas::concat_wide({Names, u", ", ([&] {
                            pas::WideString name_2 = Ally->GetName();
                            pas::WideString formatTurnDate_2 = aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + Ally->PartnershipDaysRemaining);
                            pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRating.AddInfoAboutPartner"_wref.get());
                            return aMyFunction::FormatText2(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_2), u"<Date>"_w, std::move(formatTurnDate_2));
                        }())});
                    } else {
                        Names = ([&] {
                            pas::WideString name_3 = Ally->GetName();
                            pas::WideString formatTurnDate_3 = aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + Ally->PartnershipDaysRemaining);
                            pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRating.AddInfoAboutPartner"_wref.get());
                            return aMyFunction::FormatText2(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_3), u"<Date>"_w, std::move(formatTurnDate_3));
                        }());
                    }
                }
            }
            return aMyFunction::FormatText1(aConst::LocalizedColorText(u"FormRating.PartnerBossManyText"_wref.get()), pas::WideString(), u"<Names>"_w, Names);
        };
        RewardWindow->SetActive(true);
        if (aPlayer::GetPlayer() == Rows[Sender->UserValue].Ranger) {
            {
                GI_Label::TLabelGI* RewardName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName->SetText(([&] {
                    pas::WideString name = aPlayer::GetPlayer()->GetName();
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRating.PlayerName"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name));
                }()));
            }
            if (aPlayer::GetPlayer()->CountWingmen() > 0) {
                Names = pas::WideString();
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
                    Ally = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                    if (aPlayer::GetPlayer() != Ally && aPlayer::GetPlayer() == Ally->PartnerShip) {
                        if (Names != u"") {
                            Names = pas::concat_wide({Names, u", ", ([&] {
                                pas::WideString name_2 = Ally->GetName();
                                pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + Ally->PartnershipDaysRemaining);
                                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRating.AddInfoAboutPartner"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_2), u"<Date>"_w, std::move(formatTurnDate));
                            }())});
                        } else {
                            Names = ([&] {
                                pas::WideString name_3 = Ally->GetName();
                                pas::WideString formatTurnDate_2 = aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + Ally->PartnershipDaysRemaining);
                                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRating.AddInfoAboutPartner"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_3), u"<Date>"_w, std::move(formatTurnDate_2));
                            }());
                        }
                    }
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->PiratePartners) - 1); cpp_range_2.next(I); ) {
                    Pirate = pas::list_at<aPirate::TPirate>(aPlayer::GetPlayer()->PiratePartners, I);
                    if (Names != u"") {
                        Names = pas::concat_wide({Names, u", ", ([&] {
                            pas::WideString name_4 = Pirate->GetName();
                            pas::WideString formatTurnDate_3 = aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + Pirate->PartnershipDaysRemaining);
                            pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormRating.AddInfoAboutPirate"_wref.get());
                            return aMyFunction::FormatText2(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_4), u"<Date>"_w, std::move(formatTurnDate_3));
                        }())});
                    } else {
                        Names = ([&] {
                            pas::WideString name_5 = Pirate->GetName();
                            pas::WideString formatTurnDate_4 = aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + Pirate->PartnershipDaysRemaining);
                            pas::WideString localizedColorText_5 = aConst::LocalizedColorText(u"FormRating.AddInfoAboutPirate"_wref.get());
                            return aMyFunction::FormatText2(std::move(localizedColorText_5), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_5), u"<Date>"_w, std::move(formatTurnDate_4));
                        }());
                    }
                }
                PartnerInfo = aMyFunction::FormatText1(aConst::LocalizedColorText(u"FormRating.PlayerPartnerOk"_wref.get()), pas::WideString(), u"<Names>"_w, Names);
            } else {
                PartnerInfo = aConst::LocalizedColorText(u"FormRating.PlayerPartnerNo"_wref.get());
            }
            Text = aConst::LocalizedColorText(u"FormRating.PlayerText"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<PartnerInfo>"_w, PartnerInfo, pas::WideString());
            {
                GI_Label::TLabelGI* RewardText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                RewardText->SetText(Text);
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"PlayerB"}), RewardImage->GraphBuf);
            }
        } else if (Rows[Sender->UserValue].Ranger->IsInPrison()) {
            {
                GI_Label::TLabelGI* RewardName_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName_2->SetText(([&] {
                    pas::WideString name_6 = Rows[Sender->UserValue].Ranger->GetName();
                    pas::WideString localizedColorText_6 = aConst::LocalizedColorText(u"FormRating.InPrisonName"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText_6), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_6));
                }()));
            }
            if (Rows[Sender->UserValue].Ranger->CurrentPlanet != nullptr) {
                Text = ([&] {
                    auto name_7 = pas::borrow(Rows[Sender->UserValue].Ranger->CurrentPlanet->Name);
                    pas::WideString localizedColorText_7 = aConst::LocalizedColorText(u"FormRating.InPrisonText"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText_7), u"<color=255,240,100>"_w, u"<Planet>"_w, name_7.get());
                }());
            } else {
                Text = pas::WideString();
            }
            {
                GI_Label::TLabelGI* RewardText_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                RewardText_2->SetText(Text);
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"PrisonB"}), RewardImage_2->GraphBuf);
            }
        } else if (Rows[Sender->UserValue].Ranger->PartnerShip != nullptr) {
            {
                GI_Label::TLabelGI* RewardName_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName_3->SetText(([&] {
                    pas::WideString name_8 = Rows[Sender->UserValue].Ranger->GetName();
                    pas::WideString localizedColorText_8 = aConst::LocalizedColorText(u"FormRating.PartnerName"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText_8), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_8));
                }()));
            }
            Text = aConst::LocalizedColorText(u"FormRating.PartnerText"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, Rows[Sender->UserValue].Ranger->PartnerShip->GetName(), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + Rows[Sender->UserValue].Ranger->PartnershipDaysRemaining), u"<color=255,240,100>"_w);
            if (Rows[Sender->UserValue].Ranger->PartnershipDaysRemaining == 0) {
                Text = pas::concat_wide({Text, u" ", aConst::LocalizedColorText(u"FormRating.PartnerTextDateEnd"_wref.get())});
            }
            if (Rows[Sender->UserValue].Ranger->CountWingmen() > 0) {
                Text = pas::concat_wide({Text, u"\r\n", FormatRangerWingmenHint(Rows[Sender->UserValue].Ranger)});
            }
            {
                GI_Label::TLabelGI* RewardText_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                RewardText_3->SetText(Text);
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage_3 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"DutyB"}), RewardImage_3->GraphBuf);
            }
        } else if (Rows[Sender->UserValue].Ranger->CountWingmen() > 0) {
            {
                GI_Label::TLabelGI* RewardName_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName_4->SetText(([&] {
                    pas::WideString name_9 = Rows[Sender->UserValue].Ranger->GetName();
                    pas::WideString localizedColorText_9 = aConst::LocalizedColorText(u"FormRating.PartnerName"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText_9), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_9));
                }()));
            }
            Text = FormatRangerWingmenHint(Rows[Sender->UserValue].Ranger);
            {
                GI_Label::TLabelGI* RewardText_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                RewardText_4->SetText(Text);
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage_4 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"DutyB"}), RewardImage_4->GraphBuf);
            }
        }
        {
            GI_GraphBuf::TGraphBufGI* RewardImage_5 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
            RewardImage_5->SourceHasPerPixelAlpha = true;
            RewardImage_5->SetImageKindX(GI_Main::ikxCenter);
            RewardImage_5->SetImageKindY(GI_Main::ikyCenter);
        }
        {
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
            fShip2::TfShip2::LayoutItemInfo(RewardWindow, cpp_arg_2, cpp_arg, true, true, 0);
        }
        {
            GI_Label::TLabelGI* RewardName_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
            RewardName_5->SetSize(ClassesImports::Point(RewardWindow->ClientSize.X - RewardName_5->LocalPosition.X - RewardWindow->WorkSubRect.Right, RewardName_5->ClientSize.Y));
        }
        Cursor = GetCursorPoint();
        RewardWindow->SetPosition(ClassesImports::Point(Cursor.X + 100, std::max<std::int32_t>(0, Cursor.Y - RewardWindow->ClientSize.Y - 20)));
    }

    void TfRating2::ShowCareerHint(GI_MessageLoop::TObjectGI* Sender) {
        std::uint8_t I{};
        WindowsSdk::TPoint Cursor{};
        pas::WideString Text{};
        aRanger::TRanger* Ranger{};
        // Nested in TfRating2.ShowCareerHint; caller supplies its parent frame.
        auto FormatCareerHintColumn = [&](std::int32_t Column) -> pas::WideString {
            return pas::wide_int_to_str(CareerHintColumns[Column] + 40);
        };
        RewardWindow->SetActive(true);
        Ranger = Rows[Sender->UserValue].Ranger;
        if (aPlayer::GetPlayer() == Ranger) {
            {
                GI_Label::TLabelGI* RewardName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName->SetText(([&] {
                    pas::WideString name = Ranger->GetName();
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRating.PlayerName"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name));
                }()));
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"PlayerB"}), RewardImage->GraphBuf);
            }
        } else {
            {
                GI_Label::TLabelGI* RewardName_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName_2->SetText(([&] {
                    pas::WideString name_2 = Ranger->GetName();
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRating.PartnerName"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_2));
                }()));
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"DutyB"}), RewardImage_2->GraphBuf);
            }
        }
        Text = pas::concat_wide({u"<td=", FormatCareerHintColumn(1), u"><align=left>", aConst::LocalizedColorText(u"FormRating.Rating.Title"_wref.get()), u"</align>", u"\r\n"});
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(2)); cpp_range.next(I); ) {
            Text = pas::concat_wide({Text, u"<td=", FormatCareerHintColumn(1), u"><align=left>", aConst::LocalizedColorText(pas::concat_wide({u"FormRating.Rating.", aConst::CareerTuning[I].Name})), u"<td=", FormatCareerHintColumn(2), u">:</align><td=", FormatCareerHintColumn(3), u"><align=right>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(static_cast<std::int32_t>(Ranger->CareerStatus[I])), u"<color=255,240,100>"_w), u"</align>", u"\r\n"});
        }
        {
            GI_Label::TLabelGI* RewardText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
            RewardText->SetText(Text);
        }
        {
            GI_GraphBuf::TGraphBufGI* RewardImage_3 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
            RewardImage_3->SourceHasPerPixelAlpha = true;
            RewardImage_3->SetImageKindX(GI_Main::ikxCenter);
            RewardImage_3->SetImageKindY(GI_Main::ikyCenter);
        }
        {
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
            GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
            fShip2::TfShip2::LayoutItemInfo(RewardWindow, cpp_arg_2, cpp_arg, true, true, 0);
        }
        {
            GI_Label::TLabelGI* RewardText_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
            RewardText_2->SetTextAlignX(GI_Main::taxLeft);
        }
        {
            GI_Label::TLabelGI* RewardName_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
            RewardName_3->SetSize(ClassesImports::Point(RewardWindow->ClientSize.X - RewardName_3->LocalPosition.X - RewardWindow->WorkSubRect.Right, RewardName_3->ClientSize.Y));
        }
        Cursor = GetCursorPoint();
        RewardWindow->SetPosition(ClassesImports::Point(Cursor.X + 100, std::max<std::int32_t>(0, Cursor.Y - RewardWindow->ClientSize.Y - 20)));
    }

    void TfRating2::ShowAwardHint(aRanger::TRanger* Ranger, std::int32_t AwardId) {
        aConst::TRewardInfo cpp_result{};
        aConst::TRewardInfo cpp_result_2{};
        WindowsSdk::TPoint Cursor{};
        pas::WideString Path{};
        if (HoveredAwardId != AwardId) {
            HoveredAwardId = AwardId;
            RewardWindow->SetActive(true);
            Cursor = GetCursorPoint();
            RewardWindow->SetPosition(ClassesImports::Point(Cursor.X + 100, std::max<std::int32_t>(0, Cursor.Y - RewardWindow->ClientSize.Y - 20)));
            if (AwardId < 10) {
                Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_0", pas::wide_int_to_str(AwardId)});
            } else {
                Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_", pas::wide_int_to_str(AwardId)});
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                RewardImage->SourceHasPerPixelAlpha = true;
                GI_GI::LoadGiByPathIntoGraphBuf(Path, RewardImage->GraphBuf);
                if (static_cast<std::uint32_t>(RewardImage->GraphBuf->Width) >= static_cast<std::uint32_t>(RewardImage->GraphBuf->Height)) {
                    RewardImage->GraphBuf->RescaleRgba(RewardImage->ClientSize.X, System::Round(pas::real_divide(RewardImage->ClientSize.X, static_cast<std::uint32_t>(RewardImage->GraphBuf->Width)) * static_cast<std::uint32_t>(RewardImage->GraphBuf->Height)), 5);
                } else {
                    RewardImage->GraphBuf->RescaleRgba(System::Round(pas::real_divide(RewardImage->ClientSize.Y, static_cast<std::uint32_t>(RewardImage->GraphBuf->Height)) * static_cast<std::uint32_t>(RewardImage->GraphBuf->Width)), RewardImage->ClientSize.Y, 5);
                }
                RewardImage->SetImageKindX(GI_Main::ikxCenter);
                RewardImage->SetImageKindY(GI_Main::ikyCenter);
            }
            {
                GI_Label::TLabelGI* RewardName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName->SetText((aNormalShip::TNormalShip::GetAwardInfo(AwardId, cpp_result), cpp_result).Name);
            }
            {
                GI_Label::TLabelGI* RewardText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                RewardText->SetText((aNormalShip::TNormalShip::GetAwardInfo(AwardId, cpp_result_2), cpp_result_2).Text);
            }
            {
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                fShip2::TfShip2::LayoutItemInfo(RewardWindow, cpp_arg_2, cpp_arg, true, true, 0);
            }
            {
                GI_Label::TLabelGI* RewardName_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName_2->SetSize(ClassesImports::Point(RewardWindow->ClientSize.X - RewardName_2->LocalPosition.X - RewardWindow->WorkSubRect.Right, RewardName_2->ClientSize.Y));
            }
        }
    }

    void TfRating2::HideHint() {
        HoveredAwardId = -1;
        RewardWindow->SetActive(false);
    }

    // Returns -1 if the ranger is absent.
    std::int32_t TfRating2::FindRowByRangerId(std::int32_t RangerId) {
        std::int32_t I{};
        {
            const std::int32_t cpp_last = Rows.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (Rows[I].Ranger->Id == RangerId) {
                        return I;
                    }
                }
            }
        }
        return -1;
    }

    void TfRating2::SortHeaderMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (SelectedIndex >= 0) {
            SelectedRangerId = Rows[SelectedIndex].Ranger->Id;
            SelectedIndex = -1;
        }
        TRangerRatingSortColumn Column = static_cast<TRangerRatingSortColumn>(Sender->UserValue);
        if (Column == SortColumn) {
            SortAscending = static_cast<std::uint8_t>(SortAscending ^ 1);
        } else {
            SortColumn = Column;
            SortAscending = SortColumn == rrscName || SortColumn == rrscRace;
        }
        RebuildTable();
    }

    void TfRating2::FeaturedRangerClicked(GI_MessageLoop::TObjectGI* Sender) {
        SelectRow(-1);
        SelectRow(FindRowByRangerId(Sender->UserValue));
        GR_Main::PostMouseMoveMessage();
    }

    void TfRating2::RowMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (!Sender->IsOccludedAtPoint(GetCursorPoint())) {
            if (SelectedIndex != Sender->UserState) {
                GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
                SelectRow(Sender->UserState);
            }
            GR_Main::PostMouseMoveMessage();
            GI_Main::BreakUiMessage();
        }
    }

    // Accepts -1; other indices must be valid. Rebuilds the old and new rows because selection changes row height.
    void TfRating2::SelectRow(std::int32_t Index) {
        std::int32_t Previous{};
        if (SelectedIndex != Index) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(1115);
            Previous = SelectedIndex;
            SelectedIndex = Index;
            if (Previous >= 0) {
                RebuildRow(Previous);
            }
            if (SelectedIndex >= 0) {
                RebuildRow(SelectedIndex);
            }
            if (Index >= 0) {
                SelectedRowRect = ClassesImports::Rect(0, Rows[Index].Top, 1, Rows[Index].Top + Rows[Index].Height);
                TablePanel->ScrollRectIntoView(SelectedRowRect);
            }
            HideHint();
            aGalaxy::Galaxy->PrimeIntegrityChecksum(1116);
        }
    }

    void TfRating2::ClearRows() {
        TablePanel->FreeOwnedChildren();
        GetByName(u"MainPanel"_wref.get())->Invalidate();
        Rows = nullptr;
        SelectedRowRect = ClassesImports::Rect(0, 0, 1, 1);
        TablePanel->VerticalScrollBar->SetSmallChange(TablePanel->VerticalScrollBar->LargeChange);
    }

    // Excludes ExcludedFromRating rangers; preserves selection by ID when SelectedIndex is -1.
    void TfRating2::RebuildTable() {
        pas::AnsiString cpp_text{};
        pas::AnsiString cpp_text_2{};
        pas::AnsiString cpp_text_3{};
        pas::AnsiString cpp_text_4{};
        pas::AnsiString cpp_text_5{};
        pas::AnsiString cpp_text_6{};
        pas::AnsiString cpp_text_7{};
        pas::AnsiString cpp_text_8{};
        std::int32_t I{};
        std::int32_t J{};
        aRanger::TRanger* A{};
        aRanger::TRanger* B{};
        ClearRows();
        {
            GI_Image::TImageGI* SortImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"SortImage"_wref.get()));
            if (SortColumn == rrscName) {
                SortImage->SetPosition(ClassesImports::Point(([&] {
                    std::int32_t cpp_left = GetByName(u"SortName"_wref.get())->LocalPosition.X;
                    return cpp_left + GetByName(u"SortName"_wref.get())->ClientSize.X;
                }()) - SortImage->ClientSize.X, SortImage->LocalPosition.Y));
            } else if (SortColumn == rrscExperience) {
                SortImage->SetPosition(ClassesImports::Point(([&] {
                    std::int32_t cpp_left_2 = GetByName(u"SortScore"_wref.get())->LocalPosition.X;
                    return cpp_left_2 + GetByName(u"SortScore"_wref.get())->ClientSize.X;
                }()) - SortImage->ClientSize.X, SortImage->LocalPosition.Y));
            } else if (SortColumn == rrscRace) {
                SortImage->SetPosition(ClassesImports::Point(([&] {
                    std::int32_t cpp_left_3 = GetByName(u"SortRace"_wref.get())->LocalPosition.X;
                    return cpp_left_3 + GetByName(u"SortRace"_wref.get())->ClientSize.X;
                }()) - SortImage->ClientSize.X, SortImage->LocalPosition.Y));
            } else if (SortColumn == rrscRank) {
                SortImage->SetPosition(ClassesImports::Point(([&] {
                    std::int32_t cpp_left_4 = GetByName(u"SortRank"_wref.get())->LocalPosition.X;
                    return cpp_left_4 + GetByName(u"SortRank"_wref.get())->ClientSize.X;
                }()) - SortImage->ClientSize.X, SortImage->LocalPosition.Y));
            }
            if (SortAscending) {
                SortImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"SortDown"}));
            } else {
                SortImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"SortUp"}));
            }
        }
        pas::List* List = pas::make_object<pas::List>();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
            A = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
            if (!A->ExcludedFromRating) {
                pas::list_add(List, reinterpret_cast<void*>(A));
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(List) - 2); cpp_range_2.next(I); ) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(I + 1, pas::list_count(List) - 1); cpp_range_3.next(J); ) {
                A = pas::list_at<aRanger::TRanger>(List, I);
                B = pas::list_at<aRanger::TRanger>(List, J);
                if (SortColumn == rrscName) {
                    if (SortAscending) {
                        if (([&] {
                            std::uint8_t* cpp_arg = (cpp_text_2 = static_cast<pas::AnsiString>(A->Name), cpp_text_2.pchar());
                            std::uint8_t* cpp_arg_2 = (cpp_text = static_cast<pas::AnsiString>(B->Name), cpp_text.pchar());
                            return WindowsSdk::CompareString(WindowsSdk::LOCALE_USER_DEFAULT, 0u, cpp_arg_2, -1, cpp_arg, -1);
                        }()) - 2 < 0) {
                            pas::list_exchange(List, I, J);
                        }
                    } else if (([&] {
                        std::uint8_t* cpp_arg_3 = (cpp_text_4 = static_cast<pas::AnsiString>(A->Name), cpp_text_4.pchar());
                        std::uint8_t* cpp_arg_4 = (cpp_text_3 = static_cast<pas::AnsiString>(B->Name), cpp_text_3.pchar());
                        return WindowsSdk::CompareString(WindowsSdk::LOCALE_USER_DEFAULT, 0u, cpp_arg_4, -1, cpp_arg_3, -1);
                    }()) - 2 > 0) {
                        pas::list_exchange(List, I, J);
                    }
                } else if (SortColumn == rrscExperience) {
                    if (SortAscending) {
                        if (B->TotalExperience < A->TotalExperience) {
                            pas::list_exchange(List, I, J);
                        }
                    } else if (B->TotalExperience > A->TotalExperience) {
                        pas::list_exchange(List, I, J);
                    }
                } else if (SortColumn == rrscRace) {
                    if (SortAscending) {
                        if (static_cast<std::int8_t>(B->PilotRace + 0) < static_cast<std::int8_t>(A->PilotRace + 0)) {
                            pas::list_exchange(List, I, J);
                        }
                    } else if (static_cast<std::int8_t>(B->PilotRace + 0) > static_cast<std::int8_t>(A->PilotRace + 0)) {
                        pas::list_exchange(List, I, J);
                    }
                } else if (SortColumn == rrscRank) {
                    if (SortAscending) {
                        if (static_cast<std::int8_t>(B->Rank + 0) < static_cast<std::int8_t>(A->Rank + 0)) {
                            pas::list_exchange(List, I, J);
                        }
                    } else if (static_cast<std::int8_t>(B->Rank + 0) > static_cast<std::int8_t>(A->Rank + 0)) {
                        pas::list_exchange(List, I, J);
                    }
                } else if (SortColumn == rrscCharacter) {
                    if (SortAscending) {
                        if (([&] {
                            std::uint8_t* cpp_arg_5 = (cpp_text_6 = static_cast<pas::AnsiString>(A->GetCharacterName()), cpp_text_6.pchar());
                            std::uint8_t* cpp_arg_6 = (cpp_text_5 = static_cast<pas::AnsiString>(B->GetCharacterName()), cpp_text_5.pchar());
                            return SysUtilsImports::AnsiStrComp(cpp_arg_6, cpp_arg_5);
                        }()) < 0) {
                            pas::list_exchange(List, I, J);
                        }
                    } else if (([&] {
                        std::uint8_t* cpp_arg_7 = (cpp_text_8 = static_cast<pas::AnsiString>(A->GetCharacterName()), cpp_text_8.pchar());
                        std::uint8_t* cpp_arg_8 = (cpp_text_7 = static_cast<pas::AnsiString>(B->GetCharacterName()), cpp_text_7.pchar());
                        return SysUtilsImports::AnsiStrComp(cpp_arg_8, cpp_arg_7);
                    }()) > 0) {
                        pas::list_exchange(List, I, J);
                    }
                }
            }
        }
        Rows = nullptr;
        if (pas::list_count(List) > 0) {
            Rows.set_length(pas::list_count(List));
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(List) - 1); cpp_range_4.next(I); ) {
                Rows[I].Ranger = pas::list_at<aRanger::TRanger>(List, I);
                Rows[I].Top = 0;
                Rows[I].Height = 0;
                if (SelectedIndex < 0 && Rows[I].Ranger->Id == SelectedRangerId) {
                    SelectedIndex = I;
                }
            }
        }
        pas::free(List);
        std::int32_t Top = 0;
        {
            const std::int32_t cpp_last = Rows.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Rows[I].Top = Top;
                    CreateRow(I);
                    Top += Rows[I].Height;
                }
            }
        }
        TablePanel->SetActive(true);
        TablePanel->UpdateScrollRanges();
        J = TablePanel->VerticalScrollBar->Position;
        TablePanel->VerticalScrollBar->SetPosition_2(J - 1);
        TablePanel->VerticalScrollBar->SetPosition_2(J);
        TablePanel->ScrollRectIntoView(SelectedRowRect);
    }

    void TfRating2::RebuildRow(std::int32_t Index) {
        GI_MessageLoop::TObjectGI* Previous{};
        std::int32_t I{};
        GI_MessageLoop::TObjectGI* Child = TablePanel->FirstChild;
        while (Child != nullptr) {
            Previous = Child;
            Child = Child->NextSibling;
            if (Previous->UserState == Index) {
                pas::free(Previous);
            }
        }
        std::int32_t Delta = Rows[Index].Height;
        CreateRow(Index);
        Delta = Rows[Index].Height - Delta;
        {
            const std::int32_t cpp_first = Index + 1;
            const std::int32_t cpp_last = Rows.length() - 1;
            if (cpp_first <= cpp_last) {
                for (I = cpp_first; I <= cpp_last; ++I) {
                    Rows[I].Top += Delta;
                }
            }
        }
        Child = TablePanel->FirstChild;
        while (Child != nullptr) {
            if (static_cast<std::uint32_t>(Child->UserState) > static_cast<std::uint32_t>(Index)) {
                Child->SetPosition(ClassesImports::Point(Child->LocalPosition.X, Child->LocalPosition.Y + Delta));
            }
            Child = Child->NextSibling;
        }
        TablePanel->UpdateScrollRanges();
        I = TablePanel->VerticalScrollBar->Position;
        TablePanel->VerticalScrollBar->SetPosition_2(I - 1);
        TablePanel->VerticalScrollBar->SetPosition_2(I);
    }

    void TfRating2::CreateRow(std::int32_t Index) {
        aRanger::TRanger* Ranger{};
        GI_Panel::TPanelGI* Panel{};
        GI_Panel::TPanelGI* BarPanel{};
        GI_Image::TImageGI* Image{};
        GI_Label::TLabelGI* Caption{};
        float Ratio{};
        std::int32_t ExtraKills{};
        GI_GAI::TgaiGI* Animation{};
        // Nested in TfRating2.CreateRow; caller supplies its parent frame.
        auto GetRatingRankImagePath = [&](std::uint8_t Rank) -> pas::WideString {
            pas::WideString Result{};
            if (Rank == 0) {
                return pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank1"});
            } else if (Rank == 1) {
                return pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank2"});
            } else if (Rank == 2) {
                return pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank3"});
            } else if (Rank == 3) {
                return pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank4"});
            } else if (Rank == 4) {
                return pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank5"});
            } else if (Rank == 5) {
                return pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank6"});
            } else if (Rank == 6) {
                return pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank7"});
            } else if (Rank == 7) {
                return pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank8"});
            } else {
                GR_Main::RaiseWideMessage(u"error"_wref.get());
                return Result;
            }
        };
        // Nested in TfRating2.CreateRow; requires its parent frame.
        auto CreateRatingRowAwardStrip = [&]() -> void {
            std::int32_t I{};
            std::int32_t J{};
            GI_GraphBuf::TGraphBufGI* Buffer{};
            GR_GraphBuf::TGraphBufGR* Icon{};
            std::uint8_t Award{};
            pas::WideString Path{};
            std::int32_t Count = 8;
            std::int32_t Size = GR_Main::GiScalePixels(20);
            std::int32_t Step = Size / 2;
            if (Ranger->AwardIds != nullptr && pas::list_count(Ranger->AwardIds) >= 1) {
                Buffer = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Panel, false);
                Buffer->SetDepth(11.0);
                Buffer->SetImageKindX(GI_Main::ikxLeft);
                Buffer->SetImageKindY(GI_Main::ikyBottom);
                Buffer->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(93), GR_Main::GiScalePixels(130)));
                Buffer->SetSize(ClassesImports::Point(92, Size));
                Buffer->SetPositionModeW(true);
                Buffer->MouseMoveCallback = pas::bind_method<&TfRating2::AwardsMouseMove>(this);
                Buffer->MouseLeaveCallback = pas::bind_method<&TfRating2::HintMouseLeave>(this);
                Buffer->LeftButtonDownCallback = pas::bind_method<&TfRating2::AwardsMouseDown>(this);
                Buffer->UserValue = Ranger->Id;
                reinterpret_cast<GR_GraphBuf::TGraphBufGR*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Buffer->GraphBuf)) + 0)))->AllocateRgbaTight(std::max<std::int32_t>(Buffer->ClientSize.X + 0, Step * Count + Size - Step), Size);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Size - 1); cpp_range.next(I); ) {
                    GR_GraphBuf::TGraphBufGR_FillRect32(Buffer->GraphBuf, ClassesImports::Rect(0, I, Buffer->GraphBuf->Width, I + 1), pas::shl(static_cast<std::int32_t>(System::Round(pas::real_divide(I, Size) * 2.4E+2L) + 10), 24) | pas::shl(250, 16) | pas::shl(250, 8) | 50);
                }
                Buffer->SourceHasPerPixelAlpha = true;
                Buffer->UserState = Index;
                Icon = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                I = std::max<std::int32_t>(0, pas::list_count(Ranger->AwardIds) - Count);
                J = 0;
                while (I < pas::list_count(Ranger->AwardIds)) {
                    Award = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ranger->AwardIds, I)));
                    if (Award < 10) {
                        Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_0", pas::wide_int_to_str(static_cast<std::int32_t>(Award))});
                    } else {
                        Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_", pas::wide_int_to_str(static_cast<std::int32_t>(Award))});
                    }
                    GI_GI::LoadGiByPathIntoGraphBuf(Path, Icon);
                    if (static_cast<std::uint32_t>(Icon->Width) >= static_cast<std::uint32_t>(Icon->Height)) {
                        Icon->RescaleRgba(Size, System::Round(pas::real_divide(Size, static_cast<std::uint32_t>(Icon->Width)) * static_cast<std::uint32_t>(Icon->Height)), 5);
                    } else {
                        Icon->RescaleRgba(System::Round(pas::real_divide(Size, static_cast<std::uint32_t>(Icon->Height)) * static_cast<std::uint32_t>(Icon->Width)), Size, 5);
                    }
                    if (Icon->Height <= Size && Icon->Width + J * Step <= Buffer->GraphBuf->Width) {
                        GR_GraphBuf::TGraphBufGR_BlendRect32(reinterpret_cast<GR_GraphBuf::TGraphBufGR*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Buffer->GraphBuf)) + 0))), ClassesImports::Point(J * Step, 0), Icon, ClassesImports::Rect(0, 0, Icon->Width, Icon->Height));
                    }
                    ++I;
                    ++J;
                }
                pas::free(Icon);
            }
        };
        Ranger = Rows[Index].Ranger;
        Panel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, TablePanel);
        Panel->UserState = Index;
        Panel->SetPosition(ClassesImports::Point(0, Rows[Index].Top));
        Panel->SetPositionModeW(true);
        if (SelectedIndex == Index) {
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            Image->UserState = Index;
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Open", aConst::OwnerInfo[aConst::RaceToOwner(Ranger->PilotRace) & 0x0000007f].InternalName}));
            Image->SetSize(Image->GetContentSize());
            Image->SetPosition(ClassesImports::Point(0, 0));
            Image->SetDepth(11.0);
            Image->LeftButtonDownCallback = pas::bind_method<&TfRating2::RowMouseDown>(this);
            Panel->SetSize(Image->ClientSize);
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(414), GR_Main::GiScalePixelsEx(39, 32)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(150), GR_Main::GiScalePixels(30)));
            Caption->SetFontName(GlobalsV::SmallFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& rankName = Ranger->GetRankName();
                GI_Label::TLabelGI* caption = Caption;
                caption->SetText(rankName);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(345), GR_Main::GiScalePixelsEx(62, 49)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(96), GR_Main::GiScalePixels(19)));
            Caption->SetFontName(GlobalsV::SmallFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& cpp_arg = pas::concat_wide({aConst::LocalizedText(u"FormRating.Kill"_wref.get()), u":"});
                GI_Label::TLabelGI* caption_2 = Caption;
                caption_2->SetText(cpp_arg);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(322), GR_Main::GiScalePixelsEx(77, 63)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(94), GR_Main::GiScalePixels(24)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& localizedText = aConst::LocalizedText(u"FormRating.Dominator"_wref.get());
                GI_Label::TLabelGI* caption_3 = Caption;
                caption_3->SetText(localizedText);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption->UserValue = Index;
            Caption->MouseEnterCallback = pas::bind_method<&TfRating2::ShowDominatorKillsHint>(this);
            Caption->MouseLeaveCallback = pas::bind_method<&TfRating2::HintMouseLeave>(this);
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(419), GR_Main::GiScalePixelsEx(77, 63)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(69), GR_Main::GiScalePixels(24)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& localizedText_2 = aConst::LocalizedText(u"FormRating.Pirate"_wref.get());
                GI_Label::TLabelGI* caption_4 = Caption;
                caption_4->SetText(localizedText_2);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(490), GR_Main::GiScalePixelsEx(77, 63)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(69), GR_Main::GiScalePixels(24)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& localizedText_3 = aConst::LocalizedText(u"FormRating.Other"_wref.get());
                GI_Label::TLabelGI* caption_5 = Caption;
                caption_5->SetText(localizedText_3);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(557), GR_Main::GiScalePixelsEx(77, 63)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(74), GR_Main::GiScalePixels(24)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& localizedText_4 = aConst::LocalizedText(u"FormRating.KillAllShip"_wref.get());
                GI_Label::TLabelGI* caption_6 = Caption;
                caption_6->SetText(localizedText_4);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(321), GR_Main::GiScalePixelsEx(102, 83)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(94), GR_Main::GiScalePixels(22)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            Caption->SetText(pas::wide_int_to_str(Ranger->DominatorKillCount));
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(418), GR_Main::GiScalePixelsEx(102, 83)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(69), GR_Main::GiScalePixels(22)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            Caption->SetText(pas::wide_int_to_str(Ranger->PirateKillCount));
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(489), GR_Main::GiScalePixelsEx(102, 83)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(69), GR_Main::GiScalePixels(22)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption->SetText(pas::wide_int_to_str(Ranger->TotalShipKillCount - Ranger->PirateKillCount - Ranger->DominatorKillCount));
            ExtraKills = 0;
            if (aPlayer::GetPlayer() == Ranger) {
                {
                    std::int32_t cpp_left = pas::checked_cast<aPlayer::TPlayer*>(Ranger)->HyperspaceKillCount;
                    ExtraKills = cpp_left + pas::checked_cast<aPlayer::TPlayer*>(Ranger)->BlackHoleKillCount;
                }
                Caption->SetText(pas::concat_wide({Caption->GetText(), u"/", pas::wide_int_to_str(ExtraKills)}));
            }
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(556), GR_Main::GiScalePixelsEx(102, 83)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(74), GR_Main::GiScalePixels(22)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            Caption->SetText(pas::wide_int_to_str(Ranger->TotalShipKillCount + ExtraKills));
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(321), GR_Main::GiScalePixelsEx(126, 101)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(300), GR_Main::GiScalePixels(22)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxCenter);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& cpp_arg_2 = pas::concat_wide({aConst::LocalizedText(u"FormRating.LiberationSystem"_wref.get()), u": ", pas::wide_int_to_str(Ranger->LiberatedSystemCount)});
                GI_Label::TLabelGI* caption_7 = Caption;
                caption_7->SetText(cpp_arg_2);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(208), GR_Main::GiScalePixels(61)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(120), GR_Main::GiScalePixels(18)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxLeft);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& localizedText_5 = aConst::LocalizedText(u"FormRating.Strength"_wref.get());
                GI_Label::TLabelGI* caption_8 = Caption;
                caption_8->SetText(localizedText_5);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(208), GR_Main::GiScalePixels(106)));
            Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(120), GR_Main::GiScalePixels(18)));
            Caption->SetFontName(GlobalsV::NormalFontName);
            Caption->SetWordWrapEnabled(false);
            Caption->SetTextAlignX(GI_Main::taxLeft);
            Caption->SetTextAlignY(GI_Main::tayCenterEx);
            {
                const pas::WideString& localizedText_6 = aConst::LocalizedText(u"FormRating.Capital"_wref.get());
                GI_Label::TLabelGI* caption_9 = Caption;
                caption_9->SetText(localizedText_6);
            }
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            BarPanel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
            BarPanel->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(201), GR_Main::GiScalePixels(83)));
            if (GR_Main::GiResourceVariant() == 1) {
                BarPanel->SetSize(ClassesImports::Point(82, 10));
            } else {
                BarPanel->SetSize(ClassesImports::Point(103, 11));
            }
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, BarPanel);
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"BarFrame"}));
            Image->SetSize(Image->GetContentSize());
            Image->SetPosition(ClassesImports::Point(0, 0));
            Image->SetDepth(8.0);
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, BarPanel);
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Bar"}));
            Image->SetSize(Image->GetContentSize());
            Image->SetDepth(9.0);
            Ratio = pas::real_divide(Ranger->Strength, pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(aGalaxy::Galaxy->FindStrongestRanger()))->Strength);
            if (Ratio < 0.0L) {
                Ratio = 0.0f;
            } else if (Ratio > 1.0L) {
                Ratio = 1.0f;
            }
            Image->SetPosition(ClassesImports::Point(1 - System::Round((1.0L - Ratio) * (Image->ClientSize.X / 2)), 1));
            BarPanel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
            BarPanel->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(201), GR_Main::GiScalePixels(128)));
            if (GR_Main::GiResourceVariant() == 1) {
                BarPanel->SetSize(ClassesImports::Point(82, 10));
            } else {
                BarPanel->SetSize(ClassesImports::Point(103, 11));
            }
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, BarPanel);
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"BarFrame"}));
            Image->SetSize(Image->GetContentSize());
            Image->SetPosition(ClassesImports::Point(0, 0));
            Image->SetDepth(8.0);
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, BarPanel);
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Bar"}));
            Image->SetSize(Image->GetContentSize());
            Image->SetDepth(9.0);
            Ratio = pas::real_divide(Ranger->Wealth, pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(aGalaxy::Galaxy->FindWealthiestRanger()))->Wealth);
            if (Ratio < 0.0L) {
                Ratio = 0.0f;
            } else if (Ratio > 1.0L) {
                Ratio = 1.0f;
            }
            Image->SetPosition(ClassesImports::Point(1 - System::Round((1.0L - Ratio) * (Image->ClientSize.X / 2)), 1));
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            Image->SetDepth(13.0);
            Image->SetImagePath(pas::concat_wide({u"GI,", Ranger->GetCaptainPortraitResourceBase(), u"i"}));
            Image->SetImageKindX(GI_Main::ikxCenter);
            Image->SetImageKindY(GI_Main::ikyCenter);
            if (GR_Main::GiResourceVariant() == 1) {
                Image->SetPosition(ClassesImports::Point(72, 34));
            } else {
                Image->SetPosition(ClassesImports::Point(90, 43));
            }
            if (GR_Main::GiResourceVariant() == 1) {
                Image->SetSize(ClassesImports::Point(77, 86));
            } else {
                Image->SetSize(ClassesImports::Point(99, 110));
            }
            if (GlobalsV::AnimCaptain) {
                Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Panel);
                Animation->SetDepth(12.0);
                Animation->UsesPlaybackBuffer = true;
                Animation->SetImagePath(pas::concat_wide({Ranger->GetCaptainPortraitResourceBase(), u"a"}));
                Animation->SequenceIndex = 0;
                Animation->UpdateAutoGeometry();
                Animation->TransparentColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 255);
                Animation->SetImageKindX(GI_Main::ikxCenter);
                Animation->SetImageKindY(GI_Main::ikyCenter);
                if (GR_Main::GiResourceVariant() == 1) {
                    Animation->SetPosition(ClassesImports::Point(72, 34));
                } else {
                    Animation->SetPosition(ClassesImports::Point(90, 43));
                }
                if (GR_Main::GiResourceVariant() == 1) {
                    Animation->SetSize(ClassesImports::Point(77, 86));
                } else {
                    Animation->SetSize(ClassesImports::Point(99, 110));
                }
                Animation->RestartPlayback();
            }
            if (Ranger->AwardIds != nullptr && pas::list_count(Ranger->AwardIds) > 0) {
                CreateRatingRowAwardStrip();
            }
        } else {
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            Image->UserState = Index;
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Normal", aConst::OwnerInfo[aConst::RaceToOwner(Ranger->PilotRace) & 0x0000007f].InternalName}));
            Image->SetSize(Image->GetContentSize());
            Image->SetPosition(ClassesImports::Point(0, 0));
            Image->SetDepth(11.0);
            Image->MouseEnterCallback = pas::bind_method<&TfRating2::RowMouseEnter>(this);
            Image->MouseLeaveCallback = pas::bind_method<&TfRating2::RowMouseLeave>(this);
            Image->LeftButtonDownCallback = pas::bind_method<&TfRating2::RowMouseDown>(this);
            Panel->SetSize(Image->ClientSize);
        }
        if (aPlayer::GetPlayer() == Ranger || Ranger->IsInPrison() || Ranger->PartnerShip != nullptr || Ranger->CountWingmen() > 0) {
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            if (aPlayer::GetPlayer() == Ranger) {
                Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Player"}));
            } else if (Ranger->IsInPrison()) {
                Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Prison"}));
            } else {
                Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Duty"}));
            }
            Image->SetSize(Image->GetContentSize());
            Image->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(77), 4));
            Image->SetDepth(1.0E+1);
            Image->UserValue = Index;
            Image->MouseEnterCallback = pas::bind_method<&TfRating2::ShowPartnershipHint>(this);
            Image->MouseLeaveCallback = pas::bind_method<&TfRating2::HintMouseLeave>(this);
        }
        Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
        Image->SetImagePath(GetRatingRankImagePath(Ranger->Rank));
        Image->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(100), GR_Main::GiScalePixels(30)));
        Image->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(442), GR_Main::GiScalePixels(9)));
        Image->SetImageKindX(GI_Main::ikxCenter);
        Image->SetImageKindY(GI_Main::ikyCenter);
        Image->SetDepth(8.0);
        Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
        if (GR_Main::GiResourceVariant() == 1) {
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(215), GR_Main::GiScalePixels(16) + 1));
        } else {
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(215), GR_Main::GiScalePixels(16)));
        }
        Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(117), GR_Main::GiScalePixels(17)));
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetWordWrapEnabled(false);
        Caption->SetTextAlignX(GI_Main::taxCenter);
        Caption->SetTextAlignY(GI_Main::tayCenterEx);
        {
            const pas::WideString& characterName = Ranger->GetCharacterName();
            GI_Label::TLabelGI* caption_10 = Caption;
            caption_10->SetText(characterName);
        }
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        Caption->UserValue = Index;
        Caption->MouseEnterCallback = pas::bind_method<&TfRating2::ShowCareerHint>(this);
        Caption->MouseLeaveCallback = pas::bind_method<&TfRating2::HintMouseLeave>(this);
        Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
        if (GR_Main::GiResourceVariant() == 1) {
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(73), GR_Main::GiScalePixels(16) + 1));
        } else {
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(73), GR_Main::GiScalePixels(16)));
        }
        Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(130), GR_Main::GiScalePixels(17)));
        Caption->SetFontName(GlobalsV::NormalBoldFontName);
        Caption->SetWordWrapEnabled(false);
        Caption->SetTextAlignX(GI_Main::taxCenter);
        Caption->SetTextAlignY(GI_Main::tayCenterEx);
        {
            const pas::WideString& name = Ranger->GetName();
            GI_Label::TLabelGI* caption_11 = Caption;
            caption_11->SetText(name);
        }
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
        if (GR_Main::GiResourceVariant() == 1) {
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(24), GR_Main::GiScalePixels(16) + 1));
        } else {
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(24), GR_Main::GiScalePixels(16)));
        }
        Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(45), GR_Main::GiScalePixels(17)));
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetWordWrapEnabled(false);
        Caption->SetTextAlignX(GI_Main::taxCenter);
        Caption->SetTextAlignY(GI_Main::tayCenterEx);
        Caption->SetText(pas::wide_int_to_str(Index + 1));
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
        if (GR_Main::GiResourceVariant() == 1) {
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(555), GR_Main::GiScalePixels(16) + 1));
        } else {
            Caption->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(555), GR_Main::GiScalePixels(16)));
        }
        Caption->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(75), GR_Main::GiScalePixels(17)));
        Caption->SetFontName(GlobalsV::NormalBoldFontName);
        Caption->SetWordWrapEnabled(false);
        Caption->SetTextAlignX(GI_Main::taxCenter);
        Caption->SetTextAlignY(GI_Main::tayCenterEx);
        Caption->SetText(pas::wide_int_to_str(Ranger->TotalExperience));
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        Rows[Index].Height = Panel->ClientSize.Y;
        TablePanel->VerticalScrollBar->SetSmallChange(std::min<std::int32_t>(Rows[Index].Height, TablePanel->VerticalScrollBar->SmallChange));
        if (SelectedIndex == Index) {
            SelectedRowRect = ClassesImports::Rect(0, Rows[Index].Top, 1, Rows[Index].Top + Rows[Index].Height);
        }
    }

    void TfRating2::RowMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        {
            pas::WideString cpp_arg = pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Select", aConst::OwnerInfo[aConst::RaceToOwner(Rows[Sender->UserState].Ranger->PilotRace) & 0x0000007f].InternalName});
            GI_Image::TImageGI* cpp_arg_2 = pas::checked_cast<GI_Image::TImageGI*>(Sender);
            cpp_arg_2->SetImagePath(std::move(cpp_arg));
        }
        GR_Main::SoundManager->PlaySound(u"Sound.ButtonEnter"_wref.get());
    }

    void TfRating2::RowMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        {
            pas::WideString cpp_arg = pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Normal", aConst::OwnerInfo[aConst::RaceToOwner(Rows[Sender->UserState].Ranger->PilotRace) & 0x0000007f].InternalName});
            GI_Image::TImageGI* cpp_arg_2 = pas::checked_cast<GI_Image::TImageGI*>(Sender);
            cpp_arg_2->SetImagePath(std::move(cpp_arg));
        }
        GR_Main::SoundManager->PlaySound(u"Sound.ButtonLeave"_wref.get());
    }

    void TfRating2::RefreshFeaturedRangers() {
        aShip::TShip* Ship{};
        if (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader] == nullptr) {
            GetByName(u"TraderCaptainI"_wref.get())->SetActive(false);
            GetByName(u"TraderCaptainA"_wref.get())->SetActive(false);
            GetByName(u"BestTrader"_wref.get())->SetActive(false);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButTrader"_wref.get()))->SetDisabled(true);
            GetByName(u"TraderEmpty"_wref.get())->SetActive(true);
            pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"TraderEmpty"_wref.get()))->RestartPlayback();
        } else {
            Ship = reinterpret_cast<aShip::TShip*>(aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader]);
            {
                GI_GraphButton::TGraphButtonGI* ButTrader = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButTrader"_wref.get()));
                ButTrader->SetDisabled(false);
                ButTrader->UserValue = Ship->Id;
                ButTrader->UpCallback = pas::bind_method<&TfRating2::FeaturedRangerClicked>(this);
            }
            GetByName(u"TraderEmpty"_wref.get())->SetActive(false);
            {
                GI_Label::TLabelGI* BestTrader = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"BestTrader"_wref.get()));
                BestTrader->SetText(Ship->Name);
                BestTrader->SetActive(true);
            }
            {
                GI_Image::TImageGI* TraderCaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"TraderCaptainI"_wref.get()));
                TraderCaptainI->UserValue = Ship->Id;
                TraderCaptainI->SetImagePath(pas::concat_wide({u"GI,", Ship->GetCaptainPortraitResourceBase(), u"i"}));
                TraderCaptainI->SetImageKindX(GI_Main::ikxCenter);
                TraderCaptainI->SetImageKindY(GI_Main::ikyCenter);
                TraderCaptainI->SetActive(true);
            }
            {
                GI_GAI::TgaiGI* TraderCaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"TraderCaptainA"_wref.get()));
                TraderCaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                TraderCaptainA->SetImagePath(pas::concat_wide({Ship->GetCaptainPortraitResourceBase(), u"a"}));
                TraderCaptainA->SequenceIndex = 0;
                TraderCaptainA->UpdateAutoGeometry();
                TraderCaptainA->SetSequenceFrame(aMyFunction::RandomIntRange(0, TraderCaptainA->SequenceFrameCount - 1));
                TraderCaptainA->SetImageKindX(GI_Main::ikxCenter);
                TraderCaptainA->SetImageKindY(GI_Main::ikyCenter);
                TraderCaptainA->SetActive(true);
                TraderCaptainA->RestartPlayback();
            }
        }
        if (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior] == nullptr) {
            GetByName(u"WarriorCaptainI"_wref.get())->SetActive(false);
            GetByName(u"WarriorCaptainA"_wref.get())->SetActive(false);
            GetByName(u"BestWarrior"_wref.get())->SetActive(false);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButWarior"_wref.get()))->SetDisabled(true);
            GetByName(u"WariorEmpty"_wref.get())->SetActive(true);
            pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"WariorEmpty"_wref.get()))->RestartPlayback();
        } else {
            Ship = reinterpret_cast<aShip::TShip*>(aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior]);
            {
                GI_GraphButton::TGraphButtonGI* ButWarior = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButWarior"_wref.get()));
                ButWarior->SetDisabled(false);
                ButWarior->UserValue = Ship->Id;
                ButWarior->UpCallback = pas::bind_method<&TfRating2::FeaturedRangerClicked>(this);
            }
            GetByName(u"WariorEmpty"_wref.get())->SetActive(false);
            {
                GI_Label::TLabelGI* BestWarrior = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"BestWarrior"_wref.get()));
                BestWarrior->SetText(Ship->Name);
                BestWarrior->SetActive(true);
            }
            {
                GI_Image::TImageGI* WarriorCaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"WarriorCaptainI"_wref.get()));
                WarriorCaptainI->UserValue = Ship->Id;
                WarriorCaptainI->SetImagePath(pas::concat_wide({u"GI,", Ship->GetCaptainPortraitResourceBase(), u"i"}));
                WarriorCaptainI->SetImageKindX(GI_Main::ikxCenter);
                WarriorCaptainI->SetImageKindY(GI_Main::ikyCenter);
                WarriorCaptainI->SetActive(true);
            }
            {
                GI_GAI::TgaiGI* WarriorCaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"WarriorCaptainA"_wref.get()));
                WarriorCaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                WarriorCaptainA->SetImagePath(pas::concat_wide({Ship->GetCaptainPortraitResourceBase(), u"a"}));
                WarriorCaptainA->SequenceIndex = 0;
                WarriorCaptainA->UpdateAutoGeometry();
                WarriorCaptainA->SetSequenceFrame(aMyFunction::RandomIntRange(0, WarriorCaptainA->SequenceFrameCount - 1));
                WarriorCaptainA->SetImageKindX(GI_Main::ikxCenter);
                WarriorCaptainA->SetImageKindY(GI_Main::ikyCenter);
                WarriorCaptainA->SetActive(true);
                WarriorCaptainA->RestartPlayback();
            }
        }
        if (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate] == nullptr) {
            GetByName(u"PirateCaptainI"_wref.get())->SetActive(false);
            GetByName(u"PirateCaptainA"_wref.get())->SetActive(false);
            GetByName(u"BestPirate"_wref.get())->SetActive(false);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPirate"_wref.get()))->SetDisabled(true);
            GetByName(u"PirateEmpty"_wref.get())->SetActive(true);
            pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"PirateEmpty"_wref.get()))->RestartPlayback();
        } else {
            Ship = reinterpret_cast<aShip::TShip*>(aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate]);
            {
                GI_GraphButton::TGraphButtonGI* ButPirate = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPirate"_wref.get()));
                ButPirate->SetDisabled(false);
                ButPirate->UserValue = Ship->Id;
                ButPirate->UpCallback = pas::bind_method<&TfRating2::FeaturedRangerClicked>(this);
            }
            GetByName(u"PirateEmpty"_wref.get())->SetActive(false);
            {
                GI_Label::TLabelGI* BestPirate = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"BestPirate"_wref.get()));
                BestPirate->SetText(Ship->Name);
                BestPirate->SetActive(true);
            }
            {
                GI_Image::TImageGI* PirateCaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PirateCaptainI"_wref.get()));
                PirateCaptainI->UserValue = Ship->Id;
                PirateCaptainI->SetImagePath(pas::concat_wide({u"GI,", Ship->GetCaptainPortraitResourceBase(), u"i"}));
                PirateCaptainI->SetImageKindX(GI_Main::ikxCenter);
                PirateCaptainI->SetImageKindY(GI_Main::ikyCenter);
                PirateCaptainI->SetActive(true);
            }
            {
                GI_GAI::TgaiGI* PirateCaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"PirateCaptainA"_wref.get()));
                PirateCaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                PirateCaptainA->SetImagePath(pas::concat_wide({Ship->GetCaptainPortraitResourceBase(), u"a"}));
                PirateCaptainA->SequenceIndex = 0;
                PirateCaptainA->UpdateAutoGeometry();
                PirateCaptainA->SetSequenceFrame(aMyFunction::RandomIntRange(0, PirateCaptainA->SequenceFrameCount - 1));
                PirateCaptainA->SetImageKindX(GI_Main::ikxCenter);
                PirateCaptainA->SetImageKindY(GI_Main::ikyCenter);
                PirateCaptainA->SetActive(true);
                PirateCaptainA->RestartPlayback();
            }
        }
    }

    void TfRating2::SelectMusic() {
        if (aPlayer::GetPlayer() == nullptr) {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
        } else if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
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
            } else if (pas::in_set<7, 7, 12, 12>(aPlayer::GetPlayer()->DockedTo->TypeId)) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace) & 0x0000007f].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace) & 0x0000007f].InternalName}));
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
    }

    void TfRating2::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop != nullptr && ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(2);
        }
    }

    // Only displays a hint for the player.
    void TfRating2::ShowDominatorKillsHint(GI_MessageLoop::TObjectGI* Sender) {
        std::uint8_t I{};
        WindowsSdk::TPoint Cursor{};
        pas::WideString Text{};
        aRanger::TRanger* Ranger{};
        // Nested in TfRating2.ShowDominatorKillsHint; caller supplies its parent frame.
        auto FormatDominatorKillsHintColumn = [&](std::int32_t Column) -> pas::WideString {
            return pas::wide_int_to_str(DominatorHintColumns[Column] + 40);
        };
        Ranger = Rows[Sender->UserValue].Ranger;
        if (aPlayer::GetPlayer() == Ranger) {
            RewardWindow->SetActive(true);
            if (aPlayer::GetPlayer() == Ranger) {
                {
                    GI_Label::TLabelGI* RewardName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                    RewardName->SetText(([&] {
                        pas::WideString name = Ranger->GetName();
                        pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRating.PlayerName"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name));
                    }()));
                }
                {
                    GI_GraphBuf::TGraphBufGI* RewardImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"PlayerB"}), RewardImage->GraphBuf);
                }
            } else {
                {
                    GI_Label::TLabelGI* RewardName_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                    RewardName_2->SetText(([&] {
                        pas::WideString name_2 = Ranger->GetName();
                        pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRating.PartnerName"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_2));
                    }()));
                }
                {
                    GI_GraphBuf::TGraphBufGI* RewardImage_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"DutyB"}), RewardImage_2->GraphBuf);
                }
            }
            if (aPlayer::GetPlayer() == Ranger) {
                Text = pas::concat_wide({u"<td=", FormatDominatorKillsHintColumn(1), u"><align=left>", aConst::LocalizedText(u"FormRating.Dominator"_wref.get()), u"</align>", u"\r\n"});
                for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range.next(I); ) {
                    if (aConst::DominatorDisplayOrder[I] != 0) {
                        Text = pas::concat_wide({Text, u"<td=", FormatDominatorKillsHintColumn(1), u"><align=left>", aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"ShipType.Dominator.Blazer.", SysUtils::IntToStr(aConst::DominatorDisplayOrder[I])}))), u"<td=", FormatDominatorKillsHintColumn(2), u">:</align><td=", FormatDominatorKillsHintColumn(3), u"><align=left>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(aPlayer::GetPlayer()->DominatorKillsByType[aConst::DominatorDisplayOrder[I]]), u"<color=255,240,100>"_w), u"</align>", u"\r\n"});
                    }
                }
                {
                    GI_Label::TLabelGI* RewardText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                    RewardText->SetText(Text);
                }
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage_3 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"_wref.get()));
                RewardImage_3->SourceHasPerPixelAlpha = true;
                RewardImage_3->SetImageKindX(GI_Main::ikxCenter);
                RewardImage_3->SetImageKindY(GI_Main::ikyCenter);
            }
            {
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                fShip2::TfShip2::LayoutItemInfo(RewardWindow, cpp_arg_2, cpp_arg, true, true, 0);
            }
            {
                GI_Label::TLabelGI* RewardText_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"_wref.get()));
                RewardText_2->SetTextAlignX(GI_Main::taxLeft);
            }
            {
                GI_Label::TLabelGI* RewardName_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"_wref.get()));
                RewardName_3->SetSize(ClassesImports::Point(RewardWindow->ClientSize.X - RewardName_3->LocalPosition.X - RewardWindow->WorkSubRect.Right, RewardName_3->ClientSize.Y));
            }
            Cursor = GetCursorPoint();
            RewardWindow->SetPosition(ClassesImports::Point(Cursor.X + 100, std::max<std::int32_t>(0, Cursor.Y - RewardWindow->ClientSize.Y - 20)));
        }
    }

    void TfRating2::p_destroy() {
        fRating2::TfRating2_Destroy(this);
    }

} // namespace fRating2
