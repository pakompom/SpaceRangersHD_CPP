#pragma once
#include "types/ParViewStringClass.hpp"

namespace ParViewStringClass {
    static_assert(sizeof(void*) != 4 || sizeof(ParViewStringClass::TParViewString) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ParViewStringClass::TParViewString, MinValue) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ParViewStringClass::TParViewString, MaxValue) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ParViewStringClass::TParViewString, Text) == 12);

} // namespace ParViewStringClass
