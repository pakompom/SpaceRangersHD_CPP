#pragma once
#include "types/GI_Frame.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Frame {
    void TFrameGI_Create(TFrameGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TFrameGI_Destroy(TFrameGI* Self);

} // namespace GI_Frame
