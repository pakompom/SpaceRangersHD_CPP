#pragma once
#include "runtime_support.hpp"
#include "types/ThreadCalc.hpp"

namespace aCalc {
    extern ThreadCalc::TTurnCalculationPhase TurnCalculationPhase;

    // UI-facing turn calculation wrappers.
    inline void WaitForTurnCalculationUI();

    inline std::uint8_t IsTurnCalculationRunningUI();

    void CalculateGalaxyTurnAndWait();

    inline void QueueGalaxyTurnCalculation();

    void CalculatePlayerStarTurnAndWait();

    inline void QueuePlayerStarTurnCalculation();

    inline void QueuePlayerStarPreparation();

} // namespace aCalc

#include "inline/aCalc.hpp"
