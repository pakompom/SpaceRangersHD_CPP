#pragma once
#include "types/GI_RotateImage.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RotateImage {
    void TRotateImageGI_Create(TRotateImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TRotateImageGI_Destroy(TRotateImageGI* Self);

} // namespace GI_RotateImage
