#pragma once
#include "types/GI_SBPath.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_SBPath {
    void TSBPathGI_Create(TSBPathGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TSBPathGI_Destroy(TSBPathGI* Self);

    void TSBPathGI_LoadFromConfigPath(TSBPathGI* Self, const pas::WideString& Path);

} // namespace GI_SBPath
