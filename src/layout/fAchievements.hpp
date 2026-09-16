#pragma once
#include "types/fAchievements.hpp"

namespace fAchievements {
    static_assert(sizeof(void*) != 4 || sizeof(fAchievements::TfAchievementsSlot) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fAchievements::TfAchievementsSlot, Key) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fAchievements::TfAchievementsSlot, Data) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fAchievements::TfAchievementsSlot, Background) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(fAchievements::TfAchievements) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fAchievements::TfAchievements, Rows) == 208);

} // namespace fAchievements
