#include "layout/fAchievements.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SimpleSteamApi.hpp"
#include "types/Types.hpp"
#include "types/WStringUtils.hpp"
#include "types/Windows_group.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_CountBar.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/fAchievements.hpp"

namespace fAchievements {
    void TfAchievements_Create(TfAchievements* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
    }

    void TfAchievements_Destroy(TfAchievements* Self) {
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfAchievements::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fAchievements... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* cpp_with = GetByName(u""_wref.get());
            cpp_with->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            cpp_with->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* MainPanel = cpp_with->FindByNameRecursive(u"MainPanel"_wref.get());
                MainPanel->SetPosition(ClassesImports::Point((GR_Main::GameScreenWidth - MainPanel->ClientSize.X) / 2, (GR_Main::GameScreenHeight - MainPanel->ClientSize.Y) / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfAchievements::KeyDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"_wref.get()))->UpCallback = pas::bind_method<&TfAchievements::CloseClicked>(this);
    }

    void TfAchievements::OnOpen() {
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        if (GlobalsV::PreviousScreenId != GlobalsV::screenArcadeBattle && GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(true, 0);
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        RebuildAchievementList();
    }

    void TfAchievements::OnClose() {
        std::int32_t I{};
        {
            const std::int32_t cpp_last = Rows.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Achievements::FreeAchievementData(Rows[I].Data);
                }
            }
        }
        Rows.set_length(0);
        if (GlobalsV::RequestedScreenId != GlobalsV::screenArcadeBattle) {
            GR_Main::AuxRenderBuffer->Clear();
        }
        GI_MessageLoop::TMessageLoopGI::OnClose();
    }

    void TfAchievements::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        GR_Main::AuxRenderBuffer->Clear();
        GlobalsV::RequestedScreenId = GlobalsV::AchievementsReturnScreenId;
        RequestClose(1);
    }

    void TfAchievements::KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_ESCAPE) {
            CloseClicked(Sender);
        } else if (Key == WindowsSdk::VK_PRIOR) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
            if (PanelSlot->VerticalScrollBar->Active) {
                PanelSlot->VerticalScrollBar->SetPosition_2(PanelSlot->VerticalScrollBar->Position - PanelSlot->VerticalScrollBar->LargeChange);
            }
        } else if (Key == WindowsSdk::VK_NEXT) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot_2 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
            if (PanelSlot_2->VerticalScrollBar->Active) {
                PanelSlot_2->VerticalScrollBar->SetPosition_2(PanelSlot_2->VerticalScrollBar->Position + PanelSlot_2->VerticalScrollBar->LargeChange);
            }
        }
    }

    void TfAchievements::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            if (PanelSlot->VerticalScrollBar->Active) {
                PanelSlot->VerticalScrollBar->SetPosition_2(PanelSlot->VerticalScrollBar->Position - PanelSlot->VerticalScrollBar->SmallChange);
            }
            GR_Main::PostMouseMoveMessage();
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            if (PanelSlot->VerticalScrollBar->Active) {
                PanelSlot->VerticalScrollBar->SetPosition_2(PanelSlot->VerticalScrollBar->Position + PanelSlot->VerticalScrollBar->SmallChange);
            }
            GR_Main::PostMouseMoveMessage();
        }
    }

    // Ignores the argument and appends. The native insertion-shift loop is retained even though it has no iterations.
    std::int32_t TfAchievements::AppendRow(std::int32_t UnusedIndex) {
        std::int32_t I{};
        std::int32_t Index = Rows.length() - 1 + 1;
        Rows.set_length(Rows.length() - 1 + 2);
        for (auto cpp_range = pas::for_downto<std::int32_t>(Rows.length() - 1, Index + 1); cpp_range.next(I); ) {
            Rows[I] = Rows[I - 1];
        }
        return Index;
    }

    void TfAchievements::RebuildAchievementList() {
        std::int32_t I{};
        GI_Panel::TPanelGI* Row{};
        std::int32_t Index{};
        SimpleSteamApi::PAchievementData Data{};
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
        Panel->FreeOwnedChildren();
        Rows.set_length(0);
        std::int32_t Count = Achievements::GetAvailableAchievementCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(I); ) {
            Index = AppendRow(0);
            Data = Achievements::GetAchievementData(static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[I].Key));
            Rows[Index].Key = static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[I].Key);
            Rows[Index].Data = Data;
            if (Data->HasProgress) {
                Data->Achieved = Data->Achieved || Data->Value >= Data->MaxValue;
            }
        }
        std::int32_t Y = 0;
        {
            const std::int32_t cpp_last = Rows.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (Achievements::AchievementDefinitionTable[I + 1].Key != "HULL" || Rows[I].Data->Achieved) {
                        // The native row-spacing branch survives even though its spacing is zero.
                        if (I != 0) {
                            Y += 0;
                        }
                        Row = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
                        Row->UserValue = I;
                        Row->SetPosition(ClassesImports::Point(0, Y));
                        BuildRow(Row);
                        Y += Row->ClientSize.Y;
                        Row->SetPositionModeW(true);
                        Panel->VerticalScrollBar->SetSmallChange(Row->ClientSize.Y);
                        RefreshRowBackground(I);
                    }
                }
            }
        }
        Panel->UpdateScrollRanges();
        Panel->VerticalScrollBar->SetActive(Y > Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
        I = Panel->VerticalScrollBar->Position;
        Panel->VerticalScrollBar->SetPosition_2(I - 1);
        Panel->VerticalScrollBar->SetPosition_2(I);
        Panel->Invalidate();
    }

    void TfAchievements::BuildRow(GI_MessageLoop::TObjectGI* Owner) {
        std::int32_t Index{};
        Index = Owner->UserValue;
        Rows[Index].Background = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
        {
            GI_Image::TImageGI* cpp_with = Rows[Index].Background;
            cpp_with->SetPosition(ClassesImports::Point(0, 0));
            cpp_with->SetDepth(1.0E+1);
            cpp_with->SetImagePath(u"GI,Bm.FormAchievements.Slot.Inactive"_w);
            cpp_with->SetSize(cpp_with->GetContentSize());
            Owner->SetSize(cpp_with->ClientSize);
            cpp_with->SetActive(true);
        }
        {
            GI_Label::TLabelGI* cpp_with_2 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
            cpp_with_2->SetPosition(ClassesImports::Point(122, 13));
            cpp_with_2->SetSize(ClassesImports::Point(Owner->ClientSize.X - 38, Owner->ClientSize.Y));
            cpp_with_2->SetDepth(7.0);
            cpp_with_2->SetFontName(GlobalsV::NormalBoldFontName);
            cpp_with_2->SetTextAlignX(GI_Main::taxLeft);
            cpp_with_2->SetTextAlignY(GI_Main::tayTop);
            cpp_with_2->SetText(*Rows[Index].Data->Name);
        }
        {
            GI_Label::TLabelGI* cpp_with_3 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
            cpp_with_3->SetPosition(ClassesImports::Point(120, 40));
            cpp_with_3->SetSize(ClassesImports::Point(440, 53));
            cpp_with_3->SetDepth(7.0);
            cpp_with_3->SetFontName(GlobalsV::SmallFontName);
            cpp_with_3->SetTextAlignX(GI_Main::taxCenter);
            cpp_with_3->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with_3->SetText(*Rows[Index].Data->Description);
            cpp_with_3->SetWordWrapEnabled(true);
            cpp_with_3->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        }
        if (Rows[Index].Data->HasProgress && static_cast<std::uint8_t>(Rows[Index].Data->Achieved ^ 1)) {
            TfAchievements::BuildProgressBars(Owner, 0, Rows[Index].Data->MaxValue, Rows[Index].Data->Value, Achievements::GetCurrentAchievementProgress(Rows[Index].Key, Rows[Index].Data->Value));
            {
                GI_Label::TLabelGI* cpp_with_4 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
                cpp_with_4->SetPosition(ClassesImports::Point(600, 45));
                cpp_with_4->SetSize(ClassesImports::Point(205, 20));
                cpp_with_4->SetDepth(7.0);
                cpp_with_4->SetFontName(GlobalsV::SmallFontName);
                cpp_with_4->SetTextAlignX(GI_Main::taxCenter);
                cpp_with_4->SetTextAlignY(GI_Main::tayCenterEx);
                cpp_with_4->SetText(pas::concat_wide({pas::wide_int_to_str(Rows[Index].Data->Value), GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"Achievements.Of"_wref.get()), pas::wide_int_to_str(Rows[Index].Data->MaxValue)}));
                cpp_with_4->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            }
        }
        if (Rows[Index].Data->Achieved) {
            GI_Label::TLabelGI* cpp_with_5 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
            cpp_with_5->SetPosition(ClassesImports::Point(300, 102));
            cpp_with_5->SetSize(ClassesImports::Point(505, 20));
            cpp_with_5->SetDepth(7.0);
            cpp_with_5->SetFontName(GlobalsV::SmallFontName);
            cpp_with_5->SetTextAlignX(GI_Main::taxRight);
            cpp_with_5->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with_5->SetText(pas::concat_wide_reverse({GR_Main::FormatUnixDateTime(Rows[Index].Data->Date), GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"Achievements.Achieved"_wref.get())}));
            cpp_with_5->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        }
        {
            GI_Image::TImageGI* cpp_with_6 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
            cpp_with_6->SetPosition(ClassesImports::Point(30, 34));
            cpp_with_6->SetDepth(7.0);
            cpp_with_6->SetImagePath(pas::concat_wide({u"GI,Bm.FormAchievements.Img.", Rows[Index].Key}));
            if (!Rows[Index].Data->Achieved) {
                cpp_with_6->SetImagePath(pas::concat_wide({cpp_with_6->GetImagePath(), u"D"}));
            }
            cpp_with_6->SetSize(cpp_with_6->GetContentSize());
            cpp_with_6->SetActive(true);
        }
    }

    void TfAchievements::RefreshRowBackground(std::int32_t Index) {
        if (Index >= 0 && Index <= Rows.length() - 1) {
            GI_Image::TImageGI* cpp_with = Rows[Index].Background;
            if (Rows[Index].Data->Achieved) {
                cpp_with->SetImagePath(u"GI,Bm.FormAchievements.Slot.Active"_w);
            } else {
                cpp_with->SetImagePath(u"GI,Bm.FormAchievements.Slot.Inactive"_w);
            }
        }
    }

    void TfAchievements::SelectMusic() {
    }

    void TfAchievements::BuildProgressBars(GI_MessageLoop::TObjectGI* Owner, std::int32_t MinValue, std::int32_t MaxValue, std::int32_t StoredValue, std::int32_t CurrentValue) {
        GI_CountBar::TCountBarGI* Bar = pas::construct_call<GI_CountBar::TCountBarGI>(GI_CountBar::TCountBarGI_Create, Owner);
        Bar->SetPosition(ClassesImports::Point(630, 60));
        Bar->SetSize(ClassesImports::Point(145, 20));
        Bar->SetPositionModeW(false);
        Bar->AfterThumbImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackLeft"}));
        Bar->BeforeThumbImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackRight"}));
        Bar->ThumbButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Bar->ThumbButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Bar->ThumbButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Bar->SetRange(MinValue, MaxValue);
        Bar->SetPositionInternal(std::min<std::int32_t>(StoredValue, MaxValue));
        Bar->SetHitTestDisabled(true);
        Bar->UpdateLayout();
        Bar->Invalidate();
        Bar = pas::construct_call<GI_CountBar::TCountBarGI>(GI_CountBar::TCountBarGI_Create, Owner);
        Bar->SetPosition(ClassesImports::Point(630, 60));
        Bar->SetDepth(Bar->Depth - 1.0L);
        Bar->SetSize(ClassesImports::Point(145, 20));
        Bar->SetPositionModeW(false);
        Bar->AfterThumbImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackLeftAlt"}));
        Bar->BeforeThumbImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackRightAlt"}));
        Bar->ThumbButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Bar->ThumbButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Bar->ThumbButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Bar->SetRange(MinValue, MaxValue);
        Bar->SetPositionInternal(std::min<std::int32_t>(CurrentValue, MaxValue));
        Bar->SetHitTestDisabled(true);
        Bar->UpdateLayout();
        Bar->Invalidate();
    }

    void TfAchievements::p_destroy() {
        fAchievements::TfAchievements_Destroy(this);
    }

} // namespace fAchievements
