#pragma once
#include "types/EC_Ether.hpp"

namespace EC_Ether {
    static_assert(sizeof(void*) != 4 || sizeof(EC_Ether::TEther) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEther, First) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEther, Last) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEther, Count) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEther, SortedItems) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEther, Lock) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Ether::TEtherUnit) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEtherUnit, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEtherUnit, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEtherUnit, Value) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Ether::TEtherUnit, Name) == 16);

} // namespace EC_Ether
