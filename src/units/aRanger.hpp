#pragma once
#include "types/aRanger.hpp"

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aRanger {
    extern aShip::TShip* PendingPlayerFollowTarget;

    extern std::uint8_t PlayerAutomaticControl;

    extern std::uint8_t PlayerEquipmentBrokenThisTurn;

    extern pas::Array<pas::Array<std::int32_t, 22, 27>, 0, 2> RangerSkillBonusEvaluationWeights;

    extern pas::Array<pas::Array<std::int32_t, 13, 20>, 0, 2> RangerSlotBonusEvaluationWeights;

    extern pas::List* PlayerOldQuests;

    void TRanger_Destroy(TRanger* Self);

} // namespace aRanger
