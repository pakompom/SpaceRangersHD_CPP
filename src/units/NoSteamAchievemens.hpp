#pragma once
#include "runtime_support.hpp"
#include "types/SimpleSteamApi.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace NoSteamAchievemens {
    void GetLocalAchievementData(pas::WideString Key, SimpleSteamApi::PAchievementData Data);

    void LoadLocalAchievements();

    std::uint8_t UnlockLocalAchievement(EC_BlockPar::TBlockParEC* Block);

    std::uint8_t IncreaseLocalAchievementProgress(EC_BlockPar::TBlockParEC* Block, std::int32_t Amount);

    void SaveLocalAchievements();

    void NotifyLocalAchievement(EC_BlockPar::TBlockParEC* Block);

} // namespace NoSteamAchievemens
