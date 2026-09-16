#pragma once
#include "types/GI_InfiniteImage.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_InfiniteImage {
    void TInfiniteImageGI_Create(TInfiniteImageGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TInfiniteImageGI_Destroy(TInfiniteImageGI* Self);

} // namespace GI_InfiniteImage
