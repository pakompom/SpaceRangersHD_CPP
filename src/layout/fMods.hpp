#pragma once
#include "types/fMods.hpp"

namespace fMods {
    static_assert(sizeof(void*) != 4 || sizeof(fMods::TfModsManager) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, SelectedTab) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, TabCount) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, TabButtons) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, TabPanels) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, TabHeights) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, SelectedCounts) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, WarningCounts) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, ErrorCounts) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, InvalidSelections) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fMods::TfModsManager, NeedsValidation) == 244);

} // namespace fMods
