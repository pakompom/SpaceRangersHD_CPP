#pragma once
#include "types/SE_Hole.hpp"

namespace SE_Hole {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Hole::THoleSE) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, MapImagePath) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, Animation) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, MapImage) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, SavedSequenceFrameIndex) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, State) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, HitRadius) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, GalaxyImagePath) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, GalaxyPriority) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, NameTextPath) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Hole::THoleSE, InfoTextPath) == 116);

} // namespace SE_Hole
