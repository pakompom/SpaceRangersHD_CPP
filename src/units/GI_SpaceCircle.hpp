#pragma once
#include "types/GI_SpaceCircle.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_SpaceCircle {
    void TSpaceCircleGI_Create(TSpaceCircleGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TSpaceCircleGI_Destroy(TSpaceCircleGI* Self);

} // namespace GI_SpaceCircle
