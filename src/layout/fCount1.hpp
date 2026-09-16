#pragma once
#include "types/fCount1.hpp"

namespace fCount1 {
    static_assert(sizeof(void*) != 4 || sizeof(fCount1::TfCount1) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, ImagePath) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, KindImagePath) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, Caption) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, Minimum) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, Maximum) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, Limit) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, Value) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, Items) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, Dragging) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, RepeatTimer) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fCount1::TfCount1, RepeatCount) == 248);

} // namespace fCount1
