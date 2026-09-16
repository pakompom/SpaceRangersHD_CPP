#pragma once
#include "types/GI_Label.hpp"

namespace GI_Label {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Label::TLabelGI) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, FontCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, EmbeddedImage) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextLines) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextColor) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextBorderWidth) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextBorderColor) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextShadowOffset) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextShadowColor) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, BorderEnabled) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, BorderLightColor) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, BorderDarkColor) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextAlignX) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextAlignY) == 333);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextLeft) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextTop) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, WordWrapEnabled) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, AutoHeightPadding) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, CreateEmbeddedControl) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Label::TLabelGI, TextTexture) == 360);

} // namespace GI_Label
