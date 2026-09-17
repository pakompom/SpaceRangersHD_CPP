#include "layout/aRuins.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aMissile.hpp"
#include "types/aNormalShip.hpp"
#include "types/aPirate.hpp"
#include "types/aRanger.hpp"
#include "types/aTranclucator.hpp"
#include "types/aWarrior.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
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

    const pas::Array<pas::Array<std::uint8_t, 0, 1>, 6, 12> StationPilotRaces = pas::Array<pas::Array<std::uint8_t, 0, 1>, 6, 12>{{pas::Array<std::uint8_t, 0, 1>{{static_cast<std::uint8_t>(3), static_cast<std::uint8_t>(4)}}, pas::Array<std::uint8_t, 0, 1>{{static_cast<std::uint8_t>(2), static_cast<std::uint8_t>(1)}}, pas::Array<std::uint8_t, 0, 1>{{static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(1)}}, pas::Array<std::uint8_t, 0, 1>{{static_cast<std::uint8_t>(3), static_cast<std::uint8_t>(4)}}, pas::Array<std::uint8_t, 0, 1>{{static_cast<std::uint8_t>(2), static_cast<std::uint8_t>(2)}}, pas::Array<std::uint8_t, 0, 1>{{static_cast<std::uint8_t>(4), static_cast<std::uint8_t>(4)}}, pas::Array<std::uint8_t, 0, 1>{{static_cast<std::uint8_t>(3), static_cast<std::uint8_t>(3)}}}};

    const pas::Array<aRuins::TStationHullGeneration, 6, 12> StationHullGeneration = pas::Array<aRuins::TStationHullGeneration, 6, 12>{{{.MinSize = 900, .MaxSize = 1600, .TechSizeBonus = 1500, .MinLevel = 2, .MaxLevel = 6}, {.MinSize = 900, .MaxSize = 1000, .TechSizeBonus = 1500, .MinLevel = 2, .MaxLevel = 5}, {.MinSize = 1200, .MaxSize = 1500, .TechSizeBonus = 1700, .MinLevel = 2, .MaxLevel = 7}, {.MinSize = 800, .MaxSize = 1200, .TechSizeBonus = 1100, .MinLevel = 2, .MaxLevel = 7}, {.MinSize = 800, .MaxSize = 1000, .TechSizeBonus = 1100, .MinLevel = 2, .MaxLevel = 5}, {.MinSize = 800, .MaxSize = 1000, .TechSizeBonus = 1200, .MinLevel = 2, .MaxLevel = 5}, {.MinSize = 900, .MaxSize = 1000, .TechSizeBonus = 1500, .MinLevel = 2, .MaxLevel = 5}}};

    const pas::Array<aRuins::TStationLevelRange, 6, 12> StationDefenseLevels = pas::Array<aRuins::TStationLevelRange, 6, 12>{{{.Minimum = 2, .Maximum = 6}, {.Minimum = 2, .Maximum = 5}, {.Minimum = 2, .Maximum = 7}, {.Minimum = 2, .Maximum = 8}, {.Minimum = 2, .Maximum = 5}, {.Minimum = 2, .Maximum = 4}, {.Minimum = 2, .Maximum = 5}}};

    const pas::Array<aRuins::TStationLevelRange, 6, 12> StationRepairLevels = pas::Array<aRuins::TStationLevelRange, 6, 12>{{{.Minimum = 2, .Maximum = 5}, {.Minimum = 1, .Maximum = 4}, {.Minimum = 2, .Maximum = 5}, {.Minimum = 2, .Maximum = 6}, {.Minimum = 2, .Maximum = 4}, {.Minimum = 2, .Maximum = 3}, {.Minimum = 1, .Maximum = 4}}};

    const pas::Array<aRuins::TStationWeaponGeneration, 6, 12> StationWeaponGeneration = pas::Array<aRuins::TStationWeaponGeneration, 6, 12>{{{.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 450}, {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 400}, {.BasicLevel = 4, .IntermediateLevel = 4, .AdvancedLevel = 4, .MinimumRange = 470}, {.BasicLevel = 4, .IntermediateLevel = 4, .AdvancedLevel = 4, .MinimumRange = 500}, {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 300}, {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 350}, {.BasicLevel = 4, .IntermediateLevel = 2, .AdvancedLevel = 2, .MinimumRange = 400}}};

    const pas::Array<pas::Array<std::uint8_t, 0, 2>, 6, 12> StationWeaponTypes = pas::Array<pas::Array<std::uint8_t, 0, 2>, 6, 12>{{pas::Array<std::uint8_t, 0, 2>{{static_cast<std::uint8_t>(52), static_cast<std::uint8_t>(56), static_cast<std::uint8_t>(59)}}, pas::Array<std::uint8_t, 0, 2>{{static_cast<std::uint8_t>(52), static_cast<std::uint8_t>(55), static_cast<std::uint8_t>(60)}}, pas::Array<std::uint8_t, 0, 2>{{static_cast<std::uint8_t>(52), static_cast<std::uint8_t>(58), static_cast<std::uint8_t>(56)}}, pas::Array<std::uint8_t, 0, 2>{{static_cast<std::uint8_t>(52), static_cast<std::uint8_t>(54), static_cast<std::uint8_t>(61)}}, pas::Array<std::uint8_t, 0, 2>{{static_cast<std::uint8_t>(51), static_cast<std::uint8_t>(57), static_cast<std::uint8_t>(55)}}, pas::Array<std::uint8_t, 0, 2>{{static_cast<std::uint8_t>(50), static_cast<std::uint8_t>(54), static_cast<std::uint8_t>(55)}}, pas::Array<std::uint8_t, 0, 2>{{static_cast<std::uint8_t>(52), static_cast<std::uint8_t>(55), static_cast<std::uint8_t>(60)}}}};

    const pas::Array<std::int32_t, 22, 27> StationSkillBonusWeights = pas::Array<std::int32_t, 22, 27>{{100, 100, 80, 0, 0, 0}};

    const pas::Array<std::int32_t, 6, 12> StationOfferHullLevelBonus = pas::Array<std::int32_t, 6, 12>{{0, 0, 1, 0, 0, 0, 0}};

    const pas::Array<aRuins::TStationHullTypes, 6, 12> StationOfferHullTypes = pas::Array<aRuins::TStationHullTypes, 6, 12>{{pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htPirate}, {aGalaxyStruct::htTransport}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger}, {aGalaxyStruct::htWarrior}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger, aGalaxyStruct::htDiplomat}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htTransport}, {aGalaxyStruct::htLiner}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htTransport, aGalaxyStruct::htDiplomat}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htPirate}})}};

    const pas::Array<aRuins::TStationHullTypes, 6, 12> StationOfferRareHullTypes = pas::Array<aRuins::TStationHullTypes, 6, 12>{{pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htPirate}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htWarrior}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htRanger}, {aGalaxyStruct::htTransport, aGalaxyStruct::htDiplomat}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htTransport}, {aGalaxyStruct::htLiner}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htTransport, aGalaxyStruct::htDiplomat}}), pas::constant_set<aRuins::TStationHullTypes>({{aGalaxyStruct::htPirate}})}};

    const pas::Array<std::int32_t, 6, 13> StationOfferWeaponLevelBonus = pas::Array<std::int32_t, 6, 13>{{1, 1, 2, 1, 1, 1, 2, 1}};

    const pas::Array<pas::Array<std::int32_t, 43, 49>, 6, 13> StationOfferEquipmentLevelBonus = pas::Array<pas::Array<std::int32_t, 43, 49>, 6, 13>{{pas::Array<std::int32_t, 43, 49>{{1, 0, 1, 0, 0, 0, 0}}, pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 1, 0}}, pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 0, 0}}, pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 1, 0, 1, 0}}, pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 1, 0}}, pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 1, 0}}, pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 1, 0}}, pas::Array<std::int32_t, 43, 49>{{0, 0, 0, 0, 0, 0, 0}}}};

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
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
            if (Ranger->LastDockedNonPlanetLocation == Self) {
                Ranger->LastDockedNonPlanetLocation = nullptr;
            }
        }
        pas::free(Self->EquipmentShop);
        Self->EquipmentShop = nullptr;
        if (Self->SatelliteOffer != nullptr) {
            pas::free(Self->SatelliteOffer);
            Self->SatelliteOffer = nullptr;
        }
        if (static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->RuinsProxy != Self) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"RuinsDestroyed"_w, nullptr);
            Event->AddData(Self->TypeId);
            Event->AddData(Self->Id);
            Event->AddData(Self->CurrentStar->Id);
            Event->AddTextData(Self->Name);
            Event->AddTextData(Self->TypeNameOverrideKey);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (Star->Dominion == Self) {
                Star->Dominion = nullptr;
            }
        }
        aShip::TShip_Destroy(Self);
    }

    // Also registers the station in Star. Type 13 inherits ranger-center defaults.
    void TRuins_Init(TRuins* Self, aGalaxyStruct::TStationType StationType, aGalaxy::TStar* Star, pas::WideString TypeNameOverride) {
        std::int32_t I{};
        aRanger::TRanger* Ranger{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::uint8_t Good{};
        aItem::TWeapon* Weapon{};
        aItem::TCargoHook* Hook{};
        std::uint8_t EquipmentOwner{};
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
            if (Config->CountBlocks(aConst::ShipTypeNames[Self->TypeId].Name) == 0) {
                return;
            }
            std::int32_t LastName = Config->GetBlock(aConst::ShipTypeNames[Self->TypeId].Name)->GetParamCount() - 1;
            Index = aMyFunction::NextRandomIntRange(0, LastName, Self->RandomState);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, LastName); cpp_range.next(I); ) {
                Self->Name = Config->GetBlock(aConst::ShipTypeNames[Self->TypeId].Name)->GetParamValue(Index);
                Used = false;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(J); ) {
                    OtherStar = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, J);
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(OtherStar->Ships) - 1); cpp_range_3.next(K); ) {
                        Ship = pas::list_at<aShip::TShip>(OtherStar->Ships, K);
                        if (Ship->TypeId == Self->TypeId && Ship->Name == Self->Name && Ship != Self) {
                            Used = true;
                            break;
                        }
                    }
                    if (Used) {
                        break;
                    }
                }
                if (!Used) {
                    break;
                }
                aMyFunction::IncrementWrapped(Index, 0, LastName);
                if (I == LastName) {
                    Self->Name = pas::concat_wide({Self->Name, u" ", u"-", pas::wide_int_to_str(aMyFunction::NextRandomIntRange(10, 99, Self->RandomState)), u"-"});
                }
            }
        };
        auto RandomStationEquipmentSize = [&](std::int32_t BaseSize) -> std::int32_t {
            std::int32_t round = System::Round(static_cast<long double>(BaseSize) * aConst::EquipmentSizeFactors[1] * 2.0L);
            std::int32_t round_2 = System::Round(static_cast<long double>(BaseSize) * aConst::EquipmentSizeFactors[2] * 2.0L);
            return aMyFunction::NextRandomIntRange(round_2, round, Self->RandomState);
        };
        if (StationType == aGalaxyStruct::rstCustomStation) {
            Self->TypeId = static_cast<std::uint8_t>(aGalaxyStruct::rstRangerCenter);
        } else {
            pas::store_unaligned<aGalaxyStruct::TStationType>(&Self->TypeId, StationType);
        }
        Self->TypeNameOverrideKey = TypeNameOverride;
        Self->CurrentStar = Star;
        pas::list_add(Self->CurrentStar->Ships, reinterpret_cast<void*>(Self));
        Self->HomePlanet = nullptr;
        Self->CurrentPlanet = nullptr;
        if (aGalaxy::Galaxy->CurrentTurn < 300 && aPlayer::GetPlayer()->CurrentStar->Constellation == Star->Constellation) {
            Self->PilotRace = StationPilotRaces[Self->TypeId][0];
        } else if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.5L) {
            Self->PilotRace = StationPilotRaces[Self->TypeId][0];
        } else {
            Self->PilotRace = StationPilotRaces[Self->TypeId][1];
        }
        Self->OwnerId = aConst::RaceToOwner(Self->PilotRace);
        Self->RandomizePosition();
        Self->Name = pas::WideString();
        SelectStationName(GR_Main::ModRuinNameConfig);
        if (Self->GetName().length() == 0) {
            SelectStationName(GR_Main::LanguageDataConfig->GetBlock(u"RuinName"_wref.get()));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
            pas::list_add(Self->RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(*([&] {
                auto cpp_index = aConst::RaceToOwner(Ranger->PilotRace) & 0x0000007f;
                auto* cpp_array = &aConst::OwnerRelations[aConst::RaceToOwner(Self->PilotRace) & 0x0000007f];
                return &(*cpp_array)[cpp_index];
            }())))));
        }
        Self->GenerateCombatSkills();
        Self->virtual_TShip_RefreshCurrentStanding();
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->RuinsProxy != Self) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"RuinsCreated"_w, nullptr);
            Event->AddData(StationType);
            Event->AddData(Self->Id);
            Event->AddData(Star->Id);
            Event->AddTextData(Self->Name);
            Event->AddTextData(TypeNameOverride);
        }
        Self->ChameleonActive = false;
        Self->GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        aRuins::TRuins_RefreshShopInventory(Self);
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            Self->ShopGoods[Good].Count = System::Round(static_cast<long double>(aConst::GoodsMarket[Good].BaseStock) * aConst::StationGoodsFactors[Self->TypeId][Good].StockFactor);
            Self->ShopGoods[Good].PriceState = aConst::GoodsMarket[Good].AveragePrice;
            Self->ShopGoods[Good].PurchasePrice = System::Round(Self->ShopGoods[Good].PriceState);
            Self->ShopGoods[Good].BaseSalePrice = System::Round(Self->ShopGoods[Good].PriceState * 0.98L - 1.0L);
        }
        if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            EquipmentOwner = 7;
        } else {
            EquipmentOwner = Self->OwnerId;
        }
        {
            std::uint8_t ownerId = Self->OwnerId;
            std::uint8_t cpp_arg = Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates;
            std::int32_t cpp_left_2 = aMyFunction::NextRandomIntRange(StationHullGeneration[Self->TypeId].MinSize, StationHullGeneration[Self->TypeId].MaxSize, Self->RandomState);
            pas::Extended cpp_left = cpp_left_2 + aGalaxy::Galaxy->ScaleIntByTechLevel(0, StationHullGeneration[Self->TypeId].TechSizeBonus);
            std::uint16_t roundAndTruncateToTens = aMyFunction::RoundAndTruncateToTens(cpp_left * aConst::HullCapacityScale);
            std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(StationHullGeneration[Self->TypeId].MinLevel, StationHullGeneration[Self->TypeId].MaxLevel, Self->RandomState);
            aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel = galaxy->ScaleIntByTechLevel(1, nextRandomIntRange);
            aShip::TShip* self = Self;
            aShip::TShip_CreateAndEquipHull(self, roundAndTruncateToTens, scaleIntByTechLevel, ownerId, -1, cpp_arg);
        }
        {
            std::int32_t randomStationEquipmentSize = RandomStationEquipmentSize(aConst::FuelTanksBaseSize);
            aShip::TShip* self_2 = Self;
            self_2->CreateAndEquipFuelTanks(randomStationEquipmentSize, 1, EquipmentOwner);
        }
        {
            std::int32_t randomStationEquipmentSize_2 = RandomStationEquipmentSize(aConst::EngineBaseSize);
            aShip::TShip* self_3 = Self;
            self_3->CreateAndEquipEngine(randomStationEquipmentSize_2, 1, EquipmentOwner);
        }
        {
            std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(StationDefenseLevels[Self->TypeId].Minimum, StationDefenseLevels[Self->TypeId].Maximum, Self->RandomState);
            aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel_2 = galaxy_2->ScaleIntByTechLevel(1, nextRandomIntRange_2);
            std::int32_t randomStationEquipmentSize_3 = RandomStationEquipmentSize(aConst::DefGeneratorBaseSize);
            aShip::TShip* self_4 = Self;
            self_4->CreateAndEquipDefGenerator(randomStationEquipmentSize_3, scaleIntByTechLevel_2, EquipmentOwner);
        }
        {
            std::int32_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(StationRepairLevels[Self->TypeId].Minimum, StationRepairLevels[Self->TypeId].Maximum, Self->RandomState);
            aGalaxy::TGalaxy* galaxy_3 = aGalaxy::Galaxy;
            std::uint8_t scaleIntByTechLevel_3 = galaxy_3->ScaleIntByTechLevel(1, nextRandomIntRange_3);
            std::int32_t randomStationEquipmentSize_4 = RandomStationEquipmentSize(aConst::RepairRobotBaseSize);
            aShip::TShip* self_5 = Self;
            self_5->CreateAndEquipRepairRobot(randomStationEquipmentSize_4, scaleIntByTechLevel_3, EquipmentOwner);
        }
        if (aConst::WeaponInfos[StationWeaponTypes[Self->TypeId][2]].TechLevel <= aGalaxy::Galaxy->TechLevel && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.6L) {
            Weapon = ([&] {
                std::int32_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(StationWeaponGeneration[Self->TypeId].AdvancedLevel + 1, 8, Self->RandomState);
                std::int32_t advancedLevel = StationWeaponGeneration[Self->TypeId].AdvancedLevel;
                aGalaxy::TGalaxy* galaxy_4 = aGalaxy::Galaxy;
                std::uint8_t scaleIntByTechLevel_4 = galaxy_4->ScaleIntByTechLevel(advancedLevel, nextRandomIntRange_4);
                std::int32_t randomStationEquipmentSize_5 = RandomStationEquipmentSize(aConst::WeaponInfos[StationWeaponTypes[Self->TypeId][2]].AverageSize);
                std::uint8_t cpp_arg_2 = StationWeaponTypes[Self->TypeId][2];
                aShip::TShip* self_6 = Self;
                return self_6->CreateAndEquipWeapon(cpp_arg_2, randomStationEquipmentSize_5, scaleIntByTechLevel_4, EquipmentOwner);
            }());
        } else if (aConst::WeaponInfos[StationWeaponTypes[Self->TypeId][1]].TechLevel <= aGalaxy::Galaxy->TechLevel && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.6L) {
            Weapon = ([&] {
                std::int32_t nextRandomIntRange_5 = aMyFunction::NextRandomIntRange(StationWeaponGeneration[Self->TypeId].IntermediateLevel + 1, 8, Self->RandomState);
                std::int32_t intermediateLevel = StationWeaponGeneration[Self->TypeId].IntermediateLevel;
                aGalaxy::TGalaxy* galaxy_5 = aGalaxy::Galaxy;
                std::uint8_t scaleIntByTechLevel_5 = galaxy_5->ScaleIntByTechLevel(intermediateLevel, nextRandomIntRange_5);
                std::int32_t randomStationEquipmentSize_6 = RandomStationEquipmentSize(aConst::WeaponInfos[StationWeaponTypes[Self->TypeId][1]].AverageSize);
                std::uint8_t cpp_arg_3 = StationWeaponTypes[Self->TypeId][1];
                aShip::TShip* self_7 = Self;
                return self_7->CreateAndEquipWeapon(cpp_arg_3, randomStationEquipmentSize_6, scaleIntByTechLevel_5, EquipmentOwner);
            }());
        } else {
            Weapon = ([&] {
                std::int32_t nextRandomIntRange_6 = aMyFunction::NextRandomIntRange(StationWeaponGeneration[Self->TypeId].BasicLevel + 1, 8, Self->RandomState);
                std::int32_t basicLevel = StationWeaponGeneration[Self->TypeId].BasicLevel;
                aGalaxy::TGalaxy* galaxy_6 = aGalaxy::Galaxy;
                std::uint8_t scaleIntByTechLevel_6 = galaxy_6->ScaleIntByTechLevel(basicLevel, nextRandomIntRange_6);
                std::int32_t randomStationEquipmentSize_7 = RandomStationEquipmentSize(aConst::WeaponInfos[StationWeaponTypes[Self->TypeId][0]].AverageSize);
                std::uint8_t cpp_arg_4 = StationWeaponTypes[Self->TypeId][0];
                aShip::TShip* self_8 = Self;
                return self_8->CreateAndEquipWeapon(cpp_arg_4, randomStationEquipmentSize_7, scaleIntByTechLevel_6, EquipmentOwner);
            }());
        }
        Weapon->Range = std::max<std::int32_t>(Weapon->Range, StationWeaponGeneration[Self->TypeId].MinimumRange);
        if (Self->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion)) {
            Self->CurrentStar->Dominion = Self;
            {
                std::int32_t nextRandomIntRange_7 = aMyFunction::NextRandomIntRange(5, 8, Self->RandomState);
                aGalaxy::TGalaxy* galaxy_7 = aGalaxy::Galaxy;
                std::uint8_t scaleIntByTechLevel_7 = galaxy_7->ScaleIntByTechLevel(2, nextRandomIntRange_7);
                std::int32_t randomStationEquipmentSize_8 = RandomStationEquipmentSize(aConst::CargoHookBaseSize);
                aShip::TShip* self_9 = Self;
                Hook = self_9->CreateAndEquipCargoHook(randomStationEquipmentSize_8, scaleIntByTechLevel_7, EquipmentOwner);
            }
            Hook->Range = std::max<std::int32_t>(Hook->Range, 200);
        }
        if (Self->GetCargoFreeSpace() < 0) {
            Self->GetHull()->Weight += pas::abs(Self->GetCargoFreeSpace());
        }
        Self->RefreshDerivedStats(true);
        Self->NodeReserve = 0;
        pas::store_unaligned<aGalaxyStruct::TStationType>(&Self->TypeId, StationType);
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
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
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
            EC_Buf::TBufEC_GetByte(Buffer);
        }
        std::int32_t Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err"_a));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
            Item->LoadFromBuffer(Buffer, Galaxy);
        }
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            ShopGoods[Good].Count = EC_Buf::TBufEC_GetInt32(Buffer);
            ShopGoods[Good].PriceState = EC_Buf::TBufEC_GetSingle(Buffer);
            ShopGoods[Good].PurchasePrice = EC_Buf::TBufEC_GetInt32(Buffer);
            ShopGoods[Good].BaseSalePrice = EC_Buf::TBufEC_GetInt32(Buffer);
        }
        if (GlobalsV::LoadedSaveVersion >= 107) {
            RelocationAge = EC_Buf::TBufEC_GetInt32(Buffer);
        }
        FlyToStar = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        FlyDate = EC_Buf::TBufEC_GetInt32(Buffer);
        if (SatelliteOffer != nullptr) {
            pas::free(SatelliteOffer);
            SatelliteOffer = nullptr;
        }
        SatelliteOffer = pas::construct_call<aItem::TSatellite>(aItem::TEquipment_Create);
        SatelliteOffer->LoadFromBuffer(Buffer, Galaxy);
        SatelliteOffer->TargetPlanet = nullptr;
        ModernizationSponsor = EC_Buf::TBufEC_GetBoolean(Buffer);
        SpecialServiceActive = EC_Buf::TBufEC_GetBoolean(Buffer);
        NoLanding = EC_Buf::TBufEC_GetBoolean(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 83) {
            ShopUpdateMode = static_cast<aGalaxyStruct::TShopUpdateMode>(EC_Buf::TBufEC_GetByte(Buffer));
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
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].Count)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesa"_w), Text);
        Text = pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[0]].PurchasePrice);
        for (I = 1; I <= 7; ++I) {
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].PurchasePrice)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesaSrakloe"_w), Text);
        Text = pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[0]].BaseSalePrice);
        for (I = 1; I <= 7; ++I) {
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].BaseSalePrice)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesaBruhy"_w), Text);
        EC_BlockPar::TBlockParEC* ShopBlock = Block->AddBlockByPath(EC_Str::DecodeTextW(u"EdqeSahloEp"_w));
        if (EquipmentShop != nullptr && pas::list_count(EquipmentShop) > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
                Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                Item->SaveToBlock(ShopBlock->AddBlockByPath(Text));
            }
        } else if (aPlayer::GetPlayer()->DockedTo == this && fEquipmentShop::TemporaryShopSlots != nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_2.next(I); ) {
                Slot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I);
                Item = Slot->Item;
                if (Item != nullptr) {
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    Item->SaveToBlock(ShopBlock->AddBlockByPath(Text));
                }
            }
        }
        ShopBlock->AddParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w), u""_wref.get());
        // Decoded: 'Storage'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->AddBlockByPath(EC_Str::DecodeTextW(u"Sataokrgalgae"_w));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range_3.next(I); ) {
                if (pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I)->LocationOwner == this) {
                    Item = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I)->Item;
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    Item->SaveToBlock(cpp_with->AddBlockByPath(Text));
                }
            }
            cpp_with->AddParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w), u""_wref.get());
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
        Text = Block->GetParam(EC_Str::DecodeTextW(u"SihrolpaGloiordes"_w));
        for (I = 0; I <= 7; ++I) {
            ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get())));
        }
        Text = Block->GetParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesaSrakloe"_w));
        for (I = 0; I <= 7; ++I) {
            ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].PurchasePrice = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get())));
        }
        Text = Block->GetParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesaBruhy"_w));
        for (I = 0; I <= 7; ++I) {
            ShopGoods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(I)]].BaseSalePrice = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get())));
        }
        EC_BlockPar::TBlockParEC* ShopBlock = Block->GetBlockByPath(EC_Str::DecodeTextW(u"EdqeSahloEp"_w));
        if (EquipmentShop != nullptr && pas::list_count(EquipmentShop) > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
                Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                Item->virtual_TItem_LoadFromBlock(ShopBlock->GetBlockByPath(Text));
            }
        } else if (aPlayer::GetPlayer()->DockedTo == this && fEquipmentShop::TemporaryShopSlots != nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_2.next(I); ) {
                Slot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I);
                Item = Slot->Item;
                if (Item != nullptr) {
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    Item->virtual_TItem_LoadFromBlock(ShopBlock->GetBlockByPath(Text));
                }
            }
        }
        Text = ShopBlock->GetParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w));
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) - 1); cpp_range_3.next(I); ) {
            Name = EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get());
            for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(75); ++Kind) {
                if (aConst::ItemTypeNames[Kind] == Name) {
                    if (pas::in_range(Kind, 42, 68)) {
                        Item = aItem::CreateDefaultItemByType(static_cast<aConst::TItemType>(Kind));
                        if (Item != nullptr) {
                            if (fEquipmentShop::TemporaryShopSlots != nullptr && fEquipmentShop::TemporaryShopStation == this) {
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
        // Decoded: 'Storage'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->GetBlockByPath(EC_Str::DecodeTextW(u"Sataokrgalgae"_w));
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range_4.next(I); ) {
                if (pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I)->LocationOwner == this) {
                    Item = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I)->Item;
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    Item->virtual_TItem_LoadFromBlock(cpp_with->GetBlockByPath(Text));
                }
            }
            Text = cpp_with->GetParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w));
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) - 1); cpp_range_5.next(I); ) {
                Name = EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get());
                for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(75); ++Kind) {
                    if (aConst::ItemTypeNames[Kind] == Name) {
                        if (pas::in_range(Kind, 0, 7) || pas::in_range(Kind, 42, 68) || pas::in_range(Kind, 10, 41) || pas::in_range(Kind, 69, 73)) {
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
        Self->FlyToStar = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->FlyToStar)))));
    }

    void TRuins_NextDay(TRuins* Self) {
        aShip::TShip_NextDay(Self);
        if (Self->ScriptShip != nullptr && Self->HasScriptControl()) {
            aShip::TShip_ScriptNextDay(Self);
            if (Self->ScriptShip != nullptr) {
                return;
            }
        }
        Self->virtual_TShip_NextDayLogic();
        if (Self->ScriptShip != nullptr && static_cast<std::uint8_t>(Self->HasScriptControl() ^ 1)) {
            aShip::TShip_ScriptNextDay(Self);
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
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                if (Star->Dominion == Self) {
                    return Star;
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
                Self->virtual_TShip_RepairBrokenEquipmentAtLocation();
                Self->ReloadWeapons();
                if (aGalaxy::Galaxy->IsStationShopUpdateEnabled() && static_cast<std::uint8_t>(Self->ModernizationSponsor ^ 1)) {
                    aShip::TShip_BuyEquipmentAtLocation(Self, false);
                    Self->virtual_TShip_RepairBrokenEquipmentAtLocation();
                }
            }
            Stage = 3;
            Self->virtual_TShip_AssignWeaponTargetsInStar();
            Stage = 4;
            aShip::TShip_QueueItemsWithinPickupRange(Self);
            Stage = 5;
            if (Self->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase)) {
                Stage = 6;
                if (aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) && aGalaxy::Galaxy->CurrentTurn % 60 == 0 && Self->DaysSincePlayerSeen > 30 && Self->NodeReserve < std::max<std::int32_t>(500, aPlayer::GetPlayer()->TotalExperience / 10) && Self->NodeReserve < pas::real_divide(2.0E+2L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor)) {
                    std::int32_t cpp_step = aMyFunction::NextRandomIntRange(0, System::Round(pas::real_divide(2.5E+2L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor)), Self->RandomState);
                    std::int32_t& cpp_target = Self->NodeReserve;
                    cpp_target += cpp_step;
                }
            } else if (Self->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion)) {
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
                        if (static_cast<std::uint8_t>(Self->TryRelocateToPirateStar() ^ 1) && static_cast<std::uint8_t>(Self->TryStartAbductionCycle() ^ 1)) {
                            Self->TryRepositionInStar();
                        }
                    }
                } else {
                    if (Self->CurrentStar == Self->FlyToStar) {
                        Self->FlyToStar = nullptr;
                        Self->FlyDate = aGalaxy::Galaxy->CurrentTurn + 30;
                    }
                    {
                        pas::Extended cpp_right = pas::real_max<float>(0.001f, Self->CurrentStar->GetCachedFactionStrength(aGalaxyStruct::sfPirates));
                        Imbalance = pas::real_divide(std::fabs(([&] {
                            pas::Extended cpp_right_2 = Self->CurrentStar->GetCachedFactionStrength(aGalaxyStruct::sfDominators) * 2.5L;
                            return Self->CurrentStar->GetCachedFactionStrength(aGalaxyStruct::sfCoalition) - cpp_right_2;
                        }())), cpp_right);
                    }
                    LocalBalance = Self->EvaluateLocalForceBalance(Self->Position);
                    if ((LocalBalance < -1.5E+2L || Imbalance > 3.0E+1L) && Self->FlyDate < aGalaxy::Galaxy->CurrentTurn + 25 && Self->FlyDate > aGalaxy::Galaxy->CurrentTurn) {
                        Self->FlyToStar = FindDominionHomeStar();
                        if (Self->FlyToStar != nullptr) {
                            Self->FlyDate = aGalaxy::Galaxy->CurrentTurn + 1;
                        } else {
                            Self->FlyDate = 0;
                        }
                    } else if (Imbalance > 1.5L || LocalBalance < -8.0L) {
                        Self->FlyDate = std::min<std::int32_t>(Self->FlyDate, std::max<std::int32_t>(Self->FlyDate - 15, aGalaxy::Galaxy->CurrentTurn + 4));
                    } else if (Imbalance > 1.37L || LocalBalance < -4.0L) {
                        Self->FlyDate = std::min<std::int32_t>(Self->FlyDate, std::max<std::int32_t>(Self->FlyDate - 7, aGalaxy::Galaxy->CurrentTurn + 4));
                    } else if (Imbalance > 1.25L || LocalBalance < -2.0L) {
                        Self->FlyDate = std::min<std::int32_t>(Self->FlyDate, std::max<std::int32_t>(Self->FlyDate - 3, aGalaxy::Galaxy->CurrentTurn + 4));
                    } else if (Imbalance > 1.12L || LocalBalance < -1.0L) {
                        Self->FlyDate = std::min<std::int32_t>(Self->FlyDate, std::max<std::int32_t>(Self->FlyDate - 1, aGalaxy::Galaxy->CurrentTurn + 4));
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
            aRuins::TRuins_RefreshShopInventory(Self);
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
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TRuins.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
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
            return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", GetTypeNameKey()})), Separator, Name});
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
            return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", GetTypeNameKey(), u"Small"})), u" ", aMyFunction::WrapTextInColor(Name, ColorTag)});
        }
        return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", TypeNameOverrideKey, u"Small"})), u" ", aMyFunction::WrapTextInColor(Name, ColorTag)});
    }

    std::uint8_t TRuins::GetGreetingShipCategory() {
        return aGalaxyStruct::gscTransport;
    }

    // Always rcTrader.
    aGalaxyStruct::TRangerCareer TRuins::GetDominantCareer() {
        return aGalaxyStruct::rcTrader;
    }

    aGalaxy::TStar* TRuins::GetHomeStar() {
        return nullptr;
    }

    std::uint8_t TRuins::GetStrengthScaledPirateStatus() {
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
        std::int32_t Level = aMyFunction::NextRandomIntRange(0, System::Round(aGalaxy::Galaxy->ScaleIntByTechLevel(1, 5)), RandomState);
        {
            std::uint32_t randomState = RandomState;
            std::uint8_t pickRandomEquipmentOwner = aConst::PickRandomEquipmentOwner(RandomState);
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
    void TRuins_RepairBrokenEquipmentAtLocation(TRuins* Self) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TEquipment* Artefact{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Self->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Self->Inventory, I);
            if ((!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && aShip::TShip_CanRepairEquipmentTech(Self, Item) && (Item->BrokenFlag != 0 || Item->ConditionPercent < 1.0L)) {
                Item->BrokenFlag = 0;
                if (Self->ModernizationSponsor) {
                    Item->ConditionPercent = 1.0;
                } else {
                    Item->Repair();
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Artefacts) - 1); cpp_range_2.next(I); ) {
            Artefact = pas::list_at<aItem::TEquipment>(Self->Artefacts, I);
            if (Artefact->BrokenFlag != 0 || Artefact->ConditionPercent < 1.0L) {
                Artefact->BrokenFlag = 0;
                if (Self->ModernizationSponsor) {
                    Artefact->ConditionPercent = 1.0;
                } else {
                    Artefact->Repair();
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
                    if (pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Ammo < Weapon->AmmoCapacity) {
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
    void TRuins_RefreshShopInventory(TRuins* Self) {
        std::int32_t I{};
        std::int32_t Attempts{};
        aItem::TEquipment* Item{};
        std::uint8_t Kind{};
        if (pas::is_one_of<aGalaxyStruct::sumDisabled, aGalaxyStruct::sumGoodsOnly>(Self->ShopUpdateMode)) {
            return;
        }
        if (aGalaxy::Galaxy->CurrentTurn > Self->CreationTurn + 1 && static_cast<std::int32_t>(Self->Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 7 != 0) {
            return;
        }
        std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(Self->CurrentStar->Planets) - 1, Self->RandomState);
        aMyFunction::TObjectList* planets = Self->CurrentStar->Planets;
        aPlanet::TPlanet* Planet = pas::list_at<aPlanet::TPlanet>(planets, nextRandomIntRange);
        if (Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
            Planet = nullptr;
        }
        {
            std::int32_t cpp_left = pas::list_count(Self->EquipmentShop);
            if (cpp_left >= Self->CalculateEquipmentShopTargetCount()) {
                if (Planet != nullptr) {
                    I = aMyFunction::SeededRandomIntRange(0, pas::list_count(Self->EquipmentShop) - 1, Self->Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn));
                    Item = pas::list_at<aItem::TEquipment>(Self->EquipmentShop, I);
                    if (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") {
                        pas::list_delete(Self->EquipmentShop, I);
                        pas::free(Item);
                    }
                }
            }
        }
        std::int32_t Added = 0;
        while (([&] {
            pas::Extended cpp_left_2 = pas::list_count(Self->EquipmentShop);
            return cpp_left_2 <= Self->CalculateEquipmentShopTargetCount() * 0.7L;
        }()) || ([&] {
            std::int32_t cpp_left_3 = pas::list_count(Self->EquipmentShop);
            return cpp_left_3 <= Self->CalculateEquipmentShopTargetCount();
        }()) && Planet != nullptr) {
            if (Planet == nullptr) {
                Planet = Self->FindFirstInhabitedPlanetInStar();
            }
            ++Added;
            if (Added > 10) {
                break;
            }
            Attempts = 0;
            do {
                ++Attempts;
                Kind = aMyFunction::SeededRandomIntRange(42, 50, Self->Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) * 175 + static_cast<std::uint32_t>(Attempts));
            } while (!(Attempts > 20 || Self->CountEquipmentShopItems(Kind) < pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::StationEquipmentOfferQuotas[Self->TypeId], (Kind - 42) * sizeof(std::int32_t)))));
            Item = aRuins::TRuins_GenerateEquipmentOffer(Self, aPlayer::GetPlayer(), Planet, Kind);
            if (Item != nullptr) {
                pas::list_add(Self->EquipmentShop, reinterpret_cast<void*>(Item));
                Self->RemoveSimilarShopItem(Item);
            }
        }
    }

    // Returns 10..18; advances the station RNG state.
    std::int32_t TRuins::CalculateEquipmentShopTargetCount() {
        std::uint8_t Kind{};
        std::int32_t Count = 0;
        for (Kind = static_cast<std::uint8_t>(42); Kind <= static_cast<std::uint8_t>(50); ++Kind) {
            Count += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::StationEquipmentOfferQuotas[TypeId], (Kind - 42) * sizeof(std::int32_t)));
        }
        std::int32_t Result = System::Round(Count + aMyFunction::NextRandomIntRange(-2, 2, RandomState));
        return std::max<std::int32_t>(10, std::min<std::int32_t>(Result, 18));
    }

    // Bucket 50 includes all weapon item types 50..68.
    std::int32_t TRuins::CountEquipmentShopItems(std::uint8_t ItemType) {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
            // The t_Weapon1 shop bucket counts every weapon subtype.
            if (static_cast<std::uint8_t>(Item->ItemType) == ItemType || pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && ItemType == static_cast<std::uint8_t>(aConst::t_Weapon1)) {
                ++Count;
            }
        }
        return Count;
    }

    // Excludes hulls; inclusive cost bounds. Borrowed result, nil when absent.
    aItem::TItem* TRuins::FindMostExpensiveShopItem(std::int32_t MinCost, std::int32_t MaxCost) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aItem::TItem* Best = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(EquipmentShop, I);
            if (Item->ItemType != aConst::t_Hull && Item->Cost <= MaxCost && Item->Cost >= MinCost) {
                if (Best == nullptr) {
                    Best = Item;
                } else if (reinterpret_cast<aItem::TEquipment*>(Item)->Cost > Best->Cost) {
                    Best = Item;
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
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(I); ) {
            Existing = pas::list_at<aItem::TEquipment>(EquipmentShop, I);
            if (Existing->ItemType == Item->ItemType && Existing != Item && (Existing->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Existing->ScriptItem)->Name == u"")) {
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
                        if (pas::in_range(Existing->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
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
        return Result;
    }

    // Returns a zero-based module index or -1; advances the station RNG.
    std::int32_t TRuins::SelectEquipmentOfferSpecialMicroModule(aItem::TEquipment* Item, aPlanet::TPlanet* Planet) {
        std::int32_t I{};
        std::int32_t Candidate{};
        std::int32_t Result = -1;
        {
            pas::Extended cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = System::Round(pas::real_divide(Planet->InventionLevels[7] * 100, 8.0L));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(I); ) {
            do {
                if (!Template->SpecialOnly) {
                    break;
                }
                if (TypeNameOverrideKey != u"") {
                    if (Template->OfferStationNames != u"<Any>" && pas::pos(pas::concat_wide({u"<", TypeNameOverrideKey, u">"}), Template->OfferStationNames) <= 0) {
                        break;
                    }
                } else if (!pas::contains(pas::load_unaligned<aGalaxyStruct::TShipTypeMask>(&Template->OfferStationTypes), TypeId)) {
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
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Template) + static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo))));
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
            pas::Extended cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = System::Round(pas::real_divide(Planet->InventionLevels[7] * 100, 8.0L));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(I); ) {
            do {
                if (!Template->SpecialOnly) {
                    break;
                }
                if (TypeNameOverrideKey != u"") {
                    if (Template->OfferStationNames != u"<Any>" && pas::pos(pas::concat_wide({u"<", TypeNameOverrideKey, u">"}), Template->OfferStationNames) <= 0) {
                        break;
                    }
                } else if (!pas::contains(pas::load_unaligned<aGalaxyStruct::TShipTypeMask>(&Template->OfferStationTypes), TypeId)) {
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
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Template) + static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo))));
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
            pas::Extended cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = System::Round(pas::real_divide(Planet->InventionLevels[7] * 100, 8.0L));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(I); ) {
            do {
                if (!Template->SpecialOnly) {
                    break;
                }
                if (TypeNameOverrideKey != u"") {
                    if (Template->OfferStationNames != u"<Any>" && pas::pos(pas::concat_wide({u"<", TypeNameOverrideKey, u">"}), Template->OfferStationNames) <= 0) {
                        break;
                    }
                } else if (!pas::contains(pas::load_unaligned<aGalaxyStruct::TShipTypeMask>(&Template->OfferStationTypes), TypeId)) {
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
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Template) + static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo))));
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

    // Always false.
    std::uint8_t TRuins_CanQueueReachablePlanet(TRuins* Self, aPlanet::TPlanet* Planet) {
        return false;
    }

    // Clears EnemyShip.
    void TRuins::SelectEnemyShipInStar() {
        EnemyShip = nullptr;
    }

    // Empty.
    void TRuins::EngageEnemyShip() {
    }

    // Targets hostile ships, incoming missiles and nearby asteroids.
    void TRuins_AssignWeaponTargetsInStar(TRuins* Self) {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        aAsteroid::TAsteroid* Asteroid{};
        float DistanceSquared{};
        aMissile::TMissile* Missile{};
        std::int32_t Assigned = 0;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Self->Weapons[I];
                    Weapon->Target = nullptr;
                }
            }
        }
        if (aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar || aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.7L || aGalaxy::Galaxy->IsFullStationTargetingEnabled() || Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators || Self->CurrentStar->Status.CustomFaction != u"") {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                if ((Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || aShip::TShip_RelationToShip(Ship, Self) < 10) && Ship->InNormalSpace() && (static_cast<std::uint8_t>(Self->HasIndependentScriptFaction() ^ 1) || static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1) || reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText != reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText)) {
                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Self->WeaponCount);
                    if (1 <= cpp_last_2) {
                        for (J = 1; J <= cpp_last_2; ++J) {
                            Weapon = Self->Weapons[J];
                            if (Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon) && ([&] {
                                pas::Extended cpp_right = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                                return aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) <= cpp_right;
                            }())) {
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
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Missiles) - 1); cpp_range_2.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(Self->CurrentStar->Missiles, I);
            if (Missile->OwnerShip != Self && Missile->Target == Self) {
                const std::int32_t cpp_last_3 = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last_3) {
                    for (J = 1; J <= cpp_last_3; ++J) {
                        Weapon = Self->Weapons[J];
                        if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon) && ([&] {
                            pas::Extended cpp_right_2 = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                            return aMyFunction::PointDistanceSquared(Self->Position, Missile->Position) <= cpp_right_2;
                        }())) {
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
        if (pas::list_count(Self->CurrentStar->Items) < 10 && aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Asteroids) - 1); cpp_range_3.next(I); ) {
                Asteroid = pas::list_at<aAsteroid::TAsteroid>(Self->CurrentStar->Asteroids, I);
                DistanceSquared = aMyFunction::PointDistanceSquared(Self->Position, Asteroid->Position);
                if (DistanceSquared <= 1.0E+6L) {
                    const std::int32_t cpp_last_4 = static_cast<std::int32_t>(Self->WeaponCount);
                    if (1 <= cpp_last_4) {
                        for (J = 1; J <= cpp_last_4; ++J) {
                            Weapon = Self->Weapons[J];
                            if ((aMyFunction::NextRandomUnitFloat(Self->RandomState) <= 0.9L || Self->GetHullIntegrityPercent() <= 90) && static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && aShip::TShip_IsEquipmentUsable(Self, Weapon) && static_cast<long double>(pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon))) >= DistanceSquared) {
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
        if (RelocationAge < 135.0L) {
            return Result;
        }
        {
            std::int32_t cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left <= System::Round(aMyFunction::RemapClamped(RelocationAge, 135.0, 675.0, 1.0E+2, 95.0))) {
                return Result;
            }
        }
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (Ship->AbductedByPirateClan) {
                return Result;
            }
            if (Ship->InNormalSpace() && pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr && Ship->ScriptShip == nullptr && Ship->AbsoluteScriptOrder <= 0 && pas::is_one_of<aShip::soNone, aShip::soMove>(Ship->Order)) {
                ++Count;
            }
        }
        if (Count < 5) {
            return Result;
        }
        Count = 0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
            if (CurrentStar->StarDistances[I].Distance > 40) {
                break;
            }
            Star = CurrentStar->StarDistances[I].Star;
            if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"") {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (Ship->InNormalSpace() && Ship->Order == aShip::soJump && reinterpret_cast<aGalaxy::TStar*>(Ship->OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfCoalition && reinterpret_cast<aGalaxy::TStar*>(Ship->OrderTarget)->Status.CustomFaction == u"" && Ship->EstimateOrderTravelTurns() >= 2 && pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr && Ship->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::in_range(Ship->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stTransport) && Ship->ScriptShip == nullptr && Ship->AbsoluteScriptOrder <= 0) {
                        ++Count;
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
        aNormalShip::TNormalShip* Candidate = reinterpret_cast<aNormalShip::TNormalShip*>(Ship);
        if (Candidate->AbductedByPirateClan) {
            return;
        }
        if (pas::class_cast_if<aPirate::TPirate*>(Candidate) != nullptr) {
            return;
        }
        if (pas::in_range(Candidate->CurrentStanding, aGalaxyStruct::ssPiratePassive, aGalaxyStruct::ssPirateMilitary)) {
            return;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->QuestTargetDefendShip == Candidate && aMyFunction::NextRandomIntRange(1, 100, RandomState) > 70) {
            return;
        }
        if (aPlayer::GetPlayer() == Candidate) {
            if (aGalaxy::Galaxy->CurrentTurn < pas::real_divide(2.0E+2L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor) + 3.0E+2L) {
                return;
            }
            {
                pas::Extended cpp_left = aMyFunction::NextRandomIntRange(0, 100, RandomState);
                if (cpp_left * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor < 4.0E+1L) {
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
    void TRuins_ReportAbductionOutcome(TRuins* Self) {
        pas::WideString Text{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        std::int32_t Abducted = 0;
        std::int32_t Pirates = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
            if (Ship->AbductedByPirateClan) {
                ++Abducted;
            }
            if (Ship->InNormalSpace() && pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr && Ship->ScriptShip == nullptr && pas::is_one_of<aShip::soNone, aShip::soMove>(Ship->Order)) {
                ++Pirates;
            }
        }
        Text = pas::concat_wide({u"<color=255,240,100>", Self->GetFullName(u" "_wref.get()), u"</color>", u"\r\n"});
        if (Abducted == 0) {
            Text = pas::concat_wide_reverse({aShip::TShip_LookupTalkText(Self, u"Talk.PirateClan.RuinTalkAfterAbduct.Failure"_wref.get()), Text});
        } else if (Pirates == 0) {
            Text = pas::concat_wide_reverse({aShip::TShip_LookupTalkText(Self, u"Talk.PirateClan.RuinTalkAfterAbduct.SuccessNoPirate"_wref.get()), Text});
        } else {
            Text = pas::concat_wide_reverse({aShip::TShip_LookupTalkText(Self, u"Talk.PirateClan.RuinTalkAfterAbduct.Success"_wref.get()), Text});
        }
        {
            auto& cpp_target = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get())->Targets[0].ShipId;
            cpp_target = Self->Id;
        }
    }

    float TRuins::EvaluateLocalForceBalance(EC_Struct::TPointF Point) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        float RelationFactor{};
        float Result = 0.0f;
        RefreshDerivedStats(true);
        float HullFraction = pas::real_max<pas::Extended>(0.01L, pas::real_divide(GetHull()->HullPoints, GetHull()->Weight));
        float ReferenceStrength = pas::real_max<pas::Extended>(1.0L, pas::real_divide(Strength, UsableWeaponCount + 7) + aGalaxy::Galaxy->AverageRangerStrength * 0.1L * HullFraction);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            RelationFactor = 1.0f;
            if (Ship->DockedTo == this && (FlyToStar == nullptr || FlyToStar == CurrentStar)) {
                RelationFactor = 0.75f;
            } else if (!Ship->InNormalSpace()) {
                continue;
            }
            if (aShip::TShip_GetRelationLevelToShip(this, Ship) == aGalaxyStruct::rlHostile) {
                RelationFactor = -1.0L * RelationFactor;
            } else if (aShip::TShip_GetRelationLevelToShip(this, Ship) == aGalaxyStruct::rlExcellent) {
                RelationFactor = 1.0L * RelationFactor;
            } else if (aShip::TShip_GetRelationLevelToShip(this, Ship) == aGalaxyStruct::rlGood) {
                RelationFactor = 0.5L * RelationFactor;
            } else {
                continue;
            }
            if (RelationFactor > 0.0L) {
                RelationFactor = static_cast<long double>(RelationFactor) * HullFraction;
            }
            {
                pas::Extended cpp_right = pas::real_min<pas::Extended>(1.0L, pas::real_divide(Ship->Speed, pas::real_max<double>(1.0E+2, aMyFunction::PointDistance(Point, Ship->Position))));
                Result = Result + pas::real_divide(pas::real_divide(Ship->Strength, Ship->UsableWeaponCount + 7), ReferenceStrength) * RelationFactor * cpp_right;
            }
        }
        return Result;
    }

    float TRuins_EvaluateRelocationPosition(TRuins* Self, EC_Struct::TPointF Point) {
        std::int32_t I{};
        std::int32_t RangeSquared{};
        aItem::TItem* Item{};
        float Result = 0.0f;
        if (aShip::TShip_IsEquipmentUsable(Self, Self->GetCargoHook())) {
            RangeSquared = Self->GetCargoHookRangeSquared();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Items) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Self->CurrentStar->Items, I);
                if (aItem::CanCargoHookHandleItem(Item, Self) && static_cast<long double>(aMyFunction::PointDistanceSquared(Point, Item->Position)) <= RangeSquared && Self->AcceptPickupItem(Item)) {
                    Result = static_cast<long double>(Result) + Item->Cost;
                }
            }
        }
        Result = pas::real_divide(Result, std::max<std::int32_t>(10, aGalaxy::Galaxy->ComputeScaledMiniMoney(2)));
        Result = static_cast<long double>(Result) * aMyFunction::RemapClamped(Self->RelocationAge, 3.0E+1, 9.0E+1, 0.3, 1.0);
        if (Self->CurrentStar->Status.Battle != 0) {
            return static_cast<long double>(Result) + Self->EvaluateLocalForceBalance(Point);
        }
        return Result;
    }

    // Successful repositioning initiates a teleport.
    std::uint8_t TRuins::TryRepositionInStar() {
        static const pas::Set<0, 255> StationTypes = pas::constant_set<pas::Set<0, 255>>({{6, 13}});
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
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            if (pas::list_at<aShip::TShip>(CurrentStar->Ships, I)->AbductedByPirateClan && pas::list_at<aShip::TShip>(CurrentStar->Ships, I)->InHyperspace) {
                return Result;
            }
        }
        SavedPoint = Position;
        float InitialScore = aRuins::TRuins_EvaluateRelocationPosition(this, Position);
        float BestScore = InitialScore;
        for (I = 0; I <= 100; ++I) {
            RandomizePosition();
            Score = aRuins::TRuins_EvaluateRelocationPosition(this, Position);
            if (Score >= BestScore && DistanceToNearestShipByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) > 7.0E+2L) {
                BestScore = Score;
                BestPoint = Position;
            }
        }
        if (InitialScore > 0.0L && BestScore < InitialScore + 1.0E+1L || BestScore < InitialScore + 5.0L) {
            Position = SavedPoint;
            return Result;
        }
        float Radius = 1.0E+2f;
        for (I = 0; I <= 100; ++I) {
            Position.X = BestPoint.X + aMyFunction::NextRandomIntRange(-100, 100, RandomState) * 0.01L * Radius;
            Position.Y = BestPoint.Y + aMyFunction::NextRandomIntRange(-100, 100, RandomState) * 0.01L * Radius;
            if (DistanceToNearestShipByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) > 7.0E+2L && pas::sqr(static_cast<pas::Extended>(Position.X)) + pas::sqr(static_cast<pas::Extended>(Position.Y)) >= pas::sqr(static_cast<pas::Extended>(CurrentStar->SafeRadius)) * 1.1L) {
                Score = aRuins::TRuins_EvaluateRelocationPosition(this, Position);
                if (Score >= BestScore) {
                    BestScore = Score;
                    BestPoint = Position;
                    Radius = Radius * 0.75L;
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
        if (EvaluateLocalForceBalance(Position) > -1.0E+1L) {
            return Result;
        }
        pas::List* Candidates = pas::make_object<pas::List>();
        aGalaxy::TConstellation* Constellation = CurrentStar->Constellation;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(Constellation->Stars, I);
            if (Star != CurrentStar && Star->Status.ControlFaction == aGalaxyStruct::sfPirates && Star->Status.Battle == 0 && static_cast<std::uint8_t>(aScript::IsStarProtectedByScript(Star) ^ 1) && Star->Dominion == nullptr && Star->Status.CustomFaction == u"") {
                pas::list_add(Candidates, reinterpret_cast<void*>(Star));
            }
        }
        if (pas::list_count(Candidates) == 0) {
            pas::free(Candidates);
            return Result;
        }
        Star = pas::list_at<aGalaxy::TStar>(Candidates, aMyFunction::NextRandomIntRange(0, pas::list_count(Candidates) - 1, RandomState));
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
        if (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion)) {
            if (!pas::in_set<aConst::t_Food, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtDefToArms1, aConst::t_CustomWeapon, aConst::t_MicroModule, aConst::t_MicroModule>(Item->ItemType)) {
                return Result;
            }
        } else if (!(pas::in_set<aConst::t_Food, aConst::t_Narcotics, aConst::t_Hull, aConst::t_CustomWeapon>(Item->ItemType) && Item->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator))) {
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
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Module = pas::list_at<aItem::TItem>(Inventory, I);
                if (pas::class_cast_if<aItem::TMicroModule*>(Module) != nullptr) {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range_2.next(J); ) {
                        Item = pas::list_at<aItem::TEquipment>(EquipmentShop, J);
                        if (pas::checked_cast<aItem::TMicroModule*>(Module)->CanInstallOn(Item)) {
                            Applied = true;
                            aItem::ApplyMicroModule(pas::checked_cast<aItem::TMicroModule*>(Module)->MicroModuleIndex - 1, Item);
                            pas::list_delete(Inventory, pas::list_indexof(Inventory, reinterpret_cast<void*>(Module)));
                            pas::free(Module);
                            RefreshDerivedStats(true);
                            break;
                        }
                    }
                }
                if (Applied) {
                    break;
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
        std::uint8_t Result = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))))));
        if (static_cast<std::uint8_t>(NoLanding ^ 1) || aPlayer::GetPlayer() != Ranger) {
            return std::max<std::int32_t>(50, static_cast<std::int32_t>(Result));
        }
        return Result;
    }

    // Positive changes receive the ranger's Charisma bonus; stored relation is clamped to 0..100.
    void TRuins::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))));
        std::uint8_t Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index)));
        if (Amount > 0 && static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aShip::psCharisma, false) > 0) {
            Amount += System::Round((static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * Amount * 0.2L);
        }
        std::int32_t NewRelation = Relation + Amount;
        if (NewRelation < 0) {
            Relation = 0;
        } else if (NewRelation > 100) {
            Relation = 100;
        } else {
            Relation = NewRelation;
        }
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

    void TRuins::ReactToAttack(aShip::TShip* Attacker) {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        EnemyShip = Attacker;
        if (CurrentStanding == aGalaxyStruct::ssCustom) {
            return;
        }
        std::uint8_t Independent = static_cast<std::uint8_t>(pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::FactionStandingMasks[CurrentStar->Status.ControlFaction]), CurrentStanding) ^ 1) || CurrentStar->Status.CustomFaction != u"";
        if (Attacker->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(Attacker, -10);
            if (Independent) {
                return;
            }
            if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && aPlanet::MainPiratePlanet != nullptr) {
                aPlanet::MainPiratePlanet->ChangeRelationToRanger(Attacker, -10);
            } else if (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfDominators) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Planets) - 1); cpp_range.next(I); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                    if (Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                        Planet->ChangeRelationToRanger(Attacker, -10);
                    }
                }
            }
        }
        if (Attacker->PartnerShip != nullptr && Attacker->PartnerShip->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(Attacker->PartnerShip, -5);
            if (Independent) {
                return;
            }
            if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && aPlanet::MainPiratePlanet != nullptr) {
                aPlanet::MainPiratePlanet->ChangeRelationToRanger(Attacker->PartnerShip, -5);
            } else if (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfDominators) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Planets) - 1); cpp_range_2.next(I); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                    if (Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                        Planet->ChangeRelationToRanger(Attacker->PartnerShip, -5);
                    }
                }
            }
        }
        if (pas::class_cast_if<aTranclucator::TTranclucator*>(Attacker) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip, -10);
            if (Independent) {
                return;
            }
            if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && aPlanet::MainPiratePlanet != nullptr) {
                aPlanet::MainPiratePlanet->ChangeRelationToRanger(reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip, -10);
            } else if (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfDominators) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Planets) - 1); cpp_range_3.next(I); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                    if (Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                        Planet->ChangeRelationToRanger(reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip, -10);
                    }
                }
            }
        }
        if (static_cast<std::uint8_t>(Independent ^ 1) && pas::class_cast_if<aNormalShip::TNormalShip*>(Attacker) != nullptr && pas::load_unaligned<std::uint16_t>(pas::byte_offset(&static_cast<aNormalShip::TNormalShip*>(Attacker)->CurrentSystemKills, CurrentStar->Status.ControlFaction * sizeof(std::uint16_t))) == 0) {
            pas::store_unaligned<std::uint16_t>(pas::byte_offset(&pas::checked_cast<aNormalShip::TNormalShip*>(Attacker)->CurrentSystemKills, CurrentStar->Status.ControlFaction * sizeof(std::uint16_t)), static_cast<std::uint16_t>(1));
        }
    }

    // Returns false without changing InFear.
    std::uint8_t TRuins_RecomputeFearState(TRuins* Self) {
        return false;
    }

    std::uint8_t TRuins_AcceptsRansomDemandFrom(TRuins* Self, aShip::TShip* Ship) {
        return false;
    }

    std::uint8_t TRuins_TrustsAttackRequester(TRuins* Self, aShip::TShip* Ship) {
        return true;
    }

    std::uint8_t TRuins_EvaluateAllyRelationAndStrength(TRuins* Self, aShip::TShip* Ship) {
        return false;
    }

    void TRuins::ProcessCombatDialogue() {
    }

    void TRuins::ReactToExtortionDemand(void* Ranger) {
    }

    std::uint8_t TRuins_BuildMoneyExtortionResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins_BuildCargoExtortionResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins_BuildAttackRequestResponse(TRuins* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins_AcceptPartnershipOffer(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    std::uint8_t TRuins_BuildPartnershipOfferResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    // Script.GoodsRuinsForBuy. Restocks selected goods and sets prices near the global minimum.
    void TRuins::ForceGoodsForSale(aGalaxyStruct::TItemTypeMask GoodsMask) {
        std::uint8_t Good{};
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (pas::contains(GoodsMask, Good)) {
                ShopGoods[Good].PriceState = ([&] {
                    pas::Extended cpp_right = aMyFunction::NextRandomFloatRange(0.9, 1.1, RandomState);
                    return aConst::GoodsMarket[Good].MinPrice * cpp_right;
                }());
                ShopGoods[Good].Count = ([&] {
                    std::int32_t cpp_arg = ([&] {
                        std::int32_t cpp_right_2 = aMyFunction::NextRandomIntRange(1, aConst::GoodsMarket[Good].BaseStock / 10 + 1, RandomState);
                        return ShopGoods[Good].Count + cpp_right_2;
                    }());
                    std::int32_t cpp_arg_2 = aConst::GoodsMarket[Good].BaseStock / 10 + 1;
                    return std::max<std::int32_t>(cpp_arg, cpp_arg_2);
                }());
                ShopGoods[Good].PurchasePrice = System::Round(ShopGoods[Good].PriceState);
                ShopGoods[Good].BaseSalePrice = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(ShopGoods[Good].PriceState * 0.98L - 1.0L));
            }
        }
    }

    // Sets base Accuracy and Maneuverability to 0..5 using galaxy technology and war state.
    void TRuins::GenerateCombatSkills() {
        std::int32_t Level = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 0.0, 4.0));
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
        BaseSkills[0] = Accuracy;
        BaseSkills[1] = Maneuverability;
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
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
                if (pas::class_cast_if<TRuins*>(Ship) != nullptr && Ship != this) {
                    DistanceSquared = aMyFunction::PointDistanceSquared(Point, Ship->Position);
                    if (DistanceSquared <= 4.9E+5L) {
                        return Result;
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
                pas::Extended cpp_right = aMyFunction::NextRandomIntRange(0, 50, RandomState);
                Polar.Radius = static_cast<long double>(Planet->Radius) + Planet->Orbit.Radius + 1.0E+2L + cpp_right;
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

    float TRuins_EvaluateStatBonus(TRuins* Self, aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
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
                Result = static_cast<std::int8_t>(Self->GetRadar() == nullptr);
            } else if (cpp_case == aConst::bonScan) {
                Result = Value * 20 * (Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f);
            } else if (cpp_case == aConst::bonDroid) {
                Result = pas::real_divide(Value * 20, pas::real_max<float>(0.1f, Self->GetHull()->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags))));
            } else if (cpp_case == aConst::bonDef) {
                Result = pas::real_divide(pas::real_divide(Value * 8 * 100, std::max<std::int32_t>(5, 100 - Value)) * 45.0L, std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                Result = Value * 10;
            } else if (cpp_case == aConst::bonWSplinter) {
                Result = Value * 10;
            } else if (cpp_case == aConst::bonWMissile) {
                Result = Value * 10 * (0.1L + static_cast<std::int8_t>(aShip::TShip_GetRadarRange(Self) > 0) * 0.9L);
            } else if (cpp_case == aConst::bonWRadius) {
                Result = Value;
            } else if (cpp_case >= aConst::bonSkill1 && cpp_case <= aConst::bonSkill6) {
                if (Value > 0) {
                    Result = std::min<std::int32_t>(6 - (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f), Value) * StationSkillBonusWeights[BonusKind];
                }
                if (Value > 0 && Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) > 6) {
                    Result = Result + StationSkillBonusWeights[BonusKind] * 0.05L * (Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) - 6);
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f, -Value) * -StationSkillBonusWeights[BonusKind];
                }
                if (Value < 0 && Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) < 0) {
                    Result = Result + StationSkillBonusWeights[BonusKind] * 0.03L * (Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f));
                }
            }
        }
        if (Self->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion)) {
            switch (BonusKind) {
                case aConst::bonHook: {
                    return (pas::real_min<pas::Extended>(static_cast<pas::Extended>(Value), static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]) + Value * 0.1L) * 1.0L;
                }
                case aConst::bonHookRadius: return Value * 1.5L;
                default: return Result;
            }
        }
        return Result;
    }

    float TRuins_EvaluateWeaponDamage(TRuins* Self, aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> ShockFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkShock}});
        static const pas::Set<0, 255> AcidFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkAcid}});
        float ScannerFactor{};
        aGalaxyStruct::TDamageFlagSet Flags{};
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
            Result = Result * 1.5L;
        }
        if (pas::contains(Flags, aGalaxyStruct::dkShock)) {
            Result = Result * (1.05L + (Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ShockFlags)) & 0x0000007f) * 0.05L);
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
                std::int32_t cpp_left = Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(AcidFlags));
                Result = static_cast<long double>(Result) + cpp_left * Weapon->GetShotCount();
            }
            if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
                ShotTotal = 1;
                for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->CountEquippedWeapons() & 0x0000007f); cpp_range.next(I); ) {
                    ShotTotal += Self->Weapons[I]->GetShotCount();
                }
                Result = static_cast<long double>(Result) + ShotTotal * 2;
            }
        }
        switch (static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType)) {
            case aGalaxyStruct::wstRocket: {
                Result = Result * 1.1L * Weapon->GetShotCount() * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstMissile: {
                Result = Result * (1.1L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.5L * 0.01L + StatusFactor) * Weapon->GetShotCount();
                break;
            }
            case aGalaxyStruct::wstTorpedo: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.5L * 0.01L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstChain: {
                Result = Result * (1.1L + (Weapon->GetShotCount() - 1) * 0.2L) * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstSplash: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.0L * 0.01L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstAreaDamage: {
                Result = Result * (1.0L + Weapon->Range * 1.3L * 0.01L + StatusFactor);
                break;
            }
            default: Result = Result * (1.0L + StatusFactor); break;
        }
        Result = static_cast<long double>(Result) * Weapon->GetAttackCount();
        std::uint8_t HasOtherWeapon = false;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Self->CountEquippedWeapons() & 0x0000007f); cpp_range_2.next(I); ) {
            if (!pas::in_range(Self->Weapons[I]->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                HasOtherWeapon = true;
            }
        }
        if (static_cast<std::uint8_t>(HasOtherWeapon ^ 1) && pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            return Result * 0.5L;
        }
        return Result;
    }

    // Script state can force standing 9 or preserve the current value; type 13 has no default assignment.
    void TRuins_RefreshCurrentStanding(TRuins* Self) {
        std::int32_t StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            if (Self->TypeId != static_cast<std::uint8_t>(aGalaxyStruct::rstCustomStation)) {
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
            Buyer = reinterpret_cast<aShip::TShip*>(Ship);
            Result = Planet->GenerateHullOffer(reinterpret_cast<aShip::TShip*>(Ship));
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
        std::uint8_t HullType{};
        std::uint8_t Owner{};
        std::int32_t Series{};
        std::int32_t ModuleIndex{};
        std::uint8_t Flagship{};
        aItem::THull* Result = nullptr;
        if (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstCustomStation)) {
            return GeneratePlanetHullOffer(Ship, Planet);
        }
        if (Ship != nullptr && pas::class_cast_if<aShip::TShip*>(Ship) != nullptr) {
            Buyer = reinterpret_cast<aShip::TShip*>(Ship);
            Count = 0;
            for (HullType = static_cast<std::uint8_t>(aGalaxyStruct::htRanger); HullType <= static_cast<std::uint8_t>(aGalaxyStruct::htFlagship); ++HullType) {
                if (pas::contains(StationOfferHullTypes[TypeId], HullType)) {
                    ++Count;
                }
            }
            if (Count == 0) {
                return Result;
            }
            Count = aMyFunction::NextRandomIntRange(1, Count, RandomState);
            for (HullType = static_cast<std::uint8_t>(aGalaxyStruct::htRanger); HullType <= static_cast<std::uint8_t>(aGalaxyStruct::htFlagship); ++HullType) {
                if (pas::contains(StationOfferHullTypes[TypeId], HullType)) {
                    --Count;
                }
                if (Count == 0) {
                    break;
                }
            }
            Flagship = pas::class_cast_if<aWarrior::TWarrior*>(Buyer) != nullptr && static_cast<aWarrior::TWarrior*>(Buyer)->WarriorType == aWarrior::wtFlagship;
            if (Flagship) {
                HullType = aGalaxyStruct::htFlagship;
            }
            if (aPlayer::GetPlayer() == Buyer || Buyer->GetHull()->HullType == aGalaxyStruct::htSpecial || Buyer->GetHull()->HullType == HullType) {
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
                            MaxSize = Size + (40 + 10 * (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter))) * aGalaxy::Galaxy->TechLevel;
                        } else {
                            switch (HullType) {
                                case aGalaxyStruct::htDiplomat: MaxSize = Size + 10 * aGalaxy::Galaxy->TechLevel; break;
                                default: MaxSize = Size + 25 * aGalaxy::Galaxy->TechLevel; break;
                            }
                        }
                        MinSize = std::max<std::int64_t>(static_cast<std::int64_t>(MinSize), System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5 - (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter))]));
                        MaxSize = aGalaxy::Galaxy->ScaleIntByTechLevel(System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[4]), MaxSize);
                        break;
                    }
                    case 2: {
                        Size = Buyer->GetHull()->Weight;
                        if (Flagship) {
                            Size = Size / 2;
                        }
                        MinSize = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5 - (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter))]);
                        MaxSize = std::min<std::int64_t>(static_cast<std::int64_t>(Size), System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[aGalaxy::Galaxy->ScaleIntByTechLevel(5, 1)]));
                        break;
                    }
                    default: {
                        Size = Buyer->GetHull()->Weight;
                        if (Flagship) {
                            Size = Size / 2;
                        }
                        MinSize = Size / 2;
                        if (pas::is_one_of<aGalaxyStruct::htTransport, aGalaxyStruct::htLiner>(HullType)) {
                            MaxSize = Size + 300 + 100 * (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter));
                        } else {
                            switch (HullType) {
                                case aGalaxyStruct::htDiplomat: MaxSize = Size + 50; break;
                                default: MaxSize = Size + 200; break;
                            }
                        }
                        MinSize = std::max<std::int64_t>(static_cast<std::int64_t>(MinSize), System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5 - (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter))]));
                        MaxSize = std::min<std::int64_t>(static_cast<std::int64_t>(MaxSize), System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[aGalaxy::Galaxy->ScaleIntByTechLevel(3, 1)]));
                        break;
                    }
                }
                Owner = aConst::PickRandomEquipmentOwner(RandomState);
                if (Buyer->GetHull()->OwnerId == Owner || aPlayer::GetPlayer() == Buyer) {
                    Result = pas::construct_call<aItem::THull>(aItem::TEquipment_Create);
                    Series = -1;
                    ModuleIndex = -1;
                    Result->OwnerId = Owner;
                    Result->PirateBuilt = CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && CurrentStar->Status.CustomFaction == u"";
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
                        std::uint8_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                        std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize * 2, MaxSize * 2, RandomState);
                        Result->Init(nextRandomIntRange_2, nextRandomIntRange, Owner, 10, Series, pirateBuilt);
                    } else {
                        std::uint8_t pirateBuilt_2 = Result->PirateBuilt;
                        std::uint8_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                        std::int32_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(MinSize, MaxSize, RandomState);
                        Result->Init(nextRandomIntRange_4, nextRandomIntRange_3, Owner, HullType, Series, pirateBuilt_2);
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
        std::uint8_t Owner{};
        std::uint8_t I{};
        std::int32_t ModuleIndex{};
        aItem::TWeapon* Result = nullptr;
        if (Ship != nullptr && pas::class_cast_if<aShip::TShip*>(Ship) != nullptr) {
            Buyer = reinterpret_cast<aShip::TShip*>(Ship);
            Availability = pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}});
            if (Buyer->TypeId == aGalaxyStruct::stKling && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Dominators), OwnerId)) {
                Availability = Availability + pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{4}});
            }
            if (pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Buyer->TypeId) && pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::FactionStandingMasks[aGalaxyStruct::sfPirates]), CurrentStanding) && (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates || static_cast<std::uint8_t>(pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::FactionStandingMasks[aGalaxyStruct::sfCoalition]), CurrentStanding) ^ 1))) {
                Availability = Availability + pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{2}});
            }
            if (pas::in_range(Buyer->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stWarrior) && pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::FactionStandingMasks[aGalaxyStruct::sfCoalition]), CurrentStanding) && (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition || static_cast<std::uint8_t>(pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::FactionStandingMasks[aGalaxyStruct::sfPirates]), CurrentStanding) ^ 1))) {
                Availability = Availability + pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{1}, {5, 9}});
            }
            // The native counter guard has no back edge: only one offer is generated.
            Attempts = 0;
            if (Attempts <= 100) {
                ++Attempts;
                Info = aGalaxy::Galaxy->SelectWeaponInfo(RandomState, Availability, Planet->InventionLevels[7], 1);
                aMyFunction::AdvanceRandomSeed(RandomState);
                if (static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Buyer->TypeId) ^ 1) && pas::in_range(Info->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                    static_cast<void>(Buyer->CountDirectFireWeapons() > Buyer->CountMissileWeapons());
                }
                MinSize = System::Round(static_cast<long double>(Info->AverageSize) * aConst::EquipmentSizeFactors[5]);
                MaxSize = System::Round(static_cast<long double>(Info->AverageSize) * aConst::EquipmentSizeFactors[1]);
                if (pas::class_cast_if<aWarrior::TWarrior*>(Buyer) != nullptr && static_cast<aWarrior::TWarrior*>(Buyer)->WarriorType == aWarrior::wtFlagship) {
                    MinSize *= 2;
                    MaxSize *= 2;
                }
                MinLevel = 1;
                MaxLevel = std::min<std::int32_t>(static_cast<std::int32_t>(Planet->InventionLevels[7]), static_cast<std::int32_t>(Planet->InventionLevels[Info->InventionIndex]));
                MinLevel = std::max<std::int32_t>(MinLevel, MaxLevel / 2 - 1);
                MaxLevel = std::min<std::int32_t>(8, MaxLevel + StationOfferWeaponLevelBonus[TypeId]);
                Owner = aConst::PickRandomEquipmentOwner(RandomState);
                if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::FactionStandingMasks[aGalaxyStruct::sfPirates]), CurrentStanding) && (aMyFunction::NextRandomIntRange(1, 100, RandomState) < 70 || aGalaxy::Galaxy->CoalitionDefeatedTurn != 0)) {
                    Owner = 7;
                }
                for (I = static_cast<std::uint8_t>(0); I <= static_cast<std::uint8_t>(7); ++I) {
                    if (aConst::OwnerWeaponAvailability[I] == Info->Availability) {
                        Owner = I;
                        break;
                    }
                }
                {
                    std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                    std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize, MaxSize, RandomState);
                    Result = aItem::CreateGeneratedWeapon(Info, nextRandomIntRange_2, nextRandomIntRange, Owner);
                }
                if (pas::class_cast_if<aWarrior::TWarrior*>(Buyer) != nullptr && static_cast<aWarrior::TWarrior*>(Buyer)->WarriorType == aWarrior::wtFlagship) {
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
    aItem::TEquipment* TRuins_GenerateEquipmentOffer(TRuins* Self, pas::Object* Ship, aPlanet::TPlanet* Planet, std::uint8_t ItemType) {
        std::int32_t Priority{};
        std::int32_t ModuleIndex{};
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t MinSize{};
        std::int32_t MaxSize{};
        std::int32_t SpecialModule{};
        std::uint8_t Owner{};
        aItem::TEquipment* Result = nullptr;
        if (Ship == nullptr || !(pas::class_cast_if<aShip::TShip*>(Ship) != nullptr)) {
            return Result;
        }
        aShip::TShip* Buyer = reinterpret_cast<aShip::TShip*>(Ship);
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            if (static_cast<std::uint8_t>(pas::in_set<43, 44>(ItemType) ^ 1) && Buyer->GetSlotCountForItemType(ItemType) == 0 && aPlayer::GetPlayer() != Buyer) {
                return Result;
            }
            MinLevel = 1;
            MaxLevel = Planet->InventionLevels[aConst::EquipmentInventionIndices[ItemType]];
            MinLevel = std::max<std::int32_t>(MinLevel, MaxLevel / 2 - 1);
            MaxLevel = std::min<std::int32_t>(8, MaxLevel + StationOfferEquipmentLevelBonus[Self->TypeId][ItemType]);
            MinSize = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * aConst::EquipmentSizeFactors[5]);
            MaxSize = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * aConst::EquipmentSizeFactors[1]);
            if (pas::class_cast_if<aWarrior::TWarrior*>(Buyer) != nullptr && static_cast<aWarrior::TWarrior*>(Buyer)->WarriorType == aWarrior::wtFlagship) {
                MinSize *= 2;
                MaxSize *= 2;
            }
            Owner = aConst::PickRandomEquipmentOwner(Self->RandomState);
            if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates && pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::FactionStandingMasks[aGalaxyStruct::sfPirates]), Self->CurrentStanding) && (aMyFunction::NextRandomIntRange(1, 100, Self->RandomState) < 70 || aGalaxy::Galaxy->CoalitionDefeatedTurn != 0)) {
                Owner = 7;
            }
            {
                std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, Self->RandomState);
                std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize, MaxSize, Self->RandomState);
                Result = aItem::CreateGeneratedEquipment(static_cast<aConst::TItemType>(ItemType), nextRandomIntRange_2, nextRandomIntRange, Owner);
            }
            if (Buyer->CanGenerateMicroModuleForLoadout()) {
                SpecialModule = Self->SelectEquipmentOfferSpecialMicroModule(Result, Planet);
                if (SpecialModule >= 0) {
                    aItem::ApplySpecialMicroModule(SpecialModule, Result);
                }
            }
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            Result = Self->GenerateWeaponOffer(Ship, Planet);
        } else if (ItemType == static_cast<std::uint8_t>(aConst::t_Hull)) {
            Result = Self->GenerateHullOffer(Ship, Planet);
        }
        if (Result == nullptr) {
            return Result;
        }
        switch (Self->TypeId) {
            case aGalaxyStruct::rstBusinessCenter: {
                Result->Cost = std::min<std::int64_t>(static_cast<std::int64_t>(100000000), System::Round(Result->Cost * 1.2L));
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
            if (Self->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstScienceBase)) {
                std::int32_t cpp_case = aMyFunction::NextRandomIntRange(0, 100, Self->RandomState);
                if (cpp_case >= 0 && cpp_case <= 70) {
                    Result->Improve(aItem::ikMinor);
                } else if (cpp_case >= 71 && cpp_case <= 90) {
                    Result->Improve(aItem::ikMedium);
                } else if (cpp_case >= 91 && cpp_case <= 100) {
                    Result->Improve(aItem::ikMajor);
                }
            } else if (Self->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstRangerCenter)) {
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
        if (Self->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion) && aMyFunction::NextRandomIntRange(0, 100, Self->RandomState) > 50) {
            do {
                Priority = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 7.0, 7.0E+1, 0.0));
                {
                    std::uint32_t advanceRandomSeed = aMyFunction::AdvanceRandomSeed(Self->RandomState);
                    pas::Object* self = Self;
                    aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                    ModuleIndex = aGalaxy::TGalaxy::SelectMicroModule(Priority + Attempts / 5, std::min<std::int32_t>(100, Priority + 20 + Attempts * 4), advanceRandomSeed, self);
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
    aMyFunction::TObjectList* TRuins_GenerateEquipmentOfferBatch(TRuins* Self, aShip::TShip* Ship, std::uint8_t UnusedForceGeneratedOffers) {
        aItem::TEquipment* Item{};
        std::int32_t I{};
        std::int32_t J{};
        std::uint8_t Kind{};
        aPlanet::TPlanet* Planet{};
        aMyFunction::TObjectList* Result = pas::make_object<aMyFunction::TObjectList>();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::StationEquipmentOfferQuotas[Self->TypeId].Hulls); cpp_range.next(J); ) {
            Planet = static_cast<aPlanet::TPlanet*>(Self->CurrentStar->SelectRandomInhabitedPlanet());
            Item = aRuins::TRuins_GenerateEquipmentOffer(Self, Ship, Planet, aConst::t_Hull);
            if (Item != nullptr) {
                pas::list_add(Result, reinterpret_cast<void*>(Item));
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(pas::constant_set<aConst::TItemTypeSelection>({{43, 49}}))); cpp_range_2.next(I); ) {
            Kind = aConst::GetItemTypeFromMask(pas::constant_set<aConst::TItemTypeSelection>({{43, 49}}), I);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::StationEquipmentOfferQuotas[Self->TypeId], (Kind - 42) * sizeof(std::int32_t)))); cpp_range_3.next(J); ) {
                Planet = static_cast<aPlanet::TPlanet*>(Self->CurrentStar->SelectRandomInhabitedPlanet());
                Item = aRuins::TRuins_GenerateEquipmentOffer(Self, Ship, Planet, Kind);
                if (Item != nullptr) {
                    pas::list_add(Result, reinterpret_cast<void*>(Item));
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, aConst::StationEquipmentOfferQuotas[Self->TypeId].Weapons); cpp_range_4.next(I); ) {
            Planet = static_cast<aPlanet::TPlanet*>(Self->CurrentStar->SelectRandomInhabitedPlanet());
            Item = aRuins::TRuins_GenerateEquipmentOffer(Self, Ship, Planet, aConst::t_Weapon1);
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
        std::uint8_t Race = PilotRace;
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            TargetCount = System::Round(static_cast<long double>(aConst::GoodsMarket[Good].BaseStock) * aConst::PlanetRaceMarket[Race].GoodsFactors[Good].StockFactor * aConst::StationGoodsFactors[TypeId][Good].StockFactor);
            TargetPrice = pas::real_divide(static_cast<long double>(aConst::GoodsMarket[Good].AveragePrice) * aConst::PlanetRaceMarket[Race].GoodsFactors[Good].PriceFactor * aConst::StationGoodsFactors[TypeId][Good].PriceFactor, aMyFunction::RemapClamped(ShopGoods[Good].Count, TargetCount * 0.3L, TargetCount * 2, 0.8, 1.2));
            if (static_cast<long double>(TargetPrice) > aConst::GoodsMarket[Good].MinPrice) {
                TargetPrice = pas::real_min<float>(TargetPrice, static_cast<float>(aConst::GoodsMarket[Good].MaxPrice + 1));
            } else {
                TargetPrice = pas::real_max<float>(TargetPrice, static_cast<float>(aConst::GoodsMarket[Good].MinPrice - 1));
            }
            if (static_cast<long double>(ShopGoods[Good].PriceState) - TargetPrice >= 0.0L) {
                PriceStep = static_cast<long double>(TargetPrice) * aMyFunction::NextRandomFloatRange(0.0035, 0.006, RandomState);
            } else {
                PriceStep = static_cast<long double>(-TargetPrice) * aMyFunction::NextRandomFloatRange(0.0035, 0.006, RandomState);
            }
            {
                std::int32_t cpp_case = aMyFunction::NextRandomIntRange(1, 100, RandomState);
                if (cpp_case >= 1 && cpp_case <= 70) {
                    ShopGoods[Good].PriceState = static_cast<long double>(ShopGoods[Good].PriceState) - PriceStep;
                } else if (cpp_case >= 71 && cpp_case <= 90) {
                } else {
                    ShopGoods[Good].PriceState = static_cast<long double>(ShopGoods[Good].PriceState) + PriceStep;
                }
            }
            if (static_cast<long double>(ShopGoods[Good].PriceState) < aConst::GoodsMarket[Good].MinPrice / 2) {
                ShopGoods[Good].PriceState = aConst::GoodsMarket[Good].MinPrice / 2;
            } else if (static_cast<long double>(ShopGoods[Good].PriceState) > aConst::GoodsMarket[Good].MaxPrice * 2) {
                ShopGoods[Good].PriceState = aConst::GoodsMarket[Good].MaxPrice * 2;
            }
            ShopGoods[Good].PurchasePrice = std::max<std::int64_t>(static_cast<std::int64_t>(2), System::Round(ShopGoods[Good].PriceState));
            ShopGoods[Good].BaseSalePrice = std::max<std::int64_t>(static_cast<std::int64_t>(ShopGoods[Good].PurchasePrice / 2 + 1), System::Round(static_cast<long double>(ShopGoods[Good].PriceState) * aMyFunction::RemapClamped(ShopGoods[Good].Count, TargetCount, TargetCount * 2, 0.9, 0.7) - 1.0L));
            if (ShopGoods[Good].Count - TargetCount >= 0) {
                pas::Extended cpp_left = static_cast<long double>(TargetCount) * aMyFunction::NextRandomFloatRange(0.0025, 0.005, RandomState);
                CountStep = System::Round(cpp_left + aMyFunction::NextRandomUnitFloat(RandomState));
            } else {
                pas::Extended cpp_left_2 = static_cast<long double>(-TargetCount) * aMyFunction::NextRandomFloatRange(0.0025, 0.005, RandomState);
                CountStep = System::Round(cpp_left_2 - aMyFunction::NextRandomUnitFloat(RandomState));
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
        if (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase)) {
            EquipmentFactor = aMyFunction::RemapClamped(static_cast<std::int8_t>(aPlayer::GetPlayer()->Rank * 1), 0.0, 7.0, 0.9, 0.2);
        }
        if (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase)) {
            EquipmentFactor = 0.84f;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Ship->Inventory, I);
            if ((!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair || CanRepairArtefactsAtLocation()) && aShip::TShip_CanRepairEquipmentTech(this, Item) && (Item->ItemType == aConst::t_Hull || Item->EquippedFlag != 0 && Item->ConditionPercent < 9.0E+1L)) {
                Result += System::Round(static_cast<long double>(aItem::TEquipment_CalculateRepairCost(Item)) * EquipmentFactor);
            }
        }
        EquipmentCost = Result;
        if (CanRepairArtefactsAtLocation()) {
            ArtefactFactor = 1.0f;
            if (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstScienceBase)) {
                ArtefactFactor = 0.84f;
            }
            if (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase)) {
                ArtefactFactor = 0.84f;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Artefacts) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Ship->Artefacts, I);
                if (Item->EquippedFlag != 0 && Item->ConditionPercent < 9.0E+1L) {
                    Result += System::Round(static_cast<long double>(aItem::TEquipment_CalculateRepairCost(Item)) * ArtefactFactor);
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
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Ship->Inventory, I);
            if ((!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair || CanRepairArtefactsAtLocation()) && aShip::TShip_CanRepairEquipmentTech(this, Item) && (Item->ItemType == aConst::t_Hull || Item->EquippedFlag != 0 && Item->ConditionPercent < 9.0E+1L)) {
                Item->Repair();
            }
        }
        if (CanRepairArtefactsAtLocation()) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Artefacts) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Ship->Artefacts, I);
                if (Item->EquippedFlag != 0 && Item->ConditionPercent < 9.0E+1L) {
                    Item->Repair();
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
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            aMyFunction::IncrementWrapped(Index, 0, pas::list_count(aGalaxy::Galaxy->Stars) - 1);
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, Index);
            if (Star->Status.Battle == 0 && Star->Status.ControlFaction != aGalaxyStruct::sfDominators && Star->Status.CustomFaction == u"" && Star->ShipTypeCounts[aGalaxyStruct::rstPirateBase] != 0) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (Ship->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase) && Ship->TypeNameOverrideKey == u"" && Ship->NodeReserve > 0) {
                        return pas::checked_cast<TRuins*>(Ship);
                    }
                }
            }
        }
        return nullptr;
    }

    // Zero-based module index. Deterministic chain keyed by station, current turn and player RNG state; Kind 1 selects priorities 31..69, Kind 2 selects 0..20 or 10..30, others 70..100.
    std::int32_t TRuins_SelectServiceMicroModule(TRuins* Self, std::int32_t Kind, std::int32_t Index, std::uint8_t InvertRarity) {
        std::int32_t ChainIndex = Index + 2;
        std::uint8_t Rare = ((static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) + aPlayer::GetPlayer()->RandomState) % 33 == 0) != InvertRarity;
        switch (Kind) {
            case 2: {
                if (Rare) {
                    return aGalaxy::TGalaxy::SelectMicroModule(0, 20, aGalaxy::Galaxy->CurrentTurn / 57 + 2938629 + 17 * ChainIndex + Self->Id, Self);
                }
                return aGalaxy::TGalaxy::SelectMicroModule(10, 30, aGalaxy::Galaxy->CurrentTurn / 57 + 32465621 + 17 * ChainIndex + Self->Id, Self);
            }
            case 1: {
                return aGalaxy::TGalaxy::SelectMicroModule(31, 69, aGalaxy::Galaxy->CurrentTurn / 57 + 2351417 + 17 * ChainIndex + Self->Id, Self);
            }
            default: {
                return aGalaxy::TGalaxy::SelectMicroModule(70, 100, aGalaxy::Galaxy->CurrentTurn / 57 + 17 * ChainIndex + Self->Id, Self);
            }
        }
    }

    std::uint8_t TRuins_CanDock(TRuins* Self, aShip::TShip* Ship) {
        std::uint8_t Result = false;
        if (Self->InNormalSpace() && Ship->EnemyShip != Self && aShip::TShip_GetRelationLevelToShip(Self, Ship) > aGalaxyStruct::rlHostile && static_cast<std::uint8_t>(Self->NoLanding ^ 1) && (static_cast<std::uint8_t>(pas::in_range(Self->CurrentStanding, aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary) ^ 1) || pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssPiratePassive, aGalaxyStruct::ssPirateMilitary))) {
            return true;
        }
        return Result;
    }

    // Clears Response on success; otherwise supplies a refusal message.
    std::uint8_t TRuins::CheckDockingPermission(aShip::TShip* Ship, pas::WideString& Response) {
        std::uint8_t Result = false;
        if (NoLanding || static_cast<std::uint8_t>(InNormalSpace() ^ 1)) {
            Response = GR_Main::LookupLocalizedTextByKey(u"Help.LandingCancelScript"_wref.get());
            return Result;
        } else if (pas::in_range(CurrentStanding, aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary) && static_cast<std::uint8_t>(pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssPiratePassive, aGalaxyStruct::ssPirateMilitary) ^ 1)) {
            Response = GR_Main::LookupLocalizedTextByKey(u"Help.LandingCancelPirate"_wref.get());
            return Result;
        } else if (Ship->EnemyShip == this || aShip::TShip_GetRelationLevelToShip(this, Ship) <= aGalaxyStruct::rlHostile) {
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

    void TRuins::virtual_TShip_RepairBrokenEquipmentAtLocation() {
        aRuins::TRuins_RepairBrokenEquipmentAtLocation(this);
    }

    std::uint8_t TRuins::virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return aRuins::TRuins_CanQueueReachablePlanet(this, Planet);
    }

    void TRuins::virtual_TShip_AssignWeaponTargetsInStar() {
        aRuins::TRuins_AssignWeaponTargetsInStar(this);
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

    std::uint8_t TRuins::virtual_TShip_EvaluateAllyRelationAndStrength(aShip::TShip* Ship) {
        return aRuins::TRuins_EvaluateAllyRelationAndStrength(this, Ship);
    }

    std::uint8_t TRuins::virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        return aRuins::TRuins_BuildMoneyExtortionResponse(this, OtherShip, Response, DemandedAmount);
    }

    std::uint8_t TRuins::virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) {
        return aRuins::TRuins_BuildCargoExtortionResponse(this, OtherShip, Response);
    }

    std::uint8_t TRuins::virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        return aRuins::TRuins_BuildAttackRequestResponse(this, Requester, Response, Target);
    }

    std::uint8_t TRuins::virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aRuins::TRuins_AcceptPartnershipOffer(this, OtherShip, Response, PaymentAmount);
    }

    std::uint8_t TRuins::virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aRuins::TRuins_BuildPartnershipOfferResponse(this, OtherShip, Response, PaymentAmount);
    }

    float TRuins::virtual_TShip_EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        return aRuins::TRuins_EvaluateStatBonus(this, BonusKind, Value);
    }

    float TRuins::virtual_TShip_EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
        return aRuins::TRuins_EvaluateWeaponDamage(this, Weapon, IncludeAdditiveBonuses, BaseDamage);
    }

    void TRuins::virtual_TShip_RefreshCurrentStanding() {
        aRuins::TRuins_RefreshCurrentStanding(this);
    }

    std::uint8_t TRuins::virtual_TShip_CanDock(aShip::TShip* Ship) {
        return aRuins::TRuins_CanDock(this, Ship);
    }

} // namespace aRuins
