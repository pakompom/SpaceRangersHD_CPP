#include "layout/aRuins.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/EC_Struct.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aMissile.hpp"
#include "types/aNormalShip.hpp"
#include "types/aPirate.hpp"
#include "types/aRanger.hpp"
#include "types/aTranclucator.hpp"
#include "types/aWarrior.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRuins.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fEquipmentShop.hpp"

namespace aRuins {
    using RefreshShopInventory_TQuotas = pas::Array<std::int32_t, 42, 50>;

    using CalculateEquipmentShopTargetCount_TQuotas = pas::Array<std::int32_t, 42, 50>;

    using TQuotasByItemType = pas::Array<std::int32_t, 42, 50>;

    const pas::Array<pas::Array<aGalaxyStruct::TOwnerId, 0, 1>, 6, 12> StationPilotRaces = pas::Array<pas::Array<aGalaxyStruct::TOwnerId, 0, 1>, 6, 12>{{
        pas::Array<aGalaxyStruct::TOwnerId, 0, 1>{{aGalaxyStruct::oiFeyan, aGalaxyStruct::oiGaal}},
        pas::Array<aGalaxyStruct::TOwnerId, 0, 1>{{aGalaxyStruct::oiHuman, aGalaxyStruct::oiPeleng}},
        pas::Array<aGalaxyStruct::TOwnerId, 0, 1>{{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPeleng}},
        pas::Array<aGalaxyStruct::TOwnerId, 0, 1>{{aGalaxyStruct::oiFeyan, aGalaxyStruct::oiGaal}},
        pas::Array<aGalaxyStruct::TOwnerId, 0, 1>{{aGalaxyStruct::oiHuman, aGalaxyStruct::oiHuman}},
        pas::Array<aGalaxyStruct::TOwnerId, 0, 1>{{aGalaxyStruct::oiGaal, aGalaxyStruct::oiGaal}},
        pas::Array<aGalaxyStruct::TOwnerId, 0, 1>{{aGalaxyStruct::oiFeyan, aGalaxyStruct::oiFeyan}}}};

    const pas::Array<aRuins::TStationHullGeneration, 6, 12> StationHullGeneration = pas::Array<aRuins::TStationHullGeneration, 6, 12>{{
        {.MinSize = 900, .MaxSize = 1600, .TechSizeBonus = 1500, .MinLevel = 2, .MaxLevel = 6},
        {.MinSize = 900, .MaxSize = 1000, .TechSizeBonus = 1500, .MinLevel = 2, .MaxLevel = 5},
        {.MinSize = 1200, .MaxSize = 1500, .TechSizeBonus = 1700, .MinLevel = 2, .MaxLevel = 7},
        {.MinSize = 800, .MaxSize = 1200, .TechSizeBonus = 1100, .MinLevel = 2, .MaxLevel = 7},
        {.MinSize = 800, .MaxSize = 1000, .TechSizeBonus = 1100, .MinLevel = 2, .MaxLevel = 5},
        {.MinSize = 800, .MaxSize = 1000, .TechSizeBonus = 1200, .MinLevel = 2, .MaxLevel = 5},
        {.MinSize = 900, .MaxSize = 1000, .TechSizeBonus = 1500, .MinLevel = 2, .MaxLevel = 5}}};

    const pas::Array<aRuins::TStationLevelRange, 6, 12> StationDefenseLevels = pas::Array<aRuins::TStationLevelRange, 6, 12>{{
        {.Minimum = 2, .Maximum = 6},
        {.Minimum = 2, .Maximum = 5},
        {.Minimum = 2, .Maximum = 7},
        {.Minimum = 2, .Maximum = 8},
        {.Minimum = 2, .Maximum = 5},
        {.Minimum = 2, .Maximum = 4},
        {.Minimum = 2, .Maximum = 5}}};

    const pas::Array<aRuins::TStationLevelRange, 6, 12> StationRepairLevels = pas::Array<aRuins::TStationLevelRange, 6, 12>{{
        {.Minimum = 2, .Maximum = 5},
        {.Minimum = 1, .Maximum = 4},
        {.Minimum = 2, .Maximum = 5},
        {.Minimum = 2, .Maximum = 6},
        {.Minimum = 2, .Maximum = 4},
        {.Minimum = 2, .Maximum = 3},
        {.Minimum = 1, .Maximum = 4}}};

    const pas::Array<aRuins::TStationWeaponGeneration, 6, 12> StationWeaponGeneration = pas::Array<aRuins::TStationWeaponGeneration, 6, 12>{{
        {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 450},
        {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 400},
        {.BasicLevel = 4, .IntermediateLevel = 4, .AdvancedLevel = 4, .MinimumRange = 470},
        {.BasicLevel = 4, .IntermediateLevel = 4, .AdvancedLevel = 4, .MinimumRange = 500},
        {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 300},
        {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 350},
        {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 400}}};

    const pas::Array<pas::Array<aConst::TItemType, 0, 2>, 6, 12> StationWeaponTypes = pas::Array<pas::Array<aConst::TItemType, 0, 2>, 6, 12>{{
        pas::Array<aConst::TItemType, 0, 2>{{aConst::t_Flux, aConst::t_FlowBlaster, aConst::t_AtomicVision}},
        pas::Array<aConst::TItemType, 0, 2>{{aConst::t_Flux, aConst::t_WavePhaser, aConst::t_Disintegrator}},
        pas::Array<aConst::TItemType, 0, 2>{{aConst::t_Flux, aConst::t_Multiresonator, aConst::t_FlowBlaster}},
        pas::Array<aConst::TItemType, 0, 2>{{aConst::t_Flux, aConst::t_Treton, aConst::t_Turbogravitron}},
        pas::Array<aConst::TItemType, 0, 2>{{aConst::t_FragmentationCannon, aConst::t_ElectronicCutter,
            aConst::t_WavePhaser}},
        pas::Array<aConst::TItemType, 0, 2>{{aConst::t_IndustrialLaser, aConst::t_Treton, aConst::t_WavePhaser}},
        pas::Array<aConst::TItemType, 0, 2>{{aConst::t_Flux, aConst::t_WavePhaser, aConst::t_Disintegrator}}}};

    const pas::Array<std::int32_t, 22, 27> StationSkillBonusWeights = pas::Array<std::int32_t, 22, 27>{{100, 100, 80, 0,
        0, 0}};

    const pas::Array<std::int32_t, 6, 12> StationOfferHullLevelBonus = pas::Array<std::int32_t, 6, 12>{{0, 0, 1, 0, 0,
        0, 0}};

    const pas::Array<aRuins::TStationHullTypes, 6, 12> StationOfferHullTypes = pas::Array<aRuins::TStationHullTypes, 6, 12>{{
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htPirate}, {aGalaxyStruct::htTransport}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger}, {aGalaxyStruct::htWarrior}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger, aGalaxyStruct::htDiplomat}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htTransport}, {aGalaxyStruct::htLiner}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htTransport, aGalaxyStruct::htDiplomat}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htPirate}})}};

    const pas::Array<aRuins::TStationHullTypes, 6, 12> StationOfferRareHullTypes = pas::Array<aRuins::TStationHullTypes, 6, 12>{{
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htPirate}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htWarrior}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger},
            {aGalaxyStruct::htTransport, aGalaxyStruct::htDiplomat}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htTransport}, {aGalaxyStruct::htLiner}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htTransport, aGalaxyStruct::htDiplomat}}),
        pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htPirate}})}};

    const pas::Array<std::int32_t, 6, 13> StationOfferWeaponLevelBonus = pas::Array<std::int32_t, 6, 13>{{1, 1, 2, 1, 1,
        1, 2, 1}};

    const pas::Array<pas::Array<std::int32_t, 43, 49>, 6, 13> StationOfferEquipmentLevelBonus = pas::Array<pas::Array<std::int32_t, 43, 49>, 6, 13>{{
        pas::Array<std::int32_t, 43, 49>{{1, 0, 1, 0, 0, 0, 0}},
        pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 1, 0}},
        pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 0, 0}},
        pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 1, 0, 1, 0}},
        pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 1, 0}},
        pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 1, 0}},
        pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 1, 0}},
        pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 0, 0}}}};

    void TRuins_Create(TRuins* Self) {
        aShip::TShip_Create(Self);
        Self->EquipmentShop = pas::make_object<aMyFunction::TObjectList>();
        Self->RelocationAge = 0;
        Self->FlyToStar = nullptr;
        Self->FlyDate = 0;
        Self->ModernizationSponsor = false;
        Self->SpecialServiceActive = false;
        if (aGalaxy::Galaxy != nullptr) {
            Self->RegenerateSatelliteOffer();
        }
    }

    void TRuins_Destroy(TRuins* Self) {
        std::int32_t I{};
        aRanger::TRanger* Ranger{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        aGalaxy::TStar* Star{};
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Rangers) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                    if (Ranger->LastDockedNonPlanetLocation == Self) {
                        Ranger->LastDockedNonPlanetLocation = nullptr;
                    }
                }
            }
        }
        pas::free(Self->EquipmentShop);
        Self->EquipmentShop = nullptr;
        if (Self->SatelliteOffer != nullptr) {
            pas::free(Self->SatelliteOffer);
            Self->SatelliteOffer = nullptr;
        }
        if (static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr &&
            aPlayer::GetPlayer()->RuinsProxy != Self) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"RuinsDestroyed"_w, nullptr);
            Event->AddData(Self->TypeId);
            Event->AddData(Self->Id);
            Event->AddData(Self->CurrentStar->Id);
            Event->AddTextData(Self->Name);
            Event->AddTextData(Self->TypeNameOverrideKey);
        }
        {
            const std::int32_t cpp_last_2 = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                    if (Star->Dominion == Self) {
                        Star->Dominion = nullptr;
                    }
                }
            }
        }
        aShip::TShip_Destroy(Self);
    }

    // Also registers the station in Star. Type 13 inherits ranger-center defaults.
    void TRuins::Init(aGalaxyStruct::TStationType StationType, aGalaxy::TStar* Star, pas::WideString TypeNameOverride) {
        std::int32_t I{};
        aRanger::TRanger* Ranger{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::uint8_t Good{};
        aItem::TWeapon* Weapon{};
        aItem::TCargoHook* Hook{};
        aGalaxyStruct::TOwnerId EquipmentOwner{};
        auto SelectStationName = [&](EC_BlockPar::TBlockParEC* Config) -> void {
            std::int32_t Index{};
            std::int32_t I{};
            std::int32_t J{};
            std::int32_t K{};
            std::uint8_t Used{};
            aShip::TShip* Ship{};
            aGalaxy::TStar* OtherStar{};
            if (Config == nullptr) {
                return;
            }
            if (Config->CountBlocks(aConst::ShipTypeNames[this->TypeId].Name) == 0) {
                return;
            }
            std::int32_t LastName = Config->GetBlock(
                pas::view(aConst::ShipTypeNames[this->TypeId].Name))->GetParamCount() - 1;
            Index = aMyFunction::NextRandomIntRange(0, LastName, this->RandomState);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, LastName); cpp_range.next(I); ) {
                this->Name =
                    Config->GetBlock(pas::view(aConst::ShipTypeNames[this->TypeId].Name))->GetParamValue(Index);
                Used = false;
                {
                    const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                    if (0 <= cpp_last) {
                        for (J = 0; J <= cpp_last; ++J) {
                            OtherStar = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, J);
                            {
                                const std::int32_t cpp_last_2 = pas::list_count(OtherStar->Ships) - 1;
                                if (0 <= cpp_last_2) {
                                    for (K = 0; K <= cpp_last_2; ++K) {
                                        Ship = pas::list_at<aShip::TShip>(OtherStar->Ships, K);
                                        if (Ship->TypeId == this->TypeId && Ship->Name == this->Name && Ship != this) {
                                            Used = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (Used) {
                                break;
                            }
                        }
                    }
                }
                if (!Used) {
                    break;
                }
                aMyFunction::IncrementWrapped(Index, 0, LastName);
                if (I == LastName) {
                    this->Name = pas::concat_wide({this->Name, u" ", u"-",
                            pas::wide_int_to_str(aMyFunction::NextRandomIntRange(10, 99, this->RandomState)), u"-"});
                }
            }
        };
        auto RandomStationEquipmentSize = [&](std::int32_t BaseSize) -> std::int32_t {
            return aMyFunction::NextRandomIntRange(
                pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(BaseSize) * aConst::EquipmentSizeFactors[2]) * 2.0)),
                pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(BaseSize) * aConst::EquipmentSizeFactors[1]) * 2.0)),
                this->RandomState);
        };
        if (StationType == aGalaxyStruct::rstCustomStation) {
            TypeId = aGalaxyStruct::rstRangerCenter;
        } else {
            TypeId = static_cast<aGalaxyStruct::TShipType>(StationType);
        }
        TypeNameOverrideKey = TypeNameOverride;
        CurrentStar = Star;
        pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(this));
        HomePlanet = nullptr;
        CurrentPlanet = nullptr;
        if (aGalaxy::Galaxy->CurrentTurn < aGalaxyStruct::GalaxyWarmupTurns &&
            aPlayer::GetPlayer()->CurrentStar->Constellation == Star->Constellation) {
            PilotRace = StationPilotRaces[TypeId][0];
        } else if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5) {
            PilotRace = StationPilotRaces[TypeId][0];
        } else {
            PilotRace = StationPilotRaces[TypeId][1];
        }
        OwnerId = aConst::RaceToOwner(PilotRace);
        RandomizePosition();
        Name = pas::WideString();
        SelectStationName(GR_Main::ModRuinNameConfig);
        if (GetName().length() == 0) {
            SelectStationName(GR_Main::LanguageDataConfig->GetBlock(u"RuinName"sv));
        }
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Rangers) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                    pas::list_add(RangerRelations,
                        reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(*([&] {
                            auto cpp_index = aConst::RaceToOwner(Ranger->PilotRace);
                            auto* cpp_array = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace)];
                            return &(*cpp_array)[cpp_index];
                        }())))));
                }
            }
        }
        GenerateCombatSkills();
        this->virtual_TShip_RefreshCurrentStanding();
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->RuinsProxy != this) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"RuinsCreated"_w, nullptr);
            Event->AddData(StationType);
            Event->AddData(Id);
            Event->AddData(Star->Id);
            Event->AddTextData(Name);
            Event->AddTextData(TypeNameOverride);
        }
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        RefreshShopInventory();
        for (Good = 0; Good <= 7; ++Good) {
            ShopGoods[Good].Count = pas::real_round(static_cast<float>(static_cast<double>(aConst::GoodsMarket[Good].BaseStock) *
                    aConst::StationGoodsFactors[TypeId][Good].StockFactor));
            ShopGoods[Good].PriceState = aConst::GoodsMarket[Good].AveragePrice;
            ShopGoods[Good].PurchasePrice = pas::real_round(ShopGoods[Good].PriceState);
            ShopGoods[Good].BaseSalePrice = pas::real_round(static_cast<float>(static_cast<float>(ShopGoods[Good].PriceState * 0x1.f5c28f5c28f5ep-1) - 1.0));
        }
        if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            EquipmentOwner = aGalaxyStruct::oiPirate;
        } else {
            EquipmentOwner = OwnerId;
        }
        {
            aGalaxyStruct::TOwnerId ownerId = OwnerId;
            std::uint8_t cpp_arg = CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates;
            std::int32_t cpp_left_2 = aMyFunction::NextRandomIntRange(StationHullGeneration[TypeId].MinSize,
                StationHullGeneration[TypeId].MaxSize, RandomState);
            double cpp_left = cpp_left_2 + aGalaxy::Galaxy->ScaleIntByTechLevel(0,
                    StationHullGeneration[TypeId].TechSizeBonus);
            std::uint16_t roundAndTruncateToTens = aMyFunction::RoundAndTruncateToTens(static_cast<float>(cpp_left *
                aConst::HullCapacityScale));
            std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(StationHullGeneration[TypeId].MinLevel,
                StationHullGeneration[TypeId].MaxLevel, RandomState);
            aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel = galaxy->ScaleIntByTechLevel(1, nextRandomIntRange);
            aShip::TShip* self = this;
            aShip::TShip_CreateAndEquipHull(self, roundAndTruncateToTens, scaleIntByTechLevel, ownerId, -1, cpp_arg);
        }
        {
            std::int32_t randomStationEquipmentSize = RandomStationEquipmentSize(aConst::FuelTanksBaseSize);
            aShip::TShip* self_2 = this;
            self_2->CreateAndEquipFuelTanks(randomStationEquipmentSize, 1, EquipmentOwner);
        }
        {
            std::int32_t randomStationEquipmentSize_2 = RandomStationEquipmentSize(aConst::EngineBaseSize);
            aShip::TShip* self_3 = this;
            self_3->CreateAndEquipEngine(randomStationEquipmentSize_2, 1, EquipmentOwner);
        }
        {
            std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(StationDefenseLevels[TypeId].Minimum,
                StationDefenseLevels[TypeId].Maximum, RandomState);
            aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel_2 = galaxy_2->ScaleIntByTechLevel(1, nextRandomIntRange_2);
            std::int32_t randomStationEquipmentSize_3 = RandomStationEquipmentSize(aConst::DefGeneratorBaseSize);
            aShip::TShip* self_4 = this;
            self_4->CreateAndEquipDefGenerator(randomStationEquipmentSize_3, scaleIntByTechLevel_2, EquipmentOwner);
        }
        {
            std::int32_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(StationRepairLevels[TypeId].Minimum,
                StationRepairLevels[TypeId].Maximum, RandomState);
            aGalaxy::TGalaxy* galaxy_3 = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel_3 = galaxy_3->ScaleIntByTechLevel(1, nextRandomIntRange_3);
            std::int32_t randomStationEquipmentSize_4 = RandomStationEquipmentSize(aConst::RepairRobotBaseSize);
            aShip::TShip* self_5 = this;
            self_5->CreateAndEquipRepairRobot(randomStationEquipmentSize_4, scaleIntByTechLevel_3, EquipmentOwner);
        }
        if (aConst::WeaponInfos[StationWeaponTypes[TypeId][2]].TechLevel <= aGalaxy::Galaxy->TechLevel &&
            aMyFunction::NextRandomUnitFloat(RandomState) > 0.6) {
            std::int32_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(
                StationWeaponGeneration[TypeId].AdvancedLevel + 1, 8, RandomState);
            std::int32_t advancedLevel = StationWeaponGeneration[TypeId].AdvancedLevel;
            aGalaxy::TGalaxy* galaxy_4 = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel_4 = galaxy_4->ScaleIntByTechLevel(advancedLevel, nextRandomIntRange_4);
            std::int32_t randomStationEquipmentSize_5 = RandomStationEquipmentSize(
                aConst::WeaponInfos[StationWeaponTypes[TypeId][2]].AverageSize);
            aConst::TItemType cpp_arg_2 = StationWeaponTypes[TypeId][2];
            aShip::TShip* self_6 = this;
            Weapon = self_6->CreateAndEquipWeapon(cpp_arg_2, randomStationEquipmentSize_5, scaleIntByTechLevel_4,
                    EquipmentOwner);
        } else if (aConst::WeaponInfos[StationWeaponTypes[TypeId][1]].TechLevel <= aGalaxy::Galaxy->TechLevel &&
            aMyFunction::NextRandomUnitFloat(RandomState) > 0.6) {
            std::int32_t nextRandomIntRange_5 = aMyFunction::NextRandomIntRange(
                StationWeaponGeneration[TypeId].IntermediateLevel + 1, 8, RandomState);
            std::int32_t intermediateLevel = StationWeaponGeneration[TypeId].IntermediateLevel;
            aGalaxy::TGalaxy* galaxy_5 = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel_5 = galaxy_5->ScaleIntByTechLevel(intermediateLevel, nextRandomIntRange_5);
            std::int32_t randomStationEquipmentSize_6 = RandomStationEquipmentSize(
                aConst::WeaponInfos[StationWeaponTypes[TypeId][1]].AverageSize);
            aConst::TItemType cpp_arg_3 = StationWeaponTypes[TypeId][1];
            aShip::TShip* self_7 = this;
            Weapon = self_7->CreateAndEquipWeapon(cpp_arg_3, randomStationEquipmentSize_6, scaleIntByTechLevel_5,
                    EquipmentOwner);
        } else {
            std::int32_t nextRandomIntRange_6 = aMyFunction::NextRandomIntRange(
                StationWeaponGeneration[TypeId].BasicLevel + 1, 8, RandomState);
            std::int32_t basicLevel = StationWeaponGeneration[TypeId].BasicLevel;
            aGalaxy::TGalaxy* galaxy_6 = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel_6 = galaxy_6->ScaleIntByTechLevel(basicLevel, nextRandomIntRange_6);
            std::int32_t randomStationEquipmentSize_7 = RandomStationEquipmentSize(
                aConst::WeaponInfos[StationWeaponTypes[TypeId][0]].AverageSize);
            aConst::TItemType cpp_arg_4 = StationWeaponTypes[TypeId][0];
            aShip::TShip* self_8 = this;
            Weapon = self_8->CreateAndEquipWeapon(cpp_arg_4, randomStationEquipmentSize_7, scaleIntByTechLevel_6,
                    EquipmentOwner);
        }
        Weapon->Range = std::max<std::int32_t>(Weapon->Range, StationWeaponGeneration[TypeId].MinimumRange);
        if (TypeId == aGalaxyStruct::rstDominion) {
            CurrentStar->Dominion = this;
            {
                std::int32_t nextRandomIntRange_7 = aMyFunction::NextRandomIntRange(5, 8, RandomState);
                aGalaxy::TGalaxy* galaxy_7 = aGalaxy::Galaxy;
                std::uint8_t scaleIntByTechLevel_7 = galaxy_7->ScaleIntByTechLevel(2, nextRandomIntRange_7);
                std::int32_t randomStationEquipmentSize_8 = RandomStationEquipmentSize(aConst::CargoHookBaseSize);
                aShip::TShip* self_9 = this;
                Hook = self_9->CreateAndEquipCargoHook(randomStationEquipmentSize_8, scaleIntByTechLevel_7,
                        EquipmentOwner);
            }
            Hook->Range = std::max<std::int32_t>(Hook->Range, 200);
        }
        if (GetCargoFreeSpace() < 0) {
            GetHull()->Weight += pas::abs(GetCargoFreeSpace());
        }
        RefreshDerivedStats(true);
        NodeReserve = 0;
        TypeId = static_cast<aGalaxyStruct::TShipType>(StationType);
    }

    void TRuins::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::uint8_t Good{};
        aShip::TShip::SaveToBuffer(Buffer);
        std::int32_t Count = pas::list_count(EquipmentShop);
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Item->ItemType));
            Item->SaveToBuffer(Buffer);
        }
        for (Good = 0; Good <= 7; ++Good) {
            Buffer->AddIntegerValue(ShopGoods[Good].Count);
            Buffer->AddSingle(ShopGoods[Good].PriceState);
            Buffer->AddIntegerValue(ShopGoods[Good].PurchasePrice);
            Buffer->AddIntegerValue(ShopGoods[Good].BaseSalePrice);
        }
        Buffer->AddIntegerValue(RelocationAge);
        if (FlyToStar == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(FlyToStar->Id);
        }
        Buffer->AddIntegerValue(FlyDate);
        SatelliteOffer->SaveToBuffer(Buffer);
        Buffer->AddBoolean(ModernizationSponsor);
        Buffer->AddBoolean(SpecialServiceActive);
        Buffer->AddBoolean(NoLanding);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(ShopUpdateMode));
    }

    // Requires a fresh instance. FlyToStar temporarily contains a saved star ID.
    void TRuins::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::uint8_t Good{};
        aShip::TShip::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion < 102) {
            Buffer->GetByte();
        }
        std::int32_t Count = Buffer->GetWord();
        if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
            pas::raise(pas::make_exception<pas::Abort>("Err"_a));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(Buffer->GetByte()));
            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
            Item->LoadFromBuffer(Buffer, Galaxy);
        }
        for (Good = 0; Good <= 7; ++Good) {
            ShopGoods[Good].Count = Buffer->GetInt32();
            ShopGoods[Good].PriceState = Buffer->GetSingle();
            ShopGoods[Good].PurchasePrice = Buffer->GetInt32();
            ShopGoods[Good].BaseSalePrice = Buffer->GetInt32();
        }
        if (GlobalsV::LoadedSaveVersion >= 107) {
            RelocationAge = Buffer->GetInt32();
        }
        FlyToStar =
            reinterpret_cast<aGalaxy::TStar*>(
                static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Buffer->GetUInt32())));
        FlyDate = Buffer->GetInt32();
        if (SatelliteOffer != nullptr) {
            pas::free(SatelliteOffer);
            SatelliteOffer = nullptr;
        }
        SatelliteOffer = pas::construct_call<aItem::TSatellite>(aItem::TEquipment_Create);
        SatelliteOffer->LoadFromBuffer(Buffer, Galaxy);
        SatelliteOffer->TargetPlanet = nullptr;
        ModernizationSponsor = Buffer->GetBoolean();
        SpecialServiceActive = Buffer->GetBoolean();
        NoLanding = Buffer->GetBoolean();
        if (GlobalsV::LoadedSaveVersion >= 83) {
            ShopUpdateMode = static_cast<aGalaxyStruct::TShopUpdateMode>(Buffer->GetByte());
        } else {
            ShopUpdateMode = aGalaxyStruct::sumNormal;
        }
    }

    // Includes the player's storage at this station and temporarily displayed shop stock.
    void TRuins::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        pas::WideString Text{};
        aItem::TItem* Item{};
        fEquipmentShop::TShopSlot* Slot{};
        aShip::TShip::SaveToBlock(Block);
        Text = pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[0]].Count);
        for (I = 1; I <= 7; ++I) {
            Text = pas::concat_wide({Text, u",",
                    pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].Count)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesa"_wref.get()), Text);
        Text = pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[0]].PurchasePrice);
        for (I = 1; I <= 7; ++I) {
            Text = pas::concat_wide({Text, u",",
                    pas::wide_int_to_str(
                        ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].PurchasePrice)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesaSrakloe"_wref.get()), Text);
        Text = pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[0]].BaseSalePrice);
        for (I = 1; I <= 7; ++I) {
            Text = pas::concat_wide({Text, u",",
                    pas::wide_int_to_str(
                        ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].BaseSalePrice)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesaBruhy"_wref.get()), Text);
        EC_BlockPar::TBlockParEC* ShopBlock = Block->AddBlockByPath(EC_Str::DecodeTextW(u"EdqeSahloEp"_wref.get()));
        if (EquipmentShop != nullptr && pas::list_count(EquipmentShop) > 0) {
            const std::int32_t cpp_last = pas::list_count(EquipmentShop) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
                    Text = pas::concat_wide_reverse({
                            pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))),
                            EC_Str::DecodeTextW(u"ImtreamrIodo"_wref.get())});
                    Item->SaveToBlock(ShopBlock->AddBlockByPath(Text));
                }
            }
        } else if (aPlayer::GetPlayer()->DockedTo == this && fEquipmentShop::TemporaryShopSlots != nullptr) {
            const std::int32_t cpp_last_2 = pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Slot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I);
                    Item = Slot->Item;
                    if (Item != nullptr) {
                        Text = pas::concat_wide_reverse({
                                pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))),
                                EC_Str::DecodeTextW(u"ImtreamrIodo"_wref.get())});
                        Item->SaveToBlock(ShopBlock->AddBlockByPath(Text));
                    }
                }
            }
        }
        ShopBlock->AddParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_wref.get()), u""_wref.get());
        // 'Storage'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->AddBlockByPath(
                EC_Str::DecodeTextW(u"Sataokrgalgae"_wref.get()));
            {
                const std::int32_t cpp_last_3 = pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        if (pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries,
                            I)->LocationOwner == this) {
                            Item = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I)->Item;
                            Text = pas::concat_wide_reverse({
                                    pas::wide_int64_to_str(
                                        static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))),
                                    EC_Str::DecodeTextW(u"ImtreamrIodo"_wref.get())});
                            Item->SaveToBlock(cpp_with->AddBlockByPath(Text));
                        }
                    }
                }
            }
            cpp_with->AddParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_wref.get()), u""_wref.get());
        }
    }

    // Applies edits to existing state; can append new shop and player-storage items.
    void TRuins::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        pas::WideString Text{};
        pas::WideString Name{};
        aItem::TItem* Item{};
        std::uint8_t Kind{};
        aPlayer::PStorageEntry Entry{};
        fEquipmentShop::TShopSlot* Slot{};
        aShip::TShip::LoadFromBlock(Block);
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"SihrolpaGloiordes"_wref.get())));
        for (I = 0; I <= 7; ++I) {
            ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].Count =
                SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Text), I,
                    u","sv)));
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"SihrolpaGloiordesaSrakloe"_wref.get())));
        for (I = 0; I <= 7; ++I) {
            ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].PurchasePrice =
                SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Text), I,
                    u","sv)));
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"SihrolpaGloiordesaBruhy"_wref.get())));
        for (I = 0; I <= 7; ++I) {
            ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].BaseSalePrice =
                SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Text), I,
                    u","sv)));
        }
        EC_BlockPar::TBlockParEC* ShopBlock = Block->GetBlockByPath(EC_Str::DecodeTextW(u"EdqeSahloEp"_wref.get()));
        if (EquipmentShop != nullptr && pas::list_count(EquipmentShop) > 0) {
            const std::int32_t cpp_last = pas::list_count(EquipmentShop) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
                    Text = pas::concat_wide_reverse({
                            pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))),
                            EC_Str::DecodeTextW(u"ImtreamrIodo"_wref.get())});
                    Item->virtual_TItem_LoadFromBlock(ShopBlock->GetBlockByPath(Text));
                }
            }
        } else if (aPlayer::GetPlayer()->DockedTo == this && fEquipmentShop::TemporaryShopSlots != nullptr) {
            const std::int32_t cpp_last_2 = pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Slot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I);
                    Item = Slot->Item;
                    if (Item != nullptr) {
                        Text = pas::concat_wide_reverse({
                                pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))),
                                EC_Str::DecodeTextW(u"ImtreamrIodo"_wref.get())});
                        Item->virtual_TItem_LoadFromBlock(ShopBlock->GetBlockByPath(Text));
                    }
                }
            }
        }
        Text = ShopBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"AodEdrIstaelma"_wref.get())));
        for (auto cpp_range = pas::for_to<std::int32_t>(0,
            EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) - 1); cpp_range.next(I); ) {
            Name = EC_Str::ExtractDelimitedPartW(pas::view(Text), I, u","sv);
            for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(75); ++Kind) {
                if (aConst::ItemTypeNames[static_cast<aConst::TItemType>(Kind)] == Name) {
                    if (pas::in_range(Kind, 42, 68)) {
                        Item = aItem::CreateDefaultItemByType(static_cast<aConst::TItemType>(Kind));
                        if (Item != nullptr) {
                            if (fEquipmentShop::TemporaryShopSlots != nullptr && fEquipmentShop::TemporaryShopStation ==
                                    this) {
                                fEquipmentShop::RestoreTemporaryShopStock();
                                pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                                fEquipmentShop::BuildTemporaryShopSlotGrid();
                            } else {
                                pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            }
                        }
                    }
                    break;
                }
            }
        }
        // 'Storage'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->GetBlockByPath(
                EC_Str::DecodeTextW(u"Sataokrgalgae"_wref.get()));
            {
                const std::int32_t cpp_last_3 = pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        if (pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries,
                            I)->LocationOwner == this) {
                            Item = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I)->Item;
                            Text = pas::concat_wide_reverse({
                                    pas::wide_int64_to_str(
                                        static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))),
                                    EC_Str::DecodeTextW(u"ImtreamrIodo"_wref.get())});
                            Item->virtual_TItem_LoadFromBlock(cpp_with->GetBlockByPath(Text));
                        }
                    }
                }
            }
            Text = cpp_with->GetParam(pas::view(EC_Str::DecodeTextW(u"AodEdrIstaelma"_wref.get())));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0,
                EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) - 1); cpp_range_2.next(I); ) {
                Name = EC_Str::ExtractDelimitedPartW(pas::view(Text), I, u","sv);
                for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(75); ++Kind) {
                    if (aConst::ItemTypeNames[static_cast<aConst::TItemType>(Kind)] == Name) {
                        if (pas::in_range(Kind, 0, 7) || pas::in_range(Kind, 42, 68) || pas::in_range(Kind, 10, 41) ||
                            pas::in_range(Kind, 69, 73)) {
                            Item = aItem::CreateDefaultItemByType(static_cast<aConst::TItemType>(Kind));
                            if (Item != nullptr) {
                                pas::new_value(Entry);
                                Entry->LocationOwner = this;
                                Entry->SlotIndex = aPlayer::GetPlayer()->FindNextStorageSlot(this);
                                Entry->Item = Item;
                                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                                aPlayer::GetPlayer()->RefreshStorageBubbles();
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    void TRuins_ResolveLoadedReferences(TRuins* Self, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aShip::TShip_ResolveLoadedReferences(Self, Galaxy);
        std::int32_t Count = pas::list_count(Self->EquipmentShop);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Self->EquipmentShop, I);
            Item->ResolveLoadedReferences(Galaxy);
        }
        Self->FlyToStar =
            pas::checked_cast<aGalaxy::TStar*>(
                static_cast<pas::Object*>(
                    Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->FlyToStar)))));
    }

    void TRuins_NextDay(TRuins* Self) {
        aShip::TShip_NextDay(Self);
        if (Self->ScriptShip != nullptr && Self->HasScriptControl()) {
            Self->ScriptNextDay();
            if (Self->ScriptShip != nullptr) {
                return;
            }
        }
        Self->virtual_TShip_NextDayLogic();
        if (Self->ScriptShip != nullptr && !Self->HasScriptControl()) {
            Self->ScriptNextDay();
        }
    }

    void TRuins_NextDayLogic(TRuins* Self) {
        float Imbalance{};
        float LocalBalance{};
        auto FindDominionHomeStar = [&]() -> aGalaxy::TStar* {
            std::int32_t I{};
            aGalaxy::TStar* Star{};
            aGalaxy::TStar* Result = nullptr;
            if (Self->TransitOriginStar != nullptr && Self->TransitOriginStar->Dominion == Self) {
                return Self->TransitOriginStar;
            }
            {
                const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                        if (Star->Dominion == Self) {
                            return Star;
                        }
                    }
                }
            }
            return Result;
        };
        std::int32_t Stage = 0;
        try {
            if (Self->IsDocked()) {
                Self->OrderTakeoff();
            }
            if (!Self->InNormalSpace()) {
                return;
            }
            Stage = 1;
            if (Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar != Self->CurrentStar) {
                Self->EnemyShip = nullptr;
            }
            Stage = 2;
            if (aMyFunction::NextRandomIntRange(1, 60, Self->RandomState) == 1) {
                Self->RepairBrokenEquipmentAtLocation();
                Self->ReloadWeapons();
                if (aGalaxy::Galaxy->IsStationShopUpdateEnabled() &&
                    static_cast<std::uint8_t>(Self->ModernizationSponsor ^ 1)) {
                    aShip::TShip_BuyEquipmentAtLocation(Self, false);
                    Self->RepairBrokenEquipmentAtLocation();
                }
            }
            Stage = 3;
            Self->AssignWeaponTargetsInStar();
            Stage = 4;
            Self->QueueItemsWithinPickupRange();
            Stage = 5;
            if (Self->TypeId == aGalaxyStruct::rstPirateBase) {
                Stage = 6;
                if (aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({
                    {aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) &&
                    aGalaxy::Galaxy->CurrentTurn % 60 == 0 && Self->DaysSincePlayerSeen > 30 && Self->NodeReserve <
                        std::max<std::int32_t>(500, aPlayer::GetPlayer()->TotalExperience / 10) && Self->NodeReserve <
                        static_cast<float>(2.0E+2 /
                            aConst::GalaxyDifficultyTuning[
                            aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor)) {
                    std::int32_t cpp_step = aMyFunction::NextRandomIntRange(0,
                        pas::real_round(static_cast<float>(2.5E+2 /
                            aConst::GalaxyDifficultyTuning[
                                aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor)), Self->RandomState);
                    std::int32_t& cpp_target = Self->NodeReserve;
                    cpp_target += cpp_step;
                }
            } else if (Self->TypeId == aGalaxyStruct::rstDominion) {
                Stage = 7;
                Self->AutoApplyMicroModules();
                Self->ApplyInventoryMicroModulesToShopItems();
                ++Self->RelocationAge;
                if (Self->CurrentStar->Dominion == Self) {
                    if (Self->FlyToStar != nullptr && Self->CurrentStar->Status.Battle != 0) {
                        Self->FlyToStar = nullptr;
                        Self->FlyDate = 0;
                        Self->OrderNone(false);
                    }
                    if (Self->FlyToStar == nullptr && Self->Order == aShip::soNone) {
                        Stage = 8;
                        if (static_cast<std::uint8_t>(Self->TryRelocateToPirateStar() ^ 1) &&
                            static_cast<std::uint8_t>(Self->TryStartAbductionCycle() ^ 1)) {
                            Self->TryRepositionInStar();
                        }
                    }
                } else {
                    if (Self->CurrentStar == Self->FlyToStar) {
                        Self->FlyToStar = nullptr;
                        Self->FlyDate = aGalaxy::Galaxy->CurrentTurn + 30;
                    }
                    {
                        double inline_value_3 = std::fabs(([&] {
                            double cpp_right = static_cast<float>(Self->CurrentStar->GetCachedFactionStrength(
                                aGalaxyStruct::sfDominators) * 2.5);
                            return static_cast<float>(Self->CurrentStar->GetCachedFactionStrength(aGalaxyStruct::sfCoalition) - cpp_right);
                        }()));
                        Imbalance = static_cast<float>(inline_value_3 / pas::real_max_scalar<float>(0.001f,
                                    Self->CurrentStar->GetCachedFactionStrength(aGalaxyStruct::sfPirates)));
                    }
                    LocalBalance = Self->EvaluateLocalForceBalance(Self->Position);
                    if ((LocalBalance < -1.5E+2 || Imbalance > 3.0E+1) && Self->FlyDate < aGalaxy::Galaxy->CurrentTurn +
                                25 && Self->FlyDate > aGalaxy::Galaxy->CurrentTurn) {
                        Self->FlyToStar = FindDominionHomeStar();
                        if (Self->FlyToStar != nullptr) {
                            Self->FlyDate = aGalaxy::Galaxy->CurrentTurn + 1;
                        } else {
                            Self->FlyDate = 0;
                        }
                    } else if (Imbalance > 1.5 || LocalBalance < -8.0) {
                        Self->FlyDate = std::min<std::int32_t>(Self->FlyDate,
                                std::max<std::int32_t>(Self->FlyDate - 15, aGalaxy::Galaxy->CurrentTurn + 4));
                    } else if (Imbalance > 1.37 || LocalBalance < -4.0) {
                        Self->FlyDate = std::min<std::int32_t>(Self->FlyDate,
                                std::max<std::int32_t>(Self->FlyDate - 7, aGalaxy::Galaxy->CurrentTurn + 4));
                    } else if (Imbalance > 1.25 || LocalBalance < -2.0) {
                        Self->FlyDate = std::min<std::int32_t>(Self->FlyDate,
                                std::max<std::int32_t>(Self->FlyDate - 3, aGalaxy::Galaxy->CurrentTurn + 4));
                    } else if (Imbalance > 1.12 || LocalBalance < -1.0) {
                        Self->FlyDate = std::min<std::int32_t>(Self->FlyDate,
                                std::max<std::int32_t>(Self->FlyDate - 1, aGalaxy::Galaxy->CurrentTurn + 4));
                    }
                    if (Self->FlyDate < aGalaxy::Galaxy->CurrentTurn + 3) {
                        Self->FlyToStar = FindDominionHomeStar();
                    }
                }
            }
            Stage = 9;
            if (Self->CargoFreeSpace < 0) {
                aShip::TShip_DropCargoUntilNotOverloaded(Self);
            }
            Stage = 10;
            if (aMyFunction::NextRandomIntRange(1, 17, Self->RandomState) == 1) {
                Self->RegenerateSatelliteOffer();
            }
            Stage = 11;
            Self->RefreshShopInventory();
            Stage = 12;
            Self->UpdateGoodsMarketState();
            Stage = 13;
            if (Self->FlyToStar != nullptr) {
                if (Self->CurrentStar == Self->FlyToStar) {
                    Self->FlyToStar = nullptr;
                    Self->FlyDate = 0;
                } else if (Self->FlyDate <= aGalaxy::Galaxy->CurrentTurn) {
                    EC_Struct::TPointF selectTeleportArrivalPoint = Self->SelectTeleportArrivalPoint(Self->FlyToStar);
                    aGalaxy::TStar* flyToStar = Self->FlyToStar;
                    aShip::TShip* self = Self;
                    self->OrderTeleport(flyToStar, selectTeleportArrivalPoint, 10, true);
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TRuins.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ",
                    pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    pas::WideString TRuins::GetName() {
        return Name;
    }

    pas::WideString TRuins::GetFullName(const pas::WideString& Separator) {
        pas::WideString Caption{};
        if (TypeNameOverrideKey == u"") {
            return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", GetTypeNameKey()})),
                Separator, Name});
        }
        Caption = aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", TypeNameOverrideKey}));
        if (Caption != u"") {
            return pas::concat_wide({Caption, Separator, Name});
        }
        return Name;
    }

    // Uses the station type's alternate localized caption and colors the individual name.
    pas::WideString TRuins::GetColoredFullName(const pas::WideString& ColorTag) {
        if (TypeNameOverrideKey == u"") {
            return pas::concat_wide({
                aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", GetTypeNameKey(), u"Small"})), u" ",
                aMyFunction::WrapTextInColor(pas::view(Name), pas::view(ColorTag))});
        }
        return pas::concat_wide({
            aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", TypeNameOverrideKey, u"Small"})), u" ",
            aMyFunction::WrapTextInColor(pas::view(Name), pas::view(ColorTag))});
    }

    aGalaxyStruct::TGreetingShipCategory TRuins::GetGreetingShipCategory() {
        return aGalaxyStruct::gscTransport;
    }

    // Always rcTrader.
    aGalaxyStruct::TRangerCareer TRuins::GetDominantCareer() {
        return aGalaxyStruct::rcTrader;
    }

    aGalaxy::TStar* TRuins::GetHomeStar() {
        return nullptr;
    }

    aGalaxyStruct::TPercent TRuins::GetStrengthScaledPirateStatus() {
        return 0;
    }

    // Always zero.
    std::int32_t TRuins::GetDesiredCargoFreeSpace() {
        return 0;
    }

    // Invalidates TargetPlanet on the existing offer.
    void TRuins::RegenerateSatelliteOffer() {
        if (SatelliteOffer == nullptr) {
            SatelliteOffer = pas::construct_call<aItem::TSatellite>(aItem::TEquipment_Create);
        }
        std::int32_t Level = aMyFunction::NextRandomIntRange(0,
            pas::real_round(aGalaxy::Galaxy->ScaleIntByTechLevel(1, 5)), RandomState);
        {
            std::uint32_t randomState = RandomState;
            aGalaxyStruct::TOwnerId pickRandomEquipmentOwner = aConst::PickRandomEquipmentOwner(RandomState);
            aItem::TSatellite* satelliteOffer = SatelliteOffer;
            satelliteOffer->InitGenerated(Level, pickRandomEquipmentOwner, randomState);
        }
        SatelliteOffer->TargetPlanet = nullptr;
    }

    void TRuins::RefuelAtLocation() {
        if (GetFuelTanks() != nullptr) {
            GetFuelTanks()->Fuel = GetFuelTanks()->Capacity;
        }
    }

    // Sponsored stations restore equipment condition without a repair transaction.
    void TRuins::RepairBrokenEquipmentAtLocation() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TEquipment* Artefact{};
        {
            const std::int32_t cpp_last = pas::list_count(Inventory) - 1;
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); (!(weapon != nullptr) ||
                        weapon->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) &&
                        aShip::TShip_CanRepairEquipmentTech(this, Item) && (Item->BrokenFlag != 0 ||
                            Item->ConditionPercent < 1.0)) {
                        Item->BrokenFlag = 0;
                        if (ModernizationSponsor) {
                            Item->ConditionPercent = 1.0;
                        } else {
                            Item->Repair();
                        }
                    }
                }
            }
        }
        {
            const std::int32_t cpp_last_2 = pas::list_count(Artefacts) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Artefact = pas::list_at<aItem::TEquipment>(Artefacts, I);
                    if (Artefact->BrokenFlag != 0 || Artefact->ConditionPercent < 1.0) {
                        Artefact->BrokenFlag = 0;
                        if (ModernizationSponsor) {
                            Artefact->ConditionPercent = 1.0;
                        } else {
                            Artefact->Repair();
                        }
                    }
                }
            }
        }
    }

    // Sponsored stations gain only one round per weapon; other stations refill to capacity.
    void TRuins::ReloadWeapons() {
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Weapons[I];
                    if (pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                        static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                        static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Ammo < Weapon->AmmoCapacity) {
                        if (ModernizationSponsor) {
                            ++Weapon->Ammo;
                        } else {
                            Weapon->Ammo = Weapon->AmmoCapacity;
                        }
                    }
                }
            }
        }
    }

    // Disabled in modes 1 and 3; otherwise follows the station's weekly schedule after initial setup.
    void TRuins::RefreshShopInventory() {
        std::int32_t I{};
        std::int32_t Attempts{};
        aItem::TEquipment* Item{};
        aConst::TItemType Kind{};
        if (pas::is_one_of<aGalaxyStruct::sumDisabled, aGalaxyStruct::sumGoodsOnly>(ShopUpdateMode)) {
            return;
        }
        if (aGalaxy::Galaxy->CurrentTurn > CreationTurn + 1 && static_cast<std::int32_t>(Seed +
                static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 7 != 0) {
            return;
        }
        std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(CurrentStar->Planets) - 1,
            RandomState);
        aMyFunction::TObjectList* planets = CurrentStar->Planets;
        aPlanet::TPlanet* Planet = pas::list_at<aPlanet::TPlanet>(planets, nextRandomIntRange);
        if (Planet->OwnerId == aGalaxyStruct::oiUninhabited) {
            Planet = nullptr;
        }
        {
            std::int32_t cpp_left = pas::list_count(EquipmentShop);
            if (cpp_left >= CalculateEquipmentShopTargetCount()) {
                if (Planet != nullptr) {
                    I = aMyFunction::SeededRandomIntRange(0, pas::list_count(EquipmentShop) - 1,
                            Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn));
                    Item = pas::list_at<aItem::TEquipment>(EquipmentShop, I);
                    if (Item->ScriptItem == nullptr ||
                        reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") {
                        pas::list_delete(EquipmentShop, I);
                        pas::free(Item);
                    }
                }
            }
        }
        std::int32_t Added = 0;
        while (([&] {
            double cpp_left_2 = pas::list_count(EquipmentShop);
            return cpp_left_2 <= static_cast<float>(CalculateEquipmentShopTargetCount() * 0x1.6666666666665p-1);
        }()) || ([&] {
                std::int32_t cpp_left_3 = pas::list_count(EquipmentShop);
                return cpp_left_3 <= CalculateEquipmentShopTargetCount();
            }()) && Planet != nullptr) {
            if (Planet == nullptr) {
                Planet = FindFirstInhabitedPlanetInStar();
            }
            ++Added;
            if (Added > 10) {
                break;
            }
            Attempts = 0;
            do {
                ++Attempts;
                Kind = static_cast<aConst::TItemType>(aMyFunction::SeededRandomIntRange(aConst::t_Hull,
                        aConst::WeaponCategoryItemType,
                        Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) * 175 +
                            static_cast<std::uint32_t>(Attempts)));
            } while (!(Attempts > 20 || CountEquipmentShopItems(Kind) <
                    pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::StationEquipmentOfferQuotas[TypeId],
                        (Kind - 42) * sizeof(std::int32_t)))));
            Item = aRuins::TRuins_GenerateEquipmentOffer(this, aPlayer::GetPlayer(), Planet, Kind);
            if (Item != nullptr) {
                pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                RemoveSimilarShopItem(Item);
            }
        }
    }

    // Returns 10..18; advances the station RNG state.
    std::int32_t TRuins::CalculateEquipmentShopTargetCount() {
        aConst::TItemType Kind{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Hull,
            aConst::WeaponCategoryItemType); cpp_range.next(Kind); ) {
            Count += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::StationEquipmentOfferQuotas[TypeId],
                    (Kind - 42) * sizeof(std::int32_t)));
        }
        std::int32_t Result = pas::real_round(Count + aMyFunction::NextRandomIntRange(-2, 2, RandomState));
        return std::max<std::int32_t>(10, std::min<std::int32_t>(Result, 18));
    }

    // Bucket 50 includes all weapon item types 50..68.
    std::int32_t TRuins::CountEquipmentShopItems(aConst::TItemType ItemType) {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::int32_t Count = 0;
        {
            const std::int32_t cpp_last = pas::list_count(EquipmentShop) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
                    // The WeaponCategoryItemType shop bucket counts every weapon subtype.
                    if (Item->ItemType == ItemType || pas::in_range(Item->ItemType,
                            static_cast<std::int32_t>(aConst::t_IndustrialLaser),
                            static_cast<std::int32_t>(aConst::t_CustomWeapon)) && ItemType ==
                                aConst::WeaponCategoryItemType) {
                        ++Count;
                    }
                }
            }
        }
        return Count;
    }

    // Excludes hulls; inclusive cost bounds. Borrowed result, nil when absent.
    aItem::TItem* TRuins::FindMostExpensiveShopItem(std::int32_t MinCost, std::int32_t MaxCost) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aItem::TItem* Best = nullptr;
        {
            const std::int32_t cpp_last = pas::list_count(EquipmentShop) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
                    if (Item->ItemType != aConst::t_Hull && Item->Cost <= MaxCost && Item->Cost >= MinCost) {
                        if (Best == nullptr) {
                            Best = Item;
                        } else if (reinterpret_cast<aItem::TEquipment*>(Item)->Cost > Best->Cost) {
                            Best = Item;
                        }
                    }
                }
            }
        }
        return Best;
    }

    // Frees at most one existing offer matching type and level; never inserts Item. May also compare size when below the target shop count.
    std::uint8_t TRuins::RemoveSimilarShopItem(aItem::TEquipment* Item) {
        std::int32_t I{};
        aItem::TEquipment* Existing{};
        std::uint8_t Result = false;
        {
            const std::int32_t cpp_last = pas::list_count(EquipmentShop) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Existing = pas::list_at<aItem::TEquipment>(EquipmentShop, I);
                    if (Existing->ItemType == Item->ItemType && Existing != Item && (Existing->ScriptItem == nullptr ||
                            reinterpret_cast<aScript::TScriptItem*>(Existing->ScriptItem)->Name == u"")) {
                        switch (Existing->ItemType) {
                            case aConst::t_Hull: {
                                std::int32_t cpp_left = pas::checked_cast<aItem::THull*>(Existing)->TechLevel;
                                if (cpp_left == pas::checked_cast<aItem::THull*>(Item)->TechLevel) {
                                    Result = true;
                                }
                                break;
                            }
                            case aConst::t_FuelTanks: {
                                std::int32_t cpp_left_2 = pas::checked_cast<aItem::TFuelTanks*>(Existing)->TechLevel;
                                if (cpp_left_2 == pas::checked_cast<aItem::TFuelTanks*>(Item)->TechLevel) {
                                    Result = true;
                                }
                                break;
                            }
                            case aConst::t_Engine: {
                                std::int32_t cpp_left_3 = pas::checked_cast<aItem::TEngine*>(Existing)->TechLevel;
                                if (cpp_left_3 == pas::checked_cast<aItem::TEngine*>(Item)->TechLevel) {
                                    Result = true;
                                }
                                break;
                            }
                            case aConst::t_Radar: {
                                std::int32_t cpp_left_4 = pas::checked_cast<aItem::TRadar*>(Existing)->TechLevel;
                                if (cpp_left_4 == pas::checked_cast<aItem::TRadar*>(Item)->TechLevel) {
                                    Result = true;
                                }
                                break;
                            }
                            case aConst::t_Scaner: {
                                std::int32_t cpp_left_5 = pas::checked_cast<aItem::TScaner*>(Existing)->TechLevel;
                                if (cpp_left_5 == pas::checked_cast<aItem::TScaner*>(Item)->TechLevel) {
                                    Result = true;
                                }
                                break;
                            }
                            case aConst::t_RepairRobot: {
                                std::int32_t cpp_left_6 = pas::checked_cast<aItem::TRepairRobot*>(Existing)->TechLevel;
                                if (cpp_left_6 == pas::checked_cast<aItem::TRepairRobot*>(Item)->TechLevel) {
                                    Result = true;
                                }
                                break;
                            }
                            case aConst::t_CargoHook: {
                                std::int32_t cpp_left_7 = pas::checked_cast<aItem::TCargoHook*>(Existing)->TechLevel;
                                if (cpp_left_7 == pas::checked_cast<aItem::TCargoHook*>(Item)->TechLevel) {
                                    Result = true;
                                }
                                break;
                            }
                            case aConst::t_DefGenerator: {
                                std::int32_t cpp_left_8 = pas::checked_cast<aItem::TDefGenerator*>(Existing)->TechLevel;
                                if (cpp_left_8 == pas::checked_cast<aItem::TDefGenerator*>(Item)->TechLevel) {
                                    Result = true;
                                }
                                break;
                            }
                            default: {
                                if (pas::in_range(Existing->ItemType,
                                    static_cast<std::int32_t>(aConst::t_IndustrialLaser),
                                    static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                                    std::int32_t cpp_left_9 = pas::checked_cast<aItem::TWeapon*>(Existing)->TechLevel;
                                    if (cpp_left_9 == pas::checked_cast<aItem::TWeapon*>(Item)->TechLevel) {
                                        Result = true;
                                    }
                                }
                                break;
                            }
                        }
                        if (Result) {
                            {
                                std::int32_t cpp_left_10 = pas::list_count(EquipmentShop);
                                if (cpp_left_10 < CalculateEquipmentShopTargetCount()) {
                                    Result = pas::abs(Existing->Weight - Item->Weight) < Existing->Weight / 5;
                                }
                            }
                            if (Result) {
                                pas::list_delete(EquipmentShop, I);
                                pas::free(Existing);
                                break;
                            }
                        }
                    }
                }
            }
        }
        return Result;
    }

    // Returns a zero-based module index or -1; advances the station RNG.
    std::int32_t TRuins::SelectEquipmentOfferSpecialMicroModule(aItem::TEquipment* Item, aPlanet::TPlanet* Planet) {
        std::int32_t I{};
        std::int32_t Candidate{};
        std::int32_t Result = -1;
        {
            double cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = pas::real_round(static_cast<float>(Planet->InventionLevels[aGalaxyStruct::piMainTech] * 100 / 8.0));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(
            aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(I); ) {
            do {
                if (!Template->SpecialOnly) {
                    break;
                }
                if (TypeNameOverrideKey != u"") {
                    if (Template->OfferStationNames != u"<Any>" && pas::pos(
                            pas::concat_wide({u"<", TypeNameOverrideKey, u">"}), Template->OfferStationNames) <= 0) {
                        break;
                    }
                } else if (!pas::contains(Template->OfferStationTypes, TypeId)) {
                    break;
                }
                if (!aItem::IsBonusCompatibleWithEquipment(I, Item)) {
                    break;
                }
                if (Template->Priority > Ceiling) {
                    break;
                }
                if (Count == 0) {
                    Minimum = Template->Priority;
                    Maximum = Template->Priority;
                } else {
                    Minimum = std::min<std::int32_t>(Minimum, static_cast<std::int32_t>(Template->Priority));
                    Maximum = std::max<std::int32_t>(Maximum, static_cast<std::int32_t>(Template->Priority));
                }
                aConst::MicroModuleCandidateIndices[Count] = I;
                ++Count;
            } while (!true);
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(pas::pointer_add(
                    reinterpret_cast<std::uint8_t*>(Template),
                    static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo)))));
        }
        if (Count > 0) {
            Minimum = std::max<std::int32_t>(0, Maximum - 40);
            for (I = 0; I <= 10; ++I) {
                Candidate = aMyFunction::NextRandomIntRange(0, Count - 1, RandomState);
                if (aConst::MicroModuleTemplates[aConst::MicroModuleCandidateIndices[Candidate]].Priority >= Minimum) {
                    Result = aConst::MicroModuleCandidateIndices[Candidate];
                    break;
                }
            }
        }
        return Result;
    }

    // Returns a zero-based module index or -1; advances the station RNG.
    std::int32_t TRuins::SelectHullOfferSpecialMicroModule(aItem::THull* Hull, aPlanet::TPlanet* Planet) {
        std::int32_t I{};
        std::int32_t Candidate{};
        std::int32_t Result = -1;
        {
            double cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = pas::real_round(static_cast<float>(Planet->InventionLevels[aGalaxyStruct::piMainTech] * 100 / 8.0));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(
            aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(I); ) {
            do {
                if (!Template->SpecialOnly) {
                    break;
                }
                if (TypeNameOverrideKey != u"") {
                    if (Template->OfferStationNames != u"<Any>" && pas::pos(
                            pas::concat_wide({u"<", TypeNameOverrideKey, u">"}), Template->OfferStationNames) <= 0) {
                        break;
                    }
                } else if (!pas::contains(Template->OfferStationTypes, TypeId)) {
                    break;
                }
                if (!aItem::IsBonusCompatibleWithHull(I, Hull)) {
                    break;
                }
                if (Template->Priority > Ceiling) {
                    break;
                }
                if (Count == 0) {
                    Minimum = Template->Priority;
                    Maximum = Template->Priority;
                } else {
                    Minimum = std::min<std::int32_t>(Minimum, static_cast<std::int32_t>(Template->Priority));
                    Maximum = std::max<std::int32_t>(Maximum, static_cast<std::int32_t>(Template->Priority));
                }
                aConst::MicroModuleCandidateIndices[Count] = I;
                ++Count;
            } while (!true);
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(pas::pointer_add(
                    reinterpret_cast<std::uint8_t*>(Template),
                    static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo)))));
        }
        if (Count > 0) {
            Minimum = std::max<std::int32_t>(0, Maximum - 40);
            for (I = 0; I <= 10; ++I) {
                Candidate = aMyFunction::NextRandomIntRange(0, Count - 1, RandomState);
                if (aConst::MicroModuleTemplates[aConst::MicroModuleCandidateIndices[Candidate]].Priority >= Minimum) {
                    Result = aConst::MicroModuleCandidateIndices[Candidate];
                    break;
                }
            }
        }
        return Result;
    }

    // Returns a zero-based module index or -1; advances the station RNG.
    std::int32_t TRuins::SelectWeaponOfferSpecialMicroModule(aItem::TWeapon* Weapon, aPlanet::TPlanet* Planet) {
        std::int32_t I{};
        std::int32_t Candidate{};
        std::int32_t Result = -1;
        {
            double cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = pas::real_round(static_cast<float>(Planet->InventionLevels[aGalaxyStruct::piMainTech] * 100 / 8.0));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(
            aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(I); ) {
            do {
                if (!Template->SpecialOnly) {
                    break;
                }
                if (TypeNameOverrideKey != u"") {
                    if (Template->OfferStationNames != u"<Any>" && pas::pos(
                            pas::concat_wide({u"<", TypeNameOverrideKey, u">"}), Template->OfferStationNames) <= 0) {
                        break;
                    }
                } else if (!pas::contains(Template->OfferStationTypes, TypeId)) {
                    break;
                }
                if (!aItem::IsBonusCompatibleWithWeapon(I, Weapon)) {
                    break;
                }
                if (Template->Priority > Ceiling) {
                    break;
                }
                if (Count == 0) {
                    Minimum = Template->Priority;
                    Maximum = Template->Priority;
                } else {
                    Minimum = std::min<std::int32_t>(Minimum, static_cast<std::int32_t>(Template->Priority));
                    Maximum = std::max<std::int32_t>(Maximum, static_cast<std::int32_t>(Template->Priority));
                }
                aConst::MicroModuleCandidateIndices[Count] = I;
                ++Count;
            } while (!true);
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(pas::pointer_add(
                    reinterpret_cast<std::uint8_t*>(Template),
                    static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo)))));
        }
        if (Count > 0) {
            Minimum = std::max<std::int32_t>(0, Maximum - 40);
            for (I = 0; I <= 10; ++I) {
                Candidate = aMyFunction::NextRandomIntRange(0, Count - 1, RandomState);
                if (aConst::MicroModuleTemplates[aConst::MicroModuleCandidateIndices[Candidate]].Priority >= Minimum) {
                    Result = aConst::MicroModuleCandidateIndices[Candidate];
                    break;
                }
            }
        }
        return Result;
    }

    // Empty.
    void TRuins::BuildReachablePlanetQueue() {
    }

    // Clears EnemyShip.
    void TRuins::SelectEnemyShipInStar() {
        EnemyShip = nullptr;
    }

    // Empty.
    void TRuins::EngageEnemyShip() {
    }

    // Targets hostile ships, incoming missiles and nearby asteroids.
    void TRuins::AssignWeaponTargetsInStar() {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        aAsteroid::TAsteroid* Asteroid{};
        float DistanceSquared{};
        aMissile::TMissile* Missile{};
        std::int32_t Assigned = 0;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Weapons[I];
                    Weapon->Target = nullptr;
                }
            }
        }
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar || aMyFunction::NextRandomUnitFloat(RandomState) > 0.7 ||
            aGalaxy::Galaxy->IsFullStationTargetingEnabled() || CurrentStar->Status.ControlFaction ==
                aGalaxyStruct::sfDominators || CurrentStar->Status.CustomFaction != u"") {
            const std::int32_t cpp_last_2 = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if ((Ship->OwnerId == aGalaxyStruct::oiDominator || aShip::TShip_RelationToShip(Ship, this) <
                            aGalaxyStruct::RelationBadMin) && Ship->InNormalSpace() &&
                        (!HasIndependentScriptFaction() || !Ship->HasIndependentScriptFaction() ||
                            reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText !=
                                reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText)) {
                        const std::int32_t cpp_last_3 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_3) {
                            for (J = 1; J <= cpp_last_3; ++J) {
                                Weapon = Weapons[J];
                                if (Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon) &&
                                    static_cast<double>(aMyFunction::PointDistanceSquared(Position, Ship->Position)) <=
                                        pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
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
        {
            const std::int32_t cpp_last_4 = pas::list_count(CurrentStar->Missiles) - 1;
            if (0 <= cpp_last_4) {
                for (I = 0; I <= cpp_last_4; ++I) {
                    Missile = pas::list_at<aMissile::TMissile>(CurrentStar->Missiles, I);
                    if (Missile->OwnerShip != this && Missile->Target == this) {
                        const std::int32_t cpp_last_5 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_5) {
                            for (J = 1; J <= cpp_last_5; ++J) {
                                Weapon = Weapons[J];
                                if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                    static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                    static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Target == nullptr &&
                                    aShip::TShip_IsEquipmentUsable(this, Weapon) &&
                                    static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                                        Missile->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
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
        if (pas::list_count(CurrentStar->Items) < 10 && aPlayer::GetPlayer()->CurrentStar == CurrentStar &&
            aPlayer::GetPlayer()->InNormalSpace()) {
            const std::int32_t cpp_last_6 = pas::list_count(CurrentStar->Asteroids) - 1;
            if (0 <= cpp_last_6) {
                for (I = 0; I <= cpp_last_6; ++I) {
                    Asteroid = pas::list_at<aAsteroid::TAsteroid>(CurrentStar->Asteroids, I);
                    DistanceSquared = aMyFunction::PointDistanceSquared(Position, Asteroid->Position);
                    if (DistanceSquared <=
                        pas::constant(static_cast<double>(aGalaxyStruct::AsteroidTargetRangeSquared))) {
                        const std::int32_t cpp_last_7 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_7) {
                            for (J = 1; J <= cpp_last_7; ++J) {
                                Weapon = Weapons[J];
                                if ((aMyFunction::NextRandomUnitFloat(RandomState) <= 0.9 ||
                                    GetHullIntegrityPercent() <= 90) && !pas::in_range(
                                        Weapon->GetWeaponInfo()->ShotType,
                                        static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage),
                                        static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) &&
                                    aShip::TShip_IsEquipmentUsable(this, Weapon) &&
                                    static_cast<double>(pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) >=
                                        DistanceSquared) {
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

    std::uint8_t TRuins::TryStartAbductionCycle() {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aGalaxy::TStar* Star{};
        std::uint8_t Result = false;
        if (CurrentStar->Dominion != this) {
            return Result;
        }
        if (PickupTargets != nullptr && pas::list_count(PickupTargets) > 0) {
            return Result;
        }
        if (CurrentStar->Dominion != this) {
            return Result;
        }
        if (CurrentStar->Status.Battle != 0) {
            return Result;
        }
        if (RelocationAge < 135.0) {
            return Result;
        }
        {
            std::int32_t cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left <= pas::real_round(aMyFunction::RemapClamped(RelocationAge, 135.0, 675.0, 1.0E+2, 95.0))) {
                return Result;
            }
        }
        std::int32_t Count = 0;
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (Ship->AbductedByPirateClan) {
                        return Result;
                    }
                    if (Ship->InNormalSpace() && pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr &&
                        Ship->ScriptShip == nullptr && Ship->AbsoluteScriptOrder <= 0 && pas::is_one_of<aShip::soNone,
                            aShip::soMove>(Ship->Order)) {
                        ++Count;
                    }
                }
            }
        }
        if (Count < 5) {
            return Result;
        }
        Count = 0;
        {
            const std::int32_t cpp_last_2 = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (1 <= cpp_last_2) {
                for (I = 1; I <= cpp_last_2; ++I) {
                    if (CurrentStar->StarDistances[I].Distance > 40) {
                        break;
                    }
                    Star = CurrentStar->StarDistances[I].Star;
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction ==
                            u"") {
                        const std::int32_t cpp_last_3 = pas::list_count(Star->Ships) - 1;
                        if (0 <= cpp_last_3) {
                            for (J = 0; J <= cpp_last_3; ++J) {
                                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                                if (Ship->InNormalSpace() && Ship->Order == aShip::soJump &&
                                    reinterpret_cast<aGalaxy::TStar*>(Ship->OrderTarget)->Status.ControlFaction ==
                                        aGalaxyStruct::sfCoalition &&
                                    reinterpret_cast<aGalaxy::TStar*>(Ship->OrderTarget)->Status.CustomFaction == u"" &&
                                    Ship->EstimateOrderTravelTurns() >= 2 &&
                                    pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr && Ship->OwnerId !=
                                        aGalaxyStruct::oiPirate && pas::in_range(Ship->TypeId,
                                        static_cast<std::int32_t>(aGalaxyStruct::stRanger),
                                        static_cast<std::int32_t>(aGalaxyStruct::stTransport)) && Ship->ScriptShip ==
                                        nullptr && Ship->AbsoluteScriptOrder <= 0) {
                                    ++Count;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (Count > 0) {
            RelocationAge = (RelocationAge - 90) / 2;
            OrderTeleport(CurrentStar, Position, 10, true);
            return true;
        }
        return Result;
    }

    // May redirect a departing ship to this station during its abduction cycle.
    void TRuins::TryAbductDepartingShip(aShip::TShip* Ship) {
        if (CurrentStar->Dominion != this) {
            return;
        }
        if (!InHyperspace) {
            return;
        }
        if (Order != aShip::soTeleport) {
            return;
        }
        if (OrderTarget != CurrentStar) {
            return;
        }
        if (static_cast<std::uint32_t>(OrderStateData) <= 1) {
            return;
        }
        if (Ship->ScriptShip != nullptr) {
            return;
        }
        if (Ship->HasScriptControl()) {
            return;
        }
        if (Ship->CountActiveArtefacts(aConst::t_ArtGiperJump) > 0) {
            return;
        }
        if (!(pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr)) {
            return;
        }
        aNormalShip::TNormalShip* Candidate = static_cast<aNormalShip::TNormalShip*>(Ship);
        if (Candidate->AbductedByPirateClan) {
            return;
        }
        if (pas::class_cast_if<aPirate::TPirate*>(Candidate) != nullptr) {
            return;
        }
        if (pas::in_range(Candidate->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssPiratePassive),
            static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary))) {
            return;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->QuestTargetDefendShip == Candidate &&
            aMyFunction::NextRandomIntRange(1, 100, RandomState) > 70) {
            return;
        }
        if (aPlayer::GetPlayer() == Candidate) {
            if (aGalaxy::Galaxy->CurrentTurn < static_cast<float>(static_cast<float>(2.0E+2 /
                    aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor) +
                    pas::constant(static_cast<double>(aGalaxyStruct::GalaxyWarmupTurns)))) {
                return;
            }
            {
                double cpp_left = aMyFunction::NextRandomIntRange(0, 100, RandomState);
                if (static_cast<float>(cpp_left *
                    aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor) <
                    4.0E+1) {
                    return;
                }
            }
        }
        if (Candidate->TypeId == aGalaxyStruct::stTransport) {
            Candidate->AbductedByPirateClan = true;
        } else if (Candidate->TypeId == aGalaxyStruct::stRanger) {
            switch (pas::checked_cast<aRanger::TRanger*>(Candidate)->GetDominantCareer()) {
                case aGalaxyStruct::rcTrader: {
                    Candidate->AbductedByPirateClan = aMyFunction::NextRandomIntRange(1, 100, RandomState) > 30;
                    break;
                }
                case aGalaxyStruct::rcWarrior: {
                    Candidate->AbductedByPirateClan = aMyFunction::NextRandomIntRange(1, 100, RandomState) > 70;
                    break;
                }
                case aGalaxyStruct::rcPirate: {
                    Candidate->AbductedByPirateClan = aMyFunction::NextRandomIntRange(1, 100, RandomState) > 50;
                    break;
                }
            }
        }
        if (Candidate->AbductedByPirateClan) {
            Candidate->OrderTarget = CurrentStar;
            {
                std::int32_t cpp_right = aMyFunction::NextRandomIntRange(1, 10, RandomState);
                Candidate->OrderStateData = OrderStateData + cpp_right;
            }
        }
    }

    // Posts the native Pirate Clan success/failure message after the station reappears.
    void TRuins::ReportAbductionOutcome() {
        pas::WideString Text{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        std::int32_t Abducted = 0;
        std::int32_t Pirates = 0;
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (Ship->AbductedByPirateClan) {
                        ++Abducted;
                    }
                    if (Ship->InNormalSpace() && pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr &&
                        Ship->ScriptShip == nullptr && pas::is_one_of<aShip::soNone, aShip::soMove>(Ship->Order)) {
                        ++Pirates;
                    }
                }
            }
        }
        Text = pas::concat_wide({aMyFunction::TextHighlightColorTag, GetFullName(u" "_wref.get()),
                aMyFunction::EndColorTag, u"\r\n"});
        if (Abducted == 0) {
            Text = pas::concat_wide_reverse({
                    aShip::TShip_LookupTalkText(this, u"Talk.PirateClan.RuinTalkAfterAbduct.Failure"_wref.get()),
                    Text});
        } else if (Pirates == 0) {
            Text = pas::concat_wide_reverse({
                    aShip::TShip_LookupTalkText(this,
                        u"Talk.PirateClan.RuinTalkAfterAbduct.SuccessNoPirate"_wref.get()), Text});
        } else {
            Text = pas::concat_wide_reverse({
                    aShip::TShip_LookupTalkText(this, u"Talk.PirateClan.RuinTalkAfterAbduct.Success"_wref.get()),
                    Text});
        }
        {
            auto& cpp_target = Globals::AddOrUpdatePlayerBubble(Globals::pmRadio, aGalaxy::Galaxy->CurrentTurn, Text,
                u""_wref.get())->Targets[0].ShipId;
            cpp_target = Id;
        }
    }

    float TRuins::EvaluateLocalForceBalance(EC_Struct::TPointF Point) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        float RelationFactor{};
        float Result = 0.0f;
        RefreshDerivedStats(true);
        float HullFraction = pas::real_max<double>(0.01,
            static_cast<float>(static_cast<double>(GetHull()->HullPoints) / GetHull()->Weight));
        float ReferenceStrength = pas::real_max<double>(1.0,
            static_cast<float>(static_cast<float>(static_cast<double>(Strength) / (UsableWeaponCount + 7)) + static_cast<float>(static_cast<float>(aGalaxy::Galaxy->AverageRangerStrength * 0x1.999999999999bp-4) *
                    HullFraction)));
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    RelationFactor = 1.0f;
                    if (Ship->DockedTo == this && (FlyToStar == nullptr || FlyToStar == CurrentStar)) {
                        RelationFactor = 0.75f;
                    } else if (!Ship->InNormalSpace()) {
                        continue;
                    }
                    if (aShip::TShip_GetRelationLevelToShip(this, Ship) == aGalaxyStruct::rlHostile) {
                        RelationFactor = static_cast<float>(-1.0 * RelationFactor);
                    } else if (aShip::TShip_GetRelationLevelToShip(this, Ship) == aGalaxyStruct::rlExcellent) {
                        RelationFactor = static_cast<float>(1.0 * RelationFactor);
                    } else if (aShip::TShip_GetRelationLevelToShip(this, Ship) == aGalaxyStruct::rlGood) {
                        RelationFactor = static_cast<float>(0.5 * RelationFactor);
                    } else {
                        continue;
                    }
                    if (RelationFactor > 0.0) {
                        RelationFactor = static_cast<float>(static_cast<double>(RelationFactor) * HullFraction);
                    }
                    Result = static_cast<float>(Result + static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Ship->Strength) / (Ship->UsableWeaponCount + 7)) /
                                ReferenceStrength) * RelationFactor) * pas::real_min<double>(1.0,
                                static_cast<float>(static_cast<double>(Ship->Speed) / pas::real_max_scalar<double>(1.0E+2,
                                aMyFunction::PointDistance(Point, Ship->Position))))));
                }
            }
        }
        return Result;
    }

    float TRuins::EvaluateRelocationPosition(EC_Struct::TPointF Point) {
        std::int32_t I{};
        std::int32_t RangeSquared{};
        aItem::TItem* Item{};
        float Result = 0.0f;
        if (aShip::TShip_IsEquipmentUsable(this, GetCargoHook())) {
            RangeSquared = GetCargoHookRangeSquared();
            {
                const std::int32_t cpp_last = pas::list_count(CurrentStar->Items) - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                        if (aItem::CanCargoHookHandleItem(Item, this) &&
                            static_cast<double>(aMyFunction::PointDistanceSquared(Point, Item->Position)) <=
                                RangeSquared && AcceptPickupItem(Item)) {
                            Result = static_cast<float>(static_cast<double>(Result) + Item->Cost);
                        }
                    }
                }
            }
        }
        Result = static_cast<float>(static_cast<double>(Result) / std::max<std::int32_t>(10,
                    aGalaxy::Galaxy->ComputeScaledMiniMoney(aGalaxyStruct::oiHuman)));
        Result = static_cast<float>(static_cast<double>(Result) * aMyFunction::RemapClamped(RelocationAge, 3.0E+1, 9.0E+1, 0.3, 1.0));
        if (CurrentStar->Status.Battle != 0) {
            return static_cast<float>(static_cast<double>(Result) + EvaluateLocalForceBalance(Point));
        }
        return Result;
    }

    // Successful repositioning initiates a teleport.
    std::uint8_t TRuins::TryRepositionInStar() {
        static const pas::Set<0, 255> StationTypes = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::rstRangerCenter, aGalaxyStruct::rstCustomStation}});
        EC_Struct::TPointF SavedPoint{};
        EC_Struct::TPointF BestPoint{};
        float Score{};
        std::int32_t I{};
        std::uint8_t Result = false;
        if (PickupTargets != nullptr && pas::list_count(PickupTargets) > 0) {
            return Result;
        }
        if (RelocationAge < 30) {
            return Result;
        }
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (pas::list_at<aShip::TShip>(CurrentStar->Ships, I)->AbductedByPirateClan &&
                        pas::list_at<aShip::TShip>(CurrentStar->Ships, I)->InHyperspace) {
                        return Result;
                    }
                }
            }
        }
        SavedPoint = Position;
        float InitialScore = EvaluateRelocationPosition(Position);
        float BestScore = InitialScore;
        for (I = 0; I <= 100; ++I) {
            RandomizePosition();
            Score = EvaluateRelocationPosition(Position);
            if (Score >= BestScore &&
                DistanceToNearestShipByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) > 7.0E+2) {
                BestScore = Score;
                BestPoint = Position;
            }
        }
        if (InitialScore > 0.0 && BestScore < static_cast<float>(InitialScore + 1.0E+1) || BestScore < static_cast<float>(InitialScore + 5.0)) {
            Position = SavedPoint;
            return Result;
        }
        float Radius = 1.0E+2f;
        for (I = 0; I <= 100; ++I) {
            Position.X = static_cast<float>(BestPoint.X + static_cast<float>(static_cast<float>(aMyFunction::NextRandomIntRange(-100, 100, RandomState) * 0x1.47ae147ae147ap-7) * Radius));
            Position.Y = static_cast<float>(BestPoint.Y + static_cast<float>(static_cast<float>(aMyFunction::NextRandomIntRange(-100, 100, RandomState) * 0x1.47ae147ae147ap-7) * Radius));
            if (DistanceToNearestShipByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) > 7.0E+2 &&
                static_cast<float>(pas::sqr(static_cast<double>(Position.X)) + pas::sqr(static_cast<double>(Position.Y))) >=
                    static_cast<float>(pas::sqr(static_cast<double>(CurrentStar->SafeRadius)) * 0x1.199999999999bp+0)) {
                Score = EvaluateRelocationPosition(Position);
                if (Score >= BestScore) {
                    BestScore = Score;
                    BestPoint = Position;
                    Radius = static_cast<float>(Radius * 0.75);
                }
            }
        }
        Position = SavedPoint;
        RelocationAge = (RelocationAge - 30) / 2;
        OrderTeleport(CurrentStar, BestPoint, 0, true);
        return true;
    }

    // Travel starts next turn; transfers the star's Dominion ownership reference.
    std::uint8_t TRuins::TryRelocateToPirateStar() {
        std::int32_t I{};
        aGalaxy::TStar* Star{};
        std::uint8_t Result = false;
        if (CurrentStar->Status.Battle == 0) {
            return Result;
        }
        if (RelocationAge < 45) {
            return Result;
        }
        if (CurrentStar->Dominion != this) {
            return Result;
        }
        if (EvaluateLocalForceBalance(Position) > -1.0E+1) {
            return Result;
        }
        pas::List* Candidates = pas::make_object<pas::List>();
        aGalaxy::TConstellation* Constellation = CurrentStar->Constellation;
        {
            const std::int32_t cpp_last = pas::list_count(Constellation->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Star = pas::list_at<aGalaxy::TStar>(Constellation->Stars, I);
                    if (Star != CurrentStar && Star->Status.ControlFaction == aGalaxyStruct::sfPirates &&
                        Star->Status.Battle == 0 && !aScript::IsStarProtectedByScript(Star) && Star->Dominion ==
                            nullptr && Star->Status.CustomFaction == u"") {
                        pas::list_add(Candidates, reinterpret_cast<void*>(Star));
                    }
                }
            }
        }
        if (pas::list_count(Candidates) == 0) {
            pas::free(Candidates);
            return Result;
        }
        Star = pas::list_at<aGalaxy::TStar>(Candidates,
                aMyFunction::NextRandomIntRange(0, pas::list_count(Candidates) - 1, RandomState));
        pas::free(Candidates);
        CurrentStar->Dominion = nullptr;
        Star->Dominion = this;
        FlyToStar = Star;
        FlyDate = aGalaxy::Galaxy->CurrentTurn + 1;
        RelocationAge = (RelocationAge - 150) / 2;
        return true;
    }

    std::uint8_t TRuins::AcceptPickupItem(aItem::TItem* Item) {
        std::uint8_t Result = false;
        if (TypeId == aGalaxyStruct::rstDominion) {
            if (!pas::in_set<aConst::t_Food, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy,
                aConst::t_ArtGiperJump, aConst::t_ArtDefToArms1, aConst::t_CustomWeapon, aConst::t_MicroModule,
                aConst::t_MicroModule>(Item->ItemType)) {
                return Result;
            }
        } else if (!(pas::in_set<aConst::t_Food, aConst::t_Narcotics, aConst::t_Hull,
            aConst::t_CustomWeapon>(Item->ItemType) && Item->OwnerId != aGalaxyStruct::oiDominator)) {
            return Result;
        }
        return true;
    }

    // Consumes compatible modules from Inventory, skipping index 0; modifies EquipmentShop items.
    void TRuins::ApplyInventoryMicroModulesToShopItems() {
        std::int32_t I{};
        std::int32_t J{};
        aItem::TItem* Module{};
        aItem::TEquipment* Item{};
        std::uint8_t Applied = true;
        while (Applied) {
            Applied = false;
            {
                const std::int32_t cpp_last = pas::list_count(Inventory) - 1;
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        Module = pas::list_at<aItem::TItem>(Inventory, I);
                        if (aItem::TMicroModule* microModule = pas::class_cast_if<aItem::TMicroModule*>(Module)) {
                            const std::int32_t cpp_last_2 = pas::list_count(EquipmentShop) - 1;
                            if (0 <= cpp_last_2) {
                                for (J = 0; J <= cpp_last_2; ++J) {
                                    Item = pas::list_at<aItem::TEquipment>(EquipmentShop, J);
                                    if (microModule->CanInstallOn(Item)) {
                                        Applied = true;
                                        aItem::ApplyMicroModule(microModule->MicroModuleIndex - 1, Item);
                                        pas::list_delete(Inventory,
                                            pas::list_indexof(Inventory, reinterpret_cast<void*>(Module)));
                                        pas::free(Module);
                                        RefreshDerivedStats(true);
                                        break;
                                    }
                                }
                            }
                        }
                        if (Applied) {
                            break;
                        }
                    }
                }
            }
        }
    }

    std::uint8_t TRuins::RelationToNonRanger(aShip::TShip* Ship) {
        if (pas::is_one_of<aGalaxyStruct::stKling, aGalaxyStruct::stTranclucator>(Ship->TypeId)) {
            return 50;
        }
        return 100;
    }

    // Floors the stored relation at 50, except for the player when NoLanding is set.
    std::uint8_t TRuins::RelationToRanger(void* Ranger) {
        std::uint8_t Result = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations,
            pas::list_indexof(aGalaxy::Galaxy->Rangers,
                reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))))));
        if (static_cast<std::uint8_t>(NoLanding ^ 1) || aPlayer::GetPlayer() != Ranger) {
            return std::max<std::int32_t>(50, static_cast<std::int32_t>(Result));
        }
        return Result;
    }

    // Positive changes receive the ranger's Charisma bonus; stored relation is clamped to 0..100.
    void TRuins::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Rangers,
            reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))));
        std::uint8_t Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(
            RangerRelations, Index)));
        if (Amount > 0 && static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aGalaxyStruct::psCharisma, false) >
                0) {
            Amount += pas::real_round(static_cast<float>(static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(
                    aGalaxyStruct::psCharisma, false) * Amount * 0x1.999999999999bp-3));
        }
        std::int32_t NewRelation = Relation + Amount;
        if (NewRelation < 0) {
            Relation = 0;
        } else if (NewRelation > 100) {
            Relation = 100;
        } else {
            Relation = NewRelation;
        }
        pas::list_put(RangerRelations, Index,
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Relation))));
        if (Relation < aGalaxyStruct::RelationBadMin && (EnemyShip == nullptr || EnemyShip->CurrentStar !=
                    CurrentStar)) {
            EnemyShip = static_cast<aShip::TShip*>(Ranger);
        }
        if (aPlayer::GetPlayer() == Ranger) {
            if (aMyFunction::RandomIntRange(0, 100) == 0) {
                SysUtilsImports::Sleep(1u);
            }
            if (static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index))) !=
                Relation && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
    }

    void TRuins::ReactToAttack(aShip::TShip* Attacker) {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        EnemyShip = Attacker;
        if (CurrentStanding == aGalaxyStruct::ssCustom) {
            return;
        }
        std::uint8_t Independent = !pas::contains(aConst::FactionStandingMasks[CurrentStar->Status.ControlFaction],
            CurrentStanding) || CurrentStar->Status.CustomFaction != u"";
        if (Attacker->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(Attacker, -10);
            if (Independent) {
                return;
            }
            if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && aPlanet::MainPiratePlanet !=
                    nullptr) {
                aPlanet::MainPiratePlanet->ChangeRelationToRanger(Attacker, -10);
            } else if (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfDominators) {
                const std::int32_t cpp_last = pas::list_count(CurrentStar->Planets) - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                        if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                            Planet->ChangeRelationToRanger(Attacker, -10);
                        }
                    }
                }
            }
        }
        if (Attacker->PartnerShip != nullptr && Attacker->PartnerShip->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(Attacker->PartnerShip, -5);
            if (Independent) {
                return;
            }
            if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && aPlanet::MainPiratePlanet !=
                    nullptr) {
                aPlanet::MainPiratePlanet->ChangeRelationToRanger(Attacker->PartnerShip, -5);
            } else if (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfDominators) {
                const std::int32_t cpp_last_2 = pas::list_count(CurrentStar->Planets) - 1;
                if (0 <= cpp_last_2) {
                    for (I = 0; I <= cpp_last_2; ++I) {
                        Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                        if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                            Planet->ChangeRelationToRanger(Attacker->PartnerShip, -5);
                        }
                    }
                }
            }
        }
        if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(
            Attacker); tranclucator != nullptr && tranclucator->OwnerShip != nullptr &&
            tranclucator->OwnerShip->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(tranclucator->OwnerShip, -10);
            if (Independent) {
                return;
            }
            if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && aPlanet::MainPiratePlanet !=
                    nullptr) {
                aPlanet::MainPiratePlanet->ChangeRelationToRanger(
                    reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip, -10);
            } else if (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfDominators) {
                const std::int32_t cpp_last_3 = pas::list_count(CurrentStar->Planets) - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                        if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                            Planet->ChangeRelationToRanger(
                                reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip, -10);
                        }
                    }
                }
            }
        }
        if (static_cast<std::uint8_t>(Independent ^ 1) && pas::class_cast_if<aNormalShip::TNormalShip*>(Attacker) !=
                nullptr && pas::load_unaligned<std::uint16_t>(pas::byte_offset(
                &static_cast<aNormalShip::TNormalShip*>(Attacker)->CurrentSystemKills,
                CurrentStar->Status.ControlFaction * sizeof(std::uint16_t))) == 0) {
            pas::store_unaligned<std::uint16_t>(
                pas::byte_offset(&static_cast<aNormalShip::TNormalShip*>(Attacker)->CurrentSystemKills,
                    CurrentStar->Status.ControlFaction * sizeof(std::uint16_t)), static_cast<std::uint16_t>(1));
        }
    }

    std::uint8_t TRuins::AcceptsAppealFrom(aShip::TShip* Ship) {
        return false;
    }

    void TRuins::ProcessCombatDialogue() {
    }

    void TRuins::ReactToExtortionDemand(void* Ranger) {
    }

    std::uint8_t TRuins_BuildMoneyExtortionResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t DemandedAmount) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins_BuildCargoExtortionResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t OfferedAmount) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins_BuildAttackRequestResponse(TRuins* Self, aShip::TShip* Requester, pas::WideString& Response,
        aShip::TShip* Target) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins_AcceptPartnershipOffer(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins_BuildPartnershipOfferResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    // Script.GoodsRuinsForBuy. Restocks selected goods and sets prices near the global minimum.
    void TRuins::ForceGoodsForSale(aGalaxyStruct::TItemTypeMask GoodsMask) {
        std::uint8_t Good{};
        for (Good = 0; Good <= 7; ++Good) {
            if (pas::contains(GoodsMask, Good)) {
                {
                    double cpp_right = aMyFunction::NextRandomFloatRange(0.9, 1.1, RandomState);
                    ShopGoods[Good].PriceState = static_cast<float>(aConst::GoodsMarket[Good].MinPrice * cpp_right);
                }
                {
                    std::int32_t max = ([&] {
                        std::int32_t cpp_right_2 = aMyFunction::NextRandomIntRange(1,
                            aConst::GoodsMarket[Good].BaseStock / 10 + 1, RandomState);
                        std::int32_t cpp_arg = ShopGoods[Good].Count + cpp_right_2;
                        std::int32_t cpp_arg_2 = aConst::GoodsMarket[Good].BaseStock / 10 + 1;
                        return std::max<std::int32_t>(cpp_arg, cpp_arg_2);
                    }());
                    ShopGoods[Good].Count = max;
                }
                ShopGoods[Good].PurchasePrice = pas::real_round(ShopGoods[Good].PriceState);
                ShopGoods[Good].BaseSalePrice = std::max<std::int64_t>(static_cast<std::int64_t>(1),
                        pas::real_round(static_cast<float>(static_cast<float>(ShopGoods[Good].PriceState * 0x1.f5c28f5c28f5ep-1) - 1.0)));
            }
        }
    }

    // Sets base Accuracy and Maneuverability to 0..5 using galaxy technology and war state.
    void TRuins::GenerateCombatSkills() {
        std::int32_t Level = pas::real_round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 0.0, 4.0));
        if (aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators) < 40) {
            Level -= 2;
        }
        if (aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators) > 80) {
            ++Level;
        }
        if (aGalaxy::Galaxy->WarDeltaWin[1] > -3) {
            ++Level;
        }
        if (aGalaxy::Galaxy->WarDeltaWin[1] < 3) {
            Level -= 2;
        }
        std::int32_t Accuracy = aMyFunction::NextRandomIntRange(Level - 1, Level + 1, RandomState);
        std::int32_t Maneuverability = aMyFunction::NextRandomIntRange(Level - 1, Level + 1, RandomState);
        Accuracy = std::min<std::int32_t>(5, std::max<std::int32_t>(0, Accuracy));
        Maneuverability = std::min<std::int32_t>(5, std::max<std::int32_t>(0, Maneuverability));
        BaseSkills[aGalaxyStruct::psAccuracy] = Accuracy;
        BaseSkills[aGalaxyStruct::psManeuverability] = Maneuverability;
    }

    // Immediately replaces Position with a sampled arrival point in CurrentStar.
    void TRuins::RandomizePosition() {
        EC_Struct::TPointF Point{};
        Point = SelectTeleportArrivalPoint(CurrentStar);
        Position = Point;
    }

    // Search is limited to 1001 attempts.
    EC_Struct::TPointF TRuins::SelectTeleportArrivalPoint(aGalaxy::TStar* Star) {
        EC_Struct::TPointF Result{};
        aPlanet::TPlanet* Planet{};
        aMyFunction::TPolarPoint Polar{};
        auto IsStationArrivalPointClear = [&](EC_Struct::TPointF Point) -> std::uint8_t {
            std::int32_t I{};
            aShip::TShip* Ship{};
            float DistanceSquared{};
            std::uint8_t Result = false;
            {
                const std::int32_t cpp_last = pas::list_count(Star->Ships) - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
                        if (pas::class_cast_if<TRuins*>(Ship) != nullptr && Ship != this) {
                            DistanceSquared = aMyFunction::PointDistanceSquared(Point, Ship->Position);
                            if (DistanceSquared <= 4.9E+5) {
                                return Result;
                            }
                        }
                    }
                }
            }
            return true;
        };
        std::int32_t Attempts = 0;
        std::int32_t LastPlanet = pas::list_count(Star->Planets) - 1;
        if (aGalaxy::Galaxy->AreStationsNearStarsEnabled()) {
            LastPlanet = LastPlanet / 2;
        }
        do {
            {
                std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, LastPlanet, RandomState);
                aMyFunction::TObjectList* planets = Star->Planets;
                Planet = pas::list_at<aPlanet::TPlanet>(planets, nextRandomIntRange);
            }
            {
                double cpp_right = aMyFunction::NextRandomIntRange(0, 50, RandomState);
                Polar.Radius = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Planet->Radius) + Planet->Orbit.Radius) + 1.0E+2) + cpp_right);
            }
            Polar.AngleDegrees = aMyFunction::NextRandomIntRange(0, 359, RandomState);
            Result = aMyFunction::PolarToPoint(Polar);
            ++Attempts;
        } while (!(IsStationArrivalPointClear(Result) || Attempts > 1000));
        return Result;
    }

    float TRuins::AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) {
        return Effectiveness;
    }

    float TRuins::EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
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
                Result = Value * 300;
            } else if (cpp_case == aConst::bonRadar) {
                Result = static_cast<std::int8_t>(GetRadar() == nullptr);
            } else if (cpp_case == aConst::bonScan) {
                Result = Value * 20 *
                        CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags));
            } else if (cpp_case == aConst::bonDroid) {
                Result = static_cast<float>(static_cast<double>(Value * 20) / pas::real_max_scalar<float>(0.1f,
                            GetHull()->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags))));
            } else if (cpp_case == aConst::bonDef) {
                Result = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Value * 8 * 100) / std::max<std::int32_t>(5, 100 - Value)) * 45.0) /
                        std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                Result = Value * 10;
            } else if (cpp_case == aConst::bonWSplinter) {
                Result = Value * 10;
            } else if (cpp_case == aConst::bonWMissile) {
                Result = static_cast<float>(Value * 10 * (static_cast<float>(0.1 + static_cast<float>(static_cast<std::int8_t>(GetRadarRange() > 0) * 0x1.ccccccccccccbp-1))));
            } else if (cpp_case == aConst::bonWRadius) {
                Result = Value;
            } else if (cpp_case >= aConst::bonSkill1 && cpp_case <= aConst::bonSkill6) {
                if (Value > 0) {
                    Result = std::min<std::int32_t>(
                            6 - GetEffectiveSkillLevel(aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1],
                                false), Value) * StationSkillBonusWeights[BonusKind];
                }
                if (Value > 0 && Value + GetEffectiveSkillLevel(
                            aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) > 6) {
                    Result = static_cast<float>(Result + static_cast<float>(static_cast<float>(StationSkillBonusWeights[BonusKind] * 0x1.999999999999bp-5) * (Value + GetEffectiveSkillLevel(
                                aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) - 6)));
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(
                            static_cast<std::int32_t>(GetEffectiveSkillLevel(
                                aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false)), -Value) *
                            -StationSkillBonusWeights[BonusKind];
                }
                if (Value < 0 && Value + GetEffectiveSkillLevel(
                            aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false) < 0) {
                    Result = static_cast<float>(Result + static_cast<float>(static_cast<float>(StationSkillBonusWeights[BonusKind] * 0x1.eb851eb851eb7p-6) * (Value + GetEffectiveSkillLevel(
                                aConst::EquipmentBonusSkills[BonusKind - aConst::bonSkill1], false))));
                }
            }
        }
        if (TypeId == aGalaxyStruct::rstDominion) {
            switch (BonusKind) {
                case aConst::bonHook: {
                    return static_cast<float>((static_cast<float>(pas::real_min<double>(static_cast<double>(Value),
                        static_cast<float>(static_cast<double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5])) + static_cast<float>(Value * 0x1.999999999999bp-4))) *
                        1.0);
                }
                case aConst::bonHookRadius: return static_cast<float>(Value * 1.5);
                default: return Result;
            }
        }
        return Result;
    }

    float TRuins::EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
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
            Result = static_cast<float>(Result * 1.5);
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
        switch (Weapon->GetWeaponInfo()->ShotType) {
            case aGalaxyStruct::wstRocket: Result = static_cast<float>(static_cast<float>(static_cast<float>(Result * 0x1.199999999999bp+0) * Weapon->GetShotCount()) * (static_cast<float>(1.0 + StatusFactor))); break;
            case aGalaxyStruct::wstMissile: {
                Result = static_cast<float>(static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.1 + static_cast<float>(static_cast<float>(Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.5) * 0x1.47ae147ae147ap-7)) + StatusFactor))) *
                        Weapon->GetShotCount());
                break;
            }
            case aGalaxyStruct::wstTorpedo: {
                Result = static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.0 + static_cast<float>(static_cast<float>(Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.5) * 0x1.47ae147ae147ap-7)) + StatusFactor)));
                break;
            }
            case aGalaxyStruct::wstChain: {
                Result = static_cast<float>(static_cast<float>(Result * (static_cast<float>(1.1 + static_cast<float>((Weapon->GetShotCount() - 1) * 0x1.999999999999bp-3)))) * (static_cast<float>(1.0 + StatusFactor)));
                break;
            }
            case aGalaxyStruct::wstSplash: {
                Result = static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.0 + static_cast<float>(static_cast<float>(Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.0) * 0x1.47ae147ae147ap-7)) + StatusFactor)));
                break;
            }
            case aGalaxyStruct::wstAreaDamage: {
                Result = static_cast<float>(Result * (static_cast<float>(static_cast<float>(1.0 + static_cast<float>(static_cast<float>(Weapon->Range * 0x1.4ccccccccccccp+0) * 0x1.47ae147ae147ap-7)) + StatusFactor)));
                break;
            }
            default: Result = static_cast<float>(Result * (static_cast<float>(1.0 + StatusFactor))); break;
        }
        Result = static_cast<float>(static_cast<double>(Result) * Weapon->GetAttackCount());
        std::uint8_t HasOtherWeapon = false;
        {
            const std::int32_t cpp_last_2 = static_cast<std::int32_t>(CountEquippedWeapons());
            if (1 <= cpp_last_2) {
                for (I = 1; I <= cpp_last_2; ++I) {
                    if (!pas::in_range(Weapons[I]->GetWeaponInfo()->ShotType,
                        static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage),
                        static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                        HasOtherWeapon = true;
                    }
                }
            }
        }
        if (static_cast<std::uint8_t>(HasOtherWeapon ^ 1) && pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage),
                static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            return static_cast<float>(Result * 0.5);
        }
        return Result;
    }

    // Script state can force standing 9 or preserve the current value; type 13 has no default assignment.
    void TRuins_RefreshCurrentStanding(TRuins* Self) {
        aGalaxyStruct::TScriptStandingOverrideMode StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            if (Self->TypeId != aGalaxyStruct::rstCustomStation) {
                Self->CurrentStanding = aConst::StationDefaultStandings[Self->TypeId];
            }
        }
    }

    // Type-13 fallback through the planet's hull generator; may apply a special module.
    aItem::THull* TRuins::GeneratePlanetHullOffer(pas::Object* Ship, aPlanet::TPlanet* Planet) {
        aShip::TShip* Buyer{};
        std::int32_t ModuleIndex{};
        aItem::THull* Result = nullptr;
        if (Planet != nullptr && Ship != nullptr && pas::class_cast_if<aShip::TShip*>(Ship) != nullptr) {
            Buyer = static_cast<aShip::TShip*>(Ship);
            Result = Planet->GenerateHullOffer(Buyer);
            if (Result != nullptr) {
                ModuleIndex = Result->SpecialModuleIndex - 1;
                if (ModuleIndex < 0) {
                    if (Buyer->CanGenerateSpecialHullModule()) {
                        ModuleIndex = SelectHullOfferSpecialMicroModule(Result, Planet);
                    }
                    if (Buyer->GetHull()->SpecialModuleIndex > 0 && aPlayer::GetPlayer() != Buyer && ModuleIndex < 0) {
                        ModuleIndex = Buyer->GetHull()->SpecialModuleIndex - 1;
                    }
                    if (ModuleIndex >= 0) {
                        aItem::ApplySpecialMicroModule(ModuleIndex, Result);
                    }
                }
            }
        }
        return Result;
    }

    // Returns a newly allocated offer or nil; Planet supplies local technology.
    aItem::THull* TRuins::GenerateHullOffer(pas::Object* Ship, aPlanet::TPlanet* Planet) {
        aShip::TShip* Buyer{};
        std::int32_t Count{};
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t MinSize{};
        std::int32_t MaxSize{};
        std::int32_t Size{};
        aGalaxyStruct::THullType HullType{};
        aGalaxyStruct::TOwnerId Owner{};
        std::int32_t Series{};
        std::int32_t ModuleIndex{};
        std::uint8_t Flagship{};
        aItem::THull* Result = nullptr;
        if (TypeId == aGalaxyStruct::rstCustomStation) {
            return GeneratePlanetHullOffer(Ship, Planet);
        }
        if (Ship != nullptr && pas::class_cast_if<aShip::TShip*>(Ship) != nullptr) {
            Buyer = static_cast<aShip::TShip*>(Ship);
            Count = 0;
            for (auto cpp_range = pas::for_to<aGalaxyStruct::THullType>(aGalaxyStruct::htRanger,
                aGalaxyStruct::htFlagship); cpp_range.next(HullType); ) {
                if (pas::contains(StationOfferHullTypes[TypeId], HullType)) {
                    ++Count;
                }
            }
            if (Count == 0) {
                return Result;
            }
            Count = aMyFunction::NextRandomIntRange(1, Count, RandomState);
            for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::THullType>(aGalaxyStruct::htRanger,
                aGalaxyStruct::htFlagship); cpp_range_2.next(HullType); ) {
                if (pas::contains(StationOfferHullTypes[TypeId], HullType)) {
                    --Count;
                }
                if (Count == 0) {
                    break;
                }
            }
            Flagship = pas::class_cast_if<aWarrior::TWarrior*>(Buyer) != nullptr &&
                    static_cast<aWarrior::TWarrior*>(Buyer)->WarriorType == aWarrior::wtFlagship;
            if (Flagship) {
                HullType = aGalaxyStruct::htFlagship;
            }
            if (aPlayer::GetPlayer() == Buyer || Buyer->GetHull()->HullType == aGalaxyStruct::htSpecial ||
                Buyer->GetHull()->HullType == HullType) {
                MaxLevel = Planet->InventionLevels[aConst::EquipmentInventionIndices[aConst::t_Hull]];
                MinLevel = std::max<std::int32_t>(1, MaxLevel / 2 - 1);
                MaxLevel = std::min<std::int32_t>(8, MaxLevel + StationOfferHullLevelBonus[TypeId]);
                switch (aGalaxy::Galaxy->GetHullGrowthMod()) {
                    case 1: {
                        Size = Buyer->GetHull()->EstimateCapacityWithoutBonuses();
                        if (Flagship) {
                            Size = Size / 2;
                        }
                        MinSize = Size / 2;
                        if (pas::is_one_of<aGalaxyStruct::htTransport, aGalaxyStruct::htLiner>(HullType)) {
                            MaxSize = Size + (40 + 10 * (TypeId == aGalaxyStruct::rstBusinessCenter)) *
                                        aGalaxy::Galaxy->TechLevel;
                        } else {
                            switch (HullType) {
                                case aGalaxyStruct::htDiplomat: MaxSize = Size + 10 * aGalaxy::Galaxy->TechLevel; break;
                                default: MaxSize = Size + 25 * aGalaxy::Galaxy->TechLevel; break;
                            }
                        }
                        MinSize = std::max<std::int64_t>(static_cast<std::int64_t>(MinSize),
                                pas::real_round(static_cast<float>(static_cast<double>(aConst::HullBaseSize) *
                                    aConst::EquipmentSizeFactors[5 - (TypeId == aGalaxyStruct::rstBusinessCenter)])));
                        MaxSize = aGalaxy::Galaxy->ScaleIntByTechLevel(
                                pas::real_round(static_cast<float>(static_cast<double>(aConst::HullBaseSize) *
                                    aConst::EquipmentSizeFactors[4])), MaxSize);
                        break;
                    }
                    case 2: {
                        Size = Buyer->GetHull()->Weight;
                        if (Flagship) {
                            Size = Size / 2;
                        }
                        MinSize = pas::real_round(static_cast<float>(static_cast<double>(aConst::HullBaseSize) *
                                aConst::EquipmentSizeFactors[5 - (TypeId == aGalaxyStruct::rstBusinessCenter)]));
                        MaxSize = std::min<std::int64_t>(static_cast<std::int64_t>(Size),
                                pas::real_round(static_cast<float>(static_cast<double>(aConst::HullBaseSize) *
                                    aConst::EquipmentSizeFactors[aGalaxy::Galaxy->ScaleIntByTechLevel(5, 1)])));
                        break;
                    }
                    default: {
                        Size = Buyer->GetHull()->Weight;
                        if (Flagship) {
                            Size = Size / 2;
                        }
                        MinSize = Size / 2;
                        if (pas::is_one_of<aGalaxyStruct::htTransport, aGalaxyStruct::htLiner>(HullType)) {
                            MaxSize = Size + 300 + 100 * (TypeId == aGalaxyStruct::rstBusinessCenter);
                        } else {
                            switch (HullType) {
                                case aGalaxyStruct::htDiplomat: MaxSize = Size + 50; break;
                                default: MaxSize = Size + 200; break;
                            }
                        }
                        MinSize = std::max<std::int64_t>(static_cast<std::int64_t>(MinSize),
                                pas::real_round(static_cast<float>(static_cast<double>(aConst::HullBaseSize) *
                                    aConst::EquipmentSizeFactors[5 - (TypeId == aGalaxyStruct::rstBusinessCenter)])));
                        MaxSize = std::min<std::int64_t>(static_cast<std::int64_t>(MaxSize),
                                pas::real_round(static_cast<float>(static_cast<double>(aConst::HullBaseSize) *
                                    aConst::EquipmentSizeFactors[aGalaxy::Galaxy->ScaleIntByTechLevel(3, 1)])));
                        break;
                    }
                }
                Owner = aConst::PickRandomEquipmentOwner(RandomState);
                if (Buyer->GetHull()->OwnerId == Owner || aPlayer::GetPlayer() == Buyer) {
                    Result = pas::construct_call<aItem::THull>(aItem::TEquipment_Create);
                    Series = -1;
                    ModuleIndex = -1;
                    Result->OwnerId = Owner;
                    Result->PirateBuilt = CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates &&
                            CurrentStar->Status.CustomFaction == u"";
                    if (Buyer->CanGenerateSpecialHullModule()) {
                        ModuleIndex = SelectHullOfferSpecialMicroModule(Result, Planet);
                    }
                    if (ModuleIndex < 0) {
                        if (Buyer->CanGenerateSpecialHullModule()) {
                            ModuleIndex = SelectHullOfferSpecialMicroModule(Result, Planet);
                        }
                        if (pas::contains(StationOfferRareHullTypes[TypeId], HullType)) {
                            Series = aGalaxy::Galaxy->SelectHullSeries(Owner, HullType, 1, 100);
                        } else {
                            Series = aGalaxy::Galaxy->SelectHullSeries(Owner, HullType, 1, 30);
                        }
                    }
                    if (Flagship) {
                        std::uint8_t pirateBuilt = Result->PirateBuilt;
                        std::uint8_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel,
                            RandomState);
                        std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize * 2, MaxSize * 2,
                            RandomState);
                        Result->Init(nextRandomIntRange_2, nextRandomIntRange, Owner, aGalaxyStruct::htFlagship, Series,
                            pirateBuilt);
                    } else {
                        std::uint8_t pirateBuilt_2 = Result->PirateBuilt;
                        std::uint8_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel,
                            RandomState);
                        std::int32_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(MinSize, MaxSize,
                            RandomState);
                        Result->Init(nextRandomIntRange_4, nextRandomIntRange_3, Owner, HullType, Series,
                            pirateBuilt_2);
                    }
                    if (Buyer->GetHull()->SpecialModuleIndex > 0 && aPlayer::GetPlayer() != Buyer && ModuleIndex < 0) {
                        ModuleIndex = Buyer->GetHull()->SpecialModuleIndex - 1;
                    }
                    if (ModuleIndex >= 0) {
                        aItem::ApplySpecialMicroModule(ModuleIndex, Result);
                    }
                }
            }
        }
        return Result;
    }

    // Chooses the weapon type; returns a newly allocated offer or nil.
    aItem::TWeapon* TRuins::GenerateWeaponOffer(pas::Object* Ship, aPlanet::TPlanet* Planet) {
        aShip::TShip* Buyer{};
        std::int32_t Attempts{};
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t MinSize{};
        std::int32_t MaxSize{};
        aGalaxyStruct::TWeaponAvailabilityMask Availability{};
        aConst::PWeaponInfo Info{};
        aGalaxyStruct::TOwnerId Owner{};
        aGalaxyStruct::TOwnerId CandidateOwner{};
        std::int32_t ModuleIndex{};
        aItem::TWeapon* Result = nullptr;
        if (Ship != nullptr && pas::class_cast_if<aShip::TShip*>(Ship) != nullptr) {
            Buyer = static_cast<aShip::TShip*>(Ship);
            Availability = pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{aGalaxyStruct::waFree}});
            if (Buyer->TypeId == aGalaxyStruct::stKling && pas::contains(aConst::PlanetOwnerMasks.Dominators,
                    OwnerId)) {
                Availability = Availability + pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({
                            {aGalaxyStruct::waNotSoldAndNodeRepair}});
            }
            if (pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Buyer->TypeId) && pas::contains(
                    aConst::FactionStandingMasks[aGalaxyStruct::sfPirates], CurrentStanding) &&
                (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates || !pas::contains(
                        aConst::FactionStandingMasks[aGalaxyStruct::sfCoalition], CurrentStanding))) {
                Availability = Availability + pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({
                            {aGalaxyStruct::waPirateOnly}});
            }
            if (pas::in_range(Buyer->TypeId, static_cast<std::int32_t>(aGalaxyStruct::stRanger),
                static_cast<std::int32_t>(aGalaxyStruct::stWarrior)) && pas::contains(
                    aConst::FactionStandingMasks[aGalaxyStruct::sfCoalition], CurrentStanding) &&
                (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition || !pas::contains(
                        aConst::FactionStandingMasks[aGalaxyStruct::sfPirates], CurrentStanding))) {
                Availability = Availability + pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({
                            {aGalaxyStruct::waCoalitionOnly}, {aGalaxyStruct::waMalocOnly, aGalaxyStruct::waGaalOnly}});
            }
            // The native counter guard has no back edge: only one offer is generated.
            Attempts = 0;
            if (Attempts <= 100) {
                ++Attempts;
                Info = aGalaxy::Galaxy->SelectWeaponInfo(RandomState, Availability,
                        Planet->InventionLevels[aGalaxyStruct::piMainTech], 1);
                aMyFunction::AdvanceRandomSeed(RandomState);
                if (!pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Buyer->TypeId) && pas::in_range(
                        Info->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                        static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                    static_cast<void>(Buyer->CountDirectFireWeapons() > Buyer->CountMissileWeapons());
                }
                MinSize = pas::real_round(static_cast<float>(static_cast<double>(Info->AverageSize) * aConst::EquipmentSizeFactors[5]));
                MaxSize = pas::real_round(static_cast<float>(static_cast<double>(Info->AverageSize) * aConst::EquipmentSizeFactors[1]));
                if (aWarrior::TWarrior* warrior = pas::class_cast_if<aWarrior::TWarrior*>(Buyer); warrior != nullptr &&
                    warrior->WarriorType == aWarrior::wtFlagship) {
                    MinSize *= 2;
                    MaxSize *= 2;
                }
                MinLevel = 1;
                MaxLevel = std::min<std::int32_t>(
                        static_cast<std::int32_t>(Planet->InventionLevels[aGalaxyStruct::piMainTech]),
                        static_cast<std::int32_t>(Planet->InventionLevels[Info->InventionIndex]));
                MinLevel = std::max<std::int32_t>(MinLevel, MaxLevel / 2 - 1);
                MaxLevel = std::min<std::int32_t>(8, MaxLevel + StationOfferWeaponLevelBonus[TypeId]);
                Owner = aConst::PickRandomEquipmentOwner(RandomState);
                if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && pas::contains(
                        aConst::FactionStandingMasks[aGalaxyStruct::sfPirates], CurrentStanding) &&
                    (aMyFunction::NextRandomIntRange(1, 100, RandomState) < 70 ||
                        aGalaxy::Galaxy->CoalitionDefeatedTurn != 0)) {
                    Owner = aGalaxyStruct::oiPirate;
                }
                for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc,
                    aGalaxyStruct::oiPirate); cpp_range.next(CandidateOwner); ) {
                    if (aConst::OwnerWeaponAvailability[CandidateOwner] == Info->Availability) {
                        Owner = CandidateOwner;
                        break;
                    }
                }
                {
                    std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                    std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize, MaxSize, RandomState);
                    Result = aItem::CreateGeneratedWeapon(Info, nextRandomIntRange_2, nextRandomIntRange, Owner);
                }
                if (aWarrior::TWarrior* warrior_2 = pas::class_cast_if<aWarrior::TWarrior*>(Buyer); warrior_2 !=
                    nullptr && warrior_2->WarriorType == aWarrior::wtFlagship) {
                    Result->DetailImprovement = 3;
                    Result->Improve(aItem::ikAny);
                } else if (Buyer->CanGenerateMicroModuleForLoadout()) {
                    ModuleIndex = SelectWeaponOfferSpecialMicroModule(Result, Planet);
                    if (ModuleIndex >= 0) {
                        aItem::ApplySpecialMicroModule(ModuleIndex, Result);
                    }
                }
            }
        }
        return Result;
    }

    // Accepts types 42..68; weapon types all select the weapon generator. Does not insert the result into EquipmentShop.
    aItem::TEquipment* TRuins_GenerateEquipmentOffer(TRuins* Self, pas::Object* Ship, aPlanet::TPlanet* Planet,
        aConst::TItemType ItemType) {
        std::int32_t Priority{};
        std::int32_t ModuleIndex{};
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t MinSize{};
        std::int32_t MaxSize{};
        std::int32_t SpecialModule{};
        aGalaxyStruct::TOwnerId Owner{};
        aItem::TEquipment* Result = nullptr;
        if (Ship == nullptr || !(pas::class_cast_if<aShip::TShip*>(Ship) != nullptr)) {
            return Result;
        }
        aShip::TShip* Buyer = static_cast<aShip::TShip*>(Ship);
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks),
            static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            if (!pas::is_one_of<aConst::t_FuelTanks, aConst::t_Engine>(ItemType) &&
                Buyer->GetSlotCountForItemType(ItemType) == 0 && aPlayer::GetPlayer() != Buyer) {
                return Result;
            }
            MinLevel = 1;
            MaxLevel = Planet->InventionLevels[aConst::EquipmentInventionIndices[ItemType]];
            MinLevel = std::max<std::int32_t>(MinLevel, MaxLevel / 2 - 1);
            MaxLevel = std::min<std::int32_t>(8, MaxLevel + StationOfferEquipmentLevelBonus[Self->TypeId][ItemType]);
            MinSize = pas::real_round(static_cast<float>(static_cast<double>(aConst::GetAverageItemSize(ItemType)) *
                    aConst::EquipmentSizeFactors[5]));
            MaxSize = pas::real_round(static_cast<float>(static_cast<double>(aConst::GetAverageItemSize(ItemType)) *
                    aConst::EquipmentSizeFactors[1]));
            if (aWarrior::TWarrior* warrior = pas::class_cast_if<aWarrior::TWarrior*>(Buyer); warrior != nullptr &&
                warrior->WarriorType == aWarrior::wtFlagship) {
                MinSize *= 2;
                MaxSize *= 2;
            }
            Owner = aConst::PickRandomEquipmentOwner(Self->RandomState);
            if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && pas::contains(
                    aConst::FactionStandingMasks[aGalaxyStruct::sfPirates], Self->CurrentStanding) &&
                (aMyFunction::NextRandomIntRange(1, 100, Self->RandomState) < 70 ||
                    aGalaxy::Galaxy->CoalitionDefeatedTurn != 0)) {
                Owner = aGalaxyStruct::oiPirate;
            }
            {
                std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel,
                    Self->RandomState);
                std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize, MaxSize,
                    Self->RandomState);
                Result = aItem::CreateGeneratedEquipment(ItemType, nextRandomIntRange_2, nextRandomIntRange, Owner);
            }
            if (Buyer->CanGenerateMicroModuleForLoadout()) {
                SpecialModule = Self->SelectEquipmentOfferSpecialMicroModule(Result, Planet);
                if (SpecialModule >= 0) {
                    aItem::ApplySpecialMicroModule(SpecialModule, Result);
                }
            }
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_IndustrialLaser),
            static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            Result = Self->GenerateWeaponOffer(Ship, Planet);
        } else if (ItemType == aConst::t_Hull) {
            Result = Self->GenerateHullOffer(Ship, Planet);
        }
        if (Result == nullptr) {
            return Result;
        }
        switch (Self->TypeId) {
            case aGalaxyStruct::rstBusinessCenter: {
                Result->Cost = std::min<std::int64_t>(static_cast<std::int64_t>(aGalaxyStruct::MaxMonetaryValue),
                        pas::real_round(static_cast<float>(Result->Cost * 0x1.3333333333334p+0)));
                Result->ConditionPercent = aMyFunction::NextRandomIntRange(70, 100, Self->RandomState);
                break;
            }
            case aGalaxyStruct::rstMedicalBase: {
                Result->ConditionPercent = aMyFunction::NextRandomIntRange(1, 100, Self->RandomState);
                break;
            }
            case aGalaxyStruct::rstPirateBase: {
                Result->ConditionPercent = aMyFunction::NextRandomIntRange(0, 60, Self->RandomState);
                break;
            }
            case aGalaxyStruct::rstMilitaryBase: {
                Result->ConditionPercent = aMyFunction::NextRandomIntRange(60, 100, Self->RandomState);
                break;
            }
            case aGalaxyStruct::rstDominion: {
                Result->ConditionPercent = aMyFunction::NextRandomIntRange(0, 60, Self->RandomState);
                break;
            }
        }
        if (Result->CanImprove()) {
            if (Self->TypeId == aGalaxyStruct::rstScienceBase) {
                std::int32_t cpp_case = aMyFunction::NextRandomIntRange(0, 100, Self->RandomState);
                if (cpp_case >= 0 && cpp_case <= 70) {
                    Result->Improve(aItem::ikMinor);
                } else if (cpp_case >= 71 && cpp_case <= 90) {
                    Result->Improve(aItem::ikMedium);
                } else if (cpp_case >= 91 && cpp_case <= 100) {
                    Result->Improve(aItem::ikMajor);
                }
            } else if (Self->TypeId == aGalaxyStruct::rstRangerCenter) {
                std::int32_t cpp_case_2 = aMyFunction::NextRandomIntRange(0, 100, Self->RandomState);
                if (cpp_case_2 >= 0 && cpp_case_2 <= 10) {
                    Result->Improve(aItem::ikMinor);
                } else if (cpp_case_2 >= 11 && cpp_case_2 <= 20) {
                    Result->Improve(aItem::ikMedium);
                } else if (cpp_case_2 >= 21 && cpp_case_2 <= 22) {
                    Result->Improve(aItem::ikMajor);
                }
            } else {
                std::int32_t cpp_case_3 = aMyFunction::NextRandomIntRange(0, 100, Self->RandomState);
                if (cpp_case_3 >= 0 && cpp_case_3 <= 10) {
                    Result->Improve(aItem::ikMinor);
                } else if (cpp_case_3 >= 11 && cpp_case_3 <= 20) {
                    Result->Improve(aItem::ikMedium);
                } else if (cpp_case_3 >= 21 && cpp_case_3 <= 23) {
                    Result->Improve(aItem::ikMajor);
                }
            }
        }
        std::int32_t Attempts = 0;
        if (Self->TypeId == aGalaxyStruct::rstDominion && aMyFunction::NextRandomIntRange(0, 100, Self->RandomState) >
                50) {
            do {
                Priority = pas::real_round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 7.0, 7.0E+1,
                        0.0));
                {
                    std::uint32_t advanceRandomSeed = aMyFunction::AdvanceRandomSeed(Self->RandomState);
                    pas::Object* self = Self;
                    aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                    ModuleIndex = aGalaxy::TGalaxy::SelectMicroModule(Priority + Attempts / 5,
                            std::min<std::int32_t>(100, Priority + 20 + Attempts * 4), advanceRandomSeed, self);
                }
                if (aItem::CanInstallMicroModule(ModuleIndex, Result)) {
                    aItem::ApplyMicroModule(ModuleIndex, Result);
                    break;
                }
                ++Attempts;
            } while (!(Attempts > 50));
        }
        return Result;
    }

    // Returns a new owning list of offers. Ignores the Boolean argument.
    aMyFunction::TObjectList* TRuins_GenerateEquipmentOfferBatch(TRuins* Self, aShip::TShip* Ship,
        std::uint8_t UnusedForceGeneratedOffers) {
        aItem::TEquipment* Item{};
        std::int32_t I{};
        std::int32_t J{};
        aConst::TItemType Kind{};
        aPlanet::TPlanet* Planet{};
        aMyFunction::TObjectList* Result = pas::make_object<aMyFunction::TObjectList>();
        for (auto cpp_range = pas::for_to<std::int32_t>(1,
            aConst::StationEquipmentOfferQuotas[Self->TypeId].Hulls); cpp_range.next(J); ) {
            Planet = static_cast<aPlanet::TPlanet*>(Self->CurrentStar->SelectRandomInhabitedPlanet());
            Item = aRuins::TRuins_GenerateEquipmentOffer(Self, Ship, Planet, aConst::t_Hull);
            if (Item != nullptr) {
                pas::list_add(Result, reinterpret_cast<void*>(Item));
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1,
            aConst::CountItemTypesInMask(pas::constant_set<aConst::TItemTypeSelection>({
                {43, 49}}))); cpp_range_2.next(I); ) {
            Kind = static_cast<aConst::TItemType>(aConst::GetItemTypeFromMask(
                    pas::constant_set<aConst::TItemTypeSelection>({{43, 49}}), I));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1,
                pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::StationEquipmentOfferQuotas[Self->TypeId],
                    (Kind - 42) * sizeof(std::int32_t)))); cpp_range_3.next(J); ) {
                Planet = static_cast<aPlanet::TPlanet*>(Self->CurrentStar->SelectRandomInhabitedPlanet());
                Item = aRuins::TRuins_GenerateEquipmentOffer(Self, Ship, Planet, Kind);
                if (Item != nullptr) {
                    pas::list_add(Result, reinterpret_cast<void*>(Item));
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1,
            aConst::StationEquipmentOfferQuotas[Self->TypeId].Weapons); cpp_range_4.next(I); ) {
            Planet = static_cast<aPlanet::TPlanet*>(Self->CurrentStar->SelectRandomInhabitedPlanet());
            Item = aRuins::TRuins_GenerateEquipmentOffer(Self, Ship, Planet, aConst::WeaponCategoryItemType);
            if (Item != nullptr) {
                pas::list_add(Result, reinterpret_cast<void*>(Item));
            }
        }
        return Result;
    }

    // Does nothing in modes 1 and 2.
    void TRuins::UpdateGoodsMarketState() {
        std::uint8_t Good{};
        float TargetPrice{};
        float PriceStep{};
        std::int32_t TargetCount{};
        std::int32_t CountStep{};
        if (pas::is_one_of<aGalaxyStruct::sumDisabled, aGalaxyStruct::sumEquipmentOnly>(ShopUpdateMode)) {
            return;
        }
        aGalaxyStruct::TOwnerId Race = PilotRace;
        for (Good = 0; Good <= 7; ++Good) {
            // Station markets need the same mixed-width corrections as planet
            // markets: their loaded factors and clamped endpoints are doubles.
            const float race_stock = native_pc24::multiply(aConst::GoodsMarket[Good].BaseStock,
                aConst::PlanetRaceMarket[Race].GoodsFactors[Good].StockFactor);
            TargetCount = pas::real_round(native_pc24::multiply(race_stock,
                aConst::StationGoodsFactors[TypeId][Good].StockFactor));
            const float race_price = native_pc24::multiply(aConst::GoodsMarket[Good].AveragePrice,
                aConst::PlanetRaceMarket[Race].GoodsFactors[Good].PriceFactor);
            const float station_price = native_pc24::multiply(race_price,
                aConst::StationGoodsFactors[TypeId][Good].PriceFactor);
            TargetPrice = native_pc24::divide(station_price, aMyFunction::RemapClamped(
                ShopGoods[Good].Count, static_cast<float>(TargetCount * 0x1.3333333333334p-2), TargetCount * 2, 0.8, 1.2));
            if (static_cast<double>(TargetPrice) > aConst::GoodsMarket[Good].MinPrice) {
                TargetPrice = pas::real_min_scalar<float>(TargetPrice,
                        static_cast<float>(aConst::GoodsMarket[Good].MaxPrice + 1));
            } else {
                TargetPrice = pas::real_max_scalar<float>(TargetPrice,
                        static_cast<float>(aConst::GoodsMarket[Good].MinPrice - 1));
            }
            if (static_cast<float>(static_cast<double>(ShopGoods[Good].PriceState) - TargetPrice) >= 0.0) {
                PriceStep = static_cast<float>(static_cast<double>(TargetPrice) * aMyFunction::NextRandomFloatRange(0.0035, 0.006,
                            RandomState));
            } else {
                PriceStep = static_cast<float>(static_cast<double>(-TargetPrice) * aMyFunction::NextRandomFloatRange(0.0035, 0.006,
                            RandomState));
            }
            {
                std::int32_t cpp_case = aMyFunction::NextRandomIntRange(1, 100, RandomState);
                if (cpp_case >= 1 && cpp_case <= 70) {
                    ShopGoods[Good].PriceState = static_cast<float>(static_cast<double>(ShopGoods[Good].PriceState) - PriceStep);
                } else if (cpp_case >= 71 && cpp_case <= 90) {
                } else {
                    ShopGoods[Good].PriceState = static_cast<float>(static_cast<double>(ShopGoods[Good].PriceState) + PriceStep);
                }
            }
            if (static_cast<double>(ShopGoods[Good].PriceState) < aConst::GoodsMarket[Good].MinPrice / 2) {
                ShopGoods[Good].PriceState = aConst::GoodsMarket[Good].MinPrice / 2;
            } else if (static_cast<double>(ShopGoods[Good].PriceState) > aConst::GoodsMarket[Good].MaxPrice * 2) {
                ShopGoods[Good].PriceState = aConst::GoodsMarket[Good].MaxPrice * 2;
            }
            ShopGoods[Good].PurchasePrice = std::max<std::int64_t>(static_cast<std::int64_t>(2),
                    pas::real_round(ShopGoods[Good].PriceState));
            ShopGoods[Good].BaseSalePrice = std::max<std::int64_t>(
                    static_cast<std::int64_t>(ShopGoods[Good].PurchasePrice / 2 + 1),
                    pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(ShopGoods[Good].PriceState) * aMyFunction::RemapClamped(
                        ShopGoods[Good].Count, TargetCount, TargetCount * 2, 0.9, 0.7)) - 1.0)));
            if (ShopGoods[Good].Count - TargetCount >= 0) {
                double cpp_left = static_cast<float>(static_cast<double>(TargetCount) * aMyFunction::NextRandomFloatRange(0.0025, 0.005,
                        RandomState));
                CountStep = pas::real_round(static_cast<float>(cpp_left + aMyFunction::NextRandomUnitFloat(RandomState)));
            } else {
                double cpp_left_2 = static_cast<float>(static_cast<double>(-TargetCount) * aMyFunction::NextRandomFloatRange(0.0025, 0.005,
                        RandomState));
                CountStep = pas::real_round(static_cast<float>(cpp_left_2 - aMyFunction::NextRandomUnitFloat(RandomState)));
            }
            {
                std::int32_t cpp_case_2 = aMyFunction::NextRandomIntRange(1, 100, RandomState);
                if (cpp_case_2 >= 1 && cpp_case_2 <= 20) {
                    ShopGoods[Good].Count -= CountStep;
                } else if (cpp_case_2 >= 21 && cpp_case_2 <= 95) {
                } else {
                    ShopGoods[Good].Count += CountStep;
                }
            }
            if (ShopGoods[Good].Count < 0) {
                ShopGoods[Good].Count = 0;
            }
        }
    }

    // Returns the total including eligible artefacts; EquipmentCost excludes the separate artefact pass. Military discount uses the player's rank.
    std::int32_t TRuins::CalculateRepairCost(aShip::TShip* Ship, std::int32_t& EquipmentCost) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        float ArtefactFactor{};
        std::int32_t Result = 0;
        float EquipmentFactor = 1.0f;
        // Keep the native byte load followed by signed extension under DCC32 O-.
        if (TypeId == aGalaxyStruct::rstMilitaryBase) {
            EquipmentFactor = aMyFunction::RemapClamped(aPlayer::GetPlayer()->Rank, 0.0, 7.0, 0.9, 0.2);
        }
        if (TypeId == aGalaxyStruct::rstPirateBase) {
            EquipmentFactor = 0.84f;
        }
        {
            const std::int32_t cpp_last = pas::list_count(Ship->Inventory) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Item = pas::list_at<aItem::TEquipment>(Ship->Inventory, I);
                    if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); (!(weapon != nullptr) ||
                        weapon->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair ||
                        CanRepairArtefactsAtLocation()) && aShip::TShip_CanRepairEquipmentTech(this, Item) &&
                        (Item->ItemType == aConst::t_Hull || Item->EquippedFlag != 0 && Item->ConditionPercent <
                                9.0E+1)) {
                        Result += pas::real_round(static_cast<float>(static_cast<double>(aItem::TEquipment_CalculateRepairCost(Item)) *
                                EquipmentFactor));
                    }
                }
            }
        }
        EquipmentCost = Result;
        if (CanRepairArtefactsAtLocation()) {
            ArtefactFactor = 1.0f;
            if (TypeId == aGalaxyStruct::rstScienceBase) {
                ArtefactFactor = 0.84f;
            }
            if (TypeId == aGalaxyStruct::rstPirateBase) {
                ArtefactFactor = 0.84f;
            }
            {
                const std::int32_t cpp_last_2 = pas::list_count(Ship->Artefacts) - 1;
                if (0 <= cpp_last_2) {
                    for (I = 0; I <= cpp_last_2; ++I) {
                        Item = pas::list_at<aItem::TEquipment>(Ship->Artefacts, I);
                        if (Item->EquippedFlag != 0 && Item->ConditionPercent < 9.0E+1) {
                            Result += pas::real_round(static_cast<float>(static_cast<double>(aItem::TEquipment_CalculateRepairCost(Item)) *
                                    ArtefactFactor));
                        }
                    }
                }
            }
        }
        return Result;
    }

    std::int32_t TRuins::GetRepairCost(aShip::TShip* Ship) {
        std::int32_t EquipmentCost{};
        return CalculateRepairCost(Ship, EquipmentCost);
    }

    // Charges the full quote and repairs eligible items only if affordable. Caller refreshes ship statistics.
    void TRuins::RepairShipEquipment(aShip::TShip* Ship) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        {
            std::int32_t cpp_left = GetRepairCost(Ship);
            if (cpp_left > Ship->Money) {
                return;
            }
        }
        {
            std::int32_t cpp_right = GetRepairCost(Ship);
            Ship->SetMoney(Ship->Money - cpp_right);
        }
        {
            const std::int32_t cpp_last = pas::list_count(Ship->Inventory) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Item = pas::list_at<aItem::TEquipment>(Ship->Inventory, I);
                    if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); (!(weapon != nullptr) ||
                        weapon->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair ||
                        CanRepairArtefactsAtLocation()) && aShip::TShip_CanRepairEquipmentTech(this, Item) &&
                        (Item->ItemType == aConst::t_Hull || Item->EquippedFlag != 0 && Item->ConditionPercent <
                                9.0E+1)) {
                        Item->Repair();
                    }
                }
            }
        }
        if (CanRepairArtefactsAtLocation()) {
            const std::int32_t cpp_last_2 = pas::list_count(Ship->Artefacts) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Item = pas::list_at<aItem::TEquipment>(Ship->Artefacts, I);
                    if (Item->EquippedFlag != 0 && Item->ConditionPercent < 9.0E+1) {
                        Item->Repair();
                    }
                }
            }
        }
    }

    // Returns min(NodeReserve, 250), without a lower clamp.
    std::int32_t TRuins::GetNodeSaleBatchSize() {
        return std::min<std::int32_t>(NodeReserve, 250);
    }

    // Searches from CurrentStar for a standard pirate base with positive NodeReserve in a peaceful non-Dominator system. Borrowed result or nil.
    TRuins* TRuins::FindPirateBaseWithNodes() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Index{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        Index = pas::list_indexof(aGalaxy::Galaxy->Stars, reinterpret_cast<void*>(CurrentStar));
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    aMyFunction::IncrementWrapped(Index, 0, pas::list_count(aGalaxy::Galaxy->Stars) - 1);
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, Index);
                    if (Star->Status.Battle == 0 && Star->Status.ControlFaction != aGalaxyStruct::sfDominators &&
                        Star->Status.CustomFaction == u"" && Star->ShipTypeCounts[aGalaxyStruct::rstPirateBase] != 0) {
                        const std::int32_t cpp_last_2 = pas::list_count(Star->Ships) - 1;
                        if (0 <= cpp_last_2) {
                            for (J = 0; J <= cpp_last_2; ++J) {
                                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                                if (Ship->TypeId == aGalaxyStruct::rstPirateBase && Ship->TypeNameOverrideKey == u"" &&
                                    Ship->NodeReserve > 0) {
                                    return pas::checked_cast<TRuins*>(Ship);
                                }
                            }
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    // Zero-based module index. Deterministic chain keyed by station, current turn and player RNG state; Kind 1 selects priorities 31..69, Kind 2 selects 0..20 or 10..30, others 70..100.
    std::int32_t TRuins_SelectServiceMicroModule(TRuins* Self, std::int32_t Kind, std::int32_t Index,
        std::uint8_t InvertRarity) {
        std::int32_t ChainIndex = Index + 2;
        std::uint8_t Rare = ((static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) +
            aPlayer::GetPlayer()->RandomState) % 33 == 0) != InvertRarity;
        switch (Kind) {
            case 2: {
                if (Rare) {
                    return aGalaxy::TGalaxy::SelectMicroModule(0, 20,
                        aGalaxy::Galaxy->CurrentTurn / 57 + 2938629 + 17 * ChainIndex + Self->Id, Self);
                }
                return aGalaxy::TGalaxy::SelectMicroModule(10, 30,
                    aGalaxy::Galaxy->CurrentTurn / 57 + 32465621 + 17 * ChainIndex + Self->Id, Self);
            }
            case 1: {
                return aGalaxy::TGalaxy::SelectMicroModule(31, 69,
                    aGalaxy::Galaxy->CurrentTurn / 57 + 2351417 + 17 * ChainIndex + Self->Id, Self);
            }
            default: {
                return aGalaxy::TGalaxy::SelectMicroModule(70, 100,
                    aGalaxy::Galaxy->CurrentTurn / 57 + 17 * ChainIndex + Self->Id, Self);
            }
        }
    }

    std::uint8_t TRuins_CanDock(TRuins* Self, aShip::TShip* Ship) {
        std::uint8_t Result = false;
        if (Self->InNormalSpace() && Ship->EnemyShip != Self && aShip::TShip_GetRelationLevelToShip(Self, Ship) >
                aGalaxyStruct::rlHostile && static_cast<std::uint8_t>(Self->NoLanding ^ 1) && (!pas::in_range(
                Self->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssPirateActive),
                static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary)) || pas::in_range(Ship->CurrentStanding,
                    static_cast<std::int32_t>(aGalaxyStruct::ssPiratePassive),
                    static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary)))) {
            return true;
        }
        return Result;
    }

    // Clears Response on success; otherwise supplies a refusal message.
    std::uint8_t TRuins::CheckDockingPermission(aShip::TShip* Ship, pas::WideString& Response) {
        std::uint8_t Result = false;
        if (NoLanding || !InNormalSpace()) {
            Response = GR_Main::LookupLocalizedTextByKey(u"Help.LandingCancelScript"_wref.get());
            return Result;
        } else if (pas::in_range(CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssPirateActive),
            static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary)) && !pas::in_range(Ship->CurrentStanding,
                static_cast<std::int32_t>(aGalaxyStruct::ssPiratePassive),
                static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary))) {
            Response = GR_Main::LookupLocalizedTextByKey(u"Help.LandingCancelPirate"_wref.get());
            return Result;
        } else if (Ship->EnemyShip == this || aShip::TShip_GetRelationLevelToShip(this, Ship) <=
                aGalaxyStruct::rlHostile) {
            Response = GR_Main::LookupLocalizedTextByKey(u"Help.LandingCancelWar"_wref.get());
            return Result;
        } else {
            Response = pas::WideString();
            return true;
        }
    }

    void TRuins::p_destroy() {
        aRuins::TRuins_Destroy(this);
    }

    void TRuins::virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aRuins::TRuins_ResolveLoadedReferences(this, Galaxy);
    }

    void TRuins::virtual_TShip_NextDay() {
        aRuins::TRuins_NextDay(this);
    }

    void TRuins::virtual_TShip_NextDayLogic() {
        aRuins::TRuins_NextDayLogic(this);
    }

    std::uint8_t TRuins::virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return aRuins::TRuins_CanQueueReachablePlanet(this, Planet);
    }

    std::uint8_t TRuins::virtual_TShip_RecomputeFearState() {
        return aRuins::TRuins_RecomputeFearState(this);
    }

    std::uint8_t TRuins::virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) {
        return aRuins::TRuins_AcceptsRansomDemandFrom(this, Ship);
    }

    std::uint8_t TRuins::virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) {
        return aRuins::TRuins_TrustsAttackRequester(this, Ship);
    }

    std::uint8_t TRuins::virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t DemandedAmount) {
        return aRuins::TRuins_BuildMoneyExtortionResponse(this, OtherShip, Response, DemandedAmount);
    }

    std::uint8_t TRuins::virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) {
        return aRuins::TRuins_BuildCargoExtortionResponse(this, OtherShip, Response);
    }

    std::uint8_t TRuins::virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response,
        aShip::TShip* Target) {
        return aRuins::TRuins_BuildAttackRequestResponse(this, Requester, Response, Target);
    }

    std::uint8_t TRuins::virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        return aRuins::TRuins_AcceptPartnershipOffer(this, OtherShip, Response, PaymentAmount);
    }

    std::uint8_t TRuins::virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        return aRuins::TRuins_BuildPartnershipOfferResponse(this, OtherShip, Response, PaymentAmount);
    }

    void TRuins::virtual_TShip_RefreshCurrentStanding() {
        aRuins::TRuins_RefreshCurrentStanding(this);
    }

    std::uint8_t TRuins::virtual_TShip_CanDock(aShip::TShip* Ship) {
        return aRuins::TRuins_CanDock(this, Ship);
    }

} // namespace aRuins
