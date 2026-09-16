#include "layout/fRewards.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aNormalShip.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/fRewards.hpp"

namespace fRewards {
    std::uint8_t RunRewards(GI_MessageLoop::TMessageLoopGI* ParentLoop, std::uint8_t ReadOnly) {
        std::uint8_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        ParentLoop->RootUiObject->NativeHook50();
        ParentLoop->CaptureCursorState(&State);
        ParentLoop->SetCursorActive(false);
        ParentLoop->DrawQueuedUpdateRects();
        Globals::RewardsScreen->ParentLoop = ParentLoop;
        ParentLoop->ChildLoop = Globals::RewardsScreen;
        Globals::RewardsScreen->ReadOnly = ReadOnly;
        if (Globals::RewardsScreen->Run() == 1) {
            Result = true;
        } else {
            Result = false;
        }
        Globals::RewardsScreen->ParentLoop = nullptr;
        ParentLoop->ChildLoop = nullptr;
        ParentLoop->InvalidateViewport();
        ParentLoop->RestoreCursorState(&State);
        ParentLoop->UpdateCursorPosition();
        ParentLoop->RootUiObject->NativeHook48();
        ParentLoop->Present();
        return Result;
    }

    void TfRewards::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fRewards... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* RewardPanel = MainPanel->FindByNameRecursive(u"RewardPanel"_wref.get());
                RewardPanel->SetPosition(ClassesImports::Point(RewardPanel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, RewardPanel->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfRewards::MainPanelKeyDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButExit"_wref.get()))->UpCallback = pas::bind_method<&TfRewards::CloseClicked>(this);
        AwardsPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PTable"_wref.get()));
    }

    void TfRewards::OnOpen() {
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        Ship = pas::checked_cast<aShip::TShip*>(Globals::AwardSubject);
        if (Ship->AwardVisibleCount == 0) {
            Ship->AwardVisibleCount = pas::list_count(Ship->AwardIds);
        }
        GR_Main::CaptureScreenBackground(false, 0);
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        {
            GI_PanelScrollBar::TPanelScrollBarGI* PTable = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PTable"_wref.get()));
            PTable->ScrollAxis = GI_Panel::psaVertical;
            PTable->VerticalScrollBar->SetSmallChange(System::Round(pas::real_divide(PTable->ClientSize.Y, 5.0E+1L)));
            PTable->VerticalScrollBar->SetLargeChange(PTable->ClientSize.Y);
            PTable->VerticalScrollBar->SetPageSize(PTable->ClientSize.Y);
        }
        {
            GI_Label::TLabelGI* RewardCount = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardCount"_wref.get()));
            RewardCount->SetActive(CanEditAwards());
        }
        {
            GI_GraphButton::TGraphButtonGI* Add = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Add"_wref.get()));
            Add->DownCallback = pas::bind_method<&TfRewards::IncreaseVisibleCount>(this);
            Add->SetActive(CanEditAwards());
        }
        {
            GI_GraphButton::TGraphButtonGI* Sub = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Sub"_wref.get()));
            Sub->DownCallback = pas::bind_method<&TfRewards::DecreaseVisibleCount>(this);
            Sub->SetActive(CanEditAwards());
        }
        AwardsPanel->SetScrollOffset(ClassesImports::Point(0, 0));
        BuildAwardControls();
        ClearHighlight(nullptr);
        DraggedAward = nullptr;
        HoveredAwardId = -1;
        UpdateActionCursor(false);
        RefreshVisibleCount();
    }

    void TfRewards::OnClose() {
        AwardsPanel->FreeOwnedChildren();
        GI_MessageLoop::TMessageLoopGI::OnClose();
    }

    void TfRewards::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer() == Ship) {
            GlobalsV::RequestedScreenId = GlobalsV::screenShip;
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenScanner;
        }
        RequestClose(1);
    }

    std::uint8_t TfRewards::CanEditAwards() {
        return static_cast<std::uint8_t>(ReadOnly ^ 1) && aPlayer::GetPlayer() == Ship;
    }

    pas::WideString TfRewards::GetAwardImagePath(std::int32_t AwardId) {
        if (AwardId < 10) {
            return pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_0", pas::wide_int_to_str(AwardId)});
        }
        return pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_", pas::wide_int_to_str(AwardId)});
    }

    void TfRewards::PlatformMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        aConst::TRewardInfo cpp_result{};
        aConst::TRewardInfo cpp_result_2{};
        std::int32_t AwardId{};
        GI_Image::TImageGI* Platform = reinterpret_cast<GI_Image::TImageGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        AwardId = Sender->UserIndex;
        {
            GI_Label::TLabelGI* InfoZag = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoZag"_wref.get()));
            if (AwardId != 255) {
                InfoZag->SetText((static_cast<void>(pas::checked_cast<aNormalShip::TNormalShip*>(Ship)), aNormalShip::TNormalShip::GetAwardInfo(AwardId, cpp_result), cpp_result).Name);
            } else {
                InfoZag->SetText(u""_wref.get());
            }
        }
        {
            GI_Label::TLabelGI* Info = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Info"_wref.get()));
            if (AwardId != 255) {
                Info->SetText((static_cast<void>(pas::checked_cast<aNormalShip::TNormalShip*>(Ship)), aNormalShip::TNormalShip::GetAwardInfo(AwardId, cpp_result_2), cpp_result_2).Text);
            } else {
                Info->SetText(u""_wref.get());
            }
        }
        GI_MessageLoop::TObjectGI* Obj = AwardsPanel->FirstChild;
        while (Obj != nullptr) {
            if (pas::class_cast_if<GI_Image::TImageGI*>(Obj) != nullptr && ([&] {
                pas::WideString cpp_string = reinterpret_cast<GI_Image::TImageGI*>(Obj)->GetImagePath();
                pas::WideString cpp_string_2 = pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"PlatformA"});
                return cpp_string == cpp_string_2;
            }())) {
                reinterpret_cast<GI_Image::TImageGI*>(Obj)->SetImagePath(pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"PlatformN"}));
            }
            Obj = Obj->NextSibling;
        }
        Platform->SetImagePath(pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"PlatformA"}));
        UpdateActionCursor(false);
    }

    void TfRewards::AwardMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        HoveredAwardId = Sender->UserIndex;
        PlatformMouseEnter(Sender);
    }

    void TfRewards::AwardMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        HoveredAwardId = -1;
        UpdateActionCursor(false);
    }

    void TfRewards::ClearHighlight(GI_MessageLoop::TObjectGI* Sender) {
        HoveredAwardId = -1;
        {
            GI_Label::TLabelGI* InfoZag = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoZag"_wref.get()));
            InfoZag->SetText(u""_wref.get());
        }
        {
            GI_Label::TLabelGI* Info = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Info"_wref.get()));
            Info->SetText(u""_wref.get());
        }
        GI_MessageLoop::TObjectGI* Obj = AwardsPanel->FirstChild;
        while (Obj != nullptr) {
            if (pas::class_cast_if<GI_Image::TImageGI*>(Obj) != nullptr && ([&] {
                pas::WideString cpp_string = reinterpret_cast<GI_Image::TImageGI*>(Obj)->GetImagePath();
                pas::WideString cpp_string_2 = pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"PlatformA"});
                return cpp_string == cpp_string_2;
            }())) {
                reinterpret_cast<GI_Image::TImageGI*>(Obj)->SetImagePath(pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"PlatformN"}));
            }
            Obj = Obj->NextSibling;
        }
        UpdateActionCursor(false);
    }

    void TfRewards::AwardMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t Temp{};
        if (DraggedAward == nullptr) {
            DraggedAward = Sender;
            pas::checked_cast<GI_Image::TImageGI*>(Sender)->SetImagePath(pas::WideString());
            GR_Main::SoundManager->PlaySound(u"Sound.SlotGet"_wref.get());
        } else {
            pas::list_exchange(Ship->AwardIds, Sender->UserData, DraggedAward->UserData);
            Temp = Sender->UserIndex;
            Sender->UserIndex = DraggedAward->UserIndex;
            DraggedAward->UserIndex = Temp;
            Temp = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)))->UserIndex;
            reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)))->UserIndex = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(DraggedAward->UserValue)))->UserIndex;
            reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(DraggedAward->UserValue)))->UserIndex = Temp;
            {
                GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(Sender);
                cpp_with->SetImagePath(TfRewards::GetAwardImagePath(Sender->UserIndex));
                cpp_with->SetSize(cpp_with->GetContentSize());
            }
            {
                GI_Image::TImageGI* cpp_with_2 = pas::checked_cast<GI_Image::TImageGI*>(DraggedAward);
                cpp_with_2->SetImagePath(TfRewards::GetAwardImagePath(DraggedAward->UserIndex));
                cpp_with_2->SetSize(cpp_with_2->GetContentSize());
            }
            DraggedAward = nullptr;
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            PlatformMouseEnter(Sender);
        }
        UpdateActionCursor(false);
    }

    void TfRewards::IncreaseVisibleCount(GI_MessageLoop::TObjectGI* Sender) {
        if (Ship->AwardVisibleCount < pas::list_count(Ship->AwardIds)) {
            ++Ship->AwardVisibleCount;
        }
        RefreshVisibleCount();
    }

    void TfRewards::DecreaseVisibleCount(GI_MessageLoop::TObjectGI* Sender) {
        if (Ship->AwardVisibleCount > 0) {
            --Ship->AwardVisibleCount;
        }
        RefreshVisibleCount();
    }

    void TfRewards::RefreshVisibleCount() {
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Ship->AwardVisibleCount);
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardCount"_wref.get()));
            cpp_arg->SetText(intToStr);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Sub"_wref.get()))->SetDisabled(Ship->AwardVisibleCount <= 1);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Add"_wref.get()))->SetDisabled(Ship->AwardVisibleCount >= pas::list_count(Ship->AwardIds));
    }

    void TfRewards::BuildAwardControls() {
        std::int32_t I{};
        std::int32_t Count{};
        std::int32_t X{};
        std::int32_t Y{};
        GI_Image::TImageGI* Platform{};
        std::uint8_t AwardId{};
        AwardsPanel->FreeOwnedChildren();
        if (Ship->AwardIds != nullptr) {
            Count = std::max<std::int32_t>(10, pas::list_count(Ship->AwardIds));
        } else {
            Count = 10;
        }
        if (Count > 10) {
            Count = (Count + 2) / 3 * 3;
        }
        {
            GI_MessageLoop::TObjectGI* cpp_with = pas::construct_call<GI_MessageLoop::TObjectGI>(GI_MessageLoop::TObjectGI_Create, AwardsPanel);
            cpp_with->SetPosition(ClassesImports::Point(0, 0));
            cpp_with->SetSize(ClassesImports::Point(1, 1));
            cpp_with->SetPositionModeW(true);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Platform = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, AwardsPanel);
            Platform->SetImagePath(pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"PlatformN"}));
            Platform->SetSize(Platform->GetContentSize());
            Platform->SetOrigin(EC_Struct::HalfPoint(Platform->ClientSize));
            X = AwardsPanel->ClientSize.X / 3;
            X = X * (I % 3) + (X - Platform->ClientSize.X / 2);
            Y = AwardsPanel->ClientSize.Y / 4;
            Y = Y * (I / 3) + (Y - Platform->ClientSize.Y / 2);
            Platform->SetPosition(ClassesImports::Point(X, Y));
            Platform->SetPositionModeW(true);
            if (Ship->AwardIds != nullptr && I < pas::list_count(Ship->AwardIds)) {
                AwardId = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->AwardIds, I)));
                Platform->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Platform));
                Platform->UserIndex = AwardId;
                Platform->MouseEnterCallback = pas::bind_method<&TfRewards::PlatformMouseEnter>(this);
                Platform->MouseLeaveCallback = pas::bind_method<&TfRewards::AwardMouseLeave>(this);
            } else {
                Platform->UserValue = 0;
                Platform->UserIndex = 255;
            }
            if (Ship->AwardIds != nullptr && I < pas::list_count(Ship->AwardIds)) {
                AwardId = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->AwardIds, I)));
                {
                    GI_Image::TImageGI* cpp_with_2 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, AwardsPanel);
                    cpp_with_2->SetImagePath(TfRewards::GetAwardImagePath(AwardId));
                    cpp_with_2->SetSize(cpp_with_2->GetContentSize());
                    cpp_with_2->SetOrigin(ClassesImports::Point(cpp_with_2->ClientSize.X / 2, cpp_with_2->ClientSize.Y));
                    cpp_with_2->SetPositionModeW(true);
                    cpp_with_2->SetPosition(ClassesImports::Point(X, Y));
                    cpp_with_2->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Platform));
                    cpp_with_2->UserIndex = AwardId;
                    cpp_with_2->UserData = I;
                    cpp_with_2->MouseEnterCallback = pas::bind_method<&TfRewards::AwardMouseEnter>(this);
                    cpp_with_2->MouseLeaveCallback = pas::bind_method<&TfRewards::AwardMouseLeave>(this);
                    if (CanEditAwards()) {
                        cpp_with_2->LeftButtonDownCallback = pas::bind_method<&TfRewards::AwardMouseDown>(this);
                    } else {
                        cpp_with_2->LeftButtonDownCallback = nullptr;
                    }
                }
            }
        }
        AwardsPanel->MouseLeaveCallback = pas::bind_method<&TfRewards::ClearHighlight>(this);
        AwardsPanel->UpdateScrollRanges();
        AwardsPanel->SetVerticalScrollbarEnabled(Count > 10);
    }

    void TfRewards::UpdateActionCursor(std::uint8_t CanTake) {
        if (DraggedAward == nullptr) {
            if (HoveredAwardId >= 0 && CanEditAwards()) {
                SetCursorByName(u"Take"_wref.get());
            } else {
                SetCursorByName(u"Main"_wref.get());
            }
        } else {
            SetCursorImage(TfRewards::GetAwardImagePath(DraggedAward->UserIndex), ClassesImports::Point(16, 16));
        }
    }

    void TfRewards::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop != nullptr && ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(2);
        }
    }

    void TfRewards::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Key == WindowsSdk::VK_ESCAPE || Key == WindowsSdk::VK_RETURN || Key == 'R') {
                CloseClicked(nullptr);
            }
            {
                GI_PanelScrollBar::TPanelScrollBarGI* PTable = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PTable"_wref.get()));
                if (Key == WindowsSdk::VK_UP) {
                    PTable->VerticalScrollBar->SetPosition_2(PTable->VerticalScrollBar->Position - PTable->VerticalScrollBar->SmallChange);
                } else if (Key == WindowsSdk::VK_DOWN) {
                    PTable->VerticalScrollBar->SetPosition_2(PTable->VerticalScrollBar->Position + PTable->VerticalScrollBar->SmallChange);
                } else if (Key == WindowsSdk::VK_PRIOR) {
                    PTable->VerticalScrollBar->SetPosition_2(PTable->VerticalScrollBar->Position - PTable->VerticalScrollBar->LargeChange);
                } else if (Key == WindowsSdk::VK_NEXT) {
                    PTable->VerticalScrollBar->SetPosition_2(PTable->VerticalScrollBar->Position + PTable->VerticalScrollBar->LargeChange);
                }
            }
        }
    }

    void TfRewards::SelectMusic() {
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

    void TfRewards::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            GI_PanelScrollBar::TPanelScrollBarGI* PTable = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PTable"_wref.get()));
            PTable->VerticalScrollBar->SetPosition_2(PTable->VerticalScrollBar->Position - PTable->VerticalScrollBar->SmallChange * 5);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            GI_PanelScrollBar::TPanelScrollBarGI* PTable_2 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PTable"_wref.get()));
            PTable_2->VerticalScrollBar->SetPosition_2(PTable_2->VerticalScrollBar->Position + PTable_2->VerticalScrollBar->SmallChange * 5);
        }
    }

} // namespace fRewards
