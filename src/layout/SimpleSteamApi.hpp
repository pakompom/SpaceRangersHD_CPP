#pragma once
#include "types/SimpleSteamApi.hpp"

namespace SimpleSteamApi {
    static_assert(sizeof(void*) != 4 || sizeof(SimpleSteamApi::TAchievementData) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, Name) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, Description) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, Achieved) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, HasProgress) == 9);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, Reserved0C) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, MaxValue) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, Value) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, IconPath) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SimpleSteamApi::TAchievementData, Date) == 32);

} // namespace SimpleSteamApi
