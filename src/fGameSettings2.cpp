#include "layout/fGameSettings2.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Edit.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/fFilmFile.hpp"
#include "types/fGameSettings.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/GI_CountBar.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"
#include "units/aScript.hpp"
#include "units/fGameSettings2.hpp"

namespace fGameSettings2 {
    void TfGameSettings2::InitializeLayout() {
        std::int32_t I{};
        std::int32_t J{};
        pas::WideString Text{};
        aGalaxyStruct::TOwnerId Race{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fGameSettings2... "_a);
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range.next(Race); ) {
            I = -1;
            do {
                ++I;
                Text = pas::wide_int_to_str(I);
                if (I < 10) {
                    Text = pas::concat_wide({u"0", Text});
                }
            } while (!(([&] {
                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"StyleFace", aConst::OwnerInfo[aConst::RaceToOwner(Race)].InternalName}));
                const pas::WideString& text = Text;
                return blockByPath->CountParams(text);
            }()) <= 0));
            LastPortraitByRace[Race] = I - 1;
        }
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* ImageBG = MainPanel->FindByNameRecursive(u"ImageBG"sv);
                ImageBG->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* PanelChar = MainPanel->FindByNameRecursive(u"PanelChar"sv);
                PanelChar->SetPosition(ClassesImports::Point(PanelChar->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelChar->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PanelLevels = MainPanel->FindByNameRecursive(u"PanelLevels"sv);
                PanelLevels->SetPosition(ClassesImports::Point(PanelLevels->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelLevels->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PanelSkills = MainPanel->FindByNameRecursive(u"PanelSkills"sv);
                PanelSkills->SetPosition(ClassesImports::Point(PanelSkills->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelSkills->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PanelExtended = MainPanel->FindByNameRecursive(u"PanelExtended"sv);
                PanelExtended->SetPosition(ClassesImports::Point(PanelExtended->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelExtended->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Ok = MainPanel->FindByNameRecursive(u"Ok"sv);
                Ok->SetPosition(ClassesImports::Point(Ok->LocalPosition.X + GR_Main::ExtraScreenWidth, Ok->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* Cancel = MainPanel->FindByNameRecursive(u"Cancel"sv);
                Cancel->SetPosition(ClassesImports::Point(Cancel->LocalPosition.X, Cancel->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ImageHelp = MainPanel->FindByNameRecursive(u"ImageHelp"sv);
                ImageHelp->SetPosition(ClassesImports::Point(ImageHelp->LocalPosition.X + GR_Main::ExtraScreenWidth, ImageHelp->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ImageFooter = MainPanel->FindByNameRecursive(u"ImageFooter"sv);
                ImageFooter->SetPosition(ClassesImports::Point(ImageFooter->LocalPosition.X, ImageFooter->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                ImageFooter->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, ImageFooter->ClientSize.Y));
            }
            {
                GI_MessageLoop::TObjectGI* LabelHelp = MainPanel->FindByNameRecursive(u"LabelHelp"sv);
                LabelHelp->SetPosition(ClassesImports::Point(LabelHelp->LocalPosition.X + GR_Main::ExtraScreenWidth, LabelHelp->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfGameSettings2::MainPanelKeyDown>(this);
        GetByName(u"MainPanel"sv)->LeftButtonDownCallback = pas::bind_method<&TfGameSettings2::PlayerNameMouseDown>(this);
        SetHelpCallback(pas::bind_method<&TfGameSettings2::ShowControlHelp>(this));
        CollapsedLevelPanelTop = GetByName(u"PanelLevel"sv)->LocalPosition.Y;
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelOpen"sv))->UpCallback = pas::bind_method<&TfGameSettings2::ToggleLevelPanel>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelClose"sv))->UpCallback = pas::bind_method<&TfGameSettings2::ToggleLevelPanel>(this);
        {
            GI_GraphButton::TGraphButtonGI* RaceMaloc = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceMaloc"sv));
            RaceMaloc->DownCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RaceMaloc->UpCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RaceMaloc->UserValue = 0;
        }
        {
            GI_GraphButton::TGraphButtonGI* RacePeleng = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RacePeleng"sv));
            RacePeleng->DownCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RacePeleng->UpCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RacePeleng->UserValue = 1;
        }
        {
            GI_GraphButton::TGraphButtonGI* RacePeople = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RacePeople"sv));
            RacePeople->DownCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RacePeople->UpCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RacePeople->UserValue = 2;
        }
        {
            GI_GraphButton::TGraphButtonGI* RaceFei = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceFei"sv));
            RaceFei->DownCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RaceFei->UpCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RaceFei->UserValue = 3;
        }
        {
            GI_GraphButton::TGraphButtonGI* RaceGaal = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceGaal"sv));
            RaceGaal->DownCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RaceGaal->UpCallback = pas::bind_method<&TfGameSettings2::RaceClicked>(this);
            RaceGaal->UserValue = 4;
        }
        {
            GI_GraphButton::TGraphButtonGI* Char1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char1"sv));
            Char1->DownCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
            Char1->UpCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Char2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char2"sv));
            Char2->DownCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
            Char2->UpCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Char3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char3"sv));
            Char3->DownCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
            Char3->UpCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Char4 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char4"sv));
            Char4->DownCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
            Char4->UpCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Char5 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char5"sv));
            Char5->DownCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
            Char5->UpCallback = pas::bind_method<&TfGameSettings2::CharacterPresetClicked>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"FaceLeft"sv))->DownCallback = pas::bind_method<&TfGameSettings2::PreviousPortraitClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"FaceRight"sv))->DownCallback = pas::bind_method<&TfGameSettings2::NextPortraitClicked>(this);
        {
            GI_GraphButton::TGraphButtonGI* Skill1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Skill1"sv));
            Skill1->DownCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
            Skill1->UpCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Skill2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Skill2"sv));
            Skill2->DownCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
            Skill2->UpCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Skill3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Skill3"sv));
            Skill3->DownCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
            Skill3->UpCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Skill4 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Skill4"sv));
            Skill4->DownCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
            Skill4->UpCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Skill5 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Skill5"sv));
            Skill5->DownCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
            Skill5->UpCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Skill6 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Skill6"sv));
            Skill6->DownCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
            Skill6->UpCallback = pas::bind_method<&TfGameSettings2::StartingSkillClicked>(this);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 12); cpp_range_2.next(I); ) {
            GI_GraphButton::TGraphButtonGI* cpp_with_28 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Item", SysUtils::IntToStr(I)})))));
            cpp_with_28->DownCallback = pas::bind_method<&TfGameSettings2::StartingItemClicked>(this);
            cpp_with_28->UpCallback = pas::bind_method<&TfGameSettings2::StartingItemClicked>(this);
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 11); cpp_range_3.next(I); ) {
            ItemTypeByChoice[I] = I + 43;
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 11); cpp_range_4.next(I); ) {
            if (pas::in_range(ItemTypeByChoice[I], static_cast<std::int32_t>(aConst::t_IndustrialLaser), static_cast<std::int32_t>(aConst::t_Lirecron))) {
                GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"ItemI", SysUtils::IntToStr(I + 1)})))));
                pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[static_cast<aConst::TItemType>(ItemTypeByChoice[I])], u"s"});
                cpp_arg->SetImagePath(std::move(cpp_arg_2));
            } else {
                pas::WideString cpp_arg_3 = pas::concat_wide({u"GI,Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[static_cast<aConst::TItemType>(ItemTypeByChoice[I])], pas::wide_int_to_str(1), u"s"});
                GI_Image::TImageGI* cpp_arg_4 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"ItemI", SysUtils::IntToStr(I + 1)})))));
                cpp_arg_4->SetImagePath(std::move(cpp_arg_3));
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, 4); cpp_range_5.next(I); ) {
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, 7); cpp_range_6.next(J); ) {
                GI_GraphButton::TGraphButtonGI* cpp_with_29 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Level", SysUtils::IntToStr(I), "_", SysUtils::IntToStr(J)})))));
                cpp_with_29->UpCallback = pas::bind_method<&TfGameSettings2::DifficultyComponentClicked>(this);
                cpp_with_29->DownCallback = pas::bind_method<&TfGameSettings2::DifficultyComponentClicked>(this);
                cpp_with_29->UserValue = J;
                cpp_with_29->UserIndex = I;
            }
        }
        {
            GI_GraphButton::TGraphButtonGI* LevelUser = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelUser"sv));
            LevelUser->UpCallback = pas::bind_method<&TfGameSettings2::CustomDifficultyClicked>(this);
            LevelUser->DownCallback = pas::bind_method<&TfGameSettings2::CustomDifficultyClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Level1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Level1"sv));
            Level1->UpCallback = pas::bind_method<&TfGameSettings2::DifficultyPresetClicked>(this);
            Level1->DownCallback = pas::bind_method<&TfGameSettings2::DifficultyPresetClicked>(this);
            Level1->LeftButtonDoubleClickCallback = pas::bind_method<&TfGameSettings2::CustomDifficultyMouseUp>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Level2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Level2"sv));
            Level2->UpCallback = pas::bind_method<&TfGameSettings2::DifficultyPresetClicked>(this);
            Level2->DownCallback = pas::bind_method<&TfGameSettings2::DifficultyPresetClicked>(this);
            Level2->LeftButtonDoubleClickCallback = pas::bind_method<&TfGameSettings2::CustomDifficultyMouseUp>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Level3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Level3"sv));
            Level3->UpCallback = pas::bind_method<&TfGameSettings2::DifficultyPresetClicked>(this);
            Level3->DownCallback = pas::bind_method<&TfGameSettings2::DifficultyPresetClicked>(this);
            Level3->LeftButtonDoubleClickCallback = pas::bind_method<&TfGameSettings2::CustomDifficultyMouseUp>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Level4 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Level4"sv));
            Level4->UpCallback = pas::bind_method<&TfGameSettings2::DifficultyPresetClicked>(this);
            Level4->DownCallback = pas::bind_method<&TfGameSettings2::DifficultyPresetClicked>(this);
            Level4->LeftButtonDoubleClickCallback = pas::bind_method<&TfGameSettings2::CustomDifficultyMouseUp>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->UpCallback = pas::bind_method<&TfGameSettings2::ApplyClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Cancel"sv))->UpCallback = pas::bind_method<&TfGameSettings2::CancelClicked>(this);
        {
            GI_Edit::TEditGI* PlayerName = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv));
            PlayerName->ChangedCallback = pas::bind_method<&TfGameSettings2::PlayerNameChanged>(this);
            PlayerName->MaxLength = 13;
            PlayerName->MouseEnterCallback = pas::bind_method<&TfGameSettings2::HelpMouseEnter>(this);
            PlayerName->MouseLeaveCallback = pas::bind_method<&TfGameSettings2::HelpMouseLeave>(this);
        }
        {
            GI_MessageLoop::TObjectGI* CaptainI = GetByName(u"CaptainI"sv);
            CaptainI->MouseEnterCallback = pas::bind_method<&TfGameSettings2::HelpMouseEnter>(this);
            CaptainI->MouseLeaveCallback = pas::bind_method<&TfGameSettings2::HelpMouseLeave>(this);
        }
        {
            GI_MessageLoop::TObjectGI* LevelProc = GetByName(u"LevelProc"sv);
            LevelProc->MouseEnterCallback = pas::bind_method<&TfGameSettings2::HelpMouseEnter>(this);
            LevelProc->MouseLeaveCallback = pas::bind_method<&TfGameSettings2::HelpMouseLeave>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButExtended"sv))->DownCallback = pas::bind_static_method<&TfGameSettings2::ExtendedSettingsPressed>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButCloseExt"sv))->UpCallback = pas::bind_method<&TfGameSettings2::ToggleExtendedSettings>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButReset"sv))->UpCallback = pas::bind_method<&TfGameSettings2::ResetExtendedSettingsClicked>(this);
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
            ButGroup0->UpCallback = pas::bind_method<&TfGameSettings2::ExtendedGroupClicked>(this);
            ButGroup0->DownCallback = pas::bind_method<&TfGameSettings2::ExtendedGroupClicked>(this);
            ExtendedGroupButtonTops[0] = ButGroup0->LocalPosition.Y;
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
            ButGroup1->UpCallback = pas::bind_method<&TfGameSettings2::ExtendedGroupClicked>(this);
            ButGroup1->DownCallback = pas::bind_method<&TfGameSettings2::ExtendedGroupClicked>(this);
            ExtendedGroupButtonTops[1] = ButGroup1->LocalPosition.Y;
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
            ButGroup2->UpCallback = pas::bind_method<&TfGameSettings2::ExtendedGroupClicked>(this);
            ButGroup2->DownCallback = pas::bind_method<&TfGameSettings2::ExtendedGroupClicked>(this);
            ExtendedGroupButtonTops[2] = ButGroup2->LocalPosition.Y;
        }
    }

    void TfGameSettings2::OnOpen() {
        std::int32_t I{};
        std::int32_t Value{};
        pas::WideString Text{};
        pas::WideString ValueText{};
        std::int32_t Position{};
        std::uint8_t Enabled{};
        // Nested in OnOpen; static link unused. Returns an empty string for an absent CustomRules block or key.
        auto GetNewGameCustomRule = [&](pas::WideString Path) -> pas::WideString {
            pas::WideString Result{};
            if (GR_Main::NewGameSettingsConfig->CountBlocks(u"CustomRules"_wref.get()) != 0) {
                if (([&] {
                    EC_BlockPar::TBlockParEC* blockByPath = GR_Main::NewGameSettingsConfig->GetBlockByPath(u"CustomRules"_wref.get());
                    const pas::WideString& path = Path;
                    return blockByPath->CountParamsByPath(path);
                }()) != 0) {
                    EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::NewGameSettingsConfig->GetBlockByPath(u"CustomRules"_wref.get());
                    const pas::WideString& path_2 = Path;
                    return blockByPath_2->GetParamByPath(path_2);
                }
            }
            return Result;
        };
        PlayerNameEdited = false;
        PlayerNameValid = true;
        IronWillLabel = nullptr;
        IronWill = false;
        LevelPanelTop = CollapsedLevelPanelTop;
        {
            WindowsSdk::TPoint point = ClassesImports::Point(GetByName(u"PanelLevel"sv)->LocalPosition.X, LevelPanelTop);
            GI_MessageLoop::TObjectGI* byName = GetByName(u"PanelLevel"sv);
            byName->SetPosition(point);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelOpen"sv))->SetActive(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelClose"sv))->SetActive(false);
        if (GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Race"_wref.get()) > 0) {
            Text = GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Race"_wref.get());
            if (Text == u"Maloc") {
                PlayerRace = aGalaxyStruct::oiMaloc;
                RaceClicked(GetByName(u"RaceMaloc"sv));
            } else if (Text == u"Peleng") {
                PlayerRace = aGalaxyStruct::oiPeleng;
                RaceClicked(GetByName(u"RacePeleng"sv));
            } else if (Text == u"Fei") {
                PlayerRace = aGalaxyStruct::oiFeyan;
                RaceClicked(GetByName(u"RaceFei"sv));
            } else if (Text == u"Gaal") {
                PlayerRace = aGalaxyStruct::oiGaal;
                RaceClicked(GetByName(u"RaceGaal"sv));
            } else {
                PlayerRace = aGalaxyStruct::oiHuman;
                RaceClicked(GetByName(u"RacePeople"sv));
            }
        } else {
            PlayerRace = aGalaxyStruct::oiHuman;
            RaceClicked(GetByName(u"RacePeople"sv));
        }
        if (GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Name"_wref.get()) > 0 && GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Name"_wref.get()) != u"") {
            Text = GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Name"_wref.get());
            pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->SetText(Text);
            ValidatePlayerName(Text);
        }
        if (GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Char"_wref.get()) > 0 && EC_Str::IsIntegerTextW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Char"_wref.get()))) && pas::in_range(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Char"_wref.get()))), 1, 5)) {
            CharacterPreset = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Char"_wref.get())));
        } else {
            CharacterPreset = 3;
        }
        CharacterPresetClicked(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Char", SysUtils::IntToStr(CharacterPreset)})))));
        if (GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Face"_wref.get()) > 0 && EC_Str::IsIntegerTextW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Face"_wref.get())))) {
            CaptainPortraitIndex = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Face"_wref.get())));
        } else {
            CaptainPortraitIndex = 0;
        }
        RefreshPortrait();
        SelectedSkillSlot = 0;
        if (GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Skill1"_wref.get()) > 0 && EC_Str::IsIntegerTextW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Skill1"_wref.get()))) && pas::in_range(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Skill1"_wref.get()))), 0, 5) && GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Skill2"_wref.get()) > 0 && EC_Str::IsIntegerTextW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Skill2"_wref.get()))) && pas::in_range(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Skill2"_wref.get()))), 0, 5) && ([&] {
            std::int32_t cpp_left = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Skill1"_wref.get())));
            return cpp_left != EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Skill2"_wref.get())));
        }())) {
            StartingSkills[0] = static_cast<aGalaxyStruct::TPilotSkill>(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Skill1"_wref.get()))));
            StartingSkills[1] = static_cast<aGalaxyStruct::TPilotSkill>(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Skill2"_wref.get()))));
        } else {
            StartingSkills[0] = aGalaxyStruct::psAccuracy;
            StartingSkills[1] = aGalaxyStruct::psTrading;
        }
        RefreshStartingSkills();
        SelectedItemSlot = 0;
        if (GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Item1"_wref.get()) > 0 && EC_Str::IsIntegerTextW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Item1"_wref.get()))) && pas::in_range(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Item1"_wref.get()))), 1, 12) && GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Item2"_wref.get()) > 0 && EC_Str::IsIntegerTextW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Item2"_wref.get()))) && pas::in_range(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Item2"_wref.get()))), 1, 12) && ([&] {
            std::int32_t cpp_left_2 = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Item1"_wref.get())));
            return cpp_left_2 != EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Item2"_wref.get())));
        }())) {
            StartingItemChoices[0] = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Item1"_wref.get())));
            StartingItemChoices[1] = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Item2"_wref.get())));
        } else {
            StartingItemChoices[0] = 1;
            StartingItemChoices[1] = 2;
        }
        RefreshStartingItems();
        DifficultyPreset = 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
            if (GR_Main::NewGameSettingsConfig->CountParamsByPath(static_cast<pas::WideString>(pas::concat_ansi({"Level", SysUtils::IntToStr(I)}))) > 0) {
                ValueText = GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(static_cast<pas::WideString>(pas::concat_ansi({"Level", SysUtils::IntToStr(I)})));
                if (EC_Str::IsIntegerTextW(pas::view(ValueText))) {
                    Value = EC_Str::ExtractDigitsToIntW(pas::view(ValueText));
                    if (pas::contains(pas::make_set<pas::Set<0, 255>>({{0, static_cast<std::int32_t>(aConst::MaximumNewGameDifficulty)}}), Value)) {
                        DifficultyLevels[I] = Value;
                    } else {
                        DifficultyLevels[I] = DifficultyPreset;
                    }
                }
            }
        }
        RefreshDifficultyHelp();
        RefreshDifficulty();
        std::int32_t X = 302;
        std::int32_t Y = 518;
        {
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormGameSet2.Common.IronWill"_wref.get());
            pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormGameSet2.Common.HelpIronWill"_wref.get());
            std::uint8_t ironWill = IronWill;
            TfGameSettings2* self = this;
            self->AddIronWillChoice(0, X, Y, std::move(localizedColorText), std::move(localizedColorText_2), ironWill, false);
        }
        if (GR_Main::NewGameSettingsConfig->CountParamsByPath(u"IronWill"_wref.get()) > 0) {
            if (GI_Main::ParseEnabledNameGI(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"IronWill"_wref.get())))) {
                IronWillMouseDown(IronWillLabel, 0u, ClassesImports::Point(-1000, -1000));
            }
        }
        {
            GI_MessageLoop::TObjectGI* PanelExtended = GetByName(u"PanelExtended"sv);
            PanelExtended->SetActive(false);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButExtended = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButExtended"sv));
            ButExtended->SetDown(GR_Main::NewGameSettingsConfig->CountParamsByPath(u"UseCustomRules"_wref.get()) > 0 && GI_Main::ParseEnabledNameGI(pas::view(GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"UseCustomRules"_wref.get()))));
        }
        GI_PanelScrollBar::TPanelScrollBarGI* Owner = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"sv));
        Owner->FreeOwnedChildren();
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 3); cpp_range_2.next(I); ) {
            ExtendedGroupNextY[I] = 0;
            ExtendedGroupPanels[I] = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Owner);
            {
                GI_Panel::TPanelGI* cpp_with_3 = ExtendedGroupPanels[I];
                cpp_with_3->SetSize(ClassesImports::Point(Owner->ClientSize.X, 0));
                cpp_with_3->SetPosition(ClassesImports::Point(0, 0));
                cpp_with_3->SetDepth(-1.0E+2);
                cpp_with_3->SetPositionModeW(true);
            }
        }
        BuildExtendedGroup = 0;
        Text = GetNewGameCustomRule(u"KlingStrength"_w);
        if (Text == u"") {
            Position = 0;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text)) + 1;
        }
        GI_Label::TLabelGI* ValueLabel = ([&] {
            pas::WideString localizedText = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.KlingStrength"_wref.get());
            TfGameSettings2* self_2 = this;
            return self_2->AddExtendedOptionLabel(u"KlingStrength"_w, std::move(localizedText), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 73, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedAutoPercent>(this));
        Text = GetNewGameCustomRule(u"KlingAggro"_w);
        if (Text == u"") {
            Position = 0;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text)) + 1;
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_2 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.KlingAggro"_wref.get());
            TfGameSettings2* self_3 = this;
            return self_3->AddExtendedOptionLabel(u"KlingAggro"_w, std::move(localizedText_2), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 73, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedAutoPercent>(this));
        Text = GetNewGameCustomRule(u"KlingSpawn"_w);
        if (Text == u"") {
            Position = 0;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text)) + 1;
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_3 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.KlingSpawn"_wref.get());
            TfGameSettings2* self_4 = this;
            return self_4->AddExtendedOptionLabel(u"KlingSpawn"_w, std::move(localizedText_3), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 73, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedAutoPercent>(this));
        Text = GetNewGameCustomRule(u"PirateAggro"_w);
        if (Text == u"") {
            Position = 0;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text)) + 1;
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_4 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.PirateAggro"_wref.get());
            TfGameSettings2* self_5 = this;
            return self_5->AddExtendedOptionLabel(u"PirateAggro"_w, std::move(localizedText_4), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 73, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedAutoPercent>(this));
        Text = GetNewGameCustomRule(u"CoalAggro"_w);
        if (Text == u"") {
            Position = 8;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_5 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.CoalAggro"_wref.get());
            TfGameSettings2* self_6 = this;
            return self_6->AddExtendedOptionLabel(u"CoalAggro"_w, std::move(localizedText_5), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 24, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedDifficultyPercent>(this));
        Text = GetNewGameCustomRule(u"ExtraInventions"_w);
        if (Text == u"") {
            Position = 0;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_6 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ExtraInventions"_wref.get());
            TfGameSettings2* self_7 = this;
            return self_7->AddExtendedOptionLabel(u"ExtraInventions"_w, std::move(localizedText_6), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 255, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedInteger>(this));
        Text = GetNewGameCustomRule(u"ExtraRangers"_w);
        if (Text == u"") {
            Position = 0;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_7 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ExtraRangers"_wref.get());
            TfGameSettings2* self_8 = this;
            return self_8->AddExtendedOptionLabel(u"ExtraRangers"_w, std::move(localizedText_7), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 50, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedInteger>(this));
        Text = GetNewGameCustomRule(u"ZeroStartExp"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_8 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ZeroStartExp"_wref.get());
            TfGameSettings2* self_9 = this;
            self_9->AddExtendedOptionLabel(u"ZeroStartExp"_w, std::move(localizedText_8), false);
        }
        {
            pas::WideString localizedText_9 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ZeroStartExpNo"_wref.get());
            TfGameSettings2* self_10 = this;
            self_10->AddExtendedOptionChoice(1, std::move(localizedText_9), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_10 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ZeroStartExpYes"_wref.get());
            TfGameSettings2* self_11 = this;
            self_11->AddExtendedOptionChoice(0, std::move(localizedText_10), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"KlingRacialWeapons"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_11 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.KlingRacialWeapons"_wref.get());
            TfGameSettings2* self_12 = this;
            self_12->AddExtendedOptionLabel(u"KlingRacialWeapons"_w, std::move(localizedText_11), false);
        }
        {
            pas::WideString localizedText_12 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.KlingRacialWeaponsNo"_wref.get());
            TfGameSettings2* self_13 = this;
            self_13->AddExtendedOptionChoice(1, std::move(localizedText_12), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_13 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.KlingRacialWeaponsYes"_wref.get());
            TfGameSettings2* self_14 = this;
            self_14->AddExtendedOptionChoice(0, std::move(localizedText_13), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"MaxRangeMissiles"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_14 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.MaxRangeMissiles"_wref.get());
            TfGameSettings2* self_15 = this;
            self_15->AddExtendedOptionLabel(u"MaxRangeMissiles"_w, std::move(localizedText_14), false);
        }
        {
            pas::WideString localizedText_15 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.MaxRangeMissilesNo"_wref.get());
            TfGameSettings2* self_16 = this;
            self_16->AddExtendedOptionChoice(1, std::move(localizedText_15), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_16 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.MaxRangeMissilesYes"_wref.get());
            TfGameSettings2* self_17 = this;
            self_17->AddExtendedOptionChoice(0, std::move(localizedText_16), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"HullGrowth"_w);
        if (Text == u"") {
            Position = 0;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        {
            pas::WideString localizedText_17 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.HullGrowth"_wref.get());
            TfGameSettings2* self_18 = this;
            self_18->AddExtendedOptionLabel(u"HullGrowth"_w, std::move(localizedText_17), false);
        }
        {
            std::uint8_t cpp_arg = static_cast<std::uint8_t>(pas::in_set<1, 2>(Position) ^ 1);
            pas::WideString localizedText_18 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.HullGrowthNormal"_wref.get());
            TfGameSettings2* self_19 = this;
            self_19->AddExtendedOptionChoice(0, std::move(localizedText_18), cpp_arg, false);
        }
        {
            pas::WideString localizedText_19 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.HullGrowthSlow"_wref.get());
            TfGameSettings2* self_20 = this;
            self_20->AddExtendedOptionChoice(1, std::move(localizedText_19), Position == 1, false);
        }
        {
            pas::WideString localizedText_20 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.HullGrowthTechOnly"_wref.get());
            TfGameSettings2* self_21 = this;
            self_21->AddExtendedOptionChoice(2, std::move(localizedText_20), Position == 2, false);
        }
        BuildExtendedGroup = 1;
        Text = GetNewGameCustomRule(u"AsteroidMod"_w);
        if (Text == u"") {
            Position = 8;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_21 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.AsteroidMod"_wref.get());
            TfGameSettings2* self_22 = this;
            return self_22->AddExtendedOptionLabel(u"AsteroidMod"_w, std::move(localizedText_21), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 24, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedDifficultyPercent>(this));
        Text = GetNewGameCustomRule(u"SunDamageMod"_w);
        if (Text == u"") {
            Position = 8;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_22 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.SunDamageMod"_wref.get());
            TfGameSettings2* self_23 = this;
            return self_23->AddExtendedOptionLabel(u"SunDamageMod"_w, std::move(localizedText_22), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 24, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedDifficultyPercent>(this));
        Text = GetNewGameCustomRule(u"AgPlanets"_w);
        if (Text == u"") {
            Position = 5;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_23 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.PlanetsAg"_wref.get());
            TfGameSettings2* self_24 = this;
            return self_24->AddExtendedOptionLabel(u"AgPlanets"_w, std::move(localizedText_23), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 10, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedInteger>(this));
        Text = GetNewGameCustomRule(u"MiPlanets"_w);
        if (Text == u"") {
            Position = 5;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_24 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.PlanetsMi"_wref.get());
            TfGameSettings2* self_25 = this;
            return self_25->AddExtendedOptionLabel(u"MiPlanets"_w, std::move(localizedText_24), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 10, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedInteger>(this));
        Text = GetNewGameCustomRule(u"InPlanets"_w);
        if (Text == u"") {
            Position = 5;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_25 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.PlanetsIn"_wref.get());
            TfGameSettings2* self_26 = this;
            return self_26->AddExtendedOptionLabel(u"InPlanets"_w, std::move(localizedText_25), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 10, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedInteger>(this));
        Text = GetNewGameCustomRule(u"StartCenter"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_26 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.StartCenter"_wref.get());
            TfGameSettings2* self_27 = this;
            self_27->AddExtendedOptionLabel(u"StartCenter"_w, std::move(localizedText_26), false);
        }
        {
            pas::WideString localizedText_27 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.StartCenterNo"_wref.get());
            TfGameSettings2* self_28 = this;
            self_28->AddExtendedOptionChoice(1, std::move(localizedText_27), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_28 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.StartCenterYes"_wref.get());
            TfGameSettings2* self_29 = this;
            self_29->AddExtendedOptionChoice(0, std::move(localizedText_28), Enabled, false);
        }
        BuildExtendedGroup = 2;
        Text = GetNewGameCustomRule(u"RndChaotic"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_29 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.Rnd"_wref.get());
            TfGameSettings2* self_30 = this;
            self_30->AddExtendedOptionLabel(u"RndType"_w, std::move(localizedText_29), false);
        }
        {
            pas::WideString localizedText_30 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RndDetermined"_wref.get());
            TfGameSettings2* self_31 = this;
            self_31->AddExtendedOptionChoice(1, std::move(localizedText_30), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_31 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RndChaotic"_wref.get());
            TfGameSettings2* self_32 = this;
            self_32->AddExtendedOptionChoice(0, std::move(localizedText_31), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"RuinsNearStars"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_32 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsPosition"_wref.get());
            TfGameSettings2* self_33 = this;
            self_33->AddExtendedOptionLabel(u"RuinsPosition"_w, std::move(localizedText_32), false);
        }
        {
            pas::WideString localizedText_33 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsPositionNormal"_wref.get());
            TfGameSettings2* self_34 = this;
            self_34->AddExtendedOptionChoice(1, std::move(localizedText_33), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_34 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsPositionNear"_wref.get());
            TfGameSettings2* self_35 = this;
            self_35->AddExtendedOptionChoice(0, std::move(localizedText_34), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"RuinsTargettingFull"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_35 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsTargetting"_wref.get());
            TfGameSettings2* self_36 = this;
            self_36->AddExtendedOptionLabel(u"RuinsTargetting"_w, std::move(localizedText_35), false);
        }
        {
            pas::WideString localizedText_36 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsTargettingLimited"_wref.get());
            TfGameSettings2* self_37 = this;
            self_37->AddExtendedOptionChoice(1, std::move(localizedText_36), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_37 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsTargettingFull"_wref.get());
            TfGameSettings2* self_38 = this;
            self_38->AddExtendedOptionChoice(0, std::move(localizedText_37), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"RuinsUseShop"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_38 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsUseShop"_wref.get());
            TfGameSettings2* self_39 = this;
            self_39->AddExtendedOptionLabel(u"RuinsUseShop"_w, std::move(localizedText_38), false);
        }
        {
            pas::WideString localizedText_39 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsUseShopNo"_wref.get());
            TfGameSettings2* self_40 = this;
            self_40->AddExtendedOptionChoice(1, std::move(localizedText_39), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_40 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.RuinsUseShopYes"_wref.get());
            TfGameSettings2* self_41 = this;
            self_41->AddExtendedOptionChoice(0, std::move(localizedText_40), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"SpecialShipsInGame"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_41 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.SpecialShips"_wref.get());
            TfGameSettings2* self_42 = this;
            self_42->AddExtendedOptionLabel(u"SpecialShipsInGame"_w, std::move(localizedText_41), false);
        }
        {
            pas::WideString localizedText_42 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.SpecialShipsNo"_wref.get());
            TfGameSettings2* self_43 = this;
            self_43->AddExtendedOptionChoice(1, std::move(localizedText_42), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_43 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.SpecialShipsYes"_wref.get());
            TfGameSettings2* self_44 = this;
            self_44->AddExtendedOptionChoice(0, std::move(localizedText_43), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"AkrinMod"_w);
        if (Text == u"") {
            Position = 30;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_44 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.AkrinMod"_wref.get());
            TfGameSettings2* self_45 = this;
            return self_45->AddExtendedOptionLabel(u"AkrinMod"_w, std::move(localizedText_44), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 100, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedPercent>(this));
        Text = GetNewGameCustomRule(u"NodeDropMod"_w);
        if (Text == u"") {
            Position = 8;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_45 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.NodeDropMod"_wref.get());
            TfGameSettings2* self_46 = this;
            return self_46->AddExtendedOptionLabel(u"NodeDropMod"_w, std::move(localizedText_45), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 24, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedDifficultyPercent>(this));
        Text = GetNewGameCustomRule(u"EqKnowledgeUnRestricted"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_46 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.EqKnowledge"_wref.get());
            TfGameSettings2* self_47 = this;
            self_47->AddExtendedOptionLabel(u"EqKnowledgeType"_w, std::move(localizedText_46), false);
        }
        {
            pas::WideString localizedText_47 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.EqKnowledgeRestricted"_wref.get());
            TfGameSettings2* self_48 = this;
            self_48->AddExtendedOptionChoice(1, std::move(localizedText_47), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_48 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.EqKnowledgeUnRestricted"_wref.get());
            TfGameSettings2* self_49 = this;
            self_49->AddExtendedOptionChoice(0, std::move(localizedText_48), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"DropValueMod"_w);
        if (Text == u"") {
            Position = 8;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_49 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.DropValueMod"_wref.get());
            TfGameSettings2* self_50 = this;
            return self_50->AddExtendedOptionLabel(u"DropValueMod"_w, std::move(localizedText_49), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 24, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedDifficultyPercent>(this));
        Text = GetNewGameCustomRule(u"ABDropValueMod"_w);
        if (Text == u"") {
            Position = 8;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_50 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABDropValueMod"_wref.get());
            TfGameSettings2* self_51 = this;
            return self_51->AddExtendedOptionLabel(u"ABDropValueMod"_w, std::move(localizedText_50), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 24, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedDifficultyPercent>(this));
        Text = GetNewGameCustomRule(u"ABHitpointsMod"_w);
        if (Text == u"") {
            Position = 8;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_51 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABHitpointsMod"_wref.get());
            TfGameSettings2* self_52 = this;
            return self_52->AddExtendedOptionLabel(u"ABHitpointsMod"_w, std::move(localizedText_51), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 24, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedDifficultyPercent>(this));
        Text = GetNewGameCustomRule(u"ABDamageMod"_w);
        if (Text == u"") {
            Position = 8;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_52 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABDamageMod"_wref.get());
            TfGameSettings2* self_53 = this;
            return self_53->AddExtendedOptionLabel(u"ABDamageMod"_w, std::move(localizedText_52), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 24, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedDifficultyPercent>(this));
        Text = GetNewGameCustomRule(u"ABattleRoyale"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_53 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABattleRoyale"_wref.get());
            TfGameSettings2* self_54 = this;
            self_54->AddExtendedOptionLabel(u"ABattleRoyale"_w, std::move(localizedText_53), false);
        }
        {
            pas::WideString localizedText_54 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABattleRoyaleNo"_wref.get());
            TfGameSettings2* self_55 = this;
            self_55->AddExtendedOptionChoice(1, std::move(localizedText_54), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_55 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABattleRoyaleYes"_wref.get());
            TfGameSettings2* self_56 = this;
            self_56->AddExtendedOptionChoice(0, std::move(localizedText_55), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"ABChangeEq"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_56 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABChangeEq"_wref.get());
            TfGameSettings2* self_57 = this;
            self_57->AddExtendedOptionLabel(u"ABChangeEq"_w, std::move(localizedText_56), false);
        }
        {
            pas::WideString localizedText_57 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABChangeEqNo"_wref.get());
            TfGameSettings2* self_58 = this;
            self_58->AddExtendedOptionChoice(1, std::move(localizedText_57), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_58 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.ABChangeEqYes"_wref.get());
            TfGameSettings2* self_59 = this;
            self_59->AddExtendedOptionChoice(0, std::move(localizedText_58), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"AITolerateJunk"_w);
        if (Text == u"") {
            Position = 7;
        } else {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        ValueLabel = ([&] {
            pas::WideString localizedText_59 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.AITolerateJunk"_wref.get());
            TfGameSettings2* self_60 = this;
            return self_60->AddExtendedOptionLabel(u"AITolerateJunk"_w, std::move(localizedText_59), false);
        }());
        AddExtendedOptionSlider(ValueLabel, 0, 50, Position, 1, pas::bind_static_method<&TfGameSettings2::FormatExtendedInteger>(this));
        Text = GetNewGameCustomRule(u"OldHyper"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_60 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.OldHyper"_wref.get());
            TfGameSettings2* self_61 = this;
            self_61->AddExtendedOptionLabel(u"OldHyper"_w, std::move(localizedText_60), false);
        }
        {
            pas::WideString localizedText_61 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.OldHyperNo"_wref.get());
            TfGameSettings2* self_62 = this;
            self_62->AddExtendedOptionChoice(1, std::move(localizedText_61), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_62 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.OldHyperYes"_wref.get());
            TfGameSettings2* self_63 = this;
            self_63->AddExtendedOptionChoice(0, std::move(localizedText_62), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"PirateNodes"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_63 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.PirateNodes"_wref.get());
            TfGameSettings2* self_64 = this;
            self_64->AddExtendedOptionLabel(u"PirateNodes"_w, std::move(localizedText_63), false);
        }
        {
            pas::WideString localizedText_64 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.PirateNodesNo"_wref.get());
            TfGameSettings2* self_65 = this;
            self_65->AddExtendedOptionChoice(1, std::move(localizedText_64), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_65 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.PirateNodesYes"_wref.get());
            TfGameSettings2* self_66 = this;
            self_66->AddExtendedOptionChoice(0, std::move(localizedText_65), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"AIUseShops"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_66 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.AIUseShops"_wref.get());
            TfGameSettings2* self_67 = this;
            self_67->AddExtendedOptionLabel(u"AIUseShops"_w, std::move(localizedText_66), false);
        }
        {
            pas::WideString localizedText_67 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.AIUseShopsNo"_wref.get());
            TfGameSettings2* self_68 = this;
            self_68->AddExtendedOptionChoice(1, std::move(localizedText_67), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_68 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.AIUseShopsYes"_wref.get());
            TfGameSettings2* self_69 = this;
            self_69->AddExtendedOptionChoice(0, std::move(localizedText_68), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"DuplicateArts"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_69 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.DuplicateArts"_wref.get());
            TfGameSettings2* self_70 = this;
            self_70->AddExtendedOptionLabel(u"DuplicateArts"_w, std::move(localizedText_69), false);
        }
        {
            pas::WideString localizedText_70 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.DuplicateArtsNo"_wref.get());
            TfGameSettings2* self_71 = this;
            self_71->AddExtendedOptionChoice(1, std::move(localizedText_70), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_71 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.DuplicateArtsYes"_wref.get());
            TfGameSettings2* self_72 = this;
            self_72->AddExtendedOptionChoice(0, std::move(localizedText_71), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"OldSpeedCalc"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_72 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.SpeedCalc"_wref.get());
            TfGameSettings2* self_73 = this;
            self_73->AddExtendedOptionLabel(u"SpeedCalc"_w, std::move(localizedText_72), false);
        }
        {
            pas::WideString localizedText_73 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.SpeedCalcNonLinear"_wref.get());
            TfGameSettings2* self_74 = this;
            self_74->AddExtendedOptionChoice(1, std::move(localizedText_73), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_74 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.SpeedCalcLinear"_wref.get());
            TfGameSettings2* self_75 = this;
            self_75->AddExtendedOptionChoice(0, std::move(localizedText_74), Enabled, false);
        }
        Text = GetNewGameCustomRule(u"OldMissileBonuses"_w);
        if (Text == u"") {
            Enabled = false;
        } else {
            Enabled = GI_Main::ParseEnabledNameGI(pas::view(Text));
        }
        {
            pas::WideString localizedText_75 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.MissileBonuses"_wref.get());
            TfGameSettings2* self_76 = this;
            self_76->AddExtendedOptionLabel(u"MissileBonuses"_w, std::move(localizedText_75), false);
        }
        {
            pas::WideString localizedText_76 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.MissileBonusesSplit"_wref.get());
            TfGameSettings2* self_77 = this;
            self_77->AddExtendedOptionChoice(1, std::move(localizedText_76), static_cast<std::uint8_t>(Enabled ^ 1), false);
        }
        {
            pas::WideString localizedText_77 = aConst::LocalizedText(u"FormGameSet2.Extended.ParameterNames.MissileBonusesNotSplit"_wref.get());
            TfGameSettings2* self_78 = this;
            self_78->AddExtendedOptionChoice(0, std::move(localizedText_77), Enabled, false);
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 3); cpp_range_3.next(I); ) {
            GI_Panel::TPanelGI* cpp_with_4 = ExtendedGroupPanels[I];
            cpp_with_4->SetSize(ClassesImports::Point(cpp_with_4->ClientSize.X, ExtendedGroupNextY[I]));
        }
        {
            std::int32_t cpp_arg_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButReset"sv))->CaptionLabel->GetLineHeight() * 2;
            GI_ScrollBar::TScrollBarGI* verticalScrollBar = Owner->VerticalScrollBar;
            verticalScrollBar->SetSmallChange(cpp_arg_2);
        }
        Owner->VerticalScrollBar->SetLargeChange(Owner->ClientSize.Y);
        Owner->VerticalScrollBar->SetPageSize(Owner->ClientSize.Y);
        ActiveExtendedGroup = 0;
        RefreshExtendedGroup();
    }

    void TfGameSettings2::OnClose() {
        if (LevelPanelTimer != nullptr) {
            CancelCallbackTimer(LevelPanelTimer);
            LevelPanelTimer = nullptr;
        }
        pas::free(IronWillImage);
        IronWillImage = nullptr;
        pas::free(IronWillLabel);
        IronWillLabel = nullptr;
        {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"sv));
            PanelSet->FreeOwnedChildren();
        }
    }

    void TfGameSettings2::AddIronWillChoice(std::int32_t Value, std::int32_t X, std::int32_t Y, pas::WideString Caption, pas::WideString Help, std::uint8_t Selected, std::uint8_t Disabled) {
        GI_MessageLoop::TObjectGI* Owner = GetByName(u"PanelChar"sv);
        IronWillImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
        {
            GI_Image::TImageGI* cpp_with = IronWillImage;
            cpp_with->SetName(u"ImgRadio"_wref.get());
            if (Disabled) {
                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchH"}));
            } else if (!Selected) {
                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"}));
            } else {
                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchD"}));
            }
            cpp_with->SetPosition(ClassesImports::Point(X, Y));
            cpp_with->SetSize(cpp_with->GetContentSize());
            cpp_with->SetImageKindY(GI_Main::ikyCenter);
            cpp_with->HelpText = Help;
            if (!Disabled) {
                cpp_with->LeftButtonDownCallback = pas::bind_method<&TfGameSettings2::IronWillMouseDown>(this);
                cpp_with->MouseEnterCallback = pas::bind_static_method<&TfGameSettings2::IronWillMouseEnter>(this);
                cpp_with->MouseLeaveCallback = pas::bind_static_method<&TfGameSettings2::IronWillMouseLeave>(this);
                cpp_with->HelpCallback = pas::bind_method<&TfGameSettings2::ShowControlHelp>(this);
                cpp_with->UserValue = Value;
            }
        }
        std::int32_t Width = GR_Main::GiScalePixelsEx(300, 200);
        IronWillLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
        {
            GI_Label::TLabelGI* cpp_with_2 = IronWillLabel;
            cpp_with_2->SetFontName(GlobalsV::RangerFontName);
            cpp_with_2->SetPositionModeW(false);
            cpp_with_2->SetPosition(ClassesImports::Point(X + IronWillImage->GetContentSize().X + 2, Y + GR_Main::GiScalePixelsEx(2, 1)));
            cpp_with_2->SetSize(ClassesImports::Point(Width, 1));
            cpp_with_2->SetTextAlignX(GI_Main::taxAuto);
            cpp_with_2->SetTextAlignY(GI_Main::tayAuto);
            cpp_with_2->HelpText = std::move(Help);
            cpp_with_2->SetText(Caption);
            if (Disabled) {
                cpp_with_2->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(127, 127, 127));
            } else {
                cpp_with_2->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 200, 0));
            }
            if (!Disabled) {
                cpp_with_2->LeftButtonDownCallback = pas::bind_method<&TfGameSettings2::IronWillMouseDown>(this);
                cpp_with_2->MouseEnterCallback = pas::bind_static_method<&TfGameSettings2::IronWillMouseEnter>(this);
                cpp_with_2->MouseLeaveCallback = pas::bind_static_method<&TfGameSettings2::IronWillMouseLeave>(this);
                cpp_with_2->HelpCallback = pas::bind_method<&TfGameSettings2::ShowControlHelp>(this);
            }
            cpp_with_2->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(IronWillImage));
        }
    }

    void TfGameSettings2::IronWillMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (pas::class_cast_if<GI_Label::TLabelGI*>(Sender) != nullptr) {
            Sender = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        }
        IronWill = static_cast<std::uint8_t>(IronWill ^ 1);
        if (IronWill) {
            {
                GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(Sender);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchD"});
                cpp_arg->SetImagePath(std::move(cpp_arg_2));
            }
            IronWillLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0));
        } else {
            {
                GI_Image::TImageGI* cpp_arg_3 = pas::checked_cast<GI_Image::TImageGI*>(Sender);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"});
                cpp_arg_3->SetImagePath(std::move(cpp_arg_4));
            }
            IronWillLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 200, 0));
        }
        if (Point.X != -1000 || Point.Y != -1000) {
            GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
        }
    }

    void TfGameSettings2::IronWillMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
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

    void TfGameSettings2::IronWillMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
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

    void TfGameSettings2::GeneratePlayerName() {
        pas::WideString BlockName{};
        pas::WideString Name{};
        std::int32_t Index{};
        BlockName = aConst::OwnerToSys(aConst::RaceToOwner(PlayerRace));
        do {
            Index = aMyFunction::RandomIntRange(0, GR_Main::LanguageDataConfig->GetBlock(u"ShipName"sv)->GetBlock(u"Ranger"sv)->GetBlock(pas::view(BlockName))->GetParamCount() - 1);
            Name = GR_Main::LanguageDataConfig->GetBlock(u"ShipName"sv)->GetBlock(u"Ranger"sv)->GetBlock(pas::view(BlockName))->GetParamValue(Index);
            pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->SetText(Name);
            {
                std::uint8_t cpp_arg = static_cast<std::uint8_t>(ValidatePlayerName(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text) ^ 1);
                GI_GraphButton::TGraphButtonGI* cpp_arg_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv));
                cpp_arg_2->SetDisabled(cpp_arg);
            }
        } while (!(Name.length() <= 13));
    }

    void TfGameSettings2::PlayerNameMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        SetFocusedControl(GetByName(u"PlayerName"sv));
    }

    void TfGameSettings2::ToggleLevelPanel(GI_MessageLoop::TObjectGI* Sender) {
        if (LevelPanelTimer == nullptr) {
            LevelPanelTimer = ScheduleCallbackTimer(10, 10, pas::bind_method<&TfGameSettings2::AnimateLevelPanel>(this), 0);
        }
        {
            std::uint8_t cpp_arg = static_cast<std::uint8_t>(GetByName(u"LevelOpen"sv)->Active ^ 1);
            GI_MessageLoop::TObjectGI* byName = GetByName(u"LevelOpen"sv);
            byName->SetActive(cpp_arg);
        }
        {
            std::uint8_t cpp_arg_2 = static_cast<std::uint8_t>(GetByName(u"LevelOpen"sv)->Active ^ 1);
            GI_MessageLoop::TObjectGI* byName_2 = GetByName(u"LevelClose"sv);
            byName_2->SetActive(cpp_arg_2);
        }
        RefreshStartingSkills();
        RefreshStartingItems();
        GI_Main::BreakUiMessage();
    }

    void TfGameSettings2::AnimateLevelPanel(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Level{};
        std::int32_t I{};
        if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelOpen"sv))->Active) {
            LevelPanelTop -= 10;
            if (LevelPanelTop <= CollapsedLevelPanelTop) {
                LevelPanelTop = CollapsedLevelPanelTop;
                if (LevelPanelTimer != nullptr) {
                    CancelCallbackTimer(LevelPanelTimer);
                    LevelPanelTimer = nullptr;
                }
            }
        } else {
            LevelPanelTop += 10;
            if (LevelPanelTop >= 0) {
                LevelPanelTop = 0;
                if (LevelPanelTimer != nullptr) {
                    CancelCallbackTimer(LevelPanelTimer);
                    LevelPanelTimer = nullptr;
                }
            }
        }
        {
            WindowsSdk::TPoint point = ClassesImports::Point(GetByName(u"PanelLevel"sv)->LocalPosition.X, LevelPanelTop);
            GI_MessageLoop::TObjectGI* byName = GetByName(u"PanelLevel"sv);
            byName->SetPosition(point);
        }
        {
            GI_GraphButton::TGraphButtonGI* LevelUser = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelUser"sv));
            if (!LevelUser->Down) {
                LevelUser->SetDown(true);
                LevelUser->SetDown(false);
            }
        }
        RefreshDifficulty();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 4); cpp_range.next(Level); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 7); cpp_range_2.next(I); ) {
                GI_GraphButton::TGraphButtonGI* cpp_with_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Level", SysUtils::IntToStr(Level), "_", SysUtils::IntToStr(I)})))));
                if (!cpp_with_2->Down) {
                    cpp_with_2->SetDown(true);
                    cpp_with_2->SetDown(false);
                }
            }
        }
        GR_Main::PostMouseMoveMessage();
    }

    void TfGameSettings2::RaceClicked(GI_MessageLoop::TObjectGI* Sender) {
        PlayerRace = static_cast<aGalaxyStruct::TOwnerId>(Sender->UserValue);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceMaloc"sv))->SetDown(PlayerRace == aGalaxyStruct::oiMaloc);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RacePeleng"sv))->SetDown(PlayerRace == aGalaxyStruct::oiPeleng);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RacePeople"sv))->SetDown(PlayerRace == aGalaxyStruct::oiHuman);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceFei"sv))->SetDown(PlayerRace == aGalaxyStruct::oiFeyan);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceGaal"sv))->SetDown(PlayerRace == aGalaxyStruct::oiGaal);
        CaptainPortraitIndex = 0;
        RefreshPortrait();
        if (!PlayerNameEdited) {
            GeneratePlayerName();
        }
        SetFocusedControl(GetByName(u"PlayerName"sv));
        {
            GI_Edit::TEditGI* PlayerName = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv));
            PlayerName->SetCaretPosition(PlayerName->Text.length());
        }
        PlayerNameChanged(nullptr);
    }

    void TfGameSettings2::CharacterPresetClicked(GI_MessageLoop::TObjectGI* Sender) {
        CharacterPreset = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char1"sv))->SetDown(CharacterPreset == 1);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char2"sv))->SetDown(CharacterPreset == 2);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char3"sv))->SetDown(CharacterPreset == 3);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char4"sv))->SetDown(CharacterPreset == 4);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Char5"sv))->SetDown(CharacterPreset == 5);
        PlayerNameChanged(nullptr);
    }

    void TfGameSettings2::RefreshPortrait() {
        if (CaptainPortraitIndex < 0) {
            CaptainPortraitIndex = LastPortraitByRace[PlayerRace];
        } else if (CaptainPortraitIndex > LastPortraitByRace[PlayerRace]) {
            CaptainPortraitIndex = 0;
        }
        {
            GI_Image::TImageGI* CaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"sv));
            if (LastPortraitByRace[PlayerRace] >= 0) {
                CaptainI->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(PlayerRace)].InternalName, pas::wide_int_to_str(CaptainPortraitIndex), u"i"}));
                CaptainI->SetImageKindX(GI_Main::ikxCenter);
                CaptainI->SetImageKindY(GI_Main::ikyCenter);
                CaptainI->SetActive(true);
            } else {
                CaptainI->SetActive(false);
            }
        }
        {
            GI_GAI::TgaiGI* CaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"sv));
            CaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            if (LastPortraitByRace[PlayerRace] >= 0) {
                CaptainA->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(PlayerRace)].InternalName, pas::wide_int_to_str(CaptainPortraitIndex), u"a"}));
                CaptainA->SequenceIndex = 0;
                CaptainA->UpdateAutoGeometry();
                CaptainA->SetImageKindX(GI_Main::ikxCenter);
                CaptainA->SetImageKindY(GI_Main::ikyCenter);
                CaptainA->SetActive(true);
                CaptainA->RestartPlayback();
            } else {
                CaptainA->SetActive(false);
            }
        }
    }

    void TfGameSettings2::PreviousPortraitClicked(GI_MessageLoop::TObjectGI* Sender) {
        --CaptainPortraitIndex;
        RefreshPortrait();
    }

    void TfGameSettings2::NextPortraitClicked(GI_MessageLoop::TObjectGI* Sender) {
        ++CaptainPortraitIndex;
        RefreshPortrait();
    }

    void TfGameSettings2::RefreshStartingSkills() {
        aGalaxyStruct::TPilotSkill Skill{};
        std::int32_t Number{};
        std::int32_t Slot{};
        std::uint8_t Selected{};
        {
            GI_MessageLoop::TObjectGI* SkillCur = GetByName(u"SkillCur"sv);
            SkillCur->SetPosition(ClassesImports::Point(68 + 55 * StartingSkills[SelectedSkillSlot], 109));
        }
        Number = 1;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TPilotSkill>(aGalaxyStruct::psAccuracy, aGalaxyStruct::psLeadership); cpp_range.next(Skill); ) {
            Selected = false;
            for (Slot = 0; Slot <= 1; ++Slot) {
                if (StartingSkills[Slot] == Skill) {
                    Selected = true;
                    break;
                }
            }
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(Number)})))))->SetDown(Selected);
            ++Number;
        }
    }

    void TfGameSettings2::StartingSkillClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Slot{};
        aGalaxyStruct::TPilotSkill Skill{};
        if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelOpen"sv))->Active) {
            Skill = static_cast<aGalaxyStruct::TPilotSkill>(EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName)) - 1);
            for (Slot = 0; Slot <= 1; ++Slot) {
                if (StartingSkills[Slot] == Skill) {
                    SelectedSkillSlot = Slot;
                    break;
                }
            }
            StartingSkills[SelectedSkillSlot] = Skill;
            RefreshStartingSkills();
        }
    }

    void TfGameSettings2::RefreshStartingItems() {
        std::int32_t Item{};
        std::int32_t Slot{};
        std::uint8_t Selected{};
        {
            GI_MessageLoop::TObjectGI* ItemCur = GetByName(u"ItemCur"sv);
            ItemCur->SetPosition(ClassesImports::Point(68 + (StartingItemChoices[SelectedItemSlot] - 1) % 6 * 55, 210 + (StartingItemChoices[SelectedItemSlot] - 1) / 6 * 61));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 12); cpp_range.next(Item); ) {
            Selected = false;
            for (Slot = 0; Slot <= 1; ++Slot) {
                if (StartingItemChoices[Slot] == Item) {
                    Selected = true;
                    break;
                }
            }
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Item", SysUtils::IntToStr(Item)})))))->SetDown(Selected);
        }
    }

    void TfGameSettings2::StartingItemClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Slot{};
        std::int32_t Item{};
        if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelOpen"sv))->Active) {
            Item = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
            for (Slot = 0; Slot <= 1; ++Slot) {
                if (StartingItemChoices[Slot] == Item) {
                    SelectedItemSlot = Slot;
                    break;
                }
            }
            StartingItemChoices[SelectedItemSlot] = Item;
            RefreshStartingItems();
        }
    }

    void TfGameSettings2::RefreshDifficulty() {
        std::int32_t I{};
        std::int32_t Level{};
        std::int32_t Average{};
        pas::WideString Color{};
        std::int32_t Custom = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 4); cpp_range_2.next(Level); ) {
                GI_GraphButton::TGraphButtonGI* cpp_with = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(FindControlByPath(static_cast<pas::WideString>(pas::concat_ansi({"Level", SysUtils::IntToStr(Level), "_", SysUtils::IntToStr(I)}))));
                cpp_with->SetActive(LevelPanelTop > CollapsedLevelPanelTop);
                cpp_with->SetDown(DifficultyLevels[I] == static_cast<std::uint8_t>(cpp_with->UserIndex));
            }
            if (DifficultyLevels[I] != DifficultyLevels[1]) {
                Custom = -1;
            }
        }
        if (Custom == 0) {
            DifficultyPreset = DifficultyLevels[1];
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LevelUser"sv))->SetDown(static_cast<std::int32_t>(Custom) != 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Level1"sv))->SetDown(DifficultyPreset == 0 && Custom == 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Level2"sv))->SetDown(DifficultyPreset == 1 && Custom == 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Level3"sv))->SetDown(DifficultyPreset == 2 && Custom == 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Level4"sv))->SetDown(DifficultyPreset == 3 && Custom == 0);
        Average = 0;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 7); cpp_range_3.next(I); ) {
            Average = Average + 50 + DifficultyLevels[I] * 50;
        }
        Average = Average / 8;
        if (Average == 50) {
            Color = aMyFunction::GreenColorTag;
        } else if (Average <= 100) {
            Color = u"<color=254,255,255>"_w;
        } else if (Average <= 150) {
            Color = aMyFunction::TextHighlightColorTag;
        } else if (Average <= 200) {
            Color = aMyFunction::OrangeColorTag;
        } else {
            Color = pas::concat_wide({u"<color=255,", EC_Str::IntToWideString(System::Round(aMyFunction::RemapClamped(Average, 2.0E+2, 5.0E+2, 166.0, 0.0))), u",0>"});
        }
        {
            const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(pas::view(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Average), "%"}))), pas::view(Color));
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LevelProc"sv));
            cpp_arg->SetText(wrapTextInColor);
        }
    }

    void TfGameSettings2::DifficultyComponentClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        aGalaxyStruct::TGalaxyDifficultyLevels Saved{};
        std::int32_t Component = Sender->UserValue;
        std::uint8_t Value = Sender->UserIndex;
        if (Sender->ControlName == static_cast<pas::WideString>(pas::concat_ansi({"Level4_", SysUtils::IntToStr(Component)})) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) && aConst::MaximumNewGameDifficulty > Value) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
                Saved[I] = DifficultyLevels[I];
                DifficultyLevels[I] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Level4_", SysUtils::IntToStr(I)})))))->UserIndex;
            }
            ++DifficultyLevels[Component];
            RefreshDifficultyHelp();
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 7); cpp_range_2.next(I); ) {
                DifficultyLevels[I] = Saved[I];
            }
        }
        if (Sender->ControlName == static_cast<pas::WideString>(pas::concat_ansi({"Level1_", SysUtils::IntToStr(Component)})) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) && Value > 0) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 7); cpp_range_3.next(I); ) {
                Saved[I] = DifficultyLevels[I];
                DifficultyLevels[I] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Level4_", SysUtils::IntToStr(I)})))))->UserIndex;
            }
            --DifficultyLevels[Component];
            RefreshDifficultyHelp();
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 7); cpp_range_4.next(I); ) {
                DifficultyLevels[I] = Saved[I];
            }
        }
        Value = Sender->UserIndex;
        DifficultyLevels[Component] = Value;
        std::uint8_t Custom = false;
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, 7); cpp_range_5.next(I); ) {
            if (DifficultyLevels[I] != Value) {
                Custom = true;
            }
        }
        if (!Custom) {
            DifficultyPreset = Value;
        }
        RefreshDifficulty();
    }

    void TfGameSettings2::DifficultyPresetClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        std::int32_t First{};
        std::int32_t J{};
        GI_Label::TLabelGI* CaptionLabel{};
        DifficultyPreset = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName)) - 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
            DifficultyLevels[I] = DifficultyPreset;
        }
        First = std::max<std::int32_t>(DifficultyPreset - 3, 0);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 8); cpp_range_2.next(I); ) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 3); cpp_range_3.next(J); ) {
                CaptionLabel = pas::checked_cast<GI_Label::TLabelGI*>(FindControlByPath(pas::concat_wide({u"NameGroup", EC_Str::IntToWideString(I), u"Level", EC_Str::IntToWideString(J)})));
                if (CaptionLabel != nullptr) {
                    switch (First + J) {
                        case 0: {
                            CaptionLabel->SetText(aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.NameGroup", EC_Str::IntToWideString(I), u"Easy"})));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0));
                            break;
                        }
                        case 1: {
                            CaptionLabel->SetText(aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.NameGroup", EC_Str::IntToWideString(I), u"Normal"})));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(254, 255, 255));
                            break;
                        }
                        case 2: {
                            CaptionLabel->SetText(aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.NameGroup", EC_Str::IntToWideString(I), u"Hard"})));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 240, 100));
                            break;
                        }
                        case 3: {
                            CaptionLabel->SetText(aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.NameGroup", EC_Str::IntToWideString(I), u"Expert"})));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 166, 0));
                            break;
                        }
                        default: {
                            CaptionLabel->SetText(pas::concat_wide({EC_Str::IntToWideString((First + J + 1) * 50), u"%"}));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, System::Round(aMyFunction::RemapClamped(First + J, 3.0, 9.0, 166.0, 0.0)), 0));
                            break;
                        }
                    }
                    {
                        GI_GraphButton::TGraphButtonGI* cpp_with = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Level", SysUtils::IntToStr(J + 1), "_", SysUtils::IntToStr(I - 1)})))));
                        cpp_with->UserValue = I - 1;
                        cpp_with->UserIndex = First + J;
                        switch (First + J) {
                            case 0: {
                                cpp_with->HelpText = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Easy"}));
                                break;
                            }
                            case 1: {
                                cpp_with->HelpText = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Normal"}));
                                break;
                            }
                            case 2: {
                                cpp_with->HelpText = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Hard"}));
                                break;
                            }
                            case 3: {
                                cpp_with->HelpText = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Expert"}));
                                break;
                            }
                            default: {
                                cpp_with->HelpText = pas::concat_wide({aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Expert"})), u"+"});
                                break;
                            }
                        }
                    }
                }
            }
        }
        RefreshDifficulty();
    }

    void TfGameSettings2::CustomDifficultyClicked(GI_MessageLoop::TObjectGI* Sender) {
        RefreshDifficulty();
        ToggleLevelPanel(Sender);
    }

    void TfGameSettings2::CustomDifficultyMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        RefreshDifficulty();
        ToggleLevelPanel(Sender);
    }

    void TfGameSettings2::StartNewGameGeneration() {
        std::int32_t I{};
        Globals::FilmHistory->Clear();
        Globals::NewGameGenerationThread->IronWill = IronWill;
        Globals::NewGameGenerationThread->PlayerRace = PlayerRace;
        Globals::NewGameGenerationThread->CharacterPreset = CharacterPreset;
        for (I = 0; I <= 7; ++I) {
            Globals::NewGameGenerationThread->DifficultyLevels[I] = DifficultyLevels[I];
        }
        Globals::NewGameGenerationThread->SetPriority(2);
        Globals::NewGameGenerationThread->CaptainPortraitIndex = CaptainPortraitIndex;
        for (I = 0; I <= 1; ++I) {
            Globals::NewGameGenerationThread->StartingItemTypes[I] = ItemTypeByChoice[StartingItemChoices[I] - 1];
        }
        for (I = 0; I <= 1; ++I) {
            Globals::NewGameGenerationThread->StartingSkills[I] = StartingSkills[I];
        }
        Globals::NewGameGenerationThread->Start();
    }

    void TfGameSettings2::ApplyClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        pas::WideString FileName{};
        if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->Disabled) {
            if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButExtended"sv))->Down && static_cast<std::uint8_t>(GetByName(u"PanelExtended"sv)->Active ^ 1)) {
                {
                    GI_MessageLoop::TObjectGI* PanelChar = GetByName(u"PanelChar"sv);
                    PanelChar->SetActive(false);
                }
                {
                    GI_MessageLoop::TObjectGI* PanelLevels = GetByName(u"PanelLevels"sv);
                    PanelLevels->SetActive(false);
                }
                {
                    GI_MessageLoop::TObjectGI* PanelSkills = GetByName(u"PanelSkills"sv);
                    PanelSkills->SetActive(false);
                }
                {
                    GI_MessageLoop::TObjectGI* PanelExtended = GetByName(u"PanelExtended"sv);
                    PanelExtended->SetActive(true);
                }
            } else {
                if (PlayerNameEdited) {
                    GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Name"_wref.get(), pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text);
                } else if (GR_Main::NewGameSettingsConfig->CountParamsByPath(u"Name"_wref.get()) <= 0 || ([&] {
                    pas::WideString cpp_string = GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"Name"_wref.get());
                    const pas::WideString& cpp_string_ref = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text;
                    return cpp_string != cpp_string_ref;
                }())) {
                    GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Name"_wref.get(), u""_wref.get());
                }
                if (IronWill) {
                    GR_Main::NewGameSettingsConfig->SetOrAddParam(u"IronWill"_wref.get(), u"True"_wref.get());
                } else {
                    GR_Main::NewGameSettingsConfig->SetOrAddParam(u"IronWill"_wref.get(), u"False"_wref.get());
                }
                GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Race"_wref.get(), aConst::OwnerInfo[aConst::RaceToOwner(PlayerRace)].InternalName);
                GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Char"_wref.get(), pas::wide_int_to_str(CharacterPreset));
                GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Face"_wref.get(), pas::wide_int_to_str(CaptainPortraitIndex));
                GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Skill1"_wref.get(), pas::wide_int_to_str(static_cast<std::int32_t>(StartingSkills[0])));
                GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Skill2"_wref.get(), pas::wide_int_to_str(static_cast<std::int32_t>(StartingSkills[1])));
                GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Item1"_wref.get(), pas::wide_int_to_str(StartingItemChoices[0]));
                GR_Main::NewGameSettingsConfig->SetOrAddParam(u"Item2"_wref.get(), pas::wide_int_to_str(StartingItemChoices[1]));
                for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
                    const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[I]));
                    const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"Level", SysUtils::IntToStr(I)}));
                    GR_Main::NewGameSettingsConfig->SetOrAddParam(cpp_arg, intToStr);
                }
                FileName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"newgame.txt"});
                if (GR_Main::NewGameSettingsConfig->CountBlocks(u"CustomRules"_wref.get()) == 0) {
                    GR_Main::NewGameSettingsConfig->AddBlockByPath(u"CustomRules"_wref.get());
                }
                GR_Main::NewGameSettingsConfig->SetOrAddParam(u"UseCustomRules"_wref.get(), EC_Str::BoolToWideString(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButExtended"sv))->Down));
                {
                    EC_BlockPar::TBlockParEC* CustomRules = GR_Main::NewGameSettingsConfig->GetBlockByPath(u"CustomRules"_wref.get());
                    ActiveExtendedGroup = 0;
                    CustomRules->SetOrAddParam(u"KlingStrength"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"KlingStrength"sv) - 1));
                    CustomRules->SetOrAddParam(u"KlingAggro"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"KlingAggro"sv) - 1));
                    CustomRules->SetOrAddParam(u"KlingSpawn"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"KlingSpawn"sv) - 1));
                    CustomRules->SetOrAddParam(u"PirateAggro"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"PirateAggro"sv) - 1));
                    CustomRules->SetOrAddParam(u"CoalAggro"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"CoalAggro"sv)));
                    CustomRules->SetOrAddParam(u"ExtraInventions"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"ExtraInventions"sv)));
                    CustomRules->SetOrAddParam(u"ExtraRangers"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"ExtraRangers"sv)));
                    CustomRules->SetOrAddParam(u"ZeroStartExp"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"ZeroStartExp"sv) == 0));
                    CustomRules->SetOrAddParam(u"KlingRacialWeapons"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"KlingRacialWeapons"sv) == 0));
                    CustomRules->SetOrAddParam(u"MaxRangeMissiles"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"MaxRangeMissiles"sv) == 0));
                    CustomRules->SetOrAddParam(u"HullGrowth"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"HullGrowth"sv)));
                    ActiveExtendedGroup = 1;
                    CustomRules->SetOrAddParam(u"AsteroidMod"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"AsteroidMod"sv)));
                    CustomRules->SetOrAddParam(u"SunDamageMod"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"SunDamageMod"sv)));
                    CustomRules->SetOrAddParam(u"AgPlanets"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"AgPlanets"sv)));
                    CustomRules->SetOrAddParam(u"MiPlanets"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"MiPlanets"sv)));
                    CustomRules->SetOrAddParam(u"InPlanets"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"InPlanets"sv)));
                    CustomRules->SetOrAddParam(u"StartCenter"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"StartCenter"sv) == 0));
                    ActiveExtendedGroup = 2;
                    CustomRules->SetOrAddParam(u"RndChaotic"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"RndType"sv) == 0));
                    CustomRules->SetOrAddParam(u"EqKnowledgeUnRestricted"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"EqKnowledgeType"sv) == 0));
                    CustomRules->SetOrAddParam(u"RuinsNearStars"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"RuinsPosition"sv) == 0));
                    CustomRules->SetOrAddParam(u"RuinsTargettingFull"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"RuinsTargetting"sv) == 0));
                    CustomRules->SetOrAddParam(u"RuinsUseShop"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"RuinsUseShop"sv) == 0));
                    CustomRules->SetOrAddParam(u"SpecialShipsInGame"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"SpecialShipsInGame"sv) == 0));
                    CustomRules->SetOrAddParam(u"AkrinMod"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"AkrinMod"sv)));
                    CustomRules->SetOrAddParam(u"NodeDropMod"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"NodeDropMod"sv)));
                    CustomRules->SetOrAddParam(u"DropValueMod"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"DropValueMod"sv)));
                    CustomRules->SetOrAddParam(u"ABDropValueMod"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"ABDropValueMod"sv)));
                    CustomRules->SetOrAddParam(u"ABHitpointsMod"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"ABHitpointsMod"sv)));
                    CustomRules->SetOrAddParam(u"ABDamageMod"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"ABDamageMod"sv)));
                    CustomRules->SetOrAddParam(u"ABattleRoyale"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"ABattleRoyale"sv) == 0));
                    CustomRules->SetOrAddParam(u"ABChangeEq"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"ABChangeEq"sv) == 0));
                    CustomRules->SetOrAddParam(u"AITolerateJunk"_wref.get(), pas::wide_int_to_str(GetExtendedOptionValue(u"AITolerateJunk"sv)));
                    CustomRules->SetOrAddParam(u"OldHyper"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"OldHyper"sv) == 0));
                    CustomRules->SetOrAddParam(u"PirateNodes"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"PirateNodes"sv) == 0));
                    CustomRules->SetOrAddParam(u"AIUseShops"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"AIUseShops"sv) == 0));
                    CustomRules->SetOrAddParam(u"DuplicateArts"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"DuplicateArts"sv) == 0));
                    CustomRules->SetOrAddParam(u"OldSpeedCalc"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"SpeedCalc"sv) == 0));
                    CustomRules->SetOrAddParam(u"OldMissileBonuses"_wref.get(), EC_Str::BoolToWideString(GetExtendedOptionValue(u"MissileBonuses"sv) == 0));
                }
                ToggleExtendedSettings(nullptr);
                {
                    GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"sv));
                    PanelSet->FreeOwnedChildren();
                }
                GR_DX::ReleaseAllTextureSurfaces();
                GR_Main::NewGameSettingsConfig->SaveTextFile(FileName.pchar(), true, false);
                if (Globals::NewGameGenerationThread != nullptr) {
                    pas::free(Globals::NewGameGenerationThread);
                    Globals::NewGameGenerationThread = nullptr;
                }
                Globals::NewGameGenerationThread = pas::construct_call<fGameSettings::TThreadCreateNewGame>(EC_Thread::TThreadEC_Create);
                Globals::NewGameGenerationThread->PlayerName = EC_Str::TrimWideString(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text);
                StartNewGameGeneration();
                GlobalsV::RequestedScreenId = GlobalsV::screenIntroduction;
                RequestClose(1);
            }
        }
    }

    void TfGameSettings2::CancelClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (GetByName(u"PanelExtended"sv)->Active) {
            ToggleExtendedSettings(nullptr);
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
            RequestClose(1);
        }
    }

    void TfGameSettings2::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_RIGHT)) {
            IncreaseAllDifficulties();
        }
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_LEFT)) {
            DecreaseAllDifficulties();
        }
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Key == WindowsSdk::VK_RETURN) {
                reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->ExecuteOnPressCode();
                ApplyClicked(nullptr);
            } else if (Key == WindowsSdk::VK_ESCAPE) {
                reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Cancel"sv))->ExecuteOnPressCode();
                CancelClicked(nullptr);
            }
        }
    }

    void TfGameSettings2::PlayerNameChanged(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Text{};
        if (Sender != nullptr) {
            PlayerNameEdited = true;
        }
        {
            std::uint8_t cpp_arg = static_cast<std::uint8_t>(ValidatePlayerName(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text) ^ 1);
            GI_GraphButton::TGraphButtonGI* cpp_arg_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv));
            cpp_arg_2->SetDisabled(cpp_arg);
        }
        Text = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.", aConst::OwnerInfo[aConst::RaceToOwner(PlayerRace)].InternalName, u".Char", pas::wide_int_to_str(CharacterPreset)}));
        {
            auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
            pas::WideString trimWideString = EC_Str::TrimWideString(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text);
            aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, std::move(trimWideString), textHighlightColorTag.get());
        }
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Info"sv))->SetText(Text);
    }

    // Also removes <>{} from the edit control and adjusts its caret; rejects empty names and unsupported glyphs.
    std::uint8_t TfGameSettings2::ValidatePlayerName(pas::WideString Name) {
        std::int32_t I{};
        std::uint8_t Result = true;
        Name = EC_Str::RemoveWideStringChars(pas::view(Name), u"<>{}"_w);
        {
            GI_Edit::TEditGI* PlayerName = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv));
            if (PlayerName->Text != Name) {
                I = PlayerName->CaretPosition;
                PlayerName->SetText(Name);
                if (I > 0) {
                    PlayerName->SetCaretPosition(I - 1);
                } else {
                    PlayerName->SetCaretPosition(0);
                }
            }
        }
        Name = EC_Str::TrimWideString(Name);
        if (Name.length() < 1) {
            Result = false;
        }
        if (Result) {
            GI_Edit::TEditGI* PlayerName_2 = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv));
            {
                const std::int32_t cpp_last = Name.length() - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        if (!PlayerName_2->HasGlyph(Name.read(I + 1))) {
                            Result = false;
                            break;
                        }
                    }
                }
            }
        }
        GI_Label::TLabelGI* HelpLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LabelHelp"sv));
        if (PlayerNameValid && static_cast<std::uint8_t>(Result ^ 1)) {
            HelpLabel->SetText(GR_Main::LookupLocalizedTextByKey(u"FormGameSet2.Common.ErrorName"_wref.get()));
            HelpLabel->SetActive(true);
        }
        if (Result && static_cast<std::uint8_t>(PlayerNameValid ^ 1)) {
            HelpLabel->SetActive(false);
        }
        PlayerNameValid = Result;
        return Result;
    }

    void TfGameSettings2::ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Show) {
        GI_Label::TLabelGI* HelpLabel{};
        if (PlayerNameValid) {
            HelpLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LabelHelp"sv));
            if (!ValidatePlayerName(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text)) {
                HelpLabel->SetText(GR_Main::LookupLocalizedTextByKey(u"FormGameSet2.Common.ErrorName"_wref.get()));
                HelpLabel->SetActive(true);
            } else {
                if (Sender->HelpText == u"" || Sender->IsOccludedAtPoint(GetCursorPoint())) {
                    Show = false;
                }
                HelpLabel->SetActive(Show);
                if (HelpLabel->Active && Sender == GetByName(u"Ok"sv) && static_cast<std::uint8_t>(ValidatePlayerName(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text) ^ 1)) {
                    HelpLabel->SetText(GR_Main::LookupLocalizedTextByKey(u"FormGameSet2.Common.ErrorName"_wref.get()));
                } else {
                    HelpLabel->SetText(Sender->HelpText);
                }
            }
        }
    }

    void TfGameSettings2::HelpMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        ShowControlHelp(Sender, true);
    }

    void TfGameSettings2::HelpMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        ShowControlHelp(Sender, false);
    }

    void TfGameSettings2::SelectMusic() {
        GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
    }

    void TfGameSettings2::ExtendedSettingsPressed(GI_MessageLoop::TObjectGI* Sender) {
    }

    void TfGameSettings2::ToggleExtendedSettings(GI_MessageLoop::TObjectGI* Sender) {
        {
            GI_MessageLoop::TObjectGI* PanelExtended = GetByName(u"PanelExtended"sv);
            PanelExtended->SetActive(false);
        }
        {
            GI_MessageLoop::TObjectGI* PanelChar = GetByName(u"PanelChar"sv);
            PanelChar->SetActive(true);
        }
        {
            GI_MessageLoop::TObjectGI* PanelLevels = GetByName(u"PanelLevels"sv);
            PanelLevels->SetActive(true);
        }
        {
            GI_MessageLoop::TObjectGI* PanelSkills = GetByName(u"PanelSkills"sv);
            PanelSkills->SetActive(true);
        }
    }

    void TfGameSettings2::ResetExtendedSettingsClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t OldGroup = ActiveExtendedGroup;
        ActiveExtendedGroup = 0;
        SetExtendedOptionValue(u"KlingStrength"sv, 0);
        SetExtendedOptionValue(u"KlingAggro"sv, 0);
        SetExtendedOptionValue(u"KlingSpawn"sv, 0);
        SetExtendedOptionValue(u"PirateAggro"sv, 0);
        SetExtendedOptionValue(u"CoalAggro"sv, 8);
        SetExtendedOptionValue(u"ExtraInventions"sv, 0);
        SetExtendedOptionValue(u"ExtraRangers"sv, 0);
        SetExtendedOptionValue(u"ZeroStartExp"sv, 1);
        SetExtendedOptionValue(u"KlingRacialWeapons"sv, 1);
        SetExtendedOptionValue(u"MaxRangeMissiles"sv, 1);
        SetExtendedOptionValue(u"MaxGrowth"sv, 0);
        ActiveExtendedGroup = 1;
        SetExtendedOptionValue(u"AsteroidMod"sv, 8);
        SetExtendedOptionValue(u"SunDamageMod"sv, 8);
        SetExtendedOptionValue(u"AgPlanets"sv, 5);
        SetExtendedOptionValue(u"MiPlanets"sv, 5);
        SetExtendedOptionValue(u"InPlanets"sv, 5);
        SetExtendedOptionValue(u"StartCenter"sv, 1);
        ActiveExtendedGroup = 2;
        SetExtendedOptionValue(u"RndType"sv, 1);
        SetExtendedOptionValue(u"EqKnowledgeType"sv, 1);
        SetExtendedOptionValue(u"RuinsPosition"sv, 1);
        SetExtendedOptionValue(u"RuinsTargetting"sv, 1);
        SetExtendedOptionValue(u"RuinsUseShop"sv, 1);
        SetExtendedOptionValue(u"SpecialShipsInGame"sv, 1);
        SetExtendedOptionValue(u"AkrinMod"sv, 30);
        SetExtendedOptionValue(u"NodeDropMod"sv, 8);
        SetExtendedOptionValue(u"DropValueMod"sv, 8);
        SetExtendedOptionValue(u"ABDropValueMod"sv, 8);
        SetExtendedOptionValue(u"ABHitpointsMod"sv, 8);
        SetExtendedOptionValue(u"ABDamageMod"sv, 8);
        SetExtendedOptionValue(u"ABattleRoyale"sv, 1);
        SetExtendedOptionValue(u"ABChangeEq"sv, 1);
        SetExtendedOptionValue(u"AITolerateJunk"sv, 7);
        SetExtendedOptionValue(u"OldHyper"sv, 1);
        SetExtendedOptionValue(u"PirateNodes"sv, 1);
        SetExtendedOptionValue(u"AIUseShops"sv, 1);
        SetExtendedOptionValue(u"DuplicateArts"sv, 1);
        SetExtendedOptionValue(u"SpeedCalc"sv, 1);
        SetExtendedOptionValue(u"MissileBonuses"sv, 1);
        ActiveExtendedGroup = OldGroup;
    }

    void TfGameSettings2::ExtendedGroupClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Group = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv))->SetDown(Group == 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv))->SetDown(Group == 1);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv))->SetDown(Group == 2);
        if (ActiveExtendedGroup != Group) {
            ActiveExtendedGroup = Group;
            RefreshExtendedGroup();
        }
    }

    void TfGameSettings2::RefreshExtendedGroup() {
        std::int32_t I{};
        {
            GI_GraphButton::TGraphButtonGI* ButGroup0 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"sv));
            ButGroup0->SetDown(ActiveExtendedGroup == 0);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup1 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup1"sv));
            ButGroup1->SetDown(ActiveExtendedGroup == 1);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButGroup2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup2"sv));
            ButGroup2->SetDown(ActiveExtendedGroup == 2);
        }
        for (I = 0; I <= 3; ++I) {
            GI_Panel::TPanelGI* cpp_with_4 = ExtendedGroupPanels[I];
            cpp_with_4->SetActive(I == ActiveExtendedGroup);
        }
        {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"sv));
            PanelSet->SetScrollOffset(ClassesImports::Point(0, 0));
            PanelSet->UpdateScrollRanges();
            PanelSet->SetVerticalScrollbarEnabled(ExtendedGroupNextY[ActiveExtendedGroup] > PanelSet->ClientSize.Y);
        }
    }

    GI_Label::TLabelGI* TfGameSettings2::AddExtendedOptionLabel(pas::WideString OptionName, pas::WideString Caption, std::uint8_t UnusedFlag) {
        CurrentExtendedOption = std::move(OptionName);
        if (ExtendedGroupNextY[BuildExtendedGroup] != 0) {
            GI_Image::TImageGI* cpp_with = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, ExtendedGroupPanels[BuildExtendedGroup]);
            cpp_with->SetImagePath(u"GI,Bm.FormOptions2.2Line"_w);
            cpp_with->SetPosition(ClassesImports::Point(0, ExtendedGroupNextY[BuildExtendedGroup]));
            {
                std::int32_t cpp_arg = cpp_with->GetContentSize().Y + 2;
                std::int32_t x = ExtendedGroupPanels[BuildExtendedGroup]->ClientSize.X;
                cpp_with->SetSize(ClassesImports::Point(x, cpp_arg));
            }
            cpp_with->SetImageKindX(GI_Main::ikxLeftFill);
            ExtendedGroupNextY[BuildExtendedGroup] = ExtendedGroupNextY[BuildExtendedGroup] + cpp_with->ClientSize.Y;
        }
        GI_Label::TLabelGI* Result = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ExtendedGroupPanels[BuildExtendedGroup]);
        Result->SetFontName(GlobalsV::NormalFontName);
        Result->SetPositionModeW(false);
        Result->SetPosition(ClassesImports::Point(0, ExtendedGroupNextY[BuildExtendedGroup]));
        Result->SetSize(ClassesImports::Point(ExtendedGroupPanels[BuildExtendedGroup]->ClientSize.X, 1));
        Result->SetTextAlignX(GI_Main::taxLeft);
        Result->SetTextAlignY(GI_Main::tayAuto);
        Result->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(205, 205, 205));
        Result->SetText(Caption);
        Result->HelpText = Caption;
        Result->SetTextAlignY(GI_Main::tayTop);
        Result->SetSize(ClassesImports::Point(Result->ClientSize.X, Result->ClientSize.Y + 1));
        ExtendedGroupNextY[BuildExtendedGroup] += 2;
        return Result;
    }

    void TfGameSettings2::AddExtendedOptionChoice(std::int32_t Value, pas::WideString Caption, std::uint8_t Selected, std::uint8_t Disabled) {
        std::int32_t Indent = GR_Main::GiScalePixelsEx(50, 30);
        GI_Label::TLabelGI* CaptionLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ExtendedGroupPanels[BuildExtendedGroup]);
        CaptionLabel->SetFontName(GlobalsV::NormalFontName);
        CaptionLabel->SetPositionModeW(false);
        CaptionLabel->SetPosition(ClassesImports::Point(0, ExtendedGroupNextY[BuildExtendedGroup]));
        CaptionLabel->SetSize(ClassesImports::Point(ExtendedGroupPanels[BuildExtendedGroup]->ClientSize.X - Indent, 1));
        CaptionLabel->SetTextAlignX(GI_Main::taxRight);
        CaptionLabel->SetTextAlignY(GI_Main::tayAuto);
        if (Selected) {
            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 234, 118));
        } else {
            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(205, 205, 205));
        }
        CaptionLabel->SetText(Caption);
        if (!Disabled) {
            CaptionLabel->LeftButtonDownCallback = pas::bind_method<&TfGameSettings2::ExtendedChoiceMouseDown>(this);
            CaptionLabel->MouseEnterCallback = pas::bind_static_method<&TfGameSettings2::IronWillMouseEnter>(this);
            CaptionLabel->MouseLeaveCallback = pas::bind_static_method<&TfGameSettings2::IronWillMouseLeave>(this);
        }
        CaptionLabel->SetTextAlignY(GI_Main::tayCenterEx);
        GI_Image::TImageGI* ChoiceImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, ExtendedGroupPanels[BuildExtendedGroup]);
        if (Disabled) {
            ChoiceImage->SetImagePath(u"GI,Bm.FormOptions2.2SwitchH"_w);
        } else if (!Selected) {
            ChoiceImage->SetImagePath(u"GI,Bm.FormOptions2.2SwitchN"_w);
        } else {
            ChoiceImage->SetImagePath(u"GI,Bm.FormOptions2.2SwitchD"_w);
        }
        {
            std::int32_t cpp_right = ChoiceImage->GetContentSize().X;
            std::int32_t cpp_arg = ExtendedGroupPanels[BuildExtendedGroup]->ClientSize.X - cpp_right - Indent;
            std::int32_t cpp_arg_2 = ExtendedGroupNextY[BuildExtendedGroup];
            ChoiceImage->SetPosition(ClassesImports::Point(cpp_arg, cpp_arg_2));
        }
        ChoiceImage->SetSize(ChoiceImage->GetContentSize());
        ChoiceImage->SetImageKindY(GI_Main::ikyCenter);
        if (!Disabled) {
            ChoiceImage->LeftButtonDownCallback = pas::bind_method<&TfGameSettings2::ExtendedChoiceMouseDown>(this);
            ChoiceImage->MouseEnterCallback = pas::bind_static_method<&TfGameSettings2::IronWillMouseEnter>(this);
            ChoiceImage->MouseLeaveCallback = pas::bind_static_method<&TfGameSettings2::IronWillMouseLeave>(this);
        }
        if (!Disabled) {
            ChoiceImage->SetName(CurrentExtendedOption);
        }
        ChoiceImage->UserValue = Value;
        CaptionLabel->SetSize(ClassesImports::Point(CaptionLabel->ClientSize.X - ChoiceImage->ClientSize.X - 10, std::max<std::int32_t>(CaptionLabel->ClientSize.Y, ChoiceImage->ClientSize.Y)));
        ChoiceImage->SetPosition(ClassesImports::Point(ChoiceImage->LocalPosition.X, ChoiceImage->LocalPosition.Y + (std::max<std::int32_t>(CaptionLabel->ClientSize.Y, ChoiceImage->ClientSize.Y) - CaptionLabel->ClientSize.Y) / 2));
        ExtendedGroupNextY[BuildExtendedGroup] = ExtendedGroupNextY[BuildExtendedGroup] + CaptionLabel->ClientSize.Y + GR_Main::GiScalePixels(5);
        CaptionLabel->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(ChoiceImage));
    }

    void TfGameSettings2::ExtendedChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (!(pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr)) {
            Sender = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        }
        GI_MessageLoop::TObjectGI* Control = ExtendedGroupPanels[ActiveExtendedGroup]->FirstChild;
        while (Control != nullptr) {
            if (Control->ControlName == Sender->ControlName) {
                if (Control == Sender) {
                    pas::checked_cast<GI_Image::TImageGI*>(Control)->SetImagePath(u"GI,Bm.FormOptions2.2SwitchD"_w);
                } else {
                    pas::checked_cast<GI_Image::TImageGI*>(Control)->SetImagePath(u"GI,Bm.FormOptions2.2SwitchN"_w);
                }
            }
            Control = Control->NextSibling;
        }
        if (Point.X != -1000 || Point.Y != -1000) {
            GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
        }
    }

    // Invokes Callback immediately with the new slider.
    void TfGameSettings2::AddExtendedOptionSlider(GI_Label::TLabelGI* ValueLabel, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Position, std::int32_t UnusedStep, TNewGameSliderEvent Callback) {
        GI_CountBar::TCountBarGI* Slider = pas::construct_call<GI_CountBar::TCountBarGI>(GI_CountBar::TCountBarGI_Create, ExtendedGroupPanels[BuildExtendedGroup]);
        ExtendedGroupNextY[BuildExtendedGroup] = ExtendedGroupNextY[BuildExtendedGroup];
        Slider->SetPositionModeW(false);
        Slider->SetSize(ClassesImports::Point(199, 20));
        Slider->SetPosition(ClassesImports::Point(ExtendedGroupPanels[BuildExtendedGroup]->ClientSize.X - Slider->ClientSize.X, ExtendedGroupNextY[BuildExtendedGroup]));
        Slider->DecreaseButton->SetKind(GI_GraphButton::gbkDisable);
        Slider->DecreaseButton->SetImageNormalPath(u"GI,Bm.FormOptions2.2TrackLeftN"_wref.get());
        Slider->DecreaseButton->SetImageNormalActivePath(u"GI,Bm.FormOptions2.2TrackLeftA"_wref.get());
        Slider->DecreaseButton->SetImageDownPath(u"GI,Bm.FormOptions2.2TrackLeftD"_wref.get());
        Slider->DecreaseButton->SetImageDisabledPath(u"GI,Bm.FormOptions2.2TrackLeftH"_wref.get());
        Slider->DecreaseButton->EnterSound = u"Sound.ButtonEnter"_w;
        Slider->DecreaseButton->LeaveSound = u"Sound.ButtonLeave"_w;
        Slider->DecreaseButton->ClickSound = u"Sound.ButtonClick"_w;
        Slider->IncreaseButton->SetKind(GI_GraphButton::gbkDisable);
        Slider->IncreaseButton->SetImageNormalPath(u"GI,Bm.FormOptions2.2TrackRightN"_wref.get());
        Slider->IncreaseButton->SetImageNormalActivePath(u"GI,Bm.FormOptions2.2TrackRightA"_wref.get());
        Slider->IncreaseButton->SetImageDownPath(u"GI,Bm.FormOptions2.2TrackRightD"_wref.get());
        Slider->IncreaseButton->SetImageDisabledPath(u"GI,Bm.FormOptions2.2TrackRightH"_wref.get());
        Slider->IncreaseButton->EnterSound = u"Sound.ButtonEnter"_w;
        Slider->IncreaseButton->LeaveSound = u"Sound.ButtonLeave"_w;
        Slider->IncreaseButton->ClickSound = u"Sound.ButtonClick"_w;
        Slider->MarkerImage->SetImagePath(u"GI,Bm.FormOptions2.2TrackUp"_w);
        {
            Types::TPoint contentSize = Slider->MarkerImage->GetContentSize();
            GI_Image::TImageGI* markerImage = Slider->MarkerImage;
            markerImage->SetSize(contentSize);
        }
        Slider->MarkerImage->SetOrigin(EC_Struct::HalfPoint(Slider->MarkerImage->ClientSize));
        Slider->AfterThumbImage->SetImagePath(u"GI,Bm.FormOptions2.2TrackLeft"_w);
        Slider->BeforeThumbImage->SetImagePath(u"GI,Bm.FormOptions2.2TrackRight"_w);
        Slider->ThumbButton->SetImageNormalPath(u"GI,Bm.FormOptions2.2TrackPol"_wref.get());
        Slider->ThumbButton->SetImageNormalActivePath(u"GI,Bm.FormOptions2.2TrackPol"_wref.get());
        Slider->ThumbButton->SetImageDownPath(u"GI,Bm.FormOptions2.2TrackPol"_wref.get());
        Slider->PositionChangedCallback = Callback;
        Slider->UpdateLayout();
        Slider->SetRange(Minimum, Maximum);
        Slider->SetPositionInternal(Position);
        Slider->SetName(CurrentExtendedOption);
        Slider->UserIndex = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(ValueLabel));
        ExtendedGroupNextY[BuildExtendedGroup] = ExtendedGroupNextY[BuildExtendedGroup] + Slider->ClientSize.Y + GR_Main::GiScalePixels(6);
        Callback(Slider);
    }

    // Searches only the active extended group; raises when no value is found.
    std::int32_t TfGameSettings2::GetExtendedOptionValue(const std::u16string_view& OptionName) {
        std::int32_t Result = 0;
        GI_MessageLoop::TObjectGI* Control = ExtendedGroupPanels[ActiveExtendedGroup]->FirstChild;
        while (Control != nullptr) {
            if (pas::view(Control->ControlName) == OptionName) {
                if (GI_Image::TImageGI* imageGI = pas::class_cast_if<GI_Image::TImageGI*>(Control)) {
                    if (imageGI->GetImagePath() == u"GI,Bm.FormOptions2.2SwitchD") {
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

    // Searches only the active extended group; missing options are ignored.
    void TfGameSettings2::SetExtendedOptionValue(const std::u16string_view& OptionName, std::int32_t Value) {
        GI_MessageLoop::TObjectGI* Control = ExtendedGroupPanels[ActiveExtendedGroup]->FirstChild;
        while (Control != nullptr) {
            if (pas::view(Control->ControlName) == OptionName) {
                if (pas::class_cast_if<GI_Image::TImageGI*>(Control) != nullptr && Control->UserValue == Value && pas::assigned(Control->LeftButtonDownCallback)) {
                    ExtendedChoiceMouseDown(Control, 0u, ClassesImports::Point(-1000, -1000));
                    return;
                }
                if (GI_CountBar::TCountBarGI* countBarGI = pas::class_cast_if<GI_CountBar::TCountBarGI*>(Control)) {
                    countBarGI->SetPosition_2(Value);
                    return;
                }
            }
            Control = Control->NextSibling;
        }
    }

    void TfGameSettings2::FormatExtendedInteger(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender->UserIndex != 0) {
            GI_Label::TLabelGI* cpp_with = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            cpp_with->SetText(pas::concat_wide({cpp_with->HelpText, aMyFunction::TextHighlightColorTag, u" ", pas::wide_int_to_str(reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position), aMyFunction::EndColorTag}));
        }
    }

    void TfGameSettings2::FormatExtendedAutoPercent(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Value{};
        if (Sender->UserIndex != 0) {
            GI_Label::TLabelGI* cpp_with = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            Value = reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position;
            if (Value == 0) {
                const pas::WideString& cpp_arg = pas::concat_wide({cpp_with->HelpText, aMyFunction::TextHighlightColorTag, u" ", aConst::LocalizedText(u"FormGameSet2.Extended.HelpAuto"_wref.get()), aMyFunction::EndColorTag});
                GI_Label::TLabelGI* cpp_arg_2 = cpp_with;
                cpp_arg_2->SetText(cpp_arg);
            } else if (Value <= 25) {
                cpp_with->SetText(pas::concat_wide({cpp_with->HelpText, aMyFunction::TextHighlightColorTag, u" ", pas::wide_int_to_str(50 + System::Round((Value - 1) * 6.25L)), u"%", aMyFunction::EndColorTag}));
            } else {
                cpp_with->SetText(pas::concat_wide({cpp_with->HelpText, aMyFunction::OrangeColorTag, u" ", pas::wide_int_to_str(50 + System::Round((Value - 1) * 6.25L)), u"%", aMyFunction::EndColorTag}));
            }
        }
    }

    void TfGameSettings2::FormatExtendedDifficultyPercent(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Value{};
        if (Sender->UserIndex != 0) {
            GI_Label::TLabelGI* cpp_with = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            Value = reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position;
            cpp_with->SetText(pas::concat_wide({cpp_with->HelpText, aMyFunction::TextHighlightColorTag, u" ", pas::wide_int_to_str(50 + System::Round(Value * 6.25L)), u"%", aMyFunction::EndColorTag}));
        }
    }

    void TfGameSettings2::FormatExtendedPercent(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Value{};
        if (Sender->UserIndex != 0) {
            GI_Label::TLabelGI* cpp_with = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserIndex)));
            Value = reinterpret_cast<GI_CountBar::TCountBarGI*>(Sender)->Position;
            cpp_with->SetText(pas::concat_wide({cpp_with->HelpText, aMyFunction::TextHighlightColorTag, u" ", pas::wide_int_to_str(Value), u"%", aMyFunction::EndColorTag}));
        }
    }

    void TfGameSettings2::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (GetByName(u"PanelExtended"sv)->Active) {
            GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"sv));
            if (Delta == WindowsSdk::WHEEL_DELTA) {
                PanelSet->VerticalScrollBar->SetPosition_2(PanelSet->VerticalScrollBar->Position - PanelSet->VerticalScrollBar->SmallChange);
            } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
                PanelSet->VerticalScrollBar->SetPosition_2(PanelSet->VerticalScrollBar->Position + PanelSet->VerticalScrollBar->SmallChange);
            }
        }
    }

    void TfGameSettings2::IncreaseAllDifficulties() {
        std::int32_t I{};
        for (I = 0; I <= 7; ++I) {
            if (aConst::MaximumNewGameDifficulty > DifficultyLevels[I]) {
                ++DifficultyLevels[I];
            }
        }
        GI_MessageLoop::TObjectGI* Button = FindControlByPath(u"KeyArrowRight"_wref.get());
        if (Button != nullptr && pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Button) != nullptr) {
            static_cast<GI_GraphButton::TGraphButtonGI*>(Button)->ExecuteOnPressCode();
        }
        RefreshDifficultyHelp();
        RefreshDifficulty();
    }

    void TfGameSettings2::DecreaseAllDifficulties() {
        std::int32_t I{};
        for (I = 0; I <= 7; ++I) {
            if (DifficultyLevels[I] > 0) {
                --DifficultyLevels[I];
            }
        }
        GI_MessageLoop::TObjectGI* Button = FindControlByPath(u"KeyArrowLeft"_wref.get());
        if (Button != nullptr && pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Button) != nullptr) {
            static_cast<GI_GraphButton::TGraphButtonGI*>(Button)->ExecuteOnPressCode();
        }
        RefreshDifficultyHelp();
        RefreshDifficulty();
    }

    void TfGameSettings2::RefreshDifficultyHelp() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t First{};
        GI_Label::TLabelGI* CaptionLabel{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 8); cpp_range.next(I); ) {
            First = std::max<std::int32_t>(DifficultyLevels[I - 1] - 3, 0);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 3); cpp_range_2.next(J); ) {
                CaptionLabel = pas::checked_cast<GI_Label::TLabelGI*>(FindControlByPath(pas::concat_wide({u"NameGroup", EC_Str::IntToWideString(I), u"Level", EC_Str::IntToWideString(J)})));
                if (CaptionLabel != nullptr) {
                    switch (First + J) {
                        case 0: {
                            CaptionLabel->SetText(aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.NameGroup", EC_Str::IntToWideString(I), u"Easy"})));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0));
                            break;
                        }
                        case 1: {
                            CaptionLabel->SetText(aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.NameGroup", EC_Str::IntToWideString(I), u"Normal"})));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(254, 255, 255));
                            break;
                        }
                        case 2: {
                            CaptionLabel->SetText(aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.NameGroup", EC_Str::IntToWideString(I), u"Hard"})));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 240, 100));
                            break;
                        }
                        case 3: {
                            CaptionLabel->SetText(aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.NameGroup", EC_Str::IntToWideString(I), u"Expert"})));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 166, 0));
                            break;
                        }
                        default: {
                            CaptionLabel->SetText(pas::concat_wide({EC_Str::IntToWideString((First + J + 1) * 50), u"%"}));
                            CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, System::Round(aMyFunction::RemapClamped(First + J, 3.0, 9.0, 166.0, 0.0)), 0));
                            break;
                        }
                    }
                    {
                        GI_GraphButton::TGraphButtonGI* cpp_with = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Level", SysUtils::IntToStr(J + 1), "_", SysUtils::IntToStr(I - 1)})))));
                        cpp_with->UserValue = I - 1;
                        cpp_with->UserIndex = First + J;
                        switch (First + J) {
                            case 0: {
                                cpp_with->HelpText = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Easy"}));
                                break;
                            }
                            case 1: {
                                cpp_with->HelpText = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Normal"}));
                                break;
                            }
                            case 2: {
                                cpp_with->HelpText = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Hard"}));
                                break;
                            }
                            case 3: {
                                cpp_with->HelpText = aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Expert"}));
                                break;
                            }
                            default: {
                                cpp_with->HelpText = pas::concat_wide({aConst::LocalizedColorText(pas::concat_wide({u"FormGameSet2.Common.HelpGroup", EC_Str::IntToWideString(I), u"Expert"})), u"+"});
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    void TfGameSettings2::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        aScript::ExecuteGameplayUiCode(Block, Key);
    }

} // namespace fGameSettings2
