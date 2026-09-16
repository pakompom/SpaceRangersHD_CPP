#pragma once
#include "types/SE_Asteroid.hpp"

namespace SE_Asteroid {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Asteroid::TAsteroidSE) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Asteroid::TAsteroidSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Asteroid::TAsteroidSE, MapImagePath) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Asteroid::TAsteroidSE, Animation) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Asteroid::TAsteroidSE, MapImage) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Asteroid::TAsteroidSE, SavedSequenceFrameIndex) == 92);

} // namespace SE_Asteroid
