#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_GraphBuf {
    struct TGraphBufGI;

} // namespace GI_GraphBuf

namespace GI_GraphButton {
    struct TGraphButtonGI;

} // namespace GI_GraphButton

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace GI_PolyLine {
    struct TPolyLineGI;

} // namespace GI_PolyLine

namespace aGalaxy {
    struct TStar;

} // namespace aGalaxy

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fGalaxy2 {
    struct TfGalaxy2;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfGalaxy2 : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfGalaxy2, fPanelMain::TMessageLoopGIWithMainPanel, "TfGalaxy2", 328)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void MainPanelMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        std::uint8_t CanShowExtendedRadarInfo(aGalaxy::TStar* Star);
        static std::int32_t GetRadarSummaryRadius();
        EC_Struct::TPointF MapPointToGalaxyPoint(WindowsSdk::TPoint Point);
        WindowsSdk::TPoint GalaxyPointToMapPoint(EC_Struct::TPointF Point);
        std::int32_t GalaxyDistanceToMapDistance(double Distance);
        void RebuildJumpPath();
        void ClearJumpPath();
        void ConfigureReadOnlyMap();
        void ClearReadOnlyMapCallbacks();
        void ConfigureJumpSelection();
        void ClearJumpSelectionCallbacks();
        static void MapMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MapLeftButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MapRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MapButtonUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MapDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void HideStarInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void JumpClicked(GI_MessageLoop::TObjectGI* Sender);
        void JumpMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void JumpMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ShowStarInfo(aGalaxy::TStar* Star);
        static std::uint8_t CanRevealBossPresence(aShip::TShip* Ship);
        pas::WideString BuildStarShipSummary(aGalaxy::TStar* Star, std::int32_t& LineCount);
        void UpdateJumpAnimations(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SelectMusic() override;
        void ProcessCallbackTimers() override;
        void CreateMarkerClicked(GI_MessageLoop::TObjectGI* Sender);
        void UndoMarkerClicked(GI_MessageLoop::TObjectGI* Sender);
        void ClearMarkersClicked(GI_MessageLoop::TObjectGI* Sender);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        GI_Panel::TPanelGI* MapPanel;
        std::uint8_t ViewMode;
        std::uint8_t cpp_padding[3];
        GI_GraphBuf::TGraphBufGI* HideBuffer;
        WindowsSdk::TRect MapPixelBounds;
        EC_Struct::TPointF GalaxyOrigin;
        EC_Struct::TPointF GalaxyExtent;
        aGalaxy::TStar* SelectedJumpStar;
        pas::List* RouteStars;
        GI_PolyLine::TPolyLineGI* StarLinks;
        GI_MessageLoop::PCallbackTimerGI StarInfoHideTimer;
        GI_MessageLoop::PCallbackTimerGI JumpAnimationTimer;
        std::int32_t JumpHintAnimationState;
        std::int32_t JumpLightTick;
        GI_GraphButton::TGraphButtonGI* JumpButton;
        GI_Label::TLabelGI* JumpDestinationLabel;
        GI_GAI::TgaiGI* JumpAnimation;
        pas::Array<GI_Image::TImageGI*, 0, 2> JumpLightImages;
        std::uint8_t CapturePreviewOnOpen;
        std::uint8_t cpp_padding_2[3];
        GI_GraphButton::TGraphButtonGI* CreateMarkerButton;
        pas::WideString CreateMarkerImagePath;
        pas::WideString CreateMarkerActiveImagePath;
        std::uint8_t CreateMarkerMode;
        std::uint8_t cpp_padding_3[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fGalaxy2
