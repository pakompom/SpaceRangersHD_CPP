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

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace SE_Angel {
    struct TAngelEntry;

    struct TAngelSE;

    using PAngelEntry = TAngelEntry*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TAngelSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TAngelSE, SE_Space::TObjectSE, "TAngelSE", 188)
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        void SetVelocityFromAngle(float Angle);
        void StartMotionTimer();
        void StopMotionTimer();
        std::int32_t GetFrameCount(PAngelEntry Entry);
        void ToggleFrameVariants();
        void AdvanceMotionTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        void AppendEntry();
        void RemoveEntry(PAngelEntry Entry);
        void AdvanceEntry(PAngelEntry Entry);
        void Wander();
        void SeekTarget();
        void UpdateHeading();
        void AdvanceSteps(std::int32_t Count);
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        std::int32_t TimerInterval;
        SE_Space::PSpaceTimerSE MoveTimer;
        pas::WideString ImagePath;
        std::int32_t ImageCount;
        pas::Array<pas::WideString, 0, 7> ImagePaths;
        PAngelEntry FirstEntry;
        std::int32_t FrameIndex;
        Types::TPoint SizeRange;
        std::int32_t TurnTicks;
        EC_Struct::TPointF Velocity;
        EC_Struct::TPointF GroupSize;
        float MoveAngle;
        float Speed;
        std::int32_t EntryCount;
        EC_Struct::TPointF Target;
        std::int32_t TargetDelay;
        std::uint8_t MoveState;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TAngelEntry {
        PAngelEntry Next;
        PAngelEntry Prev;
        EC_Struct::TPointF Position;
        EC_Struct::TPointF Target;
        EC_Struct::TPointF Velocity;
        GI_GAI::TgaiGI* Animation;
        float Angle;
        std::int32_t FrameIndex;
        std::uint8_t MovingUp;
        std::uint8_t FrameVariant;
        std::uint8_t cpp_padding[2];
    };
    #pragma pack(pop)

} // namespace SE_Angel
