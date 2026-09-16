#pragma once
#include "types/fStarMap.hpp"

namespace fStarMap {
    extern pas::Array<float, 0, 15> PanelSlideCurve;

    extern std::int32_t FilmCameraLookAheadSteps;

    // Returns milliseconds, adjusted by the configured film speed.
    std::int32_t GetTurnFilmFrameInterval(std::int32_t Activity);

    void TfStarMap_Create(TfStarMap* Self);

    void TfStarMap_Destroy(TfStarMap* Self);

} // namespace fStarMap
