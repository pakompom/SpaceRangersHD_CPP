#include "layout/fLoadQuest.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Sound.hpp"
#include "types/System.hpp"
#include "types/TextFieldClass.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_CacheBuf.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/TextQuest.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/fLoadQuest.hpp"
#include "units/fPanelLoad.hpp"

namespace fLoadQuest {
    std::uint32_t QuestNameColor{};

    void TfLoadQuest_Create(TfLoadQuest* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->SelectedIndex = -1;
        Self->Category = 0;
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfLoadQuest_Destroy(TfLoadQuest* Self) {
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfLoadQuest::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fLoadQuest... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GI_MessageLoop::TObjectGI* Root = GetByName(u""sv);
        Root->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Root->FindByNameRecursive(u"BGBuf"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* Panel = Root->FindByNameRecursive(u"MainPanel"sv);
        if (GR_Main::ExtraScreenHeight < 0) {
            Panel->SetPosition(ClassesImports::Point(GR_Main::ExtraScreenWidth / 2 + Panel->LocalPosition.X, (GR_Main::GameScreenHeight - Panel->ClientSize.Y) / 2));
        } else {
            Panel->SetPosition(ClassesImports::Point(GR_Main::ExtraScreenWidth / 2 + Panel->LocalPosition.X, GR_Main::ExtraScreenHeight / 2 + Panel->LocalPosition.Y));
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfLoadQuest::QuestListKeyDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"sv))->UpCallback = pas::bind_method<&TfLoadQuest::ReturnToMenu>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButCancel"sv))->UpCallback = pas::bind_method<&TfLoadQuest::ReturnToMenu>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButStart"sv))->UpCallback = pas::bind_method<&TfLoadQuest::StartSelectedQuest>(this);
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
            ButGroup0->UpCallback = pas::bind_method<&TfLoadQuest::SelectCategory>(this);
            ButGroup0->DownCallback = pas::bind_method<&TfLoadQuest::SelectCategory>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
            ButGroup1->UpCallback = pas::bind_method<&TfLoadQuest::SelectCategory>(this);
            ButGroup1->DownCallback = pas::bind_method<&TfLoadQuest::SelectCategory>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
            ButGroup2->UpCallback = pas::bind_method<&TfLoadQuest::SelectCategory>(this);
            ButGroup2->DownCallback = pas::bind_method<&TfLoadQuest::SelectCategory>(this);
        }
        QuestNameColor = GR_Main::GetStyleColorGI(u"LoadQuest.QuestsNameColor"_w, 165, 183, 143);
    }

    void TfLoadQuest::OnOpen() {
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        LoadPanel->OnOpen();
        KeyHistory = pas::WideString();
        if (GlobalsV::PreviousScreenId != GlobalsV::screenPlanetQuest) {
            if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
                GR_Main::CaptureScreenBackground(true, 0);
            }
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"sv))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
            ButGroup0->SetDown(Category == 0);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
            ButGroup1->SetDown(Category == 1);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
            ButGroup2->SetDown(Category == 2);
        }
        RebuildQuestList();
        SelectQuest(0);
    }

    void TfLoadQuest::OnClose() {
        Entries = nullptr;
        if (GlobalsV::RequestedScreenId != GlobalsV::screenPlanetQuest) {
            GR_Main::AuxRenderBuffer->Clear();
        }
        LoadPanel->OnClose();
        GI_MessageLoop::TMessageLoopGI::OnClose();
    }

    void TfLoadQuest::ReturnToMenu(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
        RequestClose(1);
    }

    void TfLoadQuest::QuestListKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey) {
        std::int32_t I{};
        std::int32_t QuestId{};
        std::uint8_t CaptureKey = false;
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT)) {
                KeyHistory = pas::concat_wide({KeyHistory, static_cast<char16_t>(VirtualKey)});
                CaptureKey = true;
            }
        }
        if (CaptureKey) {
            if (EC_Str::FindTextOffsetW(KeyHistory, u"WIN"_wref.get(), 0) >= 0) {
                KeyHistory = pas::WideString();
                if (SelectedIndex >= 0 && Entries.length() - 1 >= SelectedIndex) {
                    if (Entries[SelectedIndex].QuestId >= 0) {
                        LoadCompletionData();
                        if (Entries[SelectedIndex].QuestId >= 0 && Entries[SelectedIndex].QuestId < aGalaxyStruct::FirstLicensedQuestId) {
                            QuestId = Entries[SelectedIndex].QuestId;
                            if (QuestId < 0 || (CompletionData.length() - 1 + 1) / 2 <= QuestId || CompletionData[QuestId * 2 + 1] == 0) {
                                RecordCompletion(QuestId, 0, 1);
                                SaveCompletionData();
                                RebuildQuestList();
                            }
                        }
                    }
                }
            }
        }
        if (VirtualKey == WindowsSdk::VK_ESCAPE) {
            ReturnToMenu(Sender);
        } else if (VirtualKey == 'Q') {
            ReturnToMenu(Sender);
        } else if (VirtualKey == WindowsSdk::VK_PRIOR) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
            if (PanelSlot->VerticalScrollBar->Active) {
                PanelSlot->VerticalScrollBar->SetPosition_2(PanelSlot->VerticalScrollBar->Position - PanelSlot->VerticalScrollBar->LargeChange);
            }
        } else if (VirtualKey == WindowsSdk::VK_NEXT) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot_2 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
            if (PanelSlot_2->VerticalScrollBar->Active) {
                PanelSlot_2->VerticalScrollBar->SetPosition_2(PanelSlot_2->VerticalScrollBar->Position + PanelSlot_2->VerticalScrollBar->LargeChange);
            }
        } else if (VirtualKey == WindowsSdk::VK_HOME) {
            if (Entries.length() - 1 + 1 > 0) {
                if (Entries[0].RequiredAccess <= AccessLevel) {
                    SelectQuest(0);
                }
            }
        } else if (VirtualKey == WindowsSdk::VK_END) {
            const std::int32_t cpp_first = Entries.length() - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    if (Entries[I].RequiredAccess <= AccessLevel) {
                        SelectQuest(I);
                        break;
                    }
                }
            }
        } else if (VirtualKey == WindowsSdk::VK_UP) {
            QuestRowMouseLeave(nullptr);
            if (SelectedIndex > 0) {
                SelectQuest(SelectedIndex - 1);
            } else if (Entries.length() - 1 + 1 > 0) {
                if (Entries[0].RequiredAccess <= AccessLevel) {
                    SelectQuest(0);
                }
            }
        } else if (VirtualKey == WindowsSdk::VK_DOWN) {
            QuestRowMouseLeave(nullptr);
            if (SelectedIndex >= 0) {
                if (Entries.length() - 1 > SelectedIndex) {
                    if (Entries[SelectedIndex + 1].RequiredAccess <= AccessLevel) {
                        SelectQuest(SelectedIndex + 1);
                    }
                }
            } else if (Entries.length() - 1 + 1 > 0) {
                if (Entries[0].RequiredAccess <= AccessLevel) {
                    SelectQuest(0);
                }
            }
        } else if (VirtualKey == WindowsSdk::VK_RETURN) {
            StartSelectedQuest(nullptr);
        } else if (VirtualKey == WindowsSdk::VK_TAB) {
            QuestRowMouseLeave(nullptr);
            if (Category == 0) {
                SelectCategory(GetByName(u"ButGroup1"sv));
            } else if (Category == 1) {
                SelectCategory(GetByName(u"ButGroup2"sv));
            } else if (Category == 2) {
                SelectCategory(GetByName(u"ButGroup0"sv));
            }
        }
    }

    void TfLoadQuest::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel{};
        WindowsSdk::TRect Bounds{};
        Bounds = GetByName(u"MessageWindow"sv)->HitTestBounds;
        if (Point.X >= Bounds.Left && Point.X < Bounds.Right && Point.Y >= Bounds.Top && Point.Y < Bounds.Bottom) {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"sv));
        } else {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
        }
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            if (Panel->VerticalScrollBar->Active) {
                Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->SmallChange);
            }
            GR_Main::PostMouseMoveMessage();
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            if (Panel->VerticalScrollBar->Active) {
                Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->SmallChange);
            }
            GR_Main::PostMouseMoveMessage();
        }
    }

    // Returns a zero-based index; inserts after entries with equal access.
    std::int32_t TfLoadQuest::InsertEntryByAccess(std::int32_t RequiredAccess) {
        std::int32_t I{};
        std::int32_t Index = 0;
        while (Entries.length() - 1 >= Index && Entries[Index].RequiredAccess <= RequiredAccess) {
            ++Index;
        }
        Entries.set_length(Entries.length() - 1 + 1 + 1);
        for (auto cpp_range = pas::for_downto<std::int32_t>(Entries.length() - 1, Index + 1); cpp_range.next(I); ) {
            Entries[I] = Entries[I - 1];
        }
        Entries[Index].RequiredAccess = RequiredAccess;
        return Index;
    }

    void TfLoadQuest::RebuildQuestList() {
        std::int32_t I{};
        std::int32_t Index{};
        std::int32_t Count{};
        GI_Panel::TPanelGI* Row{};
        EC_BlockPar::TBlockParEC* List{};
        EC_BlockPar::TBlockParEC* Entry{};
        pas::WideString Name{};
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
        Panel->FreeOwnedChildren();
        LoadCompletionData();
        std::int32_t QuestIdAndTop = 0;
        Entries = nullptr;
        if (Category == 0 || Category == 1) {
            List = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.List"_wref.get());
            Count = List->GetBlockCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Name = List->GetBlockNameByIndex(I);
                Entry = List->GetBlockByIndex(I);
                QuestIdAndTop = -1;
                if (EC_Str::IsIntegerTextW(pas::view(Name))) {
                    QuestIdAndTop = SysUtils::StrToInt(static_cast<pas::AnsiString>(Name));
                }
                if (Entry->CountParams(u"Group"_wref.get()) > 0) {
                    if (EC_Str::ExtractSignedDigitsToIntW(pas::view(Entry->GetParam(u"Group"sv))) == Category) {
                        Index = InsertEntryByAccess(EC_Str::ExtractDigitsToIntW(pas::view(Entry->GetParam(u"Access"sv))));
                        Entries[Index].Name = Name;
                        Entries[Index].Title = Entry->GetParam(u"Name"sv);
                        Entries[Index].Description = GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"sv)->GetBlock(u"PlanetQuest"sv)->GetParam(pas::view(Name));
                        Entries[Index].QuestId = QuestIdAndTop;
                        Entries[Index].Image = Entry->GetParamOrMarker(u"Image"sv);
                        Entries[Index].Genre = Entry->GetParamOrMarker(u"Genre"sv);
                        Entries[Index].Length = EC_Str::ExtractDigitsToIntW(pas::view(Entry->GetParamOrMarker(u"Length"sv)));
                        Entries[Index].Difficulty = EC_Str::ExtractDigitsToIntW(pas::view(Entry->GetParamOrMarker(u"Dif"sv)));
                    }
                }
            }
        } else {
            List = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.List"_wref.get());
            Count = List->GetBlockCount();
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Name = List->GetBlockNameByIndex(I);
                Entry = List->GetBlockByIndex(I);
                QuestIdAndTop = -1;
                if (EC_Str::IsIntegerTextW(pas::view(Name))) {
                    QuestIdAndTop = SysUtils::StrToInt(static_cast<pas::AnsiString>(Name));
                }
                if (Entry->CountParams(u"Group"_wref.get()) > 0) {
                    if (EC_Str::ExtractSignedDigitsToIntW(pas::view(Entry->GetParam(u"Group"sv))) == Category) {
                        Index = InsertEntryByAccess(0);
                        Entries[Index].Name = Name;
                        Entries[Index].Title = Entry->GetParam(u"Name"sv);
                        Entries[Index].Description = GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"sv)->GetBlock(u"PlanetQuest"sv)->GetParam(pas::view(Name));
                        Entries[Index].QuestId = QuestIdAndTop;
                        Entries[Index].Image = Entry->GetParamOrMarker(u"Image"sv);
                        Entries[Index].Genre = Entry->GetParamOrMarker(u"Genre"sv);
                        Entries[Index].Length = EC_Str::ExtractDigitsToIntW(pas::view(Entry->GetParamOrMarker(u"Length"sv)));
                        Entries[Index].Difficulty = EC_Str::ExtractDigitsToIntW(pas::view(Entry->GetParamOrMarker(u"Dif"sv)));
                    }
                }
            }
        }
        AccessLevel = CalculateAccessLevel();
        std::uint8_t Alternate = true;
        Index = -100;
        // Native code reuses the final parsed quest ID as the first row's top.
        {
            const std::int32_t cpp_last = Entries.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (I != 0) {
                        QuestIdAndTop += GR_Main::GiScalePixels(5);
                    }
                    if (Entries[I].RequiredAccess != Index) {
                        Alternate = static_cast<std::uint8_t>(Alternate ^ 1);
                        Index = Entries[I].RequiredAccess;
                    }
                    Entries[I].AlternateGroup = Alternate;
                    Row = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
                    Row->UserValue = I;
                    Row->SetPosition(ClassesImports::Point(0, QuestIdAndTop));
                    InitializeQuestRow(Row);
                    QuestIdAndTop += Row->ClientSize.Y;
                    Row->SetPositionModeW(true);
                    Panel->VerticalScrollBar->SetSmallChange(GR_Main::GiScalePixels(5) + Row->ClientSize.Y);
                    UpdateQuestRow(I);
                }
            }
        }
        Panel->UpdateScrollRanges();
        Panel->VerticalScrollBar->SetActive(Panel->ClientSize.Y < QuestIdAndTop);
        Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
        I = Panel->VerticalScrollBar->Position;
        Panel->VerticalScrollBar->SetPosition_2(I - 1);
        Panel->VerticalScrollBar->SetPosition_2(I);
        if (SelectedIndex < 0 || Entries.length() - 1 + 1 <= SelectedIndex) {
            SelectedIndex = -1;
        }
        SelectQuest(SelectedIndex);
        Panel->Invalidate();
    }

    void TfLoadQuest::InitializeQuestRow(GI_MessageLoop::TObjectGI* Row) {
        std::int32_t Index{};
        GI_Image::TImageGI* LengthImage{};
        GI_Image::TImageGI* CompletionImage{};
        Index = Row->UserValue;
        Row->MouseEnterCallback = pas::bind_method<&TfLoadQuest::QuestRowMouseEnter>(this);
        Row->MouseLeaveCallback = pas::bind_method<&TfLoadQuest::QuestRowMouseLeave>(this);
        Row->LeftButtonDownCallback = pas::bind_method<&TfLoadQuest::QuestRowMouseDown>(this);
        Row->LeftButtonDoubleClickCallback = pas::bind_method<&TfLoadQuest::QuestRowDoubleClick>(this);
        Entries[Index].BackgroundImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
        GI_Image::TImageGI* Background = Entries[Index].BackgroundImage;
        Background->SetPosition(ClassesImports::Point(0, 0));
        Background->SetDepth(1.0E+1);
        if (Entries[Index].RequiredAccess <= AccessLevel) {
            if (!Entries[Index].AlternateGroup) {
                Background->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"SlotNormal"}));
            } else {
                Background->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"SlotNormal2"}));
            }
        } else {
            Background->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"SlotDisabled"}));
        }
        Background->SetSize(Background->GetContentSize());
        Row->SetSize(Background->ClientSize);
        Background->SetActive(true);
        std::int32_t TitleRight = GR_Main::GiScalePixels(262);
        if (Entries[Index].QuestId >= 0 && Entries[Index].QuestId < aGalaxyStruct::FirstLicensedQuestId) {
            LengthImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
            LengthImage->SetDepth(9.0);
            if (Entries[Index].RequiredAccess <= AccessLevel) {
                LengthImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"D", pas::wide_int_to_str(Entries[Index].Length + 1)}));
            } else {
                LengthImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"D", pas::wide_int_to_str(Entries[Index].Length + 1), u"D"}));
            }
            LengthImage->SetSize(LengthImage->GetContentSize());
            LengthImage->SetPosition(ClassesImports::Point(TitleRight - LengthImage->ClientSize.X, Row->ClientSize.Y / 2 - LengthImage->ClientSize.Y / 2));
            LengthImage->SetActive(true);
            TitleRight = TitleRight - LengthImage->ClientSize.X - 1;
        }
        std::int32_t Status = Entries[Index].QuestId;
        std::int32_t CompletionIndex = Status;
        std::int32_t GenreRight = Row->ClientSize.X;
        if (CompletionIndex >= 0) {
            if ((CompletionData.length() - 1 + 1) / 2 > CompletionIndex) {
                Status = CompletionData[CompletionIndex * 2 + 1];
                if (Status != 0) {
                    CompletionImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
                    CompletionImage->SetPosition(ClassesImports::Point(0, 0));
                    CompletionImage->SetDepth(9.0);
                    CompletionImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_33"}));
                    CompletionImage->SetSize(CompletionImage->GetContentSize());
                    CompletionImage->SetPosition(ClassesImports::Point(Row->ClientSize.X - CompletionImage->ClientSize.X - GR_Main::GiScalePixels(10), Row->ClientSize.Y / 2 - CompletionImage->ClientSize.Y / 2));
                    GenreRight = CompletionImage->LocalPosition.X;
                    CompletionImage->SetActive(true);
                }
            }
        }
        GI_Label::TLabelGI* TitleLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
        if (GR_Main::GiResourceVariant() == 1) {
            TitleLabel->SetPosition(ClassesImports::Point(11, 0));
            TitleLabel->SetSize(ClassesImports::Point(TitleRight - TitleLabel->LocalPosition.X, Row->ClientSize.Y));
        } else {
            TitleLabel->SetPosition(ClassesImports::Point(14, 0));
            TitleLabel->SetSize(ClassesImports::Point(TitleRight - TitleLabel->LocalPosition.X, Row->ClientSize.Y));
        }
        TitleLabel->SetDepth(7.0);
        TitleLabel->SetFontName(GlobalsV::NormalFontName);
        TitleLabel->SetTextAlignX(GI_Main::taxLeft);
        TitleLabel->SetTextAlignY(GI_Main::tayCenterEx);
        TitleLabel->SetText(Entries[Index].Title);
        if (Entries[Index].RequiredAccess <= AccessLevel) {
            TitleLabel->SetTextColor(QuestNameColor);
        } else {
            TitleLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(112, 112, 112));
        }
        GI_Label::TLabelGI* GenreLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
        if (GR_Main::GiResourceVariant() == 1) {
            GenreLabel->SetPosition(ClassesImports::Point(209, 0));
            GenreLabel->SetSize(ClassesImports::Point(GenreRight - 209, Row->ClientSize.Y - 2));
        } else {
            GenreLabel->SetPosition(ClassesImports::Point(267, 0));
            GenreLabel->SetSize(ClassesImports::Point(GenreRight - 267, Row->ClientSize.Y - 2));
        }
        GenreLabel->SetDepth(7.0);
        GenreLabel->SetFontName(GlobalsV::SmallFontName);
        GenreLabel->SetTextAlignX(GI_Main::taxLeft);
        GenreLabel->SetTextAlignY(GI_Main::tayCenter);
        GenreLabel->SetWordWrapEnabled(true);
        GenreLabel->SetText(EC_Str::TrimWideString(Entries[Index].Genre));
        if (Entries[Index].RequiredAccess <= AccessLevel) {
            GenreLabel->SetTextColor(QuestNameColor);
        } else {
            GenreLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(112, 112, 112));
        }
    }

    // Index is zero-based; locked entries clear the selection.
    void TfLoadQuest::SelectQuest(std::int32_t Index) {
        std::int32_t Previous = SelectedIndex;
        SelectedIndex = Index;
        if (Entries.length() - 1 + 1 <= SelectedIndex) {
            SelectedIndex = -1;
        }
        if (SelectedIndex >= 0) {
            if (Entries[SelectedIndex].RequiredAccess > AccessLevel) {
                SelectedIndex = -1;
            }
        }
        UpdateQuestRow(Previous);
        UpdateQuestRow(SelectedIndex);
        if (Index < 0 || Entries.length() - 1 < Index) {
            ShowSelectedQuestDetails();
            return;
        }
        if (SelectedIndex >= 0) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
            {
                GI_MessageLoop::TObjectGI* cpp_with_2 = Entries[SelectedIndex].BackgroundImage->Parent;
                PanelSlot->ScrollRectIntoView(cpp_with_2->GetLocalBounds());
            }
        }
        ShowSelectedQuestDetails();
    }

    void TfLoadQuest::UpdateQuestRow(std::int32_t Index) {
        if (Index < 0 || Entries.length() - 1 < Index) {
            return;
        }
        GI_Image::TImageGI* Image = Entries[Index].BackgroundImage;
        if (Entries[Index].RequiredAccess > AccessLevel) {
            return;
        }
        if (SelectedIndex == Index) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"SlotActive"}));
        } else if (HoveredIndex == Index) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"SlotOnMouse"}));
        } else if (!Entries[Index].AlternateGroup) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"SlotNormal"}));
        } else {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadQuest.", GR_Main::GiResourceSuffix(), u"SlotNormal2"}));
        }
    }

    void TfLoadQuest::QuestRowMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Previous{};
        if (HoveredIndex != Sender->UserValue) {
            Previous = HoveredIndex;
            HoveredIndex = Sender->UserValue;
            UpdateQuestRow(Previous);
            UpdateQuestRow(HoveredIndex);
        }
    }

    void TfLoadQuest::QuestRowMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Previous{};
        if (HoveredIndex != -1) {
            Previous = HoveredIndex;
            HoveredIndex = -1;
            UpdateQuestRow(Previous);
            UpdateQuestRow(HoveredIndex);
        }
    }

    void TfLoadQuest::QuestRowMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
        SelectQuest(Sender->UserValue);
        GI_Main::BreakUiMessage();
    }

    void TfLoadQuest::QuestRowDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        StartSelectedQuest(nullptr);
    }

    void TfLoadQuest::SelectCategory(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t NewCategory = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv))->SetDown(NewCategory == 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv))->SetDown(NewCategory == 1);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv))->SetDown(NewCategory == 2);
        if (Category != NewCategory) {
            {
                GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
                PanelSlot->ScrollRectIntoView(ClassesImports::Rect(0, 0, 1, 1));
            }
            Category = NewCategory;
            SelectedIndex = -1;
            RebuildQuestList();
            SelectQuest(0);
        }
    }

    void TfLoadQuest::StartSelectedQuest(GI_MessageLoop::TObjectGI* Sender) {
        if (SelectedIndex < 0) {
            return;
        }
        if (Entries.length() - 1 < SelectedIndex) {
            return;
        }
        GlobalsV::PendingQuestName = Entries[SelectedIndex].Name;
        Globals::StandaloneQuestMode = true;
        GlobalsV::QuestReturnScreenId = GlobalsV::FormToId(this);
        GlobalsV::RequestedScreenId = GlobalsV::screenPlanetQuest;
        RequestClose(1);
        GI_Main::BreakUiMessage();
    }

    // Loads the quest with HeaderOnly enabled.
    void TfLoadQuest::ShowSelectedQuestDetails() {
        TextQuest::TTextQuest* Quest{};
        pas::WideString Text{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t RowSkip{};
        void* Pixel{};
        EC_CacheBuf::TCBufControlEC* Control{};
        EC_CacheBuf::TCBufEC* Data{};
        {
            GI_GraphButton::TGraphButtonGI* ButStart = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButStart"sv));
            ButStart->SetDisabled(SelectedIndex < 0 || Entries.length() - 1 < SelectedIndex);
        }
        {
            GI_GraphBuf::TGraphBufGI* ImageMap = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"ImageMap"sv));
            ImageMap->SetActive(false);
            if (SelectedIndex >= 0 && Entries.length() - 1 >= SelectedIndex) {
                if (Entries[SelectedIndex].Image != u"") {
                    ImageMap->SetActive(true);
                    ImageMap->LoadBitmapPathAsRgba(pas::concat_wide({Entries[SelectedIndex].Image, EC_CacheBitmap::RgbaImagePathSuffix}));
                    if (ImageMap->ClientSize.X != ImageMap->GraphBuf->Width || ImageMap->ClientSize.Y != ImageMap->GraphBuf->Height) {
                        ImageMap->GraphBuf->RescaleRgba(ImageMap->ClientSize.X, ImageMap->ClientSize.Y, 5);
                    }
                    RowSkip = ImageMap->GraphBuf->PitchBytes - ImageMap->GraphBuf->Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                    Pixel = ImageMap->GraphBuf->GetPixels();
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, ImageMap->GraphBuf->Height - 1); cpp_range.next(Y); ) {
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ImageMap->GraphBuf->Width - 1); cpp_range_2.next(X); ) {
                            if (Y < 23 && ImageMap->GraphBuf->Width - 23 + Y < X) {
                                *(&static_cast<GR_GraphBuf::PColorRGBA>(Pixel)->A) = 0;
                            } else {
                                *(&static_cast<GR_GraphBuf::PColorRGBA>(Pixel)->A) = 255;
                            }
                            Pixel = static_cast<std::uint8_t*>(Pixel) + static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                        }
                        Pixel = static_cast<std::uint8_t*>(Pixel) + RowSkip;
                    }
                    ImageMap->Invalidate();
                }
            }
        }
        {
            GI_Label::TLabelGI* with_MessageText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"MessageText"sv));
            with_MessageText->SetActive(false);
            if (SelectedIndex >= 0 && Entries.length() - 1 >= SelectedIndex) {
                Quest = pas::construct_call<TextQuest::TTextQuest>(TextQuest::TTextQuest_Create);
                if (Entries[SelectedIndex].QuestId >= 0) {
                    Control = nullptr;
                    {
                        std::exception_ptr cpp_error{};
                        try {
                            Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                            EC_Cache::TCacheEC::ResetControl(Control);
                            Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.", SysUtils::IntToStr(Entries[SelectedIndex].QuestId)})));
                            Data = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                            Quest->LoadFromReader(Data->Buffer, true);
                        } catch (...) {
                            cpp_error = std::current_exception();
                        }
                        if (Control != nullptr) {
                            Control->Release();
                            pas::free(Control);
                        }
                        if (cpp_error) {
                            std::rethrow_exception(cpp_error);
                        }
                    }
                } else {
                    Control = nullptr;
                    {
                        std::exception_ptr cpp_error_2{};
                        try {
                            Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                            EC_Cache::TCacheEC::ResetControl(Control);
                            Control->SetCacheKey(pas::concat_wide({u"PlanetQuest.", Entries[SelectedIndex].Name}));
                            Data = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                            Quest->LoadFromReader(Data->Buffer, true);
                        } catch (...) {
                            cpp_error_2 = std::current_exception();
                        }
                        if (Control != nullptr) {
                            Control->Release();
                            pas::free(Control);
                        }
                        if (cpp_error_2) {
                            std::rethrow_exception(cpp_error_2);
                        }
                    }
                }
                Text = Quest->QuestDescriptionText->Text;
                {
                    auto brightBlueColorTag = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString localizedText = aConst::LocalizedText(u"FormLoadQuest.PRanger"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<Ranger>"_w, std::move(localizedText), brightBlueColorTag.get());
                }
                {
                    auto brightBlueColorTag_2 = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString localizedText_2 = aConst::LocalizedText(u"FormLoadQuest.PToPlanet"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<ToPlanet>"_w, std::move(localizedText_2), brightBlueColorTag_2.get());
                }
                {
                    auto brightBlueColorTag_3 = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString localizedText_3 = aConst::LocalizedText(u"FormLoadQuest.PToStar"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<ToStar>"_w, std::move(localizedText_3), brightBlueColorTag_3.get());
                }
                aMyFunction::ReplaceTextToken(Text, u"<Parsec>"_w, pas::wide_int_to_str(10), aMyFunction::BrightBlueColorTag);
                {
                    auto brightBlueColorTag_4 = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString formatGameTurnDate = aGalaxy::FormatGameTurnDate(1000);
                    aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, std::move(formatGameTurnDate), brightBlueColorTag_4.get());
                }
                aMyFunction::ReplaceTextToken(Text, u"<Day>"_w, pas::wide_int_to_str(30), aMyFunction::BrightBlueColorTag);
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(10000), aMyFunction::BrightBlueColorTag);
                {
                    auto brightBlueColorTag_5 = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString localizedText_4 = aConst::LocalizedText(u"FormLoadQuest.PFromPlanet"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, std::move(localizedText_4), brightBlueColorTag_5.get());
                }
                {
                    auto brightBlueColorTag_6 = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString localizedText_5 = aConst::LocalizedText(u"FormLoadQuest.PFromStar"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<FromStar>"_w, std::move(localizedText_5), brightBlueColorTag_6.get());
                }
                Text = EC_Str::ReplaceAllWideString(Text, u"<clr>"_wref.get(), pas::view(aMyFunction::BrightBlueColorTag));
                aConst::ExpandLocalizedTextMarkup(Text);
                pas::free(Quest);
                with_MessageText->SetActive(true);
                with_MessageText->SetText(Text);
            }
        }
        {
            GI_PanelScrollBar::TPanelScrollBarGI* MessageWindow = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"sv));
            MessageWindow->SetScrollOffset(ClassesImports::Point(0, 0));
            MessageWindow->UpdateScrollRanges();
            MessageWindow->VerticalScrollBar->SetActive(MessageWindow->FindByNameRecursive(u"MessageText"sv)->Active && pas::checked_cast<GI_Label::TLabelGI*>(MessageWindow->FindByNameRecursive(u"MessageText"sv))->ClientSize.Y > MessageWindow->ClientSize.Y);
            {
                std::int32_t lineHeight = pas::checked_cast<GI_Label::TLabelGI*>(MessageWindow->FindByNameRecursive(u"MessageText"sv))->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = MessageWindow->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(lineHeight);
            }
            MessageWindow->VerticalScrollBar->SetLargeChange(MessageWindow->ClientSize.Y);
            MessageWindow->VerticalScrollBar->SetPageSize(MessageWindow->ClientSize.Y);
        }
    }

    // QuestComplate.dat: CRC32, Int32 count, then Int32 entries.
    void TfLoadQuest::LoadCompletionData() {
        pas::WideString cpp_text{};
        EC_Buf::TBufEC* Buffer{};
        std::int32_t I{};
        std::int32_t Count{};
        CompletionData = nullptr;
        if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"QuestComplate.dat"})))) {
            try {
                Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                Buffer->LoadFromWideFilePath((cpp_text = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"QuestComplate.dat"}), cpp_text.pchar()));
                {
                    std::uint32_t cpp_left = Buffer->ComputeCrc32Range(4, Buffer->DataSize);
                    if (cpp_left != EC_Buf::TBufEC_GetUInt32(Buffer)) {
                        SysUtilsImports::Abort();
                    }
                }
                Count = EC_Buf::TBufEC_GetInt32(Buffer);
                CompletionData.set_length(Count);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                    CompletionData[I] = EC_Buf::TBufEC_GetInt32(Buffer);
                }
                pas::free(Buffer);
            } catch (...) {
                CompletionData = nullptr;
            }
        }
    }

    void TfLoadQuest::SaveCompletionData() {
        std::int32_t I{};
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buffer->AddDWord(0u);
        Buffer->AddInteger(CompletionData.length() - 1 + 1);
        {
            const std::int32_t cpp_last = CompletionData.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Buffer->AddIntegerValue(CompletionData[I]);
                }
            }
        }
        pas::store_unaligned<std::uint32_t>(static_cast<System::PCardinal>(Buffer->Data), Buffer->ComputeCrc32Range(4, Buffer->DataSize));
        EC_File::TFileEC* FileHandle = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        FileHandle->SetFileName(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"QuestComplate.dat"}));
        FileHandle->CreateNew();
        FileHandle->WriteBuffer(Buffer->Data, Buffer->DataSize);
        pas::free(FileHandle);
        pas::free(Buffer);
    }

    // QuestId must be 0..9999. Higher status wins; equal status minimizes an existing nonzero Value.
    void TfLoadQuest::RecordCompletion(std::int32_t QuestId, std::int32_t Value, std::int32_t Status) {
        std::int32_t I{};
        if (QuestId < 0 || QuestId >= aGalaxyStruct::FirstLicensedQuestId) {
            return;
        }
        std::int32_t Count = (CompletionData.length() - 1 + 1) / 2;
        if (QuestId >= Count) {
            CompletionData.set_length((QuestId + 1) * 2);
            {
                const std::int32_t cpp_first = Count * 2;
                const std::int32_t cpp_last = CompletionData.length() - 1;
                if (cpp_first <= cpp_last) {
                    for (I = cpp_first; I <= cpp_last; ++I) {
                        CompletionData[I] = 0;
                    }
                }
            }
        }
        if (CompletionData[QuestId * 2 + 1] > Status) {
            return;
        }
        if (CompletionData[QuestId * 2] == 0 || CompletionData[QuestId * 2 + 1] < Status) {
            CompletionData[QuestId * 2] = Value;
        } else {
            CompletionData[QuestId * 2] = std::min<std::int32_t>(CompletionData[QuestId * 2], Value);
        }
        CompletionData[QuestId * 2 + 1] = Status;
    }

    // Advances past a group when all but one quest is completed.
    std::int32_t TfLoadQuest::CalculateAccessLevel() {
        std::int32_t QuestId{};
        std::int32_t Access{};
        std::int32_t GroupCount{};
        std::int32_t CompletedCount{};
        std::int32_t CompletionIndex{};
        std::int32_t Result = 0;
        std::int32_t I = 0;
        while (Entries.length() - 1 >= I) {
            Access = Entries[I].RequiredAccess;
            GroupCount = 0;
            CompletedCount = 0;
            while (Entries.length() - 1 >= I + GroupCount) {
                if (Entries[I + GroupCount].RequiredAccess != Access) {
                    break;
                }
                QuestId = Entries[I + GroupCount].QuestId;
                if (QuestId >= 0 && QuestId < aGalaxyStruct::FirstLicensedQuestId) {
                    CompletionIndex = QuestId;
                    if (CompletionIndex >= 0) {
                        if ((CompletionData.length() - 1 + 1) / 2 > CompletionIndex) {
                            // The native inlined completion check repeats this upper bound.
                            if ((CompletionData.length() - 1 + 1) / 2 > CompletionIndex) {
                                if (CompletionData[CompletionIndex * 2 + 1] != 0) {
                                    ++CompletedCount;
                                }
                            }
                        }
                    }
                }
                ++GroupCount;
            }
            Result = Access;
            if (Access > 0 && GroupCount - 1 > CompletedCount) {
                break;
            }
            I += GroupCount;
        }
        if (Result == 0) {
            return 1;
        }
        return Result;
    }

    // X is completed, Y is total; includes only numeric quests with positive Access.
    WindowsSdk::TPoint TfLoadQuest::GetCompletionCounts() {
        WindowsSdk::TPoint Result{};
        std::int32_t I{};
        std::int32_t CompletionIndex{};
        std::int32_t QuestId{};
        EC_BlockPar::TBlockParEC* Entry{};
        pas::WideString Name{};
        Result.X = 0;
        Result.Y = 0;
        LoadCompletionData();
        EC_BlockPar::TBlockParEC* List = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.List"_wref.get());
        std::int32_t Count = List->GetBlockCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Entry = List->GetBlockByIndex(I);
            Name = List->GetBlockNameByIndex(I);
            QuestId = -1;
            if (EC_Str::IsIntegerTextW(pas::view(Name))) {
                QuestId = SysUtils::StrToInt(static_cast<pas::AnsiString>(Name));
            }
            if (QuestId >= 0 && EC_Str::ExtractDigitsToIntW(pas::view(Entry->GetParam(u"Access"sv))) > 0) {
                ++Result.Y;
                CompletionIndex = QuestId;
                if (CompletionIndex >= 0) {
                    if ((CompletionData.length() - 1 + 1) / 2 > CompletionIndex) {
                        if (CompletionData[CompletionIndex * 2 + 1] != 0) {
                            ++Result.X;
                        }
                    }
                }
            }
        }
        return Result;
    }

    // The displayed total includes groups below 3.
    pas::WideString TfLoadQuest::GetCompletionSummary() {
        WindowsSdk::TPoint Counts{};
        EC_BlockPar::TBlockParEC* Entry{};
        std::int32_t I{};
        EC_BlockPar::TBlockParEC* List = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.List"_wref.get());
        std::int32_t Count = List->GetBlockCount();
        std::int32_t Total = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Entry = List->GetBlockByIndex(I);
            if (Entry->CountParams(u"Group"_wref.get()) > 0) {
                if (pas::in_set<0, 2>(EC_Str::ExtractSignedDigitsToIntW(pas::view(Entry->GetParam(u"Group"sv))))) {
                    ++Total;
                }
            }
        }
        Counts = GetCompletionCounts();
        return static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Counts.X), "/", SysUtils::IntToStr(Total)}));
    }

    // Empty implementation.
    void TfLoadQuest::SelectMusic() {
    }

    void TfLoadQuest::p_destroy() {
        fLoadQuest::TfLoadQuest_Destroy(this);
    }

} // namespace fLoadQuest
