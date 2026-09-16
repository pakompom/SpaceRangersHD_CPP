#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_PanelScrollBar {
    struct TPanelScrollBarGI;

} // namespace GI_PanelScrollBar

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fRewards {
    struct TfRewards;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfRewards : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfRewards, GI_MessageLoop::TMessageLoopGI, "TfRewards", 228)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        std::uint8_t CanEditAwards();
        static pas::WideString GetAwardImagePath(std::int32_t AwardId);
        void PlatformMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void AwardMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void AwardMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ClearHighlight(GI_MessageLoop::TObjectGI* Sender);
        void AwardMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void IncreaseVisibleCount(GI_MessageLoop::TObjectGI* Sender);
        void DecreaseVisibleCount(GI_MessageLoop::TObjectGI* Sender);
        void RefreshVisibleCount();
        void BuildAwardControls();
        void UpdateActionCursor(std::uint8_t CanTake) override;
        void ProcessCallbackTimers() override;
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void SelectMusic() override;
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        GI_PanelScrollBar::TPanelScrollBarGI* AwardsPanel;
        aShip::TShip* Ship;
        GI_MessageLoop::TObjectGI* DraggedAward;
        std::int32_t HoveredAwardId;
        std::uint8_t ReadOnly;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fRewards
