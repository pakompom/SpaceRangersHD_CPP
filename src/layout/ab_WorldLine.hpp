#pragma once
#include "types/ab_WorldLine.hpp"

namespace ab_WorldLine {
    static_assert(sizeof(void*) != 4 || sizeof(ab_WorldLine::TabWorldLine) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, First) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, Last) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, Segment) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, Kind) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, FrontColor) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, BackColor) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, FrontEndColor) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, BackEndColor) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldLine::TabWorldLine, ShowBehindSphere) == 80);

} // namespace ab_WorldLine
