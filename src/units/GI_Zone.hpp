#pragma once
#include "types/GI_Zone.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Zone {
    void TZoneGI_Create(TZoneGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TZoneGI_Destroy(TZoneGI* Self);

} // namespace GI_Zone
