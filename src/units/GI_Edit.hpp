#pragma once
#include "types/GI_Edit.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Edit {
    void TEditGI_Create(TEditGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TEditGI_Destroy(TEditGI* Self);

    void TEditGI_LoadFromConfigPath(TEditGI* Self, const pas::WideString& Path);

} // namespace GI_Edit
