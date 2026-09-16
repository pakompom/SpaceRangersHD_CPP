#pragma once
#include "types/fScore.hpp"

namespace fScore {
    // Preserve evaluation of the localized template before the turn clamp, and
    // the separate managed temporary retained by the native compiler.
    void SetElapsedScoreTurns(TfScore* Screen, TfScoreUnit* Entry);

    void TfScoreUnit_Create(TfScoreUnit* Self);

    void TfScoreUnit_Destroy(TfScoreUnit* Self);

    void TfScore_Create(TfScore* Self);

    void TfScore_Destroy(TfScore* Self);

} // namespace fScore
