#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace fGameSettings2 {
    struct TfGameSettings2;

    using TNewGameSliderEvent = GI_MessageLoop::TObjectNotifyEventGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfGameSettings2 : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfGameSettings2, GI_MessageLoop::TMessageLoopGI, "TfGameSettings2", 364)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void AddIronWillChoice(std::int32_t Value, std::int32_t X, std::int32_t Y, pas::WideString Caption, pas::WideString Help, std::uint8_t Selected, std::uint8_t Disabled);
        void IronWillMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        static void IronWillMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void IronWillMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void GeneratePlayerName();
        void PlayerNameMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ToggleLevelPanel(GI_MessageLoop::TObjectGI* Sender);
        void AnimateLevelPanel(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void RaceClicked(GI_MessageLoop::TObjectGI* Sender);
        void CharacterPresetClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshPortrait();
        void PreviousPortraitClicked(GI_MessageLoop::TObjectGI* Sender);
        void NextPortraitClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshStartingSkills();
        void StartingSkillClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshStartingItems();
        void StartingItemClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshDifficulty();
        void DifficultyComponentClicked(GI_MessageLoop::TObjectGI* Sender);
        void DifficultyPresetClicked(GI_MessageLoop::TObjectGI* Sender);
        void CustomDifficultyClicked(GI_MessageLoop::TObjectGI* Sender);
        void CustomDifficultyMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void StartNewGameGeneration();
        void ApplyClicked(GI_MessageLoop::TObjectGI* Sender);
        void CancelClicked(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void PlayerNameChanged(GI_MessageLoop::TObjectGI* Sender);
        // Also removes <>{} from the edit control and adjusts its caret; rejects empty names and unsupported glyphs.
        std::uint8_t ValidatePlayerName(pas::WideString Name);
        void ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Show);
        void HelpMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void HelpMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void SelectMusic() override;
        static void ExtendedSettingsPressed(GI_MessageLoop::TObjectGI* Sender);
        void ToggleExtendedSettings(GI_MessageLoop::TObjectGI* Sender);
        void ResetExtendedSettingsClicked(GI_MessageLoop::TObjectGI* Sender);
        void ExtendedGroupClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshExtendedGroup();
        GI_Label::TLabelGI* AddExtendedOptionLabel(pas::WideString OptionName, pas::WideString Caption, std::uint8_t UnusedFlag);
        void AddExtendedOptionChoice(std::int32_t Value, pas::WideString Caption, std::uint8_t Selected, std::uint8_t Disabled);
        void ExtendedChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        // Invokes Callback immediately with the new slider.
        void AddExtendedOptionSlider(GI_Label::TLabelGI* ValueLabel, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Position, std::int32_t UnusedStep, TNewGameSliderEvent Callback);
        // Searches only the active extended group; raises when no value is found.
        std::int32_t GetExtendedOptionValue(const std::u16string_view& OptionName);
        // Searches only the active extended group; missing options are ignored.
        void SetExtendedOptionValue(const std::u16string_view& OptionName, std::int32_t Value);
        static void FormatExtendedInteger(GI_MessageLoop::TObjectGI* Sender);
        static void FormatExtendedAutoPercent(GI_MessageLoop::TObjectGI* Sender);
        static void FormatExtendedDifficultyPercent(GI_MessageLoop::TObjectGI* Sender);
        static void FormatExtendedPercent(GI_MessageLoop::TObjectGI* Sender);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void IncreaseAllDifficulties();
        void DecreaseAllDifficulties();
        void RefreshDifficultyHelp();
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        std::int32_t CollapsedLevelPanelTop;
        std::int32_t LevelPanelTop;
        GI_MessageLoop::PCallbackTimerGI LevelPanelTimer;
        aGalaxyStruct::TOwnerId PlayerRace;
        std::uint8_t cpp_padding[3];
        std::int32_t CharacterPreset;
        std::int32_t CaptainPortraitIndex;
        pas::Array<std::int32_t, 0, 4> LastPortraitByRace;
        pas::Array<aGalaxyStruct::TPilotSkill, 0, 1> StartingSkills;
        std::uint8_t cpp_padding_2[2];
        std::int32_t SelectedSkillSlot;
        // // Choice values are 1..12; ItemTypeByChoice is zero-based.
        pas::Array<std::int32_t, 0, 1> StartingItemChoices;
        std::int32_t SelectedItemSlot;
        pas::Array<std::uint8_t, 0, 11> ItemTypeByChoice;
        std::uint8_t DifficultyPreset;
        aGalaxyStruct::TGalaxyDifficultyLevels DifficultyLevels;
        std::uint8_t PlayerNameEdited;
        std::uint8_t PlayerNameValid;
        std::uint8_t cpp_padding_3[1];
        GI_Image::TImageGI* IronWillImage;
        GI_Label::TLabelGI* IronWillLabel;
        std::uint8_t IronWill;
        std::uint8_t cpp_padding_4[3];
        std::int32_t ActiveExtendedGroup;
        pas::Array<GI_Panel::TPanelGI*, 0, 3> ExtendedGroupPanels;
        pas::Array<std::int32_t, 0, 3> ExtendedGroupNextY;
        std::int32_t BuildExtendedGroup;
        pas::WideString CurrentExtendedOption;
        pas::Array<std::int32_t, 0, 2> ExtendedGroupButtonTops;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fGameSettings2
