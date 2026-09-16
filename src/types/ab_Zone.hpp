#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_PolyLine.hpp"
#include "types/ab_WorldImage.hpp"
#include "types/ab_WorldLine.hpp"

namespace ab_Zone {
    struct TabZone;

    struct TabZoneLink;

    using PabZone = TabZone*;

    struct TabZone {
        PabZone Prev;
        PabZone Next;
        std::int32_t RouteIndex;
        std::uint8_t cpp_padding[4];
        double Longitude;
        double PolarAngle;
        double RadiusDegrees;
        double Radius;
        EC_Struct::TVector3D Position;
        double RouteDistance;
        pas::List* Routes;
        pas::DynArray<ab_WorldLine::PabWorldLine> WorldLines;
        ab_WorldImage::PabWorldImage WorldImage;
        pas::WideString Name;
        // Initial wall health for kinds 5..8; TfAB.EnterCurrentSpace ().
        std::int32_t BarrierHealth;
        // Signed attraction/repulsion strength.
        std::int32_t GravityStrength;
        // Negative values heal.
        std::int32_t DamagePerTick;
        // Low eight bits select bonuses; bit 31 conceals the bonus icon.
        std::uint32_t BonusFlags;
        // Selects a min/max pair in BonusRespawnSeconds (three intervals).
        std::int32_t BonusRespawnClass;
        // -1 while a spawned bonus is present.
        std::int32_t NextBonusTick;
        // Native numeric tags retained; <5 participates in route tables.
        std::int32_t Kind;
        pas::Array<GI_PolyLine::PPolyLineSegmentGI, 0, 3> Segments;
        std::uint8_t cpp_padding_2[4];
    };

    using PabZoneLink = TabZoneLink*;

    #pragma pack(push, 1)
    struct TabZoneLink {
        PabZoneLink Prev;
        PabZoneLink Next;
        PabZone First;
        PabZone Last;
        double Distance;
        // Mode 1 creates collidable links between compatible barrier zones ().
        std::int32_t BarrierLinkMode;
        ab_WorldLine::PabWorldLine WorldLine;
        pas::Array<GI_PolyLine::PPolyLineSegmentGI, 0, 1> Segments;
    };
    #pragma pack(pop)

} // namespace ab_Zone
