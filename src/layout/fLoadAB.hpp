#pragma once
#include "types/fLoadAB.hpp"

namespace fLoadAB {
    static_assert(sizeof(void*) != 4 || sizeof(fLoadAB::TfLoadABSlot) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadABSlot, Name) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadABSlot, MapName) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadABSlot, ImageName) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadABSlot, Description) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadABSlot, ConfigIndex) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadABSlot, BackgroundImage) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadABSlot, Difficulty) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(fLoadAB::TfLoadAB) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadAB, Entries) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadAB, SelectedIndex) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadAB, HoveredIndex) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadAB, Category) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadAB::TfLoadAB, KeyHistory) == 224);

} // namespace fLoadAB
