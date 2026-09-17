#include "layout/fPanelLoad.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fLoad.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/fPanelLoad.hpp"

namespace fPanelLoad {
    fPanelLoad::TfPanelLoad* ActiveLoadPanel{};

    void TfPanelLoad_Create(TfPanelLoad* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->LayoutAdjusted = false;
    }

    void TfPanelLoad_Destroy(TfPanelLoad* Self) {
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TfPanelLoad::InitializeLayout(GI_MessageLoop::TMessageLoopGI* Screen) {
        std::int32_t I{};
        GI_MessageLoop::TObjectGI* Panel{};
        this->Screen = Screen;
        GR_Main::AppendLogTextThreadSafe("fPanelLoad... "_a);
        if (!LayoutAdjusted) {
            Panel = this->Screen->GetByName(u"PanelLoad"_wref.get());
            Panel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* BGImage = Panel->FindByNameRecursive(u"BGImage"_wref.get());
                BGImage->SetPosition(ClassesImports::Point(BGImage->LocalPosition.X, BGImage->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
                BGImage->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, BGImage->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ShipPanelImage = Panel->FindByNameRecursive(u"ShipPanelImage"_wref.get());
                ShipPanelImage->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_GAI::TgaiGI* LoadAnim = pas::checked_cast<GI_GAI::TgaiGI*>(Panel->FindByNameRecursive(u"LoadAnim"_wref.get()));
                LoadAnim->SetPosition(ClassesImports::Point(LoadAnim->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, LoadAnim->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
                LoadAnim->StopAutoPlayback();
            }
            {
                GI_MessageLoop::TObjectGI* LoadAnimText = Panel->FindByNameRecursive(u"LoadAnimText"_wref.get());
                LoadAnimText->SetPosition(ClassesImports::Point(LoadAnimText->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, LoadAnimText->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PLProgress = Panel->FindByNameRecursive(u"PLProgress"_wref.get());
                PLProgress->SetPosition(ClassesImports::Point(PLProgress->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PLProgress->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PLBar = Panel->FindByNameRecursive(u"PLBar"_wref.get());
                PLBar->SetActive(false);
            }
            LayoutAdjusted = true;
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TfPanelLoad::GetProgressSegmentCount() - 1); cpp_range.next(I); ) {
            ProgressSegments[I] = pas::checked_cast<GI_Image::TImageGI*>(this->Screen->GetByName(static_cast<pas::WideString>(pas::concat_ansi({"PLB", SysUtils::IntToStr(I + 1)}))));
            ProgressSegments[I]->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        }
        BackgroundImage = this->Screen->GetByName(u"BGImage"_wref.get());
        this->ShipPanelImage = this->Screen->GetByName(u"ShipPanelImage"_wref.get());
        LoadAnimation = this->Screen->GetByName(u"LoadAnim"_wref.get());
        AnimationText = this->Screen->GetByName(u"LoadAnimText"_wref.get());
        ProgressLabel = pas::checked_cast<GI_Label::TLabelGI*>(this->Screen->GetByName(u"PLProgress"_wref.get()));
        ProgressBar = this->Screen->GetByName(u"PLBar"_wref.get());
        BackgroundRestTop = BackgroundImage->LocalPosition.Y;
        ShipPanelRestTop = this->ShipPanelImage->LocalPosition.Y;
        AnimationRestTop = LoadAnimation->LocalPosition.Y;
        AnimationTextRestTop = AnimationText->LocalPosition.Y;
        ProgressLabelRestTop = ProgressLabel->LocalPosition.Y;
        ProgressBarRestTop = ProgressBar->LocalPosition.Y;
        RightShutter = pas::checked_cast<GI_Panel::TPanelGI*>(this->Screen->FindControlByPath(u"PLRight"_wref.get()));
        LeftShutter = pas::checked_cast<GI_Panel::TPanelGI*>(this->Screen->FindControlByPath(u"PLLeft"_wref.get()));
        TopShutter = pas::checked_cast<GI_Panel::TPanelGI*>(this->Screen->FindControlByPath(u"PLTop"_wref.get()));
        BottomShutter = pas::checked_cast<GI_Panel::TPanelGI*>(this->Screen->FindControlByPath(u"PLBottom"_wref.get()));
        HasShutters = RightShutter != nullptr && LeftShutter != nullptr && TopShutter != nullptr && BottomShutter != nullptr;
        if (HasShutters) {
            RightShutter->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            LeftShutter->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            TopShutter->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            BottomShutter->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            this->Screen->GetByName(u"PLRightImage"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            this->Screen->GetByName(u"PLLeftImage"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            this->Screen->GetByName(u"PLTopImage"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            this->Screen->GetByName(u"PLBottomImage"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        }
    }

    void TfPanelLoad::OnOpen() {
        Hide();
        if (Globals::LoadScreen->BackgroundStyle <= 0) {
            SelectBackgroundStyle(0);
        }
        RefreshBackgroundImages();
        SetProgress(1.0f);
        ShutterOpenFraction = 0.0f;
        SetShutterOpenFraction(ShutterOpenFraction);
        if (GlobalsV::PreviousScreenId == GlobalsV::screenLoad && GlobalsV::CurrentScreenId != GlobalsV::screenMainMenu && GlobalsV::CurrentScreenId != GlobalsV::screenGameLoad && GlobalsV::CurrentScreenId != GlobalsV::screenLoadQuest || GlobalsV::PreviousScreenId == GlobalsV::screenLoad && GlobalsV::CurrentScreenId == GlobalsV::screenMainMenu && Globals::SkipVideo || GlobalsV::PreviousScreenId == GlobalsV::screenGameLoad && GlobalsV::CurrentScreenId != GlobalsV::screenLoad || pas::is_one_of<GlobalsV::screenHangar, GlobalsV::screenPlanet, GlobalsV::screenPlanetNO, GlobalsV::screenEquipmentShop, GlobalsV::screenGovernment, GlobalsV::screenRuinsTalk, GlobalsV::screenInfo>(GlobalsV::CurrentScreenId) && GlobalsV::PreviousScreenId == GlobalsV::screenStarMap && aPlayer::GetPlayer()->RuinsMode == 0 || pas::is_one_of<GlobalsV::screenStarMap, GlobalsV::screenRuinsTalk>(GlobalsV::CurrentScreenId) && pas::is_one_of<GlobalsV::screenJump, GlobalsV::screenArcadeBattle>(GlobalsV::PreviousScreenId) || GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle && GlobalsV::PreviousScreenId == GlobalsV::screenStarMap) {
            GlobalsV::PreviousScreenId = GlobalsV::screenNone;
            StartOpeningShutters();
        }
        ActiveLoadPanel = this;
    }

    void TfPanelLoad::OnClose() {
        if (ActiveLoadPanel == this) {
            ActiveLoadPanel = nullptr;
        }
        if (ShutterTimer != nullptr) {
            Screen->CancelCallbackTimer(ShutterTimer);
            ShutterTimer = nullptr;
        }
    }

    std::int32_t TfPanelLoad::GetProgressSegmentCount() {
        return 17;
    }

    void TfPanelLoad::Show() {
        Screen->GetByName(u"PanelLoad"_wref.get())->SetActive(true);
    }

    void TfPanelLoad::Hide() {
        Screen->GetByName(u"PanelLoad"_wref.get())->SetActive(false);
    }

    // Accepts groups 0..3; selects a style for shutter or legacy artwork. Other values preserve the current style.
    void TfPanelLoad::SelectBackgroundStyle(std::int32_t StyleGroup) {
        if (!HasShutters) {
            if (StyleGroup == 0) {
                Globals::LoadScreen->BackgroundStyle = aMyFunction::RandomIntRange(1, 2);
            } else if (StyleGroup == 1) {
                Globals::LoadScreen->BackgroundStyle = aMyFunction::RandomIntRange(3, 6);
            } else if (StyleGroup == 2) {
                Globals::LoadScreen->BackgroundStyle = 7;
            } else if (StyleGroup == 3) {
                Globals::LoadScreen->BackgroundStyle = 8;
            }
        } else if (StyleGroup == 0) {
            Globals::LoadScreen->BackgroundStyle = aMyFunction::RandomIntRange(1, 7);
        } else if (StyleGroup == 1) {
            Globals::LoadScreen->BackgroundStyle = aMyFunction::RandomIntRange(8, 13);
        } else if (StyleGroup == 2) {
            Globals::LoadScreen->BackgroundStyle = 14;
        } else if (StyleGroup == 3) {
            Globals::LoadScreen->BackgroundStyle = 15;
        }
    }

    void TfPanelLoad::RefreshBackgroundImages() {
        pas::WideString Style{};
        if (!HasShutters) {
            pas::WideString cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormLoad2.Style", SysUtils::IntToStr(Globals::LoadScreen->BackgroundStyle)}));
            GI_Image::TImageGI* cpp_arg_2 = pas::checked_cast<GI_Image::TImageGI*>(BackgroundImage);
            cpp_arg_2->SetImagePath(std::move(cpp_arg));
        } else {
            if (Globals::LoadScreen->BackgroundStyle < 10) {
                Style = static_cast<pas::WideString>(pas::concat_ansi({"0", SysUtils::IntToStr(Globals::LoadScreen->BackgroundStyle)}));
            } else {
                Style = pas::wide_int_to_str(Globals::LoadScreen->BackgroundStyle);
            }
            {
                GI_Image::TImageGI* cpp_arg_3 = pas::checked_cast<GI_Image::TImageGI*>(Screen->GetByName(u"PLRightImage"_wref.get()));
                pas::WideString cpp_arg_4 = pas::concat_wide({u"GI,Bm.FormLoad2.ShutterRight", Style});
                cpp_arg_3->SetImagePath(std::move(cpp_arg_4));
            }
            {
                GI_Image::TImageGI* cpp_arg_5 = pas::checked_cast<GI_Image::TImageGI*>(Screen->GetByName(u"PLLeftImage"_wref.get()));
                pas::WideString cpp_arg_6 = pas::concat_wide({u"GI,Bm.FormLoad2.ShutterLeft", Style});
                cpp_arg_5->SetImagePath(std::move(cpp_arg_6));
            }
            {
                GI_Image::TImageGI* cpp_arg_7 = pas::checked_cast<GI_Image::TImageGI*>(Screen->GetByName(u"PLTopImage"_wref.get()));
                pas::WideString cpp_arg_8 = pas::concat_wide({u"GI,Bm.FormLoad2.ShutterTop", Style});
                cpp_arg_7->SetImagePath(std::move(cpp_arg_8));
            }
            {
                GI_Image::TImageGI* cpp_arg_9 = pas::checked_cast<GI_Image::TImageGI*>(Screen->GetByName(u"PLBottomImage"_wref.get()));
                pas::WideString cpp_arg_10 = pas::concat_wide({u"GI,Bm.FormLoad2.ShutterBottom", Style});
                cpp_arg_9->SetImagePath(std::move(cpp_arg_10));
            }
        }
    }

    // Requires a fraction in 0..1; does not clamp the progress-segment index.
    void TfPanelLoad::SetProgress(float Fraction) {
        std::int32_t I{};
        std::int32_t LastActive = System::Round(static_cast<long double>(TfPanelLoad::GetProgressSegmentCount()) * Fraction) - 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LastActive); cpp_range.next(I); ) {
            ProgressSegments[I]->SetActive(true);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(LastActive + 1, TfPanelLoad::GetProgressSegmentCount() - 1); cpp_range_2.next(I); ) {
            ProgressSegments[I]->SetActive(false);
        }
        ProgressLabel->SetText(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(System::Round(Fraction * 1.0E+2L + 0.5L)), "%"})));
        {
            GI_GAI::TgaiGI* LoadAnim = pas::checked_cast<GI_GAI::TgaiGI*>(Screen->GetByName(u"LoadAnim"_wref.get()));
            LoadAnim->SetSequenceFrame(pas::imod(System::Round(static_cast<long double>(LoadAnim->SequenceFrameCount - 1) * Fraction * 2.0L + 3.0L), LoadAnim->SequenceFrameCount - 1));
        }
    }

    void TfPanelLoad::SetShutterOpenFraction(float Fraction) {
        if (!HasShutters) {
            return;
        }
        LeftShutter->SetPosition(ClassesImports::Point(-System::Round(static_cast<long double>(static_cast<std::uint32_t>(GR_Main::GameScreenWidth)) * Fraction * 0.34L), LeftShutter->LocalPosition.Y));
        RightShutter->SetPosition(ClassesImports::Point(System::Round(static_cast<long double>(static_cast<std::uint32_t>(GR_Main::GameScreenWidth)) * Fraction * 0.34L), RightShutter->LocalPosition.Y));
        TopShutter->SetPosition(ClassesImports::Point(TopShutter->LocalPosition.X, -System::Round(static_cast<long double>(static_cast<std::uint32_t>(GR_Main::GameScreenHeight)) * Fraction * 0.61L)));
        BottomShutter->SetPosition(ClassesImports::Point(BottomShutter->LocalPosition.X, System::Round(static_cast<long double>(static_cast<std::uint32_t>(GR_Main::GameScreenHeight)) * Fraction * 0.39L)));
    }

    void TfPanelLoad::StartOpeningShutters() {
        if (GlobalsV::AnimChangeForm && HasShutters) {
            Show();
            if (ShutterTimer != nullptr) {
                Screen->CancelCallbackTimer(ShutterTimer);
                ShutterTimer = nullptr;
            }
            ShutterTimer = Screen->ScheduleCallbackTimer(17, 17, pas::bind_method<&TfPanelLoad::UpdateOpeningShutters>(this), 0);
            ShutterDirection = 1;
            return;
        }
        Hide();
    }

    // Closes Screen after the animation, or immediately when shutters are disabled.
    void TfPanelLoad::StartClosingShutters() {
        if (GlobalsV::AnimChangeForm && HasShutters) {
            Show();
            SetProgress(0.0f);
            ShutterOpenFraction = 1.0f;
            SetShutterOpenFraction(ShutterOpenFraction);
            if (ShutterTimer != nullptr) {
                Screen->CancelCallbackTimer(ShutterTimer);
                ShutterTimer = nullptr;
            }
            ShutterTimer = Screen->ScheduleCallbackTimer(17, 17, pas::bind_method<&TfPanelLoad::UpdateClosingShutters>(this), 0);
            ShutterDirection = -1;
            return;
        }
        Screen->RequestClose(1);
    }

    void TfPanelLoad::UpdateOpeningShutters(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        ShutterOpenFraction = 0.03L + ShutterOpenFraction;
        if (ShutterOpenFraction >= 1.0L) {
            ShutterOpenFraction = 1.0f;
            if (ShutterTimer != nullptr) {
                Screen->CancelCallbackTimer(ShutterTimer);
                ShutterTimer = nullptr;
            }
            Hide();
            SetShutterOpenFraction(ShutterOpenFraction);
            Screen->Present();
        } else {
            SetShutterOpenFraction(ShutterOpenFraction);
        }
    }

    void TfPanelLoad::UpdateClosingShutters(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        ShutterOpenFraction = ShutterOpenFraction - 0.03L;
        if (ShutterOpenFraction <= -0.025L) {
            ShutterOpenFraction = 0.0f;
            if (ShutterTimer != nullptr) {
                Screen->CancelCallbackTimer(ShutterTimer);
                ShutterTimer = nullptr;
            }
            SetShutterOpenFraction(ShutterOpenFraction);
            Screen->Present();
            Screen->RequestClose(1);
        } else if (ShutterOpenFraction <= 0.0L) {
            SetShutterOpenFraction(0.0f);
        } else {
            SetShutterOpenFraction(ShutterOpenFraction);
        }
    }

    std::uint8_t TfPanelLoad::IsAnimatingShutters() {
        return ShutterTimer != nullptr;
    }

    // Returns zero without an active timer.
    std::int32_t TfPanelLoad::GetShutterDirection() {
        if (ShutterTimer == nullptr) {
            return 0;
        }
        return ShutterDirection;
    }

    void TfPanelLoad::p_destroy() {
        fPanelLoad::TfPanelLoad_Destroy(this);
    }

} // namespace fPanelLoad
