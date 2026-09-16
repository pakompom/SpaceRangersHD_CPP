#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_GI {
    struct TgiGI;

} // namespace GI_GI

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace SE_Star {
    struct TStarSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStarSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TStarSE, SE_Space::TObjectSE, "TStarSE", 120)
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        std::int32_t GetSequenceFrameIndex();
        void SetSequenceFrameIndex(std::int32_t FrameIndex);
        std::uint8_t HitTestCursor() override;
        void DrawMap() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString AnimationPath;
        pas::WideString StaticImagePath;
        Types::TPoint ImageOrigin;
        pas::WideString MapImagePath;
        Types::TPoint MapImageOrigin;
        GI_Image::TImageGI* StaticImage;
        GI_GAI::TgaiGI* Animation;
        GI_GI::TgiGI* MapImage;
        std::int32_t SavedSequenceFrameIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Star
