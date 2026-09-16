#pragma once
#include "runtime_support.hpp"

namespace GI_Main {
    enum TImageKindXGI : std::uint8_t {
        ikxLeftFill = 0,
        ikxCenterFill = 1,
        ikxRightFill = 2,
        ikxLeft = 3,
        ikxCenter = 4,
        ikxRight = 5,
    };

    enum TImageKindYGI : std::uint8_t {
        ikyTopFill = 0,
        ikyCenterFill = 1,
        ikyBottomFill = 2,
        ikyTop = 3,
        ikyCenter = 4,
        ikyBottom = 5,
    };

    enum TTextAlignXGI : std::uint8_t {
        taxLeft = 0,
        taxCenter = 1,
        taxRight = 2,
        taxAuto = 3,
    };

    enum TTextAlignYGI : std::uint8_t {
        tayTop = 0,
        tayCenter = 1,
        tayCenterEx = 2,
        tayBottom = 3,
        tayAuto = 4,
    };

    inline constexpr std::int32_t agfPosition = 0x00000001;

    inline constexpr std::int32_t agfSize = 0x00000002;

} // namespace GI_Main
