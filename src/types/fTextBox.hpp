#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"

namespace GI_Edit {
    struct TEditGI;

} // namespace GI_Edit

namespace GI_GraphButton {
    struct TGraphButtonGI;

} // namespace GI_GraphButton

namespace fTextBox {
    struct TfTextBox;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfTextBox : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfTextBox, GI_MessageLoop::TMessageLoopGI, "TfTextBox", 236)
        void p_destroy() override;
        void OnOpen() override;
        void OnClose() override;
        void AcceptClicked(GI_MessageLoop::TObjectGI* Sender);
        void CancelClicked(GI_MessageLoop::TObjectGI* Sender);
        void FocusEdit();
        void TextChanged(GI_MessageLoop::TObjectGI* Sender);
        std::uint8_t IsValidText(pas::WideString Candidate);
        void EditKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessCallbackTimers() override;
        pas::WideString Caption;
        GI_Edit::TEditGI* Edit;
        pas::WideString Value;
        GI_GraphButton::TGraphButtonGI* AcceptButton;
        std::int32_t MaximumLength;
        std::int32_t OffsetX;
        std::int32_t OffsetY;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fTextBox
