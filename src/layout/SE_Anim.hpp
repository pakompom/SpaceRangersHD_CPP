#pragma once
#include "types/SE_Anim.hpp"

namespace SE_Anim {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Anim::TAnimSE) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Anim::TAnimSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Anim::TAnimSE, ImageOrigin) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Anim::TAnimSE, LoopAnimation) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Anim::TAnimSE, Animation) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Anim::TAnimSE, FinishedCallback) == 96);

} // namespace SE_Anim
