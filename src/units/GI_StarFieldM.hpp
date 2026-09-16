#pragma once
#include "types/GI_StarFieldM.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_StarFieldM {
    void TStarFieldMGI_Create(TStarFieldMGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TStarFieldMGI_Destroy(TStarFieldMGI* Self);

} // namespace GI_StarFieldM
