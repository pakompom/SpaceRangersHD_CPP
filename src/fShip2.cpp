#include "layout/fShip2.hpp"
#include "types/Achievements.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GI_Window.hpp"
#include "types/GI_XviD.hpp"
#include "types/GI_Zone.hpp"
#include "types/GR_Music.hpp"
#include "types/MessagesSdk.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SystemImports.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aMissile.hpp"
#include "types/aNormalShip.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/ab_MainForm.hpp"
#include "types/ab_Object.hpp"
#include "types/fHangar.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Sound.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/SE_Space.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aEFilmEnd.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Item.hpp"
#include "units/ab_Ship.hpp"
#include "units/fChameleon.hpp"
#include "units/fCount2.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fPanelMain.hpp"
#include "units/fRewards.hpp"
#include "units/fShip2.hpp"
#include "units/fTextBox.hpp"

namespace fShip2 {
    std::int32_t StorageImageCount = 21;

    pas::List* PlayerHoldEntries = nullptr;

    aShip::TShip* PlayerHoldShip = nullptr;

    std::uint32_t SelfSkillPointColor{};

    std::uint32_t OtherSkillPointColor{};

    // Native initialized-WideString descriptor; compiler-generated
    // finalizer clears these 13 entries (unit counter).
    pas::Array<pas::WideString, 0, 12> ShipEquipmentZoneNames = pas::Array<pas::WideString, 0, 12>{{
        u"S_Hull_0z"_w, u"S_FuelTanks_0z"_w, u"S_Engine_0z"_w, u"S_Radar_0z"_w, u"S_Scaner_0z"_w, u"S_RepairRobot_0z"_w,
        u"S_CargoHook_0z"_w, u"S_DefGenerator_0z"_w, u"S_Weapon_0z"_w, u"S_Weapon_1z"_w, u"S_Weapon_2z"_w,
        u"S_Weapon_3z"_w, u"S_Weapon_4z"_w,
    }};

    std::uint8_t RunShipEquipment(GI_MessageLoop::TMessageLoopGI* ParentLoop) {
        GI_MessageLoop::TCursorStateGI State{};
        ParentLoop->RootUiObject->OnModalSuspend();
        ParentLoop->CaptureCursorState(&State);
        ParentLoop->SetCursorActive(false);
        ParentLoop->DrawQueuedUpdateRects();
        Globals::ShipScreen->ParentLoop = ParentLoop;
        ParentLoop->ChildLoop = Globals::ShipScreen;
        std::uint8_t Result = Globals::ShipScreen->Run() == 1;
        Globals::ShipScreen->ParentLoop = nullptr;
        ParentLoop->ChildLoop = nullptr;
        ParentLoop->InvalidateViewport();
        ParentLoop->RestoreCursorState(&State);
        ParentLoop->UpdateCursorPosition();
        ParentLoop->RootUiObject->OnModalResume();
        GR_Main::PostMouseMoveMessage();
        return Result;
    }

    std::int32_t CompareStoredItems(aPlayer::PStorageEntry Left, aPlayer::PStorageEntry Right, TPlayerHoldSort Sort) {
        std::int32_t LeftValue{};
        std::int32_t RightValue{};
        pas::WideString LeftName{};
        pas::WideString RightName{};
        std::int32_t NameComparison{};
        // Nested helper; does not read the parent frame.
        auto Classify = [&](std::uint8_t ItemType) -> std::int32_t {
            std::int32_t Result = 0;
            if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
                return 1;
            } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_ArtFastRacks))) {
                return 3;
            } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                return 2;
            } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Protoplasm), static_cast<std::int32_t>(aConst::t_UselessCountableItem))) {
                return 4;
            } else {
                return Result;
            }
        };
        std::int32_t Result = 0;
        if (Left->Item == nullptr) {
            return -1;
        }
        if (Right->Item == nullptr) {
            return 1;
        }
        std::uint8_t LeftType = static_cast<std::uint8_t>(Left->Item->ItemType);
        std::uint8_t RightType = static_cast<std::uint8_t>(Right->Item->ItemType);
        std::int32_t LeftClass = Classify(LeftType);
        std::int32_t RightClass = Classify(RightType);
        if (Sort == phsType) {
            if (LeftClass < RightClass) {
                return -1;
            }
            if (LeftClass > RightClass) {
                return 1;
            }
            if (LeftType < RightType) {
                return -1;
            }
            if (LeftType > RightType) {
                return 1;
            }
            if (LeftClass == 4 && RightClass == 4 && Left->Item->ItemType == aConst::t_MicroModule && Right->Item->ItemType == aConst::t_MicroModule) {
                LeftValue = aItem::GetMicroModulePriorityColorTier(pas::checked_cast<aItem::TMicroModule*>(Left->Item)->MicroModuleIndex - 1);
                RightValue = aItem::GetMicroModulePriorityColorTier(pas::checked_cast<aItem::TMicroModule*>(Right->Item)->MicroModuleIndex - 1);
                if (LeftValue < RightValue) {
                    return -1;
                }
                if (LeftValue > RightValue) {
                    return 1;
                }
            }
            LeftName = Left->Item->GetDisplayName();
            RightName = Right->Item->GetDisplayName();
            NameComparison = EC_Str::CompareWideChars(LeftName.pchar(), RightName.pchar());
            if (NameComparison != 0) {
                return NameComparison;
            }
        }
        if (static_cast<std::int32_t>(Sort) <= static_cast<std::int32_t>(phsSize)) {
            if (LeftClass == 1) {
                LeftValue = reinterpret_cast<aItem::TGoods*>(Left->Item)->Quantity;
            } else {
                LeftValue = Left->Item->Weight;
            }
            if (RightClass == 1) {
                RightValue = reinterpret_cast<aItem::TGoods*>(Right->Item)->Quantity;
            } else {
                RightValue = Right->Item->Weight;
            }
            if (LeftValue < RightValue) {
                return -1;
            }
            if (LeftValue > RightValue) {
                return 1;
            }
        }
        LeftValue = Left->Item->Cost;
        RightValue = Right->Item->Cost;
        if (LeftValue < RightValue) {
            return -1;
        }
        if (LeftValue > RightValue) {
            return 1;
        }
        return Result;
    }

    pas::WideString RankToImage(std::uint8_t Rank) {
        pas::WideString Result{};
        if (pas::in_range(Rank, 0, 7)) {
            return static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormRating2.2Rank", SysUtils::IntToStr(Rank + 1)}));
        }
        GR_Main::RaiseWideMessage(u"No image for rank in function RankToImage(tr:TRank):WideString;"_wref.get());
        return Result;
    }

    pas::WideString RankToImageSmall(std::uint8_t Rank) {
        pas::WideString Result{};
        if (pas::in_range(Rank, 0, 7)) {
            return static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormShip2.2Rank", SysUtils::IntToStr(Rank + 1)}));
        }
        GR_Main::RaiseWideMessage(u"No image for rank in function RankToImageSmall(tr:TRank):WideString;"_wref.get());
        return Result;
    }

    pas::WideString PirateRankToImage(std::uint8_t Rank) {
        pas::WideString Result{};
        if (pas::in_range(Rank, 0, 7)) {
            return static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormShip2.PRank", SysUtils::IntToStr(Rank + 1)}));
        }
        GR_Main::RaiseWideMessage(u"No image for rank in function PirateRankToImage(tr: TPirateRank): WideString;"_wref.get());
        return Result;
    }

    pas::WideString PirateRankToImageSmall(std::uint8_t Rank) {
        pas::WideString Result{};
        if (pas::in_range(Rank, 0, 7)) {
            return static_cast<pas::WideString>(pas::concat_ansi({"GI,Bm.FormShip2.PRank", SysUtils::IntToStr(Rank + 1), "s"}));
        }
        GR_Main::RaiseWideMessage(u"No image for rank in function PirateRankToImageSmall(tr: TPirateRank): WideString;"_wref.get());
        return Result;
    }

    void InitializePlayerHoldView() {
        fShip2::FinalizePlayerHoldView();
        PlayerHoldEntries = pas::make_object<pas::List>();
    }

    void FinalizePlayerHoldView() {
        if (PlayerHoldEntries != nullptr) {
            fShip2::ClearPlayerHoldEntries();
            pas::free(PlayerHoldEntries);
            PlayerHoldEntries = nullptr;
        }
    }

    // Uses PlayerHoldShip, or the player when nil; preserves display order.
    void RefreshPlayerHoldView(std::uint8_t IncludeFilteredItems) {
        aShip::TShip* Ship{};
        std::uint8_t Goods{};
        TPlayerHoldUnit* Entry{};
        TPlayerHoldUnit* Other{};
        std::int32_t I{};
        std::int32_t J{};
        aItem::TEquipment* Item{};
        if (PlayerHoldEntries == nullptr) {
            fShip2::InitializePlayerHoldView();
        }
        if (PlayerHoldShip != nullptr) {
            Ship = PlayerHoldShip;
        } else {
            Ship = aPlayer::GetPlayer();
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
            Entry->Retained = Entry->Kind == phkEmpty;
        }
        for (Goods = static_cast<std::uint8_t>(0); Goods <= static_cast<std::uint8_t>(7); ++Goods) {
            if (!IncludeFilteredItems) {
                if (!(static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessHoldGoods(Goods))) {
                    continue;
                }
            }
            if (Ship->CargoGoods[Goods].Count > 0) {
                I = 0;
                while (I < pas::list_count(PlayerHoldEntries)) {
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                    if (static_cast<std::uint8_t>(Entry->Retained ^ 1) && Entry->Kind == phkGoods && Entry->GoodsIndex == Goods) {
                        Entry->Retained = true;
                        break;
                    }
                    ++I;
                }
                if (I >= pas::list_count(PlayerHoldEntries)) {
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_insert(PlayerHoldEntries, 0, reinterpret_cast<void*>(Entry));
                    Entry->Kind = phkGoods;
                    Entry->GoodsIndex = Goods;
                    Entry->Retained = true;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Inventory) - 1); cpp_range_2.next(J); ) {
            Item = pas::list_at<aItem::TEquipment>(Ship->Inventory, J);
            if (Item->EquippedFlag == 0 && Ship->GetHull() != Item) {
                if (IncludeFilteredItems || (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Item))) {
                    I = 0;
                    while (I < pas::list_count(PlayerHoldEntries)) {
                        Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                        if (static_cast<std::uint8_t>(Entry->Retained ^ 1) && Entry->Kind == phkEquipment && Item->Id == Entry->ItemId) {
                            Entry->Retained = true;
                            Entry->Item = Item;
                            break;
                        }
                        ++I;
                    }
                    if (I >= pas::list_count(PlayerHoldEntries)) {
                        Entry = pas::make_object<TPlayerHoldUnit>();
                        pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                        Entry->Kind = phkEquipment;
                        Entry->ItemId = Item->Id;
                        Entry->Item = Item;
                        Entry->Retained = true;
                    }
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Artefacts) - 1); cpp_range_3.next(J); ) {
            Item = pas::list_at<aItem::TEquipment>(Ship->Artefacts, J);
            if (Item->EquippedFlag == 0) {
                if (IncludeFilteredItems || (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Item))) {
                    I = 0;
                    while (I < pas::list_count(PlayerHoldEntries)) {
                        Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                        if (static_cast<std::uint8_t>(Entry->Retained ^ 1) && Entry->Kind == phkArtefact && Item->Id == Entry->ItemId) {
                            Entry->Retained = true;
                            Entry->Item = Item;
                            break;
                        }
                        ++I;
                    }
                    if (I >= pas::list_count(PlayerHoldEntries)) {
                        Entry = pas::make_object<TPlayerHoldUnit>();
                        pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                        Entry->Kind = phkArtefact;
                        Entry->ItemId = Item->Id;
                        Entry->Item = Item;
                        Entry->Retained = true;
                    }
                }
            }
        }
        I = pas::list_count(PlayerHoldEntries) - 1;
        while (I >= 0) {
            Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
            if (Entry->Kind != phkEmpty) {
                break;
            }
            Entry->Retained = false;
            --I;
        }
        I = 0;
        while (I < pas::list_count(PlayerHoldEntries)) {
            Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
            if (!Entry->Retained) {
                pas::list_delete(PlayerHoldEntries, I);
                pas::free(Entry);
            } else {
                ++I;
            }
        }
        std::int32_t Order = 0;
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(PlayerHoldEntries) - 1); cpp_range_4.next(I); ) {
            Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
            if (Entry->Kind == phkEmpty) {
                continue;
            }
            J = 0;
            while (J < I) {
                Other = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, J);
                if (Other->Kind != phkEmpty && Other->DisplayOrder == Entry->DisplayOrder) {
                    break;
                }
                ++J;
            }
            if (J < I) {
                while (true) {
                    J = 0;
                    while (J < pas::list_count(PlayerHoldEntries)) {
                        Other = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, J);
                        if (Other->Kind != phkEmpty) {
                            if (Other->DisplayOrder == Order) {
                                break;
                            }
                        }
                        ++J;
                    }
                    if (J >= pas::list_count(PlayerHoldEntries)) {
                        break;
                    }
                    ++Order;
                }
                Entry->DisplayOrder = Order;
                ++Order;
            }
        }
    }

    void RemoveEmptyPlayerHoldSlot(std::int32_t Index) {
        if (Index < 0 || Index >= pas::list_count(PlayerHoldEntries)) {
            return;
        }
        TPlayerHoldUnit* Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, Index);
        if (Entry->Kind == phkEmpty) {
            pas::list_delete(PlayerHoldEntries, Index);
            pas::free(Entry);
        }
    }

    void ClearPlayerHoldEntries() {
        std::int32_t I{};
        TPlayerHoldUnit* Entry{};
        if (PlayerHoldEntries == nullptr) {
            return;
        }
        {
            const std::int32_t cpp_first = pas::list_count(PlayerHoldEntries) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                    pas::list_delete(PlayerHoldEntries, I);
                    pas::free(Entry);
                }
            }
        }
    }

    void RemoveEmptyPlayerHoldSlots() {
        std::int32_t I{};
        TPlayerHoldUnit* Entry{};
        if (PlayerHoldEntries == nullptr) {
            return;
        }
        {
            const std::int32_t cpp_first = pas::list_count(PlayerHoldEntries) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                    if (Entry->Kind == phkEmpty) {
                        pas::list_delete(PlayerHoldEntries, I);
                        pas::free(Entry);
                    }
                }
            }
        }
    }

    // Returns true for out-of-range indices as well as empty slots.
    std::uint8_t IsPlayerHoldSlotEmpty(std::int32_t Index) {
        std::uint8_t Result = true;
        if (Index < 0 || Index >= pas::list_count(PlayerHoldEntries)) {
            return Result;
        }
        TPlayerHoldUnit* Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, Index);
        return Entry->Kind == phkEmpty;
    }

    // Ascending order; goods prices are total purchase costs, not unit prices.
    std::int32_t ComparePlayerHoldEntries(TPlayerHoldUnit* Left, TPlayerHoldUnit* Right, TPlayerHoldSort Sort) {
        std::int32_t LeftValue{};
        std::int32_t RightValue{};
        std::uint8_t LeftType{};
        std::uint8_t RightType{};
        pas::WideString LeftName{};
        pas::WideString RightName{};
        std::int32_t NameComparison{};
        std::int32_t Result = 0;
        if (Sort == phsType) {
            if (static_cast<std::int32_t>(Left->Kind) < static_cast<std::int32_t>(Right->Kind)) {
                return -1;
            }
            if (static_cast<std::int32_t>(Left->Kind) > static_cast<std::int32_t>(Right->Kind)) {
                return 1;
            }
            if (Left->Kind == phkGoods) {
                LeftType = Left->GoodsIndex;
            } else {
                LeftType = static_cast<std::uint8_t>(Left->Item->ItemType);
            }
            if (Right->Kind == phkGoods) {
                RightType = Right->GoodsIndex;
            } else {
                RightType = static_cast<std::uint8_t>(Right->Item->ItemType);
            }
            if (LeftType < RightType) {
                return -1;
            }
            if (LeftType > RightType) {
                return 1;
            }
            if (pas::is_one_of<phkEquipment, phkArtefact>(Left->Kind) && pas::is_one_of<phkEquipment, phkArtefact>(Right->Kind) && Left->Item != nullptr && Right->Item != nullptr) {
                if (Left->Item->ItemType == aConst::t_MicroModule && Right->Item->ItemType == aConst::t_MicroModule) {
                    LeftValue = aItem::GetMicroModulePriorityColorTier(pas::checked_cast<aItem::TMicroModule*>(Left->Item)->MicroModuleIndex - 1);
                    RightValue = aItem::GetMicroModulePriorityColorTier(pas::checked_cast<aItem::TMicroModule*>(Right->Item)->MicroModuleIndex - 1);
                    if (LeftValue < RightValue) {
                        return -1;
                    }
                    if (LeftValue > RightValue) {
                        return 1;
                    }
                }
                LeftName = Left->Item->GetDisplayName();
                RightName = Right->Item->GetDisplayName();
                NameComparison = EC_Str::CompareWideChars(LeftName.pchar(), RightName.pchar());
                if (NameComparison != 0) {
                    return NameComparison;
                }
            }
        }
        if (static_cast<std::int32_t>(Sort) <= static_cast<std::int32_t>(phsSize)) {
            if (Left->Kind == phkGoods) {
                LeftValue = PlayerHoldShip->CargoGoods[Left->GoodsIndex].Count;
            } else {
                LeftValue = Left->Item->Weight;
            }
            if (Right->Kind == phkGoods) {
                RightValue = PlayerHoldShip->CargoGoods[Right->GoodsIndex].Count;
            } else {
                RightValue = Right->Item->Weight;
            }
            if (LeftValue < RightValue) {
                return -1;
            }
            if (LeftValue > RightValue) {
                return 1;
            }
        }
        if (Left->Kind == phkGoods) {
            LeftValue = PlayerHoldShip->CargoGoods[Left->GoodsIndex].TotalCost;
        } else {
            LeftValue = Left->Item->Cost;
        }
        if (Right->Kind == phkGoods) {
            RightValue = PlayerHoldShip->CargoGoods[Right->GoodsIndex].TotalCost;
        } else {
            RightValue = Right->Item->Cost;
        }
        if (LeftValue < RightValue) {
            return -1;
        }
        if (LeftValue > RightValue) {
            return 1;
        }
        return Result;
    }

    void SortPlayerHoldEntries(TPlayerHoldSort Sort) {
        std::int32_t I{};
        std::int32_t J{};
        TPlayerHoldUnit* Entry{};
        aGalaxy::Galaxy->CheckIntegrityChecksum1(413);
        fShip2::RemoveEmptyPlayerHoldSlots();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldEntries) - 2); cpp_range.next(I); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, pas::list_count(PlayerHoldEntries) - 1); cpp_range_2.next(J); ) {
                TPlayerHoldUnit* cpp_arg = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, J);
                TPlayerHoldUnit* cpp_arg_2 = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                if (fShip2::ComparePlayerHoldEntries(cpp_arg_2, cpp_arg, Sort) > 0) {
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                    pas::list_put(PlayerHoldEntries, I, pas::list_get(PlayerHoldEntries, J));
                    pas::list_put(PlayerHoldEntries, J, reinterpret_cast<void*>(Entry));
                }
            }
        }
        J = 0;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldEntries) - 1); cpp_range_3.next(I); ) {
            Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
            if (Entry->Kind != phkEmpty) {
                Entry->DisplayOrder = J;
                ++J;
            }
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(414);
    }

    void RestorePlayerHoldDisplayOrder() {
        std::int32_t I{};
        std::int32_t J{};
        TPlayerHoldUnit* Entry{};
        {
            const std::int32_t cpp_first = pas::list_count(PlayerHoldEntries) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    fShip2::RemoveEmptyPlayerHoldSlot(I);
                }
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldEntries) - 2); cpp_range.next(I); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, pas::list_count(PlayerHoldEntries) - 1); cpp_range_2.next(J); ) {
                std::int32_t cpp_left = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I)->DisplayOrder;
                if (cpp_left > pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, J)->DisplayOrder) {
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                    pas::list_put(PlayerHoldEntries, I, pas::list_get(PlayerHoldEntries, J));
                    pas::list_put(PlayerHoldEntries, J, reinterpret_cast<void*>(Entry));
                }
            }
        }
    }

    // Returns -1 when absent.
    std::int32_t FindPlayerHoldIndexByOrder(std::int32_t DisplayOrder) {
        std::int32_t I{};
        TPlayerHoldUnit* Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
            if (Entry->Kind != phkEmpty) {
                if (Entry->DisplayOrder == DisplayOrder) {
                    return I;
                }
            }
        }
        return -1;
    }

    std::int32_t FindFreePlayerHoldOrder() {
        std::int32_t Result{};
        std::int32_t I{};
        TPlayerHoldUnit* Entry{};
        std::int32_t Order = 0;
        while (true) {
            I = 0;
            while (I < pas::list_count(PlayerHoldEntries)) {
                Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                if (Entry->Kind != phkEmpty) {
                    if (Entry->DisplayOrder == Order) {
                        break;
                    }
                }
                ++I;
            }
            if (I >= pas::list_count(PlayerHoldEntries)) {
                return Order;
            }
            ++Order;
        }
        return Result;
    }

    void TfShip2_Create(TfShip2* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->ShipLoopSound = pas::construct_call<GR_Sound::TSoundBufferControl>(GR_Sound::TSoundBufferControl_Create);
        Self->ShipLoopSound->Configure(u"Sound.ShipLoop"_wref.get(), 0, true);
        Self->ShipToInspect = nullptr;
        Self->RemoteHoldMode = false;
    }

    void TfShip2_Destroy(TfShip2* Self) {
        pas::free(Self->ShipLoopSound);
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfShip2::InitializeLayout() {
        std::int32_t I{};
        std::int32_t Column{};
        std::int32_t Row{};
        GI_GraphButton::TGraphButtonGI* Button{};
        {
            GI_MessageLoop::TObjectGI* InfoImage = GetByName(u"InfoImage"sv);
            ItemImageCenter = EC_Struct::AddPoints(InfoImage->LocalPosition, EC_Struct::HalfPoint(InfoImage->ClientSize));
        }
        {
            GI_MessageLoop::TObjectGI* InfoSize = GetByName(u"InfoSize"sv);
            ItemSizeLabelPosition = ClassesImports::Point(InfoSize->LocalPosition.X, InfoSize->LocalPosition.Y - InfoSize->Parent->ClientSize.Y);
        }
        {
            GI_MessageLoop::TObjectGI* InfoPrice = GetByName(u"InfoPrice"sv);
            ItemPriceLabelPosition = ClassesImports::Point(InfoPrice->LocalPosition.X, InfoPrice->LocalPosition.Y - InfoPrice->Parent->ClientSize.Y);
        }
        {
            GI_MessageLoop::TObjectGI* EmRace = GetByName(u"EmRace"sv);
            ItemRaceImagePosition = ClassesImports::Point(EmRace->LocalPosition.X - EmRace->Parent->ClientSize.X, EmRace->LocalPosition.Y - EmRace->Parent->ClientSize.Y);
        }
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fShip2... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* BGBuf = MainPanel->FindByNameRecursive(u"BGBuf"sv);
                BGBuf->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                {
                    GI_MessageLoop::TObjectGI* cpp_with_7 = BGBuf->NextSibling;
                    cpp_with_7->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                }
            }
            {
                GI_MessageLoop::TObjectGI* ADD_WarningMoney = MainPanel->FindByNameRecursive(u"ADD_WarningMoney"sv);
                ADD_WarningMoney->SetPosition(ClassesImports::Point(ADD_WarningMoney->LocalPosition.X + GR_Main::ExtraScreenWidth, ADD_WarningMoney->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ADD_Money = MainPanel->FindByNameRecursive(u"ADD_Money"sv);
                ADD_Money->SetPosition(ClassesImports::Point(ADD_Money->LocalPosition.X + GR_Main::ExtraScreenWidth, ADD_Money->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                if (GR_Main::GiResourceVariant() == 1) {
                    ADD_Money->SetDepth(-151.0);
                } else {
                    ADD_Money->SetDepth(-106.0);
                }
            }
            {
                GI_MessageLoop::TObjectGI* UsePanel_Parent = MainPanel->FindByNameRecursive(u"UsePanel"sv)->Parent;
                UsePanel_Parent->SetPosition(ClassesImports::Point(UsePanel_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, UsePanel_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* SC_Panel = MainPanel->FindByNameRecursive(u"SC_Panel"sv);
                SC_Panel->SetPosition(ClassesImports::Point(SC_Panel->LocalPosition.X, (GR_Main::GameScreenHeight - SC_Panel->ClientSize.Y) / 2 - 50));
            }
            {
                GI_MessageLoop::TObjectGI* RankWnd = MainPanel->FindByNameRecursive(u"RankWnd"sv);
                RankWnd->SetPosition(ClassesImports::Point(RankWnd->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, RankWnd->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* RewardWnd = MainPanel->FindByNameRecursive(u"RewardWnd"sv);
                RewardWnd->SetPosition(ClassesImports::Point(RewardWnd->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, RewardWnd->LocalPosition.Y));
            }
            MainPanel->FindByNameRecursive(u"Film"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_GraphButton::TGraphButtonGI* CustomBridgeInto = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(MainPanel->FindByNameRecursive(u"CustomBridgeInto"sv));
                CustomBridgeInto->SetPosition(ClassesImports::Point(CustomBridgeInto->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, CustomBridgeInto->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        BackgroundBuffer = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"sv));
        ItemInfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"PII"sv));
        ItemImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"sv));
        ItemNameLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"sv));
        ItemDescriptionLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"sv));
        ItemSizeLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"sv));
        ItemPriceLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"sv));
        ItemRaceImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"sv));
        RightOpenImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RightOpen"sv));
        SkillsPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"Skills"sv));
        FreeSkillPointsLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"SkillFreePoints"sv));
        ExperienceLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"PDS_Exp"sv));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 5); cpp_range.next(I); ) {
            HoldSlotZones[I] = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "z"})))));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 5); cpp_range_2.next(I); ) {
            SkillImages[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I)})))));
            SkillImagesP[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "p"})))));
            SkillImagesN[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "n"})))));
            SkillPanels[I] = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "c"})))));
            SkillImageRestTop[I] = SkillImages[I]->LocalPosition.Y;
            SkillButtons[I] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "Add"})))));
            SkillValueLabels[I] = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"PDS_Skill", SysUtils::IntToStr(I), "v"})))));
            SkillProgressImages[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"PDS_Skill", SysUtils::IntToStr(I), "i"})))));
            SkillGainImages[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"PDS_Skill", SysUtils::IntToStr(I), "g"})))));
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, StorageImageCount - 1); cpp_range_3.next(I); ) {
            StorageImages[I] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Storage_", SysUtils::IntToStr(I), "i"})))));
        }
        RewardsBuffer = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardsImg"sv));
        RewardsWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"RewardWnd"sv));
        ExitButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Exit"sv));
        ExitButton->UpCallback = pas::bind_method<&TfShip2::CloseClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ExitRH"sv))->UpCallback = pas::bind_method<&TfShip2::CloseClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"DestrInto"sv))->UpCallback = pas::bind_method<&TfShip2::EnterBridgeClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"CustomBridgeInto"sv))->UpCallback = pas::bind_method<&TfShip2::EnterBridgeClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Left"sv))->UpCallback = pas::bind_method<&TfShip2::HoldLeftReleased>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Left"sv))->DownCallback = pas::bind_method<&TfShip2::HoldLeftPressed>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Right"sv))->UpCallback = pas::bind_method<&TfShip2::HoldRightReleased>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Right"sv))->DownCallback = pas::bind_method<&TfShip2::HoldRightPressed>(this);
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfShip2::MainKeyDown>(this);
        GetByName(u"MainPanel"sv)->KeyUpCallback = pas::bind_method<&TfShip2::MainKeyUp>(this);
        GetByName(u"MainPanel"sv)->LeftButtonUpCallback = pas::bind_method<&TfShip2::MainLeftButtonUp>(this);
        GetByName(u"MainPanel"sv)->RightButtonDownCallback = pas::bind_method<&TfShip2::MainRightButtonDown>(this);
        {
            GI_MessageLoop::TObjectGI* Ship3D = GetByName(u"Ship3D"sv);
            ShipImageCenter = EC_Struct::AddPoints(Ship3D->LocalPosition, EC_Struct::HalfPoint(Ship3D->ClientSize));
        }
        {
            GI_Zone::TZoneGI* GateZone = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"GateZone"sv));
            GateZone->MouseEnterCallback = pas::bind_method<&TfShip2::GateMouseEnter>(this);
            GateZone->MouseLeaveCallback = pas::bind_method<&TfShip2::GateMouseLeave>(this);
            GateZone->ZoneMouseUpCallback = pas::bind_method<&TfShip2::GateMouseUp>(this);
        }
        {
            GI_Zone::TZoneGI* UseZone = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"UseZone"sv));
            UseZone->MouseEnterCallback = pas::bind_method<&TfShip2::UseMouseEnter>(this);
            UseZone->MouseLeaveCallback = pas::bind_method<&TfShip2::UseMouseLeave>(this);
            UseZone->ZoneMouseUpCallback = pas::bind_method<&TfShip2::UseMouseUp>(this);
        }
        PanelSlideWidth = GR_Main::GiScalePixels(100);
        RightPanelRestLeft = GetByName(u"PanelRight"sv)->LocalPosition.X;
        DestrPanelSlideWidth = 404;
        DestrPanelRestLeft = GetByName(u"PanelDestr"sv)->LocalPosition.X;
        GateLeftRestLeft = GetByName(u"GateLeft"sv)->LocalPosition.X;
        GateRightRestLeft = GetByName(u"GateRight"sv)->LocalPosition.X;
        UseLeftRestLeft = GetByName(u"UseLeft"sv)->LocalPosition.X;
        UseRightRestLeft = GetByName(u"UseRight"sv)->LocalPosition.X;
        StoragePanelSlideHeight = -366;
        StoragePanelRestTop = GetByName(u"SC_Storage_Panel"sv)->LocalPosition.Y;
        StorageUpButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Up"sv));
        StorageUpButton->UpCallback = pas::bind_method<&TfShip2::StorageUpClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Down"sv))->UpCallback = pas::bind_method<&TfShip2::StorageDownClicked>(this);
        PropertyHintRightEdge = GetByName(u"RankWnd"sv)->LocalPosition.X;
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"FromRH"sv))->UpCallback = pas::bind_method<&TfShip2::ToggleRemoteHoldClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ToRH"sv))->UpCallback = pas::bind_method<&TfShip2::ToggleRemoteHoldClicked>(this);
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelItemRH"sv));
        Panel->FreeOwnedChildren();
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 10); cpp_range_4.next(Row); ) {
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, 4); cpp_range_5.next(Column); ) {
                RemoteHoldImages[5 * Row + Column] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                {
                    GI_Image::TImageGI* cpp_with_18 = RemoteHoldImages[5 * Row + Column];
                    cpp_with_18->SetPosition(ClassesImports::Point(Column * GR_Main::GiScalePixelsEx(42, 33), Row * GR_Main::GiScalePixelsEx(42, 33)));
                    cpp_with_18->SetSize(ClassesImports::Point(GR_Main::GiScalePixelsEx(40, 31), GR_Main::GiScalePixelsEx(40, 31)));
                    cpp_with_18->SetName(static_cast<pas::WideString>(pas::concat_ansi({"RHItem", SysUtils::IntToStr(Column + 5 * Row)})));
                    cpp_with_18->LeftButtonDownCallback = pas::bind_method<&TfShip2::RemoteHoldItemMouseDown>(this);
                }
            }
        }
        {
            GI_GraphButton::TGraphButtonGI* SortTypeRH = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SortTypeRH"sv));
            SortTypeRH->UserValue = 0;
            SortTypeRH->UpCallback = pas::bind_method<&TfShip2::SortRemoteHoldClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* SortSizeRH = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SortSizeRH"sv));
            SortSizeRH->UserValue = 1;
            SortSizeRH->UpCallback = pas::bind_method<&TfShip2::SortRemoteHoldClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* SortMoneyRH = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SortMoneyRH"sv));
            SortMoneyRH->UserValue = 2;
            SortMoneyRH->UpCallback = pas::bind_method<&TfShip2::SortRemoteHoldClicked>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UpRH"sv))->DownCallback = pas::bind_method<&TfShip2::RemoteHoldUpPressed>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UpRH"sv))->UpCallback = pas::bind_method<&TfShip2::RemoteHoldUpReleased>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"DownRH"sv))->DownCallback = pas::bind_method<&TfShip2::RemoteHoldDownPressed>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"DownRH"sv))->UpCallback = pas::bind_method<&TfShip2::RemoteHoldDownReleased>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"StorageToShip"sv))->UpCallback = pas::bind_method<&TfShip2::StorageToShipClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SellAllFromStorage"sv))->UpCallback = pas::bind_method<&TfShip2::SellStorageClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ShipToStorage"sv))->UpCallback = pas::bind_method<&TfShip2::ShipToStorageClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SortStorageByType"sv))->UpCallback = pas::bind_method<&TfShip2::SortStorageByTypeClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SortStorageBySize"sv))->UpCallback = pas::bind_method<&TfShip2::SortStorageBySizeClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SortStorageByMoney"sv))->UpCallback = pas::bind_method<&TfShip2::SortStorageByPriceClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadRocketsInHold"sv))->UpCallback = pas::bind_method<&TfShip2::LoadHoldRocketsClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SellAllFromHold"sv))->UpCallback = pas::bind_method<&TfShip2::SellHoldClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadRocketsInSlots"sv))->UpCallback = pas::bind_method<&TfShip2::LoadEquippedRocketsClicked>(this);
        SavedCaptainFrame = 0;
        RemoteHoldVisible = false;
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, 9); cpp_range_6.next(I); ) {
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(FindControlByPath(static_cast<pas::WideString>(pas::concat_ansi({"Compl", SysUtils::IntToStr(I)}))));
            if (Button != nullptr) {
                Button->DownCallback = pas::bind_method<&TfShip2::EquipmentConfigurationClicked>(this);
                Button->UpCallback = pas::bind_method<&TfShip2::EquipmentConfigurationClicked>(this);
            }
        }
        SelfSkillPointColor = GR_Main::GetStyleColorGI(u"Ship.ExpPointsToSpendOnSelf"_w, 45, 105, 124);
        OtherSkillPointColor = GR_Main::GetStyleColorGI(u"Ship.ExpPointsToSpendOnOther"_w, 180, 60, 60);
    }

    void TfShip2::OnOpen() {
        std::int32_t SlotCount{};
        pas::WideString PortraitPath{};
        pas::WideString ReservedText{};
        std::int32_t I{};
        std::int32_t J{};
        aItem::TItem* Item{};
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        ArtefactSlotZones.set_length(aConst::DefaultHullSlotCounts[aConst::sskArtefact]);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::DefaultHullSlotCounts[aConst::sskArtefact] - 1); cpp_range.next(I); ) {
            ArtefactSlotZones[I] = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(I), "z"})))));
        }
        if (ShipToInspect != nullptr) {
            PlayerHoldShip = ShipToInspect;
        } else {
            PlayerHoldShip = aPlayer::GetPlayer();
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"sv))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Gal"sv))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Quest"sv))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"sv))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Logo"sv))->SetHitTestDisabled(true);
        HighlightRepairableEquipment = false;
        GR_Main::CustomCursorEnabled = true;
        UpdateActionCursor(ReopenRequested);
        SetCursorActive(true);
        std::uint8_t SavedFlag = ReopenRequested;
        ReopenRequested = false;
        if (SavedFlag) {
            PlayerHoldShip->ScriptItemsAct(aConst::satOnReEnteringForm, nullptr, nullptr, 0);
            if (aPlayer::GetPlayer() != PlayerHoldShip) {
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnReEnteringOtherShip, nullptr, nullptr, 0);
            }
        } else {
            PlayerHoldShip->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
            if (aPlayer::GetPlayer() != PlayerHoldShip) {
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringOtherShip, nullptr, nullptr, 0);
            }
        }
        std::uint8_t ScriptChangedFlag = ReopenRequested;
        ReopenRequested = SavedFlag;
        MainPanel->OnOpen();
        if (GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle) {
            MainPanel->Hide();
        } else {
            MainPanel->Show();
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->Inventory) - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TItem>(PlayerHoldShip->Inventory, I);
            if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); weapon != nullptr && weapon->Target != nullptr) {
                if (weapon->EquippedFlag == 0) {
                    weapon->Target = nullptr;
                } else if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(weapon->Target); (!(ship != nullptr) || aGalaxy::Galaxy->IdToShip(ship->Id, false) == nullptr) && (!(pas::class_cast_if<aItem::TItem*>(weapon->Target) != nullptr) || aGalaxy::Galaxy->IdToItem(static_cast<aItem::TItem*>(weapon->Target)->Id, false) == nullptr) && (!(pas::class_cast_if<aAsteroid::TAsteroid*>(weapon->Target) != nullptr) || aGalaxy::Galaxy->IdToAsteroid(static_cast<aAsteroid::TAsteroid*>(weapon->Target)->Id) == nullptr) && (!(pas::class_cast_if<aMissile::TMissile*>(weapon->Target) != nullptr) || aGalaxy::Galaxy->IdToMissile(static_cast<aMissile::TMissile*>(weapon->Target)->Id) == nullptr)) {
                    weapon->Target = nullptr;
                }
            }
        }
        if (!ReopenRequested) {
            fShip2::RemoveEmptyPlayerHoldSlots();
        }
        if (!ReopenRequested) {
            StorageFirstSlot = 0;
        }
        aPlayer::GetPlayer()->RepairDuplicateStorageSlots(TfShip2::GetLocalStorageOwner());
        RefreshEquipmentSlotControls();
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(420);
        if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(true, 0);
        }
        BackgroundBuffer->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        PreserveSpaceMusic = Globals::ArcadeBattleScreen == ParentLoop;
        ShipStateChanged = false;
        if (!ReopenRequested) {
            SelectedHoldKind = phkEmpty;
            SelectedHoldItem = nullptr;
            HoldFirstIndex = 0;
            RemoteHoldFirstOrder = 0;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 5); cpp_range_3.next(I); ) {
            GI_Zone::TZoneGI* cpp_with = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::IntToStr(I), "z"})))));
            cpp_with->EnterCallback = pas::bind_method<&TfShip2::ShowShipPropertyInfo>(this);
            cpp_with->LeaveCallback = pas::bind_method<&TfShip2::HideShipPropertyInfo>(this);
        }
        DisplayedItemKey = 0;
        GetByName(u"PII"sv)->SetActive(false);
        GetByName(u"InfoHull"sv)->SetActive(false);
        {
            GI_Label::TLabelGI* ShipName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ShipName"sv));
            if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) {
                ShipName->SetText(reinterpret_cast<aRuins::TRuins*>(PlayerHoldShip)->GetColoredFullName(u""_wref.get()));
            } else {
                ShipName->SetText(PlayerHoldShip->GetFullName(u"\r\n"_wref.get()));
            }
            if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip); tranclucator != nullptr && tranclucator->OwnerShip == aPlayer::GetPlayer()) {
                ShipName->LeftButtonDownCallback = pas::bind_method<&TfShip2::ShipNameMouseDown>(this);
            } else {
                ShipName->LeftButtonDownCallback = nullptr;
            }
        }
        {
            GI_Label::TLabelGI* CharName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharName"sv));
            if (pas::class_cast_if<aRanger::TRanger*>(PlayerHoldShip) != nullptr) {
                CharName->SetText(pas::checked_cast<aRanger::TRanger*>(PlayerHoldShip)->GetCharacterName());
                CharName->SetActive(true);
            } else {
                CharName->SetText(u""_wref.get());
                CharName->SetActive(false);
            }
        }
        GetByName(u"RankWnd"sv)->SetActive(false);
        {
            GI_Image::TImageGI* RankI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RankI"sv));
            RankI->SetActive(true);
            if (pas::class_cast_if<aKling::TKling*>(PlayerHoldShip) != nullptr || pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr || pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr) {
                RankI->MouseEnterCallback = nullptr;
                RankI->MouseLeaveCallback = nullptr;
            } else {
                RankI->MouseEnterCallback = pas::bind_method<&TfShip2::ShowShipPropertyInfo>(this);
                RankI->MouseLeaveCallback = pas::bind_method<&TfShip2::HideShipPropertyInfo>(this);
            }
            if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(PlayerHoldShip)) {
                RankI->SetImagePath(fShip2::RankToImage(aConst::DominatorShipDefinitions[kling->KlingType].RankImageIndex));
            } else if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) {
                RankI->SetImagePath(fShip2::RankToImage(6));
            } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr) {
                RankI->SetImagePath(fShip2::RankToImage(3));
            } else if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(PlayerHoldShip); normalShip != nullptr && PlayerHoldShip->OwnerId != aGalaxyStruct::oiPirate) {
                RankI->SetImagePath(fShip2::RankToImage(normalShip->Rank));
            } else {
                RankI->SetActive(false);
            }
        }
        {
            GI_Image::TImageGI* RankAdd = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RankAdd"sv));
            if (aPlayer::GetPlayer() == PlayerHoldShip && aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
                RankAdd->SetActive(false);
            } else if (pas::class_cast_if<aRanger::TRanger*>(PlayerHoldShip) != nullptr) {
                RankAdd->MouseEnterCallback = pas::bind_method<&TfShip2::ShowShipPropertyInfo>(this);
                RankAdd->MouseLeaveCallback = pas::bind_method<&TfShip2::HideShipPropertyInfo>(this);
                RankAdd->SetActive(pas::checked_cast<aRanger::TRanger*>(PlayerHoldShip)->CanPromoteRank());
            } else {
                RankAdd->SetActive(false);
            }
        }
        {
            GI_Image::TImageGI* RankI2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RankI2"sv));
            if (pas::class_cast_if<aNormalShip::TNormalShip*>(PlayerHoldShip) != nullptr && PlayerHoldShip->OwnerId == aGalaxyStruct::oiPirate) {
                RankI2->SetActive(true);
                RankI2->MouseEnterCallback = pas::bind_method<&TfShip2::ShowShipPropertyInfo>(this);
                RankI2->MouseLeaveCallback = pas::bind_method<&TfShip2::HideShipPropertyInfo>(this);
                RankI2->SetImagePath(fShip2::PirateRankToImage(pas::checked_cast<aNormalShip::TNormalShip*>(PlayerHoldShip)->PirateRank));
            } else {
                RankI2->SetActive(false);
            }
        }
        {
            GI_MessageLoop::TObjectGI* PRankForm = GetByName(u"PRankForm"sv);
            PRankForm->SetActive(false);
        }
        {
            GI_Image::TImageGI* RankAdd2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RankAdd2"sv));
            if (pas::class_cast_if<aNormalShip::TNormalShip*>(PlayerHoldShip) != nullptr && PlayerHoldShip->OwnerId == aGalaxyStruct::oiPirate) {
                RankAdd2->SetPosition(ClassesImports::Point(193, 58));
                RankAdd2->MouseEnterCallback = pas::bind_method<&TfShip2::ShowShipPropertyInfo>(this);
                RankAdd2->MouseLeaveCallback = pas::bind_method<&TfShip2::HideShipPropertyInfo>(this);
                RankAdd2->SetActive(pas::checked_cast<aRanger::TRanger*>(PlayerHoldShip)->CanPromotePirateRank());
            } else {
                RankAdd2->SetActive(false);
            }
        }
        if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr || pas::class_cast_if<SE_Ruins::TRuinsSE*>(PlayerHoldShip->Graphic) != nullptr) {
            GetByName(u"Ship3D"sv)->SetActive(false);
            {
                GI_GraphBuf::TGraphBufGI* Ship3DBuf = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"Ship3DBuf"sv));
                PortraitPath = PlayerHoldShip->GetShipPortraitImagePath();
                Ship3DBuf->SourceHasPerPixelAlpha = true;
                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(PortraitPath), 1, u","sv), Ship3DBuf->GraphBuf);
                if (Ship3DBuf->ClientSize.X < Ship3DBuf->GraphBuf->Width || Ship3DBuf->ClientSize.Y < Ship3DBuf->GraphBuf->Height) {
                    if (static_cast<std::uint32_t>(Ship3DBuf->GraphBuf->Width) >= static_cast<std::uint32_t>(Ship3DBuf->GraphBuf->Height)) {
                        Ship3DBuf->GraphBuf->RescaleRgba(Ship3DBuf->ClientSize.X, System::Round(pas::real_divide(Ship3DBuf->ClientSize.X, static_cast<std::uint32_t>(Ship3DBuf->GraphBuf->Width)) * static_cast<std::uint32_t>(Ship3DBuf->GraphBuf->Height)), 5);
                    } else {
                        Ship3DBuf->GraphBuf->RescaleRgba(System::Round(pas::real_divide(Ship3DBuf->ClientSize.Y, static_cast<std::uint32_t>(Ship3DBuf->GraphBuf->Height)) * static_cast<std::uint32_t>(Ship3DBuf->GraphBuf->Width)), Ship3DBuf->ClientSize.Y, 5);
                    }
                }
                Ship3DBuf->SetImageKindX(GI_Main::ikxCenter);
                Ship3DBuf->SetImageKindY(GI_Main::ikyCenter);
                Ship3DBuf->SetActive(true);
            }
        } else {
            GetByName(u"Ship3DBuf"sv)->SetActive(false);
            {
                GI_Image::TImageGI* Ship3D = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Ship3D"sv));
                Ship3D->SetImagePath(PlayerHoldShip->GetShipPortraitImagePath());
                Ship3D->SetImageKindX(GI_Main::ikxCenter);
                Ship3D->SetImageKindY(GI_Main::ikyCenter);
                {
                    Types::TPoint visualCenter = Ship3D->GetVisualCenter();
                    Types::TPoint shipImageCenter = ShipImageCenter;
                    Ship3D->SetPosition(EC_Struct::SubtractPoints(shipImageCenter, visualCenter));
                }
                Ship3D->SetActive(true);
            }
        }
        {
            GI_Zone::TZoneGI* S_Hull_0z = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"S_Hull_0z"sv));
            S_Hull_0z->ZoneMouseUpCallback = pas::bind_method<&TfShip2::HullMouseDown>(this);
        }
        GetByName(u"S_Left"sv)->SetActive(false);
        GetByName(u"S_Right"sv)->SetActive(false);
        GetByName(u"S_Left"sv)->SetActive(true);
        GetByName(u"S_Right"sv)->SetActive(true);
        {
            GI_Image::TImageGI* CaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"sv));
            CaptainI->SetImagePath(pas::concat_wide({u"GI,", aShip::TShip_GetCaptainPortraitResourceBase(PlayerHoldShip), u"i"}));
            CaptainI->SetImageKindX(GI_Main::ikxCenter);
            CaptainI->SetImageKindY(GI_Main::ikyCenter);
            CaptainI->SetActive(true);
        }
        {
            GI_GAI::TgaiGI* CaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"sv));
            CaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            CaptainA->SetImagePath(pas::concat_wide({aShip::TShip_GetCaptainPortraitResourceBase(PlayerHoldShip), u"a"}));
            CaptainA->SequenceIndex = 0;
            CaptainA->UpdateAutoGeometry();
            CaptainA->SetImageKindX(GI_Main::ikxCenter);
            CaptainA->SetImageKindY(GI_Main::ikyCenter);
            if (SavedCaptainFrame >= 0 && CaptainA->SequenceFrameCount > SavedCaptainFrame) {
                CaptainA->SetSequenceFrame(SavedCaptainFrame);
            } else {
                CaptainA->SetSequenceFrame(0);
            }
            CaptainA->SetActive(true);
            CaptainA->RestartPlayback();
        }
        RefreshRewards(reinterpret_cast<aNormalShip::TNormalShip*>(PlayerHoldShip));
        SavedShipExperience = -1;
        {
            GI_Label::TLabelGI* TextPoints = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TextPoints"sv));
            if (TfShip2::CanUsePlayerExperience()) {
                TextPoints->SetText(aConst::LocalizedColorText(u"FormShip.PlayerPoints"_wref.get()));
                TextPoints->SetTextColor(OtherSkillPointColor);
                SavedShipExperience = PlayerHoldShip->FreeExperience;
                PlayerHoldShip->FreeExperience = aPlayer::GetPlayer()->FreeExperience;
            } else {
                TextPoints->SetText(aConst::LocalizedColorText(u"FormShip.Points"_wref.get()));
                TextPoints->SetTextColor(SelfSkillPointColor);
            }
        }
        {
            GI_Label::TLabelGI* LNewExp1 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LNewExp1"sv));
            if (TfShip2::CanUsePlayerExperience()) {
                LNewExp1->SetText(aConst::LocalizedColorText(u"FormShip.LNewExp1b"_wref.get()));
            } else {
                LNewExp1->SetText(aConst::LocalizedColorText(u"FormShip.LNewExp1"_wref.get()));
            }
        }
        {
            GI_Label::TLabelGI* LNewExp2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LNewExp2"sv));
            if (TfShip2::CanUsePlayerExperience()) {
                LNewExp2->SetText(aConst::LocalizedColorText(u"FormShip.LNewExp2b"_wref.get()));
            } else {
                LNewExp2->SetText(aConst::LocalizedColorText(u"FormShip.LNewExp2"_wref.get()));
            }
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(422);
        RefreshShipView();
        InvalidateViewport();
        DrawQueuedUpdateRects();
        if (RightPanelSlideTimer != nullptr) {
            CancelCallbackTimer(RightPanelSlideTimer);
            RightPanelSlideTimer = nullptr;
        }
        if (static_cast<std::uint8_t>(ReopenRequested ^ 1) || RemoteHoldMode) {
            {
                GI_MessageLoop::TObjectGI* PanelRight = GetByName(u"PanelRight"sv);
                PanelRight->SetPosition(ClassesImports::Point(PanelSlideWidth, PanelRight->LocalPosition.Y));
                PanelRight->SetActive(static_cast<std::uint8_t>(RemoteHoldMode ^ 1));
            }
            {
                GI_MessageLoop::TObjectGI* PanelLH = GetByName(u"PanelLH"sv);
                PanelLH->SetActive(static_cast<std::uint8_t>(RemoteHoldMode ^ 1));
            }
        } else {
            {
                GI_MessageLoop::TObjectGI* PanelRight_2 = GetByName(u"PanelRight"sv);
                PanelRight_2->SetPosition(ClassesImports::Point(RightPanelRestLeft, PanelRight_2->LocalPosition.Y));
                PanelRight_2->SetActive(true);
            }
            {
                GI_MessageLoop::TObjectGI* PanelLH_2 = GetByName(u"PanelLH"sv);
                PanelLH_2->SetActive(true);
            }
        }
        if (static_cast<std::uint8_t>(ReopenRequested ^ 1) || static_cast<std::uint8_t>(RemoteHoldMode ^ 1)) {
            {
                GI_MessageLoop::TObjectGI* PanelRH = GetByName(u"PanelRH"sv);
                PanelRH->SetPosition(ClassesImports::Point(PanelSlideWidth, PanelRH->LocalPosition.Y));
                PanelRH->SetActive(RemoteHoldMode);
            }
            {
                GI_MessageLoop::TObjectGI* PanelDS = GetByName(u"PanelDS"sv);
                PanelDS->SetActive(RemoteHoldMode);
            }
        } else {
            {
                GI_MessageLoop::TObjectGI* PanelRH_2 = GetByName(u"PanelRH"sv);
                PanelRH_2->SetPosition(ClassesImports::Point(RightPanelRestLeft, PanelRH_2->LocalPosition.Y));
                PanelRH_2->SetActive(true);
            }
            {
                GI_MessageLoop::TObjectGI* PanelDS_2 = GetByName(u"PanelDS"sv);
                PanelDS_2->SetActive(true);
            }
        }
        {
            GI_MessageLoop::TObjectGI* PanelDestr = GetByName(u"PanelDestr"sv);
            if (!ReopenRequested) {
                PanelDestr->SetPosition(ClassesImports::Point(DestrPanelSlideWidth, PanelDestr->LocalPosition.Y));
            } else {
                PanelDestr->SetPosition(ClassesImports::Point(DestrPanelRestLeft, PanelDestr->LocalPosition.Y));
            }
            PanelDestr->SetActive(static_cast<std::uint8_t>(RemoteHoldMode ^ 1) && aPlayer::GetPlayer() == PlayerHoldShip && aPlayer::GetPlayer()->GetHull()->CapitalShip == 1);
        }
        {
            GI_MessageLoop::TObjectGI* DestrInto = GetByName(u"DestrInto"sv);
            DestrInto->SetActive(aPlayer::GetPlayer() == PlayerHoldShip && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && aPlayer::GetPlayer()->RuinsMode == 0 && pas::list_count(aScript::QueuedArcadeBattles) <= 0);
        }
        {
            GI_GraphButton::TGraphButtonGI* CustomBridgeInto = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"CustomBridgeInto"sv));
            CustomBridgeInto->SetActive(aPlayer::GetPlayer() == PlayerHoldShip && aPlayer::GetPlayer()->GetHull()->CapitalShip > 1 && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && aPlayer::GetPlayer()->RuinsMode == 0 && pas::list_count(aScript::QueuedArcadeBattles) <= 0);
        }
        if (!ReopenRequested) {
            RightPanelSlideStep = 20;
            RightPanelSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideRightPanelTimer>(this), 0);
        }
        if (NativeTimer384 != nullptr) {
            CancelCallbackTimer(NativeTimer384);
            NativeTimer384 = nullptr;
        }
        RefreshTimerTick();
        NativeTimer384 = ScheduleCallbackTimer(1, 1, pas::bind_method<&TfShip2::ShowItemInfoTimer>(this), 0);
        MoneyWarningVisible = false;
        RefreshMoneyWarning();
        if (pas::real_divide(PlayerHoldShip->GetHull()->HullPoints, PlayerHoldShip->GetHull()->Weight) > 0.2L) {
            GetByName(u"CenterNormalImage"sv)->SetActive(true);
            {
                GI_GAI::TgaiGI* CenterNormalAnim = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CenterNormalAnim"sv));
                CenterNormalAnim->SetActive(true);
                CenterNormalAnim->RestartPlayback();
            }
            GetByName(u"CenterDamageImage"sv)->SetActive(false);
            GetByName(u"CenterDamageAnim"sv)->SetActive(false);
        } else {
            GetByName(u"CenterNormalImage"sv)->SetActive(false);
            GetByName(u"CenterNormalAnim"sv)->SetActive(false);
            GetByName(u"CenterDamageImage"sv)->SetActive(true);
            {
                GI_GAI::TgaiGI* CenterDamageAnim = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CenterDamageAnim"sv));
                CenterDamageAnim->SetActive(true);
                CenterDamageAnim->RestartPlayback();
            }
        }
        if (ReopenRequested) {
            if (StorageUpButton->Active) {
                StorageDownClicked(nullptr);
            } else {
                StorageUpClicked(nullptr);
            }
        } else {
            GateSlideOffset = 0;
            {
                GI_MessageLoop::TObjectGI* GateLeft = GetByName(u"GateLeft"sv);
                GateLeft->SetPosition(ClassesImports::Point(GateLeftRestLeft, GateLeft->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* GateRight = GetByName(u"GateRight"sv);
                GateRight->SetPosition(ClassesImports::Point(GateRightRestLeft, GateRight->LocalPosition.Y));
            }
            if (GateSlideTimer != nullptr) {
                CancelCallbackTimer(GateSlideTimer);
                GateSlideTimer = nullptr;
            }
            UseSlideOffset = 0;
            {
                GI_MessageLoop::TObjectGI* UseLeft = GetByName(u"UseLeft"sv);
                UseLeft->SetPosition(ClassesImports::Point(UseLeftRestLeft, UseLeft->LocalPosition.Y));
            }
            {
                GI_MessageLoop::TObjectGI* UseRight = GetByName(u"UseRight"sv);
                UseRight->SetPosition(ClassesImports::Point(UseRightRestLeft, UseRight->LocalPosition.Y));
            }
            if (UseSlideTimer != nullptr) {
                CancelCallbackTimer(UseSlideTimer);
                UseSlideTimer = nullptr;
            }
            UsePanelSlideOffset = 0;
            {
                GI_MessageLoop::TObjectGI* UsePanel = GetByName(u"UsePanel"sv);
                UsePanel->SetPosition(ClassesImports::Point(UsePanel->ClientSize.X, UsePanel->LocalPosition.Y));
            }
            if (UsePanelSlideTimer != nullptr) {
                CancelCallbackTimer(UsePanelSlideTimer);
                UsePanelSlideTimer = nullptr;
            }
            pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot1_Anim"sv))->SetSequenceFrame(0);
            pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot2_Anim"sv))->SetSequenceFrame(0);
            pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot3_Anim"sv))->SetSequenceFrame(0);
            if (SpecialSlot1Timer != nullptr) {
                CancelCallbackTimer(SpecialSlot1Timer);
                SpecialSlot1Timer = nullptr;
            }
            if (SpecialSlot2Timer != nullptr) {
                CancelCallbackTimer(SpecialSlot2Timer);
                SpecialSlot2Timer = nullptr;
            }
            if (SpecialSlot3Timer != nullptr) {
                CancelCallbackTimer(SpecialSlot3Timer);
                SpecialSlot3Timer = nullptr;
            }
            StorageSlideOffset = 0;
            {
                GI_MessageLoop::TObjectGI* SC_Storage_Panel = GetByName(u"SC_Storage_Panel"sv);
                SC_Storage_Panel->SetPosition(ClassesImports::Point(SC_Storage_Panel->LocalPosition.X, StoragePanelSlideHeight));
            }
            StorageUpButton->SetActive(false);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Down"sv))->SetActive(true);
            if (StorageSlideTimer != nullptr) {
                CancelCallbackTimer(StorageSlideTimer);
                StorageSlideTimer = nullptr;
            }
            if (!aPlayer::GetPlayer()->HasAccessibleStorageAt(nullptr)) {
                StorageDownClicked(nullptr);
            }
            {
                GI_Panel::TPanelGI* SC_Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"SC_Panel"sv));
                SC_Panel->SetPosition(ClassesImports::Point(0, SC_Panel->LocalPosition.Y));
            }
        }
        {
            GI_GAI::TgaiGI* HullRepair = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"HullRepair"sv));
            if (HullRepair->Active) {
                if (!PlayServiceAnimations) {
                    HullRepair->SetActive(false);
                } else {
                    HullRepair->SetSequenceFrame(0);
                    HullRepair->CycleCompleteCallback = pas::bind_static_method<&TfShip2::HideSender>(this);
                    HullRepair->SetActive(true);
                    HullRepair->RestartPlayback();
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 7); cpp_range_4.next(I); ) {
            SlotCount = 1;
            if (aConst::EquipmentSlotLayouts[I].ItemType == aConst::t_Weapon1) {
                SlotCount = 5;
            }
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_5.next(J); ) {
                GI_GAI::TgaiGI* cpp_with_38 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(J), u"Repair"}))));
                if (cpp_with_38->Active) {
                    if (!PlayServiceAnimations) {
                        cpp_with_38->SetActive(false);
                    } else {
                        cpp_with_38->SetSequenceFrame(0);
                        cpp_with_38->CycleCompleteCallback = pas::bind_static_method<&TfShip2::HideSender>(this);
                        cpp_with_38->SetActive(true);
                        cpp_with_38->RestartPlayback();
                    }
                }
            }
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, aConst::DefaultHullSlotCounts[aConst::sskArtefact] - 1); cpp_range_6.next(J); ) {
            Control = FindControlByPath(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(J), "Repair"})));
            if (Control != nullptr) {
                Control->SetActive(false);
            }
        }
        RefreshLoadEquippedRocketsButton();
        GetByName(u"SC_Panel"sv)->SetActive(TfShip2::CanUseLocalStorage());
        GR_Main::CustomCursorEnabled = true;
        UpdateActionCursor(ReopenRequested);
        SetCursorActive(true);
        ShipLoopSound->SetVolume(1.0f);
        PlayServiceAnimations = false;
        ReopenRequested = ScriptChangedFlag;
        MainPanel->RebuildMessageButtons(false);
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(501);
        if (!ReopenRequested) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum2(502);
        }
    }

    void TfShip2::OnClose() {
        std::int32_t I{};
        aScript::TScriptShip* Binding{};
        aGalaxy::Galaxy->CheckIntegrityChecksum1(503);
        if (!ReopenRequested) {
            aGalaxy::Galaxy->CheckIntegrityChecksum2(504);
        }
        GI_MessageLoop::TMessageLoopGI::OnClose();
        if (ShipToInspect != nullptr) {
            PlayerHoldShip = ShipToInspect;
        } else {
            PlayerHoldShip = aPlayer::GetPlayer();
        }
        if (!ReopenRequested) {
            PlayerHoldShip->ScriptItemsAct(0x00000019, nullptr, nullptr, 0);
            if (aPlayer::GetPlayer() != PlayerHoldShip) {
                aPlayer::GetPlayer()->ScriptItemsAct(0x0000001c, nullptr, nullptr, 0);
            }
        }
        BackgroundBuffer->GraphBuf->Clear();
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
        StopScriptVideo();
        if (SpecialSlot1Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot1Timer);
            SpecialSlot1Timer = nullptr;
        }
        if (SpecialSlot2Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot2Timer);
            SpecialSlot2Timer = nullptr;
        }
        if (SpecialSlot3Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot3Timer);
            SpecialSlot3Timer = nullptr;
        }
        if (GateSlideTimer != nullptr) {
            CancelCallbackTimer(GateSlideTimer);
            GateSlideTimer = nullptr;
        }
        if (UseSlideTimer != nullptr) {
            CancelCallbackTimer(UseSlideTimer);
            UseSlideTimer = nullptr;
        }
        if (UsePanelSlideTimer != nullptr) {
            CancelCallbackTimer(UsePanelSlideTimer);
            UsePanelSlideTimer = nullptr;
        }
        if (StorageSlideTimer != nullptr) {
            CancelCallbackTimer(StorageSlideTimer);
            StorageSlideTimer = nullptr;
        }
        if (MoneyWarningTimer != nullptr) {
            CancelCallbackTimer(MoneyWarningTimer);
            MoneyWarningTimer = nullptr;
        }
        if (NativeTimer384 != nullptr) {
            CancelCallbackTimer(NativeTimer384);
            NativeTimer384 = nullptr;
        }
        if (ItemInfoHideTimer != nullptr) {
            CancelCallbackTimer(ItemInfoHideTimer);
            ItemInfoHideTimer = nullptr;
        }
        if (PropertyInfoHideTimer != nullptr) {
            CancelCallbackTimer(PropertyInfoHideTimer);
            PropertyInfoHideTimer = nullptr;
        }
        if (!ReopenRequested) {
            ReturnSelectedHoldEntry();
        }
        if (RightPanelSlideTimer != nullptr) {
            CancelCallbackTimer(RightPanelSlideTimer);
            RightPanelSlideTimer = nullptr;
        }
        Globals::ScriptUseItem = nullptr;
        aGalaxy::Galaxy->CheckIntegrityChecksum1(423);
        if (SavedShipExperience >= 0 && TfShip2::CanUsePlayerExperience()) {
            aPlayer::GetPlayer()->FreeExperience = PlayerHoldShip->FreeExperience;
            PlayerHoldShip->FreeExperience = SavedShipExperience;
            SavedShipExperience = -1;
        }
        PlayerHoldShip->RefreshDerivedStats(true);
        if (PlayerHoldShip->Order == aShip::soJump) {
            if (PlayerHoldShip->JumpRange < System::Round(aMyFunction::PointDistance(PlayerHoldShip->CurrentStar->Position, reinterpret_cast<aGalaxy::TStar*>(PlayerHoldShip->OrderTarget)->Position))) {
                PlayerHoldShip->OrderNone(false);
            }
        }
        if (aPlayer::GetPlayer()->ScriptShipBindings != nullptr) {
            I = pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1;
            while (I >= 0) {
                if (I >= pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings)) {
                    I = pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1;
                } else {
                    Binding = pas::list_at<aScript::TScriptShip>(aPlayer::GetPlayer()->ScriptShipBindings, I);
                    if (Binding->Script != nullptr) {
                        aScript::TScript_RunShipState(Binding->Script, Binding);
                    }
                    --I;
                }
            }
        }
        aPlayer::GetPlayer()->RefreshStorageBubbles();
        if (!ReopenRequested) {
            ShipLoopSound->SetVolume(0.0f);
            PlayerHoldShip = nullptr;
            ShipToInspect = nullptr;
        }
        GR_Main::CustomCursorEnabled = static_cast<std::uint8_t>(ReopenRequested ^ 1);
        SavedCaptainFrame = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"sv))->SequenceFrame;
        ShipStateChanged = true;
        RemoteHoldVisible = false;
        MainPanel->OnClose();
    }

    // Native Self/result stack ordering establishes this as a method.
    std::uint8_t TfShip2::CanUseLocalStorage() {
        return (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited || aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->RuinsMode == 0) && pas::list_count(aScript::QueuedArcadeBattles) <= 0;
    }

    // Borrows the current planet or docked ship.
    pas::Object* TfShip2::GetLocalStorageOwner() {
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            return aPlayer::GetPlayer()->CurrentPlanet;
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            return aPlayer::GetPlayer()->DockedTo;
        } else {
            return nullptr;
        }
    }

    void TfShip2::RefreshRewards(aNormalShip::TNormalShip* Ship) {
        std::int32_t Index{};
        std::int32_t I{};
        GR_GraphBuf::TGraphBufGR* Icon{};
        GR_GraphBuf::TGraphBufGR* Shadow{};
        std::uint8_t Award{};
        pas::WideString Path{};
        std::int32_t IconSize{};
        float Spacing{};
        std::int32_t VisibleCount{};
        std::int32_t Count{};
        HideRewardTooltip();
        if (Ship->AwardIds == nullptr || pas::list_count(Ship->AwardIds) < 1) {
            // The value expression preserves the native receiver-before-argument order.
            reinterpret_cast<GI_GraphBuf::TGraphBufGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(RewardsBuffer)) + 0)))->SetActive(false);
        } else {
            Count = std::min<std::int32_t>(Ship->AwardVisibleCount, pas::list_count(Ship->AwardIds));
            IconSize = GR_Main::GiScalePixels(20);
            VisibleCount = pas::idiv(RewardsBuffer->ClientSize.X - 2, IconSize);
            if (Count <= VisibleCount) {
                Spacing = IconSize;
            } else {
                VisibleCount = std::min<std::int32_t>(30, Count);
                Spacing = pas::real_divide(RewardsBuffer->ClientSize.X - 2 - IconSize, VisibleCount - 1);
            }
            {
                GI_GraphBuf::TGraphBufGI* cpp_with = RewardsBuffer;
                cpp_with->SetActive(true);
                cpp_with->SetImageKindX(GI_Main::ikxLeft);
                cpp_with->SetImageKindY(GI_Main::ikyBottom);
                cpp_with->GraphBuf->AllocateRgbaTight(std::max<std::int64_t>(static_cast<std::int64_t>(cpp_with->ClientSize.X), System::Round(static_cast<long double>(VisibleCount) * Spacing + IconSize - Spacing)) + 2, IconSize + 2);
                cpp_with->MouseMoveCallback = pas::bind_method<&TfShip2::RewardsMouseMove>(this);
                cpp_with->MouseLeaveCallback = pas::bind_method<&TfShip2::RewardsMouseLeave>(this);
                cpp_with->LeftButtonDownCallback = pas::bind_method<&TfShip2::RewardsMouseDown>(this);
                cpp_with->GraphBuf->ClearPixels();
                cpp_with->SourceHasPerPixelAlpha = true;
            }
            Icon = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Shadow = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Index = std::max<std::int32_t>(0, Count - VisibleCount);
            I = 0;
            while (Index < Count) {
                Award = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->AwardIds, Index)));
                if (Award < 10) {
                    Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_0", pas::wide_int_to_str(static_cast<std::int32_t>(Award))});
                } else {
                    Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_", pas::wide_int_to_str(static_cast<std::int32_t>(Award))});
                }
                GI_GI::LoadGiByPathIntoGraphBuf(Path, Icon);
                if (static_cast<std::uint32_t>(Icon->Width) >= static_cast<std::uint32_t>(Icon->Height)) {
                    Icon->RescaleRgba(IconSize, System::Round(pas::real_divide(IconSize, static_cast<std::uint32_t>(Icon->Width)) * static_cast<std::uint32_t>(Icon->Height)), 5);
                } else {
                    Icon->RescaleRgba(System::Round(pas::real_divide(IconSize, static_cast<std::uint32_t>(Icon->Height)) * static_cast<std::uint32_t>(Icon->Width)), IconSize, 5);
                }
                Shadow->AllocateRgbaTight(Icon->Width, Icon->Height);
                GR_GraphBuf::TGraphBufGR_CopyRect32(Shadow, ClassesImports::Point(0, 0), Icon, ClassesImports::Rect(0, 0, Icon->Width, Icon->Height));
                GR_GraphBuf::TGraphBufGR_MakeShadow(Shadow);
                if (!(Icon->Height > IconSize || RewardsBuffer->GraphBuf->Width < System::Round(static_cast<long double>(I) * Spacing) + Icon->Width)) {
                    GR_GraphBuf::TGraphBufGR_BlendRect32(RewardsBuffer->GraphBuf, ClassesImports::Point(System::Round(static_cast<long double>(I) * Spacing) + 2, 2), Shadow, ClassesImports::Rect(0, 0, Icon->Width, Icon->Height));
                    GR_GraphBuf::TGraphBufGR_BlendRect32(RewardsBuffer->GraphBuf, ClassesImports::Point(System::Round(static_cast<long double>(I) * Spacing), 0), Icon, ClassesImports::Rect(0, 0, Icon->Width, Icon->Height));
                }
                ++Index;
                ++I;
            }
            pas::free(Icon);
            pas::free(Shadow);
        }
    }

    void TfShip2::RewardsMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t IconSize{};
        float Spacing{};
        std::int32_t VisibleCount{};
        aNormalShip::TNormalShip* Ship{};
        std::int32_t Index{};
        std::int32_t I{};
        std::int32_t X{};
        std::uint8_t Award{};
        std::int32_t Count{};
        if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(PlayerHoldShip)) {
            Ship = normalShip;
            Count = Ship->AwardVisibleCount;
            IconSize = GR_Main::GiScalePixels(20);
            VisibleCount = pas::idiv(RewardsBuffer->ClientSize.X - 2, IconSize);
            if (Count <= VisibleCount) {
                Spacing = IconSize;
            } else {
                VisibleCount = std::min<std::int32_t>(30, Count);
                Spacing = pas::real_divide(RewardsBuffer->ClientSize.X - 2 - IconSize, VisibleCount - 1);
            }
            X = Sender->ToLocalPoint(Point).X;
            Index = std::max<std::int32_t>(0, Count - VisibleCount);
            I = 0;
            while (Index < Count) {
                if (X >= System::Round(static_cast<long double>(I) * Spacing) && X < System::Round(static_cast<long double>(I + 1) * Spacing)) {
                    break;
                }
                ++I;
                ++Index;
            }
            if (Index >= Count) {
                Index = Count - 1;
            }
            Award = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->AwardIds, Index)));
            ShowRewardTooltip(Ship, Award);
        }
    }

    void TfShip2::RewardsMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        HideRewardTooltip();
    }

    void TfShip2::ShowRewardTooltip(aNormalShip::TNormalShip* Ship, std::int32_t Award) {
        aConst::TRewardInfo cpp_result{};
        aConst::TRewardInfo cpp_result_2{};
        pas::WideString Path{};
        if (SelectedReward != Award) {
            SelectedReward = Award;
            RewardsWindow->SetActive(true);
            if (Award < 10) {
                Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_0", pas::wide_int_to_str(Award)});
            } else {
                Path = pas::concat_wide({u"Bm.FormRewards.", GR_Main::GiResourceSuffix(), u"_", pas::wide_int_to_str(Award)});
            }
            {
                GI_GraphBuf::TGraphBufGI* RewardImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RewardImage"sv));
                RewardImage->SourceHasPerPixelAlpha = true;
                GI_GI::LoadGiByPathIntoGraphBuf(Path, RewardImage->GraphBuf);
                if (static_cast<std::uint32_t>(RewardImage->GraphBuf->Width) >= static_cast<std::uint32_t>(RewardImage->GraphBuf->Height)) {
                    RewardImage->GraphBuf->RescaleRgba(RewardImage->ClientSize.X, System::Round(pas::real_divide(RewardImage->ClientSize.X, static_cast<std::uint32_t>(RewardImage->GraphBuf->Width)) * static_cast<std::uint32_t>(RewardImage->GraphBuf->Height)), 5);
                } else {
                    RewardImage->GraphBuf->RescaleRgba(System::Round(pas::real_divide(RewardImage->ClientSize.Y, static_cast<std::uint32_t>(RewardImage->GraphBuf->Height)) * static_cast<std::uint32_t>(RewardImage->GraphBuf->Width)), RewardImage->ClientSize.Y, 5);
                }
                RewardImage->SetImageKindX(GI_Main::ikxCenter);
                RewardImage->SetImageKindY(GI_Main::ikyCenter);
            }
            {
                GI_Label::TLabelGI* RewardName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"sv));
                RewardName->SetText((aNormalShip::TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name);
            }
            {
                GI_Label::TLabelGI* RewardText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"sv));
                RewardText->SetText((aNormalShip::TNormalShip::GetAwardInfo(Award, cpp_result_2), cpp_result_2).Text);
            }
            {
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardText"sv));
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"sv));
                TfShip2::LayoutItemInfo(RewardsWindow, cpp_arg_2, cpp_arg, true, true, 0);
            }
            {
                GI_Label::TLabelGI* RewardName_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RewardName"sv));
                RewardName_2->SetSize(ClassesImports::Point(RewardsWindow->ClientSize.X - RewardName_2->LocalPosition.X - RewardsWindow->WorkSubRect.Right, RewardName_2->ClientSize.Y));
            }
            RewardsWindow->SetPosition(ClassesImports::Point(GR_Main::ExtraScreenWidth / 2 + 680 - RewardsWindow->ClientSize.X, GR_Main::ExtraScreenHeight / 2 + 160));
            {
                GI_Window::TWindowGI* cpp_with_5 = RewardsWindow;
                cpp_with_5->Invalidate();
                cpp_with_5->UpdateAbsolutePosition();
                cpp_with_5->UpdateSubtreeHitBounds();
                cpp_with_5->Invalidate();
            }
            TfShip2::UpdateInfoHint(0, 0);
        }
    }

    void TfShip2::HideRewardTooltip() {
        SelectedReward = -1;
        RewardsWindow->SetActive(false);
    }

    void TfShip2::SlideRightPanelTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_MessageLoop::TObjectGI* Panel{};
        if (!RemoteHoldMode) {
            Panel = GetByName(u"PanelRight"sv);
        } else {
            Panel = GetByName(u"PanelRH"sv);
        }
        GI_MessageLoop::TObjectGI* DestrPanel = GetByName(u"PanelDestr"sv);
        std::int32_t X = Panel->LocalPosition.X + RightPanelSlideStep;
        if (X >= RightPanelRestLeft) {
            X = RightPanelRestLeft;
            if (RightPanelSlideTimer != nullptr) {
                CancelCallbackTimer(RightPanelSlideTimer);
                RightPanelSlideTimer = nullptr;
            }
            RootUiObject->UpdateAbsolutePosition();
            RootUiObject->UpdateSubtreeHitBounds();
        }
        DestrPanel->SetPosition(ClassesImports::Point(DestrPanelRestLeft - RightPanelRestLeft + X, DestrPanel->LocalPosition.Y));
        Panel->SetPosition(ClassesImports::Point(X, Panel->LocalPosition.Y));
    }

    // Checks integrity outside inspection mode, restores the return screen, closes and raises BreakUiMessage.
    void TfShip2::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (!ReopenRequested) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(427);
            ReturnSelectedHoldEntry();
        }
        GR_Main::AuxRenderBuffer->Clear();
        GlobalsV::RequestedScreenId = GlobalsV::ShipReturnScreenId;
        RequestClose(1);
        GI_Main::BreakUiMessage();
    }

    void TfShip2::RewardsMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (GlobalsV::AwardDialogsEnabled && static_cast<std::uint8_t>(PlayerHoldShip->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0 && RewardsBuffer->Active) {
            if (SelectedHoldKind != phkEmpty) {
                aGalaxy::Galaxy->CheckIntegrityChecksum1(427);
                ReturnSelectedHoldEntry();
            }
            RewardsWindow->SetActive(false);
            Globals::AwardSubject = PlayerHoldShip;
            aGalaxy::Galaxy->CheckIntegrityChecksum1(333);
            if (!fRewards::RunRewards(this, false)) {
                aGalaxy::Galaxy->CheckIntegrityChecksum1(334);
                RequestClose(2);
            } else {
                aGalaxy::Galaxy->CheckIntegrityChecksum1(334);
                ShipStateChanged = true;
                ReopenRequested = true;
                PlayTransitionSounds = false;
                CloseClicked(nullptr);
            }
        }
    }

    void TfShip2::ShipNameMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        pas::WideString Name{};
        if (static_cast<std::uint8_t>(PlayerHoldShip->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0) {
            Name = PlayerHoldShip->GetName();
            if (fTextBox::ShowTextInputDialog(this, aConst::LocalizedColorText(u"FormShip.EnterShipName"_wref.get()), Name, 30, 0, 0) == 1) {
                aGalaxy::Galaxy->CheckIntegrityChecksum1(430);
                PlayerHoldShip->Name = Name;
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ShipName"sv))->SetText(Name);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(431);
            }
        }
    }

    void TfShip2::ShowShipPropertyInfo(GI_MessageLoop::TObjectGI* Sender) {
        aNormalShip::TNormalShip* Ship{};
        pas::WideString Path{};
        pas::WideString Title{};
        pas::WideString Description{};
        pas::WideString CustomDescription{};
        pas::WideString CustomName{};
        aGalaxyStruct::TPilotSkill Skill{};
        GI_Window::TWindowGI* Window{};
        WindowsSdk::TPoint Position{};
        aShip::PCustomShipInfo Info{};
        if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(PlayerHoldShip)) {
            Ship = normalShip;
        } else {
            Ship = nullptr;
        }
        std::uint8_t Afterburner = false;
        if (RightPanelSlideTimer == nullptr) {
            Path = pas::WideString();
            Description = pas::WideString();
            Position = ClassesImports::Point(10, 10);
            if (Sender->UserValue == -1) {
                if (Sender->UserData != 0) {
                    Info = reinterpret_cast<aShip::PCustomShipInfo>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserData)));
                    aScript::RunCustomShipInfoActionCode(Info, aConst::satOnShowingItemInfo, PlayerHoldShip, nullptr, nullptr, 0);
                    CustomDescription = Info->Description;
                    if (CustomDescription == u"") {
                        CustomDescription = aConst::LocalizedColorText(pas::concat_wide({u"ShipInfo.AddInfo.CustomInfos.", Info->TypeName, u".Description"}));
                    }
                    aMyFunction::ReplaceTextToken(CustomDescription, u"<Data1>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomDescription, u"<Data2>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomDescription, u"<Data3>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomDescription, u"<TextData1>"_w, Info->TextData1, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomDescription, u"<TextData2>"_w, Info->TextData2, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomDescription, u"<TextData3>"_w, Info->TextData3, u"<color=255,240,100>"_w);
                    CustomName = aConst::LocalizedColorText(pas::concat_wide({u"ShipInfo.AddInfo.CustomInfos.", Info->TypeName, u".Name"}));
                    aMyFunction::ReplaceTextToken(CustomName, u"<Data1>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomName, u"<Data2>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomName, u"<Data3>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomName, u"<TextData1>"_w, Info->TextData1, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomName, u"<TextData2>"_w, Info->TextData2, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(CustomName, u"<TextData3>"_w, Info->TextData3, u"<color=255,240,100>"_w);
                    Sender->HelpText = pas::concat_wide({CustomName, u"~", CustomDescription});
                }
                {
                    GI_GraphBuf::TGraphBufGI* RankImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"sv));
                    RankImage->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"AI_", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Sender->UserIndex))), u"L"}), RankImage->GraphBuf);
                    RankImage->SetImageKindX(GI_Main::ikxCenter);
                    RankImage->SetImageKindY(GI_Main::ikyCenter);
                }
                Title = EC_Str::ExtractDelimitedPartW(pas::view(Sender->HelpText), 0, u"~"sv);
                Description = EC_Str::ExtractDelimitedRangeW(pas::view(Sender->HelpText), 1, EC_Str::CountDelimitedPartsW(pas::view(Sender->HelpText), u"~"sv) - 1, u"~"sv);
            } else if (GetByName(u"ForsageBut"sv) == Sender) {
                Afterburner = true;
                {
                    GI_GraphBuf::TGraphBufGI* RankImage_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"sv));
                    RankImage_2->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"ForsageIcon"}), RankImage_2->GraphBuf);
                    RankImage_2->SetImageKindX(GI_Main::ikxCenter);
                    RankImage_2->SetImageKindY(GI_Main::ikyCenter);
                }
                Title = aConst::LocalizedColorText(u"ShipInfo.Forsage.Name"_wref.get());
                Description = aConst::LocalizedColorText(u"ShipInfo.Forsage.Text"_wref.get());
                if (GlobalsV::DynamicTipsPos) {
                    Position = ClassesImports::Point(Sender->HitTestBounds.Left - 10, Sender->HitTestBounds.Top + Sender->ClientSize.Y + 10);
                }
            } else if (pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr) {
                if (Ship == nullptr) {
                    return;
                }
                if (Sender->ControlName == u"RankI" || Sender->ControlName == u"RankAdd") {
                    Path = EC_Str::ExtractDelimitedPartW(pas::view(fShip2::RankToImageSmall(Ship->Rank)), 1, u","sv);
                    {
                        GI_GraphBuf::TGraphBufGI* RankImage_3 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"sv));
                        RankImage_3->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(Path, RankImage_3->GraphBuf);
                        RankImage_3->SetImageKindX(GI_Main::ikxCenter);
                        RankImage_3->SetImageKindY(GI_Main::ikyCenter);
                    }
                    Title = ([&] {
                        pas::WideString rankLongName = Ship->GetRankLongName();
                        pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(pas::view(std::move(rankLongName)), pas::view(std::move(infoNameColorTag)));
                    }());
                    Description = Ship->GetRankDescription();
                    if (Ship->Rank != 7) {
                        if (Ship->GetRankPointsToNextRank() > 0) {
                            Description = pas::concat_wide({Description, u" ", ([&] {
                                pas::WideString nextRankName = Ship->GetNextRankName();
                                pas::WideString intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(Ship->GetRankPointsToNextRank()));
                                pas::WideString localizedText = aConst::LocalizedText(u"Rank.NextRankText"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedText), u"<color=255,240,100>"_w, u"<NextRank>"_w, std::move(nextRankName), u"<WarPoints>"_w, std::move(intToStr));
                            }())});
                        } else {
                            Description = pas::concat_wide({Description, u" ", ([&] {
                                pas::WideString nextRankName_2 = Ship->GetNextRankName();
                                pas::WideString localizedText_2 = aConst::LocalizedText(u"Rank.NextRankGetText"_wref.get());
                                return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<NextRank>"_w, std::move(nextRankName_2));
                            }())});
                        }
                    }
                } else if (Sender->ControlName == u"RankI2" || Sender->ControlName == u"RankAdd2") {
                    Path = EC_Str::ExtractDelimitedPartW(pas::view(fShip2::PirateRankToImageSmall(Ship->PirateRank)), 1, u","sv);
                    {
                        GI_GraphBuf::TGraphBufGI* RankImage_4 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"sv));
                        RankImage_4->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(Path, RankImage_4->GraphBuf);
                        RankImage_4->SetImageKindX(GI_Main::ikxCenter);
                        RankImage_4->SetImageKindY(GI_Main::ikyCenter);
                    }
                    Title = ([&] {
                        pas::WideString pirateRankLongName = Ship->GetPirateRankLongName();
                        pas::WideString infoNameColorTag_2 = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(pas::view(std::move(pirateRankLongName)), pas::view(std::move(infoNameColorTag_2)));
                    }());
                    Description = Ship->GetPirateRankDescription();
                    if (Ship->PirateRank != 7) {
                        if (Ship->GetPirateRankPointsToNextRank() > 0) {
                            Description = pas::concat_wide({Description, u" ", ([&] {
                                pas::WideString nextPirateRankName = Ship->GetNextPirateRankName();
                                pas::WideString intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(Ship->GetPirateRankPointsToNextRank()));
                                pas::WideString localizedText_3 = aConst::LocalizedText(u"RankPirate.NextRankText"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedText_3), u"<color=255,240,100>"_w, u"<NextRank>"_w, std::move(nextPirateRankName), u"<WarPoints>"_w, std::move(intToStr_2));
                            }())});
                        } else {
                            Description = pas::concat_wide({Description, u" ", ([&] {
                                pas::WideString nextPirateRankName_2 = Ship->GetNextPirateRankName();
                                pas::WideString localizedText_4 = aConst::LocalizedText(u"RankPirate.NextRankGetText"_wref.get());
                                return aMyFunction::FormatText1(std::move(localizedText_4), u"<color=255,240,100>"_w, u"<NextRank>"_w, std::move(nextPirateRankName_2));
                            }())});
                        }
                    }
                }
            } else if (pas::class_cast_if<GI_Zone::TZoneGI*>(Sender) != nullptr) {
                {
                    GI_GraphBuf::TGraphBufGI* RankImage_5 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"RankImage"sv));
                    RankImage_5->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"Skill", pas::wide_int_to_str(EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName)) + 1)}), RankImage_5->GraphBuf);
                    RankImage_5->SetImageKindX(GI_Main::ikxCenter);
                    RankImage_5->SetImageKindY(GI_Main::ikyCenter);
                }
                Skill = static_cast<aGalaxyStruct::TPilotSkill>(EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName)));
                Title = ([&] {
                    pas::WideString localizedText_5 = aConst::LocalizedText(pas::concat_wide({u"Skills.", aConst::SkillConfigNames[Skill], u".Name"}));
                    pas::WideString infoNameColorTag_3 = aMyFunction::InfoNameColorTag;
                    return aMyFunction::WrapTextInColor(pas::view(std::move(localizedText_5)), pas::view(std::move(infoNameColorTag_3)));
                }());
                Description = ([&] {
                    pas::WideString intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(aConst::PilotSkillEffects[PlayerHoldShip->GetEffectiveSkillLevel(Skill, false)][Skill]));
                    pas::WideString localizedText_6 = aConst::LocalizedText(pas::concat_wide({u"Skills.", aConst::SkillConfigNames[Skill], u".Text"}));
                    return aMyFunction::FormatText1(std::move(localizedText_6), u"<color=255,240,100>"_w, u"<SkillValue>"_w, std::move(intToStr_3));
                }());
                aMyFunction::ReplaceTextToken(Description, u"<SkillLevel>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(PlayerHoldShip->GetEffectiveSkillLevel(Skill, false))), u"<color=255,240,100>"_w);
                if (Skill == aGalaxyStruct::psTechnical) {
                    aMyFunction::ReplaceTextToken(Description, u"<N>"_w, pas::wide_int_to_str(PlayerHoldShip->GetSatelliteLimit()), u"<color=255,240,100>"_w);
                }
                if (Skill == aGalaxyStruct::psTrading) {
                    aMyFunction::ReplaceTextToken(Description, u"<SkillValue2>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aConst::TradingSkillSalePercent[PlayerHoldShip->GetEffectiveSkillLevel(Skill, false)])), u"<color=255,240,100>"_w);
                }
                if (Skill == aGalaxyStruct::psLeadership) {
                    aMyFunction::ReplaceTextToken(Description, u"<SkillValue2>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aConst::LeadershipExperiencePercent[PlayerHoldShip->GetEffectiveSkillLevel(Skill, false)])), u"<color=255,240,100>"_w);
                }
                if (PlayerHoldShip->GetBaseSkillLevel(Skill) < 6) {
                    Description = pas::concat_wide({Description, u"\r\n", u"\r\n", ([&] {
                        pas::WideString intToStr_4 = pas::wide_int_to_str(static_cast<std::int32_t>(aConst::SkillTrainingCosts[PlayerHoldShip->BaseSkills[Skill] + 1][Skill]));
                        pas::WideString localizedText_7 = aConst::LocalizedText(u"Skills.PointForNextLevel"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_7), u"<color=255,240,100>"_w, u"<PointForNextLevel>"_w, std::move(intToStr_4));
                    }())});
                }
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankName"sv))->SetText(Title);
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankText"sv))->SetText(Description);
            Window = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"RankWnd"sv));
            Window->SetPosition(ClassesImports::Point(Window->LocalPosition.X, std::max<std::int32_t>(10, Sender->HitTestBounds.Top - Sender->ClientSize.Y / 3 - 60)));
            Window->SetActive(true);
            Window->Invalidate();
            {
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankText"sv));
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankName"sv));
                TfShip2::LayoutItemInfo(Window, cpp_arg_2, cpp_arg, true, true, 0);
            }
            {
                GI_Label::TLabelGI* RankName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"RankName"sv));
                RankName->SetSize(ClassesImports::Point(Window->ClientSize.X - RankName->LocalPosition.X - Window->WorkSubRect.Right, RankName->ClientSize.Y));
            }
            if (!Afterburner) {
                Window->SetPosition(ClassesImports::Point(PropertyHintRightEdge - Window->ClientSize.X, Window->LocalPosition.Y));
            } else {
                Window->SetPosition(Position);
            }
            if (PropertyInfoHideTimer != nullptr) {
                CancelCallbackTimer(PropertyInfoHideTimer);
                PropertyInfoHideTimer = nullptr;
            }
            HideItemInfo(nullptr, 0);
        }
    }

    void TfShip2::HideShipPropertyInfo(GI_MessageLoop::TObjectGI* Sender) {
        GetByName(u"RankWnd"sv)->SetActive(false);
    }

    // Native empty three-register method; argument purposes unresolved.
    void TfShip2::UpdateInfoHint(std::int32_t First, std::int32_t Second) {
    }

    // Matches the second underscore-delimited component against eight equipment slot names; raises on no match.
    aConst::TItemType TfShip2::SlotToTip(const std::u16string_view& SlotName) {
        pas::WideString Name{};
        std::int32_t I{};
        Name = EC_Str::ExtractDelimitedPartW(SlotName, 1, u"_"sv);
        for (I = 0; I <= 7; ++I) {
            if (Name == aConst::EquipmentSlotLayouts[I].Name) {
                return aConst::EquipmentSlotLayouts[I].ItemType;
            }
        }
        pas::raise(pas::make_exception<pas::Exception>("SlotToTip"_a));
    }

    std::uint8_t TfShip2::IsCompatibleSlot(aConst::TItemType ItemType, aConst::TItemType SlotType) {
        return ItemType == SlotType || pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && pas::in_range(SlotType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon));
    }

    void TfShip2::RefreshEquipmentSlotControls() {
        std::int32_t Count{};
        std::int32_t MaximumSlots{};
        std::int32_t I{};
        std::int32_t Slot{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
            MaximumSlots = 1;
            if (aConst::EquipmentSlotLayouts[I].ItemType == aConst::t_Weapon1) {
                MaximumSlots = 5;
            }
            Count = PlayerHoldShip->GetSlotCountForItemType(aConst::EquipmentSlotLayouts[I].ItemType);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Slot); ) {
                EquipmentSlotZones[I][Slot] = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"z"}))));
                EquipmentSlotAnimations[I][Slot] = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"anim"}))));
                EquipmentSlotAnimations[I][Slot]->UserState = 0;
                GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"off"})))->SetActive(false);
                GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"Set"})))->SetActive(false);
            }
            if (aConst::EquipmentSlotLayouts[I].ItemType == aConst::t_Weapon1) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(Count, 4); cpp_range_3.next(Slot); ) {
                    GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"off"})))->SetActive(true);
                    GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"Set"})))->SetActive(false);
                }
            } else {
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(Count, 0); cpp_range_4.next(Slot); ) {
                    GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"off"})))->SetActive(true);
                    GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"Set"})))->SetActive(false);
                }
            }
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(Count, MaximumSlots - 1); cpp_range_5.next(Slot); ) {
                GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"i"})))->SetActive(false);
                GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"anim"})))->SetActive(false);
            }
        }
    }

    void TfShip2::RefreshShipView() {
        std::int32_t I{};
        std::int32_t Slot{};
        std::int32_t SlotCount{};
        std::int32_t DuplicateSlot{};
        std::int32_t Column{};
        std::int32_t Row{};
        aItem::TEquipment* Item{};
        TPlayerHoldUnit* Entry{};
        std::uint8_t Highlight{};
        std::uint8_t Boost{};
        aConst::TItemType SelectedType{};
        aConst::TItemType InstalledType{};
        aItem::TArtefact* Artefact{};
        pas::WideString Text{};
        GI_Image::TImageGI* SlotImage{};
        aGalaxy::Galaxy->CheckIntegrityChecksum1(432);
        RefreshEquipmentConfigurationButtons();
        PlayerHoldShip->RefreshAssignedItemSlots();
        PlayerHoldShip->RefreshDerivedStats(true);
        fShip2::RefreshPlayerHoldView(false);
        RefreshEquipmentSlotControls();
        {
            GI_Image::TImageGI* LifeLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"LifeLeft"sv));
            if (PlayerHoldShip->CountActiveArtefacts(aConst::t_ArtBio) > 0) {
                LifeLeft->SetActive(true);
                if (PlayerHoldShip->HasActiveDisease()) {
                    LifeLeft->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"LifeRed"}));
                } else if (PlayerHoldShip->CountPresentDiseases() > 0) {
                    LifeLeft->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"LifeYellow"}));
                } else {
                    LifeLeft->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"LifeGreen"}));
                }
            } else {
                LifeLeft->SetActive(false);
            }
        }
        {
            GI_Image::TImageGI* LifeRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"LifeRight"sv));
            if (PlayerHoldShip->CountActiveArtefacts(aConst::t_ArtBio) > 0) {
                LifeRight->SetActive(true);
                if (PlayerHoldShip->HasActiveDisease()) {
                    LifeRight->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"LifeRed"}));
                } else if (PlayerHoldShip->CountPresentDiseases() > 0) {
                    LifeRight->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"LifeYellow"}));
                } else {
                    LifeRight->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"LifeGreen"}));
                }
            } else {
                LifeRight->SetActive(false);
            }
        }
        Text = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(PlayerHoldShip->GetDefensePercent()), "%"}));
        Text = pas::concat_wide({Text, u" + ", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(PlayerHoldShip->GetArmor())), u""sv)});
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IDef"sv))->SetText(Text);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(PlayerHoldShip->CalculateMass());
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IMass"sv));
            cpp_arg->SetText(intToStr);
        }
        if (PlayerHoldShip->CalculateSpeed() <= 0) {
            Text = u"<color=255,0,0>"_w;
        } else {
            Text = pas::WideString();
        }
        {
            const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(PlayerHoldShip->CalculateSpeed())), pas::view(Text));
            GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISpeed"sv));
            cpp_arg_2->SetText(wrapTextInColor);
        }
        if (PlayerHoldShip->GetCargoFreeSpace() < 0) {
            Text = u"<color=255,0,0>"_w;
        } else {
            Text = pas::WideString();
        }
        {
            const pas::WideString& wrapTextInColor_2 = aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(PlayerHoldShip->GetCargoFreeSpace())), pas::view(Text));
            GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IEmpty"sv));
            cpp_arg_3->SetText(wrapTextInColor_2);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Left"sv))->SetDisabled(HoldFirstIndex <= 0);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Right"sv))->SetDisabled(HoldFirstIndex + 6 > pas::list_count(PlayerHoldEntries));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UpRH"sv))->SetDisabled(RemoteHoldFirstOrder <= 0);
        {
            GI_GraphButton::TGraphButtonGI* cpp_arg_4 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"DownRH"sv));
            std::uint8_t cpp_arg_5 = RemoteHoldFirstOrder >= TfShip2::GetRemoteHoldScrollLimit();
            cpp_arg_4->SetDisabled(cpp_arg_5);
        }
        RefreshActionPanels(SelectedHoldKind, SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, SelectedHoldItem, SelectedHoldOrigin);
        HoveredEquipmentAnimation = nullptr;
        {
            GI_Image::TImageGI* HullSet = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"HullSet"sv));
            HullSet->SetActive(PlayerHoldShip->GetHull()->HasMicroModule());
            if (HullSet->Active) {
                HullSet->SetImagePath(pas::concat_wide({u"GI,", aItem::GetMicroModuleBitmapResourceName(PlayerHoldShip->GetHull()->MicroModuleIndex - 1), u"Set"}));
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
            SlotCount = PlayerHoldShip->GetSlotCountForItemType(aConst::EquipmentSlotLayouts[I].ItemType);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_2.next(Slot); ) {
                Item = PlayerHoldShip->FindEquippedItemInSlot(aConst::EquipmentSlotLayouts[I].ItemType, Slot);
                SlotImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"i"}))));
                if (SlotImage->UserValue == 0) {
                    SlotImage->UserValue = SlotImage->LocalPosition.X + SlotImage->ClientSize.X / 2;
                    SlotImage->UserIndex = SlotImage->LocalPosition.Y + SlotImage->ClientSize.Y / 2;
                }
                if (Item == nullptr) {
                    SlotImage->SetActive(false);
                    SlotImage->SetImagePath(pas::WideString());
                    GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"Set"})))->SetActive(false);
                } else {
                    SlotImage->SetActive(true);
                    SlotImage->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Item), u"i"}));
                    SlotImage->SetImageKindX(GI_Main::ikxCenter);
                    SlotImage->SetImageKindY(GI_Main::ikyCenter);
                    {
                        GI_Image::TImageGI* cpp_with_4 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"Set"}))));
                        cpp_with_4->SetActive(Item->HasMicroModule());
                        if (cpp_with_4->Active) {
                            cpp_with_4->SetImagePath(pas::concat_wide({u"GI,", aItem::GetMicroModuleBitmapResourceName(Item->MicroModuleIndex - 1), u"Set"}));
                        }
                    }
                }
                if (GlobalsV::AnimItem && Item != nullptr) {
                    GI_GAI::TgaiGI* cpp_with_5 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"anim"}))));
                    cpp_with_5->UserData = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(SlotImage));
                    cpp_with_5->SetPosition(SlotImage->LocalPosition);
                    cpp_with_5->SetImagePath(pas::concat_wide({fEquipmentShop::GetShopItemIconName(Item), u"a"}));
                    cpp_with_5->SequenceIndex = 0;
                    cpp_with_5->SetActive(false);
                    if (cpp_with_5->UserState != 0) {
                        SlotImage->SetActive(false);
                        cpp_with_5->UpdateAutoGeometry();
                        cpp_with_5->SetSequenceFrame(std::min<std::int64_t>(static_cast<std::int64_t>(static_cast<std::uint32_t>(cpp_with_5->UserState)), static_cast<std::int64_t>(cpp_with_5->SequenceFrameCount - 1)));
                        cpp_with_5->SetActive(true);
                        cpp_with_5->StopAutoPlayback();
                    }
                } else {
                    GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"anim"})))->SetActive(false);
                }
                {
                    GI_Zone::TZoneGI* cpp_with_6 = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"z"}))));
                    cpp_with_6->ZoneMouseDownCallback = pas::bind_method<&TfShip2::EquipmentSlotMouseDown>(this);
                }
                if (HighlightRepairableEquipment && Item != nullptr && (Item->EquippedFlag != 0 || pas::class_cast_if<aItem::THull*>(Item) != nullptr) && Item->NeedsRepair()) {
                    Highlight = true;
                } else if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_MicroModule && Item != nullptr && Item->MicroModuleIndex == 0 && pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->CanInstallOn(Item)) {
                    Highlight = true;
                } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && pas::class_cast_if<aItem::TEquipmentWithActCode*>(SelectedHoldItem) != nullptr && Item != nullptr && aScript::RunItemConfigActionCode(SelectedHoldItem, aConst::satOnCheckingUsability, PlayerHoldShip, Item, nullptr, 0) > 0) {
                    Highlight = true;
                } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && SelectedHoldItem->ScriptItem != nullptr && Item != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnCheckingUsability, PlayerHoldShip, Item, nullptr, 0) > 0) {
                    Highlight = true;
                } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && Item != nullptr && pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr && aScript::RunItemConfigActionCode(Item, aConst::satOnCheckingUsability2, PlayerHoldShip, SelectedHoldItem, nullptr, 0) > 0) {
                    Highlight = true;
                } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && Item != nullptr && Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnCheckingUsability2, PlayerHoldShip, SelectedHoldItem, nullptr, 0) > 0) {
                    Highlight = true;
                } else if (SelectedHoldKind == phkGoods && Item != nullptr && Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnCheckingUsabilityGoods, PlayerHoldShip, reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsIndex))), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsQuantity))), 0) > 0) {
                    Highlight = true;
                } else if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_Cistern && Item != nullptr && Item->ItemType == aConst::t_FuelTanks && pas::checked_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel > 0 && ([&] {
                    std::int32_t cpp_left = pas::checked_cast<aItem::TFuelTanks*>(Item)->Fuel;
                    return cpp_left < pas::checked_cast<aItem::TFuelTanks*>(Item)->Capacity;
                }())) {
                    Highlight = true;
                } else {
                    Highlight = SelectedHoldKind == phkEquipment && TfShip2::IsCompatibleSlot(SelectedHoldItem->ItemType, aConst::EquipmentSlotLayouts[I].ItemType);
                }
                Boost = SelectedHoldKind == phkArtefact && pas::class_cast_if<aItem::TArtefact*>(SelectedHoldItem) != nullptr && aShip::TShip_IsEquipmentUsable(PlayerHoldShip, Item) && PlayerHoldShip->CanBoostArtefact(static_cast<aItem::TArtefact*>(SelectedHoldItem)->GetEffectiveType(), Item, true);
                if (Highlight && Item != nullptr && Item->NoDropFlag > 0) {
                    Highlight = false;
                }
                GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"a"})))->SetActive(Highlight && static_cast<std::uint8_t>(Boost ^ 1));
                {
                    std::uint8_t cpp_arg_6 = Item != nullptr && static_cast<std::uint8_t>(Highlight ^ 1) && aShip::TShip_IsEquipmentUsable(PlayerHoldShip, Item) && static_cast<std::uint8_t>(Boost ^ 1);
                    GI_MessageLoop::TObjectGI* byName = GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"n"})));
                    byName->SetActive(cpp_arg_6);
                }
                {
                    std::uint8_t cpp_arg_7 = Item != nullptr && static_cast<std::uint8_t>(Highlight ^ 1) && static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(PlayerHoldShip, Item) ^ 1) && static_cast<std::uint8_t>(Boost ^ 1);
                    GI_MessageLoop::TObjectGI* byName_2 = GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"b"})));
                    byName_2->SetActive(cpp_arg_7);
                }
                GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"Ex"})))->SetActive(Boost);
            }
            if (aConst::EquipmentSlotLayouts[I].ItemType == aConst::t_Weapon1) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(SlotCount, 4); cpp_range_3.next(Slot); ) {
                    pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"z"}))))->ZoneMouseDownCallback = nullptr;
                }
            } else {
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(SlotCount, 0); cpp_range_4.next(Slot); ) {
                    pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(pas::concat_wide({u"S_", aConst::EquipmentSlotLayouts[I].Name, u"_", pas::wide_int_to_str(Slot), u"z"}))))->ZoneMouseDownCallback = nullptr;
                }
            }
        }
        SlotCount = PlayerHoldShip->GetSlotCountForItemType(aConst::t_Artefact);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_5.next(Slot); ) {
            Artefact = pas::checked_cast<aItem::TArtefact*>(PlayerHoldShip->FindEquippedItemInSlot(aConst::t_Artefact, Slot));
            Highlight = SelectedHoldKind == phkArtefact && pas::in_set<aConst::t_Artefact, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtDefToArms1, aConst::t_ArtFastRacks>(SelectedHoldItem->ItemType);
            DuplicateSlot = -1;
            if (Highlight && static_cast<std::uint8_t>(aGalaxy::Galaxy->AreDuplicateArtefactsEnabled() ^ 1)) {
                SelectedType = SelectedHoldItem->ItemType;
                if (pas::in_range(SelectedType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2)) && reinterpret_cast<aItem::TArtefactCustom*>(SelectedHoldItem)->SharedUse) {
                    SelectedType = reinterpret_cast<aItem::TArtefactCustom*>(SelectedHoldItem)->CountsAsItemType;
                }
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->Artefacts) - 1); cpp_range_6.next(I); ) {
                    Item = pas::list_at<aItem::TEquipment>(PlayerHoldShip->Artefacts, I);
                    if (Item->EquippedFlag != 0) {
                        InstalledType = Item->ItemType;
                        if (pas::in_range(static_cast<std::uint8_t>(InstalledType), 8, 9) && reinterpret_cast<aItem::TArtefactCustom*>(Item)->SharedUse) {
                            InstalledType = reinterpret_cast<aItem::TArtefactCustom*>(Item)->CountsAsItemType;
                        }
                        if (SelectedType == InstalledType && (static_cast<std::uint8_t>(pas::in_range(SelectedType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2)) ^ 1) || Item->ConfigBlockName == reinterpret_cast<aItem::TEquipment*>(SelectedHoldItem)->ConfigBlockName)) {
                            DuplicateSlot = Item->AssignedSlotData;
                            break;
                        }
                    }
                }
            }
            if (DuplicateSlot >= 0 && Slot != DuplicateSlot) {
                Highlight = false;
            }
            if (static_cast<std::uint8_t>(Highlight ^ 1) && Artefact != nullptr && Artefact->BrokenFlag == 0 && (PlayerHoldShip->CanBoostArtefact(Artefact->GetEffectiveType(), nullptr, false) || SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TEquipment*>(SelectedHoldItem) != nullptr && PlayerHoldShip->CanBoostArtefact(Artefact->GetEffectiveType(), static_cast<aItem::TEquipment*>(SelectedHoldItem), true))) {
                Boost = true;
            } else {
                Boost = false;
            }
            if (Highlight && Artefact != nullptr && Artefact->NoDropFlag > 0) {
                Highlight = false;
            }
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "n"}))))->SetActive(Artefact != nullptr && static_cast<std::uint8_t>(Highlight ^ 1) && Artefact->BrokenFlag == 0 && static_cast<std::uint8_t>(Boost ^ 1));
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "b"}))))->SetActive(Artefact != nullptr && static_cast<std::uint8_t>(Highlight ^ 1) && Artefact->BrokenFlag != 0 && static_cast<std::uint8_t>(Boost ^ 1));
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "a"}))))->SetActive(Highlight && static_cast<std::uint8_t>(Boost ^ 1));
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "i"}))))->SetActive(Artefact != nullptr);
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "Ex"}))))->SetActive(Boost);
            {
                GI_Image::TImageGI* cpp_with_7 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "i"})))));
                if (Artefact == nullptr) {
                    cpp_with_7->SetImagePath(pas::WideString());
                } else {
                    cpp_with_7->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Artefact), u"s"}));
                    cpp_with_7->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_7->SetImageKindY(GI_Main::ikyCenter);
                }
            }
            {
                GI_Zone::TZoneGI* cpp_with_8 = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "z"})))));
                cpp_with_8->SetActive(Artefact != nullptr || Highlight);
                if (Highlight || SelectedHoldKind == phkEmpty) {
                    cpp_with_8->ZoneMouseDownCallback = pas::bind_method<&TfShip2::ArtefactSlotMouseDown>(this);
                } else if (Artefact != nullptr && SelectedHoldKind == phkEquipment) {
                    cpp_with_8->ZoneMouseDownCallback = pas::bind_method<&TfShip2::UseOnArtefactSlot>(this);
                } else if (Artefact != nullptr && SelectedHoldKind == phkArtefact && static_cast<std::uint8_t>(pas::in_set<aConst::t_Artefact, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtDefToArms1, aConst::t_ArtFastRacks>(SelectedHoldItem->ItemType) ^ 1)) {
                    cpp_with_8->ZoneMouseDownCallback = pas::bind_method<&TfShip2::UseOnArtefactSlot>(this);
                } else {
                    cpp_with_8->ZoneMouseDownCallback = nullptr;
                }
            }
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "off"}))))->SetActive(false);
        }
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(SlotCount, aConst::DefaultHullSlotCounts[aConst::sskArtefact] - 1); cpp_range_7.next(Slot); ) {
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "n"}))))->SetActive(false);
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "b"}))))->SetActive(false);
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "a"}))))->SetActive(false);
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "i"}))))->SetActive(false);
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "z"}))))->SetActive(false);
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "Ex"}))))->SetActive(false);
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Art", SysUtils::IntToStr(Slot), "off"}))))->SetActive(true);
        }
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, 5); cpp_range_8.next(I); ) {
            if (HoldFirstIndex + I >= pas::list_count(PlayerHoldEntries)) {
                Entry = nullptr;
            } else {
                Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, HoldFirstIndex + I);
            }
            {
                GI_Image::TImageGI* cpp_with_9 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "i"})))));
                if (Entry == nullptr || Entry->Kind == phkEmpty) {
                    cpp_with_9->SetImagePath(pas::WideString());
                } else if (Entry->Kind == phkGoods) {
                    cpp_with_9->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(Entry->GoodsIndex))}));
                    cpp_with_9->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_9->SetImageKindY(GI_Main::ikyCenter);
                } else if (Entry->Kind == phkEquipment) {
                    cpp_with_9->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Entry->Item), u"s"}));
                    cpp_with_9->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_9->SetImageKindY(GI_Main::ikyCenter);
                    if (Entry->Item == Globals::ScriptUseItem) {
                        cpp_with_9->SetImagePath(pas::WideString());
                    }
                } else if (Entry->Kind == phkArtefact) {
                    cpp_with_9->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Entry->Item), u"s"}));
                    cpp_with_9->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_9->SetImageKindY(GI_Main::ikyCenter);
                    if (Entry->Item == Globals::ScriptUseItem) {
                        cpp_with_9->SetImagePath(pas::WideString());
                    }
                }
            }
            {
                GI_Zone::TZoneGI* cpp_with_10 = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "z"})))));
                cpp_with_10->ZoneMouseDownCallback = pas::bind_method<&TfShip2::RemoteHoldItemMouseDown>(this);
            }
            GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "f"}))))->SetActive(SelectedHoldKind != phkEmpty);
        }
        for (Row = 0; Row <= 10; ++Row) {
            for (Column = 0; Column <= 4; ++Column) {
                I = fShip2::FindPlayerHoldIndexByOrder(RemoteHoldFirstOrder + Column + Row * 5);
                if (I < 0) {
                    Entry = nullptr;
                } else {
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                }
                {
                    GI_Image::TImageGI* cpp_with_11 = RemoteHoldImages[Row * 5 + Column];
                    if (Entry == nullptr || Entry->Kind == phkEmpty) {
                        cpp_with_11->SetImagePath(pas::WideString());
                    } else if (Entry->Kind == phkGoods) {
                        cpp_with_11->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(Entry->GoodsIndex))}));
                        cpp_with_11->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_11->SetImageKindY(GI_Main::ikyCenter);
                    } else if (Entry->Kind == phkEquipment) {
                        cpp_with_11->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Entry->Item), u"s"}));
                        cpp_with_11->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_11->SetImageKindY(GI_Main::ikyCenter);
                        if (Entry->Item == Globals::ScriptUseItem) {
                            cpp_with_11->SetImagePath(pas::WideString());
                        }
                    } else if (Entry->Kind == phkArtefact) {
                        cpp_with_11->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Entry->Item), u"s"}));
                        cpp_with_11->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_11->SetImageKindY(GI_Main::ikyCenter);
                        if (Entry->Item == Globals::ScriptUseItem) {
                            cpp_with_11->SetImagePath(pas::WideString());
                        }
                    }
                }
            }
        }
        if (HighlightRepairableEquipment && PlayerHoldShip->GetHull()->NeedsRepair()) {
            GetByName(u"HullA"sv)->SetActive(true);
        } else if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_MicroModule && PlayerHoldShip->GetHull()->MicroModuleIndex == 0 && pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->CanInstallOn(PlayerHoldShip->GetHull())) {
            GetByName(u"HullA"sv)->SetActive(true);
        } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && pas::class_cast_if<aItem::TEquipmentWithActCode*>(SelectedHoldItem) != nullptr && aScript::RunItemConfigActionCode(SelectedHoldItem, aConst::satOnCheckingUsability, PlayerHoldShip, PlayerHoldShip->GetHull(), nullptr, 0) > 0) {
            GetByName(u"HullA"sv)->SetActive(true);
        } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnCheckingUsability, PlayerHoldShip, PlayerHoldShip->GetHull(), nullptr, 0) > 0) {
            GetByName(u"HullA"sv)->SetActive(true);
        } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && PlayerHoldShip->GetHull()->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(PlayerHoldShip->GetHull()->ScriptItem)->RunActionCode(aConst::satOnCheckingUsability2, PlayerHoldShip, SelectedHoldItem, nullptr, 0) > 0) {
            GetByName(u"HullA"sv)->SetActive(true);
        } else if (SelectedHoldKind == phkGoods && PlayerHoldShip->GetHull()->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(PlayerHoldShip->GetHull()->ScriptItem)->RunActionCode(aConst::satOnCheckingUsabilityGoods, PlayerHoldShip, reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsIndex))), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsQuantity))), 0) > 0) {
            GetByName(u"HullA"sv)->SetActive(true);
        } else {
            std::uint8_t cpp_arg_8 = SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_Hull && TfShip2::IsHoldNormalShip() && pas::list_at<aItem::TItem>(PlayerHoldShip->Inventory, 0)->NoDropFlag == 0;
            GI_MessageLoop::TObjectGI* byName_3 = GetByName(u"HullA"sv);
            byName_3->SetActive(cpp_arg_8);
        }
        if (SelectedHoldKind == phkArtefact && pas::class_cast_if<aItem::TArtefact*>(SelectedHoldItem) != nullptr && static_cast<std::uint8_t>(GetByName(u"HullA"sv)->Active ^ 1) && PlayerHoldShip->CanBoostArtefact(static_cast<aItem::TArtefact*>(SelectedHoldItem)->GetEffectiveType(), PlayerHoldShip->GetHull(), true)) {
            GetByName(u"HullEx"sv)->SetActive(true);
        } else {
            GetByName(u"HullEx"sv)->SetActive(false);
        }
        {
            std::uint8_t cpp_arg_9 = PlayerHoldShip->GetSlotCount(aConst::sskAfterburner) > 0;
            GI_MessageLoop::TObjectGI* byName_4 = GetByName(u"Forsage"sv);
            byName_4->SetActive(cpp_arg_9);
        }
        {
            std::uint8_t cpp_arg_10 = PlayerHoldShip->AfterburnerActive && aShip::TShip_IsEquipmentUsable(PlayerHoldShip, PlayerHoldShip->GetEngine());
            GI_MessageLoop::TObjectGI* byName_5 = GetByName(u"ForsageLight"sv);
            byName_5->SetActive(cpp_arg_10);
        }
        {
            GI_GraphButton::TGraphButtonGI* ForsageBut = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ForsageBut"sv));
            ForsageBut->MouseEnterCallback = pas::bind_method<&TfShip2::ShowShipPropertyInfo>(this);
            ForsageBut->MouseLeaveCallback = pas::bind_method<&TfShip2::HideShipPropertyInfo>(this);
            ForsageBut->UpCallback = pas::bind_method<&TfShip2::ToggleAfterburner>(this);
            ForsageBut->SetDisabled(static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(PlayerHoldShip, PlayerHoldShip->GetEngine()) ^ 1) || static_cast<std::uint8_t>(PlayerHoldShip->InNormalSpace() ^ 1));
        }
        {
            GI_Label::TLabelGI* IDestrEnergy = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IDestrEnergy"sv));
            IDestrEnergy->SetText(([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(PlayerHoldShip->GetHull()->Energy);
                pas::WideString intToStr_3 = pas::wide_int_to_str(PlayerHoldShip->GetHull()->EnergyMax);
                pas::WideString localizedText = aConst::LocalizedText(u"FormShip.DestrEnergy"_wref.get());
                return aMyFunction::FormatText2(std::move(localizedText), u"<color=0,71,234>"_w, u"<Value1>"_w, std::move(intToStr_2), u"<Value2>"_w, std::move(intToStr_3));
            }()));
        }
        {
            GI_Label::TLabelGI* IDestrShields = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IDestrShields"sv));
            if (PlayerHoldShip->GetHull()->ImpulseShieldsEnabled) {
                IDestrShields->SetText(aConst::LocalizedText(u"FormShip.DestrIShield"_wref.get()));
            } else {
                IDestrShields->SetText(aConst::LocalizedText(u"FormShip.DestrNShield"_wref.get()));
            }
        }
        {
            GI_Label::TLabelGI* IDestrCount = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IDestrCount"sv));
            IDestrCount->SetText(([&] {
                pas::WideString intToStr_4 = pas::wide_int_to_str(PlayerHoldShip->CountActiveInterceptorTargets());
                pas::WideString intToStr_5 = pas::wide_int_to_str(PlayerHoldShip->GetInterceptorEnergyCost());
                pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.DestrCount"_wref.get());
                return aMyFunction::FormatText2(std::move(localizedText_2), u"<color=0,71,234>"_w, u"<Count>"_w, std::move(intToStr_4), u"<Cost>"_w, std::move(intToStr_5));
            }()));
        }
        {
            GI_Label::TLabelGI* IDestrEnergyOut = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IDestrEnergyOut"sv));
            IDestrEnergyOut->SetText(pas::concat_wide({u"-", ([&] {
                pas::WideString intToStr_6 = pas::wide_int_to_str(PlayerHoldShip->CountActiveInterceptorTargets() * 3);
                pas::WideString localizedText_3 = aConst::LocalizedText(u"FormShip.DestrPerDay"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_3), u"<color=0,71,234>"_w, u"<Value>"_w, std::move(intToStr_6));
            }())}));
        }
        {
            GI_Label::TLabelGI* IDestrEnergyIn = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IDestrEnergyIn"sv));
            IDestrEnergyIn->SetText(pas::concat_wide({u"+", ([&] {
                pas::WideString intToStr_7 = pas::wide_int_to_str(PlayerHoldShip->GetHullEnergyRegeneration());
                pas::WideString localizedText_4 = aConst::LocalizedText(u"FormShip.DestrPerDay"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_4), u"<color=0,71,234>"_w, u"<Value>"_w, std::move(intToStr_7));
            }())}));
        }
        RefreshLoadEquippedRocketsButton();
        BuildAdditionalInfoPanel();
        RefreshStorageView();
        ShowItemInfo();
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(433);
    }

    void TfShip2::RefreshActionPanels(TPlayerHoldKind Kind, std::uint8_t Good, std::int32_t Quantity, std::int32_t Cost, aItem::TItem* Item, std::int32_t Origin) {
        std::int32_t I{};
        std::int32_t TotalRepair{};
        aItem::TEquipment* Equipment{};
        // Nested in RefreshActionPanels; captures Self.
        auto Skill = [&](std::int32_t Index, std::int32_t BaseLevel, std::int32_t EffectiveLevel, std::uint8_t CanTrain) -> void {
            std::int32_t Gap = 2;
            std::int32_t Step = Gap + 5;
            std::int32_t Height = 43;
            {
                GI_Image::TImageGI* cpp_with = this->SkillImages[Index];
                cpp_with->SetActive(std::min<std::int32_t>(BaseLevel, EffectiveLevel) > 0);
                cpp_with->SetSize(ClassesImports::Point(cpp_with->ClientSize.X, Step * std::min<std::int32_t>(BaseLevel, EffectiveLevel)));
                cpp_with->SetPosition(ClassesImports::Point(cpp_with->LocalPosition.X, this->SkillImageRestTop[Index] + Height - cpp_with->ClientSize.Y));
                cpp_with->SetImageKindY(GI_Main::ikyBottom);
            }
            if (BaseLevel < EffectiveLevel) {
                {
                    GI_Panel::TPanelGI* cpp_with_2 = this->SkillPanels[Index];
                    cpp_with_2->SetSize(ClassesImports::Point(cpp_with_2->ClientSize.X, (EffectiveLevel - BaseLevel) * Step));
                    cpp_with_2->SetPosition(ClassesImports::Point(cpp_with_2->LocalPosition.X, this->SkillImageRestTop[Index] + Height - Step * EffectiveLevel));
                }
                {
                    GI_Image::TImageGI* cpp_with_3 = this->SkillImagesP[Index];
                    cpp_with_3->SetPosition(ClassesImports::Point(cpp_with_3->LocalPosition.X, -Step * (6 - EffectiveLevel) - 1));
                    cpp_with_3->SetActive(true);
                }
            } else {
                this->SkillImagesP[Index]->SetActive(false);
            }
            if (BaseLevel > EffectiveLevel) {
                {
                    GI_Panel::TPanelGI* cpp_with_4 = this->SkillPanels[Index];
                    cpp_with_4->SetSize(ClassesImports::Point(cpp_with_4->ClientSize.X, (BaseLevel - EffectiveLevel) * Step));
                    cpp_with_4->SetPosition(ClassesImports::Point(cpp_with_4->LocalPosition.X, this->SkillImageRestTop[Index] + Height - Step * BaseLevel));
                }
                {
                    GI_Image::TImageGI* cpp_with_5 = this->SkillImagesN[Index];
                    cpp_with_5->SetPosition(ClassesImports::Point(cpp_with_5->LocalPosition.X, -Step * (6 - BaseLevel) - 1));
                    cpp_with_5->SetActive(true);
                }
            } else {
                this->SkillImagesN[Index]->SetActive(false);
            }
            {
                GI_Label::TLabelGI* cpp_with_6 = this->SkillValueLabels[Index];
                cpp_with_6->SetText(pas::wide_int_to_str(EffectiveLevel));
            }
            {
                GI_Image::TImageGI* cpp_with_7 = this->SkillProgressImages[Index];
                if (EffectiveLevel > BaseLevel) {
                    cpp_with_7->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"DSGreen"}));
                } else if (EffectiveLevel < BaseLevel) {
                    cpp_with_7->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"DSRed"}));
                } else {
                    cpp_with_7->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"DSBlue"}));
                }
            }
            {
                GI_Image::TImageGI* cpp_with_8 = this->SkillGainImages[Index];
                cpp_with_8->SetActive(CanTrain);
            }
        };
        std::uint8_t OrdinaryShip = !(pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) && !(pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr);
        {
            GI_Label::TLabelGI* cpp_with = FreeSkillPointsLabel;
            cpp_with->SetText(pas::wide_int_to_str(PlayerHoldShip->FreeExperience));
        }
        {
            GI_Label::TLabelGI* cpp_with_2 = ExperienceLabel;
            cpp_with_2->SetText(pas::wide_int_to_str(PlayerHoldShip->FreeExperience));
        }
        Skill(0, PlayerHoldShip->GetBaseSkillLevel(aGalaxyStruct::psAccuracy), PlayerHoldShip->GetEffectiveSkillLevel(aGalaxyStruct::psAccuracy, false), PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psAccuracy));
        Skill(1, PlayerHoldShip->GetBaseSkillLevel(aGalaxyStruct::psManeuverability), PlayerHoldShip->GetEffectiveSkillLevel(aGalaxyStruct::psManeuverability, false), PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psManeuverability));
        Skill(2, PlayerHoldShip->GetBaseSkillLevel(aGalaxyStruct::psTechnical), PlayerHoldShip->GetEffectiveSkillLevel(aGalaxyStruct::psTechnical, false), PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psTechnical));
        Skill(3, PlayerHoldShip->GetBaseSkillLevel(aGalaxyStruct::psTrading), PlayerHoldShip->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false), PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psTrading) && OrdinaryShip);
        Skill(4, PlayerHoldShip->GetBaseSkillLevel(aGalaxyStruct::psCharisma), PlayerHoldShip->GetEffectiveSkillLevel(aGalaxyStruct::psCharisma, false), PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psCharisma) && OrdinaryShip);
        Skill(5, PlayerHoldShip->GetBaseSkillLevel(aGalaxyStruct::psLeadership), PlayerHoldShip->GetEffectiveSkillLevel(aGalaxyStruct::psLeadership, false), PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psLeadership) && OrdinaryShip);
        {
            GI_GraphButton::TGraphButtonGI* cpp_with_3 = SkillButtons[0];
            cpp_with_3->UserValue = 0;
            cpp_with_3->SetActive(PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psAccuracy));
            cpp_with_3->UpCallback = pas::bind_method<&TfShip2::TrainSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* cpp_with_4 = SkillButtons[1];
            cpp_with_4->UserValue = 1;
            cpp_with_4->SetActive(PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psManeuverability));
            cpp_with_4->UpCallback = pas::bind_method<&TfShip2::TrainSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* cpp_with_5 = SkillButtons[2];
            cpp_with_5->UserValue = 2;
            cpp_with_5->SetActive(PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psTechnical));
            cpp_with_5->UpCallback = pas::bind_method<&TfShip2::TrainSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* cpp_with_6 = SkillButtons[3];
            cpp_with_6->UserValue = 3;
            cpp_with_6->SetActive(PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psTrading) && OrdinaryShip);
            cpp_with_6->UpCallback = pas::bind_method<&TfShip2::TrainSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* cpp_with_7 = SkillButtons[4];
            cpp_with_7->UserValue = 4;
            cpp_with_7->SetActive(PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psCharisma) && OrdinaryShip);
            cpp_with_7->UpCallback = pas::bind_method<&TfShip2::TrainSkillClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* cpp_with_8 = SkillButtons[5];
            cpp_with_8->UserValue = 5;
            cpp_with_8->SetActive(PlayerHoldShip->CanTrainSkill(aGalaxyStruct::psLeadership) && OrdinaryShip);
            cpp_with_8->UpCallback = pas::bind_method<&TfShip2::TrainSkillClicked>(this);
        }
        if ((Kind == phkEquipment || Kind == phkArtefact) && pas::checked_cast<aItem::TEquipment*>(Item)->NeedsRepair() && (Item->ItemType != aConst::t_Protoplasm || aPlayer::GetPlayer()->DockedTo->TypeId != static_cast<std::uint8_t>(aGalaxyStruct::rstRangerCenter)) && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && (aPlayer::GetPlayer()->IsDockedToShip() || aPlayer::GetPlayer()->IsOnPlanet() && static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::oiDominator, aGalaxyStruct::oiUninhabited>(aPlayer::GetPlayer()->CurrentPlanet->OwnerId) ^ 1))) {
            OpenSpecialSlot1();
            {
                GI_Label::TLabelGI* SC_Slot1_Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"SC_Slot1_Text"sv));
                SC_Slot1_Text->SetText(([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(aItem::TEquipment_CalculateRepairCost(pas::checked_cast<aItem::TEquipment*>(Item)));
                    pas::WideString localizedText = aConst::LocalizedText(u"FormShip.Repair"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText), pas::WideString(), u"<Money>"_w, std::move(intToStr));
                }()));
            }
        } else {
            CloseSpecialSlot1();
        }
        if ((Kind == phkGoods || Kind == phkEquipment || Kind == phkArtefact) && (Kind == phkGoods || Item == nullptr || Item->ScriptItem == nullptr && Item->NoDropFlag == 0 || Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->CanSell) && (Kind == phkGoods || Kind == phkArtefact || !(pas::class_cast_if<aItem::THull*>(Item) != nullptr) || Origin != 0) && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && (aPlayer::GetPlayer()->IsDockedToShip() || aPlayer::GetPlayer()->IsOnPlanet() && static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::oiDominator, aGalaxyStruct::oiUninhabited>(aPlayer::GetPlayer()->CurrentPlanet->OwnerId) ^ 1))) {
            OpenSpecialSlot2();
            {
                GI_Label::TLabelGI* SC_Slot2_Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"SC_Slot2_Text"sv));
                if (Kind == phkGoods) {
                    SC_Slot2_Text->SetText(([&] {
                        pas::WideString intToStr_2 = pas::wide_int_to_str(([&] {
                            std::int32_t cpp_right = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, nullptr);
                            return Quantity * cpp_right;
                        }()));
                        pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.Sell"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_2), pas::WideString(), u"<Money>"_w, std::move(intToStr_2));
                    }()));
                } else {
                    SC_Slot2_Text->SetText(([&] {
                        pas::WideString intToStr_3 = pas::wide_int_to_str(Item->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false)));
                        pas::WideString localizedText_3 = aConst::LocalizedText(u"FormShip.Sell"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_3), pas::WideString(), u"<Money>"_w, std::move(intToStr_3));
                    }()));
                }
            }
        } else {
            CloseSpecialSlot2();
        }
        if (Kind == phkEquipment && pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && pas::in_range(static_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && static_cast<aItem::TWeapon*>(Item)->Ammo < static_cast<aItem::TWeapon*>(Item)->AmmoCapacity && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && (aPlayer::GetPlayer()->IsDockedToShip() || aPlayer::GetPlayer()->IsOnPlanet() && static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::oiDominator, aGalaxyStruct::oiUninhabited>(aPlayer::GetPlayer()->CurrentPlanet->OwnerId) ^ 1))) {
            OpenSpecialSlot3();
            {
                GI_Label::TLabelGI* SC_Slot3_Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"SC_Slot3_Text"sv));
                SC_Slot3_Text->SetText(([&] {
                    pas::WideString intToStr_4 = pas::wide_int_to_str((reinterpret_cast<aItem::TWeapon*>(Item)->AmmoCapacity - reinterpret_cast<aItem::TWeapon*>(Item)->Ammo) * aGalaxy::Galaxy->ScaleIntByTechLevel(10, 100));
                    pas::WideString localizedText_4 = aConst::LocalizedText(u"FormShip.Missile"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_4), pas::WideString(), u"<Money>"_w, std::move(intToStr_4));
                }()));
            }
        } else if (Kind == phkEquipment && pas::class_cast_if<aItem::TCistern*>(Item) != nullptr && ([&] {
            std::int32_t cpp_left = static_cast<aItem::TCistern*>(Item)->Fuel;
            return cpp_left < static_cast<aItem::TCistern*>(Item)->Capacity;
        }()) && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && (aPlayer::GetPlayer()->IsDockedToShip() || aPlayer::GetPlayer()->IsOnPlanet() && static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::oiDominator, aGalaxyStruct::oiUninhabited>(aPlayer::GetPlayer()->CurrentPlanet->OwnerId) ^ 1))) {
            OpenSpecialSlot3();
            {
                GI_Label::TLabelGI* SC_Slot3_Text_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"SC_Slot3_Text"sv));
                if (aPlayer::GetPlayer()->IsOnPlanet()) {
                    std::int32_t cpp_left_2 = pas::checked_cast<aItem::TCistern*>(Item)->Capacity;
                    I = aShip::CalculateRoundedFuelCost(cpp_left_2 - pas::checked_cast<aItem::TCistern*>(Item)->Fuel, aPlayer::GetPlayer()->CurrentPlanet->OwnerId);
                } else {
                    std::int32_t cpp_left_3 = pas::checked_cast<aItem::TCistern*>(Item)->Capacity;
                    I = aShip::CalculateRoundedFuelCost(cpp_left_3 - pas::checked_cast<aItem::TCistern*>(Item)->Fuel, aGalaxyStruct::oiUninhabited);
                }
                SC_Slot3_Text_2->SetText(([&] {
                    pas::WideString intToStr_5 = pas::wide_int_to_str(I);
                    pas::WideString localizedText_5 = aConst::LocalizedText(u"FormShip.Fuel"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_5), pas::WideString(), u"<Money>"_w, std::move(intToStr_5));
                }()));
            }
        } else if (Kind == phkEquipment && pas::class_cast_if<aItem::TFuelTanks*>(Item) != nullptr && ([&] {
            std::int32_t cpp_left_4 = static_cast<aItem::TFuelTanks*>(Item)->Fuel;
            return cpp_left_4 < static_cast<aItem::TFuelTanks*>(Item)->Capacity;
        }()) && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && (aPlayer::GetPlayer()->IsDockedToShip() || aPlayer::GetPlayer()->IsOnPlanet() && static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::oiDominator, aGalaxyStruct::oiUninhabited>(aPlayer::GetPlayer()->CurrentPlanet->OwnerId) ^ 1))) {
            OpenSpecialSlot3();
            {
                GI_Label::TLabelGI* SC_Slot3_Text_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"SC_Slot3_Text"sv));
                if (aPlayer::GetPlayer()->IsOnPlanet()) {
                    std::int32_t cpp_left_5 = pas::checked_cast<aItem::TFuelTanks*>(Item)->Capacity;
                    I = aShip::CalculateRoundedFuelCost(cpp_left_5 - pas::checked_cast<aItem::TFuelTanks*>(Item)->Fuel, aPlayer::GetPlayer()->CurrentPlanet->OwnerId);
                } else {
                    std::int32_t cpp_left_6 = pas::checked_cast<aItem::TFuelTanks*>(Item)->Capacity;
                    I = aShip::CalculateRoundedFuelCost(cpp_left_6 - pas::checked_cast<aItem::TFuelTanks*>(Item)->Fuel, aGalaxyStruct::oiUninhabited);
                }
                SC_Slot3_Text_3->SetText(([&] {
                    pas::WideString intToStr_6 = pas::wide_int_to_str(I);
                    pas::WideString localizedText_6 = aConst::LocalizedText(u"FormShip.Fuel"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_6), pas::WideString(), u"<Money>"_w, std::move(intToStr_6));
                }()));
            }
        } else {
            CloseSpecialSlot3();
        }
        TotalRepair = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->Inventory) - 1); cpp_range.next(I); ) {
            Equipment = pas::list_at<aItem::TEquipment>(PlayerHoldShip->Inventory, I);
            if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Equipment); !(weapon != nullptr) || weapon->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair || aPlayer::GetPlayer()->CanRepairArtefactsAtLocation()) {
                if (aShip::TShip_CanRepairEquipmentTech(PlayerHoldShip, Equipment) && Equipment != Item && (Equipment->EquippedFlag != 0 || pas::class_cast_if<aItem::THull*>(Equipment) != nullptr) && Equipment->NeedsRepair()) {
                    TotalRepair += aItem::TEquipment_CalculateRepairCost(Equipment);
                }
            }
        }
        {
            GI_Label::TLabelGI* FullRepareSum = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"FullRepareSum"sv));
            if (TotalRepair > 0) {
                FullRepareSum->SetText(([&] {
                    pas::WideString intToStr_7 = pas::wide_int_to_str(TotalRepair);
                    pas::WideString localizedText_7 = aConst::LocalizedText(u"FormShip.RepairAll"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_7), pas::WideString(), u"<Money>"_w, std::move(intToStr_7));
                }()));
            } else {
                FullRepareSum->SetText(aConst::LocalizedText(u"FormShip.FullRepareNo"_wref.get()));
            }
        }
        {
            GI_GraphButton::TGraphButtonGI* SC_RepareFull_But = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_RepareFull_But"sv));
            SC_RepareFull_But->SetDisabled(TotalRepair <= 0);
            SC_RepareFull_But->UpCallback = pas::bind_method<&TfShip2::RepairAllClicked>(this);
            SC_RepareFull_But->MouseEnterCallback = pas::bind_method<&TfShip2::RepairAllMouseEnter>(this);
            SC_RepareFull_But->MouseLeaveCallback = pas::bind_method<&TfShip2::RepairAllMouseLeave>(this);
        }
    }

    void TfShip2::UpdateActionCursor(std::uint8_t CanTake) {
        if (SelectedHoldKind == phkEmpty) {
            if (CanTake) {
                if (!IsCursorImageSelected(u"Take"sv)) {
                    SetCursorByName(u"Take"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"Main"sv)) {
                SetCursorByName(u"Main"_wref.get());
            }
        } else if (SelectedHoldKind == phkEquipment) {
            const pas::WideString& cpp_arg = pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(SelectedHoldItem), u"s"});
            GI_MessageLoop::TMessageLoopGI* self = this;
            self->SetCursorImage(cpp_arg, ClassesImports::Point(16, 16));
        } else if (SelectedHoldKind == phkGoods) {
            SetCursorImage(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(SelectedGoodsIndex))}), ClassesImports::Point(16, 16));
        } else if (SelectedHoldKind == phkArtefact) {
            const pas::WideString& cpp_arg_2 = pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(SelectedHoldItem), u"s"});
            GI_MessageLoop::TMessageLoopGI* self_2 = this;
            self_2->SetCursorImage(cpp_arg_2, ClassesImports::Point(16, 16));
        }
    }

    void TfShip2::TrainSkillClicked(GI_MessageLoop::TObjectGI* Sender) {
        aGalaxy::Galaxy->CheckIntegrityChecksum1(434);
        PlayerHoldShip->TrainSkill(static_cast<aGalaxyStruct::TPilotSkill>(Sender->UserValue));
        if (TfShip2::CanUsePlayerExperience()) {
            aPlayer::GetPlayer()->FreeExperience = PlayerHoldShip->FreeExperience;
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(435);
        RefreshShipView();
        ShowShipPropertyInfo(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Skill", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Sender->UserValue)), "z"})))));
        aGalaxy::Galaxy->CheckIntegrityChecksum1(436);
        if (aPlayer::GetPlayer() == PlayerHoldShip) {
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckAllSkillsAchievement();
        }
        PlayerHoldShip->ScriptItemsAct(aConst::satOnPlayerSkillIncrease, nullptr, nullptr, 0);
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(437);
    }

    void TfShip2::EquipmentSlotMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t Quantity{};
        aItem::TEquipment* Item{};
        pas::WideString Text{};
        std::int32_t ActionResult{};
        aConst::TItemType ItemType = TfShip2::SlotToTip(pas::view(Sender->ControlName));
        std::int32_t Slot = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        if (SelectedHoldKind == phkEmpty && PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot) == nullptr) {
            return;
        }
        if ((aPlayer::GetPlayer()->InHyperspace || pas::list_count(aScript::QueuedArcadeBattles) > 0) && static_cast<std::uint8_t>(aGalaxy::Galaxy->IsArcadeEquipmentChangeEnabled() ^ 1)) {
            GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
            GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"FormShip.NoEquipChangeInHyper"_wref.get()), GI_MessageBox::mbgOK, 0, 0, 0);
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(438);
        if (SelectedHoldKind == phkEmpty) {
            Item = PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot);
            if (Item != nullptr) {
                if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr && (pas::class_cast_if<aItem::TEngine*>(Item) != nullptr || pas::class_cast_if<aItem::TFuelTanks*>(Item) != nullptr || pas::class_cast_if<aItem::TCargoHook*>(Item) != nullptr && PlayerHoldShip->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion))) {
                    GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                    GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                        pas::WideString removeTextTagsW = EC_Str::RemoveTextTagsW(Item->GetDisplayName());
                        pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.RuinMoveItemInvalid"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW));
                    }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    return;
                }
                if (pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr && (pas::class_cast_if<aItem::TEngine*>(Item) != nullptr || pas::class_cast_if<aItem::TFuelTanks*>(Item) != nullptr)) {
                    GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                    GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                        pas::WideString removeTextTagsW_2 = EC_Str::RemoveTextTagsW(Item->GetDisplayName());
                        pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.TrancMoveItemInvalid"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW_2));
                    }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    return;
                }
                if (Item->EquippedFlag != 0) {
                    PlayerHoldShip->UnequipItem(Item);
                }
                pas::list_delete(PlayerHoldShip->Inventory, pas::list_indexof(PlayerHoldShip->Inventory, reinterpret_cast<void*>(Item)));
                PlayerHoldShip->RebuildEquipmentCache();
                PlayerHoldShip->RefreshDerivedStats(true);
                SelectedHoldKind = phkEquipment;
                SelectedHoldOrigin = 0;
                SelectedHoldSlot = -(Slot + 1);
                SelectedHoldUsesDisplayOrder = false;
                SelectedHoldItem = Item;
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item)) {
                    weapon->Target = nullptr;
                }
                UpdateActionCursor(true);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(439);
                GR_Main::SoundManager->PlaySound(u"Sound.SlotGet"_wref.get());
            }
        } else if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_MicroModule && PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot) != nullptr && PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot)->MicroModuleIndex == 0 && ([&] {
            aItem::TEquipment* findEquippedItemInSlot = PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot);
            aItem::TMicroModule* cpp_arg = pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem);
            return cpp_arg->CanInstallOn(findEquippedItemInSlot);
        }())) {
            Item = PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot);
            Text = ([&] {
                pas::WideString plainName = pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->GetPlainName();
                pas::WideString removeTextTagsW_3 = EC_Str::RemoveTextTagsW(Item->GetDisplayName());
                pas::WideString localizedText = aConst::LocalizedText(u"MicroModuls.AddToItem"_wref.get());
                return aMyFunction::FormatText2(std::move(localizedText), u"<color=255,240,100>"_w, u"<ModuleName>"_w, std::move(plainName), u"<ItemName>"_w, std::move(removeTextTagsW_3));
            }());
            if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                PlayerHoldShip->ScriptItemsAct(aConst::satOnPlayerUseMM, Item, SelectedHoldItem, 0);
                aItem::ApplyMicroModule(pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->MicroModuleIndex - 1, Item);
                pas::free(SelectedHoldItem);
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
                PlayerHoldShip->RefreshDerivedStats(true);
                UpdateActionCursor(true);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(440);
            }
        } else if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_Cistern && pas::checked_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel > 0 && ItemType == aConst::t_FuelTanks && PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot) != nullptr && ([&] {
            std::int32_t cpp_left = pas::checked_cast<aItem::TFuelTanks*>(PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot))->Fuel;
            return cpp_left < pas::checked_cast<aItem::TFuelTanks*>(PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot))->Capacity;
        }())) {
            Item = PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot);
            Text = aConst::LocalizedColorText(u"Items.Cistern.ToFuelTanks"_wref.get());
            if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                {
                    std::int32_t fuel = pas::checked_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel;
                    std::int32_t cpp_left_2 = pas::checked_cast<aItem::TFuelTanks*>(Item)->Capacity;
                    std::int32_t cpp_arg_2 = cpp_left_2 - pas::checked_cast<aItem::TFuelTanks*>(Item)->Fuel;
                    Quantity = std::min<std::int32_t>(fuel, cpp_arg_2);
                }
                static_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel -= Quantity;
                pas::checked_cast<aItem::TFuelTanks*>(Item)->Fuel += Quantity;
                PlayerHoldShip->RefreshDerivedStats(true);
                UpdateActionCursor(true);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(441);
            }
        } else if (SelectedHoldKind == phkEquipment && TfShip2::IsCompatibleSlot(ItemType, SelectedHoldItem->ItemType)) {
            Item = PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot);
            if (Item != nullptr && Item->EquippedFlag != 0) {
                PlayerHoldShip->UnequipItem(Item);
            }
            pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(SelectedHoldItem));
            pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | Slot;
            static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
            if (aItem::TWeapon* weapon_2 = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                weapon_2->Target = nullptr;
            }
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr && pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem) != nullptr && pas::in_range(static_cast<aItem::TWeapon*>(SelectedHoldItem)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && PlayerHoldShip->GetRadar() == nullptr) {
                GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"FormShip.TrancMissileWarning"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
            SelectedHoldKind = phkEmpty;
            SelectedHoldItem = nullptr;
            if (Item != nullptr) {
                pas::list_delete(PlayerHoldShip->Inventory, pas::list_indexof(PlayerHoldShip->Inventory, reinterpret_cast<void*>(Item)));
                SelectedHoldKind = phkEquipment;
                SelectedHoldOrigin = 0;
                SelectedHoldSlot = -(Slot + 1);
                SelectedHoldUsesDisplayOrder = false;
                SelectedHoldItem = Item;
            }
            UpdateActionCursor(true);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(442);
        } else if (pas::in_range(SelectedHoldKind, static_cast<std::int32_t>(phkGoods), static_cast<std::int32_t>(phkGoods)) && PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot) != nullptr) {
            Item = PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot);
            ActionResult = 0;
            if (Item->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnAnotherGoods, PlayerHoldShip, reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsIndex))), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsQuantity))), 0);
            }
            if (ActionResult != 0) {
                Quantity = pas::abs(ActionResult);
                if (Quantity < SelectedGoodsQuantity) {
                    SelectedGoodsCost -= System::Round(pas::real_divide(Quantity, SelectedGoodsQuantity) * SelectedGoodsCost);
                    SelectedGoodsQuantity -= Quantity;
                } else {
                    SelectedHoldKind = phkEmpty;
                    SelectedGoodsQuantity = 0;
                    SelectedGoodsCost = 0;
                }
            }
            PlayerHoldShip->RefreshDerivedStats(true);
            UpdateActionCursor(true);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(440);
            if (ActionResult < 0) {
                CloseClicked(nullptr);
            }
        } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot) != nullptr) {
            Item = PlayerHoldShip->FindEquippedItemInSlot(ItemType, Slot);
            ActionResult = 0;
            if (SelectedHoldItem->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnAnotherItem, PlayerHoldShip, Item, nullptr, ActionResult);
            }
            if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(SelectedHoldItem) != nullptr) {
                ActionResult = aScript::RunItemConfigActionCode(SelectedHoldItem, aConst::satOnAnotherItem, PlayerHoldShip, Item, nullptr, ActionResult);
            }
            if (Item->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
            }
            if (aItem::TEquipmentWithActCode* equipmentWithActCode = pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item)) {
                ActionResult = aScript::RunItemConfigActionCode(Item, aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
            }
            if (pas::in_set<1, 1, 3, 3>(ActionResult)) {
                pas::free(SelectedHoldItem);
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
            }
            PlayerHoldShip->RefreshDerivedStats(true);
            UpdateActionCursor(true);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(440);
            if (ActionResult == 3) {
                CloseClicked(nullptr);
            }
        }
        if (PlayerHoldShip->InHyperspace || pas::list_count(aScript::QueuedArcadeBattles) > 0) {
            RefreshShipView();
        } else if (!RemoteHoldVisible) {
            ShipStateChanged = true;
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    void TfShip2::ArtefactSlotMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        aItem::TEquipment* Equipment{};
        aItem::TArtefact* Item{};
        std::int32_t Slot = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        aGalaxy::Galaxy->CheckIntegrityChecksum1(443);
        if (SelectedHoldKind == phkEmpty) {
            Equipment = PlayerHoldShip->FindEquippedItemInSlot(aConst::t_Artefact, Slot);
            if (Equipment != nullptr) {
                Item = pas::checked_cast<aItem::TArtefact*>(Equipment);
                Item->Unequip();
                pas::list_delete(PlayerHoldShip->Artefacts, pas::list_indexof(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(Item)));
                PlayerHoldShip->RefreshDerivedStats(true);
                SelectedHoldKind = phkArtefact;
                SelectedHoldOrigin = 0;
                SelectedHoldSlot = -(Slot + 1);
                SelectedHoldUsesDisplayOrder = false;
                SelectedHoldItem = Item;
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(444);
                UpdateActionCursor(true);
                GR_Main::SoundManager->PlaySound(u"Sound.SlotGet"_wref.get());
            }
        } else if (SelectedHoldKind == phkArtefact) {
            Equipment = PlayerHoldShip->FindEquippedItemInSlot(aConst::t_Artefact, Slot);
            if (Equipment != nullptr && Equipment->EquippedFlag != 0) {
                Equipment->Unequip();
            }
            pas::list_add(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(SelectedHoldItem));
            pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = Slot;
            static_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
            static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
            SelectedHoldKind = phkEmpty;
            SelectedHoldItem = nullptr;
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            if (Equipment != nullptr) {
                pas::list_delete(PlayerHoldShip->Artefacts, pas::list_indexof(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(Equipment)));
                SelectedHoldKind = phkArtefact;
                SelectedHoldOrigin = 0;
                SelectedHoldSlot = -(Slot + 1);
                SelectedHoldUsesDisplayOrder = false;
                SelectedHoldItem = Equipment;
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(444);
            UpdateActionCursor(true);
        }
        if (PlayerHoldShip->InHyperspace || pas::list_count(aScript::QueuedArcadeBattles) > 0) {
            RefreshShipView();
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(445);
        } else if (!RemoteHoldVisible) {
            ShipStateChanged = true;
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    void TfShip2::UseOnArtefactSlot(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t Quantity{};
        std::int32_t ActionResult{};
        std::int32_t Slot = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        aGalaxy::Galaxy->CheckIntegrityChecksum1(443);
        aItem::TItem* Item = PlayerHoldShip->FindEquippedItemInSlot(aConst::t_Artefact, Slot);
        if (Item == nullptr) {
            return;
        }
        if (pas::in_range(SelectedHoldKind, static_cast<std::int32_t>(phkGoods), static_cast<std::int32_t>(phkGoods))) {
            if (Item->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnAnotherGoods, PlayerHoldShip, reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsIndex))), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsQuantity))), 0);
            } else {
                ActionResult = aScript::RunItemConfigActionCode(Item, aConst::satOnAnotherGoods, PlayerHoldShip, reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsIndex))), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsQuantity))), 0);
            }
            if (ActionResult != 0) {
                Quantity = pas::abs(ActionResult);
                if (Quantity < SelectedGoodsQuantity) {
                    SelectedGoodsCost -= System::Round(pas::real_divide(Quantity, SelectedGoodsQuantity) * SelectedGoodsCost);
                    SelectedGoodsQuantity -= Quantity;
                } else {
                    SelectedHoldKind = phkEmpty;
                    SelectedGoodsQuantity = 0;
                    SelectedGoodsCost = 0;
                }
            }
            PlayerHoldShip->RefreshDerivedStats(true);
            UpdateActionCursor(true);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(440);
            if (ActionResult < 0) {
                CloseClicked(nullptr);
            }
        } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind)) {
            ActionResult = 0;
            if (SelectedHoldItem->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnAnotherItem, PlayerHoldShip, Item, nullptr, ActionResult);
            }
            if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(SelectedHoldItem) != nullptr) {
                ActionResult = aScript::RunItemConfigActionCode(SelectedHoldItem, aConst::satOnAnotherItem, PlayerHoldShip, Item, nullptr, ActionResult);
            }
            if (Item->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
            }
            ActionResult = aScript::RunItemConfigActionCode(Item, aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
            if (pas::in_set<1, 1, 3, 3>(ActionResult)) {
                pas::free(SelectedHoldItem);
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
            }
            PlayerHoldShip->RefreshDerivedStats(true);
            UpdateActionCursor(true);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(440);
            if (ActionResult == 3) {
                CloseClicked(nullptr);
            }
        } else {
            return;
        }
        if (PlayerHoldShip->InHyperspace || pas::list_count(aScript::QueuedArcadeBattles) > 0) {
            RefreshShipView();
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(445);
        } else if (!RemoteHoldVisible) {
            ShipStateChanged = true;
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::RemoteHoldItemMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t J{};
        std::int32_t Count{};
        std::int32_t Missing{};
        std::int32_t DialogCount{};
        std::int32_t Available{};
        pas::WideString Text{};
        aItem::TEquipment* Equipment{};
        TPlayerHoldUnit* Entry{};
        TPlayerHoldUnit* Swap{};
        aItem::TCountableItem* Stack{};
        aItem::TFuelTanks* FuelFrom{};
        aItem::TFuelTanks* FuelTo{};
        std::uint8_t NeedsRefresh{};
        std::int32_t ActionResult{};
        // Captures Self and NeedsRefresh.
        auto RefreshAfterTransfer = [&]() -> void {
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(445);
            if (PlayerHoldShip->InHyperspace || pas::list_count(aScript::QueuedArcadeBattles) > 0 || this->RemoteHoldVisible && NeedsRefresh) {
                RefreshShipView();
            } else if (!this->RemoteHoldVisible) {
                this->ShipStateChanged = true;
                this->ReopenRequested = true;
                this->PlayTransitionSounds = false;
                CloseClicked(nullptr);
            }
        };
        std::int32_t Index = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        std::uint8_t Expanded = pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr;
        if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_Hull) {
            if (!RemoteHoldVisible) {
                const pas::WideString& localizedText = aConst::LocalizedText(u"FormShip.HullInHoldError"_wref.get());
                GI_MessageLoop::TMessageLoopGI* self = this;
                GI_MessageBox::ShowMessageBoxGI(self, localizedText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
            return;
        }
        if (!Expanded) {
            Index += HoldFirstIndex;
            if (Index < 0 || pas::list_count(PlayerHoldEntries) <= Index) {
                Entry = nullptr;
            } else {
                Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, Index);
            }
        } else {
            Index += RemoteHoldFirstOrder;
            J = fShip2::FindPlayerHoldIndexByOrder(Index);
            if (J < 0) {
                Entry = nullptr;
            } else {
                Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, J);
            }
        }
        if (SelectedHoldKind == phkEmpty && Entry == nullptr) {
            return;
        }
        NeedsRefresh = false;
        aGalaxy::Galaxy->CheckIntegrityChecksum1(446);
        if (SelectedHoldKind == phkEmpty) {
            if (Entry != nullptr) {
                TakeHoldEntry(Entry, Index, Expanded);
                NeedsRefresh = true;
                PlayerHoldShip->RefreshDerivedStats(true);
                UpdateActionCursor(true);
                GR_Main::SoundManager->PlaySound(u"Sound.SlotGet"_wref.get());
            }
            RefreshAfterTransfer();
            return;
        }
        if (SelectedHoldKind == phkGoods) {
            Count = SelectedGoodsQuantity;
            if (!RemoteHoldVisible) {
                Available = std::max<std::int32_t>(0, PlayerHoldShip->CargoFreeSpace);
                DialogCount = std::min<std::int32_t>(Count, Available);
                if (SelectedHoldOrigin == 1 && (static_cast<std::uint8_t>(Expanded ^ 1) || Entry == nullptr) && Count > 1) {
                    if (Count > 1) {
                        if (([&] {
                            const pas::WideString& formatText1 = ([&] {
                                pas::WideString lowerCaseWideString = EC_Str::LowerCaseWideString(aItem::GetStackableItemTypeName(static_cast<aConst::TItemType>(SelectedGoodsIndex)));
                                pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.FromStorageItem"_wref.get());
                                return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString));
                            }());
                            const pas::WideString& cpp_arg = pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(SelectedGoodsIndex))});
                            GI_MessageLoop::TMessageLoopGI* self_2 = this;
                            return fCount2::ShowCountDialog(self_2, cpp_arg, formatText1, 0, Count, Count, 0.0f, Available, 0, DialogCount);
                        }()) != 1) {
                            aGalaxy::Galaxy->PrimeIntegrityChecksum1(459);
                            return;
                        }
                    }
                    Count = DialogCount;
                }
            }
            if (Count < 1 || SelectedGoodsQuantity < Count) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(460);
                return;
            }
            if (Entry != nullptr && pas::is_one_of<phkEquipment, phkArtefact>(Entry->Kind)) {
                if (Entry->Item->ScriptItem != nullptr) {
                    ActionResult = reinterpret_cast<aScript::TScriptItem*>(Entry->Item->ScriptItem)->RunActionCode(aConst::satOnAnotherGoods, PlayerHoldShip, reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsIndex))), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Count))), 0);
                } else {
                    ActionResult = aScript::RunItemConfigActionCode(Entry->Item, aConst::satOnAnotherGoods, PlayerHoldShip, reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsIndex))), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Count))), 0);
                }
                if (ActionResult != 0) {
                    Count = std::min<std::int32_t>(Count, pas::abs(ActionResult));
                    if (Count < SelectedGoodsQuantity) {
                        SelectedGoodsCost -= System::Round(pas::real_divide(Count, SelectedGoodsQuantity) * SelectedGoodsCost);
                        SelectedGoodsQuantity -= Count;
                    } else {
                        SelectedHoldKind = phkEmpty;
                        SelectedGoodsQuantity = 0;
                        SelectedGoodsCost = 0;
                    }
                    PlayerHoldShip->RefreshDerivedStats(true);
                    UpdateActionCursor(true);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(4440);
                    if (ActionResult < 0) {
                        CloseClicked(nullptr);
                    }
                    return;
                }
            }
            PlayerHoldShip->CargoGoods[SelectedGoodsIndex].Count += Count;
            PlayerHoldShip->CargoGoods[SelectedGoodsIndex].TotalCost += System::Round(pas::real_divide(Count, SelectedGoodsQuantity) * SelectedGoodsCost);
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            if (!Expanded) {
                if (Entry == nullptr) {
                    Missing = Index - pas::list_count(PlayerHoldEntries) + 1;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, Missing - 1); cpp_range.next(J); ) {
                        Entry = pas::make_object<TPlayerHoldUnit>();
                        pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                        Entry->Kind = phkEmpty;
                    }
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, pas::list_count(PlayerHoldEntries) - 1);
                } else if (Entry->Kind != phkEmpty) {
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_insert(PlayerHoldEntries, Index, reinterpret_cast<void*>(Entry));
                    Entry->Kind = phkEmpty;
                }
                Entry->DisplayOrder = fShip2::FindFreePlayerHoldOrder();
                Entry->Kind = phkGoods;
                Entry->GoodsIndex = SelectedGoodsIndex;
                SelectedGoodsCost -= System::Round(pas::real_divide(Count, SelectedGoodsQuantity) * SelectedGoodsCost);
                SelectedGoodsQuantity -= Count;
                fShip2::RemoveEmptyPlayerHoldSlot(pas::list_indexof(PlayerHoldEntries, reinterpret_cast<void*>(Entry)) + 1);
                if (SelectedGoodsQuantity <= 0) {
                    SelectedHoldKind = phkEmpty;
                } else {
                    ReturnSelectedHoldEntry();
                }
            } else {
                Swap = nullptr;
                if (Entry == nullptr) {
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                } else if (Entry->Kind != phkEmpty) {
                    Swap = Entry;
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                }
                Entry->DisplayOrder = Index;
                Entry->Kind = phkGoods;
                Entry->GoodsIndex = SelectedGoodsIndex;
                SelectedGoodsCost -= System::Round(pas::real_divide(Count, SelectedGoodsQuantity) * SelectedGoodsCost);
                SelectedGoodsQuantity -= Count;
                if (Swap != nullptr) {
                    if (SelectedGoodsQuantity > 0) {
                        GR_Main::RaiseWideMessage(u"move goods"_wref.get());
                    }
                    TakeHoldEntry(Swap, Index, Expanded);
                } else if (SelectedGoodsQuantity <= 0) {
                    SelectedHoldKind = phkEmpty;
                    SelectedHoldItem = nullptr;
                } else {
                    ReturnSelectedHoldEntry();
                }
                fShip2::RestorePlayerHoldDisplayOrder();
            }
            {
                const std::int32_t cpp_first = pas::list_count(PlayerHoldEntries) - 1;
                if (cpp_first >= 0) {
                    for (J = cpp_first; J >= 0; --J) {
                        Swap = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, J);
                        if (Swap->Kind == phkGoods && SelectedGoodsIndex == Swap->GoodsIndex && Entry != Swap) {
                            if (!Expanded) {
                                Swap->Item = nullptr;
                                Swap->Kind = phkEmpty;
                            } else {
                                pas::list_delete(PlayerHoldEntries, J);
                                pas::free(Swap);
                            }
                        }
                    }
                }
            }
            PlayerHoldShip->RefreshDerivedStats(true);
            UpdateActionCursor(true);
            RefreshAfterTransfer();
            return;
        }
        if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem) != nullptr) {
            Count = static_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount;
            if (!RemoteHoldVisible) {
                Available = std::max<std::int32_t>(0, PlayerHoldShip->CargoFreeSpace);
                DialogCount = std::min<std::int32_t>(Count, Available);
                if (SelectedHoldOrigin == 1 && (static_cast<std::uint8_t>(Expanded ^ 1) || Entry == nullptr || aItem::TCountableItem_CanMerge(reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem), Entry->Item))) {
                    if (Count > 1) {
                        if (([&] {
                            const pas::WideString& formatText1_2 = ([&] {
                                pas::WideString lowerCaseWideString_2 = EC_Str::LowerCaseWideString(aItem::GetStackableItemName(SelectedHoldItem));
                                pas::WideString localizedText_3 = aConst::LocalizedText(u"FormShip.FromStorageItem"_wref.get());
                                return aMyFunction::FormatText1(std::move(localizedText_3), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString_2));
                            }());
                            const pas::WideString& cpp_arg_2 = pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(SelectedHoldItem), u"s"});
                            GI_MessageLoop::TMessageLoopGI* self_3 = this;
                            return fCount2::ShowCountDialog(self_3, cpp_arg_2, formatText1_2, 0, Count, Count, 0.0f, Available, 0, DialogCount);
                        }()) != 1) {
                            return;
                        }
                    }
                    Count = DialogCount;
                }
            }
            if (reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount != Count || Entry != nullptr && aItem::TCountableItem_CanMerge(reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem), Entry->Item)) {
                if (static_cast<std::uint8_t>(RemoteHoldVisible ^ 1) && SelectedHoldOrigin == 0 && Entry != nullptr && Entry->Item != nullptr && pas::is_one_of<phkEquipment, phkArtefact>(Entry->Kind) && static_cast<std::uint8_t>(aItem::TCountableItem_CanMerge(reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem), Entry->Item) ^ 1)) {
                    ActionResult = 0;
                    if (SelectedHoldItem->ScriptItem != nullptr) {
                        ActionResult = reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnAnotherItem, PlayerHoldShip, Entry->Item, nullptr, ActionResult);
                    }
                    if (Entry->Item->ScriptItem != nullptr) {
                        ActionResult = reinterpret_cast<aScript::TScriptItem*>(Entry->Item->ScriptItem)->RunActionCode(aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
                    }
                    if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Entry->Item) != nullptr) {
                        ActionResult = aScript::RunItemConfigActionCode(Entry->Item, aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
                    }
                    if (pas::in_set<1, 1, 3, 3>(ActionResult)) {
                        pas::free(SelectedHoldItem);
                        SelectedHoldKind = phkEmpty;
                        SelectedHoldItem = nullptr;
                    }
                    if (pas::in_range(ActionResult, 1, 3)) {
                        PlayerHoldShip->RefreshDerivedStats(true);
                        aGalaxy::Galaxy->PrimeIntegrityChecksum1(461);
                        UpdateActionCursor(true);
                        RefreshShipView();
                        if (ActionResult == 3) {
                            CloseClicked(nullptr);
                        }
                        return;
                    }
                    Count = reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount;
                }
                if (Count < 1 || reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount < Count) {
                    return;
                }
                if (Count < reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount) {
                    Stack = reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->Split(Count);
                    if (Entry != nullptr && Entry->Item != nullptr && aItem::TCountableItem_CanMerge(Stack, Entry->Item)) {
                        aItem::TCountableItem_Merge(reinterpret_cast<aItem::TCountableItem*>(Entry->Item), Stack);
                        pas::free(Stack);
                        ReturnSelectedHoldEntry();
                        return;
                    }
                    pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(Stack));
                } else {
                    Stack = reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem);
                    SelectedHoldKind = phkEmpty;
                    SelectedHoldItem = nullptr;
                    if (Entry != nullptr && Entry->Item != nullptr && aItem::TCountableItem_CanMerge(Stack, Entry->Item)) {
                        aItem::TCountableItem_Merge(reinterpret_cast<aItem::TCountableItem*>(Entry->Item), Stack);
                        pas::free(Stack);
                        SelectedHoldItem = nullptr;
                        SelectedHoldKind = phkEmpty;
                        return;
                    }
                    pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(Stack));
                }
                GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
                if (!Expanded) {
                    if (Entry == nullptr) {
                        Missing = Index - pas::list_count(PlayerHoldEntries) + 1;
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Missing - 1); cpp_range_2.next(J); ) {
                            Entry = pas::make_object<TPlayerHoldUnit>();
                            pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                            Entry->Kind = phkEmpty;
                        }
                        Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, pas::list_count(PlayerHoldEntries) - 1);
                    } else if (Entry->Kind != phkEmpty) {
                        Entry = pas::make_object<TPlayerHoldUnit>();
                        pas::list_insert(PlayerHoldEntries, Index, reinterpret_cast<void*>(Entry));
                        Entry->Kind = phkEmpty;
                    }
                    Entry->DisplayOrder = fShip2::FindFreePlayerHoldOrder();
                    Entry->Kind = phkEquipment;
                    Entry->ItemId = Stack->Id;
                    Entry->Item = Stack;
                    fShip2::RemoveEmptyPlayerHoldSlot(pas::list_indexof(PlayerHoldEntries, reinterpret_cast<void*>(Entry)) + 1);
                    if (SelectedHoldItem != nullptr) {
                        ReturnSelectedHoldEntry();
                    }
                } else {
                    Swap = nullptr;
                    if (Entry == nullptr) {
                        Entry = pas::make_object<TPlayerHoldUnit>();
                        pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                    } else if (Entry->Kind != phkEmpty) {
                        Swap = Entry;
                        Entry = pas::make_object<TPlayerHoldUnit>();
                        pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                    }
                    Entry->DisplayOrder = Index;
                    Entry->Kind = phkEquipment;
                    Entry->ItemId = Stack->Id;
                    Entry->Item = Stack;
                    if (Swap != nullptr) {
                        if (reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount > 0) {
                            GR_Main::RaiseWideMessage(u"move protoplasm"_wref.get());
                        }
                        TakeHoldEntry(Swap, Index, Expanded);
                    } else if (SelectedHoldItem != nullptr) {
                        ReturnSelectedHoldEntry();
                    }
                    fShip2::RestorePlayerHoldDisplayOrder();
                }
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldEntries) - 1); cpp_range_3.next(J); ) {
                    Swap = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, J);
                    if (Swap->Kind == phkEquipment && aItem::TCountableItem_Merge(pas::checked_cast<aItem::TCountableItem*>(Entry->Item), Swap->Item)) {
                        pas::list_delete(PlayerHoldShip->Inventory, pas::list_indexof(PlayerHoldShip->Inventory, reinterpret_cast<void*>(Swap->Item)));
                        pas::free(Swap->Item);
                        Swap->Item = nullptr;
                        Swap->Kind = phkEmpty;
                    }
                }
                PlayerHoldShip->RefreshDerivedStats(true);
                UpdateActionCursor(true);
                RefreshAfterTransfer();
                return;
            }
        }
        if (SelectedHoldKind == phkEquipment) {
            if (!RemoteHoldVisible) {
                if (SelectedHoldItem->ItemType == aConst::t_Cistern && Entry != nullptr && Entry->Kind == phkEquipment && Entry->Item->ItemType == aConst::t_Cistern && pas::checked_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel > 0 && ([&] {
                    std::int32_t cpp_left = pas::checked_cast<aItem::TCistern*>(Entry->Item)->Fuel;
                    return cpp_left < pas::checked_cast<aItem::TCistern*>(Entry->Item)->Capacity;
                }())) {
                    Text = aConst::LocalizedColorText(u"Items.Cistern.Merge"_wref.get());
                    if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                        {
                            std::int32_t fuel = pas::checked_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel;
                            std::int32_t cpp_left_2 = pas::checked_cast<aItem::TCistern*>(Entry->Item)->Capacity;
                            std::int32_t cpp_arg_3 = cpp_left_2 - pas::checked_cast<aItem::TCistern*>(Entry->Item)->Fuel;
                            Count = std::min<std::int32_t>(fuel, cpp_arg_3);
                        }
                        static_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel -= Count;
                        pas::checked_cast<aItem::TCistern*>(Entry->Item)->Fuel += Count;
                        PlayerHoldShip->RefreshDerivedStats(true);
                        aGalaxy::Galaxy->PrimeIntegrityChecksum1(454);
                        UpdateActionCursor(true);
                        RefreshShipView();
                        return;
                    }
                }
                if (SelectedHoldItem->ItemType == aConst::t_FuelTanks && Entry != nullptr && Entry->Kind == phkEquipment && Entry->Item->ItemType == aConst::t_FuelTanks) {
                    FuelFrom = pas::checked_cast<aItem::TFuelTanks*>(SelectedHoldItem);
                    FuelTo = pas::checked_cast<aItem::TFuelTanks*>(Entry->Item);
                    if (FuelFrom->Fuel > 0 && FuelTo->Fuel < FuelTo->Capacity) {
                        Text = aConst::LocalizedColorText(u"Items.Cistern.Merge"_wref.get());
                        if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                            Count = std::min<std::int32_t>(FuelFrom->Fuel, FuelTo->Capacity - FuelTo->Fuel);
                            FuelFrom->Fuel -= Count;
                            FuelTo->Fuel += Count;
                            PlayerHoldShip->RefreshDerivedStats(true);
                            aGalaxy::Galaxy->PrimeIntegrityChecksum1(455);
                            UpdateActionCursor(true);
                            RefreshShipView();
                            return;
                        }
                    }
                }
                if (SelectedHoldItem->ItemType == aConst::t_Cistern && Entry != nullptr && Entry->Kind == phkEquipment && Entry->Item->ItemType == aConst::t_FuelTanks && pas::checked_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel > 0 && ([&] {
                    std::int32_t cpp_left_3 = pas::checked_cast<aItem::TFuelTanks*>(Entry->Item)->Fuel;
                    return cpp_left_3 < pas::checked_cast<aItem::TFuelTanks*>(Entry->Item)->Capacity;
                }())) {
                    Text = aConst::LocalizedColorText(u"Items.Cistern.ToFuelTanks"_wref.get());
                    if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                        {
                            std::int32_t fuel_2 = pas::checked_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel;
                            std::int32_t cpp_left_4 = pas::checked_cast<aItem::TFuelTanks*>(Entry->Item)->Capacity;
                            std::int32_t cpp_arg_4 = cpp_left_4 - pas::checked_cast<aItem::TFuelTanks*>(Entry->Item)->Fuel;
                            Count = std::min<std::int32_t>(fuel_2, cpp_arg_4);
                        }
                        static_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel -= Count;
                        pas::checked_cast<aItem::TFuelTanks*>(Entry->Item)->Fuel += Count;
                        PlayerHoldShip->RefreshDerivedStats(true);
                        aGalaxy::Galaxy->PrimeIntegrityChecksum1(456);
                        UpdateActionCursor(true);
                        RefreshShipView();
                        return;
                    }
                }
                if (SelectedHoldItem->ItemType == aConst::t_FuelTanks && Entry != nullptr && Entry->Kind == phkEquipment && Entry->Item->ItemType == aConst::t_Cistern && pas::checked_cast<aItem::TFuelTanks*>(SelectedHoldItem)->Fuel > 0 && ([&] {
                    std::int32_t cpp_left_5 = pas::checked_cast<aItem::TCistern*>(Entry->Item)->Fuel;
                    return cpp_left_5 < pas::checked_cast<aItem::TCistern*>(Entry->Item)->Capacity;
                }())) {
                    Text = aConst::LocalizedColorText(u"Items.Cistern.FromFuelTanks"_wref.get());
                    if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                        {
                            std::int32_t fuel_3 = pas::checked_cast<aItem::TFuelTanks*>(SelectedHoldItem)->Fuel;
                            std::int32_t cpp_left_6 = pas::checked_cast<aItem::TCistern*>(Entry->Item)->Capacity;
                            std::int32_t cpp_arg_5 = cpp_left_6 - pas::checked_cast<aItem::TCistern*>(Entry->Item)->Fuel;
                            Count = std::min<std::int32_t>(fuel_3, cpp_arg_5);
                        }
                        static_cast<aItem::TFuelTanks*>(SelectedHoldItem)->Fuel -= Count;
                        pas::checked_cast<aItem::TCistern*>(Entry->Item)->Fuel += Count;
                        PlayerHoldShip->RefreshDerivedStats(true);
                        aGalaxy::Galaxy->PrimeIntegrityChecksum1(457);
                        UpdateActionCursor(true);
                        RefreshShipView();
                        return;
                    }
                }
                if (SelectedHoldItem->ItemType == aConst::t_MicroModule && Entry != nullptr && Entry->Kind == phkEquipment && reinterpret_cast<aItem::TEquipment*>(Entry->Item)->MicroModuleIndex == 0 && pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->CanInstallOn(reinterpret_cast<aItem::TEquipment*>(Entry->Item)) && static_cast<std::uint8_t>(PlayerHoldShip->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0) {
                    Text = ([&] {
                        pas::WideString plainName = pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->GetPlainName();
                        pas::WideString removeTextTagsW = EC_Str::RemoveTextTagsW(Entry->Item->GetDisplayName());
                        pas::WideString localizedText_4 = aConst::LocalizedText(u"MicroModuls.AddToItem"_wref.get());
                        return aMyFunction::FormatText2(std::move(localizedText_4), u"<color=255,240,100>"_w, u"<ModuleName>"_w, std::move(plainName), u"<ItemName>"_w, std::move(removeTextTagsW));
                    }());
                    if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                        Equipment = reinterpret_cast<aItem::TEquipment*>(Entry->Item);
                        PlayerHoldShip->ScriptItemsAct(aConst::satOnPlayerUseMM, Equipment, SelectedHoldItem, 0);
                        aItem::ApplyMicroModule(pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->MicroModuleIndex - 1, Equipment);
                        pas::free(SelectedHoldItem);
                        SelectedHoldKind = phkEmpty;
                        SelectedHoldItem = nullptr;
                        PlayerHoldShip->RefreshDerivedStats(true);
                        aGalaxy::Galaxy->PrimeIntegrityChecksum1(458);
                        UpdateActionCursor(true);
                        RefreshShipView();
                        return;
                    }
                }
                if (Entry != nullptr && Entry->Item != nullptr && pas::is_one_of<phkEquipment, phkArtefact>(Entry->Kind) && static_cast<std::uint8_t>(PlayerHoldShip->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0) {
                    ActionResult = 0;
                    if (SelectedHoldItem->ScriptItem != nullptr) {
                        ActionResult = reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnAnotherItem, PlayerHoldShip, Entry->Item, nullptr, ActionResult);
                    }
                    if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(SelectedHoldItem) != nullptr) {
                        ActionResult = aScript::RunItemConfigActionCode(SelectedHoldItem, aConst::satOnAnotherItem, PlayerHoldShip, Entry->Item, nullptr, ActionResult);
                    }
                    if (Entry->Item->ScriptItem != nullptr) {
                        ActionResult = reinterpret_cast<aScript::TScriptItem*>(Entry->Item->ScriptItem)->RunActionCode(aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
                    }
                    if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Entry->Item) != nullptr) {
                        ActionResult = aScript::RunItemConfigActionCode(Entry->Item, aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
                    }
                    if (pas::in_set<1, 1, 3, 3>(ActionResult)) {
                        pas::free(SelectedHoldItem);
                        SelectedHoldKind = phkEmpty;
                        SelectedHoldItem = nullptr;
                    }
                    if (pas::in_range(ActionResult, 1, 3)) {
                        PlayerHoldShip->RefreshDerivedStats(true);
                        aGalaxy::Galaxy->PrimeIntegrityChecksum1(449);
                        UpdateActionCursor(true);
                        RefreshShipView();
                        if (ActionResult == 3) {
                            CloseClicked(nullptr);
                        }
                        return;
                    }
                }
            }
            if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                weapon->Target = nullptr;
            }
            pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(SelectedHoldItem));
            pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            if (!Expanded) {
                if (Entry == nullptr) {
                    Count = Index - pas::list_count(PlayerHoldEntries) + 1;
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(J); ) {
                        Entry = pas::make_object<TPlayerHoldUnit>();
                        pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                        Entry->Kind = phkEmpty;
                    }
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, pas::list_count(PlayerHoldEntries) - 1);
                } else if (Entry->Kind != phkEmpty) {
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_insert(PlayerHoldEntries, Index, reinterpret_cast<void*>(Entry));
                    Entry->Kind = phkEmpty;
                }
                Entry->DisplayOrder = fShip2::FindFreePlayerHoldOrder();
                Entry->Kind = phkEquipment;
                Entry->ItemId = SelectedHoldItem->Id;
                Entry->Item = SelectedHoldItem;
                fShip2::RemoveEmptyPlayerHoldSlot(pas::list_indexof(PlayerHoldEntries, reinterpret_cast<void*>(Entry)) + 1);
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
            } else {
                if (Entry != nullptr && Entry->Item != nullptr && Entry->Item->NoDropFlag > 0) {
                    Entry = nullptr;
                }
                Swap = nullptr;
                if (Entry == nullptr) {
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                } else if (Entry->Kind != phkEmpty) {
                    Swap = Entry;
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                }
                Entry->DisplayOrder = Index;
                Entry->Kind = phkEquipment;
                Entry->ItemId = SelectedHoldItem->Id;
                Entry->Item = SelectedHoldItem;
                if (Swap != nullptr) {
                    TakeHoldEntry(Swap, Index, Expanded);
                } else {
                    SelectedHoldKind = phkEmpty;
                    SelectedHoldItem = nullptr;
                }
                fShip2::RestorePlayerHoldDisplayOrder();
            }
            PlayerHoldShip->RefreshDerivedStats(true);
            UpdateActionCursor(true);
            RefreshAfterTransfer();
            return;
        }
        if (SelectedHoldKind == phkArtefact) {
            if (static_cast<std::uint8_t>(RemoteHoldVisible ^ 1) && Entry != nullptr && Entry->Item != nullptr && pas::is_one_of<phkEquipment, phkArtefact>(Entry->Kind) && static_cast<std::uint8_t>(PlayerHoldShip->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0) {
                ActionResult = 0;
                if (SelectedHoldItem->ScriptItem != nullptr) {
                    ActionResult = reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnAnotherItem, PlayerHoldShip, Entry->Item, nullptr, ActionResult);
                }
                ActionResult = aScript::RunItemConfigActionCode(SelectedHoldItem, aConst::satOnAnotherItem, PlayerHoldShip, Entry->Item, nullptr, ActionResult);
                if (Entry->Item->ScriptItem != nullptr) {
                    ActionResult = reinterpret_cast<aScript::TScriptItem*>(Entry->Item->ScriptItem)->RunActionCode(aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
                }
                if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Entry->Item) != nullptr) {
                    ActionResult = aScript::RunItemConfigActionCode(Entry->Item, aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
                }
                if (pas::in_set<1, 1, 3, 3>(ActionResult)) {
                    pas::free(SelectedHoldItem);
                    SelectedHoldKind = phkEmpty;
                    SelectedHoldItem = nullptr;
                }
                if (pas::in_range(ActionResult, 1, 3)) {
                    PlayerHoldShip->RefreshDerivedStats(true);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(460);
                    UpdateActionCursor(true);
                    RefreshShipView();
                    if (ActionResult == 3) {
                        CloseClicked(nullptr);
                    }
                    return;
                }
            }
            pas::list_add(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(SelectedHoldItem));
            pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
            if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(SelectedHoldItem)) {
                pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship))->OwnerShip = aPlayer::GetPlayer();
            }
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            if (!Expanded) {
                if (Entry == nullptr) {
                    Count = Index - pas::list_count(PlayerHoldEntries) + 1;
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(J); ) {
                        Entry = pas::make_object<TPlayerHoldUnit>();
                        pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                        Entry->Kind = phkEmpty;
                    }
                    Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, pas::list_count(PlayerHoldEntries) - 1);
                } else if (Entry->Kind != phkEmpty) {
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_insert(PlayerHoldEntries, Index, reinterpret_cast<void*>(Entry));
                    Entry->Kind = phkEmpty;
                }
                Entry->Kind = phkArtefact;
                Entry->ItemId = SelectedHoldItem->Id;
                Entry->Item = SelectedHoldItem;
                fShip2::RemoveEmptyPlayerHoldSlot(pas::list_indexof(PlayerHoldEntries, reinterpret_cast<void*>(Entry)) + 1);
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
            } else {
                if (Entry != nullptr && Entry->Item != nullptr && Entry->Item->NoDropFlag > 0) {
                    Entry = nullptr;
                }
                Swap = nullptr;
                if (Entry == nullptr) {
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                } else if (Entry->Kind != phkEmpty) {
                    Swap = Entry;
                    Entry = pas::make_object<TPlayerHoldUnit>();
                    pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                }
                Entry->DisplayOrder = Index;
                Entry->Kind = phkArtefact;
                Entry->ItemId = SelectedHoldItem->Id;
                Entry->Item = SelectedHoldItem;
                if (Swap != nullptr) {
                    TakeHoldEntry(Swap, Index, Expanded);
                } else {
                    SelectedHoldKind = phkEmpty;
                    SelectedHoldItem = nullptr;
                }
                fShip2::RestorePlayerHoldDisplayOrder();
            }
            PlayerHoldShip->RefreshDerivedStats(true);
            UpdateActionCursor(true);
            RefreshAfterTransfer();
            return;
        }
        RefreshAfterTransfer();
    }

    void TfShip2::TakeHoldEntry(TPlayerHoldUnit* Entry, std::int32_t Slot, std::uint8_t UsesDisplayOrder) {
        if (Entry->Kind == phkEquipment) {
            SelectedHoldKind = phkEquipment;
            SelectedHoldOrigin = 0;
            SelectedHoldSlot = Slot;
            SelectedHoldUsesDisplayOrder = UsesDisplayOrder;
            SelectedHoldItem = Entry->Item;
            pas::list_delete(PlayerHoldShip->Inventory, pas::list_indexof(PlayerHoldShip->Inventory, reinterpret_cast<void*>(Entry->Item)));
            Entry->Kind = phkEmpty;
            Entry->Item = nullptr;
        } else if (Entry->Kind == phkGoods) {
            SelectedHoldKind = phkGoods;
            SelectedHoldOrigin = 0;
            SelectedHoldSlot = Slot;
            SelectedHoldUsesDisplayOrder = UsesDisplayOrder;
            SelectedGoodsIndex = Entry->GoodsIndex;
            SelectedGoodsQuantity = PlayerHoldShip->CargoGoods[SelectedGoodsIndex].Count;
            SelectedGoodsCost = PlayerHoldShip->CargoGoods[SelectedGoodsIndex].TotalCost;
            PlayerHoldShip->CargoGoods[SelectedGoodsIndex].Count = 0;
            PlayerHoldShip->CargoGoods[SelectedGoodsIndex].TotalCost = 0;
            Entry->Kind = phkEmpty;
            Entry->Item = nullptr;
        } else if (Entry->Kind == phkArtefact) {
            SelectedHoldKind = phkArtefact;
            SelectedHoldOrigin = 0;
            SelectedHoldSlot = Slot;
            SelectedHoldUsesDisplayOrder = UsesDisplayOrder;
            SelectedHoldItem = Entry->Item;
            pas::list_delete(PlayerHoldShip->Artefacts, pas::list_indexof(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(Entry->Item)));
            Entry->Kind = phkEmpty;
            Entry->Item = nullptr;
        } else {
            SelectedHoldKind = phkEmpty;
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(462);
    }

    void TfShip2::HullMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        pas::WideString Text{};
        aItem::THull* OldHull{};
        std::int32_t ActionResult{};
        std::int32_t Quantity{};
        if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && SelectedHoldItem != nullptr && pas::class_cast_if<aItem::THull*>(SelectedHoldItem) != nullptr && PlayerHoldShip->GetHull()->NoDropFlag > 0) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(463);
        if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_MicroModule && PlayerHoldShip->GetHull()->MicroModuleIndex == 0 && pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->CanInstallOn(PlayerHoldShip->GetHull())) {
            Text = ([&] {
                pas::WideString plainName = pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->GetPlainName();
                pas::WideString removeTextTagsW = EC_Str::RemoveTextTagsW(PlayerHoldShip->GetHull()->GetDisplayName());
                pas::WideString localizedText = aConst::LocalizedText(u"MicroModuls.AddToItem"_wref.get());
                return aMyFunction::FormatText2(std::move(localizedText), u"<color=255,240,100>"_w, u"<ModuleName>"_w, std::move(plainName), u"<ItemName>"_w, std::move(removeTextTagsW));
            }());
            if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                PlayerHoldShip->ScriptItemsAct(aConst::satOnPlayerUseMM, PlayerHoldShip->GetHull(), SelectedHoldItem, 0);
                aItem::ApplyMicroModule(pas::checked_cast<aItem::TMicroModule*>(SelectedHoldItem)->MicroModuleIndex - 1, PlayerHoldShip->GetHull());
                pas::free(SelectedHoldItem);
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
                PlayerHoldShip->RefreshDerivedStats(true);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(464);
                UpdateActionCursor(true);
                RefreshShipView();
            }
            return;
        } else if (pas::in_range(SelectedHoldKind, static_cast<std::int32_t>(phkGoods), static_cast<std::int32_t>(phkGoods))) {
            ActionResult = 0;
            if (PlayerHoldShip->GetHull()->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(PlayerHoldShip->GetHull()->ScriptItem)->RunActionCode(aConst::satOnAnotherGoods, PlayerHoldShip, reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsIndex))), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SelectedGoodsQuantity))), 0);
            }
            if (ActionResult != 0) {
                Quantity = pas::abs(ActionResult);
                if (Quantity < SelectedGoodsQuantity) {
                    SelectedGoodsCost -= System::Round(pas::real_divide(Quantity, SelectedGoodsQuantity) * SelectedGoodsCost);
                    SelectedGoodsQuantity -= Quantity;
                } else {
                    SelectedHoldKind = phkEmpty;
                    SelectedGoodsQuantity = 0;
                    SelectedGoodsCost = 0;
                }
            }
            PlayerHoldShip->RefreshDerivedStats(true);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(440);
            UpdateActionCursor(true);
            RefreshShipView();
            if (ActionResult < 0) {
                CloseClicked(nullptr);
            }
            return;
        } else if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && SelectedHoldItem != nullptr) {
            ActionResult = 0;
            if (SelectedHoldItem->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnAnotherItem, PlayerHoldShip, PlayerHoldShip->GetHull(), nullptr, ActionResult);
            }
            if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(SelectedHoldItem) != nullptr) {
                ActionResult = aScript::RunItemConfigActionCode(SelectedHoldItem, aConst::satOnAnotherItem, PlayerHoldShip, PlayerHoldShip->GetHull(), nullptr, ActionResult);
            }
            if (PlayerHoldShip->GetHull()->ScriptItem != nullptr) {
                ActionResult = reinterpret_cast<aScript::TScriptItem*>(PlayerHoldShip->GetHull()->ScriptItem)->RunActionCode(aConst::satOnAnotherItem2, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
            }
            if (pas::in_set<1, 1, 3, 3>(ActionResult)) {
                pas::free(SelectedHoldItem);
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
            }
            if (pas::in_range(ActionResult, 1, 3)) {
                PlayerHoldShip->RefreshDerivedStats(true);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(464);
                UpdateActionCursor(true);
                RefreshShipView();
                if (ActionResult == 3) {
                    CloseClicked(nullptr);
                }
                return;
            }
        } else if (SelectedHoldKind == phkEmpty) {
            if (TfShip2::IsHoldNormalShip() && pas::list_at<aItem::TItem>(PlayerHoldShip->Inventory, 0)->NoDropFlag <= 0) {
                SelectedHoldKind = phkEquipment;
                SelectedHoldOrigin = 0;
                SelectedHoldSlot = 0;
                SelectedHoldUsesDisplayOrder = false;
                SelectedHoldItem = pas::list_at<aItem::TItem>(PlayerHoldShip->Inventory, 0);
                PlayerHoldShip->RefreshDerivedStats(true);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(465);
                UpdateActionCursor(true);
                RefreshShipView();
                GR_Main::SoundManager->PlaySound(u"Sound.SlotGet"_wref.get());
            }
            return;
        }
        if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_Hull && SelectedHoldOrigin == 0) {
            ReturnSelectedHoldEntry();
        } else if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_Hull && SelectedHoldOrigin == 1 && TfShip2::IsHoldNormalShip()) {
            PlayerHoldShip->ScriptItemsAct(aConst::satOnPlayerChangeHull, SelectedHoldItem, PlayerHoldShip->GetHull(), 0);
            OldHull = PlayerHoldShip->GetHull();
            pas::list_insert(PlayerHoldShip->Inventory, 0, reinterpret_cast<void*>(SelectedHoldItem));
            PlayerHoldShip->Hull = pas::checked_cast<aItem::THull*>(SelectedHoldItem);
            PlayerHoldShip->GetHull()->OwnerShip = PlayerHoldShip;
            PlayerHoldShip->GetHull()->AssignedSlotData = 0u;
            PlayerHoldShip->GetHull()->EquippedFlag = 1;
            SelectedHoldKind = phkEmpty;
            SelectedHoldItem = nullptr;
            if (OldHull != nullptr) {
                OldHull->OwnerShip = nullptr;
                pas::list_delete(PlayerHoldShip->Inventory, pas::list_indexof(PlayerHoldShip->Inventory, reinterpret_cast<void*>(OldHull)));
                aPlayer::GetPlayer()->AddItemToPlayerStorage(OldHull, TfShip2::GetLocalStorageOwner(), -1);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(466);
            }
            if (PlayerHoldShip->GetHull()->ScriptItem != nullptr) {
                reinterpret_cast<aScript::TScriptItem*>(PlayerHoldShip->GetHull()->ScriptItem)->RunActionCode(aConst::satOnPlayerChangeHull, PlayerHoldShip, PlayerHoldShip->GetHull(), OldHull, 0);
            }
            PlayerHoldShip->RefreshAssignedItemSlots();
            PlayerHoldShip->RebuildEquipmentCache();
            PlayerHoldShip->RefreshDerivedStats(true);
            if (!PlayerHoldShip->ScriptChameleon) {
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&PlayerHoldShip->Graphic));
                aShip::TShip_RefreshGraphic(PlayerHoldShip);
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(467);
            UpdateActionCursor(true);
            RefreshShipView();
            ShipStateChanged = true;
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    std::uint8_t TfShip2::ConfigureChameleon() {
        std::int32_t Choice{};
        std::int32_t ExpectedCharges{};
        std::int32_t I{};
        aGalaxyStruct::TDominatorSeries Series{};
        std::uint8_t VisualType{};
        aShip::TShip* Ship{};
        // Nested helper; does not access its parent frame.
        auto ChameleonDialogChoiceToSeries = [&](std::int32_t Choice) -> aGalaxyStruct::TDominatorSeries {
            aGalaxyStruct::TDominatorSeries Result = aGalaxyStruct::dsBlazer;
            switch (Choice) {
                case 2: return aGalaxyStruct::dsBlazer;
                case 3: return aGalaxyStruct::dsKeller;
                case 4: return aGalaxyStruct::dsTerron;
                default: return Result;
            }
        };
        std::uint8_t Result = false;
        if (static_cast<std::uint8_t>(PlayerHoldShip->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0 && static_cast<std::uint8_t>(PlayerHoldShip->ScriptChameleon ^ 1) && (aShip::TShip::HasPlayerChameleonCharges() || PlayerHoldShip->ChameleonActive)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(427);
            ReturnSelectedHoldEntry();
            VisualType = PlayerHoldShip->SelectChameleonVisualType();
            if (fChameleon::ShowChameleonDialog(this, PlayerHoldShip->ChameleonCharges[0], PlayerHoldShip->ChameleonCharges[1], PlayerHoldShip->ChameleonCharges[2], VisualType, PlayerHoldShip->ChameleonActive, Choice) == 1) {
                if (Choice == 1) {
                    PlayerHoldShip->ChameleonActive = false;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                        Ship = pas::list_at<aShip::TShip>(PlayerHoldShip->CurrentStar->Ships, I);
                        if (Ship != PlayerHoldShip && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->HasScriptControl() ^ 1) && pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr && Ship->TypeId != aGalaxyStruct::stPirate) {
                            Ship->AssignWeaponTargetsInStar();
                        }
                    }
                } else {
                    Series = ChameleonDialogChoiceToSeries(Choice);
                    ExpectedCharges = std::max<std::int32_t>(0, PlayerHoldShip->ChameleonCharges[Series] - 1);
                    PlayerHoldShip->ChameleonCharges[Series] = ExpectedCharges;
                    PlayerHoldShip->ChameleonSeries = Series;
                    PlayerHoldShip->ChameleonActive = true;
                    SysUtilsImports::Sleep(1u);
                    if (PlayerHoldShip->ChameleonCharges[Series] != ExpectedCharges && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                        GR_Main::CCInterface->SetTamperDetected(true);
                    }
                }
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&PlayerHoldShip->Graphic));
                aShip::TShip_RefreshGraphic(PlayerHoldShip);
                Result = true;
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(468);
        }
        return Result;
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::HoldLeftPressed(GI_MessageLoop::TObjectGI* Sender) {
        --HoldFirstIndex;
        RefreshShipView();
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
        HoldScrollTimer = ScheduleCallbackTimer(500, 50, pas::bind_method<&TfShip2::ScrollHoldTimer>(this), 0);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::HoldLeftReleased(GI_MessageLoop::TObjectGI* Sender) {
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::HoldRightPressed(GI_MessageLoop::TObjectGI* Sender) {
        ++HoldFirstIndex;
        RefreshShipView();
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
        HoldScrollTimer = ScheduleCallbackTimer(500, 50, pas::bind_method<&TfShip2::ScrollHoldTimer>(this), 1);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::HoldRightReleased(GI_MessageLoop::TObjectGI* Sender) {
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
    }

    void TfShip2::ScrollHoldTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (UserData == 0) {
            --HoldFirstIndex;
        } else {
            ++HoldFirstIndex;
        }
        if (HoldFirstIndex <= 0) {
            HoldFirstIndex = 0;
            if (HoldScrollTimer != nullptr) {
                CancelCallbackTimer(HoldScrollTimer);
                HoldScrollTimer = nullptr;
            }
        } else if (HoldFirstIndex + 6 >= pas::list_count(PlayerHoldEntries) + 1) {
            HoldFirstIndex = std::max<std::int32_t>(0, pas::list_count(PlayerHoldEntries) + 1 - 6);
            if (HoldScrollTimer != nullptr) {
                CancelCallbackTimer(HoldScrollTimer);
                HoldScrollTimer = nullptr;
            }
        }
        RefreshShipView();
    }

    void TfShip2::ReturnSelectedHoldEntry() {
        std::int32_t I{};
        std::int32_t Count{};
        TPlayerHoldUnit* Entry{};
        if (SelectedHoldKind != phkEmpty) {
            if (SelectedHoldOrigin == 0) {
                if (SelectedHoldKind == phkGoods) {
                    PlayerHoldShip->CargoGoods[SelectedGoodsIndex].Count = SelectedGoodsQuantity;
                    PlayerHoldShip->CargoGoods[SelectedGoodsIndex].TotalCost = SelectedGoodsCost;
                } else if (SelectedHoldKind == phkEquipment) {
                    if (!(pas::class_cast_if<aItem::THull*>(SelectedHoldItem) != nullptr)) {
                        if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                            weapon->Target = nullptr;
                        }
                        pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(SelectedHoldItem));
                        pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                    }
                } else if (SelectedHoldKind == phkArtefact) {
                    pas::list_add(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(SelectedHoldItem));
                    pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                }
                if (SelectedHoldKind == phkGoods || SelectedHoldKind == phkEquipment || SelectedHoldKind == phkArtefact) {
                    if (SelectedHoldUsesDisplayOrder) {
                        if (fShip2::FindPlayerHoldIndexByOrder(SelectedHoldSlot) < 0) {
                            Entry = pas::make_object<TPlayerHoldUnit>();
                            pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                            Entry->DisplayOrder = SelectedHoldSlot;
                        } else {
                            Entry = pas::make_object<TPlayerHoldUnit>();
                            Entry->DisplayOrder = fShip2::FindFreePlayerHoldOrder();
                            pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                        }
                        Entry->Kind = SelectedHoldKind;
                        if (SelectedHoldKind == phkGoods) {
                            Entry->GoodsIndex = SelectedGoodsIndex;
                        } else {
                            Entry->ItemId = SelectedHoldItem->Id;
                            Entry->Item = SelectedHoldItem;
                        }
                        fShip2::RestorePlayerHoldDisplayOrder();
                    } else if (SelectedHoldSlot >= 0) {
                        if (fShip2::IsPlayerHoldSlotEmpty(SelectedHoldSlot)) {
                            if (pas::list_count(PlayerHoldEntries) > SelectedHoldSlot) {
                                Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, SelectedHoldSlot);
                            } else {
                                Count = SelectedHoldSlot - pas::list_count(PlayerHoldEntries) + 1;
                                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                                    Entry = pas::make_object<TPlayerHoldUnit>();
                                    pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                                    Entry->Kind = phkEmpty;
                                }
                                Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, pas::list_count(PlayerHoldEntries) - 1);
                            }
                        } else {
                            Entry = pas::make_object<TPlayerHoldUnit>();
                            pas::list_add(PlayerHoldEntries, reinterpret_cast<void*>(Entry));
                        }
                        Entry->DisplayOrder = fShip2::FindFreePlayerHoldOrder();
                        Entry->Kind = SelectedHoldKind;
                        if (SelectedHoldKind == phkGoods) {
                            Entry->GoodsIndex = SelectedGoodsIndex;
                        } else {
                            Entry->ItemId = SelectedHoldItem->Id;
                            Entry->Item = SelectedHoldItem;
                        }
                    } else if ((SelectedHoldKind == phkEquipment || SelectedHoldKind == phkArtefact) && PlayerHoldShip->FindEquippedItemInSlot(SelectedHoldItem->ItemType, -SelectedHoldSlot - 1) == nullptr) {
                        pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                        static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
                    }
                }
            } else if (SelectedHoldKind == phkGoods) {
                if (aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(TfShip2::GetLocalStorageOwner(), SelectedHoldSlot) < 0) {
                    aPlayer::GetPlayer()->AddGoodsToPlayerStorage(SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, TfShip2::GetLocalStorageOwner(), SelectedHoldSlot);
                } else {
                    aPlayer::GetPlayer()->AddGoodsToPlayerStorage(SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, TfShip2::GetLocalStorageOwner(), -1);
                }
            } else if (SelectedHoldKind == phkEquipment || SelectedHoldKind == phkArtefact) {
                if (aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(TfShip2::GetLocalStorageOwner(), SelectedHoldSlot) < 0) {
                    aPlayer::GetPlayer()->AddItemToPlayerStorage(SelectedHoldItem, TfShip2::GetLocalStorageOwner(), SelectedHoldSlot);
                } else {
                    aPlayer::GetPlayer()->AddItemToPlayerStorage(SelectedHoldItem, TfShip2::GetLocalStorageOwner(), -1);
                }
            }
            SelectedHoldKind = phkEmpty;
            SelectedHoldItem = nullptr;
            if (!GR_Main::ExitScreenLoop) {
                PlayerHoldShip->RefreshDerivedStats(true);
                if (!IsCursorImageSelected(u"Main"sv)) {
                    SetCursorByName(u"Main"_wref.get());
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(428);
                RefreshShipView();
            } else {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(428);
            }
        }
    }

    void TfShip2::RefreshEquipmentConfigurationButtons() {
        GI_GraphButton::TGraphButtonGI* Button{};
        std::int32_t I{};
        if (aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, 9); cpp_range.next(I); ) {
                Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(FindControlByPath(static_cast<pas::WideString>(pas::concat_ansi({"Compl", SysUtils::IntToStr(I)}))));
                if (Button != nullptr) {
                    Button->SetDisabled(aPlayer::GetPlayer()->SelectedEquipmentConfiguration == I && aPlayer::GetPlayer() == PlayerHoldShip);
                    Button->SetDown(aPlayer::GetPlayer()->HasEquipmentConfiguration(I) && aPlayer::GetPlayer() == PlayerHoldShip);
                }
            }
        }
    }

    void TfShip2::SelectEquipmentConfiguration(std::int32_t Index) {
        GI_GraphButton::TGraphButtonGI* Button{};
        if (aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer() != PlayerHoldShip) {
                Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(FindControlByPath(static_cast<pas::WideString>(pas::concat_ansi({"Compl", SysUtils::IntToStr(Index)}))));
                if (Button != nullptr) {
                    Button->SetDisabled(false);
                    Button->SetDown(false);
                }
            } else if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0 && aPlayer::GetPlayer()->SelectedEquipmentConfiguration != Index) {
                aGalaxy::Galaxy->CheckIntegrityChecksum1(470);
                aPlayer::GetPlayer()->SaveEquipmentConfiguration(aPlayer::GetPlayer()->SelectedEquipmentConfiguration);
                aPlayer::GetPlayer()->SelectedEquipmentConfiguration = Index;
                if (aPlayer::GetPlayer()->HasEquipmentConfiguration(Index) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
                    aPlayer::GetPlayer()->ApplyEquipmentConfiguration(Index);
                    if (Globals::GetInnermostScreenLoop() == this) {
                        RefreshShipView();
                    }
                } else {
                    RefreshEquipmentConfigurationButtons();
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnNonStandartEqChange, nullptr, nullptr, 0);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(471);
            }
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::EquipmentConfigurationClicked(GI_MessageLoop::TObjectGI* Sender) {
        SelectEquipmentConfiguration(EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName)));
    }

    void TfShip2::MainKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        std::int32_t Index{};
        std::uint8_t CanAfterburn{};
        aGalaxy::Galaxy->CheckIntegrityChecksum1(472);
        if (Key == WindowsSdk::VK_SHIFT && SelectedHoldKind == phkEquipment) {
            RefreshActionPanels(SelectedHoldKind, SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, SelectedHoldItem, SelectedHoldOrigin);
        } else if (Key == WindowsSdk::VK_CONTROL) {
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                RefreshShipView();
            }
        }
        if (aPlayer::GetPlayer() == PlayerHoldShip && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0 && Key >= '1' && Key <= '9') {
            Index = Key - '1';
            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                aPlayer::GetPlayer()->SaveEquipmentConfiguration(Index);
                RefreshEquipmentConfigurationButtons();
                GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"\"", static_cast<std::uint8_t>(Key), "\""}));
                    pas::WideString localizedText = aConst::LocalizedText(u"FormShip.SetHotEqu"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Key>"_w, std::move(cpp_arg));
                }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
            } else {
                SelectEquipmentConfiguration(Index);
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(473);
        } else {
            if (static_cast<std::uint8_t>(PlayerHoldShip->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0 && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                if (Key == 'I') {
                    ShipToStorageClicked(nullptr);
                } else if (Key == 'O') {
                    StorageToShipClicked(nullptr);
                }
            }
            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(474);
                return;
            }
            if (aPlayer::GetPlayer() == PlayerHoldShip && Key == 'K' && ConfigureChameleon()) {
                ShipStateChanged = true;
                ReopenRequested = true;
                PlayTransitionSounds = false;
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(475);
                CloseClicked(nullptr);
            }
            if (Key == 'S') {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(476);
                CloseClicked(nullptr);
            } else if (Key == 'B') {
                if (aPlayer::GetPlayer() == PlayerHoldShip && aPlayer::GetPlayer()->GetHull()->CapitalShip > 0 && aPlayer::GetPlayer()->RuinsMode == 0) {
                    if (SelectedHoldKind != phkEmpty) {
                        ReturnSelectedHoldEntry();
                    }
                    aPlayer::GetPlayer()->EnterRuinsMode(0);
                }
            } else if (Key == 'F') {
                CanAfterburn = PlayerHoldShip->GetSlotCount(aConst::sskAfterburner) > 0 && aShip::TShip_IsEquipmentUsable(PlayerHoldShip, PlayerHoldShip->GetEngine()) && PlayerHoldShip->InNormalSpace();
                if (static_cast<std::uint8_t>(PlayerHoldShip->AfterburnerActive ^ 1) && CanAfterburn) {
                    GR_Main::SoundManager->PlaySound(u"Sound.ForsageOn"_wref.get());
                    PlayerHoldShip->AfterburnerActive = true;
                    PlayerHoldShip->RefreshDerivedStats(true);
                    RefreshShipView();
                } else if (PlayerHoldShip->AfterburnerActive) {
                    GR_Main::SoundManager->PlaySound(u"Sound.ForsageOff"_wref.get());
                    PlayerHoldShip->AfterburnerActive = false;
                    PlayerHoldShip->RefreshDerivedStats(true);
                    RefreshShipView();
                }
            } else if (Key == WindowsSdk::VK_ESCAPE || Key == WindowsSdk::VK_RETURN) {
                if (SelectedHoldKind != phkEmpty) {
                    ReturnSelectedHoldEntry();
                } else {
                    GR_Main::AuxRenderBuffer->Clear();
                    GlobalsV::RequestedScreenId = GlobalsV::ShipReturnScreenId;
                    RequestClose(1);
                }
            } else if (Key == WindowsSdk::VK_LEFT) {
                if (HoldFirstIndex > 0) {
                    --HoldFirstIndex;
                    RefreshShipView();
                }
            } else if (Key == WindowsSdk::VK_RIGHT) {
                if (HoldFirstIndex + 6 <= pas::list_count(PlayerHoldEntries)) {
                    ++HoldFirstIndex;
                    RefreshShipView();
                }
            } else if (Key == WindowsSdk::VK_UP) {
                ScrollStorageUp(nullptr);
            } else if (Key == WindowsSdk::VK_DOWN) {
                ScrollStorageDown(nullptr);
            } else if (Key == WindowsSdk::VK_F11) {
                if (!MainPanel->RemoveDismissibleMessages(u"GOODS"_w)) {
                    MainPanel->RemoveDismissibleMessages(pas::WideString());
                }
            } else if (Key == 'R') {
                RewardsMouseDown(RewardsBuffer, 0u, RewardsBuffer->LocalPosition);
            } else if (Key == 'U') {
                LoadRockets(true);
            } else if (Key == 'Y') {
                LoadRockets(false);
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(471);
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::MainKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_SHIFT && SelectedHoldKind == phkEquipment) {
            RefreshActionPanels(SelectedHoldKind, SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, SelectedHoldItem, SelectedHoldOrigin);
        } else if (Key == WindowsSdk::VK_CONTROL) {
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                RefreshShipView();
            }
        }
    }

    void TfShip2::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            if (TfShip2::CanUseLocalStorage() && GetByName(u"SC_Panel"sv)->ContainsPoint(Point)) {
                ScrollStorageUp(nullptr);
            } else if (RemoteHoldMode && GetByName(u"PanelItemRH"sv)->ContainsPoint(Point)) {
                RemoteHoldFirstOrder = std::max<std::int32_t>(0, RemoteHoldFirstOrder - 5);
                RefreshShipView();
            } else if (static_cast<std::uint8_t>(RemoteHoldMode ^ 1) && GetByName(u"PanelAddInfo"sv)->ContainsPoint(Point)) {
                GI_PanelScrollBar::TPanelScrollBarGI* PanelAddInfo = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelAddInfo"sv));
                PanelAddInfo->VerticalScrollBar->SetPosition_2(PanelAddInfo->VerticalScrollBar->Position - PanelAddInfo->VerticalScrollBar->SmallChange * 5);
            } else if (HoldFirstIndex > 0) {
                --HoldFirstIndex;
                RefreshShipView();
            }
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            if (TfShip2::CanUseLocalStorage() && GetByName(u"SC_Panel"sv)->ContainsPoint(Point)) {
                ScrollStorageDown(nullptr);
            } else if (RemoteHoldMode && GetByName(u"PanelItemRH"sv)->ContainsPoint(Point)) {
                RemoteHoldFirstOrder = std::min<std::int32_t>(TfShip2::GetRemoteHoldScrollLimit(), RemoteHoldFirstOrder + 5);
                RefreshShipView();
            } else if (static_cast<std::uint8_t>(RemoteHoldMode ^ 1) && GetByName(u"PanelAddInfo"sv)->ContainsPoint(Point)) {
                GI_PanelScrollBar::TPanelScrollBarGI* PanelAddInfo_2 = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelAddInfo"sv));
                PanelAddInfo_2->VerticalScrollBar->SetPosition_2(PanelAddInfo_2->VerticalScrollBar->Position + PanelAddInfo_2->VerticalScrollBar->SmallChange * 5);
            } else if (HoldFirstIndex + 6 <= pas::list_count(PlayerHoldEntries)) {
                ++HoldFirstIndex;
                RefreshShipView();
            }
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::MainLeftButtonUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_MessageLoop::TObjectGI* Panels{};
        GI_MessageLoop::TObjectGI* Child{};
        if (GlobalsV::ClickAutoCloseForm) {
            if (GetByName(u"PanelDestr"sv)->ContainsPoint(Point)) {
                return;
            }
            if (GetByName(u"LoadRocketsInSlots"sv)->ContainsPoint(Point)) {
                return;
            }
            if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PanelLeft"sv))->HitTestPixel(Point)) {
                return;
            }
            if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RightOpen"sv))->HitTestPixel(Point)) {
                return;
            }
            if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"RHOpen"sv))->HitTestPixel(Point)) {
                return;
            }
            if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Left"sv))->ContainsPoint(Point)) {
                return;
            }
            if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Right"sv))->ContainsPoint(Point)) {
                return;
            }
            if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"UseImage"sv))->HitTestPixel(Point)) {
                return;
            }
            if (pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"GateZone"sv))->ContainsPoint(Point)) {
                return;
            }
            if (pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"UseZone"sv))->ContainsPoint(Point)) {
                return;
            }
            if (GetByName(u"CenterNormalImage"sv)->ContainsPoint(Point)) {
                return;
            }
            if (GetByName(u"CenterDamageImage"sv)->ContainsPoint(Point)) {
                return;
            }
            if (GetByName(u"SC_Panel"sv)->ContainsPoint(Point)) {
                return;
            }
            if (GetByName(u"Forsage"sv)->ContainsPoint(Point)) {
                return;
            }
            if (GetByName(u"PM_PanelMsg"sv)->ContainsPoint(Point)) {
                return;
            }
            if (GetByName(u"PanelLH"sv)->ContainsPoint(Point)) {
                return;
            }
            if (GetByName(u"PanelDS"sv)->ContainsPoint(Point)) {
                return;
            }
            Panels = FindControlByPath(u"TestHitPanels"_wref.get());
            if (Panels != nullptr) {
                Child = Panels->FirstChild;
                while (Child != nullptr) {
                    if (Child->ContainsPoint(Point)) {
                        return;
                    }
                    Child = Child->NextSibling;
                }
            }
            CloseClicked(nullptr);
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::MainRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        aConst::TItemType ItemType{};
        aConst::TItemType OtherType{};
        GI_Zone::TZoneGI* Zone{};
        std::int32_t I{};
        GI_MessageLoop::TObjectGI* Control{};
        GI_GraphButton::TGraphButtonGI* Button{};
        // Nested helper captures selected item types, KeyState, Zone and Self.
        auto EquipSelected = [&]() -> void {
            std::int32_t Slot{};
            std::int32_t Index{};
            aItem::TEquipment* Equipment{};
            if (this->SelectedHoldItem == nullptr) {
                return;
            }
            if (pas::in_range(this->SelectedHoldItem->ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_ArtFastRacks))) {
                Slot = 0;
                ItemType = this->SelectedHoldItem->ItemType;
                if (pas::is_one_of<aConst::t_Artefact, aConst::t_Artefact2>(ItemType) && reinterpret_cast<aItem::TArtefactCustom*>(this->SelectedHoldItem)->SharedUse) {
                    ItemType = reinterpret_cast<aItem::TArtefactCustom*>(this->SelectedHoldItem)->CountsAsItemType;
                }
                if (pas::in_set<aConst::t_Artefact, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtDefToArms1, aConst::t_ArtFastRacks>(ItemType) && (static_cast<std::uint8_t>(PlayerHoldShip->HasEquippedArtefactOfSameUseGroup(this->SelectedHoldItem) ^ 1) || aGalaxy::Galaxy->AreDuplicateArtefactsEnabled())) {
                    Index = 0;
                    while (Index < PlayerHoldShip->GetSlotCount(aConst::sskArtefact)) {
                        Equipment = PlayerHoldShip->FindEquippedItemInSlot(ItemType, Index);
                        if (Equipment == nullptr) {
                            Slot += Index;
                            break;
                        }
                        if (aGalaxy::Galaxy->AreDuplicateArtefactsEnabled()) {
                            ++Index;
                            continue;
                        }
                        OtherType = Equipment->ItemType;
                        if (pas::is_one_of<aConst::t_Artefact, aConst::t_Artefact2>(OtherType) && reinterpret_cast<aItem::TArtefactCustom*>(Equipment)->SharedUse) {
                            OtherType = reinterpret_cast<aItem::TArtefactCustom*>(Equipment)->CountsAsItemType;
                        }
                        if (ItemType == OtherType && static_cast<std::uint8_t>(pas::is_one_of<aConst::t_Artefact, aConst::t_Artefact2>(ItemType) ^ 1)) {
                            Slot += Index;
                            break;
                        }
                        if (ItemType == OtherType && pas::is_one_of<aConst::t_Artefact, aConst::t_Artefact2>(ItemType) && Equipment->ConfigBlockName == reinterpret_cast<aItem::TEquipment*>(this->SelectedHoldItem)->ConfigBlockName) {
                            Slot += Index;
                            break;
                        }
                        ++Index;
                    }
                    if (Index < PlayerHoldShip->GetSlotCount(aConst::sskArtefact)) {
                        ArtefactSlotMouseDown(this->ArtefactSlotZones[Slot], KeyState, Zone->LocalPosition);
                    }
                }
            } else {
                Slot = -1;
                if (pas::in_range(this->SelectedHoldItem->ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
                    Slot = this->SelectedHoldItem->ItemType - 42;
                } else if (pas::in_range(this->SelectedHoldItem->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    Slot = 8;
                }
                if (Slot < 0) {
                    return;
                }
                if (Slot >= 8) {
                    Index = 0;
                    while (Index < PlayerHoldShip->GetSlotCount(aConst::sskWeapon)) {
                        if (PlayerHoldShip->FindEquippedItemInSlot(this->SelectedHoldItem->ItemType, Index) == nullptr) {
                            Slot += Index;
                            break;
                        }
                        ++Index;
                    }
                    if (Index == PlayerHoldShip->GetSlotCount(aConst::sskWeapon)) {
                        return;
                    }
                }
                Zone = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(ShipEquipmentZoneNames[Slot])));
                if (Slot < 13) {
                    EquipmentSlotMouseDown(Zone, KeyState, Zone->LocalPosition);
                } else {
                    ArtefactSlotMouseDown(Zone, KeyState, Zone->LocalPosition);
                }
            }
        };
        if (PlayerHoldShip->InHyperspace || pas::list_count(aScript::QueuedArcadeBattles) > 0) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(427);
        if (SelectedHoldKind != phkEmpty) {
            ReturnSelectedHoldEntry();
            return;
        }
        std::uint8_t Changed = false;
        if (aPlayer::GetPlayer() == PlayerHoldShip && static_cast<std::uint8_t>(Changed ^ 1)) {
            Zone = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"S_Hull_0z"sv));
            if (Zone->ContainsPoint(Point)) {
                Changed = ConfigureChameleon();
            }
        }
        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited || aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->RuinsMode == 0) {
            if (!Changed) {
                for (auto cpp_range = pas::for_to<std::int32_t>(1, 12); cpp_range.next(I); ) {
                    Zone = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(ShipEquipmentZoneNames[I])));
                    if (Zone->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        EquipmentSlotMouseDown(Zone, KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
                                StorageItemMouseUp(Zone, KeyState, Zone->LocalPosition);
                            } else if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                                if (SelectedHoldItem == nullptr || SelectedHoldItem->ScriptItem == nullptr && SelectedHoldItem->NoDropFlag == 0 || SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->CanSell) {
                                    SpecialSlot2Clicked(Zone);
                                } else {
                                    RemoteHoldItemMouseDown(Zone, KeyState, Zone->LocalPosition);
                                }
                            } else {
                                RemoteHoldItemMouseDown(Zone, KeyState, Zone->LocalPosition);
                            }
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, aConst::DefaultHullSlotCounts[aConst::sskArtefact]); cpp_range_2.next(I); ) {
                    Zone = ArtefactSlotZones[I - 1];
                    if (Zone->Parent->Active && Zone->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        ArtefactSlotMouseDown(Zone, KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
                                StorageItemMouseUp(Zone, KeyState, Zone->LocalPosition);
                            } else if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                                if (SelectedHoldItem == nullptr || SelectedHoldItem->ScriptItem == nullptr && SelectedHoldItem->NoDropFlag == 0 || SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->CanSell) {
                                    SpecialSlot2Clicked(Zone);
                                } else {
                                    RemoteHoldItemMouseDown(Zone, KeyState, Zone->LocalPosition);
                                }
                            } else {
                                RemoteHoldItemMouseDown(Zone, KeyState, Zone->LocalPosition);
                            }
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                if (static_cast<std::uint8_t>(Changed ^ 1) && static_cast<std::uint8_t>(RemoteHoldMode ^ 1)) {
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 5); cpp_range_3.next(I); ) {
                        Control = GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "z"}))));
                        if (Control->ContainsPoint(Point)) {
                            RemoteHoldVisible = true;
                            RemoteHoldItemMouseDown(Control, KeyState, Point);
                            if (SelectedHoldKind != phkEmpty) {
                                if (SelectedHoldItem == nullptr || SelectedHoldItem->ScriptItem == nullptr && SelectedHoldItem->NoDropFlag == 0 || SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->CanSell) {
                                    SpecialSlot2Clicked(Control);
                                } else {
                                    RemoteHoldItemMouseDown(Control, KeyState, Point);
                                }
                                Changed = true;
                            }
                            RemoteHoldVisible = false;
                            break;
                        }
                    }
                }
                if (static_cast<std::uint8_t>(Changed ^ 1) && RemoteHoldMode) {
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 54); cpp_range_4.next(I); ) {
                        if (RemoteHoldImages[I]->ContainsPoint(Point)) {
                            RemoteHoldVisible = true;
                            RemoteHoldItemMouseDown(RemoteHoldImages[I], KeyState, Point);
                            if (SelectedHoldKind != phkEmpty) {
                                if (SelectedHoldItem == nullptr || SelectedHoldItem->ScriptItem == nullptr && SelectedHoldItem->NoDropFlag == 0 || SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->CanSell) {
                                    SpecialSlot2Clicked(RemoteHoldImages[I]);
                                } else {
                                    RemoteHoldItemMouseDown(RemoteHoldImages[I], KeyState, Point);
                                }
                                Changed = true;
                            }
                            RemoteHoldVisible = false;
                            break;
                        }
                    }
                }
                if (!Changed) {
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, StorageImageCount - 1); cpp_range_5.next(I); ) {
                        if (StorageImages[I]->ContainsPoint(Point)) {
                            RemoteHoldVisible = true;
                            StorageItemMouseUp(StorageImages[I], KeyState, Point);
                            if (SelectedHoldKind != phkEmpty) {
                                if (SelectedHoldItem == nullptr || SelectedHoldItem->ScriptItem == nullptr && SelectedHoldItem->NoDropFlag == 0 || SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->CanSell) {
                                    SpecialSlot2Clicked(StorageImages[I]);
                                } else {
                                    StorageItemMouseUp(StorageImages[I], KeyState, Point);
                                }
                                Changed = true;
                            }
                            RemoteHoldVisible = false;
                            break;
                        }
                    }
                }
                if (Changed) {
                    ShipStateChanged = true;
                    ReopenRequested = true;
                    PlayTransitionSounds = false;
                    CloseClicked(nullptr);
                }
                return;
            }
            if (static_cast<std::uint8_t>(Changed ^ 1) && RemoteHoldMode) {
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, 54); cpp_range_6.next(I); ) {
                    if (RemoteHoldImages[I]->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        RemoteHoldItemMouseDown(RemoteHoldImages[I], KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
                                StorageItemMouseUp(RemoteHoldImages[0], KeyState, RemoteHoldImages[0]->LocalPosition);
                            } else {
                                EquipSelected();
                                RemoteHoldItemMouseDown(RemoteHoldImages[I], KeyState, Point);
                            }
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
            if (static_cast<std::uint8_t>(Changed ^ 1) && static_cast<std::uint8_t>(RemoteHoldMode ^ 1)) {
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, 5); cpp_range_7.next(I); ) {
                    Control = GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "z"}))));
                    if (Control->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        RemoteHoldItemMouseDown(Control, KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
                                StorageItemMouseUp(StorageImages[0], KeyState, StorageImages[0]->LocalPosition);
                            } else {
                                EquipSelected();
                                RemoteHoldItemMouseDown(Control, KeyState, Point);
                            }
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
            if (!Changed) {
                for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, StorageImageCount - 1); cpp_range_8.next(I); ) {
                    if (StorageImages[I]->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        StorageItemMouseUp(StorageImages[I], KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            Control = GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(0), "z"}))));
                            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
                                RemoteHoldItemMouseDown(Control, KeyState, Control->LocalPosition);
                            } else {
                                EquipSelected();
                                StorageItemMouseUp(StorageImages[I], KeyState, Point);
                            }
                            aPlayer::GetPlayer()->CloseVacantStorageSlot(TfShip2::GetLocalStorageOwner(), I);
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
        } else if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT)) {
            if (static_cast<std::uint8_t>(Changed ^ 1) && RemoteHoldMode) {
                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, 54); cpp_range_9.next(I); ) {
                    if (RemoteHoldImages[I]->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        RemoteHoldItemMouseDown(RemoteHoldImages[I], KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            Zone = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"GateZone"sv));
                            DropSelectedOutside(RemoteHoldImages[0], KeyState, RemoteHoldImages[0]->LocalPosition);
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
            if (static_cast<std::uint8_t>(Changed ^ 1) && static_cast<std::uint8_t>(RemoteHoldMode ^ 1)) {
                for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, 5); cpp_range_10.next(I); ) {
                    Control = GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "z"}))));
                    if (Control->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        RemoteHoldItemMouseDown(Control, KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            Zone = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"GateZone"sv));
                            DropSelectedOutside(Zone, KeyState, Zone->LocalPosition);
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
        } else {
            if (!Changed) {
                for (auto cpp_range_11 = pas::for_to<std::int32_t>(1, 12); cpp_range_11.next(I); ) {
                    Zone = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(pas::view(ShipEquipmentZoneNames[I])));
                    if (Zone->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        EquipmentSlotMouseDown(Zone, KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            RemoteHoldItemMouseDown(Zone, KeyState, Zone->LocalPosition);
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
                for (auto cpp_range_12 = pas::for_to<std::int32_t>(1, aConst::DefaultHullSlotCounts[aConst::sskArtefact]); cpp_range_12.next(I); ) {
                    Zone = ArtefactSlotZones[I - 1];
                    if (Zone->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        ArtefactSlotMouseDown(Zone, KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            RemoteHoldItemMouseDown(Zone, KeyState, Zone->LocalPosition);
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
            if (static_cast<std::uint8_t>(Changed ^ 1) && RemoteHoldMode) {
                for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, 54); cpp_range_13.next(I); ) {
                    if (RemoteHoldImages[I]->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        RemoteHoldItemMouseDown(RemoteHoldImages[I], KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            EquipSelected();
                            RemoteHoldItemMouseDown(RemoteHoldImages[I], KeyState, Point);
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
            if (static_cast<std::uint8_t>(Changed ^ 1) && static_cast<std::uint8_t>(RemoteHoldMode ^ 1)) {
                for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, 5); cpp_range_14.next(I); ) {
                    Control = GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_", SysUtils::IntToStr(I), "z"}))));
                    if (Control->ContainsPoint(Point)) {
                        RemoteHoldVisible = true;
                        RemoteHoldItemMouseDown(Control, KeyState, Point);
                        if (SelectedHoldKind != phkEmpty) {
                            EquipSelected();
                            RemoteHoldItemMouseDown(Control, KeyState, Point);
                            Changed = true;
                        }
                        RemoteHoldVisible = false;
                        break;
                    }
                }
            }
        }
        if (static_cast<std::uint8_t>(Changed ^ 1) && static_cast<std::uint8_t>(RemoteHoldMode ^ 1)) {
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Left"sv));
            if (Button->ContainsPoint(Point)) {
                I = 0;
                while (HoldFirstIndex > 0 && I < 6) {
                    --HoldFirstIndex;
                    ++I;
                }
                RefreshShipView();
                if (I > 0) {
                    GR_Main::SoundManager->PlaySound(Button->ClickSound);
                }
            }
        }
        if (static_cast<std::uint8_t>(Changed ^ 1) && static_cast<std::uint8_t>(RemoteHoldMode ^ 1)) {
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"S_Right"sv));
            if (Button->ContainsPoint(Point)) {
                I = 0;
                while (HoldFirstIndex + 6 <= pas::list_count(PlayerHoldEntries) && I < 6) {
                    ++HoldFirstIndex;
                    ++I;
                }
                RefreshShipView();
                if (I > 0) {
                    GR_Main::SoundManager->PlaySound(Button->ClickSound);
                }
            }
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(473);
        if (Changed) {
            ShipStateChanged = true;
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    std::uint8_t TfShip2::TryDeployTranclucator(aShip::TShip* Ship) {
        std::int32_t I{};
        std::uint8_t Full{};
        std::uint8_t Result = false;
        if (TfShip2::CanUseLocalStorage() && pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr) {
            Full = true;
            Globals::HangarScreen->RefreshDockedShips();
            for (I = 0; I <= 8; ++I) {
                if (Globals::HangarScreen->ShipSlots[I].AnimationState == 0) {
                    Full = false;
                    break;
                }
            }
            if (!Full) {
                pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip = aPlayer::GetPlayer();
                if (aPlayer::GetPlayer()->IsOnPlanet()) {
                    Ship->CurrentPlanet = aPlayer::GetPlayer()->CurrentPlanet;
                    Result = true;
                } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                    Ship->DockedTo = aPlayer::GetPlayer()->DockedTo;
                    Result = true;
                }
                if (Result) {
                    Ship->CurrentStar = PlayerHoldShip->CurrentStar;
                    if (pas::list_indexof(PlayerHoldShip->CurrentStar->Ships, reinterpret_cast<void*>(Ship)) < 0) {
                        pas::list_add(PlayerHoldShip->CurrentStar->Ships, reinterpret_cast<void*>(Ship));
                    }
                    Ship->OrderNone(false);
                    Ship->OrderStateData = 0;
                }
            }
        }
        return Result;
    }

    void TfShip2::DropSelectedOutside(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        aItem::TItem* Item{};
        aItem::TGoods* Goods{};
        double Angle{};
        SE_Weapon::TWeaponSE* Effect{};
        aEFilmEnd::PEFilmEndEntry Entry{};
        std::int32_t Quantity{};
        std::int32_t I{};
        std::int32_t Index{};
        pas::WideString Text{};
        std::int32_t ActionResult{};
        if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::THull*>(SelectedHoldItem) != nullptr) {
            return;
        }
        if (SelectedHoldKind == phkEmpty) {
            return;
        }
        if (SelectedHoldItem != nullptr && SelectedHoldItem->NoDropFlag > 0) {
            ShowNoDropMessage(SelectedHoldItem->NoDropFlag);
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(476);
        if (PreserveSpaceMusic) {
            DropSelectedInArcade();
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(477);
            return;
        }
        if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem) != nullptr && static_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount > 1) {
            Quantity = static_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount;
            if (!RemoteHoldVisible) {
                if (([&] {
                    std::int32_t quantity = Quantity;
                    std::int32_t quantity_2 = Quantity;
                    std::int32_t quantity_3 = Quantity;
                    const pas::WideString& formatText1 = ([&] {
                        pas::WideString lowerCaseWideString = EC_Str::LowerCaseWideString(aItem::GetStackableItemName(SelectedHoldItem));
                        pas::WideString localizedText = aConst::LocalizedText(u"FormShip.ThrowItem"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString));
                    }());
                    const pas::WideString& cpp_arg = pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(SelectedHoldItem), u"s"});
                    GI_MessageLoop::TMessageLoopGI* self = this;
                    return fCount2::ShowCountDialog(self, cpp_arg, formatText1, 0, quantity, quantity_2, 0.0f, quantity_3, 0, Quantity);
                }()) != 1) {
                    return;
                }
            }
            if (Quantity < 1 || reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount < Quantity) {
                return;
            }
            Item = reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->Split(Quantity);
            if (static_cast<std::uint8_t>(PlayerHoldShip->IsOnPlanet() ^ 1) && static_cast<std::uint8_t>(PlayerHoldShip->IsDockedToShip() ^ 1) || aPlayer::GetPlayer()->RuinsMode > 0 && aPlayer::GetPlayer()->RuinsSavedDockedTo == nullptr && aPlayer::GetPlayer()->RuinsSavedPlanet == nullptr) {
                aGalaxy::TStar* cpp_with = PlayerHoldShip->CurrentStar;
                pas::list_add(cpp_with->Items, reinterpret_cast<void*>(Item));
                Angle = pas::real_divide(aMyFunction::SeededRandomIntRange(0, 360, PlayerHoldShip->CurrentStar->GenerationSeed * aGalaxy::Galaxy->CurrentTurn * Item->Id) * SystemImports::Pi, 1.8E+2L);
                Item->Position.X = System::Sin(Angle) * 1.0E+2L + PlayerHoldShip->Position.X;
                Item->Position.Y = PlayerHoldShip->Position.Y - System::Cos(Angle) * 1.0E+2L;
                {
                    SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                    EC_Struct::TPointF position = Item->Position;
                    graphObject->SetPosition(position);
                }
                if (static_cast<long double>(cpp_with->DamageRadius) * cpp_with->DamageRadius > pas::sqr(static_cast<pas::Extended>(Item->Position.X)) + pas::sqr(static_cast<pas::Extended>(Item->Position.Y))) {
                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                    Effect->SetEndpoints(nullptr, Item->GetGraphObject());
                    Effect->SetHit(0, 0, true, true);
                    if (Globals::TrailingFilmEffects == nullptr) {
                        Globals::TrailingFilmEffects = pas::construct_call<aEFilmEnd::TEFilmEnd>(aEFilmEnd::TEFilmEnd_Create);
                    }
                    Entry = Globals::TrailingFilmEffects->AppendEntry();
                    SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject), Effect);
                    {
                        SE_Space::TObjectSE* graphObject_2 = Item->GetGraphObject();
                        pas::Var<SE_Space::TObjectSE*> relatedObject1 = pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1);
                        SE_Space::RetainSpaceObject(relatedObject1, graphObject_2);
                    }
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                    pas::list_delete(cpp_with->Items, pas::list_indexof(cpp_with->Items, reinterpret_cast<void*>(Item)));
                    pas::free(Item);
                }
            } else {
                pas::free(Item);
            }
            if (reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount < 1) {
                pas::free(SelectedHoldItem);
                SelectedHoldItem = nullptr;
                SelectedHoldKind = phkEmpty;
                SetCursorByName(u"Main"_wref.get());
                RefreshShipView();
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Drop"_wref.get());
        } else if (SelectedHoldKind == phkEquipment || SelectedHoldKind == phkArtefact) {
            if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(SelectedHoldItem)) {
                pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship))->OwnerShip = nullptr;
            }
            if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) {
                if (PlayerHoldShip->NeedsEquipmentType(SelectedHoldItem->ItemType)) {
                    {
                        const pas::WideString& formatText1_2 = ([&] {
                            pas::WideString removeTextTagsW = EC_Str::RemoveTextTagsW(SelectedHoldItem->GetDisplayName());
                            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.RuinMoveItemInvalid"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW));
                        }());
                        GI_MessageLoop::TMessageLoopGI* self_2 = this;
                        GI_MessageBox::ShowMessageBoxGI(self_2, formatText1_2, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    }
                    return;
                }
            } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr) {
                if ((SelectedHoldItem->ItemType == aConst::t_FuelTanks || SelectedHoldItem->ItemType == aConst::t_Engine) && PlayerHoldShip->NeedsEquipmentType(SelectedHoldItem->ItemType)) {
                    {
                        const pas::WideString& formatText1_3 = ([&] {
                            pas::WideString removeTextTagsW_2 = EC_Str::RemoveTextTagsW(SelectedHoldItem->GetDisplayName());
                            pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.TrancMoveItemInvalid"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW_2));
                        }());
                        GI_MessageLoop::TMessageLoopGI* self_3 = this;
                        GI_MessageBox::ShowMessageBoxGI(self_3, formatText1_3, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    }
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(478);
                    return;
                }
            } else if (SelectedHoldItem->ItemType == aConst::t_FuelTanks) {
                if (PlayerHoldShip->GetFuelTanks() == nullptr) {
                    if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) {
                        Text = ([&] {
                            pas::WideString removeTextTagsW_3 = EC_Str::RemoveTextTagsW(SelectedHoldItem->GetDisplayName());
                            pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormShip.RuinMoveItemInvalid"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW_3));
                        }());
                    } else {
                        Text = aConst::LocalizedText(u"FormShip.ThrowFuelTanksError"_wref.get());
                    }
                    GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(479);
                    return;
                }
            } else if (SelectedHoldItem->ItemType == aConst::t_Engine) {
                if (PlayerHoldShip->GetEngine() == nullptr) {
                    if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) {
                        Text = ([&] {
                            pas::WideString removeTextTagsW_4 = EC_Str::RemoveTextTagsW(SelectedHoldItem->GetDisplayName());
                            pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormShip.RuinMoveItemInvalid"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW_4));
                        }());
                    } else {
                        Text = aConst::LocalizedText(u"FormShip.ThrowEngineError"_wref.get());
                    }
                    GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(480);
                    return;
                }
            } else if (SelectedHoldItem->NoDropFlag > 0) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(481);
                return;
            }
            if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                weapon->Target = nullptr;
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Drop"_wref.get());
            if (PlayerHoldShip->IsOnPlanet() || PlayerHoldShip->IsDockedToShip() && aPlayer::GetPlayer()->RuinsMode == 0 || aPlayer::GetPlayer()->RuinsMode > 0 && (aPlayer::GetPlayer()->RuinsSavedDockedTo != nullptr || aPlayer::GetPlayer()->RuinsSavedPlanet != nullptr)) {
                pas::free(SelectedHoldItem);
                SelectedHoldItem = nullptr;
            } else {
                aGalaxy::TStar* cpp_with_2 = PlayerHoldShip->CurrentStar;
                pas::list_add(cpp_with_2->Items, reinterpret_cast<void*>(SelectedHoldItem));
                Angle = pas::real_divide(aMyFunction::SeededRandomIntRange(0, 360, PlayerHoldShip->CurrentStar->GenerationSeed * aGalaxy::Galaxy->CurrentTurn * SelectedHoldItem->Id) * SystemImports::Pi, 1.8E+2L);
                SelectedHoldItem->Position.X = System::Sin(Angle) * 1.0E+2L + PlayerHoldShip->Position.X;
                SelectedHoldItem->Position.Y = PlayerHoldShip->Position.Y - System::Cos(Angle) * 1.0E+2L;
                ActionResult = PlayerHoldShip->ScriptItemsAct(aConst::satOnDropItemFixed, SelectedHoldItem, nullptr, 0);
                if (aItem::TEquipmentWithActCode* equipmentWithActCode = pas::class_cast_if<aItem::TEquipmentWithActCode*>(SelectedHoldItem)) {
                    ActionResult = aScript::RunItemConfigActionCode(SelectedHoldItem, aConst::satOnDropItemFixed, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
                }
                if (SelectedHoldItem->ScriptItem != nullptr) {
                    ActionResult = reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->RunActionCode(aConst::satOnDropItemFixed, PlayerHoldShip, SelectedHoldItem, nullptr, ActionResult);
                }
                if (ActionResult == 0) {
                    {
                        SE_Space::TObjectSE* graphObject_3 = SelectedHoldItem->GetGraphObject();
                        EC_Struct::TPointF position_2 = SelectedHoldItem->Position;
                        graphObject_3->SetPosition(position_2);
                    }
                    if (static_cast<long double>(cpp_with_2->DamageRadius) * cpp_with_2->DamageRadius > pas::sqr(static_cast<pas::Extended>(SelectedHoldItem->Position.X)) + pas::sqr(static_cast<pas::Extended>(SelectedHoldItem->Position.Y))) {
                        Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                        Effect->SetEndpoints(nullptr, SelectedHoldItem->GetGraphObject());
                        Effect->SetHit(0, 0, true, true);
                        if (Globals::TrailingFilmEffects == nullptr) {
                            Globals::TrailingFilmEffects = pas::construct_call<aEFilmEnd::TEFilmEnd>(aEFilmEnd::TEFilmEnd_Create);
                        }
                        Entry = Globals::TrailingFilmEffects->AppendEntry();
                        SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject), Effect);
                        {
                            SE_Space::TObjectSE* graphObject_4 = SelectedHoldItem->GetGraphObject();
                            pas::Var<SE_Space::TObjectSE*> relatedObject1_2 = pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1);
                            SE_Space::RetainSpaceObject(relatedObject1_2, graphObject_4);
                        }
                        pas::list_delete(cpp_with_2->Items, pas::list_indexof(cpp_with_2->Items, reinterpret_cast<void*>(SelectedHoldItem)));
                        pas::free(SelectedHoldItem);
                    }
                } else {
                    pas::list_delete(cpp_with_2->Items, pas::list_indexof(cpp_with_2->Items, reinterpret_cast<void*>(SelectedHoldItem)));
                    if (ActionResult < 0) {
                        pas::free(SelectedHoldItem);
                    }
                }
            }
            SelectedHoldKind = phkEmpty;
            SelectedHoldItem = nullptr;
            SetCursorByName(u"Main"_wref.get());
            RefreshShipView();
        } else if (SelectedHoldKind == phkGoods) {
            Quantity = SelectedGoodsQuantity;
            if (SelectedGoodsQuantity > 1) {
                if (!RemoteHoldVisible) {
                    if (([&] {
                        std::int32_t selectedGoodsQuantity = SelectedGoodsQuantity;
                        std::int32_t selectedGoodsQuantity_2 = SelectedGoodsQuantity;
                        std::int32_t quantity_4 = Quantity;
                        const pas::WideString& formatText1_4 = ([&] {
                            pas::WideString lowerCaseWideString_2 = EC_Str::LowerCaseWideString(aItem::GetStackableItemTypeName(static_cast<aConst::TItemType>(SelectedGoodsIndex)));
                            pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.ThrowItem"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString_2));
                        }());
                        const pas::WideString& cpp_arg_2 = pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(SelectedGoodsIndex))});
                        GI_MessageLoop::TMessageLoopGI* self_4 = this;
                        return fCount2::ShowCountDialog(self_4, cpp_arg_2, formatText1_4, 0, selectedGoodsQuantity, selectedGoodsQuantity_2, 0.0f, quantity_4, 0, Quantity);
                    }()) != 1) {
                        return;
                    }
                }
                if (Quantity < 1 || Quantity > SelectedGoodsQuantity) {
                    return;
                }
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Drop"_wref.get());
            if (static_cast<std::uint8_t>(PlayerHoldShip->IsOnPlanet() ^ 1) && static_cast<std::uint8_t>(PlayerHoldShip->IsDockedToShip() ^ 1) || aPlayer::GetPlayer()->RuinsMode > 0 && aPlayer::GetPlayer()->RuinsSavedDockedTo == nullptr && aPlayer::GetPlayer()->RuinsSavedPlanet == nullptr) {
                aGalaxy::TStar* cpp_with_3 = PlayerHoldShip->CurrentStar;
                Goods = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Goods->Init(static_cast<aConst::TItemType>(SelectedGoodsIndex), Quantity);
                Goods->Cost = System::Round(pas::real_divide(Quantity, SelectedGoodsQuantity) * SelectedGoodsCost);
                for (I = 0; I <= 20; ++I) {
                    Angle = pas::real_divide(aMyFunction::SeededRandomIntRange(0, 360, PlayerHoldShip->CurrentStar->GenerationSeed * aGalaxy::Galaxy->CurrentTurn * (SelectedGoodsIndex + I)) * SystemImports::Pi, 1.8E+2L);
                    Goods->Position.X = System::Sin(Angle) * 1.0E+2L + PlayerHoldShip->Position.X;
                    Goods->Position.Y = PlayerHoldShip->Position.Y - System::Cos(Angle) * 1.0E+2L;
                    Index = 0;
                    while (pas::list_count(cpp_with_3->Items) > Index) {
                        Item = pas::list_at<aItem::TItem>(cpp_with_3->Items, Index);
                        if (aMyFunction::PointDistanceSquared(Item->Position, Goods->Position) < 1.0E+2L) {
                            break;
                        }
                        ++Index;
                    }
                    if (pas::list_count(cpp_with_3->Items) <= Index) {
                        break;
                    }
                }
                pas::list_add(cpp_with_3->Items, reinterpret_cast<void*>(Goods));
                {
                    SE_Space::TObjectSE* graphObject_5 = Goods->GetGraphObject();
                    EC_Struct::TPointF position_3 = Goods->Position;
                    graphObject_5->SetPosition(position_3);
                }
            }
            SelectedGoodsCost -= System::Round(pas::real_divide(Quantity, SelectedGoodsQuantity) * SelectedGoodsCost);
            SelectedGoodsQuantity -= Quantity;
            if (SelectedGoodsQuantity < 1) {
                SelectedHoldKind = phkEmpty;
                SetCursorByName(u"Main"_wref.get());
                RefreshShipView();
            }
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(482);
        fShip2::RemoveEmptyPlayerHoldSlots();
        if (!RemoteHoldVisible) {
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    // Transfers the selected cargo to arcade space, or destroys it in arcade view mode.
    void TfShip2::DropSelectedInArcade() {
        aItem::TCountableItem* Item{};
        aItem::TGoods* Goods{};
        std::int32_t Quantity{};
        if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && SelectedHoldItem != nullptr && SelectedHoldItem->NoDropFlag > 0) {
            ShowNoDropMessage(SelectedHoldItem->NoDropFlag);
            return;
        }
        if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem) != nullptr && static_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount > 1) {
            Quantity = static_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount;
            if (([&] {
                std::int32_t quantity = Quantity;
                std::int32_t quantity_2 = Quantity;
                std::int32_t quantity_3 = Quantity;
                const pas::WideString& formatText1 = ([&] {
                    pas::WideString lowerCaseWideString = EC_Str::LowerCaseWideString(aItem::GetStackableItemName(SelectedHoldItem));
                    pas::WideString localizedText = aConst::LocalizedText(u"FormShip.ThrowItem"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString));
                }());
                const pas::WideString& cpp_arg = pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(SelectedHoldItem), u"s"});
                GI_MessageLoop::TMessageLoopGI* self = this;
                return fCount2::ShowCountDialog(self, cpp_arg, formatText1, 0, quantity, quantity_2, 0.0f, quantity_3, 0, Quantity);
            }()) != 1 || Quantity < 1 || reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount < Quantity) {
                return;
            }
            Item = reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->Split(Quantity);
            if (ab_Global::ArcadeViewMode == 0) {
                ab_Item::ab_Item_Drop(ab_Ship::PlayerArcadeShip, Item, 100, 150);
            } else {
                pas::free(Item);
            }
            if (reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount < 1) {
                pas::free(SelectedHoldItem);
                SelectedHoldItem = nullptr;
                SelectedHoldKind = phkEmpty;
                SetCursorByName(u"Main"_wref.get());
                RefreshShipView();
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Drop"_wref.get());
        } else if (SelectedHoldKind == phkEquipment || SelectedHoldKind == phkArtefact) {
            if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(SelectedHoldItem)) {
                pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship))->OwnerShip = nullptr;
            }
            if (SelectedHoldItem->ItemType == aConst::t_FuelTanks) {
                if (PlayerHoldShip->GetFuelTanks() == nullptr) {
                    {
                        const pas::WideString& localizedText_2 = aConst::LocalizedText(u"FormShip.ThrowFuelTanksError"_wref.get());
                        GI_MessageLoop::TMessageLoopGI* self_2 = this;
                        GI_MessageBox::ShowMessageBoxGI(self_2, localizedText_2, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    }
                    return;
                }
            } else if (SelectedHoldItem->ItemType == aConst::t_Engine && PlayerHoldShip->GetEngine() == nullptr) {
                {
                    const pas::WideString& localizedText_3 = aConst::LocalizedText(u"FormShip.ThrowEngineError"_wref.get());
                    GI_MessageLoop::TMessageLoopGI* self_3 = this;
                    GI_MessageBox::ShowMessageBoxGI(self_3, localizedText_3, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                }
                return;
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Drop"_wref.get());
            if (ab_Global::ArcadeViewMode != 0) {
                pas::free(SelectedHoldItem);
                SelectedHoldItem = nullptr;
            } else {
                ab_Item::ab_Item_Drop(ab_Ship::PlayerArcadeShip, SelectedHoldItem, 100, 150);
            }
            SelectedHoldKind = phkEmpty;
            SetCursorByName(u"Main"_wref.get());
            RefreshShipView();
        } else if (SelectedHoldKind == phkGoods) {
            Quantity = SelectedGoodsQuantity;
            if (SelectedGoodsQuantity > 1) {
                if (([&] {
                    std::int32_t selectedGoodsQuantity = SelectedGoodsQuantity;
                    std::int32_t selectedGoodsQuantity_2 = SelectedGoodsQuantity;
                    std::int32_t quantity_4 = Quantity;
                    const pas::WideString& formatText1_2 = ([&] {
                        pas::WideString lowerCaseWideString_2 = EC_Str::LowerCaseWideString(aItem::GetStackableItemTypeName(static_cast<aConst::TItemType>(SelectedGoodsIndex)));
                        pas::WideString localizedText_4 = aConst::LocalizedText(u"FormShip.ThrowItem"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_4), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString_2));
                    }());
                    const pas::WideString& cpp_arg_2 = pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(SelectedGoodsIndex))});
                    GI_MessageLoop::TMessageLoopGI* self_4 = this;
                    return fCount2::ShowCountDialog(self_4, cpp_arg_2, formatText1_2, 0, selectedGoodsQuantity, selectedGoodsQuantity_2, 0.0f, quantity_4, 0, Quantity);
                }()) != 1 || Quantity < 1 || Quantity > SelectedGoodsQuantity) {
                    return;
                }
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Drop"_wref.get());
            if (ab_Global::ArcadeViewMode == 0) {
                Goods = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Goods->Init(static_cast<aConst::TItemType>(SelectedGoodsIndex), Quantity);
                Goods->Cost = System::Round(pas::real_divide(Quantity, SelectedGoodsQuantity) * SelectedGoodsCost);
                ab_Item::ab_Item_Drop(ab_Ship::PlayerArcadeShip, Goods, 100, 150);
            }
            SelectedGoodsCost -= System::Round(pas::real_divide(Quantity, SelectedGoodsQuantity) * SelectedGoodsCost);
            SelectedGoodsQuantity -= Quantity;
            if (SelectedGoodsQuantity < 1) {
                SelectedHoldKind = phkEmpty;
                SetCursorByName(u"Main"_wref.get());
                RefreshShipView();
            }
        }
        GI_Main::BreakUiMessage();
    }

    void TfShip2::SpecialSlot1Clicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        std::int32_t Nodes{};
        pas::WideString Text{};
        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), u"<color=255,240,100>"_w);
                }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            } else {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                    pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), u"<color=255,240,100>"_w);
                }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(493);
        if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->NeedsRepair()) {
            if (!aShip::TShip_CanRepairEquipmentTech(aPlayer::GetPlayer(), static_cast<aItem::TEquipment*>(SelectedHoldItem))) {
                ShipStateChanged = true;
                GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedText(u"FormShip.TooAdvancedForRepair"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
                RefreshShipView();
            } else if (aPlayer::GetPlayer()->CanRepairArtefactsAtLocation() || !(pas::class_cast_if<aItem::TArtefact*>(SelectedHoldItem) != nullptr) && (!(pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem) != nullptr) || static_cast<aItem::TWeapon*>(SelectedHoldItem)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair)) {
                if (aItem::TEquipment_CalculateRepairCost(static_cast<aItem::TEquipment*>(SelectedHoldItem)) > aPlayer::GetPlayer()->Money) {
                    GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                    StartMoneyWarning();
                    return;
                }
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem); weapon != nullptr && weapon->GetWeaponInfo()->Availability == aGalaxyStruct::waNotSoldAndNodeRepair) {
                    Nodes = System::Round(aItem::TEquipment_CalculateRepairCost(static_cast<aItem::TEquipment*>(weapon)) * 0.0025L);
                    if (Nodes == 0) {
                        Nodes = 1;
                    }
                    if (aPlayer::GetPlayer()->GetAvailableNodeCount(PlayerHoldShip) < Nodes) {
                        GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                        GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                            pas::WideString intToStr = pas::wide_int_to_str(Nodes - aPlayer::GetPlayer()->GetAvailableNodeCount(PlayerHoldShip));
                            pas::WideString localizedText = aConst::LocalizedText(u"FormShip.RepairMsgNeedNode"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<NeedNode>"_w, std::move(intToStr));
                        }()), GI_MessageBox::mbgCancel, 0, 0, 0);
                        return;
                    }
                    if (GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                        pas::WideString intToStr_2 = pas::wide_int_to_str(Nodes);
                        pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.RepairMsgEnoughNode"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<NeedNode>"_w, std::move(intToStr_2));
                    }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
                        return;
                    }
                    aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), Nodes, PlayerHoldShip);
                }
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - aItem::TEquipment_CalculateRepairCost(pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)));
                pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->Repair();
                ShipStateChanged = true;
                GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
                if (SelectedHoldKind == phkEquipment) {
                    I = 0;
                    while (I < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                        if (PlayerHoldShip->FindEquippedItemInSlot(SelectedHoldItem->ItemType, I) == nullptr) {
                            break;
                        }
                        ++I;
                    }
                    if (I < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                        if (aItem::TWeapon* weapon_2 = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                            weapon_2->Target = nullptr;
                        }
                        pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(SelectedHoldItem));
                        pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | I;
                        static_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                        static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
                        SelectedHoldKind = phkEmpty;
                        SelectedHoldItem = nullptr;
                    }
                } else if (SelectedHoldKind == phkArtefact && (static_cast<std::uint8_t>(PlayerHoldShip->HasEquippedArtefactOfSameUseGroup(SelectedHoldItem) ^ 1) || aGalaxy::Galaxy->AreDuplicateArtefactsEnabled())) {
                    I = 0;
                    while (I < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                        if (PlayerHoldShip->FindEquippedItemInSlot(SelectedHoldItem->ItemType, I) == nullptr) {
                            break;
                        }
                        ++I;
                    }
                    if (I < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                        pas::list_add(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(SelectedHoldItem));
                        if (([&] {
                            std::int32_t cpp_left = PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType);
                            return cpp_left <= static_cast<std::int32_t>(pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData);
                        }()) || PlayerHoldShip->FindEquippedItemInSlot(SelectedHoldItem->ItemType, static_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData) != nullptr) {
                            static_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = I;
                        }
                        static_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                        static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
                        SelectedHoldKind = phkEmpty;
                        SelectedHoldItem = nullptr;
                    }
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(494);
                RefreshShipView();
            } else {
                ShipStateChanged = true;
                Text = pas::WideString();
                if (PlayerHoldShip->DockedTo != nullptr) {
                    if (PlayerHoldShip->DockedTo->TypeNameOverrideKey != u"") {
                        Text = aConst::LocalizedText(pas::concat_wide({u"FormShip.NotLicenseForRepair", PlayerHoldShip->DockedTo->TypeNameOverrideKey}));
                    }
                    if (Text == u"") {
                        Text = aConst::LocalizedText(pas::concat_wide({u"FormShip.NotLicenseForRepair", aConst::ShipTypeNames[PlayerHoldShip->DockedTo->TypeId].Name}));
                    }
                } else if (PlayerHoldShip->CurrentPlanet != nullptr) {
                    Text = aConst::LocalizedText(pas::concat_wide({u"FormShip.NotLicenseForRepair", aConst::OwnerInfo[PlayerHoldShip->CurrentPlanet->OwnerId].InternalName}));
                }
                if (Text == u"") {
                    Text = aConst::LocalizedText(u"FormShip.NotLicenseForRepair"_wref.get());
                }
                GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
                RefreshShipView();
            }
        }
        fShip2::RemoveEmptyPlayerHoldSlots();
        ReopenRequested = true;
        PlayTransitionSounds = false;
        CloseClicked(nullptr);
    }

    void TfShip2::SpecialSlot2Clicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Count{};
        std::int32_t Price{};
        aItem::TItem* Item{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && SelectedHoldItem != nullptr && SelectedHoldItem->NoDropFlag > 0) {
            ShowNoDropMessage(SelectedHoldItem->NoDropFlag);
            return;
        }
        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                const pas::WideString& replaceColoredToken = ([&] {
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), u"<color=255,240,100>"_w);
                }());
                GI_MessageLoop::TMessageLoopGI* self = this;
                GI_MessageBox::ShowMessageBoxGI(self, replaceColoredToken, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            } else {
                const pas::WideString& replaceColoredToken_2 = ([&] {
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                    pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), u"<color=255,240,100>"_w);
                }());
                GI_MessageLoop::TMessageLoopGI* self_2 = this;
                GI_MessageBox::ShowMessageBoxGI(self_2, replaceColoredToken_2, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
            return;
        }
        if (SelectedHoldKind == phkEmpty) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(495);
        if (SelectedHoldKind == phkEquipment || SelectedHoldKind == phkArtefact) {
            if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) {
                if (PlayerHoldShip->NeedsEquipmentType(SelectedHoldItem->ItemType)) {
                    {
                        const pas::WideString& formatText1 = ([&] {
                            pas::WideString removeTextTagsW = EC_Str::RemoveTextTagsW(SelectedHoldItem->GetDisplayName());
                            pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormShip.RuinMoveItemInvalid"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW));
                        }());
                        GI_MessageLoop::TMessageLoopGI* self_3 = this;
                        GI_MessageBox::ShowMessageBoxGI(self_3, formatText1, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    }
                    return;
                }
            } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr && (SelectedHoldItem->ItemType == aConst::t_FuelTanks || SelectedHoldItem->ItemType == aConst::t_Engine) && PlayerHoldShip->NeedsEquipmentType(SelectedHoldItem->ItemType)) {
                {
                    const pas::WideString& formatText1_2 = ([&] {
                        pas::WideString removeTextTagsW_2 = EC_Str::RemoveTextTagsW(SelectedHoldItem->GetDisplayName());
                        pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormShip.TrancMoveItemInvalid"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW_2));
                    }());
                    GI_MessageLoop::TMessageLoopGI* self_4 = this;
                    GI_MessageBox::ShowMessageBoxGI(self_4, formatText1_2, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                }
                return;
            }
            if (pas::class_cast_if<aItem::TArtefactTranclucator*>(SelectedHoldItem) != nullptr && ([&] {
                const pas::WideString& formatText1_3 = ([&] {
                    pas::WideString displayName = SelectedHoldItem->GetDisplayName();
                    pas::WideString localizedColorText_5 = aConst::LocalizedColorText(u"FormShip.SellItemQuestion"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText_5), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(displayName));
                }());
                GI_MessageLoop::TMessageLoopGI* self_5 = this;
                return GI_MessageBox::ShowMessageBoxGI(self_5, formatText1_3, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0);
            }()) == 2) {
                return;
            }
            if (aItem::TCountableItem* countableItem = pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem); countableItem != nullptr && countableItem->StackCount > 1) {
                Count = countableItem->StackCount;
                if (([&] {
                    std::int32_t stackCount = pas::checked_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount;
                    std::int32_t stackCount_2 = pas::checked_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount;
                    pas::Extended cpp_left = SelectedHoldItem->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false));
                    float round = System::Round(pas::real_divide(cpp_left, pas::checked_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount));
                    std::int32_t count = Count;
                    std::int32_t calculateResaleValue = SelectedHoldItem->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false));
                    const pas::WideString& formatText1_4 = ([&] {
                        pas::WideString lowerCaseWideString = EC_Str::LowerCaseWideString(aItem::GetStackableItemName(SelectedHoldItem));
                        pas::WideString localizedText = aConst::LocalizedText(u"FormShip.SellItem"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString));
                    }());
                    const pas::WideString& cpp_arg = pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(SelectedHoldItem), u"s"});
                    GI_MessageLoop::TMessageLoopGI* self_6 = this;
                    return fCount2::ShowCountDialog(self_6, cpp_arg, formatText1_4, 0, stackCount, stackCount_2, round, count, calculateResaleValue, Count);
                }()) != 1 || Count < 1 || Count > pas::checked_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount) {
                    return;
                }
                Item = pas::checked_cast<aItem::TCountableItem*>(SelectedHoldItem)->Split(Count);
                Price = Item->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false));
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Price);
                pas::free(Item);
            } else {
                Price = SelectedHoldItem->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false));
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Price);
                if (aItem::TCountableItem* countableItem_2 = pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem)) {
                    countableItem_2->StackCount = 0;
                }
            }
            if (Price != 0) {
                GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            }
            if (!(pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem) != nullptr)) {
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerSellsEquipment"_w, nullptr);
                Event->AddData(SelectedHoldItem->ItemType);
                Event->AddData(SelectedHoldItem->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false)));
                Event->AddData(SelectedHoldItem->Weight);
                Event->AddData(SelectedHoldItem->Id);
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
                Event->AddTextData(SelectedHoldItem->GetDisplayName());
                Event->AddTextData(SelectedHoldItem->GetCategoryConfigName());
            }
            ShipStateChanged = true;
            if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                weapon->Target = nullptr;
            }
            if (SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_Hull && pas::checked_cast<aItem::THull*>(SelectedHoldItem)->HullType == aGalaxyStruct::htSpecial) {
                pas::free(SelectedHoldItem);
                SelectedHoldItem = nullptr;
            } else if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(SelectedHoldItem->OwnerId) && SelectedHoldKind == phkEquipment && pas::in_range(SelectedHoldItem->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && (!(pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem) != nullptr) || static_cast<aItem::TWeapon*>(SelectedHoldItem)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair)) {
                fEquipmentShop::RestoreTemporaryShopStock();
                if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                    pas::list_add(aPlayer::GetPlayer()->CurrentPlanet->EquipmentShop, reinterpret_cast<void*>(SelectedHoldItem));
                } else {
                    pas::list_add(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->EquipmentShop, reinterpret_cast<void*>(SelectedHoldItem));
                }
                fEquipmentShop::BuildTemporaryShopSlotGrid();
            } else if (aItem::TCountableItem* countableItem_3 = pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem); !(countableItem_3 != nullptr) || countableItem_3->StackCount == 0) {
                pas::free(SelectedHoldItem);
                SelectedHoldItem = nullptr;
            }
            if (SelectedHoldItem == nullptr || !(pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem) != nullptr) || static_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount == 0) {
                SelectedHoldItem = nullptr;
                SelectedHoldKind = phkEmpty;
                SetCursorByName(u"Main"_wref.get());
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(496);
            RefreshShipView();
        } else if (SelectedHoldKind == phkGoods) {
            if (aPlayer::GetPlayer()->IsCargoGoodIllegalOnCurrentPlanet(SelectedGoodsIndex) && ([&] {
                const pas::WideString& paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormGS.NotPermitGoods"_wref.get());
                GI_MessageLoop::TMessageLoopGI* self_7 = this;
                return GI_MessageBox::ShowMessageBoxGI(self_7, paramByPathOrMarker, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0);
            }()) != GI_MessageBox::mbgResultOK) {
                return;
            }
            Count = SelectedGoodsQuantity;
            if (SelectedGoodsQuantity > 1) {
                if (([&] {
                    std::int32_t selectedGoodsQuantity = SelectedGoodsQuantity;
                    std::int32_t selectedGoodsQuantity_2 = SelectedGoodsQuantity;
                    float shopGoodsSellPrice = aPlayer::GetPlayer()->ShopGoodsSellPrice(SelectedGoodsIndex, nullptr);
                    std::int32_t count_2 = Count;
                    const pas::WideString& formatText1_5 = ([&] {
                        pas::WideString lowerCaseWideString_2 = EC_Str::LowerCaseWideString(aConst::GoodsMarket[SelectedGoodsIndex].DisplayName);
                        pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.SellItem"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString_2));
                    }());
                    const pas::WideString& cpp_arg_2 = pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(SelectedGoodsIndex))});
                    GI_MessageLoop::TMessageLoopGI* self_8 = this;
                    return fCount2::ShowCountDialog(self_8, cpp_arg_2, formatText1_5, 0, selectedGoodsQuantity, selectedGoodsQuantity_2, shopGoodsSellPrice, count_2, 1000000000, Count);
                }()) != 1 || Count < 1 || Count > SelectedGoodsQuantity) {
                    return;
                }
            }
            ShipStateChanged = true;
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            aPlayer::GetPlayer()->CargoGoods[SelectedGoodsIndex].Count += SelectedGoodsQuantity;
            aPlayer::GetPlayer()->CargoGoods[SelectedGoodsIndex].TotalCost += SelectedGoodsCost;
            aPlayer::GetPlayer()->SellGoodsToLocation(SelectedGoodsIndex, Count);
            SelectedHoldKind = phkEmpty;
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(497);
            SetCursorByName(u"Main"_wref.get());
            RefreshShipView();
        }
        fShip2::RemoveEmptyPlayerHoldSlots();
        ReopenRequested = true;
        PlayTransitionSounds = false;
        CloseClicked(nullptr);
    }

    void TfShip2::AdvanceScriptVideo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::uint32_t cpp_left = MMSystem::timeGetTime();
        double Progress = pas::real_divide(cpp_left - ScriptVideoStartedAt, 3.0E+4L);
        if (Progress > 1.0L) {
            Progress = 1.0;
        }
        GI_XviD::TxvidGI* Video = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"sv));
        Video->SetFramePosition(System::Round(749.0L * Progress));
        if (Progress >= 1.0L) {
            StopScriptVideo();
        }
    }

    // Returns whether a video timer was active.
    std::uint8_t TfShip2::StopScriptVideo() {
        std::uint8_t Result = ScriptVideoTimer != nullptr;
        if (GlobalsV::MusicEnabled && Result) {
            GR_Main::MusicManager->StopImmediately();
            while (GR_Main::MusicManager->IsPlaying()) {
                SysUtilsImports::Sleep(1u);
            }
        }
        if (ScriptVideoTimer != nullptr) {
            CancelCallbackTimer(ScriptVideoTimer);
            ScriptVideoTimer = nullptr;
        }
        GI_XviD::TxvidGI* Video = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"sv));
        Video->ImageClose();
        Video->SetActive(false);
        InvalidateViewport();
        ShipLoopSound->SetVolume(1.0f);
        return Result;
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::UseMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        pas::WideString Text{};
        aItem::TItem* Item{};
        std::int32_t ActionResult{};
        if (UsePanelSlideTimer != nullptr) {
            return;
        }
        if (aPlayer::GetPlayer() == PlayerHoldShip && pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->OnUseText != u"") {
            Item = SelectedHoldItem;
            aGalaxy::Galaxy->CheckIntegrityChecksum1(521);
            aGalaxy::Galaxy->CheckIntegrityChecksum2(522);
            ReturnSelectedHoldEntry();
            ActionResult = aScript::RunItemUseCode(Item, PlayerHoldShip);
            if (static_cast<std::uint8_t>(aScript::HasPendingScriptRequests() ^ 1) && ExitCode == 0) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(523);
                aGalaxy::Galaxy->PrimeIntegrityChecksum2(524);
                SetCursorByName(u"Main"_wref.get());
                if (!GetByName(u"Film"sv)->Active) {
                    RefreshShipView();
                    if (ActionResult != 1) {
                        ShipStateChanged = true;
                        ReopenRequested = true;
                        PlayTransitionSounds = false;
                    }
                    CloseClicked(nullptr);
                }
            }
        } else if (aPlayer::GetPlayer() == PlayerHoldShip && SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_UselessItem && pas::checked_cast<aItem::TUselessItem*>(SelectedHoldItem)->GetOnUseCodeText() != u"") {
            Item = SelectedHoldItem;
            aGalaxy::Galaxy->CheckIntegrityChecksum1(521);
            aGalaxy::Galaxy->CheckIntegrityChecksum2(522);
            ReturnSelectedHoldEntry();
            ActionResult = aScript::RunItemUseCode(Item, PlayerHoldShip);
            if (static_cast<std::uint8_t>(aScript::HasPendingScriptRequests() ^ 1) && ExitCode == 0) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(523);
                aGalaxy::Galaxy->PrimeIntegrityChecksum2(524);
                SetCursorByName(u"Main"_wref.get());
                RefreshShipView();
                if (!GetByName(u"Film"sv)->Active) {
                    if (ActionResult != 1) {
                        ShipStateChanged = true;
                        ReopenRequested = true;
                        PlayTransitionSounds = false;
                    }
                    CloseClicked(nullptr);
                }
            }
        } else {
            if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TTreasureMap*>(SelectedHoldItem) != nullptr) {
                GR_Main::SoundManager->PlaySound(u"Sound.UseATranc"_wref.get());
                if (GR_Main::GiResourceVariant() == 1) {
                    Text = pas::checked_cast<aItem::TTreasureMap*>(SelectedHoldItem)->PreviewTablePage1;
                } else {
                    Text = pas::checked_cast<aItem::TTreasureMap*>(SelectedHoldItem)->PreviewTablePage2;
                }
                aGalaxy::Galaxy->CheckIntegrityChecksum1(520);
                Globals::AddOrUpdatePlayerBubble(7, aGalaxy::Galaxy->CurrentTurn, Text, pas::checked_cast<aItem::TTreasureMap*>(SelectedHoldItem)->GetTargetPlanetName());
                MainPanel->RebuildMessageButtons(false);
                ReturnSelectedHoldEntry();
                ReopenRequested = true;
                PlayTransitionSounds = false;
                CloseClicked(nullptr);
            }
            if (aPlayer::GetPlayer() == PlayerHoldShip && SelectedHoldKind == phkArtefact && pas::class_cast_if<aItem::TArtefact*>(SelectedHoldItem) != nullptr && static_cast<aItem::TArtefact*>(SelectedHoldItem)->GetOnUseCodeText() != u"") {
                Item = SelectedHoldItem;
                aGalaxy::Galaxy->CheckIntegrityChecksum1(521);
                aGalaxy::Galaxy->CheckIntegrityChecksum2(522);
                ReturnSelectedHoldEntry();
                ActionResult = aScript::RunItemUseCode(Item, PlayerHoldShip);
                if (static_cast<std::uint8_t>(aScript::HasPendingScriptRequests() ^ 1) && ExitCode == 0) {
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(523);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum2(524);
                    if (!GetByName(u"Film"sv)->Active) {
                        SetCursorByName(u"Main"_wref.get());
                        RefreshShipView();
                        if (ActionResult != 1) {
                            ShipStateChanged = true;
                            ReopenRequested = true;
                            PlayTransitionSounds = false;
                        }
                        CloseClicked(nullptr);
                    }
                }
            } else if (SelectedHoldKind == phkArtefact) {
                fShip2::RemoveEmptyPlayerHoldSlots();
                ReopenRequested = true;
                PlayTransitionSounds = false;
                CloseClicked(nullptr);
            }
        }
    }

    void TfShip2::SpecialSlot3Clicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Limit{};
        std::int32_t Cost{};
        std::int32_t Affordable{};
        std::int32_t Amount{};
        float UnitPrice{};
        aItem::TCistern* Cistern{};
        aItem::TFuelTanks* FuelTanks{};
        aItem::TWeapon* Weapon{};
        pas::WideString Text{};
        aGalaxy::Galaxy->CheckIntegrityChecksum1(498);
        if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TCistern*>(SelectedHoldItem) != nullptr && ([&] {
            std::int32_t cpp_left = static_cast<aItem::TCistern*>(SelectedHoldItem)->Fuel;
            return cpp_left < static_cast<aItem::TCistern*>(SelectedHoldItem)->Capacity;
        }())) {
            Cistern = static_cast<aItem::TCistern*>(SelectedHoldItem);
            Limit = Cistern->Capacity - Cistern->Fuel;
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                UnitPrice = aShip::CalculateFuelCost(1, aPlayer::GetPlayer()->CurrentPlanet->OwnerId);
            } else {
                UnitPrice = aShip::CalculateFuelCost(1, aGalaxyStruct::oiUninhabited);
            }
            Affordable = std::min<std::int64_t>(static_cast<std::int64_t>(Limit), System::Trunc(pas::real_divide(aPlayer::GetPlayer()->Money, UnitPrice)));
            if (Affordable <= 0) {
                GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                StartMoneyWarning();
                return;
            }
            Amount = Affordable;
            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                Text = pas::concat_wide({aConst::LocalizedText(u"FormShip.FuelAct"_wref.get()), u"\r\n", u"<color=0,50,200>", aConst::LocalizedText(u"Items.Cistern.Name2"_wref.get()), u"</color>"});
                if (([&] {
                    std::int32_t money = aPlayer::GetPlayer()->Money;
                    pas::WideString cpp_arg = pas::concat_wide({fEquipmentShop::GetShopItemIconName(Cistern), u"s"});
                    pas::WideString smallFontName = GlobalsV::SmallFontName;
                    const pas::WideString& text = Text;
                    return fCount2::ShowCountDialogWithFont(this, u""_wref.get(), text, 1, Limit, Affordable, UnitPrice, Limit, money, Amount, std::move(cpp_arg), std::move(smallFontName));
                }()) != 1) {
                    return;
                }
            }
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                Cost = aShip::CalculateRoundedFuelCost(Amount, aPlayer::GetPlayer()->CurrentPlanet->OwnerId);
            } else {
                Cost = aShip::CalculateRoundedFuelCost(Amount, aGalaxyStruct::oiUninhabited);
            }
            if (aPlayer::GetPlayer()->Money < Cost) {
                GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                StartMoneyWarning();
                return;
            }
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
            Cistern->Fuel += Amount;
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(499);
            GR_Main::SoundManager->PlaySound(u"Sound.Buy"_wref.get());
            RefreshShipView();
            fShip2::RemoveEmptyPlayerHoldSlots();
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        } else if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TFuelTanks*>(SelectedHoldItem) != nullptr && ([&] {
            std::int32_t cpp_left_2 = static_cast<aItem::TFuelTanks*>(SelectedHoldItem)->Fuel;
            return cpp_left_2 < static_cast<aItem::TFuelTanks*>(SelectedHoldItem)->Capacity;
        }())) {
            FuelTanks = static_cast<aItem::TFuelTanks*>(SelectedHoldItem);
            Limit = FuelTanks->Capacity - FuelTanks->Fuel;
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                UnitPrice = aShip::CalculateFuelCost(1, aPlayer::GetPlayer()->CurrentPlanet->OwnerId);
            } else {
                UnitPrice = aShip::CalculateFuelCost(1, aGalaxyStruct::oiUninhabited);
            }
            Affordable = std::min<std::int64_t>(static_cast<std::int64_t>(Limit), System::Trunc(pas::real_divide(aPlayer::GetPlayer()->Money, UnitPrice)));
            if (Affordable <= 0) {
                GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                StartMoneyWarning();
                return;
            }
            Amount = Affordable;
            if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                Text = pas::concat_wide({aConst::LocalizedText(u"FormShip.FuelAct"_wref.get()), u"\r\n", u"<color=0,50,200>", FuelTanks->GetDisplayName(), u"</color>"});
                if (([&] {
                    std::int32_t money_2 = aPlayer::GetPlayer()->Money;
                    pas::WideString cpp_arg_2 = pas::concat_wide({fEquipmentShop::GetShopItemIconName(FuelTanks), u"s"});
                    pas::WideString smallFontName_2 = GlobalsV::SmallFontName;
                    const pas::WideString& text_2 = Text;
                    return fCount2::ShowCountDialogWithFont(this, u""_wref.get(), text_2, 1, Limit, Affordable, UnitPrice, Limit, money_2, Amount, std::move(cpp_arg_2), std::move(smallFontName_2));
                }()) != 1) {
                    return;
                }
            }
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                Cost = aShip::CalculateRoundedFuelCost(Amount, aPlayer::GetPlayer()->CurrentPlanet->OwnerId);
            } else {
                Cost = aShip::CalculateRoundedFuelCost(Amount, aGalaxyStruct::oiUninhabited);
            }
            if (aPlayer::GetPlayer()->Money < Cost) {
                GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                StartMoneyWarning();
                return;
            }
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
            FuelTanks->Fuel += Amount;
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(500);
            GR_Main::SoundManager->PlaySound(u"Sound.Buy"_wref.get());
            RefreshShipView();
            fShip2::RemoveEmptyPlayerHoldSlots();
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        } else {
            if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
                if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                    GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                        pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                        pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                        return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), u"<color=255,240,100>"_w);
                    }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                } else {
                    GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                        pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                        pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                        return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), u"<color=255,240,100>"_w);
                    }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                }
                return;
            }
            if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem) != nullptr && pas::in_range(static_cast<aItem::TWeapon*>(SelectedHoldItem)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && static_cast<aItem::TWeapon*>(SelectedHoldItem)->Ammo < static_cast<aItem::TWeapon*>(SelectedHoldItem)->AmmoCapacity) {
                Weapon = static_cast<aItem::TWeapon*>(SelectedHoldItem);
                Limit = Weapon->AmmoCapacity - Weapon->Ammo;
                UnitPrice = aGalaxy::Galaxy->ScaleIntByTechLevel(10, 100);
                Affordable = std::min<std::int64_t>(static_cast<std::int64_t>(Limit), System::Trunc(pas::real_divide(aPlayer::GetPlayer()->Money, UnitPrice)));
                if (Affordable <= 0) {
                    GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                    StartMoneyWarning();
                    return;
                }
                Amount = Affordable;
                if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                    Text = pas::concat_wide({aConst::LocalizedText(u"FormShip.MissileAct"_wref.get()), u"\r\n", u"<color=0,50,200>", Weapon->GetDisplayName(), u"</color>"});
                    if (([&] {
                        std::int32_t money_3 = aPlayer::GetPlayer()->Money;
                        pas::WideString cpp_arg_3 = pas::concat_wide({fEquipmentShop::GetShopItemIconName(Weapon), u"s"});
                        pas::WideString smallFontName_3 = GlobalsV::SmallFontName;
                        const pas::WideString& text_3 = Text;
                        return fCount2::ShowCountDialogWithFont(this, u""_wref.get(), text_3, 1, Limit, Affordable, UnitPrice, Limit, money_3, Amount, std::move(cpp_arg_3), std::move(smallFontName_3));
                    }()) != 1) {
                        return;
                    }
                }
                Cost = System::Round(static_cast<long double>(Amount) * UnitPrice);
                if (aPlayer::GetPlayer()->Money < Cost) {
                    GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
                    StartMoneyWarning();
                    return;
                }
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
                Weapon->Ammo += Amount;
                GR_Main::SoundManager->PlaySound(u"Sound.Buy"_wref.get());
                Cost = 0;
                while (Cost < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                    if (PlayerHoldShip->FindEquippedItemInSlot(SelectedHoldItem->ItemType, Cost) == nullptr) {
                        break;
                    }
                    ++Cost;
                }
                if (Cost < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                    if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                        weapon->Target = nullptr;
                    }
                    pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(SelectedHoldItem));
                    pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | Cost;
                    static_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                    static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
                    SelectedHoldKind = phkEmpty;
                    SelectedHoldItem = nullptr;
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(501);
                RefreshShipView();
                fShip2::RemoveEmptyPlayerHoldSlots();
                ReopenRequested = true;
                PlayTransitionSounds = false;
                CloseClicked(nullptr);
            }
        }
    }

    void TfShip2::RepairAllClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        std::int32_t Nodes{};
        aItem::TEquipment* Item{};
        GI_MessageLoop::TObjectGI* Control{};
        if (!TfShip2::CanUseLocalStorage()) {
            return;
        }
        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), u"<color=255,240,100>"_w);
                }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            } else {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                    pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), u"<color=255,240,100>"_w);
                }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
            return;
        }
        std::int32_t Cost = 0;
        std::int32_t NodeCost = 0;
        Nodes = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(PlayerHoldShip->Inventory, I);
            if (aShip::TShip_CanRepairEquipmentTech(PlayerHoldShip, Item) && Item != SelectedHoldItem && (Item->EquippedFlag != 0 || pas::class_cast_if<aItem::THull*>(Item) != nullptr) && Item->NeedsRepair()) {
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); weapon != nullptr && weapon->GetWeaponInfo()->Availability == aGalaxyStruct::waNotSoldAndNodeRepair) {
                    NodeCost += aItem::TEquipment_CalculateRepairCost(Item);
                } else {
                    Cost += aItem::TEquipment_CalculateRepairCost(Item);
                }
            }
        }
        if (Cost + NodeCost <= 0) {
            return;
        }
        if (!aPlayer::GetPlayer()->CanRepairArtefactsAtLocation()) {
            NodeCost = 0;
        }
        if (aPlayer::GetPlayer()->Money < Cost + NodeCost) {
            GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
            StartMoneyWarning();
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(502);
        if (NodeCost > 0) {
            Nodes = System::Round(NodeCost * 0.0025L);
            if (Nodes == 0) {
                Nodes = 1;
            }
            if (aPlayer::GetPlayer()->GetAvailableNodeCount(PlayerHoldShip) < Nodes) {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(Nodes);
                    pas::WideString localizedText = aConst::LocalizedText(u"FormShip.RepairMsgAllNeedNode"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<NeedNode>"_w, std::move(intToStr));
                }()), GI_MessageBox::mbgCancel, 0, 0, 0);
                Nodes = 0;
            } else if (GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(Nodes);
                pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.RepairMsgEnoughNode"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<NeedNode>"_w, std::move(intToStr_2));
            }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), Nodes, PlayerHoldShip);
            } else {
                Nodes = 0;
            }
        }
        if (Nodes != 0) {
            Cost += NodeCost;
        }
        if (Cost > 0) {
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->Inventory) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(PlayerHoldShip->Inventory, I);
                if (aShip::TShip_CanRepairEquipmentTech(PlayerHoldShip, Item) && (!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) || static_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair || Nodes != 0) && Item != SelectedHoldItem && (Item->EquippedFlag != 0 || pas::class_cast_if<aItem::THull*>(Item) != nullptr) && Item->NeedsRepair()) {
                    Item->Repair();
                    if (pas::class_cast_if<aItem::THull*>(Item) != nullptr) {
                        Control = GetByName(u"HullRepair"sv);
                    } else if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                        Control = GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"S_Weapon_", SysUtils::IntToStr(static_cast<std::int32_t>(Item->AssignedSlotData) & aItem::EquipmentSlotIndexMask), "Repair"}))));
                    } else {
                        Control = GetByName(pas::view(pas::concat_wide({u"S_", aConst::ItemTypeNames[Item->ItemType], u"_", pas::wide_int_to_str(static_cast<std::int32_t>(Item->AssignedSlotData) & aItem::EquipmentSlotIndexMask), u"Repair"})));
                    }
                    if (Control != nullptr) {
                        GI_GAI::TgaiGI* cpp_with = pas::checked_cast<GI_GAI::TgaiGI*>(Control);
                        cpp_with->SetActive(true);
                    }
                }
            }
            ShipStateChanged = true;
            GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
            if (SelectedHoldKind == phkEquipment) {
                I = 0;
                while (I < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                    if (PlayerHoldShip->FindEquippedItemInSlot(SelectedHoldItem->ItemType, I) == nullptr) {
                        break;
                    }
                    ++I;
                }
                if (I < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                    if (aItem::TWeapon* weapon_2 = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                        weapon_2->Target = nullptr;
                    }
                    pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(SelectedHoldItem));
                    pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | I;
                    static_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                    static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
                    SelectedHoldKind = phkEmpty;
                    SelectedHoldItem = nullptr;
                }
            } else if (SelectedHoldKind == phkArtefact && (static_cast<std::uint8_t>(PlayerHoldShip->HasEquippedArtefactOfSameUseGroup(SelectedHoldItem) ^ 1) || aGalaxy::Galaxy->AreDuplicateArtefactsEnabled())) {
                pas::list_add(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(SelectedHoldItem));
                pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = 0u;
                static_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(503);
            RefreshShipView();
            fShip2::RemoveEmptyPlayerHoldSlots();
            PlayServiceAnimations = true;
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    void TfShip2::RepairAllMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        {
            GI_GraphButton::TGraphButtonGI* SC_RepareFull_But = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_RepareFull_But"sv));
            HighlightRepairableEquipment = static_cast<std::uint8_t>(SC_RepareFull_But->Disabled ^ 1);
        }
        RefreshShipView();
    }

    void TfShip2::RepairAllMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        HighlightRepairableEquipment = false;
        RefreshShipView();
    }

    void TfShip2::LoadRockets(std::uint8_t Equipped) {
        std::int32_t I{};
        std::int32_t Cost{};
        aItem::TEquipment* Item{};
        pas::WideString Suffix{};
        if (!TfShip2::CanUseLocalStorage()) {
            return;
        }
        if (Equipped) {
            GI_MessageLoop::TObjectGI* LoadRocketsInSlots = GetByName(u"LoadRocketsInSlots"sv);
            if (static_cast<std::uint8_t>(LoadRocketsInSlots->Active ^ 1) || static_cast<std::uint8_t>(LoadRocketsInSlots->Parent->Active ^ 1)) {
                return;
            }
        }
        if (!Equipped) {
            GI_MessageLoop::TObjectGI* LoadRocketsInHold = GetByName(u"LoadRocketsInHold"sv);
            if (static_cast<std::uint8_t>(LoadRocketsInHold->Active ^ 1) || static_cast<std::uint8_t>(LoadRocketsInHold->Parent->Active ^ 1)) {
                return;
            }
        }
        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), u"<color=255,240,100>"_w);
                }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            } else {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                    pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), u"<color=255,240,100>"_w);
                }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
            return;
        }
        Cost = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(PlayerHoldShip->Inventory, I);
            if (Equipped == static_cast<std::uint8_t>(Item->EquippedFlag) && Item != SelectedHoldItem && pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && static_cast<aItem::TWeapon*>(Item)->NeedsAmmo()) {
                Cost += static_cast<aItem::TWeapon*>(Item)->CalculateAmmoRefillCost();
            }
        }
        if (Equipped) {
            Suffix = u"Slot"_w;
        } else {
            Suffix = u"Hold"_w;
        }
        if (Cost <= 0) {
            GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(pas::concat_wide({u"FormShip.ReloadAll.", Suffix, u".NoItem"})), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            return;
        }
        if (aPlayer::GetPlayer()->Money < Cost) {
            GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(Cost);
                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(pas::concat_wide({u"FormShip.ReloadAll.", Suffix, u".NoMoney"}));
                return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
            }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            return;
        }
        if (GI_MessageBox::ShowMessageBoxGI(this, ([&] {
            pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
            pas::WideString localizedColorText_4 = aConst::LocalizedColorText(pas::concat_wide({u"FormShip.ReloadAll.", Suffix, u".Confirm"}));
            return aMyFunction::FormatText1(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2));
        }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == 2) {
            return;
        }
        if (aPlayer::GetPlayer()->Money < Cost) {
            GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
            StartMoneyWarning();
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(504);
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
        std::int32_t Quantity = 0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->Inventory) - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(PlayerHoldShip->Inventory, I);
            if (Equipped == static_cast<std::uint8_t>(Item->EquippedFlag) && Item != SelectedHoldItem && pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && static_cast<aItem::TWeapon*>(Item)->NeedsAmmo()) {
                {
                    std::int32_t cpp_left = Quantity + static_cast<aItem::TWeapon*>(Item)->AmmoCapacity;
                    Quantity = cpp_left - static_cast<aItem::TWeapon*>(Item)->Ammo;
                }
                static_cast<aItem::TWeapon*>(Item)->Ammo = static_cast<aItem::TWeapon*>(Item)->AmmoCapacity;
            }
        }
        aGalaxyEvent::TGalaxyEvent* Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerBuysMissiles"_w, nullptr);
        Event->AddData(Cost);
        Event->AddData(Quantity);
        ShipStateChanged = true;
        GR_Main::SoundManager->PlaySound(u"Sound.Buy"_wref.get());
        if (SelectedHoldKind == phkEquipment) {
            I = 0;
            while (I < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                if (PlayerHoldShip->FindEquippedItemInSlot(SelectedHoldItem->ItemType, I) == nullptr) {
                    break;
                }
                ++I;
            }
            if (I < PlayerHoldShip->GetSlotCountForItemType(SelectedHoldItem->ItemType)) {
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(SelectedHoldItem)) {
                    weapon->Target = nullptr;
                }
                pas::list_add(PlayerHoldShip->Inventory, reinterpret_cast<void*>(SelectedHoldItem));
                pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | I;
                static_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
                static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
                SelectedHoldKind = phkEmpty;
                SelectedHoldItem = nullptr;
            }
        } else if (SelectedHoldKind == phkArtefact && (static_cast<std::uint8_t>(PlayerHoldShip->HasEquippedArtefactOfSameUseGroup(SelectedHoldItem) ^ 1) || aGalaxy::Galaxy->AreDuplicateArtefactsEnabled())) {
            pas::list_add(PlayerHoldShip->Artefacts, reinterpret_cast<void*>(SelectedHoldItem));
            pas::checked_cast<aItem::TEquipment*>(SelectedHoldItem)->AssignedSlotData = 0u;
            static_cast<aItem::TEquipment*>(SelectedHoldItem)->EquippedFlag = 0;
            static_cast<aItem::TEquipment*>(SelectedHoldItem)->Equip();
            SelectedHoldKind = phkEmpty;
            SelectedHoldItem = nullptr;
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(505);
        RefreshShipView();
        fShip2::RemoveEmptyPlayerHoldSlots();
        PlayServiceAnimations = true;
        ReopenRequested = true;
        PlayTransitionSounds = false;
        CloseClicked(nullptr);
    }

    void TfShip2::ToggleAfterburner(GI_MessageLoop::TObjectGI* Sender) {
        aGalaxy::Galaxy->CheckIntegrityChecksum1(506);
        PlayerHoldShip->AfterburnerActive = static_cast<std::uint8_t>(PlayerHoldShip->AfterburnerActive ^ 1);
        PlayerHoldShip->RefreshDerivedStats(true);
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(507);
        RefreshShipView();
        ShowShipPropertyInfo(GetByName(u"ForsageBut"sv));
        if (PlayerHoldShip->AfterburnerActive) {
            GR_Main::SoundManager->PlaySound(u"Sound.ForsageOn"_wref.get());
        } else {
            GR_Main::SoundManager->PlaySound(u"Sound.ForsageOff"_wref.get());
        }
        if (PlayerHoldShip->AfterburnerActive) {
            GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormShip.ForsageActivate"_wref.get()), GI_MessageBox::mbgOK, 0, 0, 0);
        }
    }

    void TfShip2::ShowItemInfo() {
        aItem::TEquipment* Item{};
        std::int32_t I{};
        std::int32_t Slot{};
        std::int32_t Count{};
        std::int32_t Column{};
        std::int32_t Row{};
        TPlayerHoldUnit* Hold{};
        aPlayer::PStorageEntry Stored{};
        GI_Window::TWindowGI* InfoWindow{};
        WindowsSdk::TPoint Position{};
        WindowsSdk::TPoint CellSize{};
        std::uint8_t Found = false;
        std::uint8_t CanTake = false;
        GI_GAI::TgaiGI* Animation = nullptr;
        Position = ClassesImports::Point(0, 0);
        CellSize = ClassesImports::Point(0, 0);
        std::uint8_t CenterX = false;
        std::uint8_t CenterY = false;
        if (!Found) {
            Count = PlayerHoldShip->GetSlotCountForItemType(aConst::t_Artefact);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Slot); ) {
                Item = PlayerHoldShip->FindEquippedItemInSlot(aConst::t_Artefact, Slot);
                if (Item != nullptr) {
                    GI_Zone::TZoneGI* cpp_with = ArtefactSlotZones[Slot];
                    if (cpp_with->HitTest(GetCursorPoint())) {
                        CenterX = true;
                        Position = ClassesImports::Point(cpp_with->HitTestBounds.Left + cpp_with->ClientSize.X / 2, cpp_with->HitTestBounds.Top + cpp_with->ClientSize.Y);
                        ShowEquipmentInfo(Item, false);
                        Found = true;
                        CanTake = true;
                        break;
                    }
                }
            }
        }
        if (!Found) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 7); cpp_range_2.next(I); ) {
                Count = PlayerHoldShip->GetSlotCountForItemType(aConst::EquipmentSlotLayouts[I].ItemType);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Slot); ) {
                    Item = PlayerHoldShip->FindEquippedItemInSlot(aConst::EquipmentSlotLayouts[I].ItemType, Slot);
                    if (Item != nullptr) {
                        GI_Zone::TZoneGI* cpp_with_2 = EquipmentSlotZones[I][Slot];
                        if (cpp_with_2->HitTest(GetCursorPoint())) {
                            CenterX = true;
                            Position = ClassesImports::Point(cpp_with_2->HitTestBounds.Left + cpp_with_2->ClientSize.X / 2, cpp_with_2->HitTestBounds.Top + cpp_with_2->ClientSize.Y);
                            ShowEquipmentInfo(Item, false);
                            Found = true;
                            CanTake = true;
                            if (GlobalsV::AnimItem) {
                                Animation = EquipmentSlotAnimations[I][Slot];
                            }
                            break;
                        }
                    }
                }
            }
        }
        if (!Found) {
            GI_Zone::TZoneGI* S_Hull_0z = pas::checked_cast<GI_Zone::TZoneGI*>(GetByName(u"S_Hull_0z"sv));
            if (S_Hull_0z->HitTest(GetCursorPoint())) {
                CenterX = true;
                Position = ClassesImports::Point(S_Hull_0z->HitTestBounds.Left + S_Hull_0z->ClientSize.X / 2, S_Hull_0z->HitTestBounds.Top + S_Hull_0z->ClientSize.Y);
                ShowEquipmentInfo(PlayerHoldShip->GetHull(), false);
                Found = true;
                if (TfShip2::IsHoldNormalShip()) {
                    CanTake = true;
                }
            }
        }
        if (static_cast<std::uint8_t>(Found ^ 1) && static_cast<std::uint8_t>(RemoteHoldMode ^ 1)) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 5); cpp_range_4.next(I); ) {
                if (HoldFirstIndex + I >= pas::list_count(PlayerHoldEntries)) {
                    Hold = nullptr;
                } else {
                    Hold = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, HoldFirstIndex + I);
                }
                {
                    GI_Zone::TZoneGI* cpp_with_4 = HoldSlotZones[I];
                    if (cpp_with_4->HitTest(GetCursorPoint())) {
                        CenterX = true;
                        Position = ClassesImports::Point(cpp_with_4->HitTestBounds.Left + cpp_with_4->ClientSize.X / 2, cpp_with_4->HitTestBounds.Top + cpp_with_4->ClientSize.Y);
                        CellSize = cpp_with_4->ClientSize;
                        if (Hold != nullptr && (Hold->Kind == phkEquipment || Hold->Kind == phkArtefact)) {
                            ShowEquipmentInfo(Hold->Item, false);
                        } else if (Hold != nullptr && Hold->Kind == phkGoods) {
                            ShowHoldGoodsInfo(Hold->GoodsIndex);
                        } else {
                            continue;
                        }
                        Found = true;
                        CanTake = true;
                        break;
                    }
                }
            }
        }
        if (static_cast<std::uint8_t>(Found ^ 1) && RemoteHoldMode) {
            for (Row = 0; Row <= 10; ++Row) {
                Column = 0;
                while (Column < 5) {
                    if (RemoteHoldImages[Row * 5 + Column]->ContainsPoint(GetCursorPoint())) {
                        CenterY = true;
                        Position.X = -(670 + GR_Main::ExtraScreenWidth / 2);
                        {
                            GI_Image::TImageGI* cpp_with_5 = RemoteHoldImages[Row * 5 + Column];
                            Position.Y = cpp_with_5->HitTestBounds.Top + cpp_with_5->ClientSize.Y / 2;
                        }
                        I = fShip2::FindPlayerHoldIndexByOrder(RemoteHoldFirstOrder + Column + Row * 5);
                        if (I >= 0) {
                            Hold = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
                            if (Hold != nullptr && (Hold->Kind == phkEquipment || Hold->Kind == phkArtefact)) {
                                ShowEquipmentInfo(Hold->Item, false);
                            } else if (Hold != nullptr && Hold->Kind == phkGoods) {
                                ShowHoldGoodsInfo(Hold->GoodsIndex);
                            } else {
                                continue;
                            }
                            Found = true;
                            CanTake = true;
                            break;
                        }
                    }
                    ++Column;
                }
                if (Column < 5) {
                    break;
                }
            }
        }
        if (static_cast<std::uint8_t>(Found ^ 1) && StorageUpButton->Active) {
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, StorageImageCount - 1); cpp_range_5.next(I); ) {
                if (StorageImages[I]->ContainsPoint(GetCursorPoint())) {
                    Slot = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(TfShip2::GetLocalStorageOwner(), StorageFirstSlot + I);
                    if (Slot >= 0) {
                        Stored = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, Slot);
                        CenterY = true;
                        Position.X = 158;
                        {
                            GI_Image::TImageGI* cpp_with_6 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Storage_", SysUtils::IntToStr(I), "i"})))));
                            Position.Y = cpp_with_6->HitTestBounds.Top + cpp_with_6->ClientSize.Y / 2;
                        }
                        if (pas::class_cast_if<aItem::TGoods*>(Stored->Item) != nullptr) {
                            ShowStoredGoodsInfo(pas::checked_cast<aItem::TGoods*>(Stored->Item));
                        } else {
                            ShowEquipmentInfo(Stored->Item, true);
                        }
                        Found = true;
                        CanTake = true;
                    }
                }
            }
        }
        if (Position.X != 0 || Position.Y != 0) {
            if (ItemInfoWindow->Active) {
                InfoWindow = ItemInfoWindow;
            } else {
                InfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"InfoHull"sv));
            }
            if (GlobalsV::DynamicTipsPos) {
                if (CenterX) {
                    Position.X -= InfoWindow->ClientSize.X / 2;
                }
                if (Position.X < 0) {
                    Position.X = pas::abs(Position.X) - InfoWindow->ClientSize.X;
                }
                if (CenterY) {
                    Position.Y -= InfoWindow->ClientSize.Y / 2;
                    if (Position.Y + InfoWindow->ClientSize.Y + 10 > GR_Main::GameScreenHeight) {
                        Position.Y = GR_Main::GameScreenHeight - InfoWindow->ClientSize.Y - 10;
                    }
                } else if (Position.Y + InfoWindow->ClientSize.Y + 10 > GR_Main::GameScreenHeight) {
                    Position.Y = Position.Y - CellSize.Y - InfoWindow->ClientSize.Y;
                }
                InfoWindow->SetPosition(Position);
            } else {
                InfoWindow->SetPosition(ClassesImports::Point(10, 10));
            }
        }
        if (SelectedHoldKind == phkEmpty) {
            if (CanTake) {
                if (!IsCursorImageSelected(u"Take"sv)) {
                    SetCursorByName(u"Take"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"Main"sv)) {
                SetCursorByName(u"Main"_wref.get());
            }
        }
        if (static_cast<std::uint8_t>(Found ^ 1) && ItemInfoHideTimer == nullptr) {
            ShowEquipmentInfo(nullptr, false);
        }
        if (Animation != HoveredEquipmentAnimation && HoveredEquipmentAnimation != nullptr) {
            HoveredEquipmentAnimation->StopAutoPlayback();
            HoveredEquipmentAnimation->UserState = HoveredEquipmentAnimation->SequenceFrame;
            HoveredEquipmentAnimation = nullptr;
        }
        if (Animation != HoveredEquipmentAnimation && Animation != nullptr) {
            HoveredEquipmentAnimation = Animation;
            HoveredEquipmentAnimation->UpdateAutoGeometry();
            HoveredEquipmentAnimation->SetSequenceFrame(std::min<std::int64_t>(static_cast<std::int64_t>(static_cast<std::uint32_t>(HoveredEquipmentAnimation->UserState)), static_cast<std::int64_t>(HoveredEquipmentAnimation->SequenceFrameCount - 1)));
            HoveredEquipmentAnimation->SetActive(true);
            HoveredEquipmentAnimation->RestartPlayback();
            reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(HoveredEquipmentAnimation->UserData)))->SetActive(false);
        }
    }

    void TfShip2::ShowItemInfoTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (RightPanelSlideTimer == nullptr) {
            ShowItemInfo();
        }
    }

    void TfShip2::HideItemInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        DisplayedItemKey = 0;
        if (ItemInfoHideTimer != nullptr) {
            CancelCallbackTimer(ItemInfoHideTimer);
            ItemInfoHideTimer = nullptr;
        }
        GetByName(u"PII"sv)->SetActive(false);
        GetByName(u"InfoHull"sv)->SetActive(false);
        RefreshActionPanels(SelectedHoldKind, SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, SelectedHoldItem, SelectedHoldOrigin);
    }

    void TfShip2::LayoutItemInfo(GI_Window::TWindowGI* Window, GI_Label::TLabelGI* Title, GI_Label::TLabelGI* Text, std::uint8_t KeepMinimumHeight, std::uint8_t WordWrap, std::int32_t MinimumWidth) {
        WindowsSdk::TPoint TextSize{};
        WindowsSdk::TPoint WindowSize{};
        WindowsSdk::TPoint TargetSize{};
        float Ratio{};
        WindowsSdk::TRect Borders{};
        float TargetRatio = 1.6230366f;
        Borders = Window->WorkSubRect;
        TargetSize = Window->AlignSizeToBorderTiles(ClassesImports::Point(0, 0));
        std::int32_t Attempts = 100;
        while (Attempts > 0) {
            Text->SetTextAlignX(GI_Main::taxCenter);
            Text->SetTextAlignY(GI_Main::tayCenter);
            Text->SetSize(ClassesImports::Point(TargetSize.X - Borders.Left - Borders.Right, 1));
            if (WordWrap) {
                Text->SetWordWrapEnabled(true);
                Text->SetTextAlignX(GI_Main::taxLeft);
                Text->SetTextAlignY(GI_Main::tayAuto);
            } else {
                Text->SetWordWrapEnabled(false);
                Text->SetTextAlignX(GI_Main::taxAuto);
                Text->SetTextAlignY(GI_Main::tayAuto);
            }
            TextSize = Text->ClientSize;
            if (KeepMinimumHeight) {
                Window->SetSize(ClassesImports::Point(TextSize.X + Borders.Left + Borders.Right, std::max<std::int32_t>(TargetSize.Y, TextSize.Y + Borders.Top + Borders.Bottom)));
            } else {
                Window->SetSize(ClassesImports::Point(TextSize.X + Borders.Left + Borders.Right, TextSize.Y + Borders.Top + Borders.Bottom));
            }
            Window->UpdateAutoGeometry();
            WindowSize = Window->ClientSize;
            Ratio = pas::real_divide(WindowSize.X, WindowSize.Y);
            if (static_cast<long double>(Ratio) - TargetRatio < -0.1L) {
                TargetSize.X += 10;
            } else if (static_cast<long double>(Ratio) - TargetRatio > 0.1L) {
                TargetSize.Y += 10;
            } else {
                break;
            }
            --Attempts;
        }
        Window->SetSize(ClassesImports::Point(std::max<std::int32_t>(MinimumWidth, Window->ClientSize.X), Window->ClientSize.Y));
        Window->UpdateAutoGeometry();
        Text->SetTextAlignX(GI_Main::taxCenter);
        Text->SetTextAlignY(GI_Main::tayCenterEx);
        Text->SetSize(ClassesImports::Point(Window->ClientSize.X - Borders.Left - Borders.Right, Window->ClientSize.Y - Borders.Top - Borders.Bottom));
        Text->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&Borders, 0)));
        Title->SetSize(ClassesImports::Point(Window->ClientSize.X - Window->WorkSubRect.Right - Title->LocalPosition.X - 15, Title->ClientSize.Y));
    }

    // Two-column planet/ship statistics, shared with live and recorded star-map panels.
    void TfShip2::LayoutObjectInfo(GI_Window::TWindowGI* Window, GI_Label::TLabelGI* Title, GI_Label::TLabelGI* Left1, GI_Label::TLabelGI* Right1, GI_Label::TLabelGI* Left2, GI_Label::TLabelGI* Right2, GI_Label::TLabelGI* Left3, GI_Label::TLabelGI* Right3, GI_Label::TLabelGI* Left4, GI_Label::TLabelGI* Right4, GI_Label::TLabelGI* Left5, GI_Label::TLabelGI* Right5, GI_Label::TLabelGI* Left6, GI_Label::TLabelGI* Right6, GI_Label::TLabelGI* Left7, GI_Label::TLabelGI* Right7, GI_Label::TLabelGI* Left8, GI_Label::TLabelGI* Right8, GI_MessageLoop::TObjectGI* Emblem, std::uint8_t KeepMinimumHeight, std::int32_t MinimumWidth) {
        float Ratio{};
        WindowsSdk::TPoint TextSize{};
        WindowsSdk::TPoint WindowSize{};
        WindowsSdk::TPoint TargetSize{};
        // Native frame retains one unused local.
        std::int32_t Attempts{};
        WindowsSdk::TRect Borders{};
        std::int32_t LeftWidth{};
        std::int32_t RightWidth{};
        std::int32_t TotalHeight{};
        std::int32_t EmblemMargin{};
        std::int32_t CenterX{};
        std::int32_t CenterY{};
        std::int32_t RowY{};
        // Nested in LayoutObjectInfo; captures column widths, total height and row count.
        auto MeasurePair = [&](GI_Label::TLabelGI* Left, GI_Label::TLabelGI* Right) -> void {
            if (Left != nullptr && Right != nullptr && Left->Active && Right->Active) {
                Left->SetTextAlignX(GI_Main::taxAuto);
                Right->SetTextAlignX(GI_Main::taxAuto);
                LeftWidth = std::max<std::int32_t>(LeftWidth, Left->ClientSize.X);
                RightWidth = std::max<std::int32_t>(RightWidth, Right->ClientSize.X);
                TotalHeight += std::max<std::int32_t>(Left->ClientSize.Y, Right->ClientSize.Y);
                ++Attempts;
            }
        };
        // Nested in LayoutObjectInfo; captures window, border dimensions and accumulated row position.
        auto PlacePair = [&](GI_Label::TLabelGI* Left, GI_Label::TLabelGI* Right) -> void {
            if (Left != nullptr && Right != nullptr && Left->Active && Right->Active) {
                CenterX = (Window->ClientSize.X - Borders.Left - Borders.Right) / 2 - (LeftWidth + RightWidth + EmblemMargin) / 2;
                CenterY = (Window->ClientSize.Y - Borders.Top - Borders.Bottom) / 2 - TotalHeight / 2;
                Left->SetPosition(ClassesImports::Point(Borders.Left + CenterX + LeftWidth - Left->ClientSize.X, RowY + Borders.Top + CenterY));
                Right->SetPosition(ClassesImports::Point(Borders.Left + CenterX + LeftWidth, RowY + Borders.Top + CenterY));
                RowY += std::max<std::int32_t>(Left->ClientSize.Y, Right->ClientSize.Y);
                ++Attempts;
            }
        };
        float TargetRatio = 1.6230366f;
        LeftWidth = 0;
        RightWidth = 0;
        TotalHeight = 0;
        RowY = 0;
        Attempts = 0;
        EmblemMargin = 0;
        if (Emblem != nullptr) {
            EmblemMargin = Emblem->ClientSize.X / 2;
        }
        Borders = Window->WorkSubRect;
        MeasurePair(Left1, Right1);
        MeasurePair(Left2, Right2);
        MeasurePair(Left3, Right3);
        MeasurePair(Left4, Right4);
        MeasurePair(Left5, Right5);
        MeasurePair(Left6, Right6);
        MeasurePair(Left7, Right7);
        MeasurePair(Left8, Right8);
        TargetSize = Window->AlignSizeToBorderTiles(ClassesImports::Point(0, 0));
        Attempts = 100;
        while (Attempts > 0) {
            TextSize = ClassesImports::Point(LeftWidth + RightWidth + EmblemMargin, TotalHeight);
            if (KeepMinimumHeight) {
                Window->SetSize(ClassesImports::Point(TextSize.X + Borders.Left + Borders.Right, std::max<std::int32_t>(TargetSize.Y, TextSize.Y + Borders.Top + Borders.Bottom)));
            } else {
                Window->SetSize(ClassesImports::Point(TextSize.X + Borders.Left + Borders.Right, TextSize.Y + Borders.Top + Borders.Bottom));
            }
            Window->UpdateAutoGeometry();
            WindowSize = Window->ClientSize;
            Ratio = pas::real_divide(WindowSize.X, WindowSize.Y);
            if (static_cast<long double>(Ratio) - TargetRatio < -0.1L) {
                TargetSize.X += 10;
            } else if (static_cast<long double>(Ratio) - TargetRatio > 0.1L) {
                TargetSize.Y += 10;
            } else {
                break;
            }
            --Attempts;
        }
        Window->SetSize(ClassesImports::Point(std::max<std::int32_t>(MinimumWidth, Window->ClientSize.X), Window->ClientSize.Y));
        Window->UpdateAutoGeometry();
        Attempts = 0;
        PlacePair(Left1, Right1);
        PlacePair(Left2, Right2);
        PlacePair(Left3, Right3);
        PlacePair(Left4, Right4);
        PlacePair(Left5, Right5);
        PlacePair(Left6, Right6);
        PlacePair(Left7, Right7);
        PlacePair(Left8, Right8);
        Title->SetSize(ClassesImports::Point(Window->ClientSize.X - Window->WorkSubRect.Right - Title->LocalPosition.X - 15, Title->ClientSize.Y));
        if (Emblem != nullptr) {
            Emblem->SetPosition(ClassesImports::Point(Window->ClientSize.X + ItemRaceImagePosition.X, Window->ClientSize.Y + ItemRaceImagePosition.Y - GR_Main::GiScalePixels(5)));
        }
    }

    void TfShip2::ShowEquipmentInfo(aItem::TItem* Item, std::uint8_t FromStorage) {
        static const pas::Set<0, 255> DurableTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        aItem::TEquipment* Equipment{};
        std::int32_t Origin{};
        pas::WideString Text{};
        std::uint8_t SpecialHull{};
        std::int32_t BarWidth{};
        std::int32_t CapWidth{};
        std::int32_t MinimumWidth{};
        Equipment = pas::checked_cast<aItem::TEquipment*>(Item);
        if (Equipment == nullptr) {
            DisplayedItemKey = 0;
            if (ItemInfoHideTimer != nullptr) {
                CancelCallbackTimer(ItemInfoHideTimer);
                ItemInfoHideTimer = nullptr;
            }
            ItemInfoHideTimer = ScheduleCallbackTimer(100, 99999, pas::bind_method<&TfShip2::HideItemInfo>(this), 0);
            if (aPlayer::GetPlayer() == PlayerHoldShip && pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->OnUseText != u"") {
                OpenUseSidePanel();
            } else if (aPlayer::GetPlayer() == PlayerHoldShip && SelectedHoldKind == phkArtefact && pas::class_cast_if<aItem::TArtefact*>(SelectedHoldItem) != nullptr && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && static_cast<aItem::TArtefact*>(SelectedHoldItem)->GetOnUseCodeText() != u"") {
                OpenUseSidePanel();
            } else if (aPlayer::GetPlayer() == PlayerHoldShip && SelectedHoldKind == phkEquipment && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && SelectedHoldItem->ItemType == aConst::t_UselessItem && pas::checked_cast<aItem::TUselessItem*>(SelectedHoldItem)->GetOnUseCodeText() != u"") {
                OpenUseSidePanel();
            } else if (SelectedHoldKind == phkEquipment && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && SelectedHoldItem->ItemType == aConst::t_TreasureMap) {
                OpenUseSidePanel();
            } else {
                CloseUseSidePanel();
            }
        } else if (DisplayedItemKey != static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item))) {
            DisplayedItemKey = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item));
            TfShip2::UpdateInfoHint(0, 0);
            GR_Main::SoundManager->PlaySound(u"Sound.ShipItemInfo"_wref.get());
            if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
                if (Item->ScriptItem != nullptr) {
                    reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnShowingItemInfo, PlayerHoldShip, nullptr, nullptr, 0);
                }
                if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
                    aScript::RunItemConfigActionCode(Item, aConst::satOnShowingItemInfo, PlayerHoldShip, nullptr, nullptr, 0);
                }
            }
            if (aPlayer::GetPlayer() == PlayerHoldShip && pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && SelectedHoldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(SelectedHoldItem->ScriptItem)->OnUseText != u"") {
                OpenUseSidePanel();
            } else if (aPlayer::GetPlayer() == PlayerHoldShip && SelectedHoldKind == phkArtefact && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && pas::class_cast_if<aItem::TArtefact*>(SelectedHoldItem) != nullptr && static_cast<aItem::TArtefact*>(SelectedHoldItem)->GetOnUseCodeText() != u"") {
                OpenUseSidePanel();
            } else if (aPlayer::GetPlayer() == PlayerHoldShip && SelectedHoldKind == phkEquipment && SelectedHoldItem->ItemType == aConst::t_UselessItem && pas::checked_cast<aItem::TUselessItem*>(SelectedHoldItem)->GetOnUseCodeText() != u"") {
                OpenUseSidePanel();
            }
            if (aPlayer::GetPlayer() == PlayerHoldShip && Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->OnUseText != u"") {
                OpenUseSidePanel();
            } else if (aPlayer::GetPlayer() == PlayerHoldShip && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr && static_cast<aItem::TArtefact*>(Item)->GetOnUseCodeText() != u"") {
                OpenUseSidePanel();
            } else if (aPlayer::GetPlayer() == PlayerHoldShip && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && Item->ItemType == aConst::t_UselessItem && pas::checked_cast<aItem::TUselessItem*>(Item)->GetOnUseCodeText() != u"") {
                OpenUseSidePanel();
            } else if (Item->ItemType == aConst::t_TreasureMap && static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1)) {
                OpenUseSidePanel();
            }
            if (ItemInfoHideTimer != nullptr) {
                CancelCallbackTimer(ItemInfoHideTimer);
                ItemInfoHideTimer = nullptr;
            }
            if (FromStorage) {
                Origin = 1;
            } else {
                Origin = 0;
            }
            if (SelectedHoldKind == phkEmpty && pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                RefreshActionPanels(phkArtefact, static_cast<std::uint8_t>(Item->ItemType), 0, 0, Item, Origin);
            } else if (SelectedHoldKind == phkEmpty) {
                RefreshActionPanels(phkEquipment, static_cast<std::uint8_t>(Item->ItemType), 0, 0, Item, Origin);
            } else {
                RefreshActionPanels(SelectedHoldKind, SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, SelectedHoldItem, SelectedHoldOrigin);
            }
            SpecialHull = (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr || pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr || pas::class_cast_if<aKling::TKling*>(PlayerHoldShip) != nullptr) && PlayerHoldShip->GetHull() == Equipment;
            if (Item->ItemType == aConst::t_Hull && static_cast<std::uint8_t>(SpecialHull ^ 1)) {
                {
                    pas::WideString infoText = Equipment->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, PlayerHoldShip);
                    aItem::THull* cpp_arg = pas::checked_cast<aItem::THull*>(Item);
                    fEquipmentShop::TfEquipmentShop* equipmentShopScreen = Globals::EquipmentShopScreen;
                    equipmentShopScreen->RefreshHullInfo(this, cpp_arg, std::move(infoText), true);
                }
                ItemInfoWindow->SetActive(false);
                GetByName(u"InfoHullImage"sv)->SetActive(false);
                if (PlayerHoldShip->GetHull() == Equipment) {
                    Text = PlayerHoldShip->GetShipPortraitImagePath();
                    if (Text != u"") {
                        GI_Image::TImageGI* InfoHullImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoHullImage"sv));
                        InfoHullImage->SetActive(true);
                        InfoHullImage->SetImagePath(u"GraphBuf"_w);
                        {
                            GI_GraphBuf::TGraphBufGI* cpp_with_2 = InfoHullImage->GraphBufControl;
                            cpp_with_2->SourceHasPerPixelAlpha = true;
                            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv), cpp_with_2->GraphBuf);
                            if (static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)) {
                                cpp_with_2->GraphBuf->RescaleRgba(cpp_with_2->ClientSize.X, System::Round(pas::real_divide(cpp_with_2->ClientSize.X, static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)), 5);
                            } else {
                                cpp_with_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_2->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_2->GraphBuf->Width)), cpp_with_2->ClientSize.Y, 5);
                            }
                        }
                        InfoHullImage->SetImageKindX(GI_Main::ikxCenter);
                        InfoHullImage->SetImageKindY(GI_Main::ikyCenter);
                        {
                            Types::TPoint visualCenter = InfoHullImage->GetVisualCenter();
                            Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                            InfoHullImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                        }
                    }
                } else {
                    GI_Image::TImageGI* InfoHullImage_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoHullImage"sv));
                    InfoHullImage_2->SetActive(true);
                    InfoHullImage_2->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Equipment), u"s"}));
                    InfoHullImage_2->SetImageKindX(GI_Main::ikxCenter);
                    InfoHullImage_2->SetImageKindY(GI_Main::ikyCenter);
                    {
                        Types::TPoint visualCenter_2 = InfoHullImage_2->GetVisualCenter();
                        Types::TPoint itemImageCenter_2 = Globals::ShipScreen->ItemImageCenter;
                        InfoHullImage_2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_2, visualCenter_2));
                    }
                }
            } else {
                ItemInfoWindow->SetActive(true);
                GetByName(u"InfoHull"sv)->SetActive(false);
                {
                    GI_Image::TImageGI* cpp_with_4 = ItemImage;
                    cpp_with_4->SetActive(PlayerHoldShip->GetHull() != Equipment);
                    if (cpp_with_4->Active) {
                        cpp_with_4->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Equipment), u"s"}));
                        cpp_with_4->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_4->SetImageKindY(GI_Main::ikyCenter);
                        {
                            Types::TPoint visualCenter_3 = cpp_with_4->GetVisualCenter();
                            Types::TPoint itemImageCenter_3 = Globals::ShipScreen->ItemImageCenter;
                            cpp_with_4->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_3, visualCenter_3));
                        }
                    }
                }
                {
                    GI_GraphBuf::TGraphBufGI* InfoImage2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoImage2"sv));
                    InfoImage2->SetActive(PlayerHoldShip->GetHull() == Equipment);
                    if (InfoImage2->Active) {
                        Text = PlayerHoldShip->GetShipPortraitImagePath();
                        InfoImage2->SetActive(Text != u"");
                        if (InfoImage2->Active) {
                            InfoImage2->SourceHasPerPixelAlpha = true;
                            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv), InfoImage2->GraphBuf);
                            if (static_cast<std::uint32_t>(InfoImage2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoImage2->GraphBuf->Height)) {
                                InfoImage2->GraphBuf->RescaleRgba(InfoImage2->ClientSize.X - 5, System::Round(pas::real_divide(InfoImage2->ClientSize.X - 5, static_cast<std::uint32_t>(InfoImage2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoImage2->GraphBuf->Height)), 5);
                            } else {
                                InfoImage2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoImage2->ClientSize.Y - 5, static_cast<std::uint32_t>(InfoImage2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoImage2->GraphBuf->Width)), InfoImage2->ClientSize.Y - 5, 5);
                            }
                            InfoImage2->SetImageKindX(GI_Main::ikxCenter);
                            InfoImage2->SetImageKindY(GI_Main::ikyCenter);
                            {
                                Types::TPoint visualCenter_4 = InfoImage2->GetVisualCenter();
                                Types::TPoint itemImageCenter_4 = Globals::ShipScreen->ItemImageCenter;
                                InfoImage2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_4, visualCenter_4));
                            }
                        }
                    }
                }
                ItemNameLabel->SetText(u""_wref.get());
                {
                    const pas::WideString& wrapTextInColor = ([&] {
                        pas::WideString displayName = Equipment->GetDisplayName();
                        pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(pas::view(std::move(displayName)), pas::view(std::move(infoNameColorTag)));
                    }());
                    GI_Label::TLabelGI* itemNameLabel = ItemNameLabel;
                    itemNameLabel->SetText(wrapTextInColor);
                }
                {
                    const pas::WideString& infoText_2 = Equipment->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, PlayerHoldShip);
                    GI_Label::TLabelGI* itemDescriptionLabel = ItemDescriptionLabel;
                    itemDescriptionLabel->SetText(infoText_2);
                }
                ItemSizeLabel->SetText(pas::wide_int_to_str(Equipment->Weight));
                ItemPriceLabel->SetText(pas::wide_int_to_str(Equipment->Cost));
                {
                    GI_Image::TImageGI* cpp_with_6 = ItemRaceImage;
                    cpp_with_6->SetImagePath(aConst::GetFactionEmblemPath(aItem::TItem_GetOwnerConfigName(Equipment)));
                    cpp_with_6->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_6->SetImageKindY(GI_Main::ikyCenter);
                }
                if (static_cast<std::uint8_t>(pas::contains(DurableTypes, static_cast<std::uint8_t>(Equipment->ItemType)) ^ 1) && Equipment->ItemType != aConst::t_Hull) {
                    {
                        GI_Image::TImageGI* InfoDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"sv));
                        InfoDurable->Parent->Parent->SetActive(false);
                    }
                    MinimumWidth = 0;
                } else {
                    if (pas::class_cast_if<aItem::THull*>(Equipment) != nullptr) {
                        float real_max = pas::real_max<float>(0.1f, Equipment->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})));
                        BarWidth = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(Equipment->Weight, aConst::HullBaseSize), real_max)) * 64.0L);
                    } else {
                        BarWidth = System::Round(pas::real_divide(64.0L, pas::real_max<float>(0.1f, Equipment->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})))));
                    }
                    BarWidth = std::min<std::int32_t>(192, std::max<std::int32_t>(32, BarWidth));
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
                            std::int32_t cpp_arg_2 = cpp_left - (InfoDurable_2->GetContentSize().X - 5);
                            std::int32_t y = InfoDurable_2->LocalPosition.Y;
                            InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_2, y));
                        } else {
                            std::int64_t cpp_left_3 = System::Round(BarWidth * pas::real_divide(Equipment->ConditionPercent, 1.0E+2L));
                            std::int32_t cpp_arg_3 = cpp_left_3 - (InfoDurable_2->GetContentSize().X - 5);
                            std::int32_t y_2 = InfoDurable_2->LocalPosition.Y;
                            InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_3, y_2));
                        }
                    }
                    {
                        GI_Image::TImageGI* InfoDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableRight"sv));
                        {
                            std::int32_t cpp_arg_4 = BarWidth + CapWidth - InfoDurableRight->GetContentSize().X;
                            std::int32_t y_3 = InfoDurableRight->LocalPosition.Y;
                            InfoDurableRight->SetPosition(ClassesImports::Point(cpp_arg_4, y_3));
                        }
                        InfoDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoDurableRight->Parent->LocalPosition.Y));
                        InfoDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableRight->Parent->ClientSize.Y));
                    }
                    {
                        GI_Image::TImageGI* InfoDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableBack"sv));
                        {
                            std::int32_t cpp_arg_5 = BarWidth + 1 - InfoDurableBack->GetContentSize().X;
                            std::int32_t y_4 = InfoDurableBack->LocalPosition.Y;
                            InfoDurableBack->SetPosition(ClassesImports::Point(cpp_arg_5, y_4));
                        }
                        InfoDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableBack->Parent->ClientSize.Y));
                    }
                }
                TfShip2::LayoutItemInfo(ItemInfoWindow, ItemNameLabel, ItemDescriptionLabel, true, true, MinimumWidth);
                ItemSizeLabel->SetPosition(ClassesImports::Point(ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + ItemSizeLabelPosition.Y));
                ItemPriceLabel->SetPosition(ClassesImports::Point(ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + ItemPriceLabelPosition.Y));
                ItemRaceImage->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + ItemRaceImagePosition.Y));
            }
        }
    }

    void TfShip2::ShowHoldGoodsInfo(std::uint8_t Good) {
        pas::WideString Text{};
        TfShip2::UpdateInfoHint(0, 0);
        std::uint8_t Changed = false;
        if (ItemInfoHideTimer != nullptr) {
            CancelCallbackTimer(ItemInfoHideTimer);
            ItemInfoHideTimer = nullptr;
        }
        if (Good + 1 != DisplayedItemKey) {
            DisplayedItemKey = Good + 1;
            if (SelectedHoldKind == phkEmpty) {
                RefreshActionPanels(phkGoods, Good, PlayerHoldShip->CargoGoods[Good].Count, PlayerHoldShip->CargoGoods[Good].TotalCost, nullptr, 0);
            } else {
                RefreshActionPanels(SelectedHoldKind, SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, SelectedHoldItem, SelectedHoldOrigin);
            }
            if (!ItemInfoWindow->Active) {
                Changed = true;
            }
            ItemInfoWindow->SetActive(true);
            GetByName(u"InfoHull"sv)->SetActive(false);
            {
                GI_Image::TImageGI* InfoImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"sv));
                if (([&] {
                    pas::WideString cpp_string = InfoImage->GetImagePath();
                    pas::WideString cpp_string_2 = pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(Good))});
                    return cpp_string != cpp_string_2;
                }())) {
                    Changed = true;
                }
                InfoImage->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(Good))}));
                InfoImage->SetImageKindX(GI_Main::ikxCenter);
                InfoImage->SetImageKindY(GI_Main::ikyCenter);
                {
                    Types::TPoint visualCenter = InfoImage->GetVisualCenter();
                    Types::TPoint itemImageCenter = ItemImageCenter;
                    InfoImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                }
            }
            Text = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Text.", SysUtils::IntToStr(Good + 1)})));
            Text = pas::concat_wide({Text, u"\r\n", ([&] {
                pas::WideString int64ToStr = pas::wide_int64_to_str(System::Round(PlayerHoldShip->GetAverageCargoCost(Good)));
                pas::WideString localizedText = aConst::LocalizedText(u"FormShip.CostGoods"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<OldCost>"_w, std::move(int64ToStr));
            }())});
            if (([&] {
                pas::WideString cpp_string_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"sv))->GetText();
                pas::WideString cpp_string_4 = aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[Good].DisplayName), pas::view(aMyFunction::InfoNameColorTag));
                return cpp_string_3 != cpp_string_4;
            }())) {
                Changed = true;
            }
            if (pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"sv))->GetText() != Text) {
                Changed = true;
            }
            if (([&] {
                pas::WideString cpp_string_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"sv))->GetText();
                pas::WideString cpp_string_6 = pas::wide_int_to_str(PlayerHoldShip->CargoGoods[Good].Count);
                return cpp_string_5 != cpp_string_6;
            }())) {
                Changed = true;
            }
            if (([&] {
                pas::WideString cpp_string_7 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"sv))->GetText();
                pas::WideString cpp_string_8 = pas::wide_int_to_str(PlayerHoldShip->CargoGoods[Good].TotalCost);
                return cpp_string_7 != cpp_string_8;
            }())) {
                Changed = true;
            }
            {
                const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[Good].DisplayName), pas::view(aMyFunction::InfoNameColorTag));
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"sv));
                cpp_arg->SetText(wrapTextInColor);
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"sv))->SetText(Text);
            {
                const pas::WideString& intToStr = pas::wide_int_to_str(PlayerHoldShip->CargoGoods[Good].Count);
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"sv));
                cpp_arg_2->SetText(intToStr);
            }
            {
                const pas::WideString& intToStr_2 = pas::wide_int_to_str(PlayerHoldShip->CargoGoods[Good].TotalCost);
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"sv));
                cpp_arg_3->SetText(intToStr_2);
            }
            {
                GI_Image::TImageGI* EmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"sv));
                if (([&] {
                    pas::WideString cpp_string_9 = EmRace->GetImagePath();
                    pas::WideString cpp_string_10 = aConst::GetFactionEmblemPath(PlayerHoldShip->GetFactionNameKey());
                    return cpp_string_9 != cpp_string_10;
                }())) {
                    Changed = true;
                }
                EmRace->SetImagePath(aConst::GetFactionEmblemPath(PlayerHoldShip->GetFactionNameKey()));
                EmRace->SetImageKindX(GI_Main::ikxCenter);
                EmRace->SetImageKindY(GI_Main::ikyCenter);
            }
            GetByName(u"InfoDurable"sv)->Parent->Parent->SetActive(false);
            TfShip2::LayoutItemInfo(ItemInfoWindow, ItemNameLabel, ItemDescriptionLabel, true, true, 0);
            ItemSizeLabel->SetPosition(ClassesImports::Point(ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + ItemSizeLabelPosition.Y));
            ItemPriceLabel->SetPosition(ClassesImports::Point(ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + ItemPriceLabelPosition.Y));
            ItemRaceImage->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + ItemRaceImagePosition.Y));
            if (Changed) {
                GR_Main::SoundManager->PlaySound(u"Sound.ShipItemInfo"_wref.get());
            }
        }
    }

    void TfShip2::ShowStoredGoodsInfo(aItem::TGoods* Goods) {
        pas::WideString Text{};
        float AverageCost{};
        TfShip2::UpdateInfoHint(0, 0);
        if (ItemInfoHideTimer != nullptr) {
            CancelCallbackTimer(ItemInfoHideTimer);
            ItemInfoHideTimer = nullptr;
        }
        if (DisplayedItemKey != static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Goods))) {
            DisplayedItemKey = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Goods));
            if (SelectedHoldKind == phkEmpty) {
                RefreshActionPanels(phkGoods, static_cast<std::uint8_t>(Goods->ItemType), Goods->Quantity, Goods->Cost, nullptr, 0);
            } else {
                RefreshActionPanels(SelectedHoldKind, SelectedGoodsIndex, SelectedGoodsQuantity, SelectedGoodsCost, SelectedHoldItem, SelectedHoldOrigin);
            }
            GR_Main::SoundManager->PlaySound(u"Sound.ShipItemInfo"_wref.get());
            GetByName(u"PII"sv)->SetActive(true);
            GetByName(u"InfoHull"sv)->SetActive(false);
            {
                GI_Image::TImageGI* InfoImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoImage"sv));
                InfoImage->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(Goods->ItemType)}));
                InfoImage->SetImageKindX(GI_Main::ikxCenter);
                InfoImage->SetImageKindY(GI_Main::ikyCenter);
                {
                    Types::TPoint visualCenter = InfoImage->GetVisualCenter();
                    Types::TPoint itemImageCenter = ItemImageCenter;
                    InfoImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                }
            }
            Text = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Text.", SysUtils::IntToStr(Goods->ItemType + 1)})));
            if (Goods->Quantity > 0) {
                AverageCost = pas::real_divide(Goods->Cost, Goods->Quantity);
            } else {
                AverageCost = 0.0f;
            }
            Text = pas::concat_wide({Text, u"\r\n", ([&] {
                pas::WideString int64ToStr = pas::wide_int64_to_str(System::Round(AverageCost));
                pas::WideString localizedText = aConst::LocalizedText(u"FormShip.CostGoods"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<OldCost>"_w, std::move(int64ToStr));
            }())});
            {
                const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[Goods->ItemType].DisplayName), pas::view(aMyFunction::InfoNameColorTag));
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"sv));
                cpp_arg->SetText(wrapTextInColor);
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"sv))->SetText(Text);
            {
                const pas::WideString& intToStr = pas::wide_int_to_str(Goods->Quantity);
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoSize"sv));
                cpp_arg_2->SetText(intToStr);
            }
            {
                const pas::WideString& intToStr_2 = pas::wide_int_to_str(Goods->Cost);
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPrice"sv));
                cpp_arg_3->SetText(intToStr_2);
            }
            {
                GI_Image::TImageGI* EmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"EmRace"sv));
                EmRace->SetImagePath(aConst::GetFactionEmblemPath(PlayerHoldShip->GetFactionNameKey()));
                EmRace->SetImageKindX(GI_Main::ikxCenter);
                EmRace->SetImageKindY(GI_Main::ikyCenter);
            }
            GetByName(u"InfoDurable"sv)->Parent->Parent->SetActive(false);
            TfShip2::LayoutItemInfo(ItemInfoWindow, ItemNameLabel, ItemDescriptionLabel, true, true, 0);
            ItemSizeLabel->SetPosition(ClassesImports::Point(ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + ItemSizeLabelPosition.Y));
            ItemPriceLabel->SetPosition(ClassesImports::Point(ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + ItemPriceLabelPosition.Y));
            ItemRaceImage->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + ItemRaceImagePosition.Y));
        }
    }

    void TfShip2::RefreshMoneyWarning() {
        if (MoneyWarningVisible && (MoneyWarningTicks & 1) == 0) {
            GetByName(u"ADD_WarningMoney"sv)->SetActive(true);
            {
                GI_Label::TLabelGI* ADD_Money = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ADD_Money"sv));
                ADD_Money->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 128, 61));
                ADD_Money->SetText(pas::wide_int_to_str(aPlayer::GetPlayer()->Money));
                ADD_Money->SetActive(true);
            }
        } else {
            GetByName(u"ADD_WarningMoney"sv)->SetActive(false);
            GetByName(u"ADD_Money"sv)->SetActive(false);
        }
    }

    void TfShip2::StartMoneyWarning() {
        if (MoneyWarningTimer != nullptr) {
            CancelCallbackTimer(MoneyWarningTimer);
            MoneyWarningTimer = nullptr;
        }
        MoneyWarningTimer = ScheduleCallbackTimer(100, 100, pas::bind_method<&TfShip2::AdvanceMoneyWarning>(this), 0);
        MoneyWarningVisible = true;
        MoneyWarningTicks = 6;
        RefreshMoneyWarning();
    }

    void TfShip2::AdvanceMoneyWarning(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        --MoneyWarningTicks;
        if (MoneyWarningTicks <= 0) {
            if (MoneyWarningTimer != nullptr) {
                CancelCallbackTimer(MoneyWarningTimer);
                MoneyWarningTimer = nullptr;
            }
            MoneyWarningVisible = false;
        }
        RefreshMoneyWarning();
    }

    void TfShip2::ShowNoDropMessage(std::int32_t Code) {
        pas::WideString Text{};
        if (Code == 1) {
            Text = aConst::LocalizedColorText(u"FormShip.NoDrop"_wref.get());
        } else {
            Text = aConst::LocalizedColorText(pas::concat_wide({u"FormShip.NoDrop", EC_Str::IntToWideString(Code)}));
        }
        std::uint32_t Options = 1u;
        if (EC_Str::CountDelimitedPartsW(pas::view(Text), u"|"sv) > 1) {
            Options = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u"|"sv)));
            Text = EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u"|"sv);
        }
        GI_MessageBox::ShowMessageBoxGI(this, Text, Options, 0, 0, 0);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::GateMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::THull*>(static_cast<pas::Object*>(SelectedHoldItem)) != nullptr) {
            CloseGate();
        } else if (SelectedHoldKind != phkEmpty) {
            OpenGate();
        } else {
            CloseGate();
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::GateMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        CloseGate();
    }

    void TfShip2::OpenGate() {
        if (GateSlideTimer != nullptr) {
            CancelCallbackTimer(GateSlideTimer);
            GateSlideTimer = nullptr;
        }
        GateSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideGateTimer>(this), 1);
        pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"GateAnim"sv))->RestartPlayback();
    }

    void TfShip2::CloseGate() {
        if (GateSlideTimer != nullptr) {
            CancelCallbackTimer(GateSlideTimer);
            GateSlideTimer = nullptr;
        }
        GateSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideGateTimer>(this), 2);
    }

    void TfShip2::SlideGateTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (UserData == 1) {
            GateSlideOffset += 2;
            if (GR_Main::GiScalePixels(20) <= GateSlideOffset) {
                GateSlideOffset = GR_Main::GiScalePixels(20);
                if (GateSlideTimer != nullptr) {
                    CancelCallbackTimer(GateSlideTimer);
                    GateSlideTimer = nullptr;
                }
            }
        } else {
            GateSlideOffset -= 2;
            if (GateSlideOffset <= 0) {
                GateSlideOffset = 0;
                if (GateSlideTimer != nullptr) {
                    CancelCallbackTimer(GateSlideTimer);
                    GateSlideTimer = nullptr;
                }
            }
        }
        {
            GI_MessageLoop::TObjectGI* GateLeft = GetByName(u"GateLeft"sv);
            GateLeft->SetPosition(ClassesImports::Point(GateLeftRestLeft - GateSlideOffset, GateLeft->LocalPosition.Y));
        }
        {
            GI_MessageLoop::TObjectGI* GateRight = GetByName(u"GateRight"sv);
            GateRight->SetPosition(ClassesImports::Point(GateRightRestLeft + GateSlideOffset, GateRight->LocalPosition.Y));
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::GateMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        DropSelectedOutside(Sender, KeyState, Point);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::UseMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (SelectedHoldKind != phkEmpty) {
            OpenUsePanel();
        } else {
            CloseUsePanel();
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::UseMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        CloseUsePanel();
    }

    void TfShip2::OpenUsePanel() {
        if (UseSlideTimer != nullptr) {
            CancelCallbackTimer(UseSlideTimer);
            UseSlideTimer = nullptr;
        }
        UseSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideUseTimer>(this), 1);
        pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"UseAnim"sv))->RestartPlayback();
    }

    void TfShip2::CloseUsePanel() {
        if (UseSlideTimer != nullptr) {
            CancelCallbackTimer(UseSlideTimer);
            UseSlideTimer = nullptr;
        }
        UseSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideUseTimer>(this), 2);
    }

    void TfShip2::SlideUseTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (UserData == 1) {
            UseSlideOffset += 2;
            if (GR_Main::GiScalePixels(20) <= UseSlideOffset) {
                UseSlideOffset = GR_Main::GiScalePixels(20);
                if (UseSlideTimer != nullptr) {
                    CancelCallbackTimer(UseSlideTimer);
                    UseSlideTimer = nullptr;
                }
            }
        } else {
            UseSlideOffset -= 2;
            if (UseSlideOffset <= 0) {
                UseSlideOffset = 0;
                if (UseSlideTimer != nullptr) {
                    CancelCallbackTimer(UseSlideTimer);
                    UseSlideTimer = nullptr;
                }
            }
        }
        {
            GI_MessageLoop::TObjectGI* UseLeft = GetByName(u"UseLeft"sv);
            UseLeft->SetPosition(ClassesImports::Point(UseLeftRestLeft - UseSlideOffset, UseLeft->LocalPosition.Y));
        }
        {
            GI_MessageLoop::TObjectGI* UseRight = GetByName(u"UseRight"sv);
            UseRight->SetPosition(ClassesImports::Point(UseRightRestLeft + UseSlideOffset, UseRight->LocalPosition.Y));
        }
    }

    void TfShip2::OpenUseSidePanel() {
        if (UsePanelSlideTimer != nullptr) {
            CancelCallbackTimer(UsePanelSlideTimer);
            UsePanelSlideTimer = nullptr;
        }
        if (TfShip2::IsHoldNormalShip()) {
            UsePanelSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideUsePanelTimer>(this), 1);
        }
    }

    void TfShip2::CloseUseSidePanel() {
        if (UsePanelSlideTimer != nullptr) {
            CancelCallbackTimer(UsePanelSlideTimer);
            UsePanelSlideTimer = nullptr;
        }
        UsePanelSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideUsePanelTimer>(this), 2);
    }

    void TfShip2::SlideUsePanelTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"UsePanel"sv));
        if (UserData == 1) {
            UsePanelSlideOffset += 6;
            if (UsePanelSlideOffset >= Panel->ClientSize.X) {
                UsePanelSlideOffset = Panel->ClientSize.X;
                if (UsePanelSlideTimer != nullptr) {
                    CancelCallbackTimer(UsePanelSlideTimer);
                    UsePanelSlideTimer = nullptr;
                }
            }
        } else {
            UsePanelSlideOffset -= 6;
            if (UsePanelSlideOffset <= 0) {
                UsePanelSlideOffset = 0;
                if (UsePanelSlideTimer != nullptr) {
                    CancelCallbackTimer(UsePanelSlideTimer);
                    UsePanelSlideTimer = nullptr;
                }
            }
        }
        Panel->SetPosition(ClassesImports::Point(Panel->ClientSize.X - UsePanelSlideOffset, Panel->LocalPosition.Y));
    }

    void TfShip2::OpenSpecialSlot1() {
        if (SpecialSlot1Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot1Timer);
            SpecialSlot1Timer = nullptr;
        }
        SpecialSlot1Timer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::AnimateSpecialSlot1>(this), 1);
        reinterpret_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot1_Anim"sv))->RestartPlayback();
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Slot1_But"sv))->UpCallback = pas::bind_method<&TfShip2::SpecialSlot1Clicked>(this);
    }

    void TfShip2::CloseSpecialSlot1() {
        if (SpecialSlot1Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot1Timer);
            SpecialSlot1Timer = nullptr;
        }
        SpecialSlot1Timer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::AnimateSpecialSlot1>(this), 2);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Slot1_But"sv))->UpCallback = nullptr;
    }

    void TfShip2::AnimateSpecialSlot1(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_GAI::TgaiGI* Animation = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot1_Anim"sv));
        if (UserData == 1) {
            Animation->SetSequenceFrame(std::min<std::int32_t>(Animation->SequenceFrame + 1, Animation->SequenceFrameCount - 1));
            if (Animation->SequenceFrameCount - 1 == Animation->SequenceFrame) {
                Animation->StopAutoPlayback();
                if (SpecialSlot1Timer != nullptr) {
                    CancelCallbackTimer(SpecialSlot1Timer);
                    SpecialSlot1Timer = nullptr;
                }
            }
        } else {
            Animation->SetSequenceFrame(std::max<std::int32_t>(Animation->SequenceFrame - 1, 0));
            if (Animation->SequenceFrame == 0) {
                Animation->StopAutoPlayback();
                if (SpecialSlot1Timer != nullptr) {
                    CancelCallbackTimer(SpecialSlot1Timer);
                    SpecialSlot1Timer = nullptr;
                }
            }
        }
    }

    void TfShip2::OpenSpecialSlot2() {
        if (SpecialSlot2Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot2Timer);
            SpecialSlot2Timer = nullptr;
        }
        SpecialSlot2Timer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::AnimateSpecialSlot2>(this), 1);
        reinterpret_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot2_Anim"sv))->RestartPlayback();
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Slot2_But"sv))->UpCallback = pas::bind_method<&TfShip2::SpecialSlot2Clicked>(this);
    }

    void TfShip2::CloseSpecialSlot2() {
        if (SpecialSlot2Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot2Timer);
            SpecialSlot2Timer = nullptr;
        }
        SpecialSlot2Timer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::AnimateSpecialSlot2>(this), 2);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Slot2_But"sv))->UpCallback = nullptr;
    }

    void TfShip2::AnimateSpecialSlot2(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_GAI::TgaiGI* Animation = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot2_Anim"sv));
        if (UserData == 1) {
            Animation->SetSequenceFrame(std::min<std::int32_t>(Animation->SequenceFrame + 1, Animation->SequenceFrameCount - 1));
            if (Animation->SequenceFrameCount - 1 == Animation->SequenceFrame) {
                Animation->StopAutoPlayback();
                if (SpecialSlot2Timer != nullptr) {
                    CancelCallbackTimer(SpecialSlot2Timer);
                    SpecialSlot2Timer = nullptr;
                }
            }
        } else {
            Animation->SetSequenceFrame(std::max<std::int32_t>(Animation->SequenceFrame - 1, 0));
            if (Animation->SequenceFrame == 0) {
                Animation->StopAutoPlayback();
                if (SpecialSlot2Timer != nullptr) {
                    CancelCallbackTimer(SpecialSlot2Timer);
                    SpecialSlot2Timer = nullptr;
                }
            }
        }
    }

    void TfShip2::OpenSpecialSlot3() {
        if (SpecialSlot3Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot3Timer);
            SpecialSlot3Timer = nullptr;
        }
        SpecialSlot3Timer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::AnimateSpecialSlot3>(this), 1);
        reinterpret_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot3_Anim"sv))->RestartPlayback();
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Slot3_But"sv))->UpCallback = pas::bind_method<&TfShip2::SpecialSlot3Clicked>(this);
    }

    void TfShip2::CloseSpecialSlot3() {
        if (SpecialSlot3Timer != nullptr) {
            CancelCallbackTimer(SpecialSlot3Timer);
            SpecialSlot3Timer = nullptr;
        }
        SpecialSlot3Timer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::AnimateSpecialSlot3>(this), 2);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Slot3_But"sv))->UpCallback = nullptr;
    }

    void TfShip2::AnimateSpecialSlot3(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_GAI::TgaiGI* Animation = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"SC_Slot3_Anim"sv));
        if (UserData == 1) {
            Animation->SetSequenceFrame(std::min<std::int32_t>(Animation->SequenceFrame + 1, Animation->SequenceFrameCount - 1));
            if (Animation->SequenceFrameCount - 1 == Animation->SequenceFrame) {
                Animation->StopAutoPlayback();
                if (SpecialSlot3Timer != nullptr) {
                    CancelCallbackTimer(SpecialSlot3Timer);
                    SpecialSlot3Timer = nullptr;
                }
            }
        } else {
            Animation->SetSequenceFrame(std::max<std::int32_t>(Animation->SequenceFrame - 1, 0));
            if (Animation->SequenceFrame == 0) {
                Animation->StopAutoPlayback();
                if (SpecialSlot3Timer != nullptr) {
                    CancelCallbackTimer(SpecialSlot3Timer);
                    SpecialSlot3Timer = nullptr;
                }
            }
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::StorageDownClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (StorageSlideTimer != nullptr) {
            CancelCallbackTimer(StorageSlideTimer);
            StorageSlideTimer = nullptr;
        }
        StorageSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideStorageTimer>(this), 1);
        StorageUpButton->SetActive(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Down"sv))->SetActive(false);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::StorageUpClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (StorageSlideTimer != nullptr) {
            CancelCallbackTimer(StorageSlideTimer);
            StorageSlideTimer = nullptr;
        }
        StorageSlideTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfShip2::SlideStorageTimer>(this), 2);
        StorageUpButton->SetActive(false);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"SC_Down"sv))->SetActive(true);
    }

    void TfShip2::SlideStorageTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"SC_Storage_Panel"sv));
        if (UserData == 1) {
            StorageSlideOffset += StorageImageCount;
            if (StoragePanelRestTop - StoragePanelSlideHeight <= StorageSlideOffset) {
                StorageSlideOffset = StoragePanelRestTop - StoragePanelSlideHeight;
                if (StorageSlideTimer != nullptr) {
                    CancelCallbackTimer(StorageSlideTimer);
                    StorageSlideTimer = nullptr;
                }
            }
        } else {
            StorageSlideOffset -= StorageImageCount;
            if (StorageSlideOffset <= 0) {
                StorageSlideOffset = 0;
                if (StorageSlideTimer != nullptr) {
                    CancelCallbackTimer(StorageSlideTimer);
                    StorageSlideTimer = nullptr;
                }
            }
        }
        Panel->SetPosition(ClassesImports::Point(Panel->LocalPosition.X, StoragePanelSlideHeight + StorageSlideOffset));
        GR_Main::PostMouseMoveMessage();
    }

    void TfShip2::RefreshStorageView() {
        std::int32_t I{};
        std::int32_t Index{};
        aPlayer::PStorageEntry Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StorageImageCount - 1); cpp_range.next(I); ) {
            Index = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(TfShip2::GetLocalStorageOwner(), StorageFirstSlot + I);
            if (Index < 0) {
                GI_Image::TImageGI* cpp_with = StorageImages[I];
                cpp_with->SetImagePath(pas::WideString());
                cpp_with->LeftButtonUpCallback = pas::bind_method<&TfShip2::StorageItemMouseUp>(this);
            } else {
                Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, Index);
                {
                    GI_Image::TImageGI* cpp_with_2 = StorageImages[I];
                    if (pas::class_cast_if<aItem::TGoods*>(Entry->Item) != nullptr) {
                        cpp_with_2->SetImagePath(pas::concat_wide({u"GI,", Entry->Item->GetBitmapResourceName()}));
                    } else {
                        cpp_with_2->SetImagePath(pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(Entry->Item), u"s"}));
                        if (Entry->Item == Globals::ScriptUseItem) {
                            cpp_with_2->SetImagePath(pas::WideString());
                        }
                    }
                    cpp_with_2->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_2->SetImageKindY(GI_Main::ikyCenter);
                    cpp_with_2->LeftButtonUpCallback = pas::bind_method<&TfShip2::StorageItemMouseUp>(this);
                }
            }
        }
        {
            GI_GraphButton::TGraphButtonGI* Storage_Up = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Storage_Up"sv));
            Storage_Up->SetDisabled(StorageFirstSlot <= 0);
            Storage_Up->UpCallback = pas::bind_method<&TfShip2::ScrollStorageUp>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Storage_Down = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Storage_Down"sv));
            Storage_Down->SetDisabled((StorageImageCount - 3 + StorageFirstSlot) / 3 * 3 > std::max<std::int32_t>(0, aPlayer::GetPlayer()->GetStorageSlotExtent(TfShip2::GetLocalStorageOwner()) / 3 * 3 - 3));
            Storage_Down->UpCallback = pas::bind_method<&TfShip2::ScrollStorageDown>(this);
        }
    }

    void TfShip2::ScrollStorageUp(GI_MessageLoop::TObjectGI* Sender) {
        StorageFirstSlot = std::max<std::int32_t>(0, StorageFirstSlot - 3);
        RefreshStorageView();
    }

    void TfShip2::ScrollStorageDown(GI_MessageLoop::TObjectGI* Sender) {
        StorageFirstSlot = (StorageFirstSlot + 3) / 3 * 3;
        std::int32_t Limit = std::max<std::int32_t>(0, aPlayer::GetPlayer()->GetStorageSlotExtent(TfShip2::GetLocalStorageOwner()) / 3 * 3 - 3);
        if (StorageFirstSlot > Limit) {
            StorageFirstSlot = Limit;
        }
        RefreshStorageView();
    }

    void TfShip2::StorageItemMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        aPlayer::PStorageEntry Entry{};
        std::int32_t Count{};
        aItem::TItem* Item{};
        if (!StorageUpButton->Active) {
            StorageUpButton->SetActive(true);
        }
        if (pas::is_one_of<phkEquipment, phkArtefact>(SelectedHoldKind) && SelectedHoldItem != nullptr && SelectedHoldItem->NoDropFlag > 0 && SelectedHoldOrigin != 1) {
            ShowNoDropMessage(SelectedHoldItem->NoDropFlag);
            return;
        }
        std::int32_t Slot = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName)) + StorageFirstSlot;
        std::int32_t Index = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(TfShip2::GetLocalStorageOwner(), Slot);
        if (Index >= 0 && SelectedHoldKind == phkEmpty && aPlayer::GetPlayer() != PlayerHoldShip && pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, Index)->Item->NoDropFlag > 0) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(510);
        if (SelectedHoldKind != phkEmpty && SelectedHoldOrigin == 0) {
            fShip2::RemoveEmptyPlayerHoldSlots();
        }
        if (SelectedHoldKind == phkEquipment && pas::class_cast_if<aItem::TCountableItem*>(SelectedHoldItem) != nullptr && static_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount >= 1) {
            Count = static_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount;
            if (static_cast<std::uint8_t>(RemoteHoldVisible ^ 1) && SelectedHoldOrigin == 0 && Count > 1) {
                if (([&] {
                    std::int32_t count = Count;
                    std::int32_t count_2 = Count;
                    std::int32_t count_3 = Count;
                    const pas::WideString& formatText1 = ([&] {
                        pas::WideString lowerCaseWideString = EC_Str::LowerCaseWideString(aItem::GetStackableItemName(SelectedHoldItem));
                        pas::WideString localizedText = aConst::LocalizedText(u"FormShip.StorageItem"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString));
                    }());
                    const pas::WideString& cpp_arg = pas::concat_wide({u"GI,", fEquipmentShop::GetShopItemIconName(SelectedHoldItem), u"s"});
                    GI_MessageLoop::TMessageLoopGI* self = this;
                    return fCount2::ShowCountDialog(self, cpp_arg, formatText1, 0, count, count_2, 0.0f, count_3, 0, Count);
                }()) != 1) {
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(515);
                    return;
                }
            }
            if (Count < 1 || reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount < Count) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(515);
                return;
            }
            Item = pas::checked_cast<aItem::TCountableItem*>(SelectedHoldItem)->Split(Count);
            aPlayer::GetPlayer()->MergeItemIntoPlayerStorage(Item, TfShip2::GetLocalStorageOwner(), Slot);
            if (reinterpret_cast<aItem::TCountableItem*>(SelectedHoldItem)->StackCount < 1) {
                pas::free(SelectedHoldItem);
                SelectedHoldItem = nullptr;
                SelectedHoldKind = phkEmpty;
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(511);
                RefreshShipView();
                UpdateActionCursor(true);
            }
            ReturnSelectedHoldEntry();
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
        } else if (SelectedHoldKind == phkEquipment || SelectedHoldKind == phkArtefact) {
            if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) {
                if (PlayerHoldShip->NeedsEquipmentType(SelectedHoldItem->ItemType)) {
                    {
                        const pas::WideString& formatText1_2 = ([&] {
                            pas::WideString removeTextTagsW = EC_Str::RemoveTextTagsW(SelectedHoldItem->GetDisplayName());
                            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.RuinMoveItemInvalid"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW));
                        }());
                        GI_MessageLoop::TMessageLoopGI* self_2 = this;
                        GI_MessageBox::ShowMessageBoxGI(self_2, formatText1_2, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                    }
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(515);
                    return;
                }
            } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr && (SelectedHoldItem->ItemType == aConst::t_FuelTanks || SelectedHoldItem->ItemType == aConst::t_Engine) && PlayerHoldShip->NeedsEquipmentType(SelectedHoldItem->ItemType)) {
                {
                    const pas::WideString& formatText1_3 = ([&] {
                        pas::WideString removeTextTagsW_2 = EC_Str::RemoveTextTagsW(SelectedHoldItem->GetDisplayName());
                        pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.TrancMoveItemInvalid"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(removeTextTagsW_2));
                    }());
                    GI_MessageLoop::TMessageLoopGI* self_3 = this;
                    GI_MessageBox::ShowMessageBoxGI(self_3, formatText1_3, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(515);
                return;
            }
            if (pas::class_cast_if<aItem::THull*>(SelectedHoldItem) != nullptr && SelectedHoldOrigin == 0) {
                if (Index >= 0) {
                    Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, Index);
                    if (!(pas::class_cast_if<aItem::THull*>(Entry->Item) != nullptr)) {
                        aGalaxy::Galaxy->PrimeIntegrityChecksum1(510);
                        return;
                    }
                    pas::list_delete(aPlayer::GetPlayer()->StorageEntries, Index);
                    SelectedHoldKind = phkEquipment;
                    SelectedHoldOrigin = 1;
                    SelectedHoldSlot = Slot;
                    SelectedHoldUsesDisplayOrder = false;
                    SelectedHoldItem = Entry->Item;
                    Entry->LocationOwner = nullptr;
                    Entry->Item = nullptr;
                    pas::dispose(Entry);
                    HullMouseDown(nullptr, 0u, ClassesImports::Point(0, 0));
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(512);
                return;
            }
            if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(SelectedHoldItem)) {
                pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship))->OwnerShip = nullptr;
            }
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            SelectedHoldKind = phkEmpty;
            aPlayer::GetPlayer()->AddItemToPlayerStorage(SelectedHoldItem, TfShip2::GetLocalStorageOwner(), Slot);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(513);
            RefreshShipView();
            UpdateActionCursor(true);
        } else if (SelectedHoldKind == phkGoods) {
            Count = SelectedGoodsQuantity;
            if (static_cast<std::uint8_t>(RemoteHoldVisible ^ 1) && SelectedHoldOrigin == 0 && Count > 1) {
                if (([&] {
                    std::int32_t selectedGoodsQuantity = SelectedGoodsQuantity;
                    std::int32_t selectedGoodsQuantity_2 = SelectedGoodsQuantity;
                    std::int32_t count_4 = Count;
                    const pas::WideString& formatText1_4 = ([&] {
                        pas::WideString lowerCaseWideString_2 = EC_Str::LowerCaseWideString(aItem::GetStackableItemTypeName(static_cast<aConst::TItemType>(SelectedGoodsIndex)));
                        pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.StorageItem"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString_2));
                    }());
                    const pas::WideString& cpp_arg_2 = pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(SelectedGoodsIndex))});
                    GI_MessageLoop::TMessageLoopGI* self_4 = this;
                    return fCount2::ShowCountDialog(self_4, cpp_arg_2, formatText1_4, 0, selectedGoodsQuantity, selectedGoodsQuantity_2, 0.0f, count_4, 0, Count);
                }()) != 1) {
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(515);
                    return;
                }
            }
            if (Count < 1 || Count > SelectedGoodsQuantity) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(515);
                return;
            }
            GR_Main::SoundManager->PlaySound(u"Sound.SlotPut"_wref.get());
            aPlayer::GetPlayer()->AddGoodsToPlayerStorage(SelectedGoodsIndex, Count, System::Round(pas::real_divide(Count, SelectedGoodsQuantity) * SelectedGoodsCost), TfShip2::GetLocalStorageOwner(), Slot);
            SelectedGoodsCost -= System::Round(pas::real_divide(Count, SelectedGoodsQuantity) * SelectedGoodsCost);
            SelectedGoodsQuantity -= Count;
            if (SelectedGoodsQuantity < 1) {
                SelectedHoldKind = phkEmpty;
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(513);
                RefreshShipView();
                UpdateActionCursor(true);
            }
            ReturnSelectedHoldEntry();
        } else if (Index >= 0 && SelectedHoldKind == phkEmpty) {
            Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, Index);
            pas::list_delete(aPlayer::GetPlayer()->StorageEntries, Index);
            if (pas::class_cast_if<aItem::TGoods*>(Entry->Item) != nullptr) {
                SelectedHoldKind = phkGoods;
                SelectedHoldOrigin = 1;
                SelectedHoldSlot = Slot;
                SelectedHoldUsesDisplayOrder = false;
                SelectedGoodsIndex = static_cast<std::uint8_t>(Entry->Item->ItemType);
                SelectedGoodsQuantity = pas::checked_cast<aItem::TGoods*>(Entry->Item)->Quantity;
                SelectedGoodsCost = pas::checked_cast<aItem::TGoods*>(Entry->Item)->Cost;
                pas::free(Entry->Item);
            } else if (pas::class_cast_if<aItem::TArtefact*>(Entry->Item) != nullptr) {
                SelectedHoldKind = phkArtefact;
                SelectedHoldOrigin = 1;
                SelectedHoldSlot = Slot;
                SelectedHoldUsesDisplayOrder = false;
                SelectedHoldItem = Entry->Item;
            } else {
                SelectedHoldKind = phkEquipment;
                SelectedHoldOrigin = 1;
                SelectedHoldSlot = Slot;
                SelectedHoldUsesDisplayOrder = false;
                SelectedHoldItem = Entry->Item;
            }
            Entry->LocationOwner = nullptr;
            Entry->Item = nullptr;
            pas::dispose(Entry);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(514);
            UpdateActionCursor(true);
            PlayerHoldShip->ScriptItemsAct(aConst::satOnReEnteringForm, nullptr, nullptr, 0);
            RefreshShipView();
            GR_Main::SoundManager->PlaySound(u"Sound.SlotGet"_wref.get());
            return;
        } else {
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(515);
            return;
        }
        aPlayer::GetPlayer()->CloseVacantStorageSlot(TfShip2::GetLocalStorageOwner(), Slot + 1);
        aGalaxy::Galaxy->CheckIntegrityChecksum1(515);
        if (!RemoteHoldVisible) {
            ShipStateChanged = true;
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    GI_MessageLoop::TObjectGI* TfShip2::CreateShipInfoImage(GI_Label::TLabelGI* Sender, EC_CacheFont::PFontObjectEC Item) {
        GI_MessageLoop::TObjectGI* Result = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Sender);
        {
            GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(Result);
            cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip2.", GR_Main::GiResourceSuffix(), u"AI_", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->ObjectId)))}));
            cpp_with->SetImageKindX(GI_Main::ikxLeft);
            return Result;
        }
    }

    void TfShip2::BuildAdditionalInfoPanel() {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel{};
        std::int32_t Height{};
        std::int32_t I{};
        std::int32_t Kind{};
        std::int32_t Turn{};
        std::uint8_t ProgramIndex{};
        pas::WideString Name{};
        pas::WideString SeriesName{};
        pas::WideString TypeName{};
        pas::WideString CountText{};
        pas::WideString Text{};
        pas::WideString ChargesText{};
        pas::WideString Description{};
        pas::WideString Title{};
        std::uint8_t ColorIndex{};
        aShip::PCustomShipInfo Info{};
        EC_BlockPar::TBlockParEC* Block{};
        // Nested helper captures the panel, accumulated height and Self.
        auto AddLine = [&](std::int32_t Icon, pas::WideString Text, pas::WideString Hint, std::int32_t Data) -> void {
            GI_Label::TLabelGI* LabelControl = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
            LabelControl->SetFontName(GlobalsV::SmallFontName);
            LabelControl->SetPosition(ClassesImports::Point(0, Height));
            LabelControl->SetSize(ClassesImports::Point(Panel->ClientSize.X, 1));
            LabelControl->SetTextAlignX(GI_Main::taxLeft);
            LabelControl->SetTextAlignY(GI_Main::tayAuto);
            LabelControl->SetWordWrapEnabled(true);
            if (GR_Main::GiResourceVariant() == 1) {
                LabelControl->SetText(pas::concat_wide_reverse({EC_Str::ReplaceAllWideString(Text, u"<br>"_wref.get(), u"\r\n"sv), static_cast<pas::WideString>(pas::concat_ansi({"<Object=", SysUtils::IntToStr(Icon), ",", SysUtils::IntToStr(21), ",", SysUtils::IntToStr(17), ",0>"}))}));
            } else {
                LabelControl->SetText(pas::concat_wide_reverse({EC_Str::ReplaceAllWideString(Text, u"<br>"_wref.get(), u"\r\n"sv), static_cast<pas::WideString>(pas::concat_ansi({"<Object=", SysUtils::IntToStr(Icon), ",", SysUtils::IntToStr(25), ",", SysUtils::IntToStr(20), ",0>"}))}));
            }
            LabelControl->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            LabelControl->SetTextAlignY(GI_Main::tayCenterEx);
            LabelControl->SetPositionModeW(true);
            LabelControl->CreateEmbeddedControl = pas::bind_static_method<&TfShip2::CreateShipInfoImage>(this);
            LabelControl->MouseEnterCallback = pas::bind_method<&TfShip2::ShowShipPropertyInfo>(this);
            LabelControl->MouseLeaveCallback = pas::bind_method<&TfShip2::HideShipPropertyInfo>(this);
            LabelControl->UserValue = -1;
            LabelControl->UserIndex = Icon;
            LabelControl->UserData = Data;
            LabelControl->HelpText = std::move(Hint);
            Height += LabelControl->ClientSize.Y;
            {
                std::int32_t lineHeight = LabelControl->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = Panel->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(lineHeight);
            }
        };
        // Nested in BuildAdditionalInfoPanel; does not access its parent frame.
        auto GetShipInfoColor = [&](std::uint8_t ColorIndex) -> pas::WideString {
            pas::WideString Result{};
            switch (ColorIndex) {
                case 0: return u"<color=255,0,0>"_w;
                case 1: return u"<color=0,128,255>"_w;
                case 2: return u"<color=0,255,0>"_w;
                default: return Result;
            }
        };
        Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelAddInfo"sv));
        Panel->FreeOwnedChildren();
        Panel->SetScrollOffset(ClassesImports::Point(0, 0));
        Panel->SetDragScrollingEnabled(true);
        Height = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 24); cpp_range.next(I); ) {
            if (PlayerHoldShip->IsHealthEffectActive(I)) {
                if (I < 13) {
                    Kind = 1;
                } else {
                    Kind = 2;
                }
                Name = pas::concat_wide({aConst::CaptainHealthDefinitions[I].Name, u"~", aConst::CaptainHealthDefinitions[I].Text});
                if (PlayerHoldShip->CountActiveArtefacts(aConst::t_ArtBio) > 0) {
                    Turn = PlayerHoldShip->CaptainHealth[I].ExpireTurn;
                    if (Kind == 1) {
                        Name = pas::concat_wide({Name, u"\r\n", ([&] {
                            pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(Turn);
                            pas::WideString localizedText = aConst::LocalizedText(u"Illness.Illness.EndDate"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate));
                        }())});
                    } else {
                        Name = pas::concat_wide({Name, u"\r\n", ([&] {
                            pas::WideString formatTurnDate_2 = aGalaxy::Galaxy->FormatTurnDate(Turn);
                            pas::WideString localizedText_2 = aConst::LocalizedText(u"Illness.Stimulant.EndDate"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_2));
                        }())});
                    }
                }
                AddLine(Kind, aConst::CaptainHealthDefinitions[I].Name, Name, 0);
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 1); cpp_range_2.next(I); ) {
            if (PlayerHoldShip->RadiationHealth[I].Progress > 0.0L) {
                Name = pas::concat_wide({aConst::RadiationHealthDefinitions[I].Name, u"~", aConst::RadiationHealthDefinitions[I].Text});
                if (PlayerHoldShip->CountActiveArtefacts(aConst::t_ArtBio) > 0) {
                    Turn = System::Round(1.0E+2L - 1.0E+2L * PlayerHoldShip->RadiationHealth[1].Progress);
                    Name = pas::concat_wide({Name, u"\r\n", ([&] {
                        pas::WideString cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Turn), "%"}));
                        pas::WideString localizedText_3 = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Illness.ExtraIllness.", SysUtils::IntToStr(I), ".TextEx"})));
                        return aMyFunction::FormatText1(std::move(localizedText_3), u"<color=255,240,100>"_w, u"<Percent>"_w, std::move(cpp_arg));
                    }())});
                }
                AddLine(1, aConst::RadiationHealthDefinitions[I].Name, Name, 0);
            }
        }
        if (aPlayer::GetPlayer() == PlayerHoldShip) {
            if (aPlayer::GetPlayer()->MedicalPolicyTicks > 0) {
                pas::WideString cpp_arg_2 = pas::concat_wide({aConst::LocalizedText(u"ShipInfo.AddInfo.MedPolicy.Name"_wref.get()), u"~", ([&] {
                    pas::WideString formatTurnDate_3 = aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + aPlayer::GetPlayer()->MedicalPolicyTicks);
                    pas::WideString localizedText_4 = aConst::LocalizedText(u"ShipInfo.AddInfo.MedPolicy.Text"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_4), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_3));
                }())});
                pas::WideString localizedText_5 = aConst::LocalizedText(u"ShipInfo.AddInfo.MedPolicy.Name"_wref.get());
                AddLine(0, std::move(localizedText_5), std::move(cpp_arg_2), 0);
            }
            if (aPlayer::GetPlayer()->DebtAmount > 0) {
                Name = aConst::LocalizedText(u"ShipInfo.AddInfo.DebtInfo.Name"_wref.get());
                AddLine(0, Name, pas::concat_wide({Name, u"~", ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->DebtAmount);
                    pas::WideString formatTurnDate_4 = aGalaxy::Galaxy->FormatTurnDate(aPlayer::GetPlayer()->DebtDueTurn);
                    pas::WideString localizedText_6 = aConst::LocalizedText(u"ShipInfo.AddInfo.DebtInfo.Text"_wref.get());
                    return aMyFunction::FormatText2(std::move(localizedText_6), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr), u"<Date>"_w, std::move(formatTurnDate_4));
                }())}), 0);
            }
            if (aPlayer::GetPlayer()->DepositAmount > 0) {
                Name = aConst::LocalizedText(u"ShipInfo.AddInfo.DepositInfo.Name"_wref.get());
                Text = aConst::LocalizedText(u"ShipInfo.AddInfo.DepositInfo.Text"_wref.get());
                aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(aPlayer::GetPlayer()->DepositStartTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->DepositAmount), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, static_cast<pas::WideString>(SysUtilsImports::FloatToStrF(aPlayer::GetPlayer()->DepositInterestRate, SysUtilsImports::ffFixed, 1, 1)), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Sum>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ComputeDepositAccruedValue()), u"<color=255,240,100>"_w);
                AddLine(0, Name, pas::concat_wide({Name, u"~", Text}), 0);
            }
            if (aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
                Name = aConst::LocalizedText(u"ShipInfo.AddInfo.PirateLicenseInfo.Name"_wref.get());
                Text = aConst::LocalizedText(u"ShipInfo.AddInfo.PirateLicenseInfo.Text"_wref.get());
                aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn + aPlayer::GetPlayer()->PirateLicenseTicks), u"<color=255,240,100>"_w);
                AddLine(0, Name, pas::concat_wide({Name, u"~", Text}), 0);
            }
            if (aPlayer::GetPlayer()->ChameleonActive || (static_cast<void>(aPlayer::GetPlayer()), aShip::TShip::HasPlayerChameleonCharges())) {
                Name = aConst::LocalizedText(u"ShipInfo.AddInfo.Chameleon.Name"_wref.get());
                Text = pas::WideString();
                if (aPlayer::GetPlayer()->ChameleonActive) {
                    SeriesName = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"ShipType.Dominator.", aConst::DominatorSeriesNames[aPlayer::GetPlayer()->ChameleonSeries], u".0"}));
                    TypeName = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"ShipType.Dominator.", aConst::DominatorSeriesNames[aPlayer::GetPlayer()->ChameleonSeries], u".", pas::wide_int_to_str(static_cast<std::int32_t>(aPlayer::GetPlayer()->ChameleonVisualType))}));
                    CountText = pas::wide_int_to_str(aPlayer::GetPlayer()->ChameleonDisplayCount);
                    Text = aMyFunction::FormatText3(aConst::LocalizedText(u"ShipInfo.AddInfo.Chameleon.Text"_wref.get()), u"<color=255,240,100>"_w, u"<Series>"_w, SeriesName, u"<Type>"_w, TypeName, u"<Count>"_w, CountText);
                }
                if (static_cast<void>(aPlayer::GetPlayer()), aShip::TShip::HasPlayerChameleonCharges()) {
                    if (Text.length() > 0) {
                        Text = pas::concat_wide({Text, u"\r\n"});
                    }
                    Text = pas::concat_wide_reverse({aConst::LocalizedText(u"ShipInfo.AddInfo.Chameleon.Charge"_wref.get()), Text});
                    ChargesText = pas::WideString();
                    for (auto cpp_range_3 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(2)); cpp_range_3.next(ColorIndex); ) {
                        if (aPlayer::GetPlayer()->ChameleonCharges[ColorIndex] > 0) {
                            SeriesName = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"ShipType.Dominator.", aConst::DominatorSeriesNames[ColorIndex], u".0"}));
                            CountText = ([&] {
                                pas::WideString intToStr_2 = pas::wide_int_to_str(aPlayer::GetPlayer()->ChameleonCharges[ColorIndex]);
                                pas::WideString localizedText_7 = aConst::LocalizedText(u"ShipInfo.AddInfo.Chameleon.Count"_wref.get());
                                return aMyFunction::FormatText1(std::move(localizedText_7), u"<color=255,240,100>"_w, u"<Count>"_w, std::move(intToStr_2));
                            }());
                            if (ChargesText.length() > 0) {
                                ChargesText = pas::concat_wide({ChargesText, u"\r\n"});
                            }
                            ChargesText = pas::concat_wide({ChargesText, aMyFunction::WrapTextInColor(pas::view(SeriesName), pas::view(GetShipInfoColor(ColorIndex))), u" - ", CountText});
                        }
                    }
                    Text = pas::concat_wide({Text, u"\r\n", ChargesText});
                }
                AddLine(0, Name, pas::concat_wide({Name, u"~", Text}), 0);
            }
            for (ProgramIndex = static_cast<std::uint8_t>(0); ProgramIndex <= static_cast<std::uint8_t>(11); ++ProgramIndex) {
                if (aPlayer::GetPlayer()->ProgramCounts[ProgramIndex] > 0) {
                    pas::WideString cpp_arg_3 = pas::concat_wide({(static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(ProgramIndex)), u"~", aPlayer::GetPlayer()->GetProgramInfoText(ProgramIndex)});
                    pas::WideString programName = (static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(ProgramIndex));
                    AddLine(3, std::move(programName), std::move(cpp_arg_3), 0);
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->CustomShipInfos) - 1); cpp_range_4.next(I); ) {
            Info = pas::list_at<aShip::TCustomShipInfo>(PlayerHoldShip->CustomShipInfos, I);
            if (!Info->DeleteQueued) {
                Block = GR_Main::LanguageDataConfig->GetBlock(u"ShipInfo"sv)->GetBlock(u"AddInfo"sv)->GetBlock(u"CustomInfos"sv)->GetBlock(pas::view(Info->TypeName));
                Description = Info->Description;
                if (Description == u"") {
                    Description = aConst::LocalizedColorText(pas::concat_wide({u"ShipInfo.AddInfo.CustomInfos.", Info->TypeName, u".Description"}));
                }
                if (Description != u"NoShow") {
                    aMyFunction::ReplaceTextToken(Description, u"<Data1>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<Data2>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<Data3>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData1>"_w, Info->TextData1, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData2>"_w, Info->TextData2, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Description, u"<TextData3>"_w, Info->TextData3, u"<color=255,240,100>"_w);
                    Title = Block->GetParam(u"Name"sv);
                    aMyFunction::ReplaceTextToken(Title, u"<Data1>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Title, u"<Data2>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Title, u"<Data3>"_w, pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Title, u"<TextData1>"_w, Info->TextData1, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Title, u"<TextData2>"_w, Info->TextData2, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Title, u"<TextData3>"_w, Info->TextData3, u"<color=255,240,100>"_w);
                    {
                        std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Icon"sv)));
                        pas::WideString cpp_arg_4 = pas::concat_wide({Title, u"~", Description});
                        AddLine(strToInt, Title, std::move(cpp_arg_4), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Info)));
                    }
                }
            }
        }
        Panel->UpdateScrollRanges();
        Panel->VerticalScrollBar->SetActive(Panel->ClientSize.Y < Height);
        if (Panel->VerticalScrollBar->Active) {
            Panel->VerticalScrollBar->SetLargeChange(Panel->ClientSize.Y);
            Panel->VerticalScrollBar->SetPageSize(Panel->ClientSize.Y);
        }
    }

    void TfShip2::HideSender(GI_MessageLoop::TObjectGI* Sender) {
        Sender->SetActive(false);
    }

    void TfShip2::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop != nullptr && ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(2);
        }
    }

    void TfShip2::ProcessWindowMessage(std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) {
        if (Message == MessagesSdk::WM_LBUTTONDOWN || Message == MessagesSdk::WM_RBUTTONDOWN || Message == MessagesSdk::WM_MBUTTONDOWN || Message == MessagesSdk::WM_KEYDOWN) {
            if (StopScriptVideo()) {
                return;
            }
        }
        GI_MessageLoop::TMessageLoopGI::ProcessWindowMessage(Message, WParam, LParam);
    }

    void TfShip2::SelectMusic() {
        if (aPlayer::GetPlayer() == nullptr) {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
        } else if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
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
            } else if (pas::in_set<7, 7, 12, 12>(aPlayer::GetPlayer()->DockedTo->TypeId)) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace)].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace)].InternalName}));
            }
        } else if (static_cast<std::uint8_t>(PreserveSpaceMusic ^ 1) && PlayerHoldShip->InNormalSpace()) {
            if (GlobalsV::MusicInSpaceEnabled) {
                if (aPlayer::GetPlayer() == PlayerHoldShip && aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 20) {
                    Globals::StarMapScreen->BattleMusicSelected = true;
                    GR_Main::MusicManager->PlayCategory(u"Destroyer"_wref.get());
                } else {
                    Globals::StarMapScreen->BattleMusicSelected = false;
                    GR_Main::MusicManager->PlayCategory(u"StarMap"_wref.get());
                }
            } else {
                GR_Main::MusicManager->RequestFadeOut();
            }
        }
    }

    std::uint8_t TfShip2::IsHoldNormalShip() {
        return pas::class_cast_if<aNormalShip::TNormalShip*>(PlayerHoldShip) != nullptr;
    }

    // Ruins with modernization sponsorship or a player-owned tranclucator.
    std::uint8_t TfShip2::CanUsePlayerExperience() {
        std::uint8_t Result = true;
        if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip); !(ruins != nullptr && ruins->ModernizationSponsor) && !(pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr && static_cast<aTranclucator::TTranclucator*>(PlayerHoldShip)->OwnerShip == aPlayer::GetPlayer())) {
            return false;
        }
        return Result;
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::ToggleRemoteHoldClicked(GI_MessageLoop::TObjectGI* Sender) {
        RemoteHoldMode = static_cast<std::uint8_t>(RemoteHoldMode ^ 1);
        {
            GI_MessageLoop::TObjectGI* PanelRight = GetByName(u"PanelRight"sv);
            PanelRight->SetPosition(ClassesImports::Point(RightPanelRestLeft, PanelRight->LocalPosition.Y));
            PanelRight->SetActive(static_cast<std::uint8_t>(RemoteHoldMode ^ 1));
        }
        {
            GI_MessageLoop::TObjectGI* PanelRH = GetByName(u"PanelRH"sv);
            PanelRH->SetPosition(ClassesImports::Point(RightPanelRestLeft, PanelRH->LocalPosition.Y));
            PanelRH->SetActive(RemoteHoldMode);
        }
        {
            GI_MessageLoop::TObjectGI* PanelLH = GetByName(u"PanelLH"sv);
            PanelLH->SetActive(static_cast<std::uint8_t>(RemoteHoldMode ^ 1));
        }
        {
            GI_MessageLoop::TObjectGI* PanelDS = GetByName(u"PanelDS"sv);
            PanelDS->SetActive(RemoteHoldMode);
        }
        {
            GI_MessageLoop::TObjectGI* PanelDestr = GetByName(u"PanelDestr"sv);
            PanelDestr->SetActive(static_cast<std::uint8_t>(RemoteHoldMode ^ 1) && aPlayer::GetPlayer() == PlayerHoldShip && aPlayer::GetPlayer()->GetHull()->CapitalShip == 1);
        }
        MainPanel->ShowControlHelp(nullptr, false);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::RemoteHoldUpPressed(GI_MessageLoop::TObjectGI* Sender) {
        RemoteHoldFirstOrder = std::max<std::int32_t>(0, RemoteHoldFirstOrder - 5);
        RefreshShipView();
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
        HoldScrollTimer = ScheduleCallbackTimer(500, 50, pas::bind_method<&TfShip2::ScrollRemoteHoldTimer>(this), 0);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::RemoteHoldUpReleased(GI_MessageLoop::TObjectGI* Sender) {
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::RemoteHoldDownPressed(GI_MessageLoop::TObjectGI* Sender) {
        RemoteHoldFirstOrder = std::min<std::int32_t>(TfShip2::GetRemoteHoldScrollLimit(), RemoteHoldFirstOrder + 5);
        RefreshShipView();
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
        HoldScrollTimer = ScheduleCallbackTimer(500, 50, pas::bind_method<&TfShip2::ScrollRemoteHoldTimer>(this), 1);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::RemoteHoldDownReleased(GI_MessageLoop::TObjectGI* Sender) {
        if (HoldScrollTimer != nullptr) {
            CancelCallbackTimer(HoldScrollTimer);
            HoldScrollTimer = nullptr;
        }
    }

    void TfShip2::ScrollRemoteHoldTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (UserData == 0) {
            RemoteHoldFirstOrder -= 5;
        } else {
            RemoteHoldFirstOrder += 5;
        }
        if (RemoteHoldFirstOrder <= 0) {
            RemoteHoldFirstOrder = 0;
            if (HoldScrollTimer != nullptr) {
                CancelCallbackTimer(HoldScrollTimer);
                HoldScrollTimer = nullptr;
            }
        } else if (RemoteHoldFirstOrder >= TfShip2::GetRemoteHoldScrollLimit()) {
            RemoteHoldFirstOrder = TfShip2::GetRemoteHoldScrollLimit();
            if (HoldScrollTimer != nullptr) {
                CancelCallbackTimer(HoldScrollTimer);
                HoldScrollTimer = nullptr;
            }
        }
        RefreshShipView();
    }

    std::int32_t TfShip2::GetRemoteHoldScrollLimit() {
        TPlayerHoldUnit* Entry{};
        std::int32_t I{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TPlayerHoldUnit>(PlayerHoldEntries, I);
            if (Entry->Kind != phkEmpty) {
                Result = std::max<std::int32_t>(Result, Entry->DisplayOrder);
            }
        }
        return std::max<std::int32_t>(0, (Result / 5 + 1) * 5 - 50);
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::SortRemoteHoldClicked(GI_MessageLoop::TObjectGI* Sender) {
        fShip2::SortPlayerHoldEntries(static_cast<TPlayerHoldSort>(Sender->UserValue));
        RefreshShipView();
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::StorageToShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (static_cast<std::uint8_t>(PlayerHoldShip->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0 && (aPlayer::GetPlayer() == PlayerHoldShip || pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr) && aPlayer::GetPlayer()->RuinsMode <= 0) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(515);
            if (PlayerHoldShip->RetrieveStoredItems(TfShip2::GetLocalStorageOwner())) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(516);
                ShipStateChanged = true;
                ReopenRequested = true;
                PlayTransitionSounds = false;
                CloseClicked(nullptr);
            }
        }
    }

    void TfShip2::SellStorageClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Text{};
        std::int32_t I{};
        aItem::TItem* Item{};
        pas::Object* Location{};
        aPlayer::PStorageEntry Entry{};
        Text = aConst::LocalizedColorText(u"FormShip.SellAllFromStorage"_wref.get());
        std::int32_t Value = 0;
        if (pas::list_count(aScript::QueuedArcadeBattles) <= 0) {
            if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                Location = aPlayer::GetPlayer()->CurrentPlanet;
            } else {
                Location = aPlayer::GetPlayer()->DockedTo;
            }
            if (Location != nullptr) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range.next(I); ) {
                    Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I);
                    if (Entry != nullptr) {
                        if (Entry->LocationOwner == Location) {
                            Item = Entry->Item;
                            if (Item != nullptr) {
                                if (Item->NoDropFlag <= 0 && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->CanSell)) {
                                    if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr) {
                                        Value += ([&] {
                                            std::int32_t cpp_right = aPlayer::GetPlayer()->ShopGoodsSellPrice(static_cast<std::uint8_t>(reinterpret_cast<aItem::TGoods*>(Item)->ItemType), reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Location)) + 0))));
                                            return reinterpret_cast<aItem::TGoods*>(Item)->Quantity * cpp_right;
                                        }());
                                    } else if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr) {
                                        Value += Item->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false));
                                    }
                                }
                            }
                        }
                    }
                }
                if (Value > 0) {
                    aMyFunction::ReplaceTextToken(Text, u"<Cost>"_w, pas::wide_int_to_str(Value), u"<color=255,240,100>"_w);
                    if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != 2) {
                        SellAllItems(1);
                    }
                }
            }
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::ShipToStorageClicked(GI_MessageLoop::TObjectGI* Sender) {
        if ((aPlayer::GetPlayer() == PlayerHoldShip || pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr) && aPlayer::GetPlayer()->RuinsMode <= 0 && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && pas::list_count(aScript::QueuedArcadeBattles) <= 0) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(519);
            if (PlayerHoldShip->StoreLooseInventoryAt(TfShip2::GetLocalStorageOwner())) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(520);
                ShipStateChanged = true;
                ReopenRequested = true;
                PlayTransitionSounds = false;
                CloseClicked(nullptr);
            }
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::SortStorageByTypeClicked(GI_MessageLoop::TObjectGI* Sender) {
        SortStorage(phsType);
        RefreshStorageView();
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::SortStorageBySizeClicked(GI_MessageLoop::TObjectGI* Sender) {
        SortStorage(phsSize);
        RefreshStorageView();
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::SortStorageByPriceClicked(GI_MessageLoop::TObjectGI* Sender) {
        SortStorage(phsPrice);
        RefreshStorageView();
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::LoadHoldRocketsClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer()->RuinsMode <= 0) {
            LoadRockets(false);
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::SellHoldClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Text{};
        std::int32_t Value{};
        std::int32_t I{};
        std::uint8_t Good{};
        aItem::TEquipment* Item{};
        pas::Object* Location{};
        if ((aPlayer::GetPlayer() == PlayerHoldShip || pas::class_cast_if<aTranclucator::TTranclucator*>(PlayerHoldShip) != nullptr) && aPlayer::GetPlayer()->RuinsMode <= 0) {
            if (aPlayer::GetPlayer()->IsDocked() && (aPlayer::GetPlayer()->CurrentPlanet == nullptr || aPlayer::GetPlayer()->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited)) {
                Text = aConst::LocalizedColorText(u"FormShip.SellAllFromHold"_wref.get());
                Value = 0;
                if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                    Location = aPlayer::GetPlayer()->CurrentPlanet;
                } else {
                    Location = aPlayer::GetPlayer()->DockedTo;
                }
                for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                    Value += ([&] {
                        std::int32_t cpp_right = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Location);
                        return PlayerHoldShip->CargoGoods[Good].Count * cpp_right;
                    }());
                }
                for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(PlayerHoldShip->Inventory) - 1); cpp_range.next(I); ) {
                    Item = pas::list_at<aItem::TEquipment>(PlayerHoldShip->Inventory, I);
                    if (Item->EquippedFlag == 0 && Item->NoDropFlag <= 0 && Item->ScriptItem == nullptr && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Item)) && Item->NoDropFlag <= 0 && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->CanSell) && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Item))) {
                        Value += Item->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false));
                    }
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(PlayerHoldShip->Artefacts) - 1); cpp_range_2.next(I); ) {
                    Item = pas::list_at<aItem::TEquipment>(PlayerHoldShip->Artefacts, I);
                    if (Item->EquippedFlag == 0 && Item->NoDropFlag <= 0 && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->CanSell) && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Item))) {
                        Value += Item->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false));
                    }
                }
                if (Value > 0) {
                    aMyFunction::ReplaceTextToken(Text, u"<Cost>"_w, pas::wide_int_to_str(Value), u"<color=255,240,100>"_w);
                    if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != 2) {
                        SellAllItems(0);
                    }
                }
            }
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::LoadEquippedRocketsClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer()->RuinsMode <= 0) {
            LoadRockets(true);
        }
    }

    void TfShip2::SortStorage(TPlayerHoldSort Sort) {
        aPlayer::PStorageEntry Entry{};
        std::int32_t LeftIndex{};
        std::int32_t RightIndex{};
        aGalaxy::Galaxy->CheckIntegrityChecksum1(515);
        std::int32_t Count = pas::list_count(aPlayer::GetPlayer()->StorageEntries);
        std::int32_t I = 0;
        std::int32_t J = 0;
        while (I < Count) {
            Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I);
            if (TfShip2::GetLocalStorageOwner() == Entry->LocationOwner) {
                if (Entry->Item == nullptr) {
                    pas::list_delete(aPlayer::GetPlayer()->StorageEntries, I);
                    pas::dispose(Entry);
                    --Count;
                    continue;
                }
                Entry->SlotIndex = J;
                ++J;
            }
            ++I;
        }
        Count = aPlayer::GetPlayer()->GetStorageSlotExtent(TfShip2::GetLocalStorageOwner());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 2); cpp_range.next(I); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, Count - 1); cpp_range_2.next(J); ) {
                LeftIndex = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(TfShip2::GetLocalStorageOwner(), I);
                RightIndex = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(TfShip2::GetLocalStorageOwner(), J);
                if (LeftIndex >= 0 && RightIndex >= 0) {
                    aPlayer::PStorageEntry cpp_arg = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, RightIndex);
                    aPlayer::PStorageEntry cpp_arg_2 = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, LeftIndex);
                    if (fShip2::CompareStoredItems(cpp_arg_2, cpp_arg, Sort) > 0) {
                        Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, LeftIndex);
                        Entry->SlotIndex = J;
                        Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, RightIndex);
                        Entry->SlotIndex = I;
                    }
                }
            }
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(516);
    }

    void TfShip2::RefreshLoadEquippedRocketsButton() {
        std::int32_t I{};
        GI_GraphButton::TGraphButtonGI* Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"LoadRocketsInSlots"sv));
        Button->SetActive(false);
        if ((PlayerHoldShip->CurrentPlanet != nullptr || PlayerHoldShip->DockedTo != nullptr || pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) && (PlayerHoldShip->CurrentPlanet == nullptr || PlayerHoldShip->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited) && (aPlayer::GetPlayer() != PlayerHoldShip || aPlayer::GetPlayer()->RuinsMode <= 0)) {
            for (I = 1; I <= 5; ++I) {
                if (PlayerHoldShip->Weapons[I] != nullptr) {
                    if (PlayerHoldShip->Weapons[I]->NeedsAmmo()) {
                        Button->SetActive(true);
                        break;
                    }
                }
            }
        }
    }

    // Zero sells hold contents; one sells local storage.
    void TfShip2::SellAllItems(std::int32_t Origin) {
        std::uint8_t PlaySaleSound{};
        std::uint8_t Changed{};
        std::int32_t I{};
        std::int32_t StorageIndex{};
        std::int32_t SavedCount{};
        std::int32_t SavedCost{};
        aItem::TItem* Item{};
        std::uint8_t Good{};
        aPlayer::PStorageEntry Entry{};
        // Nested helper captures sale/change flags.
        auto SellItem = [&](aItem::TItem* Item, std::uint8_t IgnoreRequiredEquipment) -> std::uint8_t {
            std::int32_t Count{};
            std::int32_t Cost{};
            std::int32_t Price{};
            aGalaxyEvent::TGalaxyEvent* Event{};
            std::uint8_t Result = false;
            if (Item == nullptr) {
                return Result;
            }
            if (!IgnoreRequiredEquipment) {
                if (pas::class_cast_if<aRuins::TRuins*>(PlayerHoldShip) != nullptr) {
                    if (PlayerHoldShip->NeedsEquipmentType(Item->ItemType)) {
                        return Result;
                    }
                } else if ((Item->ItemType == aConst::t_FuelTanks || Item->ItemType == aConst::t_Engine) && PlayerHoldShip->NeedsEquipmentType(Item->ItemType)) {
                    return Result;
                }
            }
            if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
                Count = aPlayer::GetPlayer()->CargoGoods[Item->ItemType].Count;
                Cost = aPlayer::GetPlayer()->CargoGoods[Item->ItemType].TotalCost;
                aPlayer::GetPlayer()->CargoGoods[Item->ItemType].Count = pas::checked_cast<aItem::TGoods*>(Item)->Quantity;
                aPlayer::GetPlayer()->CargoGoods[Item->ItemType].TotalCost = pas::checked_cast<aItem::TGoods*>(Item)->Cost;
                aPlayer::GetPlayer()->SellGoodsToLocation(static_cast<std::uint8_t>(Item->ItemType), pas::checked_cast<aItem::TGoods*>(Item)->Quantity);
                PlaySaleSound = true;
                aPlayer::GetPlayer()->CargoGoods[Item->ItemType].Count = Count;
                aPlayer::GetPlayer()->CargoGoods[Item->ItemType].TotalCost = Cost;
                pas::free(Item);
            } else {
                if (!(pas::class_cast_if<aItem::TCountableItem*>(Item) != nullptr)) {
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerSellsEquipment"_w, nullptr);
                    Event->AddData(Item->ItemType);
                    Event->AddData(Item->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false)));
                    Event->AddData(Item->Weight);
                    Event->AddData(Item->Id);
                    Event->AddTextData(Item->GetDisplayName());
                    Event->AddTextData(Item->GetCategoryConfigName());
                }
                Price = Item->CalculateResaleValue(aPlayer::GetPlayer()->GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false));
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Price);
                if (Price != 0) {
                    PlaySaleSound = true;
                }
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item)) {
                    weapon->Target = nullptr;
                }
                if (Item->ItemType == aConst::t_Hull && pas::checked_cast<aItem::THull*>(Item)->HullType == aGalaxyStruct::htSpecial) {
                    pas::free(Item);
                } else if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Item->OwnerId) && pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && (!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) || static_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair)) {
                    fEquipmentShop::RestoreTemporaryShopStock();
                    if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                        pas::list_add(aPlayer::GetPlayer()->CurrentPlanet->EquipmentShop, reinterpret_cast<void*>(Item));
                    } else {
                        pas::list_add(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->EquipmentShop, reinterpret_cast<void*>(Item));
                    }
                    fEquipmentShop::BuildTemporaryShopSlotGrid();
                } else {
                    pas::free(Item);
                }
            }
            Result = true;
            Changed = true;
            return Result;
        };
        if (aPlayer::GetPlayer()->InNormalSpace() || aPlayer::GetPlayer()->InHyperspace || aPlayer::GetPlayer()->RuinsMode > 0 || pas::list_count(aScript::QueuedArcadeBattles) > 0 || aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiUninhabited) {
            return;
        }
        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), u"<color=255,240,100>"_w);
                }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            } else {
                GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                    pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), u"<color=255,240,100>"_w);
                }()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum1(517);
        Changed = false;
        PlaySaleSound = false;
        std::uint8_t AllowIllegal = false;
        std::uint8_t DeclineIllegal = false;
        if (Origin == 0) {
            I = pas::list_count(PlayerHoldShip->Inventory) - 1;
            while (I >= 0) {
                Item = pas::list_at<aItem::TItem>(PlayerHoldShip->Inventory, I);
                if (Item->NoDropFlag == 0 && Item->ItemType != aConst::t_Hull && reinterpret_cast<aItem::TEquipment*>(Item)->EquippedFlag == 0 && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->CanSell) && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Item))) {
                    if (SellItem(Item, false)) {
                        pas::list_delete(PlayerHoldShip->Inventory, I);
                    }
                }
                --I;
            }
            I = pas::list_count(PlayerHoldShip->Artefacts) - 1;
            while (I >= 0) {
                Item = pas::list_at<aItem::TItem>(PlayerHoldShip->Artefacts, I);
                if (Item->NoDropFlag == 0 && reinterpret_cast<aItem::TEquipment*>(Item)->EquippedFlag == 0 && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->CanSell) && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Item))) {
                    if (pas::class_cast_if<aItem::TArtefactTranclucator*>(Item) != nullptr) {
                        if (GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                            pas::WideString displayName = Item->GetDisplayName();
                            pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormShip.SellItemQuestion"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(displayName));
                        }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == 2) {
                            --I;
                            continue;
                        }
                    }
                    pas::list_delete(PlayerHoldShip->Artefacts, I);
                    SellItem(Item, true);
                }
                --I;
            }
            for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                if (PlayerHoldShip->CargoGoods[Good].Count > 0) {
                    if (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessHoldGoods(Good)) {
                        if (aPlayer::GetPlayer()->IsCargoGoodIllegalOnCurrentPlanet(Good) && static_cast<std::uint8_t>(AllowIllegal ^ 1) && static_cast<std::uint8_t>(DeclineIllegal ^ 1)) {
                            if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormGS.NotPermitGoods"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
                                DeclineIllegal = true;
                            } else {
                                AllowIllegal = true;
                            }
                        }
                        if (!(aPlayer::GetPlayer()->IsCargoGoodIllegalOnCurrentPlanet(Good) && DeclineIllegal)) {
                            Changed = true;
                            PlaySaleSound = true;
                            if (aPlayer::GetPlayer() == PlayerHoldShip) {
                                aPlayer::GetPlayer()->SellGoodsToLocation(Good, aPlayer::GetPlayer()->CargoGoods[Good].Count);
                            } else {
                                SavedCount = aPlayer::GetPlayer()->CargoGoods[Good].Count;
                                SavedCost = aPlayer::GetPlayer()->CargoGoods[Good].TotalCost;
                                aPlayer::GetPlayer()->CargoGoods[Good].Count = PlayerHoldShip->CargoGoods[Good].Count;
                                aPlayer::GetPlayer()->CargoGoods[Good].TotalCost = PlayerHoldShip->CargoGoods[Good].TotalCost;
                                PlayerHoldShip->CargoGoods[Good].Count = 0;
                                PlayerHoldShip->CargoGoods[Good].TotalCost = 0;
                                aPlayer::GetPlayer()->SellGoodsToLocation(Good, aPlayer::GetPlayer()->CargoGoods[Good].Count);
                                aPlayer::GetPlayer()->CargoGoods[Good].Count = SavedCount;
                                aPlayer::GetPlayer()->CargoGoods[Good].TotalCost = SavedCost;
                            }
                        }
                    }
                }
            }
        } else {
            I = aPlayer::GetPlayer()->GetStorageSlotExtent(TfShip2::GetLocalStorageOwner()) - 1;
            while (I >= 0) {
                StorageIndex = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(TfShip2::GetLocalStorageOwner(), I);
                if (StorageIndex >= 0) {
                    Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, StorageIndex);
                    Item = Entry->Item;
                    if (Item->NoDropFlag > 0 || Item->ScriptItem != nullptr && static_cast<std::uint8_t>(reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->CanSell ^ 1)) {
                        --I;
                        continue;
                    }
                    if (pas::class_cast_if<aItem::TArtefactTranclucator*>(Item) != nullptr) {
                        if (GI_MessageBox::ShowMessageBoxGI(this, ([&] {
                            pas::WideString displayName_2 = Item->GetDisplayName();
                            pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormShip.SellItemQuestion"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<Item>"_w, std::move(displayName_2));
                        }()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == 2) {
                            --I;
                            continue;
                        }
                    }
                    if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr) {
                        if (aPlayer::GetPlayer()->IsCargoGoodIllegalOnCurrentPlanet(static_cast<std::uint8_t>(Item->ItemType)) && static_cast<std::uint8_t>(AllowIllegal ^ 1) && static_cast<std::uint8_t>(DeclineIllegal ^ 1)) {
                            if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormGS.NotPermitGoods"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
                                DeclineIllegal = true;
                            } else {
                                AllowIllegal = true;
                            }
                        }
                    }
                    if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr && aPlayer::GetPlayer()->IsCargoGoodIllegalOnCurrentPlanet(static_cast<std::uint8_t>(Item->ItemType)) && DeclineIllegal) {
                        --I;
                        continue;
                    }
                    Entry->Item = nullptr;
                    pas::list_delete(aPlayer::GetPlayer()->StorageEntries, StorageIndex);
                    pas::dispose(Entry);
                    SellItem(Item, true);
                }
                --I;
            }
        }
        if (Changed) {
            ShipStateChanged = true;
            if (PlaySaleSound) {
                GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(518);
            RefreshShipView();
            ReopenRequested = true;
            PlayTransitionSounds = false;
            CloseClicked(nullptr);
        }
    }

    // Verified callback assignment in InitializeLayout.
    void TfShip2::EnterBridgeClicked(GI_MessageLoop::TObjectGI* Sender) {
        aGalaxy::Galaxy->CheckIntegrityChecksum1(416);
        if (SelectedHoldKind != phkEmpty) {
            ReturnSelectedHoldEntry();
        }
        aPlayer::GetPlayer()->EnterRuinsMode(0);
        aGalaxy::Galaxy->PrimeIntegrityChecksum1(471);
    }

    GI_MessageLoop::TMessageLoopGI* TfShip2::GetActionParentLoop() {
        return ParentLoop;
    }

    void TfShip2::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(10012);
            aGalaxy::Galaxy->CheckIntegrityChecksum2(10013);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(20012);
            aGalaxy::Galaxy->PrimeIntegrityChecksum2(20013);
        }
    }

    void TfShip2::p_destroy() {
        fShip2::TfShip2_Destroy(this);
    }

    pas::Method<void(std::uint32_t, std::uint32_t, std::int32_t)> TfShip2::bind_ProcessWindowMessage() {
        return pas::bind_method<fShip2::TfShip2_ProcessWindowMessage>(this);
    }

} // namespace fShip2
