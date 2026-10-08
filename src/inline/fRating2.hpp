#pragma once
#include "units/fPanelMain.hpp"
#include "units/fRating2.hpp"

namespace fRating2 {
    inline void TfRating2_Create(TfRating2* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
    }

} // namespace fRating2
