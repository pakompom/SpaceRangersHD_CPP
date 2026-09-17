#include "layout/fTalk.hpp"
#include "types/BreakMessageGIException.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/SE_Process.hpp"
#include "types/SE_Space.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aNormalShip.hpp"
#include "types/aPirate.hpp"
#include "types/aPlanet.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/aWarrior.hpp"
#include "types/fStarMap.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Expression.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MathImports.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fTalk.hpp"

namespace fTalk {
    // Shared disabled-choice callback.
    GI_MessageLoop::TDialogChoiceEventGI ScriptDialogBlockCallback = nullptr;

    std::int32_t TruceOfferAmount{};

    std::int32_t ExtortionDemandAmount{};

    std::int32_t PartnerOfferAmount{};

    std::int32_t PartnerGiftAmount{};

    // Set by native conversation setup; cleared by cleanup. Suppresses recursive SF_Dialog dispatch.
    std::uint8_t TalkDialogActive = false;

    pas::Array<float, 0, 15> TalkSlideCurve = pas::Array<float, 0, 15>{{
        0.0f, 0.033f, 0.078f, 0.149f, 0.273f, 0.44f, 0.611f, 0.753f,
        0.888f, 0.963f, 1.009f, 1.025f, 1.02f, 1.012f, 1.008f, 1.0f,
    }};

    std::uint8_t IsMilitaryProtectedQuestItem(aItem::TItem* Item) {
        std::int32_t I{};
        aRanger::PQuest Quest{};
        std::uint8_t Result = false;
        if (pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr && Item->ScriptItem == nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Quests) - 1); cpp_range.next(I); ) {
                Quest = pas::list_at<aRanger::TQuest>(aPlayer::GetPlayer()->Quests, I);
                if (Quest->QuestType == aGalaxyStruct::qtSendLetter) {
                    if (([&] {
                        pas::WideString cpp_string = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(Quest->QuestNumber), ".SysName"})));
                        const pas::WideString& cpp_string_ref = pas::checked_cast<aItem::TUselessItem*>(Item)->ConfigBlockName;
                        return cpp_string == cpp_string_ref;
                    }())) {
                        Result = true;
                    }
                }
            }
        }
        return Result;
    }

    void DonateMilitaryResearchMaterial(aGalaxyStruct::TDominatorSeries Series, std::int32_t Amount) {
        aGalaxyStruct::TDominatorSeries Other{};
        std::int32_t Count{};
        if (aGalaxy::Galaxy->DominatorResearch[Series].Progress < 1.0E+2L && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Series)) {
            aGalaxy::Galaxy->DominatorResearch[Series].Material += Amount;
        } else {
            Count = 0;
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(Other); ) {
                if (aGalaxy::Galaxy->DominatorResearch[Other].Progress < 1.0E+2L && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Other)) {
                    ++Count;
                }
            }
            if (Count != 0) {
                for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_2.next(Other); ) {
                    if (aGalaxy::Galaxy->DominatorResearch[Other].Progress < 1.0E+2L && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Other)) {
                        aGalaxy::Galaxy->DominatorResearch[Other].Material += pas::idiv(Amount, Count);
                    }
                }
            }
        }
    }

    std::int32_t GetMilitaryHullRepairCost() {
        pas::Extended cpp_right = aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 2.0, 8.0, 5.0, 1.0);
        pas::Extended cpp_left = pas::real_divide(aPlayer::GetPlayer()->GetHull()->Weight - aPlayer::GetPlayer()->GetHull()->HullPoints, 1.0E+1L);
        return MathImports::Ceil(cpp_left * (aPlayer::GetPlayer()->GetCombatStatusStrength(aShip::cseBWRepairDebuff) * 0.002L + 1.0L) * cpp_right);
    }

    std::int32_t GetMilitaryEquipmentRepairCost() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if ((!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && Item->ItemType != aConst::t_Hull && Item->EquippedFlag != 0 && aShip::TShip_CanRepairEquipmentTech(Globals::TalkShip, Item) && Item->ConditionPercent < 9.0E+1L) {
                Result += System::Round(aItem::TEquipment_CalculateRepairCost(Item));
            }
        }
        if (Result > 0) {
            Result = System::Round(Result * 0.0025L * aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 2.0, 8.0, 5.0, 1.0));
            if (Result == 0) {
                return 1;
            }
        }
        return Result;
    }

    // Native modal conversation wrapper.
    std::uint8_t RunTalk(GI_MessageLoop::TMessageLoopGI* ParentLoop) {
        std::uint8_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        ParentLoop->RootUiObject->NativeHook50();
        ParentLoop->CaptureCursorState(&State);
        ParentLoop->SetCursorActive(false);
        ParentLoop->DrawQueuedUpdateRects();
        Globals::TalkScreen->ParentLoop = ParentLoop;
        ParentLoop->ChildLoop = Globals::TalkScreen;
        aShip::TShip* OtherShip = Globals::TalkShip;
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(0x00000018, OtherShip, nullptr, 0);
        }
        if (OtherShip != nullptr) {
            OtherShip->ScriptItemsAct(0x00000018, nullptr, nullptr, 0);
        }
        if (Globals::TalkScreen->Run() == 1) {
            Result = true;
            if (aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->ScriptItemsAct(0x00000019, OtherShip, nullptr, 0);
            }
            if (OtherShip != nullptr) {
                OtherShip->ScriptItemsAct(0x00000019, nullptr, nullptr, 0);
            }
        } else {
            Result = false;
        }
        Globals::TalkScreen->ParentLoop = nullptr;
        ParentLoop->ChildLoop = nullptr;
        ParentLoop->InvalidateViewport();
        ParentLoop->RestoreCursorState(&State);
        ParentLoop->UpdateCursorPosition();
        ParentLoop->RootUiObject->NativeHook48();
        return Result;
    }

    void PayPartnerGiftMoney() {
        std::int32_t Payment{};
        aPlayer::TPlayer* Player = aPlayer::GetPlayer();
        std::int32_t Remaining = aPlayer::GetPlayer()->Money - PartnerGiftAmount;
        if (Remaining < 0) {
            Payment = 0;
        } else {
            Payment = Remaining;
        }
        Player->SetMoney(Payment);
    }

    void PayPiratePartnerGiftMoney() {
        std::int32_t Payment{};
        aPlayer::TPlayer* Player = aPlayer::GetPlayer();
        std::int32_t Remaining = aPlayer::GetPlayer()->Money - PartnerGiftAmount;
        if (Remaining < 0) {
            Payment = 0;
        } else {
            Payment = Remaining;
        }
        Player->SetMoney(Payment);
    }

    void TfTalkA_Create(TfTalkA* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TfTalkA_Destroy(TfTalkA* Self) {
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TfTalk::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GI_MessageLoop::TObjectGI* Panel = GetByName(u"MainPanel"_wref.get());
        Panel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Panel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* MapPanel = Panel->FindByNameRecursive(u"MapPanel"_wref.get());
        MapPanel->SetPosition(ClassesImports::Point(MapPanel->LocalPosition.X + GR_Main::ExtraScreenWidth, MapPanel->LocalPosition.Y));
        GI_MessageLoop::TObjectGI* Sibling = MapPanel->NextSibling;
        Sibling->SetPosition(ClassesImports::Point(Sibling->LocalPosition.X + GR_Main::ExtraScreenWidth, Sibling->LocalPosition.Y));
        GI_MessageLoop::TObjectGI* CenterPlayer = Panel->FindByNameRecursive(u"CenterPlayer"_wref.get());
        CenterPlayer->SetPosition(ClassesImports::Point(CenterPlayer->LocalPosition.X + GR_Main::ExtraScreenWidth, CenterPlayer->LocalPosition.Y));
        GI_MessageLoop::TObjectGI* TalkPanel = Panel->FindByNameRecursive(u"PanelTalk"_wref.get());
        TalkPanel->SetPosition(ClassesImports::Point(TalkPanel->LocalPosition.X + GR_Main::ExtraScreenWidth, TalkPanel->LocalPosition.Y));
        Globals::ScriptDialogIndex = -1;
        MainPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"_wref.get()));
        MainPanel->MouseMoveCallback = pas::bind_method<&TfTalk::MainPanelMouseMove>(this);
        MainPanel->KeyDownCallback = pas::bind_method<&TfTalk::MainPanelKeyDown>(this);
        MainPanel->RightButtonDownCallback = pas::bind_method<&TfTalk::MainPanelMouseDown>(this);
        MainPanel->RightButtonUpCallback = pas::bind_method<&TfTalk::MainPanelMouseUp>(this);
        DialogPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelTalk"_wref.get()));
        DialogPanelLeft = DialogPanel->LocalPosition.X;
        GI_Label::TLabelGI* LabelControl = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()));
        LabelControl->CreateEmbeddedControl = pas::bind_method<&TfTalk::CreateDialogObject>(this);
        GI_GraphButton::TGraphButtonGI* AddButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()));
        AddButton->UpCallback = pas::bind_method<&TfTalk::AddMessageClicked>(this);
        GI_GraphButton::TGraphButtonGI* CenterShipButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"CenterShip"_wref.get()));
        CenterShipButton->UpCallback = pas::bind_method<&TfTalk::CenterShipClicked>(this);
        GI_GraphButton::TGraphButtonGI* CenterPlayerButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"CenterPlayer"_wref.get()));
        CenterPlayerButton->UpCallback = pas::bind_method<&TfTalk::CenterShipClicked>(this);
        GI_GraphBuf::TGraphBufGI* MapBuffer = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"MapPanel"_wref.get()));
        MapBuffer->BindExternalGraphBuf(GR_Main::RenderScratchBuffer);
        GI_GraphButton::TGraphButtonGI* CloseButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Close"_wref.get()));
        CloseButton->UpCallback = pas::bind_method<&TfTalk::CloseClicked>(this);
    }

    void TfTalk::OnOpen() {
        WindowsSdk::TPoint Position{};
        GI_GAI::TgaiGI* ExistingAnimation{};
        GI_MessageLoop::TObjectGI* MapControl{};
        GI_Image::TImageGI* Portrait{};
        GI_GAI::TgaiGI* Animation{};
        GI_Label::TLabelGI* TextLabel{};
        GI_PanelScrollBar::TPanelScrollBarGI* Choices{};
        TalkDialogActive = true;
        if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(false, 0);
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        if (SlideTimer != nullptr) {
            CancelCallbackTimer(SlideTimer);
            SlideTimer = nullptr;
        }
        SavedChoiceScroll = -1;
        MinimapEnabled = false;
        ChoiceMousePressed = false;
        if (Flag128 != 0 && MapDragging) {
            if (!IsCursorImageSelected(u"Scroll"_wref.get())) {
                SetCursorByName(u"Scroll"_wref.get());
            }
        } else if (!IsCursorImageSelected(u"Main"_wref.get())) {
            SetCursorByName(u"Main"_wref.get());
        }
        GI_GraphButton::TGraphButtonGI* CenterShipButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"CenterShip"_wref.get()));
        CenterShipButton->SetActive(aPlayer::GetPlayer()->InNormalSpace());
        GI_GraphButton::TGraphButtonGI* CenterPlayerButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"CenterPlayer"_wref.get()));
        CenterPlayerButton->SetActive(aPlayer::GetPlayer()->InNormalSpace());
        ClearDialogEffects();
        if (aPlayer::GetPlayer()->InNormalSpace()) {
            if (Flag128 == 0) {
                if (Globals::TalkShip != nullptr) {
                    Position = EC_Struct::TruncatePointF(Globals::TalkShip->Position);
                } else {
                    Position = EC_Struct::TruncatePointF(Globals::TalkPlanet->GetPosition());
                }
                Position = Globals::StarMapScreen->MapControls->ToAbsolutePoint(Position);
                AddDialogEffect(EC_Struct::PointToPointF(Position), pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 0);
                AddDialogEffect(EC_Struct::PointToPointF(Position), pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 200);
                AddDialogEffect(EC_Struct::PointToPointF(Position), pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 400);
            } else if (RequestedMapCenter != nullptr) {
                if (pas::class_cast_if<aShip::TShip*>(RequestedMapCenter) != nullptr) {
                    Position = EC_Struct::TruncatePointF(pas::checked_cast<aShip::TShip*>(RequestedMapCenter)->Position);
                } else if (pas::class_cast_if<aItem::TItem*>(RequestedMapCenter) != nullptr) {
                    Position = EC_Struct::TruncatePointF(pas::checked_cast<aItem::TItem*>(RequestedMapCenter)->Position);
                } else {
                    Position = EC_Struct::TruncatePointF(pas::checked_cast<aPlanet::TPlanet*>(RequestedMapCenter)->GetPosition());
                }
                Position = Globals::StarMapScreen->MapControls->ToAbsolutePoint(Position);
                AddDialogEffect(EC_Struct::PointToPointF(Position), pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 0);
                AddDialogEffect(EC_Struct::PointToPointF(Position), pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 200);
                AddDialogEffect(EC_Struct::PointToPointF(Position), pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 400);
            }
        }
        RequestedMapCenter = nullptr;
        if (Flag128 != 0) {
            ExistingAnimation = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"_wref.get()));
            ExistingAnimation->RestartPlayback();
            Flag128 = 0;
            MinimapEnabled = true;
            if (Flag12C) {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.AfterTrade"_wref.get());
                BuildStandardChoices(true);
                RestartTextPresentation();
            }
            Flag12C = false;
        } else {
            MapDragging = false;
            if (aPlayer::GetPlayer()->InNormalSpace()) {
                Globals::SpaceProcess->BindMinimap(GetByName(u"MapPanel"_wref.get()));
                GetByName(u"MapPanel"_wref.get())->SetActive(true);
                Globals::SpaceProcess->Space->ScrollChangedCallback = pas::bind_method<&TfTalk::MinimapScrolled>(this);
            } else {
                MapControl = GetByName(u"MapPanel"_wref.get());
                MapControl->LeftButtonDownCallback = nullptr;
                MapControl->RightButtonDownCallback = nullptr;
                MapControl->MouseEnterCallback = nullptr;
                MapControl->MouseMoveCallback = nullptr;
                MapControl->SetActive(false);
            }
            CurrentMapCenter = nullptr;
            if (!GlobalsV::MusicInSpaceEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()))->SetText(u""_wref.get());
            if (Globals::TalkShip != nullptr) {
                const pas::WideString& formatText1 = ([&] {
                    pas::WideString fullName = Globals::TalkShip->GetFullName(u"\r\n"_wref.get());
                    pas::WideString localizedText = aConst::LocalizedText(u"Talk.ShipSay"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText), pas::WideString(), u"<Name>"_w, std::move(fullName));
                }());
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkShip"_wref.get()));
                cpp_arg->SetText(formatText1);
            } else if (Globals::TalkPlanet != nullptr) {
                const pas::WideString& formatText1_2 = ([&] {
                    pas::WideString fullName_2 = Globals::TalkPlanet->GetFullName(u"\r\n"_w);
                    pas::WideString localizedText_2 = aConst::LocalizedText(u"Talk.PlanetSay"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_2), pas::WideString(), u"<Name>"_w, std::move(fullName_2));
                }());
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkShip"_wref.get()));
                cpp_arg_2->SetText(formatText1_2);
            } else {
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkShip"_wref.get()))->SetText(u""_wref.get());
            }
            if (Globals::TalkShip != nullptr && Globals::TalkShip->TypeId == aGalaxyStruct::stRanger) {
                const pas::WideString& characterName = pas::checked_cast<aRanger::TRanger*>(Globals::TalkShip)->GetCharacterName();
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkShipChar"_wref.get()));
                cpp_arg_3->SetText(characterName);
            } else if (Globals::TalkShip != nullptr && pas::class_cast_if<aRuins::TRuins*>(Globals::TalkShip) != nullptr) {
                const pas::WideString& localizedText_3 = aConst::LocalizedText(u"ShipType.TypeName.Ruins"_wref.get());
                GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkShipChar"_wref.get()));
                cpp_arg_4->SetText(localizedText_3);
            } else if (Globals::TalkShip != nullptr) {
                const pas::WideString& localizedTypeName = Globals::TalkShip->GetLocalizedTypeName();
                GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkShipChar"_wref.get()));
                cpp_arg_5->SetText(localizedTypeName);
            } else if (Globals::TalkPlanet != nullptr) {
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkShipChar"_wref.get()))->SetText(aConst::PlanetEconomyInfo[Globals::TalkPlanet->Economy].DisplayName);
            } else {
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkShipChar"_wref.get()))->SetText(u""_wref.get());
            }
            Portrait = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"_wref.get()));
            if (Globals::TalkShip != nullptr) {
                Portrait->SetImagePath(pas::concat_wide({u"GI,", aShip::TShip_GetCaptainPortraitResourceBase(Globals::TalkShip), u"i"}));
            } else if (Globals::TalkPlanet != nullptr) {
                Portrait->SetImagePath(pas::concat_wide({u"GI,", Globals::TalkPlanet->GetGovernmentPortraitGraph(), u"i"}));
            } else {
                GR_Main::RaiseWideMessage(u"talk portrait"_wref.get());
            }
            Portrait->SetImageKindX(GI_Main::ikxCenter);
            Portrait->SetImageKindY(GI_Main::ikyCenter);
            Portrait->SetActive(true);
            Animation = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"_wref.get()));
            Animation->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            if (Globals::TalkShip != nullptr) {
                Animation->SetImagePath(pas::concat_wide({aShip::TShip_GetCaptainPortraitResourceBase(Globals::TalkShip), u"a"}));
            } else if (Globals::TalkPlanet != nullptr) {
                Animation->SetImagePath(pas::concat_wide({Globals::TalkPlanet->GetGovernmentPortraitGraph(), u"a"}));
            } else {
                GR_Main::RaiseWideMessage(u"talk portrait"_wref.get());
            }
            Animation->SequenceIndex = 0;
            Animation->UpdateAutoGeometry();
            Animation->SetImageKindX(GI_Main::ikxCenter);
            Animation->SetImageKindY(GI_Main::ikyCenter);
            Animation->SetActive(true);
            Animation->RestartPlayback();
            TextLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()));
            if (GlobalsV::FontDialog == 0) {
                TextLabel->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                TextLabel->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                TextLabel->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                TextLabel->SetFontName(GlobalsV::SmoothIntroFontName);
            }
            CodeMsgOut(false);
            RestartTextPresentation();
            Choices = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
            Choices->SetVerticalScrollbarEnabled(false);
            SlideProgress = 0.0f;
            SlideTimer = ScheduleCallbackTimer(30, 30, pas::bind_method<&TfTalk::AdvanceSlide>(this), 0);
            UpdateSlidePosition();
            MinimapEnabled = true;
            Flag128 = 0;
            Flag12C = false;
        }
    }

    void TfTalk::OnClose() {
        std::int32_t I{};
        std::int32_t Count{};
        aShip::TShip* Ship{};
        aGalaxy::TStar* Star{};
        if (GR_Main::AuxRenderBuffer != nullptr) {
            GR_Main::AuxRenderBuffer->Clear();
        }
        MinimapEnabled = false;
        ClearDialogEffects();
        if (MapSelectionTimer != nullptr) {
            CancelCallbackTimer(MapSelectionTimer);
            MapSelectionTimer = nullptr;
        }
        if (TextPresentationTimer != nullptr) {
            CancelCallbackTimer(TextPresentationTimer);
            TextPresentationTimer = nullptr;
        }
        if (MinimapRefreshTimer != nullptr) {
            CancelCallbackTimer(MinimapRefreshTimer);
            MinimapRefreshTimer = nullptr;
        }
        if (SlideTimer != nullptr) {
            CancelCallbackTimer(SlideTimer);
            SlideTimer = nullptr;
        }
        if (Flag128 == 0) {
            RequestedMapCenter = nullptr;
            ClearChoices(false);
            Count = aPlayer::GetPlayer()->ProgramCounts[aGalaxyStruct::prgIntercom];
            if (Globals::TalkShip != nullptr && Globals::TalkShip->TypeId == aGalaxyStruct::stKling && Count > 0 && aPlayer::GetPlayer()->CanResolveObjectWithScanner(Globals::TalkShip)) {
                --Count;
                aPlayer::GetPlayer()->ProgramCounts[aGalaxyStruct::prgIntercom] = Count;
                SysUtilsImports::Sleep(1u);
                if (aPlayer::GetPlayer()->ProgramCounts[aGalaxyStruct::prgIntercom] != Count && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                    GR_Main::CCInterface->SetTamperDetected(true);
                }
            }
            if (aPlayer::GetPlayer() != nullptr && Globals::TalkShip != nullptr && static_cast<std::uint8_t>(Globals::TalkScripted ^ 1)) {
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnPlayerTalkedWithShip, Globals::TalkShip, nullptr, 0);
            }
            Globals::TalkShip = nullptr;
            Globals::TalkPlanet = nullptr;
            if (static_cast<std::uint8_t>(Globals::TalkScripted ^ 1) && static_cast<std::uint8_t>(SkipShipScriptAdvance ^ 1)) {
                I = 0;
                Star = aPlayer::GetPlayer()->CurrentStar;
                while (I < pas::list_count(Star->Ships)) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
                    if (Ship == aKling::KellerShip) {
                        ++I;
                    } else if (Ship == aKling::BlazerShip) {
                        ++I;
                    } else {
                        if (Ship->ScriptShip != nullptr) {
                            Ship->ScriptNextDay();
                        }
                        ++I;
                    }
                }
            }
            TalkDialogActive = false;
        }
    }

    void TfTalk::RememberChoiceScroll() {
        SavedChoiceScroll = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()))->VerticalScrollBar->Position;
    }

    void TfTalk::EnableCloseButton() {
        GI_GraphButton::TGraphButtonGI* Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Close"_wref.get()));
        Button->SetDisabled(false);
    }

    void TfTalk::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        FastExit(0);
    }

    void TfTalk::ClearChoices(std::uint8_t AllowClose) {
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Close"_wref.get()))->SetDisabled(static_cast<std::uint8_t>(AllowClose ^ 1));
        ChoiceHeight = 0;
        GI_MessageLoop::TObjectGI* Panel = GetByName(u"TalkPA"_wref.get());
        GI_MessageLoop::TObjectGI* Child = Panel->FirstChild;
        while (Child != nullptr) {
            pas::free(reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Child->UserValue))));
            Child = Child->NextSibling;
        }
        Panel->FreeOwnedChildren();
        Panel->Invalidate();
    }

    // ExtraValue is stored in the choice object at $1C; its wider meaning remains unresolved.
    void TfTalk::AddChoice(pas::WideString Text, std::int32_t Value, GI_MessageLoop::TDialogChoiceEventGI Callback, std::int32_t ExtraValue) {
        std::int32_t I{};
        SystemImports::TMethod ExitCallback{};
        std::uint8_t BlockMode = 0;
        if (aScript::ScriptDialogBlocks != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogBlocks) - 1); cpp_range.next(I); ) {
                if (EC_Str::FindTextOffsetW(Text, pas::list_at<aScript::TDialogBlock>(aScript::ScriptDialogBlocks, I)->Text, 0) >= 0) {
                    BlockMode = std::max<std::int32_t>(static_cast<std::int32_t>(BlockMode), static_cast<std::int32_t>(pas::list_at<aScript::TDialogBlock>(aScript::ScriptDialogBlocks, I)->Mode));
                }
            }
        }
        if (BlockMode >= 2) {
            return;
        }
        ExitCallback.Data = this;
        ExitCallback.Code = pas::method_address<&TfTalk::FastExit>();
        if (pas::load_unaligned<void*>(pas::byte_offset(&Callback, offsetof(SystemImports::TMethod, Code))) == ExitCallback.Code) {
            EnableCloseButton();
        }
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
        I = 0;
        while (I < Text.length()) {
            if (Text.read(I + 1) != u'-' && Text.read(I + 1) != u' ') {
                break;
            }
            ++I;
        }
        if (I > 0) {
            Text = pas::copy(Text, I + 1, Text.length() - I);
        }
        TfTalkA* Choice = pas::construct_call<TfTalkA>(TfTalkA_Create);
        Choice->Callback = Callback;
        Choice->Value = Value;
        Choice->ExtraValue = ExtraValue;
        if (BlockMode > 0) {
            Choice->Callback = nullptr;
        }
        GI_Panel::TPanelGI* Row = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
        Row->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Choice));
        Row->SetPosition(ClassesImports::Point(0, ChoiceHeight));
        Row->SetSize(ClassesImports::Point(Panel->ClientSize.X, 20));
        Row->SetPositionModeW(true);
        Row->MouseEnterCallback = pas::bind_static_method<&TfTalk::ChoiceMouseEnter>(this);
        Row->MouseLeaveCallback = pas::bind_static_method<&TfTalk::ChoiceMouseLeave>(this);
        Row->LeftButtonDownCallback = pas::bind_method<&TfTalk::ChoiceMouseDown>(this);
        Row->LeftButtonUpCallback = pas::bind_method<&TfTalk::ChoiceMouseUp>(this);
        GI_Image::TImageGI* Highlight = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
        Highlight->SetDepth(3.0);
        Highlight->SetPosition(ClassesImports::Point(0, 0));
        Highlight->SetSize(ClassesImports::Point(Panel->ClientSize.X, 20));
        Highlight->SetImagePath(pas::concat_wide({u"GI,Bm.FormGov2.", GR_Main::GiResourceSuffix(), u"Line"}));
        Highlight->SetImageKindX(GI_Main::ikxLeftFill);
        Highlight->SetImageKindY(GI_Main::ikyTopFill);
        Highlight->SetActive(false);
        {
            GI_Label::TLabelGI* cpp_with = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
            if (GlobalsV::FontDialog == 0) {
                cpp_with->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                cpp_with->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                cpp_with->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                cpp_with->SetFontName(GlobalsV::SmoothIntroFontName);
            }
            cpp_with->SetSize(ClassesImports::Point(Panel->ClientSize.X - GR_Main::GiScalePixels(20), 20));
            cpp_with->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(10), 0));
            cpp_with->SetWordWrapEnabled(true);
            cpp_with->SetTextAlignX(GI_Main::taxLeft);
            cpp_with->SetTextAlignY(GI_Main::tayAuto);
            if (!pas::assigned(Callback)) {
                Text = EC_Str::RemoveTextTagsW(Text);
            }
            cpp_with->SetText(pas::concat_wide({u"<Object=0,20,14,0>", EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"_wref.get())}));
            cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            if (!pas::assigned(Choice->Callback)) {
                cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(127, 127, 127));
            }
            cpp_with->CreateEmbeddedControl = pas::bind_method<&TfTalk::CreateDialogObject>(this);
            cpp_with->SetTextAlignY(GI_Main::tayCenterEx);
            Row->SetSize(ClassesImports::Point(Row->ClientSize.X, cpp_with->ClientSize.Y + 2 * GR_Main::GiScalePixelsEx(2, 1)));
            cpp_with->SetSize(ClassesImports::Point(cpp_with->ClientSize.X, Row->ClientSize.Y));
            Highlight->SetSize(Row->ClientSize);
            ChoiceHeight += cpp_with->ClientSize.Y;
        }
    }

    GI_MessageLoop::TObjectGI* TfTalk::CreateDialogObject(GI_Label::TLabelGI* LabelControl, EC_CacheFont::PFontObjectEC Item) {
        GI_MessageLoop::TObjectGI* Result{};
        GI_Image::TImageGI* Image{};
        GI_GraphButton::TGraphButtonGI* Button{};
        if (Item->ObjectId == 0) {
            Result = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, LabelControl);
            Image = pas::checked_cast<GI_Image::TImageGI*>(Result);
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormGov2.", GR_Main::GiResourceSuffix(), u"Answer"}));
            Image->SetImageKindX(GI_Main::ikxLeft);
        } else {
            Result = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, LabelControl);
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Result);
            Button->UserValue = Item->ObjectId;
            Button->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormTalk2.", GR_Main::GiResourceSuffix(), u"Center2N"}));
            Button->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormTalk2.", GR_Main::GiResourceSuffix(), u"Center2A"}));
            Button->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormTalk2.", GR_Main::GiResourceSuffix(), u"Center2D"}));
            Button->EnterSound = u"Sound.ButtonEnter"_w;
            Button->LeaveSound = u"Sound.ButtonLeave"_w;
            Button->ClickSound = u"Sound.ButtonClick"_w;
            Button->NormalOffset = ClassesImports::Point(3, 0);
            Button->NormalActiveOffset = ClassesImports::Point(3, 0);
            Button->DownOffset = ClassesImports::Point(3, 0);
            Button->MouseBlocking = true;
            Button->SetSize(Button->GetMaxStateImageSize());
            Button->HitKind = GI_GraphButton::gbhRect;
            Button->UpdateStateImagePlacement();
            Button->UpdateStateVisuals();
            Button->UpCallback = pas::bind_method<&TfTalk::CenterEmbeddedObject>(this);
        }
        return Result;
    }

    void TfTalk::CenterEmbeddedObject(GI_MessageLoop::TObjectGI* Sender) {
        if (TextPresentationTimer != nullptr || SlideTimer != nullptr) {
            return;
        }
        RequestedMapCenter = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        RequestedMapHover = nullptr;
        CurrentMapCenter = nullptr;
        ApplyMapSelection(nullptr, 0);
    }

    void TfTalk::CenterShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (TextPresentationTimer != nullptr || SlideTimer != nullptr) {
            return;
        }
        if (GetByName(u"CenterPlayer"_wref.get()) == Sender) {
            RequestedMapCenter = aPlayer::GetPlayer();
        } else if (Globals::TalkPlanet != nullptr) {
            RequestedMapCenter = Globals::TalkShip;
        } else {
            RequestedMapCenter = Globals::TalkShip;
        }
        RequestedMapHover = nullptr;
        CurrentMapCenter = nullptr;
        ApplyMapSelection(nullptr, 0);
    }

    void TfTalk::MainPanelMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_MessageLoop::TObjectGI* Child{};
        if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && TextPresentationTimer == nullptr && SlideTimer == nullptr) {
            if (MapDragging) {
                RequestedMapCenter = nullptr;
                CurrentMapCenter = nullptr;
                RequestedMapHover = nullptr;
                CurrentMapHover = nullptr;
                Globals::StarMapScreen->ShowObjectInfo(nullptr);
                Globals::StarMapScreen->ShowFilmObjectInfo(nullptr, 0u);
                {
                    std::int32_t cpp_left_2 = Globals::StarMapScreen->GetMapCenter().Y;
                    std::int32_t cpp_arg = cpp_left_2 + MapDragPoint.Y - Point.Y;
                    std::int32_t cpp_left = Globals::StarMapScreen->GetMapCenter().X;
                    std::int32_t cpp_arg_2 = cpp_left + MapDragPoint.X - Point.X;
                    WindowsSdk::TPoint point = ClassesImports::Point(cpp_arg_2, cpp_arg);
                    fStarMap::TfStarMap* starMapScreen = Globals::StarMapScreen;
                    starMapScreen->SetMapCenter(point);
                }
                MapDragPoint = Point;
            } else if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG"_wref.get()))->HitTestPixel(Point) || pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBGB"_wref.get()))->HitTestPixel(Point) || pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"_wref.get()))->ContainsPoint(Point)) {
                RequestedMapHover = nullptr;
                if (MapSelectionTimer == nullptr) {
                    MapSelectionTimer = ScheduleCallbackTimer(100, 100, pas::bind_method<&TfTalk::ApplyMapSelection>(this), 0);
                }
            } else {
                Child = MainPanel->FirstChild;
                while (Child != nullptr) {
                    if (Child->UserValue == 102) {
                        return;
                    }
                    Child = Child->NextSibling;
                }
                Globals::StarMapScreen->CursorControl->SetPosition(GetCursorPoint());
                RequestedMapCenter = nullptr;
                if (Globals::TalkScripted) {
                    RequestedMapHover = Globals::StarMapScreen->FindFilmObjectAtCursor(CurrentFilmObjectId);
                } else {
                    RequestedMapHover = Globals::StarMapScreen->FindObjectAtCursor();
                }
                if (MapSelectionTimer == nullptr) {
                    MapSelectionTimer = ScheduleCallbackTimer(100, 100, pas::bind_method<&TfTalk::ApplyMapSelection>(this), 0);
                }
            }
        }
    }

    void TfTalk::MainPanelMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && TextPresentationTimer == nullptr && SlideTimer == nullptr) {
            if (static_cast<std::uint8_t>(pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG"_wref.get()))->HitTestPixel(Point) ^ 1) && static_cast<std::uint8_t>(pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBGB"_wref.get()))->HitTestPixel(Point) ^ 1) && static_cast<std::uint8_t>(pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"_wref.get()))->ContainsPoint(Point) ^ 1)) {
                MapDragging = true;
                MapDragPoint = Point;
                if (!IsCursorImageSelected(u"Scroll"_wref.get())) {
                    SetCursorByName(u"Scroll"_wref.get());
                }
            }
        }
    }

    void TfTalk::MainPanelMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (!aPlayer::GetPlayer()->InHyperspace) {
            MapDragging = false;
            if (!IsCursorImageSelected(u"Main"_wref.get())) {
                SetCursorByName(u"Main"_wref.get());
            }
            GR_Main::PostMouseMoveMessage();
        }
    }

    void TfTalk::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TextScroll"_wref.get()));
        if (!Panel->ContainsPoint(Point)) {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
        }
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->SmallChange);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->SmallChange);
        }
    }

    void TfTalk::ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(true);
    }

    void TfTalk::ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(false);
    }

    void TfTalk::ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(2, 0));
        }
        ChoiceMousePressed = true;
    }

    void TfTalk::ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        TfTalkA* Choice{};
        if (Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(0, 0));
        }
        if (static_cast<std::uint8_t>(Sender->IsOccludedAtPoint(Point) ^ 1) && ChoiceMousePressed) {
            ChoiceMousePressed = false;
            Choice = reinterpret_cast<TfTalkA*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
            // DCC32 evaluates the callback receiver first with this identity expression.
            if (pas::assigned(Choice->Callback)) {
                reinterpret_cast<TfTalkA*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Choice)) * 1)))->Callback(Choice->Value);
            } else if (pas::assigned(Choice->FallbackCallback)) {
                reinterpret_cast<TfTalkA*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Choice)) * 1)))->FallbackCallback(Choice->FallbackText);
            } else {
                return;
            }
            RestartTextPresentation();
            GI_Main::BreakUiMessage();
        }
    }

    void TfTalk::RestartTextPresentation() {
        pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()))->SetActive(false);
        PresentedTextLength = 0;
        if (TextPresentationTimer != nullptr) {
            CancelCallbackTimer(TextPresentationTimer);
            TextPresentationTimer = nullptr;
        }
        TextPresentationTimer = ScheduleCallbackTimer(10, 10, pas::bind_method<&TfTalk::AdvanceTextPresentation>(this), 0);
    }

    void TfTalk::AdvanceTextPresentation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_PanelScrollBar::TPanelScrollBarGI* Choices{};
        GI_PanelScrollBar::TPanelScrollBarGI* TextPanel{};
        if (PresentedTextLength >= DialogText.length()) {
            Choices = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
            Choices->SetActive(true);
            {
                std::int32_t lineHeight = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()))->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = Choices->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(lineHeight);
            }
            Choices->VerticalScrollBar->SetLargeChange(Choices->ClientSize.Y);
            Choices->VerticalScrollBar->SetPageSize(Choices->ClientSize.Y);
            Choices->SetScrollOffset(ClassesImports::Point(0, 0));
            Choices->SetVerticalScrollbarEnabled(ChoiceHeight > Choices->ClientSize.Y);
            Choices->VerticalScrollBar->SetDepth(4.0);
            Choices->SetDragScrollingEnabled(Choices->IsVerticalScrollbarEnabled());
            Choices->UpdateScrollRanges();
            if (TextPresentationTimer != nullptr) {
                CancelCallbackTimer(TextPresentationTimer);
                TextPresentationTimer = nullptr;
            }
            if (SavedChoiceScroll >= 0) {
                Choices->VerticalScrollBar->SetPosition_2(SavedChoiceScroll);
            }
            SavedChoiceScroll = -1;
            GR_Main::PostMouseMoveMessage();
        } else {
            PresentedTextLength = DialogText.length();
            DialogText = EC_Str::ReplaceAllWideString(DialogText, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"_wref.get());
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()))->SetText(DialogText);
            TextPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TextScroll"_wref.get()));
            TextPanel->SetScrollOffset(ClassesImports::Point(0, 0));
            TextPanel->UpdateScrollRanges();
            TextPanel->VerticalScrollBar->SetActive(pas::checked_cast<GI_Label::TLabelGI*>(TextPanel->FindByNameRecursive(u"TalkText"_wref.get()))->ClientSize.Y > TextPanel->ClientSize.Y);
            {
                std::int32_t lineHeight_2 = pas::checked_cast<GI_Label::TLabelGI*>(TextPanel->FindByNameRecursive(u"TalkText"_wref.get()))->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar_2 = TextPanel->VerticalScrollBar;
                verticalScrollBar_2->SetSmallChange(lineHeight_2);
            }
            TextPanel->VerticalScrollBar->SetLargeChange(TextPanel->ClientSize.Y);
            TextPanel->VerticalScrollBar->SetPageSize(TextPanel->ClientSize.Y);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()))->SetDisabled(false);
        }
    }

    void TfTalk::AddDialogEffect(EC_Struct::TPointF Position, pas::WideString ImagePath, std::int32_t DelayMs) {
        GI_GAI::TgaiGI* Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, MainPanel);
        Animation->SetDepth(7.0);
        Animation->SetPosition(EC_Struct::TruncatePointF(Position));
        Animation->SetPositionModeW(true);
        Animation->SetImagePath(ImagePath);
        Animation->SetSize(Animation->GetContentSize());
        Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
        Animation->UserValue = 102;
        Animation->SequenceIndex = 0;
        Animation->UpdateAutoGeometry();
        if (DelayMs >= 0) {
            Animation->SetFrameDelay(0, DelayMs);
        }
        Animation->CycleCompleteCallback = pas::bind_static_method<&TfTalk::DialogEffectComplete>(this);
        Animation->RestartPlayback();
    }

    void TfTalk::ClearDialogEffects() {
        GI_MessageLoop::TObjectGI* Next{};
        GI_MessageLoop::TObjectGI* Child = MainPanel->FirstChild;
        while (Child != nullptr) {
            Next = Child;
            Child = Child->NextSibling;
            if (Next->UserValue == 102) {
                Next->SetActive(false);
                pas::free(Next);
            }
        }
    }

    void TfTalk::DialogEffectComplete(GI_MessageLoop::TObjectGI* Sender) {
        Sender->SetActive(false);
        pas::free(Sender);
        GR_Main::PostMouseMoveMessage();
    }

    void TfTalk::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop != nullptr && ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(2);
        }
    }

    void TfTalk::ApplyMapSelection(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        pas::Object* Previous{};
        if (RequestedMapCenter != nullptr) {
            Previous = CurrentMapCenter;
            CurrentMapCenter = RequestedMapCenter;
            if (CurrentMapCenter == nullptr) {
                CurrentMapCenter = Globals::TalkShip;
            }
            if (CurrentMapCenter == nullptr) {
                CurrentMapCenter = Globals::TalkPlanet;
            }
            if (CurrentMapCenter != nullptr && pas::class_cast_if<aShip::TShip*>(CurrentMapCenter) != nullptr) {
                Globals::StarMapScreen->CenterMapForTalk(pas::checked_cast<aShip::TShip*>(CurrentMapCenter)->Position);
            } else if (CurrentMapCenter != nullptr && pas::class_cast_if<aPlanet::TPlanet*>(CurrentMapCenter) != nullptr) {
                Globals::StarMapScreen->CenterMapForTalk(pas::checked_cast<aPlanet::TPlanet*>(CurrentMapCenter)->GetPosition());
            } else if (CurrentMapCenter != nullptr && pas::class_cast_if<aItem::TItem*>(CurrentMapCenter) != nullptr) {
                Globals::StarMapScreen->CenterMapForTalk(pas::checked_cast<aItem::TItem*>(CurrentMapCenter)->Position);
            }
            Globals::StarMapScreen->ShowObjectInfo(nullptr);
            Globals::StarMapScreen->ShowFilmObjectInfo(nullptr, 0u);
            if (MapSelectionTimer != nullptr) {
                CancelCallbackTimer(MapSelectionTimer);
                MapSelectionTimer = nullptr;
            }
            if (Previous != CurrentMapCenter) {
                Flag128 = 1;
                GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
                if (Globals::TalkScripted) {
                    Globals::StarMapScreen->ResumeMode = fStarMap::smrWaitForTurn;
                }
                RequestClose(1);
            }
        } else {
            Previous = CurrentMapHover;
            CurrentMapHover = RequestedMapHover;
            if (pas::class_cast_if<SE_Space::TObjectSE*>(CurrentMapHover) != nullptr) {
                Globals::StarMapScreen->ShowObjectInfo(nullptr);
                Globals::StarMapScreen->ShowFilmObjectInfo(pas::checked_cast<SE_Space::TObjectSE*>(CurrentMapHover), CurrentFilmObjectId);
            } else {
                Globals::StarMapScreen->ShowFilmObjectInfo(nullptr, 0u);
                Globals::StarMapScreen->ShowObjectInfo(CurrentMapHover);
            }
            if (MapSelectionTimer != nullptr) {
                CancelCallbackTimer(MapSelectionTimer);
                MapSelectionTimer = nullptr;
            }
            if (Previous != CurrentMapHover) {
                Flag128 = 1;
                GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
                if (Globals::TalkScripted) {
                    Globals::StarMapScreen->ResumeMode = fStarMap::smrWaitForTurn;
                }
                RequestClose(1);
            }
        }
    }

    void TfTalk::AddMessageClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Text{};
        Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()))->GetText();
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=0,50,200>"_wref.get(), u"<color=255,240,100>"_wref.get());
        Text = EC_Str::RemoveMatchingTextTagsW(Text, u"object"_wref.get(), u"OBJECT"_wref.get());
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()))->SetDisabled(true);
        GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
        Globals::AddOrUpdatePlayerBubble(7, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
        if (!aPlayer::GetPlayer()->InHyperspace) {
            ReturnToMap(0);
        }
    }

    void TfTalk::MinimapScrolled() {
        if (MinimapEnabled && Flag128 == 0 && aPlayer::GetPlayer()->InNormalSpace()) {
            Globals::SpaceProcess->Space->DrawMinimap();
            GetByName(u"MapPanel"_wref.get())->Invalidate();
            if (MinimapRefreshTimer == nullptr) {
                MinimapRefreshTimer = ScheduleCallbackTimer(1, 1, pas::bind_method<&TfTalk::FlushMinimapRefresh>(this), 0);
            }
        }
    }

    void TfTalk::FlushMinimapRefresh(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (TextPresentationTimer == nullptr && SlideTimer == nullptr) {
            if (MinimapRefreshTimer != nullptr) {
                CancelCallbackTimer(MinimapRefreshTimer);
                MinimapRefreshTimer = nullptr;
            }
            Flag128 = 1;
            GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
            if (Globals::TalkScripted) {
                Globals::StarMapScreen->ResumeMode = fStarMap::smrWaitForTurn;
            }
            RequestClose(1);
        }
    }

    void TfTalk::AdvanceSlide(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        SlideProgress = SlideProgress + 0.05L;
        if (SlideProgress >= 1.0L) {
            SlideProgress = 1.0f;
            if (SlideTimer != nullptr) {
                CancelCallbackTimer(SlideTimer);
                SlideTimer = nullptr;
            }
        }
        UpdateSlidePosition();
    }

    void TfTalk::UpdateSlidePosition() {
        if (SlideProgress < 0.0L) {
            SlideProgress = 0.0f;
        } else if (SlideProgress > 1.0L) {
            SlideProgress = 1.0f;
        }
        std::int32_t I = System::Trunc(15.0L * SlideProgress);
        std::int32_t J = I + 1;
        if (J > 15) {
            J = 15;
        }
        float T = 0.06666667f;
        T = pas::real_divide(SlideProgress - static_cast<long double>(I) * T, T);
        T = (static_cast<long double>(TalkSlideCurve[J]) - TalkSlideCurve[I]) * T + TalkSlideCurve[I];
        DialogPanel->SetPosition(ClassesImports::Point(DialogPanelLeft + DialogPanel->ClientSize.X - System::Round(static_cast<long double>(DialogPanel->ClientSize.X) * T), DialogPanel->LocalPosition.Y));
    }

    void TfTalk::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel{};
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
            if (Key == 'C') {
                CenterShipClicked(GetByName(u"CenterPlayer"_wref.get()));
            } else if (Key == WindowsSdk::VK_ESCAPE && static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Close"_wref.get()))->Disabled ^ 1)) {
                CloseClicked(nullptr);
            } else if (Key == WindowsSdk::VK_UP) {
                Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->SmallChange);
            } else if (Key == WindowsSdk::VK_DOWN) {
                Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->SmallChange);
            } else if (Key == WindowsSdk::VK_PRIOR) {
                Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->LargeChange);
            } else if (Key == WindowsSdk::VK_NEXT) {
                Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->LargeChange);
            }
        }
    }

    void TfTalk::SelectMusic() {
        if (!GlobalsV::MusicInSpaceEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
            return;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 20) {
            Globals::StarMapScreen->BattleMusicSelected = true;
            GR_Main::MusicManager->PlayCategory(u"Destroyer"_wref.get());
        } else {
            Globals::StarMapScreen->BattleMusicSelected = false;
            GR_Main::MusicManager->PlayCategory(u"StarMap"_wref.get());
        }
    }

    void TfTalk::BuildStandardChoices(std::uint8_t KeepGreeting) {
        aScript::TScriptShip* Binding{};
        aScript::TScript* Script{};
        pas::WideString Text{};
        pas::WideString Mode{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Selected{};
        std::int32_t Priority{};
        std::int32_t PartCount{};
        void* Swapped{};
        std::uint8_t ReplacedGreeting{};
        aScript::ClearScriptDialogRules();
        ClearChoices(false);
        if (aScript::CurrentScript != nullptr && Globals::ScriptDialogIndex >= 0) {
            SkipShipScriptAdvance = true;
            aScript::CurrentScript->PublishCurrentShip(Globals::TalkShip);
            aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
            return;
        }
        if (SkipShipScriptAdvance && Globals::TalkShip->ScriptShip != nullptr) {
            Binding = reinterpret_cast<aScript::TScriptShip*>(Globals::TalkShip->ScriptShip);
            Binding->Script->PublishShipContext(Binding);
            aScript::TScript_CallDialog(Binding->Script, Binding->Script->CurrentDialog);
            if (Globals::ScriptDialogIndex >= 0) {
                Binding->Script->CallDialogMessage(Globals::ScriptDialogIndex);
                return;
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range.next(I); ) {
            Script = pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, I);
            aScript::TScript_RunAuxiliaryCode(Script);
        }
        if (pas::list_count(aScript::ScriptDialogOverrides) > 0) {
            Selected = 0;
            Priority = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, 0)->Priority;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogOverrides) - 1); cpp_range_2.next(I); ) {
                if (pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, I)->Priority > Priority) {
                    Selected = I;
                    Priority = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, I)->Priority;
                }
            }
            Script = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, Selected)->Script;
            {
                std::uint32_t answerData = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, Selected)->AnswerData;
                EC_Expression::TVarEC* var = Script->InitCode->LocalVar->GetVar(u"GAnswerData"_wref.get());
                var->SetDword(answerData);
            }
            Text = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, Selected)->DialogName;
            if (Text != u"") {
                Script->PublishCurrentShip(Globals::TalkShip);
                Script->CallDialogByVariable(Text);
                if (Globals::ScriptDialogIndex < 0) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({Script->ScriptFileName, u" has overriden dialog with ", Text, u" but it failed to start"})));
                }
            }
            if (Globals::ScriptDialogIndex < 0) {
                BuildBuiltinChoices();
            } else {
                SkipShipScriptAdvance = true;
                StartScriptMessage(Script);
            }
        } else {
            Selected = -1;
            Priority = 0;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_3.next(I); ) {
                if (pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->ReplaceGreeting) {
                    if (Selected < 0 || pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Priority > Priority) {
                        Priority = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Priority;
                        Selected = I;
                    }
                }
            }
            if (Selected >= 0) {
                ReplacedGreeting = true;
                DialogText = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, Selected)->Text;
            } else {
                ReplacedGreeting = false;
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_4.next(I); ) {
                for (auto cpp_range_5 = pas::for_downto<std::int32_t>(pas::list_count(aScript::ScriptDialogInjections) - 1, I); cpp_range_5.next(J); ) {
                    if (([&] {
                        std::int32_t cpp_left = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, J)->Priority;
                        return cpp_left > pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, J - 1)->Priority;
                    }())) {
                        Swapped = pas::list_get(aScript::ScriptDialogInjections, J);
                        pas::list_put(aScript::ScriptDialogInjections, J, pas::list_get(aScript::ScriptDialogInjections, J - 1));
                        pas::list_put(aScript::ScriptDialogInjections, J - 1, Swapped);
                    }
                }
            }
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_6.next(I); ) {
                if (!pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->ReplaceGreeting) {
                    Text = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Text;
                    if (Text != u"") {
                        if (ReplacedGreeting | static_cast<std::uint8_t>(KeepGreeting ^ 1)) {
                            DialogText = pas::concat_wide({DialogText, u"\r\n", Text});
                        }
                    }
                }
                Text = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Answer;
                if (Text != u"") {
                    Mode = pas::WideString();
                    PartCount = EC_Str::CountDelimitedPartsW(Text, u"~"_wref.get());
                    if (PartCount > 1) {
                        Mode = EC_Str::ExtractDelimitedPartW(Text, 0, u"~"_wref.get());
                        Text = EC_Str::ExtractDelimitedRangeW(Text, 1, PartCount - 1, u"~"_wref.get());
                    }
                    if (Mode == u"block") {
                        AddChoice(Text, 0, ScriptDialogBlockCallback, 0);
                    } else if (Mode == u"snap") {
                        AddChoice(Text, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aScript::ScriptDialogInjections, I))), pas::bind_method<&TfTalk::RunInjectedAnswerKeepingScroll>(this), 0);
                    } else {
                        std::int32_t cpp_arg = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aScript::ScriptDialogInjections, I)));
                        pas::WideString answer = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Answer;
                        AddChoice(std::move(answer), cpp_arg, pas::bind_method<&TfTalk::RunInjectedAnswer>(this), 0);
                    }
                }
            }
            BuildBuiltinChoices();
        }
    }

    void TfTalk::BuildBuiltinChoices() {
        std::uint8_t HasAttackChoice{};
        pas::WideString TargetName{};
        GI_MessageLoop::TDialogChoiceEventGI Callback{};
        std::uint8_t ProgramIndex{};
        std::int32_t I{};
        std::uint8_t RecognizesPlayer = static_cast<std::uint8_t>(Globals::TalkShip->IsPlayerChameleonEffectiveAgainstSelf() ^ 1);
        if (Globals::TalkScripted) {
            switch (Globals::TalkType) {
                case aGalaxyStruct::tkMoneyDemand: {
                    if (aPlayer::GetPlayer()->Money >= Globals::TalkAmount) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfTalk::AcceptScriptedConversation>(this), 0);
                    } else {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerNotMoney"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    }
                    if (aPlayer::GetPlayer()->CanEscapePursuer(Globals::TalkShip)) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerLongDistance"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    } else {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    }
                    break;
                }
                case aGalaxyStruct::tkGoodsDemand: {
                    if (aPlayer::GetPlayer()->HasCargoGoods()) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Goods.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfTalk::AcceptScriptedConversation>(this), 0);
                    } else {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Goods.PlayerNotGoods"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    }
                    if (aPlayer::GetPlayer()->CanEscapePursuer(Globals::TalkShip)) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Goods.PlayerLongDistance"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    } else {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Goods.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    }
                    break;
                }
                case aGalaxyStruct::tkTruceOffer: {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Truce.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfTalk::AcceptScriptedConversation>(this), 0);
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Truce.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    break;
                }
                case aGalaxyStruct::tkAttack: {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfTalk::AcceptScriptedConversation>(this), 0);
                    if (Globals::TalkShip->EnemyShip != nullptr && (aShip::TShip_RelationToShip(Globals::TalkShip->EnemyShip, aPlayer::GetPlayer()) >= 80 || aShip::TShip_RelationToShip(Globals::TalkShip->EnemyShip, aPlayer::GetPlayer()) >= 60 && aPlayer::GetPlayer()->GetDominantCareer() != aGalaxyStruct::rcPirate)) {
                        if (!(pas::class_cast_if<aTranclucator::TTranclucator*>(Globals::TalkShip->EnemyShip) != nullptr)) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerWeFriends"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                        } else if (aPlayer::GetPlayer() == reinterpret_cast<aTranclucator::TTranclucator*>(Globals::TalkShip->EnemyShip)->OwnerShip) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerItsMyTranc"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                        } else if (reinterpret_cast<aTranclucator::TTranclucator*>(Globals::TalkShip->EnemyShip)->OwnerShip == Globals::TalkShip) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerItsYourTranc"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                        } else {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerWeFriendsTranc"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                        }
                    } else if (Globals::TalkShip->EnemyShip != nullptr && aPlayer::GetPlayer()->virtual_TShip_AcceptsRansomDemandFrom(Globals::TalkShip->EnemyShip)) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerFear"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    } else {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerHaveBusiness"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    }
                    break;
                }
                case aGalaxyStruct::tkPartnerBreak: {
                    if (Globals::TalkShip->TypeId == aGalaxyStruct::stPirate) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.AnswerLiderBreak"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    } else {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.AnswerLiderBreak"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    }
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    break;
                }
                case aGalaxyStruct::tkPartnerEnd: {
                    if (Globals::TalkShip->TypeId == aGalaxyStruct::stPirate) {
                        std::int32_t cpp_left = aPlayer::GetPlayer()->GetEffectiveSkillLevel(aShip::psLeadership, false);
                        if (cpp_left > aPlayer::GetPlayer()->CountWingmen()) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.AnswerLiderTheEnd"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                        } else {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.AnswerLiderTheEndLowLeadership"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                        }
                    } else {
                        std::int32_t cpp_left_2 = aPlayer::GetPlayer()->GetEffectiveSkillLevel(aShip::psLeadership, false);
                        if (cpp_left_2 > aPlayer::GetPlayer()->CountWingmen()) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.AnswerLiderTheEnd"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                        } else {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.AnswerLiderTheEndLowLeadership"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                        }
                    }
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    break;
                }
                case aGalaxyStruct::tkPartnerRiot: {
                    if (Globals::TalkShip->TypeId == aGalaxyStruct::stPirate) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.AnswerLiderRiot"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    } else {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.AnswerLiderRiot"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    }
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    break;
                }
                default: {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                    break;
                }
            }
        } else {
            TruceOfferAmount = std::min<std::int32_t>(aPlayer::GetPlayer()->Money, aPlayer::GetPlayer()->GetWealthScaledAmount(3));
            {
                std::int32_t cpp_left_3 = Globals::TalkShip->GetWealthScaledAmount(3);
                ExtortionDemandAmount = (cpp_left_3 + aPlayer::GetPlayer()->GetWealthScaledAmount(3)) / 2;
            }
            PartnerOfferAmount = std::min<std::int32_t>(aPlayer::GetPlayer()->Money, Globals::TalkShip->Wealth / 8);
            PartnerGiftAmount = std::min<std::int32_t>(aPlayer::GetPlayer()->Money, Globals::TalkShip->Wealth / 32);
            if ((aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip || static_cast<std::uint8_t>(pas::in_range(Globals::TalkShip->TypeId, aGalaxyStruct::stPirate, aGalaxyStruct::stPirate) ^ 1)) && RecognizesPlayer & pas::in_range(Globals::TalkShip->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stWarrior)) {
                if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Truce.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowTruceOffer>(this), 0);
                    if (aPlayer::GetPlayer()->IsHealthEffectActive(5)) {
                        Callback = ScriptDialogBlockCallback;
                    } else {
                        Callback = pas::bind_method<&TfTalk::ShowMoneyDemand>(this);
                    }
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerSend"_wref.get())}), 0, Callback, 0);
                    if (aPlayer::GetPlayer()->IsHealthEffectActive(5)) {
                        Callback = ScriptDialogBlockCallback;
                    } else {
                        Callback = pas::bind_method<&TfTalk::DemandCargo>(this);
                    }
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Goods.PlayerSend"_wref.get())}), 0, Callback, 0);
                } else {
                    HasAttackChoice = AddImmediateAttackChoices();
                    if (static_cast<std::uint8_t>(HasAttackChoice ^ 1) && aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
                        if (aPlayer::GetPlayer()->IsHealthEffectActive(5)) {
                            Callback = ScriptDialogBlockCallback;
                        } else {
                            Callback = pas::bind_method<&TfTalk::ShowMoneyDemand>(this);
                        }
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerSend"_wref.get())}), 0, Callback, 0);
                        if (aPlayer::GetPlayer()->IsHealthEffectActive(5)) {
                            Callback = ScriptDialogBlockCallback;
                        } else {
                            Callback = pas::bind_method<&TfTalk::DemandCargo>(this);
                        }
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Goods.PlayerSend"_wref.get())}), 0, Callback, 0);
                    }
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerOffersAttack"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowAttackTargets>(this), 0);
                }
                if (aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip && pas::class_cast_if<aShip::TShip*>(Globals::TalkShip->OrderTarget) != nullptr && !(pas::class_cast_if<aRuins::TRuins*>(Globals::TalkShip->OrderTarget) != nullptr) && aPlayer::GetPlayer() != Globals::TalkShip->OrderTarget && aShip::TShip_GetRelationLevelToShip(pas::checked_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget), aPlayer::GetPlayer()) > aGalaxyStruct::rlHostile && pas::checked_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget)->OrderTarget != Globals::TalkShip && aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, pas::checked_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget)) == aGalaxyStruct::rlHostile) {
                    AddChoice(([&] {
                        pas::WideString cpp_arg = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(pas::checked_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget), false), pas::checked_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget)->GetFullName(u" "_wref.get())});
                        pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Protect.PlayerSend"_wref.get())});
                        return aMyFunction::FormatText1(std::move(cpp_arg_2), pas::WideString(), u"<Target>"_w, std::move(cpp_arg));
                    }()), 0, pas::bind_method<&TfTalk::RequestProtection>(this), 0);
                }
                if (aPlayer::GetPlayer()->PickupTargets != nullptr) {
                    const std::int32_t cpp_last = static_cast<std::int32_t>(Globals::TalkShip->WeaponCount);
                    if (1 <= cpp_last) {
                        for (I = 1; I <= cpp_last; ++I) {
                            if (Globals::TalkShip->Weapons[I]->Target != nullptr && pas::list_indexof(aPlayer::GetPlayer()->PickupTargets, reinterpret_cast<void*>(Globals::TalkShip->Weapons[I]->Target)) >= 0) {
                                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.PreserveItems.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::RequestPreserveItems>(this), 0);
                                break;
                            }
                        }
                    }
                }
            }
            switch (Globals::TalkShip->TypeId) {
                case aGalaxyStruct::stRanger: {
                    if (aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
                        if (RecognizesPlayer && aPlayer::GetPlayer()->Money > 0 && aPlayer::GetPlayer()->CountWingmen() < 6) {
                            AddChoice(([&] {
                                pas::WideString name = Globals::TalkShip->GetName();
                                pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.PlayerSend"_wref.get())});
                                return aMyFunction::FormatText1(std::move(cpp_arg_3), pas::WideString(), u"<Ranger>"_w, std::move(name));
                            }()), 0, pas::bind_method<&TfTalk::ShowPartnerOffer>(this), 0);
                        }
                    } else {
                        if (aPlayer::GetPlayer() != Globals::TalkShip->OrderTarget) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.FlyToMe"_wref.get())}), 0, pas::bind_method<&TfTalk::OrderPartnerFollow>(this), 0);
                        }
                        switch (aPlayer::GetPlayer()->Order) {
                            case aShip::soLand: {
                                if (aPlayer::GetPlayer()->OrderTarget != Globals::TalkShip->OrderTarget) {
                                    if (pas::class_cast_if<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                                        TargetName = pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->Name;
                                    } else if (pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                                        TargetName = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget)->GetColoredFullName(u""_wref.get());
                                    } else {
                                        TargetName = pas::WideString();
                                    }
                                    if (TargetName.length() > 0) {
                                        AddChoice(aMyFunction::FormatText1(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.LandingToObject"_wref.get())}), u"<color=255,240,100>"_w, u"<ObjectName>"_w, TargetName), 0, pas::bind_method<&TfTalk::OrderPartnerLand>(this), 0);
                                    }
                                }
                                break;
                            }
                            case aShip::soJump: {
                                if (aPlayer::GetPlayer()->OrderTarget != Globals::TalkShip->OrderTarget) {
                                    AddChoice(([&] {
                                        auto name_2 = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget)->Name);
                                        pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.FlyToStar"_wref.get())});
                                        return aMyFunction::FormatText1(std::move(cpp_arg_4), u"<color=255,240,100>"_w, u"<Star>"_w, name_2.get());
                                    }()), 0, pas::bind_method<&TfTalk::OrderPartnerJump>(this), 0);
                                }
                                break;
                            }
                        }
                        if (Globals::TalkShip->HasLooseNonScriptItemsOrGoods() || static_cast<std::uint8_t>(aPlayer::GetPlayer()->CanResolveObjectWithScanner(Globals::TalkShip) ^ 1)) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.PlayerSendDropCargo"_wref.get())}), 0, pas::bind_method<&TfTalk::OrderPartnerDropCargo>(this), 0);
                        }
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.FinancesCheck"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowPartnerFinances>(this), 0);
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.PlayerDismissSend"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowPartnerDismissal>(this), 0);
                    }
                    if (RecognizesPlayer && aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Trade.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowTrade>(this), 0);
                    }
                    if (RecognizesPlayer && Globals::TalkShip->UsesVeteranHumanRangerAppearance() && aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) >= aGalaxyStruct::rlGood) {
                        if (aPlayer::GetPlayer() == Globals::TalkShip->PartnerShip) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSendP"_wref.get())}), 0, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                        } else {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                        }
                    }
                    break;
                }
                case aGalaxyStruct::stTransport: {
                    if (RecognizesPlayer) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Trade.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowTrade>(this), 0);
                    }
                    break;
                }
                case aGalaxyStruct::stPirate: {
                    if (aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
                        if (Globals::TalkShip->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || reinterpret_cast<aPirate::TPirate*>(Globals::TalkShip)->PirateType == 0) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowPiratePartnerOffer>(this), 0);
                        }
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Trade.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowTrade>(this), 0);
                    } else {
                        if (aPlayer::GetPlayer() != Globals::TalkShip->OrderTarget) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.FlyToMe"_wref.get())}), 0, pas::bind_method<&TfTalk::OrderPiratePartnerFollow>(this), 0);
                        }
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.Attack"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowPirateAttackTargets>(this), 0);
                        switch (aPlayer::GetPlayer()->Order) {
                            case aShip::soLand: {
                                if (aPlayer::GetPlayer()->OrderTarget != Globals::TalkShip->OrderTarget) {
                                    if (pas::class_cast_if<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                                        TargetName = pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->Name;
                                    } else if (pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                                        TargetName = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget)->GetColoredFullName(u""_wref.get());
                                    } else {
                                        TargetName = pas::WideString();
                                    }
                                    if (TargetName.length() > 0) {
                                        AddChoice(aMyFunction::FormatText1(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.LandingToObject"_wref.get())}), u"<color=255,240,100>"_w, u"<ObjectName>"_w, TargetName), 0, pas::bind_method<&TfTalk::OrderPiratePartnerLand>(this), 0);
                                    }
                                }
                                break;
                            }
                            case aShip::soJump: {
                                if (aPlayer::GetPlayer()->OrderTarget != Globals::TalkShip->OrderTarget) {
                                    AddChoice(([&] {
                                        auto name_3 = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget)->Name);
                                        pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.FlyToStar"_wref.get())});
                                        return aMyFunction::FormatText1(std::move(cpp_arg_5), u"<color=255,240,100>"_w, u"<Star>"_w, name_3.get());
                                    }()), 0, pas::bind_method<&TfTalk::OrderPiratePartnerJump>(this), 0);
                                }
                                break;
                            }
                        }
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.FinancesCheck"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowPiratePartnerFinances>(this), 0);
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.PlayerDismissSend"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowPartnerDismissal>(this), 0);
                    }
                    break;
                }
                case aGalaxyStruct::stWarrior: {
                    if (RecognizesPlayer && reinterpret_cast<aWarrior::TWarrior*>(Globals::TalkShip)->WarriorType == aWarrior::wtFlagship) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitarySupport>(this), 0);
                    }
                    break;
                }
                case aGalaxyStruct::stKling: {
                    if (Globals::TalkShip != aKling::BlazerShip && Globals::TalkShip != aKling::KellerShip && Globals::TalkShip != aKling::TerronShip && pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->ActiveProgramAppliedTurn <= 0 && aPlayer::GetPlayer()->HasProgram(aGalaxyStruct::prgIntercom) && aPlayer::GetPlayer()->CanResolveObjectWithScanner(Globals::TalkShip)) {
                        for (ProgramIndex = static_cast<std::uint8_t>(0); ProgramIndex <= static_cast<std::uint8_t>(11); ++ProgramIndex) {
                            if (aPlayer::GetPlayer()->ProgramCounts[ProgramIndex] > 0 && pas::in_range(ProgramIndex, aGalaxyStruct::prgShipwreck, aGalaxyStruct::prgDisconnection) && pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->ActiveProgramAppliedTurn == 0) {
                                AddChoice(pas::concat_wide({u"- ", ([&] {
                                    pas::WideString programName = (static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(ProgramIndex));
                                    pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Dominator.ProgrammPlayer"_wref.get());
                                    return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(programName));
                                }())}), ProgramIndex, pas::bind_method<&TfTalk::RunDominatorProgram>(this), 0);
                            }
                        }
                        if (RecognizesPlayer || aPlayer::GetPlayer()->ChameleonDetected[pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DominatorSeries] || pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DominatorSeries != aPlayer::GetPlayer()->ChameleonSeries) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Dominator.HiPlayer"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowDominatorGreeting>(this), 0);
                            if (Globals::TalkShip->CurrentStar->Id != aGalaxy::Galaxy->KellerResearchTargetStarId || aKling::KellerShip == nullptr) {
                                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Dominator.PeacePlayer"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowDominatorPeace>(this), 0);
                            }
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Dominator.GoodsPlayer"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowDominatorGoods>(this), 0);
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Dominator.CommandPlayer"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowDominatorCommand>(this), 0);
                        }
                    }
                    break;
                }
                case aGalaxyStruct::stTranclucator: {
                    if (pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip)->OwnerShip == aPlayer::GetPlayer()) {
                        AddImmediateAttackChoices();
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerOffersAttack"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowAttackTargets>(this), 0);
                        if (pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip)->GetCargoHook() != nullptr) {
                            if (pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip)->SeekItems) {
                                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.SeekItems.PlayerCancel"_wref.get())}), 0, pas::bind_method<&TfTalk::CancelTranclucatorSeekItems>(this), 0);
                            } else {
                                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.SeekItems.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::OrderTranclucatorSeekItems>(this), 0);
                            }
                        }
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.Options.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowTranclucatorOptions>(this), 0);
                        if (Globals::TalkShip->HasLooseNonScriptItemsOrGoods()) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.DropCargo.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::OrderTranclucatorDropCargo>(this), 0);
                        }
                        if (pas::in_range(aPlayer::GetPlayer()->Order, static_cast<std::int32_t>(aShip::soLand), static_cast<std::int32_t>(aShip::soLand)) && aPlayer::GetPlayer()->OrderTarget != Globals::TalkShip->OrderTarget) {
                            TargetName = pas::WideString();
                            if (pas::class_cast_if<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                                if (pas::in_set<0, 4, 7, 7>(pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->OwnerId)) {
                                    TargetName = pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->Name;
                                }
                            } else if (pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                                TargetName = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget)->GetColoredFullName(u""_wref.get());
                            }
                            if (TargetName.length() > 0) {
                                AddChoice(aMyFunction::FormatText1(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.LandingToObject"_wref.get())}), u"<color=255,240,100>"_w, u"<ObjectName>"_w, TargetName), 0, pas::bind_method<&TfTalk::OrderTranclucatorLand>(this), 0);
                                AddChoice(aMyFunction::FormatText1(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.LandingToStorage"_wref.get())}), u"<color=255,240,100>"_w, u"<ObjectName>"_w, TargetName), 0, pas::bind_method<&TfTalk::OrderTranclucatorStoreCargo>(this), 0);
                            }
                        }
                        if (aPlayer::GetPlayer() != Globals::TalkShip->OrderTarget) {
                            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.FlyToMe.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::OrderTranclucatorFollow>(this), 0);
                        }
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.Return.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfTalk::OrderTranclucatorReturn>(this), 0);
                    }
                    break;
                }
            }
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        }
    }

    void TfTalk::AddScriptExitChoice(pas::WideString Caption) {
        if (Caption != u"") {
            AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfTalk::RunScriptExitAnswer>(this), 0);
        } else {
            pas::WideString cpp_arg = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())});
            std::int32_t currentAnswer = aScript::CurrentScript->CurrentAnswer;
            AddChoice(std::move(cpp_arg), currentAnswer, pas::bind_method<&TfTalk::RunScriptExitAnswer>(this), 0);
        }
    }

    void TfTalk::CodeMsgOut(std::uint8_t KeepGreeting) {
        aScript::TScriptShip* Binding{};
        aScript::ClearScriptDialogRules();
        SkipShipScriptAdvance = false;
        if (Globals::TalkPlanet != nullptr) {
            ClearChoices(false);
            aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
        } else if (aScript::CurrentScript != nullptr && Globals::ScriptDialogIndex >= 0) {
            ClearChoices(false);
            SkipShipScriptAdvance = true;
            aScript::CurrentScript->PublishCurrentShip(Globals::TalkShip);
            aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
        } else if (Globals::TalkShip->ScriptShip != nullptr) {
            if (Globals::TalkShip == aKling::BlazerShip || Globals::TalkShip == aKling::KellerShip || Globals::TalkShip == aKling::TerronShip) {
                if (pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->IsPlayerCamouflageEffective(aPlayer::GetPlayer())) {
                    if (Globals::TalkShip == aKling::BlazerShip && aGalaxy::Galaxy->BlazerLandingPlanetId != 0) {
                        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Dominator.Chameleon.BossBlazerLand"_wref.get());
                    } else if (Globals::TalkShip == aKling::TerronShip && aGalaxy::Galaxy->TerronToStarTurn != 0) {
                        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Dominator.Chameleon.BossTerronToStar"_wref.get());
                    } else if (Globals::TalkShip == aKling::TerronShip && aGalaxy::Galaxy->TerronGrowLockTurn != 0) {
                        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Dominator.Chameleon.BossTerronGrowLock"_wref.get());
                    } else {
                        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Dominator.Chameleon.Boss", aConst::DominatorSeriesNames[pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DominatorSeries]}));
                    }
                    BuildBuiltinChoices();
                    return;
                }
            }
            ClearChoices(false);
            reinterpret_cast<aScript::TScriptShip*>(Globals::TalkShip->ScriptShip)->Script->PublishShipContext(pas::checked_cast<aScript::TScriptShip*>(Globals::TalkShip->ScriptShip));
            if (Globals::ScriptDialogIndex < 0) {
                Binding = reinterpret_cast<aScript::TScriptShip*>(Globals::TalkShip->ScriptShip);
                if (Binding->State->AuxiliaryCode != nullptr) {
                    try {
                        EC_Expression::TCodeEC_Run(Binding->State->AuxiliaryCode, aScript::ScriptProcess);
                    } catch (...) {
                        auto cpp_exception = pas::caught_object();
                        if (BreakMessageGIException::EBreakMessageGI* E = pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(cpp_exception)) {
                        } else if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                            aScript::LogScriptCallHistory();
                            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in CodeMsgOut code of script ", aScript::CurrentScript->ScriptFileName, u" (state #", Binding->Ship->GetFullName(u" "_wref.get()), u" ", Binding->State->Name, u")"}))));
                        } else {
                            throw;
                        }
                    }
                    BuildStandardChoices(false);
                } else if (Binding->State->AuxiliaryText != u"" && Binding->Script->InitCode->LocalVar->GetVarNE(Binding->State->AuxiliaryText) != nullptr) {
                    aScript::CurrentScript->CallDialogByVariable(Binding->State->AuxiliaryText);
                    if (Globals::ScriptDialogIndex < 0) {
                        if (!KeepGreeting) {
                            if (Globals::TalkScripted) {
                                DialogText = Globals::TalkText;
                            } else {
                                DialogText = TfTalk::GetShipGreeting();
                            }
                        }
                        BuildBuiltinChoices();
                    } else {
                        SkipShipScriptAdvance = true;
                        aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
                    }
                } else {
                    if (!KeepGreeting) {
                        if (Globals::TalkScripted) {
                            DialogText = Globals::TalkText;
                        } else {
                            DialogText = TfTalk::GetShipGreeting();
                        }
                    }
                    BuildStandardChoices(false);
                }
            } else {
                SkipShipScriptAdvance = true;
                aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
            }
        } else {
            if (!KeepGreeting) {
                if (Globals::TalkScripted) {
                    DialogText = Globals::TalkText;
                } else {
                    DialogText = TfTalk::GetShipGreeting();
                }
            }
            BuildStandardChoices(false);
        }
    }

    void TfTalk::RunScriptAnswer(std::int32_t Answer) {
        ClearChoices(false);
        Globals::ScriptDialogIndex = -1;
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        if (Globals::ScriptDialogIndex < 0) {
            GR_Main::RaiseWideMessage(u"I_Script"_wref.get());
        }
        aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
    }

    void TfTalk::RunScriptAnswerKeepingScroll(std::int32_t Answer) {
        RememberChoiceScroll();
        RunScriptAnswer(Answer);
    }

    void TfTalk::FastExit(std::int32_t Answer) {
        if (TextPresentationTimer == nullptr && SlideTimer == nullptr) {
            GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
            if (Globals::TalkScripted) {
                Globals::StarMapScreen->ResumeMode = fStarMap::smrWaitForTurn;
            }
            Globals::ScriptDialogIndex = -1;
            RequestClose(1);
        }
    }

    void TfTalk::RunScriptExitAnswer(std::int32_t Action) {
        aScript::CurrentScript->ExecuteDialogAnswer(Action);
        GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
        if (Globals::TalkScripted) {
            Globals::StarMapScreen->ResumeMode = fStarMap::smrWaitForTurn;
        }
        Globals::ScriptDialogIndex = -1;
        RequestClose(1);
    }

    void TfTalk::ShowGreeting(std::int32_t Action) {
        DialogText = TfTalk::GetShipGreeting();
        BuildStandardChoices(false);
    }

    void TfTalk::AcceptScriptedConversation(std::int32_t Action) {
        Globals::TalkResponse = 1;
        GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
        if (Globals::TalkScripted) {
            Globals::StarMapScreen->ResumeMode = fStarMap::smrWaitForTurn;
        }
        Globals::ScriptDialogIndex = -1;
        RequestClose(1);
    }

    void TfTalk::ReturnToMap(std::int32_t Action) {
        Flag128 = 1;
        GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
        if (Globals::TalkScripted) {
            Globals::StarMapScreen->ResumeMode = fStarMap::smrWaitForTurn;
        }
        RequestClose(1);
        GI_Main::BreakUiMessage();
    }

    void TfTalk::ShowTrade(std::int32_t Action) {
        ClearChoices(false);
        std::int32_t cpp_left = Globals::TalkShip->GetCargoFreeSpace();
        std::int32_t Capacity = cpp_left - Globals::TalkShip->GetDesiredCargoFreeSpace();
        std::int32_t Money = Globals::TalkShip->Money;
        if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) < aGalaxyStruct::rlNormal) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.AnswerBadRelations"_wref.get());
        } else if (Globals::TalkShip->EnemyShip != nullptr && (Globals::TalkShip->EnemyShip->OrderTarget == Globals::TalkShip || Globals::TalkShip->OrderTarget == Globals::TalkShip->EnemyShip) && aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
            DialogText = ([&] {
                pas::WideString cpp_arg = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Globals::TalkShip->EnemyShip, false), Globals::TalkShip->EnemyShip->GetName()});
                pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.AnswerWar"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<ShipBad>"_w, std::move(cpp_arg));
            }());
        } else if (Globals::TalkShip->GetCurrentPickupItem() != nullptr && aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
            DialogText = ([&] {
                pas::WideString cpp_arg_2 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Globals::TalkShip->GetCurrentPickupItem(), false), Globals::TalkShip->GetCurrentPickupItem()->GetDisplayName()});
                pas::WideString lookupTalkText_2 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.AnswerAlreadyTakeItem"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupTalkText_2), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(cpp_arg_2));
            }());
        } else if (static_cast<std::uint8_t>(Globals::TalkShip->HasCargoGoods() ^ 1) && (Capacity < 1 || Money < aConst::GoodsMarket[0].AveragePrice)) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.AnswerNoNeedGoods"_wref.get());
        } else if (aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, Globals::TalkShip->Position) > 2.5E+5L) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.AnswerBigDist"_wref.get());
        } else {
            if (Capacity > 0) {
                DialogText = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(Capacity);
                    pas::WideString lookupTalkText_3 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.TradeOkMayBuyOk"_wref.get());
                    return aMyFunction::FormatText1(std::move(lookupTalkText_3), u"<color=255,240,100>"_w, u"<Cnt>"_w, std::move(intToStr));
                }());
            } else {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.TradeOkMayBuyNo"_wref.get());
            }
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Trade.TradeGo"_wref.get())}), 0, pas::bind_method<&TfTalk::OpenTrade>(this), 0);
        }
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Trade.TradeBreak"_wref.get())}), 0, pas::bind_method<&TfTalk::CancelTrade>(this), 0);
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::OpenTrade(std::int32_t Action) {
        Flag128 = 2;
        GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
        if (Globals::TalkScripted) {
            Globals::StarMapScreen->ResumeMode = fStarMap::smrWaitForTurn;
        }
        RequestClose(1);
        GI_Main::BreakUiMessage();
    }

    void TfTalk::CancelTrade(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.AfterBreak"_wref.get());
        BuildStandardChoices(true);
    }

    void TfTalk::ShowMoneyDemand(std::int32_t Action) {
        std::int32_t I{};
        aShip::TShip* Partner{};
        std::uint8_t PartnerCanDemand{};
        if (aPlayer::GetPlayer()->TruceShip == Globals::TalkShip || pas::class_cast_if<aNormalShip::TNormalShip*>(Globals::TalkShip) != nullptr && static_cast<aNormalShip::TNormalShip*>(Globals::TalkShip)->LastPlayerExtortionTurn + 30 > aGalaxy::Galaxy->CurrentTurn) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Money.WeAlreadyHavePact"_wref.get());
            Globals::TalkShip->ReactToExtortionDemand(aPlayer::GetPlayer());
            BuildStandardChoices(true);
        } else {
            PartnerCanDemand = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->PiratePartners) - 1); cpp_range.next(I); ) {
                Partner = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->PiratePartners, I);
                if (Globals::TalkShip->virtual_TShip_AcceptsRansomDemandFrom(Partner)) {
                    PartnerCanDemand = true;
                    break;
                }
            }
            if (static_cast<std::uint8_t>(Globals::TalkShip->virtual_TShip_AcceptsRansomDemandFrom(aPlayer::GetPlayer()) ^ 1) && static_cast<std::uint8_t>(PartnerCanDemand ^ 1)) {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Money.", Globals::TalkShip->GetTypeNameKey(), u"No"}));
                Globals::TalkShip->ReactToExtortionDemand(aPlayer::GetPlayer());
                BuildStandardChoices(true);
            } else if (Globals::TalkShip->TypeId != aGalaxyStruct::stWarrior && Globals::TalkShip->CanEscapePursuer(aPlayer::GetPlayer())) {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Money.", Globals::TalkShip->GetTypeNameKey(), u"LongDistance"}));
                Globals::TalkShip->ReactToExtortionDemand(aPlayer::GetPlayer());
                BuildStandardChoices(true);
            } else {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Money.ComputerAsk"_wref.get());
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", ([&] {
                    pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerSendSum"_wref.get());
                    pas::WideString intToStr = pas::wide_int_to_str(ExtortionDemandAmount);
                    return aMyFunction::ReplaceColoredToken(std::move(lookupTalkText), u"<Money>"_w, std::move(intToStr), u"<color=255,240,100>"_w);
                }())}), 0, pas::bind_method<&TfTalk::DemandMoney>(this), 0);
                if (ExtortionDemandAmount / 2 > 10) {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerLess"_wref.get())}), 0, pas::bind_method<&TfTalk::HalveMoneyDemand>(this), 0);
                }
                {
                    std::int32_t cpp_left = Globals::TalkShip->GetWealthScaledAmount(3);
                    if (3 * (cpp_left + aPlayer::GetPlayer()->GetWealthScaledAmount(3)) >= 2 * ExtortionDemandAmount) {
                        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Money.PlayerMore"_wref.get())}), 0, pas::bind_method<&TfTalk::DoubleMoneyDemand>(this), 0);
                    }
                }
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Cancel"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
            }
        }
    }

    void TfTalk::DemandMoney(std::int32_t Action) {
        if (Globals::TalkShip->virtual_TShip_BuildMoneyExtortionResponse(aPlayer::GetPlayer(), DialogText, ExtortionDemandAmount)) {
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            Achievements::TryAddAchievementProgress(u"ROBBER"_w, 1);
        }
        BuildStandardChoices(true);
    }

    void TfTalk::HalveMoneyDemand(std::int32_t Action) {
        ExtortionDemandAmount = ExtortionDemandAmount / 2;
        ShowMoneyDemand(0);
    }

    void TfTalk::DoubleMoneyDemand(std::int32_t Action) {
        ExtortionDemandAmount *= 2;
        ShowMoneyDemand(0);
    }

    void TfTalk::DemandCargo(std::int32_t Action) {
        if (Globals::TalkShip->virtual_TShip_BuildCargoExtortionResponse(aPlayer::GetPlayer(), DialogText)) {
            Achievements::TryAddAchievementProgress(u"ROBBER"_w, 1);
        }
        BuildStandardChoices(true);
    }

    void TfTalk::ShowTruceOffer(std::int32_t Action) {
        pas::WideString Response{};
        if (aPlayer::GetPlayer()->TruceShip == Globals::TalkShip) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Truce.WeAlreadyHavePact"_wref.get());
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        } else if (Globals::TalkShip->UnknownVirtualC0(aPlayer::GetPlayer())) {
            ClearChoices(false);
            BuildStandardChoices(true);
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Refuse.", Globals::TalkShip->GetTypeNameKey()}));
        } else if (Globals::TalkShip->BuildTrucePaymentResponse(aPlayer::GetPlayer(), Response, 0)) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Truce.ComputerOkWithoutMoney"_wref.get());
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        } else {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Truce.ComputerAsk"_wref.get());
            ClearChoices(false);
            if (aPlayer::GetPlayer()->Money > 0) {
                AddChoice(pas::concat_wide({u"- ", ([&] {
                    pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Truce.PlayerSendSum"_wref.get());
                    pas::WideString intToStr = pas::wide_int_to_str(TruceOfferAmount);
                    return aMyFunction::ReplaceColoredToken(std::move(lookupTalkText), u"<Money>"_w, std::move(intToStr), u"<color=255,240,100>"_w);
                }())}), 0, pas::bind_method<&TfTalk::AcceptTruceOffer>(this), 0);
                if (TruceOfferAmount / 2 > 100) {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Truce.PlayerLess"_wref.get())}), 0, pas::bind_method<&TfTalk::HalveTruceOffer>(this), 0);
                }
                if (aPlayer::GetPlayer()->Money > TruceOfferAmount) {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Truce.PlayerMore"_wref.get())}), 0, pas::bind_method<&TfTalk::DoubleTruceOffer>(this), 0);
                }
            } else {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Truce.PlayerNotHaveMoney"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
            }
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Cancel"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
        }
    }

    void TfTalk::AcceptTruceOffer(std::int32_t Action) {
        if (Globals::TalkShip->BuildTrucePaymentResponse(aPlayer::GetPlayer(), DialogText, TruceOfferAmount)) {
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            ClearChoices(false);
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::FastExit>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())});
                TfTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
            }
        } else {
            BuildStandardChoices(true);
        }
    }

    void TfTalk::HalveTruceOffer(std::int32_t Action) {
        TruceOfferAmount = std::max<std::int32_t>(100, TruceOfferAmount / 2);
        ShowTruceOffer(0);
    }

    void TfTalk::DoubleTruceOffer(std::int32_t Action) {
        TruceOfferAmount = std::min<std::int32_t>(aPlayer::GetPlayer()->Money, TruceOfferAmount * 2);
        ShowTruceOffer(0);
    }

    void TfTalk::ShowAttackTargets(std::int32_t Action) {
        std::int32_t RadarRangeSquared{};
        aShip::TShip* Ship{};
        pas::WideString SavedText{};
        auto AddAvailableAttackTargets = [&]() -> void {
            std::int32_t I{};
            {
                std::int32_t cpp_left = aPlayer::GetPlayer()->GetRadarRange();
                RadarRangeSquared = cpp_left * aPlayer::GetPlayer()->GetRadarRange();
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
                if (Globals::TalkShip->OrderTarget != Ship && aPlayer::GetPlayer() != Ship && Ship != Globals::TalkShip && aPlayer::GetPlayer() != Ship->PartnerShip && Ship->InNormalSpace() && static_cast<long double>(RadarRangeSquared) > aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, Ship->Position) && static_cast<std::uint8_t>(pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) ^ 1) && aPlayer::GetPlayer()->CanSelectShipTarget(Ship) && static_cast<std::uint8_t>(pas::in_set<1, 2>(Ship->TargetingRestriction) ^ 1)) {
                    AddChoice(pas::concat_wide({u"- ", Ship->GetFullName(u" "_wref.get()), aGalaxy::GetLocalObjectLink(Ship, false)}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfTalk::RequestAttackTarget>(this), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)));
                }
            }
        };
        std::uint8_t AllowTargets = true;
        ClearChoices(false);
        if (Globals::TalkShip->virtual_TShip_RecomputeFearState() && aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Attack.ComputerInFear"_wref.get());
            AllowTargets = false;
        } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(Globals::TalkShip) != nullptr) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.Attack.Ask"_wref.get());
        } else if (!Globals::TalkShip->virtual_TShip_TrustsAttackRequester(aPlayer::GetPlayer())) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Attack.", Globals::TalkShip->GetTypeNameKey(), u"Suspect"}));
            AllowTargets = false;
        } else if (Globals::TalkShip->HasLockedOrFollowOrder() && aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip && Globals::TalkShip->TypeId != aGalaxyStruct::stWarrior) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Attack.", Globals::TalkShip->GetTypeNameKey(), u"HaveBusiness"}));
            AllowTargets = false;
        } else if (pas::class_cast_if<aShip::TShip*>(Globals::TalkShip->OrderTarget) != nullptr) {
            Ship = pas::checked_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget);
            if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, Ship) == aGalaxyStruct::rlHostile && aPlayer::GetPlayer()->CanSelectShipTarget(Ship) && static_cast<std::uint8_t>(pas::in_set<1, 2>(Ship->TargetingRestriction) ^ 1)) {
                DialogText = ([&] {
                    pas::WideString cpp_arg = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Ship, false), Ship->GetFullName(u" "_wref.get())});
                    pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Attack.ComputerReadyAttack"_wref.get());
                    return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Target>"_w, std::move(cpp_arg));
                }());
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerOk"_wref.get())}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfTalk::AcceptJointAttack>(this), 0);
                AllowTargets = true;
            } else {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Attack.", Globals::TalkShip->GetTypeNameKey(), u"HaveBusiness"}));
                AllowTargets = false;
            }
        } else {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Attack.ComputerAsk"_wref.get());
        }
        // The native routine clears the ready-attack choice above before listing targets.
        ClearChoices(false);
        if (AllowTargets) {
            AddAvailableAttackTargets();
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Cancel"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        } else {
            SavedText = DialogText;
            BuildStandardChoices(true);
            DialogText = SavedText;
        }
    }

    void TfTalk::RequestAttackTarget(std::int32_t Action) {
        aGalaxyEvent::TGalaxyEvent* Event{};
        aShip::TShip* Target = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        if (Globals::TalkShip->virtual_TShip_RecomputeFearState() && Globals::TalkShip->OrderTarget != Target && aPlayer::GetPlayer() != Globals::TalkShip->PartnerShip) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Attack.ComputerInFear"_wref.get());
            ClearChoices(false);
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::FastExit>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())});
                TfTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
            }
            return;
        }
        std::uint8_t Accepted = Globals::TalkShip->virtual_TShip_BuildAttackRequestResponse(aPlayer::GetPlayer(), DialogText, Target);
        if (Accepted) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerTalkedShipIntoAttacking"_w, nullptr);
            Event->AddData(Globals::TalkShip->Id);
            Event->AddData(Target->Id);
        }
        if (Accepted && aPlayer::GetPlayer() == Globals::TalkShip->PartnerShip) {
            if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
                DialogText = pas::concat_wide({DialogText, u"\r\n", aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get())});
                ClearChoices(false);
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this);
                    pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForAll"_wref.get())});
                    TfTalk* self_2 = this;
                    self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3, 0);
                }
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfTalk::ExitPartnerConversation>(this);
                    pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForYou"_wref.get())});
                    TfTalk* self_3 = this;
                    self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5, 0);
                }
            } else {
                ClearChoices(false);
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfTalk::FastExit>(this);
                    pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())});
                    TfTalk* self_4 = this;
                    self_4->AddChoice(std::move(cpp_arg_8), 0, cpp_arg_7, 0);
                }
            }
        } else {
            ClearChoices(false);
            if (Accepted && pas::class_cast_if<aTranclucator::TTranclucator*>(Globals::TalkShip) != nullptr) {
                AddTranclucatorGroupChoice();
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_9 = pas::bind_method<&TfTalk::FastExit>(this);
                pas::WideString cpp_arg_10 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())});
                TfTalk* self_5 = this;
                self_5->AddChoice(std::move(cpp_arg_10), 0, cpp_arg_9, 0);
            }
        }
    }

    void TfTalk::AcceptJointAttack(std::int32_t Action) {
        aShip::TShip* Target = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        Globals::TalkShip->SetJointAttackTarget(aPlayer::GetPlayer(), Target);
        if (aPlayer::GetPlayer() == Globals::TalkShip->PartnerShip) {
            if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
                DialogText = pas::concat_wide({DialogText, u"\r\n", aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get())});
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this), 0);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForYou"_wref.get())}), 0, pas::bind_method<&TfTalk::ExitPartnerConversation>(this), 0);
            } else {
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
            }
        }
        FastExit(0);
    }

    void TfTalk::RequestProtection(std::int32_t Action) {
        std::int32_t I{};
        std::int32_t Reward{};
        aItem::TWeapon* Weapon{};
        std::uint8_t CanEscape{};
        std::uint8_t FearsAttacker{};
        std::uint8_t RecognizesPlayer{};
        if (static_cast<std::uint8_t>(Globals::TalkShip->virtual_TShip_RecomputeFearState() ^ 1) && aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Protect.ComputerNotFearAndWar"_wref.get());
            BuildStandardChoices(true);
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
            return;
        }
        aShip::TShip* Target = reinterpret_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget);
        if (Globals::TalkShip->UnknownVirtualC0(Target)) {
            if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Protect.ComputerNotFearAndWar"_wref.get());
            } else {
                DialogText = ([&] {
                    pas::WideString cpp_arg = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Target, false), Target->GetFullName(u" "_wref.get())});
                    pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Protect.", Globals::TalkShip->GetTypeNameKey(), u"No"}));
                    return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Target>"_w, std::move(cpp_arg));
                }());
            }
            BuildStandardChoices(true);
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
            return;
        }
        if (Globals::TalkShip->EvaluateAllyRelationAndStrength(aPlayer::GetPlayer())) {
            CanEscape = Target->CanEscapePursuer(Globals::TalkShip);
            FearsAttacker = Target->virtual_TShip_AcceptsRansomDemandFrom(Globals::TalkShip);
            {
                double chanceToWin = aShip::TShip_ChanceToWin(Target, Globals::TalkShip);
                double cpp_arg_2 = aGalaxy::Galaxy->ComputeScaledSmallMoney(Target->OwnerId) * 0.5L;
                double cpp_arg_3 = aGalaxy::Galaxy->ComputeScaledMiniMoney(Target->OwnerId) * 0.5L;
                Reward = System::Round(aMyFunction::RemapClamped(chanceToWin, 0.1, 1.0, cpp_arg_2, cpp_arg_3));
            }
            if (Globals::TalkShip->EnemyShip == Target) {
                Globals::TalkShip->EnemyShip = nullptr;
            }
            if (Globals::TalkShip->TypeId == aGalaxyStruct::stRanger) {
                Target->ChangeRelationToRanger(Globals::TalkShip, 15);
            }
            if (Globals::TalkShip == Target->EnemyShip) {
                Target->EnemyShip = nullptr;
            }
            if (Target->TypeId == aGalaxyStruct::stRanger) {
                Globals::TalkShip->ChangeRelationToRanger(Target, 15);
            }
            Globals::TalkShip->TruceShip = Target;
            Globals::TalkShip->virtual_TShip_NextDay();
            if (Globals::TalkShip->OrderTarget == Target) {
                Globals::TalkShip->OrderNone(false);
            }
            if (Globals::TalkShip->Order == aShip::soFollowShip && pas::in_range(pas::checked_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget)->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate)) {
                Globals::TalkShip->NavigateToQueuedPlanet(false);
                if (Globals::TalkShip->Order == aShip::soFollowShip) {
                    Globals::TalkShip->OrderNone(false);
                }
            }
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Target->WeaponCount);
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        Weapon = Target->Weapons[I];
                        if (Globals::TalkShip == Weapon->Target) {
                            Weapon->Target = nullptr;
                        }
                    }
                }
            }
            {
                const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Globals::TalkShip->WeaponCount);
                if (1 <= cpp_last_2) {
                    for (I = 1; I <= cpp_last_2; ++I) {
                        Weapon = Globals::TalkShip->Weapons[I];
                        if (Weapon->Target == Target) {
                            Weapon->Target = nullptr;
                        }
                    }
                }
            }
            aShip::TShip_TruceWithShip(Globals::TalkShip, Target);
            Target->ChangeRelationToRanger(aPlayer::GetPlayer(), 50);
            if (Globals::TalkShip->TypeId == aGalaxyStruct::stPirate) {
                aPlayer::GetPlayer()->AddWarriorCareerActivity(4);
            }
            if (Target->TypeId == aGalaxyStruct::stPirate) {
                aPlayer::GetPlayer()->AddPirateCareerActivity(8);
            }
            DialogText = ([&] {
                pas::WideString cpp_arg_4 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Target, false), Target->GetFullName(u" "_wref.get())});
                pas::WideString lookupTalkText_2 = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Protect.", Globals::TalkShip->GetTypeNameKey(), u"Ok"}));
                return aMyFunction::FormatText1(std::move(lookupTalkText_2), u"<color=255,240,100>"_w, u"<Target>"_w, std::move(cpp_arg_4));
            }());
            RecognizesPlayer = static_cast<std::uint8_t>(Target->IsPlayerChameleonEffectiveAgainstSelf() ^ 1);
            if (RecognizesPlayer && pas::in_range(Target->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate)) {
                if (!FearsAttacker) {
                    Target->ShowMessageToPlayer(aShip::TShip_LookupTalkText(Target, u"Talk.Protect.TargetNotFearShip"_wref.get()));
                } else if (CanEscape && Target->GetHullIntegrityPercent() > 30) {
                    Target->ShowMessageToPlayer(aShip::TShip_LookupTalkText(Target, u"Talk.Protect.TargetMayRunAway"_wref.get()));
                } else if (Target->HasCargoGoods()) {
                    Target->JettisonCargoGoodsTowardTargetValue(Reward);
                    Target->ShowMessageToPlayer(aShip::TShip_LookupTalkText(Target, u"Talk.Protect.TargetGiveGoods"_wref.get()));
                } else if (Target->Money >= Reward) {
                    Target->SetMoney(Target->Money - Reward);
                    aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Reward);
                    Target->ShowMessageToPlayer(([&] {
                        pas::WideString intToStr = pas::wide_int_to_str(Reward);
                        pas::WideString lookupTalkText_3 = aShip::TShip_LookupTalkText(Target, u"Talk.Protect.TargetGiveMoney"_wref.get());
                        return aMyFunction::FormatText1(std::move(lookupTalkText_3), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
                    }()));
                } else {
                    Target->ShowMessageToPlayer(aShip::TShip_LookupTalkText(Target, u"Talk.Protect.TargetThanks"_wref.get()));
                }
            }
            if (pas::class_cast_if<aPirate::TPirate*>(Globals::TalkShip) != nullptr) {
                Achievements::TryAddAchievementProgress(u"NEGOCIANT"_w, 1);
            }
        } else {
            DialogText = ([&] {
                pas::WideString cpp_arg_5 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Target, false), Target->GetFullName(u" "_wref.get())});
                pas::WideString lookupTalkText_4 = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Protect.", Globals::TalkShip->GetTypeNameKey(), u"No"}));
                return aMyFunction::FormatText1(std::move(lookupTalkText_4), u"<color=255,240,100>"_w, u"<Target>"_w, std::move(cpp_arg_5));
            }());
        }
        ClearChoices(false);
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::RequestPreserveItems(std::int32_t Action) {
        std::int32_t I{};
        if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && static_cast<std::uint8_t>(Globals::TalkShip->virtual_TShip_RecomputeFearState() ^ 1)) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.PreserveItems.ComputerNotFearAndWar"_wref.get());
        } else if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) >= aGalaxyStruct::rlGood || Globals::TalkShip->EvaluateAllyRelationAndStrength(aPlayer::GetPlayer())) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.PreserveItems.", Globals::TalkShip->GetTypeNameKey(), u"Ok"}));
            if (aPlayer::GetPlayer()->PickupTargets != nullptr) {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Globals::TalkShip->WeaponCount);
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        if (Globals::TalkShip->Weapons[I]->Target != nullptr && pas::list_indexof(aPlayer::GetPlayer()->PickupTargets, reinterpret_cast<void*>(Globals::TalkShip->Weapons[I]->Target)) >= 0) {
                            Globals::TalkShip->Weapons[I]->Target = nullptr;
                        }
                    }
                }
            }
        } else {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.PreserveItems.", Globals::TalkShip->GetTypeNameKey(), u"No"}));
        }
        BuildStandardChoices(true);
    }

    void TfTalk::ShowPartnerOffer(std::int32_t Action) {
        pas::WideString Response{};
        if (aShip::TShip_RelationToShip(Globals::TalkShip, aPlayer::GetPlayer()) < 45) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.Suspect"_wref.get());
            BuildStandardChoices(true);
        } else if (Globals::TalkShip->PartnerShip != nullptr) {
            DialogText = ([&] {
                auto name = pas::borrow(pas::checked_cast<aRanger::TRanger*>(Globals::TalkShip->PartnerShip)->Name);
                pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.AlreadyHavePartner"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Partner>"_w, name.get());
            }());
            BuildStandardChoices(true);
        } else if (pas::checked_cast<aRanger::TRanger*>(Globals::TalkShip)->CountWingmen() > 0) {
            DialogText = ([&] {
                auto name_2 = pas::borrow(aPlayer::GetPlayer()->Name);
                pas::WideString lookupTalkText_2 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ILeader"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupTalkText_2), u"<color=255,240,100>"_w, u"<Ranger>"_w, name_2.get());
            }());
            BuildStandardChoices(true);
        } else {
            std::int32_t cpp_left = aPlayer::GetPlayer()->GetEffectiveSkillLevel(aShip::psLeadership, false);
            if (cpp_left <= aPlayer::GetPlayer()->CountWingmen()) {
                DialogText = ([&] {
                    auto name_3 = pas::borrow(aPlayer::GetPlayer()->Name);
                    pas::WideString lookupTalkText_3 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.NeedLeadership"_wref.get());
                    return aMyFunction::FormatText1(std::move(lookupTalkText_3), u"<color=255,240,100>"_w, u"<Ranger>"_w, name_3.get());
                }());
                BuildStandardChoices(true);
            } else if (pas::checked_cast<aRanger::TRanger*>(Globals::TalkShip)->Rank > aPlayer::GetPlayer()->Rank) {
                DialogText = ([&] {
                    auto name_4 = pas::borrow(aPlayer::GetPlayer()->Name);
                    pas::WideString lookupTalkText_4 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.YouNeedInMoreRank"_wref.get());
                    return aMyFunction::FormatText1(std::move(lookupTalkText_4), u"<color=255,240,100>"_w, u"<Ranger>"_w, name_4.get());
                }());
                BuildStandardChoices(true);
            } else {
                if (Globals::TalkShip->virtual_TShip_BuildPartnershipOfferResponse(aPlayer::GetPlayer(), Response, PartnerOfferAmount)) {
                    DialogText = ([&] {
                        pas::WideString intToStr = pas::wide_int_to_str(PartnerOfferAmount);
                        pas::WideString intToStr_2 = pas::wide_int_to_str(Globals::TalkShip->CalculatePartnershipMonths(PartnerOfferAmount, aPlayer::GetPlayer()));
                        pas::WideString lookupTalkText_5 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ComputerSayOk"_wref.get());
                        return aMyFunction::FormatText2(std::move(lookupTalkText_5), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr), u"<Month>"_w, std::move(intToStr_2));
                    }());
                } else {
                    DialogText = ([&] {
                        pas::WideString intToStr_3 = pas::wide_int_to_str(PartnerOfferAmount);
                        pas::WideString lookupTalkText_6 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ComputerSayNo"_wref.get());
                        return aMyFunction::FormatText1(std::move(lookupTalkText_6), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_3));
                    }());
                }
                ClearChoices(false);
                if (Globals::TalkShip->virtual_TShip_BuildPartnershipOfferResponse(aPlayer::GetPlayer(), Response, PartnerOfferAmount)) {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfTalk::AcceptPartnerOffer>(this), 0);
                }
                if (PartnerOfferAmount / 2 > 0) {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.PlayerLess"_wref.get())}), 0, pas::bind_method<&TfTalk::HalvePartnerOffer>(this), 0);
                }
                if (aPlayer::GetPlayer()->Money > PartnerOfferAmount) {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.PlayerMore"_wref.get())}), 0, pas::bind_method<&TfTalk::DoublePartnerOffer>(this), 0);
                }
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Cancel"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
            }
        }
    }

    void TfTalk::AcceptPartnerOffer(std::int32_t Action) {
        if (Globals::TalkShip->virtual_TShip_AcceptPartnershipOffer(aPlayer::GetPlayer(), DialogText, PartnerOfferAmount)) {
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        }
        BuildStandardChoices(true);
    }

    void TfTalk::HalvePartnerOffer(std::int32_t Action) {
        PartnerOfferAmount = PartnerOfferAmount / 2;
        ShowPartnerOffer(0);
    }

    void TfTalk::DoublePartnerOffer(std::int32_t Action) {
        if (aPlayer::GetPlayer()->Money < PartnerOfferAmount * 2) {
            PartnerOfferAmount = aPlayer::GetPlayer()->Money;
        } else {
            PartnerOfferAmount *= 2;
        }
        ShowPartnerOffer(0);
    }

    void TfTalk::OrderPartnerFollow(std::int32_t Action) {
        Globals::TalkShip->OrderFollowShip(aPlayer::GetPlayer(), 0, true);
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ComputerAgreeFlyToMe"_wref.get());
        if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
            DialogText = pas::concat_wide({DialogText, u"\r\n"});
            DialogText = pas::concat_wide_reverse({aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get()), DialogText});
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this), 0);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForYou"_wref.get())}), 0, pas::bind_method<&TfTalk::ExitPartnerConversation>(this), 0);
        } else {
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        }
    }

    void TfTalk::OrderPartnerLand(std::int32_t Action) {
        pas::WideString Name{};
        Globals::TalkShip->OrderLanding(aPlayer::GetPlayer()->OrderTarget, true);
        if (pas::class_cast_if<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
            Name = pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->Name;
        } else if (pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
            Name = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget)->GetColoredFullName(u""_wref.get());
        }
        DialogText = aMyFunction::FormatText1(aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ComputerAgreeLandingToObject"_wref.get()), u"<color=255,240,100>"_w, u"<ObjectName>"_w, Name);
        if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
            DialogText = pas::concat_wide({DialogText, u"\r\n"});
            DialogText = pas::concat_wide_reverse({aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get()), DialogText});
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this), 0);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForYou"_wref.get())}), 0, pas::bind_method<&TfTalk::ExitPartnerConversation>(this), 0);
        } else {
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        }
    }

    void TfTalk::OrderPartnerJump(std::int32_t Action) {
        Globals::TalkShip->OrderJump(pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget), true);
        DialogText = ([&] {
            auto name = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget)->Name);
            pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ComputerAgreeFlyToStar"_wref.get());
            return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Star>"_w, name.get());
        }());
        if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
            DialogText = pas::concat_wide({DialogText, u"\r\n"});
            DialogText = pas::concat_wide_reverse({aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get()), DialogText});
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this), 0);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.OrderForYou"_wref.get())}), 0, pas::bind_method<&TfTalk::ExitPartnerConversation>(this), 0);
        } else {
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        }
    }

    void TfTalk::OrderPartnerDropCargo(std::int32_t Action) {
        {
            pas::Extended cpp_left = Globals::TalkShip->NextRandomInteger(5, 15);
            std::int32_t cpp_arg = -System::Round(pas::real_divide(cpp_left, aConst::PlanetRaceMarket[Globals::TalkShip->PilotRace].PirateRelationFactor));
            void* player = aPlayer::GetPlayer();
            aShip::TShip* talkShip = Globals::TalkShip;
            talkShip->ChangeRelationToRanger(player, cpp_arg);
        }
        if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
            Globals::TalkShip->ChangeRelationToRanger(aPlayer::GetPlayer(), 10);
        }
        if (!aPlayer::GetPlayer()->CanResolveObjectWithScanner(Globals::TalkShip)) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ComputerDropCargoNo"_wref.get());
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        } else {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ComputerDropCargoOk"_wref.get());
            Globals::TalkShip->DropUnequippedItemsAndGoods();
            BuildStandardChoices(true);
        }
    }

    void TfTalk::ShowPartnerFinances(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.FinancesReport"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Globals::TalkShip->Money), u"<color=255,240,100>"_w);
        ClearChoices(false);
        if (aPlayer::GetPlayer()->Money > 0) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::ShowPartnerGift>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.FinancesOfferGift"_wref.get())});
            TfTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfTalk::ShowGreeting>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.FinancesConfirmed"_wref.get())});
            TfTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3, 0);
        }
    }

    void TfTalk::ShowPartnerGift(std::int32_t Action) {
        pas::WideString Text{};
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.FinancesWaitForGift"_wref.get());
        ClearChoices(false);
        Text = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.FinancesSendGift"_wref.get())});
        aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(PartnerGiftAmount), u"<color=255,240,100>"_w);
        AddChoice(Text, 0, pas::bind_method<&TfTalk::GivePartnerGift>(this), 0);
        if (PartnerGiftAmount / 2 > 0) {
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.PlayerLess"_wref.get())}), 0, pas::bind_method<&TfTalk::HalvePartnerGift>(this), 0);
        }
        if (aPlayer::GetPlayer()->Money > PartnerGiftAmount) {
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Partner.PlayerMore"_wref.get())}), 0, pas::bind_method<&TfTalk::DoublePartnerGift>(this), 0);
        }
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Cancel"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
    }

    void TfTalk::GivePartnerGift(std::int32_t Action) {
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        std::int32_t Change = System::Round(pas::real_divide(150 * PartnerGiftAmount, std::max<std::int32_t>(1, Globals::TalkShip->Wealth + aPlayer::GetPlayer()->Wealth)) * aConst::PlanetRaceMarket[Globals::TalkShip->PilotRace].FriendlyRelationScale);
        Change = std::max<std::int32_t>(0, std::min<std::int32_t>(100, Change));
        Globals::TalkShip->ChangeRelationToRanger(aPlayer::GetPlayer(), Change);
        fTalk::PayPartnerGiftMoney();
        Globals::TalkShip->SetMoney(Globals::TalkShip->Money + PartnerGiftAmount);
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.FinancesGotGift"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Globals::TalkShip->Money), u"<color=255,240,100>"_w);
        BuildStandardChoices(true);
    }

    void TfTalk::HalvePartnerGift(std::int32_t Action) {
        PartnerGiftAmount = PartnerGiftAmount / 2;
        ShowPartnerGift(0);
    }

    void TfTalk::DoublePartnerGift(std::int32_t Action) {
        PartnerGiftAmount *= 2;
        if (aPlayer::GetPlayer()->Money < PartnerGiftAmount) {
            PartnerGiftAmount = aPlayer::GetPlayer()->Money;
        }
        ShowPartnerGift(0);
    }

    void TfTalk::ApplyOrderToAllPartners(std::int32_t Action) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
            if (aPlayer::GetPlayer() == Ship->PartnerShip && Ship->InNormalSpace() && aPlayer::GetPlayer() != Ship && Globals::TalkShip != Ship) {
                if (aPlayer::GetPlayer() == Globals::TalkShip->OrderTarget) {
                    Ship->OrderFollowShip(aPlayer::GetPlayer(), 0, true);
                } else if (Globals::TalkShip->Order == aShip::soFollowShip) {
                    Ship->SetJointAttackTarget(Ship, pas::checked_cast<aShip::TShip*>(Globals::TalkShip->OrderTarget));
                } else if (Globals::TalkShip->Order == aShip::soLand) {
                    Ship->OrderLanding(aPlayer::GetPlayer()->OrderTarget, true);
                } else if (Globals::TalkShip->Order == aShip::soJump) {
                    Ship->OrderJump(pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget), true);
                }
            }
        }
        FastExit(0);
    }

    void TfTalk::ExitPartnerConversation(std::int32_t Action) {
        FastExit(0);
    }

    void TfTalk::OrderTranclucatorFollow(std::int32_t Action) {
        aTranclucator::TTranclucator* Ship = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        Ship->OrderFollowShip(aPlayer::GetPlayer(), 0, true);
        Ship->FollowOwner = false;
        Ship->SeekItems = false;
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.FlyToMe.Ok"_wref.get());
        ClearChoices(false);
        AddTranclucatorGroupChoice();
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::OrderTranclucatorReturn(std::int32_t Action) {
        aTranclucator::TTranclucator* Ship = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        Ship->OrderFollowShip(Ship->OwnerShip, 1, false);
        Ship->FollowOwner = true;
        Ship->SeekItems = false;
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.Return.Ok"_wref.get());
        ClearChoices(false);
        AddTranclucatorGroupChoice();
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::OrderTranclucatorSeekItems(std::int32_t Action) {
        aTranclucator::TTranclucator* Ship = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        Ship->FollowOwner = false;
        Ship->SeekItems = true;
        if (Ship->CargoFreeSpace > 0) {
            Ship->TryCollectPreferredFloatingLoot(50);
        }
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.SeekItems.Ok"_wref.get());
        ClearChoices(false);
        AddTranclucatorGroupChoice();
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::CancelTranclucatorSeekItems(std::int32_t Action) {
        aTranclucator::TTranclucator* Ship = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        Ship->FollowOwner = false;
        Ship->SeekItems = false;
        Ship->UpdateFreeFlightOrder();
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.SeekItems.Cancel"_wref.get());
        ClearChoices(false);
        AddTranclucatorGroupChoice();
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::ShowTranclucatorOptions(std::int32_t Action) {
        aTranclucator::TTranclucator* Ship{};
        std::uint32_t Digit{};
        std::int32_t I{};
        pas::WideString Text{};
        auto AddTranclucatorCollectionOption = [&](std::int32_t Kind) -> void {
            std::int32_t Value{};
            pas::WideString Caption{};
            if (Ship->GetCollectionPermission(static_cast<aTranclucator::TTranclucatorCollectionKind>(Kind))) {
                Caption = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Talk.Tranclucator.Options.CollectNo"_wref.get()), u"<color=255,0,0>"_w);
                Value = Kind * 10;
            } else {
                Caption = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Talk.Tranclucator.Options.CollectYes"_wref.get()), u"<color=45,105,45>"_w);
                Value = Kind * 10 + 1;
            }
            AddChoice(pas::concat_wide({u"- ", aMyFunction::FormatText1(Caption, u"<color=255,240,100>"_w, u"<Item>"_w, aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Talk.Tranclucator.Options.Collect", SysUtils::IntToStr(Kind)}))))}), Value, pas::bind_method<&TfTalk::ShowTranclucatorOptions>(this), 0);
        };
        auto GetTranclucatorCollectionText = [&]() -> pas::WideString {
            pas::WideString Result{};
            std::int32_t Kind{};
            for (auto cpp_range = pas::for_to<std::int32_t>(1, 6); cpp_range.next(Kind); ) {
                if (Ship->GetCollectionPermission(static_cast<aTranclucator::TTranclucatorCollectionKind>(Kind))) {
                    if (Result.length() > 0) {
                        Result = pas::concat_wide({Result, u", "});
                    }
                    Result = pas::concat_wide({Result, aShip::TShip_LookupTalkText(Globals::TalkShip, static_cast<pas::WideString>(pas::concat_ansi({"Talk.Tranclucator.Options.Collect", SysUtils::IntToStr(Kind)})))});
                }
            }
            return Result;
        };
        auto AddTranclucatorStorageOption = [&](std::int32_t Kind) -> void {
            std::int32_t Value{};
            pas::WideString Caption{};
            if (Ship->GetStoragePermission(static_cast<aTranclucator::TTranclucatorStorageKind>(Kind))) {
                Caption = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Talk.Tranclucator.Options.LandNo"_wref.get()), u"<color=255,0,0>"_w);
                Value = Kind * 1000;
            } else {
                Caption = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Talk.Tranclucator.Options.LandYes"_wref.get()), u"<color=45,105,45>"_w);
                Value = Kind * 1000 + 100;
            }
            AddChoice(pas::concat_wide({u"- ", aMyFunction::FormatText1(Caption, u"<color=255,240,100>"_w, u"<Land>"_w, aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Talk.Tranclucator.Options.Land", SysUtils::IntToStr(Kind)}))))}), Value, pas::bind_method<&TfTalk::ShowTranclucatorOptions>(this), 0);
        };
        auto GetTranclucatorStorageText = [&]() -> pas::WideString {
            pas::WideString Result{};
            std::int32_t Kind{};
            for (auto cpp_range = pas::for_to<std::int32_t>(1, 2); cpp_range.next(Kind); ) {
                if (Ship->GetStoragePermission(static_cast<aTranclucator::TTranclucatorStorageKind>(Kind))) {
                    if (Result.length() > 0) {
                        Result = pas::concat_wide({Result, u", "});
                    }
                    Result = pas::concat_wide({Result, aShip::TShip_LookupTalkText(Globals::TalkShip, static_cast<pas::WideString>(pas::concat_ansi({"Talk.Tranclucator.Options.Land", SysUtils::IntToStr(Kind)})))});
                }
            }
            return Result;
        };
        auto AddTranclucatorArrangeOption = [&]() -> void {
            std::int32_t Value{};
            pas::WideString Caption{};
            if (Ship->AutoArrange) {
                Caption = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Talk.Tranclucator.Options.ArrangeNo"_wref.get()), u"<color=255,0,0>"_w);
                Value = 10000;
            } else {
                Caption = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Talk.Tranclucator.Options.ArrangeYes"_wref.get()), u"<color=45,105,45>"_w);
                Value = 20000;
            }
            AddChoice(pas::concat_wide({u"- ", Caption}), Value, pas::bind_method<&TfTalk::ShowTranclucatorOptions>(this), 0);
        };
        auto GetTranclucatorArrangeText = [&]() -> pas::WideString {
            pas::WideString Result{};
            if (Ship->AutoArrange) {
                return pas::concat_wide({Result, aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.Options.ArrangeText"_wref.get())});
            }
            return pas::concat_wide({Result, aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.Options.ArrangeBad"_wref.get())});
        };
        auto PopTranclucatorOptionDigit = [&]() -> void {
            Digit = static_cast<std::uint32_t>(Action) % 10;
            pas::store_unaligned<std::uint32_t>(&Action, static_cast<std::uint32_t>(Action) / 10);
        };
        DialogText = pas::WideString();
        Ship = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        PopTranclucatorOptionDigit();
        std::uint8_t Enabled = Digit == 1;
        PopTranclucatorOptionDigit();
        Ship->SetCollectionPermission(static_cast<aTranclucator::TTranclucatorCollectionKind>(Digit), Enabled);
        PopTranclucatorOptionDigit();
        Enabled = Digit == 1;
        PopTranclucatorOptionDigit();
        Ship->SetStoragePermission(static_cast<aTranclucator::TTranclucatorStorageKind>(Digit), Enabled);
        PopTranclucatorOptionDigit();
        if (Digit == 2) {
            Ship->AutoArrange = true;
        } else if (Digit == 1) {
            Ship->AutoArrange = false;
        }
        if (Ship->GetCargoHook() != nullptr) {
            Text = GetTranclucatorCollectionText();
        } else {
            Text = pas::WideString();
        }
        if (Text == u"") {
            Text = u"---"_w;
        }
        aConst::ExpandLocalizedTextMarkup(Text);
        DialogText = pas::concat_wide_reverse({([&] {
            auto text = pas::borrow(Text);
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"Talk.Tranclucator.Options.CollectText"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<List>"_w, text.get());
        }()), DialogText});
        DialogText = pas::concat_wide({DialogText, u"\r\n"});
        if (Ship->GetCargoHook() != nullptr) {
            Text = GetTranclucatorStorageText();
        } else {
            Text = pas::WideString();
        }
        if (Text.length() > 0) {
            aConst::ExpandLocalizedTextMarkup(Text);
            DialogText = pas::concat_wide_reverse({([&] {
                auto text_2 = pas::borrow(Text);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"Talk.Tranclucator.Options.LandText"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<List>"_w, text_2.get());
            }()), DialogText});
        } else {
            DialogText = pas::concat_wide_reverse({aConst::LocalizedColorText(u"Talk.Tranclucator.Options.LandBad"_wref.get()), DialogText});
        }
        DialogText = pas::concat_wide({DialogText, u"\r\n"});
        Text = GetTranclucatorArrangeText();
        aConst::ExpandLocalizedTextMarkup(Text);
        DialogText = pas::concat_wide({DialogText, Text});
        RememberChoiceScroll();
        ClearChoices(true);
        if (Ship->GetCargoHook() != nullptr) {
            for (I = 1; I <= 6; ++I) {
                AddTranclucatorCollectionOption(I);
            }
        }
        if (Ship->GetCargoHook() != nullptr) {
            for (I = 1; I <= 2; ++I) {
                AddTranclucatorStorageOption(I);
            }
        }
        AddTranclucatorArrangeOption();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"Talk.Tranclucator.Options.PlayerBack"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
    }

    void TfTalk::OrderTranclucatorDropCargo(std::int32_t Action) {
        aTranclucator::TTranclucator* Ship = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        Ship->FollowOwner = false;
        Ship->SeekItems = false;
        Ship->DropUnequippedItemsAndGoods();
        Ship->UpdateFreeFlightOrder();
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.DropCargo.Ok"_wref.get());
        ClearChoices(false);
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::OrderTranclucatorLand(std::int32_t Action) {
        pas::WideString Name{};
        aTranclucator::TTranclucator* Ship = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        Ship->FollowOwner = false;
        Ship->SeekItems = false;
        Ship->StoreOnLanding = false;
        Ship->OrderLanding(aPlayer::GetPlayer()->OrderTarget, true);
        if (pas::class_cast_if<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
            Name = pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->Name;
        } else if (pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
            Name = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget)->GetColoredFullName(u""_wref.get());
        }
        DialogText = aMyFunction::FormatText1(aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.AgreeLandingToObject"_wref.get()), u"<color=255,240,100>"_w, u"<ObjectName>"_w, Name);
        ClearChoices(false);
        AddTranclucatorGroupChoice();
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::OrderTranclucatorStoreCargo(std::int32_t Action) {
        pas::WideString Name{};
        aTranclucator::TTranclucator* Ship = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        Ship->FollowOwner = false;
        Ship->SeekItems = false;
        Ship->StoreOnLanding = true;
        Ship->OrderLanding(aPlayer::GetPlayer()->OrderTarget, true);
        if (pas::class_cast_if<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
            Name = pas::checked_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget)->Name;
        } else {
            Name = pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->Name;
        }
        DialogText = aMyFunction::FormatText1(aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Tranclucator.AgreeLandingToStorage"_wref.get()), u"<color=255,240,100>"_w, u"<ObjectName>"_w, Name);
        ClearChoices(false);
        AddTranclucatorGroupChoice();
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::AddTranclucatorGroupChoice() {
        aShip::TShip* Ship{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Globals::TalkShip->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Globals::TalkShip->CurrentStar->Ships, I);
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr && Ship != Globals::TalkShip && static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == aPlayer::GetPlayer() && Ship->InNormalSpace()) {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Tranclucator.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllTranclucators>(this), 0);
                break;
            }
        }
    }

    void TfTalk::ApplyOrderToAllTranclucators(std::int32_t Action) {
        aTranclucator::TTranclucator* Target{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        aTranclucator::TTranclucator* Current = pas::checked_cast<aTranclucator::TTranclucator*>(Globals::TalkShip);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Current->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Current->CurrentStar->Ships, I);
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr && Ship != Current && static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == aPlayer::GetPlayer() && Ship->InNormalSpace()) {
                Target = reinterpret_cast<aTranclucator::TTranclucator*>(Ship);
                Target->FollowOwner = Current->FollowOwner;
                Target->SeekItems = Current->SeekItems;
                Target->StoreOnLanding = Current->StoreOnLanding;
                Target->Order = Current->Order;
                Target->OrderStateData = Current->OrderStateData;
                Target->OrderTarget = Current->OrderTarget;
                Target->OrderDestination = Current->OrderDestination;
                Target->OrderAbsolute = Current->OrderAbsolute;
                if (Current->OrderTarget == Current->EnemyShip) {
                    Target->EnemyShip = Current->EnemyShip;
                }
                if (Target->SeekItems) {
                    if (Current->CargoFreeSpace > 0) {
                        Target->TryCollectPreferredFloatingLoot(50);
                    } else {
                        Target->UpdateFreeFlightOrder();
                    }
                }
            }
        }
        FastExit(0);
    }

    void TfTalk::ShowPiratePartnerOffer(std::int32_t Action) {
        pas::WideString Response{};
        if (aShip::TShip_RelationToShip(Globals::TalkShip, aPlayer::GetPlayer()) < 45) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.Suspect"_wref.get());
            BuildStandardChoices(true);
        } else if (Globals::TalkShip->PartnerShip != nullptr) {
            DialogText = ([&] {
                auto name = pas::borrow(pas::checked_cast<aRanger::TRanger*>(Globals::TalkShip->PartnerShip)->Name);
                pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.AlreadyHavePartner"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Partner>"_w, name.get());
            }());
            BuildStandardChoices(true);
        } else if (aPlayer::GetPlayer()->GetMaxPiratePartners() <= pas::list_count(aPlayer::GetPlayer()->PiratePartners) || ([&] {
            std::int32_t cpp_left = aPlayer::GetPlayer()->GetEffectiveSkillLevel(aShip::psLeadership, false);
            return cpp_left <= aPlayer::GetPlayer()->CountWingmen();
        }()) || pas::class_cast_if<aPirate::TPirate*>(Globals::TalkShip) != nullptr && Globals::TalkShip->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && static_cast<aPirate::TPirate*>(Globals::TalkShip)->PirateRank > aPlayer::GetPlayer()->PirateRank) {
            DialogText = ([&] {
                auto name_2 = pas::borrow(aPlayer::GetPlayer()->Name);
                pas::WideString lookupTalkText_2 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.NeedPirate"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupTalkText_2), u"<color=255,240,100>"_w, u"<Ranger>"_w, name_2.get());
            }());
            BuildStandardChoices(true);
        } else {
            if (Globals::TalkShip->virtual_TShip_BuildPartnershipOfferResponse(aPlayer::GetPlayer(), Response, PartnerOfferAmount)) {
                DialogText = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(PartnerOfferAmount);
                    pas::WideString intToStr_2 = pas::wide_int_to_str(Globals::TalkShip->CalculatePartnershipMonths(PartnerOfferAmount, aPlayer::GetPlayer()));
                    pas::WideString lookupTalkText_3 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.ComputerSayOk"_wref.get());
                    return aMyFunction::FormatText2(std::move(lookupTalkText_3), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr), u"<Month>"_w, std::move(intToStr_2));
                }());
            } else {
                DialogText = ([&] {
                    pas::WideString intToStr_3 = pas::wide_int_to_str(PartnerOfferAmount);
                    pas::WideString lookupTalkText_4 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.ComputerSayNo"_wref.get());
                    return aMyFunction::FormatText1(std::move(lookupTalkText_4), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_3));
                }());
            }
            ClearChoices(false);
            if (Globals::TalkShip->virtual_TShip_BuildPartnershipOfferResponse(aPlayer::GetPlayer(), Response, PartnerOfferAmount)) {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfTalk::AcceptPiratePartnerOffer>(this), 0);
            }
            if (PartnerOfferAmount / 2 > 0) {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.PlayerLess"_wref.get())}), 0, pas::bind_method<&TfTalk::HalvePiratePartnerOffer>(this), 0);
            }
            if (aPlayer::GetPlayer()->Money > PartnerOfferAmount) {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.PlayerMore"_wref.get())}), 0, pas::bind_method<&TfTalk::DoublePiratePartnerOffer>(this), 0);
            }
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Cancel"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
        }
    }

    void TfTalk::AcceptPiratePartnerOffer(std::int32_t Action) {
        if (Globals::TalkShip->virtual_TShip_AcceptPartnershipOffer(aPlayer::GetPlayer(), DialogText, PartnerOfferAmount)) {
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            pas::list_add(aPlayer::GetPlayer()->PiratePartners, reinterpret_cast<void*>(Globals::TalkShip));
        }
        BuildStandardChoices(true);
    }

    void TfTalk::HalvePiratePartnerOffer(std::int32_t Action) {
        PartnerOfferAmount = PartnerOfferAmount / 2;
        ShowPiratePartnerOffer(0);
    }

    void TfTalk::DoublePiratePartnerOffer(std::int32_t Action) {
        if (aPlayer::GetPlayer()->Money < PartnerOfferAmount * 2) {
            PartnerOfferAmount = aPlayer::GetPlayer()->Money;
        } else {
            PartnerOfferAmount *= 2;
        }
        ShowPiratePartnerOffer(0);
    }

    void TfTalk::ShowPirateAttackTargets(std::int32_t Action) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.AttackList"_wref.get());
        ClearChoices(false);
        std::uint8_t ReservedFlag = false;
        std::uint8_t FollowMode = ReservedFlag;
        std::int32_t cpp_left = aPlayer::GetPlayer()->GetRadarRange();
        std::int32_t RadarRangeSquared = cpp_left * aPlayer::GetPlayer()->GetRadarRange();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
            if ((Globals::TalkShip->OrderTarget != Ship || Globals::TalkShip->Order != aShip::soFollowShip || Globals::TalkShip->OrderStateData == FollowMode) && aPlayer::GetPlayer() != Ship && Globals::TalkShip != Ship && aPlayer::GetPlayer() != Ship->PartnerShip && Ship->InNormalSpace()) {
                if (static_cast<long double>(RadarRangeSquared) > aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, Ship->Position) && static_cast<std::uint8_t>(pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) ^ 1) && (Ship->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || Globals::TalkShip->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aGalaxy::Galaxy->CoalitionDefeatedTurn != 0)) {
                    AddChoice(pas::concat_wide({u"- ", Ship->GetFullName(u" "_wref.get()), aGalaxy::GetLocalObjectLink(Ship, false)}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfTalk::OrderPiratePartnerAttack>(this), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)));
                }
            }
        }
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.Back"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
    }

    void TfTalk::OrderPiratePartnerAttack(std::int32_t Action) {
        aShip::TShip* Target = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        Globals::TalkShip->SetJointAttackTarget(Globals::TalkShip, Target);
        DialogText = ([&] {
            pas::WideString fullName = Target->GetFullName(u" "_wref.get());
            pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.AttackShipOk"_wref.get());
            return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<ShipName>"_w, std::move(fullName));
        }());
        if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get())});
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this), 0);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.OrderForYou"_wref.get())}), 0, pas::bind_method<&TfTalk::ExitPartnerConversation>(this), 0);
        } else {
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        }
    }

    void TfTalk::OrderPiratePartnerFollow(std::int32_t Action) {
        Globals::TalkShip->OrderFollowShip(aPlayer::GetPlayer(), 0, true);
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.ComputerAgreeFlyToMe"_wref.get());
        if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get())});
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this), 0);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.OrderForYou"_wref.get())}), 0, pas::bind_method<&TfTalk::ExitPartnerConversation>(this), 0);
        } else {
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        }
    }

    void TfTalk::OrderPiratePartnerLand(std::int32_t Action) {
        pas::WideString Name{};
        std::uint8_t Relation{};
        if (pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
            Name = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget)->GetColoredFullName(u""_wref.get());
            Relation = aShip::TShip_RelationToShip(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->OrderTarget), Globals::TalkShip);
        } else {
            Name = pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->Name;
            Relation = pas::checked_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->RelationToShip(Globals::TalkShip);
        }
        if (Relation < 10) {
            DialogText = aMyFunction::FormatText1(aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.ComputerDisagreeLandingToObject"_wref.get()), u"<color=255,240,100>"_w, u"<ObjectName>"_w, Name);
        } else {
            Globals::TalkShip->OrderLanding(aPlayer::GetPlayer()->OrderTarget, true);
            DialogText = aMyFunction::FormatText1(aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.ComputerAgreeLandingToObject"_wref.get()), u"<color=255,240,100>"_w, u"<ObjectName>"_w, Name);
        }
        if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get())});
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this), 0);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.OrderForYou"_wref.get())}), 0, pas::bind_method<&TfTalk::ExitPartnerConversation>(this), 0);
        } else {
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
        }
    }

    void TfTalk::OrderPiratePartnerJump(std::int32_t Action) {
        if (pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget)->CountPlanetsByOwner(aGalaxyStruct::oiDominator) > 0 && (Globals::TalkShip->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || aGalaxy::Galaxy->CoalitionDefeatedTurn == 0)) {
            DialogText = ([&] {
                auto name = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget)->Name);
                pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.ComputerDisagreeFlyToStar"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Star>"_w, name.get());
            }());
            ClearChoices(false);
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.Back"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
        } else {
            Globals::TalkShip->OrderJump(pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget), true);
            DialogText = ([&] {
                auto name_2 = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget)->Name);
                pas::WideString lookupTalkText_2 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.ComputerAgreeFlyToStar"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupTalkText_2), u"<color=255,240,100>"_w, u"<Star>"_w, name_2.get());
            }());
            if (aPlayer::GetPlayer()->CountPartnersInNormalSpace() > 1) {
                DialogText = pas::concat_wide({DialogText, u"\r\n", aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Partner.IsOrderForAll"_wref.get())});
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.OrderForAll"_wref.get())}), 0, pas::bind_method<&TfTalk::ApplyOrderToAllPartners>(this), 0);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.OrderForYou"_wref.get())}), 0, pas::bind_method<&TfTalk::ExitPartnerConversation>(this), 0);
            } else {
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
            }
        }
    }

    void TfTalk::ShowPiratePartnerFinances(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.FinancesReport"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Globals::TalkShip->Money), u"<color=255,240,100>"_w);
        ClearChoices(false);
        if (aPlayer::GetPlayer()->Money > 0) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::ShowPiratePartnerGift>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.FinancesOfferGift"_wref.get())});
            TfTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfTalk::ShowGreeting>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.FinancesConfirmed"_wref.get())});
            TfTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3, 0);
        }
    }

    void TfTalk::ShowPiratePartnerGift(std::int32_t Action) {
        pas::WideString Text{};
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.FinancesWaitForGift"_wref.get());
        ClearChoices(false);
        Text = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.FinancesSendGift"_wref.get())});
        aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(PartnerGiftAmount), u"<color=255,240,100>"_w);
        AddChoice(Text, 0, pas::bind_method<&TfTalk::GivePiratePartnerGift>(this), 0);
        if (PartnerGiftAmount / 2 > 0) {
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.PlayerLess"_wref.get())}), 0, pas::bind_method<&TfTalk::HalvePiratePartnerGift>(this), 0);
        }
        if (aPlayer::GetPlayer()->Money > PartnerGiftAmount) {
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Pirate.PlayerMore"_wref.get())}), 0, pas::bind_method<&TfTalk::DoublePiratePartnerGift>(this), 0);
        }
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Cancel"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowGreeting>(this), 0);
    }

    void TfTalk::GivePiratePartnerGift(std::int32_t Action) {
        std::int32_t Change{};
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        if (pas::class_cast_if<aNormalShip::TNormalShip*>(Globals::TalkShip) != nullptr) {
            Change = System::Round(pas::real_divide(150 * PartnerGiftAmount, std::max<std::int32_t>(1, Globals::TalkShip->Wealth + aPlayer::GetPlayer()->Wealth)) * aConst::PlanetRaceMarket[Globals::TalkShip->PilotRace].FriendlyRelationScale);
        } else {
            Change = System::Round(pas::real_divide(150 * PartnerGiftAmount, std::max<std::int32_t>(1, Globals::TalkShip->Wealth + aPlayer::GetPlayer()->Wealth)));
        }
        Change = std::max<std::int32_t>(0, std::min<std::int32_t>(100, Change));
        Globals::TalkShip->ChangeRelationToRanger(aPlayer::GetPlayer(), Change);
        fTalk::PayPiratePartnerGiftMoney();
        Globals::TalkShip->SetMoney(Globals::TalkShip->Money + PartnerGiftAmount);
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Pirate.FinancesGotGift"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Globals::TalkShip->Money), u"<color=255,240,100>"_w);
        BuildStandardChoices(true);
    }

    void TfTalk::HalvePiratePartnerGift(std::int32_t Action) {
        PartnerGiftAmount = PartnerGiftAmount / 2;
        ShowPiratePartnerGift(0);
    }

    void TfTalk::DoublePiratePartnerGift(std::int32_t Action) {
        PartnerGiftAmount *= 2;
        if (aPlayer::GetPlayer()->Money < PartnerGiftAmount) {
            PartnerGiftAmount = aPlayer::GetPlayer()->Money;
        }
        ShowPiratePartnerGift(0);
    }

    void TfTalk::ShowPartnerDismissal(std::int32_t Action) {
        pas::WideString Prefix{};
        if (Globals::TalkShip->TypeId == aGalaxyStruct::stPirate) {
            Prefix = u"Pirate"_w;
        } else {
            Prefix = u"Partner"_w;
        }
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.", Prefix, u".ComputerDismissQuestion"}));
        ClearChoices(false);
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), pas::concat_wide({u"Talk.", Prefix, u".PlayerDismissNo"}))}), 1, pas::bind_method<&TfTalk::ShipDismissAct>(this), 0);
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), pas::concat_wide({u"Talk.", Prefix, u".PlayerDismissGood"}))}), 2, pas::bind_method<&TfTalk::ShipDismissAct>(this), 0);
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), pas::concat_wide({u"Talk.", Prefix, u".PlayerDismissBad"}))}), 3, pas::bind_method<&TfTalk::ShipDismissAct>(this), 0);
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::ShipDismissAct(std::int32_t Action) {
        pas::WideString Prefix{};
        if (Globals::TalkShip->TypeId == aGalaxyStruct::stPirate) {
            Prefix = u"Pirate"_w;
        } else {
            Prefix = u"Partner"_w;
        }
        switch (Action) {
            case 1: {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.", Prefix, u".ComputerDismissNo"}));
                break;
            }
            case 2: {
                if (Globals::TalkShip->Order == aShip::soFollowShip && Globals::TalkShip->OrderTarget == Globals::TalkShip->PartnerShip) {
                    Globals::TalkShip->OrderNone(false);
                }
                Globals::TalkShip->PartnerShip = nullptr;
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.", Prefix, u".ComputerDismissGood"}));
                break;
            }
            case 3: {
                Globals::TalkShip->PartnerShip = nullptr;
                Globals::TalkShip->ChangeRelationToRanger(aPlayer::GetPlayer(), 10);
                Globals::TalkShip->EnemyShip = aPlayer::GetPlayer();
                Globals::TalkShip->EngageEnemyShip();
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.", Prefix, u".ComputerDismissBad"}));
                break;
            }
            default: {
                GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"ShipDismissAct: invalid data = ", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Action))})));
                break;
            }
        }
        ClearChoices(false);
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
    }

    void TfTalk::RunDominatorProgram(std::int32_t Action) {
        std::uint8_t ProgramIndex = Action;
        std::int32_t Remaining = aPlayer::GetPlayer()->ProgramCounts[ProgramIndex] - 1;
        aPlayer::GetPlayer()->ProgramCounts[ProgramIndex] = Remaining;
        SysUtilsImports::Sleep(1u);
        if (aPlayer::GetPlayer()->ProgramCounts[ProgramIndex] != Remaining && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
            GR_Main::CCInterface->SetTamperDetected(true);
        }
        if (pas::is_one_of<aGalaxyStruct::ktBoss, aGalaxyStruct::ktBertor>(pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->KlingType)) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Dominator.ProgrammNo"_wref.get());
            pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DetectAttackingPlayer(aPlayer::GetPlayer());
            ClearChoices(false);
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::FastExit>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())});
                TfTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
            }
            return;
        }
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Dominator.ProgrammOk"_wref.get());
        {
            pas::WideString programName = (static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(ProgramIndex));
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<Name>"_w, std::move(programName), u"<color=255,240,100>"_w);
        }
        pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->ActiveProgramAppliedTurn = aGalaxy::Galaxy->CurrentTurn;
        pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->ActiveProgramId = ProgramIndex;
        ++aPlayer::GetPlayer()->AchievementStats->SuccessfulDominatorHacks;
        Achievements::TrySetAchievementProgress(u"HACKER"_w, aPlayer::GetPlayer()->AchievementStats->SuccessfulDominatorHacks);
        switch (pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->ActiveProgramId) {
            case aGalaxyStruct::prgShipwreck: {
                std::int32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(1, 3, aGalaxy::Galaxy->CurrentTurn);
                aShip::TShip* talkShip = Globals::TalkShip;
                talkShip->DropItemsForDominatorProgram(seededRandomIntRange);
                break;
            }
            case aGalaxyStruct::prgSelfDestruction: Globals::TalkShip->DestroyQueued = true; break;
            case aGalaxyStruct::prgDisconnection: {
                Globals::TalkShip->OrderNone(false);
                Globals::TalkShip->ClearWeaponTargets(nullptr);
                break;
            }
        }
        pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DetectAttackingPlayer(aPlayer::GetPlayer());
        BuildStandardChoices(true);
    }

    void TfTalk::ShowDominatorGreeting(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Dominator.Hi", aConst::DominatorSeriesNames[pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DominatorSeries]}));
        BuildStandardChoices(true);
    }

    void TfTalk::ShowDominatorPeace(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Dominator.Peace", aConst::DominatorSeriesNames[pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DominatorSeries]}));
        BuildStandardChoices(true);
    }

    void TfTalk::ShowDominatorGoods(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Dominator.Goods", aConst::DominatorSeriesNames[pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DominatorSeries]}));
        BuildStandardChoices(true);
    }

    void TfTalk::ShowDominatorCommand(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.Dominator.Command", aConst::DominatorSeriesNames[pas::checked_cast<aKling::TKling*>(Globals::TalkShip)->DominatorSeries]}));
        BuildStandardChoices(true);
    }

    std::uint8_t TfTalk::AddImmediateAttackChoices() {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        std::uint8_t Result = false;
        std::int32_t cpp_left = aPlayer::GetPlayer()->GetRadarRange();
        std::int32_t RadarRangeSquared = cpp_left * aPlayer::GetPlayer()->GetRadarRange();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
            if (!pas::in_set<1, 2>(Ship->TargetingRestriction)) {
                if (Globals::TalkShip->OrderTarget == Ship && aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, Ship) == aGalaxyStruct::rlHostile && (aPlayer::GetPlayer()->OrderTarget == Ship || aRanger::PendingPlayerFollowTarget == Ship) && aShip::TShip_GetRelationLevelToShip(aPlayer::GetPlayer(), Ship) == aGalaxyStruct::rlHostile) {
                    Result = true;
                } else if (aPlayer::GetPlayer() != Ship && Globals::TalkShip != Ship && Ship->InNormalSpace() && static_cast<long double>(RadarRangeSquared) > aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, Ship->Position)) {
                    if (aShip::TShip_GetRelationLevelToShip(Ship, aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && (aPlayer::GetPlayer() == Ship->OrderTarget && Ship->TypeId != aGalaxyStruct::stKling || aPlayer::GetPlayer()->OrderTarget == Ship)) {
                        AddChoice(pas::concat_wide({u"- ", ([&] {
                            pas::WideString fullName = Ship->GetFullName(u" "_wref.get());
                            pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerSend"_wref.get());
                            return aMyFunction::ReplaceColoredToken(std::move(lookupTalkText), u"<Target>"_w, std::move(fullName), pas::WideString());
                        }()), aGalaxy::GetLocalObjectLink(Ship, false)}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfTalk::RequestAttackTarget>(this), 0);
                        Result = true;
                    } else if (aRanger::PendingPlayerFollowTarget == Ship) {
                        AddChoice(pas::concat_wide({u"- ", ([&] {
                            pas::WideString fullName_2 = Ship->GetFullName(u" "_wref.get());
                            pas::WideString lookupTalkText_2 = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerSend"_wref.get());
                            return aMyFunction::ReplaceColoredToken(std::move(lookupTalkText_2), u"<Target>"_w, std::move(fullName_2), pas::WideString());
                        }()), aGalaxy::GetLocalObjectLink(Ship, false)}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfTalk::RequestAttackTarget>(this), 0);
                        Result = true;
                    } else {
                        const std::int32_t cpp_last = static_cast<std::int32_t>(aPlayer::GetPlayer()->WeaponCount);
                        if (1 <= cpp_last) {
                            for (J = 1; J <= cpp_last; ++J) {
                                Weapon = aPlayer::GetPlayer()->Weapons[J];
                                if (Weapon->Target == Ship) {
                                    AddChoice(pas::concat_wide({u"- ", ([&] {
                                        pas::WideString fullName_3 = Ship->GetFullName(u" "_wref.get());
                                        pas::WideString lookupTalkText_3 = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Attack.PlayerSend"_wref.get());
                                        return aMyFunction::ReplaceColoredToken(std::move(lookupTalkText_3), u"<Target>"_w, std::move(fullName_3), pas::WideString());
                                    }()), aGalaxy::GetLocalObjectLink(Ship, false)}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfTalk::RequestAttackTarget>(this), 0);
                                    Result = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        return Result;
    }

    pas::WideString TfTalk::GetShipGreeting() {
        pas::WideString Result{};
        Result = aShip::TShip_GetGreetingText(Globals::TalkShip);
        if (Result == u"") {
            GR_Main::RaiseWideMessage(u"\u041d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u043e \u043f\u0440\u0438\u0432\u0435\u0442\u0441\u0442\u0432\u0438\u0435 \u043a\u043e\u0440\u0430\u0431\u043b\u044f"_wref.get());
        }
        return Result;
    }

    void TfTalk::RunInjectedAnswer(std::int32_t Action) {
        pas::WideString Text{};
        std::int32_t PartCount{};
        aScript::PScriptDialogInjection Injection = reinterpret_cast<aScript::PScriptDialogInjection>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        if (Injection->ActionCode != u"") {
            aScript::CurrentScript = Injection->ActionScript;
            aScript::ExecuteScriptText(Injection->ActionCode, aScript::CurrentScript->InitCode->LocalVar);
        }
        if (Injection->DialogName == u"") {
            FastExit(0);
        } else {
            Text = Injection->Answer;
            PartCount = EC_Str::CountDelimitedPartsW(Text, u"~"_wref.get());
            if (PartCount > 1) {
                Text = EC_Str::ExtractDelimitedPartW(static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Text))), 0, u"~"_wref.get());
                if (Text == u"snap") {
                    RememberChoiceScroll();
                }
            }
            ClearChoices(false);
            Injection->Script->PublishCurrentShip(Globals::TalkShip);
            {
                EC_Expression::TVarEC* var = aScript::CurrentScript->InitCode->LocalVar->GetVar(u"GAnswerData"_wref.get());
                std::uint32_t answerData = Injection->AnswerData;
                var->SetDword(answerData);
            }
            aScript::CurrentScript->CallDialogByVariable(Injection->DialogName);
            if (Globals::ScriptDialogIndex < 0) {
                BuildStandardChoices(false);
            } else {
                SkipShipScriptAdvance = true;
                aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
            }
        }
    }

    void TfTalk::RunInjectedAnswerKeepingScroll(std::int32_t Action) {
        RememberChoiceScroll();
        RunInjectedAnswer(Action);
    }

    void TfTalk::StartScriptMessage(aScript::TScript* Script) {
        ClearChoices(false);
        aScript::CurrentScript = Script;
        aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
    }

    void TfTalk::BuildMilitarySupportChoices() {
        ClearChoices(true);
        switch (Globals::TalkShip->PilotRace) {
            case aGalaxyStruct::oiMaloc: {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendRepairHull"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryHullRepair>(this), 0);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendGetBuff"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryBuff>(this), 0);
                break;
            }
            case aGalaxyStruct::oiPeleng: {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendRepairHull"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryHullRepair>(this), 0);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendSellRemains"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryRemains>(this), 0);
                break;
            }
            case aGalaxyStruct::oiHuman: {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendSellRemains"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryRemains>(this), 0);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendGetBuff"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryBuff>(this), 0);
                break;
            }
            case aGalaxyStruct::oiFeyan: {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendRepairEq"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryEquipmentRepair>(this), 0);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendSellRemains"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryRemains>(this), 0);
                break;
            }
            case aGalaxyStruct::oiGaal: {
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendRepairEq"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryEquipmentRepair>(this), 0);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerSendGetBuff"_wref.get())}), 0, pas::bind_method<&TfTalk::ShowMilitaryBuff>(this), 0);
                break;
            }
        }
        AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.Cancel"_wref.get())}), 0, pas::bind_method<&TfTalk::CancelMilitarySupport>(this), 0);
    }

    void TfTalk::ShowMilitarySupport(std::int32_t Action) {
        std::uint8_t Refused = true;
        if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) <= aGalaxyStruct::rlHostile) {
            DialogText = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.RefuseEnemy"_wref.get());
        } else if (aShip::TShip_GetRelationLevelToShip(Globals::TalkShip, aPlayer::GetPlayer()) <= aGalaxyStruct::rlNormal) {
            DialogText = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.RefuseWary"_wref.get());
        } else if (pas::in_range(aPlayer::GetPlayer()->CurrentStanding, aGalaxyStruct::ssPiratePassive, aGalaxyStruct::ssPirateMilitary)) {
            DialogText = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.RefusePirate"_wref.get());
        } else if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Globals::TalkShip->Position) > 4.0E+2L) {
            DialogText = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.RefuseDistance"_wref.get());
        } else {
            Refused = false;
        }
        if (Refused) {
            BuildStandardChoices(true);
        } else {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, pas::concat_wide({u"Talk.MilitarySupport.Answer", aConst::RaceToSys(Globals::TalkShip->PilotRace)}));
            {
                pas::WideString fullName = Globals::TalkShip->GetFullName(u" "_wref.get());
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<ShipName>"_w, std::move(fullName), u"<color=255,240,100>"_w);
            }
            BuildMilitarySupportChoices();
        }
    }

    void TfTalk::CancelMilitarySupport(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AfterCancel"_wref.get());
        BuildStandardChoices(true);
    }

    void TfTalk::DeclineMilitarySupport(std::int32_t Action) {
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AfterNo"_wref.get());
        BuildMilitarySupportChoices();
    }

    void TfTalk::ShowMilitaryHullRepair(std::int32_t Action) {
        std::int32_t Available{};
        pas::WideString Caption{};
        std::int32_t Cost = fTalk::GetMilitaryHullRepairCost();
        if (Cost <= 0) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerNoNeedToRepairHull"_wref.get());
            BuildMilitarySupportChoices();
        } else {
            Available = std::min<std::int32_t>(Cost, aPlayer::GetPlayer()->GetCarriedNodeCount());
            if (Available > 0) {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerRepairHull"_wref.get());
                aMyFunction::ReplaceTextToken(DialogText, u"<Nodes>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
                ClearChoices(true);
                if (Available == Cost) {
                    Caption = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.RepairHullOk"_wref.get());
                } else {
                    Caption = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.RepairHullPartialOk"_wref.get());
                }
                AddChoice(pas::concat_wide({u"- ", Caption}), Available, pas::bind_method<&TfTalk::AcceptMilitaryHullRepair>(this), 0);
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::DeclineMilitarySupport>(this);
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerNo"_wref.get())});
                    TfTalk* self = this;
                    self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
                }
            } else {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerRepairHullNoNodes"_wref.get());
                aMyFunction::ReplaceTextToken(DialogText, u"<Nodes>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
                BuildMilitarySupportChoices();
            }
        }
    }

    void TfTalk::AcceptMilitaryHullRepair(std::int32_t Action) {
        aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), Action, nullptr);
        float Fraction = pas::real_divide(Action, fTalk::GetMilitaryHullRepairCost());
        std::int32_t RepairAmount = MathImports::Ceil(aMyFunction::RemapClamped(Fraction, 0.0, 1.0, 0.01, aPlayer::GetPlayer()->GetHull()->Weight - aPlayer::GetPlayer()->GetHull()->HullPoints));
        aPlayer::GetPlayer()->GetHull()->HullPoints += RepairAmount;
        aPlayer::GetPlayer()->AddCombatStatusStrength(aShip::cseBWRepairDebuff, RepairAmount, nullptr);
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        SE_Weapon::TWeaponSE* Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
        Effect->SetEndpoints(aPlayer::GetPlayer()->Graphic, aPlayer::GetPlayer()->Graphic);
        // Multiplication by -1 retains the native DCC32 register copy before NEG.
        Effect->SetHit(aConst::OwnerToFilmColor(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)), RepairAmount * -1, false, false);
        pas::list_add(Globals::StarMapScreen->PendingSceneObjects, reinterpret_cast<void*>(Effect));
        GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AfterRepairHull"_wref.get());
        BuildMilitarySupportChoices();
    }

    void TfTalk::ShowMilitaryEquipmentRepair(std::int32_t Action) {
        std::int32_t Available{};
        pas::WideString Caption{};
        std::int32_t Cost = fTalk::GetMilitaryEquipmentRepairCost();
        if (Cost <= 0) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerNoNeedToRepairEq"_wref.get());
            BuildMilitarySupportChoices();
        } else {
            Available = std::min<std::int32_t>(Cost, aPlayer::GetPlayer()->GetCarriedNodeCount());
            if (Available > 0) {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerRepairEq"_wref.get());
                aMyFunction::ReplaceTextToken(DialogText, u"<Nodes>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
                ClearChoices(true);
                if (Available == Cost) {
                    Caption = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.RepairEqOk"_wref.get());
                } else {
                    Caption = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.RepairEqPartialOk"_wref.get());
                }
                AddChoice(pas::concat_wide({u"- ", Caption}), Available, pas::bind_method<&TfTalk::AcceptMilitaryEquipmentRepair>(this), 0);
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::DeclineMilitarySupport>(this);
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerNo"_wref.get())});
                    TfTalk* self = this;
                    self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
                }
            } else {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerRepairEqNoNodes"_wref.get());
                aMyFunction::ReplaceTextToken(DialogText, u"<Nodes>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
                BuildMilitarySupportChoices();
            }
        }
    }

    void TfTalk::AcceptMilitaryEquipmentRepair(std::int32_t Action) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        float Fraction = pas::real_divide(Action, fTalk::GetMilitaryEquipmentRepairCost());
        aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), Action, nullptr);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if ((!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && Item->ItemType != aConst::t_Hull && Item->EquippedFlag != 0 && aShip::TShip_CanRepairEquipmentTech(Globals::TalkShip, Item) && Item->ConditionPercent < 9.0E+1L) {
                Item->ConditionPercent = aMyFunction::RemapClamped(Fraction, 0.0, 1.0, Item->ConditionPercent, 1.0E+2);
                if (Item->ConditionPercent > 0.0L) {
                    Item->BrokenFlag = 0;
                }
            }
        }
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AfterRepairEq"_wref.get());
        BuildMilitarySupportChoices();
    }

    void TfTalk::ShowMilitaryRemains(std::int32_t Action) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Count = 0;
        std::int32_t Cost = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if (Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr && static_cast<std::uint8_t>(fTalk::IsMilitaryProtectedQuestItem(Item) ^ 1)) {
                ++Count;
                if (aGalaxy::Galaxy->DominatorResearch[Item->DominatorSeries].Progress < 1.0E+2L && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Item->DominatorSeries)) {
                    Cost += System::Round(Item->Cost * 1.5L);
                } else {
                    Cost += Item->Cost;
                }
            }
        }
        if (Count <= 0) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerNoRemains"_wref.get());
            BuildMilitarySupportChoices();
        } else {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerSellRemains"_wref.get());
            aMyFunction::ReplaceTextToken(DialogText, u"<Remains>"_w, pas::wide_int_to_str(Count), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<Cost>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
            ClearChoices(true);
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::SellAllMilitaryRemains>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.SellRemainsSellAll"_wref.get())});
                TfTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
            }
            if (Count > 1) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfTalk::SellIndividualMilitaryRemains>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.SellRemainsSellSome"_wref.get())});
                TfTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3, 0);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfTalk::DeclineMilitarySupport>(this);
                pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerNo"_wref.get())});
                TfTalk* self_3 = this;
                self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5, 0);
            }
        }
    }

    void TfTalk::SellAllMilitaryRemains(std::int32_t Action) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Cost = 0;
        {
            const std::int32_t cpp_first = pas::list_count(aPlayer::GetPlayer()->Inventory) - 1;
            if (cpp_first >= 1) {
                for (I = cpp_first; I >= 1; --I) {
                    Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                    if (Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr && static_cast<std::uint8_t>(fTalk::IsMilitaryProtectedQuestItem(Item) ^ 1)) {
                        pas::list_delete(aPlayer::GetPlayer()->Inventory, I);
                        if (aGalaxy::Galaxy->DominatorResearch[Item->DominatorSeries].Progress < 1.0E+2L && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Item->DominatorSeries)) {
                            Cost += System::Round(Item->Cost * 1.5L);
                        } else {
                            Cost += Item->Cost;
                        }
                        fTalk::DonateMilitaryResearchMaterial(Item->DominatorSeries, Item->Weight);
                        pas::free(Item);
                    }
                }
            }
        }
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Cost);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AfterSellRemains"_wref.get());
        BuildMilitarySupportChoices();
    }

    void TfTalk::SellIndividualMilitaryRemains(std::int32_t Action) {
        std::int32_t I{};
        std::int32_t Cost{};
        aItem::TEquipment* Item{};
        pas::WideString Caption{};
        pas::WideString BonusCaption{};
        if (Action != 0) {
            Item = reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
            I = pas::list_indexof(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
            if (I >= 0) {
                pas::list_delete(aPlayer::GetPlayer()->Inventory, I);
                if (aGalaxy::Galaxy->DominatorResearch[Item->DominatorSeries].Progress < 1.0E+2L && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Item->DominatorSeries)) {
                    Cost = System::Round(Item->Cost * 1.5L);
                } else {
                    Cost = Item->Cost;
                }
                fTalk::DonateMilitaryResearchMaterial(Item->DominatorSeries, Item->Weight);
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Cost);
                GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
                pas::free(Item);
            }
        }
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerSellSomeRemains"_wref.get());
        ClearChoices(true);
        BonusCaption = pas::concat_wide({u" ", aMyFunction::WrapTextInColor(GR_Main::LookupLocalizedTextOrEmpty(u"Talk.MilitarySupport.ItemsCool"_wref.get()), u"<color=255,240,100>"_w)});
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if (Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr && static_cast<std::uint8_t>(fTalk::IsMilitaryProtectedQuestItem(Item) ^ 1)) {
                ++Count;
                if (aGalaxy::Galaxy->DominatorResearch[Item->DominatorSeries].Progress < 1.0E+2L && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Item->DominatorSeries)) {
                    Cost = System::Round(Item->Cost * 1.5L);
                } else {
                    Cost = Item->Cost;
                }
                Caption = pas::concat_wide({Item->GetDisplayName(), u" (", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w), u" cr)"});
                if (aGalaxy::Galaxy->DominatorResearch[Item->DominatorSeries].Progress < 1.0E+2L && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Item->DominatorSeries)) {
                    Caption = pas::concat_wide({Caption, BonusCaption});
                }
                AddChoice(pas::concat_wide({u"- ", Caption}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item)), pas::bind_method<&TfTalk::SellIndividualMilitaryRemains>(this), 0);
                DialogText = pas::concat_wide({DialogText, u"\r\n", pas::wide_int_to_str(Count), u") ", Caption});
            }
        }
        if (Count <= 0) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AfterSellLastRemains"_wref.get());
            BuildMilitarySupportChoices();
        } else {
            AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfTalk::DeclineMilitarySupport>(this), 0);
        }
    }

    void TfTalk::ShowMilitaryBuff(std::int32_t Action) {
        pas::WideString Caption{};
        std::int32_t Cost = 100;
        DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AnswerBuff"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Nodes>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
        ClearChoices(true);
        if (aPlayer::GetPlayer()->GetCombatStatusStrength(aShip::cseBWBuff) > 0.01L) {
            Caption = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.BuffProlongateOk"_wref.get());
        } else {
            Caption = aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.BuffOk"_wref.get());
        }
        if (aPlayer::GetPlayer()->GetCarriedNodeCount() >= Cost) {
            AddChoice(pas::concat_wide({u"- ", Caption}), Cost, pas::bind_method<&TfTalk::AcceptMilitaryBuff>(this), 0);
        } else {
            AddChoice(pas::concat_wide({u"- ", Caption}), 0, ScriptDialogBlockCallback, 0);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfTalk::DeclineMilitarySupport>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.MilitarySupport.PlayerNo"_wref.get())});
            TfTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg, 0);
        }
    }

    void TfTalk::AcceptMilitaryBuff(std::int32_t Action) {
        aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), Action, nullptr);
        GR_Main::SoundManager->PlaySound(u"Sound.Buy"_wref.get());
        if (aPlayer::GetPlayer()->GetCombatStatusStrength(aShip::cseBWBuff) > 0.01L) {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AfterBuffProlongate"_wref.get());
        } else {
            DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.MilitarySupport.AfterBuff"_wref.get());
        }
        aPlayer::GetPlayer()->AddCombatStatusStrength(aShip::cseBWBuff, 2.0E+1f, nullptr);
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        BuildMilitarySupportChoices();
    }

    void TfTalk::DiscussOldHull(std::int32_t Action) {
        switch (Action) {
            case 0: {
                if (aPlayer::GetPlayer() == Globals::TalkShip->PartnerShip) {
                    DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswerP"_wref.get());
                    ClearChoices(false);
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend_1P"_wref.get())}), 1, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                } else {
                    DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer"_wref.get());
                    ClearChoices(false);
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend_1"_wref.get())}), 1, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend_2"_wref.get())}), 2, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                }
                break;
            }
            case 1: {
                if (aPlayer::GetPlayer() == Globals::TalkShip->PartnerShip) {
                    DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer_1P"_wref.get());
                } else {
                    DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer_1"_wref.get());
                }
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend_1_1"_wref.get())}), 11, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                break;
            }
            case 2: {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer_2"_wref.get());
                Globals::TalkShip->ChangeRelationToRanger(aPlayer::GetPlayer(), -15);
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                break;
            }
            case 11: {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer_1_1"_wref.get());
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend_1_2"_wref.get())}), 12, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                break;
            }
            case 12: {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer_1_2"_wref.get());
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend_1_3"_wref.get())}), 13, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                break;
            }
            case 13: {
                DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer_1_3"_wref.get());
                ClearChoices(false);
                if (aPlayer::GetPlayer() == Globals::TalkShip->PartnerShip) {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend_1_4P"_wref.get())}), 14, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                } else {
                    AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.ExTalk.OldHullPlayerSend_1_4"_wref.get())}), 14, pas::bind_method<&TfTalk::DiscussOldHull>(this), 0);
                }
                break;
            }
            case 14: {
                if (aPlayer::GetPlayer() == Globals::TalkShip->PartnerShip) {
                    DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer_1_4P"_wref.get());
                } else {
                    DialogText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.ExTalk.OldHullRangerAnswer_1_4"_wref.get());
                }
                ClearChoices(false);
                AddChoice(pas::concat_wide({u"- ", aShip::TShip_LookupTalkText(aPlayer::GetPlayer(), u"Talk.Exit"_wref.get())}), 0, pas::bind_method<&TfTalk::FastExit>(this), 0);
                break;
            }
        }
    }

    void TfTalk::RunScriptRestartAnswer(std::int32_t Action) {
        DialogText = pas::WideString();
        aScript::CurrentScript->ExecuteDialogAnswer(Action);
        Globals::ScriptDialogIndex = -1;
        CodeMsgOut(false);
    }

    void TfTalk::AddScriptRestartChoice(pas::WideString Caption) {
        AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfTalk::RunScriptRestartAnswer>(this), 0);
    }

    void TfTalkA::p_destroy() {
        fTalk::TfTalkA_Destroy(this);
    }

} // namespace fTalk
