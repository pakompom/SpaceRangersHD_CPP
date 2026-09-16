#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_PolyLine.hpp"
#include "types/System.hpp"
#include "types/ab_WorldImage.hpp"
#include "types/ab_WorldLine.hpp"

namespace ab_StopLine {
    struct TabStopLine;

    struct TabStopPoint;

    using PabStopPoint = TabStopPoint*;

    #pragma pack(push, 1)
    struct TabStopPoint {
        PabStopPoint Prev;
        PabStopPoint Next;
        double Longitude;
        double PolarAngle;
        float Radius;
        std::uint8_t cpp_padding[4];
        EC_Struct::TVector3D Position;
        std::int32_t ScreenX;
        std::int32_t ScreenY;
        std::uint8_t Projected;
        std::uint8_t cpp_padding_2[3];
        // Set to 1 by the latitude-ring builder; wider meaning unresolved.
        std::int32_t Kind;
        ab_WorldImage::PabWorldImage WorldImage;
        std::uint8_t cpp_padding_3[8];
        pas::Array<GI_PolyLine::PPolyLineSegmentGI, 0, 3> Segments;
        std::uint8_t cpp_padding_4[12];
    };
    #pragma pack(pop)

    using PabStopLine = TabStopLine*;

    #pragma pack(push, 1)
    struct TabStopLine {
        PabStopLine Prev;
        PabStopLine Next;
        PabStopLine NextCollision;
        std::int32_t UserValue;
        PabStopPoint First;
        PabStopPoint Last;
        System::PCardinal FirstColor;
        System::PCardinal LastColor;
        ab_WorldLine::PabWorldLine WorldLine;
        std::uint8_t Collidable;
        std::uint8_t Visible;
        std::uint8_t cpp_padding[2];
        pas::Array<GI_PolyLine::PPolyLineSegmentGI, 0, 1> Segments;
        std::uint8_t cpp_padding_2[4];
    };
    #pragma pack(pop)

} // namespace ab_StopLine
