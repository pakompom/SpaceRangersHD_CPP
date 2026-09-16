#pragma once
#include "types/ab_Hit.hpp"

namespace ab_Hit {
    extern pas::Array<ab_Hit::TabHit*, 0, 3> KellerFragments;

    extern pas::Array<double, 0, 3> KellerFragmentDistances;

    extern pas::Array<double, 0, 3> KellerFragmentValuesAC;

    extern std::int32_t KellerBreakupTicks;

    extern std::uint8_t KellerSplitActive;

    extern std::uint8_t KellerFinishRequested;

    extern std::uint8_t KellerDeathPending;

    void TabHit_Create(TabHit* Self);

    void TabHit_Destroy(TabHit* Self);

} // namespace ab_Hit
