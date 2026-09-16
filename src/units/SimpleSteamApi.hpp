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

    // steamCallBacks export loaded from steam_ach.dll; called without arguments by TSteamCallbacksThread.
    extern SimpleSteamApi::TSteamRunCallbacks SteamRunCallbacks;

    // steamResetAchievements export; full signature remains unresolved.
    extern void* SteamResetAchievements;

    extern SimpleSteamApi::TSteamUnlockAchievement SteamUnlockAchievement;

    // steamStat export; full signature remains unresolved.
    extern void* SteamStat;

    extern SimpleSteamApi::TSteamIncreaseStat SteamIncreaseStat;

    extern SimpleSteamApi::TSteamFree SteamFree;

    // steamAchievementsOverlay export; full signature remains unresolved.
    extern void* SteamAchievementsOverlay;

    extern SimpleSteamApi::TSteamAchievementsCount SteamAchievementsCount;

    extern SimpleSteamApi::TSteamAchievementData SteamAchievementData;

    // steamStatus export; full signature remains unresolved.
    extern void* SteamStatus;

    // Loads steam_ach.dll and resolves exports without checking individual addresses.
    void LoadSteamApi();

    // Calls steamFree and releases the module if present; export pointers are left unchanged.
    void UnloadSteamApi();

    void InitializeSteamAchievements();

} // namespace SimpleSteamApi
