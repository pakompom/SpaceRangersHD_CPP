#pragma once
#include "runtime_support.hpp"

namespace EC_OKGF {
    struct TOkgfReadContext;

    enum TOkgfImageKind : std::uint32_t {
        oikUnknown = 0,
        oikBmp = 1,
        oikIndexedBmp = 2,
        oikJpeg = 3,
        oikPng = 4,
        oikIndexedPsd = 5,
        oikGrayscalePsd = 6,
        oikRgbPsd = 7,
        oikCmykPsd = 8,
    };

    #pragma pack(push, 1)
    struct TOkgfReadContext {
        void* CodecContext;
        TOkgfImageKind ImageKind;
        std::int32_t Width;
        std::int32_t Height;
        std::int32_t PaletteCount;
        void* SourceData;
        std::int32_t SourceSize;
        std::int32_t OwnsSource;
    };
    #pragma pack(pop)

    using POkgfReadContext = TOkgfReadContext*;

} // namespace EC_OKGF
