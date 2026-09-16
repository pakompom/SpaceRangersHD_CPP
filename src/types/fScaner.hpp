#pragma once
#include "runtime_support.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aConst.hpp"

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

namespace aItem {
    struct TItem;

} // namespace aItem

namespace aNormalShip {
    struct TNormalShip;

} // namespace aNormalShip

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fScaner {
    struct TfScaner;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfScaner : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfScaner, GI_MessageLoop::TMessageLoopGI, "TfScaner", 636)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void BuildRewardStrip(aShip::TShip* Ship);
        void RewardsMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void RewardMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ShowRewardInfo(aNormalShip::TNormalShip* Ship, std::int32_t AwardId);
        void HideRewardInfo();
        void AdvancePanelSlide(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void RewardsMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ShowPropertyInfo(GI_MessageLoop::TObjectGI* Sender);
        void HidePropertyInfo(GI_MessageLoop::TObjectGI* Sender);
        static void RefreshRewardHint(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void CountCargoEntries();
        std::uint8_t GetCargoEntry(std::int32_t Index, aConst::TItemType& ItemType, aItem::TItem*& Item);
        void Update();
        void ScrollCargoLeft(GI_MessageLoop::TObjectGI* Sender);
        void ScrollCargoRight(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void MainPanelMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void UpdateItemHover();
        void AdvanceItemHover(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void HideItemInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ShowItemInfo(aItem::TItem* Item);
        void ShowGoodsInfo(aConst::TItemType ItemType);
        void UpdateSkills();
        static GI_MessageLoop::TObjectGI* CreateAdditionalInfoIcon(GI_Label::TLabelGI* Sender, EC_CacheFont::PFontObjectEC Item);
        void BuildAdditionalInfo();
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        void SelectMusic() override;
        GI_GraphBuf::TGraphBufGI* BackgroundBuffer;
        GI_GraphButton::TGraphButtonGI* ExitButton;
        aShip::TShip* ShipToInspect;
        GI_Window::TWindowGI* ItemInfoWindow;
        GI_Image::TImageGI* ItemImage;
        GI_Label::TLabelGI* ItemNameLabel;
        GI_Label::TLabelGI* ItemDescriptionLabel;
        GI_Label::TLabelGI* ItemSizeLabel;
        GI_Label::TLabelGI* ItemPriceLabel;
        GI_Image::TImageGI* ItemRaceImage;
        GI_Panel::TPanelGI* SkillsPanel;
        GI_Label::TLabelGI* FreeSkillPointsLabel;
        pas::Array<GI_Image::TImageGI*, 0, 5> SkillImages;
        pas::Array<GI_Image::TImageGI*, 0, 5> SkillPositiveImages;
        pas::Array<GI_Image::TImageGI*, 0, 5> SkillNegativeImages;
        pas::Array<GI_Panel::TPanelGI*, 0, 5> SkillPanels;
        pas::Array<std::int32_t, 0, 5> SkillImageRestTop;
        pas::Array<GI_GraphButton::TGraphButtonGI*, 0, 5> SkillButtons;
        GI_GraphBuf::TGraphBufGI* RewardsBuffer;
        GI_Window::TWindowGI* RewardWindow;
        GI_GAI::TgaiGI* HoveredItemAnimation;
        std::int32_t VisibleCargoCount;
        std::int32_t CargoOffset;
        std::int32_t CargoEntryCount;
        GI_MessageLoop::PCallbackTimerGI PanelSlideTimer;
        std::int32_t PanelSlideStep;
        std::int32_t PanelSlideStartX;
        std::int32_t PanelSlideEndX;
        GI_MessageLoop::PCallbackTimerGI ItemHoverTimer;
        GI_MessageLoop::PCallbackTimerGI HideItemTimer;
        GI_MessageLoop::PCallbackTimerGI PropertyHintTimer;
        WindowsSdk::TPoint ShipImageCenter;
        pas::DynArray<GI_Zone::TZoneGI*> ArtefactZones;
        pas::Array<pas::Array<GI_GAI::TgaiGI*, 0, 4>, 0, 7> EquipmentAnimations;
        aItem::TItem* HoveredItem;
        std::int32_t HoveredRewardId;
        std::uint8_t CompactHullInfo;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fScaner
