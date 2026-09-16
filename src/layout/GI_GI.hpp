#pragma once
#include "types/GI_GI.hpp"

namespace GI_GI {
    static_assert(sizeof(void*) != 4 || sizeof(GI_GI::TgiGI) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GI::TgiGI, ImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GI::TgiGI, ImageKindX) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GI::TgiGI, ImageKindY) == 293);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GI::TgiGI, Alpha) == 294);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GI::TgiGI, HardwareMirrorHorizontal) == 295);

} // namespace GI_GI
