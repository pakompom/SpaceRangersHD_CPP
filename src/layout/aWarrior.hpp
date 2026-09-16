#pragma once
#include "types/aWarrior.hpp"

namespace aWarrior {
    static_assert(sizeof(void*) != 4 || sizeof(aWarrior::TWarrior) == 1300);
    static_assert(sizeof(void*) != 4 || offsetof(aWarrior::TWarrior, WarriorType) == 1296);

} // namespace aWarrior
