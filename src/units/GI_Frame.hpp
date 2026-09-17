#pragma once
#include "types/GI_Frame.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Frame {
    void TFrameGI_Create(TFrameGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TFrameGI_Destroy(TFrameGI* Self);

    void TFrameGI_LoadFromConfigPath(TFrameGI* Self, const pas::WideString& Path);

} // namespace GI_Frame
