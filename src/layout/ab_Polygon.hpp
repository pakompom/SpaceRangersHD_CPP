#pragma once
#include "types/ab_Polygon.hpp"

namespace ab_Polygon {
    static_assert(sizeof(void*) != 4 || sizeof(ab_Polygon::TabPolygonVertex) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabPolygonVertex, Point) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabPolygonVertex, Color) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(ab_Polygon::TabPolygon) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabPolygon, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabPolygon, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabPolygon, Vertices) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabPolygon, MapValue30) == 48);
    static_assert(sizeof(void*) != 4 || sizeof(ab_Polygon::TabOptGroup) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabOptGroup, Polygons) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabOptGroup, Corners) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(ab_Polygon::TabOptUnit) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabOptUnit, Points) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_Polygon::TabOptUnit, Groups) == 4);

} // namespace ab_Polygon
