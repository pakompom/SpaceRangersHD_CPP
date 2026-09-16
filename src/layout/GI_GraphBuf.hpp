#pragma once
#include "types/GI_GraphBuf.hpp"

namespace GI_GraphBuf {
    static_assert(sizeof(void*) != 4 || sizeof(GI_GraphBuf::TGraphBufGI) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphBuf::TGraphBufGI, GraphBuf) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphBuf::TGraphBufGI, ImageKindX) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphBuf::TGraphBufGI, ImageKindY) == 293);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphBuf::TGraphBufGI, HalfAlpha) == 294);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphBuf::TGraphBufGI, SourceHasPerPixelAlpha) == 295);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphBuf::TGraphBufGI, UsesExternalGraphBuf) == 296);

} // namespace GI_GraphBuf
