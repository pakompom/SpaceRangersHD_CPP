#pragma once
#include "types/Achievements.hpp"
#include "types/SimpleSteamApi.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace Achievements {
    extern const Achievements::TAchievementDefinitionTable AchievementDefinitionTable;

    extern EC_BlockPar::TBlockParEC* AchievementDefinitions;

    std::int32_t GetCurrentAchievementProgress(pas::WideString Key, std::int32_t StoredValue);

    std::uint8_t GetAchievementBackend();

    std::int32_t GetAvailableAchievementCount();

    SimpleSteamApi::PAchievementData CreateAchievementData();

    void FreeAchievementData(SimpleSteamApi::PAchievementData Data);

    SimpleSteamApi::PAchievementData GetAchievementData(pas::WideString Key);

    std::uint8_t TryUnlockAchievement(pas::WideString Key);

    std::uint8_t TrySetAchievementProgress(pas::WideString Key, std::int32_t Value);

    std::uint8_t TryAddAchievementProgress(pas::WideString Key, std::int32_t Amount);

    void InitializeAchievementDefinitions();

    void TAchievementStats_Create(TAchievementStats* Self);

    void TAchievementStats_Destroy(TAchievementStats* Self);

} // namespace Achievements
