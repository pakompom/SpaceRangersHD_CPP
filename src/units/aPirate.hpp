#pragma once
#include "types/aPirate.hpp"

namespace aPirate {
    // Entries 22..27 of the dispatch table enter. These reads reload
    // the unchanged BonusKind byte at EBP-5; is the base biased by -22*4.
    extern const pas::Array<std::int32_t, 22, 27> PirateSkillBonusWeights;

    // EvaluateStatBonus dispatch bounds the indexed BonusKind to 13..20.
    extern const pas::Array<std::int32_t, 13, 20> PirateSlotBonusWeights;

    void TPirate_Destroy(TPirate* Self);

} // namespace aPirate
