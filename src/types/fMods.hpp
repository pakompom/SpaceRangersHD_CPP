#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_GraphButton {
    struct TGraphButtonGI;

} // namespace GI_GraphButton

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace fMods {
    struct TfModsManager;

    // Native RTTI name.
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfModsManager : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfModsManager, GI_MessageLoop::TMessageLoopGI, "TfModsManager", 248)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void TabClick(GI_MessageLoop::TObjectGI* Sender);
        void UpdateTabDisplay();
        void ValidateSelection();
        void ClearSelectionClick(GI_MessageLoop::TObjectGI* Sender);
        void SelectAll();
        void DeselectAll();
        void CloseClick(GI_MessageLoop::TObjectGI* Sender);
        void ApplyClick(GI_MessageLoop::TObjectGI* Sender);
        void KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        static void SwitchMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void SwitchMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void SwitchMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SetModSelected(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Value);
        static void UpdateModSwitch(GI_MessageLoop::TObjectGI* Sender);
        void ShowInfoClick(GI_MessageLoop::TObjectGI* Sender);
        void ShowProblemsClick(GI_MessageLoop::TObjectGI* Sender);
        void SelectMusic() override;
        std::int32_t SelectedTab;
        std::int32_t TabCount;
        pas::DynArray<GI_GraphButton::TGraphButtonGI*> TabButtons;
        pas::DynArray<GI_Panel::TPanelGI*> TabPanels;
        pas::DynArray<std::int32_t> TabHeights;
        pas::DynArray<std::int32_t> SelectedCounts;
        pas::DynArray<std::int32_t> WarningCounts;
        pas::DynArray<std::int32_t> ErrorCounts;
        pas::DynArray<std::uint8_t> InvalidSelections;
        std::uint8_t NeedsValidation;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fMods
