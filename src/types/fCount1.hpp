#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace fCount1 {
    struct TfCount1;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfCount1 : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfCount1, GI_MessageLoop::TMessageLoopGI, "TfCount1", 252)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RefreshValue();
        void AddPressed(GI_MessageLoop::TObjectGI* Sender);
        void SubPressed(GI_MessageLoop::TObjectGI* Sender);
        void AddReleased(GI_MessageLoop::TObjectGI* Sender);
        void SubReleased(GI_MessageLoop::TObjectGI* Sender);
        void RepeatChange(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t Data);
        void MaxClicked(GI_MessageLoop::TObjectGI* Sender);
        void AcceptClicked(GI_MessageLoop::TObjectGI* Sender);
        void CancelClicked(GI_MessageLoop::TObjectGI* Sender);
        void BarMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void ProcessCallbackTimers() override;
        void MainKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        pas::WideString ImagePath;
        pas::WideString KindImagePath;
        pas::WideString Caption;
        std::int32_t Minimum;
        std::int32_t Maximum;
        std::int32_t Limit;
        std::int32_t Value;
        pas::List* Items;
        std::uint8_t Dragging;
        std::uint8_t cpp_padding[3];
        GI_MessageLoop::PCallbackTimerGI RepeatTimer;
        std::uint32_t RepeatCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fCount1
