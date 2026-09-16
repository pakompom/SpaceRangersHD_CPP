#pragma once
#include "types/GI_TextButton.hpp"

namespace GI_TextButton {
    static_assert(sizeof(void*) != 4 || sizeof(GI_TextButton::TTextButtonGI) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, FontCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, ImageCache) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, Kind) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, Caption) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, CaptionColor) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, CaptionActiveColor) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, BorderLightColor) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, BorderDarkColor) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, Hover) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, Down) == 321);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, DownCallback) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_TextButton::TTextButtonGI, UpCallback) == 336);

} // namespace GI_TextButton
