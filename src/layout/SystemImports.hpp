#pragma once
#include "types/SystemImports.hpp"

namespace SystemImports {
    static_assert(sizeof(void*) != 4 || sizeof(SystemImports::TThreadStart) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SystemImports::TThreadStart, Func) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SystemImports::TThreadStart, Parameter) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(SystemImports::TMethod) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SystemImports::TMethod, Code) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SystemImports::TMethod, Data) == 4);

} // namespace SystemImports
