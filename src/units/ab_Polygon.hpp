#pragma once
#include "types/ab_Polygon.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace ab_Polygon {
    extern ab_Polygon::PabPolygon FirstPolygon;

    extern ab_Polygon::PabPolygon LastPolygon;

    extern ab_Polygon::PabPolygon PolygonStorage;

    extern pas::DynArray<ab_Polygon::TabOptGroup> PolygonGroups;

    extern pas::DynArray<ab_Polygon::TabOptUnit> PolygonCells;

    extern std::int32_t LongitudeCellCount;

    extern std::int32_t PolarCellCount;

    extern ab_Polygon::PabPolygonCell CurrentPolygonCell;

    void ab_Polygon_Clear();

    std::int32_t ab_Polygon_Count();

    void ab_Polygon_ClearVisibility();

    void ab_Polygon_LoadVisibility(EC_Buf::TBufEC* Buffer);

    void ab_Polygon_SelectVisibilityCell();

    void ab_Polygon_ProjectVisiblePoints();

    void ab_Polygon_QueueUpdateRects();

    void ab_Polygon_Draw();

    void ab_Polygon_Load(EC_Buf::TBufEC* Buffer);

} // namespace ab_Polygon
