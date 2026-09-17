#include "layout/fScaner.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GI_Window.hpp"
#include "types/GI_Zone.hpp"
#include "types/GR_Music.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aNormalShip.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/fShip2.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fRewards.hpp"
#include "units/fScaner.hpp"

namespace fScaner {
    pas::WideString GetPirateRankSmallImagePath(std::uint8_t Rank) {
        pas::WideString Result{};
        {
            std::uint8_t cpp_case = Rank;
            if (cpp_case >= 0 && cpp_case <= 7) {
                return static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormShip2.PRank", SysUtils::IntToStr(Rank + 1), "s"}));
            } else {
                GR_Main::RaiseWideMessage(u"error"_wref.get());
                return Result;
            }
        }
    }

    void TfScaner::InitializeLayout() {
        std::int32_t I{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fScaner... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* BGBuf = MainPanel->FindByNameRecursive(u"BGBuf"_wref.get());
                BGBuf->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                {
                    GI_MessageLoop::TObjectGI* cpp_with_3 = BGBuf->NextSibling;
                    cpp_with_3->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                }
            }
            {
                GI_MessageLoop::TObjectGI* PanelRight_Parent_Parent = MainPanel->FindByNameRecursive(u"PanelRight"_wref.get())->Parent->Parent;
                PanelRight_Parent_Parent->SetPosition(ClassesImports::Point(PanelRight_Parent_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelRight_Parent_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* RankWnd = MainPanel->FindByNameRecursive(u"RankWnd"_wref.get());
                RankWnd->SetPosition(ClassesImports::Point(RankWnd->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, RankWnd->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* RewardWnd = MainPanel->FindByNameRecursive(u"RewardWnd"_wref.get());
                RewardWnd->SetPosition(ClassesImports::Point(RewardWnd->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, RewardWnd->LocalPosition.Y));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        BackgroundBuffer = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()));
        ItemInfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"PII"_wref.get()));
        ItemImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"_wref.get()));
        ItemNameLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"_wref.get()));
        ItemDescriptionLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"_wref.get()));
        ItemSizeLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"_wref.get()));
        ItemPriceLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"_wref.get()));
        ItemRaceImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"_wref.get()));
        SkillsPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"Skills"_wref.get()));
        FreeSkillPointsLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"SkillFreePoints"_wref.get()));
        ExitButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Exit"_wref.get()));
        ExitButton->UpCallback = pas::bind_method<&TfScaner::CloseClicked>(this);
        RewardsBuffer = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardsImg"_wref.get()));
        RewardWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"RewardWnd"_wref.get()));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Left"_wref.get()))->UpCallback = pas::bind_method<&TfScaner::ScrollCargoLeft>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Right"_wref.get()))->UpCallback = pas::bind_method<&TfScaner::ScrollCargoRight>(this);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfScaner::MainPanelKeyDown>(this);
        GetByName(u"MainPanel"_wref.get())->LeftButtonUpCallback = pas::bind_method<&TfScaner::MainPanelMouseUp>(this);
        {
            GI_MessageLoop::TObjectGI* Ship3D = GetByName(u"Ship3D"_wref.get());
            ShipImageCenter = EC_Struct::AddPoints(Ship3D->LocalPosition, EC_Struct::HalfPoint(Ship3D->ClientSize));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 5); cpp_range.next(I); ) {
            SkillImages[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I)}))));
            SkillPositiveImages[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "p"}))));
            SkillNegativeImages[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "n"}))));
            SkillPanels[I] = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "c"}))));
            SkillImageRestTop[I] = SkillImages[I]->LocalPosition.Y;
            SkillButtons[I] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "Add"}))));
        }
        PanelSlideStartX = GR_Main::GiScalePixels(100);
        PanelSlideEndX = GetByName(u"PanelRight"_wref.get())->LocalPosition.X;
    }

    void TfScaner::OnOpen() {
        std::int32_t I{};
        std::int32_t SlotCount{};
        std::int32_t J{};
        std::int32_t MaximumSlots{};
        aRanger::TRanger* Ranger{};
        // Native frame initializes and finalizes this unreferenced managed slot.
        pas::WideString ReservedText{};
        std::int32_t Stage = 0;
        try {
            ArtefactZones.set_length(aConst::DefaultHullSlotCounts[8]);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::DefaultHullSlotCounts[8] - 1); cpp_range.next(I); ) {
                ArtefactZones[I] = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(I), "z"}))));
            }
            Stage = 1;
            if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
                GR_Main::CaptureScreenBackground(true, 0);
            }
            BackgroundBuffer->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
            Stage = 2;
            ShipToInspect = pas::checked_cast<aShip::TShip*>(Globals::ScannerTarget);
            Stage = 3;
            if (aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->ScriptItemsAct(0x00000010, ShipToInspect, nullptr, 0);
            }
            Stage = 4;
            ShipToInspect->ScriptItemsAct(0x00000018, nullptr, nullptr, 0);
            Stage = 5;
            CompactHullInfo = pas::in_range(ShipToInspect->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation));
            Stage = 6;
            CountCargoEntries();
            VisibleCargoCount = 6;
            CargoOffset = 0;
            Stage = 7;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 5); cpp_range_2.next(I); ) {
                GI_Zone::TZoneGI* cpp_with = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "z"}))));
                cpp_with->EnterCallback = pas::bind_method<&TfScaner::ShowPropertyInfo>(this);
                cpp_with->LeaveCallback = pas::bind_method<&TfScaner::HidePropertyInfo>(this);
            }
            Stage = 8;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 7); cpp_range_3.next(I); ) {
                MaximumSlots = 1;
                if (aConst::EquipmentSlotLayouts[I].ItemType == aConst::t_Weapon1) {
                    MaximumSlots = 5;
                }
                SlotCount = ShipToInspect->GetSlotCountForItemType(aConst::EquipmentSlotLayouts[I].ItemType);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_4.next(J); ) {
                    EquipmentAnimations[I][J] = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"anim"})));
                    EquipmentAnimations[I][J]->UserState = 0;
                    GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"off"}))->SetActive(false);
                    GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"Set"}))->SetActive(false);
                }
                if (aConst::EquipmentSlotLayouts[I].ItemType == aConst::t_Weapon1) {
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(SlotCount, 4); cpp_range_5.next(J); ) {
                        GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"off"}))->SetActive(true);
                        GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"Set"}))->SetActive(false);
                    }
                } else {
                    for (auto cpp_range_6 = pas::for_to<std::int32_t>(SlotCount, 0); cpp_range_6.next(J); ) {
                        GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"off"}))->SetActive(true);
                        GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"Set"}))->SetActive(false);
                    }
                }
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(SlotCount, MaximumSlots - 1); cpp_range_7.next(J); ) {
                    GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"i"}))->SetActive(false);
                    GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"anim"}))->SetActive(false);
                }
            }
            Stage = 9;
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, 5); cpp_range_8.next(I); ) {
                GI_Zone::TZoneGI* cpp_with_2 = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "z"}))));
                cpp_with_2->EnterCallback = pas::bind_method<&TfScaner::ShowPropertyInfo>(this);
                cpp_with_2->LeaveCallback = pas::bind_method<&TfScaner::HidePropertyInfo>(this);
            }
            Stage = 10;
            HoveredItem = nullptr;
            GetByName(u"PII"_wref.get())->SetActive(false);
            GetByName(u"InfoHull"_wref.get())->SetActive(false);
            Stage = 11;
            if (ShipToInspect->TypeId == aGalaxyStruct::stRanger) {
                Ranger = pas::checked_cast<aRanger::TRanger*>(ShipToInspect);
                {
                    const pas::WideString& fullName = Ranger->GetFullName(u"\r\n"_wref.get());
                    GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ShipName"_wref.get()));
                    cpp_arg->SetText(fullName);
                }
                {
                    const pas::WideString& characterName = Ranger->GetCharacterName();
                    GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharName"_wref.get()));
                    cpp_arg_2->SetText(characterName);
                }
            } else {
                {
                    const pas::WideString& fullName_2 = ShipToInspect->GetFullName(u"\r\n"_wref.get());
                    GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ShipName"_wref.get()));
                    cpp_arg_3->SetText(fullName_2);
                }
                {
                    const pas::WideString& localizedTypeName = ShipToInspect->GetLocalizedTypeName();
                    GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharName"_wref.get()));
                    cpp_arg_4->SetText(localizedTypeName);
                }
            }
            Stage = 12;
            {
                GI_Image::TImageGI* Ship3D = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Ship3D"_wref.get()));
                Ship3D->SetImagePath(ShipToInspect->GetShipPortraitImagePath());
                Ship3D->SetImageKindX(GI_Main::ikxCenter);
                Ship3D->SetImageKindY(GI_Main::ikyCenter);
                {
                    Types::TPoint visualCenter = Ship3D->GetVisualCenter();
                    Types::TPoint shipImageCenter = ShipImageCenter;
                    Ship3D->SetPosition(EC_Struct::SubtractPoints(shipImageCenter, visualCenter));
                }
            }
            Stage = 13;
            GetByName(u"RankWnd"_wref.get())->SetActive(false);
            {
                GI_Image::TImageGI* RankI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RankI"_wref.get()));
                Stage = 14;
                if (pas::class_cast_if<aKling::TKling*>(ShipToInspect) != nullptr || pas::class_cast_if<aRuins::TRuins*>(ShipToInspect) != nullptr || pas::class_cast_if<aTranclucator::TTranclucator*>(ShipToInspect) != nullptr) {
                    RankI->MouseEnterCallback = nullptr;
                    RankI->MouseLeaveCallback = nullptr;
                } else {
                    RankI->MouseEnterCallback = pas::bind_method<&TfScaner::ShowPropertyInfo>(this);
                    RankI->MouseLeaveCallback = pas::bind_method<&TfScaner::HidePropertyInfo>(this);
                }
                RankI->SetActive(true);
                Stage = 15;
                if (pas::class_cast_if<aKling::TKling*>(ShipToInspect) != nullptr) {
                    RankI->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank", pas::wide_int_to_str(aConst::DominatorShipDefinitions[pas::checked_cast<aKling::TKling*>(ShipToInspect)->KlingType].RankImageIndex)}));
                } else if (pas::class_cast_if<aRuins::TRuins*>(ShipToInspect) != nullptr) {
                    RankI->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank7"}));
                } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(ShipToInspect) != nullptr) {
                    RankI->SetImagePath(pas::concat_wide({u"GI,Bm.FormRating2.", GR_Main::GiResourceSuffix(), u"Rank4"}));
                } else if (pas::class_cast_if<aNormalShip::TNormalShip*>(ShipToInspect) != nullptr && ShipToInspect->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    RankI->SetImagePath(static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormRating2.2Rank", SysUtils::IntToStr(pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank + 1)})));
                } else {
                    RankI->SetActive(false);
                }
            }
            Stage = 16;
            {
                GI_Image::TImageGI* RankI2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RankI2"_wref.get()));
                if (pas::class_cast_if<aNormalShip::TNormalShip*>(ShipToInspect) != nullptr && ShipToInspect->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    RankI2->SetActive(true);
                    RankI2->MouseEnterCallback = pas::bind_method<&TfScaner::ShowPropertyInfo>(this);
                    RankI2->MouseLeaveCallback = pas::bind_method<&TfScaner::HidePropertyInfo>(this);
                    RankI2->SetImagePath(static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormShip2.PRank", SysUtils::IntToStr(pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->PirateRank + 1)})));
                } else {
                    RankI2->SetActive(false);
                }
            }
            Stage = 17;
            GetByName(u"PRankForm"_wref.get())->SetActive(false);
            UpdateSkills();
            GetByName(u"S_Left"_wref.get())->SetActive(false);
            GetByName(u"S_Right"_wref.get())->SetActive(false);
            GetByName(u"S_Left"_wref.get())->SetActive(true);
            GetByName(u"S_Right"_wref.get())->SetActive(true);
            Stage = 18;
            {
                GI_Image::TImageGI* CaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"_wref.get()));
                CaptainI->SetImagePath(pas::concat_wide({u"GI,", aShip::TShip_GetCaptainPortraitResourceBase(ShipToInspect), u"i"}));
                CaptainI->SetImageKindX(GI_Main::ikxCenter);
                CaptainI->SetImageKindY(GI_Main::ikyCenter);
                CaptainI->SetActive(true);
            }
            Stage = 19;
            {
                GI_GAI::TgaiGI* CaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"_wref.get()));
                CaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                CaptainA->SetImagePath(pas::concat_wide({aShip::TShip_GetCaptainPortraitResourceBase(ShipToInspect), u"a"}));
                CaptainA->SequenceIndex = 0;
                CaptainA->UpdateAutoGeometry();
                CaptainA->SetImageKindX(GI_Main::ikxCenter);
                CaptainA->SetImageKindY(GI_Main::ikyCenter);
                CaptainA->SetActive(true);
                CaptainA->RestartPlayback();
            }
            Stage = 20;
            BuildRewardStrip(ShipToInspect);
            Stage = 21;
            Update();
            Stage = 22;
            InvalidateViewport();
            Stage = 23;
            DrawQueuedUpdateRects();
            Stage = 24;
            {
                WindowsSdk::TPoint point = ClassesImports::Point(PanelSlideStartX, GetByName(u"PanelRight"_wref.get())->LocalPosition.Y);
                GI_MessageLoop::TObjectGI* byName = GetByName(u"PanelRight"_wref.get());
                byName->SetPosition(point);
            }
            PanelSlideStep = 20;
            if (PanelSlideTimer != nullptr) {
                CancelCallbackTimer(PanelSlideTimer);
                PanelSlideTimer = nullptr;
            }
            PanelSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfScaner::AdvancePanelSlide>(this), 0);
            Stage = 25;
            if (ItemHoverTimer != nullptr) {
                CancelCallbackTimer(ItemHoverTimer);
                ItemHoverTimer = nullptr;
            }
            ItemHoverTimer = ScheduleCallbackTimer(1, 1, pas::bind_method<&TfScaner::AdvanceItemHover>(this), 0);
            Stage = 26;
            if (pas::real_divide(ShipToInspect->GetHull()->HullPoints, ShipToInspect->GetHull()->Weight) > 0.2L) {
                GetByName(u"CenterNormalImage"_wref.get())->SetActive(true);
                {
                    GI_GAI::TgaiGI* CenterNormalAnim = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CenterNormalAnim"_wref.get()));
                    CenterNormalAnim->SetActive(true);
                    CenterNormalAnim->RestartPlayback();
                }
                GetByName(u"CenterDamageImage"_wref.get())->SetActive(false);
                GetByName(u"CenterDamageAnim"_wref.get())->SetActive(false);
            } else {
                GetByName(u"CenterNormalImage"_wref.get())->SetActive(false);
                GetByName(u"CenterNormalAnim"_wref.get())->SetActive(false);
                GetByName(u"CenterDamageImage"_wref.get())->SetActive(true);
                {
                    GI_GAI::TgaiGI* CenterDamageAnim = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CenterDamageAnim"_wref.get()));
                    CenterDamageAnim->SetActive(true);
                    CenterDamageAnim->RestartPlayback();
                }
            }
            Stage = 27;
            {
                std::uint8_t cpp_arg_5 = ShipToInspect->GetSlotCount(aConst::sskAfterburner) > 0;
                GI_MessageLoop::TObjectGI* byName_2 = GetByName(u"Forsage"_wref.get());
                byName_2->SetActive(cpp_arg_5);
            }
            {
                std::uint8_t cpp_arg_6 = ShipToInspect->AfterburnerActive && aShip::TShip_IsEquipmentUsable(ShipToInspect, ShipToInspect->GetEngine());
                GI_MessageLoop::TObjectGI* byName_3 = GetByName(u"ForsageLight"_wref.get());
                byName_3->SetActive(cpp_arg_6);
            }
            Stage = 28;
            {
                GI_GraphButton::TGraphButtonGI* ForsageBut = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ForsageBut"_wref.get()));
                ForsageBut->MouseEnterCallback = pas::bind_method<&TfScaner::ShowPropertyInfo>(this);
                ForsageBut->MouseLeaveCallback = pas::bind_method<&TfScaner::HidePropertyInfo>(this);
                ForsageBut->SetDisabled(true);
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TfScaner.BeforeRun, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfScaner::OnClose() {
        ShipToInspect->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        if (ItemHoverTimer != nullptr) {
            CancelCallbackTimer(ItemHoverTimer);
            ItemHoverTimer = nullptr;
        }
        if (HideItemTimer != nullptr) {
            CancelCallbackTimer(HideItemTimer);
            HideItemTimer = nullptr;
        }
        if (PropertyHintTimer != nullptr) {
            CancelCallbackTimer(PropertyHintTimer);
            PropertyHintTimer = nullptr;
        }
        if (PanelSlideTimer != nullptr) {
            CancelCallbackTimer(PanelSlideTimer);
            PanelSlideTimer = nullptr;
        }
    }

    void TfScaner::BuildRewardStrip(aShip::TShip* Ship) {
        std::uint8_t AwardId{};
        pas::WideString Path{};
        float Spacing{};
        HideRewardInfo();
        if (!(pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr) || Ship->AwardIds == nullptr || pas::list_count(Ship->AwardIds) < 1) {
            RewardsBuffer->SetActive(false);
            return;
        }
        if (Ship->AwardIds == nullptr || pas::list_count(Ship->AwardIds) < 1) {
            RewardsBuffer->SetActive(false);
            return;
        }
        std::int32_t Size = GR_Main::GiScalePixels(20);
        std::int32_t VisibleCount = pas::idiv(RewardsBuffer->ClientSize.X - 2, Size);
        if (pas::list_count(Ship->AwardIds) <= VisibleCount) {
            Spacing = Size;
        } else {
            VisibleCount = std::min<std::int32_t>(30, pas::list_count(Ship->AwardIds));
            Spacing = pas::real_divide(RewardsBuffer->ClientSize.X - 2 - Size, VisibleCount - 1);
        }
        {
            GI_GraphBuf::TGraphBufGI* cpp_with = RewardsBuffer;
            cpp_with->SetActive(true);
            cpp_with->SetImageKindX(GI_Main::ikxLeft);
            cpp_with->SetImageKindY(GI_Main::ikyBottom);
            cpp_with->GraphBuf->AllocateRgbaTight(std::max<std::int64_t>(static_cast<std::int64_t>(cpp_with->ClientSize.X), System::Round(static_cast<long double>(VisibleCount) * Spacing + Size - Spacing)) + 2, Size + 2);
            cpp_with->MouseMoveCallback = pas::bind_method<&TfScaner::RewardsMouseMove>(this);
            cpp_with->MouseLeaveCallback = pas::bind_method<&TfScaner::RewardMouseLeave>(this);
            cpp_with->LeftButtonDownCallback = pas::bind_method<&TfScaner::RewardsMouseDown>(this);
            cpp_with->GraphBuf->ClearPixels();
            cpp_with->SourceHasPerPixelAlpha = true;
        }
        GR_GraphBuf::TGraphBufGR* Image = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        GR_GraphBuf::TGraphBufGR* Shadow = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        std::int32_t I = std::max<std::int32_t>(0, pas::list_count(Ship->AwardIds) - VisibleCount);
        std::int32_t DrawIndex = 0;
        while (I < pas::list_count(Ship->AwardIds)) {
            AwardId = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->AwardIds, I)));
            if (AwardId < 10) {
                Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_0", pas::wide_int_to_str(static_cast<std::int32_t>(AwardId))});
            } else {
                Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_", pas::wide_int_to_str(static_cast<std::int32_t>(AwardId))});
            }
            GI_GI::LoadGiByPathIntoGraphBuf(Path, Image);
            if (static_cast<std::uint32_t>(Image->Width) >= static_cast<std::uint32_t>(Image->Height)) {
                Image->RescaleRgba(Size, System::Round(pas::real_divide(Size, static_cast<std::uint32_t>(Image->Width)) * static_cast<std::uint32_t>(Image->Height)), 5);
            } else {
                Image->RescaleRgba(System::Round(pas::real_divide(Size, static_cast<std::uint32_t>(Image->Height)) * static_cast<std::uint32_t>(Image->Width)), Size, 5);
            }
            Shadow->AllocateRgbaTight(Image->Width, Image->Height);
            GR_GraphBuf::TGraphBufGR_CopyRect32(Shadow, ClassesImports::Point(0, 0), Image, ClassesImports::Rect(0, 0, Image->Width, Image->Height));
            GR_GraphBuf::TGraphBufGR_MakeShadow(Shadow);
            if (Image->Height <= Size && Image->Width + System::Round(static_cast<long double>(DrawIndex) * Spacing) <= RewardsBuffer->GraphBuf->Width) {
                GR_GraphBuf::TGraphBufGR_BlendRect32(RewardsBuffer->GraphBuf, ClassesImports::Point(System::Round(static_cast<long double>(DrawIndex) * Spacing) + 2, 2), Shadow, ClassesImports::Rect(0, 0, Image->Width, Image->Height));
                GR_GraphBuf::TGraphBufGR_BlendRect32(RewardsBuffer->GraphBuf, ClassesImports::Point(System::Round(static_cast<long double>(DrawIndex) * Spacing), 0), Image, ClassesImports::Rect(0, 0, Image->Width, Image->Height));
            }
            ++I;
            ++DrawIndex;
        }
        pas::free(Image);
        pas::free(Shadow);
    }

    void TfScaner::RewardsMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        float Spacing{};
        aShip::TShip* Ship = ShipToInspect;
        std::int32_t Size = GR_Main::GiScalePixels(20);
        std::int32_t VisibleCount = pas::idiv(RewardsBuffer->ClientSize.X - 2, Size);
        if (pas::list_count(Ship->AwardIds) <= VisibleCount) {
            Spacing = Size;
        } else {
            VisibleCount = std::min<std::int32_t>(30, pas::list_count(Ship->AwardIds));
            Spacing = pas::real_divide(RewardsBuffer->ClientSize.X - 2 - Size, VisibleCount - 1);
        }
        std::int32_t X = Sender->ToLocalPoint(Point).X;
        std::int32_t I = std::max<std::int32_t>(0, pas::list_count(Ship->AwardIds) - VisibleCount);
        std::int32_t DrawIndex = 0;
        while (I < pas::list_count(Ship->AwardIds)) {
            if (X >= System::Round(static_cast<long double>(DrawIndex) * Spacing) && X < System::Round(static_cast<long double>(DrawIndex + 1) * Spacing)) {
                break;
            }
            ++DrawIndex;
            ++I;
        }
        if (I >= pas::list_count(Ship->AwardIds)) {
            I = pas::list_count(Ship->AwardIds) - 1;
        }
        std::uint8_t AwardId = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->AwardIds, I)));
        if (pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr) {
            ShowRewardInfo(pas::checked_cast<aNormalShip::TNormalShip*>(Ship), AwardId);
        }
    }

    void TfScaner::RewardMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        HideRewardInfo();
    }

    void TfScaner::ShowRewardInfo(aNormalShip::TNormalShip* Ship, std::int32_t AwardId) {
        aConst::TRewardInfo cpp_result{};
        aConst::TRewardInfo cpp_result_2{};
        WindowsSdk::TPoint CursorPoint{};
        pas::WideString Path{};
        if (HoveredRewardId == AwardId) {
            return;
        }
        HoveredRewardId = AwardId;
        RewardWindow->SetActive(true);
        CursorPoint = GetCursorPoint();
        RewardWindow->SetPosition(ClassesImports::Point(CursorPoint.X - RewardWindow->ClientSize.X - 50, std::max<std::int32_t>(10, CursorPoint.Y - RewardWindow->ClientSize.Y - 20)));
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
        TfScaner::RefreshRewardHint(nullptr, 0);
    }

    void TfScaner::HideRewardInfo() {
        HoveredRewardId = -1;
        RewardWindow->SetActive(false);
    }

    void TfScaner::AdvancePanelSlide(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_MessageLoop::TObjectGI* Panel = GetByName(u"PanelRight"_wref.get());
        std::int32_t X = Panel->LocalPosition.X + PanelSlideStep;
        if (X >= PanelSlideEndX) {
            X = PanelSlideEndX;
            if (PanelSlideTimer != nullptr) {
                CancelCallbackTimer(PanelSlideTimer);
                PanelSlideTimer = nullptr;
            }
            RootUiObject->UpdateAbsolutePosition();
            RootUiObject->UpdateSubtreeHitBounds();
        }
        Panel->SetPosition(ClassesImports::Point(X, Panel->LocalPosition.Y));
    }

    void TfScaner::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        GR_Main::AuxRenderBuffer->Clear();
        GlobalsV::RequestedScreenId = GlobalsV::ScannerReturnScreenId;
        RequestClose(1);
    }

    void TfScaner::RewardsMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (GlobalsV::UiRuntimeFlag && static_cast<std::uint8_t>(ShipToInspect->InHyperspace ^ 1) && RewardsBuffer->Active) {
            RewardWindow->SetActive(false);
            Globals::AwardSubject = ShipToInspect;
            aGalaxy::Galaxy->CheckIntegrityChecksum(337);
            if (!fRewards::RunRewards(this, true)) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum(338);
                RequestClose(2);
            } else {
                aGalaxy::Galaxy->PrimeIntegrityChecksum(338);
            }
        }
    }

    void TfScaner::ShowPropertyInfo(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString ImagePath{};
        pas::WideString Title{};
        pas::WideString Text{};
        pas::WideString Description{};
        pas::WideString Caption{};
        aShip::TPilotSkill Skill{};
        aNormalShip::TNormalShip* Ship{};
        GI_Window::TWindowGI* Window{};
        std::uint8_t UseAnchor{};
        WindowsSdk::TPoint Position{};
        aShip::PCustomShipInfo Info{};
        if (PanelSlideTimer == nullptr) {
            UseAnchor = false;
            Position = ClassesImports::Point(10, 10);
            ImagePath = pas::WideString();
            if (Sender->UserValue == -1) {
                if (Sender->UserData != 0) {
                    Info = reinterpret_cast<aShip::PCustomShipInfo>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserData)));
                    aScript::RunCustomShipInfoActionCode(Info, 0x00000031, ShipToInspect, nullptr, nullptr, 0);
                    Description = Info->Description;
                    if (Description == u"") {
                        Description = aConst::LocalizedColorText(pas::concat_wide({u"ShipInfo.AddInfo.CustomInfos.", Info->TypeName, u".Description"}));
                    }
                    aMyFunction::ReplaceTextToken(Description, u"<Data1>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<Data2>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<Data3>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData1>"_w, Info->TextData1, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData2>"_w, Info->TextData2, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData3>"_w, Info->TextData3, u"<color=255,240,100>"_w);
                    Caption = aConst::LocalizedColorText(pas::concat_wide({u"ShipInfo.AddInfo.CustomInfos.", Info->TypeName, u".Name"}));
                    aMyFunction::ReplaceTextToken(Caption, u"<Data1>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<Data2>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<Data3>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<TextData1>"_w, Info->TextData1, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<TextData2>"_w, Info->TextData2, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<TextData3>"_w, Info->TextData3, u"<color=255,240,100>"_w);
                    Sender->HelpText = pas::concat_wide({Caption, u"~", Description});
                }
                {
                    GI_GraphBuf::TGraphBufGI* RankImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"_wref.get()));
                    RankImage->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"AI_", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Sender->UserIndex))), u"L"}), RankImage->GraphBuf);
                    RankImage->SetImageKindX(GI_Main::ikxCenter);
                    RankImage->SetImageKindY(GI_Main::ikyCenter);
                }
                Title = EC_Str::ExtractDelimitedPartW(Sender->HelpText, 0, u"~"_wref.get());
                Text = EC_Str::ExtractDelimitedRangeW(Sender->HelpText, 1, EC_Str::CountDelimitedPartsW(Sender->HelpText, u"~"_wref.get()) - 1, u"~"_wref.get());
            } else if (GetByName(u"ForsageBut"_wref.get()) == Sender) {
                UseAnchor = true;
                {
                    GI_GraphBuf::TGraphBufGI* RankImage_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"_wref.get()));
                    RankImage_2->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"ForsageIcon"}), RankImage_2->GraphBuf);
                    RankImage_2->SetImageKindX(GI_Main::ikxCenter);
                    RankImage_2->SetImageKindY(GI_Main::ikyCenter);
                }
                Title = aConst::LocalizedColorText(u"ShipInfo.Forsage.Name"_wref.get());
                Text = aConst::LocalizedColorText(u"ShipInfo.Forsage.Text"_wref.get());
                if (GlobalsV::DynamicTipsPos) {
                    Position = ClassesImports::Point(Sender->HitTestBounds.Left - 10, Sender->HitTestBounds.Top + Sender->ClientSize.Y + 10);
                }
            } else if (pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr) {
                if (Sender->ControlName == u"RankI") {
                    if (pas::class_cast_if<aNormalShip::TNormalShip*>(ShipToInspect) != nullptr) {
                        if (pas::class_cast_if<aTranclucator::TTranclucator*>(ShipToInspect) != nullptr) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank4"});
                        } else if (pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank == 0) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Rank1"});
                        } else if (pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank == 1) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Rank2"});
                        } else if (pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank == 2) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Rank3"});
                        } else if (pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank == 3) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Rank4"});
                        } else if (pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank == 4) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Rank5"});
                        } else if (pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank == 5) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Rank6"});
                        } else if (pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank == 6) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Rank7"});
                        } else if (pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect)->Rank == 7) {
                            ImagePath = pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Rank8"});
                        }
                    }
                    {
                        GI_GraphBuf::TGraphBufGI* RankImage_3 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"_wref.get()));
                        RankImage_3->SetActive(ImagePath != u"");
                        RankImage_3->SourceHasPerPixelAlpha = true;
                        if (ImagePath != u"") {
                            GI_GI::LoadGiByPathIntoGraphBuf(ImagePath, RankImage_3->GraphBuf);
                            RankImage_3->SetImageKindX(GI_Main::ikxCenter);
                            RankImage_3->SetImageKindY(GI_Main::ikyCenter);
                        }
                    }
                    if (pas::class_cast_if<aNormalShip::TNormalShip*>(ShipToInspect) != nullptr) {
                        Ship = pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect);
                        Title = ([&] {
                            pas::WideString rankLongName = Ship->GetRankLongName();
                            pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(std::move(rankLongName), std::move(infoNameColorTag));
                        }());
                        Text = Ship->GetRankDescription();
                        if (Ship->Rank != 7) {
                            if (Ship->GetRankPointsToNextRank() > 0) {
                                Text = pas::concat_wide({Text, u" ", ([&] {
                                    pas::WideString nextRankName = Ship->GetNextRankName();
                                    pas::WideString intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(Ship->GetRankPointsToNextRank()));
                                    pas::WideString localizedText = aConst::LocalizedText(u"Rank.NextRankText"_wref.get());
                                    return aMyFunction::FormatText2(std::move(localizedText), u"<color=255,240,100>"_w, u"<NextRank>"_w, std::move(nextRankName), u"<WarPoints>"_w, std::move(intToStr));
                                }())});
                            } else {
                                Text = pas::concat_wide({Text, u" ", ([&] {
                                    pas::WideString nextRankName_2 = Ship->GetNextRankName();
                                    pas::WideString localizedText_2 = aConst::LocalizedText(u"Rank.NextRankGetText"_wref.get());
                                    return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<NextRank>"_w, std::move(nextRankName_2));
                                }())});
                            }
                        }
                    }
                } else if (Sender->ControlName == u"RankI2" && pas::class_cast_if<aNormalShip::TNormalShip*>(ShipToInspect) != nullptr) {
                    Ship = pas::checked_cast<aNormalShip::TNormalShip*>(ShipToInspect);
                    ImagePath = EC_Str::ExtractDelimitedPartW(fScaner::GetPirateRankSmallImagePath(Ship->PirateRank), 1, u","_wref.get());
                    {
                        GI_GraphBuf::TGraphBufGI* RankImage_4 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"_wref.get()));
                        RankImage_4->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(ImagePath, RankImage_4->GraphBuf);
                        RankImage_4->SetImageKindX(GI_Main::ikxCenter);
                        RankImage_4->SetImageKindY(GI_Main::ikyCenter);
                    }
                    Title = ([&] {
                        pas::WideString pirateRankLongName = Ship->GetPirateRankLongName();
                        pas::WideString infoNameColorTag_2 = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(std::move(pirateRankLongName), std::move(infoNameColorTag_2));
                    }());
                    Text = Ship->GetPirateRankDescription();
                    Text = pas::concat_wide({Text, u"\r\n", ([&] {
                        pas::WideString intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(Ship->GetPirateRankPointsToNextRank()));
                        pas::WideString localizedText_3 = aConst::LocalizedText(u"RankPirate.NextRankText"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_3), u"<color=255,240,100>"_w, u"<WarPoints>"_w, std::move(intToStr_2));
                    }())});
                }
            } else if (pas::class_cast_if<GI_Zone::TZoneGI*>(Sender) != nullptr) {
                {
                    GI_GraphBuf::TGraphBufGI* RankImage_5 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"_wref.get()));
                    RankImage_5->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Skill", pas::wide_int_to_str(EC_Str::ExtractDigitsToIntW(Sender->ControlName) + 1)}), RankImage_5->GraphBuf);
                    RankImage_5->SetImageKindX(GI_Main::ikxCenter);
                    RankImage_5->SetImageKindY(GI_Main::ikyCenter);
                }
                Skill = static_cast<aShip::TPilotSkill>(EC_Str::ExtractDigitsToIntW(Sender->ControlName));
                Title = ([&] {
                    pas::WideString localizedText_4 = aConst::LocalizedText(pas::concat_wide({u"Skills.", aConst::SkillConfigNames[Skill], u".Name"}));
                    pas::WideString infoNameColorTag_3 = aMyFunction::InfoNameColorTag;
                    return aMyFunction::WrapTextInColor(std::move(localizedText_4), std::move(infoNameColorTag_3));
                }());
                Text = ([&] {
                    pas::WideString intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(aConst::PilotSkillEffects[ShipToInspect->GetEffectiveSkillLevel(Skill, false) & 0x0000007f][Skill]));
                    pas::WideString localizedText_5 = aConst::LocalizedText(pas::concat_wide({u"Skills.", aConst::SkillConfigNames[Skill], u".Text"}));
                    return aMyFunction::FormatText1(std::move(localizedText_5), u"<color=255,240,100>"_w, u"<SkillValue>"_w, std::move(intToStr_3));
                }());
                aMyFunction::ReplaceTextToken(Text, u"<SkillLevel>"_w, pas::wide_int_to_str(ShipToInspect->GetEffectiveSkillLevel(Skill, false) & 0x0000007f), u"<color=255,240,100>"_w);
                if (Skill == aShip::psTechnical) {
                    aMyFunction::ReplaceTextToken(Text, u"<N>"_w, pas::wide_int_to_str(ShipToInspect->GetSatelliteLimit()), u"<color=255,240,100>"_w);
                }
                if (Skill == aShip::psTrading) {
                    aMyFunction::ReplaceTextToken(Text, u"<SkillValue2>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aConst::TradingSkillSalePercent[ShipToInspect->GetEffectiveSkillLevel(Skill, false) & 0x0000007f])), u"<color=255,240,100>"_w);
                }
                if (Skill == aShip::psLeadership) {
                    aMyFunction::ReplaceTextToken(Text, u"<SkillValue2>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aConst::LeadershipExperiencePercent[ShipToInspect->GetEffectiveSkillLevel(Skill, false) & 0x0000007f])), u"<color=255,240,100>"_w);
                }
                if (ShipToInspect->GetBaseSkillLevel(Skill) < 6) {
                    Text = pas::concat_wide({Text, u"\r\n", u"\r\n", ([&] {
                        pas::WideString intToStr_4 = pas::wide_int_to_str(static_cast<std::int32_t>(aConst::SkillTrainingCosts[ShipToInspect->BaseSkills[Skill] + 1][Skill]));
                        pas::WideString localizedText_6 = aConst::LocalizedText(u"Skills.PointForNextLevel"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_6), u"<color=255,240,100>"_w, u"<PointForNextLevel>"_w, std::move(intToStr_4));
                    }())});
                }
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankName"_wref.get()))->SetText(Title);
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankText"_wref.get()))->SetText(Text);
            Window = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"RankWnd"_wref.get()));
            Window->SetPosition(ClassesImports::Point(Window->LocalPosition.X, std::max<std::int32_t>(10, Sender->HitTestBounds.Top - Sender->ClientSize.Y / 3 - 60)));
            Window->SetActive(true);
            Window->Invalidate();
            {
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankText"_wref.get()));
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankName"_wref.get()));
                fShip2::TfShip2::LayoutItemInfo(Window, cpp_arg_2, cpp_arg, true, true, 0);
            }
            {
                GI_Label::TLabelGI* RankName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankName"_wref.get()));
                RankName->SetSize(ClassesImports::Point(Window->ClientSize.X - RankName->LocalPosition.X - Window->WorkSubRect.Right, RankName->ClientSize.Y));
            }
            if (!UseAnchor) {
                Window->SetPosition(ClassesImports::Point(Globals::ShipScreen->PropertyHintRightEdge - Window->ClientSize.X, Window->LocalPosition.Y));
            } else {
                Window->SetPosition(Position);
            }
            if (PropertyHintTimer != nullptr) {
                CancelCallbackTimer(PropertyHintTimer);
                PropertyHintTimer = nullptr;
            }
        }
    }

    void TfScaner::HidePropertyInfo(GI_MessageLoop::TObjectGI* Sender) {
        GetByName(u"RankWnd"_wref.get())->SetActive(false);
    }

    void TfScaner::RefreshRewardHint(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
    }

    void TfScaner::CountCargoEntries() {
        std::int32_t I{};
        std::uint8_t Kind{};
        aItem::TEquipment* Item{};
        CargoEntryCount = 0;
        for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(7); ++Kind) {
            if (ShipToInspect->CargoGoods[Kind].Count > 0) {
                ++CargoEntryCount;
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ShipToInspect->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(ShipToInspect->Inventory, I);
            if (Item->EquippedFlag == 0 && ShipToInspect->GetHull() != Item) {
                ++CargoEntryCount;
            }
        }
        CargoEntryCount += pas::list_count(ShipToInspect->Artefacts);
    }

    std::uint8_t TfScaner::GetCargoEntry(std::int32_t Index, aConst::TItemType& ItemType, aItem::TItem*& Item) {
        std::int32_t I{};
        aConst::TItemType Kind{};
        aItem::TEquipment* Equipment{};
        for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Food, aConst::t_Narcotics); cpp_range.next(Kind); ) {
            if (ShipToInspect->CargoGoods[Kind].Count > 0) {
                --Index;
                if (Index < 0) {
                    Item = nullptr;
                    ItemType = Kind;
                    return true;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ShipToInspect->Inventory) - 1); cpp_range_2.next(I); ) {
            Equipment = pas::list_at<aItem::TEquipment>(ShipToInspect->Inventory, I);
            if (Equipment->EquippedFlag == 0 && ShipToInspect->GetHull() != Equipment) {
                --Index;
                if (Index < 0) {
                    Item = Equipment;
                    ItemType = Equipment->ItemType;
                    return true;
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(ShipToInspect->Artefacts) - 1); cpp_range_3.next(I); ) {
            Equipment = pas::list_at<aItem::TEquipment>(ShipToInspect->Artefacts, I);
            if (Equipment->EquippedFlag == 0) {
                --Index;
                if (Index < 0) {
                    Item = Equipment;
                    ItemType = Equipment->ItemType;
                    return true;
                }
            }
        }
        Item = nullptr;
        ItemType = aConst::t_Food;
        return false;
    }

    void TfScaner::Update() {
        std::int32_t I{};
        std::int32_t SlotIndex{};
        std::int32_t SlotCount{};
        std::uint8_t CanBoost{};
        aItem::TItem* Item{};
        aConst::TItemType CargoKind{};
        aItem::TArtefact* Artefact{};
        pas::WideString Text{};
        GI_Image::TImageGI* Image{};
        std::int32_t Stage = 0;
        try {
            ShipToInspect->RefreshAssignedItemSlots();
            Stage = 1;
            Text = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(aShip::TShip_GetDefensePercent(ShipToInspect) & 0x0000007f), "%"}));
            Text = pas::concat_wide({Text, u" + ", aMyFunction::WrapTextInColor(pas::wide_int_to_str(aShip::TShip_GetArmor(ShipToInspect)), pas::WideString())});
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IDef"_wref.get()))->SetText(Text);
            Stage = 2;
            {
                const pas::WideString& intToStr = pas::wide_int_to_str(ShipToInspect->CalculateMass());
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IMass"_wref.get()));
                cpp_arg->SetText(intToStr);
            }
            Stage = 3;
            if (ShipToInspect->CalculateSpeed() <= 0) {
                Text = u"<color=255,0,0>"_w;
            } else {
                Text = pas::WideString();
            }
            {
                const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(pas::wide_int_to_str(ShipToInspect->CalculateSpeed()), Text);
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISpeed"_wref.get()));
                cpp_arg_2->SetText(wrapTextInColor);
            }
            Stage = 4;
            if (ShipToInspect->GetCargoFreeSpace() < 0) {
                Text = u"<color=255,0,0>"_w;
            } else {
                Text = pas::WideString();
            }
            {
                const pas::WideString& wrapTextInColor_2 = aMyFunction::WrapTextInColor(pas::wide_int_to_str(ShipToInspect->GetCargoFreeSpace()), Text);
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IEmpty"_wref.get()));
                cpp_arg_3->SetText(wrapTextInColor_2);
            }
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Left"_wref.get()))->SetDisabled(!(CargoOffset > 0));
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Right"_wref.get()))->SetDisabled(!(CargoOffset + VisibleCargoCount <= CargoEntryCount));
            HoveredItemAnimation = nullptr;
            Stage = 5;
            {
                GI_Image::TImageGI* HullSet = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"HullSet"_wref.get()));
                HullSet->SetActive(ShipToInspect->GetHull()->HasMicroModule());
                if (HullSet->Active) {
                    HullSet->SetImagePath(pas::concat_wide({u"GI,", aItem::GetMicroModuleBitmapResourceName(ShipToInspect->GetHull()->MicroModuleIndex - 1), u"Set"}));
                }
            }
            Stage = 6;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
                SlotCount = ShipToInspect->GetSlotCountForItemType(aConst::EquipmentSlotLayouts[I].ItemType);
                Stage = 7;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_2.next(SlotIndex); ) {
                    Item = ShipToInspect->FindEquippedItemInSlot(aConst::EquipmentSlotLayouts[I].ItemType, SlotIndex);
                    Stage = 8;
                    Image = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"i"})));
                    Stage = 9;
                    if (Image->UserValue == 0) {
                        Image->UserValue = Image->LocalPosition.X + Image->ClientSize.X / 2;
                        Image->UserIndex = Image->LocalPosition.Y + Image->ClientSize.Y / 2;
                    }
                    Stage = 10;
                    if (Item == nullptr) {
                        Image->SetActive(false);
                        Image->SetImagePath(pas::WideString());
                        GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"Set"}))->SetActive(false);
                    } else {
                        Image->SetActive(true);
                        Image->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Item), u"i"}));
                        Image->SetImageKindX(GI_Main::ikxCenter);
                        Image->SetImageKindY(GI_Main::ikyCenter);
                        {
                            GI_Image::TImageGI* cpp_with_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"Set"})));
                            cpp_with_2->SetActive(reinterpret_cast<aItem::TEquipment*>(Item)->HasMicroModule());
                            if (cpp_with_2->Active) {
                                cpp_with_2->SetImagePath(pas::concat_wide({u"GI,", aItem::GetMicroModuleBitmapResourceName(reinterpret_cast<aItem::TEquipment*>(Item)->MicroModuleIndex - 1), u"Set"}));
                            }
                        }
                    }
                    Stage = 11;
                    if (GlobalsV::AnimItem && Item != nullptr) {
                        Stage = 12;
                        {
                            GI_GAI::TgaiGI* cpp_with_3 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"anim"})));
                            cpp_with_3->UserData = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Image));
                            cpp_with_3->SetPosition(Image->LocalPosition);
                            cpp_with_3->SetImagePath(pas::concat_wide({fEquipmentShop::GetShopItemIconName(Item), u"a"}));
                            cpp_with_3->SequenceIndex = 0;
                            cpp_with_3->SetActive(false);
                            Stage = 13;
                            if (cpp_with_3->UserState != 0) {
                                Image->SetActive(false);
                                cpp_with_3->UpdateAutoGeometry();
                                cpp_with_3->SetSequenceFrame(std::min<std::int64_t>(static_cast<std::int64_t>(static_cast<std::uint32_t>(cpp_with_3->UserState)), static_cast<std::int64_t>(cpp_with_3->SequenceFrameCount - 1)));
                                cpp_with_3->SetActive(true);
                                cpp_with_3->StopAutoPlayback();
                            }
                            Stage = 14;
                        }
                    } else {
                        Stage = 15;
                        GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"anim"}))->SetActive(false);
                    }
                    Stage = 16;
                    GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"a"}))->SetActive(false);
                    {
                        std::uint8_t cpp_arg_4 = Item != nullptr && aShip::TShip_IsEquipmentUsable(ShipToInspect, reinterpret_cast<aItem::TEquipment*>(Item));
                        GI_MessageLoop::TObjectGI* byName = GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"n"}));
                        byName->SetActive(cpp_arg_4);
                    }
                    {
                        std::uint8_t cpp_arg_5 = Item != nullptr && static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(ShipToInspect, reinterpret_cast<aItem::TEquipment*>(Item)) ^ 1);
                        GI_MessageLoop::TObjectGI* byName_2 = GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"b"}));
                        byName_2->SetActive(cpp_arg_5);
                    }
                    GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"Ex"}))->SetActive(false);
                    Stage = 17;
                }
                if (aConst::EquipmentSlotLayouts[I].ItemType == aConst::t_Weapon1) {
                    Stage = 18;
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(SlotCount, 4); cpp_range_3.next(SlotIndex); ) {
                        pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"z"})))->ZoneMouseDownCallback = nullptr;
                    }
                } else {
                    Stage = 19;
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(SlotCount, 0); cpp_range_4.next(SlotIndex); ) {
                        pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"z"})))->ZoneMouseDownCallback = nullptr;
                    }
                }
            }
            Stage = 20;
            SlotCount = ShipToInspect->GetSlotCountForItemType(aConst::t_Artefact);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_5.next(SlotIndex); ) {
                Stage = 21;
                Artefact = pas::checked_cast<aItem::TArtefact*>(ShipToInspect->FindEquippedItemInSlot(aConst::t_Artefact, SlotIndex));
                CanBoost = Artefact != nullptr && Artefact->BrokenFlag == 0 && ShipToInspect->CanBoostArtefact(Artefact->ItemType, nullptr, false);
                Stage = 22;
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "n"})))->SetActive(Artefact != nullptr && Artefact->BrokenFlag == 0 && static_cast<std::uint8_t>(CanBoost ^ 1));
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "b"})))->SetActive(Artefact != nullptr && static_cast<std::uint8_t>(Artefact->BrokenFlag) && static_cast<std::uint8_t>(CanBoost ^ 1));
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "a"})))->SetActive(false);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "i"})))->SetActive(Artefact != nullptr);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "Ex"})))->SetActive(CanBoost);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "off"})))->SetActive(false);
                Stage = 23;
                {
                    GI_Image::TImageGI* cpp_with_4 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "i"}))));
                    Stage = 24;
                    if (Artefact == nullptr) {
                        cpp_with_4->SetImagePath(pas::WideString());
                    } else {
                        cpp_with_4->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Artefact), u"s"}));
                        cpp_with_4->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_4->SetImageKindY(GI_Main::ikyCenter);
                    }
                }
                Stage = 25;
                {
                    GI_Zone::TZoneGI* cpp_with_5 = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "z"}))));
                    cpp_with_5->SetActive(Artefact != nullptr);
                }
            }
            Stage = 26;
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(SlotCount, aConst::DefaultHullSlotCounts[8] - 1); cpp_range_6.next(SlotIndex); ) {
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "n"})))->SetActive(false);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "b"})))->SetActive(false);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "a"})))->SetActive(false);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "i"})))->SetActive(false);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "z"})))->SetActive(false);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "Ex"})))->SetActive(false);
                GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(SlotIndex), "off"})))->SetActive(true);
            }
            Stage = 27;
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, VisibleCargoCount - 1); cpp_range_7.next(I); ) {
                GI_Image::TImageGI* cpp_with_6 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "i"}))));
                if (!GetCargoEntry(CargoOffset + I, CargoKind, Item)) {
                    cpp_with_6->SetImagePath(pas::WideString());
                } else if (pas::in_range(CargoKind, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
                    cpp_with_6->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(CargoKind)}));
                    cpp_with_6->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_6->SetImageKindY(GI_Main::ikyCenter);
                } else {
                    cpp_with_6->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Item), u"s"}));
                    cpp_with_6->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_6->SetImageKindY(GI_Main::ikyCenter);
                }
            }
            Stage = 28;
            BuildAdditionalInfo();
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TfScaner.Update, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfScaner::ScrollCargoLeft(GI_MessageLoop::TObjectGI* Sender) {
        --CargoOffset;
        Update();
    }

    void TfScaner::ScrollCargoRight(GI_MessageLoop::TObjectGI* Sender) {
        ++CargoOffset;
        Update();
    }

    void TfScaner::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_ESCAPE || Key == 'I' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1)) {
            GR_Main::AuxRenderBuffer->Clear();
            GlobalsV::RequestedScreenId = GlobalsV::ScannerReturnScreenId;
            RequestClose(1);
        } else if (Key == WindowsSdk::VK_LEFT) {
            if (CargoOffset > 0) {
                --CargoOffset;
                Update();
            }
        } else if (Key == WindowsSdk::VK_RIGHT && CargoOffset + VisibleCargoCount <= CargoEntryCount) {
            ++CargoOffset;
            Update();
        }
    }

    void TfScaner::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            if (CargoOffset > 0) {
                --CargoOffset;
                Update();
            }
        } else if (Delta == -WindowsSdk::WHEEL_DELTA && CargoOffset + VisibleCargoCount <= CargoEntryCount) {
            ++CargoOffset;
            Update();
        }
    }

    void TfScaner::MainPanelMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (PanelSlideTimer != nullptr) {
            return;
        }
        if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PanelLeft"_wref.get()))->HitTestPixel(Point)) {
            return;
        }
        if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RightOpen"_wref.get()))->HitTestPixel(Point)) {
            return;
        }
        if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Left"_wref.get()))->ContainsPoint(Point)) {
            return;
        }
        if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Right"_wref.get()))->ContainsPoint(Point)) {
            return;
        }
        if (pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"GateZone"_wref.get()))->ContainsPoint(Point)) {
            return;
        }
        if (GetByName(u"CenterNormalImage"_wref.get())->ContainsPoint(Point)) {
            return;
        }
        if (GetByName(u"CenterDamageImage"_wref.get())->ContainsPoint(Point)) {
            return;
        }
        CloseClicked(nullptr);
    }

    void TfScaner::UpdateItemHover() {
        aItem::TItem* Item{};
        aConst::TItemType CargoKind{};
        std::int32_t I{};
        std::int32_t SlotIndex{};
        std::int32_t SlotCount{};
        GI_Window::TWindowGI* Window{};
        WindowsSdk::TPoint Position{};
        WindowsSdk::TPoint AnchorSize{};
        std::uint8_t Found = false;
        GI_GAI::TgaiGI* Animation = nullptr;
        Position = ClassesImports::Point(0, 0);
        AnchorSize = ClassesImports::Point(0, 0);
        std::uint8_t CenterX = false;
        std::uint8_t CenterY = false;
        if (!Found) {
            SlotCount = ShipToInspect->GetSlotCountForItemType(aConst::t_Artefact);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range.next(SlotIndex); ) {
                Item = ShipToInspect->FindEquippedItemInSlot(aConst::t_Artefact, SlotIndex);
                if (Item != nullptr) {
                    GI_Zone::TZoneGI* cpp_with = ArtefactZones[SlotIndex];
                    if (cpp_with->HitTest(GetCursorPoint())) {
                        CenterX = true;
                        Position = ClassesImports::Point(cpp_with->HitTestBounds.Left + cpp_with->ClientSize.X / 2, cpp_with->HitTestBounds.Top + cpp_with->ClientSize.Y);
                        ShowItemInfo(Item);
                        Found = true;
                        break;
                    }
                }
            }
        }
        if (!Found) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 7); cpp_range_2.next(I); ) {
                SlotCount = ShipToInspect->GetSlotCountForItemType(aConst::EquipmentSlotLayouts[I].ItemType);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_3.next(SlotIndex); ) {
                    Item = ShipToInspect->FindEquippedItemInSlot(aConst::EquipmentSlotLayouts[I].ItemType, SlotIndex);
                    if (Item == nullptr) {
                        continue;
                    }
                    {
                        GI_Zone::TZoneGI* cpp_with_2 = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(SlotIndex), u"z"})));
                        if (cpp_with_2->HitTest(GetCursorPoint())) {
                            CenterX = true;
                            Position = ClassesImports::Point(cpp_with_2->ClientSize.X / 2 + cpp_with_2->HitTestBounds.Left, cpp_with_2->HitTestBounds.Top + cpp_with_2->ClientSize.Y);
                            ShowItemInfo(Item);
                            Found = true;
                            if (GlobalsV::AnimItem) {
                                Animation = EquipmentAnimations[I][SlotIndex];
                            }
                            break;
                        }
                    }
                }
            }
        }
        if (!Found) {
            GI_Zone::TZoneGI* S_Hull_0z = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"S_Hull_0z"_wref.get()));
            if (S_Hull_0z->HitTest(GetCursorPoint())) {
                CenterX = true;
                Position = ClassesImports::Point(S_Hull_0z->HitTestBounds.Left + S_Hull_0z->ClientSize.X / 2, S_Hull_0z->HitTestBounds.Top + S_Hull_0z->ClientSize.Y);
                ShowItemInfo(ShipToInspect->GetHull());
                Found = true;
            }
        }
        if (!Found) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, VisibleCargoCount - 1); cpp_range_4.next(I); ) {
                GI_Zone::TZoneGI* cpp_with_4 = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "z"}))));
                if (cpp_with_4->HitTest(GetCursorPoint())) {
                    CenterX = true;
                    Position = ClassesImports::Point(cpp_with_4->HitTestBounds.Left + cpp_with_4->ClientSize.X / 2, cpp_with_4->HitTestBounds.Top + cpp_with_4->ClientSize.Y);
                    if (GetCargoEntry(CargoOffset + I, CargoKind, Item)) {
                        if (pas::in_range(CargoKind, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
                            ShowGoodsInfo(CargoKind);
                        } else {
                            ShowItemInfo(Item);
                        }
                        Found = true;
                        break;
                    }
                }
            }
        }
        if (Position.X != 0 || Position.Y != 0) {
            if (ItemInfoWindow->Active) {
                Window = ItemInfoWindow;
            } else {
                Window = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"InfoHull"_wref.get()));
            }
            if (GlobalsV::DynamicTipsPos) {
                if (CenterX) {
                    Position.X -= Window->ClientSize.X / 2;
                }
                if (Position.X < 0) {
                    Position.X = pas::abs(Position.X) - Window->ClientSize.X;
                }
                if (CenterY) {
                    Position.Y -= Window->ClientSize.Y / 2;
                    if (Window->ClientSize.Y + Position.Y + 10 > GR_Main::GameScreenHeight) {
                        Position.Y = GR_Main::GameScreenHeight - Window->ClientSize.Y - 10;
                    }
                } else if (Window->ClientSize.Y + Position.Y + 10 > GR_Main::GameScreenHeight) {
                    Position.Y = Position.Y - AnchorSize.Y - Window->ClientSize.Y;
                }
                Window->SetPosition(Position);
            } else {
                Window->SetPosition(ClassesImports::Point(10, 10));
            }
        }
        if (static_cast<std::uint8_t>(Found ^ 1) && HideItemTimer == nullptr) {
            ShowItemInfo(nullptr);
        }
        if (Animation != HoveredItemAnimation && HoveredItemAnimation != nullptr) {
            HoveredItemAnimation->StopAutoPlayback();
            HoveredItemAnimation->UserState = HoveredItemAnimation->SequenceFrame;
            HoveredItemAnimation = nullptr;
        }
        if (Animation != HoveredItemAnimation && Animation != nullptr) {
            HoveredItemAnimation = Animation;
            HoveredItemAnimation->UpdateAutoGeometry();
            HoveredItemAnimation->SetSequenceFrame(std::min<std::int64_t>(static_cast<std::int64_t>(static_cast<std::uint32_t>(HoveredItemAnimation->UserState)), static_cast<std::int64_t>(HoveredItemAnimation->SequenceFrameCount - 1)));
            HoveredItemAnimation->SetActive(true);
            HoveredItemAnimation->RestartPlayback();
            reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(HoveredItemAnimation->UserData)))->SetActive(false);
        }
    }

    void TfScaner::AdvanceItemHover(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        UpdateItemHover();
    }

    void TfScaner::HideItemInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        HoveredItem = nullptr;
        if (HideItemTimer != nullptr) {
            CancelCallbackTimer(HideItemTimer);
            HideItemTimer = nullptr;
        }
        GetByName(u"PII"_wref.get())->SetActive(false);
        GetByName(u"InfoHull"_wref.get())->SetActive(false);
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"_wref.get()))->SetText(u""_wref.get());
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"_wref.get()))->SetText(u""_wref.get());
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"_wref.get()))->SetText(u""_wref.get());
    }

    void TfScaner::ShowItemInfo(aItem::TItem* Item) {
        static const pas::Set<0, 255> DurableTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        aItem::TEquipment* Equipment{};
        pas::WideString Portrait{};
        std::int32_t BarWidth{};
        std::int32_t CapWidth{};
        std::int32_t MinimumWidth{};
        Equipment = pas::checked_cast<aItem::TEquipment*>(Item);
        if (Equipment == nullptr) {
            HoveredItem = nullptr;
            if (HideItemTimer != nullptr) {
                CancelCallbackTimer(HideItemTimer);
                HideItemTimer = nullptr;
            }
            HideItemTimer = ScheduleCallbackTimer(100, 99999, pas::bind_method<&TfScaner::HideItemInfo>(this), 0);
        } else if (HoveredItem != Item) {
            HoveredItem = Item;
            if (HideItemTimer != nullptr) {
                CancelCallbackTimer(HideItemTimer);
                HideItemTimer = nullptr;
            }
            if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
                if (Item->ScriptItem != nullptr) {
                    reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(0x00000031, ShipToInspect, nullptr, nullptr, 0);
                }
                if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
                    aScript::RunItemConfigActionCode(Item, 0x00000031, ShipToInspect, nullptr, nullptr, 0);
                }
            }
            if (Item->ItemType == aConst::t_Hull && ShipToInspect->TypeId != aGalaxyStruct::stTranclucator && ShipToInspect->TypeId != aGalaxyStruct::stKling && static_cast<std::uint8_t>(CompactHullInfo ^ 1)) {
                {
                    pas::WideString infoText = Equipment->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, ShipToInspect);
                    aItem::THull* cpp_arg = pas::checked_cast<aItem::THull*>(Item);
                    GI_MessageLoop::TMessageLoopGI* self = this;
                    fEquipmentShop::TfEquipmentShop* equipmentShopScreen = Globals::EquipmentShopScreen;
                    equipmentShopScreen->RefreshHullInfo(self, cpp_arg, std::move(infoText), true);
                }
                ItemInfoWindow->SetActive(false);
                Portrait = ShipToInspect->GetShipPortraitImagePath();
                if (Portrait != u"") {
                    GI_Image::TImageGI* InfoHullImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoHullImage"_wref.get()));
                    InfoHullImage->SetActive(true);
                    InfoHullImage->SetImagePath(u"GraphBuf"_w);
                    {
                        GI_GraphBuf::TGraphBufGI* cpp_with_2 = InfoHullImage->GraphBufControl;
                        cpp_with_2->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(Portrait, 1, u","_wref.get()), cpp_with_2->GraphBuf);
                        if (static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)) {
                            cpp_with_2->GraphBuf->RescaleRgba(cpp_with_2->ClientSize.X, System::Round(pas::real_divide(cpp_with_2->ClientSize.X, static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)), 5);
                        } else {
                            cpp_with_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_2->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width)), cpp_with_2->ClientSize.Y, 5);
                        }
                    }
                    InfoHullImage->SetImageKindX(GI_Main::ikxCenter);
                    InfoHullImage->SetImageKindY(GI_Main::ikyCenter);
                    {
                        Types::TPoint visualCenter = InfoHullImage->GetVisualCenter();
                        Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                        InfoHullImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                    }
                }
            } else {
                ItemInfoWindow->SetActive(true);
                GetByName(u"InfoHull"_wref.get())->SetActive(false);
                {
                    GI_Image::TImageGI* InfoImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"_wref.get()));
                    InfoImage->SetActive(Equipment->ItemType != aConst::t_Hull);
                    if (InfoImage->Active) {
                        InfoImage->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Equipment), u"s"}));
                        InfoImage->SetImageKindX(GI_Main::ikxCenter);
                        InfoImage->SetImageKindY(GI_Main::ikyCenter);
                        {
                            Types::TPoint visualCenter_2 = InfoImage->GetVisualCenter();
                            Types::TPoint itemImageCenter_2 = Globals::ShipScreen->ItemImageCenter;
                            InfoImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_2, visualCenter_2));
                        }
                    }
                }
                {
                    GI_GraphBuf::TGraphBufGI* InfoImage2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoImage2"_wref.get()));
                    InfoImage2->SetActive(Equipment->ItemType == aConst::t_Hull);
                    if (InfoImage2->Active) {
                        Portrait = ShipToInspect->GetShipPortraitImagePath();
                        InfoImage2->SetActive(Portrait != u"");
                        if (InfoImage2->Active) {
                            InfoImage2->SourceHasPerPixelAlpha = true;
                            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(Portrait, 1, u","_wref.get()), InfoImage2->GraphBuf);
                            if (static_cast<std::uint32_t>(InfoImage2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoImage2->GraphBuf->Height)) {
                                InfoImage2->GraphBuf->RescaleRgba(InfoImage2->ClientSize.X - 5, System::Round(pas::real_divide(InfoImage2->ClientSize.X - 5, static_cast<std::uint32_t>(InfoImage2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoImage2->GraphBuf->Height)), 5);
                            } else {
                                InfoImage2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoImage2->ClientSize.Y - 5, static_cast<std::uint32_t>(InfoImage2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoImage2->GraphBuf->Width)), InfoImage2->ClientSize.Y - 5, 5);
                            }
                            InfoImage2->SetImageKindX(GI_Main::ikxCenter);
                            InfoImage2->SetImageKindY(GI_Main::ikyCenter);
                            {
                                Types::TPoint visualCenter_3 = InfoImage2->GetVisualCenter();
                                Types::TPoint itemImageCenter_3 = Globals::ShipScreen->ItemImageCenter;
                                InfoImage2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_3, visualCenter_3));
                            }
                        }
                    }
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"_wref.get()))->SetText(u""_wref.get());
                {
                    const pas::WideString& wrapTextInColor = ([&] {
                        pas::WideString displayName = Equipment->GetDisplayName();
                        pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(std::move(displayName), std::move(infoNameColorTag));
                    }());
                    GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"_wref.get()));
                    cpp_arg_2->SetText(wrapTextInColor);
                }
                {
                    const pas::WideString& infoText_2 = Equipment->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, ShipToInspect);
                    GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"_wref.get()));
                    cpp_arg_3->SetText(infoText_2);
                }
                {
                    const pas::WideString& intToStr = pas::wide_int_to_str(Equipment->Weight);
                    GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"_wref.get()));
                    cpp_arg_4->SetText(intToStr);
                }
                {
                    const pas::WideString& intToStr_2 = pas::wide_int_to_str(Equipment->Cost);
                    GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"_wref.get()));
                    cpp_arg_5->SetText(intToStr_2);
                }
                {
                    GI_Image::TImageGI* EmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"_wref.get()));
                    EmRace->SetImagePath(aConst::GetFactionEmblemPath(aItem::TItem_GetOwnerConfigName(Equipment)));
                    EmRace->SetImageKindX(GI_Main::ikxCenter);
                    EmRace->SetImageKindY(GI_Main::ikyCenter);
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
                            std::int32_t cpp_arg_6 = cpp_left - (InfoDurable_2->GetContentSize().X - 5);
                            std::int32_t y = InfoDurable_2->LocalPosition.Y;
                            InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_6, y));
                        } else {
                            std::int64_t cpp_left_3 = System::Round(BarWidth * pas::real_divide(Equipment->ConditionPercent, 1.0E+2L));
                            std::int32_t cpp_arg_7 = cpp_left_3 - (InfoDurable_2->GetContentSize().X - 5);
                            std::int32_t y_2 = InfoDurable_2->LocalPosition.Y;
                            InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_7, y_2));
                        }
                    }
                    {
                        GI_Image::TImageGI* InfoDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableRight"_wref.get()));
                        {
                            std::int32_t cpp_arg_8 = BarWidth + CapWidth - InfoDurableRight->GetContentSize().X;
                            std::int32_t y_3 = InfoDurableRight->LocalPosition.Y;
                            InfoDurableRight->SetPosition(ClassesImports::Point(cpp_arg_8, y_3));
                        }
                        InfoDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoDurableRight->Parent->LocalPosition.Y));
                        InfoDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableRight->Parent->ClientSize.Y));
                    }
                    {
                        GI_Image::TImageGI* InfoDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableBack"_wref.get()));
                        {
                            std::int32_t cpp_arg_9 = BarWidth + 1 - InfoDurableBack->GetContentSize().X;
                            std::int32_t y_4 = InfoDurableBack->LocalPosition.Y;
                            InfoDurableBack->SetPosition(ClassesImports::Point(cpp_arg_9, y_4));
                        }
                        InfoDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableBack->Parent->ClientSize.Y));
                    }
                }
                fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, ItemNameLabel, ItemDescriptionLabel, true, true, MinimumWidth);
                ItemSizeLabel->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemSizeLabelPosition.Y));
                ItemPriceLabel->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemPriceLabelPosition.Y));
                ItemRaceImage->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
            }
        }
    }

    void TfScaner::ShowGoodsInfo(aConst::TItemType ItemType) {
        HoveredItem = nullptr;
        if (HideItemTimer != nullptr) {
            CancelCallbackTimer(HideItemTimer);
            HideItemTimer = nullptr;
        }
        GetByName(u"PII"_wref.get())->SetActive(true);
        GetByName(u"InfoHull"_wref.get())->SetActive(false);
        GetByName(u"InfoImage2"_wref.get())->SetActive(false);
        {
            GI_Image::TImageGI* InfoImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"_wref.get()));
            InfoImage->SetActive(true);
            InfoImage->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(ItemType)}));
            InfoImage->SetImageKindX(GI_Main::ikxCenter);
            InfoImage->SetImageKindY(GI_Main::ikyCenter);
            {
                Types::TPoint visualCenter = InfoImage->GetVisualCenter();
                Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                InfoImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
            }
        }
        {
            const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(aConst::GoodsMarket[ItemType].DisplayName, aMyFunction::InfoNameColorTag);
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"_wref.get()));
            cpp_arg->SetText(wrapTextInColor);
        }
        {
            const pas::WideString& localizedText = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Text.", SysUtils::IntToStr(ItemType + 1)})));
            GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"_wref.get()));
            cpp_arg_2->SetText(localizedText);
        }
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(ShipToInspect->CargoGoods[ItemType].Count);
            GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"_wref.get()));
            cpp_arg_3->SetText(intToStr);
        }
        if (aPlayer::GetPlayer() == ShipToInspect) {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(ShipToInspect->CargoGoods[ItemType].TotalCost);
            GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"_wref.get()));
            cpp_arg_4->SetText(intToStr_2);
        } else {
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"_wref.get()))->SetText(u"-"_wref.get());
        }
        {
            GI_Image::TImageGI* EmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"_wref.get()));
            EmRace->SetImagePath(aConst::GetFactionEmblemPath(ShipToInspect->GetFactionNameKey()));
            EmRace->SetImageKindX(GI_Main::ikxCenter);
            EmRace->SetImageKindY(GI_Main::ikyCenter);
        }
        GetByName(u"InfoDurable"_wref.get())->Parent->Parent->SetActive(false);
        fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, ItemNameLabel, ItemDescriptionLabel, true, true, 0);
        ItemSizeLabel->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemSizeLabelPosition.Y));
        ItemPriceLabel->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemPriceLabelPosition.Y));
        ItemRaceImage->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
    }

    void TfScaner::UpdateSkills() {
        std::int32_t I{};
        auto UpdateOne = [&](std::int32_t Index, std::int32_t BaseLevel, std::int32_t EffectiveLevel) -> void {
            std::int32_t Gap = 2;
            std::int32_t Step = 5 + Gap;
            std::int32_t Bottom = 43;
            {
                GI_Image::TImageGI* cpp_with = this->SkillImages[Index];
                cpp_with->SetActive(std::min<std::int32_t>(BaseLevel, EffectiveLevel) > 0);
                cpp_with->SetSize(ClassesImports::Point(cpp_with->ClientSize.X, Step * std::min<std::int32_t>(BaseLevel, EffectiveLevel)));
                cpp_with->SetPosition(ClassesImports::Point(cpp_with->LocalPosition.X, this->SkillImageRestTop[Index] + Bottom - cpp_with->ClientSize.Y));
                cpp_with->SetImageKindY(GI_Main::ikyBottom);
            }
            if (BaseLevel < EffectiveLevel) {
                {
                    GI_Panel::TPanelGI* cpp_with_2 = this->SkillPanels[Index];
                    cpp_with_2->SetSize(ClassesImports::Point(cpp_with_2->ClientSize.X, (EffectiveLevel - BaseLevel) * Step));
                    cpp_with_2->SetPosition(ClassesImports::Point(cpp_with_2->LocalPosition.X, this->SkillImageRestTop[Index] + Bottom - Step * EffectiveLevel));
                }
                {
                    GI_Image::TImageGI* cpp_with_3 = this->SkillPositiveImages[Index];
                    cpp_with_3->SetPosition(ClassesImports::Point(cpp_with_3->LocalPosition.X, -Step * (6 - EffectiveLevel) - 1));
                    cpp_with_3->SetActive(true);
                }
            } else {
                this->SkillPositiveImages[Index]->SetActive(false);
            }
            if (BaseLevel > EffectiveLevel) {
                {
                    GI_Panel::TPanelGI* cpp_with_4 = this->SkillPanels[Index];
                    cpp_with_4->SetSize(ClassesImports::Point(cpp_with_4->ClientSize.X, (BaseLevel - EffectiveLevel) * Step));
                    cpp_with_4->SetPosition(ClassesImports::Point(cpp_with_4->LocalPosition.X, this->SkillImageRestTop[Index] + Bottom - Step * BaseLevel));
                }
                {
                    GI_Image::TImageGI* cpp_with_5 = this->SkillNegativeImages[Index];
                    cpp_with_5->SetPosition(ClassesImports::Point(cpp_with_5->LocalPosition.X, -Step * (6 - BaseLevel) - 1));
                    cpp_with_5->SetActive(true);
                }
            } else {
                this->SkillNegativeImages[Index]->SetActive(false);
            }
        };
        {
            GI_Label::TLabelGI* cpp_with = FreeSkillPointsLabel;
            cpp_with->SetText(pas::wide_int_to_str(ShipToInspect->FreeExperience));
        }
        UpdateOne(0, ShipToInspect->GetBaseSkillLevel(aShip::psAccuracy), ShipToInspect->GetEffectiveSkillLevel(aShip::psAccuracy, false) & 0x0000007f);
        UpdateOne(1, ShipToInspect->GetBaseSkillLevel(aShip::psManeuverability), ShipToInspect->GetEffectiveSkillLevel(aShip::psManeuverability, false) & 0x0000007f);
        UpdateOne(2, ShipToInspect->GetBaseSkillLevel(aShip::psTechnical), ShipToInspect->GetEffectiveSkillLevel(aShip::psTechnical, false) & 0x0000007f);
        UpdateOne(3, ShipToInspect->GetBaseSkillLevel(aShip::psTrading), ShipToInspect->GetEffectiveSkillLevel(aShip::psTrading, false) & 0x0000007f);
        UpdateOne(4, ShipToInspect->GetBaseSkillLevel(aShip::psCharisma), ShipToInspect->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f);
        UpdateOne(5, ShipToInspect->GetBaseSkillLevel(aShip::psLeadership), ShipToInspect->GetEffectiveSkillLevel(aShip::psLeadership, false) & 0x0000007f);
        for (I = 0; I <= 5; ++I) {
            SkillButtons[I]->SetActive(false);
        }
    }

    GI_MessageLoop::TObjectGI* TfScaner::CreateAdditionalInfoIcon(GI_Label::TLabelGI* Sender, EC_CacheFont::PFontObjectEC Item) {
        GI_MessageLoop::TObjectGI* Result = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Sender);
        {
            GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(Result);
            cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"AI_", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->ObjectId)))}));
            cpp_with->SetImageKindX(GI_Main::ikxLeft);
            return Result;
        }
    }

    void TfScaner::BuildAdditionalInfo() {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel{};
        std::int32_t OffsetY{};
        std::int32_t I{};
        std::int32_t IconKind{};
        aShip::PCustomShipInfo Info{};
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Description{};
        pas::WideString Caption{};
        auto AddRow = [&](std::int32_t IconId, pas::WideString Caption, pas::WideString Help, std::int32_t Data) -> void {
            GI_Label::TLabelGI* cpp_with = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            cpp_with->SetFontName(GlobalsV::SmallFontName);
            cpp_with->SetPosition(ClassesImports::Point(0, OffsetY));
            cpp_with->SetSize(ClassesImports::Point(Panel->ClientSize.X, 1));
            cpp_with->SetTextAlignX(GI_Main::taxLeft);
            cpp_with->SetTextAlignY(GI_Main::tayAuto);
            cpp_with->SetWordWrapEnabled(true);
            if (GR_Main::GiResourceVariant() == 1) {
                cpp_with->SetText(pas::concat_wide_reverse({EC_Str::ReplaceAllWideString(Caption, u"<br>"_wref.get(), u"\r\n"_wref.get()), static_cast<pas::WideString>(pas::concat_ansi({"<Object=", SysUtils::IntToStr(IconId), ",", SysUtils::IntToStr(21), ",", SysUtils::IntToStr(17), ",0>"}))}));
            } else {
                cpp_with->SetText(pas::concat_wide_reverse({EC_Str::ReplaceAllWideString(Caption, u"<br>"_wref.get(), u"\r\n"_wref.get()), static_cast<pas::WideString>(pas::concat_ansi({"<Object=", SysUtils::IntToStr(IconId), ",", SysUtils::IntToStr(25), ",", SysUtils::IntToStr(20), ",0>"}))}));
            }
            cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            cpp_with->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with->SetPositionModeW(true);
            cpp_with->CreateEmbeddedControl = pas::bind_static_method<&TfScaner::CreateAdditionalInfoIcon>(this);
            cpp_with->MouseEnterCallback = pas::bind_method<&TfScaner::ShowPropertyInfo>(this);
            cpp_with->MouseLeaveCallback = pas::bind_method<&TfScaner::HidePropertyInfo>(this);
            cpp_with->UserValue = -1;
            cpp_with->UserIndex = IconId;
            cpp_with->UserData = Data;
            cpp_with->HelpText = Help;
            OffsetY += cpp_with->ClientSize.Y;
            {
                std::int32_t lineHeight = cpp_with->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = Panel->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(lineHeight);
            }
        };
        Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelAddInfo"_wref.get()));
        Panel->FreeOwnedChildren();
        Panel->SetScrollOffset(ClassesImports::Point(0, 0));
        Panel->SetDragScrollingEnabled(true);
        OffsetY = 0;
        for (I = 1; I <= 24; ++I) {
            if (ShipToInspect->IsHealthEffectActive(I)) {
                if (I < 13) {
                    IconKind = 1;
                } else {
                    IconKind = 2;
                }
                AddRow(IconKind, aConst::CaptainHealthDefinitions[I].Name, pas::concat_wide({aConst::CaptainHealthDefinitions[I].Name, u"~", aConst::CaptainHealthDefinitions[I].Text}), 0);
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ShipToInspect->CustomShipInfos) - 1); cpp_range.next(I); ) {
            Info = pas::list_at<aShip::TCustomShipInfo>(ShipToInspect->CustomShipInfos, I);
            if (!Info->DeleteQueued) {
                Block = GR_Main::LanguageDataConfig->GetBlock(u"ShipInfo"_wref.get())->GetBlock(u"AddInfo"_wref.get())->GetBlock(u"CustomInfos"_wref.get())->GetBlock(Info->TypeName);
                Description = Info->Description;
                if (Description == u"") {
                    Description = aConst::LocalizedColorText(pas::concat_wide({u"ShipInfo.AddInfo.CustomInfos.", Info->TypeName, u".Description"}));
                }
                if (Description != u"NoShow") {
                    aMyFunction::ReplaceTextToken(Description, u"<Data1>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<Data2>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<Data3>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData1>"_w, Info->TextData1, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData2>"_w, Info->TextData2, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData3>"_w, Info->TextData3, u"<color=255,240,100>"_w);
                    Caption = Block->GetParam(u"Name"_wref.get());
                    aMyFunction::ReplaceTextToken(Caption, u"<Data1>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<Data2>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<Data3>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<TextData1>"_w, Info->TextData1, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<TextData2>"_w, Info->TextData2, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Caption, u"<TextData3>"_w, Info->TextData3, u"<color=255,240,100>"_w);
                    {
                        std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Icon"_wref.get())));
                        pas::WideString cpp_arg = pas::concat_wide({Caption, u"~", Description});
                        AddRow(strToInt, Caption, std::move(cpp_arg), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Info)));
                    }
                }
            }
        }
        Panel->UpdateScrollRanges();
        Panel->VerticalScrollBar->SetActive(Panel->ClientSize.Y < OffsetY);
        if (Panel->VerticalScrollBar->Active) {
            Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
            Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
        }
    }

    void TfScaner::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10004);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20004);
        }
    }

    void TfScaner::SelectMusic() {
        if (aPlayer::GetPlayer() == nullptr) {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
            return;
        }
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

} // namespace fScaner
