#include "layout/fCfgSettings.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_CountBar.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Robot.hpp"
#include "units/SimpleSteamApi.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/fCfgSettings.hpp"

namespace fCfgSettings {
    std::uint32_t SettingsModeColorNormal{};

    std::uint32_t SettingsModeColorHighlighted{};

    // Uses the low 32 bits of a timestamp-counter delta across a 200 ms sleep; temporarily raises process/thread priority.
    double EstimateCpuClockMHz() {
        // Manual port: this duplicates GR_Main's RDTSC sampling and priority restoration.
        return GR_Main::MeasureCpuClockMHz();
    }

    void TfCfgSettings::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fCfgSettings... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* cpp_with_2 = MainPanel->FirstChild;
                cpp_with_2->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                {
                    GI_MessageLoop::TObjectGI* cpp_with_3 = cpp_with_2->NextSibling;
                    cpp_with_3->SetPosition(ClassesImports::Point(cpp_with_3->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, cpp_with_3->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                    {
                        GI_MessageLoop::TObjectGI* cpp_with_4 = cpp_with_3->NextSibling;
                        cpp_with_4->SetPosition(ClassesImports::Point(cpp_with_4->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, cpp_with_4->LocalPosition.Y));
                        {
                            GI_MessageLoop::TObjectGI* cpp_with_5 = cpp_with_4->NextSibling;
                            cpp_with_5->SetPosition(ClassesImports::Point(GR_Main::ExtraScreenWidth / 2 + cpp_with_5->LocalPosition.X, cpp_with_5->LocalPosition.Y));
                            cpp_with_5->SetSize(ClassesImports::Point(cpp_with_5->ClientSize.X, cpp_with_5->ClientSize.Y + std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0)));
                            {
                                GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(cpp_with_5->FindByNameRecursive(u"PanelSet"sv));
                                PanelSet->SetSize(ClassesImports::Point(PanelSet->ClientSize.X, PanelSet->ClientSize.Y + std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0)));
                                PanelSet->VerticalScrollBar->SetSize(ClassesImports::Point(PanelSet->VerticalScrollBar->ClientSize.X, PanelSet->VerticalScrollBar->ClientSize.Y + std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0)));
                            }
                            {
                                GI_MessageLoop::TObjectGI* cpp_with_7 = cpp_with_5->NextSibling;
                                cpp_with_7->SetPosition(ClassesImports::Point(cpp_with_7->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, cpp_with_7->LocalPosition.Y));
                            }
                        }
                    }
                }
            }
            {
                GI_MessageLoop::TObjectGI* ModeLeftPanel_NextSibling = MainPanel->FindByNameRecursive(u"ModeLeftPanel"sv)->NextSibling;
                ModeLeftPanel_NextSibling->SetSize(ClassesImports::Point(ModeLeftPanel_NextSibling->ClientSize.X, ModeLeftPanel_NextSibling->ClientSize.Y + std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0)));
                {
                    GI_MessageLoop::TObjectGI* cpp_with_9 = ModeLeftPanel_NextSibling->NextSibling;
                    cpp_with_9->SetSize(ClassesImports::Point(cpp_with_9->ClientSize.X, cpp_with_9->ClientSize.Y + std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0)));
                }
            }
            {
                GI_MessageLoop::TObjectGI* cpp_with_10 = MainPanel->FirstChild->NextSibling;
                cpp_with_10->SetDepth(cpp_with_10->Depth - 2.0L);
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        {
            GI_GraphButton::TGraphButtonGI* Cancel = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Cancel"sv));
            Cancel->UpCallback = pas::bind_method<&TfCfgSettings::CancelClicked>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->UpCallback = pas::bind_method<&TfCfgSettings::ApplyClicked>(this);
        GetByName(u"MainPanel"sv)->MouseMoveCallback = pas::bind_method<&TfCfgSettings::MainPanelMouseMove>(this);
        {
            GI_GraphButton::TGraphButtonGI* ButAUp = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButAUp"sv));
            ButAUp->UpCallback = pas::bind_method<&TfCfgSettings::HighPresetClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButAMiddle = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButAMiddle"sv));
            ButAMiddle->UpCallback = pas::bind_method<&TfCfgSettings::MediumPresetClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButADown = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButADown"sv));
            ButADown->UpCallback = pas::bind_method<&TfCfgSettings::LowPresetClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButAAuto = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButAAuto"sv));
            ButAAuto->UpCallback = pas::bind_method<&TfCfgSettings::AutoPresetClicked>(this);
        }
        {
            GI_MessageLoop::TObjectGI* MainPanel_2 = GetByName(u"MainPanel"sv);
            MainPanel_2->KeyDownCallback = pas::bind_method<&TfCfgSettings::MainPanelKeyDown>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
            ButGroup0->UpCallback = pas::bind_method<&TfCfgSettings::GroupClicked>(this);
            ButGroup0->DownCallback = pas::bind_method<&TfCfgSettings::GroupClicked>(this);
            GroupButtonTops[0] = ButGroup0->LocalPosition.Y;
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
            ButGroup1->UpCallback = pas::bind_method<&TfCfgSettings::GroupClicked>(this);
            ButGroup1->DownCallback = pas::bind_method<&TfCfgSettings::GroupClicked>(this);
            GroupButtonTops[1] = ButGroup1->LocalPosition.Y;
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
            ButGroup2->UpCallback = pas::bind_method<&TfCfgSettings::GroupClicked>(this);
            ButGroup2->DownCallback = pas::bind_method<&TfCfgSettings::GroupClicked>(this);
            GroupButtonTops[2] = ButGroup2->LocalPosition.Y;
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup3"sv));
            ButGroup3->UpCallback = pas::bind_method<&TfCfgSettings::GroupClicked>(this);
            ButGroup3->DownCallback = pas::bind_method<&TfCfgSettings::GroupClicked>(this);
            GroupButtonTops[3] = ButGroup3->LocalPosition.Y;
        }
        ModeLeftPosition = GetByName(u"ModeLeftPanel"sv)->LocalPosition;
        ModeRightPosition = GetByName(u"ModeRightPanel"sv)->LocalPosition;
        {
            GI_MessageLoop::TObjectGI* ModeLeftButtonN = GetByName(u"ModeLeftButtonN"sv);
            ModeLeftButtonN->MouseEnterCallback = pas::bind_method<&TfCfgSettings::ModeMouseEnter>(this);
            ModeLeftButtonN->MouseLeaveCallback = pas::bind_method<&TfCfgSettings::ModeMouseLeave>(this);
            ModeLeftButtonN->LeftButtonDownCallback = pas::bind_method<&TfCfgSettings::ModeMouseDown>(this);
            ModeLeftButtonN->LeftButtonUpCallback = pas::bind_method<&TfCfgSettings::ModeMouseUp>(this);
            ModeLeftButtonN->UserValue = 0;
        }
        {
            GI_MessageLoop::TObjectGI* ModeLeftButtonD = GetByName(u"ModeLeftButtonD"sv);
            ModeLeftButtonD->MouseEnterCallback = pas::bind_method<&TfCfgSettings::ModeMouseEnter>(this);
            ModeLeftButtonD->MouseLeaveCallback = pas::bind_method<&TfCfgSettings::ModeMouseLeave>(this);
            ModeLeftButtonD->LeftButtonDownCallback = pas::bind_method<&TfCfgSettings::ModeMouseDown>(this);
            ModeLeftButtonD->LeftButtonUpCallback = pas::bind_method<&TfCfgSettings::ModeMouseUp>(this);
            ModeLeftButtonD->UserValue = 0;
        }
        {
            GI_MessageLoop::TObjectGI* ModeRightButtonN = GetByName(u"ModeRightButtonN"sv);
            ModeRightButtonN->MouseEnterCallback = pas::bind_method<&TfCfgSettings::ModeMouseEnter>(this);
            ModeRightButtonN->MouseLeaveCallback = pas::bind_method<&TfCfgSettings::ModeMouseLeave>(this);
            ModeRightButtonN->LeftButtonDownCallback = pas::bind_method<&TfCfgSettings::ModeMouseDown>(this);
            ModeRightButtonN->LeftButtonUpCallback = pas::bind_method<&TfCfgSettings::ModeMouseUp>(this);
            ModeRightButtonN->UserValue = 1;
        }
        {
            GI_MessageLoop::TObjectGI* ModeRightButtonD = GetByName(u"ModeRightButtonD"sv);
            ModeRightButtonD->MouseEnterCallback = pas::bind_method<&TfCfgSettings::ModeMouseEnter>(this);
            ModeRightButtonD->MouseLeaveCallback = pas::bind_method<&TfCfgSettings::ModeMouseLeave>(this);
            ModeRightButtonD->LeftButtonDownCallback = pas::bind_method<&TfCfgSettings::ModeMouseDown>(this);
            ModeRightButtonD->LeftButtonUpCallback = pas::bind_method<&TfCfgSettings::ModeMouseUp>(this);
            ModeRightButtonD->UserValue = 1;
        }
        SettingsModeColorNormal = GR_Main::GetStyleColorGI(u"Settings.ModeColorNormal"_w, 0, 44, 70);
        SettingsModeColorHighlighted = GR_Main::GetStyleColorGI(u"Settings.ModeColorHighlighted"_w, 0, 255, 255);
    }

    void TfCfgSettings::OnOpen() {
        pas::WideString cpp_text{};
        std::int32_t I{};
        std::int32_t LanguageCount{};
        pas::WideString Language{};
        pas::WideString LanguageName{};
        EC_BlockPar::TBlockParEC* Block{};
        if (GR_Main::IsInstallFeatureEnabled(u"Robot"_wref.get())) {
            RobotAvailability = 1;
            if (Robot::RobotInterface != nullptr) {
                RobotAvailability = Robot::RobotInterface->Support();
            }
        } else {
            RobotAvailability = 4;
        }
        SettingsMode = 0;
        ModeButtonState = 0;
        GetByName(u"LabelHelp"sv)->SetActive(false);
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"sv));
        Panel->FreeOwnedChildren();
        for (I = 0; I <= 5; ++I) {
            GroupNextY[I] = 0;
            GroupPanels[I] = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
            {
                GI_Panel::TPanelGI* cpp_with = GroupPanels[I];
                cpp_with->SetSize(ClassesImports::Point(Panel->ClientSize.X, 0));
                cpp_with->SetPosition(ClassesImports::Point(0, 0));
                cpp_with->SetDepth(-1.0E+2);
                cpp_with->SetPositionModeW(true);
            }
        }
        BuildGroupIndex = 0;
        HasInstalledPackages = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(GR_Main::ModLanguageInstallConfigs) - 1); cpp_range.next(I); ) {
            Block = pas::list_at<EC_BlockPar::TBlockParEC>(GR_Main::ModLanguageInstallConfigs, I);
            Block = Block->GetBlock(u"Packages"sv);
            if (Block->GetParamCount() > 0) {
                HasInstalledPackages = true;
                break;
            }
        }
        if (static_cast<std::uint8_t>(SimpleSteamApi::SteamInitialized ^ 1) && GR_Main::RequestedLanguage == u"") {
            AddOptionLabel(u"Lang"_w, aConst::LocalizedText(u"FormCfgSettings.Lang"_wref.get()), true);
            if (GR_Main::AvailableLanguageCodes != u"" && EC_Str::CountDelimitedPartsW(pas::view(GR_Main::AvailableLanguageCodes), u","sv) > 0) {
                LanguageCount = EC_Str::CountDelimitedPartsW(pas::view(GR_Main::AvailableLanguageCodes), u","sv);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, LanguageCount - 1); cpp_range_2.next(I); ) {
                    Language = EC_Str::ExtractDelimitedPartW(pas::view(GR_Main::AvailableLanguageCodes), I, u","sv);
                    if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"install_", Language, u".txt"})))) {
                        Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                        Block->LoadFromTextFileWithEncodingProbe((cpp_text = pas::concat_wide({u"install_", Language, u".txt"}), cpp_text.pchar()), false);
                        LanguageName = Block->GetParam(u"LangName"sv);
                        pas::free(Block);
                    }
                    {
                        std::uint8_t cpp_arg = ([&] {
                            pas::WideString cpp_string = EC_Str::UpperCaseWideString(Language);
                            pas::WideString cpp_string_2 = EC_Str::UpperCaseWideString(GR_Main::SelectedLanguage);
                            return cpp_string == cpp_string_2;
                        }());
                        std::uint8_t cpp_arg_2 = aGalaxy::Galaxy != nullptr && HasInstalledPackages;
                        AddOptionChoice(I, LanguageName, cpp_arg, cpp_arg_2);
                    }
                }
            } else {
                Language = GR_Main::SelectedLanguage;
                AddOptionChoice(0, EC_Str::ExtractDelimitedPartW(pas::view(Language), 0, u","sv), true, false);
            }
        }
        AddOptionLabel(u"MultiThread"_w, aConst::LocalizedText(u"FormCfgSettings.MultiThread"_wref.get()), true);
        {
            std::uint8_t multiThreadEnabled = GlobalsV::MultiThreadEnabled;
            pas::WideString localizedText = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText), multiThreadEnabled, false);
        }
        {
            std::uint8_t cpp_arg_3 = static_cast<std::uint8_t>(GlobalsV::MultiThreadEnabled ^ 1);
            pas::WideString localizedText_2 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_2), cpp_arg_3, false);
        }
        AddOptionLabel(u"DefaultOrder"_w, aConst::LocalizedText(u"FormCfgSettings.DefaultOrder"_wref.get()), false);
        {
            std::uint8_t cpp_arg_4 = GlobalsV::DefaultOrder == 0;
            pas::WideString localizedText_3 = aConst::LocalizedText(u"FormCfgSettings.DefaultOrderAuto"_wref.get());
            AddOptionChoice(0, std::move(localizedText_3), cpp_arg_4, false);
        }
        {
            std::uint8_t cpp_arg_5 = GlobalsV::DefaultOrder == 2;
            pas::WideString localizedText_4 = aConst::LocalizedText(u"FormCfgSettings.DefaultOrderNear"_wref.get());
            AddOptionChoice(2, std::move(localizedText_4), cpp_arg_5, false);
        }
        {
            std::uint8_t cpp_arg_6 = GlobalsV::DefaultOrder == 3;
            pas::WideString localizedText_5 = aConst::LocalizedText(u"FormCfgSettings.DefaultOrderFar"_wref.get());
            AddOptionChoice(3, std::move(localizedText_5), cpp_arg_6, false);
        }
        AddOptionLabel(u"RightClickOnShip"_w, aConst::LocalizedText(u"FormCfgSettings.RightClickOnShip"_wref.get()), false);
        {
            std::uint8_t cpp_arg_7 = GlobalsV::RightClickOnShip == 0;
            pas::WideString localizedText_6 = aConst::LocalizedText(u"FormCfgSettings.RightClickOnShipScaner"_wref.get());
            AddOptionChoice(0, std::move(localizedText_6), cpp_arg_7, false);
        }
        {
            std::uint8_t cpp_arg_8 = GlobalsV::RightClickOnShip == 1;
            pas::WideString localizedText_7 = aConst::LocalizedText(u"FormCfgSettings.RightClickOnShipTalk"_wref.get());
            AddOptionChoice(1, std::move(localizedText_7), cpp_arg_8, false);
        }
        {
            std::uint8_t cpp_arg_9 = GlobalsV::RightClickOnShip == 2;
            pas::WideString localizedText_8 = aConst::LocalizedText(u"FormCfgSettings.RightClickOnShipChangeOrder"_wref.get());
            AddOptionChoice(2, std::move(localizedText_8), cpp_arg_9, false);
        }
        AddOptionLabel(u"FilmSpeed"_w, aConst::LocalizedText(u"FormCfgSettings.FilmSpeed"_wref.get()), false);
        {
            std::uint8_t cpp_arg_10 = GlobalsV::FilmSpeed == 0;
            pas::WideString localizedText_9 = aConst::LocalizedText(u"FormCfgSettings.FilmSpeed0"_wref.get());
            AddOptionChoice(0, std::move(localizedText_9), cpp_arg_10, false);
        }
        {
            std::uint8_t cpp_arg_11 = GlobalsV::FilmSpeed == 1;
            pas::WideString localizedText_10 = aConst::LocalizedText(u"FormCfgSettings.FilmSpeed1"_wref.get());
            AddOptionChoice(1, std::move(localizedText_10), cpp_arg_11, false);
        }
        {
            std::uint8_t cpp_arg_12 = GlobalsV::FilmSpeed == 2;
            pas::WideString localizedText_11 = aConst::LocalizedText(u"FormCfgSettings.FilmSpeed2"_wref.get());
            AddOptionChoice(2, std::move(localizedText_11), cpp_arg_12, false);
        }
        {
            std::uint8_t cpp_arg_13 = GlobalsV::FilmSpeed == 3;
            pas::WideString localizedText_12 = aConst::LocalizedText(u"FormCfgSettings.FilmSpeed3"_wref.get());
            AddOptionChoice(3, std::move(localizedText_12), cpp_arg_13, false);
        }
        AddOptionLabel(u"ViewFollowShip"_w, aConst::LocalizedText(u"FormCfgSettings.ViewFollowShip"_wref.get()), false);
        {
            std::uint8_t viewFollowShip = GlobalsV::ViewFollowShip;
            pas::WideString localizedText_13 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_13), viewFollowShip, false);
        }
        {
            std::uint8_t cpp_arg_14 = static_cast<std::uint8_t>(GlobalsV::ViewFollowShip ^ 1);
            pas::WideString localizedText_14 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_14), cpp_arg_14, false);
        }
        AddOptionLabel(u"ViewPathLength"_w, aConst::LocalizedText(u"FormCfgSettings.ViewPathLength"_wref.get()), false);
        {
            std::uint8_t viewPathLength = GlobalsV::ViewPathLength;
            pas::WideString localizedText_15 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_15), viewPathLength, false);
        }
        {
            std::uint8_t cpp_arg_15 = static_cast<std::uint8_t>(GlobalsV::ViewPathLength ^ 1);
            pas::WideString localizedText_16 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_16), cpp_arg_15, false);
        }
        AddOptionLabel(u"ActionDoubleClick"_w, aConst::LocalizedText(u"FormCfgSettings.ActionDoubleClick"_wref.get()), false);
        {
            std::uint8_t actionDoubleClick = GlobalsV::ActionDoubleClick;
            pas::WideString localizedText_17 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_17), actionDoubleClick, false);
        }
        {
            std::uint8_t cpp_arg_16 = static_cast<std::uint8_t>(GlobalsV::ActionDoubleClick ^ 1);
            pas::WideString localizedText_18 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_18), cpp_arg_16, false);
        }
        AddOptionLabel(u"ClickAutoCloseForm"_w, aConst::LocalizedText(u"FormCfgSettings.ClickAutoCloseForm"_wref.get()), false);
        {
            std::uint8_t clickAutoCloseForm = GlobalsV::ClickAutoCloseForm;
            pas::WideString localizedText_19 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_19), clickAutoCloseForm, false);
        }
        {
            std::uint8_t cpp_arg_17 = static_cast<std::uint8_t>(GlobalsV::ClickAutoCloseForm ^ 1);
            pas::WideString localizedText_20 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_20), cpp_arg_17, false);
        }
        GI_Label::TLabelGI* ValueLabel = AddOptionLabel(u"ForsageDeactivatePercent"_w, aConst::LocalizedText(u"FormCfgSettings.ForsageTurnOff"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, GlobalsV::AfterburnerStopCondition, 1, pas::bind_static_method<&TfCfgSettings::FormatForsageDeactivatePercent>(this));
        ValueLabel = AddOptionLabel(u"MaxSearchResult"_w, aConst::LocalizedText(u"FormCfgSettings.MaxSearchResult"_wref.get()), false);
        AddOptionSlider(ValueLabel, 1, 100, GlobalsV::MaxSearchResult, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        ValueLabel = AddOptionLabel(u"MaxPlayerNews"_w, aConst::LocalizedText(u"FormCfgSettings.MaxPlayerNews"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, GlobalsV::MaxPlayerNews, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        ValueLabel = AddOptionLabel(u"TurnSaveStep"_w, aConst::LocalizedText(u"FormCfgSettings.TurnSaveStep"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, aGalaxyStruct::TurnsPerYear, GlobalsV::TurnSaveStep, 1, pas::bind_static_method<&TfCfgSettings::FormatTurnSaveStep>(this));
        ValueLabel = AddOptionLabel(u"QuickSaveExtraSlots"_w, aConst::LocalizedText(u"FormCfgSettings.QuickSaveSlots"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 9, GlobalsV::QuickSaveExtraSlots, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        ValueLabel = AddOptionLabel(u"CountFilmSave"_w, aConst::LocalizedText(u"FormCfgSettings.CountFilmSave"_wref.get()), false);
        AddOptionSlider(ValueLabel, 1, 100, GlobalsV::FilmHistoryLimit, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        ValueLabel = AddOptionLabel(u"ScrollSpeed"_w, aConst::LocalizedText(u"FormCfgSettings.ScrollSpeed"_wref.get()), false);
        AddOptionSlider(ValueLabel, 1, 80, GlobalsV::ScrollStep, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        ValueLabel = AddOptionLabel(u"BeginCalcNextTurn"_w, aConst::LocalizedText(u"FormCfgSettings.BeginCalcNextTurn"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GlobalsV::BeginCalcNextTurn * 1.0E+2L), 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        ValueLabel = AddOptionLabel(u"ChangeAutoPilot"_w, aConst::LocalizedText(u"FormCfgSettings.ChangeAutoPilot"_wref.get()), false);
        AddOptionSlider(ValueLabel, 2, 20, GlobalsV::ChangeAutoPilot, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        AddOptionLabel(u"DisableAutoPilot"_w, aConst::LocalizedText(u"FormCfgSettings.DisableAutoPilot"_wref.get()), false);
        {
            std::uint8_t disableAutoPilot = GlobalsV::DisableAutoPilot;
            pas::WideString localizedText_21 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_21), disableAutoPilot, false);
        }
        {
            std::uint8_t cpp_arg_18 = static_cast<std::uint8_t>(GlobalsV::DisableAutoPilot ^ 1);
            pas::WideString localizedText_22 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_22), cpp_arg_18, false);
        }
        BuildGroupIndex = 1;
        if (GR_Main::AltResolutionSwitch) {
            AddOptionLabel(u"Resolution"_w, aConst::LocalizedText(u"FormCfgSettings.Resolution"_wref.get()), true);
            I = GR_Main::GameDisplayModeCount - 1;
            while (I >= 0) {
                {
                    GR_Main::TDisplayModeGR& cpp_with_2 = GR_Main::GameDisplayModes[I];
                    if (cpp_with_2.Width == 0) {
                        std::uint8_t cpp_arg_19 = GR_Main::SelectedGameDisplayMode == I;
                        pas::WideString localizedText_23 = aConst::LocalizedText(u"FormCfgSettings.HelpButAuto"_wref.get());
                        AddOptionChoice(I, std::move(localizedText_23), cpp_arg_19, false);
                    } else {
                        AddOptionChoice(I, static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(cpp_with_2.Width), "x", SysUtils::Int64ToStr(cpp_with_2.Height)})), GR_Main::SelectedGameDisplayMode == I, false);
                    }
                }
                --I;
            }
        } else {
            ValueLabel = AddOptionLabel(u"Resolution"_w, aConst::LocalizedText(u"FormCfgSettings.Resolution"_wref.get()), false);
            AddOptionSlider(ValueLabel, 0, GR_Main::GameDisplayModeCount - 1, GR_Main::SelectedGameDisplayMode, 1, pas::bind_static_method<&TfCfgSettings::FormatResolution>(this));
        }
        AddOptionLabel(u"VSync"_w, aConst::LocalizedText(u"FormCfgSettings.VSync"_wref.get()), false);
        {
            std::uint8_t vSyncEnabled = GR_Main::VSyncEnabled;
            pas::WideString localizedText_24 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_24), vSyncEnabled, false);
        }
        {
            std::uint8_t cpp_arg_20 = static_cast<std::uint8_t>(GR_Main::VSyncEnabled ^ 1);
            pas::WideString localizedText_25 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_25), cpp_arg_20, false);
        }
        AddOptionLabel(u"Window"_w, aConst::LocalizedText(u"FormCfgSettings.Window"_wref.get()), false);
        {
            std::uint8_t windowedModeRequested = GR_Main::WindowedModeRequested;
            pas::WideString localizedText_26 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_26), windowedModeRequested, false);
        }
        {
            std::uint8_t cpp_arg_21 = static_cast<std::uint8_t>(GR_Main::WindowedModeRequested ^ 1);
            pas::WideString localizedText_27 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_27), cpp_arg_21, false);
        }
        if (GR_Main::AlternateViewportEnabled) {
            AddOptionLabel(u"RenderMode"_w, aConst::LocalizedText(u"FormCfgSettings.RenderMode"_wref.get()), false);
            {
                std::uint8_t scaleViewportToWindow = GlobalsV::ScaleViewportToWindow;
                pas::WideString localizedText_28 = aConst::LocalizedText(u"FormCfgSettings.RenderMode1"_wref.get());
                AddOptionChoice(1, std::move(localizedText_28), scaleViewportToWindow, false);
            }
            {
                std::uint8_t cpp_arg_22 = static_cast<std::uint8_t>(GlobalsV::ScaleViewportToWindow ^ 1);
                pas::WideString localizedText_29 = aConst::LocalizedText(u"FormCfgSettings.RenderMode2"_wref.get());
                AddOptionChoice(0, std::move(localizedText_29), cpp_arg_22, false);
            }
        }
        if (!GR_Main::AlternateViewportEnabled) {
            AddOptionLabel(u"HardwareRender"_w, aConst::LocalizedText(u"FormCfgSettings.HardwareRender"_wref.get()), false);
            {
                std::uint8_t hardwareRenderingRequested = GlobalsV::HardwareRenderingRequested;
                pas::WideString localizedText_30 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
                AddOptionChoice(1, std::move(localizedText_30), hardwareRenderingRequested, false);
            }
            {
                std::uint8_t cpp_arg_23 = static_cast<std::uint8_t>(GlobalsV::HardwareRenderingRequested ^ 1);
                pas::WideString localizedText_31 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
                AddOptionChoice(0, std::move(localizedText_31), cpp_arg_23, false);
            }
        }
        AddOptionLabel(u"UseTablesForGov"_w, aConst::LocalizedText(u"FormCfgSettings.UseTablesForGov"_wref.get()), true);
        {
            std::uint8_t useTablesForGov = GlobalsV::UseTablesForGov;
            pas::WideString localizedText_32 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_32), useTablesForGov, false);
        }
        {
            std::uint8_t cpp_arg_24 = static_cast<std::uint8_t>(GlobalsV::UseTablesForGov ^ 1);
            pas::WideString localizedText_33 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_33), cpp_arg_24, false);
        }
        AddOptionLabel(u"FontSmooth"_w, aConst::LocalizedText(u"FormCfgSettings.FontSmooth"_wref.get()), true);
        {
            std::uint8_t fontSmoothingEnabled = GlobalsV::FontSmoothingEnabled;
            pas::WideString localizedText_34 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_34), fontSmoothingEnabled, false);
        }
        {
            std::uint8_t cpp_arg_25 = static_cast<std::uint8_t>(GlobalsV::FontSmoothingEnabled ^ 1);
            pas::WideString localizedText_35 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_35), cpp_arg_25, false);
        }
        AddOptionLabel(u"FontDialog"_w, aConst::LocalizedText(u"FormCfgSettings.FontDialog"_wref.get()), true);
        {
            std::uint8_t cpp_arg_26 = GlobalsV::FontDialog == 0;
            pas::WideString localizedText_36 = aConst::LocalizedText(u"FormCfgSettings.FontGalaxySmall"_wref.get());
            AddOptionChoice(0, std::move(localizedText_36), cpp_arg_26, false);
        }
        {
            std::uint8_t cpp_arg_27 = GlobalsV::FontDialog == 1;
            pas::WideString localizedText_37 = aConst::LocalizedText(u"FormCfgSettings.FontSizeMid"_wref.get());
            AddOptionChoice(1, std::move(localizedText_37), cpp_arg_27, false);
        }
        {
            std::uint8_t cpp_arg_28 = GlobalsV::FontDialog == 2;
            pas::WideString localizedText_38 = aConst::LocalizedText(u"FormCfgSettings.FontSizeBig"_wref.get());
            AddOptionChoice(2, std::move(localizedText_38), cpp_arg_28, false);
        }
        {
            std::uint8_t cpp_arg_29 = GlobalsV::FontDialog >= 3;
            pas::WideString localizedText_39 = aConst::LocalizedText(u"FormCfgSettings.FontSizeLarge"_wref.get());
            AddOptionChoice(3, std::move(localizedText_39), cpp_arg_29, false);
        }
        AddOptionLabel(u"FontQuest"_w, aConst::LocalizedText(u"FormCfgSettings.FontQuest"_wref.get()), true);
        {
            std::uint8_t cpp_arg_30 = GlobalsV::FontQuest == 0;
            pas::WideString localizedText_40 = aConst::LocalizedText(u"FormCfgSettings.FontGalaxySmall"_wref.get());
            AddOptionChoice(0, std::move(localizedText_40), cpp_arg_30, false);
        }
        {
            std::uint8_t cpp_arg_31 = GlobalsV::FontQuest == 1;
            pas::WideString localizedText_41 = aConst::LocalizedText(u"FormCfgSettings.FontSizeMid"_wref.get());
            AddOptionChoice(1, std::move(localizedText_41), cpp_arg_31, false);
        }
        {
            std::uint8_t cpp_arg_32 = GlobalsV::FontQuest == 2;
            pas::WideString localizedText_42 = aConst::LocalizedText(u"FormCfgSettings.FontSizeBig"_wref.get());
            AddOptionChoice(2, std::move(localizedText_42), cpp_arg_32, false);
        }
        {
            std::uint8_t cpp_arg_33 = GlobalsV::FontQuest >= 3;
            pas::WideString localizedText_43 = aConst::LocalizedText(u"FormCfgSettings.FontSizeLarge"_wref.get());
            AddOptionChoice(3, std::move(localizedText_43), cpp_arg_33, false);
        }
        ValueLabel = AddOptionLabel(u"Brightness"_w, aConst::LocalizedText(u"FormCfgSettings.Brightness"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GR_Main::DisplayBrightness * 5.0E+1L + 5.0E+1L), 1, pas::bind_method<&TfCfgSettings::PreviewBrightness>(this));
        ValueLabel = AddOptionLabel(u"Contrast"_w, aConst::LocalizedText(u"FormCfgSettings.Contrast"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GR_Main::DisplayContrast * 5.0E+1L + 5.0E+1L), 1, pas::bind_method<&TfCfgSettings::PreviewContrast>(this));
        AddOptionLabel(u"Intro"_w, aConst::LocalizedText(u"FormCfgSettings.Intro"_wref.get()), false);
        {
            std::uint8_t cpp_arg_34 = static_cast<std::uint8_t>(Globals::SkipIntro ^ 1);
            std::uint8_t cpp_arg_35 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"Video"_wref.get()) ^ 1);
            pas::WideString localizedText_44 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_44), cpp_arg_34, cpp_arg_35);
        }
        {
            std::uint8_t skipIntro = Globals::SkipIntro;
            pas::WideString localizedText_45 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_45), skipIntro, false);
        }
        AddOptionLabel(u"Video"_w, aConst::LocalizedText(u"FormCfgSettings.Video"_wref.get()), false);
        {
            std::uint8_t cpp_arg_36 = static_cast<std::uint8_t>(Globals::SkipVideo ^ 1);
            std::uint8_t cpp_arg_37 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"Video"_wref.get()) ^ 1);
            pas::WideString localizedText_46 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_46), cpp_arg_36, cpp_arg_37);
        }
        {
            std::uint8_t skipVideo = Globals::SkipVideo;
            pas::WideString localizedText_47 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_47), skipVideo, false);
        }
        AddOptionLabel(u"SoftwareCursor"_w, aConst::LocalizedText(u"FormCfgSettings.SoftwareCursor"_wref.get()), false);
        {
            std::uint8_t showSystemMouse = GR_Main::ShowSystemMouse;
            pas::WideString localizedText_48 = aConst::LocalizedText(u"FormCfgSettings.SoftwareCursorHardware"_wref.get());
            AddOptionChoice(1, std::move(localizedText_48), showSystemMouse, false);
        }
        {
            std::uint8_t cpp_arg_38 = static_cast<std::uint8_t>(GR_Main::ShowSystemMouse ^ 1);
            pas::WideString localizedText_49 = aConst::LocalizedText(u"FormCfgSettings.SoftwareCursorSoftware"_wref.get());
            AddOptionChoice(0, std::move(localizedText_49), cpp_arg_38, false);
        }
        AddOptionLabel(u"BGImage"_w, aConst::LocalizedText(u"FormCfgSettings.BGImage"_wref.get()), false);
        {
            std::uint8_t bgImage = GlobalsV::BGImage;
            std::uint8_t cpp_arg_39 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"BGImage"_wref.get()) ^ 1);
            pas::WideString localizedText_50 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_50), bgImage, cpp_arg_39);
        }
        {
            std::uint8_t cpp_arg_40 = static_cast<std::uint8_t>(GlobalsV::BGImage ^ 1);
            pas::WideString localizedText_51 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_51), cpp_arg_40, false);
        }
        AddOptionLabel(u"AnimCaptain"_w, aConst::LocalizedText(u"FormCfgSettings.AnimCaptain"_wref.get()), false);
        {
            std::uint8_t animCaptain = GlobalsV::AnimCaptain;
            std::uint8_t cpp_arg_41 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"AnimCaptain"_wref.get()) ^ 1);
            pas::WideString localizedText_52 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_52), animCaptain, cpp_arg_41);
        }
        {
            std::uint8_t cpp_arg_42 = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            pas::WideString localizedText_53 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_53), cpp_arg_42, false);
        }
        AddOptionLabel(u"AnimShip"_w, aConst::LocalizedText(u"FormCfgSettings.AnimShip"_wref.get()), false);
        {
            std::uint8_t animShipFull = GlobalsV::AnimShipFull;
            std::uint8_t cpp_arg_43 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"AnimShipFull"_wref.get()) ^ 1);
            pas::WideString localizedText_54 = aConst::LocalizedText(u"FormCfgSettings.AnimShipFull"_wref.get());
            AddOptionChoice(1, std::move(localizedText_54), animShipFull, cpp_arg_43);
        }
        {
            std::uint8_t cpp_arg_44 = static_cast<std::uint8_t>(GlobalsV::AnimShipFull ^ 1);
            std::uint8_t cpp_arg_45 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"AnimShipSmall"_wref.get()) ^ 1);
            pas::WideString localizedText_55 = aConst::LocalizedText(u"FormCfgSettings.AnimShipSmall"_wref.get());
            AddOptionChoice(0, std::move(localizedText_55), cpp_arg_44, cpp_arg_45);
        }
        AddOptionLabel(u"AnimItem"_w, aConst::LocalizedText(u"FormCfgSettings.AnimItem"_wref.get()), false);
        {
            std::uint8_t animItem = GlobalsV::AnimItem;
            std::uint8_t cpp_arg_46 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"AnimItem"_wref.get()) ^ 1);
            pas::WideString localizedText_56 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_56), animItem, cpp_arg_46);
        }
        {
            std::uint8_t cpp_arg_47 = static_cast<std::uint8_t>(GlobalsV::AnimItem ^ 1);
            pas::WideString localizedText_57 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_57), cpp_arg_47, false);
        }
        AddOptionLabel(u"AnimMenuShip"_w, aConst::LocalizedText(u"FormCfgSettings.AnimMenuShip"_wref.get()), false);
        {
            std::uint8_t animMenuShip = GlobalsV::AnimMenuShip;
            pas::WideString localizedText_58 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_58), animMenuShip, false);
        }
        {
            std::uint8_t cpp_arg_48 = static_cast<std::uint8_t>(GlobalsV::AnimMenuShip ^ 1);
            pas::WideString localizedText_59 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_59), cpp_arg_48, false);
        }
        AddOptionLabel(u"AnimGov"_w, aConst::LocalizedText(u"FormCfgSettings.AnimGov"_wref.get()), false);
        {
            std::uint8_t cpp_arg_49 = GlobalsV::AnimGov == 2;
            std::uint8_t cpp_arg_50 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"AnimGov"_wref.get()) ^ 1);
            pas::WideString localizedText_60 = aConst::LocalizedText(u"FormCfgSettings.AnimGovFull"_wref.get());
            AddOptionChoice(2, std::move(localizedText_60), cpp_arg_49, cpp_arg_50);
        }
        {
            std::uint8_t cpp_arg_51 = GlobalsV::AnimGov == 1;
            std::uint8_t cpp_arg_52 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"AnimGov"_wref.get()) ^ 1);
            pas::WideString localizedText_61 = aConst::LocalizedText(u"FormCfgSettings.AnimGovHalf"_wref.get());
            AddOptionChoice(1, std::move(localizedText_61), cpp_arg_51, cpp_arg_52);
        }
        {
            std::uint8_t cpp_arg_53 = GlobalsV::AnimGov == 0;
            pas::WideString localizedText_62 = aConst::LocalizedText(u"FormCfgSettings.AnimGovOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_62), cpp_arg_53, false);
        }
        AddOptionLabel(u"AnimHangar"_w, aConst::LocalizedText(u"FormCfgSettings.AnimHangar"_wref.get()), false);
        {
            std::uint8_t animHangar = GlobalsV::AnimHangar;
            pas::WideString localizedText_63 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_63), animHangar, false);
        }
        {
            std::uint8_t cpp_arg_54 = static_cast<std::uint8_t>(GlobalsV::AnimHangar ^ 1);
            pas::WideString localizedText_64 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_64), cpp_arg_54, false);
        }
        AddOptionLabel(u"AnimStar"_w, aConst::LocalizedText(u"FormCfgSettings.AnimStar"_wref.get()), false);
        {
            std::uint8_t animStar = GlobalsV::AnimStar;
            std::uint8_t cpp_arg_55 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"AnimStar"_wref.get()) ^ 1);
            pas::WideString localizedText_65 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_65), animStar, cpp_arg_55);
        }
        {
            std::uint8_t cpp_arg_56 = static_cast<std::uint8_t>(GlobalsV::AnimStar ^ 1);
            pas::WideString localizedText_66 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_66), cpp_arg_56, false);
        }
        AddOptionLabel(u"SpaceImage"_w, aConst::LocalizedText(u"FormCfgSettings.SpaceImage"_wref.get()), false);
        {
            std::uint8_t cpp_arg_57 = GlobalsV::SpaceImage >= 2;
            pas::WideString localizedText_67 = aConst::LocalizedText(u"FormCfgSettings.CometLarge"_wref.get());
            AddOptionChoice(2, std::move(localizedText_67), cpp_arg_57, false);
        }
        {
            std::uint8_t cpp_arg_58 = GlobalsV::SpaceImage == 1;
            pas::WideString localizedText_68 = aConst::LocalizedText(u"FormCfgSettings.CometSmall"_wref.get());
            AddOptionChoice(1, std::move(localizedText_68), cpp_arg_58, false);
        }
        {
            std::uint8_t cpp_arg_59 = GlobalsV::SpaceImage == 0;
            pas::WideString localizedText_69 = aConst::LocalizedText(u"FormCfgSettings.CometOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_69), cpp_arg_59, false);
        }
        AddOptionLabel(u"SputnikShow"_w, aConst::LocalizedText(u"FormCfgSettings.SputnikShow"_wref.get()), false);
        {
            std::uint8_t sputnikShow = GlobalsV::SputnikShow;
            std::uint8_t cpp_arg_60 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"SputnikShow"_wref.get()) ^ 1);
            pas::WideString localizedText_70 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_70), sputnikShow, cpp_arg_60);
        }
        {
            std::uint8_t cpp_arg_61 = static_cast<std::uint8_t>(GlobalsV::SputnikShow ^ 1);
            pas::WideString localizedText_71 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_71), cpp_arg_61, false);
        }
        AddOptionLabel(u"CircleAction"_w, aConst::LocalizedText(u"FormCfgSettings.CircleAction"_wref.get()), false);
        {
            std::uint8_t cpp_arg_62 = static_cast<std::uint8_t>(GlobalsV::CircleAction ^ 1);
            pas::WideString localizedText_72 = aConst::LocalizedText(u"FormCfgSettings.CircleActionNormal"_wref.get());
            AddOptionChoice(0, std::move(localizedText_72), cpp_arg_62, false);
        }
        {
            std::uint8_t circleAction = GlobalsV::CircleAction;
            pas::WideString localizedText_73 = aConst::LocalizedText(u"FormCfgSettings.CircleActionHide"_wref.get());
            AddOptionChoice(1, std::move(localizedText_73), circleAction, false);
        }
        AddOptionLabel(u"Tail"_w, aConst::LocalizedText(u"FormCfgSettings.Tails"_wref.get()), false);
        {
            std::uint8_t cpp_arg_63 = GlobalsV::ShipTail == 2;
            pas::WideString localizedText_74 = aConst::LocalizedText(u"FormCfgSettings.TailAll"_wref.get());
            AddOptionChoice(2, std::move(localizedText_74), cpp_arg_63, false);
        }
        {
            std::uint8_t cpp_arg_64 = GlobalsV::ShipTail == 1;
            pas::WideString localizedText_75 = aConst::LocalizedText(u"FormCfgSettings.TailPlayer"_wref.get());
            AddOptionChoice(1, std::move(localizedText_75), cpp_arg_64, false);
        }
        {
            std::uint8_t cpp_arg_65 = GlobalsV::ShipTail == 0;
            pas::WideString localizedText_76 = aConst::LocalizedText(u"FormCfgSettings.TailOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_76), cpp_arg_65, false);
        }
        AddOptionLabel(u"Comet"_w, aConst::LocalizedText(u"FormCfgSettings.Comet"_wref.get()), false);
        {
            std::uint8_t cpp_arg_66 = GlobalsV::Comet >= 2;
            pas::WideString localizedText_77 = aConst::LocalizedText(u"FormCfgSettings.CometLarge"_wref.get());
            AddOptionChoice(2, std::move(localizedText_77), cpp_arg_66, false);
        }
        {
            std::uint8_t cpp_arg_67 = GlobalsV::Comet == 1;
            pas::WideString localizedText_78 = aConst::LocalizedText(u"FormCfgSettings.CometSmall"_wref.get());
            AddOptionChoice(1, std::move(localizedText_78), cpp_arg_67, false);
        }
        {
            std::uint8_t cpp_arg_68 = GlobalsV::Comet == 0;
            pas::WideString localizedText_79 = aConst::LocalizedText(u"FormCfgSettings.CometOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_79), cpp_arg_68, false);
        }
        AddOptionLabel(u"Wind"_w, aConst::LocalizedText(u"FormCfgSettings.Wind"_wref.get()), false);
        {
            std::uint8_t cpp_arg_69 = GlobalsV::Wind >= 2;
            pas::WideString localizedText_80 = aConst::LocalizedText(u"FormCfgSettings.WindFull"_wref.get());
            AddOptionChoice(2, std::move(localizedText_80), cpp_arg_69, false);
        }
        {
            std::uint8_t cpp_arg_70 = GlobalsV::Wind == 1;
            pas::WideString localizedText_81 = aConst::LocalizedText(u"FormCfgSettings.WindSmall"_wref.get());
            AddOptionChoice(1, std::move(localizedText_81), cpp_arg_70, false);
        }
        {
            std::uint8_t cpp_arg_71 = GlobalsV::Wind == 0;
            pas::WideString localizedText_82 = aConst::LocalizedText(u"FormCfgSettings.WindOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_82), cpp_arg_71, false);
        }
        AddOptionLabel(u"PlanetClouds"_w, aConst::LocalizedText(u"FormCfgSettings.PlanetClouds"_wref.get()), false);
        {
            std::uint8_t planetClouds = GlobalsV::PlanetClouds;
            pas::WideString localizedText_83 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_83), planetClouds, false);
        }
        {
            std::uint8_t cpp_arg_72 = static_cast<std::uint8_t>(GlobalsV::PlanetClouds ^ 1);
            pas::WideString localizedText_84 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_84), cpp_arg_72, false);
        }
        AddOptionLabel(u"PlanetAtm"_w, aConst::LocalizedText(u"FormCfgSettings.PlanetAtm"_wref.get()), false);
        {
            std::uint8_t planetAtm = GlobalsV::PlanetAtm;
            pas::WideString localizedText_85 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_85), planetAtm, false);
        }
        {
            std::uint8_t cpp_arg_73 = static_cast<std::uint8_t>(GlobalsV::PlanetAtm ^ 1);
            pas::WideString localizedText_86 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_86), cpp_arg_73, false);
        }
        AddOptionLabel(u"AnimChangeForm"_w, aConst::LocalizedText(u"FormCfgSettings.AnimChangeForm"_wref.get()), false);
        {
            std::uint8_t animChangeForm = GlobalsV::AnimChangeForm;
            pas::WideString localizedText_87 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_87), animChangeForm, false);
        }
        {
            std::uint8_t cpp_arg_74 = static_cast<std::uint8_t>(GlobalsV::AnimChangeForm ^ 1);
            pas::WideString localizedText_88 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_88), cpp_arg_74, false);
        }
        AddOptionLabel(u"AnimMainFon"_w, aConst::LocalizedText(u"FormCfgSettings.AnimMainFon"_wref.get()), false);
        {
            std::uint8_t animMainFon = GlobalsV::AnimMainFon;
            pas::WideString localizedText_89 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_89), animMainFon, false);
        }
        {
            std::uint8_t cpp_arg_75 = static_cast<std::uint8_t>(GlobalsV::AnimMainFon ^ 1);
            pas::WideString localizedText_90 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_90), cpp_arg_75, false);
        }
        AddOptionLabel(u"BackgroundShade"_w, aConst::LocalizedText(u"FormCfgSettings.BackgroundShade"_wref.get()), false);
        {
            std::uint8_t backgroundShade = GlobalsV::BackgroundShade;
            pas::WideString localizedText_91 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_91), backgroundShade, false);
        }
        {
            std::uint8_t cpp_arg_76 = static_cast<std::uint8_t>(GlobalsV::BackgroundShade ^ 1);
            pas::WideString localizedText_92 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_92), cpp_arg_76, false);
        }
        AddOptionLabel(u"BackgroundGrayscale"_w, aConst::LocalizedText(u"FormCfgSettings.BackgroundGrayscale"_wref.get()), false);
        {
            std::uint8_t backgroundGrayscale = GlobalsV::BackgroundGrayscale;
            pas::WideString localizedText_93 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_93), backgroundGrayscale, false);
        }
        {
            std::uint8_t cpp_arg_77 = static_cast<std::uint8_t>(GlobalsV::BackgroundGrayscale ^ 1);
            pas::WideString localizedText_94 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_94), cpp_arg_77, false);
        }
        AddOptionLabel(u"DynamicTipsPos"_w, aConst::LocalizedText(u"FormCfgSettings.DynamicTipsPos"_wref.get()), false);
        {
            std::uint8_t dynamicTipsPos = GlobalsV::DynamicTipsPos;
            pas::WideString localizedText_95 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_95), dynamicTipsPos, false);
        }
        {
            std::uint8_t cpp_arg_78 = static_cast<std::uint8_t>(GlobalsV::DynamicTipsPos ^ 1);
            pas::WideString localizedText_96 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_96), cpp_arg_78, false);
        }
        AddOptionLabel(u"ShowFPS"_w, aConst::LocalizedText(u"FormCfgSettings.ShowFPS"_wref.get()), false);
        {
            std::uint8_t showFrameRate = GR_Main::ShowFrameRate;
            pas::WideString localizedText_97 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_97), showFrameRate, false);
        }
        {
            std::uint8_t cpp_arg_79 = static_cast<std::uint8_t>(GR_Main::ShowFrameRate ^ 1);
            pas::WideString localizedText_98 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_98), cpp_arg_79, false);
        }
        AddOptionLabel(u"ScreenShotType"_w, aConst::LocalizedText(u"FormCfgSettings.ScreenShotType"_wref.get()), false);
        {
            std::uint8_t cpp_arg_80 = GlobalsV::ScreenshotFormat == 0;
            pas::WideString localizedText_99 = aConst::LocalizedText(u"FormCfgSettings.ShotBMP"_wref.get());
            AddOptionChoice(0, std::move(localizedText_99), cpp_arg_80, false);
        }
        {
            std::uint8_t cpp_arg_81 = GlobalsV::ScreenshotFormat == 1;
            pas::WideString localizedText_100 = aConst::LocalizedText(u"FormCfgSettings.ShotPNG"_wref.get());
            AddOptionChoice(1, std::move(localizedText_100), cpp_arg_81, false);
        }
        {
            std::uint8_t cpp_arg_82 = GlobalsV::ScreenshotFormat == 2;
            pas::WideString localizedText_101 = aConst::LocalizedText(u"FormCfgSettings.ShotJPG"_wref.get());
            AddOptionChoice(2, std::move(localizedText_101), cpp_arg_82, false);
        }
        ValueLabel = AddOptionLabel(u"ScreenShotQuality"_w, aConst::LocalizedText(u"FormCfgSettings.ScreenShotQuality"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, GlobalsV::ScreenshotJpegQuality, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        AddOptionLabel(u"FontGalaxy"_w, aConst::LocalizedText(u"FormCfgSettings.FontGalaxy"_wref.get()), true);
        {
            std::uint8_t cpp_arg_83 = GlobalsV::GalaxyMapFontChoice == GlobalsV::gmfNormalBold;
            pas::WideString localizedText_102 = aConst::LocalizedText(u"FormCfgSettings.FontGalaxyNormalBold"_wref.get());
            AddOptionChoice(6, std::move(localizedText_102), cpp_arg_83, false);
        }
        {
            std::uint8_t cpp_arg_84 = GlobalsV::GalaxyMapFontChoice == GlobalsV::gmfNormal;
            pas::WideString localizedText_103 = aConst::LocalizedText(u"FormCfgSettings.FontGalaxyNormal"_wref.get());
            AddOptionChoice(5, std::move(localizedText_103), cpp_arg_84, false);
        }
        {
            std::uint8_t cpp_arg_85 = GlobalsV::GalaxyMapFontChoice == GlobalsV::gmfSmallBold;
            pas::WideString localizedText_104 = aConst::LocalizedText(u"FormCfgSettings.FontGalaxySmallBold"_wref.get());
            AddOptionChoice(4, std::move(localizedText_104), cpp_arg_85, false);
        }
        {
            std::uint8_t cpp_arg_86 = GlobalsV::GalaxyMapFontChoice == GlobalsV::gmfSmall;
            pas::WideString localizedText_105 = aConst::LocalizedText(u"FormCfgSettings.FontGalaxySmall"_wref.get());
            AddOptionChoice(3, std::move(localizedText_105), cpp_arg_86, false);
        }
        {
            std::uint8_t cpp_arg_87 = GlobalsV::GalaxyMapFontChoice == GlobalsV::gmfMini;
            pas::WideString localizedText_106 = aConst::LocalizedText(u"FormCfgSettings.FontGalaxyMini"_wref.get());
            AddOptionChoice(2, std::move(localizedText_106), cpp_arg_87, false);
        }
        {
            std::uint8_t cpp_arg_88 = GlobalsV::GalaxyMapFontChoice == GlobalsV::gmfRanger;
            pas::WideString localizedText_107 = aConst::LocalizedText(u"FormCfgSettings.FontGalaxyRanger"_wref.get());
            AddOptionChoice(1, std::move(localizedText_107), cpp_arg_88, false);
        }
        BuildGroupIndex = 2;
        AddOptionLabel(u"Sound"_w, aConst::LocalizedText(u"FormCfgSettings.Sound"_wref.get()), true);
        {
            std::uint8_t soundEnabled = GlobalsV::SoundEnabled;
            std::uint8_t cpp_arg_89 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"Sound"_wref.get()) ^ 1);
            pas::WideString localizedText_108 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_108), soundEnabled, cpp_arg_89);
        }
        {
            std::uint8_t cpp_arg_90 = static_cast<std::uint8_t>(GlobalsV::SoundEnabled ^ 1);
            pas::WideString localizedText_109 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_109), cpp_arg_90, false);
        }
        AddOptionLabel(u"SoundInSpace"_w, aConst::LocalizedText(u"FormCfgSettings.SoundInSpace"_wref.get()), false);
        {
            std::uint8_t soundInSpaceEnabled = GlobalsV::SoundInSpaceEnabled;
            std::uint8_t cpp_arg_91 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"SoundInSpace"_wref.get()) ^ 1);
            pas::WideString localizedText_110 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_110), soundInSpaceEnabled, cpp_arg_91);
        }
        {
            std::uint8_t cpp_arg_92 = static_cast<std::uint8_t>(GlobalsV::SoundInSpaceEnabled ^ 1);
            pas::WideString localizedText_111 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_111), cpp_arg_92, false);
        }
        ValueLabel = AddOptionLabel(u"SoundVolume"_w, aConst::LocalizedText(u"FormCfgSettings.SoundVolume"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GlobalsV::SoundVolume * 1.0E+2L), 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        AddOptionLabel(u"Music"_w, aConst::LocalizedText(u"FormCfgSettings.Music"_wref.get()), true);
        {
            std::uint8_t musicEnabled = GlobalsV::MusicEnabled;
            std::uint8_t cpp_arg_93 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"Music"_wref.get()) ^ 1);
            pas::WideString localizedText_112 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_112), musicEnabled, cpp_arg_93);
        }
        {
            std::uint8_t cpp_arg_94 = static_cast<std::uint8_t>(GlobalsV::MusicEnabled ^ 1);
            pas::WideString localizedText_113 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_113), cpp_arg_94, false);
        }
        AddOptionLabel(u"MusicInSpace"_w, aConst::LocalizedText(u"FormCfgSettings.MusicInSpace"_wref.get()), false);
        {
            std::uint8_t musicInSpaceEnabled = GlobalsV::MusicInSpaceEnabled;
            std::uint8_t cpp_arg_95 = static_cast<std::uint8_t>(GR_Main::IsInstallFeatureEnabled(u"MusicInSpace"_wref.get()) ^ 1);
            pas::WideString localizedText_114 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_114), musicInSpaceEnabled, cpp_arg_95);
        }
        {
            std::uint8_t cpp_arg_96 = static_cast<std::uint8_t>(GlobalsV::MusicInSpaceEnabled ^ 1);
            pas::WideString localizedText_115 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_115), cpp_arg_96, false);
        }
        ValueLabel = AddOptionLabel(u"MusicVolume"_w, aConst::LocalizedText(u"FormCfgSettings.MusicVolume"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GlobalsV::MusicVolume * 1.0E+2L), 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        AddOptionLabel(u"MusicInHyper"_w, aConst::LocalizedText(u"FormCfgSettings.MusicInHyper"_wref.get()), false);
        {
            std::uint8_t musicInHyperEnabled = GlobalsV::MusicInHyperEnabled;
            pas::WideString localizedText_116 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_116), musicInHyperEnabled, false);
        }
        {
            std::uint8_t cpp_arg_97 = static_cast<std::uint8_t>(GlobalsV::MusicInHyperEnabled ^ 1);
            pas::WideString localizedText_117 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_117), cpp_arg_97, false);
        }
        AddOptionLabel(u"MusicInPlanet"_w, aConst::LocalizedText(u"FormCfgSettings.MusicInPlanet"_wref.get()), false);
        {
            std::uint8_t musicInPlanetEnabled = GlobalsV::MusicInPlanetEnabled;
            pas::WideString localizedText_118 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_118), musicInPlanetEnabled, false);
        }
        {
            std::uint8_t cpp_arg_98 = static_cast<std::uint8_t>(GlobalsV::MusicInPlanetEnabled ^ 1);
            pas::WideString localizedText_119 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_119), cpp_arg_98, false);
        }
        BuildGroupIndex = 4;
        if (GR_Main::AltResolutionSwitch) {
            AddOptionLabel(u"RobotResolution"_w, aConst::LocalizedText(u"FormCfgSettings.Resolution"_wref.get()), true);
            I = GR_Main::RobotDisplayModeCount - 1;
            while (I >= 0) {
                {
                    GR_Main::TDisplayModeGR& cpp_with_3 = GR_Main::RobotDisplayModes[I];
                    if (cpp_with_3.Width == 0) {
                        std::uint8_t cpp_arg_99 = GR_Main::SelectedRobotDisplayMode == I;
                        pas::WideString localizedText_120 = aConst::LocalizedText(u"FormCfgSettings.HelpButAuto"_wref.get());
                        AddOptionChoice(I, std::move(localizedText_120), cpp_arg_99, false);
                    } else {
                        AddOptionChoice(I, static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(cpp_with_3.Width), "x", SysUtils::Int64ToStr(cpp_with_3.Height)})), GR_Main::SelectedRobotDisplayMode == I, false);
                    }
                }
                --I;
            }
        } else {
            ValueLabel = AddOptionLabel(u"RobotResolution"_w, aConst::LocalizedText(u"FormCfgSettings.Resolution"_wref.get()), false);
            AddOptionSlider(ValueLabel, 0, GR_Main::RobotDisplayModeCount - 1, GR_Main::SelectedRobotDisplayMode, 1, pas::bind_static_method<&TfCfgSettings::FormatRobotResolution>(this));
        }
        AddOptionLabel(u"RobotVSync"_w, aConst::LocalizedText(u"FormCfgSettings.VSync"_wref.get()), false);
        {
            std::uint8_t robotVSync = Robot::RobotVSync;
            pas::WideString localizedText_121 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_121), robotVSync, false);
        }
        {
            std::uint8_t cpp_arg_100 = static_cast<std::uint8_t>(Robot::RobotVSync ^ 1);
            pas::WideString localizedText_122 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_122), cpp_arg_100, false);
        }
        if (Robot::SupportedMultiSampleCount > 1) {
            ValueLabel = AddOptionLabel(u"RobotFSAASamples"_w, aConst::LocalizedText(u"FormCfgSettings.RobotFSAA"_wref.get()), false);
            AddOptionSlider(ValueLabel, 0, Robot::SupportedMultiSampleCount - 1, Robot::GetRobotMultiSampleIndex(), 1, pas::bind_static_method<&TfCfgSettings::FormatRobotFsaaSamples>(this));
        }
        if (static_cast<std::int32_t>(Robot::MaximumAnisotropy) > 0) {
            ValueLabel = AddOptionLabel(u"RobotAnisotropy"_w, aConst::LocalizedText(u"FormCfgSettings.RobotAnisotropy"_wref.get()), false);
            AddOptionSlider(ValueLabel, 0, Robot::MaximumAnisotropy, Robot::RobotAnisotropy, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        }
        ValueLabel = AddOptionLabel(u"RobotMaxDistance"_w, aConst::LocalizedText(u"FormCfgSettings.RobotMaxDistance"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, Robot::RobotMaxDistance, 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        ValueLabel = AddOptionLabel(u"RobotBrightness"_w, aConst::LocalizedText(u"FormCfgSettings.Brightness"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GR_Main::RobotBrightness * 5.0E+1L + 5.0E+1L), 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        ValueLabel = AddOptionLabel(u"RobotContrast"_w, aConst::LocalizedText(u"FormCfgSettings.Contrast"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GR_Main::RobotContrast * 5.0E+1L + 5.0E+1L), 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        AddOptionLabel(u"RobotShowStencilShadows"_w, aConst::LocalizedText(u"FormCfgSettings.RobotShowStencilShadows"_wref.get()), true);
        {
            std::uint8_t showStencilShadows = Robot::RobotSettings.ShowStencilShadows;
            pas::WideString localizedText_123 = aConst::LocalizedText(u"FormCfgSettings.RobotShowStencilShadowsOn"_wref.get());
            AddOptionChoice(1, std::move(localizedText_123), showStencilShadows, false);
        }
        {
            std::uint8_t cpp_arg_101 = static_cast<std::uint8_t>(Robot::RobotSettings.ShowStencilShadows ^ 1);
            pas::WideString localizedText_124 = aConst::LocalizedText(u"FormCfgSettings.RobotShowStencilShadowsOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_124), cpp_arg_101, false);
        }
        AddOptionLabel(u"RobotShowProjShadows"_w, aConst::LocalizedText(u"FormCfgSettings.RobotShowProjShadows"_wref.get()), true);
        {
            std::uint8_t showProjShadows = Robot::RobotSettings.ShowProjShadows;
            pas::WideString localizedText_125 = aConst::LocalizedText(u"FormCfgSettings.RobotShowProjShadowsOn"_wref.get());
            AddOptionChoice(1, std::move(localizedText_125), showProjShadows, false);
        }
        {
            std::uint8_t cpp_arg_102 = static_cast<std::uint8_t>(Robot::RobotSettings.ShowProjShadows ^ 1);
            pas::WideString localizedText_126 = aConst::LocalizedText(u"FormCfgSettings.RobotShowProjShadowsOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_126), cpp_arg_102, false);
        }
        AddOptionLabel(u"RobotRobotShadow"_w, aConst::LocalizedText(u"FormCfgSettings.RobotRobotShadow"_wref.get()), true);
        {
            std::uint8_t cpp_arg_103 = Robot::RobotSettings.RobotShadow == 1;
            pas::WideString localizedText_127 = aConst::LocalizedText(u"FormCfgSettings.RobotRobotShadowStencil"_wref.get());
            AddOptionChoice(1, std::move(localizedText_127), cpp_arg_103, false);
        }
        {
            std::uint8_t cpp_arg_104 = Robot::RobotSettings.RobotShadow == 0;
            pas::WideString localizedText_128 = aConst::LocalizedText(u"FormCfgSettings.RobotRobotShadowOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_128), cpp_arg_104, false);
        }
        AddOptionLabel(u"RobotSelectEx"_w, aConst::LocalizedText(u"FormCfgSettings.RobotSelectEx"_wref.get()), true);
        {
            std::uint8_t cpp_arg_105 = static_cast<std::uint8_t>(Robot::RobotSettings.SelectEx ^ 1);
            pas::WideString localizedText_129 = aConst::LocalizedText(u"FormCfgSettings.RobotSelectExNormal"_wref.get());
            AddOptionChoice(0, std::move(localizedText_129), cpp_arg_105, false);
        }
        {
            std::uint8_t selectEx = Robot::RobotSettings.SelectEx;
            pas::WideString localizedText_130 = aConst::LocalizedText(u"FormCfgSettings.RobotSelectExSpecial"_wref.get());
            AddOptionChoice(1, std::move(localizedText_130), selectEx, false);
        }
        AddOptionLabel(u"RobotLandTexturesGloss"_w, aConst::LocalizedText(u"FormCfgSettings.RobotLandTexturesGloss"_wref.get()), true);
        {
            std::uint8_t landTexturesGloss = Robot::RobotSettings.LandTexturesGloss;
            pas::WideString localizedText_131 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_131), landTexturesGloss, false);
        }
        {
            std::uint8_t cpp_arg_106 = static_cast<std::uint8_t>(Robot::RobotSettings.LandTexturesGloss ^ 1);
            pas::WideString localizedText_132 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_132), cpp_arg_106, false);
        }
        AddOptionLabel(u"RobotObjTexturesGloss"_w, aConst::LocalizedText(u"FormCfgSettings.RobotObjTexturesGloss"_wref.get()), true);
        {
            std::uint8_t objTexturesGloss = Robot::RobotSettings.ObjTexturesGloss;
            pas::WideString localizedText_133 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_133), objTexturesGloss, false);
        }
        {
            std::uint8_t cpp_arg_107 = static_cast<std::uint8_t>(Robot::RobotSettings.ObjTexturesGloss ^ 1);
            pas::WideString localizedText_134 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_134), cpp_arg_107, false);
        }
        AddOptionLabel(u"RobotSoftwareCursor"_w, aConst::LocalizedText(u"FormCfgSettings.RobotSoftwareCursor"_wref.get()), true);
        {
            std::uint8_t cpp_arg_108 = static_cast<std::uint8_t>(Robot::RobotSettings.SoftwareCursor ^ 1);
            pas::WideString localizedText_135 = aConst::LocalizedText(u"FormCfgSettings.RobotSoftwareCursorHardware"_wref.get());
            AddOptionChoice(0, std::move(localizedText_135), cpp_arg_108, false);
        }
        {
            std::uint8_t softwareCursor = Robot::RobotSettings.SoftwareCursor;
            pas::WideString localizedText_136 = aConst::LocalizedText(u"FormCfgSettings.RobotSoftwareCursorSoftware"_wref.get());
            AddOptionChoice(1, std::move(localizedText_136), softwareCursor, false);
        }
        AddOptionLabel(u"RobotSky"_w, aConst::LocalizedText(u"FormCfgSettings.RobotSky"_wref.get()), true);
        {
            std::uint8_t cpp_arg_109 = Robot::RobotSettings.Sky == 2;
            pas::WideString localizedText_137 = aConst::LocalizedText(u"FormCfgSettings.RobotSkyGood"_wref.get());
            AddOptionChoice(2, std::move(localizedText_137), cpp_arg_109, false);
        }
        {
            std::uint8_t cpp_arg_110 = Robot::RobotSettings.Sky == 1;
            pas::WideString localizedText_138 = aConst::LocalizedText(u"FormCfgSettings.RobotSkyLow"_wref.get());
            AddOptionChoice(1, std::move(localizedText_138), cpp_arg_110, false);
        }
        {
            std::uint8_t cpp_arg_111 = Robot::RobotSettings.Sky == 0;
            pas::WideString localizedText_139 = aConst::LocalizedText(u"FormCfgSettings.RobotSkyOff"_wref.get());
            AddOptionChoice(0, std::move(localizedText_139), cpp_arg_111, false);
        }
        BuildGroupIndex = 5;
        AddOptionLabel(u"RobotMusic"_w, aConst::LocalizedText(u"FormCfgSettings.RobotMusic"_wref.get()), true);
        {
            std::uint8_t robotMusic = Robot::RobotMusic;
            pas::WideString localizedText_140 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_140), robotMusic, false);
        }
        {
            std::uint8_t cpp_arg_112 = static_cast<std::uint8_t>(Robot::RobotMusic ^ 1);
            pas::WideString localizedText_141 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_141), cpp_arg_112, false);
        }
        ValueLabel = AddOptionLabel(u"RobotMusicVolume"_w, aConst::LocalizedText(u"FormCfgSettings.MusicVolume"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GlobalsV::RobotMusicVolume * 1.0E+2L), 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        AddOptionLabel(u"RobotSound"_w, aConst::LocalizedText(u"FormCfgSettings.RobotSound"_wref.get()), true);
        {
            std::uint8_t robotSound = Robot::RobotSound;
            pas::WideString localizedText_142 = aConst::LocalizedText(u"FormCfgSettings.Yes"_wref.get());
            AddOptionChoice(1, std::move(localizedText_142), robotSound, false);
        }
        {
            std::uint8_t cpp_arg_113 = static_cast<std::uint8_t>(Robot::RobotSound ^ 1);
            pas::WideString localizedText_143 = aConst::LocalizedText(u"FormCfgSettings.No"_wref.get());
            AddOptionChoice(0, std::move(localizedText_143), cpp_arg_113, false);
        }
        ValueLabel = AddOptionLabel(u"RobotSoundVolume"_w, aConst::LocalizedText(u"FormCfgSettings.SoundVolume"_wref.get()), false);
        AddOptionSlider(ValueLabel, 0, 100, System::Round(GlobalsV::RobotSoundVolume * 1.0E+2L), 1, pas::bind_static_method<&TfCfgSettings::FormatInteger>(this));
        BuildGroupIndex = 3;
        for (I = 0; I <= 5; ++I) {
            GI_Panel::TPanelGI* cpp_with_4 = GroupPanels[I];
            cpp_with_4->SetSize(ClassesImports::Point(cpp_with_4->ClientSize.X, GroupNextY[I]));
        }
        {
            std::int32_t cpp_arg_114 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv))->CaptionLabel->GetLineHeight() * 2;
            GI_ScrollBar::TScrollBarGI* verticalScrollBar = Panel->VerticalScrollBar;
            verticalScrollBar->SetSmallChange(cpp_arg_114);
        }
        Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
        Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
        ActiveGroupIndex = 0;
        RefreshVisibleGroup();
        RefreshModeUi();
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(131);
        }
    }

    void TfCfgSettings::OnClose() {
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(132);
        }
        if (ModeLeaveTimer != nullptr) {
            CancelCallbackTimer(ModeLeaveTimer);
            ModeLeaveTimer = nullptr;
        }
    }

    void TfCfgSettings::MainPanelMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_GraphButton::TGraphButtonGI* Button = nullptr;
        std::uint8_t Show = false;
        do {
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButAUp"sv));
            Show = Button->HitTest(Point);
            if (Show) {
                break;
            }
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButAMiddle"sv));
            Show = Button->HitTest(Point);
            if (Show) {
                break;
            }
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButADown"sv));
            Show = Button->HitTest(Point);
            if (Show) {
                break;
            }
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButAAuto"sv));
            Show = Button->HitTest(Point);
            if (Show) {
                break;
            }
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Cancel"sv));
            Show = Button->HitTest(Point);
            if (Show) {
                break;
            }
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv));
            Show = Button->HitTest(Point);
            if (Show) {
                break;
            }
        } while (!true);
        ShowControlHelp(Button, Show);
    }

    void TfCfgSettings::ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Show) {
        GI_Label::TLabelGI* LabelHelp = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LabelHelp"sv));
        if (Sender->HelpText == u"") {
            Show = false;
        }
        LabelHelp->SetActive(Show);
        LabelHelp->SetText(Sender->HelpText);
    }

    void TfCfgSettings::GroupClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Group = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        if (SettingsMode == 1) {
            Group = Group + 4 - 1;
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv))->SetDown(Group == 0 || Group == 4);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv))->SetDown(Group == 1 || Group == 5);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv))->SetDown(Group == 2);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup3"sv))->SetDown(Group == 3);
        if (ActiveGroupIndex != Group) {
            ActiveGroupIndex = Group;
            RefreshVisibleGroup();
            RefreshModeUi();
        }
    }

    void TfCfgSettings::RefreshVisibleGroup() {
        std::int32_t I{};
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
            ButGroup0->SetDown(ActiveGroupIndex == 0);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
            ButGroup1->SetDown(ActiveGroupIndex == 1 || ActiveGroupIndex == 4);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
            ButGroup2->SetDown(ActiveGroupIndex == 2 || ActiveGroupIndex == 5);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup3"sv));
            ButGroup3->SetDown(ActiveGroupIndex == 3);
        }
        for (I = 0; I <= 5; ++I) {
            GI_Panel::TPanelGI* cpp_with_5 = GroupPanels[I];
            cpp_with_5->SetActive(I == ActiveGroupIndex);
        }
        {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"sv));
            PanelSet->SetScrollOffset(ClassesImports::Point(0, 0));
            PanelSet->UpdateScrollRanges();
            PanelSet->SetVerticalScrollbarEnabled(GroupNextY[ActiveGroupIndex] > PanelSet->ClientSize.Y);
        }
    }

    GI_Label::TLabelGI* TfCfgSettings::AddOptionLabel(pas::WideString OptionName, pas::WideString Caption, std::uint8_t UnusedFlag) {
        CurrentOptionName = std::move(OptionName);
        if (GroupNextY[BuildGroupIndex] != 0) {
            // Native retains a zero-spacing adjustment before the separator.
            GroupNextY[BuildGroupIndex] = GroupNextY[BuildGroupIndex];
            {
                GI_Image::TImageGI* cpp_with = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, GroupPanels[BuildGroupIndex]);
                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"Line"}));
                cpp_with->SetPosition(ClassesImports::Point(0, GroupNextY[BuildGroupIndex]));
                {
                    std::int32_t cpp_arg = cpp_with->GetContentSize().Y + 2;
                    std::int32_t x = GroupPanels[BuildGroupIndex]->ClientSize.X;
                    cpp_with->SetSize(ClassesImports::Point(x, cpp_arg));
                }
                GroupNextY[BuildGroupIndex] += cpp_with->ClientSize.Y;
            }
        }
        GI_Label::TLabelGI* Result = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, GroupPanels[BuildGroupIndex]);
        Result->SetFontName(GlobalsV::NormalFontName);
        Result->SetPositionModeW(false);
        Result->SetPosition(ClassesImports::Point(0, GroupNextY[BuildGroupIndex]));
        Result->SetSize(ClassesImports::Point(GroupPanels[BuildGroupIndex]->ClientSize.X, 1));
        Result->SetTextAlignX(GI_Main::taxLeft);
        Result->SetTextAlignY(GI_Main::tayAuto);
        Result->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(205, 205, 205));
        Result->SetText(pas::concat_wide({Caption, u"."}));
        Result->HelpText = Caption;
        Result->SetTextAlignY(GI_Main::tayTop);
        Result->SetSize(ClassesImports::Point(Result->ClientSize.X, Result->ClientSize.Y + 1));
        GroupNextY[BuildGroupIndex] += 2;
        return Result;
    }

    void TfCfgSettings::AddOptionChoice(std::int32_t Value, pas::WideString Caption, std::uint8_t Selected, std::uint8_t Disabled) {
        std::int32_t RightMargin = GR_Main::GiScalePixelsEx(50, 30);
        GI_Label::TLabelGI* ValueLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, GroupPanels[BuildGroupIndex]);
        ValueLabel->SetFontName(GlobalsV::NormalFontName);
        ValueLabel->SetPositionModeW(false);
        ValueLabel->SetPosition(ClassesImports::Point(0, GroupNextY[BuildGroupIndex]));
        ValueLabel->SetSize(ClassesImports::Point(GroupPanels[BuildGroupIndex]->ClientSize.X - RightMargin, 1));
        ValueLabel->SetTextAlignX(GI_Main::taxRight);
        ValueLabel->SetTextAlignY(GI_Main::tayAuto);
        if (Selected) {
            ValueLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 234, 118));
        } else {
            ValueLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(205, 205, 205));
        }
        ValueLabel->SetText(Caption);
        if (!Disabled) {
            ValueLabel->LeftButtonDownCallback = pas::bind_method<&TfCfgSettings::OptionChoiceMouseDown>(this);
            ValueLabel->MouseEnterCallback = pas::bind_static_method<&TfCfgSettings::OptionChoiceMouseEnter>(this);
            ValueLabel->MouseLeaveCallback = pas::bind_static_method<&TfCfgSettings::OptionChoiceMouseLeave>(this);
        }
        ValueLabel->SetTextAlignY(GI_Main::tayCenterEx);
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, GroupPanels[BuildGroupIndex]);
        if (Selected) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchD"}));
        } else if (Disabled) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchH"}));
        } else {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"}));
        }
        {
            std::int32_t cpp_right = Image->GetContentSize().X;
            std::int32_t cpp_arg = GroupPanels[BuildGroupIndex]->ClientSize.X - cpp_right - RightMargin;
            std::int32_t cpp_arg_2 = GroupNextY[BuildGroupIndex];
            Image->SetPosition(ClassesImports::Point(cpp_arg, cpp_arg_2));
        }
        Image->SetSize(Image->GetContentSize());
        Image->SetImageKindY(GI_Main::ikyCenter);
        if (!Disabled) {
            Image->LeftButtonDownCallback = pas::bind_method<&TfCfgSettings::OptionChoiceMouseDown>(this);
            Image->MouseEnterCallback = pas::bind_static_method<&TfCfgSettings::OptionChoiceMouseEnter>(this);
            Image->MouseLeaveCallback = pas::bind_static_method<&TfCfgSettings::OptionChoiceMouseLeave>(this);
        }
        if (static_cast<std::uint8_t>(Disabled ^ 1) || CurrentOptionName == u"Lang") {
            Image->SetName(CurrentOptionName);
        }
        Image->UserValue = Value;
        ValueLabel->SetSize(ClassesImports::Point(ValueLabel->ClientSize.X - Image->ClientSize.X - 10, std::max<std::int32_t>(ValueLabel->ClientSize.Y, Image->ClientSize.Y)));
        Image->SetPosition(ClassesImports::Point(Image->LocalPosition.X, Image->LocalPosition.Y + (std::max<std::int32_t>(ValueLabel->ClientSize.Y, Image->ClientSize.Y) - ValueLabel->ClientSize.Y) / 2));
        GroupNextY[BuildGroupIndex] = GroupNextY[BuildGroupIndex] + ValueLabel->ClientSize.Y + GR_Main::GiScalePixels(5);
        ValueLabel->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Image));
    }

    void TfCfgSettings::OptionChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (!(pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr)) {
            Sender = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        }
        GI_MessageLoop::TObjectGI* Control = GroupPanels[ActiveGroupIndex]->FirstChild;
        while (Control != nullptr) {
            if (Control->ControlName == Sender->ControlName) {
                if (Control == Sender) {
                    GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(Control);
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchD"});
                    cpp_arg->SetImagePath(std::move(cpp_arg_2));
                } else {
                    GI_Image::TImageGI* cpp_arg_3 = pas::checked_cast<GI_Image::TImageGI*>(Control);
                    pas::WideString cpp_arg_4 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"});
                    cpp_arg_3->SetImagePath(std::move(cpp_arg_4));
                }
            }
            Control = Control->NextSibling;
        }
        if (Point.X != -1000 || Point.Y != -1000) {
            GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
        }
    }

    void TfCfgSettings::OptionChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (!(pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr)) {
            Sender = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        }
        if (([&] {
            pas::WideString cpp_string = pas::checked_cast<GI_Image::TImageGI*>(Sender)->GetImagePath();
            pas::WideString cpp_string_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"});
            return cpp_string == cpp_string_2;
        }())) {
            GI_Image::TImageGI* cpp_arg = static_cast<GI_Image::TImageGI*>(Sender);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchA"});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
    }

    void TfCfgSettings::OptionChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (!(pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr)) {
            Sender = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        }
        if (([&] {
            pas::WideString cpp_string = pas::checked_cast<GI_Image::TImageGI*>(Sender)->GetImagePath();
            pas::WideString cpp_string_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchA"});
            return cpp_string == cpp_string_2;
        }())) {
            GI_Image::TImageGI* cpp_arg = static_cast<GI_Image::TImageGI*>(Sender);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
    }

    // Invokes Callback immediately with the new slider.
    void TfCfgSettings::AddOptionSlider(GI_Label::TLabelGI* ValueLabel, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Position, std::int32_t UnusedStep, TOptionSliderEvent Callback) {
        GI_CountBar::TCountBarGI* Slider = pas::construct_call<GI_CountBar::TCountBarGI>(GI_CountBar::TCountBarGI_Create, GroupPanels[BuildGroupIndex]);
        GroupNextY[BuildGroupIndex] = GroupNextY[BuildGroupIndex];
        Slider->SetPositionModeW(false);
        if (GR_Main::GiResourceVariant() == 2) {
            Slider->SetSize(ClassesImports::Point(199, 20));
        } else {
            Slider->SetSize(ClassesImports::Point(156, 20));
        }
        Slider->SetPosition(ClassesImports::Point(GroupPanels[BuildGroupIndex]->ClientSize.X - Slider->ClientSize.X, GroupNextY[BuildGroupIndex]));
        Slider->DecreaseButton->SetKind(GI_GraphButton::gbkDisable);
        Slider->DecreaseButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackLeftN"}));
        Slider->DecreaseButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackLeftA"}));
        Slider->DecreaseButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackLeftD"}));
        Slider->DecreaseButton->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackLeftH"}));
        Slider->DecreaseButton->EnterSound = u"Sound.ButtonEnter"_w;
        Slider->DecreaseButton->LeaveSound = u"Sound.ButtonLeave"_w;
        Slider->DecreaseButton->ClickSound = u"Sound.ButtonClick"_w;
        Slider->IncreaseButton->SetKind(GI_GraphButton::gbkDisable);
        Slider->IncreaseButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackRightN"}));
        Slider->IncreaseButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackRightA"}));
        Slider->IncreaseButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackRightD"}));
        Slider->IncreaseButton->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackRightH"}));
        Slider->IncreaseButton->EnterSound = u"Sound.ButtonEnter"_w;
        Slider->IncreaseButton->LeaveSound = u"Sound.ButtonLeave"_w;
        Slider->IncreaseButton->ClickSound = u"Sound.ButtonClick"_w;
        Slider->MarkerImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackUp"}));
        {
            Types::TPoint contentSize = Slider->MarkerImage->GetContentSize();
            GI_Image::TImageGI* markerImage = Slider->MarkerImage;
            markerImage->SetSize(contentSize);
        }
        Slider->MarkerImage->SetOrigin(EC_Struct::HalfPoint(Slider->MarkerImage->ClientSize));
        Slider->AfterThumbImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackLeft"}));
        Slider->BeforeThumbImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackRight"}));
        Slider->ThumbButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Slider->ThumbButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Slider->ThumbButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"TrackPol"}));
        Slider->PositionChangedCallback = Callback;
        Slider->UpdateLayout();
        Slider->SetRange(Minimum, Maximum);
        Slider->SetPositionInternal(Position);
        Slider->SetName(CurrentOptionName);
        Slider->UserIndex = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(ValueLabel));
        GroupNextY[BuildGroupIndex] = GroupNextY[BuildGroupIndex] + Slider->ClientSize.Y + GR_Main::GiScalePixels(6);
        Callback(Slider);
    }

    // Searches only the active group.
    std::uint8_t TfCfgSettings::HasOptionValue(const std::u16string_view& OptionName) {
        GI_MessageLoop::TObjectGI* Control = GroupPanels[ActiveGroupIndex]->FirstChild;
        while (Control != nullptr) {
            if (pas::view(Control->ControlName) == OptionName) {
                if (GI_Image::TImageGI* imageGI = pas::class_cast_if<GI_Image::TImageGI*>(Control)) {
                    if (([&] {
                        pas::WideString cpp_string = imageGI->GetImagePath();
                        pas::WideString cpp_string_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchD"});
                        return cpp_string == cpp_string_2;
                    }())) {
                        return true;
                    }
                } else if (pas::class_cast_if<GI_CountBar::TCountBarGI*>(Control) != nullptr) {
                    return true;
                }
            }
            Control = Control->NextSibling;
        }
        return false;
    }

    // Searches only the active group; raises when no selected choice or slider exists.
    std::int32_t TfCfgSettings::GetOptionValue(const std::u16string_view& OptionName) {
        std::int32_t Result = 0;
        GI_MessageLoop::TObjectGI* Control = GroupPanels[ActiveGroupIndex]->FirstChild;
        while (Control != nullptr) {
            if (pas::view(Control->ControlName) == OptionName) {
                if (GI_Image::TImageGI* imageGI = pas::class_cast_if<GI_Image::TImageGI*>(Control)) {
                    if (([&] {
                        pas::WideString cpp_string = imageGI->GetImagePath();
                        pas::WideString cpp_string_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchD"});
                        return cpp_string == cpp_string_2;
                    }())) {
                        return Control->UserValue;
                    }
                } else if (GI_CountBar::TCountBarGI* countBarGI = pas::class_cast_if<GI_CountBar::TCountBarGI*>(Control)) {
                    return countBarGI->Position;
                }
            }
            Control = Control->NextSibling;
        }
        GR_Main::RaiseWideMessage(pas::concat_wide({u"UnitGet Type=", OptionName}));
        return Result;
    }

    // Searches only the active group; missing options are ignored.
    void TfCfgSettings::SetOptionValue(const std::u16string_view& OptionName, std::int32_t Value) {
        GI_MessageLoop::TObjectGI* Control = GroupPanels[ActiveGroupIndex]->FirstChild;
        while (Control != nullptr) {
            if (pas::view(Control->ControlName) == OptionName) {
                if (pas::class_cast_if<GI_Image::TImageGI*>(Control) != nullptr && Control->UserValue == Value && pas::assigned(Control->LeftButtonDownCallback)) {
                    OptionChoiceMouseDown(Control, 0u, ClassesImports::Point(-1000, -1000));
                    break;
                } else if (GI_CountBar::TCountBarGI* countBarGI = pas::class_cast_if<GI_CountBar::TCountBarGI*>(Control)) {
                    countBarGI->SetPosition_2(Value);
                    break;
                }
            }
            Control = Control->NextSibling;
        }
    }

    void TfCfgSettings::FormatResolution(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        GI_Label::TLabelGI* ValueLabel{};
        if (Sender->UserIndex != 0) {
            ValueLabel = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            Index = reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position;
            if (GR_Main::GameDisplayModes[Index].Width == 0) {
                const pas::WideString& cpp_arg = pas::concat_wide({ValueLabel->HelpText, aMyFunction::TextHighlightColorTag, u" ", aConst::LocalizedText(u"FormCfgSettings.HelpAuto"_wref.get()), aMyFunction::EndColorTag});
                GI_Label::TLabelGI* valueLabel = ValueLabel;
                valueLabel->SetText(cpp_arg);
            } else {
                ValueLabel->SetText(pas::concat_wide({ValueLabel->HelpText, aMyFunction::TextHighlightColorTag, u" ", pas::wide_int64_to_str(static_cast<std::int64_t>(GR_Main::GameDisplayModes[Index].Width)), u"x", pas::wide_int64_to_str(static_cast<std::int64_t>(GR_Main::GameDisplayModes[Index].Height)), aMyFunction::EndColorTag}));
            }
        }
    }

    void TfCfgSettings::FormatRobotResolution(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        GI_Label::TLabelGI* ValueLabel{};
        if (Sender->UserIndex != 0) {
            ValueLabel = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            Index = reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position;
            if (GR_Main::RobotDisplayModes[Index].Width == 0) {
                const pas::WideString& cpp_arg = pas::concat_wide({ValueLabel->HelpText, aMyFunction::TextHighlightColorTag, u" ", aConst::LocalizedText(u"FormCfgSettings.HelpAuto"_wref.get()), aMyFunction::EndColorTag});
                GI_Label::TLabelGI* valueLabel = ValueLabel;
                valueLabel->SetText(cpp_arg);
            } else {
                ValueLabel->SetText(pas::concat_wide({ValueLabel->HelpText, aMyFunction::TextHighlightColorTag, u" ", pas::wide_int64_to_str(static_cast<std::int64_t>(GR_Main::RobotDisplayModes[Index].Width)), u"x", pas::wide_int64_to_str(static_cast<std::int64_t>(GR_Main::RobotDisplayModes[Index].Height)), aMyFunction::EndColorTag}));
            }
        }
    }

    void TfCfgSettings::FormatRobotFsaaSamples(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        GI_Label::TLabelGI* ValueLabel{};
        if (Sender->UserIndex != 0) {
            ValueLabel = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            Index = reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position;
            {
                const pas::WideString& replaceColoredToken = aMyFunction::ReplaceColoredToken(ValueLabel->HelpText, u"<Value>"_w, pas::wide_int_to_str(Robot::SupportedMultiSamples[Index]), aMyFunction::TextHighlightColorTag);
                GI_Label::TLabelGI* valueLabel = ValueLabel;
                valueLabel->SetText(replaceColoredToken);
            }
        }
    }

    // Changes display gamma before settings are applied.
    void TfCfgSettings::PreviewBrightness(GI_MessageLoop::TObjectGI* Sender) {
        GI_Label::TLabelGI* ValueLabel{};
        if (HasOptionValue(u"Contrast"sv) && HasOptionValue(u"Brightness"sv)) {
            float cpp_arg = pas::real_divide(GetOptionValue(u"Brightness"sv) - 50, 5.0E+1L);
            float cpp_arg_2 = pas::real_divide(GetOptionValue(u"Contrast"sv) - 50, 5.0E+1L);
            GR_Main::ApplyGammaRamp(cpp_arg, cpp_arg_2);
        }
        if (Sender->UserIndex != 0) {
            ValueLabel = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            {
                const pas::WideString& replaceColoredToken = aMyFunction::ReplaceColoredToken(ValueLabel->HelpText, u"<Value>"_w, pas::wide_int_to_str(reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position), aMyFunction::TextHighlightColorTag);
                GI_Label::TLabelGI* valueLabel = ValueLabel;
                valueLabel->SetText(replaceColoredToken);
            }
        }
    }

    // Changes display gamma before settings are applied.
    void TfCfgSettings::PreviewContrast(GI_MessageLoop::TObjectGI* Sender) {
        GI_Label::TLabelGI* ValueLabel{};
        if (HasOptionValue(u"Contrast"sv) && HasOptionValue(u"Brightness"sv)) {
            float cpp_arg = pas::real_divide(GetOptionValue(u"Brightness"sv) - 50, 5.0E+1L);
            float cpp_arg_2 = pas::real_divide(GetOptionValue(u"Contrast"sv) - 50, 5.0E+1L);
            GR_Main::ApplyGammaRamp(cpp_arg, cpp_arg_2);
        }
        if (Sender->UserIndex != 0) {
            ValueLabel = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            {
                const pas::WideString& replaceColoredToken = aMyFunction::ReplaceColoredToken(ValueLabel->HelpText, u"<Value>"_w, pas::wide_int_to_str(reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position), aMyFunction::TextHighlightColorTag);
                GI_Label::TLabelGI* valueLabel = ValueLabel;
                valueLabel->SetText(replaceColoredToken);
            }
        }
    }

    void TfCfgSettings::FormatInteger(GI_MessageLoop::TObjectGI* Sender) {
        GI_Label::TLabelGI* ValueLabel{};
        if (Sender->UserIndex != 0) {
            ValueLabel = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            {
                const pas::WideString& replaceColoredToken = aMyFunction::ReplaceColoredToken(ValueLabel->HelpText, u"<Value>"_w, pas::wide_int_to_str(reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position), aMyFunction::TextHighlightColorTag);
                GI_Label::TLabelGI* valueLabel = ValueLabel;
                valueLabel->SetText(replaceColoredToken);
            }
        }
    }

    void TfCfgSettings::FormatTurnSaveStep(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Value{};
        pas::WideString Text{};
        GI_Label::TLabelGI* ValueLabel{};
        if (Sender->UserIndex != 0) {
            ValueLabel = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            Value = reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position;
            if (Value == 0) {
                Text = aConst::LocalizedText(u"FormCfgSettings.TurnSaveStepNever"_wref.get());
            } else if (Value == 1) {
                Text = aConst::LocalizedText(u"FormCfgSettings.TurnSaveStep1"_wref.get());
            } else if (Value >= 2 && Value <= 4) {
                Text = ([&] {
                    auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString intToStr = pas::wide_int_to_str(Value);
                    pas::WideString localizedText = aConst::LocalizedText(u"FormCfgSettings.TurnSaveStep2"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(localizedText), u"<Value>"_w, std::move(intToStr), textHighlightColorTag.get());
                }());
            } else {
                Text = ([&] {
                    auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString intToStr_2 = pas::wide_int_to_str(Value);
                    pas::WideString localizedText_2 = aConst::LocalizedText(u"FormCfgSettings.TurnSaveStep3"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(localizedText_2), u"<Value>"_w, std::move(intToStr_2), textHighlightColorTag_2.get());
                }());
            }
            {
                const pas::WideString& replaceColoredToken = aMyFunction::ReplaceColoredToken(ValueLabel->HelpText, u"<Text>"_w, Text, aMyFunction::TextHighlightColorTag);
                GI_Label::TLabelGI* valueLabel = ValueLabel;
                valueLabel->SetText(replaceColoredToken);
            }
        }
    }

    void TfCfgSettings::FormatForsageDeactivatePercent(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Value{};
        pas::WideString Text{};
        GI_Label::TLabelGI* ValueLabel{};
        if (Sender->UserIndex != 0) {
            ValueLabel = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            Value = reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position;
            if (Value <= 0) {
                Text = aConst::LocalizedText(u"FormCfgSettings.ForsageTurnOffNever"_wref.get());
            } else if (Value < 100) {
                Text = ([&] {
                    auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString intToStr = pas::wide_int_to_str(100 - Value);
                    pas::WideString localizedText = aConst::LocalizedText(u"FormCfgSettings.ForsageTurnOffStep"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(localizedText), u"<Value>"_w, std::move(intToStr), textHighlightColorTag.get());
                }());
            } else {
                Text = aConst::LocalizedText(u"FormCfgSettings.ForsageTurnOffAlways"_wref.get());
            }
            {
                const pas::WideString& replaceColoredToken = aMyFunction::ReplaceColoredToken(ValueLabel->HelpText, u"<Text>"_w, Text, aMyFunction::TextHighlightColorTag);
                GI_Label::TLabelGI* valueLabel = ValueLabel;
                valueLabel->SetText(replaceColoredToken);
            }
        }
    }

    void TfCfgSettings::HighPresetClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t SavedGroup = ActiveGroupIndex;
        ActiveGroupIndex = 0;
        SetOptionValue(u"CountFilmSave"sv, 30);
        SetOptionValue(u"ScrollSpeed"sv, 20);
        SetOptionValue(u"ScrollSense"sv, 1);
        SetOptionValue(u"FilmSpeed"sv, 2);
        SetOptionValue(u"BeginCalcNextTurn"sv, 100);
        ActiveGroupIndex = 1;
        SetOptionValue(u"Resolution"sv, 2);
        SetOptionValue(u"Brightness"sv, 50);
        SetOptionValue(u"Contrast"sv, 50);
        SetOptionValue(u"Video"sv, 1);
        SetOptionValue(u"BGImage"sv, 1);
        SetOptionValue(u"AnimCaptain"sv, 1);
        SetOptionValue(u"AnimShip"sv, 1);
        SetOptionValue(u"AnimItem"sv, 1);
        SetOptionValue(u"AnimMenuShip"sv, 1);
        SetOptionValue(u"AnimGov"sv, 2);
        SetOptionValue(u"AnimHangar"sv, 1);
        SetOptionValue(u"AnimStar"sv, 1);
        SetOptionValue(u"SpaceImage"sv, 2);
        SetOptionValue(u"SputnikShow"sv, 1);
        SetOptionValue(u"CircleAction"sv, 0);
        SetOptionValue(u"Tail"sv, 2);
        SetOptionValue(u"Comet"sv, 2);
        SetOptionValue(u"Wind"sv, 2);
        SetOptionValue(u"PlanetClouds"sv, 1);
        SetOptionValue(u"PlanetAtm"sv, 1);
        SetOptionValue(u"AnimChangeForm"sv, 1);
        SetOptionValue(u"AnimMainFon"sv, 1);
        ActiveGroupIndex = 2;
        SetOptionValue(u"Sound"sv, 1);
        SetOptionValue(u"SoundInSpace"sv, 1);
        SetOptionValue(u"SoundVolume"sv, 100);
        SetOptionValue(u"Music"sv, 1);
        SetOptionValue(u"MusicInSpace"sv, 1);
        SetOptionValue(u"MusicVolume"sv, 75);
        ActiveGroupIndex = 4;
        SetOptionValue(u"RobotBrightness"sv, 50);
        SetOptionValue(u"RobotContrast"sv, 50);
        SetOptionValue(u"RobotShowStencilShadows"sv, 1);
        SetOptionValue(u"RobotShowProjShadows"sv, 1);
        SetOptionValue(u"RobotRobotShadow"sv, 1);
        SetOptionValue(u"RobotLandTexturesGloss"sv, 1);
        SetOptionValue(u"RobotObjTexturesGloss"sv, 1);
        SetOptionValue(u"RobotSky"sv, 2);
        ActiveGroupIndex = 5;
        SetOptionValue(u"RobotSound"sv, 1);
        SetOptionValue(u"RobotSoundVolume"sv, 100);
        SetOptionValue(u"RobotMusic"sv, 1);
        SetOptionValue(u"RobotMusicVolume"sv, 75);
        ActiveGroupIndex = SavedGroup;
        GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormCfgSettings.AutoMax"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
    }

    void TfCfgSettings::MediumPresetClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t SavedGroup = ActiveGroupIndex;
        ActiveGroupIndex = 0;
        SetOptionValue(u"CountFilmSave"sv, 20);
        SetOptionValue(u"ScrollSpeed"sv, 20);
        SetOptionValue(u"ScrollSense"sv, 1);
        SetOptionValue(u"FilmSpeed"sv, 1);
        SetOptionValue(u"BeginCalcNextTurn"sv, 100);
        ActiveGroupIndex = 1;
        SetOptionValue(u"Brightness"sv, 50);
        SetOptionValue(u"Contrast"sv, 50);
        SetOptionValue(u"Video"sv, 1);
        SetOptionValue(u"BGImage"sv, 1);
        SetOptionValue(u"AnimCaptain"sv, 1);
        SetOptionValue(u"AnimShip"sv, 1);
        SetOptionValue(u"AnimItem"sv, 1);
        SetOptionValue(u"AnimHangar"sv, 1);
        SetOptionValue(u"AnimMenuShip"sv, 1);
        SetOptionValue(u"AnimGov"sv, 1);
        SetOptionValue(u"AnimHangar"sv, 1);
        SetOptionValue(u"AnimStar"sv, 1);
        SetOptionValue(u"SpaceImage"sv, 1);
        SetOptionValue(u"SputnikShow"sv, 1);
        SetOptionValue(u"CircleAction"sv, 0);
        SetOptionValue(u"Tail"sv, 1);
        SetOptionValue(u"Comet"sv, 1);
        SetOptionValue(u"Wind"sv, 1);
        SetOptionValue(u"PlanetClouds"sv, 0);
        SetOptionValue(u"PlanetAtm"sv, 0);
        SetOptionValue(u"AnimChangeForm"sv, 0);
        SetOptionValue(u"AnimMainFon"sv, 0);
        ActiveGroupIndex = 2;
        SetOptionValue(u"Sound"sv, 1);
        SetOptionValue(u"SoundInSpace"sv, 1);
        SetOptionValue(u"SoundVolume"sv, 100);
        SetOptionValue(u"Music"sv, 1);
        SetOptionValue(u"MusicInSpace"sv, 1);
        SetOptionValue(u"MusicVolume"sv, 75);
        ActiveGroupIndex = 4;
        SetOptionValue(u"RobotBrightness"sv, 50);
        SetOptionValue(u"RobotContrast"sv, 50);
        SetOptionValue(u"RobotShowStencilShadows"sv, 0);
        SetOptionValue(u"RobotShowProjShadows"sv, 1);
        SetOptionValue(u"RobotRobotShadow"sv, 0);
        SetOptionValue(u"RobotLandTexturesGloss"sv, 0);
        SetOptionValue(u"RobotObjTexturesGloss"sv, 0);
        SetOptionValue(u"RobotSky"sv, 1);
        ActiveGroupIndex = 5;
        SetOptionValue(u"RobotSound"sv, 1);
        SetOptionValue(u"RobotSoundVolume"sv, 100);
        SetOptionValue(u"RobotMusic"sv, 1);
        SetOptionValue(u"RobotMusicVolume"sv, 75);
        ActiveGroupIndex = SavedGroup;
        GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormCfgSettings.AutoMiddle"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
    }

    void TfCfgSettings::LowPresetClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t SavedGroup = ActiveGroupIndex;
        ActiveGroupIndex = 0;
        SetOptionValue(u"CountFilmSave"sv, 1);
        SetOptionValue(u"ScrollSpeed"sv, 20);
        SetOptionValue(u"ScrollSense"sv, 1);
        SetOptionValue(u"FilmSpeed"sv, 0);
        SetOptionValue(u"BeginCalcNextTurn"sv, 0);
        ActiveGroupIndex = 1;
        SetOptionValue(u"Resolution"sv, 1);
        SetOptionValue(u"Brightness"sv, 50);
        SetOptionValue(u"Contrast"sv, 50);
        SetOptionValue(u"Video"sv, 0);
        SetOptionValue(u"BGImage"sv, 0);
        SetOptionValue(u"AnimCaptain"sv, 0);
        SetOptionValue(u"AnimShip"sv, 0);
        SetOptionValue(u"AnimItem"sv, 0);
        SetOptionValue(u"AnimMenuShip"sv, 0);
        SetOptionValue(u"AnimGov"sv, 0);
        SetOptionValue(u"AnimHangar"sv, 0);
        SetOptionValue(u"AnimStar"sv, 0);
        SetOptionValue(u"SpaceImage"sv, 0);
        SetOptionValue(u"SputnikShow"sv, 0);
        SetOptionValue(u"CircleAction"sv, 0);
        SetOptionValue(u"Tail"sv, 0);
        SetOptionValue(u"Comet"sv, 0);
        SetOptionValue(u"Wind"sv, 0);
        SetOptionValue(u"PlanetClouds"sv, 0);
        SetOptionValue(u"PlanetAtm"sv, 0);
        SetOptionValue(u"AnimChangeForm"sv, 0);
        SetOptionValue(u"AnimMainFon"sv, 0);
        ActiveGroupIndex = 2;
        SetOptionValue(u"Sound"sv, 0);
        SetOptionValue(u"SoundInSpace"sv, 0);
        SetOptionValue(u"SoundVolume"sv, 100);
        SetOptionValue(u"Music"sv, 0);
        SetOptionValue(u"MusicInSpace"sv, 0);
        SetOptionValue(u"MusicVolume"sv, 75);
        ActiveGroupIndex = 4;
        SetOptionValue(u"RobotBrightness"sv, 50);
        SetOptionValue(u"RobotContrast"sv, 50);
        SetOptionValue(u"RobotShowStencilShadows"sv, 0);
        SetOptionValue(u"RobotShowProjShadows"sv, 0);
        SetOptionValue(u"RobotRobotShadow"sv, 0);
        SetOptionValue(u"RobotLandTexturesGloss"sv, 0);
        SetOptionValue(u"RobotObjTexturesGloss"sv, 0);
        SetOptionValue(u"RobotSky"sv, 0);
        ActiveGroupIndex = 5;
        SetOptionValue(u"RobotSound"sv, 0);
        SetOptionValue(u"RobotSoundVolume"sv, 100);
        SetOptionValue(u"RobotMusic"sv, 0);
        SetOptionValue(u"RobotMusicVolume"sv, 75);
        ActiveGroupIndex = SavedGroup;
        GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormCfgSettings.AutoMin"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
    }

    void TfCfgSettings::AutoPresetClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t ClockMHz{};
        WindowsSdk::TMemoryStatus Memory{};
        WindowsSdk::TOSVersionInfo Version{};
        {
            double real_min = ([&] {
                double estimateCpuClockMHz = fCfgSettings::EstimateCpuClockMHz();
                double estimateCpuClockMHz_2 = fCfgSettings::EstimateCpuClockMHz();
                return pas::real_min<double>(estimateCpuClockMHz, estimateCpuClockMHz_2);
            }());
            ClockMHz = System::Round(pas::real_min<double>(real_min, fCfgSettings::EstimateCpuClockMHz()));
        }
        pas::fill_memory(&Memory, static_cast<std::int32_t>(sizeof(WindowsSdk::TMemoryStatus)), static_cast<std::uint8_t>(0));
        Memory.dwLength = static_cast<std::int32_t>(sizeof(WindowsSdk::TMemoryStatus));
        WindowsSdk::GlobalMemoryStatus(Memory);
        std::int32_t MemoryMB = Memory.dwTotalPhys >> 20;
        pas::fill_memory(&Version, static_cast<std::int32_t>(sizeof(WindowsSdk::TOSVersionInfo)), static_cast<std::uint8_t>(0));
        Version.dwOSVersionInfoSize = static_cast<std::int32_t>(sizeof(WindowsSdk::TOSVersionInfo));
        WindowsSdk::GetVersionEx(Version);
        std::uint8_t ModernWindows = Version.dwMajorVersion > 5 || Version.dwMajorVersion == 5 && Version.dwMinorVersion >= 1;
        std::int32_t SavedGroup = ActiveGroupIndex;
        ActiveGroupIndex = 0;
        SetOptionValue(u"CountFilmSave"sv, 30);
        SetOptionValue(u"ScrollSpeed"sv, 20);
        SetOptionValue(u"ScrollSense"sv, 1);
        if (ClockMHz < 1000) {
            SetOptionValue(u"FilmSpeed"sv, 0);
        } else if (ClockMHz < 2000) {
            SetOptionValue(u"FilmSpeed"sv, 1);
        } else {
            SetOptionValue(u"FilmSpeed"sv, 2);
        }
        SetOptionValue(u"BeginCalcNextTurn"sv, 100);
        ActiveGroupIndex = 1;
        SetOptionValue(u"Resolution"sv, 2);
        SetOptionValue(u"Brightness"sv, 50);
        SetOptionValue(u"Contrast"sv, 50);
        SetOptionValue(u"Video"sv, ClockMHz >= 500);
        SetOptionValue(u"BGImage"sv, 1);
        SetOptionValue(u"AnimCaptain"sv, ClockMHz >= 500);
        SetOptionValue(u"AnimShip"sv, 1);
        SetOptionValue(u"AnimItem"sv, ClockMHz >= 300);
        if (ClockMHz < 700 || MemoryMB < 200) {
            SetOptionValue(u"AnimGov"sv, 0);
        } else if (ClockMHz < 1500 || MemoryMB < 300 && ClockMHz < 2400) {
            SetOptionValue(u"AnimGov"sv, 1);
        } else {
            SetOptionValue(u"AnimGov"sv, 2);
        }
        if (ClockMHz < 1500 || MemoryMB < 300 && ClockMHz < 2400) {
            SetOptionValue(u"AnimMenuShip"sv, 0);
        } else {
            SetOptionValue(u"AnimMenuShip"sv, 1);
        }
        SetOptionValue(u"SoftwareCursor"sv, ModernWindows);
        SetOptionValue(u"AnimHangar"sv, ClockMHz >= 2000 || ClockMHz >= 1400 && MemoryMB > 500);
        SetOptionValue(u"AnimStar"sv, ClockMHz >= 500);
        if (ClockMHz < 500) {
            SetOptionValue(u"SpaceImage"sv, 0);
        } else if (ClockMHz < 1900) {
            SetOptionValue(u"SpaceImage"sv, 1);
        } else {
            SetOptionValue(u"SpaceImage"sv, 2);
        }
        SetOptionValue(u"SputnikShow"sv, ClockMHz >= 500);
        SetOptionValue(u"CircleAction"sv, 0);
        if (ClockMHz < 500) {
            SetOptionValue(u"Tail"sv, 0);
        } else if (ClockMHz < 1000) {
            SetOptionValue(u"Tail"sv, 1);
        } else {
            SetOptionValue(u"Tail"sv, 2);
        }
        if (ClockMHz < 500) {
            SetOptionValue(u"Comet"sv, 0);
        } else if (ClockMHz < 1000) {
            SetOptionValue(u"Comet"sv, 1);
        } else {
            SetOptionValue(u"Comet"sv, 2);
        }
        if (ClockMHz < 500) {
            SetOptionValue(u"Wind"sv, 0);
        } else if (ClockMHz < 1000) {
            SetOptionValue(u"Wind"sv, 1);
        } else {
            SetOptionValue(u"Wind"sv, 2);
        }
        SetOptionValue(u"PlanetClouds"sv, ClockMHz >= 1900);
        SetOptionValue(u"PlanetAtm"sv, ClockMHz >= 1900);
        SetOptionValue(u"AnimChangeForm"sv, ClockMHz >= 1900);
        SetOptionValue(u"AnimMainFon"sv, ClockMHz >= 1500);
        ActiveGroupIndex = 2;
        SetOptionValue(u"Sound"sv, ClockMHz >= 200);
        SetOptionValue(u"SoundInSpace"sv, ClockMHz >= 200);
        SetOptionValue(u"SoundVolume"sv, 100);
        SetOptionValue(u"Music"sv, ClockMHz >= 400);
        SetOptionValue(u"MusicInSpace"sv, ClockMHz >= 400);
        SetOptionValue(u"MusicVolume"sv, 75);
        ActiveGroupIndex = 4;
        SetOptionValue(u"RobotBrightness"sv, 50);
        SetOptionValue(u"RobotContrast"sv, 50);
        SetOptionValue(u"RobotShowStencilShadows"sv, ClockMHz >= 2400);
        SetOptionValue(u"RobotShowProjShadows"sv, ClockMHz >= 2000);
        SetOptionValue(u"RobotRobotShadow"sv, ClockMHz >= 2400);
        SetOptionValue(u"RobotLandTexturesGloss"sv, ClockMHz >= 2000);
        SetOptionValue(u"RobotObjTexturesGloss"sv, ClockMHz >= 2000);
        if (ClockMHz >= 2000) {
            SetOptionValue(u"RobotSky"sv, 2);
        } else if (ClockMHz >= 1000) {
            SetOptionValue(u"RobotSky"sv, 1);
        } else {
            SetOptionValue(u"RobotSky"sv, 0);
        }
        SetOptionValue(u"RobotSoftwareCursor"sv, static_cast<std::uint8_t>(ModernWindows ^ 1) & 127);
        ActiveGroupIndex = 5;
        SetOptionValue(u"RobotSound"sv, ClockMHz >= 700);
        SetOptionValue(u"RobotSoundVolume"sv, 100);
        SetOptionValue(u"RobotMusic"sv, ClockMHz >= 1500);
        SetOptionValue(u"RobotMusicVolume"sv, 75);
        ActiveGroupIndex = SavedGroup;
        GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormCfgSettings.Auto"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
    }

    void TfCfgSettings::CancelClicked(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::RequestedScreenId = GlobalsV::SettingsReturnScreenId;
        RequestClose(1);
    }

    void TfCfgSettings::RefreshModeUi() {
        if (SettingsMode == 0) {
            {
                GI_Label::TLabelGI* WarningMod = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"WarningMod"sv));
                WarningMod->SetText(u""_wref.get());
                if (ActiveGroupIndex == 3) {
                    WarningMod->SetText(aConst::LocalizedText(u"FormCfgSettings.WarningMod"_wref.get()));
                }
            }
            {
                GI_Label::TLabelGI* Warning = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Warning"sv));
                Warning->SetText(u""_wref.get());
            }
            {
                GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
                ButGroup0->SetActive(true);
                ButGroup0->SetPosition(ClassesImports::Point(ButGroup0->LocalPosition.X, GroupButtonTops[0]));
            }
            {
                GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
                ButGroup1->SetPosition(ClassesImports::Point(ButGroup1->LocalPosition.X, GroupButtonTops[1]));
            }
            {
                GI_GraphButton::TGraphButtonGI* ButGroup2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
                ButGroup2->SetPosition(ClassesImports::Point(ButGroup2->LocalPosition.X, GroupButtonTops[2]));
            }
            {
                GI_GraphButton::TGraphButtonGI* ButGroup3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup3"sv));
                ButGroup3->SetActive(false);
                ButGroup3->SetPosition(ClassesImports::Point(ButGroup3->LocalPosition.X, GroupButtonTops[3]));
            }
            GetByName(u"ModeLeft"sv)->SetActive(true);
            GetByName(u"ModeRight"sv)->SetActive(false);
            GetByName(u"ModeLeftPanel"sv)->SetDepth(-1.0);
            GetByName(u"ModeRightPanel"sv)->SetDepth(-3.0);
            GetByName(u"ModeLeftSmall"sv)->SetActive(true);
            GetByName(u"ModeRightSmall"sv)->SetActive(false);
            GetByName(u"ModeLeftButtonN"sv)->SetActive(ModeButtonState < 2);
            GetByName(u"ModeLeftButtonD"sv)->SetActive(ModeButtonState >= 2);
            GetByName(u"ModeRightButtonN"sv)->SetActive(true);
            GetByName(u"ModeRightButtonD"sv)->SetActive(false);
            if (ModeButtonState != 0) {
                GetByName(u"ModeLeftPanel"sv)->SetPosition(ModeLeftPosition);
            } else {
                GetByName(u"ModeLeftPanel"sv)->SetPosition(EC_Struct::AddPoints(ModeLeftPosition, ClassesImports::Point(-1, 4)));
            }
            {
                GI_Label::TLabelGI* ModeLeftName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ModeLeftName"sv));
                if (ModeButtonState == 1) {
                    ModeLeftName->SetTextColor(SettingsModeColorHighlighted);
                    ModeLeftName->SetShadowOffset(0);
                } else {
                    ModeLeftName->SetTextColor(SettingsModeColorNormal);
                    ModeLeftName->SetShadowOffset(1);
                }
            }
            {
                GI_Label::TLabelGI* ModeRightName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ModeRightName"sv));
                ModeRightName->SetTextColor(SettingsModeColorNormal);
                ModeRightName->SetShadowOffset(1);
            }
            GetByName(u"ModeRightPanel"sv)->SetPosition(ModeRightPosition);
        } else {
            {
                GI_Label::TLabelGI* WarningMod_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"WarningMod"sv));
                WarningMod_2->SetText(u""_wref.get());
            }
            {
                GI_Label::TLabelGI* Warning_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Warning"sv));
                Warning_2->SetText(u""_wref.get());
                Warning_2->CreateEmbeddedControl = pas::bind_static_method<&TfCfgSettings::CreateWarningImage>(this);
                if (RobotAvailability == 1) {
                    Warning_2->SetText(pas::concat_wide_reverse({aConst::LocalizedText(u"FormCfgSettings.WarningNoDX9"_wref.get()), static_cast<pas::WideString>(pas::concat_ansi({"<Object=0,", SysUtils::IntToStr(GR_Main::GiScalePixels(26)), ",", SysUtils::IntToStr(GR_Main::GiScalePixelsEx(24, 18)), ",0>"}))}));
                } else if (RobotAvailability == 2) {
                    Warning_2->SetText(pas::concat_wide_reverse({aConst::LocalizedText(u"FormCfgSettings.WarningDriverOld"_wref.get()), static_cast<pas::WideString>(pas::concat_ansi({"<Object=0,", SysUtils::IntToStr(GR_Main::GiScalePixels(26)), ",", SysUtils::IntToStr(GR_Main::GiScalePixelsEx(24, 18)), ",0>"}))}));
                } else if (RobotAvailability == 3) {
                    Warning_2->SetText(pas::concat_wide_reverse({aConst::LocalizedText(u"FormCfgSettings.WarningVideoUnsupported"_wref.get()), static_cast<pas::WideString>(pas::concat_ansi({"<Object=0,", SysUtils::IntToStr(GR_Main::GiScalePixels(26)), ",", SysUtils::IntToStr(GR_Main::GiScalePixelsEx(24, 18)), ",0>"}))}));
                } else if (RobotAvailability == 4) {
                    Warning_2->SetText(pas::concat_wide_reverse({aConst::LocalizedText(u"FormCfgSettings.WarningNoInstall"_wref.get()), static_cast<pas::WideString>(pas::concat_ansi({"<Object=0,", SysUtils::IntToStr(GR_Main::GiScalePixels(26)), ",", SysUtils::IntToStr(GR_Main::GiScalePixelsEx(24, 18)), ",0>"}))}));
                }
            }
            {
                GI_GraphButton::TGraphButtonGI* ButGroup0_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
                ButGroup0_2->SetActive(false);
            }
            {
                GI_GraphButton::TGraphButtonGI* ButGroup1_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
                ButGroup1_2->SetPosition(ClassesImports::Point(ButGroup1_2->LocalPosition.X, GroupButtonTops[0]));
            }
            {
                GI_GraphButton::TGraphButtonGI* ButGroup2_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
                ButGroup2_2->SetPosition(ClassesImports::Point(ButGroup2_2->LocalPosition.X, GroupButtonTops[1]));
            }
            {
                GI_GraphButton::TGraphButtonGI* ButGroup3_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup3"sv));
                ButGroup3_2->SetActive(false);
            }
            GetByName(u"ModeLeft"sv)->SetActive(false);
            GetByName(u"ModeRight"sv)->SetActive(true);
            GetByName(u"ModeLeftPanel"sv)->SetDepth(-3.0);
            GetByName(u"ModeRightPanel"sv)->SetDepth(-1.0);
            GetByName(u"ModeLeftSmall"sv)->SetActive(false);
            GetByName(u"ModeRightSmall"sv)->SetActive(true);
            GetByName(u"ModeLeftButtonN"sv)->SetActive(true);
            GetByName(u"ModeLeftButtonD"sv)->SetActive(false);
            GetByName(u"ModeRightButtonN"sv)->SetActive(ModeButtonState < 2);
            GetByName(u"ModeRightButtonD"sv)->SetActive(ModeButtonState >= 2);
            if (ModeButtonState != 0) {
                GetByName(u"ModeRightPanel"sv)->SetPosition(ModeRightPosition);
            } else {
                GetByName(u"ModeRightPanel"sv)->SetPosition(EC_Struct::AddPoints(ModeRightPosition, ClassesImports::Point(1, 4)));
            }
            {
                GI_Label::TLabelGI* ModeRightName_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ModeRightName"sv));
                if (ModeButtonState == 1) {
                    ModeRightName_2->SetTextColor(SettingsModeColorHighlighted);
                    ModeRightName_2->SetShadowOffset(0);
                } else {
                    ModeRightName_2->SetTextColor(SettingsModeColorNormal);
                    ModeRightName_2->SetShadowOffset(1);
                }
            }
            {
                GI_Label::TLabelGI* ModeLeftName_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ModeLeftName"sv));
                ModeLeftName_2->SetTextColor(SettingsModeColorNormal);
                ModeLeftName_2->SetShadowOffset(1);
            }
            GetByName(u"ModeLeftPanel"sv)->SetPosition(ModeLeftPosition);
        }
    }

    void TfCfgSettings::ModeMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (ModeLeaveTimer != nullptr) {
            CancelCallbackTimer(ModeLeaveTimer);
            ModeLeaveTimer = nullptr;
        }
        if (Sender->UserValue == SettingsMode) {
            if (ModeButtonState != 1) {
                GR_Main::SoundManager->PlaySound(u"Sound.ButtonEnter"_wref.get());
            }
            ModeButtonState = 1;
        }
        RefreshModeUi();
    }

    void TfCfgSettings::ModeMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (ModeLeaveTimer != nullptr) {
            CancelCallbackTimer(ModeLeaveTimer);
            ModeLeaveTimer = nullptr;
        }
        ModeLeaveTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfCfgSettings::ModeLeaveTimerTick>(this), Sender->UserValue);
    }

    void TfCfgSettings::ModeLeaveTimerTick(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (ModeLeaveTimer != nullptr) {
            CancelCallbackTimer(ModeLeaveTimer);
            ModeLeaveTimer = nullptr;
        }
        if (UserData == SettingsMode) {
            if (ModeButtonState != 0) {
                GR_Main::SoundManager->PlaySound(u"Sound.ButtonLeave"_wref.get());
            }
            ModeButtonState = 0;
        }
        RefreshModeUi();
    }

    void TfCfgSettings::ModeMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->UserValue == SettingsMode) {
            if (ModeButtonState != 2) {
                GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
            }
            ModeButtonState = 2;
        }
        RefreshModeUi();
    }

    void TfCfgSettings::ModeMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->UserValue == SettingsMode) {
            if (SettingsMode == 0) {
                SettingsMode = 1;
            } else {
                SettingsMode = 0;
            }
            ModeButtonState = 0;
        }
        RefreshModeUi();
        if (SettingsMode == 0) {
            ActiveGroupIndex = 0;
            RefreshVisibleGroup();
        } else {
            ActiveGroupIndex = 4;
            RefreshVisibleGroup();
        }
    }

    void TfCfgSettings::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_ESCAPE) {
            CancelClicked(nullptr);
        } else if (Key == WindowsSdk::VK_RETURN) {
            ApplyClicked(nullptr);
        }
    }

    void TfCfgSettings::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"sv));
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            PanelSet->VerticalScrollBar->SetPosition_2(PanelSet->VerticalScrollBar->Position - PanelSet->VerticalScrollBar->SmallChange);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            PanelSet->VerticalScrollBar->SetPosition_2(PanelSet->VerticalScrollBar->Position + PanelSet->VerticalScrollBar->SmallChange);
        }
    }

    // Persists CFG.TXT; changes requiring rebuilt resources request another runtime session.
    void TfCfgSettings::ApplyClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString cpp_text{};
        pas::WideString Text{};
        GR_Sound::TSoundBuffer* Buffer{};
        pas::WideString Language{};
        pas::TextFile LanguageFile{};
        std::uint8_t RestartNeeded = false;
        std::uint8_t ResetNeeded = false;
        ActiveGroupIndex = 0;
        if (static_cast<std::uint8_t>(SimpleSteamApi::SteamInitialized ^ 1) && GR_Main::RequestedLanguage == u"") {
            Language = EC_Str::ExtractDelimitedPartW(pas::view(GR_Main::AvailableLanguageCodes), GetOptionValue(u"Lang"sv), u","sv);
            if (GR_Main::SelectedLanguage != Language) {
                GR_Main::SelectedLanguage = std::move(Language);
                Text = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Lang.txt"});
                pas::text_assign(LanguageFile, static_cast<pas::AnsiString>(Text), false);
                pas::text_open(LanguageFile, 3, false);
                pas::text_write(LanguageFile, "Lang="_a, false);
                pas::text_writeln(LanguageFile, static_cast<pas::AnsiString>(GR_Main::SelectedLanguage), false);
                pas::text_close(LanguageFile, false);
                RestartNeeded = true;
                if (GR_Main::LanguageInstallConfig != nullptr) {
                    pas::free(GR_Main::LanguageInstallConfig);
                    GR_Main::LanguageInstallConfig = nullptr;
                }
                if (!SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"install_", GR_Main::SelectedLanguage, u".txt"})))) {
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Not installed language: ", GR_Main::SelectedLanguage}))));
                } else {
                    GR_Main::LanguageInstallConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    {
                        char16_t* cpp_arg = (cpp_text = pas::concat_wide({u"install_", GR_Main::SelectedLanguage, u".txt"}), cpp_text.pchar());
                        EC_BlockPar::TBlockParEC* languageInstallConfig = GR_Main::LanguageInstallConfig;
                        languageInstallConfig->LoadFromTextFileWithEncodingProbe(cpp_arg, false);
                    }
                }
                Globals::ReloadModsRequested = true;
            }
        }
        GlobalsV::MultiThreadEnabled = GetOptionValue(u"MultiThread"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"MultiThread"_wref.get(), EC_Str::BoolToWideString(GlobalsV::MultiThreadEnabled));
        GlobalsV::DefaultOrder = GetOptionValue(u"DefaultOrder"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"DefaultOrder"_wref.get(), pas::wide_int_to_str(GlobalsV::DefaultOrder));
        GlobalsV::RightClickOnShip = GetOptionValue(u"RightClickOnShip"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RightClickOnShip"_wref.get(), pas::wide_int_to_str(GlobalsV::RightClickOnShip));
        GlobalsV::ViewFollowShip = GetOptionValue(u"ViewFollowShip"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ViewFollowShip"_wref.get(), EC_Str::BoolToWideString(GlobalsV::ViewFollowShip));
        GlobalsV::ViewPathLength = GetOptionValue(u"ViewPathLength"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ViewPathLength"_wref.get(), EC_Str::BoolToWideString(GlobalsV::ViewPathLength));
        GlobalsV::ActionDoubleClick = GetOptionValue(u"ActionDoubleClick"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ActionDoubleClick"_wref.get(), EC_Str::BoolToWideString(GlobalsV::ActionDoubleClick));
        GlobalsV::ClickAutoCloseForm = GetOptionValue(u"ClickAutoCloseForm"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ClickAutoCloseForm"_wref.get(), EC_Str::BoolToWideString(GlobalsV::ClickAutoCloseForm));
        GlobalsV::TurnSaveStep = GetOptionValue(u"TurnSaveStep"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"TurnSaveStep"_wref.get(), pas::wide_int_to_str(GlobalsV::TurnSaveStep));
        GlobalsV::QuickSaveExtraSlots = GetOptionValue(u"QuickSaveExtraSlots"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"QuickSaveExtraSlots"_wref.get(), pas::wide_int_to_str(GlobalsV::QuickSaveExtraSlots));
        GlobalsV::FilmHistoryLimit = GetOptionValue(u"CountFilmSave"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"CountFilmSave"_wref.get(), pas::wide_int_to_str(GlobalsV::FilmHistoryLimit));
        GlobalsV::ScrollStep = GetOptionValue(u"ScrollSpeed"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ScrollStep"_wref.get(), pas::wide_int_to_str(GlobalsV::ScrollStep));
        GlobalsV::MaxPlayerNews = GetOptionValue(u"MaxPlayerNews"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"MaxPlayerNews"_wref.get(), pas::wide_int_to_str(GlobalsV::MaxPlayerNews));
        GlobalsV::MaxSearchResult = GetOptionValue(u"MaxSearchResult"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"MaxSearchResult"_wref.get(), pas::wide_int_to_str(GlobalsV::MaxSearchResult));
        GlobalsV::AfterburnerStopCondition = GetOptionValue(u"ForsageDeactivatePercent"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ForsageDeactivatePercent"_wref.get(), pas::wide_int_to_str(GlobalsV::AfterburnerStopCondition));
        GlobalsV::FilmSpeed = GetOptionValue(u"FilmSpeed"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"FilmSpeed"_wref.get(), pas::wide_int_to_str(GlobalsV::FilmSpeed));
        GlobalsV::ChangeAutoPilot = GetOptionValue(u"ChangeAutoPilot"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ChangeAutoPilot"_wref.get(), pas::wide_int_to_str(GlobalsV::ChangeAutoPilot));
        GlobalsV::DisableAutoPilot = GetOptionValue(u"DisableAutoPilot"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"DisableAutoPilot"_wref.get(), EC_Str::BoolToWideString(GlobalsV::DisableAutoPilot));
        GlobalsV::BeginCalcNextTurn = pas::real_divide(GetOptionValue(u"BeginCalcNextTurn"sv), 1.0E+2L);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"BeginCalcNextTurn"_wref.get(), pas::wide_int64_to_str(System::Round(GlobalsV::BeginCalcNextTurn * 1.0E+2L)));
        ActiveGroupIndex = 1;
        std::int32_t Index = GetOptionValue(u"Resolution"sv);
        if (GR_Main::SelectedGameDisplayMode != Index) {
            if (GR_Main::GameDisplayModes[Index].Width == 0) {
                if (GR_Main::GameDisplayModes[GR_Main::SelectedGameDisplayMode].Width != GR_Main::DesktopDisplayMode.Width || GR_Main::GameDisplayModes[GR_Main::SelectedGameDisplayMode].Height != GR_Main::DesktopDisplayMode.Height) {
                    RestartNeeded = true;
                }
            } else if (GR_Main::GameDisplayModes[Index].Width != GR_Main::GameDisplayModes[GR_Main::SelectedGameDisplayMode].Width || GR_Main::GameDisplayModes[Index].Height != GR_Main::GameDisplayModes[GR_Main::SelectedGameDisplayMode].Height) {
                RestartNeeded = true;
            }
            GR_Main::SelectedGameDisplayMode = Index;
        }
        if (Index >= 0) {
            Text = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(GR_Main::GameDisplayModes[Index].Width), ",", SysUtils::Int64ToStr(GR_Main::GameDisplayModes[Index].Height)}));
            if (GR_Main::RequestedRefreshRate > 0) {
                Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(GR_Main::RequestedRefreshRate)});
            }
            GR_Main::UserSettingsConfig->SetOrAddParam(u"VideoMode"_wref.get(), Text);
        }
        GR_Main::DisplayBrightness = pas::real_divide(GetOptionValue(u"Brightness"sv) - 50, 5.0E+1L);
        GR_Main::DisplayContrast = pas::real_divide(GetOptionValue(u"Contrast"sv) - 50, 5.0E+1L);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"Brightness"_wref.get(), static_cast<pas::WideString>(pas::format("%.2f"_a, GR_Main::DisplayBrightness)));
        GR_Main::UserSettingsConfig->SetOrAddParam(u"Contrast"_wref.get(), static_cast<pas::WideString>(pas::format("%.2f"_a, GR_Main::DisplayContrast)));
        GR_Main::ApplyGammaRamp(GR_Main::DisplayBrightness, GR_Main::DisplayContrast);
        if (static_cast<std::uint8_t>(GetOptionValue(u"VSync"sv)) != GR_Main::VSyncEnabled) {
            ResetNeeded = true;
            GR_Main::VSyncEnabled = static_cast<std::uint8_t>(GR_Main::VSyncEnabled ^ 1);
            GR_Main::UserSettingsConfig->SetOrAddParam(u"VSync"_wref.get(), EC_Str::BoolToWideString(GR_Main::VSyncEnabled));
        }
        if (static_cast<std::uint8_t>(GetOptionValue(u"Window"sv)) != GR_Main::WindowedModeRequested) {
            ResetNeeded = true;
            GR_Main::WindowedModeRequested = GetOptionValue(u"Window"sv);
            GR_Main::UserSettingsConfig->SetOrAddParam(u"Window"_wref.get(), EC_Str::BoolToWideString(GR_Main::WindowedModeRequested));
        }
        if (GR_Main::AlternateViewportEnabled && static_cast<std::uint8_t>(GetOptionValue(u"RenderMode"sv)) != GlobalsV::ScaleViewportToWindow) {
            RestartNeeded = true;
            GlobalsV::ScaleViewportToWindow = GetOptionValue(u"RenderMode"sv);
            GR_Main::UserSettingsConfig->SetOrAddParam(u"RenderModeScale"_wref.get(), EC_Str::BoolToWideString(GlobalsV::ScaleViewportToWindow));
        }
        if (static_cast<std::uint8_t>(GR_Main::AlternateViewportEnabled ^ 1) && static_cast<std::uint8_t>(GetOptionValue(u"HardwareRender"sv)) != GlobalsV::HardwareRenderingRequested) {
            GlobalsV::HardwareRenderingRequested = static_cast<std::uint8_t>(GlobalsV::HardwareRenderingRequested ^ 1);
            GR_Main::UserSettingsConfig->SetOrAddParam(u"HardwareRender"_wref.get(), EC_Str::BoolToWideString(GlobalsV::HardwareRenderingRequested));
            GlobalsV::HardwareRenderingEnabled = GlobalsV::HardwareRenderingRequested && (static_cast<std::uint8_t>(GlobalsV::RunningUnderWine ^ 1) || GR_Main::UserSettingsConfig->CountParams(u"AllowHardwareRenderUnderWine"_wref.get()) != 0 && GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AllowHardwareRenderUnderWine"_wref.get())))));
        }
        GR_Main::ShowSystemMouse = GetOptionValue(u"SoftwareCursor"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ShowSystemMouse"_wref.get(), EC_Str::BoolToWideString(GR_Main::ShowSystemMouse));
        Globals::SkipIntro = static_cast<std::uint8_t>(static_cast<std::uint8_t>(GetOptionValue(u"Intro"sv)) ^ 1);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"SkipIntro"_wref.get(), EC_Str::BoolToWideString(Globals::SkipIntro));
        Globals::SkipVideo = static_cast<std::uint8_t>(static_cast<std::uint8_t>(GetOptionValue(u"Video"sv)) ^ 1);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"SkipVideo"_wref.get(), EC_Str::BoolToWideString(Globals::SkipVideo));
        GlobalsV::BGImage = GetOptionValue(u"BGImage"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"BGImage"_wref.get(), EC_Str::BoolToWideString(GlobalsV::BGImage));
        GlobalsV::AnimCaptain = GetOptionValue(u"AnimCaptain"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimCaptain"_wref.get(), EC_Str::BoolToWideString(GlobalsV::AnimCaptain));
        GlobalsV::AnimShipFull = GetOptionValue(u"AnimShip"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimShipFull"_wref.get(), EC_Str::BoolToWideString(GlobalsV::AnimShipFull));
        GlobalsV::AnimItem = GetOptionValue(u"AnimItem"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimItem"_wref.get(), EC_Str::BoolToWideString(GlobalsV::AnimItem));
        GlobalsV::AnimMenuShip = GetOptionValue(u"AnimMenuShip"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimMenuShip"_wref.get(), EC_Str::BoolToWideString(GlobalsV::AnimMenuShip));
        GlobalsV::AnimGov = GetOptionValue(u"AnimGov"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimGov"_wref.get(), pas::wide_int_to_str(GlobalsV::AnimGov));
        GlobalsV::AnimHangar = GetOptionValue(u"AnimHangar"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimHangar"_wref.get(), EC_Str::BoolToWideString(GlobalsV::AnimHangar));
        GlobalsV::AnimStar = GetOptionValue(u"AnimStar"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimStar"_wref.get(), EC_Str::BoolToWideString(GlobalsV::AnimStar));
        if (GetOptionValue(u"SpaceImage"sv) != GlobalsV::SpaceImage) {
            GlobalsV::SpaceImage = GetOptionValue(u"SpaceImage"sv);
            if (aGalaxy::Galaxy != nullptr && GlobalsV::SpaceImage > 0) {
                aGalaxy::Galaxy->GenerateSpaceBackground(aGalaxy::PlayerStar->BackgroundImage);
            }
            GR_Main::UserSettingsConfig->SetOrAddParam(u"SpaceImage"_wref.get(), pas::wide_int_to_str(GlobalsV::SpaceImage));
        }
        GlobalsV::SputnikShow = GetOptionValue(u"SputnikShow"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"SputnikShow"_wref.get(), EC_Str::BoolToWideString(GlobalsV::SputnikShow));
        GlobalsV::CircleAction = GetOptionValue(u"CircleAction"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"CircleAction"_wref.get(), EC_Str::BoolToWideString(GlobalsV::CircleAction));
        GlobalsV::ShipTail = GetOptionValue(u"Tail"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ShipTail"_wref.get(), pas::wide_int_to_str(GlobalsV::ShipTail));
        GlobalsV::Comet = GetOptionValue(u"Comet"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"Comet"_wref.get(), pas::wide_int_to_str(GlobalsV::Comet));
        GlobalsV::Wind = GetOptionValue(u"Wind"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"Wind"_wref.get(), pas::wide_int_to_str(GlobalsV::Wind));
        GlobalsV::BackgroundShade = GetOptionValue(u"BackgroundShade"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"BackgroundShade"_wref.get(), EC_Str::BoolToWideString(GlobalsV::BackgroundShade));
        GlobalsV::BackgroundGrayscale = GetOptionValue(u"BackgroundGrayscale"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"BackgroundGrayscale"_wref.get(), EC_Str::BoolToWideString(GlobalsV::BackgroundGrayscale));
        GlobalsV::PlanetClouds = GetOptionValue(u"PlanetClouds"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"PlanetClouds"_wref.get(), EC_Str::BoolToWideString(GlobalsV::PlanetClouds));
        GlobalsV::PlanetAtm = GetOptionValue(u"PlanetAtm"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"PlanetAtm"_wref.get(), EC_Str::BoolToWideString(GlobalsV::PlanetAtm));
        GlobalsV::AnimChangeForm = GetOptionValue(u"AnimChangeForm"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimChangeForm"_wref.get(), EC_Str::BoolToWideString(GlobalsV::AnimChangeForm));
        GlobalsV::AnimMainFon = GetOptionValue(u"AnimMainFon"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"AnimMainFon"_wref.get(), EC_Str::BoolToWideString(GlobalsV::AnimMainFon));
        GR_Main::ShowFrameRate = GetOptionValue(u"ShowFPS"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ShowFPS"_wref.get(), EC_Str::BoolToWideString(GR_Main::ShowFrameRate));
        GlobalsV::GalaxyMapFontChoice = static_cast<GlobalsV::TGalaxyMapFontChoice>(GetOptionValue(u"FontGalaxy"sv));
        GR_Main::UserSettingsConfig->SetOrAddParam(u"FontGalaxy"_wref.get(), pas::wide_int_to_str(static_cast<std::int32_t>(GlobalsV::GalaxyMapFontChoice)));
        GlobalsV::DynamicTipsPos = GetOptionValue(u"DynamicTipsPos"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"DynamicTipsPos"_wref.get(), EC_Str::BoolToWideString(GlobalsV::DynamicTipsPos));
        if (static_cast<std::uint8_t>(GetOptionValue(u"UseTablesForGov"sv)) != GlobalsV::UseTablesForGov) {
            RestartNeeded = true;
            GlobalsV::UseTablesForGov = GetOptionValue(u"UseTablesForGov"sv);
            GR_Main::UserSettingsConfig->SetOrAddParam(u"UseTablesForGov"_wref.get(), EC_Str::BoolToWideString(GlobalsV::UseTablesForGov));
        }
        GlobalsV::FontSmoothingEnabled = GetOptionValue(u"FontSmooth"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"FontSmooth"_wref.get(), EC_Str::BoolToWideString(GlobalsV::FontSmoothingEnabled));
        GlobalsV::FontDialog = GetOptionValue(u"FontDialog"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"FontDialog"_wref.get(), pas::wide_int_to_str(GlobalsV::FontDialog));
        GlobalsV::FontQuest = GetOptionValue(u"FontQuest"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"FontQuest"_wref.get(), pas::wide_int_to_str(GlobalsV::FontQuest));
        GlobalsV::ScreenshotFormat = GetOptionValue(u"ScreenShotType"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ScreenShotType"_wref.get(), pas::wide_int_to_str(GlobalsV::ScreenshotFormat));
        GlobalsV::ScreenshotJpegQuality = GetOptionValue(u"ScreenShotQuality"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"ScreenShotQuality"_wref.get(), pas::wide_int_to_str(GlobalsV::ScreenshotJpegQuality));
        ActiveGroupIndex = 2;
        if (static_cast<std::uint8_t>(GetOptionValue(u"Sound"sv)) != GlobalsV::SoundEnabled) {
            GR_Main::UserSettingsConfig->SetOrAddParam(u"Sound"_wref.get(), EC_Str::BoolToWideString(static_cast<std::uint8_t>(GlobalsV::SoundEnabled ^ 1)));
            RestartNeeded = true;
        }
        GlobalsV::SoundInSpaceEnabled = GetOptionValue(u"SoundInSpace"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"SoundInSpace"_wref.get(), EC_Str::BoolToWideString(GlobalsV::SoundInSpaceEnabled));
        GlobalsV::SoundVolume = pas::real_divide(GetOptionValue(u"SoundVolume"sv), 1.0E+2L);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"SoundVolume"_wref.get(), pas::wide_int_to_str(GetOptionValue(u"SoundVolume"sv)));
        if (static_cast<std::uint8_t>(GetOptionValue(u"Music"sv)) != GlobalsV::MusicEnabled) {
            GR_Main::UserSettingsConfig->SetOrAddParam(u"Music"_wref.get(), EC_Str::BoolToWideString(static_cast<std::uint8_t>(GlobalsV::MusicEnabled ^ 1)));
            RestartNeeded = true;
        }
        GlobalsV::MusicInSpaceEnabled = GetOptionValue(u"MusicInSpace"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"MusicInSpace"_wref.get(), EC_Str::BoolToWideString(GlobalsV::MusicInSpaceEnabled));
        GlobalsV::MusicVolume = pas::real_divide(GetOptionValue(u"MusicVolume"sv), 1.0E+2L);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"MusicVolume"_wref.get(), pas::wide_int_to_str(GetOptionValue(u"MusicVolume"sv)));
        GlobalsV::MusicInHyperEnabled = GetOptionValue(u"MusicInHyper"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"MusicInHyper"_wref.get(), EC_Str::BoolToWideString(GlobalsV::MusicInHyperEnabled));
        GlobalsV::MusicInPlanetEnabled = GetOptionValue(u"MusicInPlanet"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"MusicInPlanet"_wref.get(), EC_Str::BoolToWideString(GlobalsV::MusicInPlanetEnabled));
        ActiveGroupIndex = 4;
        GR_Main::SelectedRobotDisplayMode = GetOptionValue(u"RobotResolution"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotResolution"_wref.get(), static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(GR_Main::RobotDisplayModes[GR_Main::SelectedRobotDisplayMode].Width), ",", SysUtils::Int64ToStr(GR_Main::RobotDisplayModes[GR_Main::SelectedRobotDisplayMode].Height)})));
        Robot::RobotVSync = GetOptionValue(u"RobotVSync"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotVSync"_wref.get(), EC_Str::BoolToWideString(Robot::RobotVSync));
        if (Robot::SupportedMultiSampleCount > 1) {
            Robot::RobotFSAASamples = Robot::SupportedMultiSamples[GetOptionValue(u"RobotFSAASamples"sv)];
            GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotFSAASamples"_wref.get(), pas::wide_int_to_str(Robot::RobotFSAASamples));
        }
        if (static_cast<std::int32_t>(Robot::MaximumAnisotropy) > 0) {
            Robot::RobotAnisotropy = GetOptionValue(u"RobotAnisotropy"sv);
            GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotAnisotropy"_wref.get(), pas::wide_int_to_str(Robot::RobotAnisotropy));
        }
        Robot::RobotMaxDistance = GetOptionValue(u"RobotMaxDistance"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotMaxDistance"_wref.get(), pas::wide_int_to_str(Robot::RobotMaxDistance));
        GR_Main::RobotBrightness = pas::real_divide(GetOptionValue(u"RobotBrightness"sv) - 50, 5.0E+1L);
        GR_Main::RobotContrast = pas::real_divide(GetOptionValue(u"RobotContrast"sv) - 50, 5.0E+1L);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotBrightness"_wref.get(), static_cast<pas::WideString>(pas::format("%.2f"_a, GR_Main::RobotBrightness)));
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotContrast"_wref.get(), static_cast<pas::WideString>(pas::format("%.2f"_a, GR_Main::RobotContrast)));
        Robot::RobotSettings.ShowStencilShadows = GetOptionValue(u"RobotShowStencilShadows"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotShowStencilShadows"_wref.get(), EC_Str::BoolToWideString(Robot::RobotSettings.ShowStencilShadows));
        Robot::RobotSettings.ShowProjShadows = GetOptionValue(u"RobotShowProjShadows"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotShowProjShadows"_wref.get(), EC_Str::BoolToWideString(Robot::RobotSettings.ShowProjShadows));
        Robot::RobotSettings.RobotShadow = GetOptionValue(u"RobotRobotShadow"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotRobotShadow"_wref.get(), pas::wide_int_to_str(static_cast<std::int32_t>(Robot::RobotSettings.RobotShadow)));
        Robot::RobotSettings.SelectEx = GetOptionValue(u"RobotSelectEx"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotSelectEx"_wref.get(), EC_Str::BoolToWideString(Robot::RobotSettings.SelectEx));
        Robot::RobotSettings.LandTexturesGloss = GetOptionValue(u"RobotLandTexturesGloss"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotLandTexturesGloss"_wref.get(), EC_Str::BoolToWideString(Robot::RobotSettings.LandTexturesGloss));
        Robot::RobotSettings.ObjTexturesGloss = GetOptionValue(u"RobotObjTexturesGloss"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotObjTexturesGloss"_wref.get(), EC_Str::BoolToWideString(Robot::RobotSettings.ObjTexturesGloss));
        Robot::RobotSettings.SoftwareCursor = GetOptionValue(u"RobotSoftwareCursor"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotSoftwareCursor"_wref.get(), EC_Str::BoolToWideString(Robot::RobotSettings.SoftwareCursor));
        Robot::RobotSettings.Sky = GetOptionValue(u"RobotSky"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotSky"_wref.get(), pas::wide_int_to_str(static_cast<std::int32_t>(Robot::RobotSettings.Sky)));
        ActiveGroupIndex = 5;
        Robot::RobotMusic = GetOptionValue(u"RobotMusic"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotMusic"_wref.get(), EC_Str::BoolToWideString(Robot::RobotMusic));
        GlobalsV::RobotMusicVolume = pas::real_divide(GetOptionValue(u"RobotMusicVolume"sv), 1.0E+2L);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotMusicVolume"_wref.get(), pas::wide_int_to_str(GetOptionValue(u"RobotMusicVolume"sv)));
        Robot::RobotSound = GetOptionValue(u"RobotSound"sv);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotSound"_wref.get(), EC_Str::BoolToWideString(Robot::RobotSound));
        GlobalsV::RobotSoundVolume = pas::real_divide(GetOptionValue(u"RobotSoundVolume"sv), 1.0E+2L);
        GR_Main::UserSettingsConfig->SetOrAddParam(u"RobotSoundVolume"_wref.get(), pas::wide_int_to_str(GetOptionValue(u"RobotSoundVolume"sv)));
        ActiveGroupIndex = 3;
        Text = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"cfg.txt"});
        GR_Main::UserSettingsConfig->SaveTextFile(Text.pchar(), true, false);
        Globals::StarMapScreen->ConfigureMiddleButtonAction();
        if (GR_Main::SoundManager != nullptr) {
            Buffer = GR_Main::SoundManager->FirstBuffer;
            while (Buffer != nullptr) {
                if (Buffer->Streaming) {
                    Buffer->SetVolume(static_cast<long double>(GlobalsV::MusicVolume) * GlobalsV::MusicVolumeScale);
                } else {
                    Buffer->SetVolume(GlobalsV::SoundVolume);
                }
                Buffer = Buffer->Next;
            }
        }
        if (GR_Main::ShowSystemMouse) {
            while (WindowsSdk::ShowCursor(-1) < 0) {
            }
        } else {
            while (WindowsSdk::ShowCursor(0) >= 0) {
            }
        }
        if (ResetNeeded && static_cast<std::uint8_t>(RestartNeeded ^ 1)) {
            GR_Main::GR_DXReset();
        }
        if (!RestartNeeded) {
            GlobalsV::RequestedScreenId = GlobalsV::SettingsReturnScreenId;
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenNone;
            GlobalsV::PostLoadScreenId = GlobalsV::SettingsReturnScreenId;
        }
        if (!GlobalsV::HardwareRenderingEnabled) {
            GR_DX::ReleaseAllTextureSurfaces();
        }
        RequestClose(1);
    }

    // Embedded-item data is ignored.
    GI_MessageLoop::TObjectGI* TfCfgSettings::CreateWarningImage(GI_Label::TLabelGI* Owner, EC_CacheFont::PFontObjectEC Item) {
        GI_MessageLoop::TObjectGI* Result = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
        {
            GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(Result);
            cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"Warning"}));
            cpp_with->SetImageKindX(GI_Main::ikxLeft);
            cpp_with->SetImageKindY(GI_Main::ikyTop);
            return Result;
        }
    }

    void TfCfgSettings::SelectMusic() {
        if (aPlayer::GetPlayer() == nullptr) {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
        } else if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId)].InternalName, u"Pirate"}));
                } else {
                    GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
                }
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
            }
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (pas::is_one_of<aGalaxyStruct::rstPirateBase, aGalaxyStruct::rstDominion>(aPlayer::GetPlayer()->DockedTo->TypeId)) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace)].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace)].InternalName}));
            }
        } else if (aPlayer::GetPlayer()->InNormalSpace()) {
            if (GlobalsV::MusicInSpaceEnabled) {
                if (aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 20) {
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
    }

    void TfCfgSettings::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10000);
        }
        aScript::ExecuteGameplayUiCode(Block, Key);
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20000);
        }
    }

} // namespace fCfgSettings
