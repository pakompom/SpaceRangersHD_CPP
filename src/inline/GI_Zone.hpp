#pragma once
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Zone.hpp"

namespace GI_Zone {
    inline void TZoneGI_Create(TZoneGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
    }

} // namespace GI_Zone
