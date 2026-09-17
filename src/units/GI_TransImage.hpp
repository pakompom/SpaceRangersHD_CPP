#pragma once
#include "types/GI_TransImage.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_TransImage {
    void TTransImageGI_Create(TTransImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TTransImageGI_Destroy(TTransImageGI* Self);

    void TTransImageGI_LoadFromConfigPath(TTransImageGI* Self, const pas::WideString& Path);

} // namespace GI_TransImage
