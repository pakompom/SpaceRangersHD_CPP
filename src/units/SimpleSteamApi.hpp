#pragma once
#include "types/SimpleSteamApi.hpp"

namespace SimpleSteamApi {
    extern std::uint8_t SteamInitialized;

    extern SimpleSteamApi::TSteamUserId SteamUserId;

    extern SimpleSteamApi::TSteamInit SteamInit;

    extern SimpleSteamApi::TSteamCreateAchievements SteamCreateAchievements;

    extern SimpleSteamApi::TSteamInitAchievement SteamInitAchievement;

    extern SimpleSteamApi::TSteamSetLeaderboardName SteamSetLeaderboardName;

    extern SimpleSteamApi::TSteamLeaderboardFound SteamLeaderboardFound;

    extern SimpleSteamApi::TSteamUploadScore SteamUploadScore;

    extern SimpleSteamApi::TSteamLocal SteamLocal;

    extern SimpleSteamApi::TSteamRunCallbacks SteamRunCallbacks;

    extern void* SteamResetAchievements;

    extern SimpleSteamApi::TSteamUnlockAchievement SteamUnlockAchievement;

    extern void* SteamStat;

    extern SimpleSteamApi::TSteamIncreaseStat SteamIncreaseStat;

    extern SimpleSteamApi::TSteamFree SteamFree;

    extern void* SteamAchievementsOverlay;

    extern SimpleSteamApi::TSteamAchievementsCount SteamAchievementsCount;

    extern SimpleSteamApi::TSteamAchievementData SteamAchievementData;

    extern void* SteamStatus;

    void LoadSteamApi();

    void UnloadSteamApi();

    void InitializeSteamAchievements();

} // namespace SimpleSteamApi
