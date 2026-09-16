#pragma once
#include "runtime_support.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_InfiniteImage {
    struct TInfiniteImageGI;

} // namespace GI_InfiniteImage

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_SimpleImage {
    struct TSimpleImageGI;

} // namespace GI_SimpleImage

namespace SE_StarsField {
    struct TStarsFieldSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStarsFieldSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TStarsFieldSE, SE_Space::TObjectSE, "TStarsFieldSE", 88)
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        GI_InfiniteImage::TInfiniteImageGI* InfiniteImage;
        GI_SimpleImage::TSimpleImageGI* StaticImage;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_StarsField
