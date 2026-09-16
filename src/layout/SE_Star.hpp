#pragma once
#include "types/SE_Star.hpp"

namespace SE_Star {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Star::TStarSE) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, AnimationPath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, StaticImagePath) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, ImageOrigin) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, MapImagePath) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, MapImageOrigin) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, StaticImage) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, Animation) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, MapImage) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Star::TStarSE, SavedSequenceFrameIndex) == 116);

} // namespace SE_Star
