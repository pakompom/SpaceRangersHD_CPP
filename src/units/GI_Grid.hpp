#pragma once
#include "types/GI_Grid.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Grid {
    // Rows points into a Delphi dynamic array. ColumnWidths uses the EC heap.
    // Each cell is a TLabelGI child with column/row packed into the dword.
    void TGridGI_Create(TGridGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TGridGI_Destroy(TGridGI* Self);

    void TGridGI_LoadFromConfigPath(TGridGI* Self, const pas::WideString& Path);

} // namespace GI_Grid
