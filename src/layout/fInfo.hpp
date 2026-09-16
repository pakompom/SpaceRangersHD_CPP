#pragma once
#include "types/fInfo.hpp"

namespace fInfo {
    static_assert(sizeof(void*) != 4 || sizeof(fInfo::TfInfo) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, PlanetPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, StationPanel) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, LoadPanel) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, InfoPanel) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, InfoContentHeight) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, SearchMode) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, SelectedSearchCategory) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, PreviousSearchCategory) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fInfo::TfInfo, HasSearchResults) == 244);

} // namespace fInfo
