#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_AlphaImage {
    struct TAlphaImageGI;

} // namespace GI_AlphaImage

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace SE_Container {
    struct TContainerSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TContainerSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TContainerSE, SE_Space::TObjectSE, "TContainerSE", 92)
        void CopyTo(SE_Space::TObjectSE* Destination) override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        void SetDepth(float Value) override;
        float GetDepth() override;
        std::uint8_t HitTestCursor() override;
        void DrawMap() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        pas::WideString MinimapImagePath;
        GI_GAI::TgaiGI* Animation;
        GI_AlphaImage::TAlphaImageGI* MinimapImage;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Container
