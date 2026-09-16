#pragma once
#include "types/GI_PolyLine.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PolyLine {
    void TPolyLineGI_Create(TPolyLineGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TPolyLineGI_Destroy(TPolyLineGI* Self);

} // namespace GI_PolyLine
