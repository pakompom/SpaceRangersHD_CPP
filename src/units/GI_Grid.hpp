#pragma once
#include "types/GI_Grid.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Grid {
    // Rows points into a Delphi dynamic array. ColumnWidths uses the EC heap.
    // Each cell is a TLabelGI child with column/row packed into the dword.
    void TGridGI_Create(TGridGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TGridGI_Destroy(TGridGI* Self);

    void TGridGI_SetActiveCellImagePath(TGridGI* Self, pas::WideString Path);

    void TGridGI_LoadFromConfigPath(TGridGI* Self, const pas::WideString& Path);

    void TGridGI_LoadGridProperties(TGridGI* Self, EC_BlockPar::TBlockParEC* Block);

} // namespace GI_Grid
