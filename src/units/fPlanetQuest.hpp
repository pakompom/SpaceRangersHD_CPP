#pragma once
#include "types/aRanger.hpp"
#include "types/aScript.hpp"
#include "types/fPlanetQuest.hpp"

namespace fPlanetQuest {
    extern std::int32_t QuestStyleCount;

    extern aRanger::PQuest ActiveGovernmentQuest;

    extern fPlanetQuest::TTextQuestPlayerInterface* QuestPlayerInterface;

    extern aScript::PQueuedTextQuest ActiveQueuedTextQuest;

    void TfQuestA_Create(TfQuestA* Self);

    void TfQuestA_Destroy(TfQuestA* Self);

} // namespace fPlanetQuest
