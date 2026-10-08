#pragma once
#include "units/EC_Struct.hpp"
#include "units/fPanelRuins.hpp"

namespace fPanelRuins {
    inline void TfPanelRuins_Create(TfPanelRuins* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace fPanelRuins
