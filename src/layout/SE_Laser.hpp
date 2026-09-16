#pragma once
#include "types/SE_Laser.hpp"

namespace SE_Laser {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Laser::TLaserSE) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, FrameImages) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, FrameInterval) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, TargetPosition) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, SegmentSize) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, Segments) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, FrameIndex) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, AnimationTimer) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, ManualAnimation) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Laser::TLaserSE, EndPosition) == 112);

} // namespace SE_Laser
