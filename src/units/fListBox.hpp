#pragma once
#include "types/fListBox.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fListBox {
    std::uint32_t ShowListDialog(GI_MessageLoop::TMessageLoopGI* Parent, std::int32_t& SelectedIndex, pas::WideString Caption, pas::List* Items, std::int32_t OffsetX, std::int32_t OffsetY);

    void TfListBox_Create(TfListBox* Self);

    void TfListBox_Destroy(TfListBox* Self);

} // namespace fListBox
