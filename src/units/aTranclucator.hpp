#pragma once
#include "types/aTranclucator.hpp"

namespace aTranclucator {
    extern pas::Array<std::int32_t, 22, 27> TranclucatorSkillBonusWeights;

    extern pas::Array<std::int32_t, 13, 20> TranclucatorSlotBonusWeights;

    void TTranclucator_Create(TTranclucator* Self);

    void TTranclucator_Destroy(TTranclucator* Self);

} // namespace aTranclucator
