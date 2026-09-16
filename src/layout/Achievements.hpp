#pragma once
#include "types/Achievements.hpp"

namespace Achievements {
    static_assert(sizeof(void*) != 4 || sizeof(Achievements::TAchievementStats) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, AsteroidsDestroyed) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, EnemiesDestroyedByStarHeat) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, SystemsDefended) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, SystemsCapturedForPirates) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, CompletedResearchPrograms) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, SuccessfulDominatorHacks) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, PrisonersBailedOut) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, DrainedHullPoints) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, StarFuelCollected) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, StarFuelTankId) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementStats, UninhabitedPlanetsVisited) == 44);
    static_assert(sizeof(void*) != 4 || sizeof(Achievements::TAchievementInfo) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementInfo, Key) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Achievements::TAchievementInfo, MaxValue) == 4);

} // namespace Achievements
