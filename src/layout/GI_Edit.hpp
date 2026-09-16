#pragma once
#include "types/GI_Edit.hpp"

namespace GI_Edit {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Edit::TEditGI) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, FontCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, BackgroundCache) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, Text) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, TextColor) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, CaretColor) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, BorderEnabled) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, BorderLightColor) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, BorderDarkColor) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, MaxLength) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, HasFocus) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, CaretPosition) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, AutoScrollText) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, TextAlignX) == 333);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, ChangedCallback) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, FocusLostCallback) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, AcceptCharCallback) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Edit::TEditGI, ClearFocusOnEnter) == 360);

} // namespace GI_Edit
