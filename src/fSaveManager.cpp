#include "layout/fSaveManager.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/System.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fMainForm.hpp"
#include "types/fPanelLoad.hpp"
#include "types/fShip2.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Edit.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Sound.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/Windows.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/fSaveManager.hpp"

namespace fSaveManager {
    void InsertScannedSaveSlotByTime(TfSaveManager* Self, PSMSlot& Slot);

    pas::WideString AutoSaveFileName = u"AutoSave.sav"_w;

    pas::Array<pas::WideString, 1, 3> QuickSaveFileNames = pas::Array<pas::WideString, 1, 3>{{u"QuickSave.sav"_w, u"QuickSave2.sav"_w, u"QuickSave3.sav"_w}};

    pas::WideString TurnSaveFileName = u"TurnSave.sav"_w;

    std::uint32_t NewSaveNormalColor{};

    std::uint32_t NewSaveSelectedColor{};

    std::uint32_t SaveSlotNormalColor{};

    std::uint32_t SaveSlotSelectedColor{};

    void TfSaveManager_Create(TfSaveManager* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->Slots = pas::make_object<pas::List>();
        Self->SelectedSlot = -1;
        Self->PreviewSound = pas::construct_call<GR_Sound::TSoundBufferControl>(GR_Sound::TSoundBufferControl_Create);
        Self->PreviewSound->Configure(u"Sound.SaveLoadLoop"_wref.get(), 0, true);
    }

    void TfSaveManager_Destroy(TfSaveManager* Self) {
        pas::free(Self->Slots);
        pas::free(Self->PreviewSound);
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfSaveManager::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fSaveManager... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GetByName(u""_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GetByName(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        {
            GI_MessageLoop::TObjectGI* ButClose_Parent = GetByName(u"ButClose"_wref.get())->Parent;
            ButClose_Parent->SetPosition(ClassesImports::Point(ButClose_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, ButClose_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
        }
        NewSaveNormalColor = GR_Main::GetStyleColorGI(u"SaveManager.NewSaveNormal"_w, 0, 41, 65);
        NewSaveSelectedColor = GR_Main::GetStyleColorGI(u"SaveManager.NewSaveSelected"_w, 87, 149, 175);
        SaveSlotNormalColor = GR_Main::GetStyleColorGI(u"SaveManager.SlotNormal"_w, 97, 129, 143);
        SaveSlotSelectedColor = GR_Main::GetStyleColorGI(u"SaveManager.SlotSelected"_w, 65, 121, 145);
        GR_Main::AppendLogLineThreadSafe("ok"_a);
    }

    void TfSaveManager::OnOpen() {
        pas::AnsiString Directory{};
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        Directory = static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Save"}));
        if (!SysUtilsImports::DirectoryExists(Directory)) {
            SysUtilsImports::CreateDir(Directory);
        }
        if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(true, 0);
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        {
            GI_GAI::TgaiGI* Anim = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"Anim"_wref.get()));
            Anim->RestartPlayback();
        }
        SelectedSlot = -1;
        if (Globals::SaveManagerMode == smmSave) {
            SelectedSlot = 0;
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClose"_wref.get()))->UpCallback = pas::bind_method<&TfSaveManager::CloseClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButCancel"_wref.get()))->UpCallback = pas::bind_method<&TfSaveManager::CloseClicked>(this);
        {
            GI_GraphButton::TGraphButtonGI* ButLoad = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButLoad"_wref.get()));
            ButLoad->SetActive(Globals::SaveManagerMode == smmLoad);
            ButLoad->UpCallback = pas::bind_method<&TfSaveManager::LoadClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButSave = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButSave"_wref.get()));
            ButSave->SetActive(Globals::SaveManagerMode == smmSave);
            ButSave->UpCallback = pas::bind_method<&TfSaveManager::SaveClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButDelete = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButDelete"_wref.get()));
            ButDelete->UpCallback = pas::bind_method<&TfSaveManager::DeleteClicked>(this);
            ButDelete->SetDisabled(true);
            ButDelete->SetActive(true);
        }
        {
            GI_GraphBuf::TGraphBufGI* GameImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage"_wref.get()));
            GameImage->GraphBuf->Clear();
            GameImage->Invalidate();
        }
        {
            GI_GraphBuf::TGraphBufGI* GameImage2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage2"_wref.get()));
            GameImage2->GraphBuf->Clear();
            GameImage2->Invalidate();
        }
        GetByName(u"CaptionLoad"_wref.get())->SetActive(Globals::SaveManagerMode == smmLoad);
        GetByName(u"CaptionSave"_wref.get())->SetActive(Globals::SaveManagerMode == smmSave);
        if (aSaveLoad::SaveWriter != nullptr && aSaveLoad::SaveWriter->IsRunning()) {
            aSaveLoad::SaveWriter->WaitForIdle(0xffffffffu);
        }
        RebuildSlotControls();
        if (pas::list_count(Slots) <= 0) {
            PreviewSound->SetVolume(1.0f);
        }
        Closing = false;
    }

    void TfSaveManager::OnClose() {
        std::int32_t I{};
        PSMSlot Slot{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Slots) - 1); cpp_range.next(I); ) {
            Slot = pas::list_at<TSMSlot>(Slots, I);
            Slot->FileName = pas::WideString();
            pas::dispose(Slot);
        }
        pas::list_clear(Slots);
        PreviewSound->SetVolume(0.0f);
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CampaignFlag183 = 0;
        }
        if (PreviewTimer != nullptr) {
            CancelCallbackTimer(PreviewTimer);
            PreviewTimer = nullptr;
        }
        GI_PanelScrollBar::TPanelScrollBarGI* ScrollPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
        ScrollPanel->FreeOwnedChildren();
        {
            GI_GraphBuf::TGraphBufGI* GameImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage"_wref.get()));
            GameImage->GraphBuf->Clear();
        }
        {
            GI_GraphBuf::TGraphBufGI* GameImage2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage2"_wref.get()));
            GameImage2->GraphBuf->Clear();
        }
        GR_Main::AuxRenderBuffer->Clear();
        GI_MessageLoop::TMessageLoopGI::OnClose();
        Closing = false;
    }

    void TfSaveManager::RebuildSlotControls() {
        GI_Panel::TPanelGI* Panel{};
        std::int32_t I{};
        ScanSaveFiles();
        if (SelectedSlot < 0) {
            SelectedSlot = FindNewestSlot();
        }
        if (([&] {
            std::int32_t cpp_left = FindAutoSaveSlot();
            return cpp_left == SelectedSlot;
        }()) && Globals::SaveManagerMode != smmLoad) {
            SelectedSlot = 0;
        }
        if (pas::list_count(Slots) <= SelectedSlot) {
            SelectedSlot = pas::list_count(Slots) - 1;
        }
        GI_PanelScrollBar::TPanelScrollBarGI* ScrollPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
        ScrollPanel->KeyDownCallback = pas::bind_method<&TfSaveManager::SlotKeyDown>(this);
        ScrollPanel->FreeOwnedChildren();
        GetByName(u"PanelAutoSave"_wref.get())->FreeOwnedChildren();
        std::int32_t Y = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Slots) - 1); cpp_range.next(I); ) {
            std::int32_t cpp_left_2 = FindAutoSaveSlot();
            if (cpp_left_2 != I) {
                Panel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, ScrollPanel);
                Panel->UserValue = I;
                Panel->SetPosition(ClassesImports::Point(0, Y));
                InitializeSlotPanel(Panel);
                Y += Panel->ClientSize.Y;
                Panel->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I)})));
                Panel->SetPositionModeW(true);
                RefreshSlot(I, false);
                ScrollPanel->VerticalScrollBar->SetSmallChange(Panel->ClientSize.Y);
            }
        }
        if (FindAutoSaveSlot() >= 0) {
            Panel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, GetByName(u"PanelAutoSave"_wref.get()));
            Panel->UserValue = FindAutoSaveSlot();
            Panel->SetPosition(ClassesImports::Point(0, 0));
            InitializeSlotPanel(Panel);
            Panel->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(FindAutoSaveSlot())})));
            Panel->SetPositionModeW(false);
            {
                std::int32_t findAutoSaveSlot = FindAutoSaveSlot();
                TfSaveManager* self = this;
                self->RefreshSlot(findAutoSaveSlot, false);
            }
            GetByName(u"PanelAutoSave"_wref.get())->SetSize(Panel->ClientSize);
            ScrollPanel->VerticalScrollBar->SetSmallChange(Panel->ClientSize.Y);
        }
        ScrollPanel->UpdateScrollRanges();
        ScrollPanel->VerticalScrollBar->SetActive(ScrollPanel->ClientSize.Y < Y);
        ScrollPanel->VerticalScrollBar->SetLargeChange(ScrollPanel->ClientSize.Y);
        ScrollPanel->VerticalScrollBar->SetPageSize(ScrollPanel->ClientSize.Y);
        I = ScrollPanel->VerticalScrollBar->Position;
        ScrollPanel->VerticalScrollBar->SetPosition_2(I - 1);
        ScrollPanel->VerticalScrollBar->SetPosition_2(I);
        if (SelectedSlot < 0 || pas::list_count(Slots) <= SelectedSlot) {
            SelectedSlot = -1;
        }
        SelectSlot(SelectedSlot);
        FinishPreviewDelay(nullptr, 0);
    }

    void TfSaveManager::InitializeSlotPanel(GI_Panel::TPanelGI* Panel) {
        Panel->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue))})));
        Panel->MouseEnterCallback = pas::bind_method<&TfSaveManager::SlotMouseEnter>(this);
        Panel->MouseLeaveCallback = pas::bind_method<&TfSaveManager::SlotMouseLeave>(this);
        Panel->LeftButtonDownCallback = pas::bind_method<&TfSaveManager::SlotMouseDown>(this);
        if (Globals::SaveManagerMode == smmLoad || ([&] {
            std::int32_t cpp_left = FindAutoSaveSlot();
            return cpp_left != Panel->UserValue;
        }())) {
            Panel->LeftButtonDoubleClickCallback = pas::bind_method<&TfSaveManager::SlotDoubleClick>(this);
        }
        {
            GI_Image::TImageGI* cpp_with = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            cpp_with->SetPosition(ClassesImports::Point(0, 0));
            cpp_with->SetDepth(1.0E+1);
            cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormSave2.", GR_Main::GiResourceSuffix(), u"Glow"}));
            cpp_with->SetSize(cpp_with->GetContentSize());
            Panel->SetSize(cpp_with->ClientSize);
            cpp_with->SetActive(false);
            cpp_with->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "onmouse"})));
        }
        {
            GI_Image::TImageGI* cpp_with_2 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            if (GR_Main::GiResourceVariant() == 1) {
                cpp_with_2->SetPosition(ClassesImports::Point(51, 2));
            } else {
                cpp_with_2->SetPosition(ClassesImports::Point(64, 3));
            }
            cpp_with_2->SetDepth(9.0);
            cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormSave2.", GR_Main::GiResourceSuffix(), u"SlotN"}));
            cpp_with_2->SetSize(cpp_with_2->GetContentSize());
            cpp_with_2->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "BG"})));
        }
        {
            GI_Image::TImageGI* cpp_with_3 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            if (GR_Main::GiResourceVariant() == 1) {
                cpp_with_3->SetPosition(ClassesImports::Point(2, 2));
                cpp_with_3->SetSize(ClassesImports::Point(49, 49));
            } else {
                cpp_with_3->SetPosition(ClassesImports::Point(3, 3));
                cpp_with_3->SetSize(ClassesImports::Point(61, 61));
            }
            cpp_with_3->SetDepth(9.0);
            cpp_with_3->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "Emblem"})));
        }
        if (Globals::SaveManagerMode == smmLoad || ([&] {
            std::int32_t cpp_left_2 = FindAutoSaveSlot();
            return cpp_left_2 == Panel->UserValue;
        }())) {
            GI_Label::TLabelGI* cpp_with_4 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            cpp_with_4->SetPosition(ClassesImports::Point(81, 14));
            cpp_with_4->SetSize(ClassesImports::Point(409, 20));
            cpp_with_4->SetDepth(7.0);
            if (GlobalsV::FontDialog == 0) {
                cpp_with_4->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                cpp_with_4->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                cpp_with_4->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                cpp_with_4->SetFontName(GlobalsV::SmoothIntroFontName);
            }
            cpp_with_4->SetTextAlignX(GI_Main::taxLeft);
            cpp_with_4->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with_4->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "Edit"})));
            if (IsSlotEmpty(Panel->UserValue)) {
                cpp_with_4->SetText(u""_wref.get());
            } else {
                cpp_with_4->SetText(pas::list_at<TSMSlot>(Slots, Panel->UserValue)->DisplayName);
            }
            cpp_with_4->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        } else {
            GI_Edit::TEditGI* cpp_with_5 = pas::construct_call<GI_Edit::TEditGI>(GI_Edit::TEditGI_Create, Panel);
            cpp_with_5->SetPosition(ClassesImports::Point(81, 14));
            cpp_with_5->SetSize(ClassesImports::Point(409, 20));
            cpp_with_5->SetDepth(7.0);
            if (GlobalsV::FontDialog == 0) {
                cpp_with_5->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                cpp_with_5->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                cpp_with_5->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                cpp_with_5->SetFontName(GlobalsV::SmoothIntroFontName);
            }
            cpp_with_5->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "Edit"})));
            cpp_with_5->MaxLength = 55;
            if (IsSlotEmpty(Panel->UserValue)) {
                cpp_with_5->SetText(pas::WideString());
            } else {
                cpp_with_5->SetText(pas::list_at<TSMSlot>(Slots, Panel->UserValue)->DisplayName);
            }
            cpp_with_5->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            cpp_with_5->AcceptCharCallback = pas::bind_static_method<&TfSaveManager::AcceptSaveNameCharacter>(this);
        }
        {
            GI_Label::TLabelGI* cpp_with_6 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            cpp_with_6->SetPosition(ClassesImports::Point(72, 37));
            cpp_with_6->SetSize(ClassesImports::Point(130, 17));
            cpp_with_6->SetDepth(8.0);
            cpp_with_6->SetFontName(GlobalsV::RangerFontName);
            cpp_with_6->SetTextAlignX(GI_Main::taxCenter);
            cpp_with_6->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with_6->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "Captain"})));
        }
        {
            GI_Label::TLabelGI* cpp_with_7 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            cpp_with_7->SetPosition(ClassesImports::Point(207, 37));
            cpp_with_7->SetSize(ClassesImports::Point(165, 17));
            cpp_with_7->SetDepth(8.0);
            cpp_with_7->SetFontName(GlobalsV::RangerFontName);
            cpp_with_7->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(227, 227, 227));
            cpp_with_7->SetTextAlignX(GI_Main::taxCenter);
            cpp_with_7->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with_7->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "Turn"})));
        }
        {
            GI_Label::TLabelGI* cpp_with_8 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            cpp_with_8->SetPosition(ClassesImports::Point(387, 37));
            cpp_with_8->SetSize(ClassesImports::Point(108, 17));
            cpp_with_8->SetDepth(8.0);
            cpp_with_8->SetFontName(GlobalsV::RangerFontName);
            cpp_with_8->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(227, 227, 227));
            cpp_with_8->SetTextAlignX(GI_Main::taxCenter);
            cpp_with_8->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with_8->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "Money"})));
        }
        {
            GI_Label::TLabelGI* cpp_with_9 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            cpp_with_9->SetPosition(ClassesImports::Point(297, 7));
            cpp_with_9->SetSize(ClassesImports::Point(200, 15));
            cpp_with_9->SetDepth(8.0);
            cpp_with_9->SetFontName(GlobalsV::MiniFontName);
            cpp_with_9->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(227, 227, 227));
            cpp_with_9->SetTextAlignX(GI_Main::taxRight);
            cpp_with_9->SetTextAlignY(GI_Main::tayCenterEx);
            cpp_with_9->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Panel->UserValue)), "Date"})));
        }
    }

    void TfSaveManager::RefreshSlot(std::int32_t SlotIndex, std::uint8_t UnusedEditingFlag) {
        pas::WideString State{};
        std::uint32_t Color{};
        Windows::TSystemTime Time{};
        try {
            if (SelectedSlot == SlotIndex) {
                State = u"D"_w;
            } else {
                State = u"N"_w;
            }
            {
                GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "BG"}))));
                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormSave2.2Slot", State}));
            }
            {
                GI_Image::TImageGI* cpp_with_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Emblem"}))));
                cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormSave2.2", pas::list_at<TSMSlot>(Slots, SlotIndex)->RaceName, State}));
                cpp_with_2->SetImageKindX(GI_Main::ikxCenter);
                cpp_with_2->SetImageKindY(GI_Main::ikyCenter);
                cpp_with_2->SetActive(true);
            }
            {
                GI_Label::TLabelGI* cpp_with_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Date"}))));
                if (IsSlotEmpty(SlotIndex)) {
                    cpp_with_3->SetText(aConst::LocalizedText(u"FormSaveManager.New"_wref.get()));
                } else {
                    WindowsSdk::FileTimeToSystemTime(pas::ConstRef<Windows::TFileTime>(&pas::list_at<TSMSlot>(Slots, SlotIndex)->LocalWriteTime), Time);
                    cpp_with_3->SetText(static_cast<pas::WideString>(([&] {
                        System::TDateTime systemTimeToDateTime = SysUtilsImports::SystemTimeToDateTime(Time);
                        const pas::AnsiString& localizedText = static_cast<pas::AnsiString>(aConst::LocalizedText(u"FormSaveManager.DateFormatStr"_wref.get()));
                        return SysUtilsImports::FormatDateTime(localizedText, systemTimeToDateTime);
                    }())));
                }
                if (Globals::SaveManagerMode == smmSave && SlotIndex == 0) {
                    if (SelectedSlot == SlotIndex) {
                        cpp_with_3->SetTextColor(NewSaveSelectedColor);
                    } else {
                        cpp_with_3->SetTextColor(NewSaveNormalColor);
                    }
                } else if (SelectedSlot == SlotIndex) {
                    cpp_with_3->SetTextColor(SaveSlotSelectedColor);
                } else {
                    cpp_with_3->SetTextColor(SaveSlotNormalColor);
                }
            }
            if (SelectedSlot == SlotIndex) {
                Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
            } else {
                Color = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0);
            }
            if (pas::class_cast_if<GI_Edit::TEditGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Edit"})))) != nullptr) {
                GI_Edit::TEditGI* cpp_with_4 = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Edit"}))));
                cpp_with_4->SetText(pas::list_at<TSMSlot>(Slots, SlotIndex)->DisplayName);
                cpp_with_4->SetTextColor(Color);
            } else {
                GI_Label::TLabelGI* cpp_with_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Edit"}))));
                cpp_with_5->SetText(pas::list_at<TSMSlot>(Slots, SlotIndex)->DisplayName);
                cpp_with_5->SetTextColor(Color);
            }
            if (IsSlotEmpty(SlotIndex)) {
                if (SelectedSlot == SlotIndex) {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Captain"}))))->SetText(aPlayer::GetPlayer()->Name);
                    {
                        const pas::WideString& formatGameTurnDate = aGalaxy::FormatGameTurnDate(aGalaxy::Galaxy->CurrentTurn);
                        GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Turn"}))));
                        cpp_arg->SetText(formatGameTurnDate);
                    }
                    {
                        const pas::WideString& intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->Money);
                        GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Money"}))));
                        cpp_arg_2->SetText(intToStr);
                    }
                } else {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Captain"}))))->SetText(u""_wref.get());
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Turn"}))))->SetText(u""_wref.get());
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Money"}))))->SetText(u""_wref.get());
                }
            } else {
                {
                    auto pilotName = pas::borrow(pas::list_at<TSMSlot>(Slots, SlotIndex)->PilotName);
                    GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Captain"}))));
                    cpp_arg_3->SetText(pilotName.get());
                }
                {
                    const pas::WideString& formatGameTurnDate_2 = aGalaxy::FormatGameTurnDate(pas::list_at<TSMSlot>(Slots, SlotIndex)->Turn);
                    GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Turn"}))));
                    cpp_arg_4->SetText(formatGameTurnDate_2);
                }
                {
                    const pas::WideString& intToStr_2 = pas::wide_int_to_str(pas::list_at<TSMSlot>(Slots, SlotIndex)->Money);
                    GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Money"}))));
                    cpp_arg_5->SetText(intToStr_2);
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error when loading info from save file ", pas::list_at<TSMSlot>(Slots, SlotIndex)->FileName})));
            } else {
                throw;
            }
        }
    }

    void TfSaveManager::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::RequestedScreenId = GlobalsV::SaveManagerReturnScreenId;
        Closing = true;
        RequestClose(1);
    }

    void TfSaveManager::LoadClicked(GI_MessageLoop::TObjectGI* Sender) {
        GR_DX::ReleaseAllTextureSurfaces();
        Globals::MainMenuScreen->LoadPanel->SelectBackgroundStyle(0);
        if (SelectedSlot >= 0) {
            if (static_cast<std::uint8_t>(IsSlotEmpty(SelectedSlot) ^ 1) && ([&] {
                std::int32_t cpp_left = TfSaveManager::ReadSaveVersion(pas::list_at<TSMSlot>(Slots, SelectedSlot)->FileName);
                return cpp_left < aConst::MinimumLoadableSaveVersion;
            }())) {
                const pas::WideString& paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormSaveManager.LoadError"_wref.get());
                GI_MessageLoop::TMessageLoopGI* registeredScreenLoop = reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::GetRegisteredScreenLoop(GlobalsV::CurrentScreenId));
                GI_MessageBox::ShowMessageBoxGI(registeredScreenLoop, paramByPathOrMarker, GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
            } else if (!IsSlotEmpty(SelectedSlot)) {
                GlobalsV::PendingLoadFileName = static_cast<pas::AnsiString>(pas::list_at<TSMSlot>(Slots, SelectedSlot)->FileName);
                GR_Main::EditableSaveFileName = TfSaveManager::GetSaveConfigPath(static_cast<pas::WideString>(GlobalsV::PendingLoadFileName));
                if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(GR_Main::EditableSaveFileName))) {
                    if (GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"FormSaveManager.LoadDumpConfirm"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                        GR_Main::EditableSaveBlock->Clear();
                        GR_Main::EditableSaveBlock->LoadFromTextFileWithEncodingProbe(GR_Main::EditableSaveFileName.pchar(), true);
                        GR_Main::ApplyEditableSaveOnLoad = true;
                    }
                }
                Globals::ShipScreen->SelectedHoldKind = fShip2::phkEmpty;
                Globals::ShipScreen->SelectedHoldItem = nullptr;
                GlobalsV::RequestedScreenId = GlobalsV::screenGameLoad;
                RequestClose(1);
            }
        }
    }

    void TfSaveManager::SaveClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::uint8_t Saved{};
        pas::WideString FileName{};
        pas::WideString Description{};
        std::int32_t SuffixIndex{};
        if (SelectedSlot >= 0) {
            if (!IsSlotEmpty(SelectedSlot)) {
                SysUtilsImports::DeleteFile(static_cast<pas::AnsiString>(pas::list_at<TSMSlot>(Slots, SelectedSlot)->FileName));
                pas::list_at<TSMSlot>(Slots, SelectedSlot)->FileName = pas::WideString();
            }
            FileName = EC_Str::TrimWideString(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SelectedSlot), "Edit"}))))->Text);
            if (FileName == u"") {
                SetFocusedControl(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SelectedSlot), "Edit"}))));
            } else {
                Description = FileName;
                FileName = EC_Str::RemoveWideStringChars(FileName, u"<>\"/\\:"_w);
                if (GlobalsV::RunningUnderWine || GR_Main::UserSettingsConfig->CountParams(u"TransliterateSaveNames"_wref.get()) > 0 && GI_Main::ParseEnabledNameGI(EC_Str::TrimWideString(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"TransliterateSaveNames"_wref.get())))) {
                    FileName = EC_Str::TransliterateCyrillicToLatin(FileName);
                }
                FileName = ([&] {
                    const pas::WideString& cpp_arg = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"save\\", FileName, u".sav"});
                    TfSaveManager* self = this;
                    return self->BuildUniqueSavePath(cpp_arg, SuffixIndex);
                }());
                if (SuffixIndex > 0) {
                    Description = pas::concat_wide({Description, u" (", pas::wide_int_to_str(SuffixIndex), u")"});
                }
                Saved = aSaveLoad::SaveGameToFile(FileName, Description);
                if (!Saved) {
                    const pas::WideString& paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormSaveManager.SaveError"_wref.get());
                    GI_MessageLoop::TMessageLoopGI* self_2 = this;
                    GI_MessageBox::ShowMessageBoxGI(self_2, paramByPathOrMarker, GI_MessageBox::mbgOK | GI_MessageBox::mbgError, 0, 0, 0);
                }
                CloseClicked(Sender);
            }
        }
    }

    void TfSaveManager::DeleteClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::uint8_t WasAutoSave{};
        if (SelectedSlot >= 0 && static_cast<std::uint8_t>(IsSlotEmpty(SelectedSlot) ^ 1)) {
            if (([&] {
                const pas::WideString& paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormSaveManager.QueryDelete"_wref.get());
                GI_MessageLoop::TMessageLoopGI* self = this;
                return GI_MessageBox::ShowMessageBoxGI(self, paramByPathOrMarker, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0);
            }()) == GI_MessageBox::mbgResultOK) {
                if (([&] {
                    std::int32_t cpp_left = FindAutoSaveSlot();
                    return cpp_left == SelectedSlot;
                }()) && Globals::SaveManagerMode == smmLoad) {
                    WasAutoSave = true;
                } else {
                    WasAutoSave = false;
                }
                SysUtilsImports::DeleteFile(static_cast<pas::AnsiString>(pas::list_at<TSMSlot>(Slots, SelectedSlot)->FileName));
                if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(TfSaveManager::GetSaveConfigPath(pas::list_at<TSMSlot>(Slots, SelectedSlot)->FileName)))) {
                    SysUtilsImports::DeleteFile(static_cast<pas::AnsiString>(TfSaveManager::GetSaveConfigPath(pas::list_at<TSMSlot>(Slots, SelectedSlot)->FileName)));
                }
                if (WasAutoSave) {
                    SelectedSlot = 0;
                }
                RebuildSlotControls();
            }
        }
    }

    void TfSaveManager::SlotMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (static_cast<std::uint8_t>(Closing ^ 1) && (Globals::SaveManagerMode == smmLoad || ([&] {
            std::int32_t cpp_left = FindAutoSaveSlot();
            return cpp_left != Sender->UserValue;
        }()))) {
            GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
            SelectSlot(Sender->UserValue);
        }
    }

    void TfSaveManager::SlotDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Globals::SaveManagerMode == smmSave) {
            SaveClicked(Sender);
        } else {
            LoadClicked(Sender);
        }
    }

    void TfSaveManager::SlotKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_DOWN) {
            if (SelectedSlot < 0) {
                SelectSlot(0);
            } else if ((Globals::SaveManagerMode == smmLoad || FindAutoSaveSlot() < 0) && pas::list_count(Slots) - 1 <= SelectedSlot) {
                SelectSlot(0);
            } else if (Globals::SaveManagerMode != smmLoad && FindAutoSaveSlot() >= 0 && pas::list_count(Slots) - 2 <= SelectedSlot) {
                SelectSlot(0);
            } else {
                SelectSlot(SelectedSlot + 1);
            }
        } else if (Key == WindowsSdk::VK_UP) {
            if (SelectedSlot < 0) {
                SelectSlot(pas::list_count(Slots) - 1);
            } else if (SelectedSlot == 0) {
                if (Globals::SaveManagerMode == smmLoad || FindAutoSaveSlot() < 0) {
                    if (pas::list_count(Slots) - 1 != SelectedSlot) {
                        SelectSlot(pas::list_count(Slots) - 1);
                    }
                } else if (pas::list_count(Slots) - 2 != SelectedSlot) {
                    SelectSlot(pas::list_count(Slots) - 2);
                }
            } else {
                SelectSlot(SelectedSlot - 1);
            }
        } else if (Key == WindowsSdk::VK_PRIOR) {
            if (SelectedSlot != 0) {
                SelectSlot(0);
            }
        } else if (Key == WindowsSdk::VK_NEXT) {
            if (Globals::SaveManagerMode == smmLoad || FindAutoSaveSlot() < 0) {
                if (pas::list_count(Slots) - 1 != SelectedSlot) {
                    SelectSlot(pas::list_count(Slots) - 1);
                }
            } else if (pas::list_count(Slots) - 2 != SelectedSlot) {
                SelectSlot(pas::list_count(Slots) - 2);
            }
        } else if (Key == WindowsSdk::VK_DELETE) {
            DeleteClicked(nullptr);
        } else if (Key == WindowsSdk::VK_ESCAPE) {
            CloseClicked(Sender);
        } else if (Key == WindowsSdk::VK_RETURN) {
            if (Globals::SaveManagerMode == smmSave) {
                SaveClicked(Sender);
            } else {
                LoadClicked(Sender);
            }
        }
    }

    void TfSaveManager::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            SlotKeyDown(nullptr, WindowsSdk::VK_UP);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            SlotKeyDown(nullptr, WindowsSdk::VK_DOWN);
        }
    }

    std::uint8_t TfSaveManager::AcceptSaveNameCharacter(GI_MessageLoop::TObjectGI* Sender, char16_t Character) {
        return Character != u'\\' && Character != u'/' && Character != u':' && Character != u'*' && Character != u'?' && Character != u'\"' && Character != u'<' && Character != u'>' && Character != u'|';
    }

    void TfSaveManager::SelectSlot(std::int32_t SlotIndex) {
        GI_PanelScrollBar::TPanelScrollBarGI* ScrollPanel{};
        ClearSlotSelection();
        SelectedSlot = SlotIndex;
        if (pas::list_count(Slots) <= SelectedSlot) {
            SelectedSlot = -1;
        }
        {
            std::uint8_t isSlotEmpty = IsSlotEmpty(SlotIndex);
            GI_GraphButton::TGraphButtonGI* cpp_arg = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButDelete"_wref.get()));
            cpp_arg->SetDisabled(isSlotEmpty);
        }
        {
            std::uint8_t isSlotEmpty_2 = IsSlotEmpty(SlotIndex);
            GI_GraphButton::TGraphButtonGI* cpp_arg_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButLoad"_wref.get()));
            cpp_arg_2->SetDisabled(isSlotEmpty_2);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButSave"_wref.get()))->SetDisabled(SlotIndex < 0 || pas::list_count(Slots) <= SlotIndex);
        if (SelectedSlot >= 0 && pas::list_count(Slots) > SelectedSlot) {
            ScrollPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSlot"_wref.get()));
            {
                std::int32_t cpp_left = FindAutoSaveSlot();
                if (cpp_left != SlotIndex) {
                    GI_Panel::TPanelGI* cpp_with = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex)}))));
                    ScrollPanel->ScrollRectIntoView(cpp_with->GetLocalBounds());
                }
            }
            RefreshSlot(SlotIndex, Globals::SaveManagerMode == smmSave);
            if (pas::class_cast_if<GI_Edit::TEditGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Edit"})))) != nullptr) {
                if (IsSlotEmpty(SlotIndex)) {
                    GI_Edit::TEditGI* cpp_with_2 = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Edit"}))));
                    cpp_with_2->SetText(TfSaveManager::BuildCurrentSaveDescription());
                }
                SetFocusedControl(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(SlotIndex), "Edit"}))));
            }
            if (!IsSlotEmpty(SlotIndex)) {
                if (PreviewTimer != nullptr) {
                    CancelCallbackTimer(PreviewTimer);
                    PreviewTimer = nullptr;
                }
                LoadSavePreviews(pas::list_at<TSMSlot>(Slots, SlotIndex)->FileName);
            } else if (SelectedSlot == 0 && Globals::SaveManagerMode == smmSave) {
                {
                    GI_GraphBuf::TGraphBufGI* GameImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage"_wref.get()));
                    GameImage->GraphBuf->Clear();
                    GameImage->GraphBuf->AllocateRgb(GR_Main::SavePreviewGraph->Width, GR_Main::SavePreviewGraph->Height, GR_Main::SavePreviewGraph->PitchBytes);
                    {
                        void* pixels = GR_Main::SavePreviewGraph->GetPixels();
                        void* pixels_2 = GameImage->GraphBuf->GetPixels();
                        std::uint32_t cpp_arg_3 = GameImage->GraphBuf->Height * GameImage->GraphBuf->PitchBytes;
                        Windows::CopyMemory(pixels_2, pixels, cpp_arg_3);
                    }
                    GameImage->GraphBuf->RescaleRgb(GameImage->ClientSize.X, GameImage->ClientSize.Y);
                    GameImage->GraphBuf->ConvertRgbTo565();
                    GameImage->Invalidate();
                    GameImage->SetActive(false);
                }
                {
                    GI_GraphBuf::TGraphBufGI* GameImage2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage2"_wref.get()));
                    GameImage2->GraphBuf->Clear();
                    GameImage2->GraphBuf->AllocateRgb(GR_Main::SecondarySavePreviewGraph->Width, GR_Main::SecondarySavePreviewGraph->Height, GR_Main::SecondarySavePreviewGraph->PitchBytes);
                    {
                        void* pixels_3 = GR_Main::SecondarySavePreviewGraph->GetPixels();
                        void* pixels_4 = GameImage2->GraphBuf->GetPixels();
                        std::uint32_t cpp_arg_4 = GameImage2->GraphBuf->Height * GameImage2->GraphBuf->PitchBytes;
                        Windows::CopyMemory(pixels_4, pixels_3, cpp_arg_4);
                    }
                    GameImage2->GraphBuf->RescaleRgb(GameImage2->ClientSize.X, GameImage2->ClientSize.Y);
                    GameImage2->GraphBuf->ConvertRgbTo565();
                    GameImage2->Invalidate();
                    GameImage2->SetActive(false);
                }
                if (PreviewTimer != nullptr) {
                    CancelCallbackTimer(PreviewTimer);
                    PreviewTimer = nullptr;
                }
                PreviewTimer = ScheduleCallbackTimer(250, 250, pas::bind_method<&TfSaveManager::FinishPreviewDelay>(this), 0);
                PreviewSound->SetVolume(1.0f);
            } else {
                {
                    GI_GraphBuf::TGraphBufGI* GameImage_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage"_wref.get()));
                    GameImage_2->GraphBuf->Clear();
                    GameImage_2->Invalidate();
                    GameImage_2->SetActive(false);
                }
                {
                    GI_GraphBuf::TGraphBufGI* GameImage2_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage2"_wref.get()));
                    GameImage2_2->GraphBuf->Clear();
                    GameImage2_2->Invalidate();
                    GameImage2_2->SetActive(false);
                }
            }
            GR_Main::PostMouseMoveMessage();
        }
    }

    void TfSaveManager::ClearSlotSelection() {
        std::int32_t PreviousSlot{};
        if (PreviewTimer != nullptr) {
            CancelCallbackTimer(PreviewTimer);
            PreviewTimer = nullptr;
        }
        GetByName(u"GameImage"_wref.get())->SetActive(false);
        GetByName(u"GameImage2"_wref.get())->SetActive(false);
        PreviousSlot = SelectedSlot;
        SelectedSlot = -1;
        if (PreviousSlot >= 0) {
            if (pas::class_cast_if<GI_Edit::TEditGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(PreviousSlot), "Edit"})))) != nullptr) {
                SetFocusedControl(nullptr);
            }
            RefreshSlot(PreviousSlot, false);
        }
    }

    std::uint8_t TfSaveManager::AutoSaveExists() {
        return SysUtilsImports::FileExists(static_cast<pas::AnsiString>(TfSaveManager::GetAutoSavePath()));
    }

    pas::WideString TfSaveManager::GetAutoSavePath() {
        return pas::concat_wide({GR_Main::GetGameUserDirectory(), u"save\\", AutoSaveFileName});
    }

    std::int32_t TfSaveManager::FindAutoSaveSlot() {
        std::int32_t Result = -1;
        if (pas::list_count(Slots) > 0) {
            if (([&] {
                const pas::WideString& cpp_string_ref = pas::list_at<TSMSlot>(Slots, pas::list_count(Slots) - 1)->FileName;
                pas::WideString cpp_string = TfSaveManager::GetAutoSavePath();
                return cpp_string_ref == cpp_string;
            }())) {
                return pas::list_count(Slots) - 1;
            }
        }
        return Result;
    }

    pas::WideString TfSaveManager::BuildCurrentSaveDescription() {
        pas::WideString Result{};
        if (aPlayer::GetPlayer()->RuinsMode == 0) {
            if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                if (aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                    Result = aConst::LocalizedText(u"FormSaveManager.SaveInShip"_wref.get());
                    Result = EC_Str::ReplaceAllWideString(Result, u"<ShipName>"_wref.get(), aPlayer::GetPlayer()->CurrentPlanet->Name);
                } else {
                    Result = aConst::LocalizedText(u"FormSaveManager.SaveInPlanet"_wref.get());
                    Result = EC_Str::ReplaceAllWideString(Result, u"<Planet>"_wref.get(), aPlayer::GetPlayer()->CurrentPlanet->Name);
                }
            } else if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                Result = aConst::LocalizedText(u"FormSaveManager.SaveInShip"_wref.get());
                Result = ([&] {
                    const pas::WideString& fullName = aPlayer::GetPlayer()->DockedTo->GetFullName(u" "_wref.get());
                    const pas::WideString& result = Result;
                    return EC_Str::ReplaceAllWideString(result, u"<ShipName>"_wref.get(), fullName);
                }());
            } else {
                Result = aConst::LocalizedText(u"FormSaveManager.SaveInSpace"_wref.get());
            }
        } else if (aPlayer::GetPlayer()->RuinsSavedPlanet != nullptr) {
            if (aPlayer::GetPlayer()->RuinsSavedPlanet->IsMainPiratePlanet) {
                Result = aConst::LocalizedText(u"FormSaveManager.SaveInShip"_wref.get());
                Result = EC_Str::ReplaceAllWideString(Result, u"<ShipName>"_wref.get(), aPlayer::GetPlayer()->RuinsSavedPlanet->Name);
            } else {
                Result = aConst::LocalizedText(u"FormSaveManager.SaveInPlanet"_wref.get());
                Result = EC_Str::ReplaceAllWideString(Result, u"<Planet>"_wref.get(), aPlayer::GetPlayer()->RuinsSavedPlanet->Name);
            }
        } else if (aPlayer::GetPlayer()->RuinsSavedDockedTo != nullptr) {
            Result = aConst::LocalizedText(u"FormSaveManager.SaveInShip"_wref.get());
            Result = ([&] {
                const pas::WideString& fullName_2 = aPlayer::GetPlayer()->RuinsSavedDockedTo->GetFullName(u" "_wref.get());
                const pas::WideString& result_2 = Result;
                return EC_Str::ReplaceAllWideString(result_2, u"<ShipName>"_wref.get(), fullName_2);
            }());
        } else {
            Result = aConst::LocalizedText(u"FormSaveManager.SaveInSpace"_wref.get());
        }
        Result = EC_Str::ReplaceAllWideString(Result, u"<Star>"_wref.get(), aPlayer::GetPlayer()->CurrentStar->Name);
        Result = ([&] {
            const pas::WideString& name = aPlayer::GetPlayer()->CurrentStar->Constellation->GetName();
            const pas::WideString& result_3 = Result;
            return EC_Str::ReplaceAllWideString(result_3, u"<Constellation>"_wref.get(), name);
        }());
        return EC_Str::ReplaceAllWideString(Result, u"<Player>"_wref.get(), aPlayer::GetPlayer()->Name);
    }

    pas::WideString TfSaveManager::BuildUniqueSavePath(const pas::WideString& FileName, std::int32_t& SuffixIndex) {
        std::int32_t I{};
        std::int32_t ExistingSuffix{};
        std::int32_t Parts{};
        pas::WideString Directory{};
        pas::WideString BaseName{};
        pas::WideString Extension{};
        pas::WideString ExistingName{};
        pas::WideString Suffix{};
        Directory = EC_Str::TrimWideString(EC_Str::ExtractFileDirW(FileName));
        if (Directory == u"") {
            Directory = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"save"});
        }
        BaseName = EC_Str::TrimWideString(EC_Str::ExtractFileNameNoExtW(FileName));
        Extension = EC_Str::TrimWideString(EC_Str::ExtractFileExtNoDotW(FileName));
        if (Extension == u"") {
            Extension = u"sav"_w;
        }
        Parts = EC_Str::CountDelimitedPartsW(BaseName, u"()"_wref.get());
        if (Parts >= 3) {
            Suffix = EC_Str::ExtractDelimitedPartW(BaseName, Parts - 2, u"()"_wref.get());
            if (EC_Str::IsIntegerTextW(Suffix)) {
                BaseName = EC_Str::TrimWideString(EC_Str::ExtractDelimitedRangeW(BaseName, 0, Parts - 3, u"()"_wref.get()));
            }
        }
        SuffixIndex = 0;
        I = 0;
        while (I < pas::list_count(Slots)) {
            ExistingName = EC_Str::TrimWideString(EC_Str::LowerCaseWideString(EC_Str::ExtractFileNameNoExtW(pas::list_at<TSMSlot>(Slots, I)->FileName)));
            Parts = EC_Str::CountDelimitedPartsW(ExistingName, u"()"_wref.get());
            ExistingSuffix = 0;
            if (Parts >= 3) {
                Suffix = EC_Str::ExtractDelimitedPartW(ExistingName, Parts - 2, u"()"_wref.get());
                if (EC_Str::IsIntegerTextW(Suffix)) {
                    ExistingName = EC_Str::TrimWideString(EC_Str::ExtractDelimitedRangeW(ExistingName, 0, Parts - 3, u"()"_wref.get()));
                    ExistingSuffix = EC_Str::ExtractDigitsToIntW(Suffix);
                }
            }
            if (([&] {
                pas::WideString cpp_string = EC_Str::LowerCaseWideString(BaseName);
                return ExistingName == cpp_string;
            }())) {
                SuffixIndex = std::max<std::int32_t>(SuffixIndex, ExistingSuffix + 1);
            }
            ++I;
        }
        if (SuffixIndex == 0) {
            return pas::concat_wide({Directory, u"\\", BaseName, u".", Extension});
        }
        Parts = EC_Str::CountDelimitedPartsW(BaseName, u"()"_wref.get());
        if (Parts >= 3) {
            Suffix = EC_Str::ExtractDelimitedPartW(BaseName, Parts - 2, u"()"_wref.get());
            if (EC_Str::IsIntegerTextW(Suffix)) {
                return pas::concat_wide({Directory, u"\\", EC_Str::ExtractDelimitedRangeW(BaseName, 0, Parts - 3, u"()"_wref.get()), u" (", pas::wide_int_to_str(SuffixIndex), u").", Extension});
            }
            return pas::concat_wide({Directory, u"\\", BaseName, u" (", pas::wide_int_to_str(SuffixIndex), u").", Extension});
        }
        return pas::concat_wide({Directory, u"\\", BaseName, u" (", pas::wide_int_to_str(SuffixIndex), u").", Extension});
    }

    pas::WideString TfSaveManager::GetSaveConfigPath(const pas::WideString& FileName) {
        pas::WideString Directory{};
        pas::WideString BaseName{};
        pas::WideString Extension{};
        Directory = EC_Str::TrimWideString(EC_Str::ExtractFileDirW(FileName));
        BaseName = EC_Str::TrimWideString(EC_Str::ExtractFileNameNoExtW(FileName));
        Extension = u"txt"_w;
        return pas::concat_wide({Directory, u"\\", BaseName, u".", Extension});
    }

    std::uint8_t TfSaveManager::QuickSaveExists(std::int32_t SlotIndex) {
        return SysUtilsImports::FileExists(static_cast<pas::AnsiString>(TfSaveManager::GetQuickSavePath(SlotIndex)));
    }

    pas::WideString TfSaveManager::GetQuickSavePath(std::int32_t SlotIndex) {
        return pas::concat_wide({GR_Main::GetGameUserDirectory(), u"save\\", QuickSaveFileNames[SlotIndex]});
    }

    pas::WideString TfSaveManager::GetTurnSavePath() {
        return pas::concat_wide({GR_Main::GetGameUserDirectory(), u"save\\", TurnSaveFileName});
    }

    void TfSaveManager::SlotMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        {
            GI_MessageLoop::TObjectGI* byName = GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Sender->UserValue)), "onmouse"})));
            std::uint8_t cpp_arg = ([&] {
                std::int32_t cpp_left = FindAutoSaveSlot();
                return cpp_left != Sender->UserValue;
            }()) || Globals::SaveManagerMode != smmSave;
            byName->SetActive(cpp_arg);
        }
        GR_Main::SoundManager->PlaySound(u"Sound.ButtonEnter"_wref.get());
    }

    void TfSaveManager::SlotMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Sender->UserValue)), "onmouse"})))->SetActive(false);
        GR_Main::SoundManager->PlaySound(u"Sound.ButtonLeave"_wref.get());
    }

    void TfSaveManager::ScanSaveFiles() {
        PSMSlot Slot{};
        pas::AnsiString PreviousDirectory{};
        WindowsImports::THandle Search{};
        std::int32_t I{};
        EC_File::TFileEC* FileObject{};
        pas::WideString AutoPath{};
        WindowsSdk::TWin32FindData FindData{};
        Windows::TFileTime LocalTime{};
        FileObject = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        AutoPath = TfSaveManager::GetAutoSavePath();
        PreviousDirectory = SysUtilsImports::GetCurrentDir();
        SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Save"})));
        PSMSlot AutoSlot = nullptr;
        PSMSlot NewSlot = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Slots) - 1); cpp_range.next(I); ) {
            Slot = pas::list_at<TSMSlot>(Slots, I);
            Slot->FileName = pas::WideString();
            pas::dispose(Slot);
        }
        pas::list_clear(Slots);
        if (Globals::SaveManagerMode == smmSave) {
            pas::new_value(NewSlot);
            if (aPlayer::GetPlayer()->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                NewSlot->RaceName = pas::concat_wide({aConst::OwnerInfo[aGalaxyStruct::oiPirate].InternalName, aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace) & 0x0000007f].InternalName});
            } else {
                NewSlot->RaceName = aConst::OwnerInfo[aPlayer::GetPlayer()->OwnerId].InternalName;
            }
        }
        {
            try {
                FindData.dwFileAttributes = WindowsImports::FILE_ATTRIBUTE_NORMAL;
                Search = WindowsSdk::FindFirstFile(pas::literal_pointer("*.sav"), FindData);
                if (Search != WindowsImports::INVALID_HANDLE_VALUE) {
                    do {
                        if ((FindData.dwFileAttributes & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) == 0) {
                            pas::new_value(Slot);
                            Slot->FileName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"save\\", EC_Str::TrimWideString(pas::array_text<pas::WideString>(FindData.cFileName.elements, 260))});
                            if (([&] {
                                pas::WideString cpp_string = EC_Str::LowerCaseWideString(Slot->FileName);
                                pas::WideString cpp_string_2 = EC_Str::LowerCaseWideString(TfSaveManager::GetAutoSavePath());
                                return cpp_string == cpp_string_2;
                            }())) {
                                Slot->FileName = AutoPath;
                            }
                            Slot->DisplayName = EC_Str::ExtractFileNameNoExtW(Slot->FileName);
                            LocalTime = Slot->LocalWriteTime;
                            WindowsImports::FileTimeToLocalFileTime(FindData.ftLastWriteTime, LocalTime);
                            pas::store_unaligned<Windows::TFileTime>(&Slot->LocalWriteTime, LocalTime);
                            try {
                                FileObject->SetFileName(Slot->FileName);
                                if (!FileObject->TryAcquireReadHandle(true)) {
                                    GR_Main::SuppressExceptionLogCopy = true;
                                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                                }
                                if (FileObject->ReadWideString() != u"RSG") {
                                    GR_Main::SuppressExceptionLogCopy = true;
                                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                                }
                                I = EC_Str::ExtractDigitsToIntW(FileObject->ReadWideString());
                                if (I < 13 || I > aConst::CurrentSaveVersion) {
                                    GR_Main::SuppressExceptionLogCopy = true;
                                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                                }
                                if (Slot->FileName == AutoPath || GlobalsV::RunningUnderWine) {
                                    Slot->DisplayName = FileObject->ReadWideString();
                                } else {
                                    FileObject->ReadWideString();
                                }
                                Slot->Turn = EC_Str::ExtractDigitsToIntW(FileObject->ReadWideString());
                                Slot->Money = EC_Str::ExtractDigitsToIntW(FileObject->ReadWideString());
                                Slot->PilotName = FileObject->ReadWideString();
                                Slot->RaceName = FileObject->ReadWideString();
                                if (FileObject->ReadWideString() != u"EZ") {
                                    GR_Main::SuppressExceptionLogCopy = true;
                                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                                }
                                FileObject->ReleaseHandle();
                                if (Slot->FileName == AutoPath) {
                                    AutoSlot = Slot;
                                } else {
                                    fSaveManager::InsertScannedSaveSlotByTime(this, Slot);
                                }
                            } catch (...) {
                                pas::dispose(Slot);
                            }
                        }
                    } while (WindowsSdk::FindNextFile(Search, FindData) != 0);
                    WindowsImports::FindClose(Search);
                }
            } catch (...) {
                pas::free(FileObject);
                SysUtilsImports::SetCurrentDir(PreviousDirectory);
                throw;
            }
            pas::free(FileObject);
            SysUtilsImports::SetCurrentDir(PreviousDirectory);
        }
        if (NewSlot != nullptr) {
            pas::list_insert(Slots, 0, static_cast<void*>(NewSlot));
        }
        if (AutoSlot != nullptr) {
            pas::list_add(Slots, static_cast<void*>(AutoSlot));
        }
    }

    void InsertScannedSaveSlotByTime(TfSaveManager* Self, PSMSlot& Slot) {
        std::int32_t Middle{};
        PSMSlot OtherSlot{};
        Windows::TFileTime Time{};
        if (pas::list_count(Self->Slots) < 1) {
            pas::list_add(Self->Slots, static_cast<void*>(Slot));
            return;
        }
        Time = Slot->LocalWriteTime;
        std::int32_t Low = 0;
        OtherSlot = pas::list_at<TSMSlot>(Self->Slots, 0);
        std::int32_t Comparison = WindowsSdk::CompareFileTime(pas::ConstRef<Windows::TFileTime>(&OtherSlot->LocalWriteTime), Time);
        if (Comparison <= 0) {
            pas::list_insert(Self->Slots, 0, static_cast<void*>(Slot));
            return;
        }
        std::int32_t High = pas::list_count(Self->Slots) - 1;
        OtherSlot = pas::list_at<TSMSlot>(Self->Slots, High);
        Comparison = WindowsSdk::CompareFileTime(pas::ConstRef<Windows::TFileTime>(&OtherSlot->LocalWriteTime), Time);
        if (Comparison >= 0) {
            pas::list_add(Self->Slots, static_cast<void*>(Slot));
            return;
        }
        while (true) {
            if (High - Low < 2) {
                pas::list_insert(Self->Slots, High, static_cast<void*>(Slot));
                return;
            }
            Middle = (Low + High) / 2;
            OtherSlot = pas::list_at<TSMSlot>(Self->Slots, Middle);
            Comparison = WindowsSdk::CompareFileTime(pas::ConstRef<Windows::TFileTime>(&OtherSlot->LocalWriteTime), Time);
            if (Comparison == 0) {
                pas::list_insert(Self->Slots, Middle, static_cast<void*>(Slot));
                return;
            } else if (Comparison < 0) {
                High = Middle;
            } else {
                Low = Middle;
            }
        }
    }

    std::uint8_t TfSaveManager::IsSlotEmpty(std::int32_t SlotIndex) {
        if (SlotIndex < 0 || pas::list_count(Slots) <= SlotIndex) {
            return true;
        }
        return pas::list_at<TSMSlot>(Slots, SlotIndex)->FileName == u"";
    }

    std::int32_t TfSaveManager::FindNewestSlot() {
        std::int32_t I{};
        Windows::TFileTime Latest{};
        std::int32_t Result = -1;
        Latest.dwLowDateTime = 0u;
        Latest.dwHighDateTime = 0u;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Slots) - 1); cpp_range.next(I); ) {
            if (WindowsSdk::CompareFileTime(pas::ConstRef<Windows::TFileTime>(&pas::list_at<TSMSlot>(Slots, I)->LocalWriteTime), Latest) > 0) {
                Latest = pas::list_at<TSMSlot>(Slots, I)->LocalWriteTime;
                Result = I;
            }
        }
        return Result;
    }

    std::int32_t TfSaveManager::ReadSaveVersion(pas::WideString FileName) {
        EC_File::TFileEC* FileObject{};
        FileObject = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        FileObject->SetFileName(FileName);
        FileObject->AcquireReadHandle(false);
        FileObject->TryAcquireReadHandle(false);
        FileObject->ReadWideString();
        std::int32_t Result = EC_Str::ExtractDigitsToIntW(FileObject->ReadWideString());
        FileObject->ReleaseHandle();
        pas::free(FileObject);
        return Result;
    }

    void TfSaveManager::LoadSavePreviews(pas::WideString FileName) {
        std::int32_t ByteCount{};
        GI_GraphBuf::TGraphBufGI* FirstImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage"_wref.get()));
        FirstImage->SetActive(false);
        FirstImage->GraphBuf->Clear();
        FirstImage->Invalidate();
        GI_GraphBuf::TGraphBufGI* SecondImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GameImage2"_wref.get()));
        SecondImage->SetActive(false);
        SecondImage->GraphBuf->Clear();
        SecondImage->Invalidate();
        EC_File::TFileEC* FileObject = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        try {
            FileObject->SetFileName(FileName);
            FileObject->AcquireReadHandle(false);
            FileObject->ReadWideString();
            FileObject->ReadWideString();
            FileObject->ReadWideString();
            FileObject->ReadWideString();
            FileObject->ReadWideString();
            FileObject->ReadWideString();
            FileObject->ReadWideString();
            FileObject->ReadWideString();
            FileObject->ReadBuffer(&ByteCount, 4u);
            if (ByteCount > 0) {
                Buffer->SetSize(ByteCount);
                FileObject->ReadBuffer(Buffer->Data, ByteCount);
            }
            if (ByteCount > 0) {
                FirstImage->GraphBuf->LoadFromBuffer(Buffer);
                FirstImage->GraphBuf->RescaleRgb(FirstImage->ClientSize.X, FirstImage->ClientSize.Y);
                FirstImage->GraphBuf->ConvertRgbTo565();
            }
            FileObject->ReadBuffer(&ByteCount, 4u);
            if (ByteCount > 0) {
                Buffer->SetSize(ByteCount);
                FileObject->ReadBuffer(Buffer->Data, ByteCount);
            }
            if (ByteCount > 0) {
                Buffer->SetPosition(0);
                SecondImage->GraphBuf->LoadFromBuffer(Buffer);
                SecondImage->GraphBuf->RescaleRgb(FirstImage->ClientSize.X, FirstImage->ClientSize.Y);
                SecondImage->GraphBuf->ConvertRgbTo565();
            }
            FileObject->ReleaseHandle();
        } catch (...) {
            FirstImage->GraphBuf->Clear();
            SecondImage->GraphBuf->Clear();
        }
        pas::free(FileObject);
        pas::free(Buffer);
        if (PreviewTimer != nullptr) {
            CancelCallbackTimer(PreviewTimer);
            PreviewTimer = nullptr;
        }
        PreviewTimer = ScheduleCallbackTimer(250, 250, pas::bind_method<&TfSaveManager::FinishPreviewDelay>(this), 0);
        PreviewSound->SetVolume(1.0f);
    }

    void TfSaveManager::FinishPreviewDelay(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PreviewSound->SetVolume(0.0f);
        if (PreviewTimer != nullptr) {
            CancelCallbackTimer(PreviewTimer);
            PreviewTimer = nullptr;
        }
        if (static_cast<std::uint8_t>(IsSlotEmpty(SelectedSlot) ^ 1) || SelectedSlot == 0 && Globals::SaveManagerMode == smmSave) {
            GetByName(u"GameImage"_wref.get())->SetActive(true);
            GetByName(u"GameImage2"_wref.get())->SetActive(true);
        } else {
            GetByName(u"GameImage"_wref.get())->SetActive(false);
            GetByName(u"GameImage2"_wref.get())->SetActive(false);
        }
    }

    void TfSaveManager::SelectMusic() {
    }

    void TfSaveManager::p_destroy() {
        fSaveManager::TfSaveManager_Destroy(this);
    }

} // namespace fSaveManager
