#pragma once
#include "types/ab_Global.hpp"
#include "types/ab_Zone.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace ab_Zone {
    extern std::uint32_t ZoneHeap;

    extern ab_Zone::PabZone FirstZone;

    extern ab_Zone::PabZone LastZone;

    extern ab_Zone::PabZone SelectedZone;

    extern std::uint32_t ZoneLinkHeap;

    extern ab_Zone::PabZoneLink FirstZoneLink;

    extern ab_Zone::PabZoneLink LastZoneLink;

    extern ab_Zone::PabZoneLink SelectedZoneLink;

    void ab_Zone_Clear();

    PabZone ab_Zone_Add();

    void ab_Zone_Delete(PabZone Zone);

    void ab_Zone_UpdatePosition(PabZone Zone);

    void ab_Zone_UpdateImages(PabZone Zone);

    void ab_Zone_ClearImages();

    void ab_Zone_ClearSegments(PabZone Zone);

    std::int32_t ab_Zone_CountKind(std::int32_t Kind);

    PabZone ab_Zone_Get(std::int32_t Index);

    PabZone ab_Zone_GetKind(std::int32_t Kind, std::int32_t Index);

    void ab_ZoneLink_Clear();

    PabZoneLink ab_ZoneLink_Add();

    void ab_ZoneLink_Delete(PabZoneLink Link);

    void ab_ZoneLink_UpdateDistance(PabZoneLink Link);

    void ab_ZoneLink_ClearImages();

    void ab_ZoneLink_ClearSegments(PabZoneLink Link);

    void ab_Zone_Load(EC_Buf::TBufEC* Buffer);

    PabZone ab_Zone_RandomKind(std::int32_t Kind);

    ab_Global::TSphericalBearingState ab_Zone_RandomPosition(PabZone Zone);

    std::uint8_t ab_Zone_FindContainingOrNearest(double Longitude, double PolarAngle, PabZone& Nearest);

    PabZone ab_Zone_FindNearestOutside(double Longitude, double PolarAngle);

    PabZone ab_Zone_FindNearestEnabled(double Longitude, double PolarAngle);

    std::uint8_t ab_Zone_IsInsideKind10(double Longitude, double PolarAngle);

    PabZone ab_Zone_FindRoute(PabZone Source, PabZone Target);

    void ab_Zone_BuildRoutes(PabZone Zone);

    void ab_Zone_BuildAllRoutes();

    PabZone ab_Zone_GetRoute(PabZone Source, PabZone Target);

    std::uint8_t ab_Zone_IsHeadingInside(ab_Global::TSphericalBearingState Source, PabZone Zone, double& BearingDelta, double& AngularRadius);

    PabZone ab_Zone_FindReachableRouteZone(PabZone Source);

    PabZone ab_Zone_RandomRoute(PabZone Source, std::int32_t Steps);

} // namespace ab_Zone
