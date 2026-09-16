#pragma once
#include "types/ab_Ship.hpp"

namespace ab_Ship {
    extern ab_Ship::TabShip* PlayerArcadeShip;

    extern ab_Ship::TabShip* KellerArcadeShip;

    // Keeps Keller alive while present; exact visual role under review.
    extern ab_Ship::TabShip* KellerAuxiliaryShip;

    extern std::uint8_t ArcadePaused;

    extern std::uint8_t ArcadePauseWithShift;

    void ab_Ship_RepelOverlaps();

    void TabShip_Create(TabShip* Self);

    void TabShip_Destroy(TabShip* Self);

} // namespace ab_Ship
