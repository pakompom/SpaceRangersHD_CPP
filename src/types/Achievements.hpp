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
        // Includes the pre-version-99 counter layout.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // BOMBER requires at least five kills and a current player/galaxy.
        static void CheckBomberAchievement(std::int32_t KillsThisTurn);
        // Native award-presence buffer is left uninitialized by reversed FillChar arguments.
        static void CheckAllAwardsAchievement();
        static void CheckNoQuestVictoryAchievement();
        // CHAMPION requires at least 50000 points.
        static void CheckChampionVictoryAchievement(std::int32_t Score);
        static void CheckNoLoadVictoryAchievement();
        // Caller establishes a victory without firing; native HOLEPEACE unlock.
        static void CheckNoShotsArcadeVictoryAchievement();
        // MONEY: at least 10000000 credits.
        static void CheckMoneyAchievement();
        // MASTER: at least six player wingmen.
        static void CheckMasterAchievement();
        static void CheckNodesAchievement();
        // DOLGOZHID requires at least 20000 points and turn 36800.
        static void CheckLongGameVictoryAchievement(std::int32_t Score, std::int32_t FinishedTurn);
        static void CheckPacifistVictoryAchievement();
        // PIRATESYSTEMS: every registered star is pirate-controlled.
        static void CheckAllPirateSystemsAchievement();
        // Requires PlaceInRating=1 and CurrentTurn>=300.
        static void CheckFirstPlaceRatingAchievement();
        // These checks use global player/galaxy state. Victory and score eligibility
        // are checked by the caller; the instance counters are not used here.
        // SKILL: all six player base skills are at least level six.
        static void CheckAllSkillsAchievement();
        // Native SPEED threshold: calculated speed 2300.
        static void CheckSpeedAchievement();
        // SPRINTER requires fewer than seven elapsed years after turn 300.
        static void CheckFastVictoryAchievement();
        void CheckScienceAchievement();
        // Requires player pirate rank 7.
        static void CheckBaronAchievement();
        static void CheckCommanderAchievement();
        // HATER requires at least one inhabited planet in Coalition-controlled systems and hostile relations with every such planet.
        static void CheckHaterAchievement();
        // Native BEST check: all direct slots and five weapons have nonstandard stats.
        static void CheckBestEquipmentAchievement();
        // ILL: the player has experienced every one of the twelve diseases.
        static void CheckAllDiseasesAchievement();
        static void CheckAllDrugsAchievement();
        // SCRATCHDAMAGE: twenty one-point hits on the same target.
        static void CheckScratchDamageAchievement(std::int32_t HitsReceived);
        // SUNFUEL threshold: 40 units in one fuel tank.
        void CheckStarFuelAchievement();
        static void CheckMapBuilderAchievement();
        static void CheckInvestorAchievement(std::int32_t Amount);
        // TRANCLUCATORS requires ten live, normal-space Tranclucators owned by the player in the current star.
        static void CheckTranclucatorFleetAchievement();
        // ASTEROID: player asteroid kills in TStar.ProcessPlayerAsteroidKill ().
        std::int32_t AsteroidsDestroyed;
        // FRY counter.
        std::int32_t EnemiesDestroyedByStarHeat;
        // DEFENDER: qualifying Coalition or pirate defenses; native increments.
        std::int32_t SystemsDefended;
        // PIRATE progress, qualifying system captures.
        std::int32_t SystemsCapturedForPirates;
        // SCIENCE counts completed Dominator research programs.
        std::uint8_t CompletedResearchPrograms;
        std::uint8_t cpp_padding[3];
        // HACKER: accepted programs in TfTalk.RunDominatorProgram; native increment.
        std::int32_t SuccessfulDominatorHacks;
        // PRISONBAIL: ships released by TfGov.PayPrisonBail; native increment.
        std::int32_t PrisonersBailedOut;
        // Hull restored by the player's draining weapons; DRAIN progress.
        std::int32_t DrainedHullPoints;
        // SUNFUEL counter for the current fuel tank.
        std::uint32_t StarFuelCollected;
        // Resets the counter when the installed tank changes.
        std::int32_t StarFuelTankId;
        // First player landings while OwnerId=6; EXPLORER progress.
        std::int32_t UninhabitedPlanetsVisited;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Native record RTTI.
    struct TAchievementInfo {
        pas::AnsiString Key;
        std::int32_t MaxValue;
    };

    using TAchievementDefinitionTable = pas::Array<TAchievementInfo, 0, 82>;

} // namespace Achievements
