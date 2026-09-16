#pragma once
#include "types/aPirate.hpp"

namespace aPirate {
    extern const pas::Array<std::int32_t, 22, 27> PirateSkillBonusWeights;

    extern const pas::Array<std::int32_t, 13, 20> PirateSlotBonusWeights;

    void TPirate_Destroy(TPirate* Self);

} // namespace aPirate
