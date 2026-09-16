#pragma once
#include "types/Rangers.hpp"

namespace Rangers {
    static_assert(sizeof(void*) != 4 || sizeof(Rangers::TAD) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(Rangers::TSteamCallbacksThread) == 44);

} // namespace Rangers
