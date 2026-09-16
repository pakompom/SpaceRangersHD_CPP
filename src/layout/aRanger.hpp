#pragma once
#include "types/aRanger.hpp"

namespace aRanger {
    static_assert(sizeof(void*) != 4 || sizeof(aRanger::TQuest) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, QuestType) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, QuestNumber) == 2);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, Planet) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, DeadlineTurn) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, RewardMoney) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, ObjectiveTarget) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, Successful) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, Description) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, CompletionText) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TQuest, SpecialCompletionText) == 32);
    static_assert(sizeof(void*) != 4 || sizeof(aRanger::TRanger) == 1376);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, PlaceInRating) == 1296);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, ExcludedFromRating) == 1298);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, CareerStatus) == 1299);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, EminentProgress) == 1302);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, PendingCareerActivity) == 1305);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, PreferredCareer) == 1308);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, Aggression) == 1309);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, Quests) == 1312);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, PrisonTermRemaining) == 1316);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, LastDockedNonPlanetLocation) == 1320);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, BaseNodes) == 1324);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TRanger, ProgramCounts) == 1328);
    static_assert(sizeof(void*) != 4 || sizeof(aRanger::TPlayerOldQuest) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TPlayerOldQuest, Planet) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TPlayerOldQuest, Description) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TPlayerOldQuest, Successful) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TPlayerOldQuest, Declined) == 9);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TPlayerOldQuest, QuestType) == 10);
    static_assert(sizeof(void*) != 4 || offsetof(aRanger::TPlayerOldQuest, QuestNumber) == 12);

} // namespace aRanger
