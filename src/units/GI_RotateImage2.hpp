#pragma once
#include "types/GI_RotateImage2.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RotateImage2 {
    void TRotateImage2GI_Create(TRotateImage2GI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TRotateImage2GI_Destroy(TRotateImage2GI* Self);

    void TRotateImage2GI_LoadFromConfigPath(TRotateImage2GI* Self, const pas::WideString& Path);

} // namespace GI_RotateImage2
