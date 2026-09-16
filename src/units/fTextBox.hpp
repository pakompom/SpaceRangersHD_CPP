#pragma once
#include "types/fTextBox.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fTextBox {
    std::uint32_t ShowTextInputDialog(GI_MessageLoop::TMessageLoopGI* Parent, pas::WideString Caption, pas::WideString& Value, std::int32_t MaximumLength, std::int32_t OffsetX, std::int32_t OffsetY);

    void TfTextBox_Create(TfTextBox* Self);

    void TfTextBox_Destroy(TfTextBox* Self);

} // namespace fTextBox
