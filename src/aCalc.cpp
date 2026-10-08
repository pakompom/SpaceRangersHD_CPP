#include "units/ThreadCalc.hpp"
#include "units/aCalc.hpp"

// Native wrappers occupy, separate from ThreadCalc.
// PACKAGEINFO visits aCalc immediately before its ThreadCalc dependency.
namespace aCalc {
    ThreadCalc::TTurnCalculationPhase TurnCalculationPhase{};

    void CalculateGalaxyTurnAndWait() {
        ThreadCalc::StartGalaxyTurnCalculation();
        ThreadCalc::WaitForTurnCalculation();
    }

    void CalculatePlayerStarTurnAndWait() {
        ThreadCalc::StartPlayerStarTurnCalculation();
        ThreadCalc::WaitForTurnCalculation();
    }

} // namespace aCalc
