#pragma once
#include "types/fScore.hpp"

namespace fScore {
    extern const pas::WideString ScoreValueColorTag;

    void TfScoreUnit_Create(TfScoreUnit* Self);

    void TfScoreUnit_Destroy(TfScoreUnit* Self);

    void TfScore_Create(TfScore* Self);

    void TfScore_Destroy(TfScore* Self);

} // namespace fScore
