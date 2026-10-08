#pragma once
#include "units/GI_MessageLoop.hpp"
#include "units/GI_XviD.hpp"

namespace GI_XviD {
    inline void TxvidGI_Create(TxvidGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
    }

} // namespace GI_XviD
