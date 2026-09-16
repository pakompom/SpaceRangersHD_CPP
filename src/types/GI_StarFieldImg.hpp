#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_StarFieldImg {
    struct TStarFieldImageGI;

    struct TStarFieldImgGI;

    using PStarFieldImageGI = TStarFieldImageGI*;

    // Native VMT.
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStarFieldImgGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TStarFieldImgGI, GI_MessageLoop::TObjectGI, "TStarFieldImgGI", 344)
        void p_destroy() override;
        void ClearStars();
        // Adds 64 zeroed entries.
        void GrowStars();
        PStarFieldImageGI AllocateStar();
        // Copies particles only, not camera or timer state.
        void CopyStarsFrom(TStarFieldImgGI* Source);
        void InitializeStar(PStarFieldImageGI Star);
        // Clears/reseeds the animated image stars and advances 201 warm-up steps.
        void SeedStars();
        void AdvanceStars();
        void RedirectStars();
        void AnimateStars(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SetViewPosition(EC_Struct::TPointF Position);
        void Invalidate() override;
        void OnActivate() override;
        void OnDeactivate() override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        // Native empty extension hook.
        static void ApplyStarConfig(EC_BlockPar::TBlockParEC* Block);
        void UpdateAutoGeometry() override;
        void Draw(WindowsSdk::TRect ClipRect) override;
        PStarFieldImageGI Stars;
        std::int32_t StarCount;
        std::int32_t Capacity;
        // Set by construction; no reads found in the native unit.
        std::uint8_t ReservedDirty;
        std::uint8_t cpp_padding[3];
        // Stars accelerate away from this screen-space point.
        EC_Struct::TPointF FocusPoint;
        EC_Struct::TPointF ViewPosition;
        float TargetHeading;
        float CurrentHeading;
        float TargetFocusDistance;
        float CurrentFocusDistance;
        std::int32_t MotionTicks;
        GI_MessageLoop::PCallbackTimerGI AnimationTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TStarFieldImageGI {
        std::int32_t TemplateIndex;
        std::int32_t FrameIndex;
        std::int32_t LastFrame;
        float FramePosition;
        float FrameStep;
        EC_Struct::TPointF Position;
        EC_Struct::TPointF Velocity;
        EC_Struct::TPointF Acceleration;
        EC_Struct::TPointF Direction;
        WindowsSdk::TPoint ImageSize;
        // Native positive half-size, added to the pixel position.
        WindowsSdk::TPoint ImageOffset;
        WindowsSdk::TPoint PixelPosition;
    };
    #pragma pack(pop)

} // namespace GI_StarFieldImg
