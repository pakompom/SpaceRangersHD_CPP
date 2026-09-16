#pragma once
#include "types/fCount2.hpp"

namespace fCount2 {
    static_assert(sizeof(void*) != 4 || sizeof(fCount2::TfCount2) == 260);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, ImagePath) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, PreviewImagePath) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, Description) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, Minimum) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, Maximum) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, Limit) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, Value) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, UnitValue) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, Available) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, TotalLimit) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, Dragging) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, RepeatTimer) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fCount2::TfCount2, FontName) == 256);

} // namespace fCount2
