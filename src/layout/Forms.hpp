#pragma once
#include "types/Forms.hpp"

namespace Forms {
    static_assert(sizeof(void*) != 4 || sizeof(Forms::TApplication) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(Forms::TApplication, Handle) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Forms::TApplication, OnActivate) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Forms::TApplication, OnDeactivate) == 16);

} // namespace Forms
