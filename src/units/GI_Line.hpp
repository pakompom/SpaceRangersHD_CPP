#pragma once
#include "types/GI_Line.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Line {
    void TLineGI_Create(TLineGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TLineGI_Destroy(TLineGI* Self);

    void TLineGI_LoadFromConfigPath(TLineGI* Self, const pas::WideString& Path);

} // namespace GI_Line
