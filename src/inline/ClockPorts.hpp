#pragma once
#include "units/ClockPorts.hpp"

namespace ClockPorts {
    inline std::uint32_t ReadCycleCounter() {
        return pas::read_cycle_counter();
    }

} // namespace ClockPorts
