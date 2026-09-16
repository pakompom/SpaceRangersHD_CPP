#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

namespace SE_Laser {
    struct TLaserSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TLaserSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TLaserSE, SE_Space::TObjectSE, "TLaserSE", 120)
        void p_destroy() override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        void RebuildSegments();
        void ClearSegments();
        void UpdateSegmentImages();
        void StartAnimationTimer();
        void StopAnimationTimer();
        void AdvanceAnimationTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        EC_Str::TStringsEC* FrameImages;
        std::uint32_t FrameInterval;
        EC_Struct::TPointF TargetPosition;
        // Template RadiusUnit; used as sprite dimensions and beam spacing.
        std::int32_t SegmentSize;
        // Owned rotated image controls while attached.
        pas::List* Segments;
        std::int32_t FrameIndex;
        GI_MessageLoop::PCallbackTimerGI AnimationTimer;
        std::uint8_t ManualAnimation;
        std::uint8_t cpp_padding[3];
        // Rebuilt endpoint after the final segment.
        EC_Struct::TPointF EndPosition;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Laser
