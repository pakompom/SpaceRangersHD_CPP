#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/System.hpp"
#include "types/ab_StopLine.hpp"

namespace ab_Polygon {
    struct TabOptGroup;

    struct TabOptUnit;

    struct TabPolygon;

    struct TabPolygonVertex;

    using PabPolygon = TabPolygon*;

    #pragma pack(push, 1)
    struct TabPolygonVertex {
        ab_StopLine::PabStopPoint Point;
        System::PCardinal Color;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TabPolygon {
        PabPolygon Prev;
        PabPolygon Next;
        pas::Array<TabPolygonVertex, 0, 2> Vertices;
        std::uint8_t cpp_padding[16];
        std::int32_t MapValue30;
    };
    #pragma pack(pop)

    struct TabOptGroup {
        pas::DynArray<PabPolygon> Polygons;
        std::uint8_t cpp_padding[4];
        pas::Array<EC_Struct::TVector3D, 0, 3> Corners;
    };

    using PabPolygonGroup = TabOptGroup*;

    struct TabOptUnit {
        pas::DynArray<ab_StopLine::PabStopPoint> Points;
        pas::DynArray<PabPolygonGroup> Groups;
    };

    using PabPolygonCell = TabOptUnit*;

} // namespace ab_Polygon
