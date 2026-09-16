#pragma once
#include "types/fScore.hpp"

namespace fScore {
    static_assert(sizeof(void*) != 4 || sizeof(fScore::TfScore) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScore, Entries) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScore, SelectedIndex) == 212);
    static_assert(sizeof(void*) != 4 || sizeof(fScore::TScoreQuestResult) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TScoreQuestResult, Successful) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TScoreQuestResult, QuestType) == 1);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TScoreQuestResult, QuestNumber) == 2);
    static_assert(sizeof(void*) != 4 || sizeof(fScore::TfScoreUnit) == 124);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, VictoryAchieved) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, Disqualified) == 5);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, DifficultyLevels) == 6);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, DifficultyPercent) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, PlayerName) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, PortraitFaceId) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, PilotRace) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, FinishedTurn) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, Rank) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, PirateRank) == 37);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, OtherShipKillCount) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, PirateKillCount) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, DominatorKillCount) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, LiberatedSystemCount) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, CivilianKillCount) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, MilitaryKillCount) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, RangerKillCount) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, ArcadeKillCount) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, AwardCount) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, AwardIds) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, TotalExperience) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, SkillLevels) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, GenerationSeed) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, ScoreTags) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, QuestResults) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, PlanetBattles) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, PlanetBattleHistory) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, BlazerEndingState) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, KellerEndingState) == 113);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, TerronEndingState) == 114);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, PirateEndingState) == 115);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, TotalScore) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TfScoreUnit, Exported) == 120);

} // namespace fScore
