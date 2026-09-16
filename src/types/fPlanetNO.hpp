#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Window {
    struct TWindowGI;

} // namespace GI_Window

namespace GR_Sound {
    struct TSoundBufferControl;

} // namespace GR_Sound

namespace aItem {
    struct TGoods;

    struct TItem;

    struct TSatellite;

} // namespace aItem

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fPlanetNO {
    struct TProbeMarkerPixel;

    struct TProbeTrajectoryPoint;

    struct TfPlanetNO;

    #pragma pack(push, 1)
    struct TProbeTrajectoryPoint {
        EC_Struct::TPointF Position;
        EC_Struct::TPointF Direction;
        std::uint8_t cpp_padding[4];
    };
    #pragma pack(pop)

    using TProbeTrajectory = pas::Array<TProbeTrajectoryPoint, 0, 255>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfPlanetNO : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfPlanetNO, fPanelMain::TMessageLoopGIWithMainPanel, "TfPlanetNO", 31064)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void TakeoffClicked(GI_MessageLoop::TObjectGI* Sender);
        void StartTextQuest(GI_MessageLoop::TObjectGI* Sender);
        void RefreshPlanetInfo();
        void RefreshTextQuestPrompt();
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void GalaxyClicked(GI_MessageLoop::TObjectGI* Sender);
        void QuestClicked(GI_MessageLoop::TObjectGI* Sender);
        void ToggleResearchPanel(GI_MessageLoop::TObjectGI* Sender);
        void OpenResearchPanel();
        void CloseResearchPanel();
        void BuildTrajectory(std::int32_t TrajectoryIndex);
        WindowsSdk::TPoint GetRandomTrajectoryPoint(std::int32_t TrajectoryIndex);
        WindowsSdk::TPoint ProjectPointOntoTrajectory(std::int32_t TrajectoryIndex, WindowsSdk::TPoint Point);
        WindowsSdk::TPoint AdvanceTrajectoryPoint(std::int32_t TrajectoryIndex, WindowsSdk::TPoint Point);
        std::int32_t FindTrajectoryAtCursor();
        std::uint8_t IsCursorOverTrajectory(std::int32_t TrajectoryIndex);
        void ResearchMapMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ResearchMapMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void RefreshResearchPanel();
        void ScrollSatellitePageLeft(GI_MessageLoop::TObjectGI* Sender);
        void ScrollSatellitePageRight(GI_MessageLoop::TObjectGI* Sender);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void MainPanelMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SatelliteInventoryMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ResearchMapMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainPanelRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void UpdateActionCursor(std::uint8_t ForceHand) override;
        void ReturnHeldSatellite();
        static aItem::TSatellite* FindDeployedSatellite(std::int32_t TrajectoryIndex);
        static std::int32_t CountDeployedSatellites();
        void AdvanceSatelliteMarkers(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void UpdateProbeSignalSound(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void UpdateItemInfoPopup(aItem::TItem* Item);
        void ShowGoodsInfoPopup(aItem::TGoods* Item);
        void HideItemInfoPopup(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        void SelectMusic() override;
        fPanelLoad::TfPanelLoad* LoadPanel;
        pas::Array<std::int32_t, 0, 5> TrajectoryPointCounts;
        pas::Array<TProbeTrajectory, 0, 5> Trajectories;
        std::int32_t SelectedTrajectoryIndex;
        std::int32_t SatelliteInventoryPageStart;
        pas::Array<GI_Image::TImageGI*, 0, 5> SatelliteInventorySlots;
        std::uint8_t ResearchPanelVisible;
        std::uint8_t cpp_padding[3];
        GI_Window::TWindowGI* ItemInfoWindow;
        GI_Image::TImageGI* ItemInfoImage;
        GI_Label::TLabelGI* ItemInfoNameLabel;
        GI_Label::TLabelGI* ItemInfoTextLabel;
        GI_Label::TLabelGI* ItemInfoSizeLabel;
        GI_Label::TLabelGI* ItemInfoCostLabel;
        GI_Image::TImageGI* ItemInfoRaceIcon;
        GI_MessageLoop::PCallbackTimerGI ItemInfoHideTimer;
        aItem::TItem* HoveredItem;
        std::uint8_t SatellitePanelNeedsLayout;
        std::uint8_t cpp_padding_2[3];
        GI_MessageLoop::PCallbackTimerGI SatelliteMovementTimer;
        GI_MessageLoop::PCallbackTimerGI ProbeSignalTimer;
        GR_Sound::TSoundBufferControl* ProbeSignalSound;
        std::int32_t ProbeSignalCount;
        aItem::TSatellite* HeldSatellite;
        std::int32_t HeldSatelliteOrigin;
        std::uint8_t HoveringSurfaceLoot;
        std::uint8_t NewSurfaceLootDiscovered;
        std::uint8_t cpp_padding_3[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TProbeMarkerPixel {
        std::int32_t X;
        std::int32_t Y;
        System::PCardinal Pixel;
        std::uint8_t* Visited;
    };
    #pragma pack(pop)

    using PProbeMarkerPixel = TProbeMarkerPixel*;

} // namespace fPlanetNO
