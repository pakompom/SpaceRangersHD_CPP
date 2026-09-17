#pragma once
#include "types/GI_MultiImage.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_MultiImage {
    // Neutral integer expressions retain DCC32 operand materialization order.
    // See docs/development.md; they emit no extra arithmetic.
    void TMultiImageImageGI_Create(TMultiImageImageGI* Self);

    void TMultiImageImageGI_Destroy(TMultiImageImageGI* Self);

    void TMultiImageGI_Create(TMultiImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TMultiImageGI_Destroy(TMultiImageGI* Self);

    void TMultiImageGI_LoadFromConfigPath(TMultiImageGI* Self, const pas::WideString& Path);

} // namespace GI_MultiImage
