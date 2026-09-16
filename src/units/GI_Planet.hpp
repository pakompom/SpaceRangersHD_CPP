#pragma once
#include "types/GI_Planet.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Planet {
    void TPlanetGI_Create(TPlanetGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TPlanetGI_Destroy(TPlanetGI* Self);

    void TPlanetGI_RebuildAtmosphereImage(TPlanetGI* Self);

} // namespace GI_Planet
