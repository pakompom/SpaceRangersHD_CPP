#include "layout/fJournal.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Edit.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fGov.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/fJournal.hpp"
#include "units/fPanelMain.hpp"

namespace fJournal {
    pas::Array<pas::WideString, 0, 19> TelevisionClipNames = pas::Array<pas::WideString, 0, 19>{{
        u"a"_w, u"b"_w, u"c"_w, u"d"_w, u"e"_w, u"f"_w, u"g"_w, u"h"_w,
        u"k"_w, u"l"_w, u"m"_w, u"n"_w, u"o"_w, u"p"_w, u"q"_w, u"r"_w,
        u"s"_w, u"t"_w, u"u"_w, u"v"_w,
    }};

    // The native sum loop tests index < 20; the selection loop stops before index 19.
    // Selection uses each weight plus one.
    pas::Array<std::int32_t, 0, 19> TelevisionClipWeights = pas::Array<std::int32_t, 0, 19>{{
        9, 5, 9, 9, 7, 5, 7, 1,
        3, 1, 0, 3, 3, 2, 3, 2,
        3, 4, 5, 3,
    }};

    std::uint8_t RunJournal(GI_MessageLoop::TMessageLoopGI* ParentLoop) {
        std::uint8_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        ParentLoop->RootUiObject->NativeHook50();
        ParentLoop->CaptureCursorState(&State);
        ParentLoop->SetCursorActive(false);
        ParentLoop->DrawQueuedUpdateRects();
        Globals::JournalScreen->ParentLoop = ParentLoop;
        ParentLoop->ChildLoop = Globals::JournalScreen;
        if (Globals::JournalScreen->Run() == 1) {
            Result = true;
        } else {
            Result = false;
        }
        Globals::JournalScreen->ParentLoop = nullptr;
        ParentLoop->ChildLoop = nullptr;
        ParentLoop->InvalidateViewport();
        ParentLoop->RestoreCursorState(&State);
        ParentLoop->UpdateCursorPosition();
        ParentLoop->RootUiObject->NativeHook48();
        ParentLoop->Present();
        GR_Main::PostMouseMoveMessage();
        return Result;
    }

    void TfJournal_Create(TfJournal* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->JournalSelected = true;
    }

    void TfJournal_Destroy(TfJournal* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfJournal::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fJournal... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* ButFormClose_Parent = MainPanel->FindByNameRecursive(u"ButFormClose"_wref.get())->Parent;
                ButFormClose_Parent->SetPosition(ClassesImports::Point(ButFormClose_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, ButFormClose_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        InfoPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelInfo"_wref.get()));
    }

    void TfJournal::OnOpen() {
        GR_Main::CaptureScreenBackground(true, 0);
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
        }
        MainPanel->OnOpen();
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Gal"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Quest"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Logo"_wref.get()))->SetHitTestDisabled(true);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->RefreshNewsAtLocation();
        }
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->KeyDownCallback = pas::bind_method<&TfJournal::MainPanelKeyDown>(this);
            MainPanel->KeyUpCallback = pas::bind_static_method<&TfJournal::MainPanelKeyUp>(this);
            MainPanel->LeftButtonDownCallback = pas::bind_method<&TfJournal::MainPanelMouseDown>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"AddRecord"_wref.get()))->UpCallback = pas::bind_method<&TfJournal::AddRecordClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButFormClose"_wref.get()))->UpCallback = pas::bind_method<&TfJournal::CloseClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClear"_wref.get()))->UpCallback = pas::bind_method<&TfJournal::ClearInputClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButCopy"_wref.get()))->UpCallback = pas::bind_method<&TfJournal::CopyInputClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPaste"_wref.get()))->UpCallback = pas::bind_method<&TfJournal::PasteInputClicked>(this);
        this->MainPanel->RebuildMessageButtons(false);
        {
            GI_GraphButton::TGraphButtonGI* ButJournal = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButJournal"_wref.get()));
            ButJournal->UpCallback = pas::bind_method<&TfJournal::ToggleJournalNews>(this);
            ButJournal->SetDisabled(JournalSelected);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButNews = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNews"_wref.get()));
            ButNews->UpCallback = pas::bind_method<&TfJournal::ToggleJournalNews>(this);
            ButNews->SetDisabled(static_cast<std::uint8_t>(JournalSelected ^ 1));
        }
        GetByName(u"PanelJournal"_wref.get())->SetActive(JournalSelected);
        std::uint8_t Reception = TfJournal::HasTelevisionReception();
        pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"TVNone"_wref.get()))->SetActive(static_cast<std::uint8_t>(Reception ^ 1));
        pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"TV"_wref.get()))->SetActive(Reception);
        RefreshTelevisionAnimation(nullptr);
        {
            GI_Edit::TEditGI* TextRecord = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextRecord"_wref.get()));
            TextRecord->AutoScrollText = true;
            TextRecord->ClearFocusOnEnter = false;
        }
        JournalSelected = static_cast<std::uint8_t>(JournalSelected ^ 1);
        ToggleJournalNews(nullptr);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(201);
    }

    void TfJournal::OnClose() {
        aGalaxy::Galaxy->CheckIntegrityChecksum(202);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        InfoPanel->FreeOwnedChildren();
        MainPanel->OnClose();
    }

    void TfJournal::RefreshTelevisionAnimation(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        std::int32_t Choice = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 19); cpp_range.next(I); ) {
            Choice += TelevisionClipWeights[I] + 1;
        }
        Choice = aMyFunction::RandomIntRange(0, Choice - 1);
        I = 0;
        while (I < 19) {
            Choice -= TelevisionClipWeights[I] + 1;
            if (Choice < 0) {
                break;
            }
            ++I;
        }
        {
            GI_GAI::TgaiGI* TV = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"TV"_wref.get()));
            if (JournalSelected) {
                TV->SetFirstFrameImagePath(pas::concat_wide({u"Bm.News.", GR_Main::GiResourceSuffix(), u"FindI"}));
                TV->SetImagePath(pas::concat_wide({u"Bm.News.", GR_Main::GiResourceSuffix(), u"FindA"}));
                {
                    const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"[75,0-", SysUtils::IntToStr(TV->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* cpp_arg_2 = TV;
                    cpp_arg_2->LoadFrameSequenceFromText(cpp_arg);
                }
            } else {
                TV->SetFirstFrameImagePath(pas::concat_wide({u"Bm.News.", GR_Main::GiResourceSuffix(), TelevisionClipNames[I], u"i"}));
                TV->SetImagePath(pas::concat_wide({u"Bm.News.", GR_Main::GiResourceSuffix(), TelevisionClipNames[I], u"a"}));
                {
                    const pas::WideString& cpp_arg_3 = static_cast<pas::WideString>(pas::concat_ansi({"[75,0-", SysUtils::IntToStr(TV->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* cpp_arg_4 = TV;
                    cpp_arg_4->LoadFrameSequenceFromText(cpp_arg_3);
                }
            }
            TV->UpdateAutoGeometry();
            TV->CycleCompleteCallback = pas::bind_method<&TfJournal::RefreshTelevisionAnimation>(this);
            if (TfJournal::HasTelevisionReception()) {
                TV->RestartPlayback();
            } else {
                TV->StopAutoPlayback();
            }
        }
    }

    void TfJournal::ClearEntries() {
        ContentHeight = 0;
        InfoPanel->FreeOwnedChildren();
        InfoPanel->SetScrollOffset(ClassesImports::Point(0, 0));
        InfoPanel->Invalidate();
    }

    void TfJournal::FinishEntries() {
        InfoPanel->UpdateScrollRanges();
        InfoPanel->VerticalScrollBar->SetRange(0, InfoPanel->VerticalScrollBar->Maximum);
        InfoPanel->VerticalScrollBar->SetActive(InfoPanel->ClientSize.Y < ContentHeight);
        {
            std::int32_t lineHeight = pas::checked_cast<GI_Label::TLabelGI*>(Globals::GovernmentScreen->GetByName(u"TalkText"_wref.get()))->GetLineHeight();
            GI_ScrollBar::TScrollBarGI* verticalScrollBar = InfoPanel->VerticalScrollBar;
            verticalScrollBar->SetSmallChange(lineHeight);
        }
        InfoPanel->VerticalScrollBar->SetLargeChange(InfoPanel->ClientSize.Y);
        InfoPanel->VerticalScrollBar->SetPageSize(InfoPanel->ClientSize.Y);
        InfoPanel->SetScrollOffset(ClassesImports::Point(0, 0));
        InfoPanel->Invalidate();
    }

    void TfJournal::AddEntrySpacing(std::int32_t Height) {
        ContentHeight += GR_Main::GiScalePixels(Height);
    }

    void TfJournal::AddEntryHeading(pas::WideString Text, pas::WideString MessageText, std::int32_t Compact, std::int32_t RecordIndex) {
        pas::WideString ButtonPrefix{};
        std::int32_t PinWidth{};
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,0,0>"_wref.get());
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=0,255,0>"_wref.get(), u"<color=255,255,0>"_wref.get());
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, InfoPanel);
        if (Compact == 0) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"CaptionL"}));
        } else {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"Caption"}));
        }
        Image->SetSize(Image->GetContentSize());
        if (Compact == 0) {
            Image->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X / 2 - Image->ClientSize.X / 2, ContentHeight));
        } else {
            Image->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X - Image->ClientSize.X, ContentHeight));
        }
        Image->SetDepth(1.0);
        Image->SetImageKindX(GI_Main::ikxLeftFill);
        Image->SetImageKindY(GI_Main::ikyCenter);
        ContentHeight += Image->ClientSize.Y;
        Image->SetPositionModeW(true);
        {
            GI_Label::TLabelGI* cpp_with = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
            if (GlobalsV::FontDialog == 0) {
                cpp_with->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                cpp_with->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                cpp_with->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                cpp_with->SetFontName(GlobalsV::SmoothIntroFontName);
            }
            cpp_with->SetDepth(-1.0);
            cpp_with->SetSize(Image->ClientSize);
            if (Compact != 0) {
                cpp_with->SetSize(ClassesImports::Point(cpp_with->ClientSize.X - GR_Main::GiScalePixels(20), cpp_with->ClientSize.Y));
            }
            cpp_with->SetPosition(ClassesImports::Point(Image->LocalPosition.X + Image->ClientSize.X - cpp_with->ClientSize.X, Image->LocalPosition.Y));
            cpp_with->SetWordWrapEnabled(false);
            cpp_with->SetPositionModeW(true);
            cpp_with->SetTextAlignX(GI_Main::taxCenter);
            cpp_with->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with->SetText(Text);
            cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        }
        if (MessageText != u"") {
            GI_GraphButton::TGraphButtonGI* cpp_with_2 = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, InfoPanel);
            cpp_with_2->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"MemN"}));
            cpp_with_2->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"MemA"}));
            cpp_with_2->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"MemD"}));
            cpp_with_2->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"MemH"}));
            cpp_with_2->SetSize(cpp_with_2->GetMaxStateImageSize());
            PinWidth = cpp_with_2->ClientSize.X;
            if (Compact == 0) {
                cpp_with_2->SetPosition(ClassesImports::Point(Image->LocalPosition.X + Image->ClientSize.X - cpp_with_2->ClientSize.X - GR_Main::GiScalePixels(10), Image->LocalPosition.Y));
            } else {
                cpp_with_2->SetPosition(ClassesImports::Point(Image->LocalPosition.X + Image->ClientSize.X - cpp_with_2->ClientSize.X - GR_Main::GiScalePixels(10), Image->LocalPosition.Y - GR_Main::GiScalePixels(5)));
            }
            cpp_with_2->SetKind(GI_GraphButton::gbkDisable);
            cpp_with_2->SetPositionModeW(true);
            cpp_with_2->HelpText = MessageText;
            cpp_with_2->UpCallback = pas::bind_method<&TfJournal::PinEntryClicked>(this);
            if (JournalSelected) {
                ButtonPrefix = u"JrnBtn"_w;
            } else {
                ButtonPrefix = u"News"_w;
            }
            cpp_with_2->SetName(pas::concat_wide({ButtonPrefix, pas::wide_int_to_str(RecordIndex)}));
            cpp_with_2->SetDown(false);
            cpp_with_2->SetDisabled(Globals::FindPlayerBubbleByText(MessageText, false) != nullptr);
            cpp_with_2->SetHovered(true);
            cpp_with_2->Invalidate();
            cpp_with_2->SetHovered(false);
            if (JournalSelected) {
                GI_GraphButton::TGraphButtonGI* cpp_with_3 = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, InfoPanel);
                cpp_with_3->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormCount2.", GR_Main::GiResourceSuffix(), u"SubN"}));
                cpp_with_3->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormCount2.", GR_Main::GiResourceSuffix(), u"SubA"}));
                cpp_with_3->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormCount2.", GR_Main::GiResourceSuffix(), u"SubD"}));
                cpp_with_3->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormCount2.", GR_Main::GiResourceSuffix(), u"SubH"}));
                cpp_with_3->SetSize(cpp_with_3->GetMaxStateImageSize());
                cpp_with_3->SetPosition(ClassesImports::Point(Image->LocalPosition.X + Image->ClientSize.X - cpp_with_3->ClientSize.X - GR_Main::GiScalePixels(15) - PinWidth, Image->LocalPosition.Y - GR_Main::GiScalePixels(3)));
                cpp_with_3->SetKind(GI_GraphButton::gbkDisable);
                cpp_with_3->SetPositionModeW(true);
                cpp_with_3->UpCallback = pas::bind_method<&TfJournal::DeleteEntryClicked>(this);
                cpp_with_3->SetName(static_cast<pas::WideString>(pas::concat_ansi({"JrnDel", SysUtils::IntToStr(RecordIndex)})));
                cpp_with_3->UserValue = RecordIndex;
                cpp_with_3->SetDown(false);
                cpp_with_3->SetHovered(true);
                cpp_with_3->Invalidate();
                cpp_with_3->SetHovered(false);
            }
        }
    }

    void TfJournal::AddEntryText(pas::WideString Text, GI_Main::TTextAlignXGI Align, pas::WideString FontName) {
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"_wref.get());
        {
            GI_Label::TLabelGI* cpp_with = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
            if (FontName == u"") {
                if (GlobalsV::FontDialog == 0) {
                    cpp_with->SetFontName(GlobalsV::NormalFontName);
                } else if (GlobalsV::FontDialog == 1) {
                    cpp_with->SetFontName(GlobalsV::SmoothBigFontName);
                } else if (GlobalsV::FontDialog == 2) {
                    cpp_with->SetFontName(GlobalsV::SmoothHugeFontName);
                } else if (GlobalsV::FontDialog >= 3) {
                    cpp_with->SetFontName(GlobalsV::SmoothIntroFontName);
                }
            } else {
                cpp_with->SetFontName(FontName);
            }
            cpp_with->SetPosition(ClassesImports::Point(0, ContentHeight));
            cpp_with->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X, 1));
            cpp_with->SetWordWrapEnabled(true);
            cpp_with->SetPositionModeW(true);
            if (JournalSelected) {
                cpp_with->SetAutoHeightPadding(8);
            }
            cpp_with->SetTextAlignX(Align);
            cpp_with->SetTextAlignY(GI_Main::tayAuto);
            cpp_with->SetText(Text);
            cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            ContentHeight += cpp_with->ClientSize.Y;
        }
    }

    void TfJournal::ToggleJournalNews(GI_MessageLoop::TObjectGI* Sender) {
        JournalSelected = static_cast<std::uint8_t>(JournalSelected ^ 1);
        RefreshTelevisionAnimation(nullptr);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButJournal"_wref.get()))->SetDisabled(JournalSelected);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNews"_wref.get()))->SetDisabled(static_cast<std::uint8_t>(JournalSelected ^ 1));
        GetByName(u"PanelJournal"_wref.get())->SetActive(JournalSelected);
        if (JournalSelected) {
            SetFocusedControl(GetByName(u"TextRecord"_wref.get()));
        } else {
            SetFocusedControl(nullptr);
        }
        if (!JournalSelected) {
            RebuildNewsEntries();
        } else {
            RebuildJournalEntries();
        }
    }

    void TfJournal::MainPanelMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        SetFocusedControl(GetByName(u"TextRecord"_wref.get()));
    }

    void TfJournal::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        std::int32_t Position{};
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
            if (Key == 'C') {
                CopyInputClicked(nullptr);
            } else if (Key == 'V') {
                PasteInputClicked(nullptr);
            } else if (Key == WindowsSdk::VK_BACK) {
                ClearInputClicked(nullptr);
            }
        }
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && Key == WindowsSdk::VK_DELETE) {
                ClearEntriesConfirmed();
            } else if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && Key == WindowsSdk::VK_INSERT) {
                ExportEntriesConfirmed();
            } else if (!GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                Position = InfoPanel->VerticalScrollBar->Position;
                if (Key == WindowsSdk::VK_PRIOR) {
                    Position -= InfoPanel->VerticalScrollBar->LargeChange;
                }
                if (Key == WindowsSdk::VK_NEXT) {
                    Position += InfoPanel->VerticalScrollBar->LargeChange;
                }
                if (Key == WindowsSdk::VK_UP) {
                    Position -= InfoPanel->VerticalScrollBar->SmallChange;
                }
                if (Key == WindowsSdk::VK_DOWN) {
                    Position += InfoPanel->VerticalScrollBar->SmallChange;
                }
                if (Position < InfoPanel->VerticalScrollBar->Minimum) {
                    Position = InfoPanel->VerticalScrollBar->Minimum;
                }
                if (Position > InfoPanel->VerticalScrollBar->Maximum) {
                    Position = InfoPanel->VerticalScrollBar->Maximum;
                }
                InfoPanel->VerticalScrollBar->SetPosition_2(Position);
                if (Key == WindowsSdk::VK_HOME) {
                    if (!JournalSelected) {
                        InfoPanel->SetScrollOffset(ClassesImports::Point(0, 0));
                    }
                } else if (Key == WindowsSdk::VK_RETURN && JournalSelected) {
                    AddRecordClicked(nullptr);
                } else if (Key == WindowsSdk::VK_ESCAPE || Key == WindowsSdk::VK_F1) {
                    CloseClicked(nullptr);
                }
            }
        }
    }

    void TfJournal::MainPanelKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
    }

    void TfJournal::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            InfoPanel->VerticalScrollBar->SetPosition_2(InfoPanel->VerticalScrollBar->Position - InfoPanel->VerticalScrollBar->SmallChange);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            InfoPanel->VerticalScrollBar->SetPosition_2(InfoPanel->VerticalScrollBar->Position + InfoPanel->VerticalScrollBar->SmallChange);
        }
    }

    std::uint8_t TfJournal::HasTelevisionReception() {
        return aPlayer::GetPlayer() != nullptr && (aPlayer::GetPlayer()->IsDockedToShip() || aPlayer::GetPlayer()->IsOnPlanet() && pas::in_set<0, 4, 7, 7>(aPlayer::GetPlayer()->CurrentPlanet->OwnerId));
    }

    void TfJournal::PinEntryClicked(GI_MessageLoop::TObjectGI* Sender) {
        GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
        Globals::AddOrUpdatePlayerBubble(7, aGalaxy::Galaxy->CurrentTurn, Sender->HelpText, u""_wref.get());
        MainPanel->RebuildMessageButtons(false);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Sender)->SetDisabled(true);
        GI_Main::BreakUiMessage();
    }

    void TfJournal::DeleteEntryClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Position{};
        if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormInfo.DelRecord"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
            if (aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->DeleteJournalRecord(Sender->UserValue);
            }
            Position = InfoPanel->VerticalScrollBar->Position;
            RebuildJournalEntries();
            if (Position < InfoPanel->VerticalScrollBar->Minimum) {
                Position = InfoPanel->VerticalScrollBar->Minimum;
            }
            if (Position > InfoPanel->VerticalScrollBar->Maximum) {
                Position = InfoPanel->VerticalScrollBar->Maximum;
            }
            InfoPanel->VerticalScrollBar->SetPosition_2(Position);
            GI_Main::BreakUiMessage();
        }
    }

    void TfJournal::ClearEntriesConfirmed() {
        if (aPlayer::GetPlayer() != nullptr) {
            if (JournalSelected) {
                if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormInfo.ClearRecord"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                    aPlayer::GetPlayer()->ClearJournal();
                    RebuildJournalEntries();
                    GI_Main::BreakUiMessage();
                }
            } else if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormInfo.ClearNews"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                aPlayer::GetPlayer()->TrimNewsEntries(0);
                RebuildNewsEntries();
                GI_Main::BreakUiMessage();
            }
        }
    }

    void TfJournal::ExportEntriesConfirmed() {
        pas::WideString FileName{};
        if (aPlayer::GetPlayer() != nullptr) {
            if (JournalSelected) {
                if (pas::list_count(aPlayer::GetPlayer()->JournalRecords) > 0) {
                    if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormInfo.ExtractRecord"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                        FileName = aPlayer::GetPlayer()->ExportJournal();
                        GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                            pas::WideString localizedText = aConst::LocalizedText(u"FormInfo.ExtractRecordDone"_wref.get());
                            pas::WideString fileName = FileName;
                            return aMyFunction::ReplaceColoredToken(std::move(localizedText), u"<FileName>"_w, std::move(fileName), u"<color=255,240,100>"_w);
                        }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
                        RebuildJournalEntries();
                        GI_Main::BreakUiMessage();
                    }
                }
            } else if (pas::list_count(aPlayer::GetPlayer()->NewsEntries) > 0) {
                if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormInfo.ExtractNews"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                    FileName = aPlayer::GetPlayer()->ExportNews();
                    GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                        pas::WideString localizedText_2 = aConst::LocalizedText(u"FormInfo.ExtractNewsDone"_wref.get());
                        pas::WideString fileName_2 = FileName;
                        return aMyFunction::ReplaceColoredToken(std::move(localizedText_2), u"<FileName>"_w, std::move(fileName_2), u"<color=255,240,100>"_w);
                    }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
                    RebuildNewsEntries();
                    GI_Main::BreakUiMessage();
                }
            }
        }
    }

    void TfJournal::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(255);
        }
    }

    void TfJournal::SelectMusic() {
        if (aPlayer::GetPlayer() == nullptr) {
            GR_Main::MusicManager->RequestFadeOut();
        } else if (aPlayer::GetPlayer()->IsOnPlanet() || aPlayer::GetPlayer()->IsDockedToShip()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (aPlayer::GetPlayer()->IsOnPlanet()) {
                if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
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
            }
        } else if (GlobalsV::MusicInSpaceEnabled) {
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 20) {
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

    void TfJournal::RebuildNewsEntries() {
        std::int32_t I{};
        aGalaxy::PPlanetNewsEntry Entry{};
        // Native appends this zero-initialized, otherwise unassigned local.
        pas::WideString HeadingSuffix{};
        ClearEntries();
        AddEntrySpacing(10);
        std::int32_t Count = 0;
        {
            const std::int32_t cpp_first = pas::list_count(aPlayer::GetPlayer()->NewsEntries) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    ++Count;
                    if (Count > GlobalsV::MaxPlayerNews) {
                        break;
                    }
                    Entry = pas::list_at<aGalaxyStruct::TPlanetNews>(aPlayer::GetPlayer()->NewsEntries, I);
                    {
                        pas::WideString cpp_arg = pas::concat_wide({aMyFunction::WrapTextInColor(aGalaxy::Galaxy->FormatTurnDate(Entry->Turn), u"<color=255,240,100>"_w), u"\r\n", u" ", u"\r\n", Entry->Text});
                        pas::WideString wrapTextInColor = aMyFunction::WrapTextInColor(pas::concat_wide({aGalaxy::Galaxy->FormatTurnDate(Entry->Turn), HeadingSuffix}), u"<color=255,240,100>"_w);
                        AddEntryHeading(std::move(wrapTextInColor), std::move(cpp_arg), 1, 0);
                    }
                    AddEntryText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                    AddEntryText(Entry->Text, GI_Main::taxAuto, pas::WideString());
                    AddEntryText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                    AddEntrySpacing(10);
                }
            }
        }
        FinishEntries();
    }

    void TfJournal::RebuildJournalEntries() {
        std::int32_t I{};
        aPlayer::TJournalRecord* Entry{};
        ClearEntries();
        AddEntrySpacing(10);
        {
            auto normalBoldFontName = pas::borrow(GlobalsV::NormalBoldFontName);
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormInfo.RecordTitle"_wref.get());
            AddEntryText(std::move(localizedColorText), GI_Main::taxCenter, normalBoldFontName.get());
        }
        AddEntrySpacing(10);
        std::int32_t Displayed = 0;
        std::int32_t Count = pas::list_count(aPlayer::GetPlayer()->JournalRecords);
        {
            const std::int32_t cpp_first = Count - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Entry = pas::list_at<aPlayer::TJournalRecord>(aPlayer::GetPlayer()->JournalRecords, I);
                    {
                        pas::WideString cpp_arg = pas::concat_wide({aMyFunction::WrapTextInColor(aGalaxy::Galaxy->FormatTurnDate(Entry->DateTurn), u"<color=255,240,100>"_w), u"\r\n", u" ", u"\r\n", Entry->Text});
                        pas::WideString wrapTextInColor = aMyFunction::WrapTextInColor(aGalaxy::Galaxy->FormatTurnDate(Entry->DateTurn), u"<color=255,240,100>"_w);
                        AddEntryHeading(std::move(wrapTextInColor), std::move(cpp_arg), 1, I);
                    }
                    AddEntryText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                    AddEntryText(Entry->Text, GI_Main::taxAuto, pas::WideString());
                    AddEntryText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                    AddEntrySpacing(10);
                    ++Displayed;
                }
            }
        }
        if (Displayed > 0) {
            auto smallFontName = pas::borrow(GlobalsV::SmallFontName);
            pas::WideString formatText1 = ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(Displayed);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormInfo.RecordCount"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=0,50,200>"_w, u"<Count>"_w, std::move(intToStr));
            }());
            AddEntryText(std::move(formatText1), GI_Main::taxCenter, smallFontName.get());
        }
        for (I = 1; I <= 12; ++I) {
            AddEntryText(u" ."_w, GI_Main::taxCenter, pas::WideString());
        }
        FinishEntries();
    }

    void TfJournal::AddRecordClicked(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* TextRecord = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextRecord"_wref.get()));
        if (SysUtilsImports::Trim(static_cast<pas::AnsiString>(TextRecord->Text)).length() > 0) {
            aPlayer::GetPlayer()->AddJournalRecord(TextRecord->Text);
            GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
            TextRecord->SetText(pas::WideString());
            RebuildJournalEntries();
        } else {
            GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
        }
    }

    void TfJournal::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        RequestClose(2);
    }

    void TfJournal::ClearInputClicked(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* TextRecord = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextRecord"_wref.get()));
        TextRecord->SetText(pas::WideString());
    }

    void TfJournal::CopyInputClicked(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* TextRecord = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextRecord"_wref.get()));
        GR_Main::SetClipboardWideText(TextRecord->Text);
    }

    void TfJournal::PasteInputClicked(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* TextRecord = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextRecord"_wref.get()));
        TextRecord->SetText(GR_Main::GetClipboardWideText());
    }

    void TfJournal::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10001);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20001);
        }
    }

    void TfJournal::p_destroy() {
        fJournal::TfJournal_Destroy(this);
    }

} // namespace fJournal
