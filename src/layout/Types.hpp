#pragma once
#include "types/Types.hpp"

namespace Types {
    static_assert(sizeof(void*) != 4 || sizeof(Types::TPoint) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Types::TPoint, X) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Types::TPoint, Y) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(Types::TRect) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Types::TRect, Left) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Types::TRect, Top) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Types::TRect, Right) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Types::TRect, Bottom) == 12);

} // namespace Types
