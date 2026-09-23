#include "layout/fLoadRobot.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Sound.hpp"
#include "types/System.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Robot.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"
#include "units/fLoadRobot.hpp"
#include "units/fPanelLoad.hpp"

namespace fLoadRobot {
    std::uint32_t RobotMapNameColor{};

    void TfLoadRobot_Create(TfLoadRobot* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->SelectedIndex = -1;
        Self->Category = 0;
        Self->Difficulty = 1;
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfLoadRobot_Destroy(TfLoadRobot* Self) {
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfLoadRobot::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fLoadRobot... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GI_MessageLoop::TObjectGI* Root = GetByName(u""sv);
        Root->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Root->FindByNameRecursive(u"BGBuf"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GI_MessageLoop::TObjectGI* Panel = Root->FindByNameRecursive(u"MainPanel"sv);
        if (GR_Main::ExtraScreenHeight < 0) {
            Panel->SetPosition(ClassesImports::Point(Panel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, (GR_Main::GameScreenHeight - Panel->ClientSize.Y) / 2));
        } else {
            Panel->SetPosition(ClassesImports::Point(Panel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Panel->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfLoadRobot::KeyDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"sv))->UpCallback = pas::bind_method<&TfLoadRobot::CloseClick>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButCancel"sv))->UpCallback = pas::bind_method<&TfLoadRobot::CloseClick>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButStart"sv))->UpCallback = pas::bind_method<&TfLoadRobot::StartClick>(this);
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
            ButGroup0->UpCallback = pas::bind_method<&TfLoadRobot::CategoryClick>(this);
            ButGroup0->DownCallback = pas::bind_method<&TfLoadRobot::CategoryClick>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
            ButGroup1->UpCallback = pas::bind_method<&TfLoadRobot::CategoryClick>(this);
            ButGroup1->DownCallback = pas::bind_method<&TfLoadRobot::CategoryClick>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
            ButGroup2->UpCallback = pas::bind_method<&TfLoadRobot::CategoryClick>(this);
            ButGroup2->DownCallback = pas::bind_method<&TfLoadRobot::CategoryClick>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Dif1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif1"sv));
            Dif1->UpCallback = pas::bind_method<&TfLoadRobot::DifficultyClick>(this);
            Dif1->DownCallback = pas::bind_method<&TfLoadRobot::DifficultyClick>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Dif2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif2"sv));
            Dif2->UpCallback = pas::bind_method<&TfLoadRobot::DifficultyClick>(this);
            Dif2->DownCallback = pas::bind_method<&TfLoadRobot::DifficultyClick>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Dif3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif3"sv));
            Dif3->UpCallback = pas::bind_method<&TfLoadRobot::DifficultyClick>(this);
            Dif3->DownCallback = pas::bind_method<&TfLoadRobot::DifficultyClick>(this);
        }
        RobotMapNameColor = GR_Main::GetStyleColorGI(u"LoadRobot.MapsNameColor"_w, 88, 229, 255);
    }

    void TfLoadRobot::OnOpen() {
        pas::WideString StartText{};
        pas::WideString WinText{};
        pas::WideString LossText{};
        pas::WideString TerronName{};
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        LoadPanel->OnOpen();
        KeyHistory = pas::WideString();
        if (BattleResult == 1) {
            if (Entries[SelectedIndex].MapIndex >= 0) {
                StartText = Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].RobotsStart;
                WinText = Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].RobotsWin;
                LossText = Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].RobotsLoss;
            } else {
                StartText = aConst::LocalizedText(u"FormLoadRobot.StdBegin"_wref.get());
                WinText = aConst::LocalizedText(u"FormLoadRobot.StdVictory"_wref.get());
                LossText = aConst::LocalizedText(u"FormLoadRobot.StdDefeat"_wref.get());
            }
            {
                auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText = aConst::LocalizedText(u"FormLoadRobot.PStar"_wref.get());
                aMyFunction::ReplaceTextToken(StartText, u"<Star>"_w, std::move(localizedText), textHighlightColorTag.get());
            }
            {
                auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText_2 = aConst::LocalizedText(u"FormLoadRobot.PPlanet"_wref.get());
                aMyFunction::ReplaceTextToken(StartText, u"<Planet>"_w, std::move(localizedText_2), textHighlightColorTag_2.get());
            }
            {
                auto textHighlightColorTag_3 = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText_3 = aConst::LocalizedText(u"FormLoadRobot.PPlayer"_wref.get());
                aMyFunction::ReplaceTextToken(StartText, u"<Player>"_w, std::move(localizedText_3), textHighlightColorTag_3.get());
            }
            aConst::ExpandLocalizedTextMarkupAndPrefixLines(StartText);
            StartText = pas::concat_wide({pas::wide_int_to_str(Difficulty), StartText});
            StartText = pas::concat_wide({pas::wide_int_to_str(2), StartText});
            StartText = pas::concat_wide({pas::wide_int_to_str(3), StartText});
            {
                auto textHighlightColorTag_4 = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText_4 = aConst::LocalizedText(u"FormLoadRobot.PStar"_wref.get());
                aMyFunction::ReplaceTextToken(WinText, u"<Star>"_w, std::move(localizedText_4), textHighlightColorTag_4.get());
            }
            {
                auto textHighlightColorTag_5 = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText_5 = aConst::LocalizedText(u"FormLoadRobot.PPlanet"_wref.get());
                aMyFunction::ReplaceTextToken(WinText, u"<Planet>"_w, std::move(localizedText_5), textHighlightColorTag_5.get());
            }
            {
                auto textHighlightColorTag_6 = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText_6 = aConst::LocalizedText(u"FormLoadRobot.PPlayer"_wref.get());
                aMyFunction::ReplaceTextToken(WinText, u"<Player>"_w, std::move(localizedText_6), textHighlightColorTag_6.get());
            }
            aConst::ExpandLocalizedTextMarkupAndPrefixLines(WinText);
            {
                auto textHighlightColorTag_7 = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText_7 = aConst::LocalizedText(u"FormLoadRobot.PStar"_wref.get());
                aMyFunction::ReplaceTextToken(LossText, u"<Star>"_w, std::move(localizedText_7), textHighlightColorTag_7.get());
            }
            {
                auto textHighlightColorTag_8 = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText_8 = aConst::LocalizedText(u"FormLoadRobot.PPlanet"_wref.get());
                aMyFunction::ReplaceTextToken(LossText, u"<Planet>"_w, std::move(localizedText_8), textHighlightColorTag_8.get());
            }
            {
                auto textHighlightColorTag_9 = pas::borrow(aMyFunction::TextHighlightColorTag);
                pas::WideString localizedText_9 = aConst::LocalizedText(u"FormLoadRobot.PPlayer"_wref.get());
                aMyFunction::ReplaceTextToken(LossText, u"<Player>"_w, std::move(localizedText_9), textHighlightColorTag_9.get());
            }
            aConst::ExpandLocalizedTextMarkupAndPrefixLines(LossText);
            TerronName = aConst::LocalizedText(u"FormLoadRobot.PPlace"_wref.get());
            LoadPanel->OnOpen();
            LoadPanel->SelectBackgroundStyle(3);
            LoadPanel->RefreshBackgroundImages();
            BattleResult = Robot::FRun(Entries[SelectedIndex].FileName, StartText, WinText, LossText, TerronName);
            GR_Main::PostMouseMoveMessage();
            if (BattleResult != 0) {
                if (Entries[SelectedIndex].MapIndex >= 0) {
                    if (BattleResult == 3) {
                        LoadCompletionData();
                        if (Difficulty == 1) {
                            RecordCompletion(Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].Id, -Robot::RobotBattleStatistics.SignedTimeMs / 1000, 2);
                        } else {
                            RecordCompletion(Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].Id, -Robot::RobotBattleStatistics.SignedTimeMs / 1000, 1);
                        }
                        SaveCompletionData();
                    }
                }
                BattleResult = 0;
                GlobalsV::RequestedScreenId = GlobalsV::screenLoadRobot;
                RequestClose(1);
            }
        } else {
            if (BattleResult == 0) {
                if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
                    GR_Main::CaptureScreenBackground(true, 0);
                }
                pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"sv))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
            } else {
                BattleResult = 0;
            }
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
            RebuildEntries();
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif1"sv))->SetDown(Difficulty == 1);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif2"sv))->SetDown(Difficulty == 2);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif3"sv))->SetDown(Difficulty == 3);
            SelectEntry(0);
        }
    }

    void TfLoadRobot::OnClose() {
        if (BattleResult == 0) {
            Entries = nullptr;
        }
        LoadPanel->OnClose();
        GI_MessageLoop::TMessageLoopGI::OnClose();
    }

    void TfLoadRobot::CloseClick(GI_MessageLoop::TObjectGI* Sender) {
        GR_Main::AuxRenderBuffer->Clear();
        GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
        RequestClose(1);
    }

    void TfLoadRobot::KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        std::int32_t I{};
        std::int32_t MapId{};
        std::uint8_t CheatKeys = false;
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT)) {
            KeyHistory = pas::concat_wide({KeyHistory, static_cast<char16_t>(Key)});
            CheatKeys = true;
        }
        if (CheatKeys) {
            if (EC_Str::FindTextOffsetW(KeyHistory, u"WIN"_wref.get(), 0) >= 0) {
                KeyHistory = pas::WideString();
                if (SelectedIndex >= 0 && SelectedIndex <= Entries.length() - 1 && Entries[SelectedIndex].MapIndex >= 0) {
                    LoadCompletionData();
                    while (Entries[SelectedIndex].MapIndex >= 0 && Entries[SelectedIndex].MapIndex <= Globals::RobotMapDefinitions.length() - 1) {
                        MapId = Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].Id;
                        if (MapId >= 0 && MapId < (CompletionData.length() - 1 + 1) / 2 && CompletionData[MapId * 2 + 1] != 0) {
                            break;
                        }
                        RecordCompletion(MapId, 0, 1);
                        SaveCompletionData();
                        RebuildEntries();
                        break;
                    }
                }
            }
        }
        if (Key == WindowsSdk::VK_ESCAPE) {
            CloseClick(Sender);
        } else if (Key == 'R') {
            CloseClick(Sender);
        } else if (Key == WindowsSdk::VK_PRIOR) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
            if (PanelSlot->VerticalScrollBar->Active) {
                PanelSlot->VerticalScrollBar->SetPosition_2(PanelSlot->VerticalScrollBar->Position - PanelSlot->VerticalScrollBar->LargeChange);
            }
        } else if (Key == WindowsSdk::VK_NEXT) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSlot_2 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
            if (PanelSlot_2->VerticalScrollBar->Active) {
                PanelSlot_2->VerticalScrollBar->SetPosition_2(PanelSlot_2->VerticalScrollBar->Position + PanelSlot_2->VerticalScrollBar->LargeChange);
            }
        } else if (Key == WindowsSdk::VK_HOME) {
            if (Entries.length() - 1 + 1 > 0 && Entries[0].Access <= UnlockedAccess) {
                SelectEntry(0);
            }
        } else if (Key == WindowsSdk::VK_END) {
            const std::int32_t cpp_first = Entries.length() - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    if (Entries[I].Access <= UnlockedAccess) {
                        SelectEntry(I);
                        break;
                    }
                }
            }
        } else if (Key == WindowsSdk::VK_UP) {
            EntryMouseLeave(nullptr);
            if (SelectedIndex > 0) {
                SelectEntry(SelectedIndex - 1);
            } else if (Entries.length() - 1 + 1 > 0 && Entries[0].Access <= UnlockedAccess) {
                SelectEntry(0);
            }
        } else if (Key == WindowsSdk::VK_DOWN) {
            EntryMouseLeave(nullptr);
            if (SelectedIndex >= 0) {
                if (SelectedIndex < Entries.length() - 1 && Entries[SelectedIndex + 1].Access <= UnlockedAccess) {
                    SelectEntry(SelectedIndex + 1);
                }
            } else if (Entries.length() - 1 + 1 > 0 && Entries[0].Access <= UnlockedAccess) {
                SelectEntry(0);
            }
        } else if (Key == WindowsSdk::VK_RETURN) {
            StartClick(nullptr);
        } else if (Key == WindowsSdk::VK_TAB) {
            EntryMouseLeave(nullptr);
            if (Category == 0) {
                CategoryClick(GetByName(u"ButGroup1"sv));
            } else if (Category == 1) {
                CategoryClick(GetByName(u"ButGroup2"sv));
            } else if (Category == 2) {
                CategoryClick(GetByName(u"ButGroup0"sv));
            }
        }
    }

    void TfLoadRobot::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
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

    std::int32_t TfLoadRobot::InsertEntry(std::int32_t Access) {
        std::int32_t J{};
        std::int32_t I = 0;
        while (I <= Entries.length() - 1 && Entries[I].Access <= Access) {
            ++I;
        }
        Entries.set_length(Entries.length() - 1 + 1 + 1);
        for (auto cpp_range = pas::for_downto<std::int32_t>(Entries.length() - 1, I + 1); cpp_range.next(J); ) {
            Entries[J] = Entries[J - 1];
        }
        Entries[I].Access = Access;
        return I;
    }

    void TfLoadRobot::RebuildEntries() {
        pas::WideString cpp_text{};
        std::int32_t I{};
        std::int32_t EntryIndex{};
        GI_Panel::TPanelGI* EntryPanel{};
        pas::WideString Path{};
        std::uint32_t FindHandle{};
        WindowsSdk::TWin32FindDataW FindData{};
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
        Panel->FreeOwnedChildren();
        LoadCompletionData();
        std::int32_t Y = 0;
        Entries = nullptr;
        if (Category == 0 || Category == 1) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Globals::RobotMapDefinitions.length() - 1); cpp_range.next(I); ) {
                if (Globals::RobotMapDefinitions[I].Group == Category) {
                    EntryIndex = InsertEntry(Globals::RobotMapDefinitions[I].Access);
                    Entries[EntryIndex].Name = Globals::RobotMapDefinitions[I].Name;
                    Entries[EntryIndex].FileName = Globals::RobotMapDefinitions[I].Map;
                    Entries[EntryIndex].MapIndex = I;
                    Entries[EntryIndex].Side = Globals::RobotMapDefinitions[I].Side;
                    Entries[EntryIndex].Length = Globals::RobotMapDefinitions[I].Length;
                }
            }
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Globals::RobotMapDefinitions.length() - 1); cpp_range_2.next(I); ) {
                if (Globals::RobotMapDefinitions[I].Group == Category) {
                    EntryIndex = InsertEntry(0);
                    Entries[EntryIndex].Name = Globals::RobotMapDefinitions[I].Name;
                    Entries[EntryIndex].FileName = Globals::RobotMapDefinitions[I].Map;
                    Entries[EntryIndex].MapIndex = I;
                    Entries[EntryIndex].Side = Globals::RobotMapDefinitions[I].Side;
                    Entries[EntryIndex].Length = Globals::RobotMapDefinitions[I].Length;
                }
            }
            Path = pas::WideString();
            if (GR_Main::InstallConfig->CountParams(u"RobotPath"_wref.get()) > 0) {
                Path = GR_Main::InstallConfig->GetParam(u"RobotPath"sv);
            }
            FindHandle = WindowsSdk::FindFirstFileW((cpp_text = pas::concat_wide({Path, u"Matrix\\Map\\*.cmap"}), cpp_text.pchar()), FindData);
            if (FindHandle != WindowsImports::INVALID_HANDLE_VALUE) {
                do {
                    if ((FindData.dwFileAttributes & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) == 0) {
                        Path = EC_Str::TrimWideString(EC_Str::LowerCaseWideString(pas::array_text<pas::WideString>(FindData.cFileName.elements, 260)));
                        I = 0;
                        while (I <= Globals::RobotMapDefinitions.length() - 1) {
                            if (Path == EC_Str::TrimWideString(EC_Str::LowerCaseWideString(Globals::RobotMapDefinitions[I].Map))) {
                                break;
                            }
                            ++I;
                        }
                        if (I > Globals::RobotMapDefinitions.length() - 1) {
                            EntryIndex = InsertEntry(0);
                            Entries[EntryIndex].Name = EC_Str::ExtractFileNameNoExtW(pas::array_text<pas::WideString>(FindData.cFileName.elements, 260));
                            Entries[EntryIndex].FileName = pas::array_text<pas::WideString>(FindData.cFileName.elements, 260);
                            Entries[EntryIndex].MapIndex = -1;
                            Entries[EntryIndex].Side = 0;
                            Entries[EntryIndex].Length = -1;
                        }
                    }
                } while (WindowsSdk::FindNextFileW(FindHandle, FindData));
                WindowsImports::FindClose(FindHandle);
            }
        }
        UnlockedAccess = GetUnlockedAccess();
        std::uint8_t Alternate = true;
        EntryIndex = -100;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Entries.length() - 1); cpp_range_3.next(I); ) {
            if (I != 0) {
                Y += GR_Main::GiScalePixels(5);
            }
            if (Entries[I].Access != EntryIndex) {
                Alternate = static_cast<std::uint8_t>(Alternate ^ 1);
                EntryIndex = Entries[I].Access;
            }
            Entries[I].AlternateBackground = Alternate;
            EntryPanel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
            EntryPanel->UserValue = I;
            EntryPanel->SetPosition(ClassesImports::Point(0, Y));
            BuildEntryPanel(EntryPanel);
            Y += EntryPanel->ClientSize.Y;
            EntryPanel->SetPositionModeW(true);
            Panel->VerticalScrollBar->SetSmallChange(EntryPanel->ClientSize.Y + GR_Main::GiScalePixels(5));
            UpdateEntryImage(I);
        }
        Panel->UpdateScrollRanges();
        Panel->VerticalScrollBar->SetActive(Panel->ClientSize.Y < Y);
        Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
        I = Panel->VerticalScrollBar->Position;
        Panel->VerticalScrollBar->SetPosition_2(I - 1);
        Panel->VerticalScrollBar->SetPosition_2(I);
        if (SelectedIndex < 0 || SelectedIndex >= Entries.length() - 1 + 1) {
            SelectedIndex = -1;
        }
        SelectEntry(SelectedIndex);
        Panel->Invalidate();
    }

    void TfLoadRobot::BuildEntryPanel(GI_MessageLoop::TObjectGI* Panel) {
        std::int32_t Index{};
        std::int32_t MapId{};
        std::int32_t Hours{};
        std::int32_t Minutes{};
        std::int32_t Seconds{};
        pas::WideString TimeText{};
        Index = Panel->UserValue;
        Panel->MouseEnterCallback = pas::bind_method<&TfLoadRobot::EntryMouseEnter>(this);
        Panel->MouseLeaveCallback = pas::bind_method<&TfLoadRobot::EntryMouseLeave>(this);
        Panel->LeftButtonDownCallback = pas::bind_method<&TfLoadRobot::EntryMouseDown>(this);
        Panel->LeftButtonDoubleClickCallback = pas::bind_method<&TfLoadRobot::EntryDoubleClick>(this);
        Entries[Index].Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
        {
            GI_Image::TImageGI* cpp_with = Entries[Index].Image;
            cpp_with->SetPosition(ClassesImports::Point(0, 0));
            cpp_with->SetDepth(1.0E+1);
            if (Entries[Index].Access <= UnlockedAccess) {
                if (!Entries[Index].AlternateBackground) {
                    cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"SlotNormal"}));
                } else {
                    cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"SlotNormal2"}));
                }
            } else {
                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"SlotDisabled"}));
            }
            cpp_with->SetSize(cpp_with->GetContentSize());
            Panel->SetSize(cpp_with->ClientSize);
            cpp_with->SetActive(true);
        }
        std::int32_t RightEdge = GR_Main::GiScalePixels(313);
        if ((Entries[Index].Side & 4) != 0) {
            GI_Image::TImageGI* cpp_with_2 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            cpp_with_2->SetDepth(9.0);
            if (Entries[Index].Access <= UnlockedAccess) {
                cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"IconKeller"}));
            } else {
                cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"IconKellerD"}));
            }
            cpp_with_2->SetSize(cpp_with_2->GetContentSize());
            cpp_with_2->SetPosition(ClassesImports::Point(RightEdge - cpp_with_2->ClientSize.X, Panel->ClientSize.Y / 2 - cpp_with_2->ClientSize.Y / 2));
            cpp_with_2->SetActive(true);
            RightEdge = RightEdge - cpp_with_2->ClientSize.X - 1;
        }
        if ((Entries[Index].Side & 2) != 0) {
            GI_Image::TImageGI* cpp_with_3 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            cpp_with_3->SetDepth(9.0);
            if (Entries[Index].Access <= UnlockedAccess) {
                cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"IconTerron"}));
            } else {
                cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"IconTerronD"}));
            }
            cpp_with_3->SetSize(cpp_with_3->GetContentSize());
            cpp_with_3->SetPosition(ClassesImports::Point(RightEdge - cpp_with_3->ClientSize.X, Panel->ClientSize.Y / 2 - cpp_with_3->ClientSize.Y / 2));
            cpp_with_3->SetActive(true);
            RightEdge = RightEdge - cpp_with_3->ClientSize.X - 1;
        }
        if ((Entries[Index].Side & 1) != 0) {
            GI_Image::TImageGI* cpp_with_4 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            cpp_with_4->SetDepth(9.0);
            if (Entries[Index].Access <= UnlockedAccess) {
                cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"IconBlazer"}));
            } else {
                cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"IconBlazerD"}));
            }
            cpp_with_4->SetSize(cpp_with_4->GetContentSize());
            cpp_with_4->SetPosition(ClassesImports::Point(RightEdge - cpp_with_4->ClientSize.X, Panel->ClientSize.Y / 2 - cpp_with_4->ClientSize.Y / 2));
            cpp_with_4->SetActive(true);
            RightEdge = RightEdge - cpp_with_4->ClientSize.X - 1;
        }
        std::int32_t MapIndex = Entries[Index].MapIndex;
        if (MapIndex >= 0 && MapIndex <= Globals::RobotMapDefinitions.length() - 1) {
            MapId = Globals::RobotMapDefinitions[MapIndex].Id;
            if (MapId >= 0 && MapId < (CompletionData.length() - 1 + 1) / 2) {
                MapIndex = CompletionData[MapId * 2 + 1];
                if (MapIndex != 0) {
                    GI_Image::TImageGI* cpp_with_5 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                    cpp_with_5->SetPosition(ClassesImports::Point(0, 0));
                    cpp_with_5->SetDepth(9.0);
                    if (MapIndex == 1) {
                        cpp_with_5->SetImagePath(pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_06"}));
                    } else {
                        cpp_with_5->SetImagePath(pas::concat_wide({u"GI,Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_13"}));
                    }
                    cpp_with_5->SetSize(cpp_with_5->GetContentSize());
                    cpp_with_5->SetPosition(ClassesImports::Point(Panel->ClientSize.X - cpp_with_5->ClientSize.X - GR_Main::GiScalePixels(10), Panel->ClientSize.Y / 2 - cpp_with_5->ClientSize.Y / 2));
                    cpp_with_5->SetActive(true);
                }
            }
        }
        {
            GI_Label::TLabelGI* cpp_with_6 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            if (GR_Main::GiResourceVariant() == 1) {
                cpp_with_6->SetPosition(ClassesImports::Point(11, 0));
                cpp_with_6->SetSize(ClassesImports::Point(RightEdge - cpp_with_6->LocalPosition.X, Panel->ClientSize.Y));
            } else {
                cpp_with_6->SetPosition(ClassesImports::Point(14, 0));
                cpp_with_6->SetSize(ClassesImports::Point(RightEdge - cpp_with_6->LocalPosition.X, Panel->ClientSize.Y));
            }
            cpp_with_6->SetDepth(7.0);
            cpp_with_6->SetFontName(GlobalsV::NormalFontName);
            cpp_with_6->SetTextAlignX(GI_Main::taxLeft);
            cpp_with_6->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with_6->SetText(Entries[Index].Name);
            if (Entries[Index].Access <= UnlockedAccess) {
                cpp_with_6->SetTextColor(RobotMapNameColor);
            } else {
                cpp_with_6->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(112, 112, 112));
            }
        }
        {
            GI_Label::TLabelGI* cpp_with_7 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            if (GR_Main::GiResourceVariant() == 1) {
                cpp_with_7->SetPosition(ClassesImports::Point(252, 0));
                cpp_with_7->SetSize(ClassesImports::Point(109, Panel->ClientSize.Y));
            } else {
                cpp_with_7->SetPosition(ClassesImports::Point(323, 0));
                cpp_with_7->SetSize(ClassesImports::Point(140, Panel->ClientSize.Y));
            }
            cpp_with_7->SetDepth(7.0);
            cpp_with_7->SetFontName(GlobalsV::NormalFontName);
            cpp_with_7->SetTextAlignX(GI_Main::taxRight);
            cpp_with_7->SetTextAlignY(GI_Main::tayCenterEx);
            MapIndex = Entries[Index].MapIndex;
            do {
                if (MapIndex >= 0 && MapIndex <= Globals::RobotMapDefinitions.length() - 1) {
                    MapId = Globals::RobotMapDefinitions[MapIndex].Id;
                    if (MapId >= 0 && MapId < (CompletionData.length() - 1 + 1) / 2 && CompletionData[MapId * 2 + 1] != 0) {
                        RightEdge = CompletionData[MapId * 2];
                        Hours = RightEdge / 3600;
                        Minutes = (RightEdge - Hours * 3600) / 60;
                        Seconds = RightEdge - Hours * 3600 - Minutes * 60;
                        if (Hours < 10) {
                            TimeText = pas::concat_wide({TimeText, u"0"});
                        }
                        TimeText = pas::concat_wide({TimeText, pas::wide_int_to_str(Hours), u":"});
                        if (Minutes < 10) {
                            TimeText = pas::concat_wide({TimeText, u"0"});
                        }
                        TimeText = pas::concat_wide({TimeText, pas::wide_int_to_str(Minutes), u":"});
                        if (Seconds < 10) {
                            TimeText = pas::concat_wide({TimeText, u"0"});
                        }
                        TimeText = pas::concat_wide({TimeText, pas::wide_int_to_str(Seconds)});
                        TimeText = ([&] {
                            const pas::WideString& localizedText = aConst::LocalizedText(u"FormLoadRobot.Time"_wref.get());
                            const pas::WideString& timeText = TimeText;
                            return EC_Str::ReplaceAllWideString(localizedText, u"<Val>"_wref.get(), pas::view(timeText));
                        }());
                        cpp_with_7->SetText(TimeText);
                        break;
                    }
                }
                if (Entries[Index].Length < 0) {
                    cpp_with_7->SetText(u""_wref.get());
                } else {
                    cpp_with_7->SetText(aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"FormLoadRobot.Length", SysUtils::IntToStr(Entries[Index].Length)}))));
                }
            } while (!true);
            if (Entries[Index].Access <= UnlockedAccess) {
                cpp_with_7->SetTextColor(RobotMapNameColor);
            } else {
                cpp_with_7->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(112, 112, 112));
            }
        }
    }

    void TfLoadRobot::SelectEntry(std::int32_t Index) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel{};
        GI_MessageLoop::TObjectGI* EntryPanel{};
        std::int32_t Previous = SelectedIndex;
        SelectedIndex = Index;
        if (SelectedIndex >= Entries.length() - 1 + 1) {
            SelectedIndex = -1;
        }
        if (SelectedIndex >= 0 && Entries[SelectedIndex].Access > UnlockedAccess) {
            SelectedIndex = -1;
        }
        UpdateEntryImage(Previous);
        UpdateEntryImage(SelectedIndex);
        if (Index < 0 || Index > Entries.length() - 1) {
            UpdateSelectionDetails();
            return;
        }
        if (SelectedIndex >= 0) {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
            EntryPanel = Entries[SelectedIndex].Image->Parent;
            Panel->ScrollRectIntoView(EntryPanel->GetLocalBounds());
        }
        UpdateSelectionDetails();
    }

    void TfLoadRobot::UpdateEntryImage(std::int32_t Index) {
        if (Index < 0 || Index > Entries.length() - 1) {
            return;
        }
        GI_Image::TImageGI* Image = Entries[Index].Image;
        if (Entries[Index].Access > UnlockedAccess) {
            return;
        }
        if (SelectedIndex == Index) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"SlotActive"}));
        } else if (HoveredIndex == Index) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"SlotOnMouse"}));
        } else if (!Entries[Index].AlternateBackground) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"SlotNormal"}));
        } else {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormLoadRobot.", GR_Main::GiResourceSuffix(), u"SlotNormal2"}));
        }
    }

    void TfLoadRobot::EntryMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (BattleResult != 0) {
            return;
        }
        if (HoveredIndex == Sender->UserValue) {
            return;
        }
        std::int32_t Previous = HoveredIndex;
        HoveredIndex = Sender->UserValue;
        UpdateEntryImage(Previous);
        UpdateEntryImage(HoveredIndex);
    }

    void TfLoadRobot::EntryMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (BattleResult != 0) {
            return;
        }
        if (HoveredIndex == -1) {
            return;
        }
        std::int32_t Previous = HoveredIndex;
        HoveredIndex = -1;
        UpdateEntryImage(Previous);
        UpdateEntryImage(HoveredIndex);
    }

    void TfLoadRobot::EntryMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (BattleResult != 0) {
            return;
        }
        GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
        SelectEntry(Sender->UserValue);
    }

    void TfLoadRobot::EntryDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        StartClick(nullptr);
    }

    void TfLoadRobot::CategoryClick(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t NewCategory = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv))->SetDown(NewCategory == 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv))->SetDown(NewCategory == 1);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv))->SetDown(NewCategory == 2);
        if (Category == NewCategory) {
            return;
        }
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"sv));
        Panel->ScrollRectIntoView(ClassesImports::Rect(0, 0, 1, 1));
        Category = NewCategory;
        SelectedIndex = -1;
        RebuildEntries();
        SelectEntry(0);
    }

    void TfLoadRobot::DifficultyClick(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t NewDifficulty = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif1"sv))->SetDown(NewDifficulty == 1);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif2"sv))->SetDown(NewDifficulty == 2);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif3"sv))->SetDown(NewDifficulty == 3);
        if (Difficulty != NewDifficulty) {
            Difficulty = NewDifficulty;
        }
    }

    void TfLoadRobot::StartClick(GI_MessageLoop::TObjectGI* Sender) {
        if (BattleResult != 0) {
            return;
        }
        if (SelectedIndex < 0 || SelectedIndex > Entries.length() - 1) {
            return;
        }
        BattleResult = 1;
        GlobalsV::RequestedScreenId = GlobalsV::screenLoadRobot;
        LoadPanel->SelectBackgroundStyle(3);
        LoadPanel->RefreshBackgroundImages();
        LoadPanel->StartClosingShutters();
    }

    void TfLoadRobot::UpdateSelectionDetails() {
        pas::WideString Text{};
        pas::WideString MapName{};
        pas::WideString Extension{};
        EC_File::TFileEC* F{};
        std::uint8_t Success{};
        EC_Buf::TBufEC* Buffer{};
        {
            GI_GraphButton::TGraphButtonGI* ButStart = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButStart"sv));
            ButStart->SetDisabled(SelectedIndex < 0 || SelectedIndex > Entries.length() - 1);
        }
        {
            GI_GraphBuf::TGraphBufGI* ImageMap = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"ImageMap"sv));
            ImageMap->SetActive(false);
            if (SelectedIndex >= 0 && SelectedIndex <= Entries.length() - 1) {
                if (EC_Str::CountDelimitedPartsW(pas::view(Entries[SelectedIndex].FileName), u"\\/"sv) == 1) {
                    Text = pas::concat_wide({u"Matrix\\Map\\", EC_Str::ExtractFileNameNoExtW(Entries[SelectedIndex].FileName), u".jpg"});
                } else {
                    MapName = Entries[SelectedIndex].FileName;
                    Extension = EC_Str::ExtractDelimitedPartW(pas::view(MapName), EC_Str::CountDelimitedPartsW(pas::view(MapName), u"."sv) - 1, u"."sv);
                    Text = EC_Str::ReplaceAllWideString(Entries[SelectedIndex].FileName, Extension, u"jpg"sv);
                }
                F = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
                F->SetFileName(Text);
                Success = F->TryAcquireReadHandle(false);
                if (Success) {
                    Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                    Buffer->SetSize(F->GetSize());
                    F->ReadBuffer(Buffer->Data, Buffer->DataSize);
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
                pas::free(F);
            }
        }
        {
            GI_Label::TLabelGI* with_MessageText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"MessageText"sv));
            with_MessageText->SetActive(false);
            if (SelectedIndex >= 0 && SelectedIndex <= Entries.length() - 1 && Entries[SelectedIndex].MapIndex >= 0 && Entries[SelectedIndex].MapIndex <= Globals::RobotMapDefinitions.length() - 1) {
                with_MessageText->SetActive(true);
                Text = Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].GovTextStart;
                {
                    auto brightBlueColorTag = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString localizedText = aConst::LocalizedText(u"FormLoadRobot.PStar"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, std::move(localizedText), brightBlueColorTag.get());
                }
                {
                    auto brightBlueColorTag_2 = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString localizedText_2 = aConst::LocalizedText(u"FormLoadRobot.PPlanet"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, std::move(localizedText_2), brightBlueColorTag_2.get());
                }
                {
                    auto brightBlueColorTag_3 = pas::borrow(aMyFunction::BrightBlueColorTag);
                    pas::WideString localizedText_3 = aConst::LocalizedText(u"FormLoadRobot.PPlayer"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<Player>"_w, std::move(localizedText_3), brightBlueColorTag_3.get());
                }
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(1000), aMyFunction::BrightBlueColorTag);
                with_MessageText->SetText(Text);
                Text = Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].FromAuthor;
                if (Text != u"") {
                    with_MessageText->SetText(pas::concat_wide({with_MessageText->GetText(), u"\r\n", u" ", u"\r\n", Text}));
                }
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
        Success = false;
        {
            GI_GraphButton::TGraphButtonGI* Dif2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif2"sv));
            Dif2->SetDisabled(false);
            if (SelectedIndex >= 0 && SelectedIndex <= Entries.length() - 1 && Entries[SelectedIndex].MapIndex >= 0 && Entries[SelectedIndex].MapIndex <= Globals::RobotMapDefinitions.length() - 1) {
                if (Globals::RobotMapDefinitions[Entries[SelectedIndex].MapIndex].ReinforcementsDisabled) {
                    if (Dif2->Down) {
                        Dif2->SetDown(false);
                        Success = true;
                    }
                    Dif2->SetDisabled(true);
                }
            }
        }
        if (Success) {
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Dif1"sv))->SetDown(true);
        }
    }

    void TfLoadRobot::LoadCompletionData() {
        EC_Buf::TBufEC* Buffer{};
        std::int32_t I{};
        std::int32_t Count{};
        pas::WideString FileName{};
        CompletionData = nullptr;
        FileName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"robotcomplate.dat"});
        if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(FileName))) {
            try {
                Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                Buffer->LoadFromWideFilePath(FileName.pchar());
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

    void TfLoadRobot::SaveCompletionData() {
        std::int32_t I{};
        pas::AnsiString FileName{};
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
        EC_File::TFileEC* F = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        FileName = static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"robotcomplate.dat"}));
        F->SetFileName(static_cast<pas::WideString>(FileName));
        F->CreateNew();
        F->WriteBuffer(Buffer->Data, Buffer->DataSize);
        pas::free(F);
        pas::free(Buffer);
    }

    void TfLoadRobot::RecordCompletion(std::int32_t MapId, std::int32_t Score, std::int32_t Level) {
        std::int32_t Index{};
        std::int32_t Count = (CompletionData.length() - 1 + 1) / 2;
        if (MapId >= Count) {
            CompletionData.set_length(2 * (MapId + 1));
            {
                const std::int32_t cpp_first = 2 * Count;
                const std::int32_t cpp_last = CompletionData.length() - 1;
                if (cpp_first <= cpp_last) {
                    for (Index = cpp_first; Index <= cpp_last; ++Index) {
                        CompletionData[Index] = 0;
                    }
                }
            }
        }
        if (CompletionData[2 * MapId + 1] > Level) {
            return;
        }
        if (CompletionData[2 * MapId] == 0 || CompletionData[2 * MapId + 1] < Level) {
            CompletionData[2 * MapId] = Score;
        } else {
            CompletionData[2 * MapId] = std::min<std::int32_t>(CompletionData[2 * MapId], Score);
        }
        CompletionData[2 * MapId + 1] = Level;
    }

    std::int32_t TfLoadRobot::GetUnlockedAccess() {
        std::int32_t MapIndex{};
        std::int32_t Access{};
        std::int32_t Count{};
        std::int32_t Completed{};
        std::int32_t MapId{};
        std::int32_t Result = 0;
        std::int32_t I = 0;
        while (I <= Entries.length() - 1) {
            Access = Entries[I].Access;
            Count = 0;
            Completed = 0;
            while (I + Count <= Entries.length() - 1) {
                if (Entries[I + Count].Access != Access) {
                    break;
                }
                MapIndex = Entries[I + Count].MapIndex;
                if (MapIndex >= 0 && MapIndex <= Globals::RobotMapDefinitions.length() - 1) {
                    MapId = Globals::RobotMapDefinitions[MapIndex].Id;
                    if (MapId >= 0 && MapId < (CompletionData.length() - 1 + 1) / 2) {
                        if (MapId < (CompletionData.length() - 1 + 1) / 2 && CompletionData[MapId * 2 + 1] != 0) {
                            ++Completed;
                        }
                    }
                }
                ++Count;
            }
            Result = Access;
            if (Access > 0 && Completed < Count - 1) {
                break;
            }
            I += Count;
        }
        if (Result == 0) {
            return 1;
        }
        return Result;
    }

    // X completed, Y eligible; groups greater than -1 are eligible.
    WindowsSdk::TPoint TfLoadRobot::GetCompletionCounts() {
        WindowsSdk::TPoint Result{};
        std::int32_t I{};
        std::int32_t CompletionIndex{};
        LoadCompletionData();
        Result.X = 0;
        Result.Y = 0;
        {
            const std::int32_t cpp_last = Globals::RobotMapDefinitions.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (Globals::RobotMapDefinitions[I].Group > -1) {
                        ++Result.Y;
                    }
                    CompletionIndex = Globals::RobotMapDefinitions[I].Id;
                    if (CompletionIndex >= 0 && Globals::RobotMapDefinitions[I].Group > -1) {
                        if ((CompletionData.length() - 1 + 1) / 2 > CompletionIndex) {
                            if (CompletionData[CompletionIndex * 2 + 1] != 0) {
                                ++Result.X;
                            }
                        }
                    }
                }
            }
        }
        return Result;
    }

    // Menu summary; displayed total includes groups 0..2.
    pas::WideString TfLoadRobot::GetCompletionSummary() {
        WindowsSdk::TPoint Counts{};
        std::int32_t I{};
        Counts = GetCompletionCounts();
        std::int32_t Total = 0;
        {
            const std::int32_t cpp_last = Globals::RobotMapDefinitions.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (pas::in_set<0, 2>(Globals::RobotMapDefinitions[I].Group)) {
                        ++Total;
                    }
                }
            }
        }
        return static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Counts.X), "/", SysUtils::IntToStr(Total)}));
    }

    void TfLoadRobot::SelectMusic() {
    }

    void TfLoadRobot::p_destroy() {
        fLoadRobot::TfLoadRobot_Destroy(this);
    }

} // namespace fLoadRobot
