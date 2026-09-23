#include "layout/aTransport.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aMissile.hpp"
#include "types/aPlanet.hpp"
#include "types/aScript.hpp"
#include "types/aTranclucator.hpp"
#include "units/EC_Buf.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aShip.hpp"
#include "units/aTransport.hpp"

namespace aTransport {
    pas::Array<std::int32_t, 22, 27> TransportSkillBonusWeights = pas::Array<std::int32_t, 22, 27>{{80, 80, 100, 100, 60, 5}};

    // Native eight-entry slot table; artefact/afterburner weights are zero.
    pas::Array<std::int32_t, 13, 20> TransportSlotBonusWeights = pas::Array<std::int32_t, 13, 20>{{200, 0, 150, 0, 150, 50, 0, 0}};

    void TTransport_Destroy(TTransport* Self) {
        --Self->HomePlanet->HomeTransportCount;
        --aGalaxy::Galaxy->TransportCount;
        aNormalShip::TNormalShip_Destroy(Self);
    }

    // Registers Self with the planet's star; requires a fresh instance.
    void TTransport_InitGenerated(TTransport* Self, aPlanet::TPlanet* Planet, std::int32_t InitialMoney, TTransportType SubType, std::uint8_t RandomizeSubType) {
        aRanger::TRanger* Ranger{};
        std::uint8_t Good{};
        // Nested name-selection helper; caller-popped static link and parent Self at -4.
        auto SelectUniqueName = [&](EC_BlockPar::TBlockParEC* Config) -> void {
            std::int32_t Index{};
            std::int32_t Attempt{};
            std::int32_t I{};
            std::int32_t J{};
            aShip::TShip* Ship{};
            aGalaxy::TStar* Star{};
            std::uint8_t Duplicate{};
            EC_BlockPar::TBlockParEC* Block{};
            if (Config == nullptr) {
                return;
            }
            if (Config->CountBlocks(pas::concat_wide({u"Transport", aConst::TransportTypeNames[Self->TransportType]})) > 0) {
                Block = Config->GetBlock(pas::view(pas::concat_wide({u"Transport", aConst::TransportTypeNames[Self->TransportType]})));
            } else if (Config->CountBlocks(u"Transport"_wref.get()) > 0) {
                Block = Config->GetBlock(u"Transport"sv);
            } else {
                return;
            }
            if (aConst::RaceToOwner(Self->PilotRace) == Self->OwnerId) {
                Block = Block->GetBlock(pas::view(aConst::OwnerToSys(Self->OwnerId)));
            } else if (Block->CountBlocks(pas::concat_wide_reverse({aConst::OwnerToSys(aConst::RaceToOwner(Self->PilotRace)), aConst::OwnerToSys(Self->OwnerId)})) > 0) {
                Block = Block->GetBlock(pas::view(pas::concat_wide_reverse({aConst::OwnerToSys(aConst::RaceToOwner(Self->PilotRace)), aConst::OwnerToSys(Self->OwnerId)})));
            } else if (Block->CountBlocks(aConst::OwnerToSys(Self->OwnerId)) > 0) {
                Block = Block->GetBlock(pas::view(aConst::OwnerToSys(Self->OwnerId)));
            } else if (Block->CountBlocks(aConst::OwnerToSys(aConst::RaceToOwner(Self->PilotRace))) > 0) {
                Block = Block->GetBlock(pas::view(aConst::OwnerToSys(aConst::RaceToOwner(Self->PilotRace))));
            }
            std::int32_t FirstIndex = 0;
            std::int32_t LastIndex = Block->GetParamCount() - 1;
            Index = aMyFunction::NextRandomIntRange(FirstIndex, LastIndex, Self->RandomState);
            for (auto cpp_range = pas::for_to<std::int32_t>(FirstIndex, LastIndex); cpp_range.next(Attempt); ) {
                Self->Name = Block->GetParamValue(Index);
                Duplicate = false;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                        if (Self != Ship && Ship->TypeId == aGalaxyStruct::stTransport && pas::checked_cast<TTransport*>(Ship)->Name == Self->Name) {
                            Duplicate = true;
                            break;
                        }
                    }
                }
                if (!Duplicate) {
                    break;
                }
                aMyFunction::IncrementWrapped(Index, FirstIndex, LastIndex);
                if (Attempt == LastIndex) {
                    Self->Name = pas::concat_wide({Self->Name, u" ", u"-", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Self->Id) % 100 + 1)), u"-"});
                }
            }
        };
        ++aGalaxy::Galaxy->TransportCount;
        ++Planet->HomeTransportCount;
        Self->HomePlanet = Planet;
        Self->CurrentPlanet = Self->HomePlanet;
        Self->CurrentStar = Self->CurrentPlanet->CurrentStar;
        pas::list_add(Self->CurrentStar->Ships, reinterpret_cast<void*>(Self));
        Self->PilotRace = Self->HomePlanet->RaceId;
        Self->OwnerId = aConst::RaceToOwner(Self->PilotRace);
        Self->SetMoney(InitialMoney);
        Self->TypeId = aGalaxyStruct::stTransport;
        if (RandomizeSubType) {
            std::int32_t cpp_case = aMyFunction::NextRandomIntRange(0, 100, Self->RandomState);
            if (cpp_case >= 0 && cpp_case <= 45) {
                Self->TransportType = ttTransport;
            } else if (cpp_case >= 46 && cpp_case <= 79) {
                Self->TransportType = ttLiner;
            } else if (cpp_case >= 80 && cpp_case <= 100) {
                Self->TransportType = ttDiplomat;
            }
        } else {
            Self->TransportType = SubType;
        }
        Self->Name = pas::WideString();
        SelectUniqueName(GR_Main::ModShipNameConfig);
        if (Self->GetName().length() == 0) {
            SelectUniqueName(GR_Main::LanguageDataConfig->GetBlock(u"ShipName"sv));
        }
        for (Good = 0; Good <= 7; ++Good) {
            Self->CargoGoods[Good].Count = 0;
            Self->CargoGoods[Good].TotalCost = 0;
        }
        if (aPlayer::GetPlayer() != nullptr) {
            Self->Rank = aMyFunction::NextRandomIntRange(0, aPlayer::GetPlayer()->Rank, Self->RandomState);
            if (Self->Rank > 3) {
                Self->Rank = 3;
            }
            {
                std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, static_cast<std::int32_t>(aConst::CoalitionRankPointThresholds[Self->Rank]) / 2, Self->RandomState);
                aNormalShip::TNormalShip* self = Self;
                self->AddRankPoints(nextRandomIntRange);
            }
            if (!aGalaxy::Galaxy->IsZeroStartingExperienceEnabled()) {
                Self->GainExperience(System::Round(aMyFunction::RemapClamped(Self->Rank, 0.0, 3.0, aConst::TotalSkillTrainingCost / 10, aConst::TotalSkillTrainingCost / 4)), aGalaxyStruct::esUnscaled);
                {
                    double techLevel = aGalaxy::Galaxy->TechLevel;
                    double cpp_arg = aConst::TotalSkillTrainingCost / 20;
                    double nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(aConst::TotalSkillTrainingCost / 20, aConst::TotalSkillTrainingCost / 3, Self->RandomState);
                    std::int32_t round = System::Round(aMyFunction::RemapClamped(techLevel, 3.0, 8.0, cpp_arg, nextRandomIntRange_2));
                    aShip::TShip* self_2 = Self;
                    self_2->GainExperience(round, aGalaxyStruct::esUnscaled);
                }
            }
        }
        Self->ChameleonActive = false;
        Self->GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        std::int32_t I = 5;
        switch (Self->TransportType) {
            case ttTransport: I = 3; break;
            case ttLiner: I = 4; break;
            case ttDiplomat: I = 5; break;
        }
        {
            aGalaxyStruct::TOwnerId ownerId = Self->OwnerId;
            std::int32_t selectRandomHullSeries = Self->SelectRandomHullSeries();
            std::uint8_t cpp_arg_2 = Self->HomePlanet->OwnerId == aGalaxyStruct::oiPirate;
            std::uint16_t round_2 = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[I]);
            aShip::TShip* self_3 = Self;
            aShip::TShip_CreateAndEquipHull(self_3, round_2, 1, ownerId, selectRandomHullSeries, cpp_arg_2);
        }
        Self->CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, Self->HomePlanet->OwnerId);
        Self->CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 1, Self->HomePlanet->OwnerId);
        if (aMyFunction::NextRandomIntRange(1, 10, Self->RandomState) > 9 && Self->GetSlotCountForItemType(aConst::t_CargoHook) > 0) {
            aGalaxyStruct::TOwnerId ownerId_2 = Self->HomePlanet->OwnerId;
            std::uint8_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(1, 1, Self->RandomState);
            std::int32_t cargoHookBaseSize = aConst::CargoHookBaseSize;
            aShip::TShip* self_4 = Self;
            self_4->CreateAndEquipCargoHook(cargoHookBaseSize, nextRandomIntRange_3, ownerId_2);
        }
        if (Self->GetSlotCount(aConst::sskWeapon) > Self->WeaponCount) {
            Self->CreateAndEquipWeapon(aConst::t_IndustrialLaser, aConst::WeaponInfos[aConst::t_IndustrialLaser].AverageSize, 1, Self->HomePlanet->OwnerId);
        }
        if (Self->GetSlotCountForItemType(aConst::t_Radar) > 0) {
            aGalaxyStruct::TOwnerId ownerId_3 = Self->HomePlanet->OwnerId;
            pas::Extended cpp_left = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, Self->RandomState)];
            std::int32_t round_3 = System::Round(cpp_left * aConst::RadarBaseSize);
            aShip::TShip* self_5 = Self;
            self_5->CreateAndEquipRadar(round_3, 1, ownerId_3);
        }
        Self->TrainSkillsAutomatically();
        Self->RefreshDerivedStats(true);
        Self->virtual_TShip_RefreshCurrentStanding();
        Self->SmoothedSpeed = Self->Speed;
        Self->SmoothedEnemySpeed = Self->Speed;
        aShip::TShip_BuyEquipmentAtLocation(Self, true);
        aShip::TShip_BuyEquipmentAtLocation(Self, true);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
            pas::list_add(Self->RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(aConst::OwnerRelations[Self->OwnerId][Ranger->OwnerId]))));
        }
    }

    void TTransport::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aNormalShip::TNormalShip::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(TransportType));
    }

    void TTransport::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aNormalShip::TNormalShip::LoadFromBuffer(Buffer, Galaxy);
        TransportType = static_cast<TTransportType>(EC_Buf::TBufEC_GetByte(Buffer));
    }

    void TTransport_ResolveLoadedReferences(TTransport* Self, aGalaxy::TGalaxy* Galaxy) {
        aNormalShip::TNormalShip_ResolveLoadedReferences(Self, Galaxy);
    }

    void TTransport_NextDay(TTransport* Self) {
        aNormalShip::TNormalShip_NextDay(Self);
        try {
            if (Self->TradeExperience > 0) {
                if (Self->IsHealthEffectActive(aGalaxyStruct::heBusinessMark)) {
                    Self->GainExperience(System::Round(Self->TradeExperience * 1.5L), aGalaxyStruct::esTraderCareer);
                } else {
                    Self->GainExperience(Self->TradeExperience, aGalaxyStruct::esTraderCareer);
                }
                Self->TradeExperience = 0;
            }
            if (Self->ScriptShip != nullptr && Self->HasScriptControl()) {
                Self->ScriptNextDay();
                if (Self->ScriptShip != nullptr) {
                    return;
                }
            }
            Self->virtual_TShip_NextDayLogic();
            if (Self->ScriptShip != nullptr && static_cast<std::uint8_t>(Self->HasScriptControl() ^ 1)) {
                Self->ScriptNextDay();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TTransport.NextDay ", Self->GetFullName(u" "_wref.get())}))));
            } else {
                throw;
            }
        }
    }

    void TTransport_NextDayLogic(TTransport* Self) {
        EC_Struct::TPointF Destination{};
        aPlanet::TPlanet* Planet{};
        std::int32_t Stage = 0;
        try {
            if (Self->CurrentPlanet != nullptr) {
                Stage = 1;
                if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Self->CurrentPlanet->OwnerId)) {
                    Stage = 2;
                    Self->RepairBrokenEquipmentAtLocation();
                    Self->AutoEquipInventory();
                    Self->OptimizeInventory();
                    Self->RefuelAtLocation();
                    Self->ReloadWeaponAmmo();
                    if (Self->RepairHullAtLocation()) {
                        return;
                    }
                    Self->ProcessTrading();
                    aShip::TShip_BuyEquipmentAtLocation(Self, false);
                    Self->RestoreEssentialEquipment();
                    Self->ProcessUnseenProgression();
                    Self->TrainSkillsAutomatically();
                    if (Self->TransportType == ttLiner) {
                        Self->SetMoney(Self->Money + aGalaxy::Galaxy->ComputeScaledMiniMoney(Self->CurrentPlanet->OwnerId));
                    }
                    if (Self->TransportType == ttDiplomat) {
                        std::uint32_t cpp_right = static_cast<std::uint32_t>(Self->GetEffectiveSkillLevel(aGalaxyStruct::psCharisma, false)) + 1 >> 1;
                        Self->SetMoney(Self->Money + aGalaxy::Galaxy->ComputeScaledMiniMoney(Self->CurrentPlanet->OwnerId) * cpp_right);
                    }
                }
                Self->OrderTakeoff();
                Stage = 3;
            } else if (Self->DockedTo != nullptr) {
                if (Self->DockedTo->InNormalSpace()) {
                    Self->OrderTakeoff();
                } else {
                    Self->OrderNone(false);
                }
            } else {
                Stage = 4;
                if (Self->InNormalSpace()) {
                    Stage = 5;
                    Self->BuildReachablePlanetQueue();
                    Stage = 6;
                    if (Self->virtual_TShip_RecomputeFearState()) {
                        Self->TryOfferRansomToPursuer();
                    }
                    Self->ProcessCombatDialogue();
                    Self->AssignWeaponTargetsInStar();
                    if (!Self->InFear) {
                        Stage = 7;
                        Self->SelectEnemyShipInStar();
                        Self->EngageEnemyShip();
                    } else {
                        Stage = 8;
                        if (Self->Order != aShip::soLand && Self->Order != aShip::soJump) {
                            Self->NavigateToEscapePlanet(true);
                        }
                        if (pas::is_one_of<aShip::soLand, aShip::soJump>(Self->Order) && Self->EstimateOrderTravelTurns() > 4 && Self->EnemyShip != nullptr && Self->EnemyShip->OrderTarget == Self && pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Self->EnemyShip->TypeId) && Self->EnemyShip->EstimateOrderTravelTurns() < 3 && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L && static_cast<std::int32_t>(Self->Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 2 == 0) {
                            if (Self->JettisonCargoGoodsTowardTargetValue(std::max<std::int32_t>(200, aGalaxy::Galaxy->ComputeScaledMiniMoney(Self->OwnerId) / 2))) {
                                Self->NotifyFearCargoDrop(Self->EnemyShip);
                            }
                        }
                    }
                    if (Self->Order == aShip::soNone) {
                        Stage = 9;
                        Planet = Self->SelectRepairOrTradePlanet();
                        if (Planet == nullptr) {
                            Destination.X = aMyFunction::NextRandomIntRange(-2000, 2000, Self->RandomState);
                            Destination.Y = aMyFunction::NextRandomIntRange(-2000, 2000, Self->RandomState);
                            Self->OrderMove(Destination, false);
                        } else if (Self->CurrentStar == Planet->CurrentStar) {
                            Self->OrderLanding(Planet, false);
                        } else {
                            Self->OrderJump(Planet->CurrentStar, false);
                        }
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TTransport.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    // May issue a movement order when repairs are needed; result is borrowed and may be nil.
    aPlanet::TPlanet* TTransport::SelectRepairOrTradePlanet() {
        if (HasHullDamageOrBrokenEquippedItems()) {
            return NavigateToQueuedPlanet(false);
        }
        return SelectTradePlanet();
    }

    // Requires a non-nil PlanetQueue; an empty queue returns nil. Result is borrowed.
    aPlanet::TPlanet* TTransport::SelectTradePlanet() {
        aPlanet::TPlanet* Result{};
        std::int32_t Attempts{};
        if (pas::list_count(PlanetQueue) > 0) {
            Attempts = System::Round(aMyFunction::RemapClamped(aGalaxy::TGalaxy::GetCoalitionToPirateSystemRatio(), 0.3, 1.0, 4.0, 1.0));
            do {
                {
                    std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(PlanetQueue) - 1, RandomState);
                    pas::List* planetQueue = PlanetQueue;
                    Result = pas::list_at<aPlanet::TPlanet>(planetQueue, nextRandomIntRange);
                }
                --Attempts;
            } while (!(Attempts == 0 || Result->IsCoalitionOwned));
            return Result;
        }
        return nullptr;
    }

    void TTransport::BuildReachablePlanetQueue() {
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TStar* Star{};
        std::int32_t SmallestShipCount{};
        ClearPlanetQueue();
        PlanetQueue = pas::make_object<pas::List>();
        if (HomePlanet != LastDockedPlanet && static_cast<long double>(JumpRange) >= aMyFunction::PointDistance(HomePlanet->CurrentStar->Position, CurrentStar->Position) && GetHull()->HullPoints > GetHull()->Weight * 0.5L && HomePlanet->IsCoalitionOwned) {
            pas::list_add(PlanetQueue, reinterpret_cast<void*>(HomePlanet));
        } else {
            SmallestShipCount = 100000;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[I].Star));
                if (CurrentStar->StarDistances[I].Distance <= JumpRange && (pas::list_count(Star->Ships) <= 15 || pas::list_count(Star->Ships) <= SmallestShipCount || pas::list_count(PlanetQueue) <= 0) && Star->Status.ControlFaction != aGalaxyStruct::sfDominators && Star->Constellation->Id != 20 && Star->Status.CustomFaction == u"") {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(J); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                        if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Planet->OwnerId) && Planet != LastDockedPlanet && (LastDockedPlanet->CurrentStar == CurrentStar || CurrentStar == Star || CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators || CurrentStar->Status.CustomFaction != u"")) {
                            pas::list_add(PlanetQueue, reinterpret_cast<void*>(Planet));
                            SmallestShipCount = std::min<std::int32_t>(SmallestShipCount, pas::list_count(Star->Ships));
                        }
                    }
                }
            }
        }
    }

    // AI ownership check only; does not test travel range.
    std::uint8_t TTransport_CanQueueReachablePlanet(TTransport* Self, aPlanet::TPlanet* Planet) {
        return Planet->OwnerId != aGalaxyStruct::oiDominator;
    }

    // Requires CurrentPlanet. Can sell below cost; purchases can exhaust money and cargo space.
    void TTransport::ProcessTrading() {
        std::uint8_t Good{};
        std::int32_t Quantity{};
        switch (TransportType) {
            case ttTransport: {
                for (Good = 0; Good <= 7; ++Good) {
                    if (pas::in_range(Good, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics)) && CurrentPlanet->Goods[Good].Count > 0 && ([&] {
                        std::int32_t cpp_left = ShopGoodsPurchasePrice(Good, nullptr);
                        return cpp_left < aConst::GoodsMarket[Good].AveragePrice;
                    }()) && CargoFreeSpace > 0) {
                        {
                            pas::Extended cpp_right = ShopGoodsPurchasePrice(Good, nullptr);
                            std::int64_t trunc = System::Trunc(pas::real_divide(Money, cpp_right));
                            std::int64_t cargoFreeSpace = static_cast<std::int64_t>(CargoFreeSpace);
                            Quantity = std::min<std::int64_t>(trunc, cargoFreeSpace);
                        }
                        Quantity = std::min<std::int32_t>(Quantity, CurrentPlanet->Goods[Good].Count);
                        BuyGoodsFromLocation(Good, Quantity);
                    } else if (CargoGoods[Good].Count > 0) {
                        if (([&] {
                            pas::Extended cpp_left_2 = ShopGoodsSellPrice(Good, nullptr);
                            return cpp_left_2 > GetAverageCargoCost(Good);
                        }()) || aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L) {
                            SellGoodsToLocation(Good, CargoGoods[Good].Count);
                        }
                    }
                }
                break;
            }
            case ttLiner: {
                for (Good = 0; Good <= 7; ++Good) {
                    if (pas::in_set<0, 3, 5, 5, 7, 7>(Good) && CurrentPlanet->Goods[Good].Count > 0 && ([&] {
                        std::int32_t cpp_left_3 = ShopGoodsPurchasePrice(Good, nullptr);
                        return cpp_left_3 < aConst::GoodsMarket[Good].AveragePrice;
                    }()) && CargoFreeSpace > 0) {
                        {
                            pas::Extended cpp_right_2 = ShopGoodsPurchasePrice(Good, nullptr);
                            std::int64_t trunc_2 = System::Trunc(pas::real_divide(Money, cpp_right_2));
                            std::int64_t cargoFreeSpace_2 = static_cast<std::int64_t>(CargoFreeSpace);
                            Quantity = std::min<std::int64_t>(trunc_2, cargoFreeSpace_2);
                        }
                        Quantity = std::min<std::int32_t>(Quantity, CurrentPlanet->Goods[Good].Count);
                        BuyGoodsFromLocation(Good, Quantity);
                    } else if (CargoGoods[Good].Count > 0) {
                        if (([&] {
                            pas::Extended cpp_left_4 = ShopGoodsSellPrice(Good, nullptr);
                            return cpp_left_4 > GetAverageCargoCost(Good);
                        }()) || aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L) {
                            SellGoodsToLocation(Good, CargoGoods[Good].Count);
                        }
                    }
                }
                break;
            }
            case ttDiplomat: {
                for (Good = 0; Good <= 7; ++Good) {
                    if (pas::in_set<2, 3, 5, 7>(Good) && CurrentPlanet->Goods[Good].Count > 0 && ([&] {
                        std::int32_t cpp_left_5 = ShopGoodsPurchasePrice(Good, nullptr);
                        return cpp_left_5 < aConst::GoodsMarket[Good].AveragePrice;
                    }()) && CargoFreeSpace > 0) {
                        {
                            pas::Extended cpp_right_3 = ShopGoodsPurchasePrice(Good, nullptr);
                            std::int64_t trunc_3 = System::Trunc(pas::real_divide(Money, cpp_right_3));
                            std::int64_t cargoFreeSpace_3 = static_cast<std::int64_t>(CargoFreeSpace);
                            Quantity = std::min<std::int64_t>(trunc_3, cargoFreeSpace_3);
                        }
                        Quantity = std::min<std::int32_t>(Quantity, CurrentPlanet->Goods[Good].Count);
                        BuyGoodsFromLocation(Good, Quantity);
                    } else if (CargoGoods[Good].Count > 0) {
                        if (([&] {
                            pas::Extended cpp_left_6 = ShopGoodsSellPrice(Good, nullptr);
                            return cpp_left_6 > GetAverageCargoCost(Good);
                        }()) || aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L) {
                            SellGoodsToLocation(Good, CargoGoods[Good].Count);
                        }
                    }
                }
                break;
            }
        }
    }

    // Restores equipment condition without charging Money.
    void TTransport::RepairBrokenEquipmentAtLocation() {
        std::int32_t I{};
        aItem::TEquipment* Equipment{};
        aItem::TArtefact* Artefact{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Equipment = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Equipment->BrokenFlag != 0 || Equipment->ConditionPercent < 2.0E+1L) {
                Equipment->Repair();
            }
        }
        if (CanRepairArtefactsAtLocation()) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
                Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
                if (Artefact->BrokenFlag != 0 || Artefact->ConditionPercent < 2.0E+1L) {
                    if (Artefact->EquippedFlag != 0) {
                        Artefact->Repair();
                    }
                }
            }
        }
    }

    aGalaxy::TStar* TTransport::GetHomeStar() {
        return HomePlanet->CurrentStar;
    }

    pas::WideString TTransport::GetName() {
        return Name;
    }

    pas::WideString TTransport::GetFullName(const pas::WideString& Separator) {
        pas::WideString Path{};
        pas::WideString Text{};
        if (TypeNameOverrideKey == u"") {
            return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"ShipType.", aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)), u".", GetTypeNameKey()})), Separator, Name});
        }
        Path = pas::concat_wide({u"ShipType.", aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)), u".", TypeNameOverrideKey});
        if (GR_Main::LanguageDataConfig->CountParamsByPath(Path) > 0) {
            Text = aConst::LocalizedText(Path);
        } else {
            Text = aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", TypeNameOverrideKey}));
        }
        if (Text != u"") {
            return pas::concat_wide({Text, Separator, Name});
        }
        return Name;
    }

    pas::WideString TTransport::GetTypeNameKey() {
        return aConst::TransportTypeNames[TransportType];
    }

    aGalaxyStruct::TGreetingShipCategory TTransport::GetGreetingShipCategory() {
        switch (TransportType) {
            case ttTransport: return aGalaxyStruct::gscTransport;
            case ttLiner: return aGalaxyStruct::gscLiner;
            default: return aGalaxyStruct::gscDiplomat;
        }
    }

    // Always rcTrader.
    aGalaxyStruct::TRangerCareer TTransport::GetDominantCareer() {
        return aGalaxyStruct::rcTrader;
    }

    aGalaxyStruct::TPercent TTransport::GetStrengthScaledPirateStatus() {
        return 0;
    }

    std::int32_t TTransport::GetDesiredCargoFreeSpace() {
        std::int32_t Result = 0;
        switch (TransportType) {
            case ttTransport: return System::Trunc(GetHull()->Weight * 0.3L);
            case ttLiner: return static_cast<std::int32_t>(System::Trunc(GetHull()->Weight * 0.1L)) + 30;
            case ttDiplomat: return static_cast<std::int32_t>(System::Trunc(GetHull()->Weight * 0.1L)) + 10;
            default: return Result;
        }
    }

    // Fills installed fuel tanks without charging Money.
    void TTransport::RefuelAtLocation() {
        if (GetFuelTanks() != nullptr) {
            GetFuelTanks()->Fuel = GetFuelTanks()->Capacity;
        }
    }

    void TTransport::ProcessUnseenProgression() {
        std::uint8_t Award{};
        if (DaysSincePlayerSeen >= 60 && aPlayer::GetPlayer() != nullptr) {
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.06L) {
                std::int32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(100, 500, Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 59) + 789);
                aShip::TShip* self = this;
                self->GainExperience(seededRandomIntRange, aGalaxyStruct::esUnscaled);
            }
            if (aPlayer::GetPlayer()->Rank > Rank && aMyFunction::NextRandomUnitFloat(RandomState) < 0.03L && Rank < 4) {
                {
                    std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(10, 20, RandomState);
                    aNormalShip::TNormalShip* self_2 = this;
                    self_2->AddRankPoints(nextRandomIntRange);
                }
                TryPromoteRank();
            }
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.01L && (AwardIds == nullptr || 2 * Rank > pas::list_count(AwardIds))) {
                switch (TransportType) {
                    case ttTransport: {
                        Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}, {aGalaxyStruct::atCowardice}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation}}));
                        break;
                    }
                    case ttLiner: {
                        Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation}}));
                        break;
                    }
                    case ttDiplomat: {
                        Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment, aGalaxyStruct::atPerfidy}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation}}));
                        break;
                    }
                    default: Award = aGalaxyStruct::AwardNotFound; break;
                }
                if (Award != aGalaxyStruct::AwardNotFound) {
                    AddAward(Award);
                }
            }
        }
    }

    std::uint8_t TTransport::RelationToNonRanger(aShip::TShip* Ship) {
        std::int32_t Value{};
        if (Ship->TypeId == aGalaxyStruct::stTransport) {
            Value = System::Round(*([&] {
                auto cpp_index = aConst::RaceToOwner(Ship->PilotRace);
                auto* cpp_array = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace)];
                return &(*cpp_array)[cpp_index];
            }()) * (0.7L * aConst::PlanetRaceMarket[PilotRace].FriendlyRelationScale));
            if (Value > 100) {
                Value = 100;
            }
            return Value;
        } else if (Ship->TypeId == aGalaxyStruct::stPirate) {
            Value = System::Round(*([&] {
                auto cpp_index_2 = aConst::RaceToOwner(Ship->PilotRace);
                auto* cpp_array_2 = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace)];
                return &(*cpp_array_2)[cpp_index_2];
            }()) * (0.5L * aConst::PlanetRaceMarket[PilotRace].PirateRelationFactor));
            // Native's reversed clamp always produces 20, but still performs the calculation.
            Value = std::min<std::int32_t>(20, std::max<std::int32_t>(Value, 70));
            return Value;
        } else if (Ship->TypeId == aGalaxyStruct::rstDominion) {
            return 40;
        } else if (Ship->TypeId == aGalaxyStruct::rstPirateBase) {
            return 50;
        } else if (pas::is_one_of<aGalaxyStruct::stKling, aGalaxyStruct::stTranclucator>(Ship->TypeId)) {
            return 50;
        } else {
            return 100;
        }
    }

    std::uint8_t TTransport::RelationToRanger(void* Ranger) {
        return static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))))));
    }

    void TTransport::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))));
        std::uint8_t Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index)));
        if (static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aGalaxyStruct::psCharisma, false) > 0 && Amount > 0) {
            Amount += System::Round(Amount * static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aGalaxyStruct::psCharisma, false) * 0.2L);
        }
        std::int32_t Value = Amount + Relation;
        if (Value < 0) {
            Relation = 0;
        } else if (Value > 100) {
            Relation = 100;
        } else {
            Relation = Value;
        }
        pas::list_put(RangerRelations, Index, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Relation))));
        if (Relation < aGalaxyStruct::RelationBadMin && (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar)) {
            EnemyShip = static_cast<aShip::TShip*>(Ranger);
        }
        if (aPlayer::GetPlayer() == Ranger) {
            if (aMyFunction::RandomIntRange(0, 100) == 0) {
                SysUtilsImports::Sleep(1u);
            }
            if (static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index))) != Relation && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
    }

    void TTransport::ReactToAttack(aShip::TShip* Attacker) {
        EnemyShip = Attacker;
        if (Attacker->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(Attacker, -10);
            if (Attacker->PartnerShip != nullptr && Attacker->PartnerShip->TypeId == aGalaxyStruct::stRanger) {
                ChangeRelationToRanger(Attacker->PartnerShip, -5);
            }
            if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(Attacker)) {
                if (tranclucator->OwnerShip != nullptr) {
                    if (tranclucator->OwnerShip->TypeId == aGalaxyStruct::stRanger) {
                        ChangeRelationToRanger(tranclucator->OwnerShip, -10);
                    }
                }
            }
        }
    }

    // Updates InFear and may replace EnemyShip.
    std::uint8_t TTransport_RecomputeFearState(TTransport* Self) {
        std::uint8_t Result{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        std::int32_t EnemyCount{};
        double Tolerance{};
        double Threat{};
        if (Self->HasNoUsableWeapons() && Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar == Self->CurrentStar) {
            Result = true;
            Self->InFear = true;
            return Result;
        }
        Result = Self->GetHull()->HullPoints < Self->GetHull()->Weight * 0.2L || Self->EnemyShip != nullptr && (Self->EnemyShip->OrderTarget == Self || aPlayer::GetPlayer() == Self->EnemyShip) && Self->virtual_TShip_AcceptsRansomDemandFrom(Self->EnemyShip);
        if (!Result) {
            Threat = 0.0;
            EnemyCount = 0;
            Tolerance = aMyFunction::RemapClamped(Self->GetHull()->HullPoints, 5.0E+1, Self->GetHull()->Weight, 0.0, 3.0);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                if (Ship->InNormalSpace() && Ship != Self) {
                    if (Ship == Self->EnemyShip) {
                        Threat = static_cast<long double>(aShip::TShip_ChanceToWin(Ship, Self)) + Threat;
                        ++EnemyCount;
                    } else if (Ship->EnemyShip == Self && Ship->OrderTarget == Self || aShip::TShip_RelationToShip(Ship, Self) < aGalaxyStruct::RelationBadMin && aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) < 2.5E+5L) {
                        Threat = static_cast<long double>(aShip::TShip_ChanceToWin(Ship, Self)) + Threat;
                        if (Self->EnemyShip == nullptr || Self->EnemyShip->CurrentStar != Self->CurrentStar || Self->EnemyShip->IsOutsideStarSpace()) {
                            Self->EnemyShip = Ship;
                        } else if (Self->EnemyShip != Ship && Self->OrderTarget != Self->EnemyShip) {
                            if (aMyFunction::PointDistanceSquared(Self->EnemyShip->Position, Self->Position) > aMyFunction::PointDistanceSquared(Ship->Position, Self->Position)) {
                                Self->EnemyShip = Ship;
                            }
                        }
                    }
                }
            }
            if (static_cast<long double>(EnemyCount - 1) * Threat * 0.5L + Threat > Tolerance) {
                Result = true;
            }
        }
        Self->InFear = Result;
        return Result;
    }

    void TTransport::TryOfferRansomToPursuer() {
        pas::WideString Response{};
        std::int32_t Amount{};
        float LowOffer{};
        float HighOffer{};
        std::uint8_t Accepted{};
        aShip::TShip* Ship{};
        if (InNormalSpace() && EnemyShip != nullptr && EnemyShip->OrderTarget == this && EnemyShip->TruceShip != this && Money > 100 && static_cast<std::uint8_t>(CanEscapePursuer(EnemyShip) ^ 1) && static_cast<std::uint8_t>(pas::contains(aConst::NonNegotiatingShipTypes, EnemyShip->TypeId) ^ 1) && static_cast<long double>(([&] {
            std::int32_t cpp_left = EnemyShip->GetMaxWeaponRange();
            return cpp_left * EnemyShip->GetMaxWeaponRange();
        }())) >= aMyFunction::PointDistanceSquared(Position, EnemyShip->Position) && (aGalaxy::Galaxy->CurrentTurn * EnemyShip->Id % 3 == 0 && aMyFunction::NextRandomUnitFloat(RandomState) > 0.2L || GetHullIntegrityPercent() < 20) && static_cast<std::uint8_t>(NoTalk ^ 1) && static_cast<std::uint8_t>(EnemyShip->NoTalk ^ 1)) {
            LowOffer = std::min<std::int32_t>(Money, GetWealthScaledAmount(1));
            HighOffer = pas::real_min<pas::Extended>(static_cast<pas::Extended>(Money), ([&] {
                std::int32_t cpp_left_2 = GetWealthScaledAmount(4);
                return cpp_left_2 + EnemyShip->GetWealthScaledAmount(4);
            }()) * 0.5L);
            Amount = System::Round(pas::real_max<double>(1.0E+2, aMyFunction::RemapClamped(GetHull()->HullPoints, 0.0, GetHull()->Weight, HighOffer, LowOffer)));
            Ship = EnemyShip;
            Accepted = Ship->BuildTrucePaymentResponse(this, Response, Amount);
            if (aPlayer::GetPlayer() != Ship && aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
                NotifyTruceOffer(Ship, Response, Amount);
            }
            if (Accepted && this->virtual_TShip_RecomputeFearState()) {
                TryOfferRansomToPursuer();
            }
        }
    }

    std::uint8_t TTransport_AcceptsRansomDemandFrom(TTransport* Self, aShip::TShip* Ship) {
        float LicenseFactor{};
        float AbductionFactor{};
        if (aPlayer::GetPlayer() == Ship && aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
            LicenseFactor = 1.2f;
        } else {
            LicenseFactor = 1.0f;
        }
        if (Self->AbductedByPirateClan) {
            AbductionFactor = 0.5f;
        } else {
            AbductionFactor = 1.0f;
        }
        return Self->GetHull()->Weight * (0.5L * LicenseFactor * aConst::OwnerInfo[Self->OwnerId].FearThresholdScale + 0.2L) > Self->GetHull()->HullPoints && ([&] {
            pas::Extended cpp_right = static_cast<long double>(aShip::TShip_ChanceToWin(Self, Ship)) * AbductionFactor;
            return static_cast<long double>(LicenseFactor) * aConst::OwnerInfo[Self->OwnerId].FearThresholdScale > cpp_right;
        }());
    }

    std::uint8_t TTransport_TrustsAttackRequester(TTransport* Self, aShip::TShip* Ship) {
        return aShip::TShip_RelationToShip(Self, Ship) >= aGalaxyStruct::RelationNormalMin;
    }

    std::uint8_t TTransport::AcceptsAppealFrom(aShip::TShip* Ship) {
        pas::Extended cpp_right = aMyFunction::RemapClamped(Ship->Strength, 0.9L * Strength, Strength * 3.0L, 0.0, 1.0E+2);
        return aShip::TShip_RelationToShip(this, Ship) + cpp_right > 1.1E+2L;
    }

    void TTransport::AssignWeaponTargetsInStar() {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        aItem::TItem* Item{};
        aAsteroid::TAsteroid* Asteroid{};
        float Distance{};
        aMissile::TMissile* Missile{};
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Weapons[I];
                    Weapon->Target = nullptr;
                }
            }
        }
        std::int32_t Assigned = 0;
        if (CurrentStar->Status.Battle != 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (Ship->OwnerId == aGalaxyStruct::oiDominator && Ship->InNormalSpace()) {
                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last_2) {
                        for (J = 1; J <= cpp_last_2; ++J) {
                            Weapon = Weapons[J];
                            if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, Ship->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                                    Weapon->Target = Ship;
                                    ++Assigned;
                                    if (Assigned == WeaponCount) {
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace()) {
            const std::int32_t cpp_last_3 = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last_3) {
                for (J = 1; J <= cpp_last_3; ++J) {
                    Weapon = Weapons[J];
                    if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                        if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, EnemyShip->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                            Weapon->Target = EnemyShip;
                            ++Assigned;
                            if (Assigned == WeaponCount) {
                                return;
                            }
                        }
                    }
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Missiles) - 1); cpp_range_2.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(CurrentStar->Missiles, I);
            if (Missile->Target == this && Missile->OwnerShip != this) {
                const std::int32_t cpp_last_4 = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last_4) {
                    for (J = 1; J <= cpp_last_4; ++J) {
                        Weapon = Weapons[J];
                        if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                            if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, Missile->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                                Weapon->Target = Missile;
                                ++Assigned;
                                if (Assigned == WeaponCount) {
                                    return;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Asteroids) - 1); cpp_range_3.next(I); ) {
                Asteroid = pas::list_at<aAsteroid::TAsteroid>(CurrentStar->Asteroids, I);
                Distance = aMyFunction::PointDistanceSquared(Position, Asteroid->Position);
                if (Distance <= pas::constant(static_cast<long double>(aGalaxyStruct::AsteroidTargetRangeSquared))) {
                    const std::int32_t cpp_last_5 = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last_5) {
                        for (J = 1; J <= cpp_last_5; ++J) {
                            Weapon = Weapons[J];
                            // Native asteroid targeting can overwrite an existing assignment.
                            if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                if (static_cast<long double>(pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) >= Distance) {
                                    Weapon->Target = Asteroid;
                                    ++Assigned;
                                    if (Assigned == WeaponCount) {
                                        return;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (aGalaxy::Galaxy->GetAIJunkToleranceLevel() < pas::list_count(CurrentStar->Items)) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range_4.next(I); ) {
                Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                if ((Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") && (Item->ItemType == aConst::t_Minerals || 2 * aGalaxy::Galaxy->GetAIJunkToleranceLevel() <= pas::list_count(CurrentStar->Items)) && (Item->ItemType == aConst::t_Minerals || Item->OwnerId == aGalaxyStruct::oiDominator)) {
                    if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1) || aShip::TShip_GetRelationLevelToShip(this, aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad || aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >= 8.0E+2L || aMyFunction::NextRandomUnitFloat(RandomState) <= 0.1L && aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >= 2.0E+2L) {
                        if (CanSafelyDetonateItem(Item) && static_cast<std::uint8_t>(IsRecentlyDroppedItem(Item) ^ 1)) {
                            const std::int32_t cpp_last_6 = static_cast<std::int32_t>(WeaponCount);
                            if (1 <= cpp_last_6) {
                                for (J = 1; J <= cpp_last_6; ++J) {
                                    Weapon = Weapons[J];
                                    if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                        if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, Item->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                                            Weapon->Target = Item;
                                            ++Assigned;
                                            if (Assigned == WeaponCount) {
                                                return;
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
                if (aPlayer::GetPlayer()->InNormalSpace() && IsPlayerChameleonEffectiveAgainstSelf()) {
                    const std::int32_t cpp_last_7 = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last_7) {
                        for (J = 1; J <= cpp_last_7; ++J) {
                            Weapon = Weapons[J];
                            if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                                    Weapon->Target = aPlayer::GetPlayer();
                                    ++Assigned;
                                    if (Assigned == WeaponCount) {
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void TTransport::SelectEnemyShipInStar() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        if ((EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar || pas::is_one_of<aGalaxyStruct::ssDominator, aGalaxyStruct::ssCustom>(EnemyShip->CurrentStanding)) && UsableWeaponCount != 0 && (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfCoalition || CurrentStar->Status.CustomFaction != u"")) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (pas::is_one_of<aGalaxyStruct::ssDominator, aGalaxyStruct::ssCustom>(Ship->CurrentStanding) && Ship->InNormalSpace()) {
                    EnemyShip = Ship;
                    if (([&] {
                        pas::Extended cpp_left = aShip::TShip_ChanceToWin(this, Ship);
                        return cpp_left - aConst::OwnerInfo[OwnerId].FearThresholdScale;
                    }()) > 0.0L) {
                        break;
                    }
                }
            }
        }
    }

    void TTransport::EngageEnemyShip() {
        if (Order == aShip::soFollowShip) {
            OrderNone(false);
        }
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace()) {
            if (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates || CurrentStar->Status.Battle != 0) {
                OrderFollowShip(EnemyShip, aShip::fmMinWeaponRange, false);
            }
            if (aShip::TShip_ChanceToWin(this, EnemyShip) < 0.9L) {
                aShip::TShip_RequestAlliesAttackShip(this, EnemyShip);
            }
        }
    }

    void TTransport::ProcessCombatDialogue() {
        if (EnemyShip != nullptr && OrderTarget == EnemyShip && static_cast<std::int32_t>(Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 7 == 0 && aShip::TShip_ChanceToWin(this, EnemyShip) < 1.3L && GetHullIntegrityPercent() > 30) {
            aShip::TShip_RequestAlliesAttackShip(this, EnemyShip);
        }
    }

    void TTransport::ReactToExtortionDemand(void* Ranger) {
        if (aPlayer::GetPlayer() == Ranger || aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L) {
            ChangeRelationToRanger(Ranger, -15);
            HomePlanet->ChangeRelationToRanger(Ranger, -2);
            pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))->AddPirateCareerActivity(2);
        }
    }

    std::uint8_t TTransport_BuildMoneyExtortionResponse(TTransport* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        float LicenseFactor{};
        // Nested response helper with caller-popped static link.
        auto PayDemand = [&]() -> void {
            aGalaxyEvent::TGalaxyEvent* Event{};
            Self->AbductedByPirateClan = false;
            if (aPlayer::GetPlayer() == OtherShip) {
                Self->LastPlayerExtortionTurn = aGalaxy::Galaxy->CurrentTurn;
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerExtortsMoney"_w, nullptr);
                Event->AddData(DemandedAmount);
                Event->AddData(Self->TypeId);
                Event->AddData(Self->CurrentStar->Id);
                Event->AddData(Self->Id);
                Event->AddData(Self->OwnerId);
                Event->AddTextData(Self->GetName());
                Event->AddTextData(Self->TypeNameOverrideKey);
                if (aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
                    aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + System::Round(DemandedAmount * 0.9L));
                    aPlayer::GetPlayer()->PendingPirateLicenseCash += System::Round(DemandedAmount * 0.1L);
                    if (aPlayer::GetPlayer()->PendingPirateLicenseCash > aGalaxyStruct::MaxMonetaryValue) {
                        aPlayer::GetPlayer()->PendingPirateLicenseCash = aGalaxyStruct::MaxMonetaryValue;
                    }
                } else {
                    aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + DemandedAmount);
                }
            } else {
                OtherShip->SetMoney(OtherShip->Money + DemandedAmount);
            }
            Self->SetMoney(Self->Money - DemandedAmount);
            aShip::TShip_TruceWithShip(OtherShip, Self);
            if (OtherShip->OwnerId == aGalaxyStruct::oiPirate) {
                reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->AddPirateRankPoints(1u);
            }
            if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(OtherShip)) {
                aRanger::TRanger_ApplyExtortionReputationPenalty(ranger, Self);
            }
        };
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer() == OtherShip && aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
            LicenseFactor = 0.8f;
        } else {
            LicenseFactor = 1.0f;
        }
        std::int32_t NextDemandTurn = Self->LastPlayerExtortionTurn + 30;
        if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr) {
            Self->ReactToExtortionDemand(OtherShip);
        }
        if (aPlayer::GetPlayer() != OtherShip && (Self->EnemyShip == nullptr || Self->CurrentStar != Self->EnemyShip->CurrentStar)) {
            Self->EnemyShip = OtherShip;
        }
        if (OtherShip->TruceShip == Self) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Money.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && Self->PlayerExtortionPactActive) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Money.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Money.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (!Self->virtual_TShip_AcceptsRansomDemandFrom(OtherShip)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        } else if (Self->CanEscapePursuer(OtherShip)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"LongDistance"}), OtherShip);
            return Result;
        } else {
            double cpp_arg = static_cast<long double>(aShip::TShip_GetWinChancePercent(Self, OtherShip)) * LicenseFactor;
            double wealthScaledAmount = Self->GetWealthScaledAmount(4);
            double wealthScaledAmount_2 = Self->GetWealthScaledAmount(2);
            if (static_cast<long double>(DemandedAmount) > aMyFunction::RemapClamped(cpp_arg, 0.0, 1.0E+2, wealthScaledAmount, wealthScaledAmount_2)) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"SumIsVeryBig"}), OtherShip);
                return Result;
            } else if (Self->Money < DemandedAmount) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Money.AnswerNotMoney"_wref.get(), OtherShip);
                return Result;
            } else {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"Ok"}), OtherShip);
                PayDemand();
                return true;
            }
        }
    }

    std::uint8_t TTransport_BuildCargoExtortionResponse(TTransport* Self, aShip::TShip* OtherShip, pas::WideString& Response) {
        // Nested response helper with caller-popped static link.
        auto DropDemand = [&]() -> void {
            std::uint8_t Good{};
            std::int32_t Pass{};
            std::int32_t Count{};
            float Divisor{};
            aGalaxyEvent::TGalaxyEvent* Event{};
            Self->AbductedByPirateClan = false;
            std::int32_t TotalValue = 0;
            std::uint8_t Enough = false;
            std::int32_t LowValue = Self->GetWealthScaledAmount(1);
            std::int32_t HighValue = Self->GetWealthScaledAmount(4);
            for (Pass = 1; Pass <= 3; ++Pass) {
                for (Good = 0; Good <= 7; ++Good) {
                    if (Self->CargoGoods[Good].Count > 0) {
                        Divisor = aMyFunction::RemapClamped(Self->CargoGoods[Good].Count * aConst::GoodsMarket[Good].AveragePrice, LowValue, HighValue, 2.0, 8.0);
                        Count = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(pas::real_divide(Self->CargoGoods[Good].Count, Divisor)));
                        TotalValue += Count * aConst::GoodsMarket[Good].AveragePrice;
                        aShip::TShip_DropGoodsIntoSpace(Self, Good, Count);
                        if (TotalValue > HighValue) {
                            Enough = true;
                            break;
                        }
                    }
                }
                if (Enough) {
                    break;
                }
            }
            aShip::TShip_TruceWithShip(OtherShip, Self);
            if (aPlayer::GetPlayer() == OtherShip) {
                Self->LastPlayerExtortionTurn = aGalaxy::Galaxy->CurrentTurn;
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerExtortsGoods"_w, nullptr);
                Event->AddData(TotalValue);
                Event->AddData(Self->TypeId);
                Event->AddData(Self->CurrentStar->Id);
                Event->AddData(Self->Id);
                Event->AddData(Self->OwnerId);
                Event->AddTextData(Self->GetName());
                Event->AddTextData(Self->TypeNameOverrideKey);
            }
            OtherShip->OrderMove(Self->Position, true);
            if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(OtherShip)) {
                aRanger::TRanger_ApplyExtortionReputationPenalty(ranger, Self);
            }
            if (OtherShip->OwnerId == aGalaxyStruct::oiPirate) {
                reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->AddPirateRankPoints(1u);
            }
        };
        std::uint8_t Result = false;
        std::uint8_t Forced = aPlayer::GetPlayer() == OtherShip && OtherShip->IsHealthEffectActive(aGalaxyStruct::heOneEyedKhamas);
        std::int32_t NextDemandTurn = Self->LastPlayerExtortionTurn + 30;
        if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr) {
            Self->ReactToExtortionDemand(OtherShip);
        }
        if (aPlayer::GetPlayer() != OtherShip && (Self->EnemyShip == nullptr || Self->CurrentStar != Self->EnemyShip->CurrentStar)) {
            Self->EnemyShip = OtherShip;
        }
        if (OtherShip->TruceShip == Self) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Goods.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && Self->PlayerExtortionPactActive) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Goods.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Goods.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (static_cast<std::uint8_t>(Self->virtual_TShip_AcceptsRansomDemandFrom(OtherShip) ^ 1) && static_cast<std::uint8_t>(Forced ^ 1)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Goods.", Self->GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        } else if (Self->CanEscapePursuer(OtherShip) && static_cast<std::uint8_t>(Forced ^ 1)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Goods.", Self->GetTypeNameKey(), u"LongDistance"}), OtherShip);
            return Result;
        } else if (!Self->HasCargoGoods()) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Goods.AnswerNotGoods"_wref.get(), OtherShip);
            return Result;
        } else {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Goods.", Self->GetTypeNameKey(), u"Ok"}), OtherShip);
            DropDemand();
            if (aPlayer::GetPlayer() == OtherShip) {
                Self->PlayerExtortionPactActive = true;
            }
            return true;
        }
    }

    std::uint8_t TTransport::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) {
        // Nested response helper with caller-popped static link.
        auto AcceptPayment = [&]() -> void {
            OtherShip->SetMoney(OtherShip->Money - OfferedAmount);
            SetMoney(this->Money + OfferedAmount);
            aShip::TShip_TruceWithShip(this, OtherShip);
        };
        std::uint8_t Result = false;
        std::int32_t NextDemandTurn = LastPlayerExtortionTurn + 30;
        if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(OtherShip)) {
            ranger->AddTraderCareerActivity(1);
        }
        if (OtherShip->TruceShip == this) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && PlayerExtortionPactActive) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (this->virtual_TShip_RecomputeFearState() || aShip::TShip_ChanceToWin(this, OtherShip) < 1.0L && GetHullIntegrityPercent() < 40 || aShip::TShip_ChanceToWin(this, OtherShip) < 0.2L || static_cast<long double>(OfferedAmount) > ([&] {
            double winChancePercent = aShip::TShip_GetWinChancePercent(this, OtherShip);
            double wealthScaledAmount = GetWealthScaledAmount(1);
            double wealthScaledAmount_2 = GetWealthScaledAmount(4);
            return aMyFunction::RemapClamped(winChancePercent, 0.0, 1.0E+2, wealthScaledAmount, wealthScaledAmount_2);
        }())) {
            Response = aShip::TShip_LookupVisibleTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Ok"}), OtherShip);
            AcceptPayment();
            return true;
        } else {
            Response = aShip::TShip_LookupVisibleTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        }
    }

    std::uint8_t TTransport_BuildAttackRequestResponse(TTransport* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        std::uint8_t Result{};
        // Nested response helper with caller-popped static link.
        auto AcceptRequest = [&]() -> void {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Ok"}), Requester);
            Self->SetJointAttackTarget(Requester, Target);
            Result = true;
        };
        Result = false;
        if (pas::class_cast_if<aRanger::TRanger*>(Requester) != nullptr) {
            if (pas::in_range(Target->TypeId, static_cast<std::int32_t>(aGalaxyStruct::stRanger), static_cast<std::int32_t>(aGalaxyStruct::stPirate))) {
                Target->ChangeRelationToRanger(Requester, -20);
            }
            if (Target->OwnerId == aGalaxyStruct::oiDominator || Target->TypeId == aGalaxyStruct::stPirate) {
                pas::checked_cast<aRanger::TRanger*>(Requester)->AddWarriorCareerActivity(1);
            } else {
                pas::checked_cast<aRanger::TRanger*>(Requester)->AddPirateCareerActivity(8);
            }
        }
        if (Self->OrderTarget == Target && aShip::TShip_GetRelationLevelToShip(Self, Target) == aGalaxyStruct::rlHostile) {
            AcceptRequest();
        } else if (Self->TruceShip == Target) {
            Response = ([&] {
                pas::WideString name = Target->GetName();
                pas::WideString lookupVisibleTalkText = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Attack.WeAlreadyHavePact"_wref.get(), Requester);
                pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                return aMyFunction::FormatText1(std::move(lookupVisibleTalkText), std::move(textHighlightColorTag), u"<Target>"_w, std::move(name));
            }());
        } else if (aShip::TShip_RelationToShip(Self, Target) >= aGalaxyStruct::RelationNormalMin) {
            if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(Target); !(tranclucator != nullptr)) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"WeFriends"}), Requester);
            } else if (tranclucator->OwnerShip == Self) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"ItsMyTranc"}), Requester);
            } else if (tranclucator->OwnerShip == Requester) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"ItsYourTranc"}), Requester);
            } else {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"WeFriendsTranc"}), Requester);
            }
        } else if (!Self->virtual_TShip_TrustsAttackRequester(Requester)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Suspect"}), Requester);
        } else if (Self->InFear || Self->virtual_TShip_AcceptsRansomDemandFrom(Target)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Fear"}), Requester);
        } else if (Self->HasLockedOrFollowOrder()) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else {
            AcceptRequest();
        }
        return Result;
    }

    std::uint8_t TTransport_AcceptPartnershipOffer(TTransport* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    std::uint8_t TTransport_BuildPartnershipOfferResponse(TTransport* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    float TTransport::AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) {
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        float MoneyPenalty{};
        float WeightPenalty{};
        float FragilityScale{};
        std::int32_t Price{};
        float DesiredMoneyFraction{};
        float HullValueScale{};
        switch (TransportType) {
            case ttTransport: {
                DesiredMoneyFraction = 0.2f;
                FragilityScale = 0.8f;
                HullValueScale = 0.1f;
                break;
            }
            case ttLiner: {
                DesiredMoneyFraction = 0.14f;
                FragilityScale = 1.0f;
                HullValueScale = 0.5f;
                break;
            }
            case ttDiplomat: {
                DesiredMoneyFraction = 0.1f;
                FragilityScale = 1.2f;
                HullValueScale = 1.0f;
                break;
            }
            default: {
                DesiredMoneyFraction = 1.0f;
                FragilityScale = 1.0f;
                HullValueScale = 1.0f;
                break;
            }
        }
        if (pas::is_one_of<aConst::t_FuelTanks, aConst::t_Radar, aConst::t_Scaner, aConst::t_CargoHook>(Item->ItemType)) {
            FragilityScale = FragilityScale * 0.5L;
        }
        if (StrengthInAverageRanger < 0.3L) {
            DesiredMoneyFraction = DesiredMoneyFraction * 0.7L;
        }
        if (StrengthInAverageRanger > 0.9L) {
            DesiredMoneyFraction = DesiredMoneyFraction * 1.5L;
        }
        DesiredMoneyFraction = pas::real_min<pas::Extended>(0.99L, pas::real_max<pas::Extended>(0.01L, (([&] {
            std::int32_t cpp_left = aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) + 1;
            pas::Extended cpp_right = cpp_left + aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfPirates);
            return pas::real_divide(aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition), cpp_right);
        }()) + 0.1L) * DesiredMoneyFraction));
        float DesiredFreeFraction = pas::real_max<pas::Extended>(0.01L, pas::real_min<pas::Extended>(0.99L, pas::real_divide(GetDesiredCargoFreeSpace(), std::max<std::int32_t>(100, GetHull()->Weight))));
        {
            float real_max_3 = pas::real_max<float>(0.01f, SmoothedMoneyFraction);
            {
                pas::Extended inline_value = pas::sqr(([&] {
                    pas::Extended cpp_left_2 = pas::real_divide(1.0L, real_max_3) - 1.0L;
                    return pas::real_divide(cpp_left_2, pas::real_divide(1.0L, DesiredMoneyFraction) - 1.0L);
                }()));
                MoneyPenalty = pas::real_divide(inline_value, pas::real_max<pas::Extended>(SmoothedWealth * 0.05L, 1.0E+3L));
            }
        }
        float EffectivenessScale = pas::real_divide(2.0L, pas::real_max<float>(1.0E+1f, SmoothedEquipmentEffectiveness));
        {
            float real_max_6 = pas::real_max<float>(0.01f, SmoothedFreeCapacityFraction);
            {
                pas::Extended inline_value_2 = pas::sqr(([&] {
                    pas::Extended cpp_left_3 = pas::real_divide(1.0L, real_max_6) - 1.0L;
                    return pas::real_divide(cpp_left_3, pas::real_divide(1.0L, DesiredFreeFraction) - 1.0L);
                }()));
                WeightPenalty = pas::real_divide(inline_value_2, pas::real_max<pas::Extended>(1.0E+1L, GetHull()->Weight * 0.1L));
            }
        }
        if (Item->OwnerId == OwnerId && TransportType == ttDiplomat) {
            EffectivenessScale = EffectivenessScale * 1.1L;
        }
        MoneyPenalty = MoneyPenalty * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-15, 15, Id + Seed));
        EffectivenessScale = EffectivenessScale * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-15, 15, Seed + 3 * Id));
        WeightPenalty = WeightPenalty * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-15, 15, Seed + 5 * Id));
        switch (PriceMode) {
            case 4: Price = Item->Cost; break;
            case 3: Price = Item->CalculateResaleValue(GetEffectiveSkillLevel(aGalaxyStruct::psTrading, false)); break;
            case 1: Price = -Item->Cost; break;
            case 2: Price = 0; break;
            case 0: {
                Price = 0;
                WeightPenalty = 0.0f;
                FragilityScale = 0.0f;
                break;
            }
            default: {
                Price = 0;
                WeightPenalty = 0.0f;
                break;
            }
        }
        if (Item->ItemType != aConst::t_Hull) {
            return static_cast<long double>(Effectiveness) * EffectivenessScale * (1.0L + (2.0L - reinterpret_cast<aItem::TEquipment*>(Item)->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags))) * FragilityScale) - static_cast<long double>(Item->Weight) * WeightPenalty - static_cast<long double>(Price) * MoneyPenalty;
        }
        return (pas::real_divide(static_cast<long double>(Item->Weight) * HullValueScale, pas::real_max<float>(0.01f, reinterpret_cast<aItem::TEquipment*>(Item)->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags)))) + Effectiveness) * EffectivenessScale + static_cast<long double>(Item->Weight) * WeightPenalty - Price * 0.25L * MoneyPenalty;
    }

    float TTransport::EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        float Result = 0.0f;
        if (Value == 0) {
            return Result;
        }
        {
            aConst::TEquipmentBonusKind cpp_case = BonusKind;
            if (cpp_case == aConst::bonHull) {
                Result = Value * 150;
            } else if (cpp_case == aConst::bonFuel) {
                Result = Value * 3.5L;
            } else if (cpp_case == aConst::bonSpeed) {
                Result = Value * 1.1L;
            } else if (cpp_case == aConst::bonJump) {
                Result = Value * 30;
            } else if (cpp_case == aConst::bonRadar) {
                Result = Value * 0.07L;
            } else if (cpp_case == aConst::bonScan) {
                Result = Value * 3 + Value * 20 * CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags));
            } else if (cpp_case == aConst::bonDroid) {
                Result = pas::real_divide(Value * 8, pas::real_max<float>(0.1f, GetHull()->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags))));
            } else if (cpp_case == aConst::bonDef) {
                Result = pas::real_divide(pas::real_divide(Value * 6 * 100, std::max<std::int32_t>(5, 100 - Value)) * 45.0L, std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                Result = Value * 9;
            } else if (cpp_case == aConst::bonWSplinter) {
                Result = Value * 9;
            } else if (cpp_case == aConst::bonWMissile) {
                Result = Value * 9 * (0.1L + static_cast<std::int8_t>(GetRadarRange() > 0) * 0.9L);
            } else if (cpp_case == aConst::bonWRadius) {
                Result = Value * 1.2L * pas::sqr(pas::real_divide(std::max<std::int32_t>(100, SmoothedEnemySpeed), std::max<std::int32_t>(100, SmoothedSpeed)));
            } else if (cpp_case == aConst::bonMass) {
                Result = aMyFunction::RemapClamped(Value + GetHull()->Weight * 0.2L, aConst::HullMassEvaluationStart, aConst::HullMassEvaluationEnd, 1.0, 0.333) * 5.5E+3L;
            } else if (cpp_case == aConst::bonSlotRadar) {
                if (GetSlotCount(aConst::sskRadar) == 0 && Value > 0) {
                    Result = TransportSlotBonusWeights[BonusKind] * 0.3L;
                } else if (GetRadar() != nullptr && Value < 0) {
                    Result = -TransportSlotBonusWeights[BonusKind] - TransportSlotBonusWeights[aConst::bonSlotWeapon] * CountMissileWeapons();
                } else if (GetSlotCount(aConst::sskRadar) == 1 && Value < 0) {
                    Result = TransportSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotDroid) {
                if (GetSlotCount(aConst::sskRepairRobot) == 0 && Value > 0) {
                    Result = TransportSlotBonusWeights[BonusKind] * 0.3L;
                } else if (GetRepairRobot() != nullptr && Value < 0) {
                    Result = -TransportSlotBonusWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskRepairRobot) == 1 && Value < 0) {
                    Result = TransportSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotDef) {
                if (GetSlotCount(aConst::sskDefGenerator) == 0 && Value > 0) {
                    Result = TransportSlotBonusWeights[BonusKind] * 0.3L;
                } else if (GetDefGenerator() != nullptr && Value < 0) {
                    Result = -TransportSlotBonusWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskDefGenerator) == 1 && Value < 0) {
                    Result = TransportSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotWeapon) {
                if (GetSlotCount(aConst::sskWeapon) < 5 && Value > 0) {
                    Result = std::min<std::int32_t>(Value, 5 - GetSlotCount(aConst::sskWeapon)) * TransportSlotBonusWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -GetSlotCount(aConst::sskWeapon)) * TransportSlotBonusWeights[BonusKind];
                }
                {
                    std::int32_t max_6 = std::max<std::int32_t>(Value + GetSlotCount(aConst::sskWeapon), 1);
                    if (CountEquippedWeapons() > max_6) {
                        std::int32_t max_7 = std::max<std::int32_t>(1, Value + GetSlotCount(aConst::sskWeapon));
                        Result = Result - TransportSlotBonusWeights[BonusKind] * 0.6L * (CountEquippedWeapons() - max_7);
                    }
                }
            } else if (cpp_case >= aConst::bonSkill1 && cpp_case <= aConst::bonSkill6) {
                if (Value > 0) {
                    Result = std::min<std::int32_t>(6 - GetEffectiveSkillLevel(aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false), Value) * TransportSkillBonusWeights[BonusKind];
                }
                if (Value > 0 && Value + GetEffectiveSkillLevel(aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) > 6) {
                    Result = Result + TransportSkillBonusWeights[BonusKind] * 0.05L * (Value + GetEffectiveSkillLevel(aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) - 6);
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(static_cast<std::int32_t>(GetEffectiveSkillLevel(aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false)), -Value) * -TransportSkillBonusWeights[BonusKind];
                }
                if (Value < 0 && Value + GetEffectiveSkillLevel(aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) < 0) {
                    Result = Result + TransportSkillBonusWeights[BonusKind] * 0.03L * (Value + GetEffectiveSkillLevel(aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false));
                }
            } else {
                Result = 0.0f;
            }
        }
        if (TransportType == ttDiplomat && pas::in_set<aConst::bonSpeed, aConst::bonSpeed, aConst::bonWEnergy, aConst::bonWRadius, aConst::bonSlotWeapon, aConst::bonSlotWeapon, aConst::bonSkill5, aConst::bonSkill5, aConst::bonMass, aConst::bonMass>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (TransportType == ttTransport && pas::is_one_of<aConst::bonFuel, aConst::bonJump>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (TransportType == ttLiner && pas::is_one_of<aConst::bonHull, aConst::bonRadar, aConst::bonDroid, aConst::bonDef>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonSkill1), static_cast<std::int32_t>(aConst::bonSkill6))) {
            pas::Extended cpp_left = Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-50, 50, Seed + 131 * BonusKind));
            return cpp_left * aConst::RaceSkillEvaluationFactors[PilotRace][aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1]];
        }
        return Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Seed + 131 * BonusKind));
    }

    float TTransport::EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> ShockFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkShock}});
        static const pas::Set<0, 255> AcidFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkAcid}});
        float ScannerFactor{};
        aGalaxyStruct::TDamageFlagSet Flags{};
        std::int32_t I{};
        std::int32_t ShotTotal{};
        Flags = Weapon->GetDamageFlags();
        if (Flags * static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags) != pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}) && GetScanner() != nullptr && GetRadar() != nullptr) {
            std::int32_t cpp_right = aItem::DefenseDamageFactorToPercent(aItem::GetGeneratedDefenseDamageFactor(aGalaxy::Galaxy->TechLevel));
            ScannerFactor = aMyFunction::RemapClamped(GetScannerPower() - cpp_right + 1, -5.0, 1.0E+1, 0.1, 2.0);
        } else {
            ScannerFactor = 0.0f;
        }
        float Result = static_cast<long double>(BaseDamage) * GetWeaponArtefactDamageFactor(Weapon);
        if (pas::contains(Flags, aGalaxyStruct::dkDestruct)) {
            Result = Result * 1.1L;
        }
        if (pas::contains(Flags, aGalaxyStruct::dkDrain)) {
            Result = Result * 1.5L;
        }
        if (pas::contains(Flags, aGalaxyStruct::dkShock)) {
            Result = Result * (1.1L + CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ShockFlags)) * 0.05L);
        }
        float StatusFactor = 1.0f;
        if (pas::contains(Flags, aGalaxyStruct::dkScanBonus)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkBonusToDamaged)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkReduceEngine)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        StatusFactor = StatusFactor - 1.0L;
        if (IncludeAdditiveBonuses) {
            if (pas::contains(Flags, aGalaxyStruct::dkReduceEngine)) {
                Result = Result + ScannerFactor * 1.0E+1L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkBlockWeapon)) {
                Result = Result + ScannerFactor * 1.0E+1L;
            }
            {
                std::int32_t cpp_left = CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(AcidFlags));
                Result = static_cast<long double>(Result) + cpp_left * Weapon->GetShotCount();
            }
            if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
                ShotTotal = 1;
                {
                    const std::int32_t cpp_last = static_cast<std::int32_t>(CountEquippedWeapons());
                    if (1 <= cpp_last) {
                        for (I = 1; I <= cpp_last; ++I) {
                            ShotTotal += Weapons[I]->GetShotCount();
                        }
                    }
                }
                Result = static_cast<long double>(Result) + ShotTotal * 2;
            }
        }
        float SpeedFactor = pas::real_divide(std::max<std::int32_t>(100, SmoothedEnemySpeed) * GetHull()->Weight, aConst::HullBaseSize * pas::real_max<pas::Extended>(1.0E+2L, static_cast<long double>(SmoothedSpeed) * aConst::EquipmentSizeFactors[1]));
        switch (Weapon->GetWeaponInfo()->ShotType) {
            case aGalaxyStruct::wstRocket: {
                Result = Result * 1.0L * Weapon->GetShotCount() * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstMissile: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.2L * 0.01L + StatusFactor) * Weapon->GetShotCount();
                break;
            }
            case aGalaxyStruct::wstTorpedo: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.2L * 0.01L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstChain: {
                Result = Result * (1.1L + (Weapon->GetShotCount() - 1) * 0.2L) * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstSplash: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.2L * 0.01L * SpeedFactor + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstAreaDamage: {
                Result = Result * (1.0L + Weapon->Range * 0.16L * 0.01L * SpeedFactor + StatusFactor);
                break;
            }
            default: Result = Result * (1.0L + StatusFactor); break;
        }
        Result = static_cast<long double>(Result) * Weapon->GetAttackCount();
        return Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Seed + Weapon->GetWeaponInfo()->TypeHash));
    }

    void TTransport_RefreshCurrentStanding(TTransport* Self) {
        aGalaxyStruct::TScriptStandingOverrideMode StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            if (Self->CurrentSystemKills.Pirate > 0 && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                Self->CurrentStanding = aGalaxyStruct::ssCoalitionActive;
            } else {
                Self->CurrentStanding = aGalaxyStruct::ssCoalitionPassive;
            }
        }
    }

    void TTransport::p_destroy() {
        aTransport::TTransport_Destroy(this);
    }

    void TTransport::virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aTransport::TTransport_ResolveLoadedReferences(this, Galaxy);
    }

    void TTransport::virtual_TShip_NextDay() {
        aTransport::TTransport_NextDay(this);
    }

    void TTransport::virtual_TShip_NextDayLogic() {
        aTransport::TTransport_NextDayLogic(this);
    }

    std::uint8_t TTransport::virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return aTransport::TTransport_CanQueueReachablePlanet(this, Planet);
    }

    std::uint8_t TTransport::virtual_TShip_RecomputeFearState() {
        return aTransport::TTransport_RecomputeFearState(this);
    }

    std::uint8_t TTransport::virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) {
        return aTransport::TTransport_AcceptsRansomDemandFrom(this, Ship);
    }

    std::uint8_t TTransport::virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) {
        return aTransport::TTransport_TrustsAttackRequester(this, Ship);
    }

    std::uint8_t TTransport::virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        return aTransport::TTransport_BuildMoneyExtortionResponse(this, OtherShip, Response, DemandedAmount);
    }

    std::uint8_t TTransport::virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) {
        return aTransport::TTransport_BuildCargoExtortionResponse(this, OtherShip, Response);
    }

    std::uint8_t TTransport::virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        return aTransport::TTransport_BuildAttackRequestResponse(this, Requester, Response, Target);
    }

    std::uint8_t TTransport::virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aTransport::TTransport_AcceptPartnershipOffer(this, OtherShip, Response, PaymentAmount);
    }

    std::uint8_t TTransport::virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aTransport::TTransport_BuildPartnershipOfferResponse(this, OtherShip, Response, PaymentAmount);
    }

    void TTransport::virtual_TShip_RefreshCurrentStanding() {
        aTransport::TTransport_RefreshCurrentStanding(this);
    }

} // namespace aTransport
