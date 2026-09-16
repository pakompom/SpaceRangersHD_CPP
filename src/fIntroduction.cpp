#include "layout/fIntroduction.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fGameSettings.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Label.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/fIntroduction.hpp"

namespace fIntroduction {
    std::int32_t IntroductionPulseCounter = 0;

    // Shared with TThreadCreateNewGame; stage 8 means generation completed.
    std::int32_t NewGameGenerationStage{};

    std::uint32_t IntroductionBlinkColorA{};

    std::uint32_t IntroductionBlinkColorB{};

    std::uint8_t IntroductionPulseRed{};

    std::uint8_t IntroductionPulseGreen{};

    std::uint8_t IntroductionPulseBlue{};

    std::int32_t DisplayedGenerationStage{};

    void TfIntroduction::InitializeLayout() {
        std::int32_t I{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fIntroduction... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* ImageFon1 = MainPanel->FindByNameRecursive(u"ImageFon1"_wref.get());
                ImageFon1->SetPosition(ClassesImports::Point(ImageFon1->LocalPosition.X, GR_Main::GameScreenHeight - ImageFon1->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ImageFon2 = MainPanel->FindByNameRecursive(u"ImageFon2"_wref.get());
                ImageFon2->SetPosition(ClassesImports::Point(ImageFon2->LocalPosition.X, GR_Main::GameScreenHeight - ImageFon2->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ImageTop = MainPanel->FindByNameRecursive(u"ImageTop"_wref.get());
                ImageTop->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, ImageTop->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ImageBottom = MainPanel->FindByNameRecursive(u"ImageBottom"_wref.get());
                ImageBottom->SetPosition(ClassesImports::Point(ImageBottom->LocalPosition.X, ImageBottom->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                ImageBottom->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, ImageBottom->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ImageScreen = MainPanel->FindByNameRecursive(u"ImageScreen"_wref.get());
                ImageScreen->SetPosition(ClassesImports::Point(ImageScreen->LocalPosition.X, ImageScreen->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ok = MainPanel->FindByNameRecursive(u"Ok"_wref.get());
                Ok->SetPosition(ClassesImports::Point(Ok->LocalPosition.X + GR_Main::ExtraScreenWidth, Ok->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(1, 8); cpp_range.next(I); ) {
                {
                    GI_MessageLoop::TObjectGI* cpp_with_8 = MainPanel->FindByNameRecursive(static_cast<pas::WideString>(pas::concat_ansi({"ICW", SysUtils::IntToStr(I)})));
                    cpp_with_8->SetPosition(ClassesImports::Point(cpp_with_8->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, cpp_with_8->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* cpp_with_9 = MainPanel->FindByNameRecursive(static_cast<pas::WideString>(pas::concat_ansi({"MCW", SysUtils::IntToStr(I)})));
                    cpp_with_9->SetPosition(ClassesImports::Point(cpp_with_9->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, cpp_with_9->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
            }
            {
                GI_MessageLoop::TObjectGI* PanelText = MainPanel->FindByNameRecursive(u"PanelText"_wref.get());
                PanelText->SetSize(ClassesImports::Point(PanelText->ClientSize.X + GR_Main::ExtraScreenWidth, PanelText->ClientSize.Y + GR_Main::ExtraScreenHeight));
                {
                    GI_MessageLoop::TObjectGI* GBText = PanelText->FindByNameRecursive(u"GBText"_wref.get());
                    GBText->SetSize(ClassesImports::Point(GBText->ClientSize.X + GR_Main::ExtraScreenWidth, GBText->ClientSize.Y));
                }
            }
        }
        IntroductionBlinkColorA = GR_Main::GetStyleColorGI(u"Introduction.BlinkColorA"_w, 27, 68, 98);
        IntroductionBlinkColorB = GR_Main::GetStyleColorGI(u"Introduction.BlinkColorB"_w, 93, 152, 166);
        GR_Main::AppendLogLineThreadSafe("ok"_a);
    }

    void TfIntroduction::OnOpen() {
        std::int32_t I{};
        pas::WideString Text{};
        DisplayedGenerationStage = 0;
        NewGameGenerationStage = 0;
        ProgressPulsePhase = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 8); cpp_range.next(I); ) {
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"MCW", SysUtils::IntToStr(I)}))))->SetTextColor(IntroductionBlinkColorA);
            GetByName(static_cast<pas::WideString>(pas::concat_ansi({"ICW", SysUtils::IntToStr(I)})))->SetActive(false);
        }
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfIntroduction::MainPanelKeyDown>(this);
        {
            GI_MessageLoop::TObjectGI* PanelText = GetByName(u"PanelText"_wref.get());
            TextPanelTop = PanelText->LocalPosition.Y;
            TextPanelHeight = PanelText->ClientSize.Y;
        }
        {
            GI_GraphButton::TGraphButtonGI* Ok = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"_wref.get()));
            Ok->UpCallback = pas::bind_method<&TfIntroduction::ContinueClicked>(this);
            Ok->MouseEnterCallback = pas::bind_method<&TfIntroduction::ContinueMouseEnter>(this);
            Ok->SetDisabled(true);
        }
        if (Globals::NewGameGenerationThread == nullptr) {
            Text = ([&] {
                auto name = pas::borrow(aPlayer::GetPlayer()->Name);
                pas::WideString localizedText = aConst::LocalizedText(u"FormIntroduction.Text"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Player>"_w, name.get());
            }());
        } else {
            Text = ([&] {
                auto playerName = pas::borrow(Globals::NewGameGenerationThread->PlayerName);
                pas::WideString localizedText_2 = aConst::LocalizedText(u"FormIntroduction.Text"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<Player>"_w, playerName.get());
            }());
        }
        {
            GI_GraphBuf::TGraphBufGI* GBText = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GBText"_wref.get()));
            GBText->SourceHasPerPixelAlpha = true;
            if (GR_Main::ExtraScreenWidth > 127 && GR_Main::ExtraScreenHeight > 127) {
                GI_Label::RenderLabelTextToBuffer(GBText->GraphBuf, GBText->ClientSize.X, 1, 0, Text, GlobalsV::BigFontName, 0xfffdffd7u, 0xff373737u, 0xffdbda9cu);
            } else if (GR_Main::ExtraScreenWidth > 127) {
                GI_Label::RenderLabelTextToBuffer(GBText->GraphBuf, GBText->ClientSize.X, 1, 0, Text, GlobalsV::SmoothIntroFontName, 0xfffdffd7u, 0xff373737u, 0xffdbda9cu);
            } else {
                GI_Label::RenderLabelTextToBuffer(GBText->GraphBuf, GBText->ClientSize.X, 1, 0, Text, GlobalsV::IntroFontName, 0xfffdffd7u, 0xff373737u, 0xffdbda9cu);
            }
            GBText->SetSize(ClassesImports::Point(GBText->ClientSize.X, GBText->GraphBuf->Height));
            GBText->SetPosition(ClassesImports::Point(GBText->LocalPosition.X, TextPanelHeight));
        }
        if (GenerationProgressTimer != nullptr) {
            CancelCallbackTimer(GenerationProgressTimer);
            GenerationProgressTimer = nullptr;
        }
        GenerationProgressTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfIntroduction::UpdateGenerationProgress>(this), 0);
        if (TextScrollTimer != nullptr) {
            CancelCallbackTimer(TextScrollTimer);
            TextScrollTimer = nullptr;
        }
        TextScrollTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfIntroduction::ScrollIntroductionText>(this), 0);
        if (BackgroundTimer != nullptr) {
            CancelCallbackTimer(BackgroundTimer);
            BackgroundTimer = nullptr;
        }
        if (GlobalsV::AnimMainFon) {
            BackgroundTimer = ScheduleCallbackTimer(40, 40, pas::bind_method<&TfIntroduction::ScrollBackground>(this), 0);
        }
        IntroductionPulseRed = 20;
        IntroductionPulseGreen = 30;
        IntroductionPulseBlue = 50;
        BackgroundScrollOffset = 0;
        ScrollBackground(nullptr, 0);
    }

    void TfIntroduction::OnClose() {
        if (ContinueBlinkTimer != nullptr) {
            CancelCallbackTimer(ContinueBlinkTimer);
            ContinueBlinkTimer = nullptr;
        }
        if (GenerationProgressTimer != nullptr) {
            CancelCallbackTimer(GenerationProgressTimer);
            GenerationProgressTimer = nullptr;
        }
        if (TextScrollTimer != nullptr) {
            CancelCallbackTimer(TextScrollTimer);
            TextScrollTimer = nullptr;
        }
        if (BackgroundTimer != nullptr) {
            CancelCallbackTimer(BackgroundTimer);
            BackgroundTimer = nullptr;
        }
        if (Globals::NewGameGenerationThread != nullptr) {
            pas::free(Globals::NewGameGenerationThread);
            Globals::NewGameGenerationThread = nullptr;
        }
        {
            GI_GraphBuf::TGraphBufGI* GBText = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GBText"_wref.get()));
            GBText->GraphBuf->Clear();
        }
    }

    void TfIntroduction::UpdateGenerationProgress(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Delta{};
        float Amount{};
        if (IntroductionPulseCounter > 50) {
            Delta = -3;
        } else {
            Delta = 3;
        }
        IntroductionPulseRed += Delta;
        IntroductionPulseGreen += Delta;
        IntroductionPulseBlue += Delta;
        aMyFunction::IncrementWrapped(IntroductionPulseCounter, 0, 100);
        if (IntroductionPulseCounter == 0 || IntroductionPulseRed <= 20) {
            IntroductionPulseRed = 20;
            IntroductionPulseGreen = 30;
            IntroductionPulseBlue = 50;
        }
        ProgressPulsePhase = ProgressPulsePhase + 0.05L;
        if (ProgressPulsePhase >= 1.0L) {
            ProgressPulsePhase = 0.0f;
            if (DisplayedGenerationStage < NewGameGenerationStage) {
                ++DisplayedGenerationStage;
                if (DisplayedGenerationStage + 0 >= 1 && DisplayedGenerationStage + 0 <= 8) {
                    GetByName(static_cast<pas::WideString>(pas::concat_ansi({"ICW", SysUtils::IntToStr(DisplayedGenerationStage)})))->SetActive(true);
                }
                if (DisplayedGenerationStage + 0 >= 1 && DisplayedGenerationStage + 0 <= 8) {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"MCW", SysUtils::IntToStr(DisplayedGenerationStage)}))))->SetTextColor(IntroductionBlinkColorA);
                }
            }
        }
        if (ProgressPulsePhase < 0.5L) {
            Amount = ProgressPulsePhase * 2.0L;
        } else {
            Amount = 1.0L - (ProgressPulsePhase - 0.5L) * 2.0L;
        }
        if (DisplayedGenerationStage + 1 >= 1 && DisplayedGenerationStage + 1 <= 8) {
            std::uint32_t interpolateRgb = GR_Main::CurrentPixelFormat->InterpolateRgb(IntroductionBlinkColorA, IntroductionBlinkColorB, Amount);
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"MCW", SysUtils::IntToStr(DisplayedGenerationStage + 1)}))));
            cpp_arg->SetTextColor(interpolateRgb);
        }
        if (Globals::NewGameGenerationThread == nullptr || static_cast<std::uint8_t>(Globals::NewGameGenerationThread->IsRunning() ^ 1) && DisplayedGenerationStage == 8) {
            if (GenerationProgressTimer != nullptr) {
                CancelCallbackTimer(GenerationProgressTimer);
                GenerationProgressTimer = nullptr;
            }
            {
                GI_GraphButton::TGraphButtonGI* Ok = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"_wref.get()));
                Ok->SetHovered(false);
                Ok->SetDisabled(false);
            }
            if (Globals::NewGameGenerationThread != nullptr) {
                pas::free(Globals::NewGameGenerationThread);
                Globals::NewGameGenerationThread = nullptr;
            }
            RootUiObject->ProcessMouseMove(0u, ClassesImports::Point(-1, -1));
            GR_Main::PostMouseMoveMessage();
            if (ContinueBlinkTimer != nullptr) {
                CancelCallbackTimer(ContinueBlinkTimer);
                ContinueBlinkTimer = nullptr;
            }
            ContinueBlinkTimer = ScheduleCallbackTimer(200, 200, pas::bind_method<&TfIntroduction::BlinkContinueButton>(this), 0);
        }
    }

    void TfIntroduction::BlinkContinueButton(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        pas::WideString Enter{};
        pas::WideString Leave{};
        {
            GI_GraphButton::TGraphButtonGI* Ok = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"_wref.get()));
            Enter = Ok->EnterSound;
            Ok->EnterSound = pas::WideString();
            Leave = Ok->LeaveSound;
            Ok->LeaveSound = pas::WideString();
            Ok->SetHovered(static_cast<std::uint8_t>(Ok->IsHovered() ^ 1));
            Ok->EnterSound = Enter;
            Ok->LeaveSound = Leave;
        }
    }

    void TfIntroduction::ScrollIntroductionText(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Limit{};
        {
            GI_GraphBuf::TGraphBufGI* GBText = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GBText"_wref.get()));
            GBText->SetPosition(EC_Struct::AddPoints(GBText->LocalPosition, ClassesImports::Point(0, -1)));
            Limit = (TextPanelTop + TextPanelHeight) / 2 - GBText->ClientSize.Y / 2;
            if (static_cast<long double>(GBText->LocalPosition.Y) < Limit) {
                if (TextScrollTimer != nullptr) {
                    CancelCallbackTimer(TextScrollTimer);
                    TextScrollTimer = nullptr;
                }
            }
        }
    }

    void TfIntroduction::ScrollBackground(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Offset{};
        ++BackgroundScrollOffset;
        {
            GI_MessageLoop::TObjectGI* ImageFon1 = GetByName(u"ImageFon1"_wref.get());
            Offset = pas::imod(BackgroundScrollOffset, ImageFon1->ClientSize.X);
            ImageFon1->SetPosition(ClassesImports::Point(0 - Offset, ImageFon1->LocalPosition.Y));
        }
        {
            GI_MessageLoop::TObjectGI* ImageFon2 = GetByName(u"ImageFon2"_wref.get());
            ImageFon2->SetPosition(ClassesImports::Point(ImageFon2->ClientSize.X - Offset, ImageFon2->LocalPosition.Y));
        }
    }

    void TfIntroduction::ContinueMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (ContinueBlinkTimer != nullptr) {
            CancelCallbackTimer(ContinueBlinkTimer);
            ContinueBlinkTimer = nullptr;
        }
    }

    void TfIntroduction::ContinueClicked(GI_MessageLoop::TObjectGI* Sender) {
        GR_DX::ReleaseAllTextureSurfaces();
        if (aPlayer::GetPlayer()->DockedTo != nullptr) {
            GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
        } else if (aPlayer::GetPlayer()->CurrentPlanet == nullptr) {
            GR_Main::RaiseWideMessage(u"No player location"_wref.get());
        } else if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
            GlobalsV::RequestedScreenId = GlobalsV::screenPlanetNO;
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
        }
        RequestClose(1);
    }

    void TfIntroduction::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1) && (Key == WindowsSdk::VK_SPACE || Key == WindowsSdk::VK_RETURN || Key == WindowsSdk::VK_RIGHT)) {
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"_wref.get()))->Disabled) {
                ContinueClicked(nullptr);
            }
        }
    }

    void TfIntroduction::SelectMusic() {
        GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
    }

} // namespace fIntroduction
