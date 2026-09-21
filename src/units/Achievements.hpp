#pragma once
#include "types/Achievements.hpp"
#include "types/SimpleSteamApi.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace Achievements {
    extern const Achievements::TAchievementDefinitionTable AchievementDefinitionTable;

    extern EC_BlockPar::TBlockParEC* AchievementDefinitions;

    std::int32_t GetCurrentAchievementProgress(const std::u16string_view& Key, std::int32_t StoredValue);

    // 1=Steam, 2=Steam without achievement support, 3=local.
    std::uint8_t GetAchievementBackend();

    // Capped at 82.
    std::int32_t GetAvailableAchievementCount();

    // Allocates three 255-character caller-owned string buffers.
    SimpleSteamApi::PAchievementData CreateAchievementData();

    // Nil-safe; releases all three string cells and the record.
    void FreeAchievementData(SimpleSteamApi::PAchievementData Data);

    // Caller owns the result; nil for an unknown key.
    SimpleSteamApi::PAchievementData GetAchievementData(pas::WideString Key);

    // Checks availability, unlock budget and per-save duplicates; records successful unlocks in the current player's AwardedAchievementKeys.
    std::uint8_t TryUnlockAchievement(pas::WideString Key);

    // Raises progress to the supplied value, capped at the target; never reduces existing progress.
    std::uint8_t TrySetAchievementProgress(pas::WideString Key, std::int32_t Value);

    // Caps the increment at the configured achievement target; checks platform availability and per-save completion.
    std::uint8_t TryAddAchievementProgress(pas::WideString Key, std::int32_t Amount);

    void InitializeAchievementDefinitions();

    void TAchievementStats_Create(TAchievementStats* Self);

    void TAchievementStats_Destroy(TAchievementStats* Self);

} // namespace Achievements
