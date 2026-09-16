#pragma once
#include "types/ab_ShipAI.hpp"

namespace ab_ShipAI {
    static_assert(sizeof(void*) != 4 || sizeof(ab_ShipAI::TabShipAI) == 904);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, CurrentZone) == 736);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, InsideCurrentZone) == 740);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, CurrentZoneBearing) == 744);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, CurrentZoneAngularRadius) == 752);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, HeadingInsideCurrentZone) == 760);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, TargetShip) == 764);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, TargetBearing) == 768);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, ReverseTargetBearing) == 784);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, TargetPathClear) == 800);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, RouteZone) == 804);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, RouteBearing) == 808);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, RouteAngularRadius) == 816);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, HeadingInsideRoute) == 824);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, DirectPathClear) == 825);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, DirectBearing) == 832);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, DirectTargetLongitude) == 848);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, DirectTargetPolarAngle) == 856);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, Intent) == 864);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, CombatManeuver) == 868);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, ManeuverUntilTick) == 872);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, TargetBonus) == 876);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, BonusRouteZone) == 880);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, AvoidanceZone) == 884);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, DamagingZone) == 888);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, LastDamageTick) == 892);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, RecentHitCount) == 896);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, IncomingThreat) == 900);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, RetreatRequested) == 901);
    static_assert(sizeof(void*) != 4 || offsetof(ab_ShipAI::TabShipAI, AIEnabled) == 902);

} // namespace ab_ShipAI
