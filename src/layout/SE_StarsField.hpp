#pragma once
#include "types/SE_StarsField.hpp"

namespace SE_StarsField {
    static_assert(sizeof(void*) != 4 || sizeof(SE_StarsField::TStarsFieldSE) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_StarsField::TStarsFieldSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_StarsField::TStarsFieldSE, InfiniteImage) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_StarsField::TStarsFieldSE, StaticImage) == 84);

} // namespace SE_StarsField
