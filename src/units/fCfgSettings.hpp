#pragma once
#include "types/fCfgSettings.hpp"

namespace fCfgSettings {
    extern std::uint32_t SettingsModeColorNormal;

    extern std::uint32_t SettingsModeColorHighlighted;

    // Uses the low 32 bits of a timestamp-counter delta across a 200 ms sleep; temporarily raises process/thread priority.
    double EstimateCpuClockMHz();

} // namespace fCfgSettings
