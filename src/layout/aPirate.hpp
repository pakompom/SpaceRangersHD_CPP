#pragma once
#include "types/aPirate.hpp"

namespace aPirate {
    static_assert(sizeof(void*) != 4 || sizeof(aPirate::TPirate) == 1308);
    static_assert(sizeof(void*) != 4 || offsetof(aPirate::TPirate, PrisonTermRemaining) == 1296);
    static_assert(sizeof(void*) != 4 || offsetof(aPirate::TPirate, PirateType) == 1300);
    static_assert(sizeof(void*) != 4 || offsetof(aPirate::TPirate, RaidPressure) == 1304);

} // namespace aPirate
