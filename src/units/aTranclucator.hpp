#pragma once
#include "types/aTranclucator.hpp"

namespace aTranclucator {
    extern pas::Array<std::int32_t, 22, 27> TranclucatorSkillBonusWeights;

    // The dispatch table confines these reads to BonusKind=13..20.
    // Each reloads the unchanged byte at EBP-5; is the base biased by -13*4.
    extern pas::Array<std::int32_t, 13, 20> TranclucatorSlotBonusWeights;

    void TTranclucator_Create(TTranclucator* Self);

    void TTranclucator_Destroy(TTranclucator* Self);

} // namespace aTranclucator
