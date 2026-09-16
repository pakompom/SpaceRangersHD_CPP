#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RotateImage5 {
    struct TRotateImage5GI;

} // namespace GI_RotateImage5

namespace SE_Missile {
    struct TMissileSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMissileSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TMissileSE, SE_Space::TObjectSE, "TMissileSE", 92)
        void p_destroy() override;
        // Native diagnostic name: TMissileSE.Connect.
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        std::uint8_t GetAngle() override;
        void SetAngle(std::uint8_t Value) override;
        std::uint8_t HitTestCursor() override;
        void AdvanceAnimationTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        void DrawMap() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        // Native empty override.
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        // Native empty override.
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        std::uint8_t Angle;
        std::uint8_t cpp_padding[3];
        float ImageScale;
        GI_RotateImage5::TRotateImage5GI* Image;
        SE_Space::PSpaceTimerSE AnimationTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Missile
