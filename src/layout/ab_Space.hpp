#pragma once
#include "types/ab_Space.hpp"

namespace ab_Space {
    static_assert(sizeof(void*) != 4 || sizeof(ab_Space::TabSpace) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, GridPosition) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, MapPosition) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, IncomingCount) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, OutgoingCount) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Color28) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Color2C) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Color30) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Color34) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, AppearanceIndex) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, MapPath) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, BoundaryKind) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, PortalSlotCount) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Danger) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, ApproachDanger) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, RouteCost) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Objects) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, ImageActive) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpace, Image) == 104);
    static_assert(sizeof(void*) != 4 || sizeof(ab_Space::TabSpaceLink) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpaceLink, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpaceLink, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpaceLink, First) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpaceLink, Last) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpaceLink, ExitIndex) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Space::TabSpaceLink, Points) == 20);

} // namespace ab_Space
