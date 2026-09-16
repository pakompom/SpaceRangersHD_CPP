#pragma once
#include "types/ThreadCalc.hpp"

namespace ThreadCalc {
    // Smoothed film-progress threshold derived from measured galaxy-turn duration.
    extern float AdaptiveBeginCalcNextTurn;

    // Measured milliseconds; native smoothing uses signed arithmetic.
    extern std::int32_t LastGalaxyTurnDuration;

    void StartGalaxyTurnCalculation();

    void StartPlayerStarTurnCalculation();

    void StartPlayerStarPreparation();

    std::uint8_t IsTurnCalculationRunning();

    // Requires an initialized calculation thread.
    void WaitForTurnCalculation();

    void ProcessPlayerStarTurn();

    void TThreadCalc_Execute(TThreadCalc* Self);

} // namespace ThreadCalc
