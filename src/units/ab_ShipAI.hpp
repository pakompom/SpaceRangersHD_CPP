#pragma once
#include "types/aConst.hpp"
#include "types/ab_ShipAI.hpp"

namespace ab_ShipAI {
    std::int32_t RewardTechArgument(std::int32_t Value);

    void SelectArcadeRewardWeapon(TabShipAI* Ship, std::int32_t Tech, aConst::PWeaponInfo& Info);

    void TabShipAI_Create(TabShipAI* Self);

    void TabShipAI_Destroy(TabShipAI* Self);

} // namespace ab_ShipAI
