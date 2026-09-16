#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_GraphButton {
    struct TGraphButtonGI;

} // namespace GI_GraphButton

namespace GI_PanelScrollBar {
    struct TPanelScrollBarGI;

} // namespace GI_PanelScrollBar

namespace fListBox {
    struct TfListBox;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfListBox : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfListBox, GI_MessageLoop::TMessageLoopGI, "TfListBox", 240)
        void p_destroy() override;
        void OnOpen() override;
        void OnClose() override;
        void PopulateChoices();
        void AcceptClicked(GI_MessageLoop::TObjectGI* Sender);
        void CancelClicked(GI_MessageLoop::TObjectGI* Sender);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ChoiceDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ProcessCallbackTimers() override;
        // Reset to -1 on opening; set only on acceptance.
        std::int32_t SelectedIndex;
        GI_GraphButton::TGraphButtonGI* AcceptButton;
        GI_PanelScrollBar::TPanelScrollBarGI* ScrollPanel;
        GI_MessageLoop::TObjectGI* SelectedControl;
        pas::WideString Caption;
        // Borrowed PWideString entries.
        pas::List* Items;
        std::int32_t OffsetX;
        std::int32_t OffsetY;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fListBox
