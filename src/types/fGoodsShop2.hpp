#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GR_Sound {
    struct TSoundBufferControl;

} // namespace GR_Sound

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fPanelPlanet {
    struct TfPanelPlanet;

} // namespace fPanelPlanet

namespace fPanelRuins {
    struct TfPanelRuins;

} // namespace fPanelRuins

namespace fGoodsShop2 {
    struct TGoodsShopTradeRow;

    struct TfGoodsShop2;

    // Ship-to-ship market row; scalar layout is also consumed by the integrity checksum.
    #pragma pack(push, 1)
    struct TGoodsShopTradeRow {
        std::int32_t Count;
        float MaximumPrice;
        std::int32_t PurchasePrice;
        std::int32_t BaseSalePrice;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfGoodsShop2 : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfGoodsShop2, fPanelMain::TMessageLoopGIWithMainPanel, "TfGoodsShop2", 404)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        // Rebuilds cargo/market controls and prices for the current trading context.
        void RefreshGoodsDisplay();
        void GoodsMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void GoodsRightMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void GoodsMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void UpdateActionCursor(std::uint8_t CanTake) override;
        void GoodsMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void GoodsMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        std::int32_t GetMaximumTradeCount(std::int32_t Index, std::uint8_t IgnoreCargoSpace);
        static std::int32_t GetAvailableGoodsCount(std::int32_t Index);
        static pas::WideString BuildPriceText(pas::Object* Location);
        void SavePricesClicked(GI_MessageLoop::TObjectGI* Sender);
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void GalaxyClicked(GI_MessageLoop::TObjectGI* Sender);
        void QuestClicked(GI_MessageLoop::TObjectGI* Sender);
        void MenuClicked(GI_MessageLoop::TObjectGI* Sender);
        void ShipClicked(GI_MessageLoop::TObjectGI* Sender);
        void FinishModalTrade();
        void CaptureMerchantBackground(GI_MessageLoop::TObjectGI* Sender);
        void MerchantAnimationComplete(GI_MessageLoop::TObjectGI* Sender);
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshMoneyWarning();
        void FlashMoneyWarning();
        void MoneyWarningTick(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void RefreshCargoWarning();
        void FlashCargoWarning();
        void CargoWarningTick(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible);
        void ShowHelpText(pas::WideString Value, std::uint8_t Visible);
        void ProcessCallbackTimers() override;
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        void SelectMusic() override;
        fPanelPlanet::TfPanelPlanet* PlanetPanel;
        fPanelRuins::TfPanelRuins* StationPanel;
        fPanelLoad::TfPanelLoad* LoadPanel;
        // -1 when no market/cargo row is being dragged.
        std::int32_t DraggedGoodsIndex;
        std::int32_t NameFaceHeight;
        // Total original extent from name top to character-description bottom.
        std::int32_t FaceCaptionHeight;
        // Suppresses parent star-map presentation during modal transitions; other uses unresolved.
        std::uint8_t FlagEC;
        std::uint8_t cpp_padding[3];
        pas::Array<TGoodsShopTradeRow, 0, 7> TradeRows;
        // Trading partner cargo limit.
        std::int32_t PartnerCargoLimit;
        // Trading partner money limit.
        std::int32_t PartnerMoneyLimit;
        std::uint8_t MoneyWarningActive;
        std::uint8_t cpp_padding_2[3];
        std::int32_t MoneyWarningTicks;
        GI_MessageLoop::PCallbackTimerGI MoneyWarningTimer;
        std::uint8_t CargoWarningActive;
        std::uint8_t cpp_padding_3[3];
        std::int32_t CargoWarningTicks;
        GI_MessageLoop::PCallbackTimerGI CargoWarningTimer;
        GR_Sound::TSoundBufferControl* AmbientSound;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fGoodsShop2
