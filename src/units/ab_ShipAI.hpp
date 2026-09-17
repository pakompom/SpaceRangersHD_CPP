#pragma once
#include "types/aConst.hpp"
#include "types/ab_ShipAI.hpp"

namespace ab_ShipAI {
    // Source helper: preserve the native full-width load before a Byte stack argument.
    // Passing the local directly lets DCC32 narrow MOV EAX to MOV AL. This identity
    // inlines without a call, extra assignment or temporary in ApplyDamage.
    std::int32_t RewardTechArgument(std::int32_t Value);

    // Source helper: keep the seed evaluation before both clamps, with their
    // temporaries preceding the seed slot in the native frame.
    void SelectArcadeRewardWeapon(TabShipAI* Ship, std::int32_t Tech, aConst::PWeaponInfo& Info);

    void TabShipAI_Create(TabShipAI* Self);

    void TabShipAI_Destroy(TabShipAI* Self);

} // namespace ab_ShipAI
