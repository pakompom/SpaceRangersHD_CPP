#pragma once
#include "types/GI_StarField.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_StarField {
    void TStarFieldList_Create(TStarFieldList* Self);

    void TStarFieldList_Destroy(TStarFieldList* Self);

    void TStarFieldGI_Create(TStarFieldGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TStarFieldGI_Destroy(TStarFieldGI* Self);

} // namespace GI_StarField
