#pragma once
#include "types/GI_GI.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_GI {
    void LoadGiByPathIntoGraphBuf(const pas::WideString& GiPath, GR_GraphBuf::TGraphBufGR* Destination);

    void TgiGI_Create(TgiGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TgiGI_Destroy(TgiGI* Self);

} // namespace GI_GI
