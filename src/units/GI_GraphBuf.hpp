#pragma once
#include "types/GI_GraphBuf.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_GraphBuf {
    void TGraphBufGI_Create(TGraphBufGI* Self, GI_MessageLoop::TObjectGI* Owner, std::uint8_t UseTexture);

    // Frees GraphBuf only when it is owned.
    void TGraphBufGI_Destroy(TGraphBufGI* Self);

} // namespace GI_GraphBuf
