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

namespace SE_Hole {
    struct THoleSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct THoleSE : SE_Space::TObjectSE {
        PAS_CLASS_META(THoleSE, SE_Space::TObjectSE, "THoleSE", 120)
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetState(std::int32_t Value);
        void SetPosition(EC_Struct::TPointF APosition) override;
        void DrawMap() override;
        std::uint8_t HitTestCursor() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void AnimationCycleComplete(GI_MessageLoop::TObjectGI* Sender);
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        pas::WideString MapImagePath;
        GI_GAI::TgaiGI* Animation;
        GI_Image::TImageGI* MapImage;
        std::int32_t SavedSequenceFrameIndex;
        std::int32_t State;
        std::int32_t HitRadius;
        pas::WideString GalaxyImagePath;
        std::int32_t GalaxyPriority;
        pas::WideString NameTextPath;
        pas::WideString InfoTextPath;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Hole
