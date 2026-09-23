#include "layout/fEquipmentShop.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Data.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aPlanet.hpp"
#include "types/aRuins.hpp"
#include "types/fRuinsTalk.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fPanelPlanet.hpp"
#include "units/fPanelRuins.hpp"
#include "units/fShip2.hpp"

namespace fEquipmentShop {
    std::int32_t ShopGridRowCount = 4;

    std::int32_t ShopVisibleColumnCount = 6;

    pas::List* TemporaryShopSlots = nullptr;

    aPlanet::TPlanet* TemporaryShopPlanet = nullptr;

    aRuins::TRuins* TemporaryShopStation = nullptr;

    aGalaxyStruct::TDominatorSeriesNameTable ShopDominatorImagePrefixes = aGalaxyStruct::TDominatorSeriesNameTable{{u"B"_w, u"K"_w, u"T"_w}};

    // Native no-op. Sole caller passes nil; original parameter meaning is unresolved.
    void TemporaryShopStockHook(void* Argument) {
    }

    // Transfers ownership of market inventory into the temporary slots.
    void BuildTemporaryShopSlotGrid() {
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t I{};
        std::int32_t J{};
        TShopSlot* Slot{};
        TShopSlot* Other{};
        aItem::TItem* Item{};
        std::int32_t ItemOrder{};
        std::int32_t OtherOrder{};
        aRuins::TRuins* Station{};
        fEquipmentShop::TemporaryShopStockHook(nullptr);
        fEquipmentShop::ClearTemporaryShopSlotGrid();
        TemporaryShopSlots = pas::make_object<pas::List>();
        if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentPlanet->EquipmentShop) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->CurrentPlanet->EquipmentShop, I);
                Slot = pas::construct_call<TShopSlot>(TShopSlot_Create);
                pas::list_add(TemporaryShopSlots, reinterpret_cast<void*>(Slot));
                Slot->Item = Item;
            }
            pas::list_clear(aPlayer::GetPlayer()->CurrentPlanet->EquipmentShop);
            TemporaryShopPlanet = aPlayer::GetPlayer()->CurrentPlanet;
        } else {
            if (aPlayer::GetPlayer()->DockedTo == nullptr) {
                return;
            }
            if (!(pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo) != nullptr)) {
                return;
            }
            Station = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Station->EquipmentShop) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Station->EquipmentShop, I);
                Slot = pas::construct_call<TShopSlot>(TShopSlot_Create);
                pas::list_add(TemporaryShopSlots, reinterpret_cast<void*>(Slot));
                Slot->Item = Item;
            }
            pas::list_clear(Station->EquipmentShop);
            TemporaryShopStation = Station;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(TemporaryShopSlots) - 2); cpp_range_3.next(I); ) {
            Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
            if (Slot->Item == nullptr) {
                continue;
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(I + 1, pas::list_count(TemporaryShopSlots) - 1); cpp_range_4.next(J); ) {
                Other = pas::list_at<TShopSlot>(TemporaryShopSlots, J);
                if (Other->Item == nullptr) {
                    continue;
                }
                if (pas::in_range(Slot->Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    ItemOrder = Slot->Item->ItemType - 75;
                } else {
                    ItemOrder = Slot->Item->ItemType;
                }
                if (pas::in_range(Other->Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    OtherOrder = Other->Item->ItemType - 75;
                } else {
                    OtherOrder = Other->Item->ItemType;
                }
                if (ItemOrder > OtherOrder || ItemOrder == OtherOrder && Slot->Item->Cost > Other->Item->Cost) {
                    Item = Slot->Item;
                    Slot->Item = Other->Item;
                    Other->Item = Item;
                }
            }
        }
        std::int32_t EmptyCount = 0;
        if (pas::list_count(TemporaryShopSlots) < ShopVisibleColumnCount * ShopGridRowCount) {
            EmptyCount = ShopVisibleColumnCount * ShopGridRowCount - pas::list_count(TemporaryShopSlots);
        } else if (pas::imod(pas::list_count(TemporaryShopSlots), ShopGridRowCount) != 0) {
            EmptyCount = ShopGridRowCount - pas::imod(pas::list_count(TemporaryShopSlots), ShopGridRowCount);
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, EmptyCount); cpp_range_5.next(I); ) {
            Slot = pas::construct_call<TShopSlot>(TShopSlot_Create);
            Slot->Item = nullptr;
            pas::list_add(TemporaryShopSlots, reinterpret_cast<void*>(Slot));
        }
        EmptyCount = 0;
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, ShopGridRowCount - 1); cpp_range_6.next(Y); ) {
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::idiv(pas::list_count(TemporaryShopSlots), ShopGridRowCount) - 1); cpp_range_7.next(X); ) {
                Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, EmptyCount);
                Slot->GridPoint = ClassesImports::Point(X, Y);
                ++EmptyCount;
            }
        }
    }

    // Returns ownership of remaining items to the original market.
    void RestoreTemporaryShopStock() {
        std::int32_t I{};
        std::int32_t Count{};
        TShopSlot* Slot{};
        if (TemporaryShopSlots == nullptr) {
            return;
        }
        if (TemporaryShopPlanet != nullptr) {
            Count = pas::list_count(TemporaryShopSlots);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
                if (Slot->Item != nullptr) {
                    pas::list_add(TemporaryShopPlanet->EquipmentShop, reinterpret_cast<void*>(Slot->Item));
                }
                Slot->Item = nullptr;
            }
        } else if (TemporaryShopStation != nullptr) {
            Count = pas::list_count(TemporaryShopSlots);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
                if (Slot->Item != nullptr) {
                    pas::list_add(TemporaryShopStation->EquipmentShop, reinterpret_cast<void*>(Slot->Item));
                }
                Slot->Item = nullptr;
            }
        }
        fEquipmentShop::ClearTemporaryShopSlotGrid();
    }

    // Frees remaining items without returning them to the market.
    void ClearTemporaryShopSlotGrid() {
        std::int32_t I{};
        std::int32_t Count{};
        TShopSlot* Slot{};
        if (TemporaryShopSlots != nullptr) {
            Count = pas::list_count(TemporaryShopSlots);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
                pas::free(Slot);
            }
            pas::list_clear(TemporaryShopSlots);
            pas::free(TemporaryShopSlots);
            TemporaryShopSlots = nullptr;
            TemporaryShopPlanet = nullptr;
            TemporaryShopStation = nullptr;
        }
    }

    TShopSlot* FindShopSlotByGridPoint(WindowsSdk::TPoint Point) {
        std::int32_t I{};
        TShopSlot* Slot{};
        std::int32_t Count = pas::list_count(TemporaryShopSlots);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
            if (Slot->GridPoint.X == Point.X && Slot->GridPoint.Y == Point.Y) {
                return Slot;
            }
        }
        return nullptr;
    }

    TShopSlot* FindShopSlotByItem(aItem::TItem* Item) {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(TemporaryShopSlots) - 1); cpp_range.next(I); ) {
            if (pas::list_at<TShopSlot>(TemporaryShopSlots, I)->Item == Item) {
                return pas::list_at<TShopSlot>(TemporaryShopSlots, I);
            }
        }
        return nullptr;
    }

    pas::WideString GetShopItemIconName(aItem::TItem* Item) {
        pas::WideString Result{};
        aGalaxyStruct::TOwnerId Owner{};
        if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr && pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator)) && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(aGalaxyStruct::heHolyFanaticism)) {
            Owner = Item->OwnerId;
            Item->OwnerId = aGalaxyStruct::oiDominator;
            Result = Item->GetBitmapResourceName();
            Item->OwnerId = Owner;
            return Result;
        }
        return Item->GetBitmapResourceName();
    }

    // Owns Item while the location's shop list is detached, and frees remaining controls.
    void TShopSlot_Create(TShopSlot* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TShopSlot_Destroy(TShopSlot* Self) {
        if (Self->Item != nullptr) {
            pas::free(Self->Item);
            Self->Item = nullptr;
        }
        if (Self->SlotImage != nullptr) {
            pas::free(Self->SlotImage);
            Self->SlotImage = nullptr;
        }
        if (Self->BorderImage != nullptr) {
            pas::free(Self->BorderImage);
            Self->BorderImage = nullptr;
        }
        if (Self->TypeOverlayImage != nullptr) {
            pas::free(Self->TypeOverlayImage);
            Self->TypeOverlayImage = nullptr;
        }
        if (Self->ItemIconImage != nullptr) {
            pas::free(Self->ItemIconImage);
            Self->ItemIconImage = nullptr;
        }
        if (Self->ItemAnimation != nullptr) {
            pas::free(Self->ItemAnimation);
            Self->ItemAnimation = nullptr;
        }
        if (Self->MicroModuleImage != nullptr) {
            pas::free(Self->MicroModuleImage);
            Self->MicroModuleImage = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TShopSlot::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddAnsiChar(GridPoint.X);
        Buffer->AddAnsiChar(GridPoint.Y);
        if (Item == nullptr) {
            Buffer->AddAnsiChar('\000');
        } else {
            Buffer->AddAnsiChar('\001');
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Item->ItemType));
            if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item)) {
                weapon->Target = nullptr;
            }
            Item->SaveToBuffer(Buffer);
        }
    }

    void TShopSlot::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        GridPoint.X = EC_Buf::TBufEC_GetByte(Buffer);
        GridPoint.Y = EC_Buf::TBufEC_GetByte(Buffer);
        if (EC_Buf::TBufEC_GetByte(Buffer) == 1) {
            Item = pas::checked_cast<aItem::TEquipment*>(static_cast<pas::Object*>(aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)))));
            Item->LoadFromBuffer(Buffer, Galaxy);
        }
    }

    void TfEquipmentShop_Create(TfEquipmentShop* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->PlanetPanel = pas::construct_call<fPanelPlanet::TfPanelPlanet>(fPanelPlanet::TfPanelPlanet_Create);
        Self->StationPanel = pas::construct_call<fPanelRuins::TfPanelRuins>(fPanelRuins::TfPanelRuins_Create);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfEquipmentShop_Destroy(TfEquipmentShop* Self) {
        if (Self->PlanetPanel != nullptr) {
            pas::free(Self->PlanetPanel);
            Self->PlanetPanel = nullptr;
        }
        if (Self->StationPanel != nullptr) {
            pas::free(Self->StationPanel);
            Self->StationPanel = nullptr;
        }
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfEquipmentShop::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        PlanetPanel->InitializeLayout(this);
        StationPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        std::int32_t ExtraWidth = GR_Main::ExtraScreenWidth / 198 * 198;
        if (ExtraWidth > 198) {
            ExtraWidth = 198;
        }
        ShopVisibleColumnCount = 6 + ExtraWidth / 99;
        GR_Main::AppendLogTextThreadSafe("fEquipmentShop... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGCity"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGCity2"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* PanelShop = MainPanel->FindByNameRecursive(u"PanelShop"sv);
                PanelShop->SetSize(ClassesImports::Point(PanelShop->ClientSize.X + ExtraWidth, PanelShop->ClientSize.Y));
                PanelShop->SetPosition(ClassesImports::Point((GR_Main::GameScreenWidth - PanelShop->ClientSize.X) / 2, PanelShop->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
                if (ExtraWidth > 0) {
                    {
                        GI_MessageLoop::TObjectGI* Right = PanelShop->FindByNameRecursive(u"Right"sv);
                        Right->SetPosition(ClassesImports::Point(Right->LocalPosition.X + ExtraWidth, Right->LocalPosition.Y));
                    }
                    {
                        GI_MessageLoop::TObjectGI* ButFormClose = PanelShop->FindByNameRecursive(u"ButFormClose"sv);
                        ButFormClose->SetPosition(ClassesImports::Point(ButFormClose->LocalPosition.X + ExtraWidth, ButFormClose->LocalPosition.Y));
                    }
                    {
                        GI_MessageLoop::TObjectGI* PanelImage = PanelShop->FindByNameRecursive(u"PanelImage"sv);
                        PanelImage->SetSize(ClassesImports::Point(PanelImage->ClientSize.X + ExtraWidth, PanelImage->ClientSize.Y));
                    }
                    {
                        GI_MessageLoop::TObjectGI* BGImageRace = PanelShop->FindByNameRecursive(u"BGImageRace"sv);
                        BGImageRace->SetSize(ClassesImports::Point(BGImageRace->ClientSize.X + ExtraWidth, BGImageRace->ClientSize.Y));
                    }
                    {
                        GI_MessageLoop::TObjectGI* PanelGoods = PanelShop->FindByNameRecursive(u"PanelGoods"sv);
                        PanelGoods->SetSize(ClassesImports::Point(PanelGoods->ClientSize.X + ExtraWidth, PanelGoods->ClientSize.Y));
                    }
                }
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"sv))->UpCallback = pas::bind_method<&TfEquipmentShop::EndTurnClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"sv))->UpCallback = pas::bind_method<&TfEquipmentShop::ShipClicked>(this);
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfEquipmentShop::MainPanelKeyDown>(this);
        ItemInfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"PII"sv));
        {
            GI_MessageLoop::TObjectGI* InfoHullSize = GetByName(u"InfoHullSize"sv);
            HullSizeOffset = ClassesImports::Point(InfoHullSize->LocalPosition.X, InfoHullSize->LocalPosition.Y - InfoHullSize->Parent->ClientSize.Y);
        }
        {
            GI_MessageLoop::TObjectGI* InfoHullPrice = GetByName(u"InfoHullPrice"sv);
            HullPriceOffset = ClassesImports::Point(InfoHullPrice->LocalPosition.X, InfoHullPrice->LocalPosition.Y - InfoHullPrice->Parent->ClientSize.Y);
        }
        {
            GI_MessageLoop::TObjectGI* InfoHullEmRace = GetByName(u"InfoHullEmRace"sv);
            HullRaceOffset = ClassesImports::Point(InfoHullEmRace->LocalPosition.X - InfoHullEmRace->Parent->ClientSize.X, InfoHullEmRace->LocalPosition.Y - InfoHullEmRace->Parent->ClientSize.Y);
        }
    }

    void TfEquipmentShop::OnOpen() {
        aGalaxyStruct::TOwnerId Owner{};
        std::int32_t Size{};
        pas::WideString BackgroundPath{};
        pas::List* SavedSlots{};
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
        }
        MainPanel->OnOpen();
        LoadPanel->OnOpen();
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            fPanelPlanet::TfPanelPlanet::OnOpen();
            PlanetPanel->Show();
            StationPanel->Hide();
        } else {
            PlanetPanel->Hide();
            StationPanel->OnOpen();
            StationPanel->Show();
        }
        {
            GI_GraphButton::TGraphButtonGI* ButFormClose = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButFormClose"sv));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                ButFormClose->UpCallback = pas::bind_method<&fPanelPlanet::TfPanelPlanet::PlanetClicked>(PlanetPanel);
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                ButFormClose->UpCallback = pas::bind_method<&fPanelRuins::TfPanelRuins::ServicesClicked>(StationPanel);
            }
        }
        {
            GI_Image::TImageGI* BGCity2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity2"sv));
            BGCity2->SetActive(aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstMilitaryBase);
            if (BGCity2->Active) {
                BGCity2->SetImagePath(pas::concat_wide({u"GAI,", aPlayer::GetPlayer()->CurrentStar->GetBackgroundImagePath(Size)}));
                BGCity2->GaiImageControl->LoadFrameSequenceFromText(GI_GAI::SingleFrameAnimationSpec);
                BGCity2->SetImageKindX(GI_Main::ikxCenter);
                BGCity2->SetImageKindY(GI_Main::ikyCenter);
            }
        }
        {
            GI_Image::TImageGI* BGCity = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity"sv));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                BGCity->SetActive(true);
                {
                    pas::WideString governmentBackgroundGraph = aPlayer::GetPlayer()->CurrentPlanet->GetGovernmentBackgroundGraph();
                    GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(BGCity->FindByNameRecursive(u"BGCity"sv));
                    cpp_arg->SetImagePath(std::move(governmentBackgroundGraph));
                }
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                BGCity->SetActive(true);
                if (aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey != u"") {
                    BackgroundPath = pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey, u"bg"});
                    if (GR_Main::CacheDataRoot->FileExistsByPath(BackgroundPath)) {
                        BGCity->SetImagePath(pas::concat_wide({u"GI,", BackgroundPath}));
                    } else {
                        BGCity->SetImagePath(pas::concat_wide({u"GI,Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[aPlayer::GetPlayer()->DockedTo->TypeId].Name, u"bg"}));
                    }
                } else {
                    BGCity->SetImagePath(pas::concat_wide({u"GI,Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[aPlayer::GetPlayer()->DockedTo->TypeId].Name, u"bg"}));
                }
            } else {
                BGCity->SetActive(false);
            }
        }
        {
            GI_Image::TImageGI* BGImageRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGImageRace"sv));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                Owner = aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId);
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                Owner = aPlayer::GetPlayer()->DockedTo->OwnerId;
            } else {
                Owner = aGalaxyStruct::oiMaloc;
            }
            BGImageRace->SetActive(true);
            if (Owner == aGalaxyStruct::oiPeleng) {
                BGImageRace->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Peleng"}));
            } else if (Owner == aGalaxyStruct::oiHuman) {
                BGImageRace->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"People"}));
            } else if (Owner == aGalaxyStruct::oiFeyan) {
                BGImageRace->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Fei"}));
            } else if (Owner == aGalaxyStruct::oiGaal) {
                BGImageRace->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Gaal"}));
            } else {
                BGImageRace->SetActive(false);
            }
        }
        {
            GI_GraphButton::TGraphButtonGI* Left = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Left"sv));
            Left->DownCallback = pas::bind_method<&TfEquipmentShop::ScrollLeft>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Right = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Right"sv));
            Right->DownCallback = pas::bind_method<&TfEquipmentShop::ScrollRight>(this);
        }
        GetByName(u"PII"sv)->SetActive(false);
        ClearGoodsControls();
        fEquipmentShop::RestoreTemporaryShopStock();
        if (aPlayer::GetPlayer() != nullptr) {
            SavedSlots = TemporaryShopSlots;
            TemporaryShopSlots = nullptr;
            aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnEnteringForm, nullptr, nullptr, 0);
            TemporaryShopSlots = SavedSlots;
        }
        fEquipmentShop::BuildTemporaryShopSlotGrid();
        BuildGoodsControls();
        UpdateScrollButtons();
        PreviewSlot = nullptr;
        if (OpenPreviewTimer != nullptr) {
            CancelCallbackTimer(OpenPreviewTimer);
            OpenPreviewTimer = nullptr;
        }
        if (GlobalsV::AnimItem) {
            OpenPreviewTimer = ScheduleCallbackTimer(100, 99999, pas::bind_method<&TfEquipmentShop::ChooseAnimatedPreview>(this), 0);
        }
        MainPanel->RebuildMessageButtons(false);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(190);
    }

    void TfEquipmentShop::OnClose() {
        TShopSlot* Slot{};
        std::int32_t I{};
        std::int32_t Count{};
        aGalaxy::Galaxy->CheckIntegrityChecksum(191);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnLeavingForm, nullptr, nullptr, 0);
        }
        if (TemporaryShopSlots != nullptr) {
            Count = pas::list_count(TemporaryShopSlots);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
                if (Slot->SlotImage != nullptr) {
                    pas::free(Slot->SlotImage);
                    Slot->SlotImage = nullptr;
                }
                if (Slot->BorderImage != nullptr) {
                    pas::free(Slot->BorderImage);
                    Slot->BorderImage = nullptr;
                }
                if (Slot->TypeOverlayImage != nullptr) {
                    pas::free(Slot->TypeOverlayImage);
                    Slot->TypeOverlayImage = nullptr;
                }
                if (Slot->ItemIconImage != nullptr) {
                    pas::free(Slot->ItemIconImage);
                    Slot->ItemIconImage = nullptr;
                }
                if (Slot->ItemAnimation != nullptr) {
                    pas::free(Slot->ItemAnimation);
                    Slot->ItemAnimation = nullptr;
                }
                if (Slot->MicroModuleImage != nullptr) {
                    pas::free(Slot->MicroModuleImage);
                    Slot->MicroModuleImage = nullptr;
                }
            }
        }
        if (OpenPreviewTimer != nullptr) {
            CancelCallbackTimer(OpenPreviewTimer);
            OpenPreviewTimer = nullptr;
        }
        if (ScrollTimer != nullptr) {
            CancelCallbackTimer(ScrollTimer);
            ScrollTimer = nullptr;
        }
        if (ItemInfoTimer != nullptr) {
            CancelCallbackTimer(ItemInfoTimer);
            ItemInfoTimer = nullptr;
        }
        fEquipmentShop::RestoreTemporaryShopStock();
        MainPanel->OnClose();
        LoadPanel->OnClose();
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsOnPlanet()) {
            fPanelPlanet::TfPanelPlanet::OnClose();
        } else {
            fPanelRuins::TfPanelRuins::OnClose();
        }
    }

    void TfEquipmentShop::BuildGoodsControls() {
        TShopSlot* Slot{};
        std::int32_t I{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t Cost{};
        pas::WideString LevelSuffix{};
        UnknownEC = 0;
        ContentColumnCount = 0;
        std::int32_t Count = pas::list_count(TemporaryShopSlots);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
            if (ContentColumnCount <= Slot->GridPoint.X) {
                ContentColumnCount = Slot->GridPoint.X + 1;
            }
        }
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelGoods"sv));
        float CellWidth = pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount);
        float CellHeight = pas::real_divide(Panel->ClientSize.Y, ShopGridRowCount);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, ContentColumnCount - 1); cpp_range_2.next(I); ) {
            GI_Image::TImageGI* cpp_with = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
            cpp_with->SetPositionModeW(true);
            cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Line"}));
            cpp_with->SetSize(cpp_with->GetContentSize());
            cpp_with->SetOrigin(ClassesImports::Point(cpp_with->ClientSize.X / 2, 0));
            cpp_with->SetPosition(ClassesImports::Point(System::Round(static_cast<long double>(I) * CellWidth), Panel->ClientSize.Y / 2 - cpp_with->ClientSize.Y / 2));
            cpp_with->SetDepth(4.0);
            cpp_with->UserData = 1;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, ShopGridRowCount - 1); cpp_range_3.next(Y); ) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, ContentColumnCount - 1); cpp_range_4.next(X); ) {
                Slot = fEquipmentShop::FindShopSlotByGridPoint(ClassesImports::Point(X, Y));
                Slot->SlotImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                {
                    GI_Image::TImageGI* cpp_with_2 = Slot->SlotImage;
                    cpp_with_2->SetPositionModeW(true);
                    {
                        std::int32_t round = System::Round(static_cast<long double>(Y) * CellHeight);
                        std::int32_t round_2 = System::Round(static_cast<long double>(X) * CellWidth);
                        cpp_with_2->SetPosition(ClassesImports::Point(round_2, round));
                    }
                    {
                        std::int32_t trunc = System::Trunc(CellHeight);
                        std::int32_t trunc_2 = System::Trunc(CellWidth);
                        cpp_with_2->SetSize(ClassesImports::Point(trunc_2, trunc));
                    }
                    cpp_with_2->SetDepth(3.0);
                    cpp_with_2->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Slot));
                    if (Slot->Item == nullptr) {
                        cpp_with_2->SetActive(false);
                    } else {
                        if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Slot->Item)) {
                            weapon->Target = nullptr;
                        }
                        cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotN"}));
                        cpp_with_2->MouseEnterCallback = pas::bind_method<&TfEquipmentShop::ItemMouseEnter>(this);
                        cpp_with_2->MouseLeaveCallback = pas::bind_method<&TfEquipmentShop::ItemMouseLeave>(this);
                        cpp_with_2->LeftButtonUpCallback = pas::bind_method<&TfEquipmentShop::ItemMouseUp>(this);
                    }
                    cpp_with_2->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_2->SetImageKindY(GI_Main::ikyCenter);
                }
                Slot->BorderImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                {
                    GI_Image::TImageGI* cpp_with_3 = Slot->BorderImage;
                    cpp_with_3->SetPositionModeW(true);
                    {
                        std::int32_t round_3 = System::Round(static_cast<long double>(Y) * CellHeight);
                        std::int32_t round_4 = System::Round(static_cast<long double>(X) * CellWidth);
                        cpp_with_3->SetPosition(ClassesImports::Point(round_4, round_3));
                    }
                    {
                        std::int32_t trunc_3 = System::Trunc(CellHeight);
                        std::int32_t trunc_4 = System::Trunc(CellWidth);
                        cpp_with_3->SetSize(ClassesImports::Point(trunc_4, trunc_3));
                    }
                    cpp_with_3->SetDepth(3.0);
                    if (Slot->Item == nullptr) {
                        cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotBorderN"}));
                    } else {
                        Cost = Slot->Item->GetConditionAdjustedCost();
                        if (pas::class_cast_if<aItem::THull*>(Slot->Item) != nullptr) {
                            Cost = std::max<std::int32_t>(1, Cost - aPlayer::GetPlayer()->GetHull()->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false)));
                        }
                        if (aPlayer::GetPlayer()->Money < Cost || Slot->Item->ItemType != aConst::t_Hull && aPlayer::GetPlayer()->GetCargoFreeSpace() < Slot->Item->Weight) {
                            cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotBorderH"}));
                        } else {
                            cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotBorderN"}));
                        }
                    }
                    cpp_with_3->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_3->SetImageKindY(GI_Main::ikyCenter);
                }
                if (Slot->Item != nullptr) {
                    Slot->TypeOverlayImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                    {
                        GI_Image::TImageGI* cpp_with_4 = Slot->TypeOverlayImage;
                        cpp_with_4->SetPositionModeW(true);
                        {
                            std::int32_t round_5 = System::Round(static_cast<long double>(Y) * CellHeight);
                            std::int32_t round_6 = System::Round(static_cast<long double>(X) * CellWidth);
                            cpp_with_4->SetPosition(ClassesImports::Point(round_6, round_5));
                        }
                        {
                            std::int32_t trunc_5 = System::Trunc(CellHeight);
                            std::int32_t trunc_6 = System::Trunc(CellWidth);
                            cpp_with_4->SetSize(ClassesImports::Point(trunc_6, trunc_5));
                        }
                        cpp_with_4->SetDepth(3.0);
                        if (reinterpret_cast<aItem::TEquipment*>(Slot->Item)->GetLevel() != 0) {
                            LevelSuffix = pas::concat_wide({u"_", EC_Str::IntToWideString(reinterpret_cast<aItem::TEquipment*>(Slot->Item)->GetLevel())});
                        } else if (aItem::TMicroModule* microModule = pas::class_cast_if<aItem::TMicroModule*>(Slot->Item)) {
                            LevelSuffix = pas::concat_wide({u"_", EC_Str::IntToWideString(aItem::GetMicroModulePriorityColorTier(static_cast<aItem::TEquipment*>(microModule)->MicroModuleIndex - 1))});
                        } else {
                            LevelSuffix = pas::WideString();
                        }
                        if (reinterpret_cast<aItem::TEquipment*>(Slot->Item)->ConfigBlockName != u"" && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({u"Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Slot", reinterpret_cast<aItem::TEquipment*>(Slot->Item)->ConfigBlockName, LevelSuffix}))) {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Slot", reinterpret_cast<aItem::TEquipment*>(Slot->Item)->ConfigBlockName, LevelSuffix}));
                        } else if (aItem::TMicroModule* microModule_2 = pas::class_cast_if<aItem::TMicroModule*>(Slot->Item); microModule_2 != nullptr && aConst::MicroModuleTemplates[static_cast<aItem::TEquipment*>(microModule_2)->MicroModuleIndex - 1].KindGraph != u"" && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({u"Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Slot", aConst::MicroModuleTemplates[static_cast<aItem::TEquipment*>(microModule_2)->MicroModuleIndex - 1].KindGraph, LevelSuffix}))) {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Slot", aConst::MicroModuleTemplates[reinterpret_cast<aItem::TEquipment*>(Slot->Item)->MicroModuleIndex - 1].KindGraph, LevelSuffix}));
                        } else if (GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({u"Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Slot", aConst::ItemTypeNames[Slot->Item->ItemType], LevelSuffix}))) {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"Slot", aConst::ItemTypeNames[Slot->Item->ItemType], LevelSuffix}));
                        } else if (pas::class_cast_if<aItem::TArtefact*>(Slot->Item) != nullptr && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({u"Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotArtefact"}))) {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotArtefact"}));
                        } else if (pas::class_cast_if<aItem::TWeapon*>(Slot->Item) != nullptr && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({u"Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotWeapon", LevelSuffix}))) {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotWeapon", LevelSuffix}));
                        } else {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotLevel", EC_Str::IntToWideString(reinterpret_cast<aItem::TEquipment*>(Slot->Item)->GetLevel())}));
                        }
                        cpp_with_4->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_4->SetImageKindY(GI_Main::ikyCenter);
                    }
                }
                if (Slot->Item != nullptr) {
                    Slot->ItemIconImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                    {
                        GI_Image::TImageGI* cpp_with_5 = Slot->ItemIconImage;
                        cpp_with_5->SetPositionModeW(true);
                        {
                            std::int32_t round_7 = System::Round(static_cast<long double>(Y) * CellHeight);
                            std::int32_t round_8 = System::Round(static_cast<long double>(X) * CellWidth);
                            cpp_with_5->SetPosition(ClassesImports::Point(round_8, round_7));
                        }
                        {
                            std::int32_t cpp_arg = System::Trunc(CellWidth) - GR_Main::GiScalePixels(10);
                            std::int32_t trunc_7 = System::Trunc(CellHeight);
                            cpp_with_5->SetSize(ClassesImports::Point(cpp_arg, trunc_7));
                        }
                        cpp_with_5->SetDepth(2.0);
                        cpp_with_5->SetActive(true);
                        cpp_with_5->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Slot->Item), u"i"}));
                        cpp_with_5->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_5->SetImageKindY(GI_Main::ikyCenter);
                        cpp_with_5->SetActive(true);
                    }
                    if (GlobalsV::AnimItem) {
                        Slot->ItemAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Panel);
                        {
                            GI_GAI::TgaiGI* cpp_with_6 = Slot->ItemAnimation;
                            cpp_with_6->SetPositionModeW(true);
                            {
                                std::int32_t round_9 = System::Round(static_cast<long double>(Y) * CellHeight);
                                std::int32_t round_10 = System::Round(static_cast<long double>(X) * CellWidth);
                                cpp_with_6->SetPosition(ClassesImports::Point(round_10, round_9));
                            }
                            {
                                std::int32_t cpp_arg_2 = System::Trunc(CellWidth) - GR_Main::GiScalePixels(10);
                                std::int32_t trunc_8 = System::Trunc(CellHeight);
                                cpp_with_6->SetSize(ClassesImports::Point(cpp_arg_2, trunc_8));
                            }
                            cpp_with_6->SetDepth(1.0);
                            cpp_with_6->SetImagePath(pas::concat_wide({fEquipmentShop::GetShopItemIconName(Slot->Item), u"a"}));
                            cpp_with_6->SequenceIndex = 0;
                            cpp_with_6->SetActive(false);
                        }
                    }
                }
                if (Slot->Item != nullptr && reinterpret_cast<aItem::TEquipment*>(Slot->Item)->HasMicroModule() && pas::in_range(Slot->Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    Slot->MicroModuleImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                    {
                        GI_Image::TImageGI* cpp_with_7 = Slot->MicroModuleImage;
                        cpp_with_7->SetImagePath(pas::concat_wide({u"GI,", aItem::GetMicroModuleBitmapResourceName(reinterpret_cast<aItem::TEquipment*>(Slot->Item)->MicroModuleIndex - 1), u"Set"}));
                        cpp_with_7->SetPositionModeW(true);
                        {
                            std::int32_t cpp_arg_3 = System::Round(static_cast<long double>(Y) * CellHeight) + GR_Main::GiScalePixels(45);
                            std::int32_t cpp_arg_4 = System::Round(static_cast<long double>(X) * CellWidth) + GR_Main::GiScalePixels(5);
                            cpp_with_7->SetPosition(ClassesImports::Point(cpp_arg_4, cpp_arg_3));
                        }
                        cpp_with_7->SetSize(cpp_with_7->GetContentSize());
                        cpp_with_7->SetDepth(0.0);
                        cpp_with_7->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_7->SetImageKindY(GI_Main::ikyCenter);
                        cpp_with_7->SetActive(true);
                    }
                }
            }
        }
        pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(Panel)->UpdateScrollRanges();
        Panel->ScrollChangedCallback = pas::bind_method<&TfEquipmentShop::PanelScrollChanged>(this);
        TargetScrollX = 0;
        Panel->SetScrollOffset(ClassesImports::Point(0, 0));
    }

    void TfEquipmentShop::ClearGoodsControls() {
        TShopSlot* Slot{};
        std::int32_t I{};
        std::int32_t Count{};
        GI_MessageLoop::TObjectGI* Current{};
        if (TemporaryShopSlots != nullptr) {
            Count = pas::list_count(TemporaryShopSlots);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
                if (Slot->SlotImage != nullptr) {
                    pas::free(Slot->SlotImage);
                    Slot->SlotImage = nullptr;
                }
                if (Slot->BorderImage != nullptr) {
                    pas::free(Slot->BorderImage);
                    Slot->BorderImage = nullptr;
                }
                if (Slot->TypeOverlayImage != nullptr) {
                    pas::free(Slot->TypeOverlayImage);
                    Slot->TypeOverlayImage = nullptr;
                }
                if (Slot->ItemIconImage != nullptr) {
                    pas::free(Slot->ItemIconImage);
                    Slot->ItemIconImage = nullptr;
                }
                if (Slot->ItemAnimation != nullptr) {
                    pas::free(Slot->ItemAnimation);
                    Slot->ItemAnimation = nullptr;
                }
                if (Slot->MicroModuleImage != nullptr) {
                    pas::free(Slot->MicroModuleImage);
                    Slot->MicroModuleImage = nullptr;
                }
            }
        }
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelGoods"sv));
        GI_MessageLoop::TObjectGI* Child = Panel->FirstChild;
        while (Child != nullptr) {
            Current = Child;
            Child = Child->NextSibling;
            if (Current->UserData == 1) {
                pas::free(Current);
            }
        }
    }

    void TfEquipmentShop::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstDominion && aPlayer::GetPlayer()->DockedTo->Order == aShip::soTeleport && static_cast<std::uint32_t>(aPlayer::GetPlayer()->DockedTo->OrderStateData) > 0 && static_cast<std::uint8_t>(aPlayer::GetPlayer()->DockedTo->InHyperspace ^ 1)) {
            Globals::RuinsTalkScreen->DepartWithStation(1);
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstDominion && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != aPlayer::GetPlayer()->CurrentStar && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate <= aGalaxy::Galaxy->CurrentTurn) {
            Globals::RuinsTalkScreen->DepartWithStation(1);
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstMilitaryBase && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != aPlayer::GetPlayer()->CurrentStar && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate <= aGalaxy::Galaxy->CurrentTurn) {
            if (aPlayer::GetPlayer()->Speed <= 0) {
                Globals::RuinsTalkScreen->DepartWithStation(1);
                return;
            }
            StationPanel->TakeOffForStationTravel();
        } else {
            RefreshItemInfo(nullptr);
            aGalaxy::Galaxy->CheckIntegrityChecksum(192);
            fEquipmentShop::RestoreTemporaryShopStock();
            MainPanel->EndTurnClicked(Sender);
            MainPanel->RebuildMessageButtons(false);
            if (ExitCode == 0) {
                fEquipmentShop::BuildTemporaryShopSlotGrid();
                aGalaxy::Galaxy->PrimeIntegrityChecksum(193);
                ClearGoodsControls();
                BuildGoodsControls();
                UpdateScrollButtons();
                if (OpenPreviewTimer != nullptr) {
                    CancelCallbackTimer(OpenPreviewTimer);
                    OpenPreviewTimer = nullptr;
                }
                if (GlobalsV::AnimItem) {
                    OpenPreviewTimer = ScheduleCallbackTimer(aMyFunction::RandomIntRange(3000, 6000), 99999, pas::bind_method<&TfEquipmentShop::ChooseAnimatedPreview>(this), 0);
                }
            }
        }
    }

    void TfEquipmentShop::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode != 0) {
            return;
        }
        HideItemInfo(nullptr, 0);
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        Globals::ShipScreen->PlayTransitionSounds = true;
        aGalaxy::Galaxy->CheckIntegrityChecksum(194);
        while (true) {
            fShip2::RunShipEquipment(this);
            MainPanel->RefreshMoneyAndCargo();
            MainPanel->RebuildMessageButtons(false);
            ClearGoodsControls();
            BuildGoodsControls();
            UpdateScrollButtons();
            if (!Globals::ShipScreen->ReopenRequested) {
                break;
            }
            SetCursorActive(false);
            DrawQueuedUpdateRects();
            GR_Main::CaptureScreenBackground(true, 0);
            SetCursorActive(true);
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(195);
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
                GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
                RequestClose(1);
            }
        }
    }

    void TfEquipmentShop::UpdateScrollButtons() {
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelGoods"sv));
        {
            GI_GraphButton::TGraphButtonGI* Left = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Left"sv));
            Left->SetDisabled(Panel->ScrollOffset.X <= 0);
        }
        {
            GI_GraphButton::TGraphButtonGI* Right = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Right"sv));
            {
                pas::Extended cpp_left = pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount) * ContentColumnCount;
                Right->SetDisabled(Panel->ScrollOffset.X >= System::Round(cpp_left - pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount) * ShopVisibleColumnCount) - 1);
            }
        }
    }

    void TfEquipmentShop::StartSlotAnimatedPreview(TShopSlot* Slot) {
        if (Slot->ItemAnimation != nullptr) {
            if (!Slot->ItemAnimation->Active) {
                Slot->ItemAnimation->UpdateAutoGeometry();
                Slot->ItemAnimation->SetImageKindX(GI_Main::ikxCenter);
                Slot->ItemAnimation->SetImageKindY(GI_Main::ikyCenter);
                Slot->ItemAnimation->SetActive(true);
            }
            Slot->ItemAnimation->RestartPlayback();
            if (Slot->ItemIconImage != nullptr) {
                pas::free(Slot->ItemIconImage);
                Slot->ItemIconImage = nullptr;
            }
        }
    }

    // Playback continues until the current animation cycle ends.
    void TfEquipmentShop::ScheduleSlotPreviewStop(TShopSlot* Slot) {
        if (Slot->ItemAnimation != nullptr) {
            Slot->ItemAnimation->CycleCompleteCallback = pas::bind_static_method<&TfEquipmentShop::PreviewCycleComplete>(this);
        }
    }

    void TfEquipmentShop::ItemMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        TShopSlot* Slot = reinterpret_cast<TShopSlot*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        Slot->SlotImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotA"}));
        Slot->SlotImage->SetImageKindX(GI_Main::ikxCenter);
        Slot->SlotImage->SetImageKindY(GI_Main::ikyCenter);
        TfEquipmentShop::StartSlotAnimatedPreview(Slot);
        GI_MessageLoop::TObjectGI* Panel = GetByName(u"PanelShop"sv);
        ItemInfoAnchor = ClassesImports::Point(Sender->HitTestBounds.Left + Sender->ClientSize.X / 2, std::min<std::int32_t>(Sender->HitTestBounds.Bottom, Panel->HitTestBounds.Bottom - Sender->ClientSize.Y - 24));
        RefreshItemInfo(Slot->Item);
        if (!IsCursorImageSelected(u"Take"sv)) {
            SetCursorByName(u"Take"_wref.get());
        }
    }

    void TfEquipmentShop::ItemMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        TShopSlot* Slot = reinterpret_cast<TShopSlot*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        Slot->SlotImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotN"}));
        Slot->SlotImage->SetImageKindX(GI_Main::ikxCenter);
        Slot->SlotImage->SetImageKindY(GI_Main::ikyCenter);
        ScheduleSlotPreviewStop(Slot);
        RefreshItemInfo(nullptr);
        if (!IsCursorImageSelected(u"Main"sv)) {
            SetCursorByName(u"Main"_wref.get());
        }
    }

    void TfEquipmentShop::ScrollLeft(GI_MessageLoop::TObjectGI* Sender) {
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelGoods"sv));
        pas::Extended cpp_left = Panel->ScrollOffset.X - pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount);
        std::int32_t Column = System::Round(pas::real_divide(cpp_left, pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount)));
        if (Column < 0) {
            Column = 0;
        }
        TargetScrollX = System::Round(pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount) * Column);
        if (ScrollTimer == nullptr && TargetScrollX != Panel->ScrollOffset.X) {
            ScrollTimer = ScheduleCallbackTimer(0, 20, pas::bind_method<&TfEquipmentShop::ScrollTick>(this), 0);
        }
        UpdateScrollButtons();
    }

    void TfEquipmentShop::ScrollRight(GI_MessageLoop::TObjectGI* Sender) {
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelGoods"sv));
        pas::Extended cpp_left = Panel->ScrollOffset.X + pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount);
        std::int32_t Column = System::Round(pas::real_divide(cpp_left, pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount)));
        if (Column > ContentColumnCount - ShopVisibleColumnCount) {
            Column = std::max<std::int32_t>(ContentColumnCount - ShopVisibleColumnCount, 0);
        }
        TargetScrollX = System::Round(pas::real_divide(Panel->ClientSize.X, ShopVisibleColumnCount) * Column);
        if (ScrollTimer == nullptr && TargetScrollX != Panel->ScrollOffset.X) {
            ScrollTimer = ScheduleCallbackTimer(0, 20, pas::bind_method<&TfEquipmentShop::ScrollTick>(this), 0);
        }
        UpdateScrollButtons();
    }

    void TfEquipmentShop::ScrollTick(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelGoods"sv));
        if (Panel->ScrollOffset.X == TargetScrollX) {
            if (ScrollTimer != nullptr) {
                CancelCallbackTimer(ScrollTimer);
                ScrollTimer = nullptr;
            }
        } else if (TargetScrollX < Panel->ScrollOffset.X) {
            Panel->SetScrollOffset(ClassesImports::Point(std::max<std::int32_t>(TargetScrollX, Panel->ScrollOffset.X - 12), 0));
        } else {
            Panel->SetScrollOffset(ClassesImports::Point(std::min<std::int32_t>(TargetScrollX, Panel->ScrollOffset.X + 12), 0));
        }
        UpdateScrollButtons();
    }

    void TfEquipmentShop::PanelScrollChanged(GI_MessageLoop::TObjectGI* Sender) {
        pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelGoods"sv))->PanelScrollChanged(Sender);
        UpdateScrollButtons();
    }

    void TfEquipmentShop::ItemMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        TShopSlot* Slot{};
        std::int32_t Y{};
        std::int32_t X{};
        std::int32_t Cost{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        aItem::TItem* PurchasedItem{};
        std::int32_t Destination{};
        Slot = reinterpret_cast<TShopSlot*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        if (Slot != nullptr && Slot->Item != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(198);
            aGalaxy::Galaxy->PendingEquipmentPurchasePrice = Slot->Item->GetConditionAdjustedCost();
            aPlayer::GetPlayer()->GetCargoFreeSpace();
            if (pas::class_cast_if<aItem::THull*>(Slot->Item) != nullptr && aPlayer::GetPlayer()->Money < aGalaxy::Galaxy->PendingEquipmentPurchasePrice && aPlayer::GetPlayer()->GetHull()->ScriptItem == nullptr && aPlayer::GetPlayer()->GetHull()->NoDropFlag == 0) {
                aGalaxy::Galaxy->PendingEquipmentPurchasePrice = std::max<std::int32_t>(1, aGalaxy::Galaxy->PendingEquipmentPurchasePrice - aPlayer::GetPlayer()->GetHull()->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false)));
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(199);
            if (aPlayer::GetPlayer()->Money < aGalaxy::Galaxy->PendingEquipmentPurchasePrice) {
                GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                if (pas::class_cast_if<aItem::THull*>(Slot->Item) != nullptr) {
                    GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                        pas::WideString intToWideString = EC_Str::IntToWideString(aGalaxy::Galaxy->PendingEquipmentPurchasePrice - aPlayer::GetPlayer()->Money);
                        pas::WideString paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormShop.NoMoney"_wref.get());
                        pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText1(std::move(paramByPathOrMarker), std::move(textHighlightColorTag), u"<Money>"_w, std::move(intToWideString));
                    }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
                }
                aGalaxy::Galaxy->CheckIntegrityChecksum(309);
                MainPanel->FlashMoneyWarning();
                if (!(pas::class_cast_if<aItem::THull*>(Slot->Item) != nullptr) && aPlayer::GetPlayer()->CargoFreeSpace < Slot->Item->Weight) {
                    MainPanel->FlashCargoWarning();
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(304);
            } else if (!(pas::class_cast_if<aItem::THull*>(Slot->Item) != nullptr) && aPlayer::GetPlayer()->CargoFreeSpace < Slot->Item->Weight) {
                GR_Main::SoundManager->PlaySound(u"Sound.NoSize"_wref.get());
                MainPanel->FlashCargoWarning();
                aGalaxy::Galaxy->PrimeIntegrityChecksum(305);
            } else {
                if (pas::class_cast_if<aItem::THull*>(Slot->Item) != nullptr) {
                    if (Slot->Item->GetConditionAdjustedCost() <= aPlayer::GetPlayer()->Money) {
                        if (GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                            pas::WideString intToWideString_2 = EC_Str::IntToWideString(aGalaxy::Galaxy->PendingEquipmentPurchasePrice);
                            pas::WideString paramByPathOrMarker_2 = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormShop.BuyHull"_wref.get());
                            pas::WideString textHighlightColorTag_2 = aMyFunction::TextHighlightColorTag;
                            return aMyFunction::FormatText1(std::move(paramByPathOrMarker_2), std::move(textHighlightColorTag_2), u"<Money>"_w, std::move(intToWideString_2));
                        }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
                            return;
                        }
                    } else if (GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                        pas::WideString intToWideString_3 = EC_Str::IntToWideString(aGalaxy::Galaxy->PendingEquipmentPurchasePrice);
                        pas::WideString paramByPathOrMarker_3 = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormShop.UpgradeHull"_wref.get());
                        pas::WideString textHighlightColorTag_3 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText1(std::move(paramByPathOrMarker_3), std::move(textHighlightColorTag_3), u"<Money>"_w, std::move(intToWideString_3));
                    }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
                        return;
                    }
                } else if (GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString removeTextTagsW = EC_Str::RemoveTextTagsW(Slot->Item->GetDisplayName());
                    pas::WideString intToWideString_4 = EC_Str::IntToWideString(aGalaxy::Galaxy->PendingEquipmentPurchasePrice);
                    pas::WideString paramByPathOrMarker_4 = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormShop.Buy"_wref.get());
                    pas::WideString textHighlightColorTag_4 = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText2(std::move(paramByPathOrMarker_4), std::move(textHighlightColorTag_4), u"<Item>"_w, std::move(removeTextTagsW), u"<Money>"_w, std::move(intToWideString_4));
                }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
                    return;
                }
                GR_Main::SoundManager->PlaySound(u"Sound.Buy"_wref.get());
                aGalaxy::Galaxy->CheckIntegrityChecksum(196);
                PurchasedItem = Slot->Item;
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerBuysEquipment"_w, nullptr);
                Event->AddData(Slot->Item->ItemType);
                Event->AddData(Slot->Item->GetConditionAdjustedCost());
                Event->AddData(Slot->Item->Weight);
                Event->AddData(Slot->Item->Id);
                if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                    Event->AddData(0);
                    Event->AddData(aPlayer::GetPlayer()->CurrentPlanet->Id);
                } else if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                    Event->AddData(1);
                    Event->AddData(aPlayer::GetPlayer()->DockedTo->Id);
                } else {
                    Event->AddData(2);
                    Event->AddData(0);
                }
                Event->AddTextData(Slot->Item->GetDisplayName());
                Event->AddTextData(Slot->Item->GetCategoryConfigName());
                if (pas::class_cast_if<aItem::THull*>(Slot->Item) != nullptr) {
                    if (Slot->Item->GetConditionAdjustedCost() > aPlayer::GetPlayer()->Money) {
                        Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerSellsEquipment"_w, nullptr);
                        Event->AddData(aPlayer::GetPlayer()->GetHull()->ItemType);
                        Event->AddData(aPlayer::GetPlayer()->GetHull()->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false)));
                        Event->AddData(aPlayer::GetPlayer()->GetHull()->Weight);
                        Event->AddData(aPlayer::GetPlayer()->GetHull()->Id);
                        if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                            Event->AddData(0);
                            Event->AddData(aPlayer::GetPlayer()->CurrentPlanet->Id);
                        } else if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                            Event->AddData(1);
                            Event->AddData(aPlayer::GetPlayer()->DockedTo->Id);
                        } else {
                            Event->AddData(2);
                            Event->AddData(0);
                        }
                        Event->AddTextData(aPlayer::GetPlayer()->GetHull()->GetDisplayName());
                        Event->AddTextData(aPlayer::GetPlayer()->GetHull()->GetCategoryConfigName());
                        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - aGalaxy::Galaxy->PendingEquipmentPurchasePrice);
                        aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnPlayerChangeHull, Slot->Item, aPlayer::GetPlayer()->GetHull(), 0);
                        pas::list_delete(aPlayer::GetPlayer()->Inventory, pas::list_indexof(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(aPlayer::GetPlayer()->GetHull())));
                        pas::free(aPlayer::GetPlayer()->GetHull());
                        pas::list_insert(aPlayer::GetPlayer()->Inventory, 0, reinterpret_cast<void*>(Slot->Item));
                        aPlayer::GetPlayer()->Hull = pas::checked_cast<aItem::THull*>(Slot->Item);
                        aPlayer::GetPlayer()->GetHull()->OwnerShip = aPlayer::GetPlayer();
                        aPlayer::GetPlayer()->GetHull()->AssignedSlotData = 0u;
                        if (Slot->Item->ScriptItem != nullptr) {
                            reinterpret_cast<aScript::TScriptItem*>(Slot->Item->ScriptItem)->RunActionCode(aGalaxyStruct::satOnPlayerChangeHull, aPlayer::GetPlayer(), Slot->Item, nullptr, 0);
                        }
                        Slot->Item = nullptr;
                        aPlayer::GetPlayer()->RefreshAssignedItemSlots();
                        aPlayer::GetPlayer()->RebuildEquipmentCache();
                        aPlayer::GetPlayer()->RefreshDerivedStats(true);
                        if (!aPlayer::GetPlayer()->ScriptChameleon) {
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&aPlayer::GetPlayer()->Graphic));
                            aShip::TShip_RefreshGraphic(aPlayer::GetPlayer());
                        }
                        Destination = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(aPlayer::GetPlayer()));
                    } else {
                        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Slot->Item->GetConditionAdjustedCost());
                        if (aPlayer::GetPlayer()->IsOnPlanet()) {
                            aPlayer::GetPlayer()->AddItemToPlayerStorage(Slot->Item, aPlayer::GetPlayer()->CurrentPlanet, -1);
                        } else {
                            aPlayer::GetPlayer()->AddItemToPlayerStorage(Slot->Item, aPlayer::GetPlayer()->DockedTo, -1);
                        }
                        Slot->Item = nullptr;
                        GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormShop.AfterBuyHull"_wref.get()), GI_MessageBox::mbgOK, 0, 0, 0);
                        if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                            Destination = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(aPlayer::GetPlayer()->CurrentPlanet));
                        } else {
                            Destination = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(aPlayer::GetPlayer()->DockedTo));
                        }
                    }
                } else {
                    aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - aGalaxy::Galaxy->PendingEquipmentPurchasePrice);
                    reinterpret_cast<aItem::TEquipment*>(Slot->Item)->EquippedFlag = 0;
                    if (pas::class_cast_if<aItem::TArtefact*>(Slot->Item) != nullptr) {
                        pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Slot->Item));
                    } else {
                        pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Slot->Item));
                    }
                    Slot->Item = nullptr;
                    Destination = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(aPlayer::GetPlayer()));
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnPlayerBuyEq, PurchasedItem, nullptr, Destination);
                aPlayer::GetPlayer()->RefreshDerivedStats(true);
                aGalaxy::Galaxy->PrimeIntegrityChecksum(197);
                {
                    GI_Image::TImageGI* cpp_with = Slot->SlotImage;
                    cpp_with->SetActive(false);
                    cpp_with->MouseEnterCallback = nullptr;
                    cpp_with->MouseLeaveCallback = nullptr;
                    cpp_with->LeftButtonUpCallback = nullptr;
                }
                {
                    GI_Image::TImageGI* cpp_with_2 = Slot->BorderImage;
                    cpp_with_2->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotBorderN"}));
                    cpp_with_2->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_2->SetImageKindY(GI_Main::ikyCenter);
                }
                if (Slot->TypeOverlayImage != nullptr) {
                    pas::free(Slot->TypeOverlayImage);
                    Slot->TypeOverlayImage = nullptr;
                }
                if (Slot->ItemIconImage != nullptr) {
                    pas::free(Slot->ItemIconImage);
                    Slot->ItemIconImage = nullptr;
                }
                if (Slot->ItemAnimation != nullptr) {
                    pas::free(Slot->ItemAnimation);
                    Slot->ItemAnimation = nullptr;
                }
                if (Slot->MicroModuleImage != nullptr) {
                    pas::free(Slot->MicroModuleImage);
                    Slot->MicroModuleImage = nullptr;
                }
                RefreshItemInfo(nullptr);
                if (!IsCursorImageSelected(u"Main"sv)) {
                    SetCursorByName(u"Main"_wref.get());
                }
                for (auto cpp_range = pas::for_to<std::int32_t>(0, ShopGridRowCount - 1); cpp_range.next(Y); ) {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ContentColumnCount - 1); cpp_range_2.next(X); ) {
                        Slot = fEquipmentShop::FindShopSlotByGridPoint(ClassesImports::Point(X, Y));
                        {
                            GI_Image::TImageGI* cpp_with_3 = Slot->BorderImage;
                            if (Slot->Item == nullptr) {
                                cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotBorderN"}));
                            } else {
                                Cost = Slot->Item->GetConditionAdjustedCost();
                                if (pas::class_cast_if<aItem::THull*>(Slot->Item) != nullptr) {
                                    Cost = std::max<std::int32_t>(1, Cost - aPlayer::GetPlayer()->GetHull()->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false)));
                                }
                                if (aPlayer::GetPlayer()->Money < Cost || Slot->Item->ItemType != aConst::t_Hull && aPlayer::GetPlayer()->CargoFreeSpace < Slot->Item->Weight) {
                                    cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotBorderH"}));
                                } else {
                                    cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.FormShop2.", GR_Main::GiResourceSuffix(), u"SlotBorderN"}));
                                }
                            }
                            cpp_with_3->SetImageKindX(GI_Main::ikxCenter);
                            cpp_with_3->SetImageKindY(GI_Main::ikyCenter);
                        }
                    }
                }
            }
        }
    }

    void TfEquipmentShop::ChooseAnimatedPreview(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t I{};
        TShopSlot* Slot{};
        if (pas::list_indexof(TemporaryShopSlots, reinterpret_cast<void*>(PreviewSlot)) >= 0) {
            ScheduleSlotPreviewStop(PreviewSlot);
        }
        PreviewSlot = nullptr;
        std::int32_t Attempts = 20;
        while (Attempts > 0) {
            I = aMyFunction::RandomIntRange(0, pas::list_count(TemporaryShopSlots) - 1);
            Slot = pas::list_at<TShopSlot>(TemporaryShopSlots, I);
            if (Slot->Item != nullptr) {
                TfEquipmentShop::StartSlotAnimatedPreview(Slot);
                PreviewSlot = Slot;
                break;
            }
            --Attempts;
        }
        if (OpenPreviewTimer != nullptr) {
            CancelCallbackTimer(OpenPreviewTimer);
            OpenPreviewTimer = nullptr;
        }
        if (GlobalsV::AnimItem) {
            OpenPreviewTimer = ScheduleCallbackTimer(aMyFunction::RandomIntRange(3000, 6000), 99999, pas::bind_method<&TfEquipmentShop::ChooseAnimatedPreview>(this), 0);
        }
    }

    void TfEquipmentShop::PreviewCycleComplete(GI_MessageLoop::TObjectGI* Sender) {
        pas::checked_cast<GI_GAI::TgaiGI*>(Sender)->StopAutoPlayback();
    }

    void TfEquipmentShop::HideItemInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (ItemInfoTimer != nullptr) {
            CancelCallbackTimer(ItemInfoTimer);
            ItemInfoTimer = nullptr;
        }
        GetByName(u"PII"sv)->SetActive(false);
        GetByName(u"InfoHull"sv)->SetActive(false);
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"sv))->SetText(u""_wref.get());
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"sv))->SetText(u""_wref.get());
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"sv))->SetText(u""_wref.get());
    }

    void TfEquipmentShop::RefreshItemInfo(aItem::TItem* Item) {
        static const pas::Set<0, 255> DurableTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        aItem::TEquipment* Equipment{};
        pas::WideString Price{};
        std::int32_t BarWidth{};
        std::int32_t CapWidth{};
        std::int32_t MinimumWidth{};
        Equipment = pas::checked_cast<aItem::TEquipment*>(Item);
        if (Equipment == nullptr) {
            if (ItemInfoTimer != nullptr) {
                CancelCallbackTimer(ItemInfoTimer);
                ItemInfoTimer = nullptr;
            }
            ItemInfoTimer = ScheduleCallbackTimer(100, 99999, pas::bind_method<&TfEquipmentShop::HideItemInfo>(this), 0);
            MainPanel->HelpLabel->SetActive(false);
            MainPanel->SlideMessagesIn();
            if (GlobalsV::AnimItem) {
                OpenPreviewTimer = ScheduleCallbackTimer(1000, 99999, pas::bind_method<&TfEquipmentShop::ChooseAnimatedPreview>(this), 0);
            }
        } else {
            if (OpenPreviewTimer != nullptr) {
                CancelCallbackTimer(OpenPreviewTimer);
                OpenPreviewTimer = nullptr;
            }
            if (pas::list_indexof(TemporaryShopSlots, reinterpret_cast<void*>(PreviewSlot)) >= 0) {
                ScheduleSlotPreviewStop(PreviewSlot);
            }
            PreviewSlot = nullptr;
            if (ItemInfoTimer != nullptr) {
                CancelCallbackTimer(ItemInfoTimer);
                ItemInfoTimer = nullptr;
            }
            {
                GI_Label::TLabelGI* cpp_with = MainPanel->HelpLabel;
                cpp_with->SetActive(true);
                cpp_with->SetText(([&] {
                    pas::WideString displayName = Equipment->GetDisplayName();
                    pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"FormShop.BuyHelp"_wref.get());
                    pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                    return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey), std::move(infoNameColorTag), u"<Item>"_w, std::move(displayName));
                }()));
            }
            MainPanel->SlideMessagesOut();
            if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
                if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                    if (Item->ScriptItem != nullptr) {
                        reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aGalaxyStruct::satOnShowingItemInfo, nullptr, aPlayer::GetPlayer()->CurrentPlanet, nullptr, 0);
                    }
                    if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
                        aScript::RunItemConfigActionCode(Item, aGalaxyStruct::satOnShowingItemInfo, nullptr, aPlayer::GetPlayer()->CurrentPlanet, nullptr, 0);
                    }
                } else {
                    if (Item->ScriptItem != nullptr) {
                        reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aGalaxyStruct::satOnShowingItemInfo, nullptr, aPlayer::GetPlayer()->DockedTo, nullptr, 0);
                    }
                    if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
                        aScript::RunItemConfigActionCode(Item, aGalaxyStruct::satOnShowingItemInfo, nullptr, aPlayer::GetPlayer()->DockedTo, nullptr, 0);
                    }
                }
            }
            if (Item->ItemType == aConst::t_Hull) {
                {
                    pas::WideString infoText = Equipment->virtual_TItem_GetInfoText(aMyFunction::TextHighlightColorTag, nullptr);
                    aItem::THull* cpp_arg = pas::checked_cast<aItem::THull*>(Item);
                    RefreshHullInfo(this, cpp_arg, std::move(infoText), false);
                }
                ItemInfoWindow->SetActive(false);
            } else {
                ItemInfoWindow->SetActive(true);
                GetByName(u"InfoHull"sv)->SetActive(false);
                {
                    GI_Image::TImageGI* InfoImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"sv));
                    InfoImage->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Equipment), u"s"}));
                    InfoImage->SetImageKindX(GI_Main::ikxCenter);
                    InfoImage->SetImageKindY(GI_Main::ikyCenter);
                    {
                        Types::TPoint visualCenter = InfoImage->GetVisualCenter();
                        Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                        InfoImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                    }
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"sv))->SetText(u""_wref.get());
                {
                    const pas::WideString& wrapTextInColor = ([&] {
                        pas::WideString displayName_2 = Equipment->GetDisplayName();
                        pas::WideString infoNameColorTag_2 = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(pas::view(std::move(displayName_2)), pas::view(std::move(infoNameColorTag_2)));
                    }());
                    GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"sv));
                    cpp_arg_2->SetText(wrapTextInColor);
                }
                {
                    const pas::WideString& infoText_2 = Equipment->virtual_TItem_GetInfoText(aMyFunction::TextHighlightColorTag, nullptr);
                    GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"sv));
                    cpp_arg_3->SetText(infoText_2);
                }
                {
                    const pas::WideString& intToWideString = EC_Str::IntToWideString(Equipment->Weight);
                    GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"sv));
                    cpp_arg_4->SetText(intToWideString);
                }
                Price = EC_Str::IntToWideString(Equipment->GetConditionAdjustedCost());
                if (Equipment->GetConditionAdjustedCost() < Equipment->Cost) {
                    Price = aMyFunction::WrapTextInColor(pas::view(Price), pas::view(aMyFunction::RedColorTag));
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"sv))->SetText(Price);
                {
                    GI_Image::TImageGI* EmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"sv));
                    EmRace->SetImagePath(aConst::GetFactionEmblemPath(aItem::TItem_GetOwnerConfigName(Equipment)));
                    EmRace->SetImageKindX(GI_Main::ikxCenter);
                    EmRace->SetImageKindY(GI_Main::ikyCenter);
                }
                if (static_cast<std::uint8_t>(pas::contains(DurableTypes, static_cast<std::uint8_t>(Equipment->ItemType)) ^ 1) && Equipment->ItemType != aConst::t_Hull) {
                    {
                        GI_Image::TImageGI* InfoDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"sv));
                        InfoDurable->Parent->Parent->SetActive(false);
                    }
                    MinimumWidth = 0;
                } else {
                    BarWidth = std::min<std::int64_t>(static_cast<std::int64_t>(192), std::max<std::int64_t>(static_cast<std::int64_t>(32), System::Round(pas::real_divide(64.0L, pas::real_max<float>(0.1f, Equipment->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})))))));
                    {
                        GI_Image::TImageGI* InfoDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableLeft"sv));
                        CapWidth = InfoDurableLeft->GetContentSize().X;
                        MinimumWidth = 2 * CapWidth + BarWidth + InfoDurableLeft->LocalPosition.X + InfoDurableLeft->Parent->LocalPosition.X + 2 * InfoDurableLeft->Parent->Parent->LocalPosition.X;
                    }
                    {
                        GI_Image::TImageGI* InfoDurable_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"sv));
                        InfoDurable_2->Parent->Parent->SetActive(true);
                        InfoDurable_2->Parent->Parent->SetSize(ClassesImports::Point(2 * CapWidth + BarWidth, InfoDurable_2->Parent->Parent->ClientSize.Y));
                        InfoDurable_2->Parent->SetSize(ClassesImports::Point(BarWidth + 2, InfoDurable_2->Parent->Parent->ClientSize.Y));
                        if (Equipment->ItemType == aConst::t_Hull) {
                            pas::Extended cpp_left_2 = pas::checked_cast<aItem::THull*>(Equipment)->HullPoints;
                            std::int64_t cpp_left = System::Round(pas::real_divide(cpp_left_2, pas::checked_cast<aItem::THull*>(Equipment)->Weight) * BarWidth);
                            std::int32_t cpp_arg_5 = cpp_left - (InfoDurable_2->GetContentSize().X - 5);
                            std::int32_t y = InfoDurable_2->LocalPosition.Y;
                            InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_5, y));
                        } else {
                            std::int64_t cpp_left_3 = System::Round(BarWidth * pas::real_divide(Equipment->ConditionPercent, 1.0E+2L));
                            std::int32_t cpp_arg_6 = cpp_left_3 - (InfoDurable_2->GetContentSize().X - 5);
                            std::int32_t y_2 = InfoDurable_2->LocalPosition.Y;
                            InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_6, y_2));
                        }
                    }
                    {
                        GI_Image::TImageGI* InfoDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableRight"sv));
                        {
                            std::int32_t cpp_arg_7 = BarWidth + CapWidth - InfoDurableRight->GetContentSize().X;
                            std::int32_t y_3 = InfoDurableRight->LocalPosition.Y;
                            InfoDurableRight->SetPosition(ClassesImports::Point(cpp_arg_7, y_3));
                        }
                        InfoDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoDurableRight->Parent->LocalPosition.Y));
                        InfoDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableRight->Parent->ClientSize.Y));
                    }
                    {
                        GI_Image::TImageGI* InfoDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableBack"sv));
                        {
                            std::int32_t cpp_arg_8 = BarWidth + 1 - InfoDurableBack->GetContentSize().X;
                            std::int32_t y_4 = InfoDurableBack->LocalPosition.Y;
                            InfoDurableBack->SetPosition(ClassesImports::Point(cpp_arg_8, y_4));
                        }
                        InfoDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableBack->Parent->ClientSize.Y));
                    }
                }
                {
                    GI_Label::TLabelGI* cpp_arg_9 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"sv));
                    GI_Label::TLabelGI* cpp_arg_10 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"sv));
                    fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, cpp_arg_10, cpp_arg_9, true, true, MinimumWidth);
                }
                GetByName(u"InfoSize"sv)->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemSizeLabelPosition.Y));
                GetByName(u"InfoPrice"sv)->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemPriceLabelPosition.Y));
                GetByName(u"EmRace"sv)->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
                if (GlobalsV::DynamicTipsPos) {
                    ItemInfoWindow->SetPosition(ClassesImports::Point(ItemInfoAnchor.X - ItemInfoWindow->ClientSize.X / 2, ItemInfoAnchor.Y));
                } else {
                    ItemInfoWindow->SetPosition(ClassesImports::Point(10, 10));
                }
            }
        }
    }

    void TfEquipmentShop::BuildHullSlotOverlays(GI_MessageLoop::TObjectGI* Parent, aItem::THull* Hull, std::int32_t OffsetX, std::int32_t OffsetY) {
        GI_MessageLoop::TObjectGI* Root{};
        std::int32_t I{};
        // Nested in BuildHullSlotOverlays; captures root, parent and offsets.
        auto AddOverlay = [&](const std::u16string_view& Name) -> void {
            pas::WideString Path{};
            GI_GraphBuf::TGraphBufGI* Graph{};
            GI_MessageLoop::TObjectGI* Control = Root->FindByNameRecursive(Name);
            if (Control != nullptr) {
                Graph = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Parent, false);
                Graph->SetPositionModeW(true);
                Graph->SetPosition(ClassesImports::Point(Control->LocalPosition.X + OffsetX, Control->LocalPosition.Y + OffsetY));
                Graph->SetSize(Control->ClientSize);
                Graph->SourceHasPerPixelAlpha = true;
                Path = pas::checked_cast<GI_Image::TImageGI*>(Control)->GetImagePath();
                if (EC_Str::CountDelimitedPartsW(pas::view(Path), u","sv) == 2) {
                    Path = EC_Str::ExtractDelimitedPartW(pas::view(Path), 1, u","sv);
                }
                GI_GI::LoadGiByPathIntoGraphBuf(Path, Graph->GraphBuf);
            }
        };
        Root = GetByName(u"InfoHull"sv);
        if (Hull->GetSlotCount(aConst::sskAfterburner) >= 1) {
            AddOverlay(u"InfoHull_Forsage"sv);
        }
        if (Hull->GetSlotCount(aConst::sskWeapon) < 1) {
            AddOverlay(u"InfoHull_W1"sv);
        }
        if (Hull->GetSlotCount(aConst::sskWeapon) < 2) {
            AddOverlay(u"InfoHull_W2"sv);
        }
        if (Hull->GetSlotCount(aConst::sskWeapon) < 3) {
            AddOverlay(u"InfoHull_W3"sv);
        }
        if (Hull->GetSlotCount(aConst::sskWeapon) < 4) {
            AddOverlay(u"InfoHull_W4"sv);
        }
        if (Hull->GetSlotCount(aConst::sskWeapon) < 5) {
            AddOverlay(u"InfoHull_W5"sv);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::DefaultHullSlotCounts[aConst::sskArtefact]); cpp_range.next(I); ) {
            if (Hull->GetSlotCount(aConst::sskArtefact) < I) {
                AddOverlay(pas::view(pas::concat_wide({u"InfoHull_A", EC_Str::IntToWideString(I)})));
            }
        }
        if (Hull->GetSlotCount(aConst::sskEngine) < 1) {
            AddOverlay(u"InfoHull_Engine"sv);
        }
        if (Hull->GetSlotCount(aConst::sskFuelTanks) < 1) {
            AddOverlay(u"InfoHull_FuelTanks"sv);
        }
        if (Hull->GetSlotCount(aConst::sskScanner) < 1) {
            AddOverlay(u"InfoHull_Scaner"sv);
        }
        if (Hull->GetSlotCount(aConst::sskRadar) < 1) {
            AddOverlay(u"InfoHull_Radar"sv);
        }
        if (Hull->GetSlotCount(aConst::sskRepairRobot) < 1) {
            AddOverlay(u"InfoHull_RepairRobot"sv);
        }
        if (Hull->GetSlotCount(aConst::sskCargoHook) < 1) {
            AddOverlay(u"InfoHull_CargoHook"sv);
        }
        if (Hull->GetSlotCount(aConst::sskDefGenerator) < 1) {
            AddOverlay(u"InfoHull_DefGenerator"sv);
        }
    }

    void TfEquipmentShop::RefreshHullInfo(GI_MessageLoop::TMessageLoopGI* Target, aItem::THull* Hull, pas::WideString Text, std::uint8_t SuppressImage) {
        GI_MessageLoop::TObjectGI* Control{};
        std::int32_t I{};
        pas::WideString PreviewPath{};
        pas::WideString SeriesName{};
        aGalaxyStruct::TKlingType DisplayKind{};
        aGalaxyStruct::TDominatorSeries Series{};
        std::int32_t BarWidth{};
        std::int32_t CapWidth{};
        std::int32_t MinimumWidth{};
        GI_Window::TWindowGI* Window = pas::checked_cast<GI_Window::TWindowGI*>(Target->GetByName(u"InfoHull"sv));
        GI_Label::TLabelGI* TextLabel = pas::checked_cast<GI_Label::TLabelGI*>(Target->GetByName(u"InfoHullText"sv));
        Window->SetActive(true);
        {
            const pas::WideString& wrapTextInColor = ([&] {
                pas::WideString displayName = Hull->GetDisplayName();
                pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                return aMyFunction::WrapTextInColor(pas::view(std::move(displayName)), pas::view(std::move(infoNameColorTag)));
            }());
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(Target->GetByName(u"InfoHullName"sv));
            cpp_arg->SetText(wrapTextInColor);
        }
        SeriesName = Hull->GetSeriesName();
        if (SeriesName != u"") {
            SeriesName = pas::concat_wide({u"\r\n", aMyFunction::WrapTextInColor(pas::view(SeriesName), pas::view(aMyFunction::InfoHullSeriesColorTag))});
        }
        {
            const pas::WideString& cpp_arg_2 = pas::concat_wide({pas::checked_cast<GI_Label::TLabelGI*>(Target->GetByName(u"InfoHullName"sv))->GetText(), SeriesName});
            GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(Target->GetByName(u"InfoHullName"sv));
            cpp_arg_3->SetText(cpp_arg_2);
        }
        TextLabel->SetText(Text);
        {
            std::uint8_t cpp_arg_4 = Hull->GetSlotCount(aConst::sskAfterburner) >= 1;
            GI_MessageLoop::TObjectGI* byName = Target->GetByName(u"InfoHull_Forsage"sv);
            byName->SetActive(cpp_arg_4);
        }
        {
            std::uint8_t cpp_arg_5 = !(Hull->GetSlotCount(aConst::sskWeapon) >= 1);
            GI_MessageLoop::TObjectGI* byName_2 = Target->GetByName(u"InfoHull_W1"sv);
            byName_2->SetActive(cpp_arg_5);
        }
        {
            std::uint8_t cpp_arg_6 = !(Hull->GetSlotCount(aConst::sskWeapon) >= 2);
            GI_MessageLoop::TObjectGI* byName_3 = Target->GetByName(u"InfoHull_W2"sv);
            byName_3->SetActive(cpp_arg_6);
        }
        {
            std::uint8_t cpp_arg_7 = !(Hull->GetSlotCount(aConst::sskWeapon) >= 3);
            GI_MessageLoop::TObjectGI* byName_4 = Target->GetByName(u"InfoHull_W3"sv);
            byName_4->SetActive(cpp_arg_7);
        }
        {
            std::uint8_t cpp_arg_8 = !(Hull->GetSlotCount(aConst::sskWeapon) >= 4);
            GI_MessageLoop::TObjectGI* byName_5 = Target->GetByName(u"InfoHull_W4"sv);
            byName_5->SetActive(cpp_arg_8);
        }
        {
            std::uint8_t cpp_arg_9 = !(Hull->GetSlotCount(aConst::sskWeapon) >= 5);
            GI_MessageLoop::TObjectGI* byName_6 = Target->GetByName(u"InfoHull_W5"sv);
            byName_6->SetActive(cpp_arg_9);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::DefaultHullSlotCounts[aConst::sskArtefact]); cpp_range.next(I); ) {
            Control = Target->FindControlByPath(pas::concat_wide({u"InfoHull_A", EC_Str::IntToWideString(I)}));
            if (Control != nullptr) {
                Control->SetActive(!(Hull->GetSlotCount(aConst::sskArtefact) >= I));
            }
        }
        {
            std::uint8_t cpp_arg_10 = !(Hull->GetSlotCount(aConst::sskEngine) >= 1);
            GI_MessageLoop::TObjectGI* byName_7 = Target->GetByName(u"InfoHull_Engine"sv);
            byName_7->SetActive(cpp_arg_10);
        }
        {
            std::uint8_t cpp_arg_11 = !(Hull->GetSlotCount(aConst::sskFuelTanks) >= 1);
            GI_MessageLoop::TObjectGI* byName_8 = Target->GetByName(u"InfoHull_FuelTanks"sv);
            byName_8->SetActive(cpp_arg_11);
        }
        {
            std::uint8_t cpp_arg_12 = !(Hull->GetSlotCount(aConst::sskScanner) >= 1);
            GI_MessageLoop::TObjectGI* byName_9 = Target->GetByName(u"InfoHull_Scaner"sv);
            byName_9->SetActive(cpp_arg_12);
        }
        {
            std::uint8_t cpp_arg_13 = !(Hull->GetSlotCount(aConst::sskRadar) >= 1);
            GI_MessageLoop::TObjectGI* byName_10 = Target->GetByName(u"InfoHull_Radar"sv);
            byName_10->SetActive(cpp_arg_13);
        }
        {
            std::uint8_t cpp_arg_14 = !(Hull->GetSlotCount(aConst::sskRepairRobot) >= 1);
            GI_MessageLoop::TObjectGI* byName_11 = Target->GetByName(u"InfoHull_RepairRobot"sv);
            byName_11->SetActive(cpp_arg_14);
        }
        {
            std::uint8_t cpp_arg_15 = !(Hull->GetSlotCount(aConst::sskCargoHook) >= 1);
            GI_MessageLoop::TObjectGI* byName_12 = Target->GetByName(u"InfoHull_CargoHook"sv);
            byName_12->SetActive(cpp_arg_15);
        }
        {
            std::uint8_t cpp_arg_16 = !(Hull->GetSlotCount(aConst::sskDefGenerator) >= 1);
            GI_MessageLoop::TObjectGI* byName_13 = Target->GetByName(u"InfoHull_DefGenerator"sv);
            byName_13->SetActive(cpp_arg_16);
        }
        DisplayKind = aGalaxyStruct::ktBoss;
        Series = aGalaxyStruct::dsBlazer;
        aGalaxyStruct::THullType HullKind = Hull->HullType;
        if (Hull->OwnerShip != nullptr && aPlayer::GetPlayer() == Hull->OwnerShip && aPlayer::GetPlayer()->ChameleonActive && pas::in_range(aPlayer::GetPlayer()->ChameleonVisualType, static_cast<std::int32_t>(aGalaxyStruct::ktBoss), static_cast<std::int32_t>(aGalaxyStruct::ktKlig)) && aPlayer::GetPlayer()->ChameleonVisualType != aGalaxyStruct::ktBoss) {
            HullKind = aGalaxyStruct::htKling;
            DisplayKind = aPlayer::GetPlayer()->ChameleonVisualType;
            Series = aPlayer::GetPlayer()->ChameleonSeries;
        }
        if (Hull->OwnerShip != nullptr && pas::class_cast_if<aKling::TKling*>(static_cast<pas::Object*>(Hull->OwnerShip)) != nullptr) {
            HullKind = aGalaxyStruct::htKling;
            DisplayKind = pas::checked_cast<aKling::TKling*>(static_cast<pas::Object*>(Hull->OwnerShip))->KlingType;
            Series = pas::checked_cast<aKling::TKling*>(static_cast<pas::Object*>(Hull->OwnerShip))->DominatorSeries;
        }
        if (!SuppressImage) {
            GI_Image::TImageGI* InfoHullImage = pas::checked_cast<GI_Image::TImageGI*>(Target->GetByName(u"InfoHullImage"sv));
            if (HullKind == aGalaxyStruct::htKling) {
                InfoHullImage->SetImagePath(u"GraphBuf"_w);
                PreviewPath = pas::WideString();
                if (DisplayKind != aGalaxyStruct::ktBoss) {
                    PreviewPath = GR_Main::GameDataConfig->GetParamByPathOrMarker(pas::concat_wide({u"SE.Ship.", aConst::DominatorSeriesNames[Series], u".", ShopDominatorImagePrefixes[Series], EC_Str::IntToWideString(DisplayKind), u".", GR_Main::GiResourceSuffix(), u"ImageP"}));
                }
                if (PreviewPath != u"") {
                    GI_GraphBuf::TGraphBufGI* cpp_with_2 = InfoHullImage->GraphBufControl;
                    cpp_with_2->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(PreviewPath), 1, u","sv), cpp_with_2->GraphBuf);
                    if (static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)) {
                        cpp_with_2->GraphBuf->RescaleRgba(cpp_with_2->ClientSize.X, System::Round(pas::real_divide(cpp_with_2->ClientSize.X, static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)), 5);
                    } else {
                        cpp_with_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_2->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width)), cpp_with_2->ClientSize.Y, 5);
                    }
                } else {
                    GI_GraphBuf::TGraphBufGI* cpp_with_3 = InfoHullImage->GraphBufControl;
                    cpp_with_3->SourceHasPerPixelAlpha = true;
                    if (DisplayKind == aGalaxyStruct::ktBoss) {
                        switch (Series) {
                            case aGalaxyStruct::dsTerron: {
                                GI_GI::LoadGiByPathIntoGraphBuf(u"Bm.Ruins.Terroni"_wref.get(), cpp_with_3->GraphBuf);
                                break;
                            }
                            case aGalaxyStruct::dsKeller: {
                                GI_GI::LoadGiByPathIntoGraphBuf(u"Bm.Ruins.Kelleri"_wref.get(), cpp_with_3->GraphBuf);
                                break;
                            }
                            case aGalaxyStruct::dsBlazer: {
                                GI_GI::LoadGiByPathIntoGraphBuf(u"Bm.Ruins.Blazeri"_wref.get(), cpp_with_3->GraphBuf);
                                break;
                            }
                        }
                    }
                    if (static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Height)) {
                        cpp_with_3->GraphBuf->RescaleRgba(cpp_with_3->ClientSize.X, System::Round(pas::real_divide(cpp_with_3->ClientSize.X, static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Height)), 5);
                    } else {
                        cpp_with_3->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_3->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Width)), cpp_with_3->ClientSize.Y, 5);
                    }
                }
            } else {
                InfoHullImage->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Hull), u"s"}));
            }
            InfoHullImage->SetImageKindX(GI_Main::ikxCenter);
            InfoHullImage->SetImageKindY(GI_Main::ikyCenter);
            {
                Types::TPoint visualCenter = InfoHullImage->GetVisualCenter();
                Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                InfoHullImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
            }
        }
        {
            const pas::WideString& intToWideString = EC_Str::IntToWideString(Hull->Weight);
            GI_Label::TLabelGI* cpp_arg_17 = pas::checked_cast<GI_Label::TLabelGI*>(Target->GetByName(u"InfoHullSize"sv));
            cpp_arg_17->SetText(intToWideString);
        }
        {
            const pas::WideString& intToWideString_2 = EC_Str::IntToWideString(Hull->GetConditionAdjustedCost());
            GI_Label::TLabelGI* cpp_arg_18 = pas::checked_cast<GI_Label::TLabelGI*>(Target->GetByName(u"InfoHullPrice"sv));
            cpp_arg_18->SetText(intToWideString_2);
        }
        {
            GI_Image::TImageGI* InfoHullEmRace = pas::checked_cast<GI_Image::TImageGI*>(Target->GetByName(u"InfoHullEmRace"sv));
            InfoHullEmRace->SetImagePath(aConst::GetFactionEmblemPath(aItem::TItem_GetOwnerConfigName(Hull)));
            InfoHullEmRace->SetImageKindX(GI_Main::ikxCenter);
            InfoHullEmRace->SetImageKindY(GI_Main::ikyCenter);
        }
        {
            float real_max = pas::real_max<float>(0.1f, Hull->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})));
            BarWidth = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(Hull->Weight, aConst::HullBaseSize), real_max)) * 64.0L);
        }
        BarWidth = std::min<std::int32_t>(192, std::max<std::int32_t>(32, BarWidth));
        {
            GI_Image::TImageGI* InfoDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableLeft"sv));
            CapWidth = InfoDurableLeft->GetContentSize().X;
            MinimumWidth = 2 * CapWidth + BarWidth + InfoDurableLeft->LocalPosition.X + InfoDurableLeft->Parent->LocalPosition.X + 2 * InfoDurableLeft->Parent->Parent->LocalPosition.X;
        }
        {
            GI_Image::TImageGI* InfoHullDurable = pas::checked_cast<GI_Image::TImageGI*>(Target->GetByName(u"InfoHullDurable"sv));
            InfoHullDurable->Parent->Parent->SetSize(ClassesImports::Point(2 * CapWidth + BarWidth, InfoHullDurable->Parent->Parent->ClientSize.Y));
            InfoHullDurable->Parent->SetSize(ClassesImports::Point(BarWidth + 2, InfoHullDurable->Parent->Parent->ClientSize.Y));
            {
                std::int64_t cpp_left = System::Round(pas::real_divide(Hull->HullPoints, Hull->Weight) * BarWidth);
                std::int32_t cpp_arg_19 = cpp_left - (InfoHullDurable->GetContentSize().X - 5);
                std::int32_t y = InfoHullDurable->LocalPosition.Y;
                InfoHullDurable->SetPosition(ClassesImports::Point(cpp_arg_19, y));
            }
        }
        {
            GI_Image::TImageGI* InfoHullDurableRight = pas::checked_cast<GI_Image::TImageGI*>(Target->GetByName(u"InfoHullDurableRight"sv));
            {
                std::int32_t cpp_arg_20 = BarWidth + CapWidth - InfoHullDurableRight->GetContentSize().X;
                std::int32_t y_2 = InfoHullDurableRight->LocalPosition.Y;
                InfoHullDurableRight->SetPosition(ClassesImports::Point(cpp_arg_20, y_2));
            }
            InfoHullDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoHullDurableRight->Parent->LocalPosition.Y));
            InfoHullDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoHullDurableRight->Parent->ClientSize.Y));
        }
        {
            GI_Image::TImageGI* InfoHullDurableBack = pas::checked_cast<GI_Image::TImageGI*>(Target->GetByName(u"InfoHullDurableBack"sv));
            {
                std::int32_t cpp_arg_21 = BarWidth + 1 - InfoHullDurableBack->GetContentSize().X;
                std::int32_t y_3 = InfoHullDurableBack->LocalPosition.Y;
                InfoHullDurableBack->SetPosition(ClassesImports::Point(cpp_arg_21, y_3));
            }
            InfoHullDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoHullDurableBack->Parent->ClientSize.Y));
        }
        fShip2::TfShip2::LayoutItemInfo(Window, pas::checked_cast<GI_Label::TLabelGI*>(Target->GetByName(u"InfoHullName"sv)), TextLabel, false, true, MinimumWidth);
        Target->GetByName(u"InfoHullSize"sv)->SetPosition(ClassesImports::Point(HullSizeOffset.X, Window->ClientSize.Y + HullSizeOffset.Y));
        Target->GetByName(u"InfoHullPrice"sv)->SetPosition(ClassesImports::Point(HullPriceOffset.X, Window->ClientSize.Y + HullPriceOffset.Y));
        Target->GetByName(u"InfoHullEmRace"sv)->SetPosition(ClassesImports::Point(Window->ClientSize.X + HullRaceOffset.X, Window->ClientSize.Y + HullRaceOffset.Y));
        if (GlobalsV::DynamicTipsPos) {
            Window->SetPosition(ClassesImports::Point(ItemInfoAnchor.X - Window->ClientSize.X / 2, ItemInfoAnchor.Y));
        } else {
            Window->SetPosition(ClassesImports::Point(10, 10));
        }
    }

    void TfEquipmentShop::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA && static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Left"sv))->Disabled ^ 1)) {
            ScrollLeft(nullptr);
            RefreshItemInfo(nullptr);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA && static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Right"sv))->Disabled ^ 1)) {
            ScrollRight(nullptr);
            RefreshItemInfo(nullptr);
        }
    }

    void TfEquipmentShop::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1) && ExitCode == 0) {
            if (Key == 'S') {
                ShipClicked(nullptr);
            } else if ((Key == WindowsSdk::VK_LEFT || Key == WindowsSdk::VK_UP) && static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Left"sv))->Disabled ^ 1)) {
                ScrollLeft(nullptr);
                RefreshItemInfo(nullptr);
            } else if ((Key == WindowsSdk::VK_RIGHT || Key == WindowsSdk::VK_DOWN) && static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Right"sv))->Disabled ^ 1)) {
                ScrollRight(nullptr);
                RefreshItemInfo(nullptr);
            } else if (Key == WindowsSdk::VK_SPACE) {
                if (GetByName(u"PM_EndTurn"sv)->Active) {
                    EndTurnClicked(nullptr);
                }
            } else {
                MainPanel->ProcessKeyDown(Key);
                PlanetPanel->ProcessKeyDown(Key);
                StationPanel->ProcessKeyDown(Key);
            }
        }
    }

    void TfEquipmentShop::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (MainPanel->NavigationLocked) {
            return;
        }
        if (GR_Main::ExitScreenLoop) {
            return;
        }
        if (pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10011);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20011);
        }
    }

    void TfEquipmentShop::SelectMusic() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->GetShutterDirection() == -1) {
            return;
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
            return;
        }
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId)].InternalName, u"Pirate"}));
                } else {
                    GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
                }
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
            }
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (pas::is_one_of<aGalaxyStruct::rstPirateBase, aGalaxyStruct::rstDominion>(aPlayer::GetPlayer()->DockedTo->TypeId)) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace)].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace)].InternalName}));
            }
        }
    }

    void TShopSlot::p_destroy() {
        fEquipmentShop::TShopSlot_Destroy(this);
    }

    void TfEquipmentShop::p_destroy() {
        fEquipmentShop::TfEquipmentShop_Destroy(this);
    }

} // namespace fEquipmentShop
