#pragma once
#include "types/GI_Image.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Image {
    void TImageGI_Create(TImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TImageGI_Destroy(TImageGI* Self);

} // namespace GI_Image
