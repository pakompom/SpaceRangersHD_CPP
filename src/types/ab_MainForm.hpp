#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PolyLine.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/ab_Zone.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

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

namespace GI_StarField {
    struct TStarFieldGI;

} // namespace GI_StarField

namespace GI_Window {
    struct TWindowGI;

} // namespace GI_Window

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aItem {
    struct TWeapon;

} // namespace aItem

namespace aPath {
    struct TSPath;

} // namespace aPath

namespace ab_Item {
    struct TabItem;

} // namespace ab_Item

namespace ab_MainForm {
    struct TArcadeMapColorHeader;

    struct TArcadeMapColorSequence;

    struct TArcadeMapColorVariant;

} // namespace ab_MainForm

namespace ab_Space {
    struct TabSpace;

} // namespace ab_Space

namespace fLoad {
    struct TCacheLoader;

} // namespace fLoad

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace ab_MainForm {
    struct TArcadeMapDrag;

    struct TfAB;

    #pragma pack(push, 1)
    struct TArcadeMapDrag {
        std::uint8_t Active;
        WindowsSdk::TPoint Position;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfAB : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfAB, GI_MessageLoop::TMessageLoopGI, "TfAB", 856)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RequestExit(GI_MessageLoop::TObjectGI* Sender);
        void BattleKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey);
        void BattleKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey);
        void BattleMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        static void BattleMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void BattleRightMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void BattleRightMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void BattleMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ClearOverlaySegments();
        std::uint8_t ScreenPointToSphere(WindowsSdk::TPoint Point, double& Longitude, double& PolarAngle);
        void UpdateWeaponPanel();
        static void WeaponStateChanged(GI_MessageLoop::TObjectGI* Sender);
        void ClearWeaponPanel();
        void UpdateWeaponHighlights(std::uint8_t Force);
        void WeaponSelect(GI_MessageLoop::TObjectGI* Sender);
        void ToggleWeaponGroup(GI_MessageLoop::TObjectGI* Sender);
        void WeaponButtonClick(GI_MessageLoop::TObjectGI* Sender);
        static void NormalizeWeaponSelection();
        void UpdateAutopilotButtons();
        void ToggleAutopilot(GI_MessageLoop::TObjectGI* Sender);
        void TogglePause(GI_MessageLoop::TObjectGI* Sender);
        static void ReportSurvivingShips();
        void ClearBattle();
        void ClearMap();
        void LoadMap(EC_Buf::TBufEC* Buffer, std::uint8_t LoadPolygons);
        void LoadMapResource(pas::WideString Path, std::uint8_t LoadPolygons);
        void LoadMapFile(pas::WideString Path, std::uint8_t LoadPolygons);
        void ClearGrid();
        void BuildGrid();
        void ABSpaceBuild(std::int32_t GridSize, float Angle);
        void ResetBattleControls();
        void BeginMapTransition();
        void EnterMapView();
        void EnterCurrentSpace();
        static void AdvanceMapColors();
        void TimerTakt(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ScrollMapTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void FinishCampaignTransition();
        void InvalidateFrame();
        void DrawShipHealthBars();
        void ClearEnemyStatus(std::int32_t Index);
        void ClearTrackedShipStatus(std::int32_t Index);
        void UpdateShipStatusIcons();
        void DrawFrame() override;
        static void WorldImageCycleComplete(GI_MessageLoop::TObjectGI* Sender);
        void AppendShipPathArc(EC_Struct::TPointF Destination);
        void AppendShipPathLine(EC_Struct::TPointF Destination);
        void AppendShipPath(EC_Struct::TPointF Destination);
        static void BuildSpaceRoute(pas::List* Route, ab_Space::TabSpace* Origin, ab_Space::TabSpace* Destination);
        void RebuildShipPath();
        void BuildShipPathImages();
        void UpdateShipPathImages();
        void ClearShipPath();
        void BeginKellerDialogTransition();
        void BeginBattleExit();
        void ShowSpaceInfo(ab_Space::TabSpace* Space);
        void HideObjectInfo();
        void ShowItemInfo(ab_Item::TabItem* Item);
        void CancelCargoPickup();
        void OpenShipEquipment(GI_MessageLoop::TObjectGI* Sender);
        void SyncWeaponInventory();
        static void PickUpItem(ab_Item::TabItem* Item);
        std::int32_t RandomRange(std::int32_t BoundA, std::int32_t BoundB);
        double RandomFloat(double BoundA, double BoundB);
        void UpdateHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Show);
        void ControlMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void ControlMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void HideHelp();
        void ShowVictory();
        void CloseVictory(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey);
        void SelectMusic() override;
        GI_Panel::TPanelGI* MapPanel;
        GI_Panel::TPanelGI* WorldPanel;
        GI_StarField::TStarFieldGI* StarField;
        GI_GAI::TgaiGI* StartStarImage;
        GI_GAI::TgaiGI* EndStarImage;
        GI_Panel::TPanelGI* ItemPanel;
        GI_Window::TWindowGI* ItemInfoWindow;
        GI_GraphButton::TGraphButtonGI* AutoButton;
        GI_GraphButton::TGraphButtonGI* ManualButton;
        GI_Label::TLabelGI* BattleHelpLabel;
        GI_Panel::TPanelGI* VictoryPanel;
        GI_Panel::TPanelGI* DefeatPanel;
        SE_Space::TObjectSE* PlayerVisual;
        EC_Struct::TPointF PlayerMapPosition;
        aPath::TSPath* ShipPath;
        pas::List* RouteSpaces;
        TArcadeMapDrag MapDrag;
        std::uint8_t cpp_padding[3];
        GI_MessageLoop::PCallbackTimerGI ScrollTimer;
        pas::WideString MapBackgroundPath;
        pas::Array<GI_MessageLoop::TObjectGI*, 0, 4> WeaponButtons;
        pas::Array<GI_Image::TImageGI*, 0, 4> WeaponIcons;
        pas::Array<GI_Image::TImageGI*, 0, 4> WeaponChargeImages;
        pas::Array<GI_Image::TImageGI*, 0, 4> WeaponPrimaryImages;
        pas::Array<GI_Image::TImageGI*, 0, 4> WeaponSecondaryImages;
        std::uint8_t cpp_padding_2[4];
        GI_GraphButton::TGraphButtonGI* PlayButton;
        GI_GraphButton::TGraphButtonGI* PauseButton;
        GI_PolyLine::TPolyLineGI* WorldLines;
        GI_MessageLoop::PCallbackTimerGI UpdateTimer;
        std::int32_t WorldCenterX;
        std::int32_t WorldCenterY;
        pas::Array<GI_Image::TImageGI*, 0, 7> BonusIcons;
        pas::Array<GI_GAI::TgaiGI*, 0, 7> BonusRings;
        pas::Array<GI_MessageLoop::TObjectGI*, 0, 7> EnemyIcons;
        pas::Array<GI_GAI::TgaiGI*, 0, 7> EnemyHealthRings;
        pas::Array<GI_MessageLoop::TObjectGI*, 0, 7> EnemyRewardIcons;
        pas::Array<GI_MessageLoop::TObjectGI*, 0, 7> EnemyRewardBackdrops;
        pas::Array<GI_MessageLoop::TObjectGI*, 0, 7> TrackedShipIcons;
        pas::Array<GI_GAI::TgaiGI*, 0, 7> TrackedShipHealthRings;
        pas::Array<aItem::TWeapon*, 0, 4> CampaignWeapons;
        std::uint8_t ForwardKeyDown;
        std::uint8_t ReverseKeyDown;
        std::uint8_t BrakeKeyDown;
        std::uint8_t TurnLeftKeyDown;
        std::uint8_t TurnRightKeyDown;
        std::uint8_t PrimaryFireKeyDown;
        std::uint8_t SecondaryFireKeyDown;
        std::uint8_t cpp_padding_3[1];
        pas::List* GridLines;
        std::int32_t MapState2C8;
        pas::WideString Text2CC;
        std::uint8_t cpp_padding_4[16];
        pas::Array<GI_PolyLine::PPolyLineSegmentGI, 0, 3> OverlaySegments;
        std::uint8_t cpp_padding_5[32];
        double TransitionSpeed;
        std::uint8_t CampaignTransitionStarted;
        std::uint8_t CampaignLoadStarted;
        std::uint8_t CampaignLoadFinished;
        std::uint8_t cpp_padding_6[1];
        fLoad::TCacheLoader* CacheLoader;
        std::int32_t DefeatCountdownTicks;
        float CampaignLoadProgress;
        std::int32_t DepartureTurn;
        std::int32_t ArrivalTurn;
        pas::Object* InfoObject;
        ab_Item::TabItem* CargoPickupItem;
        ab_Zone::PabZone CargoPickupZone;
        std::uint32_t InitialRandomSeed;
        std::uint32_t RandomSeed;
        std::uint8_t ViewModeBeforeDefeat;
        std::uint8_t SimulationPaused;
        std::uint8_t cpp_padding_7[2];
        GI_MessageLoop::PCallbackTimerGI VictoryTimer;
        pas::List* ListedObjects;
        fPanelLoad::TfPanelLoad* LoadPanel;
        pas::WideString SelectedMapName;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TArcadeMapColorHeader {
        std::int32_t CurrentColor;
        std::int32_t VariantCount;
        std::int32_t SelectedVariant;
        std::int32_t ByteSize;
    };
    #pragma pack(pop)

    using PArcadeMapColorHeader = TArcadeMapColorHeader*;

    #pragma pack(push, 1)
    struct TArcadeMapColorVariant {
        std::int32_t SequenceOffset;
        std::int32_t AppearanceTag;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TArcadeMapColorSequence {
        std::int32_t FrameIndex;
        std::int32_t FrameCount;
    };
    #pragma pack(pop)

    using PArcadeMapColorSequence = TArcadeMapColorSequence*;

    using PArcadeMapColorVariant = TArcadeMapColorVariant*;

} // namespace ab_MainForm
