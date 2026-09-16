#pragma once
#include "types/GI_Grid.hpp"

namespace GI_Grid {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Grid::TGridRowGI) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridRowGI, Height) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridRowGI, AutoHeightMinimum) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridRowGI, AutoHeight) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(GI_Grid::TGridGI) == 448);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, ColumnCount) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, RowCount) == 372);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, ColumnWidths) == 376);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, Rows) == 380);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, FontName) == 384);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, TextColor) == 388);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, GridType) == 392);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, GridColor) == 396);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, BackgroundImage) == 400);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, ActiveCellImage) == 404);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, ActiveCellFrame) == 408);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, ActiveCell) == 412);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, RowSelect) == 420);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, ColSelect) == 421);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, SelectionChangedCallback) == 424);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, CanSelectCellCallback) == 432);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Grid::TGridGI, RepeatedCellClickCallback) == 440);

} // namespace GI_Grid
