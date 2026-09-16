#pragma once
#include "types/fRewards.hpp"

namespace fRewards {
    static_assert(sizeof(void*) != 4 || sizeof(fRewards::TfRewards) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fRewards::TfRewards, AwardsPanel) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fRewards::TfRewards, Ship) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fRewards::TfRewards, DraggedAward) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fRewards::TfRewards, HoveredAwardId) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fRewards::TfRewards, ReadOnly) == 224);

} // namespace fRewards
