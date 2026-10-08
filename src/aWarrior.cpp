#include "layout/aWarrior.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Process.hpp"
#include "types/Types.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aMissile.hpp"
#include "types/aPirate.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "types/aScript.hpp"
#include "types/aTranclucator.hpp"
#include "units/ClassesImports.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MathImports.hpp"
#include "units/SE_Space.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aEFilmEnd.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPlayer.hpp"
#include "units/aShip.hpp"
#include "units/aWarrior.hpp"

namespace aWarrior {
    static void GetWeaponRangeBounds(aShip::TShip* Ship, std::int32_t& Minimum, std::int32_t& Maximum, TWarrior* Self);

    static std::int32_t AreaWeaponScore(aShip::TShip* Ship, TWarrior* Self);

    const pas::Array<std::int32_t, 22, 27> WarriorSkillBonusWeights = pas::Array<std::int32_t, 22, 27>{{100, 100, 80,
        10, 10, 10}};

    // EvaluateStatBonus dispatches bonus kinds 13..20 to these slot cases.
    const pas::Array<std::int32_t, 13, 20> WarriorSlotBonusWeights = pas::Array<std::int32_t, 13, 20>{{80, 80, 200, 0,
        200, 150, 0, 10}};

    void TWarrior_Destroy(TWarrior* Self) {
        std::int32_t Index = pas::list_indexof(Self->HomePlanet->Warriors, reinterpret_cast<void*>(Self));
        if (Index >= 0) {
            pas::list_delete(Self->HomePlanet->Warriors, Index);
        }
        aNormalShip::TNormalShip_Destroy(Self);
    }

    // Sets location, money and WarriorType; registers the ship with its star and home garrison.
    void TWarrior::InitGenerated(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, TWarriorType Kind) {
        std::int32_t FirstNameIndex{};
        std::int32_t LastNameIndex{};
        std::uint8_t TechLevel{};
        EC_BlockPar::TBlockParEC* Config{};
        aItem::TWeapon* Weapon{};
        // Nested helper with caller-popped static link.
        auto SelectTechLevel = [&](std::int32_t Minimum, std::int32_t Maximum) -> std::int32_t {
            std::int32_t Result = pas::real_round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 1.0, 8.0,
                Minimum, Maximum));
            return std::max<std::int32_t>(
                std::min<std::int32_t>(Result + aMyFunction::NextRandomIntRange(-2, 2, this->RandomState), Maximum),
                Minimum);
        };
        HomePlanet = Planet;
        CurrentPlanet = HomePlanet;
        pas::list_add(CurrentPlanet->Warriors, reinterpret_cast<void*>(this));
        CurrentStar = CurrentPlanet->CurrentStar;
        PilotRace = HomePlanet->RaceId;
        OwnerId = aConst::RaceToOwner(PilotRace);
        SetMoney(InitialMoney);
        TypeId = aGalaxyStruct::stWarrior;
        WarriorType = Kind;
        if (WarriorType == wtFlagship) {
            --aGalaxy::Galaxy->RangerSpawnQuotas[PilotRace];
        }
        Name = pas::WideString();
        if (GR_Main::ModShipNameConfig != nullptr && GR_Main::ModShipNameConfig->CountBlocks(u"Warrior"_wref.get()) >
                0) {
            Config = GR_Main::ModShipNameConfig->GetBlock(u"Warrior"sv);
            if (aConst::RaceToOwner(PilotRace) == OwnerId) {
                Config = Config->GetBlock(pas::view(aConst::OwnerToSys(OwnerId)));
            } else {
                if (Config->CountBlocks(aConst::OwnerToSys(OwnerId)) > 0) {
                    Config = Config->GetBlock(pas::view(aConst::OwnerToSys(OwnerId)));
                }
                if (Config->CountBlocks(aConst::OwnerToSys(aConst::RaceToOwner(PilotRace))) > 0) {
                    Config = Config->GetBlock(pas::view(aConst::OwnerToSys(aConst::RaceToOwner(PilotRace))));
                }
            }
            LastNameIndex = 0;
            FirstNameIndex = Config->GetParamCount() - 1;
            Name = pas::concat_wide({
                    Config->GetParamValue(aMyFunction::NextRandomIntRange(LastNameIndex, FirstNameIndex, RandomState)),
                    u" ", u"-",
                    pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 100 + 1)), u"-"});
        }
        if (GetName().length() == 0) {
            Config = GR_Main::LanguageDataConfig->GetBlock(u"ShipName"sv)->GetBlock(u"Warrior"sv);
            if (aConst::RaceToOwner(PilotRace) == OwnerId) {
                Config = Config->GetBlock(pas::view(aConst::OwnerToSys(OwnerId)));
            } else {
                if (Config->CountBlocks(aConst::OwnerToSys(OwnerId)) > 0) {
                    Config = Config->GetBlock(pas::view(aConst::OwnerToSys(OwnerId)));
                }
                if (Config->CountBlocks(aConst::OwnerToSys(aConst::RaceToOwner(PilotRace))) > 0) {
                    Config = Config->GetBlock(pas::view(aConst::OwnerToSys(aConst::RaceToOwner(PilotRace))));
                }
            }
            LastNameIndex = 0;
            FirstNameIndex = Config->GetParamCount() - 1;
            Name = pas::concat_wide({
                    Config->GetParamValue(aMyFunction::NextRandomIntRange(LastNameIndex, FirstNameIndex, RandomState)),
                    u" ", u"-",
                    pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 100 + 1)), u"-"});
        }
        if (aPlayer::GetPlayer() != nullptr) {
            {
                std::int32_t min = std::min<std::int32_t>(5, aPlayer::GetPlayer()->Rank + 2);
                Rank = aMyFunction::NextRandomIntRange(0, min, RandomState);
            }
            if (Rank > 5) {
                Rank = 5;
            }
            if (WarriorType == wtFlagship) {
                ++Rank;
                if (Rank < 4) {
                    Rank = 4;
                }
            }
            {
                std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0,
                    static_cast<std::int32_t>(aConst::CoalitionRankPointThresholds[Rank]) / 2, RandomState);
                aNormalShip::TNormalShip* self = this;
                self->AddRankPoints(nextRandomIntRange);
            }
            if (!aGalaxy::Galaxy->IsZeroStartingExperienceEnabled()) {
                GainExperience(
                    pas::real_round(aMyFunction::RemapClamped(Rank, 0.0, 1.0E+3, aConst::TotalSkillTrainingCost / 6,
                        aConst::TotalSkillTrainingCost / 2)), aGalaxyStruct::esUnscaled);
                {
                    double techLevel = aGalaxy::Galaxy->TechLevel;
                    double nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(0, aConst::TotalSkillTrainingCost / 2,
                        RandomState);
                    std::int32_t round = pas::real_round(aMyFunction::RemapClamped(techLevel, 3.0, 8.0, 0.0,
                        nextRandomIntRange_2));
                    aShip::TShip* self_2 = this;
                    self_2->GainExperience(round, aGalaxyStruct::esUnscaled);
                }
            }
        }
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        aRanger::TRanger* Ranger = pas::checked_cast<aRanger::TRanger*>(aGalaxy::Galaxy->StrongestRanger);
        if (Ranger == nullptr) {
            TechLevel = 4;
        } else {
            TechLevel = Ranger->GetHull()->TechLevel;
        }
        if (WarriorType == wtFlagship) {
            {
                aGalaxyStruct::TOwnerId raceToOwner = aConst::RaceToOwner(PilotRace);
                std::uint8_t selectTechLevel = SelectTechLevel(3, 8);
                std::uint16_t round_2 = pas::real_round(static_cast<float>(static_cast<double>(3 * aConst::HullBaseSize) *
                    aConst::EquipmentSizeFactors[5]));
                aShip::TShip* self_3 = this;
                aShip::TShip_CreateAndEquipHull(self_3, round_2, selectTechLevel, raceToOwner, -1, false);
            }
            CreateAndEquipFuelTanks(
                pas::real_round(static_cast<float>(static_cast<double>(2 * aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5])),
                1, OwnerId);
            CreateAndEquipEngine(
                pas::real_round(static_cast<float>(static_cast<double>(2 * aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1])), 1,
                OwnerId);
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                Weapon = CreateAndEquipWeapon(aConst::t_Flux, 2 * aConst::WeaponInfos[aConst::t_Flux].AverageSize, 1,
                        OwnerId);
                Weapon->DetailImprovement = 3;
                Weapon->Improve(aItem::ikAny);
            }
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                Weapon = CreateAndEquipWeapon(aConst::t_Flux, 2 * aConst::WeaponInfos[aConst::t_Flux].AverageSize, 1,
                        OwnerId);
                Weapon->DetailImprovement = 3;
                Weapon->Improve(aItem::ikAny);
            }
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                Weapon = CreateAndEquipWeapon(aConst::t_Flux, 2 * aConst::WeaponInfos[aConst::t_Flux].AverageSize, 1,
                        OwnerId);
                Weapon->DetailImprovement = 3;
                Weapon->Improve(aItem::ikAny);
            }
            if (GetSlotCountForItemType(aConst::t_Radar) > 0) {
                aGalaxyStruct::TOwnerId ownerId = OwnerId;
                double cpp_right = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, RandomState)];
                std::int32_t round_3 = pas::real_round(static_cast<float>(2 * aConst::RadarBaseSize * cpp_right));
                aShip::TShip* self_4 = this;
                self_4->CreateAndEquipRadar(round_3, 1, ownerId);
            }
            if (GetSlotCountForItemType(aConst::t_Scaner) > 0) {
                aGalaxyStruct::TOwnerId ownerId_2 = OwnerId;
                double cpp_right_2 = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, RandomState)];
                std::int32_t round_4 = pas::real_round(static_cast<float>(2 * aConst::ScannerBaseSize * cpp_right_2));
                aShip::TShip* self_5 = this;
                self_5->CreateAndEquipScanner(round_4, 1, ownerId_2);
            }
        } else {
            {
                std::int32_t min_2 = std::min<std::int32_t>(static_cast<std::int32_t>(TechLevel),
                    SelectTechLevel(1, 6));
                aGalaxyStruct::TOwnerId raceToOwner_2 = aConst::RaceToOwner(PilotRace);
                std::int32_t selectRandomHullSeries = SelectRandomHullSeries();
                std::uint16_t round_5 = pas::real_round(static_cast<float>(static_cast<double>(aConst::HullBaseSize) *
                    aConst::EquipmentSizeFactors[5]));
                aShip::TShip* self_6 = this;
                aShip::TShip_CreateAndEquipHull(self_6, round_5, min_2, raceToOwner_2, selectRandomHullSeries, false);
            }
            CreateAndEquipFuelTanks(
                pas::real_round(static_cast<float>(static_cast<double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5])), 1,
                OwnerId);
            CreateAndEquipEngine(
                pas::real_round(static_cast<float>(static_cast<double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1])), 1,
                OwnerId);
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                CreateAndEquipWeapon(aConst::t_IndustrialLaser,
                    aConst::WeaponInfos[aConst::t_IndustrialLaser].AverageSize, 1, OwnerId);
            }
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                CreateAndEquipWeapon(aConst::t_Flux, aConst::WeaponInfos[aConst::t_Flux].AverageSize, 1, OwnerId);
            }
            if (GetSlotCountForItemType(aConst::t_Radar) > 0) {
                aGalaxyStruct::TOwnerId ownerId_3 = OwnerId;
                double cpp_left = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, RandomState)];
                std::int32_t round_6 = pas::real_round(static_cast<float>(cpp_left * aConst::RadarBaseSize));
                aShip::TShip* self_7 = this;
                self_7->CreateAndEquipRadar(round_6, 1, ownerId_3);
            }
        }
        if (GetCargoFreeSpace() < 0) {
            GetHull()->Weight = GetHull()->Weight + pas::abs(GetCargoFreeSpace()) + 10;
            GetHull()->HullPoints = GetHull()->Weight;
        }
        TrainSkillsAutomatically();
        RefreshDerivedStats(true);
        this->virtual_TShip_RefreshCurrentStanding();
        SmoothedSpeed = Speed;
        SmoothedEnemySpeed = Speed;
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        if (WarriorType == wtFlagship) {
            aShip::TShip_BuyEquipmentAtLocation(this, true);
            aShip::TShip_BuyEquipmentAtLocation(this, true);
            aShip::TShip_BuyEquipmentAtLocation(this, true);
        }
    }

    void TWarrior::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aNormalShip::TNormalShip::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(WarriorType));
    }

    void TWarrior::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aNormalShip::TNormalShip::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion >= 130) {
            WarriorType = static_cast<TWarriorType>(Buffer->GetByte());
        } else {
            WarriorType = wtRegular;
        }
    }

    void TWarrior_NextDay(TWarrior* Self) {
        aNormalShip::TNormalShip_NextDay(Self);
        try {
            if (Self->ScriptShip != nullptr && Self->HasScriptControl()) {
                Self->ScriptNextDay();
                if (Self->ScriptShip != nullptr) {
                    return;
                }
            }
            if (Self->WarriorType == wtFlagship) {
                Self->SetMoney(Self->Money + std::max<std::int32_t>(1000,
                        std::min<std::int32_t>(5000, aGalaxy::Galaxy->MaxRangerWealth / 15)));
            }
            Self->virtual_TShip_NextDayLogic();
            if (Self->ScriptShip != nullptr && !Self->HasScriptControl()) {
                Self->ScriptNextDay();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TWarrior.NextDay ", Self->GetFullName(u" "_wref.get())}))));
            } else {
                throw;
            }
        }
    }

    void TWarrior_NextDayLogic(TWarrior* Self) {
        static const pas::Set<0, 255> FriendlyStationMask = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::ssCoalitionMilitary}, {aGalaxyStruct::ssCoalitionActive}});
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Station{};
        std::int32_t Stage = 0;
        if (Self->WarriorType == wtFlagship) {
            aWarrior::TWarrior_NextDayFlagshipLogic(Self);
        } else {
            try {
                if (Self->CurrentPlanet != nullptr) {
                    Stage = 1;
                    if (pas::contains(aConst::PlanetOwnerMasks.Coalition, Self->CurrentPlanet->OwnerId)) {
                        Stage = 2;
                        Self->RepairBrokenEquipmentAtLocation();
                        Self->AutoEquipInventory();
                        Self->OptimizeInventory();
                        Self->RefuelAtLocation();
                        Self->ReloadWeaponAmmo();
                        Self->ProcessUnseenProgression();
                        Self->TrainSkillsAutomatically();
                        if (Self->RepairHullAtLocation()) {
                            return;
                        }
                        Stage = 3;
                        if (Self->LiberationGroup != nullptr) {
                            Self->ProcessLiberationGroupRoute();
                        }
                        if (Self->CurrentPlanet != Self->HomePlanet || Self->LiberationGroup != nullptr ||
                            Self->IsHomePatrolTurn()) {
                            Self->OrderTakeoff();
                        }
                    } else {
                        Self->OrderTakeoff();
                    }
                    Stage = 4;
                } else if (Self->DockedTo != nullptr) {
                    Stage = 5;
                    if (!pas::in_range(Self->DockedTo->TypeId,
                        static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter),
                        static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                        if (Self->DockedTo->InNormalSpace()) {
                            Self->OrderTakeoff();
                        } else {
                            Self->OrderNone(false);
                        }
                        return;
                    }
                    Self->SynchronizeDockedLocation();
                    Self->RepairBrokenEquipmentAtLocation();
                    Self->AutoEquipInventory();
                    Self->OptimizeInventory();
                    Self->RefuelAtLocation();
                    Self->ReloadWeaponAmmo();
                    Self->ProcessUnseenProgression();
                    Self->TrainSkillsAutomatically();
                    if (Self->RepairHullAtLocation()) {
                        return;
                    }
                    aShip::TShip_BuyEquipmentAtLocation(Self, false);
                    Self->RestoreEssentialEquipment();
                    Stage = 6;
                    if (Self->LiberationGroup != nullptr) {
                        Self->ProcessLiberationGroupRoute();
                    }
                    if (Self->DockedTo->TypeId == aGalaxyStruct::rstMilitaryBase &&
                        reinterpret_cast<aRuins::TRuins*>(Self->DockedTo)->FlyToStar != nullptr &&
                        reinterpret_cast<aRuins::TRuins*>(Self->DockedTo)->FlyToStar != Self->DockedTo->CurrentStar) {
                        return;
                    }
                    if (Self->DockedTo->InNormalSpace()) {
                        Self->OrderTakeoff();
                    }
                    Stage = 7;
                } else if (Self->InNormalSpace()) {
                    Stage = 8;
                    Self->virtual_TShip_RecomputeFearState();
                    Self->AssignWeaponTargetsInStar();
                    Self->AfterburnerActive = false;
                    Stage = 9;
                    if (Self->InFear) {
                        Stage = 10;
                        if (Self->Order == aShip::soLand || Self->Order == aShip::soJump) {
                            Self->UpdateAfterburnerState();
                        }
                        if (Self->Order != aShip::soLand) {
                            Stage = 11;
                            Self->BuildReachablePlanetQueue();
                            Planet = Self->SelectNearestQueuedPlanet();
                            if (Planet != nullptr && Planet->CurrentStar == Self->CurrentStar) {
                                Stage = 12;
                                Self->OrderLanding(Planet, true);
                                Self->UpdateAfterburnerState();
                            } else {
                                Stage = 13;
                                Station =
                                    Self->FindNearestDockableStation(
                                        static_cast<aGalaxyStruct::TShipStandings>(FriendlyStationMask));
                                if (Station != nullptr) {
                                    Stage = 14;
                                    Self->OrderLanding(Station, true);
                                    Self->UpdateAfterburnerState();
                                } else if (Self->LiberationGroup == nullptr) {
                                    Stage = 15;
                                    Self->SelectEnemyShipInStar();
                                    Self->EngageEnemyShip();
                                    if (Self->Order == aShip::soNone) {
                                        Self->MoveToRandomPatrolPoint();
                                    }
                                }
                            }
                        }
                        Stage = 16;
                    } else {
                        Stage = 17;
                        if (Self->LiberationGroup == nullptr) {
                            Stage = 18;
                            Self->SelectEnemyShipInStar();
                            Self->EngageEnemyShip();
                            Stage = 19;
                            if (Self->Order == aShip::soNone) {
                                if (Self->IsHomePatrolTurn()) {
                                    Self->MoveToRandomPlanetOrbit();
                                } else if (!Self->NavigateToHomePlanet()) {
                                    Self->MoveToRandomPatrolPoint();
                                }
                            }
                            Stage = 20;
                        }
                    }
                    if (Self->LiberationGroup != nullptr) {
                        Stage = 21;
                        Self->AfterburnerActive = false;
                        Self->RefreshDerivedStats(true);
                        Self->ProcessLiberationGroupRoute();
                        Stage = 22;
                        if (Self->LiberationGroup == nullptr) {
                            Stage = 23;
                            Self->SelectEnemyShipInStar();
                            Self->EngageEnemyShip();
                            if (Self->Order == aShip::soNone && !Self->NavigateToHomePlanet()) {
                                Self->MoveToRandomPatrolPoint();
                            }
                        }
                    }
                }
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                        static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                        u"Error in procedure TWarrior.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ",
                        pas::wide_int_to_str(Stage)}))));
                } else {
                    throw;
                }
            }
        }
    }

    // Flagship branch; its diagnostic retains TWarrior.NextDayLogic.
    void TWarrior_NextDayFlagshipLogic(TWarrior* Self) {
        static const pas::Set<0, 255> FriendlyStationMask = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::ssCoalitionMilitary}, {aGalaxyStruct::ssCoalitionActive}});
        static const pas::Set<0, 255> AnyStationMask = pas::constant_set<pas::Set<0, 255>>({});
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Station{};
        aShip::TShip* Ship{};
        pas::List* Stations{};
        std::int32_t I{};
        auto RepairHullWithNodes = [&]() -> void {
            std::int32_t Needed{};
            std::int32_t Available{};
            std::int32_t Restored{};
            float Fraction{};
            aEFilmEnd::PEFilmEndEntry Entry{};
            SE_Weapon::TWeaponSE* Effect{};
            if (pas::is_one_of<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPeleng>(Self->PilotRace) && Self->InFear) {
                Available = Self->GetCarriedNodeCount();
                if (Available > 0) {
                    Needed = MathImports::Ceil(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>((Self->GetHull()->Weight - Self->GetHull()->HullPoints) *
                            (static_cast<float>(static_cast<float>(Self->GetCombatStatusStrength(aShip::cseBWRepairDebuff) * 0.002) + 1.0))) / 1.0E+1) / 2.0) *
                            aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 2.0, 8.0, 5.0, 1.0)));
                    if (Needed > 0) {
                        Available = std::min<std::int32_t>(Needed, Available);
                        Self->ConsumeNodes(Available);
                        Fraction = static_cast<float>(static_cast<double>(Available) / Needed);
                        Restored = MathImports::Ceil(aMyFunction::RemapClamped(Fraction, 0.0, 1.0, 0.01,
                                Self->GetHull()->Weight - Self->GetHull()->HullPoints));
                        Self->GetHull()->HullPoints += Restored;
                        Self->AddCombatStatusStrength(aShip::cseBWRepairDebuff, static_cast<float>(Restored / 2.0), nullptr);
                        Self->RefreshDerivedStats(true);
                        if (aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar &&
                            aPlayer::GetPlayer()->InNormalSpace()) {
                            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create,
                                    u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                            Effect->SetEndpoints(nullptr, Self->Graphic);
                            Effect->SetHit(aConst::OwnerToFilmColor(Self->OwnerId), -Restored, false, true);
                            if (Globals::TrailingFilmEffects == nullptr) {
                                Globals::TrailingFilmEffects =
                                    pas::construct_call<aEFilmEnd::TEFilmEnd>(aEFilmEnd::TEFilmEnd_Create);
                            }
                            Entry = Globals::TrailingFilmEffects->AppendEntry();
                            SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject), Effect);
                            SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1),
                                nullptr);
                            Entry->SceneObject->AttachToSpace(Globals::SpaceProcess->Space);
                        }
                    }
                }
            }
        };
        auto RepairEquipmentWithNodes = [&]() -> void {
            std::int32_t I{};
            aItem::TEquipment* Equipment{};
            std::uint8_t NeedsRepair{};
            std::int32_t TotalCost{};
            std::int32_t Available{};
            std::int32_t Threshold{};
            float Fraction{};
            if (pas::is_one_of<aGalaxyStruct::oiFeyan, aGalaxyStruct::oiGaal>(Self->PilotRace)) {
                Available = Self->GetCarriedNodeCount();
                if (Available > 0) {
                    NeedsRepair = false;
                    TotalCost = 0;
                    Threshold = pas::real_round(aMyFunction::RemapClamped(Self->CargoFreeSpace, 0.0,
                            Self->GetHull()->Weight / 4, 9.0E+1, 5.0E+1));
                    {
                        const std::int32_t cpp_last = pas::list_count(Self->Inventory) - 1;
                        if (1 <= cpp_last) {
                            for (I = 1; I <= cpp_last; ++I) {
                                Equipment = pas::list_at<aItem::TEquipment>(Self->Inventory, I);
                                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(
                                    Equipment); (!(weapon != nullptr) || weapon->GetWeaponInfo()->Availability !=
                                        aGalaxyStruct::waNotSoldAndNodeRepair) && aShip::TShip_CanRepairEquipmentTech(
                                        Self, Equipment) && Equipment->ItemType != aConst::t_Hull &&
                                    Equipment->EquippedFlag != 0) {
                                    if (static_cast<double>(Threshold) >= Equipment->ConditionPercent) {
                                        NeedsRepair = true;
                                    }
                                    TotalCost += pas::real_round(aItem::TEquipment_CalculateRepairCost(Equipment));
                                }
                            }
                        }
                    }
                    if (NeedsRepair) {
                        TotalCost = pas::real_round(static_cast<float>(static_cast<float>(static_cast<float>(TotalCost * 0x1.47ae147ae147ap-9) / 2.0) * aMyFunction::RemapClamped(
                                    aGalaxy::Galaxy->TechLevel, 2.0, 8.0, 5.0, 1.0)));
                        if (TotalCost == 0) {
                            TotalCost = 1;
                        }
                        Available = std::min<std::int32_t>(TotalCost, Available);
                        Fraction = static_cast<float>(static_cast<double>(Available) / TotalCost);
                        Self->ConsumeNodes(Available);
                        {
                            const std::int32_t cpp_last_2 = pas::list_count(Self->Inventory) - 1;
                            if (0 <= cpp_last_2) {
                                for (I = 0; I <= cpp_last_2; ++I) {
                                    Equipment = pas::list_at<aItem::TEquipment>(Self->Inventory, I);
                                    if (aItem::TWeapon* weapon_2 = pas::class_cast_if<aItem::TWeapon*>(
                                        Equipment); (!(weapon_2 != nullptr) ||
                                        weapon_2->GetWeaponInfo()->Availability !=
                                            aGalaxyStruct::waNotSoldAndNodeRepair) &&
                                        aShip::TShip_CanRepairEquipmentTech(Self, Equipment) && Equipment->ItemType !=
                                            aConst::t_Hull && Equipment->EquippedFlag != 0) {
                                        Equipment->ConditionPercent = aMyFunction::RemapClamped(Fraction, 0.0, 1.0,
                                                Equipment->ConditionPercent, 1.0E+2);
                                        if (Equipment->ConditionPercent > 0.0) {
                                            Equipment->BrokenFlag = 0;
                                        }
                                    }
                                }
                            }
                        }
                        Self->RefreshDerivedStats(true);
                    }
                }
            }
        };
        std::int32_t Stage = 0;
        try {
            if (Self->CurrentPlanet != nullptr) {
                Stage = 1;
                if (pas::contains(aConst::PlanetOwnerMasks.Coalition, Self->CurrentPlanet->OwnerId)) {
                    Stage = 2;
                    Self->DepositCarriedNodes();
                    Self->RepairBrokenEquipmentAtLocation();
                    Self->AutoEquipInventory();
                    Self->OptimizeInventory();
                    Self->RefuelAtLocation();
                    Self->ReloadWeaponAmmo();
                    Self->ProcessUnseenProgression();
                    Self->TrainSkillsAutomatically();
                    if (Self->RepairHullAtLocation()) {
                        return;
                    }
                    Stage = 3;
                    if (Self->LiberationGroup != nullptr) {
                        Self->ProcessLiberationGroupRoute();
                    }
                    if (Self->CurrentPlanet != Self->HomePlanet || Self->LiberationGroup != nullptr ||
                        Self->IsHomePatrolTurn()) {
                        Self->OrderTakeoff();
                    }
                } else {
                    Self->OrderTakeoff();
                }
                Stage = 4;
            } else if (Self->DockedTo != nullptr) {
                Stage = 5;
                Self->SynchronizeDockedLocation();
                Self->DepositCarriedNodes();
                Self->RepairBrokenEquipmentAtLocation();
                Self->AutoEquipInventory();
                Self->OptimizeInventory();
                Self->RefuelAtLocation();
                Self->ReloadWeaponAmmo();
                Self->ProcessUnseenProgression();
                Self->TrainSkillsAutomatically();
                if (Self->RepairHullAtLocation()) {
                    return;
                }
                aShip::TShip_BuyEquipmentAtLocation(Self, false);
                Self->RestoreEssentialEquipment();
                Stage = 6;
                if (Self->LiberationGroup != nullptr) {
                    Self->ProcessLiberationGroupRoute();
                }
                if (aGalaxy::TGalaxy::FindMilitaryBaseInTransit() == Self->DockedTo) {
                    return;
                }
                if (Self->DockedTo->InNormalSpace()) {
                    Self->OrderTakeoff();
                }
                Stage = 7;
            } else if (Self->InNormalSpace()) {
                Stage = 8;
                Self->virtual_TShip_RecomputeFearState();
                RepairHullWithNodes();
                RepairEquipmentWithNodes();
                Self->QueueItemsWithinPickupRange();
                Self->AssignWeaponTargetsInStar();
                Self->AfterburnerActive = false;
                Stage = 9;
                if (Self->InFear) {
                    Stage = 10;
                    if (Self->Order == aShip::soLand || Self->Order == aShip::soJump) {
                        Self->UpdateAfterburnerState();
                    }
                    if (Self->Order != aShip::soLand) {
                        Stage = 11;
                        Self->BuildReachablePlanetQueue();
                        Planet = Self->SelectNearestQueuedPlanet();
                        if (Planet != nullptr && Planet->CurrentStar == Self->CurrentStar) {
                            Stage = 12;
                            Self->OrderLanding(Planet, true);
                            Self->UpdateAfterburnerState();
                        } else {
                            Stage = 13;
                            Station =
                                Self->FindNearestDockableStation(
                                    static_cast<aGalaxyStruct::TShipStandings>(FriendlyStationMask));
                            if (Station != nullptr) {
                                Stage = 14;
                                Self->OrderLanding(Station, true);
                                Self->UpdateAfterburnerState();
                            } else if (Self->LiberationGroup == nullptr) {
                                Stage = 15;
                                Self->SelectEnemyShipInStar();
                                Self->ManeuverFlagship();
                            }
                        }
                    }
                    Stage = 16;
                } else {
                    Stage = 17;
                    if (Self->LiberationGroup == nullptr) {
                        Stage = 18;
                        Self->SelectEnemyShipInStar();
                        Self->ManeuverFlagship();
                        Stage = 19;
                        if (Self->Order == aShip::soNone) {
                            if (aMyFunction::NextRandomIntRange(1, 100, Self->RandomState) > 80) {
                                Station = nullptr;
                                Stations = pas::make_object<pas::List>();
                                {
                                    const std::int32_t cpp_last = pas::list_count(Self->CurrentStar->Ships) - 1;
                                    if (0 <= cpp_last) {
                                        for (I = 0; I <= cpp_last; ++I) {
                                            Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                                            if (pas::in_range(Ship->TypeId,
                                                static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter),
                                                static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) &&
                                                Ship->virtual_TShip_CanDock(Self) && pas::in_range(
                                                    Ship->CurrentStanding,
                                                    static_cast<std::int32_t>(aGalaxyStruct::ssCoalitionMilitary),
                                                    static_cast<std::int32_t>(aGalaxyStruct::ssNeutral))) {
                                                if (static_cast<double>(aMyFunction::PointDistanceSquared(
                                                    Self->Position, Ship->Position)) >=
                                                    pas::sqr(Self->CalculateSpeed())) {
                                                    pas::list_add(Stations, reinterpret_cast<void*>(Ship));
                                                }
                                            }
                                        }
                                    }
                                }
                                if (pas::list_count(Stations) > 0) {
                                    Station = pas::list_at<aShip::TShip>(Stations,
                                            aMyFunction::NextRandomIntRange(0, pas::list_count(Stations) - 1,
                                                Self->RandomState));
                                }
                                pas::free(Stations);
                                if (Station != nullptr) {
                                    Self->OrderLanding(Station, false);
                                }
                            }
                            if (Self->Order == aShip::soNone) {
                                Self->TryCollectBestFloatingItem(50);
                            }
                            if (Self->Order == aShip::soNone && Self->IsHomePatrolTurn()) {
                                Self->MoveToRandomPlanetOrbit();
                            }
                            if (Self->Order == aShip::soNone && !Self->NavigateToHomePlanet()) {
                                Self->MoveToRandomPatrolPoint();
                            }
                        }
                        Stage = 20;
                    }
                }
                if (Self->LiberationGroup != nullptr) {
                    Stage = 21;
                    Self->AfterburnerActive = false;
                    Self->RefreshDerivedStats(true);
                    Self->ProcessLiberationGroupRoute();
                    Stage = 22;
                    if (Self->LiberationGroup == nullptr) {
                        Stage = 23;
                        Self->SelectEnemyShipInStar();
                        Self->ManeuverFlagship();
                        if (Self->Order == aShip::soNone) {
                            Self->TryCollectBestFloatingItem(50);
                        }
                        if (Self->Order == aShip::soNone) {
                            if (aMyFunction::NextRandomIntRange(1, 100, Self->RandomState) > 40) {
                                Station =
                                    Self->FindNearestDockableStation(
                                        static_cast<aGalaxyStruct::TShipStandings>(AnyStationMask));
                                if (Station != nullptr) {
                                    Self->OrderLanding(Station, false);
                                }
                            }
                            if (Self->Order == aShip::soNone && !Self->NavigateToHomePlanet()) {
                                Self->MoveToRandomPatrolPoint();
                            }
                        }
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TWarrior.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ",
                    pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    std::uint8_t TWarrior::NavigateToHomePlanet() {
        if (HomePlanet->CurrentStar == CurrentStar) {
            OrderLanding(HomePlanet, false);
            return true;
        } else if (GetFuelTanks()->Fuel == GetFuelTanks()->Capacity || static_cast<double>(JumpRange * JumpRange) >=
                aMyFunction::PointDistanceSquared(HomePlanet->CurrentStar->Position, CurrentStar->Position)) {
            OrderJump(HomePlanet->CurrentStar, false);
            return true;
        } else {
            BuildReachablePlanetQueue();
            if (pas::list_count(PlanetQueue) > 0) {
                {
                    std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0,
                        pas::list_count(PlanetQueue) - 1, RandomState);
                    pas::List* planetQueue = PlanetQueue;
                    pas::Object* cpp_arg = pas::list_at<pas::Object>(planetQueue, nextRandomIntRange);
                    aShip::TShip* self = this;
                    self->OrderLanding(cpp_arg, false);
                }
                return true;
            }
            return false;
        }
    }

    void TWarrior::BuildReachablePlanetQueue() {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        ClearPlanetQueue();
        PlanetQueue = pas::make_object<pas::List>();
        if (Speed != 0 && CurrentStar->Status.CustomFaction == u"") {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Planets) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                    if (Planet->IsCoalitionOwned) {
                        pas::list_add(PlanetQueue, reinterpret_cast<void*>(Planet));
                    }
                }
            }
        }
    }

    void TWarrior::MoveToRandomPatrolPoint() {
        EC_Struct::TPointF Destination{};
        Destination.X = aMyFunction::NextRandomIntRange(-2000, 2000, RandomState);
        Destination.Y = aMyFunction::NextRandomIntRange(-2000, 2000, RandomState);
        OrderMove(Destination, false);
    }

    void TWarrior::RepairBrokenEquipmentAtLocation() {
        std::int32_t I{};
        aItem::TEquipment* Equipment{};
        aItem::TArtefact* Artefact{};
        {
            const std::int32_t cpp_last = pas::list_count(Inventory) - 1;
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Equipment = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if (Equipment->BrokenFlag != 0 || Equipment->ConditionPercent < 3.0E+1) {
                        Equipment->Repair();
                    }
                }
            }
        }
        if (CanRepairArtefactsAtLocation()) {
            const std::int32_t cpp_last_2 = pas::list_count(Artefacts) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
                    if (Artefact->BrokenFlag != 0 || Artefact->ConditionPercent < 3.0E+1) {
                        if (Artefact->EquippedFlag != 0) {
                            Artefact->Repair();
                        }
                    }
                }
            }
        }
    }

    aGalaxy::TStar* TWarrior::GetHomeStar() {
        return HomePlanet->CurrentStar;
    }

    pas::WideString TWarrior::GetName() {
        return Name;
    }

    pas::WideString TWarrior::GetFullName(const pas::WideString& Separator) {
        pas::WideString Path{};
        pas::WideString Text{};
        if (TypeNameOverrideKey == u"") {
            if (WarriorType == wtFlagship) {
                return pas::concat_wide({
                    aConst::LocalizedText(pas::concat_wide({u"ShipType.",
                        aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)), u".", GetTypeNameKey(), u"Big"})),
                    Separator, Name});
            }
            return pas::concat_wide({
                aConst::LocalizedText(pas::concat_wide({u"ShipType.",
                    aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)), u".", GetTypeNameKey()})), Separator, Name});
        }
        Path = pas::concat_wide({u"ShipType.", aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)), u".",
                TypeNameOverrideKey});
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

    pas::WideString TWarrior::GetTypeNameKey() {
        return u"Warrior"_w;
    }

    aGalaxyStruct::TGreetingShipCategory TWarrior::GetGreetingShipCategory() {
        return aGalaxyStruct::gscWarrior;
    }

    // Always rcWarrior.
    aGalaxyStruct::TRangerCareer TWarrior::GetDominantCareer() {
        return aGalaxyStruct::rcWarrior;
    }

    aGalaxyStruct::TPercent TWarrior::GetStrengthScaledPirateStatus() {
        return 0;
    }

    std::int32_t TWarrior::GetDesiredCargoFreeSpace() {
        return 0;
    }

    // Fills installed fuel tanks without charging Money.
    void TWarrior::RefuelAtLocation() {
        if (GetFuelTanks() != nullptr) {
            GetFuelTanks()->Fuel = GetFuelTanks()->Capacity;
        }
    }

    void TWarrior::ProcessUnseenProgression() {
        std::uint8_t Award{};
        if (DaysSincePlayerSeen >= 60 && aPlayer::GetPlayer() != nullptr) {
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.05) {
                std::int32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(100, 500, RandomState);
                aShip::TShip* self = this;
                self->GainExperience(seededRandomIntRange, aGalaxyStruct::esUnscaled);
            }
            if (aPlayer::GetPlayer()->Rank > Rank && aMyFunction::NextRandomUnitFloat(RandomState) < 0.01 && (Rank <
                    4 || WarriorType == wtFlagship && Rank < 6)) {
                {
                    std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(10, 20, RandomState);
                    aNormalShip::TNormalShip* self_2 = this;
                    self_2->AddRankPoints(nextRandomIntRange);
                }
                TryPromoteRank();
            }
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.02 && CurrentPlanet != nullptr && (AwardIds ==
                    nullptr || 2 * (Rank + 1) > pas::list_count(AwardIds))) {
                Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId),
                        pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment},
                            {aGalaxyStruct::atSecretMission}}),
                        pas::constant_set<aGalaxyStruct::TShipTypeMask>({
                            {aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation}}));
                if (Award != aGalaxyStruct::AwardNotFound) {
                    AddAward(Award);
                }
            }
        }
    }

    std::uint8_t TWarrior::RelationToNonRanger(aShip::TShip* Ship) {
        if (Ship->TypeId == aGalaxyStruct::stPirate) {
            return pas::real_round(pas::real_max<double>(1.0E+1,
                pas::real_min<double>(2.0E+1,
                    static_cast<float>(aConst::OwnerRelations[aConst::RaceToOwner(PilotRace)][Ship->OwnerId] * (static_cast<float>(0.5 *
                        aConst::PlanetRaceMarket[PilotRace].PirateRelationFactor))))));
        } else if (pas::is_one_of<aGalaxyStruct::stKling, aGalaxyStruct::stTranclucator>(Ship->TypeId)) {
            return 50;
        } else {
            return 100;
        }
    }

    std::uint8_t TWarrior::RelationToRanger(void* Ranger) {
        return static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(HomePlanet->RangerRelations,
            pas::list_indexof(aGalaxy::Galaxy->Rangers,
                reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))))));
    }

    void TWarrior::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Rangers,
            reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))));
        std::uint8_t Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(
            HomePlanet->RangerRelations, Index)));
        if (static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aGalaxyStruct::psCharisma, false) > 0 && Amount >
                0) {
            Amount += pas::real_round(static_cast<float>(Amount * static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(
                        aGalaxyStruct::psCharisma, false) * 0x1.999999999999bp-3));
        }
        std::int32_t Value = Amount + Relation;
        if (Value < 0) {
            Relation = 0;
        } else if (Value > 100) {
            Relation = 100;
        } else {
            Relation = Value;
        }
        pas::list_put(HomePlanet->RangerRelations, Index,
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Relation))));
        if (Relation < aGalaxyStruct::RelationBadMin && (EnemyShip == nullptr || EnemyShip->CurrentStar !=
                    CurrentStar)) {
            EnemyShip = static_cast<aShip::TShip*>(Ranger);
        }
        if (aPlayer::GetPlayer() == Ranger) {
            if (aMyFunction::RandomIntRange(0, 100) == 0) {
                SysUtilsImports::Sleep(1u);
            }
            if (static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(HomePlanet->RangerRelations,
                Index))) != Relation && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
    }

    void TWarrior::ReactToAttack(aShip::TShip* Attacker) {
        EnemyShip = Attacker;
        if (pas::contains(aConst::PlanetOwnerMasks.Coalition, HomePlanet->OwnerId) && Attacker->TypeId ==
                aGalaxyStruct::stRanger) {
            HomePlanet->ChangeRelationToRanger(Attacker, -3);
            if (Attacker->PartnerShip != nullptr && Attacker->PartnerShip->TypeId == aGalaxyStruct::stRanger) {
                HomePlanet->ChangeRelationToRanger(Attacker->PartnerShip, -3);
            }
            if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(
                Attacker); tranclucator != nullptr && tranclucator->OwnerShip != nullptr &&
                tranclucator->OwnerShip->TypeId == aGalaxyStruct::stRanger) {
                HomePlanet->ChangeRelationToRanger(tranclucator->OwnerShip, -3);
            }
        }
    }

    std::uint8_t TWarrior_RecomputeFearState(TWarrior* Self) {
        std::uint8_t Result{};
        if (Self->HasNoUsableWeapons() && Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar ==
                Self->CurrentStar) {
            Result = true;
            Self->InFear = true;
        } else {
            {
                bool cpp_condition_2 = Self->GetHull()->HullPoints < static_cast<float>(Self->GetHull()->Weight * 0x1.3333333333334p-2) &&
                    Self->GetHull()->HullPoints < 200 || Self->GetHull()->HullPoints < 130;
                if (!cpp_condition_2) {
                    bool cpp_condition = static_cast<float>(static_cast<float>(Self->GetHull()->Weight * 0.5) *
                        aConst::OwnerInfo[Self->OwnerId].FearThresholdScale) > Self->GetHull()->HullPoints &&
                        Self->EnemyShip != nullptr && Self->EnemyShip->OrderTarget == Self;
                    if (cpp_condition) {
                        double cpp_left = aShip::TShip_ChanceToWin(Self, Self->EnemyShip);
                        cpp_condition = static_cast<float>(cpp_left - static_cast<float>(aConst::OwnerInfo[Self->OwnerId].FearThresholdScale / 2.0)) < 0.0;
                    }
                    cpp_condition_2 = cpp_condition;
                }
                Result = cpp_condition_2;
            }
            Self->InFear = Result;
        }
        return Result;
    }

    std::uint8_t TWarrior_AcceptsRansomDemandFrom(TWarrior* Self, aShip::TShip* Ship) {
        bool cpp_condition = static_cast<float>(static_cast<float>(Self->GetHull()->Weight * 0.5) * aConst::OwnerInfo[Self->OwnerId].FearThresholdScale) >
            Self->GetHull()->HullPoints;
        if (cpp_condition) {
            double cpp_left = aShip::TShip_ChanceToWin(Self, Ship);
            cpp_condition = static_cast<float>(cpp_left - static_cast<float>(aConst::OwnerInfo[Self->OwnerId].FearThresholdScale / 2.0)) < 0.0;
        }
        return cpp_condition;
    }

    std::uint8_t TWarrior::AcceptsAppealFrom(aShip::TShip* Ship) {
        double cpp_right = aMyFunction::RemapClamped(Ship->Strength, static_cast<float>(0x1.ccccccccccccbp-1 * Strength), static_cast<float>(Strength * 3.0), 0.0, 1.0E+2);
        return static_cast<float>(aShip::TShip_RelationToShip(this, Ship) + cpp_right) > 1.6E+2;
    }

    // Native diagnostic name: TWarrior.ArmsToTarget.
    void TWarrior::AssignWeaponTargetsInStar() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Assigned{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        float Distance{};
        aItem::TItem* Item{};
        aAsteroid::TAsteroid* Asteroid{};
        aMissile::TMissile* Missile{};
        std::uint8_t SameRacePlanet{};
        std::uint8_t IgnoreRanger{};
        std::int32_t Stage = 0;
        if (WarriorType == wtFlagship) {
            aWarrior::TWarrior_AssignFlagshipWeaponTargets(this);
        } else {
            try {
                {
                    const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last) {
                        for (I = 1; I <= cpp_last; ++I) {
                            Weapon = Weapons[I];
                            Weapon->Target = nullptr;
                        }
                    }
                }
                Assigned = 0;
                Stage = 1;
                if (CurrentStar->Status.Battle != 0) {
                    const std::int32_t cpp_last_2 = pas::list_count(CurrentStar->Ships) - 1;
                    if (0 <= cpp_last_2) {
                        for (I = 0; I <= cpp_last_2; ++I) {
                            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                            if (Ship->OwnerId == aGalaxyStruct::oiDominator && Ship->InNormalSpace()) {
                                Distance = aMyFunction::PointDistance(Position, Ship->Position);
                                {
                                    const std::int32_t cpp_last_3 = static_cast<std::int32_t>(WeaponCount);
                                    if (1 <= cpp_last_3) {
                                        for (J = 1; J <= cpp_last_3; ++J) {
                                            Weapon = Weapons[J];
                                            if ((!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                                static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                                static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) || Weapon->Ammo !=
                                                    0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(
                                                    this, Weapon)) {
                                                if (static_cast<double>(aShip::TShip_GetWeaponRange(this, Weapon)) >=
                                                    Distance) {
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
                    }
                }
                Stage = 2;
                if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace()) {
                    {
                        bool cpp_condition = pas::class_cast_if<aPlanet::TPlanet*>(EnemyShip->OrderTarget) != nullptr &&
                            static_cast<aPlanet::TPlanet*>(EnemyShip->OrderTarget)->RaceId == HomePlanet->RaceId;
                        if (cpp_condition) {
                            std::int32_t cpp_left = pas::checked_cast<aPlanet::TPlanet*>(
                                EnemyShip->OrderTarget)->RelationToShip(EnemyShip);
                            cpp_condition = cpp_left <= aShip::TShip_RelationToShip(this, EnemyShip);
                        }
                        SameRacePlanet = cpp_condition;
                    }
                    IgnoreRanger = EnemyShip->TypeId == aGalaxyStruct::stRanger && (EnemyShip->OrderTarget ==
                                HomePlanet || SameRacePlanet) && !EnemyShip->IsAttackingShip(this);
                    if (IgnoreRanger) {
                        const std::int32_t cpp_last_4 = static_cast<std::int32_t>(EnemyShip->WeaponCount);
                        if (1 <= cpp_last_4) {
                            for (I = 1; I <= cpp_last_4; ++I) {
                                if (EnemyShip->Weapons[I]->Target != nullptr &&
                                    pas::class_cast_if<aShip::TShip*>(EnemyShip->Weapons[I]->Target) != nullptr &&
                                    !(pas::class_cast_if<aPirate::TPirate*>(EnemyShip->Weapons[I]->Target) !=
                                        nullptr)) {
                                    if (aShip::TShip_GetRelationLevelToShip(this,
                                        pas::checked_cast<aShip::TShip*>(EnemyShip->Weapons[I]->Target)) >
                                        aGalaxyStruct::rlHostile) {
                                        IgnoreRanger = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (IgnoreRanger) {
                        ClearWeaponTargets(EnemyShip);
                    } else {
                        const std::int32_t cpp_last_5 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_5) {
                            for (J = 1; J <= cpp_last_5; ++J) {
                                Weapon = Weapons[J];
                                if ((!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                    static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                    static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) || Weapon->Ammo != 0) &&
                                    Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                    if (static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                                        EnemyShip->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
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
                }
                Stage = 3;
                {
                    const std::int32_t cpp_last_6 = pas::list_count(CurrentStar->Ships) - 1;
                    if (0 <= cpp_last_6) {
                        for (I = 0; I <= cpp_last_6; ++I) {
                            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                            Stage = 31;
                            if (Ship->InNormalSpace() && Ship != this && EnemyShip != Ship) {
                                Stage = 32;
                                if (aShip::TShip_RelationToShip(this, Ship) < aGalaxyStruct::RelationBadMin ||
                                    EnemyShip == Ship || Ship->EnemyShip == this) {
                                    Stage = 33;
                                    if (TruceShip != Ship) {
                                        Stage = 34;
                                        if (LiberationGroup == nullptr) {
                                            Stage = 35;
                                            {
                                                const std::int32_t cpp_last_7 = static_cast<std::int32_t>(WeaponCount);
                                                if (1 <= cpp_last_7) {
                                                    for (J = 1; J <= cpp_last_7; ++J) {
                                                        Stage = 36;
                                                        Weapon = Weapons[J];
                                                        if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                                            static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                                            static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ||
                                                            Weapon->Ammo != 0) {
                                                            Stage = 37;
                                                            if (aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                                                if (static_cast<double>(
                                                                    aMyFunction::PointDistanceSquared(Position,
                                                                        Ship->Position)) <=
                                                                    pas::sqr(aShip::TShip_GetWeaponRange(this,
                                                                        Weapon))) {
                                                                    Stage = 38;
                                                                    // Native hostile-ship pass can replace an earlier assignment.
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
                                    }
                                }
                            }
                        }
                    }
                }
                Stage = 4;
                {
                    const std::int32_t cpp_last_8 = pas::list_count(CurrentStar->Missiles) - 1;
                    if (0 <= cpp_last_8) {
                        for (I = 0; I <= cpp_last_8; ++I) {
                            Missile = pas::list_at<aMissile::TMissile>(CurrentStar->Missiles, I);
                            if (Missile->Target == this && Missile->OwnerShip != this) {
                                const std::int32_t cpp_last_9 = static_cast<std::int32_t>(WeaponCount);
                                if (1 <= cpp_last_9) {
                                    for (J = 1; J <= cpp_last_9; ++J) {
                                        Weapon = Weapons[J];
                                        if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                            static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                            static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Target ==
                                                nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                            if (static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                                                Missile->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this,
                                                    Weapon))) {
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
                    }
                }
                Stage = 5;
                if (aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
                    const std::int32_t cpp_last_10 = pas::list_count(CurrentStar->Asteroids) - 1;
                    if (0 <= cpp_last_10) {
                        for (I = 0; I <= cpp_last_10; ++I) {
                            Asteroid = pas::list_at<aAsteroid::TAsteroid>(CurrentStar->Asteroids, I);
                            Distance = aMyFunction::PointDistanceSquared(Position, Asteroid->Position);
                            if (Distance <=
                                pas::constant(static_cast<double>(aGalaxyStruct::AsteroidTargetRangeSquared))) {
                                const std::int32_t cpp_last_11 = static_cast<std::int32_t>(WeaponCount);
                                if (1 <= cpp_last_11) {
                                    for (J = 1; J <= cpp_last_11; ++J) {
                                        Weapon = Weapons[J];
                                        // Native asteroid targeting can overwrite an existing assignment.
                                        if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                            static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage),
                                            static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) &&
                                            aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                            if (static_cast<double>(pas::sqr(aShip::TShip_GetWeaponRange(this,
                                                Weapon))) >= Distance) {
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
                }
                Stage = 6;
                if (static_cast<float>(aGalaxy::Galaxy->GetAIJunkToleranceLevel() * 1.5) < pas::list_count(CurrentStar->Items)) {
                    const std::int32_t cpp_last_12 = pas::list_count(CurrentStar->Items) - 1;
                    if (0 <= cpp_last_12) {
                        for (I = 0; I <= cpp_last_12; ++I) {
                            Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                            if ((Item->ItemType == aConst::t_Minerals || Item->OwnerId == aGalaxyStruct::oiDominator) &&
                                (Item->ScriptItem == nullptr ||
                                    reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"")) {
                                if (aPlayer::GetPlayer()->CurrentStar != CurrentStar ||
                                    aShip::TShip_GetRelationLevelToShip(this, aPlayer::GetPlayer()) <=
                                        aGalaxyStruct::rlBad || aMyFunction::PointDistance(
                                        aPlayer::GetPlayer()->Position, Item->Position) >= 8.0E+2 ||
                                    aMyFunction::NextRandomUnitFloat(RandomState) <= 0.1 && aMyFunction::PointDistance(
                                            aPlayer::GetPlayer()->Position, Item->Position) >= 2.0E+2) {
                                    if (CanSafelyDetonateItem(Item)) {
                                        const std::int32_t cpp_last_13 = static_cast<std::int32_t>(WeaponCount);
                                        if (1 <= cpp_last_13) {
                                            for (J = 1; J <= cpp_last_13; ++J) {
                                                Weapon = Weapons[J];
                                                if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                                    static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                                    static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) &&
                                                    Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this,
                                                        Weapon)) {
                                                    if (static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                                                        Item->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this,
                                                            Weapon))) {
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
                }
                Stage = 7;
                if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == CurrentStar &&
                    aPlayer::GetPlayer()->InNormalSpace() && IsPlayerChameleonEffectiveAgainstSelf()) {
                    const std::int32_t cpp_last_14 = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last_14) {
                        for (J = 1; J <= cpp_last_14; ++J) {
                            Weapon = Weapons[J];
                            if ((!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) || Weapon->Ammo != 0) &&
                                Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                if (static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                                    aPlayer::GetPlayer()->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this,
                                        Weapon))) {
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
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                        static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                        u"Error in procedure TWarrior.ArmsToTarget ", GetFullName(u" "_wref.get()), u" label = ",
                        pas::wide_int_to_str(Stage)}))));
                } else {
                    throw;
                }
            }
        }
    }

    // Flagship branch; shares the TWarrior.ArmsToTarget diagnostic.
    void TWarrior_AssignFlagshipWeaponTargets(TWarrior* Self) {
        pas::Array<float, 1, 5> Scores{};
        std::int32_t Assigned{};
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        float Distance{};
        aItem::TItem* Item{};
        aAsteroid::TAsteroid* Asteroid{};
        aMissile::TMissile* Missile{};
        std::uint8_t SameRacePlanet{};
        std::uint8_t IgnoreRanger{};
        auto ScoreTarget = [&](aShip::TShip* Ship) -> void {
            std::int32_t J{};
            std::int32_t Range{};
            float Distance{};
            aItem::TWeapon* Weapon{};
            float Score = 0.0f;
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last) {
                    for (J = 1; J <= cpp_last; ++J) {
                        Weapon = Self->Weapons[J];
                        if (aShip::TShip_IsEquipmentUsable(Self, Weapon) && (!pas::in_range(
                                Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) || Weapon->Ammo > 0)) {
                            Range = aShip::TShip_GetWeaponRange(Self, Weapon);
                            Distance = aMyFunction::PointDistance(Self->Position, Ship->Position);
                            if (static_cast<double>(Range) >= Distance) {
                                if (Score == 0.0) {
                                    double cpp_left = Ship->CalculateAttackStrength();
                                    Score = static_cast<float>(static_cast<float>(cpp_left / Ship->CalculateDefenseStrength()) * aMyFunction::RemapClamped(
                                                Distance, 0.0, Range, 1.5, 1.0));
                                }
                                if (Score > Scores[J] || Weapon->Target == nullptr) {
                                    if (Weapon->Target == nullptr) {
                                        ++Assigned;
                                    }
                                    Scores[J] = Score;
                                    Weapon->Target = Ship;
                                }
                            }
                        }
                    }
                }
            }
        };
        std::int32_t Stage = 0;
        try {
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        Weapon = Self->Weapons[I];
                        Weapon->Target = nullptr;
                        Scores[I] = 0.0f;
                    }
                }
            }
            Assigned = 0;
            Stage = 1;
            if (Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar == Self->CurrentStar &&
                Self->EnemyShip->InNormalSpace()) {
                {
                    bool cpp_condition = pas::class_cast_if<aPlanet::TPlanet*>(Self->EnemyShip->OrderTarget) !=
                        nullptr && static_cast<aPlanet::TPlanet*>(Self->EnemyShip->OrderTarget)->RaceId ==
                            Self->HomePlanet->RaceId;
                    if (cpp_condition) {
                        std::int32_t cpp_left = pas::checked_cast<aPlanet::TPlanet*>(
                            Self->EnemyShip->OrderTarget)->RelationToShip(Self->EnemyShip);
                        cpp_condition = cpp_left <= aShip::TShip_RelationToShip(Self, Self->EnemyShip);
                    }
                    SameRacePlanet = cpp_condition;
                }
                IgnoreRanger = Self->EnemyShip->TypeId == aGalaxyStruct::stRanger && (Self->EnemyShip->OrderTarget ==
                            Self->HomePlanet || SameRacePlanet) && !Self->EnemyShip->IsAttackingShip(Self);
                if (!IgnoreRanger) {
                    ScoreTarget(Self->EnemyShip);
                }
            }
            Stage = 2;
            {
                const std::int32_t cpp_last_2 = pas::list_count(Self->CurrentStar->Ships) - 1;
                if (0 <= cpp_last_2) {
                    for (I = 0; I <= cpp_last_2; ++I) {
                        Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                        if (Ship->InNormalSpace() && Self->EnemyShip != Ship && Self->TruceShip != Ship &&
                            Self->LiberationGroup == nullptr && aShip::TShip_GetRelationLevelToShip(Self, Ship) <=
                                aGalaxyStruct::rlHostile) {
                            ScoreTarget(Ship);
                        }
                    }
                }
            }
            if (Assigned == Self->WeaponCount) {
                return;
            }
            Stage = 4;
            {
                const std::int32_t cpp_last_3 = pas::list_count(Self->CurrentStar->Missiles) - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        Missile = pas::list_at<aMissile::TMissile>(Self->CurrentStar->Missiles, I);
                        if (Missile->Target == Self && Missile->OwnerShip != Self) {
                            const std::int32_t cpp_last_4 = static_cast<std::int32_t>(Self->WeaponCount);
                            if (1 <= cpp_last_4) {
                                for (J = 1; J <= cpp_last_4; ++J) {
                                    Weapon = Self->Weapons[J];
                                    if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                        static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                        static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Target ==
                                            nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                                        if (static_cast<double>(aMyFunction::PointDistanceSquared(Self->Position,
                                            Missile->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(Self,
                                                Weapon))) {
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
                }
            }
            Stage = 5;
            if (aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar) {
                const std::int32_t cpp_last_5 = pas::list_count(Self->CurrentStar->Asteroids) - 1;
                if (0 <= cpp_last_5) {
                    for (I = 0; I <= cpp_last_5; ++I) {
                        Asteroid = pas::list_at<aAsteroid::TAsteroid>(Self->CurrentStar->Asteroids, I);
                        Distance = aMyFunction::PointDistanceSquared(Self->Position, Asteroid->Position);
                        if (Distance <= pas::constant(static_cast<double>(aGalaxyStruct::AsteroidTargetRangeSquared))) {
                            const std::int32_t cpp_last_6 = static_cast<std::int32_t>(Self->WeaponCount);
                            if (1 <= cpp_last_6) {
                                for (J = 1; J <= cpp_last_6; ++J) {
                                    Weapon = Self->Weapons[J];
                                    // Native asteroid targeting can overwrite an existing assignment.
                                    if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                        static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage),
                                        static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) &&
                                        aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                                        if (static_cast<double>(pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon))) >=
                                            Distance) {
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
            }
            Stage = 6;
            if (static_cast<float>(aGalaxy::Galaxy->GetAIJunkToleranceLevel() * 1.5) < pas::list_count(Self->CurrentStar->Items)) {
                const std::int32_t cpp_last_7 = pas::list_count(Self->CurrentStar->Items) - 1;
                if (0 <= cpp_last_7) {
                    for (I = 0; I <= cpp_last_7; ++I) {
                        Item = pas::list_at<aItem::TItem>(Self->CurrentStar->Items, I);
                        if ((Item->ItemType == aConst::t_Minerals || Item->OwnerId == aGalaxyStruct::oiDominator) &&
                            (Item->ScriptItem == nullptr ||
                                reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") &&
                            static_cast<std::uint8_t>(Self->AcceptPickupItem(Item) ^ 1)) {
                            if (aPlayer::GetPlayer()->CurrentStar != Self->CurrentStar ||
                                aShip::TShip_GetRelationLevelToShip(Self, aPlayer::GetPlayer()) <=
                                    aGalaxyStruct::rlBad || aMyFunction::PointDistance(aPlayer::GetPlayer()->Position,
                                    Item->Position) >= 8.0E+2 || aMyFunction::NextRandomUnitFloat(Self->RandomState) <=
                                    0.1 && aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >=
                                        2.0E+2) {
                                if (Self->CanSafelyDetonateItem(Item)) {
                                    const std::int32_t cpp_last_8 = static_cast<std::int32_t>(Self->WeaponCount);
                                    if (1 <= cpp_last_8) {
                                        for (J = 1; J <= cpp_last_8; ++J) {
                                            Weapon = Self->Weapons[J];
                                            if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                                static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                                static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) &&
                                                Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self,
                                                    Weapon)) {
                                                if (static_cast<double>(aMyFunction::PointDistanceSquared(
                                                    Self->Position, Item->Position)) <=
                                                    pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon))) {
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
            Stage = 7;
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar &&
                aPlayer::GetPlayer()->InNormalSpace() && Self->IsPlayerChameleonEffectiveAgainstSelf()) {
                const std::int32_t cpp_last_9 = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last_9) {
                    for (J = 1; J <= cpp_last_9; ++J) {
                        Weapon = Self->Weapons[J];
                        if ((!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                            static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                            static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) || Weapon->Ammo != 0) &&
                            Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                            if (static_cast<double>(aMyFunction::PointDistanceSquared(Self->Position,
                                aPlayer::GetPlayer()->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(Self,
                                    Weapon))) {
                                Weapon->Target = aPlayer::GetPlayer();
                                ++Assigned;
                                if (Assigned == Self->WeaponCount) {
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TWarrior.ArmsToTarget ", Self->GetFullName(u" "_wref.get()), u" label = ",
                    pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    aShip::TShip* TWarrior::FindNearestFriendlyFlagship() {
        double Distance{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        aShip::TShip* Result = nullptr;
        double BestDistance = 0.0;
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (Ship != this && pas::class_cast_if<TWarrior*>(Ship) != nullptr &&
                        static_cast<TWarrior*>(Ship)->WarriorType == wtFlagship && Ship->InNormalSpace()) {
                        if (aShip::TShip_GetRelationLevelToShip(Ship, this) > aGalaxyStruct::rlHostile) {
                            Distance = aMyFunction::PointDistanceSquared(Position, Ship->Position);
                            if (Result == nullptr || Distance < BestDistance) {
                                Result = Ship;
                                BestDistance = Distance;
                            }
                        }
                    }
                }
            }
        }
        return Result;
    }

    void TWarrior::SelectEnemyShipInStar() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        aShip::TShip* BestShip{};
        double Chance{};
        double BestChance{};
        double Distance{};
        double TravelTime{};
        double BestDistance{};
        std::uint8_t InFlagshipRange{};
        std::int32_t Range{};
        std::int32_t MinRange{};
        std::int32_t MaxRange{};
        double Score{};
        double BestScore{};
        double OwnAttack{};
        aItem::TWeapon* Weapon{};
        if (WarriorType == wtFlagship) {
            InFlagshipRange = true;
        } else {
            Ship = FindNearestFriendlyFlagship();
            InFlagshipRange = Ship != nullptr && static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                        Ship->Position)) < pas::sqr(Ship->GetRadarRange());
        }
        if (InFlagshipRange) {
            BestScore = -1.0;
            BestShip = nullptr;
            MinRange = -1;
            MaxRange = -1;
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        Weapon = Weapons[I];
                        if (aShip::TShip_IsEquipmentUsable(this, Weapon) && (!pas::in_range(
                                Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) || Weapon->Ammo > 0)) {
                            Range = aShip::TShip_GetWeaponRange(this, Weapon);
                            if (Range < MinRange || MinRange < 0) {
                                MinRange = Range;
                            }
                            if (Range > MaxRange || MaxRange < 0) {
                                MaxRange = Range;
                            }
                        }
                    }
                }
            }
            OwnAttack = CalculateAttackStrength();
            {
                const std::int32_t cpp_last_2 = pas::list_count(CurrentStar->Ships) - 1;
                if (0 <= cpp_last_2) {
                    for (I = 0; I <= cpp_last_2; ++I) {
                        Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                        if (Ship->InNormalSpace() && aShip::TShip_RelationToShip(this, Ship) <
                                aGalaxyStruct::RelationBadMin && TruceShip != Ship) {
                            {
                                double real_max = pas::real_max<double>(1.0,
                                    static_cast<float>(static_cast<double>(OwnAttack) / Ship->CalculateDefenseStrength()));
                                Score = static_cast<float>(Ship->CalculateAttackStrength() * real_max);
                            }
                            Distance = aMyFunction::PointDistance(Position, Ship->Position);
                            if (static_cast<double>(MaxRange) > Distance) {
                                Score = static_cast<float>(static_cast<double>(Score) * aMyFunction::RemapClamped(Distance, MinRange,
                                            MaxRange, 1.5, 1.0));
                            } else {
                                Score = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Score) * Distance) / (Speed + 1)) * aMyFunction::RemapClamped(
                                            static_cast<float>(static_cast<double>(Speed + 1) / (Ship->Speed + 1)), 0.5, 2.0, 0.25, 1.0));
                            }
                            if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship)) {
                                if (kling->KlingType == aGalaxyStruct::ktBertor) {
                                    Score = static_cast<float>(Score * 5.0);
                                }
                                if (kling->KlingType == aGalaxyStruct::ktKlig) {
                                    Score = static_cast<float>(Score * 0.25);
                                }
                            }
                            if (Score > BestScore) {
                                BestScore = Score;
                                BestShip = Ship;
                            }
                        }
                    }
                }
            }
            if (BestShip != nullptr) {
                EnemyShip = BestShip;
            }
        } else if (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar || EnemyShip->ConsecutiveDockedDays >
                3) {
            if (UsableWeaponCount != 0) {
                BestChance = -1.0;
                BestDistance = 1.0;
                {
                    const std::int32_t cpp_last_3 = pas::list_count(CurrentStar->Ships) - 1;
                    if (0 <= cpp_last_3) {
                        for (I = 0; I <= cpp_last_3; ++I) {
                            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                            if (Ship->InNormalSpace() && aShip::TShip_RelationToShip(this, Ship) <
                                    aGalaxyStruct::RelationBadMin && TruceShip != Ship) {
                                if (pas::is_one_of<aGalaxyStruct::ssDominator,
                                    aGalaxyStruct::ssPirateMilitary>(Ship->CurrentStanding)) {
                                    EnemyShip = Ship;
                                    if (aShip::TShip_ChanceToWin(this, Ship) > 1.0) {
                                        return;
                                    }
                                } else if (LiberationGroup == nullptr || aPlayer::GetPlayer() == Ship &&
                                        Ship->CurrentStanding == aGalaxyStruct::ssPirateActive) {
                                    Chance = aShip::TShip_ChanceToWin(this, Ship);
                                    TravelTime = static_cast<float>(static_cast<float>(static_cast<double>(aMyFunction::PointDistance(Position,
                                            Ship->Position)) / (Speed + 1)) + 0.1);
                                    if (static_cast<float>(static_cast<double>(Chance) / TravelTime) > static_cast<float>(static_cast<double>(BestChance) /
                                            BestDistance)) {
                                        EnemyShip = Ship;
                                        BestChance = Chance;
                                        BestDistance = TravelTime;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void TWarrior::EngageEnemyShip() {
        aShip::TShip* Ship{};
        std::uint8_t RetreatToFlagship{};
        if (Order == aShip::soFollowShip) {
            OrderNone(false);
        }
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar) {
            Ship = FindNearestFriendlyFlagship();
            RetreatToFlagship = Ship != nullptr && EnemyShip->InNormalSpace() &&
                    static_cast<std::uint8_t>(EnemyShip->HasNoUsableWeapons() ^ 1) && EnemyShip->EnemyShip == this &&
                    EnemyShip->OrderTarget == this && EnemyShip->Speed > Speed && aShip::TShip_GetRelationLevelToShip(
                        Ship, EnemyShip) <= aGalaxyStruct::rlHostile;
            if (EnemyShip->InNormalSpace()) {
                if (RetreatToFlagship) {
                    OrderFollowShip(Ship, aShip::fmFollowNear, false);
                } else {
                    OrderFollowShip(EnemyShip, aShip::fmMinWeaponRange, false);
                }
                if (aShip::TShip_ChanceToWin(this, EnemyShip) < 0.8) {
                    aShip::TShip_RequestAlliesAttackShip(this, EnemyShip);
                }
            } else if (aShip::TShip_ChanceToWin(this, EnemyShip) > 0.5) {
                if (EnemyShip->CurrentPlanet != nullptr) {
                    if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(EnemyShip); ranger !=
                        nullptr && static_cast<std::uint32_t>(ranger->PrisonTermRemaining) > 0) {
                        EnemyShip = nullptr;
                        OrderNone(false);
                        return;
                    }
                    if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(EnemyShip); pirate !=
                        nullptr && pirate->PrisonTermRemaining > 0) {
                        EnemyShip = nullptr;
                        OrderNone(false);
                        return;
                    }
                    OrderMove(EnemyShip->CurrentPlanet->GetPosition(), false);
                } else if (EnemyShip->DockedTo != nullptr) {
                    OrderMove(EnemyShip->DockedTo->Position, false);
                }
            }
        }
    }

    void TWarrior::ManeuverFlagship() {
        pas::List* Enemies{};
        float SupportWeight{};
        std::int32_t OwnMinRange{};
        std::int32_t OwnMaxRange{};
        float OwnAttack{};
        pas::List* Allies{};
        std::int32_t EnemiesInRange{};
        float AttackWeight{};
        float OwnDefense{};
        float FearWeight{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        EC_Struct::TPointF Candidate{};
        EC_Struct::TPointF Destination{};
        float Score{};
        auto ProjectOutsideStar = [&](EC_Struct::TPointF Point) -> EC_Struct::TPointF {
            EC_Struct::TPointF Result{};
            float SquaredDistance = static_cast<float>(pas::sqr(static_cast<double>(Point.X)) + pas::sqr(static_cast<double>(Point.Y)));
            float Radius = this->CurrentStar->SafeRadius;
            std::int32_t Attempts = 0;
            if (SquaredDistance < 0.1) {
                while (true) {
                    Result.X = aMyFunction::NextRandomIntRange(pas::real_round(static_cast<float>(-0x1.4ccccccccccccp+0 * Radius)),
                            pas::real_round(static_cast<float>(0x1.4ccccccccccccp+0 * Radius)), this->RandomState);
                    Result.Y = aMyFunction::NextRandomIntRange(pas::real_round(static_cast<float>(-0x1.4ccccccccccccp+0 * Radius)),
                            pas::real_round(static_cast<float>(0x1.4ccccccccccccp+0 * Radius)), this->RandomState);
                    if (static_cast<float>(pas::sqr(static_cast<double>(Result.X)) + pas::sqr(static_cast<double>(Result.Y))) >
                        pas::sqr(static_cast<double>(Radius))) {
                        break;
                    }
                    ++Attempts;
                    if (Attempts > 100) {
                        return Result;
                    }
                }
            } else if (pas::sqr(static_cast<double>(Radius)) >= SquaredDistance) {
                SquaredDistance = static_cast<float>((static_cast<float>(Radius + 1.0)) / System::Sqrt(SquaredDistance));
                Result.X = static_cast<float>(static_cast<double>(Point.X) * SquaredDistance);
                Result.Y = static_cast<float>(static_cast<double>(Point.Y) * SquaredDistance);
            } else {
                Result = Point;
            }
            return Result;
        };
        auto EvaluatePosition = [&](EC_Struct::TPointF Point) -> float {
            aShip::TShip* Ship{};
            float Benefit{};
            float Risk{};
            float Threat{};
            float Distance{};
            float HookRange{};
            std::int32_t Minimum{};
            std::int32_t Maximum{};
            float ShipDistance{};
            float Attack{};
            std::int32_t I{};
            aItem::TItem* Item{};
            float AttackPotential = 0.0f;
            float SupportPotential = 0.0f;
            float IncomingStrength = 0.0f;
            float NodeValue = 0.0f;
            float BestEnemyPotential = 0.0f;
            {
                const std::int32_t cpp_last = pas::list_count(Enemies) - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Ship = pas::list_at<aShip::TShip>(Enemies, I);
                        aWarrior::GetWeaponRangeBounds(Ship, Minimum, Maximum, this);
                        ShipDistance = aMyFunction::PointDistance(Point, Ship->Position);
                        {
                            double cpp_right_2 = static_cast<float>(0.1 *
                                aMyFunction::RemapClamped(SupportWeight, 0.0, 2.0, 0.1, 1.0));
                            double cpp_right = static_cast<float>(1.0 + static_cast<float>(aWarrior::AreaWeaponScore(Ship, this) * cpp_right_2));
                            Attack = static_cast<float>(Ship->CalculateAttackStrength() * cpp_right);
                        }
                        Threat = static_cast<float>(static_cast<double>(Attack) * aMyFunction::RemapClamped(ShipDistance, Minimum, Maximum,
                                    1.0, 0.0));
                        if (static_cast<float>(Attack * 0x1.fae147ae147b0p-1) > Threat) {
                            Threat = static_cast<float>(Threat + static_cast<float>(static_cast<float>((static_cast<float>(static_cast<double>(Attack) - Threat)) * Ship->Speed) /
                                        (static_cast<float>(static_cast<float>(static_cast<double>(Ship->Speed) + ShipDistance) - Minimum))));
                        }
                        if (Ship->OrderTarget == this) {
                            Threat = static_cast<float>(static_cast<double>(Threat) * aMyFunction::RemapClamped(
                                        static_cast<float>(static_cast<double>(this->Speed) / std::max<std::int32_t>(1, Ship->Speed)), 0.5,
                                        1.0, 0.0, 1.0));
                        }
                        IncomingStrength = static_cast<float>(static_cast<double>(IncomingStrength) + Threat);
                        Threat = static_cast<float>(static_cast<double>(Attack) * aMyFunction::RemapClamped(ShipDistance, OwnMinRange,
                                    OwnMaxRange, 1.0, 0.0));
                        if (static_cast<float>(Attack * 0x1.fae147ae147b0p-1) > Threat) {
                            Threat = static_cast<float>(Threat + static_cast<float>(static_cast<float>((static_cast<float>(static_cast<double>(Attack) - Threat)) * this->Speed) /
                                        (static_cast<float>(static_cast<float>(static_cast<double>(this->Speed) + ShipDistance) - OwnMinRange))));
                        }
                        {
                            double real_max_scalar = pas::real_max_scalar<double>(static_cast<double>(OwnAttack),
                                Ship->CalculateDefenseStrength());
                            Threat = static_cast<float>(Threat / (static_cast<float>(real_max_scalar * (static_cast<float>(1.0 + static_cast<float>(Ship->GetRepairStrengthFactor() * (static_cast<float>(0.1 *
                                        aMyFunction::RemapClamped(SupportWeight, 0.0, 2.0, 1.0, 0.1)))))))));
                        }
                        if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship); kling != nullptr &&
                            kling->KlingType == aGalaxyStruct::ktBertor) {
                            Threat = static_cast<float>(1.5 * Threat);
                        }
                        if (Threat > BestEnemyPotential) {
                            BestEnemyPotential = Threat;
                        }
                        AttackPotential = static_cast<float>(AttackPotential + static_cast<float>(0x1.999999999999bp-4 * Threat));
                    }
                }
            }
            AttackPotential = static_cast<float>(static_cast<double>(AttackPotential) + BestEnemyPotential);
            {
                const std::int32_t cpp_last_2 = pas::list_count(Allies) - 1;
                if (0 <= cpp_last_2) {
                    for (I = 0; I <= cpp_last_2; ++I) {
                        Ship = pas::list_at<aShip::TShip>(Allies, I);
                        aWarrior::GetWeaponRangeBounds(Ship, Minimum, Maximum, this);
                        ShipDistance = aMyFunction::PointDistance(Point, Ship->Position);
                        Attack = Ship->CalculateAttackStrength();
                        SupportPotential = static_cast<float>(SupportPotential + static_cast<float>(static_cast<float>(static_cast<double>(Attack) * aMyFunction::RemapClamped(
                                    ShipDistance, OwnMinRange, OwnMaxRange, 1.0, 0.0)) / pas::real_max_scalar<double>(
                                    static_cast<double>(OwnAttack), Ship->CalculateDefenseStrength())));
                        Threat = static_cast<float>(static_cast<double>(Attack) * aMyFunction::RemapClamped(ShipDistance, Minimum, Maximum,
                                    1.0, 0.0));
                        Attack = static_cast<float>(static_cast<double>(Attack) - Threat);
                        if (Attack > static_cast<float>(Threat * 0x1.47ae147ae147ap-7)) {
                            Threat = static_cast<float>(Threat + static_cast<float>(static_cast<float>(static_cast<double>(Attack) * Ship->Speed) /
                                        (static_cast<float>(static_cast<float>(static_cast<double>(Ship->Speed) + ShipDistance) - Minimum))));
                        }
                        IncomingStrength = static_cast<float>(IncomingStrength - static_cast<float>(0.5 * Threat));
                    }
                }
            }
            if (GetCargoHook() != nullptr) {
                const std::int32_t cpp_last_3 = pas::list_count(this->CurrentStar->Items) - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        Item = pas::list_at<aItem::TItem>(this->CurrentStar->Items, I);
                        if (aItem::TProtoplasm* protoplasm = pas::class_cast_if<aItem::TProtoplasm*>(
                            Item); protoplasm != nullptr && aItem::CanCargoHookHandleItem(Item, this) &&
                            !aShip::TShip_IsItemInPickupRange(this, Item)) {
                            Distance = System::Sqrt(static_cast<float>(pas::sqr(static_cast<float>(static_cast<double>(Point.X) - Item->Position.X)) +
                                    pas::sqr(static_cast<float>(static_cast<double>(Point.Y) - Item->Position.Y))));
                            HookRange = GetCargoHookRange();
                            if (Distance <= HookRange) {
                                NodeValue = static_cast<float>(static_cast<double>(NodeValue) + std::min<std::int32_t>(
                                            protoplasm->StackCount, this->CargoFreeSpace));
                            } else {
                                std::int32_t min_2 = std::min<std::int32_t>(protoplasm->StackCount,
                                    this->CargoFreeSpace);
                                {
                                    double cargoHookRange = GetCargoHookRange();
                                    double cpp_arg = 2 * GetCargoHookRange();
                                    NodeValue = static_cast<float>(NodeValue + static_cast<float>(static_cast<double>(min_2) * aMyFunction::RemapClamped(
                                                Distance, cargoHookRange, cpp_arg, 0.5, 0.0)));
                                }
                            }
                        }
                    }
                }
            }
            if (this->PilotRace != aGalaxyStruct::oiFeyan) {
                NodeValue = static_cast<float>(NodeValue * 2.0);
            }
            if (EnemiesInRange > 0) {
                Benefit = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(AttackPotential) * AttackWeight) + static_cast<float>(0x1.999999999999bp-5 * NodeValue)) +
                        static_cast<float>(static_cast<double>(SupportPotential) * SupportWeight));
                Risk = static_cast<float>(static_cast<float>(static_cast<double>(IncomingStrength) / OwnDefense) * FearWeight);
            } else {
                Benefit = static_cast<float>(static_cast<float>(static_cast<double>(AttackPotential) * AttackWeight) + static_cast<float>(0x1.999999999999bp-5 * NodeValue));
                Risk = 0.0f;
            }
            Distance = System::Sqrt(static_cast<float>(pas::sqr(static_cast<double>(Point.X)) + pas::sqr(static_cast<double>(Point.Y))));
            Benefit = static_cast<float>(static_cast<double>(Benefit) * aMyFunction::RemapClamped(Distance,
                        static_cast<float>(this->CurrentStar->MapDiameter * 0x1.6666666666665p-1), static_cast<float>(this->CurrentStar->MapDiameter * 0x1.3333333333334p+0), 1.0, 0.5));
            if (Distance > static_cast<float>(this->CurrentStar->MapDiameter * 0x1.3333333333334p+0)) {
                Benefit = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(this->CurrentStar->MapDiameter) * Benefit) * 0x1.3333333333334p+0) / Distance);
            }
            return static_cast<float>(static_cast<double>(Benefit) - Risk);
        };
        auto BoostWithNodes = [&]() -> void {
            float Factor{};
            if (pas::is_one_of<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiHuman,
                aGalaxyStruct::oiGaal>(this->PilotRace) && EnemiesInRange > 0) {
                Factor = static_cast<float>(static_cast<float>(aMyFunction::RemapClamped(this->CargoFreeSpace, 0.0, GetHull()->Weight / 4, 0.5, 1.0) * (static_cast<float>(3.0 /
                            (EnemiesInRange + 2)))) * aMyFunction::RemapClamped(GetHull()->HullPoints, 0.0,
                            GetHull()->Weight / 2, 0.1, 1.0));
                while (static_cast<float>(5.0E+1 - static_cast<float>(9.0E+1 * Factor)) > GetCombatStatusStrength(aShip::cseBWBuff) && GetCarriedNodeCount() >=
                        static_cast<std::int64_t>(50)) {
                    ConsumeNodes(50);
                    AddCombatStatusStrength(aShip::cseBWBuff, 2.0E+1f, nullptr);
                }
                RefreshDerivedStats(true);
            }
        };
        if (HasNoUsableWeapons()) {
            EngageEnemyShip();
            return;
        }
        Enemies = pas::make_object<pas::List>();
        Allies = pas::make_object<pas::List>();
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (Ship != this && Ship->InNormalSpace()) {
                        if (aShip::TShip_GetRelationLevelToShip(this, Ship) <= aGalaxyStruct::rlHostile) {
                            pas::list_add(Enemies, reinterpret_cast<void*>(Ship));
                        } else if (aShip::TShip_GetRelationLevelToShip(this, Ship) > aGalaxyStruct::rlNormal &&
                            (pas::class_cast_if<TWarrior*>(Ship) != nullptr || Ship->EnemyShip != nullptr &&
                                    aShip::TShip_GetRelationLevelToShip(this, Ship->EnemyShip) <=
                                    aGalaxyStruct::rlHostile)) {
                            pas::list_add(Allies, reinterpret_cast<void*>(Ship));
                        }
                    }
                }
            }
        }
        aWarrior::GetWeaponRangeBounds(this, OwnMinRange, OwnMaxRange, this);
        OwnAttack = CalculateAttackStrength();
        OwnDefense = CalculateDefenseStrength();
        if (pas::list_count(Enemies) <= 2 || pas::list_count(Allies) <= 0 || OwnMaxRange < 300) {
            pas::free(Enemies);
            pas::free(Allies);
            EngageEnemyShip();
            return;
        }
        std::int32_t EnemyAreaScore = 0;
        std::int32_t AllyAreaScore = aWarrior::AreaWeaponScore(this, this);
        float EnemyRepair = 0.0f;
        float AllyRepair = GetRepairStrengthFactor();
        EnemiesInRange = 0;
        {
            const std::int32_t cpp_last_2 = pas::list_count(Enemies) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Ship = pas::list_at<aShip::TShip>(Enemies, I);
                    EnemyAreaScore += aWarrior::AreaWeaponScore(Ship, this);
                    EnemyRepair = static_cast<float>(static_cast<double>(EnemyRepair) + Ship->GetRepairStrengthFactor());
                    if (static_cast<double>(aMyFunction::PointDistanceSquared(Position, Ship->Position)) <
                        pas::sqr(OwnMaxRange)) {
                        ++EnemiesInRange;
                    }
                }
            }
        }
        {
            const std::int32_t cpp_last_3 = pas::list_count(Allies) - 1;
            if (0 <= cpp_last_3) {
                for (I = 0; I <= cpp_last_3; ++I) {
                    Ship = pas::list_at<aShip::TShip>(Allies, I);
                    AllyAreaScore += aWarrior::AreaWeaponScore(Ship, this);
                    AllyRepair = static_cast<float>(static_cast<double>(AllyRepair) + Ship->GetRepairStrengthFactor());
                }
            }
        }
        BoostWithNodes();
        AttackWeight = 1.0f;
        SupportWeight = aMyFunction::RemapClamped(
                static_cast<float>(static_cast<float>(static_cast<double>(AllyAreaScore) * EnemyRepair) - static_cast<float>(static_cast<double>(EnemyAreaScore) * AllyRepair)),
                -1.0E+2, 1.0E+2, -2.0, 2.0);
        FearWeight = aMyFunction::RemapClamped(GetHull()->HullPoints, 0.0, GetHull()->Weight, 2.0, 0.75);
        aShip::TShip* BestShip = nullptr;
        float BestShipScore = 0.0f;
        {
            const std::int32_t cpp_last_4 = pas::list_count(Allies) - 1;
            if (0 <= cpp_last_4) {
                for (I = 0; I <= cpp_last_4; ++I) {
                    Ship = pas::list_at<aShip::TShip>(Allies, I);
                    {
                        double cpp_right = pas::real_trunc(static_cast<float>(static_cast<double>(aMyFunction::PointDistance(
                            Ship->Position, Position)) / (Speed + 1))) + 1;
                        Score = static_cast<float>(EvaluatePosition(Ship->Position) / cpp_right);
                    }
                    if (Score > BestShipScore || BestShip == nullptr) {
                        BestShipScore = Score;
                        BestShip = Ship;
                    }
                }
            }
        }
        {
            const std::int32_t cpp_last_5 = pas::list_count(Enemies) - 1;
            if (0 <= cpp_last_5) {
                for (I = 0; I <= cpp_last_5; ++I) {
                    Ship = pas::list_at<aShip::TShip>(Enemies, I);
                    {
                        double cpp_right_2 = pas::real_trunc(static_cast<float>(static_cast<double>(aMyFunction::PointDistance(
                            Ship->Position, Position)) / (Speed + 1))) + 1;
                        Score = static_cast<float>(EvaluatePosition(Ship->Position) / cpp_right_2);
                    }
                    if (Score > BestShipScore || BestShip == nullptr) {
                        BestShipScore = Score;
                        BestShip = Ship;
                    }
                }
            }
        }
        std::int32_t RandomDistance = aMyFunction::NextRandomIntRange(Speed / 3, Speed, RandomState);
        std::int32_t Angle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
        Destination.X = static_cast<float>(static_cast<float>(System::Cos(aMyFunction::HeadingDegreesToRadians(Angle)) * RandomDistance) + Position.X);
        Destination.Y = static_cast<float>(static_cast<float>(System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * RandomDistance) + Position.Y);
        Destination = ProjectOutsideStar(Destination);
        float BestPositionScore = EvaluatePosition(Destination);
        for (I = 1; I <= 40; ++I) {
            RandomDistance = aMyFunction::NextRandomIntRange(Speed / 3, Speed, RandomState);
            Angle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
            Candidate.X = static_cast<float>(static_cast<float>(System::Cos(aMyFunction::HeadingDegreesToRadians(Angle)) * RandomDistance) + Position.X);
            Candidate.Y = static_cast<float>(static_cast<float>(System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * RandomDistance) + Position.Y);
            Candidate = ProjectOutsideStar(Candidate);
            {
                double cpp_right_3 = pas::real_trunc(static_cast<float>(static_cast<double>(aMyFunction::PointDistance(Candidate,
                    Position)) / (Speed + 1))) + 1;
                Score = static_cast<float>(EvaluatePosition(Candidate) / cpp_right_3);
            }
            if (Score > BestPositionScore) {
                BestPositionScore = Score;
                Destination = Candidate;
            }
        }
        pas::free(Enemies);
        pas::free(Allies);
        if (BestShip != nullptr && BestShipScore >= BestPositionScore) {
            if (aShip::TShip_GetRelationLevelToShip(this, BestShip) > aGalaxyStruct::rlHostile) {
                OrderFollowShip(BestShip, aShip::fmFollowNear, false);
            } else {
                OrderFollowShip(BestShip, aShip::fmMinWeaponRange, false);
            }
        } else {
            OrderMove(Destination, false);
        }
    }

    static void GetWeaponRangeBounds(aShip::TShip* Ship, std::int32_t& Minimum, std::int32_t& Maximum, TWarrior* Self) {
        std::int32_t I{};
        std::int32_t Range{};
        aItem::TWeapon* Weapon{};
        Minimum = -1;
        Maximum = -1;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Ship->Weapons[I];
                    // Native uses the flagship's usability check for other ships' weapons.
                    if (aShip::TShip_IsEquipmentUsable(Self, Weapon) && (!pas::in_range(
                            Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                            static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) || Weapon->Ammo > 0)) {
                        Range = aShip::TShip_GetWeaponRange(Ship, Weapon);
                        if (Minimum > Range || Minimum < 0) {
                            Minimum = Range;
                        }
                        if (Maximum < Range || Maximum < 0) {
                            Maximum = Range;
                        }
                    }
                }
            }
        }
    }

    static std::int32_t AreaWeaponScore(aShip::TShip* Ship, TWarrior* Self) {
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        std::int32_t Result = 0;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Ship->Weapons[I];
                    if (aShip::TShip_IsEquipmentUsable(Self, Weapon) && (!pas::in_range(
                            Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                            static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) || Weapon->Ammo > 0)) {
                        switch (Weapon->GetWeaponInfo()->ShotType) {
                            case aGalaxyStruct::wstChain: ++Result; break;
                            case aGalaxyStruct::wstSplash:
                            case aGalaxyStruct::wstTorpedo:
                            case aGalaxyStruct::wstMissile: Result += 2; break;
                            case aGalaxyStruct::wstAreaDamage: Result += 5; break;
                        }
                    }
                }
            }
        }
        if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship)) {
            switch (kling->KlingType) {
                case aGalaxyStruct::ktBertor: Result += 12; return Result;
                case aGalaxyStruct::ktKlig: Result += 3; return Result;
                default: return Result;
            }
        }
        return Result;
    }

    // Scores Coalition systems and same-race garrisons; moves the flagship between home rosters without changing its current position.
    void TWarrior::ReassignFlagshipHomePlanet() {
        float Score{};
        aPlanet::TPlanet* Planet{};
        aPlanet::TPlanet* BestPlanet{};
        aGalaxy::TStar* Star{};
        aGalaxy::TStar* NearbyStar{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        float FlagshipFactor{};
        float GarrisonStrength{};
        float NearbyThreat{};
        aShip::TShip* Ship{};
        TWarrior* Warrior{};
        std::int32_t RacePlanetCount{};
        std::int32_t WarriorCount{};
        std::int32_t StationCount{};
        float BestScore = 0.0f;
        aGalaxy::TStar* BestStar = nullptr;
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction ==
                            u"" && Star->ConstellationGraphIndex != 20) {
                        FlagshipFactor = 1.0f;
                        GarrisonStrength = 0.0f;
                        RacePlanetCount = 0;
                        StationCount = 0;
                        {
                            const std::int32_t cpp_last_2 = pas::list_count(Star->Ships) - 1;
                            if (0 <= cpp_last_2) {
                                for (J = 0; J <= cpp_last_2; ++J) {
                                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                                    if (pas::in_range(Ship->TypeId,
                                        static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter),
                                        static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && pas::contains(
                                            aConst::NonTargetableStationStandingMasks[aGalaxyStruct::sfCoalition],
                                            Ship->CurrentStanding)) {
                                        ++StationCount;
                                    }
                                }
                            }
                        }
                        {
                            const std::int32_t cpp_last_3 = pas::list_count(Star->Planets) - 1;
                            if (0 <= cpp_last_3) {
                                for (J = 0; J <= cpp_last_3; ++J) {
                                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                                    if (pas::contains(aConst::PlanetOwnerMasks.Coalition, Planet->OwnerId)) {
                                        if (Planet->RaceId == PilotRace) {
                                            ++RacePlanetCount;
                                        }
                                        {
                                            const std::int32_t cpp_last_4 = pas::list_count(Planet->Warriors) - 1;
                                            if (0 <= cpp_last_4) {
                                                for (K = 0; K <= cpp_last_4; ++K) {
                                                    Warrior = pas::list_at<TWarrior>(Planet->Warriors, K);
                                                    if (Warrior == this) {
                                                        continue;
                                                    }
                                                    GarrisonStrength = static_cast<float>(static_cast<double>(GarrisonStrength) +
                                                            aMyFunction::RemapClamped(Warrior->Strength, static_cast<float>(0x1.999999999999bp-4 * Strength),
                                                                static_cast<float>(1.0E+1 * Strength), 0.3, 3.0));
                                                    if (Warrior->WarriorType == wtFlagship) {
                                                        if (Warrior->PilotRace == PilotRace) {
                                                            FlagshipFactor = static_cast<float>(FlagshipFactor * 0x1.999999999999bp-5);
                                                        } else {
                                                            FlagshipFactor = static_cast<float>(FlagshipFactor * 0x1.999999999999bp-3);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (GarrisonStrength != 0.0 && RacePlanetCount != 0) {
                            NearbyThreat = 0.0f;
                            for (auto cpp_range = pas::for_to<std::int32_t>(1,
                                pas::list_count(aGalaxy::Galaxy->Stars) / 5); cpp_range.next(J); ) {
                                NearbyStar =
                                    pas::checked_cast<aGalaxy::TStar*>(
                                        static_cast<pas::Object*>(Star->StarDistances[J].Star));
                                if (NearbyStar->ConstellationGraphIndex != 20) {
                                    if (NearbyStar->Status.CustomFaction != u"") {
                                        NearbyThreat = static_cast<float>(NearbyThreat + static_cast<float>(5.0 / (J + 5)));
                                    } else {
                                        switch (NearbyStar->Status.ControlFaction) {
                                            case aGalaxyStruct::sfCoalition: break;
                                            case aGalaxyStruct::sfPirates: {
                                                NearbyThreat = static_cast<float>(NearbyThreat + static_cast<float>(1.0 / (J + 5)));
                                                break;
                                            }
                                            case aGalaxyStruct::sfDominators: {
                                                NearbyThreat = static_cast<float>(NearbyThreat + static_cast<float>(5.0 / (J + 5)));
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            Score = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(RacePlanetCount) * GarrisonStrength) * NearbyThreat) *
                                    FlagshipFactor) * (StationCount + 3));
                            if (HomePlanet->CurrentStar == Star) {
                                Score = static_cast<float>(Score * 0x1.4ccccccccccccp+0);
                            }
                            if (Score > BestScore || BestStar == nullptr) {
                                BestScore = Score;
                                BestStar = Star;
                            }
                        }
                    }
                }
            }
        }
        if (BestStar != nullptr) {
            BestScore = 0.0f;
            BestPlanet = nullptr;
            {
                const std::int32_t cpp_last_5 = pas::list_count(BestStar->Planets) - 1;
                if (0 <= cpp_last_5) {
                    for (I = 0; I <= cpp_last_5; ++I) {
                        Planet = pas::list_at<aPlanet::TPlanet>(BestStar->Planets, I);
                        if (pas::contains(aConst::PlanetOwnerMasks.Coalition, Planet->OwnerId) && Planet->RaceId ==
                                PilotRace) {
                            WarriorCount = 0;
                            FlagshipFactor = 1.0f;
                            {
                                const std::int32_t cpp_last_6 = pas::list_count(Planet->Warriors) - 1;
                                if (0 <= cpp_last_6) {
                                    for (K = 0; K <= cpp_last_6; ++K) {
                                        Warrior = pas::list_at<TWarrior>(Planet->Warriors, K);
                                        if (Warrior != this) {
                                            ++WarriorCount;
                                            if (Warrior->WarriorType == wtFlagship) {
                                                FlagshipFactor = static_cast<float>(FlagshipFactor * 0x1.999999999999bp-4);
                                            }
                                        }
                                    }
                                }
                            }
                            Score = static_cast<float>(static_cast<double>(WarriorCount) * FlagshipFactor);
                            if (HomePlanet == Planet) {
                                Score = static_cast<float>(Score * 0x1.4ccccccccccccp+0);
                            }
                            if (Score > BestScore || BestPlanet == nullptr) {
                                BestScore = Score;
                                BestPlanet = Planet;
                            }
                        }
                    }
                }
            }
            if (BestPlanet != nullptr && BestPlanet != HomePlanet) {
                I = pas::list_indexof(HomePlanet->Warriors, reinterpret_cast<void*>(this));
                if (I >= 0) {
                    pas::list_delete(HomePlanet->Warriors, I);
                }
                I = pas::list_indexof(BestPlanet->Warriors, reinterpret_cast<void*>(this));
                if (I < 0) {
                    pas::list_add(BestPlanet->Warriors, reinterpret_cast<void*>(this));
                }
                HomePlanet = BestPlanet;
            }
        }
    }

    void TWarrior::ProcessCombatDialogue() {
    }

    void TWarrior::ReactToExtortionDemand(void* Ranger) {
        if (aPlayer::GetPlayer() == Ranger || aMyFunction::NextRandomUnitFloat(RandomState) < 0.05) {
            HomePlanet->ChangeRelationToRanger(Ranger, -5);
            pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))->AddPirateCareerActivity(4);
        }
    }

    std::uint8_t TWarrior_BuildMoneyExtortionResponse(TWarrior* Self, aShip::TShip* OtherShip,
        pas::WideString& Response, std::int32_t DemandedAmount) {
        std::uint8_t Result = false;
        if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr) {
            Self->ReactToExtortionDemand(OtherShip);
        }
        if (aPlayer::GetPlayer() != OtherShip && (Self->EnemyShip == nullptr || Self->EnemyShip->CurrentStar !=
                    Self->CurrentStar)) {
            Self->EnemyShip = OtherShip;
        }
        Response = aShip::TShip_LookupVisibleTalkText(Self,
                pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"No"}), OtherShip);
        return Result;
    }

    std::uint8_t TWarrior_BuildCargoExtortionResponse(TWarrior* Self, aShip::TShip* OtherShip,
        pas::WideString& Response) {
        std::uint8_t Result = false;
        if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr) {
            Self->ReactToExtortionDemand(OtherShip);
        }
        if (aPlayer::GetPlayer() != OtherShip && (Self->EnemyShip == nullptr || Self->EnemyShip->CurrentStar !=
                    Self->CurrentStar)) {
            Self->EnemyShip = OtherShip;
        }
        Response = aShip::TShip_LookupVisibleTalkText(Self,
                pas::concat_wide({u"Talk.Goods.", Self->GetTypeNameKey(), u"No"}), OtherShip);
        return Result;
    }

    std::uint8_t TWarrior::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t OfferedAmount) {
        // Nested helper with caller-popped static link.
        auto AcceptPayment = [&]() -> void {
            std::int32_t I{};
            std::int32_t J{};
            aShip::TShip* Ship{};
            aPlanet::TPlanet* Planet{};
            aItem::TWeapon* Weapon{};
            OtherShip->SetMoney(OtherShip->Money - OfferedAmount);
            SetMoney(this->Money + OfferedAmount);
            aShip::TShip_TruceWithShip(this, OtherShip);
            if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(OtherShip)) {
                normalShip->CurrentSystemKills.Normal = 0;
            }
            if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(OtherShip)) {
                ranger->AddTraderCareerActivity(1);
                Planet = this->HomePlanet;
                {
                    const std::int32_t cpp_last = pas::list_count(this->CurrentStar->Ships) - 1;
                    if (0 <= cpp_last) {
                        for (I = 0; I <= cpp_last; ++I) {
                            Ship = pas::list_at<aShip::TShip>(this->CurrentStar->Ships, I);
                            if (TWarrior* warrior = pas::class_cast_if<TWarrior*>(Ship); warrior != nullptr &&
                                warrior->HomePlanet == Planet && this != Ship) {
                                if (OtherShip == Ship->EnemyShip) {
                                    Ship->EnemyShip = nullptr;
                                }
                                {
                                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Ship->WeaponCount);
                                    if (1 <= cpp_last_2) {
                                        for (J = 1; J <= cpp_last_2; ++J) {
                                            Weapon = Ship->Weapons[J];
                                            if (OtherShip == Weapon->Target) {
                                                Weapon->Target = nullptr;
                                            }
                                        }
                                    }
                                }
                                // Native repeats this check after clearing weapon targets.
                                if (OtherShip == Ship->EnemyShip) {
                                    Ship->EnemyShip = nullptr;
                                }
                                if (OtherShip->EnemyShip == Ship) {
                                    OtherShip->EnemyShip = nullptr;
                                }
                                Ship->ChangeRelationToRanger(OtherShip, 30);
                                if (Ship->Order == aShip::soFollowShip &&
                                    pas::checked_cast<aShip::TShip*>(Ship->OrderTarget) == OtherShip) {
                                    Ship->OrderNone(false);
                                    Ship->virtual_TShip_NextDay();
                                }
                            }
                        }
                    }
                }
            }
        };
        std::uint8_t Result = false;
        std::int32_t NextDemandTurn = LastPlayerExtortionTurn + 30;
        if (RefusesFactionNegotiation(OtherShip)) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Refuse.Warrior"_wref.get(), OtherShip);
            return false;
        } else if (OtherShip->TruceShip == this) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && PlayerExtortionPactActive) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(
            OtherShip); normalShip != nullptr && OtherShip->OwnerId == aGalaxyStruct::oiPirate &&
            OtherShip->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition &&
            normalShip->CurrentSystemKills.Normal > 0) {
            if (static_cast<float>(2 * Wealth * 0x1.1111111111112p-4) < OfferedAmount) {
                Response = aShip::TShip_LookupVisibleTalkText(this,
                        pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Ok"}), OtherShip);
                AcceptPayment();
                return true;
            }
            Response = aShip::TShip_LookupVisibleTalkText(this,
                    pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        } else if (static_cast<float>(Wealth * 0x1.1111111111112p-4) < OfferedAmount) {
            Response = aShip::TShip_LookupVisibleTalkText(this,
                    pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Ok"}), OtherShip);
            AcceptPayment();
            return true;
        } else {
            Response = aShip::TShip_LookupVisibleTalkText(this,
                    pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        }
    }

    std::uint8_t TWarrior_BuildAttackRequestResponse(TWarrior* Self, aShip::TShip* Requester, pas::WideString& Response,
        aShip::TShip* Target) {
        std::uint8_t Result{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        // Nested helper with caller-popped static link.
        auto AcceptRequest = [&]() -> void {
            Response = aShip::TShip_LookupVisibleTalkText(Self,
                    pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Ok"}), Requester);
            Self->SetJointAttackTarget(Requester, Target);
            Result = true;
        };
        Result = false;
        if (pas::class_cast_if<aRanger::TRanger*>(Requester) != nullptr) {
            if (pas::in_range(Target->TypeId, static_cast<std::int32_t>(aGalaxyStruct::stRanger),
                static_cast<std::int32_t>(aGalaxyStruct::stPirate))) {
                Target->ChangeRelationToRanger(Requester, -20);
            }
            if (Target->OwnerId == aGalaxyStruct::oiDominator || Target->TypeId == aGalaxyStruct::stPirate) {
                pas::checked_cast<aRanger::TRanger*>(Requester)->AddWarriorCareerActivity(1);
            } else {
                pas::checked_cast<aRanger::TRanger*>(Requester)->AddPirateCareerActivity(8);
            }
        }
        std::int32_t HostileCount = 0;
        if (Self->WarriorType == wtFlagship) {
            const std::int32_t cpp_last = pas::list_count(Self->CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                    if (Ship != Self && Ship->InNormalSpace()) {
                        if (aShip::TShip_GetRelationLevelToShip(Self, Ship) <= aGalaxyStruct::rlHostile) {
                            ++HostileCount;
                        }
                        if (HostileCount > 1) {
                            break;
                        }
                    }
                }
            }
        }
        if (Self->OrderTarget == Target && aShip::TShip_GetRelationLevelToShip(Self, Target) ==
                aGalaxyStruct::rlHostile && HostileCount <= 1) {
            AcceptRequest();
        } else if (Self->LiberationGroup != nullptr && Requester->LiberationGroup != Self->LiberationGroup) {
            Response = aShip::TShip_LookupVisibleTalkText(Self,
                    pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else if (Self->TruceShip == Target) {
            Response = ([&] {
                    pas::WideString name = Target->GetName();
                    pas::WideString lookupVisibleTalkText = aShip::TShip_LookupVisibleTalkText(Self,
                        u"Talk.Attack.WeAlreadyHavePact"_wref.get(), Requester);
                    pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText1(std::move(lookupVisibleTalkText), std::move(textHighlightColorTag),
                        u"<Target>"_w, std::move(name));
                }());
        } else if (aShip::TShip_RelationToShip(Self, Target) >= aGalaxyStruct::RelationNormalMin) {
            if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(
                Target); !(tranclucator != nullptr)) {
                Response = aShip::TShip_LookupVisibleTalkText(Self,
                        pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"WeFriends"}), Requester);
            } else if (tranclucator->OwnerShip == Self) {
                Response = aShip::TShip_LookupVisibleTalkText(Self,
                        pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"ItsMyTranc"}), Requester);
            } else if (tranclucator->OwnerShip == Requester) {
                Response = aShip::TShip_LookupVisibleTalkText(Self,
                        pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"ItsYourTranc"}), Requester);
            } else {
                Response = aShip::TShip_LookupVisibleTalkText(Self,
                        pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"WeFriendsTranc"}), Requester);
            }
        } else if (Self->virtual_TShip_AcceptsRansomDemandFrom(Target) || Self->virtual_TShip_RecomputeFearState()) {
            Response = aShip::TShip_LookupVisibleTalkText(Self,
                    pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Fear"}), Requester);
        } else if (!Self->virtual_TShip_TrustsAttackRequester(Requester)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self,
                    pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Suspect"}), Requester);
        } else if (Self->HasLockedOrFollowOrder() && pas::class_cast_if<aNormalShip::TNormalShip*>(Requester) !=
                nullptr && static_cast<aNormalShip::TNormalShip*>(Requester)->Rank < Self->Rank) {
            Response = aShip::TShip_LookupVisibleTalkText(Self,
                    pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else if (HostileCount > 0) {
            Response = aShip::TShip_LookupVisibleTalkText(Self,
                    pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else {
            AcceptRequest();
        }
        return Result;
    }

    std::uint8_t TWarrior_AcceptPartnershipOffer(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    std::uint8_t TWarrior_BuildPartnershipOfferResponse(TWarrior* Self, aShip::TShip* OtherShip,
        pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    void TWarrior::MoveToRandomPlanetOrbit() {
        aMyFunction::TPolarPoint Polar{};
        aPlanet::TPlanet* Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, 0);
        Polar = Planet->Orbit;
        Polar.AngleDegrees = aMyFunction::NextRandomIntRange(0, 359, RandomState);
        OrderMove(aMyFunction::PolarToPoint(Polar), false);
    }

    // Requires the home system and (Id + CurrentTurn) mod 100 < 25.
    std::uint8_t TWarrior::IsHomePatrolTurn() {
        return GetHomeStar() == CurrentStar && (Id + aGalaxy::Galaxy->CurrentTurn) % 100 < 25;
    }

    void TWarrior::ConsumeNodes(std::int32_t Amount) {
        std::int32_t I{};
        std::int32_t RemainingWeight{};
        aItem::TItem* Item{};
        {
            const std::int32_t cpp_first = pas::list_count(Inventory) - 1;
            if (cpp_first >= 1) {
                for (I = cpp_first; I >= 1; --I) {
                    Item = pas::list_at<aItem::TItem>(Inventory, I);
                    if (Item->ItemType == aConst::t_Protoplasm) {
                        if (Amount < Item->Weight) {
                            RemainingWeight = Item->Weight - Amount;
                            pas::checked_cast<aItem::TProtoplasm*>(Item)->StackCount -= Amount;
                            Item->Cost = pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(Item->Cost) / Item->Weight) *
                                    RemainingWeight));
                            Item->Weight = RemainingWeight;
                            Amount = 0;
                        } else {
                            Amount -= Item->Weight;
                            pas::list_delete(Inventory, I);
                            pas::free(Item);
                        }
                    }
                    if (Amount == 0) {
                        break;
                    }
                }
            }
        }
    }

    std::uint8_t TWarrior::RefusesFactionNegotiation(aShip::TShip* OtherShip) {
        std::uint8_t Result = false;
        if (OtherShip->CurrentStanding == aGalaxyStruct::ssPirateMilitary) {
            return true;
        }
        if ((CurrentStar->Status.ControlFaction != aGalaxyStruct::sfCoalition || CurrentStar->Status.CustomFaction !=
                u"") && pas::is_one_of<aGalaxyStruct::ssPirateActive,
                aGalaxyStruct::ssPirateMilitary>(OtherShip->CurrentStanding)) {
            return true;
        }
        return Result;
    }

    float TWarrior::AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) {
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        float MoneyPenalty{};
        float WeightPenalty{};
        std::int32_t Price{};
        float DesiredMoneyFraction = 0.1f;
        float FragilityScale = 1.5f;
        if (pas::is_one_of<aConst::t_FuelTanks, aConst::t_Radar, aConst::t_CargoHook>(Item->ItemType)) {
            FragilityScale = static_cast<float>(FragilityScale * 0.5);
        }
        float HullValueScale = 2.0f;
        float DesiredFreeFraction = pas::real_max<double>(0.01,
            pas::real_min<double>(0.99,
                static_cast<float>(static_cast<double>(GetDesiredCargoFreeSpace()) / std::max<std::int32_t>(100, GetHull()->Weight))));
        {
            float real_max_scalar = pas::real_max_scalar<float>(0.01f, SmoothedMoneyFraction);
            MoneyPenalty = static_cast<float>(pas::sqr(static_cast<float>((static_cast<float>(static_cast<float>(1.0 / real_max_scalar) - 1.0)) / (static_cast<float>(static_cast<float>(1.0 / DesiredMoneyFraction) - 1.0)))) /
                    pas::real_max<double>(static_cast<float>(SmoothedWealth * 0x1.999999999999bp-5), 1.0E+3));
        }
        float EffectivenessScale = static_cast<float>(2.0 / pas::real_max_scalar<float>(1.0E+1f, SmoothedEquipmentEffectiveness));
        {
            float real_max_scalar_3 = pas::real_max_scalar<float>(0.01f, SmoothedFreeCapacityFraction);
            WeightPenalty = static_cast<float>(pas::sqr(static_cast<float>((static_cast<float>(static_cast<float>(1.0 / real_max_scalar_3) - 1.0)) / (static_cast<float>(static_cast<float>(1.0 / DesiredFreeFraction) - 1.0)))) /
                    pas::real_max<double>(1.0E+1, static_cast<float>(GetHull()->Weight * 0x1.999999999999bp-4)));
        }
        if (OwnerId == Item->OwnerId) {
            EffectivenessScale = static_cast<float>(EffectivenessScale * 0x1.199999999999bp+0);
        }
        MoneyPenalty = static_cast<float>(static_cast<float>(MoneyPenalty * 0x1.47ae147ae147ap-7) * (100 + aMyFunction::SeededRandomIntRange(-10, 10, Id + Seed)));
        EffectivenessScale = static_cast<float>(static_cast<float>(EffectivenessScale * 0x1.47ae147ae147ap-7) * (100 + aMyFunction::SeededRandomIntRange(-10, 10,
                    Seed + 3 * Id)));
        WeightPenalty = static_cast<float>(static_cast<float>(WeightPenalty * 0x1.47ae147ae147ap-7) * (100 + aMyFunction::SeededRandomIntRange(-10, 10, Seed + 5 * Id)));
        if (WarriorType == wtFlagship) {
            EffectivenessScale = static_cast<float>(2.0 * EffectivenessScale);
        }
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
            return static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Effectiveness) * EffectivenessScale) * (static_cast<float>(1.0 + static_cast<float>((static_cast<float>(2.0 -
                        reinterpret_cast<aItem::TEquipment*>(
                        Item)->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags)))) *
                        FragilityScale)))) - static_cast<float>(static_cast<double>(Item->Weight) * WeightPenalty)) -
                static_cast<float>(static_cast<double>(Price) * MoneyPenalty));
        }
        return static_cast<float>(static_cast<float>(static_cast<float>((static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Item->Weight) * HullValueScale) / pas::real_max_scalar<float>(0.01f,
                reinterpret_cast<aItem::TEquipment*>(
                    Item)->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags)))) + Effectiveness)) *
            EffectivenessScale) + static_cast<float>(static_cast<double>(Item->Weight) * WeightPenalty)) - static_cast<float>(static_cast<float>(Price * 0.25) * MoneyPenalty));
    }

    float TWarrior::EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        float Result = 0.0f;
        if (Value == 0) {
            return Result;
        }
        {
            aConst::TEquipmentBonusKind cpp_case = BonusKind;
            if (cpp_case == aConst::bonHull) {
                Result = Value * 200;
            } else if (cpp_case == aConst::bonFuel) {
                Result = static_cast<float>(static_cast<float>(0.5 * Value) * (static_cast<float>(1.0 - static_cast<float>(static_cast<std::int8_t>(WarriorType == wtFlagship) * 0.5))));
            } else if (cpp_case == aConst::bonSpeed) {
                Result = static_cast<float>(Value * 0x1.999999999999bp-3);
            } else if (cpp_case == aConst::bonJump) {
                Result = Value * 5;
            } else if (cpp_case == aConst::bonRadar) {
                Result = static_cast<float>(static_cast<float>(0x1.999999999999bp-6 * Value) * (static_cast<float>(1.0 + static_cast<float>(static_cast<std::int8_t>(WarriorType == wtFlagship) * 0.5))));
            } else if (cpp_case == aConst::bonScan) {
                Result = Value * 3 + Value * 30 *
                            CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags));
            } else if (cpp_case == aConst::bonDroid) {
                Result = static_cast<float>(static_cast<float>(static_cast<double>(Value * 10) / pas::real_max_scalar<float>(0.1f,
                            GetHull()->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags)))) * (static_cast<float>(1.0 +
                            static_cast<float>(static_cast<std::int8_t>(WarriorType == wtFlagship) * 0.5))));
            } else if (cpp_case == aConst::bonHook) {
                Result = static_cast<float>(static_cast<float>((static_cast<float>(pas::real_min<double>(static_cast<double>(Value),
                        static_cast<float>(static_cast<double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5])) + static_cast<float>(Value * 0x1.999999999999bp-4))) *
                        0x1.4ccccccccccccp+0) * static_cast<std::int8_t>(WarriorType == wtFlagship));
            } else if (cpp_case == aConst::bonDef) {
                Result = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Value * 5 * 100) / std::max<std::int32_t>(5, 100 - Value)) * 45.0) /
                        std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                Result = Value * 12;
            } else if (cpp_case == aConst::bonWSplinter) {
                Result = Value * 12;
            } else if (cpp_case == aConst::bonWMissile) {
                Result = static_cast<float>(static_cast<float>(Value * 12 * (static_cast<float>(0.1 + static_cast<float>(static_cast<std::int8_t>(GetRadarRange() > 0) * 0x1.ccccccccccccbp-1)))) * (1 - (WarriorType ==
                            wtFlagship)));
            } else if (cpp_case == aConst::bonWRadius) {
                if (WarriorType == wtFlagship) {
                    Result = Value * 3;
                } else {
                    Result = static_cast<float>(static_cast<float>(Value * 1.5) * pas::sqr(static_cast<float>(static_cast<double>(std::max<std::int32_t>(100,
                                SmoothedEnemySpeed)) / std::max<std::int32_t>(100, SmoothedSpeed))));
                }
            } else if (cpp_case == aConst::bonHookRadius) {
                Result = static_cast<float>(static_cast<float>(Value * 1.0) * static_cast<std::int8_t>(WarriorType == wtFlagship));
            } else if (cpp_case == aConst::bonMass) {
                Result = static_cast<float>(aMyFunction::RemapClamped(Value, aConst::HullMassEvaluationStart,
                        aConst::HullMassEvaluationEnd, 1.0, 0.333) * 1.0E+3);
            } else if (cpp_case == aConst::bonSlotRadar) {
                if (GetSlotCount(aConst::sskRadar) == 0 && Value > 0) {
                    Result = static_cast<float>(WarriorSlotBonusWeights[BonusKind] * 0x1.3333333333334p-2);
                } else if (GetRadar() != nullptr && Value < 0) {
                    Result = -WarriorSlotBonusWeights[BonusKind] - WarriorSlotBonusWeights[aConst::bonSlotWeapon] *
                                CountMissileWeapons();
                } else if (GetSlotCount(aConst::sskRadar) == 1 && Value < 0) {
                    Result = static_cast<float>(WarriorSlotBonusWeights[BonusKind] * -0x1.3333333333334p-2);
                }
            } else if (cpp_case == aConst::bonSlotScaner) {
                if (GetSlotCount(aConst::sskScanner) == 0 && Value > 0) {
                    Result = static_cast<float>(WarriorSlotBonusWeights[BonusKind] * 0x1.3333333333334p-2);
                } else if (GetScanner() != nullptr && Value < 0) {
                    Result = static_cast<float>(-WarriorSlotBonusWeights[BonusKind] -
                            static_cast<float>(static_cast<float>(CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) * 0x1.999999999999bp-4) *
                                WarriorSlotBonusWeights[aConst::bonSlotWeapon]));
                } else if (GetSlotCount(aConst::sskScanner) == 1 && Value < 0) {
                    Result = static_cast<float>(WarriorSlotBonusWeights[BonusKind] * -0x1.3333333333334p-2);
                }
            } else if (cpp_case == aConst::bonSlotDroid) {
                if (GetSlotCount(aConst::sskRepairRobot) == 0 && Value > 0) {
                    Result = static_cast<float>(WarriorSlotBonusWeights[BonusKind] * 0x1.3333333333334p-2);
                } else if (GetRepairRobot() != nullptr && Value < 0) {
                    Result = -WarriorSlotBonusWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskRepairRobot) == 1 && Value < 0) {
                    Result = static_cast<float>(WarriorSlotBonusWeights[BonusKind] * -0x1.3333333333334p-2);
                }
            } else if (cpp_case == aConst::bonSlotDef) {
                if (GetSlotCount(aConst::sskDefGenerator) == 0 && Value > 0) {
                    Result = static_cast<float>(WarriorSlotBonusWeights[BonusKind] * 0x1.3333333333334p-2);
                } else if (GetDefGenerator() != nullptr && Value < 0) {
                    Result = -WarriorSlotBonusWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskDefGenerator) == 1 && Value < 0) {
                    Result = static_cast<float>(WarriorSlotBonusWeights[BonusKind] * -0x1.3333333333334p-2);
                }
            } else if (cpp_case == aConst::bonSlotWeapon) {
                if (GetSlotCount(aConst::sskWeapon) < 5 && Value > 0) {
                    Result = std::min<std::int32_t>(Value, 5 - GetSlotCount(aConst::sskWeapon)) *
                            WarriorSlotBonusWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -GetSlotCount(aConst::sskWeapon)) *
                            WarriorSlotBonusWeights[BonusKind];
                }
                {
                    std::int32_t max_6 = std::max<std::int32_t>(Value + GetSlotCount(aConst::sskWeapon), 1);
                    if (CountEquippedWeapons() > max_6) {
                        std::int32_t max_7 = std::max<std::int32_t>(1, Value + GetSlotCount(aConst::sskWeapon));
                        Result = static_cast<float>(Result - static_cast<float>(static_cast<float>(WarriorSlotBonusWeights[BonusKind] * 0x1.3333333333334p-1) * (CountEquippedWeapons() - max_7)));
                    }
                }
            } else if (cpp_case == aConst::bonSlotForsage) {
                if (GetSlotCount(aConst::sskAfterburner) == 0 && Value > 0) {
                    Result = WarriorSlotBonusWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskAfterburner) == 1 && Value < 0) {
                    Result = -WarriorSlotBonusWeights[BonusKind];
                }
            } else if (cpp_case >= aConst::bonSkill1 && cpp_case <= aConst::bonSkill6) {
                if (Value > 0) {
                    Result = std::min<std::int32_t>(
                            6 - GetEffectiveSkillLevel(aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1],
                                false), Value) * WarriorSkillBonusWeights[BonusKind];
                }
                if (Value > 0 && Value + GetEffectiveSkillLevel(
                            aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) > 6) {
                    Result = static_cast<float>(Result + static_cast<float>(static_cast<float>(WarriorSkillBonusWeights[BonusKind] * 0x1.999999999999bp-5) * (Value + GetEffectiveSkillLevel(
                                aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) - 6)));
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(
                            static_cast<std::int32_t>(GetEffectiveSkillLevel(
                                aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false)), -Value) *
                            -WarriorSkillBonusWeights[BonusKind];
                }
                if (Value < 0 && Value + GetEffectiveSkillLevel(
                            aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) < 0) {
                    Result = static_cast<float>(Result + static_cast<float>(static_cast<float>(WarriorSkillBonusWeights[BonusKind] * 0x1.eb851eb851eb7p-6) * (Value + GetEffectiveSkillLevel(
                                aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false))));
                }
            } else {
                Result = 0.0f;
            }
        }
        if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonSkill1),
            static_cast<std::int32_t>(aConst::bonSkill6))) {
            double cpp_left = static_cast<float>(static_cast<float>(Result * 0x1.47ae147ae147ap-7) * (100 + aMyFunction::SeededRandomIntRange(-30, 30,
                        Seed + 131 * BonusKind)));
            return static_cast<float>(cpp_left * aConst::RaceSkillEvaluationFactors[PilotRace][aConst::EquipmentBonusSkills[BonusKind -
                    aConst::bonSkill1]]);
        }
        return static_cast<float>(static_cast<float>(Result * 0x1.47ae147ae147ap-7) * (100 + aMyFunction::SeededRandomIntRange(-10, 10, Seed + 131 * BonusKind)));
    }

    float TWarrior::EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses,
        float BaseDamage) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> ShockFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkShock}});
        static const pas::Set<0, 255> AcidFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkAcid}});
        float ScannerFactor{};
        aGalaxyStruct::TDamageFlagSet Flags{};
        std::int32_t I{};
        std::int32_t ShotTotal{};
        Flags = Weapon->GetDamageFlags();
        if (Flags * static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags) !=
            pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}) && GetScanner() != nullptr && GetRadar() != nullptr) {
            ScannerFactor = aMyFunction::RemapClamped(
                    GetScannerPower() -
                        aItem::DefenseDamageFactorToPercent(
                        aItem::GetGeneratedDefenseDamageFactor(aGalaxy::Galaxy->TechLevel)) + 1, -5.0, 1.0E+1, 0.1,
                    2.0);
        } else {
            ScannerFactor = 0.0f;
        }
        float Result = static_cast<float>(static_cast<double>(BaseDamage) * GetWeaponArtefactDamageFactor(Weapon));
        if (pas::contains(Flags, aGalaxyStruct::dkDrain)) {
            Result = static_cast<float>(Result * (static_cast<float>(1.5 - static_cast<float>(static_cast<std::int8_t>(WarriorType == wtFlagship) * 0x1.999999999999bp-2))));
        }
        if (pas::contains(Flags, aGalaxyStruct::dkShock)) {
            Result = static_cast<float>(Result * (static_cast<float>(1.05 +
                        static_cast<float>(CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ShockFlags)) * 0x1.999999999999bp-5))));
        }
        if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
            Result = static_cast<float>(Result * 0x1.0ccccccccccccp+0);
        }
        float StatusFactor = 1.0f;
        if (pas::contains(Flags, aGalaxyStruct::dkScanBonus)) {
            StatusFactor = static_cast<float>(StatusFactor * (static_cast<float>(1.0 + static_cast<float>(ScannerFactor * 0x1.999999999999bp-4))));
        }
        if (pas::contains(Flags, aGalaxyStruct::dkBonusToDamaged)) {
            StatusFactor = static_cast<float>(StatusFactor * (static_cast<float>(1.0 + static_cast<float>(ScannerFactor * 0x1.999999999999bp-4))));
        }
        StatusFactor = static_cast<float>(StatusFactor - 1.0);
        if (IncludeAdditiveBonuses) {
            if (pas::contains(Flags, aGalaxyStruct::dkBlockWeapon)) {
                Result = static_cast<float>(Result + static_cast<float>(ScannerFactor * 5.0));
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDroidBlock)) {
                Result = static_cast<float>(Result + static_cast<float>(ScannerFactor * 5.0));
            }
            {
                std::int32_t cpp_left = CountWeaponsByDamageFlags(
                    static_cast<aGalaxyStruct::TDamageFlagSet>(AcidFlags));
                Result = static_cast<float>(static_cast<double>(Result) + cpp_left * Weapon->GetShotCount());
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
                Result = static_cast<float>(static_cast<double>(Result) + ShotTotal * 2);
            }
        }
        float SpeedFactor = static_cast<float>(std::max<std::int32_t>(100, SmoothedEnemySpeed) * GetHull()->Weight /
            (static_cast<float>(aConst::HullBaseSize * pas::real_max<double>(1.0E+2,
                    static_cast<float>(static_cast<double>(SmoothedSpeed) * aConst::EquipmentSizeFactors[1])))));
        switch (Weapon->GetWeaponInfo()->ShotType) {
            case aGalaxyStruct::wstRocket: Result = static_cast<float>(static_cast<float>(static_cast<float>(Result * 0x1.199999999999bp+0) * Weapon->GetShotCount()) * (static_cast<float>(1.0 + StatusFactor))); break;
            case aGalaxyStruct::wstMissile: {
                Result = static_cast<float>(static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.1 + static_cast<float>(static_cast<float>(Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.0) * 0x1.47ae147ae147ap-7)) + StatusFactor))) *
                        Weapon->GetShotCount());
                break;
            }
            case aGalaxyStruct::wstTorpedo: {
                Result = static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.0 + static_cast<float>(static_cast<float>(Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.0) * 0x1.47ae147ae147ap-7)) + StatusFactor)));
                break;
            }
            case aGalaxyStruct::wstChain: {
                Result = static_cast<float>(static_cast<float>(Result * (static_cast<float>(1.1 + static_cast<float>((Weapon->GetShotCount() - 1) * 0x1.999999999999bp-3)))) * (static_cast<float>(1.0 + StatusFactor)));
                break;
            }
            case aGalaxyStruct::wstSplash: {
                Result = static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.0 + static_cast<float>(static_cast<float>(static_cast<float>(Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.5) * 0x1.47ae147ae147ap-7) * SpeedFactor)) +
                            StatusFactor)));
                break;
            }
            case aGalaxyStruct::wstExploder: {
                Result = static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.0 + static_cast<float>(static_cast<float>(static_cast<float>(Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0x1.999999999999bp-1) * 0x1.47ae147ae147ap-7) * SpeedFactor)) +
                            StatusFactor)));
                break;
            }
            case aGalaxyStruct::wstAreaDamage: {
                Result = static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.0 + static_cast<float>(static_cast<float>(static_cast<float>(Weapon->Range * 0x1.199999999999bp+1) * 0x1.47ae147ae147ap-7) * SpeedFactor)) + StatusFactor)));
                break;
            }
            default: Result = static_cast<float>(Result * (static_cast<float>(1.0 + StatusFactor))); break;
        }
        Result = static_cast<float>(static_cast<double>(Result) * Weapon->GetAttackCount());
        return static_cast<float>(static_cast<float>(Result * 0x1.47ae147ae147ap-7) * (100 + aMyFunction::SeededRandomIntRange(-20, 20,
                    Seed + Weapon->GetWeaponInfo()->TypeHash)));
    }

    std::uint8_t TWarrior::AcceptPickupItem(aItem::TItem* Item) {
        std::uint8_t Result = false;
        if (WarriorType == wtFlagship) {
            if (Item->ItemType == aConst::t_Protoplasm && CargoFreeSpace > 0) {
                return true;
            }
            if (pas::in_range(PilotRace, static_cast<std::int32_t>(aGalaxyStruct::oiPeleng),
                static_cast<std::int32_t>(aGalaxyStruct::oiFeyan)) && pas::class_cast_if<aItem::TUselessItem*>(Item) !=
                    nullptr) {
                if (static_cast<aItem::TUselessItem*>(Item)->IsDominatorRemains()) {
                    return true;
                }
            }
        }
        return Result;
    }

    std::uint8_t TWarrior::AcceptPickupDistance(aItem::TItem* Item, double Distance) {
        if (Speed < 1) {
            return false;
        }
        return static_cast<double>(30 * Speed) >= Distance || Item->Cost >=
                static_cast<float>(static_cast<double>(aMyFunction::RemapClamped(static_cast<float>(static_cast<double>(Distance) / Speed), 1.0, 1.0E+1, 0.01,
                    0.05)) * Wealth);
    }

    void TWarrior_RefreshCurrentStanding(TWarrior* Self) {
        aGalaxyStruct::TScriptStandingOverrideMode StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            Self->CurrentStanding = aGalaxyStruct::ssCoalitionMilitary;
        }
    }

    void TWarrior::p_destroy() {
        aWarrior::TWarrior_Destroy(this);
    }

    void TWarrior::virtual_TShip_NextDay() {
        aWarrior::TWarrior_NextDay(this);
    }

    void TWarrior::virtual_TShip_NextDayLogic() {
        aWarrior::TWarrior_NextDayLogic(this);
    }

    std::uint8_t TWarrior::virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return aWarrior::TWarrior_CanQueueReachablePlanet(this, Planet);
    }

    std::uint8_t TWarrior::virtual_TShip_RecomputeFearState() {
        return aWarrior::TWarrior_RecomputeFearState(this);
    }

    std::uint8_t TWarrior::virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) {
        return aWarrior::TWarrior_AcceptsRansomDemandFrom(this, Ship);
    }

    std::uint8_t TWarrior::virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) {
        return aWarrior::TWarrior_TrustsAttackRequester(this, Ship);
    }

    std::uint8_t TWarrior::virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t DemandedAmount) {
        return aWarrior::TWarrior_BuildMoneyExtortionResponse(this, OtherShip, Response, DemandedAmount);
    }

    std::uint8_t TWarrior::virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip,
        pas::WideString& Response) {
        return aWarrior::TWarrior_BuildCargoExtortionResponse(this, OtherShip, Response);
    }

    std::uint8_t TWarrior::virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response,
        aShip::TShip* Target) {
        return aWarrior::TWarrior_BuildAttackRequestResponse(this, Requester, Response, Target);
    }

    std::uint8_t TWarrior::virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        return aWarrior::TWarrior_AcceptPartnershipOffer(this, OtherShip, Response, PaymentAmount);
    }

    std::uint8_t TWarrior::virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip,
        pas::WideString& Response, std::int32_t PaymentAmount) {
        return aWarrior::TWarrior_BuildPartnershipOfferResponse(this, OtherShip, Response, PaymentAmount);
    }

    void TWarrior::virtual_TShip_RefreshCurrentStanding() {
        aWarrior::TWarrior_RefreshCurrentStanding(this);
    }

} // namespace aWarrior
