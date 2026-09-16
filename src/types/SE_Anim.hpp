#pragma once
#include "runtime_support.hpp"
#include "types/Classes.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace SE_Anim {
    struct TAnimSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TAnimSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TAnimSE, SE_Space::TObjectSE, "TAnimSE", 104)
        void p_destroy() override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        void AnimationCycleComplete(GI_MessageLoop::TObjectGI* Sender);
        std::uint8_t HitTestCursor() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        Types::TPoint ImageOrigin;
        std::uint8_t LoopAnimation;
        std::uint8_t cpp_padding[3];
        GI_GAI::TgaiGI* Animation;
        Classes::TNotifyEvent FinishedCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Anim
