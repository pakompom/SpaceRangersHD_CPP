#pragma once
#include "units/EC_Struct.hpp"
#include "units/fPanelPlanet.hpp"

namespace fPanelPlanet {
    inline void TfPanelPlanet_Create(TfPanelPlanet* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace fPanelPlanet
