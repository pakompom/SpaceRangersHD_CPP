#pragma once
#include "types/GI_AImage.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_AImage {
    void TAImageGI_Create(TAImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TAImageGI_Destroy(TAImageGI* Self);

} // namespace GI_AImage
