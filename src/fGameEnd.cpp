#include "layout/fGameEnd.hpp"
#include "types/EC_Buf.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyEvent.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/fAbout.hpp"
#include "types/fSaveManager.hpp"
#include "types/fScore.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Frame.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SimpleSteamApi.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/ThreadCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/fGameEnd.hpp"

namespace fGameEnd {
    void TfGameEnd::InitializeLayout() {
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fGameEnd... "_a);
        ViewportRect = ClassesImports::Rect(GR_Main::ExtraScreenWidth / 2, GR_Main::ExtraScreenHeight / 2, ViewportRect.Left + GR_Main::ExtraScreenWidth / 2, ViewportRect.Top + GR_Main::ExtraScreenHeight / 2);
        {
            GI_Frame::TFrameGI* cpp_with = pas::construct_call<GI_Frame::TFrameGI>(GI_Frame::TFrameGI_Create, GetByName(u"MainPanel"sv));
            cpp_with->SetName(u"FrameLoad"_wref.get());
            cpp_with->SetFillColor(0u);
            cpp_with->SetFill(true);
            cpp_with->SetKind(GI_Frame::fkRect);
            cpp_with->SetPosition(ClassesImports::Point(0, 0));
            cpp_with->SetDepth(1.0);
            cpp_with->SetSize(ClassesImports::Point(0, 0));
        }
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* ImageFon1 = MainPanel->FindByNameRecursive(u"ImageFon1"sv);
                ImageFon1->SetPosition(ClassesImports::Point(ImageFon1->LocalPosition.X, GR_Main::GameScreenHeight - ImageFon1->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ImageFon2 = MainPanel->FindByNameRecursive(u"ImageFon2"sv);
                ImageFon2->SetPosition(ClassesImports::Point(ImageFon2->LocalPosition.X, GR_Main::GameScreenHeight - ImageFon2->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ImageTop = MainPanel->FindByNameRecursive(u"ImageTop"sv);
                ImageTop->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, ImageTop->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ImageBottom = MainPanel->FindByNameRecursive(u"ImageBottom"sv);
                ImageBottom->SetPosition(ClassesImports::Point(ImageBottom->LocalPosition.X, ImageBottom->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                ImageBottom->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, ImageBottom->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* ImageScreen = MainPanel->FindByNameRecursive(u"ImageScreen"sv);
                ImageScreen->SetPosition(ClassesImports::Point(ImageScreen->LocalPosition.X, ImageScreen->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Maloc = MainPanel->FindByNameRecursive(u"Maloc"sv);
                Maloc->SetPosition(ClassesImports::Point(Maloc->LocalPosition.X, Maloc->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Peleng = MainPanel->FindByNameRecursive(u"Peleng"sv);
                Peleng->SetPosition(ClassesImports::Point(Peleng->LocalPosition.X, Peleng->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Fei = MainPanel->FindByNameRecursive(u"Fei"sv);
                Fei->SetPosition(ClassesImports::Point(Fei->LocalPosition.X, Fei->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Gaal = MainPanel->FindByNameRecursive(u"Gaal"sv);
                Gaal->SetPosition(ClassesImports::Point(Gaal->LocalPosition.X, Gaal->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin = MainPanel->FindByNameRecursive(u"PirateWin"sv);
                PirateWin->SetPosition(ClassesImports::Point(PirateWin->LocalPosition.X, PirateWin->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin1Maloc = MainPanel->FindByNameRecursive(u"PirateWin1Maloc"sv);
                PirateWin1Maloc->SetPosition(ClassesImports::Point(PirateWin1Maloc->LocalPosition.X, PirateWin1Maloc->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin1Peleng = MainPanel->FindByNameRecursive(u"PirateWin1Peleng"sv);
                PirateWin1Peleng->SetPosition(ClassesImports::Point(PirateWin1Peleng->LocalPosition.X, PirateWin1Peleng->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin1People = MainPanel->FindByNameRecursive(u"PirateWin1People"sv);
                PirateWin1People->SetPosition(ClassesImports::Point(PirateWin1People->LocalPosition.X, PirateWin1People->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin1Fei = MainPanel->FindByNameRecursive(u"PirateWin1Fei"sv);
                PirateWin1Fei->SetPosition(ClassesImports::Point(PirateWin1Fei->LocalPosition.X, PirateWin1Fei->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin1Gaal = MainPanel->FindByNameRecursive(u"PirateWin1Gaal"sv);
                PirateWin1Gaal->SetPosition(ClassesImports::Point(PirateWin1Gaal->LocalPosition.X, PirateWin1Gaal->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin2 = MainPanel->FindByNameRecursive(u"PirateWin2"sv);
                PirateWin2->SetPosition(ClassesImports::Point(PirateWin2->LocalPosition.X, PirateWin2->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin3Maloc = MainPanel->FindByNameRecursive(u"PirateWin3Maloc"sv);
                PirateWin3Maloc->SetPosition(ClassesImports::Point(PirateWin3Maloc->LocalPosition.X, PirateWin3Maloc->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin3Peleng = MainPanel->FindByNameRecursive(u"PirateWin3Peleng"sv);
                PirateWin3Peleng->SetPosition(ClassesImports::Point(PirateWin3Peleng->LocalPosition.X, PirateWin3Peleng->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin3People = MainPanel->FindByNameRecursive(u"PirateWin3People"sv);
                PirateWin3People->SetPosition(ClassesImports::Point(PirateWin3People->LocalPosition.X, PirateWin3People->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin3Fei = MainPanel->FindByNameRecursive(u"PirateWin3Fei"sv);
                PirateWin3Fei->SetPosition(ClassesImports::Point(PirateWin3Fei->LocalPosition.X, PirateWin3Fei->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin3Gaal = MainPanel->FindByNameRecursive(u"PirateWin3Gaal"sv);
                PirateWin3Gaal->SetPosition(ClassesImports::Point(PirateWin3Gaal->LocalPosition.X, PirateWin3Gaal->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin4Maloc = MainPanel->FindByNameRecursive(u"PirateWin4Maloc"sv);
                PirateWin4Maloc->SetPosition(ClassesImports::Point(PirateWin4Maloc->LocalPosition.X, PirateWin4Maloc->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin4Peleng = MainPanel->FindByNameRecursive(u"PirateWin4Peleng"sv);
                PirateWin4Peleng->SetPosition(ClassesImports::Point(PirateWin4Peleng->LocalPosition.X, PirateWin4Peleng->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin4People = MainPanel->FindByNameRecursive(u"PirateWin4People"sv);
                PirateWin4People->SetPosition(ClassesImports::Point(PirateWin4People->LocalPosition.X, PirateWin4People->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin4Fei = MainPanel->FindByNameRecursive(u"PirateWin4Fei"sv);
                PirateWin4Fei->SetPosition(ClassesImports::Point(PirateWin4Fei->LocalPosition.X, PirateWin4Fei->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin4Gaal = MainPanel->FindByNameRecursive(u"PirateWin4Gaal"sv);
                PirateWin4Gaal->SetPosition(ClassesImports::Point(PirateWin4Gaal->LocalPosition.X, PirateWin4Gaal->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin5Maloc = MainPanel->FindByNameRecursive(u"PirateWin5Maloc"sv);
                PirateWin5Maloc->SetPosition(ClassesImports::Point(PirateWin5Maloc->LocalPosition.X, PirateWin5Maloc->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin5Peleng = MainPanel->FindByNameRecursive(u"PirateWin5Peleng"sv);
                PirateWin5Peleng->SetPosition(ClassesImports::Point(PirateWin5Peleng->LocalPosition.X, PirateWin5Peleng->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin5People = MainPanel->FindByNameRecursive(u"PirateWin5People"sv);
                PirateWin5People->SetPosition(ClassesImports::Point(PirateWin5People->LocalPosition.X, PirateWin5People->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin5Fei = MainPanel->FindByNameRecursive(u"PirateWin5Fei"sv);
                PirateWin5Fei->SetPosition(ClassesImports::Point(PirateWin5Fei->LocalPosition.X, PirateWin5Fei->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin5Gaal = MainPanel->FindByNameRecursive(u"PirateWin5Gaal"sv);
                PirateWin5Gaal->SetPosition(ClassesImports::Point(PirateWin5Gaal->LocalPosition.X, PirateWin5Gaal->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin6 = MainPanel->FindByNameRecursive(u"PirateWin6"sv);
                PirateWin6->SetPosition(ClassesImports::Point(PirateWin6->LocalPosition.X, PirateWin6->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin7 = MainPanel->FindByNameRecursive(u"PirateWin7"sv);
                PirateWin7->SetPosition(ClassesImports::Point(PirateWin7->LocalPosition.X, PirateWin7->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PirateWin8 = MainPanel->FindByNameRecursive(u"PirateWin8"sv);
                PirateWin8->SetPosition(ClassesImports::Point(PirateWin8->LocalPosition.X, PirateWin8->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* MainPanel_2 = MainPanel->FindByNameRecursive(u"MainPanel"sv);
                Control = MainPanel_2->FirstChild;
                while (Control != nullptr) {
                    if (EC_Str::FindTextOffsetW(Control->ControlName, u"CustomEnd"_wref.get(), 0) == 0) {
                        Control->SetPosition(ClassesImports::Point(Control->LocalPosition.X, Control->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
                    }
                    Control = Control->NextSibling;
                }
            }
            {
                GI_MessageLoop::TObjectGI* Ok = MainPanel->FindByNameRecursive(u"Ok"sv);
                Ok->SetPosition(ClassesImports::Point(Ok->LocalPosition.X + GR_Main::ExtraScreenWidth, Ok->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* Load = MainPanel->FindByNameRecursive(u"Load"sv);
                Load->SetPosition(ClassesImports::Point(Load->LocalPosition.X + GR_Main::ExtraScreenWidth, Load->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* LabelHelp = MainPanel->FindByNameRecursive(u"LabelHelp"sv);
                LabelHelp->SetPosition(ClassesImports::Point(LabelHelp->LocalPosition.X + GR_Main::ExtraScreenWidth, LabelHelp->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* PanelText = MainPanel->FindByNameRecursive(u"PanelText"sv);
                PanelText->SetSize(ClassesImports::Point(PanelText->ClientSize.X + GR_Main::ExtraScreenWidth, PanelText->ClientSize.Y + GR_Main::ExtraScreenHeight));
                {
                    GI_MessageLoop::TObjectGI* GBText = PanelText->FindByNameRecursive(u"GBText"sv);
                    GBText->SetSize(ClassesImports::Point(GBText->ClientSize.X + GR_Main::ExtraScreenWidth, GBText->ClientSize.Y));
                }
            }
            GR_Main::AppendLogLineThreadSafe("ok"_a);
        }
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfGameEnd::MainPanelKeyDown>(this);
        SetHelpCallback(pas::bind_method<&TfGameEnd::ShowControlHelp>(this));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->UpCallback = pas::bind_method<&TfGameEnd::ContinueClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Load"sv))->UpCallback = pas::bind_method<&TfGameEnd::LoadClicked>(this);
    }

    void TfGameEnd::OnOpen() {
        pas::WideString Text{};
        pas::WideString DeathCause{};
        WindowsSdk::TPoint Position{};
        WindowsSdk::TPoint Size{};
        std::int32_t I{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        GI_MessageLoop::TObjectGI* Control{};
        pas::WideString CustomText{};
        pas::WideString CustomPicture{};
        ThreadCalc::WaitForTurnCalculation();
        if (SimpleSteamApi::SteamInitialized && static_cast<std::uint8_t>(SimpleSteamApi::SteamLeaderboardFound() ^ 1)) {
            SimpleSteamApi::SteamSetLeaderboardName("Scores"_a);
        }
        std::uint8_t CustomWin = false;
        std::uint8_t CustomLoss = false;
        if (GlobalsV::GameEndReason == GlobalsV::gerDefault) {
            const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Event = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I);
                    if (Event->EventType == u"CustomLose") {
                        CustomText = Event->GetTextData(0);
                        CustomPicture = Event->GetTextData(1);
                        CustomLoss = true;
                        break;
                    }
                    if (Event->EventType == u"CustomWin") {
                        CustomText = Event->GetTextData(0);
                        CustomPicture = Event->GetTextData(1);
                        CustomWin = true;
                        break;
                    }
                }
            }
        }
        if (GlobalsV::GameEndReason > 4 || CustomWin) {
            pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageFon1"sv))->SetImagePath(u"GI,Bm.FormIntro2.2bg"_w);
            pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageFon2"sv))->SetImagePath(u"GI,Bm.FormIntro2.2bg"_w);
        } else {
            pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageFon1"sv))->SetImagePath(u"GI,Bm.FormEnd2.2bg"_w);
            pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageFon2"sv))->SetImagePath(u"GI,Bm.FormEnd2.2bg"_w);
        }
        BackgroundScrollOffset = 0;
        if (BackgroundTimer != nullptr) {
            CancelCallbackTimer(BackgroundTimer);
            BackgroundTimer = nullptr;
        }
        if (GlobalsV::AnimMainFon) {
            BackgroundTimer = ScheduleCallbackTimer(40, 40, pas::bind_method<&TfGameEnd::ScrollBackground>(this), 0);
        }
        ScrollBackground(nullptr, 0);
        SelectMusic();
        if (aPlayer::GetPlayer() != nullptr) {
            Globals::ScoreScreen->RecordPlayerResult(aPlayer::GetPlayer() != nullptr && GlobalsV::GameEndReason != GlobalsV::gerPlayerDeath && static_cast<std::uint8_t>(CustomLoss ^ 1));
        }
        fScore::TfScoreUnit* Score = pas::list_at<fScore::TfScoreUnit>(Globals::ScoreScreen->Entries, Globals::ScoreScreen->SelectedIndex);
        std::uint8_t DefaultLoss = GlobalsV::GameEndReason <= 4 && static_cast<std::uint8_t>(CustomWin ^ 1) && CustomPicture == u"";
        GetByName(u"Maloc"sv)->SetActive(Score->PilotRace == aGalaxyStruct::oiMaloc && DefaultLoss);
        GetByName(u"Peleng"sv)->SetActive(Score->PilotRace == aGalaxyStruct::oiPeleng && DefaultLoss);
        GetByName(u"Fei"sv)->SetActive(Score->PilotRace == aGalaxyStruct::oiFeyan && DefaultLoss);
        GetByName(u"Gaal"sv)->SetActive(Score->PilotRace == aGalaxyStruct::oiGaal && DefaultLoss);
        GetByName(u"PirateWin1Maloc"sv)->SetActive(pas::in_set<5, 5, 9, 9, 11, 11>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiMaloc);
        GetByName(u"PirateWin1Peleng"sv)->SetActive(pas::in_set<5, 5, 9, 9, 11, 11>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiPeleng);
        GetByName(u"PirateWin1People"sv)->SetActive(pas::in_set<5, 5, 9, 9, 11, 11>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiHuman);
        GetByName(u"PirateWin1Fei"sv)->SetActive(pas::in_set<5, 5, 9, 9, 11, 11>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiFeyan);
        GetByName(u"PirateWin1Gaal"sv)->SetActive(pas::in_set<5, 5, 9, 9, 11, 11>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiGaal);
        GetByName(u"PirateWin2"sv)->SetActive(pas::in_set<6, 6, 10, 10>(GlobalsV::GameEndReason));
        GetByName(u"PirateWin3Maloc"sv)->SetActive(pas::in_set<7, 7, 18, 18>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiMaloc);
        GetByName(u"PirateWin3Peleng"sv)->SetActive(pas::in_set<7, 7, 18, 18>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiPeleng);
        GetByName(u"PirateWin3People"sv)->SetActive(pas::in_set<7, 7, 18, 18>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiHuman);
        GetByName(u"PirateWin3Fei"sv)->SetActive(pas::in_set<7, 7, 18, 18>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiFeyan);
        GetByName(u"PirateWin3Gaal"sv)->SetActive(pas::in_set<7, 7, 18, 18>(GlobalsV::GameEndReason) && Score->PilotRace == aGalaxyStruct::oiGaal);
        GetByName(u"PirateWin4Maloc"sv)->SetActive(GlobalsV::GameEndReason == 8 && Score->PilotRace == aGalaxyStruct::oiMaloc);
        GetByName(u"PirateWin4Peleng"sv)->SetActive(GlobalsV::GameEndReason == 8 && Score->PilotRace == aGalaxyStruct::oiPeleng);
        GetByName(u"PirateWin4People"sv)->SetActive(GlobalsV::GameEndReason == 8 && Score->PilotRace == aGalaxyStruct::oiHuman);
        GetByName(u"PirateWin4Fei"sv)->SetActive(GlobalsV::GameEndReason == 8 && Score->PilotRace == aGalaxyStruct::oiFeyan);
        GetByName(u"PirateWin4Gaal"sv)->SetActive(GlobalsV::GameEndReason == 8 && Score->PilotRace == aGalaxyStruct::oiGaal);
        GetByName(u"PirateWin5Maloc"sv)->SetActive(pas::in_range(GlobalsV::GameEndReason, 12, 14) && Score->PilotRace == aGalaxyStruct::oiMaloc);
        GetByName(u"PirateWin5Peleng"sv)->SetActive(pas::in_range(GlobalsV::GameEndReason, 12, 14) && Score->PilotRace == aGalaxyStruct::oiPeleng);
        GetByName(u"PirateWin5People"sv)->SetActive(pas::in_range(GlobalsV::GameEndReason, 12, 14) && Score->PilotRace == aGalaxyStruct::oiHuman);
        GetByName(u"PirateWin5Fei"sv)->SetActive(pas::in_range(GlobalsV::GameEndReason, 12, 14) && Score->PilotRace == aGalaxyStruct::oiFeyan);
        GetByName(u"PirateWin5Gaal"sv)->SetActive(pas::in_range(GlobalsV::GameEndReason, 12, 14) && Score->PilotRace == aGalaxyStruct::oiGaal);
        GetByName(u"PirateWin6"sv)->SetActive(GlobalsV::GameEndReason == 15);
        GetByName(u"PirateWin7"sv)->SetActive(GlobalsV::GameEndReason == 16);
        GetByName(u"PirateWin8"sv)->SetActive(GlobalsV::GameEndReason == 17);
        GetByName(u"PirateWin"sv)->SetActive(GlobalsV::GameEndReason > 18 || CustomWin && CustomPicture == u"");
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            Control = MainPanel->FirstChild;
            while (Control != nullptr) {
                if (EC_Str::FindTextOffsetW(Control->ControlName, u"CustomEnd"_wref.get(), 0) == 0) {
                    Control->SetActive(Control->ControlName == pas::concat_wide({u"CustomEnd", CustomPicture}));
                }
                Control = Control->NextSibling;
            }
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Load"sv))->SetDisabled(GlobalsV::GameEndReason > 4);
        if (GlobalsV::GameEndReason > 4) {
            {
                GI_MessageLoop::TObjectGI* Load = GetByName(u"Load"sv);
                Position = Load->LocalPosition;
                Size = Load->ClientSize;
                Position.X -= 5;
                Position.Y -= 5;
                Size.X += 10;
                Size.Y += 10;
            }
            {
                GI_MessageLoop::TObjectGI* FrameLoad = GetByName(u"FrameLoad"sv);
                FrameLoad->SetPosition(Position);
                FrameLoad->SetSize(Size);
            }
        }
        {
            GI_MessageLoop::TObjectGI* PanelText = GetByName(u"PanelText"sv);
            TextPanelTop = PanelText->LocalPosition.Y;
            TextPanelHeight = PanelText->ClientSize.Y;
        }
        if (GlobalsV::GameEndReason > 4) {
            Text = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"FormGameEnd.WinPirate", SysUtils::IntToStr(GlobalsV::GameEndReason)})));
        } else if (GlobalsV::GameEndReason == GlobalsV::gerTerronConversion) {
            if (aGalaxy::Galaxy->CoalitionDefeatedTurn != 0) {
                Text = aConst::LocalizedColorText(u"FormGameEnd.LossConvertToTerron3"_wref.get());
            } else if (aGalaxy::Galaxy->PirateWinType != 3) {
                Text = aConst::LocalizedColorText(u"FormGameEnd.LossConvertToTerron2"_wref.get());
            } else if (aGalaxy::Galaxy->KellerSeriesResolvedTurn != 0 || aGalaxy::Galaxy->BlazerSeriesResolvedTurn != 0) {
                Text = aConst::LocalizedColorText(u"FormGameEnd.LossConvertToTerron"_wref.get());
            } else {
                Text = aConst::LocalizedColorText(u"FormGameEnd.LossConvertToTerron1"_wref.get());
            }
        } else if (CustomText != u"") {
            Text = std::move(CustomText);
        } else {
            Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.Loss"_wref.get(), pas::random(100000, &System::RandSeed));
            Event = nullptr;
            {
                const std::int32_t cpp_first_2 = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
                if (cpp_first_2 >= 0) {
                    for (I = cpp_first_2; I >= 0; --I) {
                        if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn < aGalaxy::Galaxy->CurrentTurn) {
                            break;
                        }
                        if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->EventType == u"PlayerDeath") {
                            Event = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I);
                        }
                    }
                }
            }
            if (Event != nullptr) {
                DeathCause = Event->GetTextData(0);
                if (DeathCause == u"PlanetCaptured") {
                    Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.LossInPlanet"_wref.get(), pas::random(100000, &System::RandSeed));
                    if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                        if (aPlayer::GetPlayer()->CurrentPlanet->CustomFaction != u"" && aConst::LocalizedColorText(pas::concat_wide({u"FormGameEnd.LossInPlanet", aPlayer::GetPlayer()->CurrentPlanet->CustomFaction})) != u"") {
                            Text = ([&] {
                                std::int32_t cpp_arg = pas::random(100000, &System::RandSeed);
                                const pas::WideString& cpp_arg_2 = pas::concat_wide({u"FormGameEnd.LossInPlanet", aPlayer::GetPlayer()->CurrentPlanet->CustomFaction});
                                return aConst::PickLocalizedTextVariant(cpp_arg_2, cpp_arg);
                            }());
                        } else if (aPlayer::GetPlayer()->CurrentPlanet->CurrentStar->Status.CustomFaction != u"" && aConst::LocalizedColorText(pas::concat_wide({u"FormGameEnd.LossInPlanet", aPlayer::GetPlayer()->CurrentPlanet->CurrentStar->Status.CustomFaction})) != u"") {
                            Text = ([&] {
                                std::int32_t cpp_arg_3 = pas::random(100000, &System::RandSeed);
                                const pas::WideString& cpp_arg_4 = pas::concat_wide({u"FormGameEnd.LossInPlanet", aPlayer::GetPlayer()->CurrentPlanet->CurrentStar->Status.CustomFaction});
                                return aConst::PickLocalizedTextVariant(cpp_arg_4, cpp_arg_3);
                            }());
                        }
                        aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                    } else {
                        aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, pas::WideString(), pas::WideString());
                    }
                }
                if (DeathCause == u"StationDestroyed") {
                    Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.LossInShip"_wref.get(), pas::random(100000, &System::RandSeed));
                }
                if (DeathCause == u"KilledByBeamWeapon") {
                    Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.LossKilledByWeaponHit"_wref.get(), pas::random(100000, &System::RandSeed));
                }
                if (DeathCause == u"KilledByMissile") {
                    Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.LossKilledByMissileHit"_wref.get(), pas::random(100000, &System::RandSeed));
                }
                if (DeathCause == u"KilledByInterceptor") {
                    Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.LossKilledByInterceptorHit"_wref.get(), pas::random(100000, &System::RandSeed));
                }
                if (DeathCause == u"KilledByAsteroid") {
                    Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.LossKilledByAsteroidHit"_wref.get(), pas::random(100000, &System::RandSeed));
                }
                if (DeathCause == u"KilledByExplosion") {
                    Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.LossKilledByExplosion"_wref.get(), pas::random(100000, &System::RandSeed));
                }
                if (DeathCause == u"KilledBySunDamage") {
                    Text = aConst::PickLocalizedTextVariant(u"FormGameEnd.LossKilledBySunDamage"_wref.get(), pas::random(100000, &System::RandSeed));
                }
            }
        }
        if (Globals::LastLoadedPlayerName == u"") {
            Globals::LastLoadedPlayerName = u"GPlayerName='"_w;
        }
        aMyFunction::ReplaceTextToken(Text, u"<Player>"_w, Globals::LastLoadedPlayerName, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::FormatGameTurnDate(aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
        if (EC_Str::FindTextOffsetW(Text, u"<Money>"_wref.get(), 0) >= 0) {
            if (aConst::LastMedicalPolicyTicks == 0) {
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, u"10.000"_w, u"<color=255,240,100>"_w);
            } else {
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, u"20.000"_w, u"<color=255,240,100>"_w);
                Achievements::TryUnlockAchievement(u"INSURANCE"_w);
            }
        }
        aMyFunction::ReplaceTextToken(Text, u"<br>"_w, u"\r\n"_w, pas::WideString());
        {
            GI_GraphBuf::TGraphBufGI* GBText = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GBText"sv));
            GBText->SourceHasPerPixelAlpha = true;
            if (GR_Main::ExtraScreenWidth > 127 || GR_Main::ExtraScreenHeight > 127) {
                GI_Label::RenderLabelTextToBuffer(GBText->GraphBuf, GBText->ClientSize.X, 1, 0, Text, u"Font.2Big"_wref.get(), 0xfffff3d2u, 0xff373737u, 0xffdbda9cu);
            } else {
                GI_Label::RenderLabelTextToBuffer(GBText->GraphBuf, GBText->ClientSize.X, 1, 0, Text, u"Font.2Intro"_wref.get(), 0xfffff3d2u, 0xff373737u, 0xffdbda9cu);
            }
            GBText->SetSize(ClassesImports::Point(GBText->ClientSize.X, GBText->GraphBuf->Height));
            GBText->SetPosition(ClassesImports::Point(GBText->LocalPosition.X, TextPanelHeight));
        }
        if (TextScrollTimer != nullptr) {
            CancelCallbackTimer(TextScrollTimer);
            TextScrollTimer = nullptr;
        }
        TextScrollTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfGameEnd::ScrollEndingText>(this), 0);
    }

    void TfGameEnd::OnClose() {
        if (TextScrollTimer != nullptr) {
            CancelCallbackTimer(TextScrollTimer);
            TextScrollTimer = nullptr;
        }
        {
            GI_GraphBuf::TGraphBufGI* GBText = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GBText"sv));
            GBText->GraphBuf->Clear();
        }
        if (BackgroundTimer != nullptr) {
            CancelCallbackTimer(BackgroundTimer);
            BackgroundTimer = nullptr;
        }
    }

    void TfGameEnd::ScrollBackground(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Offset{};
        ++BackgroundScrollOffset;
        {
            GI_MessageLoop::TObjectGI* ImageFon1 = GetByName(u"ImageFon1"sv);
            Offset = pas::imod(BackgroundScrollOffset, ImageFon1->ClientSize.X);
            ImageFon1->SetPosition(ClassesImports::Point(0 - Offset, ImageFon1->LocalPosition.Y));
        }
        {
            GI_MessageLoop::TObjectGI* ImageFon2 = GetByName(u"ImageFon2"sv);
            ImageFon2->SetPosition(ClassesImports::Point(ImageFon2->ClientSize.X - Offset, ImageFon2->LocalPosition.Y));
        }
    }

    void TfGameEnd::ScrollEndingText(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Limit{};
        {
            GI_GraphBuf::TGraphBufGI* GBText = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GBText"sv));
            GBText->SetPosition(EC_Struct::AddPoints(GBText->LocalPosition, ClassesImports::Point(0, -1)));
            Limit = (TextPanelTop + TextPanelHeight) * 0.45L - GBText->ClientSize.Y * 0.5L;
            if (Limit < 0.0L) {
                Limit = 2.0L * Limit;
            }
            if (static_cast<long double>(GBText->LocalPosition.Y) < Limit) {
                if (TextScrollTimer != nullptr) {
                    CancelCallbackTimer(TextScrollTimer);
                    TextScrollTimer = nullptr;
                }
            }
        }
    }

    void TfGameEnd::ContinueClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
            pas::free(aSaveLoad::MemorySnapshotBuffer);
        }
        aSaveLoad::MemorySnapshotBuffer = nullptr;
        GlobalsV::MemorySnapshotActive = false;
        if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
            pas::free(aGalaxy::Galaxy);
        }
        aGalaxy::Galaxy = nullptr;
        if (GlobalsV::GameEndReason <= 4) {
            GlobalsV::RequestedScreenId = GlobalsV::screenScores;
        } else {
            Globals::AboutScreen->ReturnToScores = true;
            GlobalsV::RequestedScreenId = GlobalsV::screenAbout;
        }
        RequestClose(1);
        GI_Main::BreakUiMessage();
    }

    void TfGameEnd::LoadClicked(GI_MessageLoop::TObjectGI* Sender) {
        ShowControlHelp(nullptr, false);
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        Globals::SaveManagerMode = fSaveManager::smmLoad;
        GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
        RequestClose(1);
    }

    void TfGameEnd::ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Show) {
        GI_Label::TLabelGI* LabelControl = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LabelHelp"sv));
        if (Sender == nullptr || Sender->HelpText == u"" || Sender->IsOccludedAtPoint(GetCursorPoint())) {
            Show = false;
        }
        LabelControl->SetActive(Show);
        if (Sender != nullptr) {
            LabelControl->SetText(Sender->HelpText);
        }
    }

    void TfGameEnd::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Key == WindowsSdk::VK_F3) {
                LoadClicked(nullptr);
            } else if (Key == WindowsSdk::VK_ESCAPE || Key == WindowsSdk::VK_RETURN) {
                ContinueClicked(nullptr);
            }
        }
    }

    void TfGameEnd::SelectMusic() {
        std::int32_t I{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (GlobalsV::GameEndReason > 4) {
            GR_Main::MusicManager->PlayCategory(u"Win"_wref.get());
            return;
        }
        if (GlobalsV::GameEndReason != GlobalsV::gerDefault || aGalaxy::Galaxy == nullptr) {
            GR_Main::MusicManager->PlayCategory(u"Loss"_wref.get());
            return;
        }
        if (GlobalsV::GameEndReason == GlobalsV::gerDefault) {
            const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Event = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I);
                    if (aGalaxy::Galaxy->CurrentTurn > Event->Turn) {
                        break;
                    }
                    if (Event->EventType == u"CustomLose") {
                        GR_Main::MusicManager->PlayCategory(u"Loss"_wref.get());
                        return;
                    }
                    if (Event->EventType == u"CustomWin") {
                        GR_Main::MusicManager->PlayCategory(u"Win"_wref.get());
                        return;
                    }
                }
            }
        }
        GR_Main::MusicManager->PlayCategory(u"Loss"_wref.get());
    }

} // namespace fGameEnd
