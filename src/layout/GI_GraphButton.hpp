#pragma once
#include "types/GI_GraphButton.hpp"

namespace GI_GraphButton {
    static_assert(sizeof(void*) != 4 || sizeof(GI_GraphButton::TGraphButtonGI) == 500);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, Kind) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, HitKind) == 289);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, Down) == 290);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, Disabled) == 291);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, DownCallback) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, UpCallback) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, StateChangedCallback) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ImageNormal) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ImageNormalActive) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ImageDown) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ImageDownActive) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ImageDisabled) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ImageDisabledActive) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ImageHit) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, CaptionLabel) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, NormalOffset) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, NormalActiveOffset) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, DownOffset) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, DownActiveOffset) == 376);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, DisabledOffset) == 384);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, DisabledActiveOffset) == 392);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, HitOffset) == 400);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, EnterSound) == 408);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, LeaveSound) == 412);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ClickSound) == 416);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, CaptionOffsets) == 420);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, CaptionColors) == 436);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, CaptionShadowColors) == 460);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, CaptionAlignX) == 484);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, CaptionAlignY) == 485);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, ImageAutoUpdateFlags) == 488);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, UpOnlyDown) == 492);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GraphButton::TGraphButtonGI, OnPressCode) == 496);

} // namespace GI_GraphButton
