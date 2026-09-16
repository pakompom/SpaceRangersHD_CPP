#pragma once
#include "types/aRanger.hpp"

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aRanger {
    extern aShip::TShip* PendingPlayerFollowTarget;

    // Auto-equips in the follow helper and uses NPC dialogue branches for the player.
    extern std::uint8_t PlayerAutomaticControl;

    // Set by equipped-item breakage; ShouldContinuePlayerTravel checks it.
    extern std::uint8_t PlayerEquipmentBrokenThisTurn;

    // Career, then bonSkill1..bonSkill6.
    extern pas::Array<pas::Array<std::int32_t, 22, 27>, 0, 2> RangerSkillBonusEvaluationWeights;

    // Career, then bonSlotRadar..bonSlotForsage.
    extern pas::Array<pas::Array<std::int32_t, 13, 20>, 0, 2> RangerSlotBonusEvaluationWeights;

    // Owned PPlayerOldQuest records.
    extern pas::List* PlayerOldQuests;

    // Requires registered ranger/home-planet state. Removes quests and relation-column entries, adjusts the player index and refreshes galaxy ratings.
    void TRanger_Destroy(TRanger* Self);

} // namespace aRanger
