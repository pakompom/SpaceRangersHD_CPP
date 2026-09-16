#pragma once
#include "types/Registry.hpp"

namespace Registry {
    static_assert(sizeof(void*) != 4 || sizeof(Registry::TRegistry) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(Registry::TRegistry, FCurrentKey) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Registry::TRegistry, FRootKey) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Registry::TRegistry, FLazyWrite) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Registry::TRegistry, FCurrentPath) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Registry::TRegistry, FCloseRootKey) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(Registry::TRegistry, FAccess) == 24);

} // namespace Registry
