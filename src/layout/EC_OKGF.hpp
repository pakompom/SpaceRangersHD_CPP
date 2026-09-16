#pragma once
#include "types/EC_OKGF.hpp"

namespace EC_OKGF {
    static_assert(sizeof(void*) != 4 || sizeof(EC_OKGF::TOkgfReadContext) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_OKGF::TOkgfReadContext, CodecContext) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_OKGF::TOkgfReadContext, ImageKind) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_OKGF::TOkgfReadContext, Width) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_OKGF::TOkgfReadContext, Height) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_OKGF::TOkgfReadContext, PaletteCount) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_OKGF::TOkgfReadContext, SourceData) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_OKGF::TOkgfReadContext, SourceSize) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_OKGF::TOkgfReadContext, OwnsSource) == 28);

} // namespace EC_OKGF
