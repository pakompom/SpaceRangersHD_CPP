#include "layout/fAbout.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/fAbout.hpp"

namespace fAbout {
    void TfAbout::InitializeLayout() {
        std::int32_t Shift{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fAbout... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* BGImage = MainPanel->FindByNameRecursive(u"BGImage"sv);
                BGImage->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* LogoPanel = MainPanel->FindByNameRecursive(u"LogoPanel"sv);
                LogoPanel->SetPosition(ClassesImports::Point(LogoPanel->LocalPosition.X, LogoPanel->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ShadeBottom = MainPanel->FindByNameRecursive(u"ShadeBottom"sv);
                ShadeBottom->SetPosition(ClassesImports::Point(ShadeBottom->LocalPosition.X, ShadeBottom->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* PanelImage = MainPanel->FindByNameRecursive(u"PanelImage"sv);
                PanelImage->SetPosition(ClassesImports::Point(PanelImage->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelImage->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* Caption = MainPanel->FindByNameRecursive(u"Caption"sv);
                Caption->SetPosition(ClassesImports::Point(Caption->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Caption->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* SubCaption = MainPanel->FindByNameRecursive(u"SubCaption"sv);
                SubCaption->SetPosition(ClassesImports::Point(SubCaption->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, SubCaption->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* LogoElemental = MainPanel->FindByNameRecursive(u"LogoElemental"sv);
                Shift = LogoElemental->LocalPosition.X * GR_Main::GameScreenWidth / 1024 - LogoElemental->LocalPosition.X;
                LogoElemental->SetPosition(ClassesImports::Point(LogoElemental->LocalPosition.X + Shift, LogoElemental->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* LabelElemental = MainPanel->FindByNameRecursive(u"LabelElemental"sv);
                LabelElemental->SetPosition(ClassesImports::Point(LabelElemental->LocalPosition.X + Shift, LabelElemental->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* Logo1C = MainPanel->FindByNameRecursive(u"Logo1C"sv);
                Shift = Logo1C->LocalPosition.X * GR_Main::GameScreenWidth / 1024 - Logo1C->LocalPosition.X;
                Logo1C->SetPosition(ClassesImports::Point(Logo1C->LocalPosition.X + Shift, Logo1C->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* Label1C = MainPanel->FindByNameRecursive(u"Label1C"sv);
                Label1C->SetPosition(ClassesImports::Point(Label1C->LocalPosition.X + Shift, Label1C->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* LogoKatauri = MainPanel->FindByNameRecursive(u"LogoKatauri"sv);
                Shift = LogoKatauri->LocalPosition.X * GR_Main::GameScreenWidth / 1024 - LogoKatauri->LocalPosition.X;
                LogoKatauri->SetPosition(ClassesImports::Point(LogoKatauri->LocalPosition.X + Shift, LogoKatauri->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* LabelKatauri = MainPanel->FindByNameRecursive(u"LabelKatauri"sv);
                LabelKatauri->SetPosition(ClassesImports::Point(LabelKatauri->LocalPosition.X + Shift, LabelKatauri->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* LogoSnk = MainPanel->FindByNameRecursive(u"LogoSnk"sv);
                Shift = LogoSnk->LocalPosition.X * GR_Main::GameScreenWidth / 1024 - LogoSnk->LocalPosition.X;
                LogoSnk->SetPosition(ClassesImports::Point(LogoSnk->LocalPosition.X + Shift, LogoSnk->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* LabelSnk = MainPanel->FindByNameRecursive(u"LabelSnk"sv);
                LabelSnk->SetPosition(ClassesImports::Point(LabelSnk->LocalPosition.X + Shift, LabelSnk->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* PAbout = MainPanel->FindByNameRecursive(u"PAbout"sv);
                PAbout->SetPosition(ClassesImports::Point(PAbout->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PAbout->LocalPosition.Y));
                PAbout->SetSize(ClassesImports::Point(PAbout->ClientSize.X, PAbout->ClientSize.Y + GR_Main::ExtraScreenHeight));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        ViewportPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PAbout"sv));
        CreditsPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PAboutI"sv));
        {
            GI_MessageLoop::TObjectGI* MainPanel_2 = GetByName(u"MainPanel"sv);
            MainPanel_2->KeyDownCallback = pas::bind_method<&TfAbout::CloseKeyDown>(this);
            MainPanel_2->LeftButtonUpCallback = pas::bind_method<&TfAbout::CloseMouseDown>(this);
            MainPanel_2->RightButtonUpCallback = pas::bind_method<&TfAbout::CloseMouseDown>(this);
        }
    }

    void TfAbout::OnOpen() {
        std::int32_t I{};
        pas::WideString Kind{};
        FirstMusicSelection = true;
        GR_Main::MusicManager->RequestFadeOut();
        if (ScrollTimer != nullptr) {
            CancelCallbackTimer(ScrollTimer);
            ScrollTimer = nullptr;
        }
        ScrollTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfAbout::ScrollCredits>(this), 0);
        CreditsPanel->SetPosition(ClassesImports::Point(0, ViewportPanel->ClientSize.Y));
        ClearCredits();
        EC_BlockPar::TBlockParEC* Block = GR_Main::LanguageDataConfig->GetBlock(u"FormAbout"sv);
        std::int32_t Count = Block->GetParamCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Kind = Block->GetParamName(I);
            if (Kind == u"T" || Kind == u"N") {
                if (Kind == u"T") {
                    AddCreditLine(Block->GetParamValue(I), 105, 235, 235);
                } else {
                    AddCreditLine(Block->GetParamValue(I), 255, 255, 255);
                }
            } else if (Kind == u"S") {
                Kind = Block->GetParamValue(I);
                AddCreditSpacing(GR_Main::GiScalePixels(EC_Str::ExtractDigitsToIntW(pas::view(Kind))));
            } else if (Kind == u"L") {
                AddCreditSeparator();
            }
        }
        CreditsPanel->SetSize(ClassesImports::Point(CreditsPanel->ClientSize.X, CreditsHeight));
    }

    void TfAbout::OnClose() {
        if (ScrollTimer != nullptr) {
            CancelCallbackTimer(ScrollTimer);
            ScrollTimer = nullptr;
        }
    }

    void TfAbout::ClearCredits() {
        CreditsHeight = 0;
        CreditsPanel->FreeOwnedChildren();
    }

    void TfAbout::AddCreditLine(pas::WideString Text, std::uint8_t Red, std::uint8_t Green, std::uint8_t Blue) {
        GI_Label::TLabelGI* LabelControl = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, CreditsPanel);
        LabelControl->SetPosition(ClassesImports::Point(0, CreditsHeight));
        LabelControl->SetSize(ClassesImports::Point(ViewportPanel->ClientSize.X, 1));
        LabelControl->SetFontName(GlobalsV::AuthorsFontName);
        LabelControl->SetWordWrapEnabled(false);
        LabelControl->SetPositionModeW(false);
        LabelControl->SetTextAlignX(GI_Main::taxCenter);
        LabelControl->SetTextAlignY(GI_Main::tayAuto);
        LabelControl->SetText(Text);
        LabelControl->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue));
        LabelControl->SetTextAlignX(GI_Main::taxCenter);
        LabelControl->SetTextAlignY(GI_Main::tayCenter);
        LabelControl->SetSize(ClassesImports::Point(LabelControl->ClientSize.X, LabelControl->ClientSize.Y + 4));
        CreditsHeight += LabelControl->ClientSize.Y;
    }

    void TfAbout::AddCreditSeparator() {
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, CreditsPanel);
        Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormAbout2.", GR_Main::GiResourceSuffix(), u"Line"}));
        Image->SetPosition(ClassesImports::Point(0, CreditsHeight));
        {
            std::int32_t cpp_arg = Image->GetContentSize().Y + 4;
            std::int32_t x = ViewportPanel->ClientSize.X;
            Image->SetSize(ClassesImports::Point(x, cpp_arg));
        }
        Image->SetPositionModeW(false);
        Image->SetImageKindX(GI_Main::ikxCenter);
        Image->SetImageKindY(GI_Main::ikyCenter);
        CreditsHeight += Image->ClientSize.Y;
    }

    void TfAbout::AddCreditSpacing(std::int32_t Height) {
        CreditsHeight += Height;
    }

    void TfAbout::ScrollCredits(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        CreditsPanel->SetPosition(ClassesImports::Point(0, CreditsPanel->LocalPosition.Y - 1));
        if (-CreditsPanel->LocalPosition.Y >= CreditsPanel->ClientSize.Y) {
            CreditsPanel->SetPosition(ClassesImports::Point(0, ViewportPanel->ClientSize.Y));
        }
    }

    void TfAbout::CloseMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, Types::TPoint Point) {
        if (ReturnToScores) {
            GlobalsV::RequestedScreenId = GlobalsV::screenScores;
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
        }
        RequestClose(1);
    }

    void TfAbout::CloseKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (ReturnToScores) {
            GlobalsV::RequestedScreenId = GlobalsV::screenScores;
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
        }
        RequestClose(1);
    }

    void TfAbout::SelectMusic() {
        if (FirstMusicSelection) {
            GR_Main::MusicManager->PlayCategory(u"Song"_wref.get());
            FirstMusicSelection = false;
        } else {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
        }
    }

} // namespace fAbout
