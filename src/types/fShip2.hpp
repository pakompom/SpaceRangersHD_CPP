#pragma once
#include "runtime_support.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aConst.hpp"
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

namespace GI_Window {
    struct TWindowGI;

} // namespace GI_Window

namespace GI_Zone {
    struct TZoneGI;

} // namespace GI_Zone

namespace GR_Sound {
    struct TSoundBufferControl;

} // namespace GR_Sound

namespace aItem {
    struct TGoods;

    struct TItem;

} // namespace aItem

namespace aNormalShip {
    struct TNormalShip;

} // namespace aNormalShip

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fShip2 {
    struct TPlayerHoldUnit;

    struct TfShip2;

    enum TPlayerHoldKind : std::uint32_t {
        phkEmpty = 0,
        phkGoods = 1,
        phkEquipment = 2,
        phkArtefact = 3,
    };

    enum TPlayerHoldSort : std::uint32_t {
        phsType = 0,
        phsSize = 1,
        phsPrice = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfShip2 : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfShip2, fPanelMain::TMessageLoopGIWithMainPanel, "TfShip2", 1416)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        // Native Self/result stack ordering establishes this as a method.
        static std::uint8_t CanUseLocalStorage();
        // Borrows the current planet or docked ship.
        static pas::Object* GetLocalStorageOwner();
        void RefreshRewards(aNormalShip::TNormalShip* Ship);
        void RewardsMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void RewardsMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ShowRewardTooltip(aNormalShip::TNormalShip* Ship, std::int32_t Award);
        void HideRewardTooltip();
        void SlideRightPanelTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        // Checks integrity outside inspection mode, restores the return screen, closes and raises BreakUiMessage.
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void RewardsMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ShipNameMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ShowShipPropertyInfo(GI_MessageLoop::TObjectGI* Sender);
        void HideShipPropertyInfo(GI_MessageLoop::TObjectGI* Sender);
        // Native empty three-register method; argument purposes unresolved.
        static void UpdateInfoHint(std::int32_t First, std::int32_t Second);
        // Matches the second underscore-delimited component against eight equipment slot names; raises on no match.
        static std::uint8_t SlotToTip(pas::WideString SlotName);
        static std::uint8_t IsCompatibleSlot(aConst::TItemType ItemType, aConst::TItemType SlotType);
        void RefreshEquipmentSlotControls();
        void RefreshShipView();
        void RefreshActionPanels(TPlayerHoldKind Kind, std::uint8_t Good, std::int32_t Quantity, std::int32_t Cost, aItem::TItem* Item, std::int32_t Origin);
        void UpdateActionCursor(std::uint8_t CanTake) override;
        void TrainSkillClicked(GI_MessageLoop::TObjectGI* Sender);
        void EquipmentSlotMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ArtefactSlotMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void UseOnArtefactSlot(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        // Verified callback assignment in InitializeLayout.
        void RemoteHoldItemMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void TakeHoldEntry(TPlayerHoldUnit* Entry, std::int32_t Slot, std::uint8_t UsesDisplayOrder);
        void HullMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        std::uint8_t ConfigureChameleon();
        // Verified callback assignment in InitializeLayout.
        void HoldLeftPressed(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void HoldLeftReleased(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void HoldRightPressed(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void HoldRightReleased(GI_MessageLoop::TObjectGI* Sender);
        void ScrollHoldTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ReturnSelectedHoldEntry();
        void RefreshEquipmentConfigurationButtons();
        void SelectEquipmentConfiguration(std::int32_t Index);
        // Verified callback assignment in InitializeLayout.
        void EquipmentConfigurationClicked(GI_MessageLoop::TObjectGI* Sender);
        void MainKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        // Verified callback assignment in InitializeLayout.
        void MainKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        // Verified callback assignment in InitializeLayout.
        void MainLeftButtonUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        // Verified callback assignment in InitializeLayout.
        void MainRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        std::uint8_t TryDeployTranclucator(aShip::TShip* Ship);
        void DropSelectedOutside(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        // Transfers the selected cargo to arcade space, or destroys it in arcade view mode.
        void DropSelectedInArcade();
        void SpecialSlot1Clicked(GI_MessageLoop::TObjectGI* Sender);
        void SpecialSlot2Clicked(GI_MessageLoop::TObjectGI* Sender);
        void AdvanceScriptVideo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        // Returns whether a video timer was active.
        std::uint8_t StopScriptVideo();
        // Verified callback assignment in InitializeLayout.
        void UseMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SpecialSlot3Clicked(GI_MessageLoop::TObjectGI* Sender);
        void RepairAllClicked(GI_MessageLoop::TObjectGI* Sender);
        void RepairAllMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void RepairAllMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void LoadRockets(std::uint8_t Equipped);
        void ToggleAfterburner(GI_MessageLoop::TObjectGI* Sender);
        void ShowItemInfo();
        void ShowItemInfoTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void HideItemInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        static void LayoutItemInfo(GI_Window::TWindowGI* Window, GI_Label::TLabelGI* Title, GI_Label::TLabelGI* Text, std::uint8_t KeepMinimumHeight, std::uint8_t WordWrap, std::int32_t MinimumWidth);
        // Two-column planet/ship statistics, shared with live and recorded star-map panels.
        void LayoutObjectInfo(GI_Window::TWindowGI* Window, GI_Label::TLabelGI* Title, GI_Label::TLabelGI* Left1, GI_Label::TLabelGI* Right1, GI_Label::TLabelGI* Left2, GI_Label::TLabelGI* Right2, GI_Label::TLabelGI* Left3, GI_Label::TLabelGI* Right3, GI_Label::TLabelGI* Left4, GI_Label::TLabelGI* Right4, GI_Label::TLabelGI* Left5, GI_Label::TLabelGI* Right5, GI_Label::TLabelGI* Left6, GI_Label::TLabelGI* Right6, GI_Label::TLabelGI* Left7, GI_Label::TLabelGI* Right7, GI_Label::TLabelGI* Left8, GI_Label::TLabelGI* Right8, GI_MessageLoop::TObjectGI* Emblem, std::uint8_t KeepMinimumHeight, std::int32_t MinimumWidth);
        void ShowEquipmentInfo(aItem::TItem* Item, std::uint8_t FromStorage);
        void ShowHoldGoodsInfo(std::uint8_t Good);
        void ShowStoredGoodsInfo(aItem::TGoods* Goods);
        void RefreshMoneyWarning();
        void StartMoneyWarning();
        void AdvanceMoneyWarning(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ShowNoDropMessage(std::int32_t Code);
        // Verified callback assignment in InitializeLayout.
        void GateMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void GateMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void OpenGate();
        void CloseGate();
        void SlideGateTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        // Verified callback assignment in InitializeLayout.
        void GateMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        // Verified callback assignment in InitializeLayout.
        void UseMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void UseMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void OpenUsePanel();
        void CloseUsePanel();
        void SlideUseTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void OpenUseSidePanel();
        void CloseUseSidePanel();
        void SlideUsePanelTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void OpenSpecialSlot1();
        void CloseSpecialSlot1();
        void AnimateSpecialSlot1(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void OpenSpecialSlot2();
        void CloseSpecialSlot2();
        void AnimateSpecialSlot2(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void OpenSpecialSlot3();
        void CloseSpecialSlot3();
        void AnimateSpecialSlot3(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        // Verified callback assignment in InitializeLayout.
        void StorageDownClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void StorageUpClicked(GI_MessageLoop::TObjectGI* Sender);
        void SlideStorageTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void RefreshStorageView();
        void ScrollStorageUp(GI_MessageLoop::TObjectGI* Sender);
        void ScrollStorageDown(GI_MessageLoop::TObjectGI* Sender);
        void StorageItemMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        static GI_MessageLoop::TObjectGI* CreateShipInfoImage(GI_Label::TLabelGI* Sender, EC_CacheFont::PFontObjectEC Item);
        void BuildAdditionalInfoPanel();
        static void HideSender(GI_MessageLoop::TObjectGI* Sender);
        void ProcessCallbackTimers() override;
        void ProcessWindowMessage(std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) override;
        pas::Method<void(std::uint32_t, std::uint32_t, std::int32_t)> bind_ProcessWindowMessage() override;
        void SelectMusic() override;
        static std::uint8_t IsHoldNormalShip();
        // Ruins with modernization sponsorship or a player-owned tranclucator.
        static std::uint8_t CanUsePlayerExperience();
        // Verified callback assignment in InitializeLayout.
        void ToggleRemoteHoldClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void RemoteHoldUpPressed(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void RemoteHoldUpReleased(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void RemoteHoldDownPressed(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void RemoteHoldDownReleased(GI_MessageLoop::TObjectGI* Sender);
        void ScrollRemoteHoldTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        static std::int32_t GetRemoteHoldScrollLimit();
        // Verified callback assignment in InitializeLayout.
        void SortRemoteHoldClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void StorageToShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void SellStorageClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void ShipToStorageClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void SortStorageByTypeClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void SortStorageBySizeClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void SortStorageByPriceClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void LoadHoldRocketsClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void SellHoldClicked(GI_MessageLoop::TObjectGI* Sender);
        // Verified callback assignment in InitializeLayout.
        void LoadEquippedRocketsClicked(GI_MessageLoop::TObjectGI* Sender);
        void SortStorage(TPlayerHoldSort Sort);
        void RefreshLoadEquippedRocketsButton();
        // Zero sells hold contents; one sells local storage.
        void SellAllItems(std::int32_t Origin);
        // Verified callback assignment in InitializeLayout.
        void EnterBridgeClicked(GI_MessageLoop::TObjectGI* Sender);
        GI_MessageLoop::TMessageLoopGI* GetActionParentLoop() override;
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        // Gates star-map input and selects OnOpen item-script events; full meaning unresolved.
        std::uint8_t FlagD4;
        // Restarts active hull/equipment repair animations on the next OnOpen.
        std::uint8_t PlayServiceAnimations;
        std::uint8_t cpp_padding[2];
        GI_Window::TWindowGI* ItemInfoWindow;
        GI_Image::TImageGI* ItemImage;
        GI_Label::TLabelGI* ItemNameLabel;
        GI_Label::TLabelGI* ItemDescriptionLabel;
        GI_Label::TLabelGI* ItemSizeLabel;
        GI_Label::TLabelGI* ItemPriceLabel;
        GI_Image::TImageGI* ItemRaceImage;
        GI_Image::TImageGI* RightOpenImage;
        GI_Panel::TPanelGI* SkillsPanel;
        GI_Label::TLabelGI* FreeSkillPointsLabel;
        GI_Label::TLabelGI* ExperienceLabel;
        pas::Array<GI_Image::TImageGI*, 0, 5> SkillImages;
        pas::Array<GI_Image::TImageGI*, 0, 5> SkillImagesP;
        pas::Array<GI_Image::TImageGI*, 0, 5> SkillImagesN;
        pas::Array<GI_Panel::TPanelGI*, 0, 5> SkillPanels;
        pas::Array<std::int32_t, 0, 5> SkillImageRestTop;
        pas::Array<GI_GraphButton::TGraphButtonGI*, 0, 5> SkillButtons;
        pas::Array<GI_Label::TLabelGI*, 0, 5> SkillValueLabels;
        pas::Array<GI_Image::TImageGI*, 0, 5> SkillProgressImages;
        pas::Array<GI_Image::TImageGI*, 0, 5> SkillGainImages;
        pas::Array<GI_Zone::TZoneGI*, 0, 5> HoldSlotZones;
        // Five slots per equipment category.
        pas::Array<pas::Array<GI_Zone::TZoneGI*, 0, 4>, 0, 7> EquipmentSlotZones;
        // Five slots per equipment category.
        pas::Array<pas::Array<GI_GAI::TgaiGI*, 0, 4>, 0, 7> EquipmentSlotAnimations;
        // Dynamically sized to the ship's artefact slot count.
        pas::DynArray<GI_Zone::TZoneGI*> ArtefactSlotZones;
        GI_GraphBuf::TGraphBufGI* BackgroundBuffer;
        GI_GraphButton::TGraphButtonGI* ExitButton;
        GI_GraphBuf::TGraphBufGI* RewardsBuffer;
        // Verified by InitializeLayout assigning RewardWnd.
        GI_Window::TWindowGI* RewardsWindow;
        std::int32_t HoldFirstIndex;
        // Reset by the main menu's New handler.
        TPlayerHoldKind SelectedHoldKind;
        // Selected goods type.
        std::uint8_t SelectedGoodsIndex;
        std::uint8_t cpp_padding_2[3];
        // Selected goods quantity.
        std::int32_t SelectedGoodsQuantity;
        // Total purchase cost of the selected goods.
        std::int32_t SelectedGoodsCost;
        // Selected equipment or artefact; nil for goods.
        aItem::TItem* SelectedHoldItem;
        // Zero is ship inventory; nonzero returns to local storage.
        std::int32_t SelectedHoldOrigin;
        // Original hold/storage index; negative values encode equipment slots as -slot-1.
        std::int32_t SelectedHoldSlot;
        // Interpret SelectedHoldSlot as a persistent display order.
        std::uint8_t SelectedHoldUsesDisplayOrder;
        std::uint8_t cpp_padding_3[3];
        GI_MessageLoop::PCallbackTimerGI RightPanelSlideTimer;
        std::int32_t RightPanelSlideStep;
        std::int32_t PanelSlideWidth;
        std::int32_t RightPanelRestLeft;
        std::int32_t DestrPanelSlideWidth;
        std::int32_t DestrPanelRestLeft;
        GI_MessageLoop::PCallbackTimerGI NativeTimer384;
        GI_MessageLoop::PCallbackTimerGI ItemInfoHideTimer;
        GI_MessageLoop::PCallbackTimerGI PropertyInfoHideTimer;
        WindowsSdk::TPoint ShipImageCenter;
        // Used by the shared item/arena hover panels.
        WindowsSdk::TPoint ItemImageCenter;
        // Offset from the information window's bottom.
        WindowsSdk::TPoint ItemSizeLabelPosition;
        // Offset from the information window's bottom.
        WindowsSdk::TPoint ItemPriceLabelPosition;
        // Offset from the information window's bottom-right corner.
        WindowsSdk::TPoint ItemRaceImagePosition;
        // Zero for no item, Good+1 for hold goods, or an item pointer for equipment/storage.
        std::int32_t DisplayedItemKey;
        // Accumulated across modal ship-screen reopenings; full meaning unresolved.
        std::uint8_t Flag3BC;
        // Suppresses space-music changes in SelectMusic.
        std::uint8_t PreserveSpaceMusic;
        std::uint8_t cpp_padding_4[2];
        std::int32_t SelectedReward;
        std::uint8_t MoneyWarningVisible;
        std::uint8_t cpp_padding_5[3];
        std::int32_t MoneyWarningTicks;
        GI_MessageLoop::PCallbackTimerGI MoneyWarningTimer;
        GI_GAI::TgaiGI* HoveredEquipmentAnimation;
        GI_MessageLoop::PCallbackTimerGI GateSlideTimer;
        std::int32_t GateSlideOffset;
        std::int32_t GateLeftRestLeft;
        std::int32_t GateRightRestLeft;
        GI_MessageLoop::PCallbackTimerGI UseSlideTimer;
        std::int32_t UseSlideOffset;
        std::int32_t UseLeftRestLeft;
        std::int32_t UseRightRestLeft;
        GI_MessageLoop::PCallbackTimerGI UsePanelSlideTimer;
        std::int32_t UsePanelSlideOffset;
        std::uint8_t cpp_padding_6[8];
        GI_MessageLoop::PCallbackTimerGI SpecialSlot1Timer;
        GI_MessageLoop::PCallbackTimerGI SpecialSlot2Timer;
        GI_MessageLoop::PCallbackTimerGI SpecialSlot3Timer;
        pas::Array<GI_Image::TImageGI*, 0, 20> StorageImages;
        GI_GraphButton::TGraphButtonGI* StorageUpButton;
        GI_MessageLoop::PCallbackTimerGI StorageSlideTimer;
        std::int32_t StorageSlideOffset;
        std::int32_t StoragePanelSlideHeight;
        std::int32_t StoragePanelRestTop;
        std::int32_t StorageFirstSlot;
        // Captured RankWnd left edge; the scanner positions its property window immediately to its left.
        std::int32_t PropertyHintRightEdge;
        std::uint32_t ScriptVideoStartedAt;
        GI_MessageLoop::PCallbackTimerGI ScriptVideoTimer;
        GI_MessageLoop::PCallbackTimerGI HoldScrollTimer;
        GR_Sound::TSoundBufferControl* ShipLoopSound;
        std::uint8_t HighlightRepairableEquipment;
        std::uint8_t cpp_padding_7[3];
        std::int32_t SavedCaptainFrame;
        std::uint8_t RemoteHoldVisible;
        std::uint8_t cpp_padding_8[3];
        // Native OnOpen selects this ship when non-nil.
        aShip::TShip* ShipToInspect;
        std::int32_t SavedShipExperience;
        // Native constructor initializes False; meaning unresolved.
        std::uint8_t RemoteHoldMode;
        std::uint8_t cpp_padding_9[3];
        pas::Array<GI_Image::TImageGI*, 0, 54> RemoteHoldImages;
        std::int32_t RemoteHoldFirstOrder;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPlayerHoldUnit : pas::Object {
        PAS_CLASS_META(TPlayerHoldUnit, pas::Object, "TPlayerHoldUnit", 28)
        TPlayerHoldKind Kind;
        std::uint8_t GoodsIndex;
        std::uint8_t cpp_padding[3];
        std::int32_t ItemId;
        std::int32_t DisplayOrder;
        aItem::TItem* Item;
        std::uint8_t Retained;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fShip2
