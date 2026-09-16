#pragma once
#include "types/aKling.hpp"

namespace aKling {
    static_assert(sizeof(void*) != 4 || sizeof(aKling::TKling) == 1244);
    static_assert(sizeof(void*) != 4 || offsetof(aKling::TKling, KlingType) == 1232);
    static_assert(sizeof(void*) != 4 || offsetof(aKling::TKling, DominatorSeries) == 1233);
    static_assert(sizeof(void*) != 4 || offsetof(aKling::TKling, ActiveProgramAppliedTurn) == 1236);
    static_assert(sizeof(void*) != 4 || offsetof(aKling::TKling, ActiveProgramId) == 1240);
    static_assert(sizeof(void*) != 4 || offsetof(aKling::TKling, AuraEffectShownThisTurn) == 1241);

} // namespace aKling
