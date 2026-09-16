#pragma once
#include "runtime_support.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fPanelMain.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PanelScrollBar {
    struct TPanelScrollBarGI;

} // namespace GI_PanelScrollBar

namespace GI_Window {
    struct TWindowGI;

} // namespace GI_Window

namespace aRanger {
    struct TRanger;

} // namespace aRanger

namespace fRating2 {
    struct TRangerRatingRow;

    struct TfRating2;

    #pragma pack(push, 1)
    struct TRangerRatingRow {
        aRanger::TRanger* Ranger;
        std::int32_t Top;
        std::int32_t Height;
    };
    #pragma pack(pop)

    enum TRangerRatingSortColumn : std::uint32_t {
        rrscName = 1,
        rrscExperience = 2,
        rrscRace = 3,
        rrscRank = 4,
        rrscCharacter = 5,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfRating2 : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfRating2, fPanelMain::TMessageLoopGIWithMainPanel, "TfRating2", 264)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void BackgroundMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void BackgroundMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void AwardsMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void AwardsMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void HintMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ShowPartnershipHint(GI_MessageLoop::TObjectGI* Sender);
        void ShowCareerHint(GI_MessageLoop::TObjectGI* Sender);
        void ShowAwardHint(aRanger::TRanger* Ranger, std::int32_t AwardId);
        void HideHint();
        std::int32_t FindRowByRangerId(std::int32_t RangerId);
        void SortHeaderMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void FeaturedRangerClicked(GI_MessageLoop::TObjectGI* Sender);
        void RowMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SelectRow(std::int32_t Index);
        void ClearRows();
        void RebuildTable();
        void RebuildRow(std::int32_t Index);
        void CreateRow(std::int32_t Index);
        void RowMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void RowMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void RefreshFeaturedRangers();
        void SelectMusic() override;
        void ProcessCallbackTimers() override;
        void ShowDominatorKillsHint(GI_MessageLoop::TObjectGI* Sender);
        GI_Window::TWindowGI* RewardWindow;
        pas::DynArray<TRangerRatingRow> Rows;
        std::int32_t SelectedIndex;
        std::int32_t SelectedRangerId;
        TRangerRatingSortColumn SortColumn;
        std::uint8_t SortAscending;
        std::uint8_t cpp_padding[3];
        GI_PanelScrollBar::TPanelScrollBarGI* TablePanel;
        WindowsSdk::TRect SelectedRowRect;
        std::uint8_t BackgroundClickStarted;
        std::uint8_t cpp_padding_2[3];
        std::int32_t HoveredAwardId;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fRating2
