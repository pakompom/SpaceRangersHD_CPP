#pragma once
#include "units/ThreadCalc.hpp"
#include "units/aCalc.hpp"

namespace aCalc {
    // UI-facing turn calculation wrappers.
    inline void WaitForTurnCalculationUI() {
        ThreadCalc::WaitForTurnCalculation();
    }

    inline std::uint8_t IsTurnCalculationRunningUI() {
        return ThreadCalc::IsTurnCalculationRunning();
    }

    inline void QueueGalaxyTurnCalculation() {
        ThreadCalc::StartGalaxyTurnCalculation();
    }

    inline void QueuePlayerStarTurnCalculation() {
        ThreadCalc::StartPlayerStarTurnCalculation();
    }

    inline void QueuePlayerStarPreparation() {
        ThreadCalc::StartPlayerStarPreparation();
    }

} // namespace aCalc
