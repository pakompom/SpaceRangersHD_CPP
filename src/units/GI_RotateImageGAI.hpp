#pragma once
#include "types/GI_RotateImageGAI.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RotateImageGAI {
    void TRotateImageGaiGI_Create(TRotateImageGaiGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TRotateImageGaiGI_Destroy(TRotateImageGaiGI* Self);

} // namespace GI_RotateImageGAI
