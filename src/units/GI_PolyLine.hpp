#pragma once
#include "types/GI_PolyLine.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PolyLine {
    void TPolyLineGI_Create(TPolyLineGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TPolyLineGI_Destroy(TPolyLineGI* Self);

    void TPolyLineGI_LoadFromConfigPath(TPolyLineGI* Self, const pas::WideString& Path);

} // namespace GI_PolyLine
