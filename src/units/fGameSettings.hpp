#pragma once
#include "types/fGameSettings.hpp"

namespace aGalaxy {
    struct TStar;

} // namespace aGalaxy

namespace fGameSettings {
    // Source control for the native Extended temporaries and axis evaluation order.
    void CalculateSquaredEdgeDistance(aGalaxy::TStar* Star, float& Distance);

    void TThreadCreateNewGame_Execute(TThreadCreateNewGame* Self);

} // namespace fGameSettings
