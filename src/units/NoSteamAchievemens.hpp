#pragma once
#include "runtime_support.hpp"
#include "types/SimpleSteamApi.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace NoSteamAchievemens {
    // Fills caller-owned strings/counters for a registered local achievement.
    void GetLocalAchievementData(pas::WideString Key, SimpleSteamApi::PAchievementData Data);

    // Reads achievements.dat, expands zlib, decodes its payload and verifies the additive checksum. Unknown keys do not consume their value fields in the native reader.
    void LoadLocalAchievements();

    // Returns true even if already unlocked; absent timestamps allow a fresh unlock.
    std::uint8_t UnlockLocalAchievement(EC_BlockPar::TBlockParEC* Block);

    // Positive increments only; clamps to MaxValue and saves accepted changes.
    std::uint8_t IncreaseLocalAchievementProgress(EC_BlockPar::TBlockParEC* Block, std::int32_t Amount);

    // Writes the native checksummed, encoded and compressed achievements.dat format.
    void SaveLocalAchievements();

    // Queues the localized achievement toast when its controller exists.
    void NotifyLocalAchievement(EC_BlockPar::TBlockParEC* Block);

} // namespace NoSteamAchievemens
