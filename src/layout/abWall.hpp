#pragma once
#include "types/abWall.hpp"

namespace abWall {
    static_assert(sizeof(void*) != 4 || sizeof(abWall::TabWall) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(abWall::TabWall, Zone) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(abWall::TabWall, WorldImage) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(abWall::TabWall, DirectionFrameCount) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(abWall::TabWall, StopPoint) == 220);

} // namespace abWall
