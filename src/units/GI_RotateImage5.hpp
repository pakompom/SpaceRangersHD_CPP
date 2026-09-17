#pragma once
#include "types/GI_RotateImage5.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RotateImage5 {
    extern std::int32_t RotateImageConstructionStage;

    void TRotateImage5GI_Create(TRotateImage5GI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TRotateImage5GI_Destroy(TRotateImage5GI* Self);

    void TRotateImage5GI_LoadFromConfigPath(TRotateImage5GI* Self, const pas::WideString& Path);

} // namespace GI_RotateImage5
