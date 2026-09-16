#include "layout/aWarrior.hpp"
#include "types/EC_BlockPar.hpp"
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
#include "types/aShip.hpp"
#include "types/aTranclucator.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
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
#include "units/aWarrior.hpp"

namespace aWarrior {
    void GetWeaponRangeBounds(aShip::TShip* Ship, std::int32_t& Minimum, std::int32_t& Maximum, TWarrior* Self);

    std::int32_t AreaWeaponScore(aShip::TShip* Ship, TWarrior* Self);

    const pas::Array<std::int32_t, 22, 27> WarriorSkillBonusWeights = pas::Array<std::int32_t, 22, 27>{{100, 100, 80, 10, 10, 10}};

    // EvaluateStatBonus dispatches bonus kinds 13..20 to these slot cases.
    const pas::Array<std::int32_t, 13, 20> WarriorSlotBonusWeights = pas::Array<std::int32_t, 13, 20>{{80, 80, 200, 0, 200, 150, 0, 10}};

    void TWarrior_Destroy(TWarrior* Self) {
        std::int32_t Index = pas::list_indexof(Self->HomePlanet->Warriors, reinterpret_cast<void*>(Self));
        if (Index >= 0) {
            pas::list_delete(Self->HomePlanet->Warriors, Index);
        }
        aNormalShip::TNormalShip_Destroy(Self);
    }

    // Sets location, money and WarriorType; registers the ship with its star and home garrison.
    void TWarrior::InitGenerated(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::uint8_t Kind) {
        std::int32_t FirstNameIndex{};
        std::int32_t LastNameIndex{};
        std::uint8_t TechLevel{};
        EC_BlockPar::TBlockParEC* Config{};
        aItem::TWeapon* Weapon{};
        // Nested helper with caller-popped static link.
        auto SelectTechLevel = [&](std::int32_t Minimum, std::int32_t Maximum) -> std::int32_t {
            std::int32_t Result = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 1.0, 8.0, Minimum, Maximum));
            return std::max<std::int32_t>(std::min<std::int32_t>(Result + aMyFunction::NextRandomIntRange(-2, 2, this->RandomState), Maximum), Minimum);
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
        if (GR_Main::ModShipNameConfig != nullptr && GR_Main::ModShipNameConfig->CountBlocks(u"Warrior"_wref.get()) > 0) {
            Config = GR_Main::ModShipNameConfig->GetBlock(u"Warrior"_wref.get());
            if (aConst::RaceToOwner(PilotRace) == OwnerId) {
                Config = Config->GetBlock(aConst::OwnerToSys(OwnerId));
            } else {
                if (Config->CountBlocks(aConst::OwnerToSys(OwnerId)) > 0) {
                    Config = Config->GetBlock(aConst::OwnerToSys(OwnerId));
                }
                if (Config->CountBlocks(aConst::OwnerToSys(aConst::RaceToOwner(PilotRace))) > 0) {
                    Config = Config->GetBlock(aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)));
                }
            }
            LastNameIndex = 0;
            FirstNameIndex = Config->GetParamCount() - 1;
            Name = pas::concat_wide({Config->GetParamValue(aMyFunction::NextRandomIntRange(LastNameIndex, FirstNameIndex, RandomState)), u" ", u"-", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 100 + 1)), u"-"});
        }
        if (GetName().length() == 0) {
            Config = GR_Main::LanguageDataConfig->GetBlock(u"ShipName"_wref.get())->GetBlock(u"Warrior"_wref.get());
            if (aConst::RaceToOwner(PilotRace) == OwnerId) {
                Config = Config->GetBlock(aConst::OwnerToSys(OwnerId));
            } else {
                if (Config->CountBlocks(aConst::OwnerToSys(OwnerId)) > 0) {
                    Config = Config->GetBlock(aConst::OwnerToSys(OwnerId));
                }
                if (Config->CountBlocks(aConst::OwnerToSys(aConst::RaceToOwner(PilotRace))) > 0) {
                    Config = Config->GetBlock(aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)));
                }
            }
            LastNameIndex = 0;
            FirstNameIndex = Config->GetParamCount() - 1;
            Name = pas::concat_wide({Config->GetParamValue(aMyFunction::NextRandomIntRange(LastNameIndex, FirstNameIndex, RandomState)), u" ", u"-", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 100 + 1)), u"-"});
        }
        if (aPlayer::GetPlayer() != nullptr) {
            Rank = aMyFunction::NextRandomIntRange(0, std::min<std::int32_t>(5, aPlayer::GetPlayer()->Rank + 2), RandomState);
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
                std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, static_cast<std::int32_t>(aConst::CoalitionRankPointThresholds[Rank]) / 2, RandomState);
                aNormalShip::TNormalShip* self = this;
                self->AddRankPoints(nextRandomIntRange);
            }
            if (!aGalaxy::Galaxy->IsZeroStartingExperienceEnabled()) {
                GainExperience(System::Round(aMyFunction::RemapClamped(static_cast<std::int8_t>(Rank + static_cast<std::uint8_t>(0)), 0.0, 1.0E+3, aConst::TotalSkillTrainingCost / 6, aConst::TotalSkillTrainingCost / 2)), 0);
                {
                    double techLevel = aGalaxy::Galaxy->TechLevel;
                    double nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(0, aConst::TotalSkillTrainingCost / 2, RandomState);
                    std::int32_t round = System::Round(aMyFunction::RemapClamped(techLevel, 3.0, 8.0, 0.0, nextRandomIntRange_2));
                    aShip::TShip* self_2 = this;
                    self_2->GainExperience(round, 0);
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
                std::uint8_t raceToOwner = aConst::RaceToOwner(PilotRace);
                std::uint16_t round_2 = System::Round(static_cast<long double>(3 * aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]);
                std::uint8_t selectTechLevel = SelectTechLevel(3, 8);
                aShip::TShip* self_3 = this;
                self_3->CreateAndEquipHull(round_2, selectTechLevel, raceToOwner, -1, false);
            }
            CreateAndEquipFuelTanks(System::Round(static_cast<long double>(2 * aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
            CreateAndEquipEngine(System::Round(static_cast<long double>(2 * aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 1, OwnerId);
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                Weapon = CreateAndEquipWeapon(aConst::t_Weapon3, 2 * aConst::WeaponInfos[aConst::t_Weapon3].AverageSize, 1, OwnerId);
                Weapon->DetailImprovement = 3;
                Weapon->Improve(aItem::ikAny);
            }
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                Weapon = CreateAndEquipWeapon(aConst::t_Weapon3, 2 * aConst::WeaponInfos[aConst::t_Weapon3].AverageSize, 1, OwnerId);
                Weapon->DetailImprovement = 3;
                Weapon->Improve(aItem::ikAny);
            }
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                Weapon = CreateAndEquipWeapon(aConst::t_Weapon3, 2 * aConst::WeaponInfos[aConst::t_Weapon3].AverageSize, 1, OwnerId);
                Weapon->DetailImprovement = 3;
                Weapon->Improve(aItem::ikAny);
            }
            if (GetSlotCountForItemType(aConst::t_Radar) > 0) {
                std::uint8_t ownerId = OwnerId;
                pas::Extended cpp_right = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, RandomState)];
                std::int32_t round_3 = System::Round(2 * aConst::RadarBaseSize * cpp_right);
                aShip::TShip* self_4 = this;
                self_4->CreateAndEquipRadar(round_3, 1, ownerId);
            }
            if (GetSlotCountForItemType(aConst::t_Scaner) > 0) {
                std::uint8_t ownerId_2 = OwnerId;
                pas::Extended cpp_right_2 = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, RandomState)];
                std::int32_t round_4 = System::Round(2 * aConst::ScannerBaseSize * cpp_right_2);
                aShip::TShip* self_5 = this;
                self_5->CreateAndEquipScanner(round_4, 1, ownerId_2);
            }
        } else {
            {
                std::uint8_t raceToOwner_2 = aConst::RaceToOwner(PilotRace);
                std::int32_t selectRandomHullSeries = SelectRandomHullSeries();
                std::uint16_t round_5 = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]);
                std::uint8_t cpp_arg = std::min<std::int32_t>(static_cast<std::int32_t>(TechLevel), SelectTechLevel(1, 6));
                aShip::TShip* self_6 = this;
                self_6->CreateAndEquipHull(round_5, cpp_arg, raceToOwner_2, selectRandomHullSeries, false);
            }
            CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
            CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 1, OwnerId);
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                CreateAndEquipWeapon(aConst::t_Weapon1, aConst::WeaponInfos[aConst::t_Weapon1].AverageSize, 1, OwnerId);
            }
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                CreateAndEquipWeapon(aConst::t_Weapon3, aConst::WeaponInfos[aConst::t_Weapon3].AverageSize, 1, OwnerId);
            }
            if (GetSlotCountForItemType(aConst::t_Radar) > 0) {
                std::uint8_t ownerId_3 = OwnerId;
                pas::Extended cpp_left = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, RandomState)];
                std::int32_t round_6 = System::Round(cpp_left * aConst::RadarBaseSize);
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
        RefreshCurrentStanding();
        SmoothedSpeed = Speed;
        SmoothedEnemySpeed = Speed;
        BuyEquipmentAtLocation(true);
        BuyEquipmentAtLocation(true);
        BuyEquipmentAtLocation(true);
        BuyEquipmentAtLocation(true);
        BuyEquipmentAtLocation(true);
        if (WarriorType == wtFlagship) {
            BuyEquipmentAtLocation(true);
            BuyEquipmentAtLocation(true);
            BuyEquipmentAtLocation(true);
        }
    }

    void TWarrior::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aNormalShip::TNormalShip::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(WarriorType);
    }

    void TWarrior::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aNormalShip::TNormalShip::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion >= 130) {
            WarriorType = EC_Buf::TBufEC_GetByte(Buffer);
        } else {
            WarriorType = wtRegular;
        }
    }

    void TWarrior::NextDay() {
        aNormalShip::TNormalShip::NextDay();
        try {
            if (ScriptShip != nullptr && HasScriptControl()) {
                ScriptNextDay();
                if (ScriptShip != nullptr) {
                    return;
                }
            }
            if (WarriorType == wtFlagship) {
                SetMoney(Money + std::max<std::int32_t>(1000, std::min<std::int32_t>(5000, aGalaxy::Galaxy->MaxRangerWealth / 15)));
            }
            NextDayLogic();
            if (ScriptShip != nullptr && static_cast<std::uint8_t>(HasScriptControl() ^ 1)) {
                ScriptNextDay();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TWarrior.NextDay ", GetFullName(u" "_wref.get())}))));
            } else {
                throw;
            }
        }
    }

    void TWarrior::NextDayLogic() {
        static const pas::Set<0, 255> FriendlyStationMask = pas::constant_set<pas::Set<0, 255>>({{2}, {3}});
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Station{};
        std::int32_t Stage = 0;
        if (WarriorType == wtFlagship) {
            NextDayFlagshipLogic();
        } else {
            try {
                if (CurrentPlanet != nullptr) {
                    Stage = 1;
                    if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), CurrentPlanet->OwnerId)) {
                        Stage = 2;
                        RepairBrokenEquipmentAtLocation();
                        AutoEquipInventory();
                        OptimizeInventory();
                        RefuelAtLocation();
                        ReloadWeaponAmmo();
                        ProcessUnseenProgression();
                        TrainSkillsAutomatically();
                        if (RepairHullAtLocation()) {
                            return;
                        }
                        Stage = 3;
                        if (LiberationGroup != nullptr) {
                            ProcessLiberationGroupRoute();
                        }
                        if (CurrentPlanet != HomePlanet || LiberationGroup != nullptr || IsHomePatrolTurn()) {
                            OrderTakeoff();
                        }
                    } else {
                        OrderTakeoff();
                    }
                    Stage = 4;
                } else if (DockedTo != nullptr) {
                    Stage = 5;
                    if (!pas::in_range(DockedTo->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                        if (DockedTo->InNormalSpace()) {
                            OrderTakeoff();
                        } else {
                            OrderNone(false);
                        }
                        return;
                    }
                    SynchronizeDockedLocation();
                    RepairBrokenEquipmentAtLocation();
                    AutoEquipInventory();
                    OptimizeInventory();
                    RefuelAtLocation();
                    ReloadWeaponAmmo();
                    ProcessUnseenProgression();
                    TrainSkillsAutomatically();
                    if (RepairHullAtLocation()) {
                        return;
                    }
                    BuyEquipmentAtLocation(false);
                    RestoreEssentialEquipment();
                    Stage = 6;
                    if (LiberationGroup != nullptr) {
                        ProcessLiberationGroupRoute();
                    }
                    if (DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase) && reinterpret_cast<aRuins::TRuins*>(DockedTo)->FlyToStar != nullptr && reinterpret_cast<aRuins::TRuins*>(DockedTo)->FlyToStar != DockedTo->CurrentStar) {
                        return;
                    }
                    if (DockedTo->InNormalSpace()) {
                        OrderTakeoff();
                    }
                    Stage = 7;
                } else if (InNormalSpace()) {
                    Stage = 8;
                    RecomputeFearState();
                    AssignWeaponTargetsInStar();
                    AfterburnerActive = false;
                    Stage = 9;
                    if (InFear) {
                        Stage = 10;
                        if (Order == aShip::soLand || Order == aShip::soJump) {
                            UpdateAfterburnerState();
                        }
                        if (Order != aShip::soLand) {
                            Stage = 11;
                            BuildReachablePlanetQueue();
                            Planet = SelectNearestQueuedPlanet();
                            if (Planet != nullptr && Planet->CurrentStar == CurrentStar) {
                                Stage = 12;
                                OrderLanding(Planet, true);
                                UpdateAfterburnerState();
                            } else {
                                Stage = 13;
                                Station = FindNearestDockableStation(static_cast<aShip::TStationStandingMask>(FriendlyStationMask));
                                if (Station != nullptr) {
                                    Stage = 14;
                                    OrderLanding(Station, true);
                                    UpdateAfterburnerState();
                                } else if (LiberationGroup == nullptr) {
                                    Stage = 15;
                                    SelectEnemyShipInStar();
                                    EngageEnemyShip();
                                    if (Order == aShip::soNone) {
                                        MoveToRandomPatrolPoint();
                                    }
                                }
                            }
                        }
                        Stage = 16;
                    } else {
                        Stage = 17;
                        if (LiberationGroup == nullptr) {
                            Stage = 18;
                            SelectEnemyShipInStar();
                            EngageEnemyShip();
                            Stage = 19;
                            if (Order == aShip::soNone) {
                                if (IsHomePatrolTurn()) {
                                    MoveToRandomPlanetOrbit();
                                } else if (!NavigateToHomePlanet()) {
                                    MoveToRandomPatrolPoint();
                                }
                            }
                            Stage = 20;
                        }
                    }
                    if (LiberationGroup != nullptr) {
                        Stage = 21;
                        AfterburnerActive = false;
                        RefreshDerivedStats(true);
                        ProcessLiberationGroupRoute();
                        Stage = 22;
                        if (LiberationGroup == nullptr) {
                            Stage = 23;
                            SelectEnemyShipInStar();
                            EngageEnemyShip();
                            if (Order == aShip::soNone && static_cast<std::uint8_t>(NavigateToHomePlanet() ^ 1)) {
                                MoveToRandomPatrolPoint();
                            }
                        }
                    }
                }
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TWarrior.NextDayLogic ", GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
                } else {
                    throw;
                }
            }
        }
    }

    // Flagship branch; its diagnostic retains TWarrior.NextDayLogic.
    void TWarrior::NextDayFlagshipLogic() {
        static const pas::Set<0, 255> FriendlyStationMask = pas::constant_set<pas::Set<0, 255>>({{2}, {3}});
        static const pas::Set<0, 255> AnyStationMask = pas::constant_set<pas::Set<0, 255>>({{0, 15}}) - pas::constant_set<pas::Set<0, 255>>({{0, 15}});
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
            if (pas::in_set<0, 1>(this->PilotRace) && this->InFear) {
                Available = GetCarriedNodeCount();
                if (Available > 0) {
                    {
                        pas::Extended cpp_right = aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 2.0, 8.0, 5.0, 1.0);
                        Needed = MathImports::Ceil(pas::real_divide(pas::real_divide((GetHull()->Weight - GetHull()->HullPoints) * (GetCombatStatusStrength(aShip::cseBWRepairDebuff) * 0.002L + 1.0L), 1.0E+1L), 2.0L) * cpp_right);
                    }
                    if (Needed > 0) {
                        Available = std::min<std::int32_t>(Needed, Available);
                        ConsumeNodes(Available);
                        Fraction = pas::real_divide(Available, Needed);
                        Restored = MathImports::Ceil(aMyFunction::RemapClamped(Fraction, 0.0, 1.0, 0.01, GetHull()->Weight - GetHull()->HullPoints));
                        GetHull()->HullPoints += Restored;
                        AddCombatStatusStrength(aShip::cseBWRepairDebuff, pas::real_divide(Restored, 2.0L), nullptr);
                        RefreshDerivedStats(true);
                        if (aPlayer::GetPlayer()->CurrentStar == this->CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
                            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                            Effect->SetEndpoints(nullptr, this->Graphic);
                            Effect->SetHit(aConst::OwnerToFilmColor(this->OwnerId), -Restored, false, true);
                            if (Globals::TrailingFilmEffects == nullptr) {
                                Globals::TrailingFilmEffects = pas::construct_call<aEFilmEnd::TEFilmEnd>(aEFilmEnd::TEFilmEnd_Create);
                            }
                            Entry = Globals::TrailingFilmEffects->AppendEntry();
                            SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject), Effect);
                            SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1), nullptr);
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
            if (pas::in_set<3, 4>(this->PilotRace)) {
                Available = GetCarriedNodeCount();
                if (Available > 0) {
                    NeedsRepair = false;
                    TotalCost = 0;
                    Threshold = System::Round(aMyFunction::RemapClamped(this->CargoFreeSpace, 0.0, GetHull()->Weight / 4, 9.0E+1, 5.0E+1));
                    for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(this->Inventory) - 1); cpp_range.next(I); ) {
                        Equipment = pas::list_at<aItem::TEquipment>(this->Inventory, I);
                        if ((!(pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && CanRepairEquipmentTech(Equipment) && Equipment->ItemType != aConst::t_Hull && Equipment->EquippedFlag != 0) {
                            if (static_cast<long double>(Threshold) >= Equipment->ConditionPercent) {
                                NeedsRepair = true;
                            }
                            TotalCost += System::Round(Equipment->CalculateRepairCost());
                        }
                    }
                    if (NeedsRepair) {
                        {
                            pas::Extended cpp_right = aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 2.0, 8.0, 5.0, 1.0);
                            TotalCost = System::Round(pas::real_divide(TotalCost * 0.0025L, 2.0L) * cpp_right);
                        }
                        if (TotalCost == 0) {
                            TotalCost = 1;
                        }
                        Available = std::min<std::int32_t>(TotalCost, Available);
                        Fraction = pas::real_divide(Available, TotalCost);
                        ConsumeNodes(Available);
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(this->Inventory) - 1); cpp_range_2.next(I); ) {
                            Equipment = pas::list_at<aItem::TEquipment>(this->Inventory, I);
                            if ((!(pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && CanRepairEquipmentTech(Equipment) && Equipment->ItemType != aConst::t_Hull && Equipment->EquippedFlag != 0) {
                                Equipment->ConditionPercent = aMyFunction::RemapClamped(Fraction, 0.0, 1.0, Equipment->ConditionPercent, 1.0E+2);
                                if (Equipment->ConditionPercent > 0.0L) {
                                    Equipment->BrokenFlag = 0;
                                }
                            }
                        }
                        RefreshDerivedStats(true);
                    }
                }
            }
        };
        std::int32_t Stage = 0;
        try {
            if (CurrentPlanet != nullptr) {
                Stage = 1;
                if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), CurrentPlanet->OwnerId)) {
                    Stage = 2;
                    DepositCarriedNodes();
                    RepairBrokenEquipmentAtLocation();
                    AutoEquipInventory();
                    OptimizeInventory();
                    RefuelAtLocation();
                    ReloadWeaponAmmo();
                    ProcessUnseenProgression();
                    TrainSkillsAutomatically();
                    if (RepairHullAtLocation()) {
                        return;
                    }
                    Stage = 3;
                    if (LiberationGroup != nullptr) {
                        ProcessLiberationGroupRoute();
                    }
                    if (CurrentPlanet != HomePlanet || LiberationGroup != nullptr || IsHomePatrolTurn()) {
                        OrderTakeoff();
                    }
                } else {
                    OrderTakeoff();
                }
                Stage = 4;
            } else if (DockedTo != nullptr) {
                Stage = 5;
                SynchronizeDockedLocation();
                DepositCarriedNodes();
                RepairBrokenEquipmentAtLocation();
                AutoEquipInventory();
                OptimizeInventory();
                RefuelAtLocation();
                ReloadWeaponAmmo();
                ProcessUnseenProgression();
                TrainSkillsAutomatically();
                if (RepairHullAtLocation()) {
                    return;
                }
                BuyEquipmentAtLocation(false);
                RestoreEssentialEquipment();
                Stage = 6;
                if (LiberationGroup != nullptr) {
                    ProcessLiberationGroupRoute();
                }
                if (aGalaxy::TGalaxy::FindMilitaryBaseInTransit() == DockedTo) {
                    return;
                }
                if (DockedTo->InNormalSpace()) {
                    OrderTakeoff();
                }
                Stage = 7;
            } else if (InNormalSpace()) {
                Stage = 8;
                RecomputeFearState();
                RepairHullWithNodes();
                RepairEquipmentWithNodes();
                QueueItemsWithinPickupRange();
                AssignWeaponTargetsInStar();
                AfterburnerActive = false;
                Stage = 9;
                if (InFear) {
                    Stage = 10;
                    if (Order == aShip::soLand || Order == aShip::soJump) {
                        UpdateAfterburnerState();
                    }
                    if (Order != aShip::soLand) {
                        Stage = 11;
                        BuildReachablePlanetQueue();
                        Planet = SelectNearestQueuedPlanet();
                        if (Planet != nullptr && Planet->CurrentStar == CurrentStar) {
                            Stage = 12;
                            OrderLanding(Planet, true);
                            UpdateAfterburnerState();
                        } else {
                            Stage = 13;
                            Station = FindNearestDockableStation(static_cast<aShip::TStationStandingMask>(FriendlyStationMask));
                            if (Station != nullptr) {
                                Stage = 14;
                                OrderLanding(Station, true);
                                UpdateAfterburnerState();
                            } else if (LiberationGroup == nullptr) {
                                Stage = 15;
                                SelectEnemyShipInStar();
                                ManeuverFlagship();
                            }
                        }
                    }
                    Stage = 16;
                } else {
                    Stage = 17;
                    if (LiberationGroup == nullptr) {
                        Stage = 18;
                        SelectEnemyShipInStar();
                        ManeuverFlagship();
                        Stage = 19;
                        if (Order == aShip::soNone) {
                            if (aMyFunction::NextRandomIntRange(1, 100, RandomState) > 80) {
                                Station = nullptr;
                                Stations = pas::make_object<pas::List>();
                                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                                    if (pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Ship->CanDock(this) && pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssNeutral)) {
                                        if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, Ship->Position)) >= pas::sqr(CalculateSpeed())) {
                                            pas::list_add(Stations, reinterpret_cast<void*>(Ship));
                                        }
                                    }
                                }
                                if (pas::list_count(Stations) > 0) {
                                    Station = pas::list_at<aShip::TShip>(Stations, aMyFunction::NextRandomIntRange(0, pas::list_count(Stations) - 1, RandomState));
                                }
                                pas::free(Stations);
                                if (Station != nullptr) {
                                    OrderLanding(Station, false);
                                }
                            }
                            if (Order == aShip::soNone) {
                                TryCollectBestFloatingItem(50);
                            }
                            if (Order == aShip::soNone && IsHomePatrolTurn()) {
                                MoveToRandomPlanetOrbit();
                            }
                            if (Order == aShip::soNone && static_cast<std::uint8_t>(NavigateToHomePlanet() ^ 1)) {
                                MoveToRandomPatrolPoint();
                            }
                        }
                        Stage = 20;
                    }
                }
                if (LiberationGroup != nullptr) {
                    Stage = 21;
                    AfterburnerActive = false;
                    RefreshDerivedStats(true);
                    ProcessLiberationGroupRoute();
                    Stage = 22;
                    if (LiberationGroup == nullptr) {
                        Stage = 23;
                        SelectEnemyShipInStar();
                        ManeuverFlagship();
                        if (Order == aShip::soNone) {
                            TryCollectBestFloatingItem(50);
                        }
                        if (Order == aShip::soNone) {
                            if (aMyFunction::NextRandomIntRange(1, 100, RandomState) > 40) {
                                Station = FindNearestDockableStation(static_cast<aShip::TStationStandingMask>(AnyStationMask));
                                if (Station != nullptr) {
                                    OrderLanding(Station, false);
                                }
                            }
                            if (Order == aShip::soNone && static_cast<std::uint8_t>(NavigateToHomePlanet() ^ 1)) {
                                MoveToRandomPatrolPoint();
                            }
                        }
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TWarrior.NextDayLogic ", GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    std::uint8_t TWarrior::NavigateToHomePlanet() {
        if (HomePlanet->CurrentStar == CurrentStar) {
            OrderLanding(HomePlanet, false);
            return true;
        } else if (GetFuelTanks()->Fuel == GetFuelTanks()->Capacity || static_cast<long double>(JumpRange * JumpRange) >= aMyFunction::PointDistanceSquared(HomePlanet->CurrentStar->Position, CurrentStar->Position)) {
            OrderJump(HomePlanet->CurrentStar, false);
            return true;
        } else {
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
            return false;
        }
    }

    void TWarrior::BuildReachablePlanetQueue() {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        ClearPlanetQueue();
        PlanetQueue = pas::make_object<pas::List>();
        if (Speed != 0 && CurrentStar->Status.CustomFaction == u"") {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Planets) - 1); cpp_range.next(I); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                if (Planet->IsCoalitionOwned) {
                    pas::list_add(PlanetQueue, reinterpret_cast<void*>(Planet));
                }
            }
        }
    }

    // AI ownership check only; does not test travel range.
    std::uint8_t TWarrior::CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate);
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
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Equipment = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Equipment->BrokenFlag != 0 || Equipment->ConditionPercent < 3.0E+1L) {
                Equipment->Repair();
            }
        }
        if (CanRepairArtefactsAtLocation()) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
                Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
                if (Artefact->BrokenFlag != 0 || Artefact->ConditionPercent < 3.0E+1L) {
                    if (Artefact->EquippedFlag != 0) {
                        Artefact->Repair();
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
                return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"ShipType.", aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)), u".", GetTypeNameKey(), u"Big"})), Separator, Name});
            }
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

    pas::WideString TWarrior::GetTypeNameKey() {
        return u"Warrior"_w;
    }

    std::uint8_t TWarrior::GetGreetingShipCategory() {
        return aGalaxyStruct::gscWarrior;
    }

    // Always rcWarrior.
    aGalaxyStruct::TRangerCareer TWarrior::GetDominantCareer() {
        return aGalaxyStruct::rcWarrior;
    }

    std::uint8_t TWarrior::GetStrengthScaledPirateStatus() {
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
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L) {
                std::int32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(100, 500, RandomState);
                aShip::TShip* self = this;
                self->GainExperience(seededRandomIntRange, 0);
            }
            if (aPlayer::GetPlayer()->Rank > Rank && aMyFunction::NextRandomUnitFloat(RandomState) < 0.01L && (Rank < 4 || WarriorType == wtFlagship && Rank < 6)) {
                {
                    std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(10, 20, RandomState);
                    aNormalShip::TNormalShip* self_2 = this;
                    self_2->AddRankPoints(nextRandomIntRange);
                }
                TryPromoteRank();
            }
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.02L && CurrentPlanet != nullptr && (AwardIds == nullptr || 2 * (Rank + 1) > pas::list_count(AwardIds))) {
                Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}, {aGalaxyStruct::atSecretMission}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                if (Award != aGalaxyStruct::AwardNotFound) {
                    AddAward(Award);
                }
            }
        }
    }

    std::uint8_t TWarrior::RelationToNonRanger(aShip::TShip* Ship) {
        if (Ship->TypeId == aGalaxyStruct::stPirate) {
            return System::Round(pas::real_max<pas::Extended>(1.0E+1L, pas::real_min<pas::Extended>(2.0E+1L, aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f][Ship->OwnerId] * (0.5L * aConst::PlanetRaceMarket[PilotRace].PirateRelationFactor))));
        } else if (pas::is_one_of<aGalaxyStruct::stKling, aGalaxyStruct::stTranclucator>(Ship->TypeId)) {
            return 50;
        } else {
            return 100;
        }
    }

    std::uint8_t TWarrior::RelationToRanger(void* Ranger) {
        return static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(HomePlanet->RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))))));
    }

    void TWarrior::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))));
        std::uint8_t Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(HomePlanet->RangerRelations, Index)));
        if (static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aShip::psCharisma, false) > 0 && Amount > 0) {
            Amount += System::Round(Amount * (static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.2L);
        }
        std::int32_t Value = Amount + Relation;
        if (Value < 0) {
            Relation = 0;
        } else if (Value > 100) {
            Relation = 100;
        } else {
            Relation = Value;
        }
        pas::list_put(HomePlanet->RangerRelations, Index, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Relation))));
        if (Relation < 10 && (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar)) {
            EnemyShip = static_cast<aShip::TShip*>(Ranger);
        }
        if (aPlayer::GetPlayer() == Ranger) {
            if (aMyFunction::RandomIntRange(0, 100) == 0) {
                SysUtilsImports::Sleep(1u);
            }
            if (static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(HomePlanet->RangerRelations, Index))) != Relation && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
    }

    void TWarrior::ReactToAttack(aShip::TShip* Attacker) {
        EnemyShip = Attacker;
        if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), HomePlanet->OwnerId) && Attacker->TypeId == aGalaxyStruct::stRanger) {
            HomePlanet->ChangeRelationToRanger(Attacker, -3);
            if (Attacker->PartnerShip != nullptr && Attacker->PartnerShip->TypeId == aGalaxyStruct::stRanger) {
                HomePlanet->ChangeRelationToRanger(Attacker->PartnerShip, -3);
            }
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(Attacker) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip->TypeId == aGalaxyStruct::stRanger) {
                HomePlanet->ChangeRelationToRanger(reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip, -3);
            }
        }
    }

    std::uint8_t TWarrior::RecomputeFearState() {
        std::uint8_t Result{};
        if (HasNoUsableWeapons() && EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar) {
            Result = true;
            InFear = true;
        } else {
            Result = GetHull()->HullPoints < GetHull()->Weight * 0.3L && GetHull()->HullPoints < 200 || GetHull()->HullPoints < 130 || GetHull()->Weight * 0.5L * aConst::OwnerInfo[OwnerId].FearThresholdScale > GetHull()->HullPoints && EnemyShip != nullptr && EnemyShip->OrderTarget == this && ([&] {
                pas::Extended cpp_left = ChanceToWin(EnemyShip);
                return cpp_left - pas::real_divide(aConst::OwnerInfo[OwnerId].FearThresholdScale, 2.0L);
            }()) < 0.0L;
            InFear = Result;
        }
        return Result;
    }

    std::uint8_t TWarrior::AcceptsRansomDemandFrom(aShip::TShip* Ship) {
        return GetHull()->Weight * 0.5L * aConst::OwnerInfo[OwnerId].FearThresholdScale > GetHull()->HullPoints && ([&] {
            pas::Extended cpp_left = ChanceToWin(Ship);
            return cpp_left - pas::real_divide(aConst::OwnerInfo[OwnerId].FearThresholdScale, 2.0L);
        }()) < 0.0L;
    }

    std::uint8_t TWarrior::TrustsAttackRequester(aShip::TShip* Ship) {
        return RelationToShip(Ship) >= 30;
    }

    std::uint8_t TWarrior::EvaluateAllyRelationAndStrength(aShip::TShip* Ship) {
        pas::Extended cpp_right = aMyFunction::RemapClamped(Ship->Strength, 0.9L * Strength, Strength * 3.0L, 0.0, 1.0E+2);
        return (RelationToShip(Ship) & 0x0000007f) + cpp_right > 1.6E+2L;
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
            AssignFlagshipWeaponTargets();
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
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                        Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                        if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && Ship->InNormalSpace()) {
                            Distance = aMyFunction::PointDistance(Position, Ship->Position);
                            {
                                const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
                                if (1 <= cpp_last_2) {
                                    for (J = 1; J <= cpp_last_2; ++J) {
                                        Weapon = Weapons[J];
                                        if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && IsEquipmentUsable(Weapon)) {
                                            if (static_cast<long double>(GetWeaponRange(Weapon)) >= Distance) {
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
                Stage = 2;
                if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace()) {
                    SameRacePlanet = pas::class_cast_if<aPlanet::TPlanet*>(EnemyShip->OrderTarget) != nullptr && static_cast<aPlanet::TPlanet*>(EnemyShip->OrderTarget)->RaceId == HomePlanet->RaceId && ([&] {
                        std::int32_t cpp_left = pas::checked_cast<aPlanet::TPlanet*>(EnemyShip->OrderTarget)->RelationToShip(EnemyShip);
                        return cpp_left <= RelationToShip(EnemyShip);
                    }());
                    IgnoreRanger = EnemyShip->TypeId == aGalaxyStruct::stRanger && (EnemyShip->OrderTarget == HomePlanet || SameRacePlanet) && static_cast<std::uint8_t>(EnemyShip->IsAttackingShip(this) ^ 1);
                    if (IgnoreRanger) {
                        const std::int32_t cpp_last_3 = static_cast<std::int32_t>(EnemyShip->WeaponCount);
                        if (1 <= cpp_last_3) {
                            for (I = 1; I <= cpp_last_3; ++I) {
                                if (EnemyShip->Weapons[I]->Target != nullptr && pas::class_cast_if<aShip::TShip*>(EnemyShip->Weapons[I]->Target) != nullptr && !(pas::class_cast_if<aPirate::TPirate*>(EnemyShip->Weapons[I]->Target) != nullptr)) {
                                    if (GetRelationLevelToShip(pas::checked_cast<aShip::TShip*>(EnemyShip->Weapons[I]->Target)) > aGalaxyStruct::rlHostile) {
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
                        const std::int32_t cpp_last_4 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_4) {
                            for (J = 1; J <= cpp_last_4; ++J) {
                                Weapon = Weapons[J];
                                if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && IsEquipmentUsable(Weapon)) {
                                    pas::Extended cpp_right = pas::sqr(GetWeaponRange(Weapon));
                                    if (aMyFunction::PointDistanceSquared(Position, EnemyShip->Position) <= cpp_right) {
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
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    Stage = 31;
                    if (Ship->InNormalSpace() && Ship != this && EnemyShip != Ship) {
                        Stage = 32;
                        if (RelationToShip(Ship) < 10 || EnemyShip == Ship || Ship->EnemyShip == this) {
                            Stage = 33;
                            if (TruceShip != Ship) {
                                Stage = 34;
                                if (LiberationGroup == nullptr) {
                                    Stage = 35;
                                    {
                                        const std::int32_t cpp_last_5 = static_cast<std::int32_t>(WeaponCount);
                                        if (1 <= cpp_last_5) {
                                            for (J = 1; J <= cpp_last_5; ++J) {
                                                Stage = 36;
                                                Weapon = Weapons[J];
                                                if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) {
                                                    Stage = 37;
                                                    if (IsEquipmentUsable(Weapon)) {
                                                        pas::Extended cpp_right_2 = pas::sqr(GetWeaponRange(Weapon));
                                                        if (aMyFunction::PointDistanceSquared(Position, Ship->Position) <= cpp_right_2) {
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
                Stage = 4;
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Missiles) - 1); cpp_range_3.next(I); ) {
                    Missile = pas::list_at<aMissile::TMissile>(CurrentStar->Missiles, I);
                    if (Missile->Target == this && Missile->OwnerShip != this) {
                        const std::int32_t cpp_last_6 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_6) {
                            for (J = 1; J <= cpp_last_6; ++J) {
                                Weapon = Weapons[J];
                                if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && IsEquipmentUsable(Weapon)) {
                                    pas::Extended cpp_right_3 = pas::sqr(GetWeaponRange(Weapon));
                                    if (aMyFunction::PointDistanceSquared(Position, Missile->Position) <= cpp_right_3) {
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
                Stage = 5;
                if (aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Asteroids) - 1); cpp_range_4.next(I); ) {
                        Asteroid = pas::list_at<aAsteroid::TAsteroid>(CurrentStar->Asteroids, I);
                        Distance = aMyFunction::PointDistanceSquared(Position, Asteroid->Position);
                        if (Distance <= 1.0E+6L) {
                            const std::int32_t cpp_last_7 = static_cast<std::int32_t>(WeaponCount);
                            if (1 <= cpp_last_7) {
                                for (J = 1; J <= cpp_last_7; ++J) {
                                    Weapon = Weapons[J];
                                    // Native asteroid targeting can overwrite an existing assignment.
                                    if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && IsEquipmentUsable(Weapon)) {
                                        if (static_cast<long double>(pas::sqr(GetWeaponRange(Weapon))) >= Distance) {
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
                Stage = 6;
                if (aGalaxy::Galaxy->GetAIJunkToleranceLevel() * 1.5L < pas::list_count(CurrentStar->Items)) {
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range_5.next(I); ) {
                        Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                        if ((Item->ItemType == aConst::t_Minerals || Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"")) {
                            if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad || aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >= 8.0E+2L || aMyFunction::NextRandomUnitFloat(RandomState) <= 0.1L && aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >= 2.0E+2L) {
                                if (CanSafelyDetonateItem(Item)) {
                                    const std::int32_t cpp_last_8 = static_cast<std::int32_t>(WeaponCount);
                                    if (1 <= cpp_last_8) {
                                        for (J = 1; J <= cpp_last_8; ++J) {
                                            Weapon = Weapons[J];
                                            if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && IsEquipmentUsable(Weapon)) {
                                                pas::Extended cpp_right_4 = pas::sqr(GetWeaponRange(Weapon));
                                                if (aMyFunction::PointDistanceSquared(Position, Item->Position) <= cpp_right_4) {
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
                Stage = 7;
                if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace() && IsPlayerChameleonEffectiveAgainstSelf()) {
                    const std::int32_t cpp_last_9 = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last_9) {
                        for (J = 1; J <= cpp_last_9; ++J) {
                            Weapon = Weapons[J];
                            if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && IsEquipmentUsable(Weapon)) {
                                pas::Extended cpp_right_5 = pas::sqr(GetWeaponRange(Weapon));
                                if (aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) <= cpp_right_5) {
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
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TWarrior.ArmsToTarget ", GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
                } else {
                    throw;
                }
            }
        }
    }

    // Flagship branch; shares the TWarrior.ArmsToTarget diagnostic.
    void TWarrior::AssignFlagshipWeaponTargets() {
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
                const std::int32_t cpp_last = static_cast<std::int32_t>(this->WeaponCount);
                if (1 <= cpp_last) {
                    for (J = 1; J <= cpp_last; ++J) {
                        Weapon = this->Weapons[J];
                        if (IsEquipmentUsable(Weapon) && (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo > 0)) {
                            Range = GetWeaponRange(Weapon);
                            Distance = aMyFunction::PointDistance(this->Position, Ship->Position);
                            if (static_cast<long double>(Range) >= Distance) {
                                if (Score == 0.0L) {
                                    pas::Extended cpp_right = aMyFunction::RemapClamped(Distance, 0.0, Range, 1.5, 1.0);
                                    pas::Extended cpp_left = Ship->CalculateAttackStrength();
                                    Score = pas::real_divide(cpp_left, Ship->CalculateDefenseStrength()) * cpp_right;
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
                const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        Weapon = Weapons[I];
                        Weapon->Target = nullptr;
                        Scores[I] = 0.0f;
                    }
                }
            }
            Assigned = 0;
            Stage = 1;
            if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace()) {
                SameRacePlanet = pas::class_cast_if<aPlanet::TPlanet*>(EnemyShip->OrderTarget) != nullptr && static_cast<aPlanet::TPlanet*>(EnemyShip->OrderTarget)->RaceId == HomePlanet->RaceId && ([&] {
                    std::int32_t cpp_left = pas::checked_cast<aPlanet::TPlanet*>(EnemyShip->OrderTarget)->RelationToShip(EnemyShip);
                    return cpp_left <= RelationToShip(EnemyShip);
                }());
                IgnoreRanger = EnemyShip->TypeId == aGalaxyStruct::stRanger && (EnemyShip->OrderTarget == HomePlanet || SameRacePlanet) && static_cast<std::uint8_t>(EnemyShip->IsAttackingShip(this) ^ 1);
                if (!IgnoreRanger) {
                    ScoreTarget(EnemyShip);
                }
            }
            Stage = 2;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (Ship->InNormalSpace() && EnemyShip != Ship && TruceShip != Ship && LiberationGroup == nullptr && GetRelationLevelToShip(Ship) <= aGalaxyStruct::rlHostile) {
                    ScoreTarget(Ship);
                }
            }
            if (Assigned == WeaponCount) {
                return;
            }
            Stage = 4;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Missiles) - 1); cpp_range_2.next(I); ) {
                Missile = pas::list_at<aMissile::TMissile>(CurrentStar->Missiles, I);
                if (Missile->Target == this && Missile->OwnerShip != this) {
                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last_2) {
                        for (J = 1; J <= cpp_last_2; ++J) {
                            Weapon = Weapons[J];
                            if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && IsEquipmentUsable(Weapon)) {
                                pas::Extended cpp_right = pas::sqr(GetWeaponRange(Weapon));
                                if (aMyFunction::PointDistanceSquared(Position, Missile->Position) <= cpp_right) {
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
            Stage = 5;
            if (aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Asteroids) - 1); cpp_range_3.next(I); ) {
                    Asteroid = pas::list_at<aAsteroid::TAsteroid>(CurrentStar->Asteroids, I);
                    Distance = aMyFunction::PointDistanceSquared(Position, Asteroid->Position);
                    if (Distance <= 1.0E+6L) {
                        const std::int32_t cpp_last_3 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_3) {
                            for (J = 1; J <= cpp_last_3; ++J) {
                                Weapon = Weapons[J];
                                // Native asteroid targeting can overwrite an existing assignment.
                                if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && IsEquipmentUsable(Weapon)) {
                                    if (static_cast<long double>(pas::sqr(GetWeaponRange(Weapon))) >= Distance) {
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
            Stage = 6;
            if (aGalaxy::Galaxy->GetAIJunkToleranceLevel() * 1.5L < pas::list_count(CurrentStar->Items)) {
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range_4.next(I); ) {
                    Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                    if ((Item->ItemType == aConst::t_Minerals || Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") && static_cast<std::uint8_t>(AcceptPickupItem(Item) ^ 1)) {
                        if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad || aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >= 8.0E+2L || aMyFunction::NextRandomUnitFloat(RandomState) <= 0.1L && aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Item->Position) >= 2.0E+2L) {
                            if (CanSafelyDetonateItem(Item)) {
                                const std::int32_t cpp_last_4 = static_cast<std::int32_t>(WeaponCount);
                                if (1 <= cpp_last_4) {
                                    for (J = 1; J <= cpp_last_4; ++J) {
                                        Weapon = Weapons[J];
                                        if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && IsEquipmentUsable(Weapon)) {
                                            pas::Extended cpp_right_2 = pas::sqr(GetWeaponRange(Weapon));
                                            if (aMyFunction::PointDistanceSquared(Position, Item->Position) <= cpp_right_2) {
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
            Stage = 7;
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace() && IsPlayerChameleonEffectiveAgainstSelf()) {
                const std::int32_t cpp_last_5 = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last_5) {
                    for (J = 1; J <= cpp_last_5; ++J) {
                        Weapon = Weapons[J];
                        if ((static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && IsEquipmentUsable(Weapon)) {
                            pas::Extended cpp_right_3 = pas::sqr(GetWeaponRange(Weapon));
                            if (aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) <= cpp_right_3) {
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
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TWarrior.ArmsToTarget ", GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
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
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (Ship != this && pas::class_cast_if<TWarrior*>(Ship) != nullptr && static_cast<TWarrior*>(Ship)->WarriorType == wtFlagship && Ship->InNormalSpace()) {
                if (Ship->GetRelationLevelToShip(this) > aGalaxyStruct::rlHostile) {
                    Distance = aMyFunction::PointDistanceSquared(Position, Ship->Position);
                    if (Result == nullptr || Distance < BestDistance) {
                        Result = Ship;
                        BestDistance = Distance;
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
            InFlagshipRange = Ship != nullptr && static_cast<long double>(aMyFunction::PointDistanceSquared(Position, Ship->Position)) < pas::sqr(Ship->GetRadarRange());
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
                        if (IsEquipmentUsable(Weapon) && (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo > 0)) {
                            Range = GetWeaponRange(Weapon);
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
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (Ship->InNormalSpace() && RelationToShip(Ship) < 10 && TruceShip != Ship) {
                    {
                        pas::Extended cpp_right = pas::real_max<pas::Extended>(1.0L, pas::real_divide(OwnAttack, Ship->CalculateDefenseStrength()));
                        Score = Ship->CalculateAttackStrength() * cpp_right;
                    }
                    Distance = aMyFunction::PointDistance(Position, Ship->Position);
                    if (static_cast<long double>(MaxRange) > Distance) {
                        Score = static_cast<long double>(Score) * aMyFunction::RemapClamped(Distance, MinRange, MaxRange, 1.5, 1.0);
                    } else {
                        pas::Extended cpp_right_2 = aMyFunction::RemapClamped(pas::real_divide(Speed + 1, Ship->Speed + 1), 0.5, 2.0, 0.25, 1.0);
                        Score = pas::real_divide(static_cast<long double>(Score) * Distance, Speed + 1) * cpp_right_2;
                    }
                    if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) {
                        if (pas::checked_cast<aKling::TKling*>(Ship)->KlingType == aGalaxyStruct::ktBertor) {
                            Score = Score * 5.0L;
                        }
                        if (pas::checked_cast<aKling::TKling*>(Ship)->KlingType == aGalaxyStruct::ktKlig) {
                            Score = Score * 0.25L;
                        }
                    }
                    if (Score > BestScore) {
                        BestScore = Score;
                        BestShip = Ship;
                    }
                }
            }
            if (BestShip != nullptr) {
                EnemyShip = BestShip;
            }
        } else if (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar || EnemyShip->ConsecutiveDockedDays > 3) {
            if (UsableWeaponCount != 0) {
                BestChance = -1.0;
                BestDistance = 1.0;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (Ship->InNormalSpace() && RelationToShip(Ship) < 10 && TruceShip != Ship) {
                        if (pas::is_one_of<aGalaxyStruct::ssDominator, aGalaxyStruct::ssPirateMilitary>(Ship->CurrentStanding)) {
                            EnemyShip = Ship;
                            if (ChanceToWin(Ship) > 1.0L) {
                                return;
                            }
                        } else if (LiberationGroup == nullptr || aPlayer::GetPlayer() == Ship && Ship->CurrentStanding == aGalaxyStruct::ssPirateActive) {
                            Chance = ChanceToWin(Ship);
                            TravelTime = pas::real_divide(aMyFunction::PointDistance(Position, Ship->Position), Speed + 1) + 0.1L;
                            {
                                pas::Extended cpp_left = pas::real_divide(Chance, TravelTime);
                                if (cpp_left > pas::real_divide(BestChance, BestDistance)) {
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

    void TWarrior::EngageEnemyShip() {
        aShip::TShip* Ship{};
        std::uint8_t RetreatToFlagship{};
        if (Order == aShip::soFollowShip) {
            OrderNone(false);
        }
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar) {
            Ship = FindNearestFriendlyFlagship();
            RetreatToFlagship = Ship != nullptr && EnemyShip->InNormalSpace() && static_cast<std::uint8_t>(EnemyShip->HasNoUsableWeapons() ^ 1) && EnemyShip->EnemyShip == this && EnemyShip->OrderTarget == this && EnemyShip->Speed > Speed && Ship->GetRelationLevelToShip(EnemyShip) <= aGalaxyStruct::rlHostile;
            if (EnemyShip->InNormalSpace()) {
                if (RetreatToFlagship) {
                    OrderFollowShip(Ship, 0, false);
                } else {
                    OrderFollowShip(EnemyShip, 1, false);
                }
                if (ChanceToWin(EnemyShip) < 0.8L) {
                    RequestAlliesAttackShip(EnemyShip);
                }
            } else if (ChanceToWin(EnemyShip) > 0.5L) {
                if (EnemyShip->CurrentPlanet != nullptr) {
                    if (pas::class_cast_if<aRanger::TRanger*>(EnemyShip) != nullptr && static_cast<std::uint32_t>(static_cast<aRanger::TRanger*>(EnemyShip)->PrisonTermRemaining) > 0) {
                        EnemyShip = nullptr;
                        OrderNone(false);
                        return;
                    }
                    if (pas::class_cast_if<aPirate::TPirate*>(EnemyShip) != nullptr && static_cast<aPirate::TPirate*>(EnemyShip)->PrisonTermRemaining > 0) {
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
            float SquaredDistance = pas::sqr(static_cast<pas::Extended>(Point.X)) + pas::sqr(static_cast<pas::Extended>(Point.Y));
            float Radius = this->CurrentStar->SafeRadius;
            std::int32_t Attempts = 0;
            if (SquaredDistance < 0.1L) {
                while (true) {
                    {
                        std::int32_t round = System::Round(1.3L * Radius);
                        std::int32_t round_2 = System::Round(-1.3L * Radius);
                        Result.X = aMyFunction::NextRandomIntRange(round_2, round, this->RandomState);
                    }
                    {
                        std::int32_t round_3 = System::Round(1.3L * Radius);
                        std::int32_t round_4 = System::Round(-1.3L * Radius);
                        Result.Y = aMyFunction::NextRandomIntRange(round_4, round_3, this->RandomState);
                    }
                    if (pas::sqr(static_cast<pas::Extended>(Result.X)) + pas::sqr(static_cast<pas::Extended>(Result.Y)) > pas::sqr(static_cast<pas::Extended>(Radius))) {
                        break;
                    }
                    ++Attempts;
                    if (Attempts > 100) {
                        return Result;
                    }
                }
            } else if (pas::sqr(static_cast<pas::Extended>(Radius)) >= SquaredDistance) {
                SquaredDistance = pas::real_divide(Radius + 1.0L, System::Sqrt(SquaredDistance));
                Result.X = static_cast<long double>(Point.X) * SquaredDistance;
                Result.Y = static_cast<long double>(Point.Y) * SquaredDistance;
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
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Enemies) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Enemies, I);
                aWarrior::GetWeaponRangeBounds(Ship, Minimum, Maximum, this);
                ShipDistance = aMyFunction::PointDistance(Point, Ship->Position);
                {
                    pas::Extended cpp_right_2 = 0.1L * aMyFunction::RemapClamped(SupportWeight, 0.0, 2.0, 0.1, 1.0);
                    pas::Extended cpp_right = 1.0L + aWarrior::AreaWeaponScore(Ship, this) * cpp_right_2;
                    Attack = Ship->CalculateAttackStrength() * cpp_right;
                }
                Threat = static_cast<long double>(Attack) * aMyFunction::RemapClamped(ShipDistance, Minimum, Maximum, 1.0, 0.0);
                if (Attack * 0.99L > Threat) {
                    Threat = Threat + pas::real_divide((static_cast<long double>(Attack) - Threat) * Ship->Speed, static_cast<long double>(Ship->Speed) + ShipDistance - Minimum);
                }
                if (Ship->OrderTarget == this) {
                    Threat = static_cast<long double>(Threat) * aMyFunction::RemapClamped(pas::real_divide(this->Speed, std::max<std::int32_t>(1, Ship->Speed)), 0.5, 1.0, 0.0, 1.0);
                }
                IncomingStrength = static_cast<long double>(IncomingStrength) + Threat;
                Threat = static_cast<long double>(Attack) * aMyFunction::RemapClamped(ShipDistance, OwnMinRange, OwnMaxRange, 1.0, 0.0);
                if (Attack * 0.99L > Threat) {
                    Threat = Threat + pas::real_divide((static_cast<long double>(Attack) - Threat) * this->Speed, static_cast<long double>(this->Speed) + ShipDistance - OwnMinRange);
                }
                {
                    pas::Extended cpp_right_4 = 0.1L * aMyFunction::RemapClamped(SupportWeight, 0.0, 2.0, 1.0, 0.1);
                    pas::Extended cpp_right_3 = 1.0L + Ship->GetRepairStrengthFactor() * cpp_right_4;
                    Threat = pas::real_divide(Threat, pas::real_max<double>(static_cast<double>(OwnAttack), Ship->CalculateDefenseStrength()) * cpp_right_3);
                }
                if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr && static_cast<aKling::TKling*>(Ship)->KlingType == aGalaxyStruct::ktBertor) {
                    Threat = 1.5L * Threat;
                }
                if (Threat > BestEnemyPotential) {
                    BestEnemyPotential = Threat;
                }
                AttackPotential = AttackPotential + 0.1L * Threat;
            }
            AttackPotential = static_cast<long double>(AttackPotential) + BestEnemyPotential;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Allies) - 1); cpp_range_2.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Allies, I);
                aWarrior::GetWeaponRangeBounds(Ship, Minimum, Maximum, this);
                ShipDistance = aMyFunction::PointDistance(Point, Ship->Position);
                Attack = Ship->CalculateAttackStrength();
                {
                    pas::Extended cpp_left = static_cast<long double>(Attack) * aMyFunction::RemapClamped(ShipDistance, OwnMinRange, OwnMaxRange, 1.0, 0.0);
                    SupportPotential = SupportPotential + pas::real_divide(cpp_left, pas::real_max<double>(static_cast<double>(OwnAttack), Ship->CalculateDefenseStrength()));
                }
                Threat = static_cast<long double>(Attack) * aMyFunction::RemapClamped(ShipDistance, Minimum, Maximum, 1.0, 0.0);
                Attack = static_cast<long double>(Attack) - Threat;
                if (Attack > Threat * 0.01L) {
                    Threat = Threat + pas::real_divide(static_cast<long double>(Attack) * Ship->Speed, static_cast<long double>(Ship->Speed) + ShipDistance - Minimum);
                }
                IncomingStrength = IncomingStrength - 0.5L * Threat;
            }
            if (GetCargoHook() != nullptr) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(this->CurrentStar->Items) - 1); cpp_range_3.next(I); ) {
                    Item = pas::list_at<aItem::TItem>(this->CurrentStar->Items, I);
                    if (pas::class_cast_if<aItem::TProtoplasm*>(Item) != nullptr && aItem::CanCargoHookHandleItem(Item, this) && static_cast<std::uint8_t>(IsItemInPickupRange(Item) ^ 1)) {
                        Distance = System::Sqrt(pas::sqr(static_cast<long double>(Point.X) - Item->Position.X) + pas::sqr(static_cast<long double>(Point.Y) - Item->Position.Y));
                        HookRange = GetCargoHookRange();
                        if (Distance <= HookRange) {
                            NodeValue = static_cast<long double>(NodeValue) + std::min<std::int32_t>(pas::checked_cast<aItem::TProtoplasm*>(Item)->StackCount, this->CargoFreeSpace);
                        } else {
                            double cargoHookRange = GetCargoHookRange();
                            double cpp_arg = 2 * GetCargoHookRange();
                            pas::Extended cpp_right_5 = aMyFunction::RemapClamped(Distance, cargoHookRange, cpp_arg, 0.5, 0.0);
                            NodeValue = NodeValue + std::min<std::int32_t>(pas::checked_cast<aItem::TProtoplasm*>(Item)->StackCount, this->CargoFreeSpace) * cpp_right_5;
                        }
                    }
                }
            }
            if (this->PilotRace != static_cast<std::uint8_t>(aGalaxyStruct::oiFeyan)) {
                NodeValue = NodeValue * 2.0L;
            }
            if (EnemiesInRange > 0) {
                Benefit = static_cast<long double>(AttackPotential) * AttackWeight + 0.05L * NodeValue + static_cast<long double>(SupportPotential) * SupportWeight;
                Risk = pas::real_divide(IncomingStrength, OwnDefense) * FearWeight;
            } else {
                Benefit = static_cast<long double>(AttackPotential) * AttackWeight + 0.05L * NodeValue;
                Risk = 0.0f;
            }
            Distance = System::Sqrt(pas::sqr(static_cast<pas::Extended>(Point.X)) + pas::sqr(static_cast<pas::Extended>(Point.Y)));
            Benefit = static_cast<long double>(Benefit) * aMyFunction::RemapClamped(Distance, this->CurrentStar->MapDiameter * 0.7L, this->CurrentStar->MapDiameter * 1.2L, 1.0, 0.5);
            if (Distance > this->CurrentStar->MapDiameter * 1.2L) {
                Benefit = pas::real_divide(static_cast<long double>(this->CurrentStar->MapDiameter) * Benefit * 1.2L, Distance);
            }
            return static_cast<long double>(Benefit) - Risk;
        };
        auto BoostWithNodes = [&]() -> void {
            float Factor{};
            if (pas::in_set<0, 0, 2, 2, 4, 4>(this->PilotRace) && EnemiesInRange > 0) {
                {
                    pas::Extended cpp_left_2 = aMyFunction::RemapClamped(this->CargoFreeSpace, 0.0, GetHull()->Weight / 4, 0.5, 1.0);
                    pas::Extended cpp_left = cpp_left_2 * pas::real_divide(3.0L, EnemiesInRange + 2);
                    Factor = cpp_left * aMyFunction::RemapClamped(GetHull()->HullPoints, 0.0, GetHull()->Weight / 2, 0.1, 1.0);
                }
                while (5.0E+1L - 9.0E+1L * Factor > GetCombatStatusStrength(aShip::cseBWBuff) && GetCarriedNodeCount() >= static_cast<std::int64_t>(50)) {
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
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (Ship != this && Ship->InNormalSpace()) {
                if (GetRelationLevelToShip(Ship) <= aGalaxyStruct::rlHostile) {
                    pas::list_add(Enemies, reinterpret_cast<void*>(Ship));
                } else if (GetRelationLevelToShip(Ship) > aGalaxyStruct::rlNormal && (pas::class_cast_if<TWarrior*>(Ship) != nullptr || Ship->EnemyShip != nullptr && GetRelationLevelToShip(Ship->EnemyShip) <= aGalaxyStruct::rlHostile)) {
                    pas::list_add(Allies, reinterpret_cast<void*>(Ship));
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
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Enemies) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Enemies, I);
            EnemyAreaScore += aWarrior::AreaWeaponScore(Ship, this);
            EnemyRepair = static_cast<long double>(EnemyRepair) + Ship->GetRepairStrengthFactor();
            if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, Ship->Position)) < pas::sqr(OwnMaxRange)) {
                ++EnemiesInRange;
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Allies) - 1); cpp_range_3.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Allies, I);
            AllyAreaScore += aWarrior::AreaWeaponScore(Ship, this);
            AllyRepair = static_cast<long double>(AllyRepair) + Ship->GetRepairStrengthFactor();
        }
        BoostWithNodes();
        AttackWeight = 1.0f;
        SupportWeight = aMyFunction::RemapClamped(static_cast<long double>(AllyAreaScore) * EnemyRepair - static_cast<long double>(EnemyAreaScore) * AllyRepair, -1.0E+2, 1.0E+2, -2.0, 2.0);
        FearWeight = aMyFunction::RemapClamped(GetHull()->HullPoints, 0.0, GetHull()->Weight, 2.0, 0.75);
        aShip::TShip* BestShip = nullptr;
        float BestShipScore = 0.0f;
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Allies) - 1); cpp_range_4.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Allies, I);
            {
                pas::Extended cpp_right = System::Trunc(pas::real_divide(aMyFunction::PointDistance(Ship->Position, Position), Speed + 1)) + 1;
                Score = pas::real_divide(EvaluatePosition(Ship->Position), cpp_right);
            }
            if (Score > BestShipScore || BestShip == nullptr) {
                BestShipScore = Score;
                BestShip = Ship;
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Enemies) - 1); cpp_range_5.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Enemies, I);
            {
                pas::Extended cpp_right_2 = System::Trunc(pas::real_divide(aMyFunction::PointDistance(Ship->Position, Position), Speed + 1)) + 1;
                Score = pas::real_divide(EvaluatePosition(Ship->Position), cpp_right_2);
            }
            if (Score > BestShipScore || BestShip == nullptr) {
                BestShipScore = Score;
                BestShip = Ship;
            }
        }
        std::int32_t RandomDistance = aMyFunction::NextRandomIntRange(Speed / 3, Speed, RandomState);
        std::int32_t Angle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
        Destination.X = System::Cos(aMyFunction::HeadingDegreesToRadians(Angle)) * RandomDistance + Position.X;
        Destination.Y = System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * RandomDistance + Position.Y;
        Destination = ProjectOutsideStar(Destination);
        float BestPositionScore = EvaluatePosition(Destination);
        for (I = 1; I <= 40; ++I) {
            RandomDistance = aMyFunction::NextRandomIntRange(Speed / 3, Speed, RandomState);
            Angle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
            Candidate.X = System::Cos(aMyFunction::HeadingDegreesToRadians(Angle)) * RandomDistance + Position.X;
            Candidate.Y = System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * RandomDistance + Position.Y;
            Candidate = ProjectOutsideStar(Candidate);
            {
                pas::Extended cpp_right_3 = System::Trunc(pas::real_divide(aMyFunction::PointDistance(Candidate, Position), Speed + 1)) + 1;
                Score = pas::real_divide(EvaluatePosition(Candidate), cpp_right_3);
            }
            if (Score > BestPositionScore) {
                BestPositionScore = Score;
                Destination = Candidate;
            }
        }
        pas::free(Enemies);
        pas::free(Allies);
        if (BestShip != nullptr && BestShipScore >= BestPositionScore) {
            if (GetRelationLevelToShip(BestShip) > aGalaxyStruct::rlHostile) {
                OrderFollowShip(BestShip, 0, false);
            } else {
                OrderFollowShip(BestShip, 1, false);
            }
        } else {
            OrderMove(Destination, false);
        }
    }

    void GetWeaponRangeBounds(aShip::TShip* Ship, std::int32_t& Minimum, std::int32_t& Maximum, TWarrior* Self) {
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
                    if (Self->IsEquipmentUsable(Weapon) && (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo > 0)) {
                        Range = Ship->GetWeaponRange(Weapon);
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

    std::int32_t AreaWeaponScore(aShip::TShip* Ship, TWarrior* Self) {
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        std::int32_t Result = 0;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Ship->Weapons[I];
                    if (Self->IsEquipmentUsable(Weapon) && (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo > 0)) {
                        switch (static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType)) {
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
        if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) {
            switch (pas::checked_cast<aKling::TKling*>(Ship)->KlingType) {
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
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"" && Star->ConstellationGraphIndex != 20) {
                FlagshipFactor = 1.0f;
                GarrisonStrength = 0.0f;
                RacePlanetCount = 0;
                StationCount = 0;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::NonTargetableStationStandingMasks[aGalaxyStruct::sfCoalition]), Ship->CurrentStanding)) {
                        ++StationCount;
                    }
                }
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_3.next(J); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                    if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), Planet->OwnerId)) {
                        if (Planet->RaceId == PilotRace) {
                            ++RacePlanetCount;
                        }
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_4.next(K); ) {
                            Warrior = pas::list_at<TWarrior>(Planet->Warriors, K);
                            if (Warrior != this) {
                                GarrisonStrength = static_cast<long double>(GarrisonStrength) + aMyFunction::RemapClamped(Warrior->Strength, 0.1L * Strength, 1.0E+1L * Strength, 0.3, 3.0);
                                if (Warrior->WarriorType == wtFlagship) {
                                    if (static_cast<std::uint8_t>(Warrior->PilotRace + static_cast<std::uint8_t>(0)) == PilotRace) {
                                        FlagshipFactor = FlagshipFactor * 0.05L;
                                    } else {
                                        FlagshipFactor = FlagshipFactor * 0.2L;
                                    }
                                }
                            }
                        }
                    }
                }
                if (GarrisonStrength != 0.0L && RacePlanetCount != 0) {
                    NearbyThreat = 0.0f;
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) / 5); cpp_range_5.next(J); ) {
                        NearbyStar = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Star->StarDistances[J].Star));
                        if (NearbyStar->ConstellationGraphIndex != 20) {
                            if (NearbyStar->Status.CustomFaction != u"") {
                                NearbyThreat = NearbyThreat + pas::real_divide(5.0L, J + 5);
                            } else {
                                switch (NearbyStar->Status.ControlFaction) {
                                    case aGalaxyStruct::sfCoalition: break;
                                    case aGalaxyStruct::sfPirates: {
                                        NearbyThreat = NearbyThreat + pas::real_divide(1.0L, J + 5);
                                        break;
                                    }
                                    case aGalaxyStruct::sfDominators: {
                                        NearbyThreat = NearbyThreat + pas::real_divide(5.0L, J + 5);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    Score = static_cast<long double>(RacePlanetCount) * GarrisonStrength * NearbyThreat * FlagshipFactor * (StationCount + 3);
                    if (HomePlanet->CurrentStar == Star) {
                        Score = Score * 1.3L;
                    }
                    if (Score > BestScore || BestStar == nullptr) {
                        BestScore = Score;
                        BestStar = Star;
                    }
                }
            }
        }
        if (BestStar != nullptr) {
            BestScore = 0.0f;
            BestPlanet = nullptr;
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(BestStar->Planets) - 1); cpp_range_6.next(I); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(BestStar->Planets, I);
                if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), Planet->OwnerId) && Planet->RaceId == PilotRace) {
                    WarriorCount = 0;
                    FlagshipFactor = 1.0f;
                    for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_7.next(K); ) {
                        Warrior = pas::list_at<TWarrior>(Planet->Warriors, K);
                        if (Warrior != this) {
                            ++WarriorCount;
                            if (Warrior->WarriorType == wtFlagship) {
                                FlagshipFactor = FlagshipFactor * 0.1L;
                            }
                        }
                    }
                    Score = static_cast<long double>(WarriorCount) * FlagshipFactor;
                    if (HomePlanet == Planet) {
                        Score = Score * 1.3L;
                    }
                    if (Score > BestScore || BestPlanet == nullptr) {
                        BestScore = Score;
                        BestPlanet = Planet;
                    }
                }
            }
            if (BestPlanet != nullptr && reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(BestPlanet)) + 0))) != HomePlanet) {
                I = pas::list_indexof(HomePlanet->Warriors, reinterpret_cast<void*>(this));
                if (I >= 0) {
                    pas::list_delete(HomePlanet->Warriors, I);
                }
                I = pas::list_indexof(BestPlanet->Warriors, reinterpret_cast<void*>(this));
                if (I < 0) {
                    pas::list_add(BestPlanet->Warriors, reinterpret_cast<void*>(this));
                }
                HomePlanet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(BestPlanet)) + 0)));
            }
        }
    }

    void TWarrior::ProcessCombatDialogue() {
    }

    void TWarrior::ReactToExtortionDemand(void* Ranger) {
        if (aPlayer::GetPlayer() == Ranger || aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L) {
            HomePlanet->ChangeRelationToRanger(Ranger, -5);
            pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))->AddPirateCareerActivity(4);
        }
    }

    std::uint8_t TWarrior::BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        std::uint8_t Result = false;
        if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr) {
            ReactToExtortionDemand(OtherShip);
        }
        if (aPlayer::GetPlayer() != OtherShip && (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar)) {
            EnemyShip = OtherShip;
        }
        Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Money.", GetTypeNameKey(), u"No"}), OtherShip);
        return Result;
    }

    std::uint8_t TWarrior::BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) {
        std::uint8_t Result = false;
        if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr) {
            ReactToExtortionDemand(OtherShip);
        }
        if (aPlayer::GetPlayer() != OtherShip && (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar)) {
            EnemyShip = OtherShip;
        }
        Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Goods.", GetTypeNameKey(), u"No"}), OtherShip);
        return Result;
    }

    std::uint8_t TWarrior::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) {
        // Nested helper with caller-popped static link.
        auto AcceptPayment = [&]() -> void {
            std::int32_t I{};
            std::int32_t J{};
            aShip::TShip* Ship{};
            aPlanet::TPlanet* Planet{};
            aItem::TWeapon* Weapon{};
            OtherShip->SetMoney(OtherShip->Money - OfferedAmount);
            SetMoney(this->Money + OfferedAmount);
            TruceWithShip(OtherShip);
            if (pas::class_cast_if<aNormalShip::TNormalShip*>(OtherShip) != nullptr) {
                reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->CurrentSystemKills.Normal = 0;
            }
            if (pas::class_cast_if<aRanger::TRanger*>(OtherShip) != nullptr) {
                pas::checked_cast<aRanger::TRanger*>(OtherShip)->AddTraderCareerActivity(1);
                Planet = this->HomePlanet;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(this->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(this->CurrentStar->Ships, I);
                    if (pas::class_cast_if<TWarrior*>(Ship) != nullptr && static_cast<TWarrior*>(Ship)->HomePlanet == Planet && this != Ship) {
                        if (OtherShip == Ship->EnemyShip) {
                            Ship->EnemyShip = nullptr;
                        }
                        {
                            const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
                            if (1 <= cpp_last) {
                                for (J = 1; J <= cpp_last; ++J) {
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
                        if (Ship->Order == aShip::soFollowShip && pas::checked_cast<aShip::TShip*>(Ship->OrderTarget) == OtherShip) {
                            Ship->OrderNone(false);
                            Ship->NextDay();
                        }
                    }
                }
            }
        };
        std::uint8_t Result = false;
        std::int32_t NextDemandTurn = LastPlayerExtortionTurn + 30;
        if (UnknownVirtualC0(OtherShip)) {
            Response = LookupVisibleTalkText(u"Talk.Refuse.Warrior"_wref.get(), OtherShip);
            return false;
        } else if (OtherShip->TruceShip == this) {
            Response = LookupVisibleTalkText(u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && PlayerExtortionPactActive) {
            Response = LookupVisibleTalkText(u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = LookupVisibleTalkText(u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (pas::class_cast_if<aNormalShip::TNormalShip*>(OtherShip) != nullptr && OtherShip->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && OtherShip->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition && reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->CurrentSystemKills.Normal > 0) {
            if (2 * Wealth * 0.06666666666666666667L < OfferedAmount) {
                Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Ok"}), OtherShip);
                AcceptPayment();
                return true;
            }
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        } else if (Wealth * 0.06666666666666666667L < OfferedAmount) {
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Ok"}), OtherShip);
            AcceptPayment();
            return true;
        } else {
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        }
    }

    std::uint8_t TWarrior::BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        std::uint8_t Result{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        // Nested helper with caller-popped static link.
        auto AcceptRequest = [&]() -> void {
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"Ok"}), Requester);
            SetJointAttackTarget(Requester, Target);
            Result = true;
        };
        Result = false;
        if (pas::class_cast_if<aRanger::TRanger*>(Requester) != nullptr) {
            if (pas::in_range(Target->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate)) {
                Target->ChangeRelationToRanger(Requester, -20);
            }
            if (Target->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || Target->TypeId == aGalaxyStruct::stPirate) {
                pas::checked_cast<aRanger::TRanger*>(Requester)->AddWarriorCareerActivity(1);
            } else {
                pas::checked_cast<aRanger::TRanger*>(Requester)->AddPirateCareerActivity(8);
            }
        }
        std::int32_t HostileCount = 0;
        if (WarriorType == wtFlagship) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (Ship != this && Ship->InNormalSpace()) {
                    if (GetRelationLevelToShip(Ship) <= aGalaxyStruct::rlHostile) {
                        ++HostileCount;
                    }
                    if (HostileCount > 1) {
                        break;
                    }
                }
            }
        }
        if (OrderTarget == Target && GetRelationLevelToShip(Target) == aGalaxyStruct::rlHostile && HostileCount <= 1) {
            AcceptRequest();
        } else if (LiberationGroup != nullptr && Requester->LiberationGroup != LiberationGroup) {
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else if (TruceShip == Target) {
            Response = ([&] {
                pas::WideString name = Target->GetName();
                pas::WideString lookupVisibleTalkText = LookupVisibleTalkText(u"Talk.Attack.WeAlreadyHavePact"_wref.get(), Requester);
                return aMyFunction::FormatText1(std::move(lookupVisibleTalkText), u"<color=255,240,100>"_w, u"<Target>"_w, std::move(name));
            }());
        } else if (RelationToShip(Target) >= 30) {
            if (!(pas::class_cast_if<aTranclucator::TTranclucator*>(Target) != nullptr)) {
                Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"WeFriends"}), Requester);
            } else if (reinterpret_cast<aTranclucator::TTranclucator*>(Target)->OwnerShip == this) {
                Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"ItsMyTranc"}), Requester);
            } else if (reinterpret_cast<aTranclucator::TTranclucator*>(Target)->OwnerShip == Requester) {
                Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"ItsYourTranc"}), Requester);
            } else {
                Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"WeFriendsTranc"}), Requester);
            }
        } else if (AcceptsRansomDemandFrom(Target) || RecomputeFearState()) {
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"Fear"}), Requester);
        } else if (!TrustsAttackRequester(Requester)) {
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"Suspect"}), Requester);
        } else if (HasLockedOrFollowOrder() && pas::class_cast_if<aNormalShip::TNormalShip*>(Requester) != nullptr && static_cast<aNormalShip::TNormalShip*>(Requester)->Rank < Rank) {
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else if (HostileCount > 0) {
            Response = LookupVisibleTalkText(pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else {
            AcceptRequest();
        }
        return Result;
    }

    std::uint8_t TWarrior::AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    std::uint8_t TWarrior::BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
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
                            Item->Cost = System::Round(pas::real_divide(Item->Cost, Item->Weight) * RemainingWeight);
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

    std::uint8_t TWarrior::UnknownVirtualC0(void* Argument) {
        std::uint8_t Result = false;
        if (static_cast<aShip::TShip*>(Argument)->CurrentStanding == aGalaxyStruct::ssPirateMilitary) {
            return true;
        }
        if ((CurrentStar->Status.ControlFaction != aGalaxyStruct::sfCoalition || CurrentStar->Status.CustomFaction != u"") && pas::is_one_of<aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary>(static_cast<aShip::TShip*>(Argument)->CurrentStanding)) {
            return true;
        }
        return Result;
    }

    float TWarrior::AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) {
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        std::int32_t Price{};
        float DesiredMoneyFraction = 0.1f;
        float FragilityScale = 1.5f;
        if (pas::is_one_of<aConst::t_FuelTanks, aConst::t_Radar, aConst::t_CargoHook>(Item->ItemType)) {
            FragilityScale = FragilityScale * 0.5L;
        }
        float HullValueScale = 2.0f;
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
        if (OwnerId == Item->OwnerId) {
            EffectivenessScale = EffectivenessScale * 1.1L;
        }
        MoneyPenalty = MoneyPenalty * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-10, 10, Id + Seed));
        EffectivenessScale = EffectivenessScale * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-10, 10, Seed + 3 * Id));
        WeightPenalty = WeightPenalty * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-10, 10, Seed + 5 * Id));
        if (WarriorType == wtFlagship) {
            EffectivenessScale = 2.0L * EffectivenessScale;
        }
        switch (PriceMode) {
            case 4: Price = Item->Cost; break;
            case 3: Price = Item->CalculateResaleValue(GetEffectiveSkillLevel(aShip::psTrading, false)); break;
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

    float TWarrior::EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
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
                Result = 0.5L * Value * (1.0L - static_cast<std::int8_t>(WarriorType == wtFlagship) * 0.5L);
            } else if (cpp_case == aConst::bonSpeed) {
                Result = Value * 0.2L;
            } else if (cpp_case == aConst::bonJump) {
                Result = Value * 5;
            } else if (cpp_case == aConst::bonRadar) {
                Result = 0.025L * Value * (1.0L + static_cast<std::int8_t>(WarriorType == wtFlagship) * 0.5L);
            } else if (cpp_case == aConst::bonScan) {
                Result = Value * 3 + Value * 30 * (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f);
            } else if (cpp_case == aConst::bonDroid) {
                Result = pas::real_divide(Value * 10, pas::real_max<float>(0.1f, GetHull()->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags)))) * (1.0L + static_cast<std::int8_t>(WarriorType == wtFlagship) * 0.5L);
            } else if (cpp_case == aConst::bonHook) {
                Result = (pas::real_min<pas::Extended>(static_cast<pas::Extended>(Value), static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]) + Value * 0.1L) * 1.3L * static_cast<std::int8_t>(WarriorType == wtFlagship);
            } else if (cpp_case == aConst::bonDef) {
                Result = pas::real_divide(pas::real_divide(Value * 5 * 100, std::max<std::int32_t>(5, 100 - Value)) * 45.0L, std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                Result = Value * 12;
            } else if (cpp_case == aConst::bonWSplinter) {
                Result = Value * 12;
            } else if (cpp_case == aConst::bonWMissile) {
                Result = Value * 12 * (0.1L + static_cast<std::int8_t>(GetRadarRange() > 0) * 0.9L) * (1 - ((WarriorType == wtFlagship) & 0x0000007f));
            } else if (cpp_case == aConst::bonWRadius) {
                if (WarriorType == wtFlagship) {
                    Result = Value * 3;
                } else {
                    Result = Value * 1.5L * pas::sqr(pas::real_divide(std::max<std::int32_t>(100, SmoothedEnemySpeed), std::max<std::int32_t>(100, SmoothedSpeed)));
                }
            } else if (cpp_case == aConst::bonHookRadius) {
                Result = Value * 1.0L * static_cast<std::int8_t>(WarriorType == wtFlagship);
            } else if (cpp_case == aConst::bonMass) {
                Result = aMyFunction::RemapClamped(Value, aConst::HullMassEvaluationStart, aConst::HullMassEvaluationEnd, 1.0, 0.333) * 1.0E+3L;
            } else if (cpp_case == aConst::bonSlotRadar) {
                if (GetSlotCount(aConst::sskRadar) == 0 && Value > 0) {
                    Result = WarriorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (GetRadar() != nullptr && Value < 0) {
                    Result = -WarriorSlotBonusWeights[BonusKind] - WarriorSlotBonusWeights[18] * (CountMissileWeapons() & 0x0000007f);
                } else if (GetSlotCount(aConst::sskRadar) == 1 && Value < 0) {
                    Result = WarriorSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotScaner) {
                if (GetSlotCount(aConst::sskScanner) == 0 && Value > 0) {
                    Result = WarriorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (GetScanner() != nullptr && Value < 0) {
                    Result = -WarriorSlotBonusWeights[BonusKind] - (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f) * 0.1L * WarriorSlotBonusWeights[18];
                } else if (GetSlotCount(aConst::sskScanner) == 1 && Value < 0) {
                    Result = WarriorSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotDroid) {
                if (GetSlotCount(aConst::sskRepairRobot) == 0 && Value > 0) {
                    Result = WarriorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (GetRepairRobot() != nullptr && Value < 0) {
                    Result = -WarriorSlotBonusWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskRepairRobot) == 1 && Value < 0) {
                    Result = WarriorSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotDef) {
                if (GetSlotCount(aConst::sskDefGenerator) == 0 && Value > 0) {
                    Result = WarriorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (GetDefGenerator() != nullptr && Value < 0) {
                    Result = -WarriorSlotBonusWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskDefGenerator) == 1 && Value < 0) {
                    Result = WarriorSlotBonusWeights[BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotWeapon) {
                if (GetSlotCount(aConst::sskWeapon) < 5 && Value > 0) {
                    Result = std::min<std::int32_t>(Value, 5 - GetSlotCount(aConst::sskWeapon)) * WarriorSlotBonusWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -GetSlotCount(aConst::sskWeapon)) * WarriorSlotBonusWeights[BonusKind];
                }
                {
                    std::int32_t cpp_right = std::max<std::int32_t>(Value + GetSlotCount(aConst::sskWeapon), 1);
                    if ((CountEquippedWeapons() & 0x0000007f) > cpp_right) {
                        std::int32_t cpp_right_2 = std::max<std::int32_t>(1, Value + GetSlotCount(aConst::sskWeapon));
                        Result = Result - WarriorSlotBonusWeights[BonusKind] * 0.6L * ((CountEquippedWeapons() & 0x0000007f) - cpp_right_2);
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
                    Result = std::min<std::int32_t>(6 - (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f), Value) * WarriorSkillBonusWeights[BonusKind];
                }
                if (Value > 0 && Value + (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) > 6) {
                    Result = Result + WarriorSkillBonusWeights[BonusKind] * 0.05L * (Value + (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) - 6);
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f, -Value) * -WarriorSkillBonusWeights[BonusKind];
                }
                if (Value < 0 && Value + (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) < 0) {
                    Result = Result + WarriorSkillBonusWeights[BonusKind] * 0.03L * (Value + (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f));
                }
            } else {
                Result = 0.0f;
            }
        }
        if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonSkill1), static_cast<std::int32_t>(aConst::bonSkill6))) {
            pas::Extended cpp_left = Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-30, 30, Seed + 131 * BonusKind));
            return cpp_left * aConst::RaceSkillEvaluationFactors[PilotRace][aConst::EquipmentBonusSkills[BonusKind - 22]];
        }
        return Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-10, 10, Seed + 131 * BonusKind));
    }

    float TWarrior::EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> ShockFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkShock}});
        static const pas::Set<0, 255> AcidFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkAcid}});
        float ScannerFactor{};
        aGalaxyStruct::TDamageFlagSet Flags{};
        std::int32_t I{};
        std::int32_t ShotTotal{};
        Flags = Weapon->GetDamageFlags();
        if (Flags * static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags) != pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}) && GetScanner() != nullptr && GetRadar() != nullptr) {
            std::int32_t cpp_right = aItem::DefenseDamageFactorToPercent(aItem::GetGeneratedDefenseDamageFactor(aGalaxy::Galaxy->TechLevel)) & 0x0000007f;
            ScannerFactor = aMyFunction::RemapClamped(GetScannerPower() - cpp_right + 1, -5.0, 1.0E+1, 0.1, 2.0);
        } else {
            ScannerFactor = 0.0f;
        }
        float Result = static_cast<long double>(BaseDamage) * GetWeaponArtefactDamageFactor(Weapon);
        if (pas::contains(Flags, aGalaxyStruct::dkDrain)) {
            Result = Result * (1.5L - static_cast<std::int8_t>(WarriorType == wtFlagship) * 0.4L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkShock)) {
            Result = Result * (1.05L + (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ShockFlags)) & 0x0000007f) * 0.05L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
            Result = Result * 1.05L;
        }
        float StatusFactor = 1.0f;
        if (pas::contains(Flags, aGalaxyStruct::dkScanBonus)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkBonusToDamaged)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        StatusFactor = StatusFactor - 1.0L;
        if (IncludeAdditiveBonuses) {
            if (pas::contains(Flags, aGalaxyStruct::dkBlockWeapon)) {
                Result = Result + ScannerFactor * 5.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDroidBlock)) {
                Result = Result + ScannerFactor * 5.0L;
            }
            {
                std::int32_t cpp_left = CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(AcidFlags));
                Result = static_cast<long double>(Result) + cpp_left * Weapon->GetShotCount();
            }
            if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
                ShotTotal = 1;
                for (auto cpp_range = pas::for_to<std::int32_t>(1, CountEquippedWeapons() & 0x0000007f); cpp_range.next(I); ) {
                    ShotTotal += Weapons[I]->GetShotCount();
                }
                Result = static_cast<long double>(Result) + ShotTotal * 2;
            }
        }
        float SpeedFactor = pas::real_divide(std::max<std::int32_t>(100, SmoothedEnemySpeed) * GetHull()->Weight, aConst::HullBaseSize * pas::real_max<pas::Extended>(1.0E+2L, static_cast<long double>(SmoothedSpeed) * aConst::EquipmentSizeFactors[1]));
        switch (static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType)) {
            case aGalaxyStruct::wstRocket: {
                Result = Result * 1.1L * Weapon->GetShotCount() * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstMissile: {
                Result = Result * (1.1L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.0L * 0.01L + StatusFactor) * Weapon->GetShotCount();
                break;
            }
            case aGalaxyStruct::wstTorpedo: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.0L * 0.01L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstChain: {
                Result = Result * (1.1L + (Weapon->GetShotCount() - 1) * 0.2L) * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstSplash: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.5L * 0.01L * SpeedFactor + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstExploder: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.8L * 0.01L * SpeedFactor + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstAreaDamage: {
                Result = Result * (1.0L + Weapon->Range * 2.2L * 0.01L * SpeedFactor + StatusFactor);
                break;
            }
            default: Result = Result * (1.0L + StatusFactor); break;
        }
        Result = static_cast<long double>(Result) * Weapon->GetAttackCount();
        return Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Seed + Weapon->GetWeaponInfo()->TypeHash));
    }

    std::uint8_t TWarrior::AcceptPickupItem(aItem::TItem* Item) {
        std::uint8_t Result = false;
        if (WarriorType == wtFlagship) {
            if (Item->ItemType == aConst::t_Protoplasm && CargoFreeSpace > 0) {
                return true;
            }
            if (pas::in_range(PilotRace, static_cast<std::int32_t>(aGalaxyStruct::oiPeleng), static_cast<std::int32_t>(aGalaxyStruct::oiFeyan)) && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
                if (pas::checked_cast<aItem::TUselessItem*>(Item)->IsDominatorRemains()) {
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
        return static_cast<long double>(30 * Speed) >= Distance || Item->Cost >= static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(Distance, Speed), 1.0, 1.0E+1, 0.01, 0.05)) * Wealth;
    }

    void TWarrior::RefreshCurrentStanding() {
        std::int32_t StandingMode = GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            CurrentStanding = aGalaxyStruct::ssCoalitionMilitary;
        }
    }

    void TWarrior::p_destroy() {
        aWarrior::TWarrior_Destroy(this);
    }

} // namespace aWarrior
