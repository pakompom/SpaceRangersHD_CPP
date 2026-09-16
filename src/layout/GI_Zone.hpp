#pragma once
#include "types/GI_Zone.hpp"

namespace GI_Zone {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Zone::TZoneGI) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Zone::TZoneGI, Kind) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Zone::TZoneGI, CursorInside) == 289);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Zone::TZoneGI, EnterCallback) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Zone::TZoneGI, LeaveCallback) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Zone::TZoneGI, ZoneMouseDownCallback) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Zone::TZoneGI, ZoneMouseUpCallback) == 320);

} // namespace GI_Zone
