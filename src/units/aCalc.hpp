#pragma once
#include "runtime_support.hpp"
#include "types/ThreadCalc.hpp"

namespace aCalc {
    extern ThreadCalc::TTurnCalculationPhase TurnCalculationPhase;

    void WaitForTurnCalculationUI();

    std::uint8_t IsTurnCalculationRunningUI();

    void CalculateGalaxyTurnAndWait();

    void QueueGalaxyTurnCalculation();

    void CalculatePlayerStarTurnAndWait();

    void QueuePlayerStarTurnCalculation();

    void QueuePlayerStarPreparation();

} // namespace aCalc
