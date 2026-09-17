#pragma once
#include "types/GI_AlphaImage.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_AlphaImage {
    void TAlphaImageGI_Create(TAlphaImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TAlphaImageGI_Destroy(TAlphaImageGI* Self);

    void TAlphaImageGI_LoadFromConfigPath(TAlphaImageGI* Self, const pas::WideString& Path);

} // namespace GI_AlphaImage
