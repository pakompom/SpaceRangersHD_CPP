#pragma once
#include "types/ab_StopLine.hpp"

namespace ab_StopLine {
    static_assert(sizeof(void*) != 4 || sizeof(ab_StopLine::TabStopPoint) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, Longitude) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, PolarAngle) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, Radius) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, Position) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, ScreenX) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, ScreenY) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, Projected) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, Kind) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, WorldImage) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopPoint, Segments) == 84);
    static_assert(sizeof(void*) != 4 || sizeof(ab_StopLine::TabStopLine) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, NextCollision) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, UserValue) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, First) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, Last) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, FirstColor) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, LastColor) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, WorldLine) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, Collidable) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, Visible) == 37);
    static_assert(sizeof(void*) != 4 || offsetof(ab_StopLine::TabStopLine, Segments) == 40);

} // namespace ab_StopLine
