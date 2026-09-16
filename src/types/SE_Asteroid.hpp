#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace SE_Asteroid {
    struct TAsteroidSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TAsteroidSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TAsteroidSE, SE_Space::TObjectSE, "TAsteroidSE", 96)
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        std::int32_t GetSequenceFrameIndex();
        void SetSequenceFrameIndex(std::int32_t FrameIndex);
        // Requires an attached space.
        void DrawMap() override;
        std::uint8_t HitTestCursor() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        pas::WideString MapImagePath;
        GI_GAI::TgaiGI* Animation;
        GI_Image::TImageGI* MapImage;
        std::int32_t SavedSequenceFrameIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Asteroid
