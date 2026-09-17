#pragma once
#include "types/GI_AImage.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_AImage {
    void TAImageGI_Create(TAImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TAImageGI_Destroy(TAImageGI* Self);

    void TAImageGI_LoadFromConfigPath(TAImageGI* Self, const pas::WideString& Path);

    // Numeric parameter names supply frame delays; values select child images.
    void TAImageGI_LoadAnimationProperties(TAImageGI* Self, EC_BlockPar::TBlockParEC* Block);

} // namespace GI_AImage
