#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aEFilm.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Circle {
    struct TCircleGI;

} // namespace GI_Circle

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

namespace GI_SpaceCircle {
    struct TSpaceCircleGI;

} // namespace GI_SpaceCircle

namespace GI_SpaceImg {
    struct TSpaceImgGI;

} // namespace GI_SpaceImg

namespace GI_StarField {
    struct TStarFieldGI;

} // namespace GI_StarField

namespace GI_Window {
    struct TWindowGI;

} // namespace GI_Window

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aAsteroid {
    struct TAsteroid;

} // namespace aAsteroid

namespace aGalaxy {
    struct THole;

} // namespace aGalaxy

namespace aItem {
    struct TItem;

} // namespace aItem

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fStarMap {
    struct TStarMapReservedEntry;

    struct TfStarMap;

    enum TStarMapMode : std::uint8_t {
        smmInactive = 0,
        smmOrders = 1,
        smmTurnFilm = 2,
    };

    enum TStarMapResumeMode : std::uint8_t {
        smrNormal = 0,
        smrOrders = 1,
        smrTurnFilm = 2,
        smrWaitForTurn = 3,
    };

    enum TStarMapPathKind : std::uint32_t {
        smpNone = 0,
        smpAsteroid = 1,
        smpShip = 2,
    };

    #pragma pack(push, 1)
    struct TStarMapReservedEntry {
        pas::Array<std::uint8_t, 0, 7> Data;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfStarMap : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfStarMap, fPanelMain::TMessageLoopGIWithMainPanel, "TfStarMap", 668)
        void p_destroy() override;
        WindowsSdk::TPoint GetMapCenter();
        void SetMapCenterManually(WindowsSdk::TPoint Point);
        void SetMapCenter(WindowsSdk::TPoint Center);
        void CenterMapForTalk(EC_Struct::TPointF Position);
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RestorePendingSceneObjects();
        static void BuildSpaceBackground(GI_StarField::TStarFieldGI* StarField, GI_SpaceImg::TSpaceImgGI* SpaceImage, std::uint32_t Seed, std::int32_t BackgroundIndex);
        static void SaveSpaceImageState(GI_SpaceImg::TSpaceImgGI* SpaceImage);
        void SaveSpaceBackground();
        void DeferredEndTurn(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void MapKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void MapKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void OpenFilmHistoryClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshScoreModsLabel();
        void BuildShipPathOverlay(aShip::TShip* Ship, std::uint8_t DelayEndImage, pas::WideString InitialImagePath);
        void UpdatePathEndImage(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ClearPathOverlay(std::uint8_t PlayerPath);
        void ShowAsteroidPath(aAsteroid::TAsteroid* Asteroid);
        void ClearAsteroidPath();
        void ClearPartnerButtons();
        void RebuildPartnerButtons();
        void PartnerClicked(GI_MessageLoop::TObjectGI* Sender);
        void PartnerRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void PartnerMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void PartnerMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void AddMapAnimation(EC_Struct::TPointF Position, pas::WideString ImagePath, std::int32_t DelayMs);
        void ClearMapAnimations();
        static void MapAnimationFinished(GI_MessageLoop::TObjectGI* Sender);
        pas::Object* FindObjectAtCursor();
        void QueueInterfaceImages();
        void ShowLargeHelp(const pas::WideString& Text);
        void HideLargeHelp();
        void AnimateLargeHelp(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void RedrawMap();
        void DrawFrame() override;
        void RunTalkDialogs();
        void GalaxyClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void StartOrderMode();
        void StopOrderMode();
        void HideOrderInterface();
        void ConfigureMiddleButtonAction();
        void ScrollMap(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        static void AdvanceSpaceEffects(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        std::uint8_t IsMapPointBlocked(GI_MessageLoop::TObjectGI* Sender, WindowsSdk::TPoint Point);
        void MapLeftButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MapMiddleButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MapRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MapMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void OrderKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void OrderKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void SelectAllUsableWeapons();
        void SelectUntargetedWeapons();
        void ShowObjectInfo(pas::Object* Obj);
        void MapScrollChanged();
        static pas::WideString GetPriceSnapshotKey(pas::Object* Obj);
        void SaveVisiblePriceSnapshots();
        void CenterOnShip(aShip::TShip* Ship);
        void CenterOnDominator(std::int32_t Selection);
        void CenterShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void CenterShipMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void CenterShipMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void AllWeaponsClicked(GI_MessageLoop::TObjectGI* Sender);
        void ScannerClicked(GI_MessageLoop::TObjectGI* Sender);
        void TalkClicked(GI_MessageLoop::TObjectGI* Sender);
        void SelectInterceptorTarget();
        void BeginCustomSelection();
        void ToggleWeaponPanelClicked(GI_MessageLoop::TObjectGI* Sender);
        void WeaponButtonDown(GI_MessageLoop::TObjectGI* Sender);
        void WeaponButtonUp(GI_MessageLoop::TObjectGI* Sender);
        void RefreshWeaponButtons();
        void RefreshActionRanges();
        void HideActionRanges();
        void RebuildTargetMarkers();
        void ClearTargetMarkers();
        void UpdateActionCursor(std::uint8_t CanTake) override;
        void UpdateWeaponPanelPosition();
        void AnimateWeaponPanel(std::int32_t Target);
        void AdvanceWeaponPanel(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void UpdateSpacePanelPosition();
        void AnimateSpacePanel(std::int32_t Target);
        void AdvanceSpacePanel(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void StartTurnFilm();
        void StopTurnFilm(std::uint8_t StopTurnProcessing);
        void RestartTurnFilm();
        void ProcessTurnFilm();
        void AdvanceFilmFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void UpdateTurnCalculation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void BreakTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void UpdateFilmCamera();
        void CenterFilmShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void CenterFilmShipMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void CenterFilmShipMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void FilmMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        SE_Space::TObjectSE* FindFilmObjectAtCursor(std::uint32_t& ObjectId);
        void ShowFilmObjectInfo(SE_Space::TObjectSE* Obj, std::uint32_t ObjectId);
        void PrepareTalkDisplay();
        void WaitForTurnOrTalk();
        void UpdateTerronTransformation();
        void TerronTransformationStarted(GI_MessageLoop::TObjectGI* Sender);
        void TerronTransformationFrame(GI_MessageLoop::TObjectGI* Sender);
        static void TerronTransformationFinished(GI_MessageLoop::TObjectGI* Sender);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        void SelectMusic() override;
        fPanelLoad::TfPanelLoad* LoadPanel;
        TStarMapMode Mode;
        TStarMapResumeMode ResumeMode;
        std::uint8_t ScrollLeftHeld;
        std::uint8_t ScrollRightHeld;
        std::uint8_t ScrollUpHeld;
        std::uint8_t ScrollDownHeld;
        std::uint8_t cpp_padding[2];
        GI_GraphButton::TGraphButtonGI* CenterShipButton;
        std::uint8_t cpp_padding_2[4];
        GI_MessageLoop::TObjectGI* TerronFadeImage;
        GI_GraphBuf::TGraphBufGI* LargeHelpBuffer;
        pas::WideString LargeHelpText;
        GI_MessageLoop::PCallbackTimerGI LargeHelpTimer;
        float LargeHelpProgress;
        aGalaxy::THole* PendingHoleRefresh;
        std::int32_t PlanetBattleMapId;
        std::int32_t PlanetBattleState;
        pas::Object* DisplayedObject;
        std::uint8_t SuppressMiddleFollowCycle;
        std::uint8_t cpp_padding_3[3];
        GI_MessageLoop::PCallbackTimerGI SpaceEffectsTimer;
        pas::Object* CursorObject;
        GI_MessageLoop::PCallbackTimerGI MapScrollTimer;
        GI_MessageLoop::PCallbackTimerGI PlayerPathTimer;
        std::uint8_t ScannerSelectionActive;
        std::uint8_t TalkSelectionActive;
        pas::Array<std::uint8_t, 0, 4> SelectedWeapons;
        std::uint8_t InterceptorSelectionActive;
        std::uint8_t CustomSelectionActive;
        std::uint8_t cpp_padding_4[3];
        aItem::TItem* CustomSelectionItem;
        pas::WideString CustomSelectionInfoName;
        std::int32_t CustomSelectionRadius;
        std::uint32_t CustomSelectionColor;
        pas::WideString CustomSelectionAllowedCursor;
        pas::WideString CustomSelectionDeniedCursor;
        pas::WideString CustomSelectionSuccessText;
        pas::WideString CustomSelectionOutOfRangeText;
        pas::WideString CustomSelectionFailureText;
        std::uint8_t cpp_padding_5[4];
        TStarMapPathKind MinimapPathKind;
        GI_Panel::TPanelGI* WeaponPanel;
        std::int32_t WeaponPanelRestTop;
        float WeaponPanelProgress;
        std::int32_t WeaponPanelTarget;
        GI_MessageLoop::PCallbackTimerGI WeaponPanelTimer;
        GI_Panel::TPanelGI* SpacePanel;
        std::int32_t SpacePanelRestTop;
        float SpacePanelProgress;
        std::int32_t SpacePanelTarget;
        GI_MessageLoop::PCallbackTimerGI SpacePanelTimer;
        std::uint8_t AnimateSpacePanelOnResume;
        std::uint8_t cpp_padding_6[3];
        GI_MessageLoop::PCallbackTimerGI DeferredEndTurnTimer;
        aAsteroid::TAsteroid* PathAsteroid;
        GI_MessageLoop::PCallbackTimerGI FilmFrameTimer;
        std::int32_t FilmStepIndex;
        aEFilm::PEFilmCommand NextFilmCommand;
        GI_MessageLoop::PCallbackTimerGI FilmProgressTimer;
        std::uint8_t ContinueTurnCalculation;
        std::uint8_t BreakRequested;
        std::uint8_t BreakOnNextFilm;
        std::uint8_t Flag19F;
        std::int32_t ReservedFilmState1A0;
        float FilmFrameIntervalMs;
        float FilmFrameIntervalDelta;
        EC_Struct::TPointF FilmCameraPosition;
        EC_Struct::TPointF FilmCameraTarget;
        std::int32_t FilmCameraTargetUntilStep;
        std::int32_t FilmCameraEventIndex;
        std::int32_t FilmCameraTargetKind;
        float FilmCameraShakeAngle;
        EC_Struct::TPointF FilmCameraShakeOffset;
        float FilmCameraSpeed;
        std::uint8_t FilmCameraMoving;
        std::uint8_t cpp_padding_7[3];
        pas::DynArray<TStarMapReservedEntry> ReservedEntries1;
        pas::DynArray<TStarMapReservedEntry> ReservedEntries2;
        std::int32_t ReservedFilmState1E4;
        std::uint8_t cpp_padding_8[4];
        std::int32_t TrailingEffectSteps;
        SE_Space::TObjectSE* DisplayedFilmObject;
        GI_Panel::TPanelGI* MapControls;
        GI_Panel::TPanelGI* PartnerPanel;
        GI_Panel::TPanelGI* SecondaryPartnerPanel;
        GI_Window::TWindowGI* InfoWindow;
        GI_Label::TLabelGI* InfoTextLabel;
        GI_Window::TWindowGI* ItemInfoWindow;
        GI_Panel::TPanelGI* ShipInfoPanel;
        GI_Panel::TPanelGI* PlanetInfoPanel;
        GI_Window::TWindowGI* StarInfoWindow;
        GI_Panel::TPanelGI* StandardInfoPanel;
        GI_StarField::TStarFieldGI* StarField;
        GI_Circle::TCircleGI* ActionCircle;
        GI_SpaceCircle::TSpaceCircleGI* ActionColorCircle;
        pas::Array<GI_SpaceCircle::TSpaceCircleGI*, 0, 3> WeaponColorCircles;
        pas::Array<GI_GraphButton::TGraphButtonGI*, 0, 4> WeaponButtons;
        pas::Array<GI_Image::TImageGI*, 0, 4> WeaponImages;
        GI_GraphButton::TGraphButtonGI* AllWeaponsButton;
        GI_GraphButton::TGraphButtonGI* HideSpacePanelButton;
        GI_GraphButton::TGraphButtonGI* ShowSpacePanelButton;
        GI_GraphButton::TGraphButtonGI* ScannerButton;
        GI_GraphButton::TGraphButtonGI* TalkButton;
        GI_GraphButton::TGraphButtonGI* TurnFilmButton;
        GI_Image::TImageGI* WeaponBackgroundImage;
        GI_Image::TImageGI* SpaceBackgroundImage;
        aShip::TShip* ShipToInspect;
        WindowsSdk::TPoint HitObjectPosition;
        WindowsSdk::TPoint HitObjectSize;
        std::uint8_t BattleMusicSelected;
        std::uint8_t EndTurnAfterOpen;
        std::uint8_t cpp_padding_9[2];
        pas::List* PendingSceneObjects;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fStarMap
