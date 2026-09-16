#pragma once
#include "types/GlobalsV.hpp"

namespace GlobalsV {
    static_assert(sizeof(void*) != 4 || sizeof(GlobalsV::TSpaceImageTemplate) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GlobalsV::TSpaceImageTemplate, Kind) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GlobalsV::TSpaceImageTemplate, Weight) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GlobalsV::TSpaceImageTemplate, CacheControl) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GlobalsV::TSpaceImageTemplate, CachedData) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(GlobalsV::TStarFieldImageTemplate) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GlobalsV::TStarFieldImageTemplate, Reserved) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GlobalsV::TStarFieldImageTemplate, Weight) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GlobalsV::TStarFieldImageTemplate, CacheControl) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GlobalsV::TStarFieldImageTemplate, CachedData) == 12);

} // namespace GlobalsV
