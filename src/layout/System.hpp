#pragma once
#include "types/System.hpp"

namespace System {
    static_assert(sizeof(void*) != 4 || sizeof(System::TGUID) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(System::TGUID, D1) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(System::TGUID, D2) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(System::TGUID, D3) == 6);
    static_assert(sizeof(void*) != 4 || offsetof(System::TGUID, D4) == 8);

} // namespace System
