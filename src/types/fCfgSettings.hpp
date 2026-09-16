#pragma once
#include "runtime_support.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace fCfgSettings {
    struct TfCfgSettings;

    using TOptionSliderEvent = GI_MessageLoop::TObjectNotifyEventGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfCfgSettings : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfCfgSettings, GI_MessageLoop::TMessageLoopGI, "TfCfgSettings", 320)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void MainPanelMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Show);
        void GroupClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshVisibleGroup();
        GI_Label::TLabelGI* AddOptionLabel(pas::WideString OptionName, pas::WideString Caption, std::uint8_t UnusedFlag);
        void AddOptionChoice(std::int32_t Value, pas::WideString Caption, std::uint8_t Selected, std::uint8_t Disabled);
        void OptionChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        static void OptionChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void OptionChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void AddOptionSlider(GI_Label::TLabelGI* ValueLabel, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Position, std::int32_t UnusedStep, TOptionSliderEvent Callback);
        std::uint8_t HasOptionValue(pas::WideString OptionName);
        std::int32_t GetOptionValue(pas::WideString OptionName);
        void SetOptionValue(pas::WideString OptionName, std::int32_t Value);
        static void FormatResolution(GI_MessageLoop::TObjectGI* Sender);
        static void FormatRobotResolution(GI_MessageLoop::TObjectGI* Sender);
        static void FormatRobotFsaaSamples(GI_MessageLoop::TObjectGI* Sender);
        void PreviewBrightness(GI_MessageLoop::TObjectGI* Sender);
        void PreviewContrast(GI_MessageLoop::TObjectGI* Sender);
        static void FormatInteger(GI_MessageLoop::TObjectGI* Sender);
        static void FormatTurnSaveStep(GI_MessageLoop::TObjectGI* Sender);
        static void FormatForsageDeactivatePercent(GI_MessageLoop::TObjectGI* Sender);
        void HighPresetClicked(GI_MessageLoop::TObjectGI* Sender);
        void MediumPresetClicked(GI_MessageLoop::TObjectGI* Sender);
        void LowPresetClicked(GI_MessageLoop::TObjectGI* Sender);
        void AutoPresetClicked(GI_MessageLoop::TObjectGI* Sender);
        void CancelClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshModeUi();
        void ModeMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void ModeMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ModeLeaveTimerTick(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ModeMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ModeMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void ApplyClicked(GI_MessageLoop::TObjectGI* Sender);
        static GI_MessageLoop::TObjectGI* CreateWarningImage(GI_Label::TLabelGI* Owner, EC_CacheFont::PFontObjectEC Item);
        void SelectMusic() override;
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        std::int32_t ActiveGroupIndex;
        std::int32_t BuildGroupIndex;
        pas::WideString CurrentOptionName;
        pas::Array<GI_Panel::TPanelGI*, 0, 5> GroupPanels;
        pas::Array<std::int32_t, 0, 5> GroupNextY;
        std::int32_t SettingsMode;
        std::int32_t ModeButtonState;
        WindowsSdk::TPoint ModeLeftPosition;
        WindowsSdk::TPoint ModeRightPosition;
        pas::Array<std::int32_t, 0, 3> GroupButtonTops;
        GI_MessageLoop::PCallbackTimerGI ModeLeaveTimer;
        std::int32_t RobotAvailability;
        std::uint8_t HasInstalledPackages;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fCfgSettings
