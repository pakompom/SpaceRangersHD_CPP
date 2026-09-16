#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PanelScrollBar {
    struct TPanelScrollBarGI;

} // namespace GI_PanelScrollBar

namespace aGalaxy {
    struct TStar;

} // namespace aGalaxy

namespace aItem {
    struct TItem;

} // namespace aItem

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fPanelPlanet {
    struct TfPanelPlanet;

} // namespace fPanelPlanet

namespace fPanelRuins {
    struct TfPanelRuins;

} // namespace fPanelRuins

namespace fInfo {
    struct TfInfo;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfInfo : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfInfo, fPanelMain::TMessageLoopGIWithMainPanel, "TfInfo", 248)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RefreshNewsAnimation(GI_MessageLoop::TObjectGI* Sender);
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void ClearInfoContents();
        void FinishInfoLayout();
        void AddInfoSpacing(std::int32_t Pixels);
        void AddInfoSeparator();
        // LayoutKind zero centers the heading; nonzero aligns it to the right.
        void AddInfoHeading(pas::WideString Title, pas::WideString BookmarkText, std::int32_t LayoutKind, std::int32_t BookmarkIndex, std::int32_t GoodsReference);
        void AddSearchPriceLabel(pas::WideString Text);
        void AddInfoText(pas::WideString Text, GI_Main::TTextAlignXGI Alignment, pas::WideString Font);
        void AddInfoImageText(pas::WideString ImagePath, pas::WideString Text);
        void AddPlanetInfoText(aPlanet::TPlanet* Planet, pas::WideString Text);
        void AddItemInfoText(aItem::TItem* Item, pas::WideString Text);
        void AddStarInfoText(aGalaxy::TStar* Star, pas::WideString Text);
        void FocusSearchField(std::uint8_t Force);
        void ToggleSearchMode(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        static void MainPanelKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void BookmarkClicked(GI_MessageLoop::TObjectGI* Sender);
        void SelectMusic() override;
        void ShowNews();
        void ShowSearch();
        void SearchClicked(GI_MessageLoop::TObjectGI* Sender);
        void RunSearch(GI_MessageLoop::TObjectGI* Sender);
        void AddEquipmentInfoText(aItem::TItem* Item, pas::WideString Text);
        // The explicit receiver value preserves native loading before the False argument.
        void ToggleVisibleBookmark();
        static std::uint8_t IsAtBusinessCenter();
        void CategoryStateChanged(GI_MessageLoop::TObjectGI* Sender);
        void CategoryClicked(GI_MessageLoop::TObjectGI* Sender);
        void NextSearchPageClicked(GI_MessageLoop::TObjectGI* Sender);
        void BindFilterLabels(GI_MessageLoop::TObjectGI* Parent);
        static void FilterLabelMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void FilterLabelMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        static void FilterLabelMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        static void FilterLabelMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ClearSearchText(GI_MessageLoop::TObjectGI* Sender);
        void CopySearchText(GI_MessageLoop::TObjectGI* Sender);
        void PasteSearchText(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch12Name(GI_MessageLoop::TObjectGI* Sender);
        void CopySearch12Name(GI_MessageLoop::TObjectGI* Sender);
        void PasteSearch12Name(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch15Name(GI_MessageLoop::TObjectGI* Sender);
        void CopySearch15Name(GI_MessageLoop::TObjectGI* Sender);
        void PasteSearch15Name(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearchField(pas::WideString Name);
        void ClearSearch01Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch02Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch03Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch04Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch05Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch06Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch07Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch09Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch10Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch11Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch12Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch13Filters(GI_MessageLoop::TObjectGI* Sender);
        void ClearSearch15Filters(GI_MessageLoop::TObjectGI* Sender);
        static std::int32_t GetSearchResultLimit();
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        fPanelPlanet::TfPanelPlanet* PlanetPanel;
        fPanelRuins::TfPanelRuins* StationPanel;
        fPanelLoad::TfPanelLoad* LoadPanel;
        GI_PanelScrollBar::TPanelScrollBarGI* InfoPanel;
        std::int32_t InfoContentHeight;
        std::uint8_t SearchMode;
        std::uint8_t cpp_padding[3];
        // Zero selects the general name search.
        std::int32_t SelectedSearchCategory;
        std::int32_t PreviousSearchCategory;
        std::uint8_t HasSearchResults;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fInfo
