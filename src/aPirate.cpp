#include "layout/aPirate.hpp"
#include "types/Achievements.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aMissile.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "types/aScript.hpp"
#include "types/aTranclucator.hpp"
#include "units/EC_Buf.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPirate.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aShip.hpp"

namespace aPirate {
    // Entries 22..27 of the dispatch table enter. These reads reload
    // the unchanged BonusKind byte at EBP-5; is the base biased by -22*4.
    const pas::Array<std::int32_t, 22, 27> PirateSkillBonusWeights = pas::Array<std::int32_t, 22, 27>{{90, 90, 90, 90, 30, 5}};

    // EvaluateStatBonus dispatch bounds the indexed BonusKind to 13..20.
    const pas::Array<std::int32_t, 13, 20> PirateSlotBonusWeights = pas::Array<std::int32_t, 13, 20>{{100, 150, 150, 400, 150, 100, 20, 100}};

    void TPirate_Destroy(TPirate* Self) {
        if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            --aGalaxy::Galaxy->PirateClanCount;
        } else {
            --aGalaxy::Galaxy->PirateCount;
        }
        aNormalShip::TNormalShip_Destroy(Self);
    }

    // Sets location, money and PirateType; registers the ship with its star.
    void TPirate::InitGenerated(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::uint8_t Kind) {
        std::int32_t I{};
        aRanger::TRanger* Ranger{};
        // Nested helper with caller-popped static link.
        auto SelectUniqueName = [&](EC_BlockPar::TBlockParEC* Config) -> void {
            std::int32_t Index{};
            std::int32_t Attempt{};
            std::int32_t I{};
            std::int32_t J{};
            std::int32_t LastIndex{};
            std::int32_t FirstIndex{};
            std::uint8_t Duplicate{};
            aGalaxy::TStar* Star{};
            aShip::TShip* Ship{};
            EC_BlockPar::TBlockParEC* Block{};
            if (Config != nullptr && Config->CountBlocks(u"Pirate"_wref.get()) != 0) {
                Block = Config->GetBlock(u"Pirate"_wref.get());
                if (aConst::RaceToOwner(this->PilotRace) == this->OwnerId) {
                    Block = Block->GetBlock(aConst::OwnerToSys(this->OwnerId));
                } else {
                    if (Block->CountBlocks(aConst::OwnerToSys(this->OwnerId)) > 0) {
                        Block = Block->GetBlock(aConst::OwnerToSys(this->OwnerId));
                    }
                    if (Block->CountBlocks(aConst::OwnerToSys(aConst::RaceToOwner(this->PilotRace))) > 0) {
                        Block = Block->GetBlock(aConst::OwnerToSys(aConst::RaceToOwner(this->PilotRace)));
                    }
                }
                FirstIndex = 0;
                LastIndex = Block->GetParamCount() - 1;
                Index = aMyFunction::NextRandomIntRange(FirstIndex, LastIndex, this->RandomState);
                for (auto cpp_range = pas::for_to<std::int32_t>(FirstIndex, LastIndex); cpp_range.next(Attempt); ) {
                    this->Name = Block->GetParamValue(Index);
                    Duplicate = false;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
                        Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                            Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                            if (this != Ship && Ship->TypeId == aGalaxyStruct::stPirate && pas::checked_cast<TPirate*>(Ship)->Name == this->Name) {
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
                        this->Name = pas::concat_wide({this->Name, u" ", u"-", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(this->Id) % 100 + 1)), u"-"});
                    }
                }
            }
        };
        std::uint8_t HasHull = GetHull() != nullptr;
        if (HasHull || Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            ++aGalaxy::Galaxy->PirateClanCount;
        } else {
            ++aGalaxy::Galaxy->PirateCount;
        }
        HomePlanet = Planet;
        if (!HasHull) {
            CurrentPlanet = HomePlanet;
        }
        CurrentStar = HomePlanet->CurrentStar;
        pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(this));
        if (HasHull) {
            OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiPirate);
            if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), GetHull()->OwnerId)) {
                PilotRace = aConst::OwnerToRace(GetHull()->OwnerId);
            } else if (HomePlanet->IsMainPiratePlanet) {
                PilotRace = aConst::OwnerToRace(aConst::PickRandomEquipmentOwner(HomePlanet->RandomState));
            } else {
                PilotRace = HomePlanet->RaceId;
            }
        } else {
            if (HomePlanet->IsMainPiratePlanet) {
                PilotRace = aConst::OwnerToRace(aConst::PickRandomEquipmentOwner(HomePlanet->RandomState));
            } else {
                PilotRace = HomePlanet->RaceId;
            }
            if (HomePlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiPirate);
            } else {
                OwnerId = aConst::RaceToOwner(PilotRace);
            }
        }
        SetMoney(InitialMoney);
        TypeId = aGalaxyStruct::stPirate;
        PirateType = Kind;
        Name = pas::WideString();
        SelectUniqueName(GR_Main::ModShipNameConfig);
        if (GetName().length() == 0) {
            SelectUniqueName(GR_Main::LanguageDataConfig->GetBlock(u"ShipName"_wref.get()));
        }
        if (aPlayer::GetPlayer() != nullptr) {
            Rank = aMyFunction::NextRandomIntRange(0, aPlayer::GetPlayer()->Rank, RandomState);
            if (Rank > 3) {
                Rank = 3;
            }
            {
                std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, static_cast<std::int32_t>(aConst::CoalitionRankPointThresholds[Rank]) / 2, RandomState);
                aNormalShip::TNormalShip* self = this;
                self->AddRankPoints(nextRandomIntRange);
            }
            if (!aGalaxy::Galaxy->IsZeroStartingExperienceEnabled()) {
                GainExperience(System::Round(aMyFunction::RemapClamped(static_cast<std::int8_t>(Rank + static_cast<std::uint8_t>(0)), 0.0, 3.0, aConst::TotalSkillTrainingCost / 8, aConst::TotalSkillTrainingCost / 2)), 0);
            }
            if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PirateRank = aMyFunction::NextRandomIntRange(0, aPlayer::GetPlayer()->PirateRank, RandomState);
                if (PirateRank > 3) {
                    PirateRank = 3;
                }
                {
                    std::uint32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(0, static_cast<std::int32_t>(aConst::PirateRankPointThresholds[PirateRank]) / 2, RandomState);
                    aNormalShip::TNormalShip* self_2 = this;
                    self_2->AddPirateRankPoints(nextRandomIntRange_2);
                }
            }
        }
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        if (!HasHull) {
            if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                std::uint8_t raceToOwner = aConst::RaceToOwner(PilotRace);
                std::int32_t selectRandomHullSeries = aShip::TShip_SelectRandomHullSeries(this);
                std::uint16_t round = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]);
                aShip::TShip* self_3 = this;
                aShip::TShip_CreateAndEquipHull(self_3, round, 1, raceToOwner, selectRandomHullSeries, true);
            } else {
                std::uint8_t ownerId = OwnerId;
                std::int32_t selectRandomHullSeries_2 = aShip::TShip_SelectRandomHullSeries(this);
                std::uint16_t round_2 = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]);
                aShip::TShip* self_4 = this;
                aShip::TShip_CreateAndEquipHull(self_4, round_2, 1, ownerId, selectRandomHullSeries_2, false);
            }
            CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
            {
                std::uint8_t ownerId_2 = OwnerId;
                pas::Extended cpp_left = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(1, 2, RandomState)];
                std::int32_t round_3 = System::Round(cpp_left * aConst::EngineBaseSize);
                std::uint8_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(1, 2, RandomState);
                aShip::TShip* self_5 = this;
                self_5->CreateAndEquipEngine(round_3, nextRandomIntRange_3, ownerId_2);
            }
            if (GetSlotCountForItemType(aConst::t_CargoHook) > 0) {
                std::uint8_t ownerId_3 = OwnerId;
                std::uint8_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(1, 2, RandomState);
                std::int32_t cargoHookBaseSize = aConst::CargoHookBaseSize;
                aShip::TShip* self_6 = this;
                self_6->CreateAndEquipCargoHook(cargoHookBaseSize, nextRandomIntRange_4, ownerId_3);
            }
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                CreateAndEquipWeapon(aConst::t_Weapon1, aConst::WeaponInfos[aConst::t_Weapon1].AverageSize, 1, OwnerId);
            }
            if (GetSlotCountForItemType(aConst::t_Radar) > 0) {
                std::uint8_t ownerId_4 = OwnerId;
                pas::Extended cpp_left_2 = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, RandomState)];
                std::int32_t round_4 = System::Round(cpp_left_2 * aConst::RadarBaseSize);
                aShip::TShip* self_7 = this;
                self_7->CreateAndEquipRadar(round_4, 1, ownerId_4);
            }
        } else {
            aShip::TShip_RefreshGraphic(this);
        }
        RefreshDerivedStats(true);
        this->virtual_TShip_RefreshCurrentStanding();
        SmoothedSpeed = Speed;
        SmoothedEnemySpeed = Speed;
        if (!HasHull) {
            aShip::TShip_BuyEquipmentAtLocation(this, true);
            aShip::TShip_BuyEquipmentAtLocation(this, true);
            aShip::TShip_BuyEquipmentAtLocation(this, true);
            if (PirateType != 0) {
                aShip::TShip_BuyEquipmentAtLocation(this, true);
            }
            if (PirateType != 0) {
                aShip::TShip_BuyEquipmentAtLocation(this, true);
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
            pas::list_add(RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(std::min<std::int32_t>(50, aConst::OwnerRelations[OwnerId][Ranger->OwnerId] - 20)))));
        }
        PrisonTermRemaining = 0u;
        RaidPressure = 0.0f;
    }

    void TPirate::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aNormalShip::TNormalShip::SaveToBuffer(Buffer);
        Buffer->AddDWord(PrisonTermRemaining);
        Buffer->AddAnsiChar(PirateType);
        Buffer->AddSingle(RaidPressure);
    }

    void TPirate::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aNormalShip::TNormalShip::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion >= 60) {
            PrisonTermRemaining = EC_Buf::TBufEC_GetUInt32(Buffer);
        } else {
            PrisonTermRemaining = EC_Buf::TBufEC_GetByte(Buffer);
        }
        PirateType = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 79) {
            RaidPressure = EC_Buf::TBufEC_GetSingle(Buffer);
        } else {
            RaidPressure = 0.0f;
        }
    }

    void TPirate_NextDay(TPirate* Self) {
        aNormalShip::TNormalShip_NextDay(Self);
        try {
            if (Self->ScriptShip != nullptr && Self->HasScriptControl()) {
                aShip::TShip_ScriptNextDay(Self);
                if (Self->ScriptShip != nullptr) {
                    if (Self->PrisonTermRemaining > 0) {
                        Self->ProcessImprisonment();
                    }
                    return;
                }
            }
            Self->virtual_TShip_NextDayLogic();
            if (Self->ScriptShip != nullptr && static_cast<std::uint8_t>(Self->HasScriptControl() ^ 1)) {
                aShip::TShip_ScriptNextDay(Self);
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TPirate.NextDay ", Self->GetFullName(u" "_wref.get())}))));
            } else {
                throw;
            }
        }
    }

    void TPirate_NextDayLogic(TPirate* Self) {
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Station{};
        aShip::TShip* Ship{};
        std::uint8_t ClanShip{};
        std::uint8_t Collecting{};
        std::int32_t I{};
        std::int32_t Count{};
        std::int32_t Stage = 0;
        try {
            ClanShip = Self->PirateType != 0 && Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate);
            if (Self->CurrentPlanet != nullptr) {
                Stage = 1;
                if (static_cast<std::uint8_t>(pas::in_set<0, 4, 7, 7>(Self->CurrentPlanet->OwnerId) ^ 1) || ClanShip && static_cast<std::uint8_t>(pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.PirateClan), Self->CurrentPlanet->OwnerId) ^ 1)) {
                    Self->OrderTakeoff();
                    return;
                }
                Stage = 2;
                if (!ClanShip) {
                    if (Self->ProcessImprisonment()) {
                        return;
                    }
                }
                Self->LastDockedPlanet = Self->CurrentPlanet;
                Self->virtual_TShip_RepairBrokenEquipmentAtLocation();
                Self->AutoEquipInventory();
                Stage = 3;
                Self->OptimizeInventory();
                Self->SellAllCargoGoods();
                Self->RefuelAtLocation();
                Self->ReloadWeaponAmmo();
                if (Self->RepairHullAtLocation()) {
                    return;
                }
                Stage = 4;
                aShip::TShip_BuyEquipmentAtLocation(Self, false);
                Self->RestoreEssentialEquipment();
                Self->ProcessUnseenProgression();
                aNormalShip::TNormalShip_TrainSkillsAutomatically(Self);
                Self->OrderTakeoff();
                return;
            }
            if (Self->DockedTo != nullptr) {
                Stage = 5;
                if (!pas::in_range(Self->DockedTo->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                    if (Self->DockedTo->InNormalSpace()) {
                        Self->OrderTakeoff();
                    } else {
                        Self->OrderNone(false);
                    }
                    return;
                }
                Self->SynchronizeDockedLocation();
                Self->virtual_TShip_RepairBrokenEquipmentAtLocation();
                Self->AutoEquipInventory();
                Stage = 6;
                Self->OptimizeInventory();
                Self->SellAllCargoGoods();
                Self->RefuelAtLocation();
                Self->ReloadWeaponAmmo();
                aNormalShip::TNormalShip_TrainSkillsAutomatically(Self);
                if (Self->RepairHullAtLocation() && static_cast<std::uint8_t>(ClanShip ^ 1)) {
                    return;
                }
                Stage = 7;
                aShip::TShip_BuyEquipmentAtLocation(Self, false);
                Self->RestoreEssentialEquipment();
                Self->ProcessUnseenProgression();
                aNormalShip::TNormalShip_TrainSkillsAutomatically(Self);
                if (Self->DockedTo->InNormalSpace() && (reinterpret_cast<aRuins::TRuins*>(Self->DockedTo)->FlyToStar == nullptr || reinterpret_cast<aRuins::TRuins*>(Self->DockedTo)->FlyToStar == Self->CurrentStar)) {
                    Self->OrderTakeoff();
                } else {
                    if ((reinterpret_cast<aRuins::TRuins*>(Self->DockedTo)->FlyToStar == nullptr || reinterpret_cast<aRuins::TRuins*>(Self->DockedTo)->FlyToStar->Dominion != Self->DockedTo) && (Self->DockedTo->OrderTarget == nullptr || !(pas::class_cast_if<aGalaxy::TStar*>(Self->DockedTo->OrderTarget) != nullptr) || reinterpret_cast<aGalaxy::TStar*>(Self->DockedTo->OrderTarget)->Dominion != Self->DockedTo)) {
                        Self->RaidPressure = 0.05L + Self->RaidPressure;
                    }
                    Self->OrderNone(false);
                }
                return;
            }
            if (Self->InNormalSpace()) {
                Stage = 8;
                Self->AutoApplyMicroModules();
                Self->BuildReachablePlanetQueue();
                if (Self->virtual_TShip_RecomputeFearState()) {
                    aPirate::TPirate_TryOfferRansomToPursuer(Self);
                }
                Self->ProcessCombatDialogue();
                Stage = 9;
                Self->virtual_TShip_AssignWeaponTargetsInStar();
                Self->ReviewPartnership();
                Self->AfterburnerActive = false;
                if (ClanShip) {
                    Stage = 10;
                    Self->SelectEnemyShipInStar();
                    Self->EngageEnemyShip();
                    Self->virtual_TShip_RecomputeFearState();
                }
                Stage = 11;
                if (Self->TryRetreatFromSystem()) {
                    return;
                }
                if (Self->InFear && (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates || static_cast<std::uint8_t>(ClanShip ^ 1))) {
                    Stage = 12;
                    if (static_cast<std::uint8_t>(pas::is_one_of<aShip::soLand, aShip::soJump>(Self->Order) ^ 1) && static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1)) {
                        Stage = 13;
                        if (ClanShip) {
                            Planet = Self->SelectNearestQueuedPlanet();
                            if (Planet != nullptr && Planet->CurrentStar == Self->CurrentStar) {
                                Self->OrderLanding(Planet, true);
                            } else {
                                Station = Self->FindNearestDockableStation(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::NonTargetableStationStandingMasks[aGalaxyStruct::sfPirates]));
                                if (Station != nullptr) {
                                    Self->OrderLanding(Station, true);
                                } else {
                                    Self->EngageEnemyShip();
                                }
                            }
                        } else {
                            if (aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.8L) {
                                Self->TryDockAtStation(pas::constant_set<aGalaxyStruct::TShipTypeMask>({{6, 13}}));
                            }
                            if (Self->Order != aShip::soLand) {
                                if (aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.66L) {
                                    Self->NavigateToEscapePlanet(true);
                                } else {
                                    Self->NavigateToQueuedPlanet(true);
                                }
                            }
                            if (pas::is_one_of<aShip::soLand, aShip::soJump>(Self->Order) && Self->EstimateOrderTravelTurns() > 4 && Self->EnemyShip != nullptr && Self->EnemyShip->OrderTarget == Self && pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Self->EnemyShip->TypeId) && Self->EnemyShip->EstimateOrderTravelTurns() < 3 && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L && static_cast<std::int32_t>(Self->Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 2 == 0) {
                                if (Self->JettisonCargoGoodsTowardTargetValue(std::max<std::int32_t>(200, aGalaxy::Galaxy->ComputeScaledMiniMoney(Self->OwnerId)))) {
                                    Self->NotifyFearCargoDrop(Self->EnemyShip);
                                }
                            }
                        }
                    }
                    Stage = 14;
                    if (pas::is_one_of<aShip::soLand, aShip::soJump>(Self->Order)) {
                        Self->UpdateAfterburnerState();
                    } else if (ClanShip) {
                        Self->EngageEnemyShip();
                    } else if (Self->Order == aShip::soNone) {
                        Self->SelectEnemyShipInStar();
                        Self->EngageEnemyShip();
                    }
                } else {
                    Stage = 15;
                    aShip::TShip_QueueItemsWithinPickupRange(Self);
                    Stage = 16;
                    if (Self->PartnerShip != nullptr) {
                        if (!Self->OrderAbsolute) {
                            if (aShip::TShip_TryMirrorPartnerTravelOrders(Self)) {
                                return;
                            }
                        } else {
                            return;
                        }
                    }
                    Stage = 17;
                    if (!ClanShip) {
                        Stage = 18;
                        Collecting = aShip::TShip_TryCollectBestFloatingItem(Self, 50);
                        if (static_cast<std::uint8_t>(Collecting ^ 1) && Self->GetDesiredCargoFreeSpace() > Self->CargoFreeSpace) {
                            if (aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.8L) {
                                Self->TryDockAtStation(pas::constant_set<aGalaxyStruct::TShipTypeMask>({{6, 13}}));
                            } else {
                                Self->NavigateToQueuedPlanet(true);
                            }
                        }
                        Stage = 19;
                        if (Self->EnemyShip != nullptr && Self->OrderTarget == Self->EnemyShip && Self->EnemyShip->CurrentPlanet != nullptr && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L) {
                            Self->OrderNone(false);
                        }
                        if (static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1) && static_cast<std::uint8_t>(Collecting ^ 1)) {
                            Stage = 20;
                            Self->SelectEnemyShipInStar();
                            Self->EngageEnemyShip();
                            if (Self->Order == aShip::soNone && static_cast<std::uint8_t>(Self->InFear ^ 1)) {
                                Self->TryJumpToNearbyBattle(0);
                            }
                            if (Self->Order == aShip::soNone) {
                                Self->NavigateToServicePlanet(false);
                            }
                        }
                    }
                }
                Stage = 21;
                if (ClanShip) {
                    if (Self->Order == aShip::soNone && Self->HasHullDamageOrBrokenEquippedItems()) {
                        Self->NavigateToServicePlanet(false);
                    }
                    Stage = 22;
                    if (Self->Order == aShip::soNone && static_cast<std::uint8_t>(aShip::TShip_TryCollectBestFloatingItem(Self, 50) ^ 1) && (Self->GetDesiredCargoFreeSpace() > Self->CargoFreeSpace || aMyFunction::NextRandomIntRange(1, 10, Self->RandomState) <= 2)) {
                        Self->NavigateToQueuedPlanet(true);
                    }
                    if (Self->Order == aShip::soMove && Self->PickupTargets == nullptr && aMyFunction::NextRandomIntRange(1, 10, Self->RandomState) <= 3) {
                        aShip::TShip_TryCollectBestFloatingItem(Self, 50);
                    }
                    Stage = 23;
                    if (Self->Order == aShip::soNone && aMyFunction::NextRandomIntRange(1, 10, Self->RandomState) <= 2) {
                        Station = Self->FindNearestDockableStation(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::NonTargetableStationStandingMasks[aGalaxyStruct::sfPirates]));
                        if (Station != nullptr) {
                            Self->OrderLanding(Station, true);
                        }
                    }
                    Stage = 24;
                    if (Self->Order == aShip::soNone) {
                        Self->SelectIncidentalEnemy();
                    }
                }
                Stage = 25;
                if (Self->CurrentStar->Dominion != nullptr && reinterpret_cast<aShip::TShip*>(Self->CurrentStar->Dominion)->virtual_TShip_CanDock(Self) && reinterpret_cast<aRuins::TRuins*>(Self->CurrentStar->Dominion)->FlyToStar != nullptr && Self->CurrentStar->Status.Battle == 0) {
                    Count = 0;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                        Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                        if (static_cast<std::uint8_t>(Ship->InHyperspace ^ 1) && pas::class_cast_if<TPirate*>(Ship) != nullptr && Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && Ship->PartnerShip == nullptr && static_cast<std::uint8_t>(Ship->HasScriptControl() ^ 1) && Ship != Self && Ship->DockedTo != Self->CurrentStar->Dominion) {
                            ++Count;
                        }
                    }
                    Stage = 26;
                    if (Count >= 3) {
                        Self->OrderLanding(Self->CurrentStar->Dominion, false);
                        if (aGalaxy::Galaxy->CurrentTurn + Self->EstimateOrderTravelTurns() + 1 > reinterpret_cast<aRuins::TRuins*>(Self->CurrentStar->Dominion)->FlyDate) {
                            Self->UpdateAfterburnerState();
                        }
                    }
                }
                Stage = 27;
                if (Self->CurrentStar->Dominion != nullptr && reinterpret_cast<aShip::TShip*>(Self->CurrentStar->Dominion)->InNormalSpace() && pas::is_one_of<aShip::soNone, aShip::soMove>(Self->Order)) {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                        Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                        if (Ship->InHyperspace) {
                            if (Ship->AbductedByPirateClan) {
                                Self->OrderMove(reinterpret_cast<aShip::TShip*>(Self->CurrentStar->Dominion)->Position, false);
                                break;
                            }
                        }
                    }
                }
                Stage = 28;
                if (Self->Order == aShip::soNone) {
                    if (ClanShip) {
                        Self->MoveToRandomPlanetOrbit();
                    } else {
                        Self->OrderRandomFreeFlightMove();
                    }
                }
                return;
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TPirate.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    std::uint8_t TPirate::NavigateToServicePlanet(std::uint8_t Absolute) {
        aPlanet::TPlanet* Planet{};
        std::uint8_t Result = false;
        if (PirateType != 0) {
            BuildReachablePlanetQueue();
            if (pas::list_count(PlanetQueue) > 0) {
                {
                    std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(PlanetQueue) - 1, RandomState);
                    pas::List* planetQueue = PlanetQueue;
                    pas::Object* cpp_arg = pas::list_at<pas::Object>(planetQueue, nextRandomIntRange);
                    aShip::TShip* self = this;
                    self->OrderLanding(cpp_arg, false);
                }
                return true;
            }
        } else {
            Planet = SelectServicePlanet();
            if (Planet != nullptr) {
                if (CurrentStar == Planet->CurrentStar) {
                    OrderLanding(Planet, Absolute);
                } else {
                    OrderJump(Planet->CurrentStar, Absolute);
                }
                return true;
            }
        }
        return Result;
    }

    aPlanet::TPlanet* TPirate::SelectServicePlanet() {
        aPlanet::TPlanet* Result{};
        if (pas::list_count(PlanetQueue) > 0) {
            Result = pas::list_at<aPlanet::TPlanet>(reinterpret_cast<pas::List*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(PlanetQueue)) + 0))), 0);
            if (GetFuelTanks()->Fuel < GetFuelTanks()->Capacity || HasHullDamageOrBrokenEquippedItems() || aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L) {
                return Result;
            }
            std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(PlanetQueue) - 1, RandomState);
            pas::List* planetQueue = PlanetQueue;
            return pas::list_at<aPlanet::TPlanet>(planetQueue, nextRandomIntRange);
        }
        return nullptr;
    }

    void TPirate::BuildReachablePlanetQueue() {
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TStar* Star{};
        ClearPlanetQueue();
        PlanetQueue = pas::make_object<pas::List>();
        if (Speed == 0) {
            return;
        }
        if (PirateType != 0) {
            if (CurrentStar->Status.CustomFaction == u"") {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Planets) - 1); cpp_range.next(I); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                    if (Planet->OwnerId == OwnerId) {
                        pas::list_add(PlanetQueue, reinterpret_cast<void*>(Planet));
                    }
                }
            }
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[I].Star));
                if (I > 0 && CurrentStar->StarDistances[I].Distance > JumpRange) {
                    break;
                }
                if (Star->Constellation->Id != 20) {
                    if (Star->Status.CustomFaction != u"") {
                        return;
                    }
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_3.next(J); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                        if (pas::in_set<0, 4, 7, 7>(Planet->OwnerId) && this->virtual_TShip_CanQueueReachablePlanet(Planet)) {
                            pas::list_add(PlanetQueue, reinterpret_cast<void*>(Planet));
                        }
                    }
                }
            }
        }
    }

    // AI ownership check only; does not test travel range.
    std::uint8_t TPirate_CanQueueReachablePlanet(TPirate* Self, aPlanet::TPlanet* Planet) {
        return Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && (Self->PirateType == 0 || Planet->OwnerId == Self->OwnerId);
    }

    // The native entry stores but never reads dl; the sole caller passes zero.
    void TPirate::TryJumpToNearbyBattle(std::uint8_t UnusedMode) {
        std::int32_t I{};
        aGalaxy::TStar* Star{};
        std::uint8_t Good{};
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (CargoGoods[Good].Count > 0) {
                return;
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            if (CurrentStar->StarDistances[I].Distance > JumpRange) {
                break;
            }
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[I].Star));
            if (Star->Status.Battle != 0 && (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition || GetFuelTanks()->Fuel / 2 > CurrentStar->StarDistances[I].Distance) && pas::list_count(Star->Ships) - 3 >= Star->ShipTypeCounts[aGalaxyStruct::stKling] && pas::list_count(Star->Ships) < 13) {
                OrderJump(Star, false);
                return;
            }
        }
    }

    std::uint8_t TPirate::TryDockAtStation(aGalaxyStruct::TShipTypeMask Types) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (pas::contains(Types, Ship->TypeId) && Ship->InNormalSpace() && Ship->virtual_TShip_CanDock(this)) {
                OrderLanding(Ship, true);
                return true;
            }
        }
        return false;
    }

    // Prefers the leader's route, then nearby service locations or reachable non-Dominator stars.
    void TPirate::SelectNearestReachableDestination() {
        std::int32_t I{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        std::int32_t Turns{};
        std::int32_t CandidateTurns{};
        pas::Object* Target{};
        std::int32_t BestTurns{};
        if (PartnerShip != nullptr) {
            if (PartnerShip->CurrentStar == CurrentStar) {
                if (Order == aShip::soLand && PartnerShip->OrderTarget == OrderTarget) {
                    return;
                }
                if (PartnerShip->Order == aShip::soLand) {
                    if (CanRefuel() || HasCargoGoods() && GetDesiredCargoFreeSpace() > CargoFreeSpace || pas::class_cast_if<aRuins::TRuins*>(PartnerShip->OrderTarget) != nullptr || ([&] {
                        std::int32_t cpp_left = GetHull()->Weight - GetDesiredCargoFreeSpace();
                        return cpp_left < GetCarriedItemWeight();
                    }()) || GetHullIntegrityPercent() < 70 || HasHullDamageOrBrokenEquippedItems()) {
                        if (!(pas::class_cast_if<aRuins::TRuins*>(PartnerShip->OrderTarget) != nullptr) || pas::checked_cast<aRuins::TRuins*>(PartnerShip->OrderTarget)->virtual_TShip_CanDock(this)) {
                            OrderLanding(PartnerShip->OrderTarget, true);
                        }
                        return;
                    }
                } else if (PartnerShip->Order == aShip::soJump) {
                    Star = pas::checked_cast<aGalaxy::TStar*>(PartnerShip->OrderTarget);
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"") {
                        OrderJump(Star, false);
                        return;
                    }
                }
            } else if (PartnerShip->Order == aShip::soJump && pas::class_cast_if<aGalaxy::TStar*>(PartnerShip->OrderTarget) != nullptr && PartnerShip->OrderTarget != CurrentStar) {
                Star = pas::checked_cast<aGalaxy::TStar*>(PartnerShip->OrderTarget);
                if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"") {
                    OrderJump(Star, true);
                    return;
                }
            } else {
                Star = PartnerShip->CurrentStar;
                if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"") {
                    OrderJump(Star, true);
                    return;
                }
            }
        }
        if (pas::is_one_of<aShip::soLand, aShip::soJump, aShip::soJumpHole>(Order)) {
            Turns = EstimateOrderTravelTurns();
            Target = OrderTarget;
            BestTurns = Turns;
        } else {
            Turns = 1000;
            Target = nullptr;
            BestTurns = Turns;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
            if (this->virtual_TShip_CanQueueReachablePlanet(Planet) && pas::in_set<0, 4, 7, 7>(Planet->OwnerId)) {
                CandidateTurns = EstimateTravelTurnsToObject(Planet);
                if (BestTurns > CandidateTurns) {
                    BestTurns = CandidateTurns;
                    Target = Planet;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Ship->virtual_TShip_CanDock(this) && Ship->EnemyShip != this && EnemyShip != Ship) {
                CandidateTurns = EstimateTravelTurnsToObject(Ship);
                if (BestTurns > CandidateTurns) {
                    BestTurns = CandidateTurns;
                    Target = Ship;
                }
            }
        }
        if (!(pas::class_cast_if<aPlanet::TPlanet*>(Target) != nullptr) && !(pas::class_cast_if<aShip::TShip*>(Target) != nullptr)) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_3.next(I); ) {
                if (CurrentStar->StarDistances[I].Distance > JumpRange) {
                    break;
                }
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[I].Star));
                if (Star->Status.ControlFaction != aGalaxyStruct::sfDominators && Star->Status.CustomFaction == u"") {
                    CandidateTurns = EstimateTravelTurnsToObject(Star);
                    if (BestTurns > CandidateTurns) {
                        BestTurns = CandidateTurns;
                        Target = Star;
                    }
                }
            }
        }
        if (Target != nullptr && Target != OrderTarget) {
            if (pas::class_cast_if<aPlanet::TPlanet*>(Target) != nullptr) {
                OrderLanding(Target, false);
            } else if (pas::class_cast_if<aShip::TShip*>(Target) != nullptr) {
                OrderLanding(Target, false);
            } else if (pas::class_cast_if<aGalaxy::TStar*>(Target) != nullptr) {
                OrderJump(pas::checked_cast<aGalaxy::TStar*>(Target), false);
            }
        }
    }

    void TPirate::SellAllCargoGoods() {
        std::uint8_t Good{};
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (CargoGoods[Good].Count > 0) {
                SellGoodsToLocation(Good, CargoGoods[Good].Count);
            }
        }
    }

    void TPirate_RepairBrokenEquipmentAtLocation(TPirate* Self) {
        std::int32_t I{};
        std::int32_t Cost{};
        aItem::TEquipment* Equipment{};
        aItem::TEquipment* Artefact{};
        if (Self->PirateType != 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Self->Inventory) - 1); cpp_range.next(I); ) {
                Equipment = pas::list_at<aItem::TEquipment>(Self->Inventory, I);
                if ((!(pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && aShip::TShip_CanRepairEquipmentTech(Self, Equipment) && (Equipment->BrokenFlag != 0 || Equipment->ConditionPercent < 3.0E+1L) && Equipment->EquippedFlag != 0) {
                    Equipment->Repair();
                }
            }
            if (Self->CanRepairArtefactsAtLocation()) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Artefacts) - 1); cpp_range_2.next(I); ) {
                    Artefact = pas::list_at<aItem::TEquipment>(Self->Artefacts, I);
                    if (Artefact->BrokenFlag != 0 || Artefact->ConditionPercent < 3.0E+1L) {
                        if (Artefact->EquippedFlag != 0) {
                            Artefact->Repair();
                        }
                    }
                }
            }
        } else {
            {
                const std::int32_t cpp_first = pas::list_count(Self->Inventory) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        Equipment = pas::list_at<aItem::TEquipment>(Self->Inventory, I);
                        if ((!(pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && aShip::TShip_CanRepairEquipmentTech(Self, Equipment) && (Equipment->BrokenFlag != 0 || Equipment->ConditionPercent < 5.0E+1L) && Equipment->EquippedFlag != 0) {
                            Cost = aItem::TEquipment_CalculateRepairCost(Equipment);
                            if (Self->Money > Cost) {
                                Self->SetMoney(Self->Money - Cost);
                            }
                            Equipment->Repair();
                        }
                    }
                }
            }
            if (Self->CanRepairArtefactsAtLocation()) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Artefacts) - 1); cpp_range_3.next(I); ) {
                    Artefact = pas::list_at<aItem::TEquipment>(Self->Artefacts, I);
                    if ((Artefact->BrokenFlag != 0 || Artefact->ConditionPercent < 5.0E+1L) && Artefact->EquippedFlag != 0) {
                        Cost = aItem::TEquipment_CalculateRepairCost(Artefact);
                        if (Self->Money > Cost) {
                            Self->SetMoney(Self->Money - Cost);
                        }
                        Artefact->Repair();
                    }
                }
            }
        }
    }

    aGalaxy::TStar* TPirate::GetHomeStar() {
        return HomePlanet->CurrentStar;
    }

    pas::WideString TPirate::GetName() {
        return Name;
    }

    pas::WideString TPirate::GetFullName(const pas::WideString& Separator) {
        pas::WideString Path{};
        pas::WideString RaceName{};
        pas::WideString Text{};
        RaceName = aConst::OwnerToSys(aConst::RaceToOwner(PilotRace));
        if (TypeNameOverrideKey != u"") {
            Path = pas::concat_wide({u"ShipType.", RaceName, u".", TypeNameOverrideKey});
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
        return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"ShipType.", RaceName, u".", GetTypeNameKey()})), Separator, Name});
    }

    std::uint8_t TPirate::GetGreetingShipCategory() {
        if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            return aGalaxyStruct::gscPirateClan;
        }
        return aGalaxyStruct::gscPirate;
    }

    // Always rcPirate.
    aGalaxyStruct::TRangerCareer TPirate::GetDominantCareer() {
        return aGalaxyStruct::rcPirate;
    }

    std::uint8_t TPirate::GetStrengthScaledPirateStatus() {
        return 50;
    }

    std::int32_t TPirate::GetDesiredCargoFreeSpace() {
        if (PirateType != 0) {
            return System::Trunc(aMyFunction::RemapClamped(GetHull()->Weight, static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5], static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[1], 1.0E+1, 5.0E+1));
        }
        return System::Trunc(aMyFunction::RemapClamped(GetHull()->Weight, static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5], static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[1], 3.0E+1, 1.5E+2));
    }

    // Fills installed fuel tanks without charging Money.
    void TPirate::RefuelAtLocation() {
        if (GetFuelTanks() != nullptr) {
            GetFuelTanks()->Fuel = GetFuelTanks()->Capacity;
        }
    }

    void TPirate::ProcessUnseenProgression() {
        std::uint8_t Award{};
        double ProgressFactor{};
        double StrengthFactor{};
        if (DaysSincePlayerSeen >= 60 && aPlayer::GetPlayer() != nullptr) {
            if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && PirateType != 0) {
                ProgressFactor = static_cast<std::int8_t>(aGalaxy::Galaxy->DifficultyLevels[0] + static_cast<std::uint8_t>(0)) * 0.2L + 0.6L;
                ProgressFactor = static_cast<long double>(aMyFunction::RemapClamped(aGalaxy::Galaxy->WarDeltaWin[2], -1.0E+1, 1.0E+1, 1.3, 0.7)) * ProgressFactor;
                StrengthFactor = static_cast<std::int8_t>(aGalaxy::Galaxy->DifficultyLevels[0] + static_cast<std::uint8_t>(0)) * 0.1L + 0.6L;
                if (0.1L * ProgressFactor > aMyFunction::NextRandomUnitFloat(RandomState) && (0.4L * StrengthFactor > StrengthInBestRanger || aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L)) {
                    if (([&] {
                        pas::Extended cpp_left = aMyFunction::NextRandomFloatRange(0.0, 0.7, RandomState);
                        return cpp_left > WealthInBestRanger;
                    }()) && Money < 25000) {
                        SetMoney(Money + aGalaxy::Galaxy->ComputeScaledBigMoney(2));
                    } else {
                        aShip::TShip_ImproveRandomEquipment(this, true);
                    }
                }
                if (0.05L * ProgressFactor > aMyFunction::NextRandomUnitFloat(RandomState) && (0.3L * StrengthFactor > StrengthInBestRanger || 0.7L * StrengthFactor > StrengthInAverageRanger || 0.001L * ProgressFactor > aMyFunction::NextRandomUnitFloat(RandomState))) {
                    aShip::TShip_GenerateExtraWeapon(this);
                }
                if (0.05L * ProgressFactor > aMyFunction::NextRandomUnitFloat(RandomState)) {
                    std::int32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(100, 500, RandomState);
                    aShip::TShip* self = this;
                    self->GainExperience(seededRandomIntRange, 0);
                }
                if (aPlayer::GetPlayer()->PirateRank > PirateRank && aMyFunction::NextRandomUnitFloat(RandomState) < 0.01L && PirateRank < 4) {
                    // Native adds Coalition rank points before attempting a pirate promotion.
                    {
                        std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(16, 32, RandomState);
                        aNormalShip::TNormalShip* self_2 = this;
                        self_2->AddRankPoints(nextRandomIntRange);
                    }
                    TryPromotePirateRank();
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.02L && CurrentPlanet != nullptr && (AwardIds == nullptr || 2 * (Rank + 1) > pas::list_count(AwardIds))) {
                    Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}, {aGalaxyStruct::atSecretMission}, {aGalaxyStruct::atPerfidy}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                    if (Award != aGalaxyStruct::AwardNotFound) {
                        AddAward(Award);
                    }
                }
            } else {
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L && (StrengthInBestRanger < 0.7L || aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L)) {
                    if (([&] {
                        pas::Extended cpp_left_2 = aMyFunction::NextRandomFloatRange(0.0, 0.7, RandomState);
                        return cpp_left_2 > WealthInBestRanger;
                    }()) && Money < 25000) {
                        SetMoney(Money + aGalaxy::Galaxy->ComputeScaledBigMoney(2));
                    } else {
                        aShip::TShip_ImproveRandomEquipment(this, true);
                    }
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L && (StrengthInBestRanger < 0.5L || StrengthInAverageRanger < 1.0L || aMyFunction::NextRandomUnitFloat(RandomState) < 0.001L)) {
                    aShip::TShip_GenerateExtraWeapon(this);
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L) {
                    std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(250, 1000, RandomState);
                    aShip::TShip* self_3 = this;
                    self_3->GainExperience(nextRandomIntRange_2, 0);
                }
                if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aPlayer::GetPlayer()->PirateRank > PirateRank && aMyFunction::NextRandomUnitFloat(RandomState) < 0.01L && PirateRank < 4) {
                    {
                        std::uint16_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(16, 32, RandomState);
                        aNormalShip::TNormalShip* self_4 = this;
                        self_4->AddRankPoints(nextRandomIntRange_3);
                    }
                    TryPromotePirateRank();
                }
                if (aPlayer::GetPlayer()->Rank > Rank && aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L && Rank < 4) {
                    {
                        std::uint16_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(10, 20, RandomState);
                        aNormalShip::TNormalShip* self_5 = this;
                        self_5->AddRankPoints(nextRandomIntRange_4);
                    }
                    TryPromoteRank();
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.03L && CurrentPlanet != nullptr && (AwardIds == nullptr || 2 * (Rank + 1) > pas::list_count(AwardIds))) {
                    Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment, aGalaxyStruct::atPerfidy}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                    if (Award != aGalaxyStruct::AwardNotFound) {
                        AddAward(Award);
                    }
                }
            }
        }
    }

    std::uint8_t TPirate::RelationToNonRanger(aShip::TShip* Ship) {
        if (CurrentStanding == aGalaxyStruct::ssPirateMilitary) {
            std::uint8_t cpp_case = Ship->TypeId;
            if (cpp_case == aGalaxyStruct::stTransport) {
                if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition && CurrentStar->Status.CustomFaction == u"" && CurrentStar == Ship->CurrentStar && TruceShip != Ship && Ship->TruceShip != this && static_cast<std::uint8_t>(IsHullDestroyed() ^ 1) && static_cast<std::uint8_t>(this->virtual_TShip_AcceptsRansomDemandFrom(Ship) ^ 1)) {
                    return 0;
                }
                return std::min<std::int32_t>(50, std::max<std::int32_t>(20, aConst::OwnerRelations[OwnerId][Ship->OwnerId] - 20));
            } else if (cpp_case == aGalaxyStruct::stPirate) {
                if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    return std::min<std::int32_t>(100, 100 + aMyFunction::SeededRandomIntRange(-20, 20, Seed + Ship->Seed));
                }
                return std::max<std::int32_t>(50, static_cast<std::int32_t>(aConst::OwnerRelations[OwnerId][Ship->OwnerId]));
            } else if (cpp_case == aGalaxyStruct::stWarrior) {
                return std::min<std::int32_t>(50, std::max<std::int32_t>(10, *([&] {
                    auto cpp_index = aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f;
                    auto* cpp_array = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                    return &(*cpp_array)[cpp_index];
                }()) - 30));
            } else if (cpp_case == aGalaxyStruct::stKling || cpp_case == aGalaxyStruct::stTranclucator) {
                return 50;
            } else if (cpp_case >= aGalaxyStruct::rstRangerCenter && cpp_case <= aGalaxyStruct::rstCustomStation) {
                if (pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding)) {
                    return 30;
                }
                return 100;
            } else {
                return 0;
            }
        }
        if (CurrentStanding == aGalaxyStruct::ssPirateActive && Ship->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && Ship->CurrentStar->Status.CustomFaction == u"" && pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding)) {
            return 0;
        }
        {
            std::uint8_t cpp_case_2 = Ship->TypeId;
            if (cpp_case_2 == aGalaxyStruct::stTransport) {
                return std::min<std::int32_t>(50, std::max<std::int32_t>(20, *([&] {
                    auto cpp_index_4 = aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f;
                    auto* cpp_array_4 = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                    return &(*cpp_array_4)[cpp_index_4];
                }()) - 20));
            } else if (cpp_case_2 == aGalaxyStruct::stPirate) {
                return std::max<std::int32_t>(60, static_cast<std::int32_t>(*([&] {
                    auto cpp_index_3 = aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f;
                    auto* cpp_array_3 = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                    return &(*cpp_array_3)[cpp_index_3];
                }())));
            } else if (cpp_case_2 == aGalaxyStruct::stWarrior) {
                return std::min<std::int32_t>(50, std::max<std::int32_t>(10, *([&] {
                    auto cpp_index_2 = aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f;
                    auto* cpp_array_2 = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                    return &(*cpp_array_2)[cpp_index_2];
                }()) - 30));
            } else if (cpp_case_2 == aGalaxyStruct::stKling || cpp_case_2 == aGalaxyStruct::stTranclucator) {
                return 50;
            } else if (cpp_case_2 >= aGalaxyStruct::rstRangerCenter && cpp_case_2 <= aGalaxyStruct::rstCustomStation) {
                if (pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding) && OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    return 30;
                }
                return 100;
            } else {
                return 50;
            }
        }
    }

    std::uint8_t TPirate::RelationToRanger(void* Ranger) {
        std::uint8_t Result = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, Ranger))));
        if (PirateType != 0) {
            if (aPlanet::MainPiratePlanet != nullptr && aPlanet::MainPiratePlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                return std::min<std::int32_t>(static_cast<std::int32_t>(Result), aPlanet::MainPiratePlanet->RelationToRanger(pas::list_indexof(aGalaxy::Galaxy->Rangers, Ranger)));
            }
            return 0;
        }
        return Result;
    }

    void TPirate::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))));
        std::uint8_t Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index)));
        if (static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aShip::psCharisma, false) > 0 && Amount > 0) {
            Amount += System::Round(Amount * (static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.2L);
        }
        std::int32_t Value = std::max<std::int32_t>(0, std::min<std::int32_t>(100, Amount + Relation));
        Relation = Value;
        pas::list_put(RangerRelations, Index, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Relation))));
        if (Relation < 10 && (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar)) {
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

    void TPirate::ReactToAttack(aShip::TShip* Attacker) {
        std::int32_t Index{};
        std::int32_t Value{};
        if (pas::class_cast_if<aTranclucator::TTranclucator*>(Attacker) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip->TypeId == aGalaxyStruct::stRanger) {
            ReactToAttack(reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip);
        }
        EnemyShip = Attacker;
        if (pas::class_cast_if<aRanger::TRanger*>(Attacker) != nullptr) {
            ChangeRelationToRanger(Attacker, -10);
            if (pas::is_one_of<aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary>(CurrentStanding) && pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Attacker->CurrentStanding) && aPlanet::MainPiratePlanet != nullptr && aPlanet::MainPiratePlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                Index = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(Attacker));
                Value = std::max<std::int32_t>(0, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aPlanet::MainPiratePlanet->RangerRelations, Index))) - 3);
                pas::list_put(aPlanet::MainPiratePlanet->RangerRelations, Index, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Value))));
            }
        }
        if (Attacker->PartnerShip != nullptr) {
            if (Attacker->PartnerShip->TypeId == aGalaxyStruct::stRanger) {
                ChangeRelationToRanger(Attacker->PartnerShip, -5);
                if (pas::is_one_of<aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary>(CurrentStanding) && pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Attacker->PartnerShip->CurrentStanding) && aPlanet::MainPiratePlanet != nullptr && aPlanet::MainPiratePlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    Index = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(Attacker->PartnerShip));
                    Value = std::max<std::int32_t>(0, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aPlanet::MainPiratePlanet->RangerRelations, Index))) - 1);
                    pas::list_put(aPlanet::MainPiratePlanet->RangerRelations, Index, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Value))));
                }
            }
        }
    }

    std::uint8_t TPirate_RecomputeFearState(TPirate* Self) {
        std::uint8_t Result{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        std::int32_t EnemyCount{};
        double Tolerance{};
        double Threat{};
        if (aShip::TShip_HasNoUsableWeapons(Self) && Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar == Self->CurrentStar) {
            Result = true;
            Self->InFear = true;
            return Result;
        }
        if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && Self->PirateType != 0) {
            Result = Self->GetHull()->HullPoints < Self->GetHull()->Weight * 0.15L && Self->GetHull()->HullPoints < 100 || Self->GetHull()->HullPoints < 65 || Self->GetHull()->Weight * 0.25L * aConst::OwnerInfo[Self->OwnerId].FearThresholdScale > Self->GetHull()->HullPoints && Self->EnemyShip != nullptr && Self->EnemyShip->OrderTarget == Self && ([&] {
                pas::Extended cpp_left = aShip::TShip_ChanceToWin(Self, Self->EnemyShip);
                return cpp_left - pas::real_divide(aConst::OwnerInfo[Self->OwnerId].FearThresholdScale, 2.0L);
            }()) < 0.0L;
        } else {
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
                        } else if (Ship->EnemyShip == Self && Ship->OrderTarget == Self || aShip::TShip_RelationToShip(Ship, Self) < 10 && aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) < 2.5E+5L) {
                            ++EnemyCount;
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
                if (static_cast<long double>(EnemyCount - 1) * Threat * 0.33L + Threat > Tolerance) {
                    Result = true;
                }
            }
        }
        Self->InFear = Result;
        if (Self->InFear) {
            Self->RaidPressure = 0.5L * Self->RaidPressure;
        }
        return Result;
    }

    void TPirate_TryOfferRansomToPursuer(TPirate* Self) {
        pas::WideString Response{};
        std::int32_t Amount{};
        float LowOffer{};
        float HighOffer{};
        std::uint8_t Accepted{};
        aShip::TShip* Ship{};
        if (Self->InNormalSpace() && Self->EnemyShip != nullptr && Self->EnemyShip->OrderTarget == Self && Self->EnemyShip->TruceShip != Self && Self->Money > 100 && static_cast<std::uint8_t>(Self->CanEscapePursuer(Self->EnemyShip) ^ 1) && static_cast<std::uint8_t>(pas::contains(aConst::NonNegotiatingShipTypes, Self->EnemyShip->TypeId) ^ 1) && static_cast<std::uint8_t>(Self->NoTalk ^ 1) && static_cast<std::uint8_t>(Self->EnemyShip->NoTalk ^ 1) && (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || Self->PirateType == 0 || static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Self->EnemyShip->CurrentStanding) ^ 1))) {
            if (([&] {
                std::int32_t cpp_left_2 = aShip::TShip_GetMaxWeaponRange(Self->EnemyShip);
                pas::Extended cpp_left = cpp_left_2 * aShip::TShip_GetMaxWeaponRange(Self->EnemyShip);
                return cpp_left >= aMyFunction::PointDistanceSquared(Self->Position, Self->EnemyShip->Position);
            }()) && (aGalaxy::Galaxy->CurrentTurn * Self->EnemyShip->Id % 3 == 0 && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.2L || Self->GetHullIntegrityPercent() < 20)) {
                LowOffer = std::min<std::int32_t>(Self->Money, Self->GetWealthScaledAmount(1));
                HighOffer = pas::real_min<pas::Extended>(static_cast<pas::Extended>(Self->Money), ([&] {
                    std::int32_t cpp_left_3 = Self->GetWealthScaledAmount(3);
                    return cpp_left_3 + Self->EnemyShip->GetWealthScaledAmount(4);
                }()) * 0.5L);
                Amount = System::Round(pas::real_max<double>(1.0E+2, aMyFunction::RemapClamped(Self->GetHull()->HullPoints, 0.0, Self->GetHull()->Weight, HighOffer, LowOffer)));
                Ship = Self->EnemyShip;
                Accepted = Ship->BuildTrucePaymentResponse(Self, Response, Amount);
                if (aPlayer::GetPlayer() != Ship && aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar) {
                    Self->NotifyTruceOffer(Ship, Response, Amount);
                }
                if (Accepted && Self->virtual_TShip_RecomputeFearState()) {
                    aPirate::TPirate_TryOfferRansomToPursuer(Self);
                }
            }
        }
    }

    std::uint8_t TPirate_AcceptsRansomDemandFrom(TPirate* Self, aShip::TShip* Ship) {
        std::uint8_t Result = Self->GetHull()->Weight * 0.6L * aConst::OwnerInfo[Self->OwnerId].FearThresholdScale > Self->GetHull()->HullPoints && ([&] {
            pas::Extended cpp_left = aShip::TShip_ChanceToWin(Self, Ship);
            pas::Extended cpp_right = cpp_left * (1.0L + Self->RaidPressure);
            return pas::real_divide(aConst::OwnerInfo[Self->OwnerId].FearThresholdScale, 2.0L) > cpp_right;
        }());
        if (Result && Self->EnemyShip == Ship) {
            Self->RaidPressure = 0.0f;
        }
        return Result;
    }

    std::uint8_t TPirate_TrustsAttackRequester(TPirate* Self, aShip::TShip* Ship) {
        return aShip::TShip_RelationToShip(Self, Ship) >= 30;
    }

    std::uint8_t TPirate_EvaluateAllyRelationAndStrength(TPirate* Self, aShip::TShip* Ship) {
        pas::Extended cpp_right = aMyFunction::RemapClamped(Ship->Strength, 0.9L * Self->Strength, Self->Strength * 3.0L, 0.0, 1.0E+2);
        return (aShip::TShip_RelationToShip(Self, Ship) & 0x0000007f) + cpp_right > 1.3E+2L;
    }

    // True while the current turn is spent in prison.
    std::uint8_t TPirate::ProcessImprisonment() {
        std::uint8_t Result{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        // Nested helper; caller-popped static link.
        auto Imprison = [&]() -> void {
            std::int32_t I{};
            aShip::TShip* Ship{};
            pas::WideString Text{};
            this->PrisonTermRemaining = aMyFunction::NextRandomIntRange(61, 140, this->RandomState);
            this->CurrentSystemKills.Normal = 0;
            this->CurrentSystemKills.Pirate = 0;
            this->EnemyShip = nullptr;
            Result = true;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(this->CurrentPlanet->Warriors) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(this->CurrentPlanet->Warriors, I);
                if (this == Ship->EnemyShip) {
                    Ship->EnemyShip = nullptr;
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(this->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(this->CurrentStar->Ships, I);
                if (this == Ship->EnemyShip) {
                    Ship->EnemyShip = nullptr;
                }
            }
            Text = aConst::PickLocalizedTextVariant(pas::concat_wide({u"GalaxyNews.GoToPrison.", GetTypeNameKey()}), this->Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
            aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, this->CurrentStar->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, this->CurrentPlanet->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Month>"_w, pas::wide_int64_to_str(static_cast<std::int64_t>(this->PrisonTermRemaining / 30)), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, GetName(), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<FullName>"_w, GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
            if (aPlayer::GetPlayer()->CurrentStar == this->CurrentStar && aPlayer::GetPlayer()->InNormalSpace() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
            }
        };
        Result = false;
        if (CurrentPlanet == nullptr) {
            return Result;
        }
        if (PrisonTermRemaining > 0) {
            if (CurrentStar->Status.Battle != 0 && CurrentStar->LastDominatorPresenceTurn >= aGalaxy::Galaxy->CurrentTurn - 1 || CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PrisonTermRemaining = 0u;
                this->virtual_TShip_RefreshCurrentStanding();
                return false;
            }
            --PrisonTermRemaining;
            if (PrisonTermRemaining == 0) {
                Result = false;
                this->virtual_TShip_RefreshCurrentStanding();
                return Result;
            }
            return true;
        }
        if (CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && (CurrentStar->Status.Battle == 0 || CurrentStar->LastDominatorPresenceTurn < aGalaxy::Galaxy->CurrentTurn - 1)) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentPlanet->Warriors) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentPlanet->Warriors, I);
                if (Ship->EnemyShip == this) {
                    Imprison();
                    this->virtual_TShip_RefreshCurrentStanding();
                    return Result;
                }
            }
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.01L || CurrentPlanet->GetRelationLevelToShip(this) == aGalaxyStruct::rlHostile) {
                Imprison();
                this->virtual_TShip_RefreshCurrentStanding();
            }
        }
        return Result;
    }

    void TPirate_AssignWeaponTargetsInStar(TPirate* Self) {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        aItem::TItem* Item{};
        aAsteroid::TAsteroid* Asteroid{};
        float Distance{};
        aMissile::TMissile* Missile{};
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Self->Weapons[I];
                    Weapon->Target = nullptr;
                }
            }
        }
        std::int32_t Assigned = 0;
        if (Self->CurrentStar->Status.Battle != 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && Ship->InNormalSpace()) {
                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Self->WeaponCount);
                    if (1 <= cpp_last_2) {
                        for (J = 1; J <= cpp_last_2; ++J) {
                            Weapon = Self->Weapons[J];
                            if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                                pas::Extended cpp_right = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                                if (aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) <= cpp_right) {
                                    Weapon->Target = Ship;
                                    ++Assigned;
                                    if (Assigned == Self->WeaponCount) {
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar == Self->CurrentStar && Self->EnemyShip->InNormalSpace()) {
            const std::int32_t cpp_last_3 = static_cast<std::int32_t>(Self->WeaponCount);
            if (1 <= cpp_last_3) {
                for (J = 1; J <= cpp_last_3; ++J) {
                    Weapon = Self->Weapons[J];
                    if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                        pas::Extended cpp_right_2 = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                        if (aMyFunction::PointDistanceSquared(Self->Position, Self->EnemyShip->Position) <= cpp_right_2) {
                            Weapon->Target = Self->EnemyShip;
                            ++Assigned;
                            if (Assigned == Self->WeaponCount) {
                                return;
                            }
                        }
                    }
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
            if (Ship->InNormalSpace() && Ship != Self && (aShip::TShip_RelationToShip(Self, Ship) < 10 || Ship == Self->EnemyShip || Ship->EnemyShip == Self) && Self->TruceShip != Ship) {
                const std::int32_t cpp_last_4 = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last_4) {
                    for (J = 1; J <= cpp_last_4; ++J) {
                        Weapon = Self->Weapons[J];
                        // Native hostile-ship pass can replace an earlier weapon target.
                        if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                            pas::Extended cpp_right_3 = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                            if (aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) <= cpp_right_3) {
                                Weapon->Target = Ship;
                                ++Assigned;
                                if (Assigned == Self->WeaponCount) {
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Missiles) - 1); cpp_range_3.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(Self->CurrentStar->Missiles, I);
            if (Missile->Target == Self && Missile->OwnerShip != Self) {
                const std::int32_t cpp_last_5 = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last_5) {
                    for (J = 1; J <= cpp_last_5; ++J) {
                        Weapon = Self->Weapons[J];
                        if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                            pas::Extended cpp_right_4 = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                            if (aMyFunction::PointDistanceSquared(Self->Position, Missile->Position) <= cpp_right_4) {
                                Weapon->Target = Missile;
                                ++Assigned;
                                if (Assigned == Self->WeaponCount) {
                                    return;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (aShip::TShip_IsEquipmentUsable(Self, Self->GetCargoHook()) && static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1)) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Asteroids) - 1); cpp_range_4.next(I); ) {
                Asteroid = pas::list_at<aAsteroid::TAsteroid>(Self->CurrentStar->Asteroids, I);
                if (Asteroid->MineralCount > Self->CargoFreeSpace) {
                    continue;
                }
                Distance = aMyFunction::PointDistanceSquared(Self->Position, Asteroid->Position);
                if (Distance <= 1.0E+6L) {
                    const std::int32_t cpp_last_6 = static_cast<std::int32_t>(Self->WeaponCount);
                    if (1 <= cpp_last_6) {
                        for (J = 1; J <= cpp_last_6; ++J) {
                            Weapon = Self->Weapons[J];
                            // Native asteroid targeting can overwrite an existing assignment.
                            if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                                if (static_cast<long double>(pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon))) >= Distance) {
                                    Weapon->Target = Asteroid;
                                    ++Assigned;
                                    if (Assigned == Self->WeaponCount) {
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
        if (aGalaxy::Galaxy->GetAIJunkToleranceLevel() < pas::list_count(Self->CurrentStar->Items)) {
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Items) - 1); cpp_range_5.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Self->CurrentStar->Items, I);
                if ((Item->ItemType == aConst::t_Minerals || static_cast<std::uint8_t>(pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics)) ^ 1)) && (Item->ItemType != aConst::t_Minerals || static_cast<std::uint8_t>(Self->IsRecentlyDroppedItem(Item) ^ 1)) && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") && static_cast<std::uint8_t>(aItem::CanCargoHookHandleItem(Item, Self) ^ 1)) {
                    if (aPlayer::GetPlayer()->CurrentStar != Self->CurrentStar || aShip::TShip_GetRelationLevelToShip(Self, aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad || aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >= 6.0E+2L || aMyFunction::NextRandomUnitFloat(Self->RandomState) <= 0.2L && aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >= 2.0E+2L) {
                        if (Self->CanSafelyDetonateItem(Item)) {
                            const std::int32_t cpp_last_7 = static_cast<std::int32_t>(Self->WeaponCount);
                            if (1 <= cpp_last_7) {
                                for (J = 1; J <= cpp_last_7; ++J) {
                                    Weapon = Self->Weapons[J];
                                    if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                                        pas::Extended cpp_right_5 = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                                        if (aMyFunction::PointDistanceSquared(Self->Position, Item->Position) <= cpp_right_5) {
                                            Weapon->Target = Item;
                                            ++Assigned;
                                            if (Assigned == Self->WeaponCount) {
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
    }

    void TPirate::SelectEnemyShipInStar() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        aShip::TShip* PreviousEnemy{};
        double Chance{};
        double BestChance{};
        double Distance{};
        double BestDistance{};
        std::uint8_t PriorityTargetFound{};
        if ((GetCargoHook() != nullptr || PirateType != 0) && (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar || EnemyShip->ConsecutiveDockedDays > 3 && PirateType != 0) && UsableWeaponCount != 0) {
            if (aPlayer::GetPlayer()->QuestTargetDefendShip != nullptr) {
                if (PirateType == 0 && aPlayer::GetPlayer()->QuestTargetDefendShip->CurrentStar == CurrentStar && aPlayer::GetPlayer()->QuestTargetDefendShip != this && aPlayer::GetPlayer()->QuestTargetDefendShip->InNormalSpace() && aPlayer::GetPlayer()->QuestTargetDefendShip != TruceShip && aPlayer::GetPlayer()->QuestTargetDefendShip->ScriptShip == nullptr) {
                    EnemyShip = aPlayer::GetPlayer()->QuestTargetDefendShip;
                    this->virtual_TShip_AssignWeaponTargetsInStar();
                    return;
                }
            }
            PriorityTargetFound = false;
            if (PirateType == 0) {
                PreviousEnemy = EnemyShip;
                EnemyShip = nullptr;
                BestChance = 0.0;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (PriorityTargetFound && Ship->TargetingRestriction != 6) {
                        continue;
                    }
                    if (pas::in_set<1, 3, 5, 5>(Ship->TargetingRestriction) || Ship == this || static_cast<std::uint8_t>(Ship->InNormalSpace() ^ 1) || TruceShip == Ship || Ship->TruceShip == this || aPlayer::GetPlayer() == Ship && aPlayer::GetPlayer()->TruceShip == this || Ship->LiberationGroup != nullptr || aPlayer::GetPlayer()->QuestTargetKillShip == Ship || pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && (OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || CurrentStar->Status.Battle == 0 || pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssNeutral, aGalaxyStruct::ssPirateMilitary)) || Ship->TypeId == aGalaxyStruct::stTranclucator) {
                        continue;
                    }
                    if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators || CurrentStar->Status.CustomFaction != u"" || CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                        if (!(pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr) || OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && Ship->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                            EnemyShip = Ship;
                            if (aShip::TShip_ChanceToWin(this, Ship) > 0.5L) {
                                return;
                            }
                        }
                    } else if (Ship->TargetingRestriction == 6 && static_cast<std::uint8_t>(PriorityTargetFound ^ 1)) {
                        BestChance = aShip::TShip_ChanceToWin(this, Ship);
                        PriorityTargetFound = true;
                        EnemyShip = Ship;
                    } else {
                        if (([&] {
                            std::int32_t cpp_left = aMyFunction::NextRandomIntRange(0, 30, RandomState) + 60;
                            return cpp_left < (aShip::TShip_RelationToShip(this, Ship) & 0x0000007f);
                        }()) && static_cast<std::uint8_t>(Ship->AbductedByPirateClan ^ 1)) {
                            continue;
                        }
                        Chance = aShip::TShip_ChanceToWin(this, Ship);
                        if (aPlayer::GetPlayer() == Ship) {
                            if (aGalaxy::Galaxy->CurrentTurn < pas::real_divide(1.0E+2L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor) + 3.0E+2L || ([&] {
                                pas::Extended cpp_left_2 = aMyFunction::NextRandomIntRange(0, 100, RandomState);
                                return cpp_left_2 * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor;
                            }()) < 4.0E+1L) {
                                continue;
                            }
                            Chance = static_cast<long double>(Chance) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor;
                        } else if (aPlayer::GetPlayer()->QuestTargetDefendShip == Ship) {
                            Chance = Chance * 5.0L;
                        }
                        if (Chance < 0.3L && StrengthInAverageRanger < 0.7L) {
                            continue;
                        }
                        if (Chance < 1.5L && Ship->TypeId == aGalaxyStruct::stRanger && Ship->GetDominantCareer() != aGalaxyStruct::rcPirate) {
                            continue;
                        }
                        if (Chance < 3.5L && Ship->TypeId == aGalaxyStruct::stRanger && Ship->GetDominantCareer() == aGalaxyStruct::rcPirate) {
                            continue;
                        }
                        if (Chance < 3.5L && Ship->TypeId == aGalaxyStruct::stPirate) {
                            continue;
                        }
                        if (static_cast<std::uint8_t>(IsTargetStillPursuable(Ship) ^ 1) && aPlayer::GetPlayer() != Ship && aPlayer::GetPlayer()->QuestTargetDefendShip != Ship) {
                            continue;
                        }
                        if (Chance > BestChance) {
                            EnemyShip = Ship;
                            BestChance = Chance;
                            if (aPlayer::GetPlayer() == EnemyShip) {
                                break;
                            }
                        }
                    }
                }
                if (EnemyShip != nullptr) {
                    if (EnemyShip->TargetingRestriction == 6 || static_cast<std::uint8_t>(aShip::TShip_CanContactShip(this, EnemyShip) ^ 1)) {
                        this->virtual_TShip_AssignWeaponTargetsInStar();
                        return;
                    }
                    if (!aShip::TShip_TryExtortShip(this, EnemyShip)) {
                        this->virtual_TShip_AssignWeaponTargetsInStar();
                        return;
                    }
                }
                EnemyShip = PreviousEnemy;
            } else if (UsableWeaponCount != 0) {
                EnemyShip = nullptr;
                BestDistance = 1.0E+5;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (Ship->InNormalSpace() && TruceShip != Ship && Ship->TruceShip != this && (static_cast<std::uint8_t>(pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) ^ 1) || static_cast<std::uint8_t>(pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssNeutral, aGalaxyStruct::ssPirateMilitary) ^ 1)) && (aShip::TShip_RelationToShip(this, Ship) < 10 || Ship->AbductedByPirateClan)) {
                        Distance = aMyFunction::PointDistance(Position, Ship->Position);
                        if (static_cast<long double>(aMyFunction::NextRandomFloatRange(0.3, 3.0, RandomState)) * BestDistance > Distance) {
                            EnemyShip = Ship;
                            BestDistance = Distance;
                        }
                    }
                }
            }
        }
    }

    void TPirate::SelectIncidentalEnemy() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        double Distance{};
        EnemyShip = nullptr;
        double BestDistance = 1.0E+5;
        std::int32_t Aggression = System::Round(aMyFunction::RemapClamped(aGalaxy::TGalaxy::GetCoalitionToPirateSystemRatio(), 0.5, 1.0, 1.0E+2, -2.0E+2));
        {
            pas::Extended cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left < aMyFunction::RemapClamped(CurrentStar->CountPirateShips(true), 3.0E+1, 6.0E+1, 0.0, 1.0E+2) * (1.0L + RaidPressure * 0.1L)) {
                Aggression = 100;
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (Ship->InNormalSpace() && TruceShip != Ship && static_cast<std::uint8_t>(pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) ^ 1) && Ship->TypeId != aGalaxyStruct::stTranclucator && Ship != this && static_cast<std::uint8_t>(pas::in_set<1, 3, 5, 5>(Ship->TargetingRestriction) ^ 1) && static_cast<std::uint8_t>(this->virtual_TShip_AcceptsRansomDemandFrom(Ship) ^ 1) && (aShip::TShip_RelationToShip(this, Ship) < 80 || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= Aggression - 1) && (aShip::TShip_RelationToShip(this, Ship) < 60 || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= Aggression + 33) && (aShip::TShip_RelationToShip(this, Ship) < 30 || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= Aggression + 66) && (aShip::TShip_RelationToShip(this, Ship) < 10 || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= Aggression + 100) && (!(pas::class_cast_if<TPirate*>(Ship) != nullptr) && aPlayer::GetPlayer() != Ship && (Ship->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || !(pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr)) || (Ship->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || Ship->PilotRace != PilotRace || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= 50) && (static_cast<std::int8_t>(pas::checked_cast<aNormalShip::TNormalShip*>(Ship)->PirateRank) <= static_cast<std::int8_t>(PirateRank + static_cast<std::uint8_t>(0)) || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= 70) && (pas::checked_cast<aNormalShip::TNormalShip*>(Ship)->PirateRank <= PirateRank + 1 || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= 70) && (pas::checked_cast<aNormalShip::TNormalShip*>(Ship)->PirateRank <= PirateRank + 2 || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= 70) && (pas::checked_cast<aNormalShip::TNormalShip*>(Ship)->PirateRank != 7 || aMyFunction::NextRandomIntRange(1, 100, RandomState) <= 50))) {
                Distance = aMyFunction::PointDistance(Position, Ship->Position);
                if (static_cast<long double>(aMyFunction::NextRandomFloatRange(0.3, 3.0, RandomState)) * BestDistance > Distance) {
                    EnemyShip = Ship;
                    BestDistance = Distance;
                }
            }
        }
        if (EnemyShip == nullptr) {
            RaidPressure = std::max<std::int32_t>(Aggression, 0) * 0.001L + RaidPressure;
        }
    }

    void TPirate::EngageEnemyShip() {
        if (Order == aShip::soFollowShip) {
            OrderNone(false);
        }
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar) {
            if (EnemyShip->InNormalSpace()) {
                OrderFollowShip(EnemyShip, 1, false);
                if (aShip::TShip_ChanceToWin(this, EnemyShip) < 0.8L) {
                    aShip::TShip_RequestAlliesAttackShip(this, EnemyShip);
                }
            } else if (aShip::TShip_ChanceToWin(this, EnemyShip) > 2.0L && GetHullIntegrityPercent() > 70 && EnemyShip->GetHullIntegrityPercent() > 70) {
                if (EnemyShip->CurrentPlanet != nullptr) {
                    if (pas::class_cast_if<aRanger::TRanger*>(EnemyShip) != nullptr && static_cast<std::uint32_t>(static_cast<aRanger::TRanger*>(EnemyShip)->PrisonTermRemaining) > 0) {
                        EnemyShip = nullptr;
                        OrderNone(false);
                    } else if (pas::class_cast_if<TPirate*>(EnemyShip) != nullptr && static_cast<TPirate*>(EnemyShip)->PrisonTermRemaining > 0) {
                        EnemyShip = nullptr;
                        OrderNone(false);
                    } else {
                        OrderMove(EnemyShip->CurrentPlanet->GetPosition(), false);
                    }
                } else if (EnemyShip->DockedTo != nullptr) {
                    OrderMove(EnemyShip->DockedTo->Position, false);
                }
            }
        }
    }

    void TPirate::ProcessCombatDialogue() {
        if (EnemyShip != nullptr && OrderTarget == EnemyShip && static_cast<std::int32_t>(Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 5 == 0 && static_cast<std::uint8_t>(this->virtual_TShip_AcceptsRansomDemandFrom(EnemyShip) ^ 1)) {
            aShip::TShip_TryExtortShip(this, EnemyShip);
        }
        if (EnemyShip != nullptr && OrderTarget == EnemyShip && static_cast<std::int32_t>(Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 4 == 0 && (aShip::TShip_ChanceToWin(this, EnemyShip) < 1.1L && GetHullIntegrityPercent() > 30 || OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && CurrentStar->Status.CustomFaction == u"" && aShip::TShip_ChanceToWin(this, EnemyShip) < 2.0L)) {
            aShip::TShip_RequestAlliesAttackShip(this, EnemyShip);
        }
    }

    void TPirate::ReactToExtortionDemand(void* Ranger) {
        if (aPlayer::GetPlayer() == Ranger || aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L) {
            ChangeRelationToRanger(Ranger, -15);
            HomePlanet->ChangeRelationToRanger(Ranger, -1);
            pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))->AddPirateCareerActivity(1);
        }
    }

    std::uint8_t TPirate_BuildMoneyExtortionResponse(TPirate* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        // Nested helper with caller-popped static link.
        auto PayDemand = [&]() -> void {
            aGalaxyEvent::TGalaxyEvent* Event{};
            OtherShip->SetMoney(OtherShip->Money + DemandedAmount);
            Self->SetMoney(Self->Money - DemandedAmount);
            aShip::TShip_TruceWithShip(OtherShip, Self);
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
            }
        };
        std::uint8_t Result = false;
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
        } else if (Self->UnknownVirtualC0(OtherShip) || static_cast<std::uint8_t>(Self->virtual_TShip_AcceptsRansomDemandFrom(OtherShip) ^ 1)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        } else if (Self->CanEscapePursuer(OtherShip)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"LongDistance"}), OtherShip);
            return Result;
        } else {
            double cpp_arg = aShip::TShip_GetWinChancePercent(Self, OtherShip) & 0x0000007f;
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

    std::uint8_t TPirate_BuildCargoExtortionResponse(TPirate* Self, aShip::TShip* OtherShip, pas::WideString& Response) {
        // Nested helper with caller-popped static link.
        auto DropDemand = [&]() -> void {
            std::uint8_t Good{};
            std::int32_t Pass{};
            std::int32_t Count{};
            float Divisor{};
            aGalaxyEvent::TGalaxyEvent* Event{};
            std::int32_t TotalValue = 0;
            std::uint8_t Enough = false;
            std::int32_t LowValue = Self->GetWealthScaledAmount(2);
            std::int32_t HighValue = Self->GetWealthScaledAmount(4);
            for (Pass = 1; Pass <= 3; ++Pass) {
                for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
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
        };
        std::uint8_t Result = false;
        std::uint8_t Forced = aPlayer::GetPlayer() == OtherShip && OtherShip->IsHealthEffectActive(14);
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
        } else if (Self->UnknownVirtualC0(OtherShip) || !(Self->virtual_TShip_AcceptsRansomDemandFrom(OtherShip) || Forced)) {
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

    std::uint8_t TPirate::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) {
        // Nested helper with caller-popped static link.
        auto AcceptPayment = [&]() -> void {
            std::int32_t I{};
            aShip::TShip* Ship{};
            OtherShip->AbductedByPirateClan = false;
            if (this->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && this->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && this->CurrentStar->Status.CustomFaction == u"" && pas::class_cast_if<aNormalShip::TNormalShip*>(OtherShip) != nullptr) {
                if (reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->CurrentSystemKills.Pirate > 0) {
                    reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->CurrentSystemKills.Pirate = 0;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(this->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                        Ship = pas::list_at<aShip::TShip>(this->CurrentStar->Ships, I);
                        if (pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr && Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                            aShip::TShip_TruceWithShip(Ship, OtherShip);
                        }
                    }
                }
            }
            if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr) {
                if (aPlanet::MainPiratePlanet != nullptr && aPlanet::MainPiratePlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    aPlanet::MainPiratePlanet->ChangeRelationToRanger(OtherShip, 10);
                }
                pas::checked_cast<aRanger::TRanger*>(OtherShip)->AddTraderCareerActivity(1);
            }
            OtherShip->SetMoney(OtherShip->Money - OfferedAmount);
            SetMoney(this->Money + OfferedAmount);
            aShip::TShip_TruceWithShip(this, OtherShip);
        };
        if (UnknownVirtualC0(OtherShip)) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Refuse.Pirate"_wref.get(), OtherShip);
            return false;
        }
        if (aPlayer::GetPlayer() == PartnerShip && aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Pirate.TalkWithParent"_wref.get(), OtherShip);
            return aPlayer::GetPlayer()->BuildTrucePaymentResponse(OtherShip, Response, OfferedAmount);
        }
        std::uint8_t Result = false;
        std::int32_t NextDemandTurn = LastPlayerExtortionTurn + 30;
        if (OtherShip->TruceShip == this) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && PlayerExtortionPactActive) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::class_cast_if<aNormalShip::TNormalShip*>(OtherShip) != nullptr && OtherShip->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && OtherShip->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && OtherShip->CurrentStar->Status.CustomFaction == u"" && reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->CurrentSystemKills.Pirate > 0) {
            if (2 * Wealth * 0.06666666666666666667L < OfferedAmount) {
                Response = aShip::TShip_LookupVisibleTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Ok"}), OtherShip);
                AcceptPayment();
                return true;
            }
            Response = aShip::TShip_LookupVisibleTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        } else if (this->virtual_TShip_RecomputeFearState() || aShip::TShip_ChanceToWin(this, OtherShip) < 1.0L && GetHullIntegrityPercent() < 40 && this->virtual_TShip_AcceptsRansomDemandFrom(OtherShip) || aShip::TShip_ChanceToWin(this, OtherShip) < 0.2L && this->virtual_TShip_AcceptsRansomDemandFrom(OtherShip) || static_cast<long double>(OfferedAmount) > ([&] {
            double cpp_arg = aShip::TShip_GetWinChancePercent(this, OtherShip) & 0x0000007f;
            double wealthScaledAmount = GetWealthScaledAmount(1);
            double wealthScaledAmount_2 = GetWealthScaledAmount(5);
            return aMyFunction::RemapClamped(cpp_arg, 0.0, 1.0E+2, wealthScaledAmount, wealthScaledAmount_2);
        }())) {
            Response = aShip::TShip_LookupVisibleTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Ok"}), OtherShip);
            AcceptPayment();
            return true;
        } else {
            Response = aShip::TShip_LookupVisibleTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        }
    }

    std::uint8_t TPirate_BuildAttackRequestResponse(TPirate* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        std::uint8_t Result{};
        // Nested helper with caller-popped static link.
        auto AcceptRequest = [&]() -> void {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Ok"}), Requester);
            Self->SetJointAttackTarget(Requester, Target);
            Result = true;
        };
        // Nested helper with caller-popped static link.
        auto FriendsPreferred = [&]() -> std::uint8_t {
            pas::Extended cpp_right_2 = aMyFunction::RemapClamped(aGalaxy::TGalaxy::GetCoalitionToPirateSystemRatio(), 0.33, 1.0, 1.3, 0.0);
            std::int64_t cpp_right = System::Round((aShip::TShip_RelationToShip(Self, Requester) & 0x0000007f) * cpp_right_2);
            return (static_cast<std::uint32_t>(aShip::TShip_RelationToShip(Self, Target)) & 0x0000007f) >= cpp_right;
        };
        Result = false;
        if (pas::class_cast_if<aRanger::TRanger*>(Requester) != nullptr) {
            if (pas::in_range(Target->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate)) {
                Target->ChangeRelationToRanger(Requester, -20);
            }
            if (Target->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || Target->TypeId == aGalaxyStruct::stPirate) {
                pas::checked_cast<aRanger::TRanger*>(Requester)->AddWarriorCareerActivity(1);
            } else {
                pas::checked_cast<aRanger::TRanger*>(Requester)->AddPirateCareerActivity(1);
            }
        }
        if (Self->OrderTarget == Target && aShip::TShip_GetRelationLevelToShip(Self, Target) == aGalaxyStruct::rlHostile) {
            AcceptRequest();
        } else if (Self->TruceShip == Target) {
            Response = ([&] {
                pas::WideString name = Target->GetName();
                pas::WideString lookupVisibleTalkText = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Attack.WeAlreadyHavePact"_wref.get(), Requester);
                return aMyFunction::FormatText1(std::move(lookupVisibleTalkText), u"<color=255,240,100>"_w, u"<Target>"_w, std::move(name));
            }());
        } else if (aShip::TShip_RelationToShip(Self, Target) >= 60 && FriendsPreferred()) {
            if (!(pas::class_cast_if<aTranclucator::TTranclucator*>(Target) != nullptr)) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"WeFriends"}), Requester);
            } else if (reinterpret_cast<aTranclucator::TTranclucator*>(Target)->OwnerShip == Self) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"ItsMyTranc"}), Requester);
            } else if (reinterpret_cast<aTranclucator::TTranclucator*>(Target)->OwnerShip == Requester) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"ItsYourTranc"}), Requester);
            } else {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"WeFriendsTranc"}), Requester);
            }
        } else if (Self->virtual_TShip_AcceptsRansomDemandFrom(Target) || Self->InFear) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Fear"}), Requester);
        } else if (!Self->virtual_TShip_TrustsAttackRequester(Requester)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Suspect"}), Requester);
        } else if (Self->HasLockedOrFollowOrder()) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else {
            AcceptRequest();
        }
        return Result;
    }

    std::uint8_t TPirate_AcceptPartnershipOffer(TPirate* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result{};
        if (Self->virtual_TShip_BuildPartnershipOfferResponse(OtherShip, Response, PaymentAmount)) {
            Self->PartnershipDaysRemaining = 30 * Self->CalculatePartnershipMonths(PaymentAmount, OtherShip);
            Response = ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(Self->CalculatePartnershipMonths(PaymentAmount, OtherShip));
                auto name = pas::borrow(pas::checked_cast<aRanger::TRanger*>(OtherShip)->Name);
                pas::WideString lookupVisibleTalkText = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Pirate.Ok"_wref.get(), OtherShip);
                return aMyFunction::FormatText2(std::move(lookupVisibleTalkText), u"<color=255,240,100>"_w, u"<Month>"_w, std::move(intToStr), u"<Ranger>"_w, name.get());
            }());
            Self->PartnerShip = OtherShip;
            Self->OrderAbsolute = false;
            Result = true;
            Self->SetMoney(Self->Money + PaymentAmount);
            OtherShip->SetMoney(OtherShip->Money - PaymentAmount);
            if (aPlayer::GetPlayer() == OtherShip) {
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckMasterAchievement();
            }
            return Result;
        }
        return false;
    }

    std::uint8_t TPirate_BuildPartnershipOfferResponse(TPirate* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        if (Self->PartnerShip == OtherShip) {
            return true;
        }
        std::uint8_t Result = false;
        if (aShip::TShip_RelationToShip(Self, OtherShip) < 45) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Pirate.Suspect"_wref.get(), OtherShip);
        } else if (Self->PartnerShip != nullptr) {
            Response = ([&] {
                auto name = pas::borrow(pas::checked_cast<aRanger::TRanger*>(Self->PartnerShip)->Name);
                pas::WideString lookupVisibleTalkText = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Pirate.AlreadyHavePartner"_wref.get(), OtherShip);
                return aMyFunction::FormatText1(std::move(lookupVisibleTalkText), u"<color=255,240,100>"_w, u"<Partner>"_w, name.get());
            }());
        } else if (pas::class_cast_if<aPlayer::TPlayer*>(OtherShip) != nullptr && reinterpret_cast<aPlayer::TPlayer*>(OtherShip)->GetMaxPiratePartners() <= pas::list_count(reinterpret_cast<aPlayer::TPlayer*>(OtherShip)->PiratePartners) || Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aPlayer::GetPlayer()->PirateRank < Self->PirateRank) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Pirate.NeedPirate"_wref.get(), OtherShip);
        } else if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr && ([&] {
            std::int32_t cpp_left = OtherShip->GetEffectiveSkillLevel(aShip::psLeadership, false);
            return cpp_left <= static_cast<aRanger::TRanger*>(OtherShip)->CountWingmen();
        }())) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Partner.NeedLeadership"_wref.get(), OtherShip);
        } else if (Self->CalculatePartnershipMonths(PaymentAmount, OtherShip) == 0) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Pirate.SmallMoney"_wref.get(), OtherShip);
        } else {
            Result = true;
        }
        Response = aMyFunction::FormatText1(Response, u"<color=255,240,100>"_w, u"<Ranger>"_w, pas::checked_cast<aRanger::TRanger*>(OtherShip)->Name);
        return Result;
    }

    void TPirate::ReviewPartnership() {
        aShip::TShip* Leader{};
        std::int32_t Longest{};
        std::int32_t I{};
        // Nested in the partnership check; caller-popped static link.
        auto CanNotifyPartner = [&]() -> std::uint8_t {
            std::uint8_t Result{};
            if (this->CurrentStar != this->PartnerShip->CurrentStar || static_cast<std::uint8_t>(InNormalSpace() ^ 1) || static_cast<std::uint8_t>(this->PartnerShip->InNormalSpace() ^ 1)) {
                return false;
            } else if (aShip::TShip_CanContactShip(this, this->PartnerShip)) {
                return true;
            } else if (static_cast<std::uint8_t>(this->PartnerShip->NoTalk ^ 1) || aPlayer::GetPlayer() != this->PartnerShip) {
                return false;
            } else {
                this->PartnerShip->NoTalk = false;
                Result = aShip::TShip_CanContactShip(this, this->PartnerShip);
                this->PartnerShip->NoTalk = true;
                return Result;
            }
        };
        if (PartnerShip != nullptr) {
            if (PartnershipDaysRemaining < 1) {
                if (CanNotifyPartner()) {
                    Leader = PartnerShip;
                    if (Order == aShip::soFollowShip && OrderTarget == PartnerShip) {
                        OrderNone(false);
                    }
                    pas::list_remove(aPlayer::GetPlayer()->PiratePartners, reinterpret_cast<void*>(this));
                    PartnerShip = nullptr;
                    NotifyPiratePartnershipExpired(Leader);
                }
            } else if (aShip::TShip_RelationToShip(this, PartnerShip) < 30) {
                if (CanNotifyPartner()) {
                    Leader = PartnerShip;
                    if (Order == aShip::soFollowShip && OrderTarget == PartnerShip) {
                        OrderNone(false);
                    }
                    pas::list_remove(aPlayer::GetPlayer()->PiratePartners, reinterpret_cast<void*>(this));
                    PartnerShip = nullptr;
                    NotifyPiratePartnerRelationBreak(Leader);
                }
            } else if (EnemyShip != nullptr && EnemyShip->PartnerShip != nullptr && EnemyShip->PartnerShip == PartnerShip) {
                if (CanNotifyPartner()) {
                    Leader = PartnerShip;
                    if (Order == aShip::soFollowShip && OrderTarget == PartnerShip) {
                        OrderNone(false);
                    }
                    pas::list_remove(aPlayer::GetPlayer()->PiratePartners, reinterpret_cast<void*>(this));
                    PartnerShip = nullptr;
                    NotifyPiratePartnerRebellion(Leader);
                }
            } else if (CanNotifyPartner()) {
                if (aPlayer::GetPlayer()->GetMaxDominionShips() < pas::list_count(aPlayer::GetPlayer()->PiratePartners)) {
                    Longest = PartnershipDaysRemaining;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->PiratePartners) - 1); cpp_range.next(I); ) {
                        if (pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->PiratePartners, I)->PartnershipDaysRemaining > Longest) {
                            Longest = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->PiratePartners, I)->PartnershipDaysRemaining;
                        }
                    }
                    if (Longest >= PartnershipDaysRemaining) {
                        Leader = PartnerShip;
                        if (Order == aShip::soFollowShip && OrderTarget == PartnerShip) {
                            OrderNone(false);
                        }
                        pas::list_remove(aPlayer::GetPlayer()->PiratePartners, reinterpret_cast<void*>(this));
                        PartnerShip = nullptr;
                        NotifyPiratePartnerRatingBreak(Leader);
                    }
                }
            }
        }
    }

    void TPirate::MoveToRandomPlanetOrbit() {
        aMyFunction::TPolarPoint Polar{};
        aPlanet::TPlanet* Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, 0);
        Polar = Planet->Orbit;
        Polar.AngleDegrees = aMyFunction::NextRandomIntRange(0, 359, RandomState);
        OrderMove(aMyFunction::PolarToPoint(Polar), false);
    }

    std::uint8_t TPirate::UnknownVirtualC0(void* Argument) {
        std::uint8_t Result = false;
        if (PirateType != 0 && pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(static_cast<aShip::TShip*>(Argument)->CurrentStanding) && (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates || CurrentStar->Status.CustomFaction != u"" || aPlanet::MainPiratePlanet == nullptr || aPlanet::MainPiratePlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || static_cast<aShip::TShip*>(Argument)->CurrentStar == aPlanet::MainPiratePlanet->CurrentStar)) {
            return true;
        }
        return Result;
    }

    std::uint8_t TPirate::TryRetreatFromSystem() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        float DominatorAndCustomStrength{};
        float CoalitionStrength{};
        float PirateStrength{};
        float StrengthScale{};
        aGalaxy::TStar* Star{};
        aGalaxy::TStar* PirateStar{};
        aGalaxy::TStar* AnyStar{};
        float Angle{};
        float PirateAngle{};
        float AnyAngle{};
        std::uint8_t Urgent{};
        std::uint8_t Result = false;
        if (CurrentStar->Constellation->Id == 20) {
            return Result;
        }
        std::uint8_t PirateSystem = CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && CurrentStar->Status.CustomFaction == u"";
        std::uint8_t HostileSystem = CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators || CurrentStar->Status.CustomFaction != u"";
        aRuins::TRuins* Station = nullptr;
        if (!PirateSystem) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr && Ship->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion) && CurrentStar->Dominion != Ship && Ship->InNormalSpace() && Ship->virtual_TShip_CanDock(this) && (reinterpret_cast<aRuins::TRuins*>(Ship)->FlyToStar == nullptr || reinterpret_cast<aRuins::TRuins*>(Ship)->FlyToStar == CurrentStar)) {
                    Station = reinterpret_cast<aRuins::TRuins*>(Ship);
                    break;
                }
            }
        }
        float RetreatFactor = (100 + aMyFunction::SeededRandomIntRange(-15, 15, Seed + Id)) * 0.02L;
        if (EnemyShip == nullptr || EnemyShip->OrderTarget != this || EnemyShip->EstimateOrderTravelTurns() > 2) {
            RetreatFactor = RetreatFactor * 0.5L;
        }
        if (Station == nullptr) {
            RetreatFactor = static_cast<long double>(aMyFunction::RemapClamped(RaidPressure, 0.0, 15.0, 1.0, 0.1)) * RetreatFactor;
        }
        if (InFear && static_cast<std::uint8_t>(PirateSystem ^ 1)) {
            RetreatFactor = 1.4L * RetreatFactor;
        }
        if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || HostileSystem && Station != nullptr) {
            if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && static_cast<std::uint8_t>(PirateSystem ^ 1)) {
                RetreatFactor = RetreatFactor * 0.25L;
            }
            DominatorAndCustomStrength = CurrentStar->GetCachedFactionStrength(aGalaxyStruct::sfDominators);
            CoalitionStrength = CurrentStar->GetCachedFactionStrength(aGalaxyStruct::sfCoalition);
            PirateStrength = CurrentStar->GetCachedFactionStrength(aGalaxyStruct::sfPirates);
            StrengthScale = pas::real_divide(1.0L, pas::real_max<float>(1.0f, aGalaxy::Galaxy->AverageRangerStrength));
            if (TransitOriginStar != nullptr) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (pas::is_one_of<aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary>(Ship->CurrentStanding) && Ship->OrderTarget == CurrentStar) {
                        PirateStrength = pas::real_min<pas::Extended>(1.0E+1L, pas::real_max<pas::Extended>(0.1L, static_cast<long double>(Ship->Strength) * StrengthScale)) * 0.75L + PirateStrength;
                    }
                }
            }
            if (std::fabs(CoalitionStrength - 2.5L * DominatorAndCustomStrength) * RetreatFactor >= PirateStrength || aShip::TShip_HasNoUsableWeapons(this) && static_cast<std::uint8_t>(PirateSystem ^ 1)) {
                Urgent = InFear && EnemyShip != nullptr && EnemyShip->OrderTarget == this && EnemyShip->EstimateOrderTravelTurns() <= 2;
                if (Order == aShip::soJump) {
                    if (Urgent) {
                        UpdateAfterburnerState();
                    }
                    return true;
                } else if (Station != nullptr) {
                    OrderLanding(Station, false);
                    if (Urgent || aGalaxy::Galaxy->CurrentTurn + EstimateOrderTravelTurns() + 1 > Station->FlyDate) {
                        UpdateAfterburnerState();
                    }
                    return true;
                } else {
                    I = 1;
                    PirateStar = nullptr;
                    PirateAngle = 0.0f;
                    AnyStar = nullptr;
                    AnyAngle = 0.0f;
                    while (pas::list_count(aGalaxy::Galaxy->Stars) > I && CurrentStar->StarDistances[I].Distance <= JumpRange) {
                        Star = CurrentStar->StarDistances[I].Star;
                        ++I;
                        {
                            pas::Extended cpp_left = aMyFunction::PointBearingDegrees(CurrentStar->Position, Star->Position);
                            Angle = cpp_left - aMyFunction::PointBearingDegrees(aMyFunction::MakeFloatPoint(0, 0), Position);
                        }
                        Angle = aMyFunction::WrapSignedHeadingDegrees(Angle);
                        if (AnyStar == nullptr || std::fabs(static_cast<pas::Extended>(Angle)) < AnyAngle) {
                            AnyStar = Star;
                            AnyAngle = std::fabs(static_cast<pas::Extended>(Angle));
                        }
                        if (Star->Status.ControlFaction == aGalaxyStruct::sfPirates && Star->Status.CustomFaction == u"" && (PirateStar == nullptr || std::fabs(static_cast<pas::Extended>(Angle)) < PirateAngle)) {
                            PirateStar = Star;
                            PirateAngle = std::fabs(static_cast<pas::Extended>(Angle));
                        }
                    }
                    if (InFear && 2.0L * RetreatFactor * PirateStrength < std::fabs(static_cast<long double>(CoalitionStrength) - DominatorAndCustomStrength) && aMyFunction::NextRandomIntRange(0, 9, RandomState) < 5) {
                        PirateStar = AnyStar;
                    }
                    if (PirateStar != nullptr) {
                        if (OrderTarget != PirateStar) {
                            OrderJump(PirateStar, false);
                        }
                        Result = true;
                        if (Urgent) {
                            UpdateAfterburnerState();
                        }
                    }
                    return Result;
                }
            }
        }
        return Result;
    }

    float TPirate::AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) {
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        float FragilityScale{};
        std::int32_t Price{};
        float HullValueScale{};
        switch (PirateType) {
            case 0: {
                FragilityScale = 1.2f;
                HullValueScale = 1.0f;
                break;
            }
            case 1: {
                FragilityScale = 0.8f;
                HullValueScale = 2.0f;
                break;
            }
            case 2: {
                FragilityScale = 1.0f;
                HullValueScale = 1.5f;
                break;
            }
            case 3: {
                FragilityScale = 1.2f;
                HullValueScale = 0.5f;
                break;
            }
            default: {
                FragilityScale = 1.0f;
                HullValueScale = 1.0f;
                break;
            }
        }
        if (pas::is_one_of<aConst::t_FuelTanks, aConst::t_Radar, aConst::t_Scaner>(Item->ItemType)) {
            FragilityScale = FragilityScale * 0.5L;
        }
        float DesiredMoneyFraction = 0.1f;
        float DesiredFreeFraction = pas::real_max<pas::Extended>(0.01L, pas::real_min<pas::Extended>(0.99L, pas::real_divide(GetDesiredCargoFreeSpace(), std::max<std::int32_t>(100, GetHull()->Weight))));
        pas::Extended cpp_left = pas::sqr(([&] {
            pas::Extended cpp_left_2 = pas::real_divide(1.0L, pas::real_max<float>(0.01f, SmoothedMoneyFraction)) - 1.0L;
            return pas::real_divide(cpp_left_2, pas::real_divide(1.0L, DesiredMoneyFraction) - 1.0L);
        }()));
        float MoneyPenalty = pas::real_divide(cpp_left, pas::real_max<pas::Extended>(SmoothedWealth * 0.05L, 1.0E+3L));
        float EffectivenessScale = pas::real_divide(2.0L, pas::real_max<float>(1.0E+1f, SmoothedEquipmentEffectiveness));
        pas::Extended cpp_left_3 = pas::sqr(([&] {
            pas::Extended cpp_left_4 = pas::real_divide(1.0L, pas::real_max<float>(0.01f, SmoothedFreeCapacityFraction)) - 1.0L;
            return pas::real_divide(cpp_left_4, pas::real_divide(1.0L, DesiredFreeFraction) - 1.0L);
        }()));
        float WeightPenalty = pas::real_divide(cpp_left_3, pas::real_max<pas::Extended>(1.0E+1L, GetHull()->Weight * 0.1L));
        MoneyPenalty = MoneyPenalty * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-25, 25, Id + Seed));
        EffectivenessScale = EffectivenessScale * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-25, 25, Seed + 3 * Id));
        WeightPenalty = WeightPenalty * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-25, 25, Seed + 5 * Id));
        switch (PriceMode) {
            case 4: Price = Item->Cost; break;
            case 3: {
                Price = aItem::TItem_CalculateResaleValue(Item, GetEffectiveSkillLevel(aShip::psTrading, false));
                break;
            }
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

    float TPirate_EvaluateStatBonus(TPirate* Self, aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
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
                Result = Value * 2;
            } else if (cpp_case == aConst::bonSpeed) {
                Result = Value * 1.2L;
            } else if (cpp_case == aConst::bonJump) {
                Result = Value * 20;
            } else if (cpp_case == aConst::bonRadar) {
                Result = Value * 0.05L;
            } else if (cpp_case == aConst::bonScan) {
                Result = Value * 10 + Value * 35 * (Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f);
            } else if (cpp_case == aConst::bonDroid) {
                Result = pas::real_divide(Value * 10, pas::real_max<float>(0.1f, Self->GetHull()->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags))));
            } else if (cpp_case == aConst::bonHook) {
                Result = (pas::real_min<pas::Extended>(static_cast<pas::Extended>(Value), static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]) + Value * 0.1L) * 1.3L;
            } else if (cpp_case == aConst::bonDef) {
                Result = pas::real_divide(pas::real_divide(Value * 5 * 100, std::max<std::int32_t>(5, 100 - Value)) * 45.0L, std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                Result = Value * 12;
            } else if (cpp_case == aConst::bonWSplinter) {
                Result = Value * 12;
            } else if (cpp_case == aConst::bonWMissile) {
                Result = Value * 12 * (0.1L + static_cast<std::int8_t>(aShip::TShip_GetRadarRange(Self) > 0) * 0.9L);
            } else if (cpp_case == aConst::bonWRadius) {
                Result = Value * pas::sqr(pas::real_divide(std::max<std::int32_t>(100, Self->SmoothedEnemySpeed), std::max<std::int32_t>(100, Self->SmoothedSpeed)));
            } else if (cpp_case == aConst::bonHookRadius) {
                Result = Value * 0.1L;
            } else if (cpp_case == aConst::bonMass) {
                Result = aMyFunction::RemapClamped(Value + Self->GetHull()->Weight * 0.1L, aConst::HullMassEvaluationStart, aConst::HullMassEvaluationEnd, 1.0, 0.333) * 6.0E+3L;
            } else if (cpp_case == aConst::bonSlotRadar) {
                if (Self->GetSlotCount(aConst::sskRadar) == 0 && Value > 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetRadar() != nullptr && Value < 0) {
                    Result = -PirateSlotBonusWeights[BonusKind] - PirateSlotBonusWeights[18] * (Self->CountMissileWeapons() & 0x0000007f);
                } else if (Self->GetSlotCount(aConst::sskRadar) == 1 && Value < 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotScaner) {
                if (Self->GetSlotCount(aConst::sskScanner) == 0 && Value > 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetScanner() != nullptr && Value < 0) {
                    Result = -PirateSlotBonusWeights[BonusKind] - (Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f) * 0.1L * PirateSlotBonusWeights[18];
                } else if (Self->GetSlotCount(aConst::sskScanner) == 1 && Value < 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotDroid) {
                if (Self->GetSlotCount(aConst::sskRepairRobot) == 0 && Value > 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetRepairRobot() != nullptr && Value < 0) {
                    Result = -PirateSlotBonusWeights[BonusKind];
                } else if (Self->GetSlotCount(aConst::sskRepairRobot) == 1 && Value < 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotHook) {
                if (Self->GetSlotCount(aConst::sskCargoHook) == 0 && Value > 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetCargoHook() != nullptr && Value < 0) {
                    Result = -PirateSlotBonusWeights[BonusKind];
                } else if (Self->GetSlotCount(aConst::sskCargoHook) == 1 && Value < 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotDef) {
                if (Self->GetSlotCount(aConst::sskDefGenerator) == 0 && Value > 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetDefGenerator() != nullptr && Value < 0) {
                    Result = -PirateSlotBonusWeights[BonusKind];
                } else if (Self->GetSlotCount(aConst::sskDefGenerator) == 1 && Value < 0) {
                    Result = PirateSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotWeapon) {
                if (Self->GetSlotCount(aConst::sskWeapon) < 5 && Value > 0) {
                    Result = std::min<std::int32_t>(Value, 5 - Self->GetSlotCount(aConst::sskWeapon)) * PirateSlotBonusWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -Self->GetSlotCount(aConst::sskWeapon)) * PirateSlotBonusWeights[BonusKind];
                }
                {
                    std::int32_t cpp_right = std::max<std::int32_t>(Value + Self->GetSlotCount(aConst::sskWeapon), 1);
                    if ((Self->CountEquippedWeapons() & 0x0000007f) > cpp_right) {
                        std::int32_t cpp_right_2 = std::max<std::int32_t>(1, Value + Self->GetSlotCount(aConst::sskWeapon));
                        Result = Result - PirateSlotBonusWeights[BonusKind] * 0.6L * ((Self->CountEquippedWeapons() & 0x0000007f) - cpp_right_2);
                    }
                }
            } else if (cpp_case == aConst::bonSlotArt) {
                if (Self->GetSlotCount(aConst::sskArtefact) < aConst::DefaultHullSlotCounts[8] && Value > 0) {
                    Result = std::min<std::int32_t>(Value, aConst::DefaultHullSlotCounts[8] - Self->GetSlotCount(aConst::sskArtefact)) * PirateSlotBonusWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -Self->GetSlotCount(aConst::sskArtefact)) * PirateSlotBonusWeights[BonusKind];
                }
                if (Self->Artefacts != nullptr) {
                    if (pas::list_count(Self->Artefacts) > std::max<std::int32_t>(Value + Self->GetSlotCount(aConst::sskArtefact), 0)) {
                        Result = -1.0E+3f;
                    }
                }
            } else if (cpp_case == aConst::bonSlotForsage) {
                if (Self->GetSlotCount(aConst::sskAfterburner) == 0 && Value > 0) {
                    Result = PirateSlotBonusWeights[BonusKind];
                } else if (Self->GetSlotCount(aConst::sskAfterburner) == 1 && Value < 0) {
                    Result = -PirateSlotBonusWeights[BonusKind];
                }
            } else if (cpp_case >= aConst::bonSkill1 && cpp_case <= aConst::bonSkill6) {
                if (Value > 0) {
                    Result = std::min<std::int32_t>(6 - (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f), Value) * PirateSkillBonusWeights[BonusKind];
                }
                if (Value > 0 && Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) > 6) {
                    Result = Result + PirateSkillBonusWeights[BonusKind] * 0.05L * (Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) - 6);
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f, -Value) * -PirateSkillBonusWeights[BonusKind];
                }
                if (Value < 0 && Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) < 0) {
                    Result = Result + PirateSkillBonusWeights[BonusKind] * 0.03L * (Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f));
                }
            } else {
                Result = 0.0f;
            }
        }
        if (Self->PirateType == 0 && pas::is_one_of<aConst::bonSlotForsage, aConst::bonMass>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (Self->PirateType == 0 && pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonSpeed), static_cast<std::int32_t>(aConst::bonSpeed))) {
            Result = pas::real_divide(Result * 1.2L * std::max<std::int32_t>(100, Self->SmoothedEnemySpeed), std::max<std::int32_t>(100, Self->SmoothedSpeed));
        }
        if (Self->PirateType == 1 && pas::in_set<aConst::bonHull, aConst::bonHull, aConst::bonSpeed, aConst::bonSpeed, aConst::bonDroid, aConst::bonDroid, aConst::bonDef, aConst::bonWMissile>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (Self->PirateType == 2 && pas::in_set<aConst::bonScan, aConst::bonScan, aConst::bonWEnergy, aConst::bonWRadius>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (Self->PirateType == 3 && pas::is_one_of<aConst::bonSpeed, aConst::bonRadar, aConst::bonDroid, aConst::bonWRadius, aConst::bonMass>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (Self->PirateType == 3 && pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonWEnergy), static_cast<std::int32_t>(aConst::bonWMissile))) {
            Result = Result * 0.7L;
        }
        if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonSkill1), static_cast<std::int32_t>(aConst::bonSkill6))) {
            pas::Extended cpp_left = Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-75, 75, Self->Seed + 131 * BonusKind));
            return cpp_left * aConst::RaceSkillEvaluationFactors[Self->PilotRace][aConst::EquipmentBonusSkills[BonusKind - 22]];
        }
        return Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-25, 25, Self->Seed + 131 * BonusKind));
    }

    float TPirate_EvaluateWeaponDamage(TPirate* Self, aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> ShockFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkShock}});
        static const pas::Set<0, 255> AcidFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkAcid}});
        static const pas::Set<0, 255> DisablingFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkDecelerate}, {aGalaxyStruct::dkDestruct}, {aGalaxyStruct::dkMagnetic}});
        float ScannerFactor{};
        aGalaxyStruct::TDamageFlagSet Flags{};
        float SlowFactor{};
        std::int32_t I{};
        std::int32_t ShotTotal{};
        Flags = Weapon->GetDamageFlags();
        if (Flags * static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags) != pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}) && Self->GetScanner() != nullptr && Self->GetRadar() != nullptr) {
            std::int32_t cpp_right = aItem::DefenseDamageFactorToPercent(aItem::GetGeneratedDefenseDamageFactor(aGalaxy::Galaxy->TechLevel)) & 0x0000007f;
            ScannerFactor = aMyFunction::RemapClamped(aShip::TShip_GetScannerPower(Self) - cpp_right + 1, -5.0, 1.0E+1, 0.1, 2.0);
        } else {
            ScannerFactor = 0.0f;
        }
        float Result = static_cast<long double>(BaseDamage) * Self->GetWeaponArtefactDamageFactor(Weapon);
        if (pas::contains(Flags, aGalaxyStruct::dkDrain)) {
            switch (Self->PirateType) {
                case 1: Result = Result * 1.7L; break;
                default: Result = Result * 1.5L; break;
            }
        }
        if (pas::contains(Flags, aGalaxyStruct::dkShock)) {
            switch (Self->PirateType) {
                case 2: {
                    Result = Result * (1.3L + (Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ShockFlags)) & 0x0000007f) * 0.15L);
                    break;
                }
                default: {
                    Result = Result * (1.2L + (Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ShockFlags)) & 0x0000007f) * 0.05L);
                    break;
                }
            }
        }
        if (pas::contains(Flags, aGalaxyStruct::dkMagnetic)) {
            switch (Self->PirateType) {
                case 2: Result = Result * 1.8L; break;
                case 3: Result = Result * 3.0L; break;
                default: Result = Result * 1.4L; break;
            }
        }
        if (pas::contains(Flags, aGalaxyStruct::dkDestruct)) {
            SlowFactor = 1.0f;
        } else {
            SlowFactor = 0.0f;
        }
        if (pas::contains(Flags, aGalaxyStruct::dkReduceEngine)) {
            SlowFactor = 1.0L + 2.0L * SlowFactor;
        }
        float DisruptionFactor = 0.0f;
        if (pas::contains(Flags, aGalaxyStruct::dkDecelerate)) {
            DisruptionFactor = DisruptionFactor + 1.0L;
        }
        if (Self->CountActiveArtefacts(aConst::t_ArtDecelerate) > 0 && pas::contains(Flags, aGalaxyStruct::dkSplinter)) {
            std::int32_t cpp_right_2 = (Self->CanBoostArtefact(aConst::t_ArtDecelerate, Weapon, false) & 127) * 1 + 1;
            DisruptionFactor = static_cast<long double>(DisruptionFactor) + Self->CountActiveArtefacts(aConst::t_ArtDecelerate) * cpp_right_2;
        }
        float TotalDisruption = DisruptionFactor;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->CountEquippedWeapons() & 0x0000007f); cpp_range.next(I); ) {
            if (pas::contains(Self->Weapons[I]->GetDamageFlags(), aGalaxyStruct::dkDecelerate)) {
                TotalDisruption = TotalDisruption + 1.0L;
            }
            if (Self->CountActiveArtefacts(aConst::t_ArtDecelerate) > 0 && pas::contains(Self->Weapons[I]->GetDamageFlags(), aGalaxyStruct::dkSplinter)) {
                // The native loop tests each equipped weapon but boosts the candidate.
                std::int32_t cpp_right_3 = (Self->CanBoostArtefact(aConst::t_ArtDecelerate, Weapon, false) & 127) * 1 + 1;
                TotalDisruption = static_cast<long double>(TotalDisruption) + Self->CountActiveArtefacts(aConst::t_ArtDecelerate) * cpp_right_3;
            }
        }
        {
            pas::Extended cpp_left = pas::real_divide(DisruptionFactor, pas::real_max<float>(TotalDisruption, 1.0f));
            DisruptionFactor = cpp_left * aMyFunction::RemapClamped(TotalDisruption, 1.0, 3.0, 1.0, 2.0);
        }
        if (SlowFactor > 0.01L) {
            switch (Self->PirateType) {
                case 2: Result = Result * (1.0L + SlowFactor * 0.1L); break;
                case 3: Result = Result * (1.0L + SlowFactor * 0.5L); break;
                default: Result = Result * (1.0L + SlowFactor * 0.05L); break;
            }
        }
        float StatusFactor = 1.0f;
        if (pas::contains(Flags, aGalaxyStruct::dkScanBonus)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkBonusToDamaged)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.2L);
        }
        StatusFactor = StatusFactor - 1.0L;
        if (Self->PirateType == 3) {
            Result = (3.0E+2L + Result) * 0.5L;
        }
        if (IncludeAdditiveBonuses) {
            if (DisruptionFactor > 0.01L) {
                switch (Self->PirateType) {
                    case 2: Result = Result + 5.0L * DisruptionFactor; break;
                    case 3: Result = Result + 5.0E+1L * DisruptionFactor; break;
                    default: Result = Result + 1.0E+1L * DisruptionFactor; break;
                }
            }
            if (SlowFactor > 0.01L) {
                switch (Self->PirateType) {
                    case 2: Result = Result + 2.0L * SlowFactor; break;
                    case 3: Result = Result + 1.0E+1L * SlowFactor; break;
                    default: Result = Result + 1.0L * SlowFactor; break;
                }
            }
            {
                std::int32_t cpp_left_2 = Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(AcidFlags));
                Result = static_cast<long double>(Result) + cpp_left_2 * Weapon->GetShotCount();
            }
            if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
                ShotTotal = 1;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Self->CountEquippedWeapons() & 0x0000007f); cpp_range_2.next(I); ) {
                    ShotTotal += Self->Weapons[I]->GetShotCount();
                }
                switch (Self->PirateType) {
                    case 1: Result = static_cast<long double>(Result) + ShotTotal * 5; break;
                    default: Result = static_cast<long double>(Result) + ShotTotal * 3; break;
                }
            }
            if (pas::contains(Flags, aGalaxyStruct::dkMoreDrop)) {
                Result = Result + ScannerFactor * 15.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDropCargo)) {
                Result = Result + ScannerFactor * 15.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkBlockWeapon)) {
                Result = Result + ScannerFactor * 5.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDroidBlock)) {
                Result = Result + ScannerFactor * 5.0L;
            }
        }
        float SpeedFactor = pas::real_divide(std::max<std::int32_t>(100, Self->SmoothedEnemySpeed) * Self->GetHull()->Weight, aConst::HullBaseSize * pas::real_max<pas::Extended>(1.0E+2L, static_cast<long double>(Self->SmoothedSpeed) * aConst::EquipmentSizeFactors[1]));
        switch (static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType)) {
            case aGalaxyStruct::wstRocket: {
                Result = Result * 0.9L * Weapon->GetShotCount() * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstMissile: {
                Result = Result * (0.9L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.2L * 0.01L + StatusFactor) * Weapon->GetShotCount();
                break;
            }
            case aGalaxyStruct::wstTorpedo: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.2L * 0.01L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstChain: {
                Result = Result * (1.3L + (Weapon->GetShotCount() - 1) * 0.1L) * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstSplash: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.4L * 0.01L * SpeedFactor + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstAreaDamage: {
                Result = Result * (1.0L + Weapon->Range * 0.3L * 0.01L * SpeedFactor + StatusFactor);
                break;
            }
            default: Result = Result * (1.0L + StatusFactor); break;
        }
        Result = static_cast<long double>(Result) * Weapon->GetAttackCount();
        Result = Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Self->Seed + Weapon->GetWeaponInfo()->TypeHash));
        if (Self->PirateType == 3 && Flags * static_cast<aGalaxyStruct::TDamageFlagSet>(DisablingFlags) != pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}) && Self->CountWeaponsByDamageFlags(Flags * static_cast<aGalaxyStruct::TDamageFlagSet>(DisablingFlags)) == 0) {
            return Result * 4.0L;
        } else if (pas::contains(Flags, aGalaxyStruct::dkMissile)) {
            return Result * 1.2L;
        } else {
            return Result;
        }
    }

    std::uint8_t TPirate::AcceptPickupItem(aItem::TItem* Item) {
        return true;
    }

    std::uint8_t TPirate::AcceptPickupDistance(aItem::TItem* Item, double Distance) {
        if (Speed < 1) {
            return false;
        }
        return pas::in_set<2, 3, 5, 7>(static_cast<std::uint8_t>(Item->ItemType)) || static_cast<long double>(2 * Speed) >= Distance || Item->Cost >= static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(Distance, Speed), 1.0, 1.0E+1, 0.01, 0.05)) * Wealth;
    }

    void TPirate_RefreshCurrentStanding(TPirate* Self) {
        std::uint8_t Owner{};
        std::int32_t StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer() == Self->PartnerShip) {
                Owner = aPlayer::GetPlayer()->OwnerId;
            } else {
                Owner = Self->OwnerId;
            }
            if (aShip::TShip_IsInPrison(Self) || Self->PirateType == 0 && Owner != 7) {
                Self->CurrentStanding = aGalaxyStruct::ssNeutral;
            } else if (Owner == 7 && Self->PirateType != 0) {
                Self->CurrentStanding = aGalaxyStruct::ssPirateMilitary;
            } else if (Owner == 7 && (Self->CurrentSystemKills.Normal > 0 || Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates)) {
                Self->CurrentStanding = aGalaxyStruct::ssPirateActive;
            } else {
                Self->CurrentStanding = aGalaxyStruct::ssPiratePassive;
            }
        }
    }

    void TPirate::p_destroy() {
        aPirate::TPirate_Destroy(this);
    }

    void TPirate::virtual_TShip_NextDay() {
        aPirate::TPirate_NextDay(this);
    }

    void TPirate::virtual_TShip_NextDayLogic() {
        aPirate::TPirate_NextDayLogic(this);
    }

    std::uint8_t TPirate::virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return aPirate::TPirate_CanQueueReachablePlanet(this, Planet);
    }

    void TPirate::virtual_TShip_RepairBrokenEquipmentAtLocation() {
        aPirate::TPirate_RepairBrokenEquipmentAtLocation(this);
    }

    std::uint8_t TPirate::virtual_TShip_RecomputeFearState() {
        return aPirate::TPirate_RecomputeFearState(this);
    }

    std::uint8_t TPirate::virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) {
        return aPirate::TPirate_AcceptsRansomDemandFrom(this, Ship);
    }

    std::uint8_t TPirate::virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) {
        return aPirate::TPirate_TrustsAttackRequester(this, Ship);
    }

    std::uint8_t TPirate::virtual_TShip_EvaluateAllyRelationAndStrength(aShip::TShip* Ship) {
        return aPirate::TPirate_EvaluateAllyRelationAndStrength(this, Ship);
    }

    void TPirate::virtual_TShip_AssignWeaponTargetsInStar() {
        aPirate::TPirate_AssignWeaponTargetsInStar(this);
    }

    std::uint8_t TPirate::virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        return aPirate::TPirate_BuildMoneyExtortionResponse(this, OtherShip, Response, DemandedAmount);
    }

    std::uint8_t TPirate::virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) {
        return aPirate::TPirate_BuildCargoExtortionResponse(this, OtherShip, Response);
    }

    std::uint8_t TPirate::virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        return aPirate::TPirate_BuildAttackRequestResponse(this, Requester, Response, Target);
    }

    std::uint8_t TPirate::virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aPirate::TPirate_AcceptPartnershipOffer(this, OtherShip, Response, PaymentAmount);
    }

    std::uint8_t TPirate::virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aPirate::TPirate_BuildPartnershipOfferResponse(this, OtherShip, Response, PaymentAmount);
    }

    float TPirate::virtual_TShip_EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        return aPirate::TPirate_EvaluateStatBonus(this, BonusKind, Value);
    }

    float TPirate::virtual_TShip_EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
        return aPirate::TPirate_EvaluateWeaponDamage(this, Weapon, IncludeAdditiveBonuses, BaseDamage);
    }

    void TPirate::virtual_TShip_RefreshCurrentStanding() {
        aPirate::TPirate_RefreshCurrentStanding(this);
    }

} // namespace aPirate
