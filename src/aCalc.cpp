#include "units/ThreadCalc.hpp"
#include "units/aCalc.hpp"

namespace aCalc {
    ThreadCalc::TTurnCalculationPhase TurnCalculationPhase{};

    void WaitForTurnCalculationUI() {
        ThreadCalc::WaitForTurnCalculation();
    }

    std::uint8_t IsTurnCalculationRunningUI() {
        return ThreadCalc::IsTurnCalculationRunning();
    }

    void CalculateGalaxyTurnAndWait() {
        ThreadCalc::StartGalaxyTurnCalculation();
        ThreadCalc::WaitForTurnCalculation();
    }

    void QueueGalaxyTurnCalculation() {
        ThreadCalc::StartGalaxyTurnCalculation();
    }

    void CalculatePlayerStarTurnAndWait() {
        ThreadCalc::StartPlayerStarTurnCalculation();
        ThreadCalc::WaitForTurnCalculation();
    }

    void QueuePlayerStarTurnCalculation() {
        ThreadCalc::StartPlayerStarTurnCalculation();
    }

    void QueuePlayerStarPreparation() {
        ThreadCalc::StartPlayerStarPreparation();
    }

} // namespace aCalc
