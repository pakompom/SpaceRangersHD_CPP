#include "layout/fMainForm.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/EC_Data.hpp"
#include "types/EC_File.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_Label.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aItem.hpp"
#include "types/fAbout.hpp"
#include "types/fLoadAB.hpp"
#include "types/fLoadQuest.hpp"
#include "types/fLoadRobot.hpp"
#include "types/fSaveManager.hpp"
#include "types/fShip2.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Robot.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/fMainForm.hpp"
#include "units/fMods.hpp"
#include "units/fPanelLoad.hpp"

namespace fMainForm {
    void TfMainForm_Create(TfMainForm* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfMainForm_Destroy(TfMainForm* Self) {
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfMainForm::InitializeLayout() {
        std::int32_t OffsetX{};
        std::int32_t OffsetY{};
        std::int32_t LogoShift{};
        pas::WideString Extension{};
        GI_MessageLoop::TObjectGI* CaptionControl{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fMainForm... "_a);
        {
            GI_Label::TLabelGI* LVersion = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LVersion"_wref.get()));
            if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1280) {
                LVersion->SetPosition(ClassesImports::Point(LVersion->LocalPosition.X + GR_Main::ExtraScreenWidth, LVersion->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                LVersion->SetText(EC_Str::ReplaceAllWideString(aConst::LocalizedText(u"FormMain.Version"_wref.get()), u"<Value>"_wref.get(), u"2.1.2500"_wref.get()));
                LogoShift = 0;
            } else {
                LVersion->SetPosition(ClassesImports::Point(LVersion->LocalPosition.X + GR_Main::ExtraScreenWidth, LVersion->LocalPosition.Y + GR_Main::ExtraScreenHeight - 7));
                LVersion->SetText(EC_Str::ReplaceAllWideString(aConst::LocalizedText(u"FormMain.Version2"_wref.get()), u"<Value>"_wref.get(), u"2.1.2500"_wref.get()));
                LogoShift = 16;
            }
        }
        {
            GI_MessageLoop::TObjectGI* LogoElemental = GetByName(u"LogoElemental"_wref.get());
            LogoElemental->SetPosition(ClassesImports::Point(LogoElemental->LocalPosition.X + GR_Main::ExtraScreenWidth, LogoElemental->LocalPosition.Y + GR_Main::ExtraScreenHeight - LogoShift));
        }
        {
            GI_MessageLoop::TObjectGI* Logo1C = GetByName(u"Logo1C"_wref.get());
            Logo1C->SetPosition(ClassesImports::Point(Logo1C->LocalPosition.X + GR_Main::ExtraScreenWidth, Logo1C->LocalPosition.Y + GR_Main::ExtraScreenHeight - LogoShift));
        }
        {
            GI_MessageLoop::TObjectGI* LogoKatauri = GetByName(u"LogoKatauri"_wref.get());
            LogoKatauri->SetPosition(ClassesImports::Point(LogoKatauri->LocalPosition.X + GR_Main::ExtraScreenWidth, LogoKatauri->LocalPosition.Y + GR_Main::ExtraScreenHeight - LogoShift));
        }
        {
            GI_MessageLoop::TObjectGI* LogoSNK = GetByName(u"LogoSNK"_wref.get());
            LogoSNK->SetPosition(ClassesImports::Point(LogoSNK->LocalPosition.X + GR_Main::ExtraScreenWidth, LogoSNK->LocalPosition.Y + GR_Main::ExtraScreenHeight - LogoShift));
        }
        if (GR_Main::ExtraScreenWidth != 0 || GR_Main::ExtraScreenHeight != 0) {
            ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
            OffsetX = 0;
            OffsetY = 0;
            if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1600) {
                OffsetX = -250;
                if (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 900 && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) < 1040) {
                    OffsetY = pas::shr(1040 - GR_Main::GameScreenHeight, 1);
                }
            }
            {
                GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
                MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                {
                    GI_MessageLoop::TObjectGI* AnimMain = MainPanel->FindByNameRecursive(u"AnimMain"_wref.get());
                    if (GR_Main::ExtraScreenHeight < 0) {
                        AnimMain->SetPosition(ClassesImports::Point(AnimMain->LocalPosition.X, AnimMain->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                        AnimMain->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, AnimMain->ClientSize.Y));
                    } else {
                        AnimMain->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                    }
                }
                {
                    GI_MessageLoop::TObjectGI* MicroText = MainPanel->FindByNameRecursive(u"MicroText"_wref.get());
                    MicroText->SetPosition(ClassesImports::Point(MicroText->LocalPosition.X, MicroText->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* Circle = MainPanel->FindByNameRecursive(u"Circle"_wref.get());
                    Circle->SetPosition(ClassesImports::Point(Circle->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + OffsetX, Circle->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2 + OffsetY));
                }
                {
                    GI_MessageLoop::TObjectGI* New = MainPanel->FindByNameRecursive(u"New"_wref.get());
                    New->SetPosition(ClassesImports::Point(New->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + OffsetX, New->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2 + OffsetY));
                }
                {
                    GI_MessageLoop::TObjectGI* Score = MainPanel->FindByNameRecursive(u"Score"_wref.get());
                    Score->SetPosition(ClassesImports::Point(Score->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + OffsetX, Score->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2 + OffsetY));
                }
                {
                    GI_MessageLoop::TObjectGI* with_Achievements = MainPanel->FindByNameRecursive(u"Achievements"_wref.get());
                    with_Achievements->SetPosition(ClassesImports::Point(with_Achievements->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + OffsetX, with_Achievements->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2 + OffsetY));
                }
                {
                    GI_MessageLoop::TObjectGI* Load = MainPanel->FindByNameRecursive(u"Load"_wref.get());
                    Load->SetPosition(ClassesImports::Point(Load->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + OffsetX, Load->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2 + OffsetY));
                }
                {
                    GI_MessageLoop::TObjectGI* Settings = MainPanel->FindByNameRecursive(u"Settings"_wref.get());
                    Settings->SetPosition(ClassesImports::Point(Settings->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + OffsetX, Settings->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2 + OffsetY));
                }
                {
                    GI_MessageLoop::TObjectGI* About = MainPanel->FindByNameRecursive(u"About"_wref.get());
                    About->SetPosition(ClassesImports::Point(About->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + OffsetX, About->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2 + OffsetY));
                }
                {
                    GI_MessageLoop::TObjectGI* Exit = MainPanel->FindByNameRecursive(u"Exit"_wref.get());
                    Exit->SetPosition(ClassesImports::Point(Exit->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + OffsetX, Exit->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2 + OffsetY));
                }
                {
                    GI_MessageLoop::TObjectGI* AnimAddonShip = MainPanel->FindByNameRecursive(u"AnimAddonShip"_wref.get());
                    AnimAddonShip->SetPosition(ClassesImports::Point(AnimAddonShip->LocalPosition.X, AnimAddonShip->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LoadRobot = MainPanel->FindByNameRecursive(u"LoadRobot"_wref.get());
                    LoadRobot->SetPosition(ClassesImports::Point(LoadRobot->LocalPosition.X, LoadRobot->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LoadQuest = MainPanel->FindByNameRecursive(u"LoadQuest"_wref.get());
                    LoadQuest->SetPosition(ClassesImports::Point(LoadQuest->LocalPosition.X, LoadQuest->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LoadAB = MainPanel->FindByNameRecursive(u"LoadAB"_wref.get());
                    LoadAB->SetPosition(ClassesImports::Point(LoadAB->LocalPosition.X, LoadAB->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* Mods = MainPanel->FindByNameRecursive(u"Mods"_wref.get());
                    Mods->SetPosition(ClassesImports::Point(Mods->LocalPosition.X, Mods->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LoadRobotCnt = MainPanel->FindByNameRecursive(u"LoadRobotCnt"_wref.get());
                    LoadRobotCnt->SetPosition(ClassesImports::Point(LoadRobotCnt->LocalPosition.X, LoadRobotCnt->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LoadQuestCnt = MainPanel->FindByNameRecursive(u"LoadQuestCnt"_wref.get());
                    LoadQuestCnt->SetPosition(ClassesImports::Point(LoadQuestCnt->LocalPosition.X, LoadQuestCnt->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LoadABCnt = MainPanel->FindByNameRecursive(u"LoadABCnt"_wref.get());
                    LoadABCnt->SetPosition(ClassesImports::Point(LoadABCnt->LocalPosition.X, LoadABCnt->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* ModsCnt = MainPanel->FindByNameRecursive(u"ModsCnt"_wref.get());
                    ModsCnt->SetPosition(ClassesImports::Point(ModsCnt->LocalPosition.X, ModsCnt->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LRobot = MainPanel->FindByNameRecursive(u"LRobot"_wref.get());
                    LRobot->SetPosition(ClassesImports::Point(LRobot->LocalPosition.X, LRobot->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LQuest = MainPanel->FindByNameRecursive(u"LQuest"_wref.get());
                    LQuest->SetPosition(ClassesImports::Point(LQuest->LocalPosition.X, LQuest->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LAB = MainPanel->FindByNameRecursive(u"LAB"_wref.get());
                    LAB->SetPosition(ClassesImports::Point(LAB->LocalPosition.X, LAB->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* LMods = MainPanel->FindByNameRecursive(u"LMods"_wref.get());
                    LMods->SetPosition(ClassesImports::Point(LMods->LocalPosition.X, LMods->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
                if (GR_Main::ExtraScreenWidth > 0) {
                    CaptionControl = MainPanel->FindByNameRecursive(u"Caption"_wref.get());
                    if (pas::class_cast_if<GI_GAI::TgaiGI*>(CaptionControl) != nullptr) {
                        GI_GAI::TgaiGI* cpp_with_30 = pas::checked_cast<GI_GAI::TgaiGI*>(CaptionControl);
                        if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1600 && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 900) {
                            cpp_with_30->SetSize(ClassesImports::Point(843, 218));
                            cpp_with_30->SetPosition(ClassesImports::Point((GR_Main::GameScreenWidth - cpp_with_30->ClientSize.X) / 2, 57));
                            cpp_with_30->SetImagePath(u"Bm.FormMain3.CaptionLarge"_wref.get());
                        } else {
                            cpp_with_30->SetPosition(ClassesImports::Point(cpp_with_30->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + 50, cpp_with_30->LocalPosition.Y));
                        }
                    } else {
                        GI_Image::TImageGI* cpp_with_31 = pas::checked_cast<GI_Image::TImageGI*>(CaptionControl);
                        if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1600 && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 900) {
                            cpp_with_31->SetSize(ClassesImports::Point(843, 218));
                            cpp_with_31->SetPosition(ClassesImports::Point((GR_Main::GameScreenWidth - cpp_with_31->ClientSize.X) / 2, 57));
                            Extension = EC_Str::ExtractFileExtNoDotW(static_cast<pas::WideString>(SysUtilsImports::Trim(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(GR_Main::CacheDataRoot->FindEntry(u"Bm"_wref.get())->ChildData->FindEntry(u"FormMain3"_wref.get())->ChildData->FindEntry(u"CaptionLarge"_wref.get())->SharedFileRef->FileRef->FileName)))));
                            if (Extension == u"gai") {
                                cpp_with_31->SetImagePath(u"GAI,Bm.FormMain3.CaptionLarge"_w);
                            } else {
                                cpp_with_31->SetImagePath(u"GI,Bm.FormMain3.CaptionLarge"_w);
                            }
                        } else {
                            cpp_with_31->SetPosition(ClassesImports::Point(cpp_with_31->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + 50, cpp_with_31->LocalPosition.Y));
                        }
                    }
                    {
                        GI_Image::TImageGI* CaptionBlur = pas::checked_cast<GI_Image::TImageGI*>(MainPanel->FindByNameRecursive(u"CaptionBlur"_wref.get()));
                        if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1600 && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 900) {
                            CaptionBlur->SetPosition(ClassesImports::Point((GR_Main::GameScreenWidth - CaptionBlur->ClientSize.X) / 2 - 7, -29));
                        } else {
                            CaptionBlur->SetPosition(ClassesImports::Point(CaptionBlur->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + 50, CaptionBlur->LocalPosition.Y));
                        }
                    }
                    {
                        GI_Image::TImageGI* SubName = pas::checked_cast<GI_Image::TImageGI*>(MainPanel->FindByNameRecursive(u"SubName"_wref.get()));
                        if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1600 && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 900) {
                            SubName->SetImagePath(u"GI,Bm.FormMain3.SubLarge"_w);
                            SubName->SetSize(ClassesImports::Point(932, 76));
                            SubName->SetPosition(ClassesImports::Point((GR_Main::GameScreenWidth - SubName->ClientSize.X) / 2 - 54, 320));
                        } else {
                            SubName->SetPosition(ClassesImports::Point(SubName->LocalPosition.X + GR_Main::ExtraScreenWidth / 2 + 50, SubName->LocalPosition.Y));
                        }
                    }
                }
                {
                    GI_MessageLoop::TObjectGI* Planet = MainPanel->FindByNameRecursive(u"Planet"_wref.get());
                    Planet->SetPosition(ClassesImports::Point(Planet->LocalPosition.X, Planet->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                }
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        {
            GI_GraphButton::TGraphButtonGI* LoadRobot_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadRobot"_wref.get()));
            LoadRobot_2->SetActive(Robot::RobotInterface != nullptr);
            LoadRobot_2->UpCallback = pas::bind_method<&TfMainForm::RobotBattleClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* LoadQuest_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadQuest"_wref.get()));
            LoadQuest_2->UpCallback = pas::bind_method<&TfMainForm::TextQuestClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* LoadAB_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadAB"_wref.get()));
            LoadAB_2->UpCallback = pas::bind_method<&TfMainForm::ArcadeBattleClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Mods_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Mods"_wref.get()));
            Mods_2->UpCallback = pas::bind_method<&TfMainForm::ModsClicked>(this);
        }
    }

    void TfMainForm::OnOpen() {
        std::int32_t I{};
        GR_Main::SuppressModRetryPrompt = true;
        LoadPanel->OnOpen();
        LastMenuShipAnimation = -1;
        LastGaalShipAnimation = -1;
        MenuTextState = pas::WideString();
        if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
            pas::free(aSaveLoad::MemorySnapshotBuffer);
        }
        aSaveLoad::MemorySnapshotBuffer = nullptr;
        GlobalsV::MemorySnapshotActive = false;
        if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
            pas::free(aGalaxy::Galaxy);
        }
        aGalaxy::Galaxy = nullptr;
        while (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->Destroying) {
            SysUtilsImports::Sleep(1u);
        }
        I = 0;
        while (FindControlByPath(static_cast<pas::WideString>(pas::concat_ansi({"TempGAI", SysUtils::IntToStr(I)}))) != nullptr) {
            pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TempGAI", SysUtils::IntToStr(I)}))))->RestartPlayback();
            ++I;
        }
        GetByName(u"MainPanel"_wref.get())->MouseMoveCallback = pas::bind_method<&TfMainForm::MainPanelMouseMove>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Exit"_wref.get()))->UpCallback = pas::bind_method<&TfMainForm::QuitClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"New"_wref.get()))->UpCallback = pas::bind_method<&TfMainForm::NewGameClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Load"_wref.get()))->UpCallback = pas::bind_method<&TfMainForm::LoadGameClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Settings"_wref.get()))->UpCallback = pas::bind_method<&TfMainForm::SettingsClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Achievements"_wref.get()))->UpCallback = pas::bind_method<&TfMainForm::AchievementsClicked>(this);
        {
            GI_GraphButton::TGraphButtonGI* Score = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Score"_wref.get()));
            Score->UpCallback = pas::bind_method<&TfMainForm::ScoresClicked>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"About"_wref.get()))->UpCallback = pas::bind_method<&TfMainForm::AboutClicked>(this);
        SelectMusic();
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfMainForm::MainPanelKeyDown>(this);
        BackgroundScrollOffset = 0;
        if (BackgroundTimer != nullptr) {
            CancelCallbackTimer(BackgroundTimer);
            BackgroundTimer = nullptr;
        }
        if (GlobalsV::AnimMainFon) {
            BackgroundTimer = ScheduleCallbackTimer(40, 40, pas::bind_method<&TfMainForm::ScrollBackground>(this), 0);
        }
        ScrollBackground(nullptr, 0);
        if (pas::class_cast_if<GI_GAI::TgaiGI*>(GetByName(u"Logo1C"_wref.get())) != nullptr) {
            GI_GAI::TgaiGI* Logo1C = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"Logo1C"_wref.get()));
            Logo1C->SequenceIndex = 0;
            Logo1C->UpdateAutoGeometry();
            Logo1C->StopAutoPlayback();
            Logo1C->PrimeImageCaches();
        }
        if (pas::class_cast_if<GI_GAI::TgaiGI*>(GetByName(u"LogoElemental"_wref.get())) != nullptr) {
            GI_GAI::TgaiGI* LogoElemental = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"LogoElemental"_wref.get()));
            LogoElemental->SequenceIndex = 0;
            LogoElemental->UpdateAutoGeometry();
            LogoElemental->StopAutoPlayback();
            LogoElemental->PrimeImageCaches();
        }
        {
            GI_GAI::TgaiGI* AnimAddonShip = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimAddonShip"_wref.get()));
            AnimAddonShip->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimMenuShip ^ 1);
            AnimAddonShip->SetPosition(ClassesImports::Point(0, GR_Main::ExtraScreenHeight / 2 + 60));
            AnimAddonShip->SetDepth(29.0);
            if (GlobalsV::AnimMenuShip) {
                AnimAddonShip->SetFirstFrameImagePath(u"Bm.FormMain3.2Ship1"_wref.get());
                AnimAddonShip->SetImagePath(u"Bm.FormMain3.2ShipA1"_wref.get());
                AnimAddonShip->PrimeImageCaches();
                AnimAddonShip->SetFirstFrameImagePath(u"Bm.FormMain3.2Ship2"_wref.get());
                AnimAddonShip->SetImagePath(u"Bm.FormMain3.2ShipA2"_wref.get());
                AnimAddonShip->PrimeImageCaches();
                AnimAddonShip->SetFirstFrameImagePath(u"Bm.FormMain3.2Ship3"_wref.get());
                AnimAddonShip->SetImagePath(u"Bm.FormMain3.2ShipA3"_wref.get());
                AnimAddonShip->PrimeImageCaches();
                AnimAddonShip->CycleCompleteCallback = pas::bind_method<&TfMainForm::MenuShipAnimationFinished>(this);
                MenuShipAnimationFinished(AnimAddonShip->FindByNameRecursive(u"AnimAddonShip"_wref.get()));
            } else {
                AnimAddonShip->SetFirstFrameImagePath(u"Bm.FormMain3.2Ship1"_wref.get());
                AnimAddonShip->PrimeImageCaches();
            }
            AnimAddonShip->SetActive(true);
        }
        if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1600) {
            if (FindControlByPath(u"AnimGaalShip"_wref.get()) != nullptr) {
                GI_GAI::TgaiGI* AnimGaalShip = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimGaalShip"_wref.get()));
                AnimGaalShip->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimMenuShip ^ 1);
                AnimGaalShip->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - 661, pas::shr(GR_Main::GameScreenHeight - 642, 1) + 70));
                AnimGaalShip->SetDepth(29.0);
                if (GlobalsV::AnimMenuShip) {
                    AnimGaalShip->SetFirstFrameImagePath(u"Bm.FormMain3.AnimGaalShip01"_wref.get());
                    AnimGaalShip->SetImagePath(u"Bm.FormMain3.AnimGaalShip01A"_wref.get());
                    AnimGaalShip->PrimeImageCaches();
                    AnimGaalShip->SetFirstFrameImagePath(u"Bm.FormMain3.AnimGaalShip02"_wref.get());
                    AnimGaalShip->SetImagePath(u"Bm.FormMain3.AnimGaalShip02A"_wref.get());
                    AnimGaalShip->PrimeImageCaches();
                    AnimGaalShip->SetFirstFrameImagePath(u"Bm.FormMain3.AnimGaalShip03"_wref.get());
                    AnimGaalShip->SetImagePath(u"Bm.FormMain3.AnimGaalShip03A"_wref.get());
                    AnimGaalShip->PrimeImageCaches();
                    AnimGaalShip->CycleCompleteCallback = pas::bind_method<&TfMainForm::GaalShipAnimationFinished>(this);
                    GaalShipAnimationFinished(AnimGaalShip->FindByNameRecursive(u"AnimGaalShip"_wref.get()));
                } else {
                    AnimGaalShip->SetFirstFrameImagePath(u"Bm.FormMain3.AnimGaalShip01"_wref.get());
                    AnimGaalShip->PrimeImageCaches();
                }
                AnimGaalShip->SetActive(true);
            }
        } else if (FindControlByPath(u"AnimGaalShip"_wref.get()) != nullptr) {
            GetByName(u"AnimGaalShip"_wref.get())->SetActive(false);
        }
        {
            GI_Label::TLabelGI* LoadRobotCnt = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LoadRobotCnt"_wref.get()));
            LoadRobotCnt->SetText(Globals::LoadRobotScreen->GetCompletionSummary());
        }
        {
            GI_Label::TLabelGI* LoadQuestCnt = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LoadQuestCnt"_wref.get()));
            LoadQuestCnt->SetText(Globals::LoadQuestScreen->GetCompletionSummary());
        }
        {
            GI_Label::TLabelGI* LoadABCnt = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LoadABCnt"_wref.get()));
            LoadABCnt->SetText(fLoadAB::TfLoadAB::GetCatalogSummary());
        }
        {
            GI_Label::TLabelGI* ModsCnt = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ModsCnt"_wref.get()));
            if (GR_Main::SkipModsOnReload) {
                ModsCnt->SetText(pas::concat_wide({u"<color=255,0,0>", EC_Str::IntToWideString(EC_Str::CountDelimitedPartsW(GR_Main::SelectedMods, u","_wref.get())), u"</color>"}));
            } else {
                ModsCnt->SetText(EC_Str::IntToWideString(EC_Str::CountDelimitedPartsW(GR_Main::SelectedMods, u","_wref.get())));
            }
        }
        if (GlobalsV::ShowWineWarning) {
            GlobalsV::ShowWineWarning = false;
            GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"Warning.WeRunOnWine"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
        }
        if (GlobalsV::ShowXonarWarning) {
            GlobalsV::ShowXonarWarning = false;
            GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"Warning.XonarDetected"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
        }
    }

    void TfMainForm::OnClose() {
        LoadPanel->OnClose();
    }

    void TfMainForm::QuitClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormMain.MsgExit"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
            GlobalsV::RequestedScreenId = GlobalsV::screenNone;
            RequestClose(1);
        }
    }

    void TfMainForm::NewGameClicked(GI_MessageLoop::TObjectGI* Sender) {
        Globals::ShipScreen->SelectedHoldKind = fShip2::phkEmpty;
        Globals::ShipScreen->SelectedHoldItem = nullptr;
        GlobalsV::RequestedScreenId = GlobalsV::screenNewGame;
        RequestClose(1);
    }

    void TfMainForm::AchievementsClicked(GI_MessageLoop::TObjectGI* Sender) {
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        GlobalsV::AchievementsReturnScreenId = GlobalsV::FormToId(this);
        GlobalsV::RequestedScreenId = GlobalsV::screenAchievements;
        RequestClose(1);
    }

    void TfMainForm::LoadGameClicked(GI_MessageLoop::TObjectGI* Sender) {
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        Globals::SaveManagerMode = fSaveManager::smmLoad;
        GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
        RequestClose(1);
    }

    void TfMainForm::SettingsClicked(GI_MessageLoop::TObjectGI* Sender) {
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        GlobalsV::SettingsReturnScreenId = GlobalsV::FormToId(this);
        GlobalsV::RequestedScreenId = GlobalsV::screenSettings;
        RequestClose(1);
    }

    void TfMainForm::ScoresClicked(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::RequestedScreenId = GlobalsV::screenScores;
        RequestClose(1);
    }

    void TfMainForm::AboutClicked(GI_MessageLoop::TObjectGI* Sender) {
        Globals::AboutScreen->ReturnToScores = false;
        GlobalsV::RequestedScreenId = GlobalsV::screenAbout;
        RequestClose(1);
    }

    void TfMainForm::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            return;
        }
        if (Key == 'Q') {
            reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadQuest"_wref.get()))->ExecuteOnPressCode();
            TextQuestClicked(nullptr);
        } else if (Key == 'R' && GR_Main::IsInstallFeatureEnabled(u"Robot"_wref.get()) && Robot::RobotInterface != nullptr && Robot::RobotInterface->Support() == 0 && FindControlByPath(u"PanelRL"_wref.get()) == nullptr) {
            reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadRobot"_wref.get()))->ExecuteOnPressCode();
            RobotBattleClicked(nullptr);
        } else if (Key == 'A' || Key == 'F') {
            reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadAB"_wref.get()))->ExecuteOnPressCode();
            ArcadeBattleClicked(nullptr);
        } else if (Key == 'M') {
            ModsClicked(nullptr);
        } else if (Key == WindowsSdk::VK_F3 || Key == 'L') {
            reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Load"_wref.get()))->ExecuteOnPressCode();
            GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
            Globals::SaveManagerMode = fSaveManager::smmLoad;
            GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
            RequestClose(1);
        } else if (Key == WindowsSdk::VK_ESCAPE) {
            if (FindControlByPath(u"PanelAB"_wref.get()) != nullptr) {
                ClosePopup();
            } else {
                QuitClicked(nullptr);
            }
        } else if (Key == 'N' || Key == WindowsSdk::VK_RETURN) {
            reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"New"_wref.get()))->ExecuteOnPressCode();
            NewGameClicked(nullptr);
        } else if (Key == 'C') {
            reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Settings"_wref.get()))->ExecuteOnPressCode();
            SettingsClicked(nullptr);
        }
    }

    void TfMainForm::MainPanelMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        RefreshTimerTick();
    }

    void TfMainForm::MenuShipAnimationFinished(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        Index = 1;
        do {
            std::int32_t cpp_case = aMyFunction::RandomIntRange(0, 99);
            if (cpp_case >= 0 && cpp_case <= 49) {
                Index = 1;
            } else if (cpp_case >= 50 && cpp_case <= 69) {
                Index = 2;
            } else if (cpp_case >= 70 && cpp_case <= 99) {
                Index = 3;
            }
        } while (!(Index == 1 || Index != LastMenuShipAnimation));
        LastMenuShipAnimation = Index;
        {
            GI_GAI::TgaiGI* AnimAddonShip = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimAddonShip"_wref.get()));
            AnimAddonShip->SetFirstFrameImagePath(pas::concat_wide({u"Bm.FormMain3.", GR_Main::GiResourceSuffix(), u"Ship", pas::wide_int_to_str(Index)}));
            AnimAddonShip->SetImagePath(pas::concat_wide({u"Bm.FormMain3.", GR_Main::GiResourceSuffix(), u"ShipA", pas::wide_int_to_str(Index)}));
            AnimAddonShip->SequenceIndex = 0;
            AnimAddonShip->UpdateAutoGeometry();
            AnimAddonShip->RestartPlayback();
        }
    }

    void TfMainForm::GaalShipAnimationFinished(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        Index = 1;
        do {
            std::int32_t cpp_case = aMyFunction::RandomIntRange(0, 99);
            if (cpp_case >= 0 && cpp_case <= 49) {
                Index = 1;
            } else if (cpp_case >= 50 && cpp_case <= 69) {
                Index = 2;
            } else if (cpp_case >= 70 && cpp_case <= 99) {
                Index = 3;
            }
        } while (!(Index == 1 || Index != LastGaalShipAnimation));
        LastGaalShipAnimation = Index;
        {
            GI_GAI::TgaiGI* AnimGaalShip = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"AnimGaalShip"_wref.get()));
            AnimGaalShip->SetFirstFrameImagePath(static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormMain3.AnimGaalShip0", SysUtils::IntToStr(Index)})));
            AnimGaalShip->SetImagePath(static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormMain3.AnimGaalShip0", SysUtils::IntToStr(Index), "A"})));
            AnimGaalShip->SequenceIndex = 0;
            AnimGaalShip->UpdateAutoGeometry();
            AnimGaalShip->RestartPlayback();
        }
    }

    void TfMainForm::ScrollBackground(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Offset{};
        ++BackgroundScrollOffset;
        {
            GI_MessageLoop::TObjectGI* ImageFon1 = GetByName(u"ImageFon1"_wref.get());
            Offset = pas::imod(BackgroundScrollOffset, ImageFon1->ClientSize.X);
            ImageFon1->SetPosition(ClassesImports::Point(0 - Offset, 0));
        }
        {
            GI_MessageLoop::TObjectGI* ImageFon2 = GetByName(u"ImageFon2"_wref.get());
            ImageFon2->SetPosition(ClassesImports::Point(ImageFon2->ClientSize.X - Offset, 0));
        }
    }

    void TfMainForm::ClosePopup() {
        GI_MessageLoop::TObjectGI* Control{};
        PopupState = 0;
        if (FindControlByPath(u"PanelAB"_wref.get()) != nullptr) {
            Control = FindControlByPath(u"PanelAB"_wref.get());
            Control->Invalidate();
            pas::free(Control);
        }
    }

    void TfMainForm::RobotBattleClicked(GI_MessageLoop::TObjectGI* Sender) {
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        GlobalsV::RequestedScreenId = GlobalsV::screenLoadRobot;
        RequestClose(1);
    }

    void TfMainForm::TextQuestClicked(GI_MessageLoop::TObjectGI* Sender) {
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        GlobalsV::RequestedScreenId = GlobalsV::screenLoadQuest;
        RequestClose(1);
    }

    void TfMainForm::ArcadeBattleClicked(GI_MessageLoop::TObjectGI* Sender) {
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        GlobalsV::RequestedScreenId = GlobalsV::screenLoadArcade;
        RequestClose(1);
    }

    void TfMainForm::ModsClicked(GI_MessageLoop::TObjectGI* Sender) {
        SetCursorActive(false);
        Present();
        SetCursorActive(true);
        fMods::ShowModsManager(this);
        if (Globals::ReloadModsRequested) {
            GlobalsV::RequestedScreenId = GlobalsV::screenNone;
            GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
            RequestClose(1);
        }
    }

    void TfMainForm::SelectMusic() {
        GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
    }

    void TfMainForm::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        aScript::ExecuteGameplayUiCode(Block, Key);
    }

    void TfMainForm::p_destroy() {
        fMainForm::TfMainForm_Destroy(this);
    }

} // namespace fMainForm
