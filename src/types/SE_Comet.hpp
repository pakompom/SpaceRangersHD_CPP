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

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace SE_Comet {
    struct TCometTrailEntry;

    struct TCometSE;

    using PCometTrailEntry = TCometTrailEntry*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCometSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TCometSE, SE_Space::TObjectSE, "TCometSE", 288)
        void p_destroy() override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        void ResetTrajectory(float Angle);
        void StartMotionTimer();
        void StopMotionTimer();
        void AdvanceMotionTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        void ApplyAttraction(EC_Struct::TPointF Center, float Strength);
        void ExplosionFinished(GI_MessageLoop::TObjectGI* Sender);
        void RemoveTrailEntry(PCometTrailEntry Entry);
        void ExplodeAndRespawn();
        void AdvanceSteps(std::int32_t Count);
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        std::int32_t TimerInterval;
        SE_Space::PSpaceTimerSE MoveTimer;
        pas::WideString ImagePath;
        // Native managed-field table includes this slot; no access found in the retained comet routines.
        pas::WideString ReservedImageText;
        pas::WideString ExplosionPath;
        pas::WideString ExplosionFrames;
        pas::WideString TrailPath;
        pas::WideString TrailFrames;
        PCometTrailEntry FirstTrailEntry;
        std::uint8_t cpp_padding[4];
        std::int32_t SavedFrameIndex;
        std::uint8_t cpp_padding_2[120];
        std::int32_t TrailHistoryCount;
        GI_GAI::TgaiGI* Animation;
        GI_GAI::TgaiGI* CompletedExplosion;
        GI_GAI::TgaiGI* CurrentExplosion;
        std::int32_t SkipMoves;
        EC_Struct::TPointF Velocity;
        float MoveAngle;
        float Radius;
        float Speed;
        float StarAttraction;
        float ObjectAttraction;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TCometTrailEntry {
        PCometTrailEntry Next;
        PCometTrailEntry Prev;
        EC_Struct::TPointF Position;
        EC_Struct::TPointF Velocity;
        GI_GAI::TgaiGI* Animation;
        std::uint8_t Finished;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

} // namespace SE_Comet
