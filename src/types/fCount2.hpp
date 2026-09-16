#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace fCount2 {
    struct TfCount2;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfCount2 : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfCount2, GI_MessageLoop::TMessageLoopGI, "TfCount2", 260)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RefreshValue();
        void IncreaseMouseDown(GI_MessageLoop::TObjectGI* Sender);
        void DecreaseMouseDown(GI_MessageLoop::TObjectGI* Sender);
        void IncreaseMouseUp(GI_MessageLoop::TObjectGI* Sender);
        void DecreaseMouseUp(GI_MessageLoop::TObjectGI* Sender);
        void RepeatChange(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void MaximumClicked(GI_MessageLoop::TObjectGI* Sender);
        void AcceptClicked(GI_MessageLoop::TObjectGI* Sender);
        void CancelClicked(GI_MessageLoop::TObjectGI* Sender);
        void SliderMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SliderMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SliderMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void ProcessCallbackTimers() override;
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        pas::WideString ImagePath;
        pas::WideString PreviewImagePath;
        pas::WideString Description;
        std::int32_t Minimum;
        std::int32_t Maximum;
        std::int32_t Limit;
        std::int32_t Value;
        float UnitValue;
        std::int32_t Available;
        std::int32_t TotalLimit;
        std::uint8_t Dragging;
        std::uint8_t cpp_padding[3];
        GI_MessageLoop::PCallbackTimerGI RepeatTimer;
        pas::WideString FontName;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fCount2
