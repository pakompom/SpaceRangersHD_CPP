#pragma once
#include "types/aWarrior.hpp"

namespace aWarrior {
    extern const pas::Array<std::int32_t, 22, 27> WarriorSkillBonusWeights;

    // EvaluateStatBonus dispatches bonus kinds 13..20 to these slot cases.
    extern const pas::Array<std::int32_t, 13, 20> WarriorSlotBonusWeights;

    void TWarrior_Destroy(TWarrior* Self);

} // namespace aWarrior
