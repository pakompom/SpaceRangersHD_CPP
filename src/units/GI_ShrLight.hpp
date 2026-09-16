#pragma once
#include "types/GI_ShrLight.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_ShrLight {
    void TShrLightGI_Create(TShrLightGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TShrLightGI_Destroy(TShrLightGI* Self);

} // namespace GI_ShrLight
