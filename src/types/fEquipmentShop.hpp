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

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Window {
    struct TWindowGI;

} // namespace GI_Window

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aItem {
    struct THull;

    struct TItem;

} // namespace aItem

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fPanelPlanet {
    struct TfPanelPlanet;

} // namespace fPanelPlanet

namespace fPanelRuins {
    struct TfPanelRuins;

} // namespace fPanelRuins

namespace fEquipmentShop {
    struct TShopSlot;

    struct TfEquipmentShop;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TShopSlot : EC_Struct::TObjectEx {
        PAS_CLASS_META(TShopSlot, EC_Struct::TObjectEx, "TShopSlot", 40)
        void p_destroy() override;
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        WindowsSdk::TPoint GridPoint;
        aItem::TItem* Item;
        GI_Image::TImageGI* SlotImage;
        GI_Image::TImageGI* BorderImage;
        GI_Image::TImageGI* TypeOverlayImage;
        GI_Image::TImageGI* ItemIconImage;
        GI_GAI::TgaiGI* ItemAnimation;
        GI_Image::TImageGI* MicroModuleImage;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfEquipmentShop : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfEquipmentShop, fPanelMain::TMessageLoopGIWithMainPanel, "TfEquipmentShop", 288)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void BuildGoodsControls();
        void ClearGoodsControls();
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void UpdateScrollButtons();
        static void StartSlotAnimatedPreview(TShopSlot* Slot);
        void ScheduleSlotPreviewStop(TShopSlot* Slot);
        void ItemMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void ItemMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ScrollLeft(GI_MessageLoop::TObjectGI* Sender);
        void ScrollRight(GI_MessageLoop::TObjectGI* Sender);
        void ScrollTick(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void PanelScrollChanged(GI_MessageLoop::TObjectGI* Sender);
        void ItemMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ChooseAnimatedPreview(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        static void PreviewCycleComplete(GI_MessageLoop::TObjectGI* Sender);
        void HideItemInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void RefreshItemInfo(aItem::TItem* Item);
        void BuildHullSlotOverlays(GI_MessageLoop::TObjectGI* Parent, aItem::THull* Hull, std::int32_t OffsetX, std::int32_t OffsetY);
        void RefreshHullInfo(GI_MessageLoop::TMessageLoopGI* Target, aItem::THull* Hull, pas::WideString Text, std::uint8_t SuppressImage);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        void SelectMusic() override;
        fPanelPlanet::TfPanelPlanet* PlanetPanel;
        fPanelRuins::TfPanelRuins* StationPanel;
        fPanelLoad::TfPanelLoad* LoadPanel;
        WindowsSdk::TPoint ItemInfoAnchor;
        GI_Window::TWindowGI* ItemInfoWindow;
        std::int32_t UnknownEC;
        std::int32_t ContentColumnCount;
        std::int32_t TargetScrollX;
        GI_MessageLoop::PCallbackTimerGI ScrollTimer;
        GI_MessageLoop::PCallbackTimerGI ItemInfoTimer;
        GI_MessageLoop::PCallbackTimerGI OpenPreviewTimer;
        TShopSlot* PreviewSlot;
        WindowsSdk::TPoint HullSizeOffset;
        WindowsSdk::TPoint HullPriceOffset;
        WindowsSdk::TPoint HullRaceOffset;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fEquipmentShop
