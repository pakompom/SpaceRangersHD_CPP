#pragma once
#include "types/FGInt.hpp"

namespace FGInt {
    static_assert(sizeof(void*) != 4 || sizeof(FGInt::TFGInt) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(FGInt::TFGInt, Sign) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(FGInt::TFGInt, Number) == 4);

} // namespace FGInt
