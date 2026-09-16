#pragma once
#include "types/fPlanetQuest.hpp"

namespace fPlanetQuest {
    static_assert(sizeof(void*) != 4 || sizeof(fPlanetQuest::TfPlanetQuest) == 276);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, QuestName) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, Quest) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, CurrentDate) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, DaysElapsed) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, CurrentPicture) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, MoneyLimitComplement) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, CurrentText) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, NextChoiceTop) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, ParameterPanelWidth) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, ParameterPanelHeight) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, PageAnimationTimer) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, PreviousStyleIndex) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, ChoiceCount) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, ParameterPanelOrigin) == 262);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfPlanetQuest, QuestId) == 272);
    static_assert(sizeof(void*) != 4 || sizeof(fPlanetQuest::TTextQuestPlayerInterface) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(fPlanetQuest::TfQuestA) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfQuestA, Callback) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetQuest::TfQuestA, Value) == 16);

} // namespace fPlanetQuest
