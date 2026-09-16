#pragma once
#include "units/ClockPorts.hpp"

namespace ClockPorts {
    // Explicit host boundary for the game's two RDTSC samples.
    inline std::uint32_t ReadCycleCounter() {
        return pas::read_cycle_counter();
    }

} // namespace ClockPorts
