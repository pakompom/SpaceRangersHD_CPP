#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GraphBuf {
    struct TGraphBufGI;

} // namespace GI_GraphBuf

namespace GI_Window {
    struct TWindowGI;

} // namespace GI_Window

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fPanelPlanet {
    struct TfPanelPlanet;

} // namespace fPanelPlanet

namespace fPanelRuins {
    struct TfPanelRuins;

} // namespace fPanelRuins

namespace fHangar {
    struct THangarShipSlot;

    struct TfHangar;

    #pragma pack(push, 1)
    struct THangarShipSlot {
        std::int32_t AnimationState;
        std::int32_t ShipId;
        GR_GraphBuf::TGraphBufGR* ImageBuffer;
        std::int32_t Opacity;
        GI_GraphBuf::TGraphBufGI* ImageControl;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfHangar : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfHangar, fPanelMain::TMessageLoopGIWithMainPanel, "TfHangar", 432)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void CaptureDispatcherMirror(GI_MessageLoop::TObjectGI* Sender);
        void DispatcherAnimationComplete(GI_MessageLoop::TObjectGI* Sender);
        static std::uint8_t TryTakeOff();
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void BeginTakeOff();
        void MainKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void SelectMusic() override;
        std::uint8_t IsServiceButtonDown();
        void RefreshServiceButtons();
        void RepairHullClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefuelClicked(GI_MessageLoop::TObjectGI* Sender);
        void TakeOffClicked(GI_MessageLoop::TObjectGI* Sender);
        static void StopAnimation(GI_MessageLoop::TObjectGI* Sender);
        std::uint8_t RefreshTakeOffStatus();
        void AmbientAnimationComplete(GI_MessageLoop::TObjectGI* Sender);
        void StartAmbientAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        static float GetShipPortraitScale(aShip::TShip* Ship);
        void LoadDockedShipImage(std::int32_t Index, pas::WideString ImagePath, std::uint8_t LargeHull, float Scale);
        void SetDockedShipOpacity(std::int32_t Index, std::uint8_t Alpha);
        void AnimateDockedShips(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void RefreshDockedShips();
        void MainMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void HideShipInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ShowShipInfo(aShip::TShip* Ship);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        fPanelPlanet::TfPanelPlanet* PlanetPanel;
        fPanelRuins::TfPanelRuins* StationPanel;
        fPanelLoad::TfPanelLoad* LoadPanel;
        GI_Window::TWindowGI* ShipInfoWindow;
        aShip::TShip* HoveredShip;
        GI_MessageLoop::PCallbackTimerGI ShipInfoHideTimer;
        std::uint8_t TakeOffPending;
        std::uint8_t cpp_padding[3];
        GI_MessageLoop::PCallbackTimerGI AmbientAnimationTimer;
        GI_MessageLoop::PCallbackTimerGI DockedShipsTimer;
        pas::Array<THangarShipSlot, 0, 8> ShipSlots;
        aShip::TShip* SelectedShip;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fHangar
