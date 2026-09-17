#pragma once
#include "types/GI_RadioGroup.hpp"
#include "types/Types.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RadioGroup {
    void TRadioGroupGI_Create(TRadioGroupGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TRadioGroupGI_Destroy(TRadioGroupGI* Self);

    void TRadioGroupGI_AddItem(TRadioGroupGI* Self, pas::WideString Name, Types::TPoint Position);

    void TRadioGroupGI_LoadFromConfigPath(TRadioGroupGI* Self, const pas::WideString& Path);

} // namespace GI_RadioGroup
