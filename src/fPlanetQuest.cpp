#include "layout/fPlanetQuest.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/EC_Data.hpp"
#include "types/EventClass.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/LocationClass.hpp"
#include "types/ParameterClass.hpp"
#include "types/PathClass.hpp"
#include "types/SystemImports.hpp"
#include "types/TextFieldClass.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aRanger.hpp"
#include "types/aShip.hpp"
#include "types/fHangar.hpp"
#include "types/fLoadQuest.hpp"
#include "types/fSaveManager.hpp"
#include "types/fScore.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBuf.hpp"
#include "units/EC_Expression.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/TextQuest.hpp"
#include "units/TextQuestInterface.hpp"
#include "units/ThreadCalc.hpp"
#include "units/ValueListClass.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/fPlanetQuest.hpp"

namespace fPlanetQuest {
    void AddQuestTextParagraph(const pas::WideString& Text, std::int32_t FontMode, TfPlanetQuest* Self, GI_PanelScrollBar::TPanelScrollBarGI*& Panel, std::int32_t& NextTop);

    std::int32_t QuestStyleCount = 0;

    aRanger::PQuest ActiveGovernmentQuest = nullptr;

    fPlanetQuest::TTextQuestPlayerInterface* QuestPlayerInterface = nullptr;

    aScript::PQueuedTextQuest ActiveQueuedTextQuest = nullptr;

    void TfQuestA_Create(TfQuestA* Self) {
        pas::object_create(Self);
    }

    void TfQuestA_Destroy(TfQuestA* Self) {
        pas::object_destroy(Self);
    }

    pas::WideString TfPlanetQuest::GetTextBeforeDelimiter(const std::u16string_view& Text, char16_t Delimiter) {
        std::int32_t I{};
        pas::WideString S{};
        std::int32_t N = static_cast<std::int32_t>(Text.length());
        for (auto cpp_range = pas::for_to<std::int32_t>(1, N); cpp_range.next(I); ) {
            if (Text[I - 1] == Delimiter) {
                break;
            }
            S = pas::concat_wide({S, Text[I - 1]});
        }
        return S;
    }

    // A nonempty string without a comma is read past its end.
    pas::WideString TfPlanetQuest::GetTextAfterComma(const std::u16string_view& Text, char16_t IgnoredDelimiter) {
        std::int32_t I{};
        pas::WideString S{};
        std::int32_t N = static_cast<std::int32_t>(Text.length());
        if (N != 0) {
            I = 1;
            while (Text[I - 1] != u',') {
                ++I;
            }
            ++I;
            while (I <= N) {
                S = pas::concat_wide({S, Text[I - 1]});
                ++I;
            }
        }
        return S;
    }

    // Hexadecimal complement of the quest buffer's CRC32.
    pas::WideString TfPlanetQuest::GetQuestContentHash(std::int32_t QuestId) {
        pas::WideString Result{};
        EC_CacheBuf::TCBufEC* Data{};
        EC_CacheBuf::TCBufControlEC* Control = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.", SysUtils::IntToStr(QuestId)})));
                Data = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                Result = EC_Expression::ScriptDwordToHex(Data->Buffer->ComputeCrc32() ^ 0xffffffffu);
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
        return Result;
    }

    void TfPlanetQuest::LoadQuestById(std::int32_t QuestId) {
        EC_CacheBuf::TCBufEC* Data{};
        this->QuestId = QuestId;
        EC_CacheBuf::TCBufControlEC* Control = nullptr;
        QuestName = pas::wide_int_to_str(QuestId);
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.", SysUtils::IntToStr(QuestId)})));
                Data = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                Quest->LoadFromReader(Data->Buffer, false);
                if (!Globals::StandaloneQuestMode) {
                    if (QuestId >= aGalaxyStruct::FirstLicensedQuestId) {
                        if (GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"sv)->CountBlocks(u"PlanetQuestLic"_wref.get()) <= 0 || ([&] {
                            pas::WideString cpp_string = ([&] {
                                const pas::WideString& intToStr = pas::wide_int_to_str(QuestId);
                                EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"sv)->GetBlock(u"PlanetQuestLic"sv);
                                return block->GetParamOrMarker(pas::view(intToStr));
                            }());
                            pas::WideString cpp_string_2 = EC_Expression::ScriptDwordToHex(Data->Buffer->ComputeCrc32() ^ 0xffffffffu);
                            return cpp_string != cpp_string_2;
                        }())) {
                            GR_Main::CCInterface->SetTamperDetected(true);
                        }
                    }
                }
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
    }

    void TfPlanetQuest::LoadQuestByName(const pas::WideString& Name) {
        EC_CacheBuf::TCBufEC* Data{};
        QuestId = -1;
        EC_CacheBuf::TCBufControlEC* Control = nullptr;
        QuestName = Name;
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(pas::concat_wide({u"PlanetQuest.", Name}));
                Data = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                Quest->LoadFromReader(Data->Buffer, false);
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
    }

    // Applies PQI overrides only through quest version 1111111124.
    void TfPlanetQuest::StartLoadedQuest() {
        ClearChoices();
        DaysElapsed = 0;
        if (Quest->FormatVersion <= 1111111124) {
            ApplyLegacyPictureOverrides();
        }
        if (aPlayer::GetPlayer() == nullptr) {
            CurrentDate = EC_Str::TrimWideString(aGalaxy::Galaxy->FormatTurnDate(aGalaxyStruct::GalaxyWarmupTurns));
        } else {
            CurrentDate = EC_Str::TrimWideString(aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn));
        }
        Quest->PlayerInterface = QuestPlayerInterface;
        ImportExternalParameters();
        if (aPlayer::GetPlayer() == nullptr) {
            Quest->Start(-1, false);
        } else {
            Quest->Start(aPlayer::GetPlayer()->Money, true);
        }
        FinishChoiceLayout();
    }

    void TfPlanetQuest::ClearChoices() {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"ActionListWindow"sv));
        Panel->FreeOwnedChildren();
        Panel->Invalidate();
        NextChoiceTop = 10;
        ParameterPanelWidth = 0;
        ParameterPanelHeight = 0;
        ChoiceCount = 0;
        GI_Panel::TPanelGI* Spacer = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, GetByName(u"ActionListWindow"sv));
        Spacer->SetPosition(ClassesImports::Point(0, 0));
        Spacer->SetSize(ClassesImports::Point(10, 10));
        Spacer->SetPositionModeW(true);
    }

    void TfPlanetQuest::AddChoice(pas::WideString Text, std::int32_t Value, TQuestChoiceEvent Callback) {
        pas::WideString Path{};
        std::int32_t Skip = 0;
        while (Skip < Text.length()) {
            if (Text.read(Skip + 1) != u'-' && Text.read(Skip + 1) != u' ') {
                break;
            }
            ++Skip;
        }
        if (Skip > 0) {
            Text = pas::copy(Text, Skip + 1, Text.length() - Skip);
        }
        GI_PanelScrollBar::TPanelScrollBarGI* Owner = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"ActionListWindow"sv));
        TfQuestA* Choice = pas::construct_call<TfQuestA>(TfQuestA_Create);
        Choice->Callback = Callback;
        Choice->Value = Value;
        GI_Panel::TPanelGI* Panel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Owner);
        Panel->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Choice));
        Panel->SetName(pas::wide_int_to_str(ChoiceCount));
        Panel->SetPosition(ClassesImports::Point(0, NextChoiceTop));
        Panel->SetSize(ClassesImports::Point(Owner->ClientSize.X, 20));
        Panel->SetPositionModeW(true);
        Panel->MouseEnterCallback = pas::bind_static_method<&TfPlanetQuest::ChoiceMouseEnter>(this);
        Panel->MouseLeaveCallback = pas::bind_static_method<&TfPlanetQuest::ChoiceMouseLeave>(this);
        Panel->LeftButtonDownCallback = pas::bind_static_method<&TfPlanetQuest::ChoiceMouseDown>(this);
        Panel->LeftButtonUpCallback = pas::bind_method<&TfPlanetQuest::ChoiceMouseUp>(this);
        GI_Image::TImageGI* Highlight = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
        Highlight->SetDepth(3.0);
        Highlight->SetPosition(ClassesImports::Point(0, 0));
        Highlight->SetSize(ClassesImports::Point(Owner->ClientSize.X, 20));
        Path = pas::concat_wide({u"Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1), u"Line"});
        if (GR_Main::CacheDataRoot->FileExistsByPath(Path)) {
            Highlight->SetImagePath(pas::concat_wide({u"GI,", Path}));
        } else {
            Highlight->SetImagePath(pas::concat_wide({u"GI,Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S1Line"}));
        }
        Highlight->SetImageKindX(GI_Main::ikxLeftFill);
        Highlight->SetImageKindY(GI_Main::ikyTopFill);
        Highlight->SetActive(false);
        GI_Label::TLabelGI* TextLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
        TextLabel->SetName(pas::wide_int_to_str(ChoiceCount));
        if (GlobalsV::FontQuest == 0) {
            TextLabel->SetFontName(GlobalsV::NormalFontName);
        } else if (GlobalsV::FontQuest == 1) {
            TextLabel->SetFontName(GlobalsV::SmoothBigFontName);
        } else if (GlobalsV::FontQuest == 2) {
            TextLabel->SetFontName(GlobalsV::SmoothHugeFontName);
        } else if (GlobalsV::FontQuest >= 3) {
            TextLabel->SetFontName(GlobalsV::SmoothIntroFontName);
        }
        TextLabel->SetSize(ClassesImports::Point(Owner->ClientSize.X - 20, 20));
        TextLabel->SetPosition(ClassesImports::Point(10, 0));
        TextLabel->SetDepth(2.0);
        TextLabel->SetWordWrapEnabled(true);
        TextLabel->SetTextAlignX(GI_Main::taxLeft);
        TextLabel->SetTextAlignY(GI_Main::tayAuto);
        TextLabel->SetText(pas::concat_wide({u"<Object=0,20,14,0>", Text}));
        TextLabel->SetPositionModeW(true);
        TextLabel->SetActive(true);
        TextLabel->CreateEmbeddedControl = pas::bind_static_method<&TfPlanetQuest::CreateChoiceInlineObject>(this);
        TextLabel->SetTextAlignY(GI_Main::tayCenterEx);
        TextLabel->SetTextColor(GetTextColor(GlobalsV::QuestStyleIndex));
        Panel->SetSize(ClassesImports::Point(Panel->ClientSize.X, GR_Main::GiScalePixelsEx(2, 1) * 2 + TextLabel->ClientSize.Y));
        TextLabel->SetSize(ClassesImports::Point(TextLabel->ClientSize.X, Panel->ClientSize.Y));
        Highlight->SetSize(Panel->ClientSize);
        NextChoiceTop += Panel->ClientSize.Y;
        {
            std::int32_t lineHeight = TextLabel->GetLineHeight();
            GI_ScrollBar::TScrollBarGI* verticalScrollBar = Owner->VerticalScrollBar;
            verticalScrollBar->SetSmallChange(lineHeight);
        }
        Owner->UpdateScrollRanges();
        ++ChoiceCount;
    }

    // Value and callback are ignored.
    void TfPlanetQuest::AddDisabledChoice(pas::WideString Text, std::int32_t Value, TQuestChoiceEvent Callback) {
        pas::WideString Path{};
        std::int32_t Skip = 0;
        while (Skip < Text.length()) {
            if (Text.read(Skip + 1) != u'-' && Text.read(Skip + 1) != u' ') {
                break;
            }
            ++Skip;
        }
        if (Skip > 0) {
            Text = pas::copy(Text, Skip + 1, Text.length() - Skip);
        }
        Text = EC_Str::RemoveMatchingTextTagsW(Text, u"color"sv, u"COLOR"sv);
        Text = EC_Str::RemoveMatchingTextTagsW(Text, u"/color"sv, u"/COLOR"sv);
        GI_PanelScrollBar::TPanelScrollBarGI* Owner = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"ActionListWindow"sv));
        GI_Panel::TPanelGI* Panel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Owner);
        Panel->SetName(pas::wide_int_to_str(ChoiceCount));
        Panel->SetPosition(ClassesImports::Point(0, NextChoiceTop));
        Panel->SetSize(ClassesImports::Point(Owner->ClientSize.X, 20));
        Panel->SetPositionModeW(true);
        Panel->MouseEnterCallback = pas::bind_static_method<&TfPlanetQuest::DisabledChoiceMouseEnter>(this);
        Panel->MouseLeaveCallback = pas::bind_static_method<&TfPlanetQuest::DisabledChoiceMouseLeave>(this);
        Panel->LeftButtonDownCallback = pas::bind_static_method<&TfPlanetQuest::ChoiceMouseDown>(this);
        Panel->LeftButtonUpCallback = pas::bind_static_method<&TfPlanetQuest::DisabledChoiceMouseUp>(this);
        GI_Image::TImageGI* Highlight = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
        Highlight->SetDepth(3.0);
        Highlight->SetPosition(ClassesImports::Point(0, 0));
        Highlight->SetSize(ClassesImports::Point(Owner->ClientSize.X, 20));
        Path = pas::concat_wide({u"Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1), u"Line"});
        if (GR_Main::CacheDataRoot->FileExistsByPath(Path)) {
            Highlight->SetImagePath(pas::concat_wide({u"GI,", Path}));
        } else {
            Highlight->SetImagePath(pas::concat_wide({u"GI,Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S1Line"}));
        }
        Highlight->SetImageKindX(GI_Main::ikxLeftFill);
        Highlight->SetImageKindY(GI_Main::ikyTopFill);
        Highlight->SetActive(false);
        GI_Label::TLabelGI* TextLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
        TextLabel->SetName(pas::wide_int_to_str(ChoiceCount));
        if (GlobalsV::FontQuest == 0) {
            TextLabel->SetFontName(GlobalsV::NormalFontName);
        } else if (GlobalsV::FontQuest == 1) {
            TextLabel->SetFontName(GlobalsV::SmoothBigFontName);
        } else if (GlobalsV::FontQuest == 2) {
            TextLabel->SetFontName(GlobalsV::SmoothHugeFontName);
        } else if (GlobalsV::FontQuest >= 3) {
            TextLabel->SetFontName(GlobalsV::SmoothIntroFontName);
        }
        TextLabel->SetSize(ClassesImports::Point(Owner->ClientSize.X - 20, 20));
        TextLabel->SetPosition(ClassesImports::Point(10, 0));
        TextLabel->SetDepth(2.0);
        TextLabel->SetWordWrapEnabled(true);
        TextLabel->SetTextAlignX(GI_Main::taxLeft);
        TextLabel->SetTextAlignY(GI_Main::tayAuto);
        TextLabel->SetText(pas::concat_wide({u"<Object=0,20,14,0>", Text}));
        TextLabel->SetPositionModeW(false);
        TextLabel->SetActive(true);
        TextLabel->UserState = 1;
        TextLabel->CreateEmbeddedControl = pas::bind_static_method<&TfPlanetQuest::CreateChoiceInlineObject>(this);
        TextLabel->SetTextAlignY(GI_Main::tayCenterEx);
        TextLabel->SetTextColor(GetDisabledTextColor(GlobalsV::QuestStyleIndex));
        Panel->SetSize(ClassesImports::Point(Panel->ClientSize.X, GR_Main::GiScalePixelsEx(2, 1) * 2 + TextLabel->ClientSize.Y));
        TextLabel->SetSize(Panel->ClientSize);
        Highlight->SetSize(Panel->ClientSize);
        NextChoiceTop += Panel->ClientSize.Y;
        Owner->UpdateScrollRanges();
        ++ChoiceCount;
    }

    GI_MessageLoop::TObjectGI* TfPlanetQuest::CreateChoiceInlineObject(GI_Label::TLabelGI* Sender, EC_CacheFont::PFontObjectEC Item) {
        pas::WideString Path{};
        GI_MessageLoop::TObjectGI* Result = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Sender);
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Result);
        Path = pas::concat_wide({u"Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1), u"Answer"});
        if (Sender->UserState == 1) {
            if (GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({Path, u"H"}))) {
                Image->SetImagePath(pas::concat_wide({u"GI,", Path, u"H"}));
            } else {
                Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S1AnswerH"}));
            }
        } else if (GR_Main::CacheDataRoot->FileExistsByPath(Path)) {
            Image->SetImagePath(pas::concat_wide({u"GI,", Path}));
        } else {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S1Answer"}));
        }
        Image->SetImageKindX(GI_Main::ikxLeft);
        Image->SetSize(Image->GetContentSize());
        return Result;
    }

    void TfPlanetQuest::ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(true);
    }

    void TfPlanetQuest::ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(false);
    }

    void TfPlanetQuest::ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(2, 0));
        }
    }

    void TfPlanetQuest::ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(163);
        }
        TfQuestA* Choice = reinterpret_cast<TfQuestA*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        Sender->UserValue = 0;
        ClearChoices();
        if (Choice != nullptr) {
            if (pas::assigned(Choice->Callback)) {
                Choice->Callback(Choice->Value);
            }
            pas::free(Choice);
        }
        FinishChoiceLayout();
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(164);
        }
        GR_Main::PostMouseMoveMessage();
        GI_Main::BreakUiMessage();
    }

    void TfPlanetQuest::DisabledChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(true);
    }

    void TfPlanetQuest::DisabledChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender != nullptr && Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(0, 0));
            Sender->FirstChild->SetActive(false);
        }
    }

    void TfPlanetQuest::DisabledChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->FirstChild != nullptr && Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(0, 0));
        }
    }

    // Only exact deltas of +120 and -120 are handled.
    void TfPlanetQuest::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel1{};
        GI_PanelScrollBar::TPanelScrollBarGI* Panel2{};
        GI_PanelScrollBar::TPanelScrollBarGI* Panel3{};
        GI_PanelScrollBar::TPanelScrollBarGI* Panel4{};
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            if (GetByName(u"ActionListWindow"sv)->ContainsPoint(Point)) {
                Panel1 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"ActionListWindow"sv));
                Panel1->SetScrollOffset(ClassesImports::Point(0, Panel1->ScrollOffset.Y - Panel1->VerticalScrollBar->SmallChange));
                Panel1->PanelScrollChanged(nullptr);
            } else {
                Panel2 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"sv));
                Panel2->SetScrollOffset(ClassesImports::Point(0, Panel2->ScrollOffset.Y - Panel2->VerticalScrollBar->SmallChange));
                Panel2->PanelScrollChanged(nullptr);
            }
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            if (GetByName(u"ActionListWindow"sv)->ContainsPoint(Point)) {
                Panel3 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"ActionListWindow"sv));
                Panel3->SetScrollOffset(ClassesImports::Point(0, Panel3->ScrollOffset.Y + Panel3->VerticalScrollBar->SmallChange));
                Panel3->PanelScrollChanged(nullptr);
            } else {
                Panel4 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"sv));
                Panel4->SetScrollOffset(ClassesImports::Point(0, Panel4->ScrollOffset.Y + Panel4->VerticalScrollBar->SmallChange));
                Panel4->PanelScrollChanged(nullptr);
            }
        }
    }

    void TfPlanetQuest::FinishChoiceLayout() {
        GI_MessageLoop::TObjectGI* Control{};
        std::int32_t LineCount{};
        std::int32_t Page{};
        std::int32_t ExtraOffset{};
        NextChoiceTop += 10;
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"ActionListWindow"sv));
        GI_Panel::TPanelGI* Spacer = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
        Spacer->SetPosition(ClassesImports::Point(0, NextChoiceTop - 10));
        Spacer->SetSize(ClassesImports::Point(10, 10));
        Spacer->SetPositionModeW(true);
        Panel->SetActive(true);
        Panel->SetVerticalScrollbarEnabled(NextChoiceTop > Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
        Panel->SetScrollOffset(ClassesImports::Point(0, 0));
        Panel->SetDragScrollingEnabled(Panel->IsVerticalScrollbarEnabled());
        Panel->UpdateScrollRanges();
        if (GlobalsV::QuestPageAnimationEnabled) {
            GetByName(u"ActionListWindow"sv)->SetActive(false);
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"sv));
            Panel->SetUnlimitedWorldEnabled(true);
            Panel->SetDragScrollingEnabled(false);
            Control = Panel->FirstChild;
            LineCount = 0;
            Page = 1;
            ExtraOffset = 0;
            while (Control != nullptr) {
                Control->UserIndex = Control->LocalPosition.Y;
                if (pas::class_cast_if<GI_Label::TLabelGI*>(Control) != nullptr) {
                    if (Control->LocalPosition.Y < Panel->ClientSize.Y) {
                        Control->UserValue = Page;
                        if (Page > 1) {
                            Control->SetPosition(ClassesImports::Point(Control->LocalPosition.X, -Control->LocalPosition.Y - Control->ClientSize.Y - ExtraOffset));
                        }
                        LineCount += pas::checked_cast<GI_Label::TLabelGI*>(Control)->GetRenderedLineCount();
                        if (LineCount > 5) {
                            LineCount = 0;
                            ++Page;
                        }
                    }
                }
                Control = Control->NextSibling;
            }
            if (PageAnimationTimer != nullptr) {
                CancelCallbackTimer(PageAnimationTimer);
                PageAnimationTimer = nullptr;
            }
            PageAnimationTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfPlanetQuest::AnimateTextPage>(this), 0);
        }
    }

    void TfPlanetQuest::AnimateTextPage(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Step{};
        std::uint8_t Moving = false;
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"sv));
        GI_MessageLoop::TObjectGI* Control = Panel->FirstChild;
        while (Control != nullptr) {
            if (pas::class_cast_if<GI_Label::TLabelGI*>(Control) != nullptr) {
                if (Control->LocalPosition.Y < Control->UserIndex) {
                    Moving = true;
                    Step = System::Round(pas::real_min<pas::Extended>(1.0L, pas::real_divide(Control->UserIndex - Control->LocalPosition.Y, 1.0E+2L)) * 3.0E+1L);
                    if (Step < 1) {
                        Step = 1;
                    }
                    Control->SetPosition(ClassesImports::Point(Control->LocalPosition.X, std::min<std::int32_t>(Control->UserIndex, Control->LocalPosition.Y + Step)));
                }
            }
            Control = Control->NextSibling;
        }
        if (!Moving) {
            Panel->SetUnlimitedWorldEnabled(false);
            Panel->SetDragScrollingEnabled(true);
            if (PageAnimationTimer != nullptr) {
                CancelCallbackTimer(PageAnimationTimer);
                PageAnimationTimer = nullptr;
            }
            GetByName(u"ActionListWindow"sv)->SetActive(true);
            GR_Main::PostMouseMoveMessage();
        }
    }

    void TfPlanetQuest::ClearParameterPanel() {
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"ParamsShowWindow"sv));
        Panel->FreeOwnedChildren();
        ParameterPanelWidth = 0;
        ParameterPanelHeight = 0;
    }

    void TfPlanetQuest::AppendParameterText(pas::WideString Text) {
        EC_Str::TStringsEC* Lines{};
        GI_Label::TLabelGI* TextLabel{};
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"ParamsShowWindow"sv));
        std::uint8_t FixedWidth = EC_Str::FindTextOffsetW(EC_Str::LowerCaseWideString(Text), u"<fix>"_wref.get(), 0) >= 0;
        Text = EC_Str::RemoveMatchingTextTagsW(Text, u"fix"sv, u"FIX"sv);
        Text = EC_Str::RemoveMatchingTextTagsW(Text, u"/fix"sv, u"/FIX"sv);
        Text = ([&] {
            const pas::WideString& textColorTag = GetTextColorTag(GlobalsV::QuestStyleIndex);
            const pas::WideString& textHighlightColorTag = aMyFunction::TextHighlightColorTag;
            const pas::WideString& text = Text;
            return EC_Str::ReplaceAllWideString(text, textColorTag, pas::view(textHighlightColorTag));
        }());
        Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
        Lines->SetText(Text);
        Lines->First();
        while (!Lines->IsAtEnd()) {
            TextLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            if (FixedWidth) {
                TextLabel->SetFontName(pas::concat_wide({u"Font.", GR_Main::GiResourceSuffix(), u"Fix"}));
            } else {
                TextLabel->SetFontName(GlobalsV::NormalFontName);
            }
            TextLabel->SetSize(ClassesImports::Point(1, 1));
            TextLabel->SetTextAlignX(GI_Main::taxAuto);
            TextLabel->SetTextAlignY(GI_Main::tayAuto);
            TextLabel->SetWordWrapEnabled(false);
            TextLabel->SetText(Lines->GetCurrentText());
            TextLabel->SetTextColor(GetTextColor(0));
            TextLabel->SetPosition(ClassesImports::Point(0, ParameterPanelHeight));
            TextLabel->SetTextAlignY(GI_Main::tayCenterEx);
            if (TextLabel->ClientSize.Y < 10) {
                TextLabel->SetSize(ClassesImports::Point(TextLabel->ClientSize.X, TextLabel->ClientSize.Y + 10));
            }
            ParameterPanelHeight += TextLabel->ClientSize.Y;
            TextLabel->UserData = FixedWidth;
            if (FixedWidth) {
                ParameterPanelHeight += 2;
            }
            ParameterPanelWidth = std::max<std::int32_t>(ParameterPanelWidth, TextLabel->ClientSize.X);
            Lines->Next();
        }
        pas::free(Lines);
    }

    void TfPlanetQuest::LayoutParameterPanel() {
        GI_Window::TWindowGI* Window = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"ParamsShowWindowParent"sv));
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"ParamsShowWindow"sv));
        Window->SetPosition(ParameterPanelOrigin);
        Window->SetSize(ClassesImports::Point(ParameterPanelWidth + Window->WorkSubRect.Left + Window->WorkSubRect.Right, ParameterPanelHeight + Window->WorkSubRect.Top + Window->WorkSubRect.Bottom));
        Window->UpdateAutoGeometry();
        Panel->SetSize(Window->ClientSize);
        std::int32_t Y = 0;
        GI_MessageLoop::TObjectGI* Control = Panel->FirstChild;
        while (Control != nullptr) {
            Control->SetPosition(ClassesImports::Point(Window->WorkSubRect.Left, (Window->ClientSize.Y - Window->WorkSubRect.Top - Window->WorkSubRect.Bottom - ParameterPanelHeight) / 2 + (Window->WorkSubRect.Top + Y)));
            Y += Control->ClientSize.Y;
            if (Control->UserData != 0) {
                Y += 2;
            }
            Control = Control->NextSibling;
        }
        if (GR_Main::GameScreenHeight - GR_Main::GiScalePixels(30) < Window->LocalPosition.Y + Window->ClientSize.Y) {
            Window->SetPosition(ClassesImports::Point(Window->LocalPosition.X, GR_Main::GameScreenHeight - GR_Main::GiScalePixels(30) - Window->ClientSize.Y));
        }
        if (GR_Main::GameScreenWidth - GR_Main::GiScalePixels(10) < Window->LocalPosition.X + Window->ClientSize.X) {
            Window->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - GR_Main::GiScalePixels(10) - Window->ClientSize.X, Window->LocalPosition.Y));
        }
    }

    // <fix> sections use the fixed-width font.
    void TfPlanetQuest::SetQuestText(const pas::WideString& Text) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel{};
        std::int32_t NextTop{};
        EC_Str::TStringsEC* Lines{};
        pas::WideString LowerText{};
        std::int32_t StartIndex{};
        std::int32_t TagIndex{};
        std::int32_t TextLength{};
        auto AddQuestTextLines = [&](const pas::WideString& Text, std::int32_t FontMode) -> void {
            std::int32_t N{};
            std::int32_t StartIndex{};
            std::int32_t EndIndex{};
            N = Text.length();
            StartIndex = 0;
            while (StartIndex < N) {
                EndIndex = EC_Str::FindTextOffsetW(Text, u"\n"_wref.get(), StartIndex);
                if (EndIndex < 0) {
                    fPlanetQuest::AddQuestTextParagraph(pas::copy(Text, StartIndex + 1, N - StartIndex), FontMode, this, Panel, NextTop);
                    break;
                }
                fPlanetQuest::AddQuestTextParagraph(pas::copy(Text, StartIndex + 1, EndIndex - StartIndex + 1), FontMode, this, Panel, NextTop);
                StartIndex = EndIndex + 1;
            }
        };
        CurrentText = Text;
        Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"sv));
        Panel->FreeOwnedChildren();
        if (Text != u"") {
            NextTop = 0;
            // The native routine retains this allocation although the nested helpers do not use it.
            Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
            TextLength = Text.length();
            LowerText = EC_Str::LowerCaseWideString(Text);
            StartIndex = 0;
            while (StartIndex < TextLength) {
                TagIndex = EC_Str::FindTextOffsetW(LowerText, u"<fix>"_wref.get(), StartIndex);
                if (StartIndex < TagIndex) {
                    AddQuestTextLines(pas::copy(Text, StartIndex + 1, TagIndex - StartIndex), 0);
                }
                if (TagIndex >= 0) {
                    TagIndex += 5;
                    while (TagIndex < TextLength && (Text.read(TagIndex + 1) == u' ' || Text.read(TagIndex + 1) == u'\t' || Text.read(TagIndex + 1) == u'\r')) {
                        ++TagIndex;
                    }
                    if (TagIndex < TextLength) {
                        if (Text.read(TagIndex + 1) == u'\n') {
                            ++TagIndex;
                        }
                    }
                    StartIndex = TagIndex;
                    TagIndex = EC_Str::FindTextOffsetW(LowerText, u"</fix>"_wref.get(), StartIndex);
                    if (StartIndex < TagIndex) {
                        AddQuestTextLines(pas::copy(Text, StartIndex + 1, TagIndex - StartIndex), 1);
                    }
                    if (TagIndex >= 0) {
                        TagIndex += 6;
                        while (TagIndex < TextLength && (Text.read(TagIndex + 1) == u' ' || Text.read(TagIndex + 1) == u'\t' || Text.read(TagIndex + 1) == u'\r')) {
                            ++TagIndex;
                        }
                        if (TagIndex < TextLength) {
                            if (Text.read(TagIndex + 1) == u'\n') {
                                ++TagIndex;
                            }
                        }
                        StartIndex = TagIndex;
                    } else {
                        AddQuestTextLines(pas::copy(Text, StartIndex + 1, TextLength - StartIndex), 1);
                        break;
                    }
                } else {
                    AddQuestTextLines(pas::copy(Text, StartIndex + 1, TextLength - StartIndex), 0);
                    break;
                }
            }
            pas::free(Lines);
            Panel->SetScrollOffset(ClassesImports::Point(0, 0));
            Panel->SetVerticalScrollbarEnabled(Panel->ClientSize.Y < NextTop);
            if (Panel->FirstChild != nullptr) {
                std::int32_t lineHeight = pas::checked_cast<GI_Label::TLabelGI*>(Panel->FirstChild)->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = Panel->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(lineHeight);
            }
            Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
            Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
            Panel->UpdateScrollRanges();
            Panel->Invalidate();
        }
    }

    void AddQuestTextParagraph(const pas::WideString& Text, std::int32_t FontMode, TfPlanetQuest* Self, GI_PanelScrollBar::TPanelScrollBarGI*& Panel, std::int32_t& NextTop) {
        std::uint8_t Indent{};
        std::int32_t I{};
        GI_Label::TLabelGI* TextLabel{};
        if (Text != u"") {
            Indent = false;
            if (FontMode == 0) {
                Indent = true;
                I = 0;
                while (I < Text.length()) {
                    if (pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + I) == u'-') {
                        Indent = false;
                        break;
                    } else {
                        if (pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + I) != u' ' && pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + I) != u'\t') {
                            break;
                        }
                        ++I;
                    }
                }
            }
            TextLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            if (FontMode == 0) {
                if (GlobalsV::FontQuest == 0) {
                    TextLabel->SetFontName(GlobalsV::NormalFontName);
                } else if (GlobalsV::FontQuest == 1) {
                    TextLabel->SetFontName(GlobalsV::SmoothBigFontName);
                } else if (GlobalsV::FontQuest == 2) {
                    TextLabel->SetFontName(GlobalsV::SmoothHugeFontName);
                } else if (GlobalsV::FontQuest >= 3) {
                    TextLabel->SetFontName(GlobalsV::SmoothIntroFontName);
                }
            } else {
                TextLabel->SetFontName(pas::concat_wide({u"Font.", GR_Main::GiResourceSuffix(), u"Fix"}));
            }
            TextLabel->SetPosition(ClassesImports::Point(0, NextTop));
            TextLabel->SetSize(ClassesImports::Point(Panel->ClientSize.X, 1));
            TextLabel->SetWordWrapEnabled(true);
            TextLabel->SetTextAlignX(GI_Main::taxAuto);
            TextLabel->SetTextAlignY(GI_Main::tayAuto);
            if (Indent) {
                TextLabel->SetText(pas::concat_wide({u"     ", Text}));
            } else {
                TextLabel->SetText(Text);
            }
            TextLabel->SetPositionModeW(true);
            TextLabel->SetTextColor(Self->GetTextColor(GlobalsV::QuestStyleIndex));
            TextLabel->SetTextAlignY(GI_Main::tayTop);
            // Preserve native getter order: rendered line count, then line height.
            {
                std::int32_t max = std::max<std::int32_t>(1, TextLabel->GetRenderedLineCount());
                std::int32_t cpp_arg = max * TextLabel->GetLineHeight() + 4;
                std::int32_t x = TextLabel->ClientSize.X;
                TextLabel->SetSize(ClassesImports::Point(x, cpp_arg));
            }
            NextTop = TextLabel->LocalPosition.Y + TextLabel->ClientSize.Y - 2;
        }
    }

    void TfPlanetQuest::InitializeLayout() {
        std::int32_t I{};
        std::int32_t Shift{};
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fPlanetQuest... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* ImageFrame = MainPanel->FindByNameRecursive(u"ImageFrame"sv);
                ImageFrame->SetPosition(ClassesImports::Point(ImageFrame->LocalPosition.X + GR_Main::ExtraScreenWidth, ImageFrame->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* PQI = MainPanel->FindByNameRecursive(u"PQI"sv);
                PQI->SetPosition(ClassesImports::Point(PQI->LocalPosition.X + GR_Main::ExtraScreenWidth, PQI->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* AnimTextOn = MainPanel->FindByNameRecursive(u"AnimTextOn"sv);
                AnimTextOn->SetPosition(ClassesImports::Point(AnimTextOn->LocalPosition.X + GR_Main::ExtraScreenWidth, AnimTextOn->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* AnimTextOff = MainPanel->FindByNameRecursive(u"AnimTextOff"sv);
                AnimTextOff->SetPosition(ClassesImports::Point(AnimTextOff->LocalPosition.X + GR_Main::ExtraScreenWidth, AnimTextOff->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(1, 4); cpp_range.next(I); ) {
                GI_MessageLoop::TObjectGI* cpp_with_6 = MainPanel->FindByNameRecursive(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Style", SysUtils::IntToStr(I)}))));
                cpp_with_6->SetPosition(ClassesImports::Point(cpp_with_6->LocalPosition.X + GR_Main::ExtraScreenWidth, cpp_with_6->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ButtonExit = MainPanel->FindByNameRecursive(u"ButtonExit"sv);
                ButtonExit->SetPosition(ClassesImports::Point(ButtonExit->LocalPosition.X + GR_Main::ExtraScreenWidth, ButtonExit->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            I = 0;
            Shift = 39;
            if (GR_Main::ExtraScreenWidth < Shift) {
                I = Shift - GR_Main::ExtraScreenWidth;
            }
            Shift -= I;
            {
                GI_MessageLoop::TObjectGI* BGStyle = MainPanel->FindByNameRecursive(u"BGStyle"sv);
                BGStyle->SetPosition(ClassesImports::Point(BGStyle->LocalPosition.X + Shift, BGStyle->LocalPosition.Y));
                BGStyle->SetSize(ClassesImports::Point(BGStyle->ClientSize.X + (GR_Main::ExtraScreenWidth - Shift), BGStyle->ClientSize.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* BGImage = MainPanel->FindByNameRecursive(u"BGImage"sv);
                BGImage->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - BGImage->ClientSize.X, BGImage->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* PanelImage = MainPanel->FindByNameRecursive(u"PanelImage"sv);
                if (GR_Main::ExtraScreenWidth > 0 || GR_Main::ExtraScreenHeight > 0) {
                    PanelImage->SetPosition(ClassesImports::Point(0, PanelImage->LocalPosition.Y));
                    PanelImage->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                } else {
                    PanelImage->SetSize(ClassesImports::Point(PanelImage->ClientSize.X, GR_Main::GameScreenHeight));
                }
            }
            {
                GI_MessageLoop::TObjectGI* QuestPanel = MainPanel->FindByNameRecursive(u"QuestPanel"sv);
                QuestPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                {
                    GI_PanelScrollBar::TPanelScrollBarGI* MessageWindow = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(QuestPanel->FindByNameRecursive(u"MessageWindow"sv));
                    MessageWindow->SetPosition(ClassesImports::Point(MessageWindow->LocalPosition.X + Shift, MessageWindow->LocalPosition.Y));
                    MessageWindow->SetSize(ClassesImports::Point(MessageWindow->ClientSize.X + (GR_Main::ExtraScreenWidth - Shift), MessageWindow->ClientSize.Y + GR_Main::ExtraScreenHeight / 2));
                    MessageWindow->VerticalScrollBar->SetPosition(ClassesImports::Point(MessageWindow->VerticalScrollBar->LocalPosition.X + GR_Main::ExtraScreenWidth, MessageWindow->VerticalScrollBar->LocalPosition.Y));
                    MessageWindow->VerticalScrollBar->SetSize(ClassesImports::Point(MessageWindow->VerticalScrollBar->ClientSize.X, MessageWindow->VerticalScrollBar->ClientSize.Y + GR_Main::ExtraScreenHeight / 2));
                }
                {
                    GI_PanelScrollBar::TPanelScrollBarGI* ActionListWindow = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(QuestPanel->FindByNameRecursive(u"ActionListWindow"sv));
                    ActionListWindow->SetPosition(ClassesImports::Point(ActionListWindow->LocalPosition.X + Shift, ActionListWindow->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
                    ActionListWindow->SetSize(ClassesImports::Point(ActionListWindow->ClientSize.X + (GR_Main::ExtraScreenWidth - Shift), ActionListWindow->ClientSize.Y + GR_Main::ExtraScreenHeight / 2));
                    ActionListWindow->VerticalScrollBar->SetPosition(ClassesImports::Point(ActionListWindow->VerticalScrollBar->LocalPosition.X + GR_Main::ExtraScreenWidth, ActionListWindow->VerticalScrollBar->LocalPosition.Y));
                    ActionListWindow->VerticalScrollBar->SetSize(ClassesImports::Point(ActionListWindow->VerticalScrollBar->ClientSize.X, ActionListWindow->VerticalScrollBar->ClientSize.Y + GR_Main::ExtraScreenHeight / 2));
                }
            }
            {
                GI_MessageLoop::TObjectGI* LabelHelp = MainPanel->FindByNameRecursive(u"LabelHelp"sv);
                LabelHelp->SetPosition(ClassesImports::Point(LabelHelp->LocalPosition.X + GR_Main::ExtraScreenWidth, LabelHelp->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ParamsShowWindowParent = MainPanel->FindByNameRecursive(u"ParamsShowWindowParent"sv);
                ParamsShowWindowParent->SetPosition(ClassesImports::Point(ParamsShowWindowParent->LocalPosition.X + GR_Main::ExtraScreenWidth, ParamsShowWindowParent->LocalPosition.Y));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        ParameterPanelOrigin = GetByName(u"ParamsShowWindowParent"sv)->LocalPosition;
        SetHelpCallback(pas::bind_method<&TfPlanetQuest::ShowControlHelp>(this));
        {
            GI_GraphButton::TGraphButtonGI* AnimTextOn_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"AnimTextOn"sv));
            AnimTextOn_2->UserValue = 0;
            AnimTextOn_2->UpCallback = pas::bind_method<&TfPlanetQuest::SelectPageMode>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* AnimTextOff_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"AnimTextOff"sv));
            AnimTextOff_2->UserValue = 1;
            AnimTextOff_2->UpCallback = pas::bind_method<&TfPlanetQuest::SelectPageMode>(this);
        }
        I = 1;
        while (true) {
            Control = FindControlByPath(pas::concat_wide({u"Style", EC_Str::IntToWideString(I)}));
            if (Control == nullptr) {
                break;
            }
            {
                GI_GraphButton::TGraphButtonGI* cpp_with_18 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control);
                cpp_with_18->UserValue = I - 1;
                cpp_with_18->UpCallback = pas::bind_method<&TfPlanetQuest::SelectStyle>(this);
                cpp_with_18->DownCallback = pas::bind_method<&TfPlanetQuest::SelectStyle>(this);
            }
            ++I;
        }
        QuestStyleCount = I - 1;
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfPlanetQuest::QuestKeyDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButtonExit"sv))->UpCallback = pas::bind_method<&TfPlanetQuest::RequestLoadGame>(this);
        QuestPlayerInterface = pas::construct_call<TTextQuestPlayerInterface>(TextQuestInterface::TTextQuestInterface_Create);
    }

    void TfPlanetQuest::OnOpen() {
        std::int32_t I{};
        std::int32_t J{};
        aRanger::PQuest GovernmentQuest{};
        std::uint8_t Found{};
        std::int32_t Stage = 0;
        try {
            SelectMusic();
            ActiveQueuedTextQuest = nullptr;
            if (aGalaxy::Galaxy != nullptr) {
                EC_Cache::EvictMainMenuShipCachesWhenAddressSpaceHigh();
            }
            Stage = 1;
            if (aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnEnteringForm, nullptr, nullptr, 0);
            }
            Stage = 2;
            if (GlobalsV::QuestStyleIndex < 0 || GlobalsV::QuestStyleIndex >= QuestStyleCount) {
                GlobalsV::QuestStyleIndex = 0;
            }
            GetByName(u"PQI"sv)->SetActive(false);
            CurrentPicture = pas::WideString();
            pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"QuestPanel"sv))->SetActive(true);
            Stage = 3;
            ClearChoices();
            SetQuestText(u""_wref.get());
            ClearParameterPanel();
            LayoutParameterPanel();
            Stage = 4;
            if (pas::list_count(aScript::QueuedTextQuests) == 0 && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet->TextQuestId >= aGalaxyStruct::FirstLicensedQuestId && (GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"sv)->CountBlocks(u"PlanetQuestLic"_wref.get()) <= 0 || ([&] {
                pas::WideString cpp_string = ([&] {
                    const pas::WideString& intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->CurrentPlanet->TextQuestId);
                    EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"sv)->GetBlock(u"PlanetQuestLic"sv);
                    return block->GetParamOrMarker(pas::view(intToStr));
                }());
                pas::WideString cpp_string_2 = TfPlanetQuest::GetQuestContentHash(aPlayer::GetPlayer()->CurrentPlanet->TextQuestId);
                return cpp_string == cpp_string_2;
            }()))) {
                MoneyLimitComplement = aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiHuman) + aPlayer::GetPlayer()->Money ^ 0xffffffffu;
            } else if (aPlayer::GetPlayer() != nullptr && pas::list_count(aScript::QueuedTextQuests) == 0) {
                MoneyLimitComplement = aPlayer::GetPlayer()->Money + 50000 ^ 0xffffffffu;
            } else {
                MoneyLimitComplement = 1000000000 ^ 0xffffffffu;
            }
            Stage = 5;
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->InPrison) {
                Stage = 6;
                Quest = pas::construct_call<TextQuest::TTextQuest>(TextQuest::TTextQuest_Create);
                if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId != aGalaxyStruct::oiPirate) {
                    LoadQuestByName(u"Prison"_wref.get());
                } else {
                    LoadQuestByName(u"PirateClanPrison"_wref.get());
                }
                Stage = 7;
                Quest->ToStarText->Text = aPlayer::GetPlayer()->CurrentStar->Name;
                Quest->ToPlanetText->Text = aPlayer::GetPlayer()->CurrentPlanet->Name;
                Quest->DateText->Text = pas::WideString();
                Quest->MoneyText->Text = pas::WideString();
                Quest->FromPlanetText->Text = aPlayer::GetPlayer()->CurrentPlanet->Name;
                Quest->FromStarText->Text = aPlayer::GetPlayer()->CurrentStar->Name;
                Quest->RangerText->Text = aPlayer::GetPlayer()->Name;
                Stage = 8;
                StartLoadedQuest();
                Stage = 9;
            } else if (pas::list_count(aScript::QueuedTextQuests) > 0) {
                Stage = 10;
                ActiveQueuedTextQuest = pas::list_at<aScript::TScriptTQRequest>(aScript::QueuedTextQuests, 0);
                Quest = pas::construct_call<TextQuest::TTextQuest>(TextQuest::TTextQuest_Create);
                if (EC_Str::IsIntegerTextW(pas::view(ActiveQueuedTextQuest->Name))) {
                    LoadQuestById(SysUtils::StrToInt(static_cast<pas::AnsiString>(ActiveQueuedTextQuest->Name)));
                } else {
                    LoadQuestByName(ActiveQueuedTextQuest->Name);
                }
                Stage = 11;
                if (aPlayer::GetPlayer()->CurrentStar != nullptr) {
                    Quest->ToStarText->Text = aPlayer::GetPlayer()->CurrentStar->Name;
                    Quest->FromStarText->Text = aPlayer::GetPlayer()->CurrentStar->Name;
                } else {
                    Quest->ToStarText->Text = pas::WideString();
                    Quest->FromStarText->Text = pas::WideString();
                }
                if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                    Quest->ToPlanetText->Text = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    Quest->FromPlanetText->Text = aPlayer::GetPlayer()->CurrentPlanet->Name;
                } else {
                    Quest->ToPlanetText->Text = pas::WideString();
                    Quest->FromPlanetText->Text = pas::WideString();
                }
                Quest->DateText->Text = pas::WideString();
                Quest->MoneyText->Text = pas::WideString();
                Quest->RangerText->Text = aPlayer::GetPlayer()->Name;
                Stage = 12;
                StartLoadedQuest();
                Stage = 13;
            } else if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(Globals::StandaloneQuestMode ^ 1)) {
                Stage = 14;
                Found = false;
                if (aPlayer::GetPlayer()->CurrentPlanet->TextQuestId > -1) {
                    if (pas::list_count(aPlayer::GetPlayer()->Quests) > 0) {
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Quests) - 1); cpp_range.next(I); ) {
                            GovernmentQuest = pas::list_at<aRanger::TQuest>(aPlayer::GetPlayer()->Quests, I);
                            if (GovernmentQuest->QuestType == aGalaxyStruct::qtPlanetQuest && pas::class_cast_if<aPlanet::TPlanet*>(GovernmentQuest->ObjectiveTarget) != nullptr && pas::checked_cast<aPlanet::TPlanet*>(GovernmentQuest->ObjectiveTarget) == aPlayer::GetPlayer()->CurrentPlanet) {
                                Stage = 15;
                                ActiveGovernmentQuest = GovernmentQuest;
                                Quest = pas::construct_call<TextQuest::TTextQuest>(TextQuest::TTextQuest_Create);
                                LoadQuestById(aPlayer::GetPlayer()->CurrentPlanet->TextQuestId);
                                Stage = 16;
                                Quest->ToStarText->Text = aPlayer::GetPlayer()->CurrentStar->Name;
                                Quest->ToPlanetText->Text = aPlayer::GetPlayer()->CurrentPlanet->Name;
                                Quest->DateText->Text = aGalaxy::Galaxy->FormatTurnDate(GovernmentQuest->DeadlineTurn);
                                Quest->MoneyText->Text = pas::wide_int_to_str(GovernmentQuest->RewardMoney);
                                Quest->FromPlanetText->Text = GovernmentQuest->Planet->Name;
                                Quest->FromStarText->Text = GovernmentQuest->Planet->CurrentStar->Name;
                                Quest->RangerText->Text = aPlayer::GetPlayer()->Name;
                                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Quest->GetParameterCount()); cpp_range_2.next(J); ) {
                                    if (Quest->GetParameter(J)->Enabled && Quest->GetParameter(J)->NameText->Text == u"GRewardMoney") {
                                        Quest->GetParameter(J)->Value = GovernmentQuest->RewardMoney;
                                        break;
                                    }
                                }
                                Stage = 17;
                                StartLoadedQuest();
                                Stage = 18;
                                Found = true;
                                break;
                            }
                        }
                    }
                }
                if (!Found) {
                    GlobalsV::RequestedScreenId = GlobalsV::QuestReturnScreenId;
                    RequestClose(1);
                }
            } else {
                Stage = 19;
                Quest = pas::construct_call<TextQuest::TTextQuest>(TextQuest::TTextQuest_Create);
                if (EC_Str::IsIntegerTextW(pas::view(GlobalsV::PendingQuestName))) {
                    LoadQuestById(SysUtils::StrToInt(static_cast<pas::AnsiString>(GlobalsV::PendingQuestName)));
                } else {
                    LoadQuestByName(GlobalsV::PendingQuestName);
                }
                Stage = 20;
                Quest->ToStarText->Text = aConst::LocalizedText(u"FormLoadQuest.PToStar"_wref.get());
                Quest->ToPlanetText->Text = aConst::LocalizedText(u"FormLoadQuest.PToPlanet"_wref.get());
                Quest->DateText->Text = aGalaxy::FormatGameTurnDate(1500);
                Quest->MoneyText->Text = u"10000"_w;
                Quest->FromPlanetText->Text = aConst::LocalizedText(u"FormLoadQuest.PFromPlanet"_wref.get());
                Quest->FromStarText->Text = aConst::LocalizedText(u"FormLoadQuest.PFromStar"_wref.get());
                Quest->RangerText->Text = aConst::LocalizedText(u"FormLoadQuest.PRanger"_wref.get());
                Stage = 21;
                StartLoadedQuest();
                Stage = 22;
            }
            Stage = 23;
            ApplyStyle();
            Stage = 24;
            reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"Style", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1)}))))->ExecuteOnPressCode();
            Stage = 25;
            if (aGalaxy::Galaxy != nullptr) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum(150);
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TfPlanetQuest.BeforeRun, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfPlanetQuest::OnClose() {
        std::int64_t Money{};
        std::int64_t CappedMoney{};
        aPlayer::TPlayer* Player{};
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(151);
        }
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnLeavingForm, nullptr, nullptr, 0);
        }
        QuestId = -1;
        if (PageAnimationTimer != nullptr) {
            CancelCallbackTimer(PageAnimationTimer);
            PageAnimationTimer = nullptr;
        }
        ClearParameterPanel();
        ClearChoices();
        if (aPlayer::GetPlayer() != nullptr) {
            Player = aPlayer::GetPlayer();
            Money = aPlayer::GetPlayer()->Money;
            // The native inlined Int64 minimum compares an unsigned limit with signed money.
            if (static_cast<std::int64_t>(MoneyLimitComplement ^ 0xffffffffu) < Money) {
                CappedMoney = MoneyLimitComplement ^ 0xffffffffu;
            } else {
                CappedMoney = Money;
            }
            Player->SetMoney(CappedMoney);
        }
        if (Quest != nullptr) {
            pas::free(Quest);
            Quest = nullptr;
        }
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ProcessQuestTimersAndOutcomes();
        }
        if (GlobalsV::RequestedScreenId == GlobalsV::screenMainMenu || GlobalsV::RequestedScreenId == GlobalsV::screenLoad) {
            aScript::ClearPendingScriptRequests();
        }
    }

    // StyleIndex is zero-based.
    pas::WideString TfPlanetQuest::GetTextColorTag(std::int32_t StyleIndex) {
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(u"Style"_wref.get());
        if (Block->CountBlocks(u"QTextColor"_wref.get()) > 0) {
            Block = Block->GetBlock(u"QTextColor"sv);
            if (Block->CountParams(pas::concat_wide({u"Sel", EC_Str::IntToWideString(StyleIndex + 1)})) > 0) {
                return pas::concat_wide({u"<color=", Block->GetParam(pas::view(pas::concat_wide({u"Sel", EC_Str::IntToWideString(StyleIndex + 1)}))), u">"});
            }
        }
        if (StyleIndex == 0) {
            return static_cast<pas::WideString>(pas::concat_ansi({"<color=", SysUtils::IntToStr(255), ",", SysUtils::IntToStr(240), ",", SysUtils::IntToStr(100), ">"}));
        } else if (StyleIndex == 1) {
            return static_cast<pas::WideString>(pas::concat_ansi({"<color=", SysUtils::IntToStr(255), ",", SysUtils::IntToStr(240), ",", SysUtils::IntToStr(100), ">"}));
        } else if (StyleIndex == 2) {
            return static_cast<pas::WideString>(pas::concat_ansi({"<color=", SysUtils::IntToStr(0), ",", SysUtils::IntToStr(4), ",", SysUtils::IntToStr(173), ">"}));
        } else if (StyleIndex == 3) {
            return static_cast<pas::WideString>(pas::concat_ansi({"<color=", SysUtils::IntToStr(0), ",", SysUtils::IntToStr(4), ",", SysUtils::IntToStr(173), ">"}));
        } else {
            return GetTextColorTag(0);
        }
    }

    std::uint32_t TfPlanetQuest::GetTextColor(std::int32_t StyleIndex) {
        pas::WideString S{};
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(u"Style"_wref.get());
        if (Block->CountBlocks(u"QTextColor"_wref.get()) > 0) {
            Block = Block->GetBlock(u"QTextColor"sv);
            if (Block->CountParams(pas::concat_wide({u"Text", EC_Str::IntToWideString(StyleIndex + 1)})) > 0) {
                S = Block->GetParam(pas::view(pas::concat_wide({u"Text", EC_Str::IntToWideString(StyleIndex + 1)})));
                std::int32_t extractDigitsToIntW = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(S), 2, u","sv)));
                std::int32_t extractDigitsToIntW_2 = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(S), 1, u","sv)));
                std::int32_t extractDigitsToIntW_3 = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(S), 0, u","sv)));
                return GR_Main::CurrentPixelFormat->PackRgb(extractDigitsToIntW_3, extractDigitsToIntW_2, extractDigitsToIntW);
            }
        }
        if (StyleIndex == 0) {
            return GR_Main::CurrentPixelFormat->PackRgbBytes(212, 208, 180);
        } else if (StyleIndex == 1) {
            return GR_Main::CurrentPixelFormat->PackRgbBytes(212, 208, 180);
        } else if (StyleIndex == 2) {
            return GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0);
        } else if (StyleIndex == 3) {
            return GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0);
        } else {
            return GetTextColor(0);
        }
    }

    std::uint32_t TfPlanetQuest::GetDisabledTextColor(std::int32_t StyleIndex) {
        pas::WideString S{};
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(u"Style"_wref.get());
        if (Block->CountBlocks(u"QTextColor"_wref.get()) > 0) {
            Block = Block->GetBlock(u"QTextColor"sv);
            if (Block->CountParams(pas::concat_wide({u"Grey", EC_Str::IntToWideString(StyleIndex + 1)})) > 0) {
                S = Block->GetParam(pas::view(pas::concat_wide({u"Grey", EC_Str::IntToWideString(StyleIndex + 1)})));
                std::int32_t extractDigitsToIntW = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(S), 2, u","sv)));
                std::int32_t extractDigitsToIntW_2 = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(S), 1, u","sv)));
                std::int32_t extractDigitsToIntW_3 = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(S), 0, u","sv)));
                return GR_Main::CurrentPixelFormat->PackRgb(extractDigitsToIntW_3, extractDigitsToIntW_2, extractDigitsToIntW);
            }
        }
        if (StyleIndex == 0) {
            return GR_Main::CurrentPixelFormat->PackRgbBytes(120, 120, 120);
        } else if (StyleIndex == 1) {
            return GR_Main::CurrentPixelFormat->PackRgbBytes(120, 120, 120);
        } else if (StyleIndex == 2) {
            return GR_Main::CurrentPixelFormat->PackRgbBytes(120, 120, 120);
        } else if (StyleIndex == 3) {
            return GR_Main::CurrentPixelFormat->PackRgbBytes(120, 120, 120);
        } else {
            return GetDisabledTextColor(0);
        }
    }

    void TfPlanetQuest::ApplyStyle() {
        std::int32_t I{};
        pas::WideString Path{};
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"AnimTextOn"sv))->SetActive(GlobalsV::QuestPageAnimationEnabled);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"AnimTextOff"sv))->SetActive(static_cast<std::uint8_t>(GlobalsV::QuestPageAnimationEnabled ^ 1));
        for (auto cpp_range = pas::for_to<std::int32_t>(1, QuestStyleCount); cpp_range.next(I); ) {
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Style", SysUtils::IntToStr(I)})))))->SetDisabled(GlobalsV::QuestStyleIndex == I - 1);
        }
        {
            pas::WideString cpp_arg = pas::concat_wide({u"GI,Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1)});
            GI_Image::TImageGI* cpp_arg_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGStyle"sv));
            cpp_arg_2->SetImagePath(std::move(cpp_arg));
        }
        {
            const pas::WideString& cpp_arg_3 = pas::concat_wide({u"Style.ScrollBar.", GR_Main::GiResourceSuffix(), u"PQS", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1)});
            GI_ScrollBar::TScrollBarGI* verticalScrollBar = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"MessageWindow"sv))->VerticalScrollBar;
            verticalScrollBar->SetConfigPath(cpp_arg_3);
        }
        {
            const pas::WideString& cpp_arg_4 = pas::concat_wide({u"Style.ScrollBar.", GR_Main::GiResourceSuffix(), u"PQS", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1)});
            GI_ScrollBar::TScrollBarGI* verticalScrollBar_2 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"ActionListWindow"sv))->VerticalScrollBar;
            verticalScrollBar_2->SetConfigPath(cpp_arg_4);
        }
        GI_MessageLoop::TObjectGI* Control = GetByName(u"MessageWindow"sv)->FirstChild;
        while (Control != nullptr) {
            if (GI_Label::TLabelGI* labelGI = pas::class_cast_if<GI_Label::TLabelGI*>(Control)) {
                GI_Label::TLabelGI* cpp_with = labelGI;
                {
                    std::uint32_t textColor = GetTextColor(GlobalsV::QuestStyleIndex);
                    GI_Label::TLabelGI* cpp_arg_5 = cpp_with;
                    cpp_arg_5->SetTextColor(textColor);
                }
                {
                    const pas::WideString& replaceAllWideString = ([&] {
                        const pas::WideString& textColorTag = GetTextColorTag(GlobalsV::QuestStyleIndex);
                        const pas::WideString& textColorTag_2 = GetTextColorTag(PreviousStyleIndex);
                        const pas::WideString& text = cpp_with->GetText();
                        return EC_Str::ReplaceAllWideString(text, textColorTag_2, pas::view(textColorTag));
                    }());
                    GI_Label::TLabelGI* cpp_arg_6 = cpp_with;
                    cpp_arg_6->SetText(replaceAllWideString);
                }
            }
            Control = Control->NextSibling;
        }
        Control = GetByName(u"ActionListWindow"sv)->FirstChild;
        while (Control != nullptr) {
            if (Control->FirstChild != nullptr) {
                {
                    GI_Image::TImageGI* cpp_with_2 = pas::checked_cast<GI_Image::TImageGI*>(Control->FirstChild);
                    Path = pas::concat_wide({u"Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1), u"Line"});
                    if (GR_Main::CacheDataRoot->FileExistsByPath(Path)) {
                        cpp_with_2->SetImagePath(pas::concat_wide({u"GI,", Path}));
                    } else {
                        cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S1Line"}));
                    }
                    cpp_with_2->SetImageKindX(GI_Main::ikxLeftFill);
                    cpp_with_2->SetImageKindY(GI_Main::ikyTopFill);
                }
                {
                    GI_Label::TLabelGI* cpp_with_3 = pas::checked_cast<GI_Label::TLabelGI*>(Control->FirstChild->NextSibling);
                    if (cpp_with_3->UserState == 1) {
                        std::uint32_t disabledTextColor = GetDisabledTextColor(GlobalsV::QuestStyleIndex);
                        GI_Label::TLabelGI* cpp_arg_7 = cpp_with_3;
                        cpp_arg_7->SetTextColor(disabledTextColor);
                    } else {
                        std::uint32_t textColor_2 = GetTextColor(GlobalsV::QuestStyleIndex);
                        GI_Label::TLabelGI* cpp_arg_8 = cpp_with_3;
                        cpp_arg_8->SetTextColor(textColor_2);
                    }
                    {
                        const pas::WideString& replaceAllWideString_2 = ([&] {
                            const pas::WideString& textColorTag_3 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                            const pas::WideString& textColorTag_4 = GetTextColorTag(PreviousStyleIndex);
                            const pas::WideString& text_2 = cpp_with_3->GetText();
                            return EC_Str::ReplaceAllWideString(text_2, textColorTag_4, pas::view(textColorTag_3));
                        }());
                        GI_Label::TLabelGI* cpp_arg_9 = cpp_with_3;
                        cpp_arg_9->SetText(replaceAllWideString_2);
                    }
                }
                if (Control->FirstChild->NextSibling->FirstChild != nullptr) {
                    GI_Image::TImageGI* cpp_with_4 = pas::checked_cast<GI_Image::TImageGI*>(Control->FirstChild->NextSibling->FirstChild->FirstChild);
                    Path = pas::concat_wide({u"Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S", EC_Str::IntToWideString(GlobalsV::QuestStyleIndex + 1), u"Answer"});
                    if (Control->FirstChild->NextSibling->UserState == 1) {
                        if (GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({Path, u"H"}))) {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,", Path, u"H"}));
                        } else {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S1AnswerH"}));
                        }
                    } else if (GR_Main::CacheDataRoot->FileExistsByPath(Path)) {
                        cpp_with_4->SetImagePath(pas::concat_wide({u"GI,", Path}));
                    } else {
                        cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormPQuest2.", GR_Main::GiResourceSuffix(), u"S1Answer"}));
                    }
                }
            }
            Control = Control->NextSibling;
        }
    }

    void TfPlanetQuest::SelectPageMode(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Filename{};
        if (static_cast<std::uint8_t>(Sender->UserValue) != GlobalsV::QuestPageAnimationEnabled) {
            GlobalsV::QuestPageAnimationEnabled = Sender->UserValue;
            GR_Main::UserSettingsConfig->SetOrAddParam(u"PQuestAnim"_wref.get(), EC_Str::BoolToWideString(GlobalsV::QuestPageAnimationEnabled));
            Filename = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"cfg.txt"});
            GR_Main::UserSettingsConfig->SaveTextFile(Filename.pchar(), true, false);
            if (GlobalsV::QuestPageAnimationEnabled) {
                ShowControlHelp(GetByName(u"AnimTextOn"sv), true);
            } else {
                ShowControlHelp(GetByName(u"AnimTextOff"sv), true);
            }
            ApplyStyle();
        }
    }

    void TfPlanetQuest::SelectStyle(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString cpp_text{};
        if (GlobalsV::QuestStyleIndex != Sender->UserValue) {
            PreviousStyleIndex = GlobalsV::QuestStyleIndex;
            GlobalsV::QuestStyleIndex = Sender->UserValue;
            GR_Main::UserSettingsConfig->SetOrAddParam(u"PQuestStyle"_wref.get(), pas::wide_int_to_str(GlobalsV::QuestStyleIndex));
            {
                char16_t* cpp_arg = (cpp_text = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"cfg.txt"}), cpp_text.pchar());
                EC_BlockPar::TBlockParEC* userSettingsConfig = GR_Main::UserSettingsConfig;
                userSettingsConfig->SaveTextFile(cpp_arg, true, false);
            }
            ApplyStyle();
        }
    }

    void TfPlanetQuest::ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible) {
        GI_Label::TLabelGI* TextLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LabelHelp"sv));
        if (Sender->HelpText == u"") {
            Visible = false;
        }
        TextLabel->SetActive(Visible);
        if (Sender->ControlName == u"ButtonExit" && ActiveQueuedTextQuest != nullptr) {
            TextLabel->SetText(aConst::LocalizedText(u"FormPQuest.HelpExitAlt"_wref.get()));
        } else {
            TextLabel->SetText(Sender->HelpText);
        }
    }

    // Suppresses repeated picture names.
    void TfPlanetQuest::SetQuestPicture(pas::WideString Name) {
        GI_GraphBuf::TGraphBufGI* Image{};
        if (CurrentPicture != Name) {
            CurrentPicture = Name;
            Image = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"PQI"sv));
            Image->SetActive(true);
            Image->LoadBitmapPathAsRgb(pas::concat_wide({u"Bm.PQI.", Name, u"?RGB"}));
            if (GR_Main::GiResourceVariant() == 1) {
                std::int32_t round = System::Round(pas::real_divide(static_cast<std::uint32_t>(Image->GraphBuf->Height) * 800, 1024.0L));
                std::int32_t round_2 = System::Round(pas::real_divide(static_cast<std::uint32_t>(Image->GraphBuf->Width) * 800, 1024.0L));
                Image->GraphBuf->RescaleRgb(round_2, round);
            }
            Image->GraphBuf->ConvertRgbTo565();
            Image->Invalidate();
        }
    }

    void TfPlanetQuest::RequestLoadGame(GI_MessageLoop::TObjectGI* Sender) {
        std::uint8_t Standalone{};
        if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormGameMenu.QExit"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
            Standalone = aGalaxy::Galaxy == nullptr;
            aScript::ClearPendingScriptRequests();
            if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
                pas::free(aSaveLoad::MemorySnapshotBuffer);
            }
            aSaveLoad::MemorySnapshotBuffer = nullptr;
            GlobalsV::MemorySnapshotActive = false;
            if (aGalaxy::Galaxy != nullptr) {
                if (!aGalaxy::Galaxy->Destroying) {
                    pas::free(aGalaxy::Galaxy);
                }
            }
            aGalaxy::Galaxy = nullptr;
            Globals::ScreenLoadMode = 4;
            if (Standalone) {
                GlobalsV::PostLoadScreenId = GlobalsV::QuestReturnScreenId;
            } else {
                GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
            }
            GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
            RequestClose(1);
        }
    }

    void TfPlanetQuest::QuestKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey) {
        std::int32_t Index{};
        GI_PanelScrollBar::TPanelScrollBarGI* Panel{};
        GI_MessageLoop::TObjectGI* Choice{};
        GI_MessageLoop::TObjectGI* Control{};
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (VirtualKey == WindowsSdk::VK_ESCAPE) {
                RequestLoadGame(nullptr);
            }
            if (VirtualKey >= '1' && VirtualKey <= '9') {
                Index = VirtualKey - '1';
                Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"ActionListWindow"sv));
                Control = Panel->FirstChild;
                while (Control != nullptr && (!(pas::class_cast_if<GI_Panel::TPanelGI*>(Control) != nullptr) || pas::load_unaligned<void*>(pas::byte_offset(&Control->LeftButtonUpCallback, offsetof(SystemImports::TMethod, Code))) != pas::method_address<&TfPlanetQuest::ChoiceMouseUp>() && pas::load_unaligned<void*>(pas::byte_offset(&Control->LeftButtonUpCallback, offsetof(SystemImports::TMethod, Code))) != pas::static_method_address<&TfPlanetQuest::DisabledChoiceMouseUp>())) {
                    Control = Control->NextSibling;
                }
                Choice = Control;
                while (Choice != nullptr && Index >= 0) {
                    if (Index == 0) {
                        Choice->LeftButtonUpCallback(Choice, 0u, ClassesImports::Point(0, 0));
                        return;
                    } else {
                        --Index;
                        Control = Choice->NextSibling;
                        while (Control != nullptr && (!(pas::class_cast_if<GI_Panel::TPanelGI*>(Control) != nullptr) || pas::load_unaligned<void*>(pas::byte_offset(&Control->LeftButtonUpCallback, offsetof(SystemImports::TMethod, Code))) != pas::method_address<&TfPlanetQuest::ChoiceMouseUp>() && pas::load_unaligned<void*>(pas::byte_offset(&Control->LeftButtonUpCallback, offsetof(SystemImports::TMethod, Code))) != pas::static_method_address<&TfPlanetQuest::DisabledChoiceMouseUp>())) {
                            Control = Control->NextSibling;
                        }
                        Choice = Control;
                    }
                }
            }
            if (VirtualKey == 'R') {
                if (ActiveQueuedTextQuest == nullptr) {
                    if (Globals::SaveManagerScreen->AutoSaveExists()) {
                        if (GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgLoad"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                            if (aPlayer::GetPlayer() != nullptr) {
                                GlobalsV::PendingLoadFileName = static_cast<pas::AnsiString>(fSaveManager::TfSaveManager::GetAutoSavePath());
                                GlobalsV::RequestedScreenId = GlobalsV::screenGameLoad;
                                RequestClose(1);
                            } else {
                                Globals::StandaloneQuestMode = true;
                                GlobalsV::RequestedScreenId = GlobalsV::screenPlanetQuest;
                                GlobalsV::CurrentScreenId = GlobalsV::screenNone;
                                RequestClose(1);
                            }
                        }
                    }
                }
            }
        }
    }

    void TfPlanetQuest::SelectMusic() {
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
        } else {
            GR_Main::MusicManager->PlayCategory(u"Quest"_wref.get());
        }
    }

    pas::WideString TfPlanetQuest::ExpandTemplateText(pas::WideString Text) {
        pas::WideString Result{};
        pas::WideString Expanded{};
        pas::WideString SourceLineBreak{};
        pas::WideString ReplacementLineBreak{};
        pas::WideString IndentedLineBreak{};
        if (aPlayer::GetPlayer() == nullptr) {
            CurrentDate = EC_Str::TrimWideString(aGalaxy::Galaxy->FormatTurnDate(DaysElapsed + aGalaxyStruct::GalaxyWarmupTurns));
        } else {
            CurrentDate = EC_Str::TrimWideString(aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn));
        }
        Expanded = ExpandExternalText(Text);
        SourceLineBreak = u"\r\n"_w;
        ReplacementLineBreak = u"\r\n"_w;
        IndentedLineBreak = u"\r\n          "_w;
        Expanded = ([&] {
            const pas::WideString& wrapTextInColor = ([&] {
                pas::WideString textColorTag = GetTextColorTag(GlobalsV::QuestStyleIndex);
                pas::WideString trimWideString = EC_Str::TrimWideString(Quest->ToStarText->Text);
                return aMyFunction::WrapTextInColor(pas::view(std::move(trimWideString)), pas::view(std::move(textColorTag)));
            }());
            const pas::WideString& expanded = Expanded;
            return EC_Str::ReplaceAllWideString(expanded, u"<ToStar>"_wref.get(), pas::view(wrapTextInColor));
        }());
        Expanded = ([&] {
            const pas::WideString& wrapTextInColor_2 = ([&] {
                pas::WideString textColorTag_2 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                pas::WideString trimWideString_2 = EC_Str::TrimWideString(Quest->ToPlanetText->Text);
                return aMyFunction::WrapTextInColor(pas::view(std::move(trimWideString_2)), pas::view(std::move(textColorTag_2)));
            }());
            const pas::WideString& expanded_2 = Expanded;
            return EC_Str::ReplaceAllWideString(expanded_2, u"<ToPlanet>"_wref.get(), pas::view(wrapTextInColor_2));
        }());
        Expanded = ([&] {
            const pas::WideString& wrapTextInColor_3 = ([&] {
                pas::WideString textColorTag_3 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                pas::WideString text = Quest->DateText->Text;
                return aMyFunction::WrapTextInColor(pas::view(std::move(text)), pas::view(std::move(textColorTag_3)));
            }());
            const pas::WideString& expanded_3 = Expanded;
            return EC_Str::ReplaceAllWideString(expanded_3, u"<Date>"_wref.get(), pas::view(wrapTextInColor_3));
        }());
        Expanded = ([&] {
            const pas::WideString& wrapTextInColor_4 = ([&] {
                pas::WideString textColorTag_4 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                pas::WideString text_2 = Quest->MoneyText->Text;
                return aMyFunction::WrapTextInColor(pas::view(std::move(text_2)), pas::view(std::move(textColorTag_4)));
            }());
            const pas::WideString& expanded_4 = Expanded;
            return EC_Str::ReplaceAllWideString(expanded_4, u"<Money>"_wref.get(), pas::view(wrapTextInColor_4));
        }());
        Expanded = ([&] {
            const pas::WideString& wrapTextInColor_5 = ([&] {
                pas::WideString textColorTag_5 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                pas::WideString trimWideString_3 = EC_Str::TrimWideString(Quest->FromPlanetText->Text);
                return aMyFunction::WrapTextInColor(pas::view(std::move(trimWideString_3)), pas::view(std::move(textColorTag_5)));
            }());
            const pas::WideString& expanded_5 = Expanded;
            return EC_Str::ReplaceAllWideString(expanded_5, u"<FromPlanet>"_wref.get(), pas::view(wrapTextInColor_5));
        }());
        Expanded = ([&] {
            const pas::WideString& wrapTextInColor_6 = ([&] {
                pas::WideString textColorTag_6 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                pas::WideString trimWideString_4 = EC_Str::TrimWideString(Quest->FromStarText->Text);
                return aMyFunction::WrapTextInColor(pas::view(std::move(trimWideString_4)), pas::view(std::move(textColorTag_6)));
            }());
            const pas::WideString& expanded_6 = Expanded;
            return EC_Str::ReplaceAllWideString(expanded_6, u"<FromStar>"_wref.get(), pas::view(wrapTextInColor_6));
        }());
        Expanded = ([&] {
            const pas::WideString& wrapTextInColor_7 = ([&] {
                pas::WideString textColorTag_7 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                pas::WideString trimWideString_5 = EC_Str::TrimWideString(Quest->RangerText->Text);
                return aMyFunction::WrapTextInColor(pas::view(std::move(trimWideString_5)), pas::view(std::move(textColorTag_7)));
            }());
            const pas::WideString& expanded_7 = Expanded;
            return EC_Str::ReplaceAllWideString(expanded_7, u"<Ranger>"_wref.get(), pas::view(wrapTextInColor_7));
        }());
        Expanded = ([&] {
            const pas::WideString& wrapTextInColor_8 = ([&] {
                pas::WideString textColorTag_8 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                pas::WideString currentDate = CurrentDate;
                return aMyFunction::WrapTextInColor(pas::view(std::move(currentDate)), pas::view(std::move(textColorTag_8)));
            }());
            const pas::WideString& expanded_8 = Expanded;
            return EC_Str::ReplaceAllWideString(expanded_8, u"<CurDate>"_wref.get(), pas::view(wrapTextInColor_8));
        }());
        Expanded = EC_Str::ReplaceAllWideString(Expanded, SourceLineBreak, pas::view(ReplacementLineBreak));
        Expanded = EC_Str::ReplaceAllWideString(Expanded, IndentedLineBreak, pas::view(ReplacementLineBreak));
        if (pas::pos(u"<", Expanded) > 0) {
            Expanded = EC_Str::ReplaceAllWideString(Expanded, u"<br>"_wref.get(), u"\r\n"sv);
            Expanded = EC_Str::ReplaceAllWideString(Expanded, u"<ll>"_wref.get(), u"\r\n \r\n"sv);
            // Native quirk: <Player> is expanded into Result, then overwritten below.
            if (aPlayer::GetPlayer() != nullptr) {
                Result = ([&] {
                    const pas::WideString& wrapTextInColor_9 = ([&] {
                        pas::WideString textColorTag_9 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                        pas::WideString name = aPlayer::GetPlayer()->Name;
                        return aMyFunction::WrapTextInColor(pas::view(std::move(name)), pas::view(std::move(textColorTag_9)));
                    }());
                    const pas::WideString& result = Result;
                    return EC_Str::ReplaceAllWideString(result, u"<Player>"_wref.get(), pas::view(wrapTextInColor_9));
                }());
            }
            Expanded = ([&] {
                const pas::WideString& textColorTag_10 = GetTextColorTag(GlobalsV::QuestStyleIndex);
                const pas::WideString& expanded_9 = Expanded;
                return EC_Str::ReplaceAllWideString(expanded_9, u"<clr>"_wref.get(), pas::view(textColorTag_10));
            }());
            Expanded = EC_Str::ReplaceAllWideString(Expanded, u"<clrEnd>"_wref.get(), pas::view(aMyFunction::EndColorTag));
        }
        return Expanded;
    }

    // Empty callback.
    void TfPlanetQuest::IgnoreChoice(std::int32_t Value) {
    }

    void TfPlanetQuest::ContinueToLocation(std::int32_t LocationId) {
        ClearChoices();
        Quest->EnterLocation(LocationId);
    }

    void TfPlanetQuest::ContinueAlongPath(std::int32_t PathId) {
        ClearChoices();
        Quest->FollowPath(PathId);
    }

    // Value is unused.
    void TfPlanetQuest::ContinueToOutcome(std::int32_t Value) {
        ClearChoices();
        Quest->ShowOutcome();
    }

    // Queued script quests report status 2.
    void TfPlanetQuest::CompleteQuestSuccess(std::int32_t Value) {
        pas::WideString News{};
        pas::WideString ItemName{};
        aRanger::PQuest GovernmentQuest{};
        std::int32_t I{};
        aItem::TUselessItem* Item{};
        ClearChoices();
        if (ActiveQueuedTextQuest == nullptr) {
            if (aPlayer::GetPlayer() != nullptr) {
                if (!aPlayer::GetPlayer()->InPrison) {
                    if (pas::list_count(aPlayer::GetPlayer()->Quests) > 0) {
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Quests) - 1); cpp_range.next(I); ) {
                            GovernmentQuest = pas::list_at<aRanger::TQuest>(aPlayer::GetPlayer()->Quests, I);
                            if (GovernmentQuest->QuestType == aGalaxyStruct::qtPlanetQuest && pas::class_cast_if<aPlanet::TPlanet*>(GovernmentQuest->ObjectiveTarget) != nullptr && aPlayer::GetPlayer()->CurrentPlanet == pas::checked_cast<aPlanet::TPlanet*>(GovernmentQuest->ObjectiveTarget)) {
                                if (Quest->CompleteOnFinish) {
                                    GovernmentQuest->Successful = true;
                                    News = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Successful.PlanetaryQuest"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed));
                                    aMyFunction::ReplaceTextToken(News, u"<FromPlanet>"_w, GovernmentQuest->Planet->Name, aMyFunction::TextHighlightColorTag);
                                    aMyFunction::ReplaceTextToken(News, u"<ToPlanet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, aMyFunction::TextHighlightColorTag);
                                    Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, aGalaxy::Galaxy->CurrentTurn, News, u""_wref.get());
                                }
                                ItemName = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.ItemForPlanetQuest.", SysUtils::IntToStr(GovernmentQuest->QuestNumber)})));
                                if (ItemName != u"none") {
                                    Item = pas::construct_call<aItem::TUselessItem>(aItem::TUselessItem_Create);
                                    Item->Init(ItemName, aGalaxyStruct::dsBlazer, 0u, false);
                                    pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
                                }
                                aPlayer::GetPlayer()->CurrentPlanet->TextQuestId = -1;
                                pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"QuestPanel"sv))->SetActive(false);
                                if (aPlayer::GetPlayer()->CurrentPlanet->IsCoalitionOwned || aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                                    aPlayer::GetPlayer()->CurrentPlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), Quest->SuccessRelationDelta);
                                    if (aPlayer::GetPlayer()->CurrentPlanet->RelationToShip(aPlayer::GetPlayer()) < 20) {
                                        aPlayer::GetPlayer()->CurrentPlanet->SetRelationLevelToRanger(aPlayer::GetPlayer(), aGalaxyStruct::rlBad);
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (QuestId >= 0) {
            if (ActiveQueuedTextQuest == nullptr || aPlayer::GetPlayer() == nullptr || static_cast<std::uint8_t>(pas::in_set<4, 4, 6, 6>(aPlayer::GetPlayer()->PirateRank) ^ 1)) {
                if (aPlayer::GetPlayer() != nullptr) {
                    Achievements::TryAddAchievementProgress(u"QUEST"_w, 1);
                }
                Globals::LoadQuestScreen->LoadCompletionData();
                Globals::LoadQuestScreen->RecordCompletion(QuestId, 0, 1);
                Globals::LoadQuestScreen->SaveCompletionData();
            }
        }
        GlobalsV::RequestedScreenId = GlobalsV::QuestReturnScreenId;
        RequestClose(1);
        if (ActiveQueuedTextQuest != nullptr) {
            ActiveQueuedTextQuest = nullptr;
            aScript::CompleteQueuedTextQuest(aScript::sqsSuccess);
        }
    }

    // Queued script quests report status 3.
    void TfPlanetQuest::CompleteQuestFailure(std::int32_t Value) {
        pas::WideString News{};
        aRanger::PQuest GovernmentQuest{};
        std::int32_t I{};
        if (aPlayer::GetPlayer() == nullptr) {
            GlobalsV::RequestedScreenId = GlobalsV::QuestReturnScreenId;
        } else if (aPlayer::GetPlayer()->InPrison) {
            aPlayer::GetPlayer()->InPrison = false;
            if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate && aPlanet::MainPiratePlanet != nullptr) {
                    aPlanet::MainPiratePlanet->SetRelationLevelToRanger(aPlayer::GetPlayer(), aGalaxyStruct::rlHostile);
                } else {
                    aPlayer::GetPlayer()->CurrentPlanet->SetRelationLevelToRanger(aPlayer::GetPlayer(), aGalaxyStruct::rlHostile);
                }
            }
            if (!fHangar::TfHangar::TryTakeOff()) {
                GlobalsV::RequestedScreenId = GlobalsV::screenHangar;
            }
        } else {
            if (ActiveQueuedTextQuest == nullptr) {
                if (aPlayer::GetPlayer() != nullptr) {
                    if (pas::list_count(aPlayer::GetPlayer()->Quests) > 0) {
                        const std::int32_t cpp_first = pas::list_count(aPlayer::GetPlayer()->Quests) - 1;
                        if (cpp_first >= 0) {
                            for (I = cpp_first; I >= 0; --I) {
                                GovernmentQuest = pas::list_at<aRanger::TQuest>(aPlayer::GetPlayer()->Quests, I);
                                if (GovernmentQuest->QuestType == aGalaxyStruct::qtPlanetQuest && pas::class_cast_if<aPlanet::TPlanet*>(GovernmentQuest->ObjectiveTarget) != nullptr && aPlayer::GetPlayer()->CurrentPlanet == pas::checked_cast<aPlanet::TPlanet*>(GovernmentQuest->ObjectiveTarget)) {
                                    GovernmentQuest->Successful = false;
                                    static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::PublishQuestStatus(GovernmentQuest, -1);
                                    News = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Failure.PlanetaryQuest"_wref.get(), static_cast<std::int32_t>(aPlayer::GetPlayer()->Seed) * (aGalaxy::Galaxy->CurrentTurn / 10));
                                    aMyFunction::ReplaceTextToken(News, u"<ToPlanet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, aMyFunction::TextHighlightColorTag);
                                    aMyFunction::ReplaceTextToken(News, u"<FromPlanet>"_w, GovernmentQuest->Planet->Name, aMyFunction::TextHighlightColorTag);
                                    {
                                        auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                                        pas::WideString relationLevelTextToShip = GovernmentQuest->Planet->GetRelationLevelTextToShip(aPlayer::GetPlayer());
                                        aMyFunction::ReplaceTextToken(News, u"<Relation>"_w, std::move(relationLevelTextToShip), textHighlightColorTag.get());
                                    }
                                    Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, aGalaxy::Galaxy->CurrentTurn, News, u""_wref.get());
                                    aPlayer::GetPlayer()->CurrentPlanet->TextQuestId = -1;
                                    aPlayer::GetPlayer()->ArchiveQuest(I);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (ActiveQueuedTextQuest == nullptr && Globals::SaveManagerScreen->AutoSaveExists() && GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgLoad"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                GlobalsV::PendingLoadFileName = static_cast<pas::AnsiString>(fSaveManager::TfSaveManager::GetAutoSavePath());
                GlobalsV::RequestedScreenId = GlobalsV::screenGameLoad;
            } else {
                GlobalsV::RequestedScreenId = GlobalsV::QuestReturnScreenId;
            }
        }
        RequestClose(1);
        if (ActiveQueuedTextQuest != nullptr) {
            ActiveQueuedTextQuest = nullptr;
            aScript::CompleteQueuedTextQuest(aScript::sqsFailure);
        }
    }

    void TfPlanetQuest::CompleteQuestDeath(std::int32_t Value) {
        if (aPlayer::GetPlayer() == nullptr) {
            GlobalsV::RequestedScreenId = GlobalsV::QuestReturnScreenId;
        } else if (Globals::SaveManagerScreen->AutoSaveExists() && GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgLoad"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
            GlobalsV::PendingLoadFileName = static_cast<pas::AnsiString>(fSaveManager::TfSaveManager::GetAutoSavePath());
            GlobalsV::RequestedScreenId = GlobalsV::screenGameLoad;
        } else {
            Globals::ScoreScreen->RecordPlayerResult(false);
            pas::free(aPlayer::GetPlayer());
            GlobalsV::GameEndReason = 1;
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
        }
        aScript::ClearPendingScriptRequests();
        RequestClose(1);
    }

    // PQI keys are quest,L|P|PAR,indices; picture names lose the Bm.PQI. prefix.
    void TfPlanetQuest::ApplyLegacyPictureOverrides() {
        std::int32_t Found{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        pas::WideString Kind{};
        pas::WideString Indices{};
        pas::WideString Name{};
        pas::WideString Picture{};
        ValueListClass::TValuesList* Values = pas::construct_call<ValueListClass::TValuesList>(ValueListClass::TValuesList_Create);
        EC_BlockPar::TBlockParEC* Config = GR_Main::GameDataConfig->GetBlock(u"PQI"sv);
        std::int32_t Count = Config->GetParamCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Name = Config->GetParamName(I);
            if (TfPlanetQuest::GetTextBeforeDelimiter(pas::view(Name), u',') == QuestName) {
                Kind = TfPlanetQuest::GetTextAfterComma(pas::view(Name), u',');
                Indices = TfPlanetQuest::GetTextAfterComma(pas::view(Kind), u',');
                Kind = TfPlanetQuest::GetTextBeforeDelimiter(pas::view(Kind), u',');
                Values->LoadFromSemicolonText(Indices);
                Picture = Config->GetParamValue(I);
                Picture = EC_Str::ReplaceAllWideString(Picture, u"Bm.PQI."_wref.get(), u""sv);
                if (Kind == u"L") {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Values->Count); cpp_range_2.next(J); ) {
                        Found = -1;
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, Quest->GetLocationCount()); cpp_range_3.next(K); ) {
                            if (Quest->GetLocation(K)->Id == Values->Values[J]) {
                                Found = K;
                                break;
                            }
                        }
                        if (Found > 0) {
                            for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, Quest->GetLocation(Found)->EventCount); cpp_range_4.next(K); ) {
                                Quest->GetLocation(Found)->Events[K]->Picture->Text = Picture;
                            }
                        }
                    }
                }
                if (Kind == u"P") {
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, Values->Count); cpp_range_5.next(J); ) {
                        Found = -1;
                        for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, Quest->GetPathCount()); cpp_range_6.next(K); ) {
                            if (Quest->GetPath(K)->Id == Values->Values[J]) {
                                Found = K;
                                break;
                            }
                        }
                        if (Found > 0) {
                            Quest->GetPath(Found)->Event->Picture->Text = Picture;
                        }
                    }
                }
                if (Kind == u"PAR") {
                    for (auto cpp_range_7 = pas::for_to<std::int32_t>(1, Values->Count); cpp_range_7.next(J); ) {
                        if (Quest->GetParameterCount() >= Values->Values[J]) {
                            if (Quest->GetParameter(Values->Values[J])->Enabled) {
                                if (Quest->GetParameter(Values->Values[J])->CriticalOutcome != TextQuestInterface::qoNone) {
                                    Quest->GetParameter(Values->Values[J])->CriticalEvent->Picture->Text = Picture;
                                }
                            }
                        }
                    }
                }
            }
        }
        pas::destroy(Values);
    }

    // Uses the first enabled money parameter; writes the player's clamped balance back.
    void TfPlanetQuest::ExportMoneyToPlayer() {
        std::int32_t I{};
        if (aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Quest->GetParameterCount()); cpp_range.next(I); ) {
                if (Quest->GetParameter(I)->Enabled) {
                    if (Quest->GetParameter(I)->IsMoney) {
                        aPlayer::GetPlayer()->SetMoney(Quest->GetParameter(I)->Value);
                        Quest->GetParameter(I)->Value = aPlayer::GetPlayer()->Money;
                        break;
                    }
                }
            }
        }
    }

    // Uses the first enabled money parameter.
    void TfPlanetQuest::ImportMoneyFromPlayer() {
        std::int32_t I{};
        if (aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Quest->GetParameterCount()); cpp_range.next(I); ) {
                if (Quest->GetParameter(I)->Enabled) {
                    if (Quest->GetParameter(I)->IsMoney) {
                        Quest->GetParameter(I)->Value = aPlayer::GetPlayer()->Money;
                        break;
                    }
                }
            }
        }
    }

    // ext_name maps to GQuestVarExt_name; queued-script scope takes precedence over global scope.
    void TfPlanetQuest::ExportExternalParameters() {
        std::int32_t I{};
        pas::WideString Name{};
        EC_Expression::TVarEC* Variable{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Quest->GetParameterCount()); cpp_range.next(I); ) {
            if (EC_Str::FindTextPosW(u"ext_"_wref.get(), Quest->GetParameter(I)->NameText->Text) == 1) {
                Name = Quest->GetParameter(I)->NameText->Text;
                Name.write(1) = u'E';
                Name = pas::concat_wide({u"GQuestVar", Name});
                Variable = nullptr;
                if (ActiveQueuedTextQuest != nullptr) {
                    if (ActiveQueuedTextQuest->Script != nullptr) {
                        Variable = ActiveQueuedTextQuest->Script->InitCode->LocalVar->GetVarNE(Name);
                    }
                }
                if (Variable == nullptr) {
                    Variable = Globals::SharedScriptVariables->GetVarNE(Name);
                }
                if (Variable != nullptr) {
                    if (EC_Expression::TVarEC_RealVType(Variable) == EC_Expression::vkInt) {
                        Variable->SetInt(Quest->GetParameter(I)->Value);
                    }
                }
            }
        }
    }

    // Writes clamped parameter values back to the script variables.
    void TfPlanetQuest::ImportExternalParameters() {
        std::int32_t I{};
        pas::WideString Name{};
        EC_Expression::TVarEC* Variable{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Quest->GetParameterCount()); cpp_range.next(I); ) {
            if (EC_Str::FindTextPosW(u"ext_"_wref.get(), Quest->GetParameter(I)->NameText->Text) == 1) {
                Name = Quest->GetParameter(I)->NameText->Text;
                Name.write(1) = u'E';
                Name = pas::concat_wide({u"GQuestVar", Name});
                Variable = nullptr;
                if (ActiveQueuedTextQuest != nullptr) {
                    if (ActiveQueuedTextQuest->Script != nullptr) {
                        Variable = ActiveQueuedTextQuest->Script->InitCode->LocalVar->GetVarNE(Name);
                    }
                }
                if (Variable == nullptr) {
                    Variable = Globals::SharedScriptVariables->GetVarNE(Name);
                }
                if (Variable != nullptr) {
                    if (EC_Expression::TVarEC_RealVType(Variable) == EC_Expression::vkInt) {
                        {
                            ParameterClass::TParameter* parameter = Quest->GetParameter(I);
                            std::int32_t pasint = Variable->GetInt();
                            parameter->SetValue(pasint);
                        }
                        Variable->SetInt(Quest->GetParameter(I)->Value);
                    }
                }
            }
        }
    }

    // <txt_name> uses global GQuestVarExt_name; queued-script scope is ignored.
    pas::WideString TfPlanetQuest::ExpandExternalText(pas::WideString Text) {
        std::int32_t I{};
        pas::WideString Name{};
        pas::WideString Token{};
        pas::WideString Expanded{};
        EC_Expression::TVarEC* Variable{};
        Expanded = std::move(Text);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Quest->GetParameterCount()); cpp_range.next(I); ) {
            if (EC_Str::FindTextPosW(u"ext_"_wref.get(), Quest->GetParameter(I)->NameText->Text) != 1) {
                continue;
            }
            Name = Quest->GetParameter(I)->NameText->Text;
            Token = Name;
            Name.write(1) = u'E';
            Token.write(1) = u't';
            Token = pas::concat_wide({u"<", Token, u">"});
            Name = pas::concat_wide({u"GQuestVar", Name});
            Variable = Globals::SharedScriptVariables->GetVarNE(Name);
            if (Variable != nullptr) {
                Expanded = ([&] {
                    const pas::WideString& wrapTextInColor = ([&] {
                        pas::WideString textColorTag = GetTextColorTag(GlobalsV::QuestStyleIndex);
                        pas::WideString trimWideString = EC_Str::TrimWideString(Variable->GetString());
                        return aMyFunction::WrapTextInColor(pas::view(std::move(trimWideString)), pas::view(std::move(textColorTag)));
                    }());
                    const pas::WideString& token = Token;
                    const pas::WideString& expanded = Expanded;
                    return EC_Str::ReplaceAllWideString(expanded, token, pas::view(wrapTextInColor));
                }());
            }
        }
        return Expanded;
    }

    // Suppresses repeated text after template expansion.
    void TTextQuestPlayerInterface::ShowText(pas::WideString Text) {
        pas::WideString ExpandedText{};
        ExpandedText = Globals::PlanetQuestScreen->ExpandTemplateText(Text);
        if (ExpandedText != EC_Str::TrimWideString(Globals::PlanetQuestScreen->CurrentText)) {
            const pas::WideString& expandTemplateText = Globals::PlanetQuestScreen->ExpandTemplateText(Text);
            TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
            planetQuestScreen->SetQuestText(expandTemplateText);
        }
    }

    void TTextQuestPlayerInterface::ShowPicture(pas::WideString Name) {
        Globals::PlanetQuestScreen->SetQuestPicture(Name);
    }

    void TTextQuestPlayerInterface::PlayMusic(pas::WideString Name) {
        GR_Main::MusicManager->RequestFadeOut();
        if (GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->PlayCategory(Name);
        }
    }

    void TTextQuestPlayerInterface::PlaySound(pas::WideString Name) {
        GR_Main::SoundManager->PlaySound(pas::concat_wide({u"Sound.", Name}));
    }

    void TTextQuestPlayerInterface::ShowParameters(pas::WideString Text) {
        Globals::PlanetQuestScreen->ClearParameterPanel();
        {
            pas::WideString trimWideString = EC_Str::TrimWideString(Globals::PlanetQuestScreen->ExpandTemplateText(Text));
            TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
            planetQuestScreen->AppendParameterText(std::move(trimWideString));
        }
        Globals::PlanetQuestScreen->LayoutParameterPanel();
        Globals::PlanetQuestScreen->ExportMoneyToPlayer();
        Globals::PlanetQuestScreen->ExportExternalParameters();
    }

    void TTextQuestPlayerInterface::AddContinueAction() {
        TQuestChoiceEvent cpp_arg = pas::bind_method<&TfPlanetQuest::ContinueToOutcome>(Globals::PlanetQuestScreen);
        pas::WideString cpp_arg_2 = pas::concat_wide({u"  - ", aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgContinue"_wref.get())});
        TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
        planetQuestScreen->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
    }

    void TTextQuestPlayerInterface::AddSuccessAction() {
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InPrison) {
            TQuestChoiceEvent cpp_arg = pas::bind_method<&TfPlanetQuest::CompleteQuestSuccess>(Globals::PlanetQuestScreen);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"  - ", aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgSuccessPrison"_wref.get())});
            TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
            planetQuestScreen->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        } else if (ActiveQueuedTextQuest != nullptr && ActiveQueuedTextQuest->SuccessCaption != u"") {
            Globals::PlanetQuestScreen->AddChoice(pas::concat_wide({u" - ", ActiveQueuedTextQuest->SuccessCaption}), 0, pas::bind_method<&TfPlanetQuest::CompleteQuestSuccess>(Globals::PlanetQuestScreen));
        } else {
            TQuestChoiceEvent cpp_arg_3 = pas::bind_method<&TfPlanetQuest::CompleteQuestSuccess>(Globals::PlanetQuestScreen);
            pas::WideString cpp_arg_4 = pas::concat_wide({u" - ", aConst::LocalizedColorText(u"Planet.NotCivil.QuestPlay.MsgSuccess"_wref.get())});
            TfPlanetQuest* planetQuestScreen_2 = Globals::PlanetQuestScreen;
            planetQuestScreen_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
    }

    void TTextQuestPlayerInterface::AddDeathAction() {
        TQuestChoiceEvent cpp_arg = pas::bind_method<&TfPlanetQuest::CompleteQuestDeath>(Globals::PlanetQuestScreen);
        pas::WideString cpp_arg_2 = pas::concat_wide({u"  - ", aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgDeath"_wref.get())});
        TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
        planetQuestScreen->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
    }

    void TTextQuestPlayerInterface::AddFailureAction() {
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InPrison) {
            TQuestChoiceEvent cpp_arg = pas::bind_method<&TfPlanetQuest::CompleteQuestFailure>(Globals::PlanetQuestScreen);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"  - ", aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgFailPrison"_wref.get())});
            TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
            planetQuestScreen->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        } else if (ActiveQueuedTextQuest != nullptr && ActiveQueuedTextQuest->FailureCaption != u"") {
            Globals::PlanetQuestScreen->AddChoice(pas::concat_wide({u" - ", ActiveQueuedTextQuest->FailureCaption}), 0, pas::bind_method<&TfPlanetQuest::CompleteQuestFailure>(Globals::PlanetQuestScreen));
        } else {
            TQuestChoiceEvent cpp_arg_3 = pas::bind_method<&TfPlanetQuest::CompleteQuestFailure>(Globals::PlanetQuestScreen);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"  - ", aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgFail"_wref.get())});
            TfPlanetQuest* planetQuestScreen_2 = Globals::PlanetQuestScreen;
            planetQuestScreen_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
    }

    void TTextQuestPlayerInterface::AddPathAction(pas::WideString Text, std::int32_t PathId) {
        TQuestChoiceEvent cpp_arg = pas::bind_method<&TfPlanetQuest::ContinueAlongPath>(Globals::PlanetQuestScreen);
        pas::WideString cpp_arg_2 = pas::concat_wide({u"  - ", Globals::PlanetQuestScreen->ExpandTemplateText(Text)});
        TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
        planetQuestScreen->AddChoice(std::move(cpp_arg_2), PathId, cpp_arg);
    }

    void TTextQuestPlayerInterface::AddDisabledPath(pas::WideString Text) {
        TQuestChoiceEvent cpp_arg = pas::bind_static_method<&TfPlanetQuest::IgnoreChoice>(Globals::PlanetQuestScreen);
        pas::WideString cpp_arg_2 = pas::concat_wide({u"  - ", Globals::PlanetQuestScreen->ExpandTemplateText(Text)});
        TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
        planetQuestScreen->AddDisabledChoice(std::move(cpp_arg_2), 0, cpp_arg);
    }

    void TTextQuestPlayerInterface::AddPathContinueAction(std::int32_t PathId) {
        TQuestChoiceEvent cpp_arg = pas::bind_method<&TfPlanetQuest::ContinueAlongPath>(Globals::PlanetQuestScreen);
        pas::WideString cpp_arg_2 = pas::concat_wide({u"  - ", aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgContinue"_wref.get())});
        TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
        planetQuestScreen->AddChoice(std::move(cpp_arg_2), PathId, cpp_arg);
    }

    void TTextQuestPlayerInterface::AddLocationContinueAction(std::int32_t LocationId) {
        TQuestChoiceEvent cpp_arg = pas::bind_method<&TfPlanetQuest::ContinueToLocation>(Globals::PlanetQuestScreen);
        pas::WideString cpp_arg_2 = pas::concat_wide({u"  - ", aConst::LocalizedText(u"Planet.NotCivil.QuestPlay.MsgContinue"_wref.get())});
        TfPlanetQuest* planetQuestScreen = Globals::PlanetQuestScreen;
        planetQuestScreen->AddChoice(std::move(cpp_arg_2), LocationId, cpp_arg);
    }

    void TTextQuestPlayerInterface::AdvanceDays(std::int32_t Days) {
        std::int32_t I{};
        Globals::PlanetQuestScreen->ExportMoneyToPlayer();
        Globals::PlanetQuestScreen->ExportExternalParameters();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Days); cpp_range.next(I); ) {
            ++Globals::PlanetQuestScreen->DaysElapsed;
            // The native standalone path stops after one increment, even when Days is greater than one.
            if (Globals::StandaloneQuestMode) {
                break;
            }
            ThreadCalc::WaitForTurnCalculation();
            Globals::PruneExpiredPersistentPlayerMessages();
            aCalc::CalculatePlayerStarTurnAndWait();
            if (GR_Main::ExitScreenLoop) {
                break;
            }
            aCalc::QueueGalaxyTurnCalculation();
            Globals::PlanetQuestScreen->ImportMoneyFromPlayer();
            Globals::PlanetQuestScreen->ImportExternalParameters();
        }
    }

    void TfPlanetQuest::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (GR_Main::ExitScreenLoop) {
            return;
        }
        if (pas::in_set<0, 0, 2, 2, 4, 4, 6, 6>(static_cast<std::int32_t>(aCalc::TurnCalculationPhase))) {
            if (aGalaxy::Galaxy != nullptr) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(10109);
            }
            aScript::ExecuteGameplayUiCode(Block, Key);
            if (aGalaxy::Galaxy != nullptr) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum(20109);
            }
        }
    }

    void TfQuestA::p_destroy() {
        fPlanetQuest::TfQuestA_Destroy(this);
    }

} // namespace fPlanetQuest
