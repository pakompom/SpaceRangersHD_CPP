#pragma once
#include "types/GI_SimpleImage.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_SimpleImage {
    void TSimpleImageGI_Create(TSimpleImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TSimpleImageGI_Destroy(TSimpleImageGI* Self);

} // namespace GI_SimpleImage
