#pragma once
#include "types/GI_Grid.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Grid {
    void TGridGI_Create(TGridGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TGridGI_Destroy(TGridGI* Self);

} // namespace GI_Grid
