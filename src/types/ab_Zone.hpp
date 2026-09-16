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
        std::int32_t BarrierHealth;
        std::int32_t GravityStrength;
        std::int32_t DamagePerTick;
        std::uint32_t BonusFlags;
        std::int32_t BonusRespawnClass;
        std::int32_t NextBonusTick;
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
        std::int32_t BarrierLinkMode;
        ab_WorldLine::PabWorldLine WorldLine;
        pas::Array<GI_PolyLine::PPolyLineSegmentGI, 0, 1> Segments;
    };
    #pragma pack(pop)

} // namespace ab_Zone
