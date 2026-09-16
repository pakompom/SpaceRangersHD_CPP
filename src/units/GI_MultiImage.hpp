#pragma once
#include "types/GI_MultiImage.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_MultiImage {
    void TMultiImageImageGI_Create(TMultiImageImageGI* Self);

    void TMultiImageImageGI_Destroy(TMultiImageImageGI* Self);

    void TMultiImageGI_Create(TMultiImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TMultiImageGI_Destroy(TMultiImageGI* Self);

} // namespace GI_MultiImage
