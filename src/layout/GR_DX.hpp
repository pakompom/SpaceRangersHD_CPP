#pragma once
#include "types/GR_DX.hpp"

namespace GR_DX {
    static_assert(sizeof(void*) != 4 || sizeof(GR_DX::TTextureGR) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TTextureGR, LastUseTick) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TTextureGR, SurfaceCount) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TTextureGR, Surfaces) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TTextureGR, ResidentBytes) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(GR_DX::TScreenVertexGR) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TScreenVertexGR, X) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TScreenVertexGR, Y) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TScreenVertexGR, Z) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TScreenVertexGR, RHW) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TScreenVertexGR, Color) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TScreenVertexGR, U) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_DX::TScreenVertexGR, V) == 24);

} // namespace GR_DX
