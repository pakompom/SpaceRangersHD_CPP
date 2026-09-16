#pragma once
#include "runtime_support.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace Achievements {
    struct TAchievementInfo;

    struct TAchievementStats;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TAchievementStats : pas::Object {
        PAS_CLASS_META(TAchievementStats, pas::Object, "TAchievementStats", 48)
        void p_destroy() override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        static void CheckBomberAchievement(std::int32_t KillsThisTurn);
        static void CheckAllAwardsAchievement();
        static void CheckNoQuestVictoryAchievement();
        static void CheckChampionVictoryAchievement(std::int32_t Score);
        static void CheckNoLoadVictoryAchievement();
        static void CheckNoShotsArcadeVictoryAchievement();
        static void CheckMoneyAchievement();
        static void CheckMasterAchievement();
        static void CheckNodesAchievement();
        static void CheckLongGameVictoryAchievement(std::int32_t Score, std::int32_t FinishedTurn);
        static void CheckPacifistVictoryAchievement();
        static void CheckAllPirateSystemsAchievement();
        static void CheckFirstPlaceRatingAchievement();
        static void CheckAllSkillsAchievement();
        static void CheckSpeedAchievement();
        static void CheckFastVictoryAchievement();
        void CheckScienceAchievement();
        static void CheckBaronAchievement();
        static void CheckCommanderAchievement();
        static void CheckHaterAchievement();
        static void CheckBestEquipmentAchievement();
        static void CheckAllDiseasesAchievement();
        static void CheckAllDrugsAchievement();
        static void CheckScratchDamageAchievement(std::int32_t HitsReceived);
        void CheckStarFuelAchievement();
        static void CheckMapBuilderAchievement();
        static void CheckInvestorAchievement(std::int32_t Amount);
        static void CheckTranclucatorFleetAchievement();
        std::int32_t AsteroidsDestroyed;
        std::int32_t EnemiesDestroyedByStarHeat;
        std::int32_t SystemsDefended;
        std::int32_t SystemsCapturedForPirates;
        std::uint8_t CompletedResearchPrograms;
        std::uint8_t cpp_padding[3];
        std::int32_t SuccessfulDominatorHacks;
        std::int32_t PrisonersBailedOut;
        std::int32_t DrainedHullPoints;
        std::uint32_t StarFuelCollected;
        std::int32_t StarFuelTankId;
        std::int32_t UninhabitedPlanetsVisited;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    struct TAchievementInfo {
        pas::AnsiString Key;
        std::int32_t MaxValue;
    };

    using TAchievementDefinitionTable = pas::Array<TAchievementInfo, 0, 82>;

} // namespace Achievements
