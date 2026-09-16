#pragma once
#include "types/SE_BGObj.hpp"

namespace SE_BGObj {
    static_assert(sizeof(void*) != 4 || sizeof(SE_BGObj::TBGObjSE) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_BGObj::TBGObjSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_BGObj::TBGObjSE, Radius) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_BGObj::TBGObjSE, Image) == 84);

} // namespace SE_BGObj
