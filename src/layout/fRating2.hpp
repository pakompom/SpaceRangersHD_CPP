#pragma once
#include "types/fRating2.hpp"

namespace fRating2 {
    static_assert(sizeof(void*) != 4 || sizeof(fRating2::TRangerRatingRow) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TRangerRatingRow, Ranger) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TRangerRatingRow, Top) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TRangerRatingRow, Height) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(fRating2::TfRating2) == 264);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, RewardWindow) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, Rows) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, SelectedIndex) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, SelectedRangerId) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, SortColumn) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, SortAscending) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, TablePanel) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, SelectedRowRect) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, BackgroundClickStarted) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fRating2::TfRating2, HoveredAwardId) == 260);

} // namespace fRating2
