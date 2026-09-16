#pragma once
#include "runtime_support.hpp"
#include "types/ab_Global.hpp"
#include "types/ab_Ship.hpp"
#include "types/ab_Zone.hpp"

namespace aItem {
    struct TItem;

} // namespace aItem

namespace ab_Item {
    struct TabItem;

} // namespace ab_Item

namespace ab_Object {
    struct TabObject;

} // namespace ab_Object

namespace ab_ShipAI {
    struct TabShipAI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabShipAI : ab_Ship::TabShip {
        PAS_CLASS_META(TabShipAI, ab_Ship::TabShip, "TabShipAI", 904)
        void p_destroy() override;
        aItem::TItem* GetRewardItem(std::uint8_t Preview);
        void ApplyDamage(std::int32_t Amount, ab_Object::TabObject* Source, std::uint8_t Disrupt) override;
        void UpdateState() override;
        void Advance() override;
        void NoticeCollision();
        void NoticeDamagingZone(ab_Zone::PabZone Zone);
        void ResetIntent();
        void DecideActions();
        void SetDirectDestination(float Longitude, float PolarAngle);
        void FollowDirectDestination();
        void AvoidImmediateObstacle();
        void ClearRoute();
        void SetRoute(ab_Zone::PabZone Target);
        void FollowRoute();
        void ApproachTarget();
        std::int32_t ScoreApproach();
        void EvadeCloseTarget();
        std::int32_t ScoreCloseEvasion();
        void FlankTarget();
        std::int32_t ScoreFlanking();
        void ReverseTowardTarget();
        std::int32_t ScoreReverseTurn();
        void MatchReversingTarget();
        std::int32_t ScoreReverseFollowing();
        std::uint8_t TryMoveToDestination(ab_Zone::PabZone Zone, double Longitude, double PolarAngle);
        void SetAndFollowRoute(ab_Zone::PabZone Target);
        void FollowDestinationRoute();
        ab_Zone::PabZone CurrentZone;
        std::uint8_t InsideCurrentZone;
        std::uint8_t cpp_padding[3];
        double CurrentZoneBearing;
        double CurrentZoneAngularRadius;
        std::uint8_t HeadingInsideCurrentZone;
        std::uint8_t cpp_padding_2[3];
        ab_Ship::TabShip* TargetShip;
        ab_Global::TSphericalBearingDistance TargetBearing;
        ab_Global::TSphericalBearingDistance ReverseTargetBearing;
        std::uint8_t TargetPathClear;
        std::uint8_t cpp_padding_3[3];
        ab_Zone::PabZone RouteZone;
        double RouteBearing;
        double RouteAngularRadius;
        std::uint8_t HeadingInsideRoute;
        std::uint8_t DirectPathClear;
        std::uint8_t cpp_padding_4[6];
        ab_Global::TSphericalBearingDistance DirectBearing;
        double DirectTargetLongitude;
        double DirectTargetPolarAngle;
        // Native numeric intent; used by scripted/campaign steering.
        std::int32_t Intent;
        // am* selector; negative values request a new choice.
        std::int32_t CombatManeuver;
        std::int32_t ManeuverUntilTick;
        ab_Item::TabItem* TargetBonus;
        ab_Zone::PabZone BonusRouteZone;
        ab_Zone::PabZone AvoidanceZone;
        ab_Zone::PabZone DamagingZone;
        std::int32_t LastDamageTick;
        std::int32_t RecentHitCount;
        std::uint8_t IncomingThreat;
        std::uint8_t RetreatRequested;
        std::uint8_t AIEnabled;
        std::uint8_t cpp_padding_5[1];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Native DecideActions () selects and dispatches these maneuvers.
    inline constexpr std::int32_t amUnselected = -1;

    inline constexpr std::int32_t amApproach = 0;

    inline constexpr std::int32_t amCloseEvasion = 1;

    inline constexpr std::int32_t amFlank = 2;

    inline constexpr std::int32_t amReverseTurn = 3;

    inline constexpr std::int32_t amFollowReverse = 4;

} // namespace ab_ShipAI
