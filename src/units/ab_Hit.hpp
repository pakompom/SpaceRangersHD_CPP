#pragma once
#include "types/ab_Hit.hpp"

namespace ab_Hit {
    // Native objects are TabShip instances.
    extern pas::Array<ab_Hit::TabHit*, 0, 3> KellerFragments;

    // Advance and KellerBreakupComplete iterate the four fragment slots, indices 0..3.
    extern pas::Array<double, 0, 3> KellerFragmentDistances;

    // Initialized by breakup; later use under review.
    extern pas::Array<double, 0, 3> KellerFragmentValuesAC;

    extern std::int32_t KellerBreakupTicks;

    extern std::uint8_t KellerSplitActive;

    extern std::uint8_t KellerFinishRequested;

    extern std::uint8_t KellerDeathPending;

    void TabHit_Create(TabHit* Self);

    void TabHit_Destroy(TabHit* Self);

} // namespace ab_Hit
