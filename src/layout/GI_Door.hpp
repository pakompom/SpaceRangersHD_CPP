#pragma once
#include "types/GI_Door.hpp"

namespace GI_Door {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Door::TDoorGI) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Door::TDoorGI, Image) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Door::TDoorGI, FrameStep) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Door::TDoorGI, StepTimer) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Door::TDoorGI, StepTime) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Door::TDoorGI, ClickCallback) == 304);

} // namespace GI_Door
