#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_Door {
    struct TDoorGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TDoorGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TDoorGI, GI_MessageLoop::TObjectGI, "TDoorGI", 312)
        void p_destroy() override;
        void Clear() override;
        void SetStepTime(std::int32_t Value);
        void SetSize(Types::TPoint Size) override;
        void StartStepTimer();
        void StopStepTimer();
        void StepFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void OnActivate() override;
        void OnDeactivate() override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadDoorProperties(EC_BlockPar::TBlockParEC* Block);
        GI_GAI::TgaiGI* Image;
        std::int32_t FrameStep;
        GI_MessageLoop::PCallbackTimerGI StepTimer;
        std::int32_t StepTime;
        GI_MessageLoop::TObjectNotifyEventGI ClickCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Door
