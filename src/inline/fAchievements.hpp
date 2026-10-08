#pragma once
#include "units/GI_MessageLoop.hpp"
#include "units/fAchievements.hpp"

namespace fAchievements {
    inline void TfAchievements_Create(TfAchievements* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
    }

} // namespace fAchievements
