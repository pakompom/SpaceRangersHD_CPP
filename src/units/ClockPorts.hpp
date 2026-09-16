#pragma once
#include "runtime_support.hpp"

namespace ClockPorts {
    // Explicit host boundary for the game's two RDTSC samples.
    inline std::uint32_t ReadCycleCounter();

} // namespace ClockPorts

#include "inline/ClockPorts.hpp"
