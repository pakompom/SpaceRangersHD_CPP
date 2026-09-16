#pragma once
#include "types/EC_Str.hpp"

namespace EC_Str {
    static_assert(sizeof(void*) != 4 || sizeof(EC_Str::TStringsEC) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TStringsEC, FirstElement) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TStringsEC, LastElement) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TStringsEC, CurrentElement) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Str::TStringsElEC) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TStringsElEC, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TStringsElEC, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TStringsElEC, Text) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TStringsElEC, Data) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Str::TWideCasePair) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TWideCasePair, LowerChar) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Str::TWideCasePair, UpperChar) == 2);

} // namespace EC_Str
