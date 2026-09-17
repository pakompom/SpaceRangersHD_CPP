#pragma once
#include "types/GI_Door.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Door {
    void TDoorGI_Create(TDoorGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TDoorGI_Destroy(TDoorGI* Self);

    void TDoorGI_LoadFromConfigPath(TDoorGI* Self, const pas::WideString& Path);

} // namespace GI_Door
