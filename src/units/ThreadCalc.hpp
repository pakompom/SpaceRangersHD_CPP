#pragma once
#include "types/ThreadCalc.hpp"

namespace ThreadCalc {
    extern float AdaptiveBeginCalcNextTurn;

    extern std::int32_t LastGalaxyTurnDuration;

    void StartGalaxyTurnCalculation();

    void StartPlayerStarTurnCalculation();

    void StartPlayerStarPreparation();

    std::uint8_t IsTurnCalculationRunning();

    void WaitForTurnCalculation();

    void ProcessPlayerStarTurn();

    void TThreadCalc_Execute(TThreadCalc* Self);

} // namespace ThreadCalc
