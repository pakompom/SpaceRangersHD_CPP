#pragma once
#include "types/fCustom.hpp"

namespace fCustom {
    static_assert(sizeof(void*) != 4 || sizeof(fCustom::TfCustomLoop) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fCustom::TfCustomLoop, ReservedBeforeText) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fCustom::TfCustomLoop, ReservedAfterText) == 212);

} // namespace fCustom
