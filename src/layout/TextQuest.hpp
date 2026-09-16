#pragma once
#include "types/TextQuest.hpp"

namespace TextQuest {
    static_assert(sizeof(void*) != 4 || sizeof(TextQuest::TTextQuest) == 128);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, Locations) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, Paths) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, FormatVersion) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, MajorVersion) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, MinorVersion) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, ChangeLogText) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, EditorScreenWidth) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, EditorScreenHeight) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, EditorGridWidth) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, EditorGridHeight) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, Difficulty) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, CompleteOnFinish) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, IssuerRaceMask) == 49);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, TargetOwnerMask) == 50);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, PlayerCareerMask) == 51);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, PlayerRaceMask) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, SuccessRelationDelta) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, DefaultTraversalLimit) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, QuestDescriptionText) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, QuestSuccessGovMessageText) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, ToStarText) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, ToPlanetText) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, DateText) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, MoneyText) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, FromPlanetText) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, FromStarText) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, RangerText) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, PlayerInterface) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, TextShown) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, DisplayedEvent) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, OutcomeEvent) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, Outcome) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, Parameters) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(TextQuest::TTextQuest, LastEventSource) == 124);

} // namespace TextQuest
