#include "layout/fLoadAB.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/ab_MainForm.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
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
#include "units/fLoadAB.hpp"

namespace fLoadAB {
    std::uint32_t ArcadeNameColor{};

    void TfLoadAB_Create(TfLoadAB* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->SelectedIndex = -1;
        Self->Category = 0;
    }

    void TfLoadAB_Destroy(TfLoadAB* Self) {
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfLoadAB::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fLoadAB... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GI_MessageLoop::TObjectGI* Root = GetByName(u""_wref.get());
        Root->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Root->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* Panel = Root->FindByNameRecursive(u"MainPanel"_wref.get());
        Panel->SetPosition(ClassesImports::Point(Panel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Panel->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfLoadAB::ArenaListKeyDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"_wref.get()))->UpCallback = pas::bind_method<&TfLoadAB::ReturnToMenu>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButCancel"_wref.get()))->UpCallback = pas::bind_method<&TfLoadAB::ReturnToMenu>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButStart"_wref.get()))->UpCallback = pas::bind_method<&TfLoadAB::StartSelectedArena>(this);
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"_wref.get()));
            ButGroup0->UpCallback = pas::bind_method<&TfLoadAB::SelectCategory>(this);
            ButGroup0->DownCallback = pas::bind_method<&TfLoadAB::SelectCategory>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"_wref.get()));
            ButGroup1->UpCallback = pas::bind_method<&TfLoadAB::SelectCategory>(this);
            ButGroup1->DownCallback = pas::bind_method<&TfLoadAB::SelectCategory>(this);
        }
        ArcadeNameColor = GR_Main::GetStyleColorGI(u"LoadAB.MapsNameColor"_w, 192, 112, 112);
    }

    void TfLoadAB::OnOpen() {
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        KeyHistory = pas::WideString();
        if (GlobalsV::PreviousScreenId != GlobalsV::screenArcadeBattle && GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(true, 0);
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"_wref.get()));
            ButGroup0->SetDown(Category == 0);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"_wref.get()));
            ButGroup1->SetDown(Category == 1);
        }
        RebuildArenaList();
        SelectArena(0);
    }

    void TfLoadAB::OnClose() {
        Entries = nullptr;
        if (GlobalsV::RequestedScreenId != GlobalsV::screenArcadeBattle) {
            GR_Main::AuxRenderBuffer->Clear();
        }
        GI_MessageLoop::TMessageLoopGI::OnClose();
    }

    void TfLoadAB::ReturnToMenu(GI_MessageLoop::TObjectGI* Sender) {
        GR_Main::AuxRenderBuffer->Clear();
        GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
        RequestClose(1);
    }

    void TfLoadAB::ArenaListKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey) {
        if (VirtualKey == WindowsSdk::VK_ESCAPE) {
            ReturnToMenu(Sender);
        } else if (VirtualKey == 'F') {
            ReturnToMenu(Sender);
        } else if (VirtualKey == WindowsSdk::VK_PRIOR) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
            if (PanelSlot->VerticalScrollBar->Active) {
                PanelSlot->VerticalScrollBar->SetPosition_2(PanelSlot->VerticalScrollBar->Position - PanelSlot->VerticalScrollBar->LargeChange);
            }
        } else if (VirtualKey == WindowsSdk::VK_NEXT) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot_2 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
            if (PanelSlot_2->VerticalScrollBar->Active) {
                PanelSlot_2->VerticalScrollBar->SetPosition_2(PanelSlot_2->VerticalScrollBar->Position + PanelSlot_2->VerticalScrollBar->LargeChange);
            }
        } else if (VirtualKey == WindowsSdk::VK_HOME) {
            SelectArena(0);
        } else if (VirtualKey == WindowsSdk::VK_END) {
            SelectArena(Entries.length() - 1);
        } else if (VirtualKey == WindowsSdk::VK_UP) {
            ArenaRowMouseLeave(nullptr);
            if (SelectedIndex > 0) {
                SelectArena(SelectedIndex - 1);
            }
        } else if (VirtualKey == WindowsSdk::VK_DOWN) {
            ArenaRowMouseLeave(nullptr);
            if (Entries.length() - 1 > SelectedIndex) {
                SelectArena(SelectedIndex + 1);
            }
        } else if (VirtualKey == WindowsSdk::VK_RETURN) {
            StartSelectedArena(nullptr);
        } else if (VirtualKey == WindowsSdk::VK_TAB) {
            ArenaRowMouseLeave(nullptr);
            if (Category == 0) {
                SelectCategory(GetByName(u"ButGroup1"_wref.get()));
            } else {
                SelectCategory(GetByName(u"ButGroup0"_wref.get()));
            }
        }
    }

    void TfLoadAB::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel{};
        WindowsSdk::TRect Bounds{};
        Bounds = GetByName(u"MessageWindow"_wref.get())->HitTestBounds;
        if (Point.X >= Bounds.Left && Point.X < Bounds.Right && Point.Y >= Bounds.Top && Point.Y < Bounds.Bottom) {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"_wref.get()));
        } else {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
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

    std::int32_t TfLoadAB::AppendEntry(std::int32_t IgnoredAccess) {
        std::int32_t I{};
        std::int32_t Index = Entries.length() - 1 + 1;
        Entries.set_length(Entries.length() - 1 + 2);
        // Retained native insertion loop; appending makes its range empty.
        for (auto cpp_range = pas::for_downto<std::int32_t>(Entries.length() - 1, Index + 1); cpp_range.next(I); ) {
            Entries[I] = Entries[I - 1];
        }
        return Index;
    }

    void TfLoadAB::RebuildArenaList() {
        EC_BlockPar::TBlockParEC* Entry{};
        std::int32_t I{};
        std::int32_t Index{};
        std::int32_t EntryCategory{};
        GI_Panel::TPanelGI* Row{};
        pas::WideString Name{};
        auto ReadArcadeDescription = [&](const pas::WideString& Path) -> pas::WideString {
            pas::WideString Result{};
            std::int32_t J{};
            std::int32_t LineCount = Entry->CountParamsByPath(Path);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, LineCount - 1); cpp_range.next(J); ) {
                if (Result != u"") {
                    Result = pas::concat_wide({Result, u"\r\n"});
                }
                Result = pas::concat_wide({Result, Entry->GetParamByPath(pas::concat_wide({Path, u":", pas::wide_int_to_str(J)}))});
            }
            return Result;
        };
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
        Panel->FreeOwnedChildren();
        TfLoadAB::PrepareCatalog();
        Entries = nullptr;
        EC_BlockPar::TBlockParEC* List = GR_Main::LanguageDataConfig->GetBlockByPath(u"ABMap"_wref.get());
        std::int32_t Count = List->GetBlockCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Name = List->GetBlockNameByIndex(I);
            Entry = List->GetBlockByIndex(I);
            if (Entry->CountParamsByPath(u"Group"_wref.get()) > 0) {
                EntryCategory = EC_Str::ExtractDigitsToIntW(Entry->GetParam(u"Group"_wref.get()));
            } else {
                EntryCategory = 0;
            }
            if (EntryCategory == Category) {
                Index = AppendEntry(EC_Str::ExtractDigitsToIntW(Entry->GetParam(u"Access"_wref.get())));
                Entries[Index].Name = Entry->GetParam(u"Name"_wref.get());
                Entries[Index].ImageName = Entry->GetParamOrMarker(u"Image"_wref.get());
                Entries[Index].MapName = Entry->GetParam(u"Map"_wref.get());
                Entries[Index].Description = ReadArcadeDescription(u"Desc"_wref.get());
                Entries[Index].ConfigIndex = I;
                Entries[Index].Difficulty = EC_Str::ExtractDigitsToIntW(Entry->GetParamOrMarker(u"Dif"_wref.get()));
            }
        }
        std::int32_t Top = 0;
        {
            const std::int32_t cpp_last = Entries.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (I != 0) {
                        Top += 5;
                    }
                    Row = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
                    Row->UserValue = I;
                    Row->SetPosition(ClassesImports::Point(0, Top));
                    InitializeArenaRow(Row);
                    Top += Row->ClientSize.Y;
                    Row->SetPositionModeW(true);
                    Panel->VerticalScrollBar->SetSmallChange(Row->ClientSize.Y + 5);
                    UpdateArenaRow(I);
                }
            }
        }
        Panel->UpdateScrollRanges();
        Panel->VerticalScrollBar->SetActive(Top > Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
        I = Panel->VerticalScrollBar->Position;
        Panel->VerticalScrollBar->SetPosition_2(I - 1);
        Panel->VerticalScrollBar->SetPosition_2(I);
        if (SelectedIndex < 0 || Entries.length() - 1 + 1 <= SelectedIndex) {
            SelectedIndex = -1;
        }
        SelectArena(SelectedIndex);
        Panel->Invalidate();
    }

    void TfLoadAB::InitializeArenaRow(GI_MessageLoop::TObjectGI* Row) {
        std::int32_t Index{};
        GI_Image::TImageGI* DifficultyImage{};
        Index = Row->UserValue;
        Row->MouseEnterCallback = pas::bind_method<&TfLoadAB::ArenaRowMouseEnter>(this);
        Row->MouseLeaveCallback = pas::bind_method<&TfLoadAB::ArenaRowMouseLeave>(this);
        Row->LeftButtonDownCallback = pas::bind_method<&TfLoadAB::ArenaRowMouseDown>(this);
        Row->LeftButtonDoubleClickCallback = pas::bind_method<&TfLoadAB::ArenaRowDoubleClick>(this);
        Entries[Index].BackgroundImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
        GI_Image::TImageGI* Background = Entries[Index].BackgroundImage;
        Background->SetPosition(ClassesImports::Point(0, 0));
        Background->SetDepth(1.0E+1);
        Background->SetImagePath(u"GI,Bm.FormLoadAB.SlotNormal"_w);
        Background->SetSize(Background->GetContentSize());
        Row->SetSize(Background->ClientSize);
        Background->SetActive(true);
        GI_Label::TLabelGI* TitleLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
        TitleLabel->SetPosition(ClassesImports::Point(14, 0));
        TitleLabel->SetSize(ClassesImports::Point(Row->ClientSize.X - 38, Row->ClientSize.Y));
        TitleLabel->SetDepth(7.0);
        TitleLabel->SetFontName(GlobalsV::NormalFontName);
        TitleLabel->SetTextAlignX(GI_Main::taxLeft);
        TitleLabel->SetTextAlignY(GI_Main::tayCenterEx);
        TitleLabel->SetText(Entries[Index].Name);
        TitleLabel->SetTextColor(ArcadeNameColor);
        if (Entries[Index].ConfigIndex >= 0 && Entries[Index].ConfigIndex < 10000) {
            DifficultyImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
            DifficultyImage->SetDepth(9.0);
            DifficultyImage->SetImagePath(static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormLoadAB.Dif", SysUtils::IntToStr(Entries[Index].Difficulty + 1)})));
            DifficultyImage->SetSize(DifficultyImage->GetContentSize());
            DifficultyImage->SetPosition(ClassesImports::Point(Row->ClientSize.X - DifficultyImage->ClientSize.X - 10, Row->ClientSize.Y / 2 - DifficultyImage->ClientSize.Y / 2));
            DifficultyImage->SetActive(true);
        }
    }

    void TfLoadAB::SelectArena(std::int32_t Index) {
        std::int32_t Previous = SelectedIndex;
        SelectedIndex = Index;
        if (Entries.length() - 1 + 1 <= SelectedIndex) {
            SelectedIndex = -1;
        }
        UpdateArenaRow(Previous);
        UpdateArenaRow(SelectedIndex);
        if (Index < 0 || Entries.length() - 1 < Index) {
            ShowSelectedArenaDetails();
            return;
        }
        if (SelectedIndex >= 0) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
            {
                GI_MessageLoop::TObjectGI* cpp_with_2 = Entries[SelectedIndex].BackgroundImage->Parent;
                PanelSlot->ScrollRectIntoView(cpp_with_2->GetLocalBounds());
            }
        }
        ShowSelectedArenaDetails();
    }

    void TfLoadAB::UpdateArenaRow(std::int32_t Index) {
        if (Index < 0 || Entries.length() - 1 < Index) {
            return;
        }
        GI_Image::TImageGI* Image = Entries[Index].BackgroundImage;
        if (SelectedIndex == Index) {
            Image->SetImagePath(u"GI,Bm.FormLoadAB.SlotActive"_w);
        } else if (HoveredIndex == Index) {
            Image->SetImagePath(u"GI,Bm.FormLoadAB.SlotOnMouse"_w);
        } else {
            Image->SetImagePath(u"GI,Bm.FormLoadAB.SlotNormal"_w);
        }
    }

    void TfLoadAB::ArenaRowMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Previous{};
        if (HoveredIndex != Sender->UserValue) {
            Previous = HoveredIndex;
            HoveredIndex = Sender->UserValue;
            UpdateArenaRow(Previous);
            UpdateArenaRow(HoveredIndex);
        }
    }

    void TfLoadAB::ArenaRowMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Previous{};
        if (HoveredIndex != -1) {
            Previous = HoveredIndex;
            HoveredIndex = -1;
            UpdateArenaRow(Previous);
            UpdateArenaRow(HoveredIndex);
        }
    }

    void TfLoadAB::ArenaRowMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
        SelectArena(Sender->UserValue);
        GI_Main::BreakUiMessage();
    }

    void TfLoadAB::ArenaRowDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        StartSelectedArena(nullptr);
    }

    void TfLoadAB::SelectCategory(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t NewCategory = EC_Str::ExtractDigitsToIntW(Sender->ControlName);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"_wref.get()))->SetDown(NewCategory == 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"_wref.get()))->SetDown(NewCategory == 1);
        if (Category != NewCategory) {
            {
                GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
                PanelSlot->ScrollRectIntoView(ClassesImports::Rect(0, 0, 1, 1));
            }
            Category = NewCategory;
            SelectedIndex = -1;
            RebuildArenaList();
            SelectArena(0);
        }
    }

    void TfLoadAB::StartSelectedArena(GI_MessageLoop::TObjectGI* Sender) {
        if (SelectedIndex < 0) {
            return;
        }
        if (Entries.length() - 1 < SelectedIndex) {
            return;
        }
        Globals::ArcadeBattleScreen->SelectedMapName = Entries[SelectedIndex].MapName;
        GlobalsV::RequestedScreenId = GlobalsV::screenArcadeBattle;
        RequestClose(1);
        GI_Main::BreakUiMessage();
    }

    void TfLoadAB::ShowSelectedArenaDetails() {
        pas::WideString Path{};
        EC_File::TFileEC* FileHandle{};
        std::uint8_t Opened{};
        EC_Buf::TBufEC* Buffer{};
        {
            GI_GraphButton::TGraphButtonGI* ButStart = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButStart"_wref.get()));
            ButStart->SetDisabled(SelectedIndex < 0 || Entries.length() - 1 < SelectedIndex);
        }
        {
            GI_GraphBuf::TGraphBufGI* ImageMap = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"ImageMap"_wref.get()));
            ImageMap->SetActive(false);
            if (SelectedIndex >= 0 && Entries.length() - 1 >= SelectedIndex) {
                if (Category == 0 && EC_Str::CountDelimitedPartsW(Entries[SelectedIndex].ImageName, u"\\"_wref.get()) <= 1) {
                    Path = pas::concat_wide({u"Data\\ABMap\\", Entries[SelectedIndex].ImageName});
                } else {
                    Path = Entries[SelectedIndex].ImageName;
                }
                FileHandle = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
                FileHandle->SetFileName(Path);
                Opened = FileHandle->TryAcquireReadHandle(false);
                if (Opened) {
                    Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                    Buffer->SetSize(FileHandle->GetSize());
                    FileHandle->ReadBuffer(Buffer->Data, Buffer->DataSize);
                    ImageMap->SetActive(true);
                    ImageMap->GraphBuf->LoadImageRgb(Buffer);
                    ImageMap->SourceHasPerPixelAlpha = false;
                    if (ImageMap->ClientSize.X != ImageMap->GraphBuf->Width || ImageMap->ClientSize.Y != ImageMap->GraphBuf->Height) {
                        ImageMap->GraphBuf->RescaleRgb(ImageMap->ClientSize.X, ImageMap->ClientSize.Y);
                    }
                    ImageMap->GraphBuf->ConvertRgbTo565();
                    ImageMap->Invalidate();
                    pas::free(Buffer);
                }
                pas::free(FileHandle);
            }
        }
        {
            GI_Label::TLabelGI* with_MessageText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"MessageText"_wref.get()));
            with_MessageText->SetActive(false);
            if (SelectedIndex >= 0 && Entries.length() - 1 >= SelectedIndex) {
                with_MessageText->SetActive(true);
                with_MessageText->SetText(Entries[SelectedIndex].Description);
            }
        }
        {
            GI_PanelScrollBar::TPanelScrollBarGI* MessageWindow = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"_wref.get()));
            MessageWindow->SetScrollOffset(ClassesImports::Point(0, 0));
            MessageWindow->UpdateScrollRanges();
            MessageWindow->VerticalScrollBar->SetActive(MessageWindow->FindByNameRecursive(u"MessageText"_wref.get())->Active && pas::checked_cast<GI_Label::TLabelGI*>(MessageWindow->FindByNameRecursive(u"MessageText"_wref.get()))->ClientSize.Y > MessageWindow->ClientSize.Y);
            {
                std::int32_t lineHeight = pas::checked_cast<GI_Label::TLabelGI*>(MessageWindow->FindByNameRecursive(u"MessageText"_wref.get()))->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = MessageWindow->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(lineHeight);
            }
            MessageWindow->VerticalScrollBar->SetLargeChange(MessageWindow->ClientSize.Y);
            MessageWindow->VerticalScrollBar->SetPageSize(MessageWindow->ClientSize.Y);
        }
    }

    // Native empty hook, retained during catalog rebuild.
    void TfLoadAB::PrepareCatalog() {
    }

    // Returns the configured ABMap entry count for the main menu.
    pas::WideString TfLoadAB::GetCatalogSummary() {
        return pas::wide_int_to_str(GR_Main::LanguageDataConfig->GetBlockByPath(u"ABMap"_wref.get())->GetBlockCount());
    }

    void TfLoadAB::SelectMusic() {
    }

    void TfLoadAB::p_destroy() {
        fLoadAB::TfLoadAB_Destroy(this);
    }

} // namespace fLoadAB
