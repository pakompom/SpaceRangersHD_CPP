#pragma once
#include "types/GI_PlanetButton.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PlanetButton {
    void TPlanetButtonGI_Create(TPlanetButtonGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TPlanetButtonGI_Destroy(TPlanetButtonGI* Self);

} // namespace GI_PlanetButton
