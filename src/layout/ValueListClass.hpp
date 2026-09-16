#pragma once
#include "types/ValueListClass.hpp"

namespace ValueListClass {
    static_assert(sizeof(void*) != 4 || sizeof(ValueListClass::TValuesList) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ValueListClass::TValuesList, AcceptListed) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ValueListClass::TValuesList, Values) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ValueListClass::TValuesList, Count) == 12);

} // namespace ValueListClass
