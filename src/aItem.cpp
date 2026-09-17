#include "layout/aItem.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Data.hpp"
#include "types/EC_Expression.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aMissile.hpp"
#include "types/aPirate.hpp"
#include "types/aPlanet.hpp"
#include "types/fHangar.hpp"
#include "types/fScaner.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/SystemImports.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/aTranclucator.hpp"
#include "units/fShip2.hpp"

namespace aItem {
    // Native managed-string defaults.
    pas::Array<pas::WideString, 1, 8> EquipmentLevelLetters = pas::Array<pas::WideString, 1, 8>{{u"A"_w, u"B"_w, u"C"_w, u"D"_w, u"E"_w, u"F"_w, u"G"_w, u"H"_w}};

    pas::Array<pas::Array<std::int32_t, 0, 3>, 1, 2> TreasureMapColumnPositions = pas::Array<pas::Array<std::int32_t, 0, 3>, 1, 2>{{pas::Array<std::int32_t, 0, 3>{{20, 40, 280, 380}}, pas::Array<std::int32_t, 0, 3>{{20, 40, 380, 490}}}};

    pas::Array<std::int32_t, 1, 2> TreasureMapRuleLengths = pas::Array<std::int32_t, 1, 2>{{76, 98}};

    // Nested native helpers include the caller's saved EBP explicitly in the IDA ABI.
    // Selects across built-in artefacts, custom artefacts and configured useless items. Pool must be nonempty; AnyAvailable is the union of the three eligibility flags.
    TEquipmentWithActCode* CreateRandomLootItem(TItemLootPool Pool, std::uint8_t Owner, std::uint32_t Seed) {
        TEquipmentWithActCode* Result{};
        std::int32_t Count = aConst::ArtefactLootPools[Pool].length();
        std::int32_t Index = Count + aConst::CustomArtefactLootPools[Pool].length() + aConst::UselessItemLootPools[Pool].length();
        Index = aMyFunction::SeededRandomIntRange(0, Index - 1, Seed);
        if (Index < aConst::ArtefactLootPools[Pool].length()) {
            return pas::checked_cast<TEquipmentWithActCode*>(static_cast<pas::Object*>(aItem::CreateConfiguredArtefactByItemType(aConst::ArtefactLootPools[Pool][Index], Owner)));
        }
        Index -= Count;
        if (Index < aConst::CustomArtefactLootPools[Pool].length()) {
            Result = pas::construct_call<TArtefactCustom>(TArtefact_Create);
            Result->ConfigBlockName = aConst::CustomArtefactLootPools[Pool][Index];
            reinterpret_cast<TArtefactCustom*>(Result)->LoadConfig(true);
            reinterpret_cast<TArtefactCustom*>(Result)->Data[1] = 0;
            reinterpret_cast<TArtefactCustom*>(Result)->Data[2] = 0;
            reinterpret_cast<TArtefactCustom*>(Result)->Data[3] = 0;
            reinterpret_cast<TArtefactCustom*>(Result)->Init(Owner, Result->ItemType);
        } else {
            Index -= aConst::CustomArtefactLootPools[Pool].length();
            Result = pas::construct_call<TUselessItem>(TUselessItem_Create);
            reinterpret_cast<TUselessItem*>(Result)->Init(aConst::UselessItemLootPools[Pool][Index], aGalaxyStruct::dsBlazer, 0u, true);
            Result->OwnerId = Owner;
        }
        return Result;
    }

    // Returns a one-based template index, or 0 if the saved template cannot be resolved.
    std::int32_t ReadSavedMicroModuleIndex(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        std::uint32_t NameHash{};
        std::int32_t Result = EC_Buf::TBufEC_GetInt32(Buffer);
        if (Result > 0) {
            NameHash = EC_Buf::TBufEC_GetUInt32(Buffer);
            if (aConst::MicroModuleTemplates.length() - 1 + 1 < Result || aConst::MicroModuleTemplates[Result - 1].ConfigNameHash != NameHash) {
                Result = 0;
                {
                    const std::int32_t cpp_last = aConst::MicroModuleTemplates.length() - 1;
                    if (0 <= cpp_last) {
                        for (I = 0; I <= cpp_last; ++I) {
                            if (aConst::MicroModuleTemplates[I].ConfigNameHash == NameHash) {
                                Result = I + 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
        return Result;
    }

    // Applies the ordered item-type insertions for save versions before 164, 78, 131, 78 and 127; arithmetic wraps in a byte.
    aConst::TItemType MigrateSavedItemType(std::uint8_t ItemType) {
        if (GlobalsV::LoadedSaveVersion < 164 && static_cast<aConst::TItemType>(ItemType) > aConst::t_Artefact) {
            ++ItemType;
        }
        if (GlobalsV::LoadedSaveVersion < 78 && static_cast<aConst::TItemType>(ItemType) > aConst::t_ArtBio) {
            ++ItemType;
        }
        if (GlobalsV::LoadedSaveVersion < 131 && static_cast<aConst::TItemType>(ItemType) > aConst::t_ArtPDTurret) {
            ++ItemType;
        }
        if (GlobalsV::LoadedSaveVersion < 78 && static_cast<aConst::TItemType>(ItemType) > aConst::t_Weapon15) {
            ItemType += 3;
        }
        if (GlobalsV::LoadedSaveVersion < 127 && static_cast<aConst::TItemType>(ItemType) > aConst::t_Weapon18) {
            ++ItemType;
        }
        return static_cast<aConst::TItemType>(ItemType);
    }

    std::int32_t GetBaseHullSlotCount(aConst::TShipSlotKind Kind, std::uint8_t HullType, std::uint8_t Owner, void* Ship) {
        std::int32_t Result = 0;
        switch (HullType) {
            case aGalaxyStruct::htRanger: return aConst::RangerHullSlots[Owner][Kind];
            case aGalaxyStruct::htWarrior: return aConst::WarriorHullSlots[Owner][Kind];
            case aGalaxyStruct::htPirate: return aConst::PirateHullSlots[Owner][Kind];
            case aGalaxyStruct::htTransport: return aConst::TransportHullSlots[Owner][Kind];
            case aGalaxyStruct::htLiner: return aConst::LinerHullSlots[Owner][Kind];
            case aGalaxyStruct::htDiplomat: return aConst::DiplomatHullSlots[Owner][Kind];
            case aGalaxyStruct::htTranclucator: return aConst::TranclucatorHullSlots[Kind];
            case aGalaxyStruct::htKling: {
                if (Ship == nullptr || !(pas::class_cast_if<aKling::TKling*>(static_cast<pas::Object*>(Ship)) != nullptr)) {
                    return aConst::DominatorHullSlots[0][Kind];
                }
                return aConst::DominatorHullSlots[pas::checked_cast<aKling::TKling*>(static_cast<pas::Object*>(Ship))->KlingType][Kind];
            }
            case aGalaxyStruct::htStation: {
                if (Ship == nullptr) {
                    return aConst::StationHullSlots[0][Kind];
                } else if (!pas::in_range(static_cast<aShip::TShip*>(Ship)->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                    return aConst::StationHullSlots[0][Kind];
                } else {
                    return aConst::StationHullSlots[static_cast<aShip::TShip*>(Ship)->TypeId - aGalaxyStruct::rstRangerCenter][Kind];
                }
            }
            case aGalaxyStruct::htSpecial: return aConst::HullType9Slots[Kind];
            case aGalaxyStruct::htFlagship: return aConst::HullType10Slots[Kind];
            default: GR_Main::RaiseWideMessage(u"SlotCount error"_wref.get()); return Result;
        }
    }

    std::int32_t CalculateGeneratedHullCost(std::uint32_t Capacity, std::uint32_t Level, std::uint8_t Owner, std::uint8_t HullType) {
        aConst::TShipSlotKind Kind{};
        double Factor = 1.0;
        if (pas::in_range(HullType, aGalaxyStruct::htRanger, aGalaxyStruct::htDiplomat)) {
            for (auto cpp_range = pas::for_to<aConst::TShipSlotKind>(aConst::sskRadar, aConst::sskAfterburner); cpp_range.next(Kind); ) {
                switch (Kind) {
                    case aConst::sskRadar: {
                        if (aItem::GetBaseHullSlotCount(Kind, HullType, Owner, nullptr) == 0) {
                            Factor = 0.7L * Factor;
                        }
                        break;
                    }
                    case aConst::sskScanner: {
                        if (aItem::GetBaseHullSlotCount(Kind, HullType, Owner, nullptr) == 0) {
                            Factor = 0.9L * Factor;
                        }
                        break;
                    }
                    case aConst::sskRepairRobot: {
                        if (aItem::GetBaseHullSlotCount(Kind, HullType, Owner, nullptr) == 0) {
                            Factor = 0.7L * Factor;
                        }
                        break;
                    }
                    case aConst::sskCargoHook: {
                        if (aItem::GetBaseHullSlotCount(Kind, HullType, Owner, nullptr) == 0) {
                            Factor = 0.6L * Factor;
                        }
                        break;
                    }
                    case aConst::sskDefGenerator: {
                        if (aItem::GetBaseHullSlotCount(Kind, HullType, Owner, nullptr) == 0) {
                            Factor = 0.7L * Factor;
                        }
                        break;
                    }
                    case aConst::sskWeapon: {
                        Factor = static_cast<long double>(aMyFunction::RemapClamped(aItem::GetBaseHullSlotCount(Kind, HullType, Owner, nullptr), 2.0, 5.0, 0.5, 1.0)) * Factor;
                        break;
                    }
                    case aConst::sskArtefact: {
                        Factor = static_cast<long double>(aMyFunction::RemapClamped(aItem::GetBaseHullSlotCount(Kind, HullType, Owner, nullptr), 0.0, 4.0, 0.7, 1.0)) * Factor;
                        break;
                    }
                    case aConst::sskAfterburner: {
                        if (aItem::GetBaseHullSlotCount(Kind, HullType, Owner, nullptr) == 0) {
                            Factor = 0.85L * Factor;
                        }
                        break;
                    }
                }
            }
            if (Factor < 0.2L) {
                Factor = 0.2;
            }
        } else if (HullType == aGalaxyStruct::htFlagship) {
            Factor = 0.05;
        }
        return aMyFunction::RoundAndTruncateToTens(aMyFunction::RemapClamped(Level, 1.0, 8.0, 1.0, 8.0) * 2.0E+3L * Factor * (std::max<std::int64_t>(static_cast<std::int64_t>(Capacity), static_cast<std::int64_t>(250)) * 0.01L - 0.006L) * (std::max<std::int64_t>(static_cast<std::int64_t>(Capacity), static_cast<std::int64_t>(1000)) * 0.002L - 0.001L) * (std::max<std::int64_t>(static_cast<std::int64_t>(Capacity), static_cast<std::int64_t>(2000)) * 5.0E-4L));
    }

    std::int32_t CalculateGeneratedFuelCapacity(std::uint32_t Weight, std::int32_t Level) {
        return System::Round(pas::real_divide(Weight, aConst::FuelTanksBaseSize) * 2.0E+1L + aConst::FuelCapacityByLevel[Level]);
    }

    std::int32_t CalculateGeneratedFuelTanksCost(std::uint32_t Weight, std::int32_t Level, std::uint8_t Owner) {
        return aMyFunction::RoundAndTruncateToTens(pas::real_divide(Weight, aConst::FuelTanksBaseSize) * static_cast<std::uint32_t>(Level * Level) * 5.0E+2L * aConst::OwnerInfo[Owner].FuelPriceFactor);
    }

    std::int32_t CalculateGeneratedEngineCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        return aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(aConst::EngineBaseSize, Weight), 0.5, 2.0, 1.0, 2.0)) * (Level * Level) * 5.0E+2L * aConst::OwnerInfo[Owner].FuelPriceFactor);
    }

    std::int32_t CalculateGeneratedRadarCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        return aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(aConst::RadarBaseSize, Weight), 0.5, 2.0, 1.0, 2.0)) * (Level * Level) * 5.0E+2L * aConst::OwnerInfo[Owner].FuelPriceFactor);
    }

    std::int32_t CalculateGeneratedScanerCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        return aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(aConst::ScannerBaseSize, Weight), 0.5, 2.0, 1.0, 2.0)) * (Level * Level) * 5.0E+2L * aConst::OwnerInfo[Owner].FuelPriceFactor);
    }

    std::int32_t CalculateGeneratedRepairRobotCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        return aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(aConst::RepairRobotBaseSize, Weight), 0.5, 2.0, 1.0, 2.0)) * (Level * Level) * 5.0E+2L * aConst::OwnerInfo[Owner].FuelPriceFactor);
    }

    std::int32_t CalculateGeneratedCargoHookCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        return aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(aConst::CargoHookBaseSize, Weight), 0.5, 2.0, 1.0, 2.0)) * (Level * Level) * 5.0E+2L * aConst::OwnerInfo[Owner].FuelPriceFactor);
    }

    std::int32_t CalculateGeneratedDefGeneratorCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        return aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(aConst::DefGeneratorBaseSize, Weight), 0.5, 2.0, 1.0, 2.0)) * (Level * Level) * 5.0E+2L * aConst::OwnerInfo[Owner].FuelPriceFactor);
    }

    double GetGeneratedDefenseDamageFactor(std::uint8_t Level) {
        return aConst::DefGeneratorLevelFactors[Level];
    }

    std::uint8_t DefenseDamageFactorToPercent(double Factor) {
        return System::Round((1.0L - Factor) * 1.0E+2L);
    }

    double DefensePercentToDamageFactor(std::int32_t Percent) {
        return 1.0L - Percent * 0.01L;
    }

    // ModuleIndex is zero-based. Expands all bonus tokens in the configured description.
    pas::WideString GetMicroModuleInfoText(std::int32_t ModuleIndex, pas::WideString ColorTag) {
        pas::WideString Result{};
        std::int32_t Value{};
        std::int32_t Fraction{};
        pas::WideString Text{};
        aConst::TEquipmentBonusKind Kind{};
        std::int32_t Index = ModuleIndex + 1;
        Result = aConst::LocalizedColorText(pas::concat_wide({u"MicroModuls.", aConst::MicroModuleTemplates[ModuleIndex].ConfigName, u".Text"}));
        for (auto cpp_range = pas::for_to<aConst::TEquipmentBonusKind>(aConst::bonHull, aConst::bonNull); cpp_range.next(Kind); ) {
            if (pas::in_range(Kind, static_cast<std::int32_t>(aConst::bonExtraAkrinEff), static_cast<std::int32_t>(aConst::bonExtraAkrinPenalty))) {
                Value = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Index - 1].StatBonuses, Kind * sizeof(std::int32_t)));
                if (Value == 0) {
                    aMyFunction::ReplaceTextToken(Result, pas::concat_wide({u"<", aConst::EquipmentBonusNames[Kind], u">"}), u"--"_w, ColorTag);
                } else {
                    if (Value > 0) {
                        Text = u"+"_w;
                    } else {
                        Text = u"-"_w;
                    }
                    Value = pas::abs(Value);
                    Fraction = Value % 100;
                    Value = Value / 100;
                    if (Fraction != 0) {
                        Text = pas::concat_wide({Text, pas::wide_int_to_str(Value), u".", pas::wide_int_to_str(Fraction)});
                    } else {
                        Text = pas::concat_wide({Text, pas::wide_int_to_str(Value)});
                    }
                    aMyFunction::ReplaceTextToken(Result, pas::concat_wide({u"<", aConst::EquipmentBonusNames[Kind], u">"}), Text, ColorTag);
                }
            } else {
                Value = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Index - 1].StatBonuses, Kind * sizeof(std::int32_t)));
                if (Value > 0) {
                    pas::WideString cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(Value)}));
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"<", aConst::EquipmentBonusNames[Kind], u">"});
                    aMyFunction::ReplaceTextToken(Result, std::move(cpp_arg_2), std::move(cpp_arg), ColorTag);
                } else if (Value < 0) {
                    pas::WideString intToStr = pas::wide_int_to_str(Value);
                    pas::WideString cpp_arg_3 = pas::concat_wide({u"<", aConst::EquipmentBonusNames[Kind], u">"});
                    aMyFunction::ReplaceTextToken(Result, std::move(cpp_arg_3), std::move(intToStr), ColorTag);
                } else {
                    aMyFunction::ReplaceTextToken(Result, pas::concat_wide({u"<", aConst::EquipmentBonusNames[Kind], u">"}), u"--"_w, ColorTag);
                }
            }
        }
        return Result;
    }

    std::uint8_t GetMicroModulePriorityColorTier(std::int32_t ModuleIndex) {
        std::uint8_t cpp_case = aConst::MicroModuleTemplates[ModuleIndex].Priority;
        if (cpp_case >= 0 && cpp_case <= 30) {
            return 3;
        } else if (cpp_case >= 31 && cpp_case <= 69) {
            return 2;
        } else if (cpp_case >= 70 && cpp_case <= 100) {
            return 1;
        } else {
            return 3;
        }
    }

    pas::WideString GetMicroModuleNameColorTag(std::int32_t ModuleIndex) {
        if (aConst::MicroModuleTemplates[ModuleIndex].Color != u"") {
            return pas::concat_wide({u"<color=", aConst::MicroModuleTemplates[ModuleIndex].Color, u">"});
        }
        switch (aItem::GetMicroModulePriorityColorTier(ModuleIndex)) {
            case 2: return u"<color=255,240,100>"_w;
            case 3: return u"<color=255,0,0>"_w;
            default: return u"<color=17,139,255>"_w;
        }
    }

    pas::WideString GetMicroModuleTextColorTag(std::int32_t ModuleIndex) {
        if (aConst::MicroModuleTemplates[ModuleIndex].Color != u"") {
            return pas::concat_wide({u"<color=", aConst::MicroModuleTemplates[ModuleIndex].Color, u">"});
        }
        return u"<color=255,167,84>"_w;
    }

    pas::WideString GetMicroModuleBitmapResourceName(std::int32_t ModuleIndex) {
        if (ModuleIndex >= 0 && aConst::MicroModuleTemplates[ModuleIndex].KindGraph != u"") {
            return pas::concat_wide({u"Bm.Micromoduls.", GR_Main::GiResourceSuffix(), u"MM", aConst::MicroModuleTemplates[ModuleIndex].KindGraph, u"_"});
        }
        return pas::concat_wide({u"Bm.Micromoduls.", GR_Main::GiResourceSuffix(), u"MM", pas::wide_int_to_str(static_cast<std::int32_t>(aItem::GetMicroModulePriorityColorTier(ModuleIndex))), u"_"});
    }

    // Returns nil outside item types 10..41.
    TArtefact* CreateConfiguredArtefactByItemType(aConst::TItemType ItemType, std::uint8_t Owner) {
        TArtefact* Item{};
        TArtefact* Result = nullptr;
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_ArtefactHull), static_cast<std::int32_t>(aConst::t_ArtFastRacks))) {
            Item = reinterpret_cast<TArtefact*>(aItem::CreateItemByType(ItemType));
            if (ItemType == aConst::t_ArtefactTransmitter) {
                reinterpret_cast<TArtefactTransmitter*>(Item)->InitTransmitter(Owner);
            } else if (ItemType == aConst::t_ArtefactTranclucator) {
                reinterpret_cast<TArtefactTranclucator*>(Item)->InitTranclucator(Owner, nullptr, nullptr);
            } else {
                Item->Init(Owner, ItemType);
            }
            return Item;
        }
        return Result;
    }

    std::int32_t CalculateGeneratedWeaponCost(aConst::PWeaponInfo Info, std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        float LevelCost = static_cast<long double>(aMyFunction::RemapClamped(Level, 1.0, 8.0, 1.0, 4.0)) * Info->CostFactor;
        return aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(Info->AverageSize, Weight), 0.5, 2.0, 1.0, 2.0)) * LevelCost * 2.5E+2L * aConst::OwnerInfo[Owner].FuelPriceFactor);
    }

    // Constructs the instance without calling its Init routine.
    TItem* CreateItemByType(aConst::TItemType ItemType) {
        TItem* Result = nullptr;
        switch (ItemType) {
            case aConst::t_Hull: return pas::construct_call<THull>(TEquipment_Create);
            case aConst::t_FuelTanks: return pas::construct_call<TFuelTanks>(TEquipment_Create);
            case aConst::t_Engine: return pas::construct_call<TEngine>(TEquipment_Create);
            case aConst::t_Radar: return pas::construct_call<TRadar>(TEquipment_Create);
            case aConst::t_Scaner: return pas::construct_call<TScaner>(TEquipment_Create);
            case aConst::t_RepairRobot: return pas::construct_call<TRepairRobot>(TEquipment_Create);
            case aConst::t_CargoHook: return pas::construct_call<TCargoHook>(TCargoHook_Create);
            case aConst::t_DefGenerator: return pas::construct_call<TDefGenerator>(TEquipment_Create);
            case aConst::t_Protoplasm: return pas::construct_call<TProtoplasm>(TEquipment_Create);
            case aConst::t_UselessItem: return pas::construct_call<TUselessItem>(TUselessItem_Create);
            case aConst::t_MicroModule: return pas::construct_call<TMicroModule>(TEquipment_Create);
            case aConst::t_Cistern: return pas::construct_call<TCistern>(TEquipment_Create);
            case aConst::t_Satellite: return pas::construct_call<TSatellite>(TEquipment_Create);
            case aConst::t_TreasureMap: return pas::construct_call<TTreasureMap>(TEquipment_Create);
            case aConst::t_UselessCountableItem: return pas::construct_call<TCountableItem>(TEquipment_Create);
            case aConst::t_ArtefactTransmitter: return pas::construct_call<TArtefactTransmitter>(TArtefact_Create);
            case aConst::t_ArtefactTranclucator: return pas::construct_call<TArtefactTranclucator>(TArtefact_Create);
            case aConst::t_Artefact:
            case aConst::t_Artefact2: return pas::construct_call<TArtefactCustom>(TArtefact_Create);
            case aConst::t_CustomWeapon: return pas::construct_call<TCustomWeapon>(TEquipment_Create);
            default: {
                if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_Weapon18))) {
                    return pas::construct_call<TWeapon>(TEquipment_Create);
                } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
                    return pas::construct_call<TGoods>(TItem_Create);
                } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_ArtefactHull), static_cast<std::int32_t>(aConst::t_ArtFastRacks))) {
                    return pas::construct_call<TArtefact>(TArtefact_Create);
                } else {
                    pas::make_exception<pas::Exception>("Error CreateItemByType"_a);
                    return Result;
                }
            }
        }
    }

    TItem* CreateDefaultItemByType(aConst::TItemType ItemType) {
        TItem* Result{};
        TItem* Item{};
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_ArtefactHull), static_cast<std::int32_t>(aConst::t_ArtFastRacks))) {
            return aItem::CreateConfiguredArtefactByItemType(ItemType, 6);
        } else if (ItemType == aConst::t_Hull) {
            Item = aItem::CreateItemByType(ItemType);
            reinterpret_cast<THull*>(Item)->Init(250, 1, aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace), 0, -1, false);
            return Item;
        } else if (ItemType == aConst::t_CustomWeapon) {
            return nullptr;
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            return aItem::CreateGeneratedEquipment(ItemType, 20, 1, 6);
        } else {
            Item = aItem::CreateItemByType(ItemType);
            Result = Item;
            if (Item != nullptr) {
                switch (ItemType) {
                    case aConst::t_Protoplasm: reinterpret_cast<TProtoplasm*>(Item)->Init_2(10, 0); return Result;
                    case aConst::t_UselessItem: {
                        reinterpret_cast<TUselessItem*>(Item)->Init(EC_Str::DecodeTextW(u"EdxYahmrpelwefAjsktleoruoeiddc"_w), aGalaxyStruct::dsBlazer, 0u, false);
                        return Result;
                    }
                    case aConst::t_MicroModule: reinterpret_cast<TMicroModule*>(Item)->Init(1); return Result;
                    case aConst::t_Cistern: reinterpret_cast<TCistern*>(Item)->Init(10, 10, 6); return Result;
                    case aConst::t_Satellite: {
                        {
                            std::uint32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(1, 10000, aGalaxy::Galaxy->RandomState);
                            std::uint8_t ownerId = aPlayer::GetPlayer()->OwnerId;
                            reinterpret_cast<TSatellite*>(Item)->InitGenerated(1, ownerId, nextRandomIntRange);
                        }
                        return Result;
                    }
                    default: {
                        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
                            reinterpret_cast<TGoods*>(Item)->Init(ItemType, 10);
                            return Result;
                        }
                        pas::free(Item);
                        return nullptr;
                    }
                }
            }
            return Result;
        }
    }

    // Clamps Level to 1..8; custom weapons require CreateGeneratedWeapon.
    TEquipment* CreateGeneratedEquipment(aConst::TItemType ItemType, std::int32_t Weight, std::int32_t Level, std::uint8_t Owner) {
        TItem* Item{};
        std::uint8_t ActualLevel{};
        std::int32_t MinimumLevel = std::max<std::int32_t>(1, Level);
        TEquipment* Result = nullptr;
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            Item = aItem::CreateItemByType(ItemType);
            Result = reinterpret_cast<TEquipment*>(Item);
            if (Item != nullptr) {
                ActualLevel = std::max<std::int32_t>(0, std::min<std::int32_t>(MinimumLevel, 8));
                switch (ItemType) {
                    case aConst::t_Hull: {
                        reinterpret_cast<THull*>(Item)->Init(Weight, ActualLevel, Owner, 0, -1, false);
                        return Result;
                    }
                    case aConst::t_FuelTanks: {
                        reinterpret_cast<TFuelTanks*>(Item)->Init(Weight, ActualLevel, Owner);
                        return Result;
                    }
                    case aConst::t_Engine: {
                        reinterpret_cast<TEngine*>(Item)->Init(Weight, ActualLevel, Owner);
                        return Result;
                    }
                    case aConst::t_Radar: {
                        reinterpret_cast<TRadar*>(Item)->Init(Weight, ActualLevel, Owner);
                        return Result;
                    }
                    case aConst::t_Scaner: {
                        reinterpret_cast<TScaner*>(Item)->Init(Weight, ActualLevel, Owner);
                        return Result;
                    }
                    case aConst::t_RepairRobot: {
                        reinterpret_cast<TRepairRobot*>(Item)->Init(Weight, ActualLevel, Owner);
                        return Result;
                    }
                    case aConst::t_CargoHook: {
                        reinterpret_cast<TCargoHook*>(Item)->Init(Weight, ActualLevel, Owner);
                        return Result;
                    }
                    case aConst::t_DefGenerator: {
                        reinterpret_cast<TDefGenerator*>(Item)->Init(Weight, ActualLevel, Owner);
                        return Result;
                    }
                    default: {
                        if (ItemType == aConst::t_CustomWeapon) {
                            pas::raise(pas::make_exception<pas::Exception>("Error CreateEq - cant create custom weapons"_a));
                        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                            reinterpret_cast<TWeapon*>(Item)->Init(ItemType, Weight, ActualLevel, Owner);
                            return Result;
                        } else {
                            pas::free(Item);
                            return nullptr;
                        }
                    }
                }
            }
        }
        return Result;
    }

    TWeapon* CreateGeneratedWeapon(aConst::PWeaponInfo Info, std::int32_t Weight, std::int32_t Level, std::uint8_t Owner) {
        TWeapon* Result = reinterpret_cast<TWeapon*>(aItem::CreateItemByType(Info->ItemType));
        if (pas::in_range(Info->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_Weapon18))) {
            Result->Init(Info->ItemType, Weight, Level, Owner);
        } else {
            reinterpret_cast<TCustomWeapon*>(Result)->InitCustom(Info, false, Weight, Level, Owner);
        }
        return Result;
    }

    // Module indices are zero-based; compatibility checks also accept special bonuses.
    std::uint8_t CanInstallMicroModule(std::int32_t ModuleIndex, TEquipment* Item) {
        std::uint8_t Result = false;
        if (Item->MicroModuleIndex != 0) {
            return Result;
        }
        if (Item->SpecialModuleIndex != 0 && aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].BlocksMicroModuleSlot) {
            return Result;
        }
        if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[ModuleIndex].StatBonuses, aConst::bonExtraAkrinEff * sizeof(std::int32_t))) != 0 && Item->SpecialModuleIndex == 0) {
            return Result;
        }
        if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[ModuleIndex].StatBonuses, aConst::bonExtraAkrinPenalty * sizeof(std::int32_t))) != 0 && Item->SpecialModuleIndex == 0) {
            return Result;
        }
        if (aConst::MicroModuleTemplates[ModuleIndex].SpecialOnly) {
            return Result;
        }
        if (pas::class_cast_if<TWeapon*>(Item) != nullptr) {
            return aItem::IsBonusCompatibleWithWeapon(ModuleIndex, reinterpret_cast<TWeapon*>(Item));
        } else if (pas::class_cast_if<THull*>(Item) != nullptr) {
            return aItem::IsBonusCompatibleWithHull(ModuleIndex, reinterpret_cast<THull*>(Item));
        } else {
            return aItem::IsBonusCompatibleWithEquipment(ModuleIndex, Item);
        }
    }

    std::uint8_t IsBonusCompatibleWithEquipment(std::int32_t ModuleIndex, TEquipment* Item) {
        std::uint8_t Result = false;
        if (!pas::contains(pas::load_unaligned<aGalaxyStruct::TItemTypeMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedItemTypes), static_cast<std::uint8_t>(Item->ItemType))) {
            return Result;
        }
        if (Item->CustomFaction != u"") {
            if (pas::pos(pas::concat_wide({u"<", Item->CustomFaction, u">"}), aConst::MicroModuleTemplates[ModuleIndex].AllowedCustomHullFactions) > 0) {
                return true;
            }
            if (Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) || Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && pas::load_unaligned<aGalaxy::TDominatorSeriesMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask) != pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0, 2}})) {
                return Result;
            }
        }
        Result = pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), Item->OwnerId);
        if (Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && static_cast<std::uint8_t>(pas::contains(pas::load_unaligned<aGalaxy::TDominatorSeriesMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask), static_cast<std::uint8_t>(Item->DominatorSeries)) ^ 1)) {
            return false;
        }
        return Result;
    }

    std::uint8_t IsBonusCompatibleWithHull(std::int32_t ModuleIndex, THull* Hull) {
        std::uint8_t Result = false;
        if (!pas::contains(pas::load_unaligned<aGalaxyStruct::TItemTypeMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedItemTypes), aConst::t_Hull)) {
            return Result;
        }
        if (Hull->CustomFaction != u"") {
            if (pas::pos(pas::concat_wide({u"<", Hull->CustomFaction, u">"}), aConst::MicroModuleTemplates[ModuleIndex].AllowedCustomHullFactions) > 0) {
                return true;
            }
            if (Hull->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) || Hull->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && pas::load_unaligned<aGalaxy::TDominatorSeriesMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask) != pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0, 2}})) {
                return Result;
            }
        }
        if ((pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), Hull->OwnerId) || Hull->PirateBuilt && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), aGalaxyStruct::oiPirate)) && (Hull->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || pas::contains(pas::load_unaligned<aGalaxy::TDominatorSeriesMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask), static_cast<std::uint8_t>(Hull->DominatorSeries)))) {
            return true;
        }
        return Result;
    }

    std::uint8_t IsBonusCompatibleWithWeapon(std::int32_t ModuleIndex, TWeapon* Weapon) {
        pas::WideString AllowedTypes{};
        aConst::PWeaponInfo Info{};
        std::uint8_t Result = false;
        if (Weapon->CustomFaction != u"" && pas::pos(pas::concat_wide({u"<", Weapon->CustomFaction, u">"}), aConst::MicroModuleTemplates[ModuleIndex].AllowedCustomHullFactions) > 0 || pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), Weapon->OwnerId) && (Weapon->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || pas::contains(pas::load_unaligned<aGalaxy::TDominatorSeriesMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask), static_cast<std::uint8_t>(Weapon->DominatorSeries))) && (Weapon->CustomFaction == u"" || Weapon->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) && (Weapon->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || pas::load_unaligned<aGalaxy::TDominatorSeriesMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask) == pas::constant_set<aGalaxy::TDominatorSeriesMask>({{0, 2}})))) {
            if (pas::in_range(Weapon->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_Weapon18))) {
                if (pas::contains(pas::load_unaligned<aGalaxyStruct::TItemTypeMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedItemTypes), static_cast<std::uint8_t>(Weapon->ItemType))) {
                    return true;
                }
                return Result;
            } else if (Weapon->ItemType == aConst::t_CustomWeapon) {
                Info = Weapon->GetWeaponInfo();
                AllowedTypes = aConst::MicroModuleTemplates[ModuleIndex].AllowedCustomWeaponTypes;
                if (AllowedTypes == u"Any") {
                    return true;
                } else if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Info->DamageFlags), aGalaxyStruct::dkMissile) && pas::pos(u"<WMissile>", AllowedTypes) > 0) {
                    return true;
                } else if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Info->DamageFlags), aGalaxyStruct::dkSplinter) && pas::pos(u"<WSplinter>", AllowedTypes) > 0) {
                    return true;
                } else if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Info->DamageFlags), aGalaxyStruct::dkEnergy) && pas::pos(u"<WEnergy>", AllowedTypes) > 0) {
                    return true;
                } else if (pas::pos(pas::concat_wide({u"<", Info->ConfigName, u">"}), AllowedTypes) > 0) {
                    return true;
                } else {
                    return Result;
                }
            } else {
                return true;
            }
        }
        return Result;
    }

    // Does not check compatibility or remove an existing module; -1 or nil returns False.
    std::uint8_t ApplyMicroModule(std::int32_t ModuleIndex, TEquipment* Item) {
        std::uint8_t BonusKind{};
        if (ModuleIndex == -1 || Item == nullptr) {
            return false;
        }
        std::uint8_t Result = true;
        Item->MicroModuleIndex = ModuleIndex + 1;
        Item->Weight = System::Round(pas::real_max<pas::Extended>(1.0L, pas::real_divide(Item->Weight, 1.0E+2L) * aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].SizePercent));
        Item->Cost = std::min<std::int64_t>(static_cast<std::int64_t>(100000000), System::Round(pas::real_max<pas::Extended>(1.0L, pas::real_divide(Item->Cost, 1.0E+2L) * aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].CostPercent)));
        switch (Item->ItemType) {
            case aConst::t_Hull: {
                pas::checked_cast<THull*>(Item)->Armor += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t)));
                pas::checked_cast<THull*>(Item)->HullPoints = std::min<std::int32_t>(Item->Weight, pas::checked_cast<THull*>(Item)->HullPoints);
                return Result;
            }
            case aConst::t_FuelTanks: {
                pas::checked_cast<TFuelTanks*>(Item)->Capacity += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t)));
                return Result;
            }
            case aConst::t_Engine: {
                pas::checked_cast<TEngine*>(Item)->Speed += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonSpeed * sizeof(std::int32_t)));
                pas::checked_cast<TEngine*>(Item)->JumpRange += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonJump * sizeof(std::int32_t)));
                return Result;
            }
            case aConst::t_Radar: {
                pas::checked_cast<TRadar*>(Item)->Range += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t)));
                return Result;
            }
            case aConst::t_Scaner: {
                pas::checked_cast<TScaner*>(Item)->ScanPower += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonScan * sizeof(std::int32_t)));
                return Result;
            }
            case aConst::t_RepairRobot: {
                pas::checked_cast<TRepairRobot*>(Item)->RepairPoints += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t)));
                return Result;
            }
            case aConst::t_CargoHook: {
                pas::checked_cast<TCargoHook*>(Item)->PickupPower += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t)));
                pas::checked_cast<TCargoHook*>(Item)->Range += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t)));
                {
                    auto& cpp_target = pas::checked_cast<TCargoHook*>(Item)->MinPullSpeed;
                    cpp_target = static_cast<long double>(pas::checked_cast<TCargoHook*>(Item)->MinPullSpeed) + pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHookMinSpeed * sizeof(std::int32_t)));
                }
                {
                    auto& cpp_target_2 = pas::checked_cast<TCargoHook*>(Item)->MaxPullSpeed;
                    cpp_target_2 = static_cast<long double>(pas::checked_cast<TCargoHook*>(Item)->MaxPullSpeed) + pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHookMaxSpeed * sizeof(std::int32_t)));
                }
                return Result;
            }
            case aConst::t_DefGenerator: {
                if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t))) != 0) {
                    auto& cpp_target_3 = pas::checked_cast<TDefGenerator*>(Item)->DamageFactor;
                    cpp_target_3 = pas::checked_cast<TDefGenerator*>(Item)->DamageFactor - (1.0L - aItem::DefensePercentToDamageFactor(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t)))));
                }
                return Result;
            }
            default: {
                if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    BonusKind = aConst::WeaponDamageClasses[aConst::ClassifyWeaponDamageFlags(reinterpret_cast<TWeapon*>(Item)->GetWeaponInfo()->DamageFlags)].BonusKind;
                    pas::checked_cast<TWeapon*>(Item)->MaxDamage += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
                    pas::checked_cast<TWeapon*>(Item)->Range += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t)));
                    if (pas::in_range(reinterpret_cast<TWeapon*>(Item)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                        pas::checked_cast<TWeapon*>(Item)->AmmoCapacity += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonAmmo * sizeof(std::int32_t)));
                    }
                } else {
                    pas::make_exception<pas::Exception>("\314\350\352\360\356\354\356\344\363\353\374 \342 \356\341\356\360\363\344\356\342\340\355\350\345 \365\356\362\345\353\350 \342\361\362\340\342\350\362\374, \342 \352\356\362\356\360\356\345 \342\361\362\340\342\350\362\374 \354\350\352\360\356\354\356\344\363\353\374 \355\345\353\374\347\377!"_a);
                }
                return Result;
            }
        }
    }

    void ApplySpecialMicroModule(std::int32_t ModuleIndex, TEquipment* Item) {
        std::uint8_t BonusKind{};
        if (ModuleIndex == -1 || Item == nullptr || Item->SpecialModuleIndex != 0) {
            GR_Main::RaiseWideMessage(u"SpecialToEquipment"_wref.get());
            return;
        }
        Item->SpecialModuleIndex = ModuleIndex + 1;
        Item->Weight = System::Round(pas::real_max<pas::Extended>(1.0L, pas::real_divide(Item->Weight, 1.0E+2L) * aConst::MicroModuleTemplates[ModuleIndex].SizePercent));
        Item->Cost = std::min<std::int32_t>(100000000, aMyFunction::RoundAndTruncateToTens(pas::real_max<pas::Extended>(1.0E+1L, pas::real_divide(Item->Cost, 1.0E+2L) * aConst::MicroModuleTemplates[ModuleIndex].CostPercent)));
        if (Item->ItemType == aConst::t_Hull) {
            pas::checked_cast<THull*>(Item)->HullType = aGalaxyStruct::htSpecial;
            pas::checked_cast<THull*>(Item)->HullPoints = Item->Weight;
            if (Item->Cost < 0 || Item->Cost > 100000000) {
                Item->Cost = 100000000;
            }
        }
        if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            BonusKind = aConst::WeaponDamageClasses[aConst::ClassifyWeaponDamageFlags(reinterpret_cast<TWeapon*>(Item)->GetWeaponInfo()->DamageFlags)].BonusKind;
            pas::checked_cast<TWeapon*>(Item)->MaxDamage += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[ModuleIndex].StatBonuses, BonusKind * sizeof(std::int32_t)));
            pas::checked_cast<TWeapon*>(Item)->Range += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[ModuleIndex].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t)));
            if (pas::in_range(reinterpret_cast<TWeapon*>(Item)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                pas::checked_cast<TWeapon*>(Item)->AmmoCapacity += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[ModuleIndex].StatBonuses, aConst::bonAmmo * sizeof(std::int32_t)));
            }
        }
        if (aConst::MicroModuleTemplates[ModuleIndex].CustomFaction != u"") {
            Item->CustomFaction = aConst::MicroModuleTemplates[ModuleIndex].CustomFaction;
        }
    }

    void RemoveMicroModule(TEquipment* Item) {
        std::uint8_t BonusKind{};
        if (Item == nullptr || Item->MicroModuleIndex == 0) {
            return;
        }
        Item->Weight = System::Round(pas::real_max<pas::Extended>(1.0L, pas::real_divide(Item->Weight, aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].SizePercent) * 1.0E+2L));
        Item->Cost = System::Round(pas::real_max<pas::Extended>(1.0L, pas::real_divide(Item->Cost, aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].CostPercent) * 1.0E+2L));
        switch (Item->ItemType) {
            case aConst::t_Hull: {
                pas::checked_cast<THull*>(Item)->Armor -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t)));
                pas::checked_cast<THull*>(Item)->HullPoints = std::min<std::int32_t>(Item->Weight, pas::checked_cast<THull*>(Item)->HullPoints);
                break;
            }
            case aConst::t_FuelTanks: {
                pas::checked_cast<TFuelTanks*>(Item)->Capacity -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t)));
                break;
            }
            case aConst::t_Engine: {
                pas::checked_cast<TEngine*>(Item)->Speed -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonSpeed * sizeof(std::int32_t)));
                pas::checked_cast<TEngine*>(Item)->JumpRange -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonJump * sizeof(std::int32_t)));
                break;
            }
            case aConst::t_Radar: {
                pas::checked_cast<TRadar*>(Item)->Range -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t)));
                break;
            }
            case aConst::t_Scaner: {
                pas::checked_cast<TScaner*>(Item)->ScanPower -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonScan * sizeof(std::int32_t)));
                break;
            }
            case aConst::t_RepairRobot: {
                pas::checked_cast<TRepairRobot*>(Item)->RepairPoints -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t)));
                break;
            }
            case aConst::t_CargoHook: {
                pas::checked_cast<TCargoHook*>(Item)->PickupPower -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t)));
                pas::checked_cast<TCargoHook*>(Item)->Range -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t)));
                {
                    auto& cpp_target = pas::checked_cast<TCargoHook*>(Item)->MinPullSpeed;
                    cpp_target = static_cast<long double>(pas::checked_cast<TCargoHook*>(Item)->MinPullSpeed) - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHookMinSpeed * sizeof(std::int32_t)));
                }
                {
                    auto& cpp_target_2 = pas::checked_cast<TCargoHook*>(Item)->MaxPullSpeed;
                    cpp_target_2 = static_cast<long double>(pas::checked_cast<TCargoHook*>(Item)->MaxPullSpeed) - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonHookMaxSpeed * sizeof(std::int32_t)));
                }
                break;
            }
            case aConst::t_DefGenerator: {
                if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t))) != 0) {
                    auto& cpp_target_3 = pas::checked_cast<TDefGenerator*>(Item)->DamageFactor;
                    cpp_target_3 = pas::checked_cast<TDefGenerator*>(Item)->DamageFactor + (1.0L - aItem::DefensePercentToDamageFactor(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t)))));
                }
                break;
            }
            default: {
                if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    BonusKind = aConst::WeaponDamageClasses[aConst::ClassifyWeaponDamageFlags(reinterpret_cast<TWeapon*>(Item)->GetWeaponInfo()->DamageFlags)].BonusKind;
                    pas::checked_cast<TWeapon*>(Item)->MaxDamage -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
                    pas::checked_cast<TWeapon*>(Item)->Range -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t)));
                    if (pas::in_range(reinterpret_cast<TWeapon*>(Item)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                        pas::checked_cast<TWeapon*>(Item)->AmmoCapacity -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonAmmo * sizeof(std::int32_t)));
                    }
                }
                break;
            }
        }
        Item->MicroModuleIndex = 0;
    }

    void RemoveSpecialMicroModule(TEquipment* Item) {
        std::uint8_t BonusKind{};
        if (Item == nullptr || Item->SpecialModuleIndex == 0) {
            return;
        }
        Item->Weight = System::Round(pas::real_max<pas::Extended>(1.0L, pas::real_divide(Item->Weight * 100, aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].SizePercent)));
        Item->Cost = System::Round(pas::real_max<pas::Extended>(1.0L, pas::real_divide(Item->Cost * 100, aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].CostPercent)));
        if (Item->ItemType == aConst::t_Hull) {
            pas::checked_cast<THull*>(Item)->HullPoints = std::min<std::int32_t>(Item->Weight, pas::checked_cast<THull*>(Item)->HullPoints);
            if (Item->Cost < 0 || Item->Cost > 100000000) {
                Item->Cost = 100000000;
            }
        }
        if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            BonusKind = aConst::WeaponDamageClasses[aConst::ClassifyWeaponDamageFlags(reinterpret_cast<TWeapon*>(Item)->GetWeaponInfo()->DamageFlags)].BonusKind;
            pas::checked_cast<TWeapon*>(Item)->MaxDamage -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
            pas::checked_cast<TWeapon*>(Item)->Range -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t)));
            if (pas::in_range(reinterpret_cast<TWeapon*>(Item)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                pas::checked_cast<TWeapon*>(Item)->AmmoCapacity -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].StatBonuses, aConst::bonAmmo * sizeof(std::int32_t)));
            }
        }
        if (Item->CustomFaction != u"" && aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].CustomFaction == Item->CustomFaction) {
            Item->CustomFaction = pas::WideString();
        }
        Item->SpecialModuleIndex = 0;
    }

    std::uint8_t CanCargoHookHandleItem(TItem* Item, void* Ship) {
        aShip::TShip* Owner = static_cast<aShip::TShip*>(Ship);
        std::uint8_t Result = false;
        if (!aShip::TShip_IsEquipmentUsable(Owner, Owner->GetCargoHook())) {
            return Result;
        }
        if (aShip::TShip_CalculateCargoHookPower(Owner, Owner->GetCargoHook()) < Item->Weight) {
            return Result;
        }
        if (pas::class_cast_if<TUselessItem*>(Item) != nullptr && reinterpret_cast<TUselessItem*>(Item)->ConfigBlockName == u"ExampleAsteroid") {
            return Result;
        }
        if (aPlayer::GetPlayer() != Owner) {
            if (Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name != u"") {
                return Result;
            }
            if (pas::class_cast_if<TSatellite*>(Item) != nullptr || Item->DestroyFlag > 0) {
                return Result;
            }
        }
        return true;
    }

    pas::WideString GetItemTypeBitmapPath(aConst::TItemType ItemType) {
        return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType]});
    }

    // Goods use their market display name; nodes use the generic node name; other types return empty.
    pas::WideString GetStackableItemTypeName(aConst::TItemType ItemType) {
        pas::WideString Result{};
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
            return aConst::GoodsMarket[ItemType].DisplayName;
        } else if (ItemType == aConst::t_Protoplasm) {
            return aConst::LocalizedText(u"Items.Nod.Name"_wref.get());
        } else {
            return Result;
        }
    }

    // Custom countables use their configured name; all other types use GetStackableItemTypeName. Ignores per-instance name overrides.
    pas::WideString GetStackableItemName(TItem* Item) {
        if (Item->ItemType == aConst::t_UselessCountableItem) {
            return aConst::LocalizedText(pas::concat_wide({u"Items.CustomCountables.", pas::checked_cast<TCountableItem*>(Item)->ConfigBlockName, u".Name"}));
        }
        return aItem::GetStackableItemTypeName(Item->ItemType);
    }

    // Preserve the native evaluation order: select the percentage before clamping
    // capacity. The inline helper also retains the compiler's separate temporaries.
    void CalculateHullCapacityIncrease(THull* Hull, std::int32_t LowPercent, std::int32_t HighPercent, std::int32_t& Increase) {
        std::int32_t Capacity{};
        std::int32_t Percent = aMyFunction::SeededRandomIntRange(LowPercent, HighPercent, Hull->Id * 214571);
        if (Hull->Weight > 500) {
            Capacity = Hull->Weight;
        } else {
            Capacity = 500;
        }
        Increase = System::Round(Percent * Capacity * 0.01L);
    }

    void TItem_Create(TItem* Self) {
        EC_Struct::TObjectEx_Create(Self);
        if (aGalaxy::Galaxy != nullptr) {
            Self->Id = aGalaxy::Galaxy->NextItemId;
            ++aGalaxy::Galaxy->NextItemId;
        }
        Self->NameOverride = pas::WideString();
    }

    void TItem_Destroy(TItem* Self) {
        if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr && Self->ScriptItem != nullptr) {
            reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->RunActionCode(aConst::satOnItemDestroy, nullptr, nullptr, nullptr, 0);
        }
        Self->Id = 0;
        if (Self->ScriptItem != nullptr) {
            pas::checked_cast<aScript::TScriptItem*>(Self->ScriptItem)->Item = nullptr;
            Self->ScriptItem = nullptr;
        }
        if (Self->GraphObject != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->GraphObject));
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TItem::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddDWord(Id);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(ItemType));
        Buffer->AddSingle(Position.X);
        Buffer->AddSingle(Position.Y);
        Buffer->AddIntegerValue(Weight);
        Buffer->AddAnsiChar(OwnerId);
        Buffer->AddDWord(Cost);
        Buffer->AddIntegerValue(DestroyFlag);
        if (NameOverride == u"") {
            Buffer->AddBoolean(false);
        } else {
            Buffer->AddBoolean(true);
            Buffer->AddWideStringZ(NameOverride);
        }
        Buffer->AddAnsiChar(NoDropFlag);
    }

    // Object-reference fields in descendants hold saved IDs until ResolveLoadedReferences; updates Galaxy.NextItemId.
    void TItem::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        Id = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (Galaxy->NextItemId <= static_cast<std::uint32_t>(Id)) {
            Galaxy->NextItemId = Id + 1;
        }
        ItemType = aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer));
        Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
        Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        Weight = EC_Buf::TBufEC_GetInt32(Buffer);
        OwnerId = EC_Buf::TBufEC_GetByte(Buffer);
        Cost = EC_Buf::TBufEC_GetUInt32(Buffer);
        DestroyFlag = EC_Buf::TBufEC_GetInt32(Buffer);
        if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
            NameOverride = Buffer->ReadWideString();
        }
        NoDropFlag = EC_Buf::TBufEC_GetByte(Buffer);
    }

    void TItem::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        {
            const pas::WideString& displayName = GetDisplayName();
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"IQNaaWmee"_w);
            Block->AddParam(decodeTextW, displayName);
        }
        Block->AddParam(EC_Str::DecodeTextW(u"InToyAple"_w), aConst::ItemTypeNames[ItemType]);
        Block->AddParam(EC_Str::DecodeTextW(u"OpwRn3ewr"_w), aConst::OwnerInfo[OwnerId].InternalName);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Weight);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"SaiRzoe"_w);
            Block->AddParam(decodeTextW_2, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Cost);
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"CfoTsat"_w);
            Block->AddParam(decodeTextW_3, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(NoDropFlag));
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"NeonDarlokpl"_w);
            Block->AddParam(decodeTextW_4, intToStr_3);
        }
        if (ScriptItem != nullptr) {
            Block->AddParam(EC_Str::DecodeTextW(u"IsSacaraiOpit"_w), reinterpret_cast<aScript::TScriptItem*>(ScriptItem)->Script->ScriptFileName);
        }
    }

    void TItem_LoadFromBlock(TItem* Self, EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        pas::WideString Text{};
        Text = Block->GetParam(EC_Str::DecodeTextW(u"OpwRn3ewr"_w));
        for (I = 0; I <= 7; ++I) {
            if (Text == aConst::OwnerInfo[static_cast<std::uint8_t>(I)].InternalName) {
                Self->OwnerId = I;
            }
        }
        Self->Weight = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"SaiRzoe"_w))));
        Self->Cost = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"CfoTsat"_w))));
        Text = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"NeonDarlokpl"_w)))));
        if (Text == u"false") {
            Self->NoDropFlag = 0;
        } else if (Text == u"true") {
            Self->NoDropFlag = 1;
        } else {
            Self->NoDropFlag = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
        }
        if (pas::class_cast_if<TGoods*>(Self) != nullptr) {
            pas::checked_cast<TGoods*>(Self)->Quantity = Self->Weight;
        }
        if (pas::class_cast_if<TCountableItem*>(Self) != nullptr) {
            std::int32_t cpp_right = pas::checked_cast<TCountableItem*>(Self)->GetUnitSize();
            pas::checked_cast<TCountableItem*>(Self)->StackCount = pas::idiv(Self->Weight, cpp_right);
        }
        if (Self->ItemType == aConst::t_ArtefactTranclucator && pas::checked_cast<TArtefactTranclucator*>(Self)->Ship != nullptr) {
            static_cast<aTranclucator::TTranclucator*>(pas::checked_cast<TArtefactTranclucator*>(Self)->Ship)->ArtefactSize = Self->Weight;
        }
    }

    void TItem::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
    }

    // Generic Items.SmallInfo label used outside radar range.
    pas::WideString TItem::GetSmallInfoText() {
        return GR_Main::LookupLocalizedTextByKey(u"Items.SmallInfo"_wref.get());
    }

    // Applies the trading-skill percentage to Cost minus repair cost; equipment has a minimum value of 1. Goods use Cost directly.
    std::int32_t TItem::CalculateResaleValue(std::uint8_t TradingSkill) {
        if (pas::class_cast_if<TEquipment*>(this) != nullptr) {
            return std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round((Cost - aItem::TEquipment_CalculateRepairCost(pas::checked_cast<TEquipment*>(this))) * 0.01L * aConst::PilotSkillEffects[TradingSkill][aShip::psTrading]));
        }
        return System::Round(Cost * 0.01L * aConst::PilotSkillEffects[TradingSkill][aShip::psTrading]);
    }

    // Equipment deducts repair cost, with a minimum result of 1; goods return Cost unchanged.
    std::int32_t TItem::GetConditionAdjustedCost() {
        if (pas::class_cast_if<TEquipment*>(this) != nullptr) {
            return std::max<std::int32_t>(1, Cost - aItem::TEquipment_CalculateRepairCost(pas::checked_cast<TEquipment*>(this)));
        }
        return Cost;
    }

    // Groups weapon types under Weapon and built-in artefacts under Artefact; otherwise returns the item-type configuration name.
    pas::WideString TItem::GetCategoryConfigName() {
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            return u"Weapon"_w;
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_ArtefactHull), static_cast<std::int32_t>(aConst::t_ArtFastRacks))) {
            return u"Artefact"_w;
        } else {
            return aConst::ItemTypeNames[ItemType];
        }
    }

    pas::WideString TItem::GetShortName() {
        return pas::WideString();
    }

    pas::WideString TItem_GetInfoText(TItem* Self, pas::WideString ColorTag, void* Ship) {
        return pas::WideString();
    }

    // Lazily creates and initializes the retained scene container; returns a borrowed reference. Appearance depends on item kind, size, faction, and drop flags.
    SE_Space::TObjectSE* TItem::GetGraphObject() {
        // Captures the item at ParentFrame-4; caller removes ParentFrame.
        auto CreateContainer = [&](pas::WideString GraphKey) -> void {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Container"_wref.get(), pas::concat_wide({u"Item.", GraphKey}), ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphObject = pas::Var<SE_Space::TObjectSE*>(&this->GraphObject);
            SE_Space::RetainSpaceObject(graphObject, createSpaceObjectByName);
        };
        if (GraphObject == nullptr) {
            if (pas::class_cast_if<TMicroModule*>(this) != nullptr) {
                if (aConst::MicroModuleTemplates[pas::checked_cast<TMicroModule*>(this)->MicroModuleIndex - 1].KindGraph != u"" && ([&] {
                    const pas::WideString& cpp_arg = pas::concat_wide({u"mm_", aConst::MicroModuleTemplates[pas::checked_cast<TMicroModule*>(this)->MicroModuleIndex - 1].KindGraph});
                    EC_BlockPar::TBlockParEC* block = GR_Main::GameDataConfig->GetBlock(u"SE"_wref.get())->GetBlock(u"Item"_wref.get());
                    return block->FindBlock(cpp_arg);
                }()) != nullptr) {
                    CreateContainer(pas::concat_wide({u"mm_", aConst::MicroModuleTemplates[pas::checked_cast<TMicroModule*>(this)->MicroModuleIndex - 1].KindGraph}));
                } else {
                    CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"mm_", SysUtils::IntToStr(aItem::GetMicroModulePriorityColorTier(pas::checked_cast<TMicroModule*>(this)->MicroModuleIndex - 1))})));
                }
            } else if (ItemType == aConst::t_ArtefactBomb) {
                CreateContainer(u"Bomb"_w);
            } else if (pas::class_cast_if<TUselessItem*>(this) != nullptr && ([&] {
                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Item"_wref.get());
                const pas::WideString& configBlockName = reinterpret_cast<TUselessItem*>(this)->ConfigBlockName;
                return blockByPath->CountBlocks(configBlockName);
            }()) > 0) {
                CreateContainer(reinterpret_cast<TUselessItem*>(this)->ConfigBlockName);
            } else if (pas::class_cast_if<TCistern*>(this) != nullptr) {
                CreateContainer(u"Cistern"_w);
            } else if (pas::class_cast_if<TGoods*>(this) != nullptr && static_cast<TGoods*>(this)->NaturalFlag) {
                if (Weight <= 29) {
                    CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"m0_", SysUtils::IntToStr(aMyFunction::SeededRandomIntRange(0, 2, Id * 25457))})));
                } else if (Weight <= 59) {
                    CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"m1_", SysUtils::IntToStr(aMyFunction::SeededRandomIntRange(0, 2, Id * 25457))})));
                } else {
                    CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"m2_", SysUtils::IntToStr(aMyFunction::SeededRandomIntRange(0, 2, Id * 25457))})));
                }
            } else if (pas::class_cast_if<TProtoplasm*>(this) != nullptr && static_cast<TProtoplasm*>(this)->DropFlag != 0) {
                if (Weight <= 29) {
                    CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"n0_", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Id) % 5)})));
                } else if (Weight <= 59) {
                    CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"n1_", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Id) % 5)})));
                } else if (Weight <= 99) {
                    CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"n2_", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Id) % 5)})));
                } else {
                    CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"n3_", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Id) % 5)})));
                }
            } else if (pas::class_cast_if<TCountableItem*>(this) != nullptr && static_cast<TCountableItem*>(this)->DropFlag != 0) {
                if (Weight <= 29) {
                    CreateContainer(pas::concat_wide({pas::checked_cast<TCountableItem*>(this)->ConfigBlockName, u"0_", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 5))}));
                } else if (Weight <= 59) {
                    CreateContainer(pas::concat_wide({pas::checked_cast<TCountableItem*>(this)->ConfigBlockName, u"1_", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 5))}));
                } else if (Weight <= 99) {
                    CreateContainer(pas::concat_wide({pas::checked_cast<TCountableItem*>(this)->ConfigBlockName, u"2_", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 5))}));
                } else {
                    CreateContainer(pas::concat_wide({pas::checked_cast<TCountableItem*>(this)->ConfigBlockName, u"3_", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 5))}));
                }
            } else if (pas::class_cast_if<TEquipment*>(this) != nullptr && static_cast<TEquipment*>(this)->CustomFaction != u"") {
                if (Weight <= 29) {
                    CreateContainer(pas::concat_wide({pas::checked_cast<TEquipment*>(this)->CustomFaction, u"0"}));
                } else if (Weight <= 59) {
                    CreateContainer(pas::concat_wide({pas::checked_cast<TEquipment*>(this)->CustomFaction, u"1"}));
                } else if (Weight <= 99) {
                    CreateContainer(pas::concat_wide({pas::checked_cast<TEquipment*>(this)->CustomFaction, u"2"}));
                } else {
                    CreateContainer(pas::concat_wide({pas::checked_cast<TEquipment*>(this)->CustomFaction, u"3"}));
                }
            } else if (pas::class_cast_if<TEquipment*>(this) != nullptr && OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                if (pas::checked_cast<TEquipment*>(this)->DominatorSeries == aGalaxyStruct::dsBlazer) {
                    if (Weight <= 29) {
                        CreateContainer(u"db0"_w);
                    } else if (Weight <= 59) {
                        CreateContainer(u"db1"_w);
                    } else if (Weight <= 99) {
                        CreateContainer(u"db2"_w);
                    } else {
                        CreateContainer(u"db3"_w);
                    }
                } else if (pas::checked_cast<TEquipment*>(this)->DominatorSeries == aGalaxyStruct::dsKeller) {
                    if (Weight <= 29) {
                        CreateContainer(u"dk0"_w);
                    } else if (Weight <= 59) {
                        CreateContainer(u"dk1"_w);
                    } else if (Weight <= 99) {
                        CreateContainer(u"dk2"_w);
                    } else {
                        CreateContainer(u"dk3"_w);
                    }
                } else if (pas::checked_cast<TEquipment*>(this)->DominatorSeries == aGalaxyStruct::dsTerron) {
                    if (Weight <= 29) {
                        CreateContainer(u"dt0"_w);
                    } else if (Weight <= 59) {
                        CreateContainer(u"dt1"_w);
                    } else if (Weight <= 99) {
                        CreateContainer(u"dt2"_w);
                    } else {
                        CreateContainer(u"dt3"_w);
                    }
                } else {
                    GR_Main::RaiseWideMessage(u"Item graph"_wref.get());
                }
            } else if (Weight <= 29) {
                CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"c0_", SysUtils::IntToStr(aMyFunction::SeededRandomIntRange(0, 7, Id * 25457))})));
            } else if (Weight <= 59) {
                CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"c1_", SysUtils::IntToStr(aMyFunction::SeededRandomIntRange(0, 7, Id * 25457))})));
            } else if (Weight <= 99) {
                CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"c2_", SysUtils::IntToStr(aMyFunction::SeededRandomIntRange(0, 7, Id * 25457))})));
            } else {
                CreateContainer(static_cast<pas::WideString>(pas::concat_ansi({"c3_", SysUtils::IntToStr(aMyFunction::SeededRandomIntRange(0, 7, Id * 25457))})));
            }
            GraphObject->SetPosition(Position);
        }
        return GraphObject;
    }

    void TItem::ReleaseGraphObject() {
        if (GraphObject != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&GraphObject));
        }
    }

    // Manufacturer/faction resource key, including custom factions and Dominator series.
    pas::WideString TItem_GetOwnerConfigName(TItem* Self) {
        if (pas::class_cast_if<TGoods*>(Self) != nullptr) {
            return aConst::OwnerInfo[aGalaxyStruct::oiUninhabited].InternalName;
        } else if (reinterpret_cast<TEquipment*>(Self)->CustomFaction != u"") {
            return reinterpret_cast<TEquipment*>(Self)->CustomFaction;
        } else if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && pas::class_cast_if<TEquipment*>(Self) != nullptr) {
            return aConst::DominatorSeriesNames[reinterpret_cast<TEquipment*>(Self)->DominatorSeries];
        } else if (pas::class_cast_if<THull*>(Self) != nullptr && static_cast<THull*>(Self)->PirateBuilt) {
            return pas::concat_wide({aConst::OwnerInfo[aGalaxyStruct::oiPirate].InternalName, aConst::OwnerToSys(Self->OwnerId)});
        } else {
            return aConst::OwnerInfo[Self->OwnerId].InternalName;
        }
    }

    void TEquipment_Create(TEquipment* Self) {
        aItem::TItem_Create(Self);
        Self->ExtraSpecials = nullptr;
        Self->EquippedFlag = 0;
    }

    void TEquipment_Destroy(TEquipment* Self) {
        std::int32_t I{};
        PExtraSpecial Entry{};
        if (Self->ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(Self->ExtraSpecials, I);
                pas::dispose(Entry);
            }
            pas::free(Self->ExtraSpecials);
        }
        Self->ExtraSpecials = nullptr;
        aItem::TItem_Destroy(Self);
    }

    void TEquipment::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t Index{};
        std::int32_t ModuleIndex{};
        PExtraSpecial Entry{};
        aItem::TItem::SaveToBuffer(Buffer);
        if (CustomFaction == u"") {
            Buffer->AddBoolean(false);
        } else {
            Buffer->AddBoolean(true);
            Buffer->AddWideStringZ(CustomFaction);
        }
        if (ConfigBlockName == u"") {
            Buffer->AddBoolean(false);
        } else {
            Buffer->AddBoolean(true);
            Buffer->AddWideStringZ(ConfigBlockName);
        }
        Buffer->AddBoolean(EquippedFlag);
        Buffer->AddSingle(ConditionPercent);
        Buffer->AddBoolean(BrokenFlag);
        Buffer->AddAnsiChar(AssignedSlotData);
        Buffer->AddIntegerValue(MicroModuleIndex);
        if (MicroModuleIndex > 0) {
            Buffer->AddDWord(aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNameHash);
        }
        Buffer->AddIntegerValue(SpecialModuleIndex);
        if (SpecialModuleIndex > 0) {
            Buffer->AddDWord(aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNameHash);
        }
        if (ExtraSpecials == nullptr) {
            Buffer->AddIntegerValue(0);
        } else {
            Buffer->AddIntegerValue(pas::list_count(ExtraSpecials));
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(Index); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, Index);
                ModuleIndex = Entry->ModuleIndexPlusOne;
                Buffer->AddIntegerValue(ModuleIndex);
                if (ModuleIndex > 0) {
                    Buffer->AddDWord(aConst::MicroModuleTemplates[ModuleIndex - 1].ConfigNameHash);
                }
                Buffer->AddIntegerValue(Entry->Count);
            }
        }
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(DominatorSeries));
    }

    void TEquipment::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t ConfigNumber{};
        std::int32_t Count{};
        std::int32_t Index{};
        std::int32_t ExistingIndex{};
        std::int32_t ModuleIndex{};
        PExtraSpecial Entry{};
        // Returns a one-based template index, or 0 if absent.
        auto FindLegacyMicroModuleIndex = [&](std::int32_t ConfigNumber) -> std::int32_t {
            std::int32_t I{};
            std::int32_t Result = 0;
            {
                const std::int32_t cpp_last = aConst::MicroModuleTemplates.length() - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        if (aConst::MicroModuleTemplates[I].ConfigNumber == ConfigNumber) {
                            Result = I + 1;
                            break;
                        }
                    }
                }
            }
            return Result;
        };
        aItem::TItem::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion >= 152) {
            if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                CustomFaction = Buffer->ReadWideString();
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 86) {
            if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                ConfigBlockName = Buffer->ReadWideString();
            }
        }
        EquippedFlag = EC_Buf::TBufEC_GetBoolean(Buffer);
        ConditionPercent = EC_Buf::TBufEC_GetSingle(Buffer);
        BrokenFlag = EC_Buf::TBufEC_GetBoolean(Buffer);
        AssignedSlotData = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 157) {
            MicroModuleIndex = aItem::ReadSavedMicroModuleIndex(Buffer);
            SpecialModuleIndex = aItem::ReadSavedMicroModuleIndex(Buffer);
        } else if (GlobalsV::LoadedSaveVersion >= 67) {
            MicroModuleIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            SpecialModuleIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 98) {
                if (MicroModuleIndex > 0) {
                    ConfigNumber = EC_Buf::TBufEC_GetInt32(Buffer);
                    if (aConst::MicroModuleTemplates.length() - 1 + 1 < MicroModuleIndex || aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber != ConfigNumber) {
                        MicroModuleIndex = FindLegacyMicroModuleIndex(ConfigNumber);
                    }
                }
                if (SpecialModuleIndex > 0) {
                    ConfigNumber = EC_Buf::TBufEC_GetInt32(Buffer);
                    if (aConst::MicroModuleTemplates.length() - 1 + 1 < SpecialModuleIndex || aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNumber != ConfigNumber) {
                        SpecialModuleIndex = FindLegacyMicroModuleIndex(ConfigNumber);
                    }
                }
            }
        } else {
            MicroModuleIndex = EC_Buf::TBufEC_GetByte(Buffer);
            SpecialModuleIndex = EC_Buf::TBufEC_GetByte(Buffer);
        }
        if (GlobalsV::LoadedSaveVersion < 98) {
            if (MicroModuleIndex != 0) {
                if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber > 24) {
                    ++MicroModuleIndex;
                }
                if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber > 124) {
                    ++MicroModuleIndex;
                }
                if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber > 219) {
                    ++MicroModuleIndex;
                }
            }
            if (SpecialModuleIndex != 0) {
                if (aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNumber > 24) {
                    ++SpecialModuleIndex;
                }
                if (aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNumber > 124) {
                    ++SpecialModuleIndex;
                }
                if (aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNumber > 219) {
                    ++SpecialModuleIndex;
                }
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 116) {
            Count = EC_Buf::TBufEC_GetInt32(Buffer);
            if (Count > 0) {
                ExtraSpecials = pas::make_object<pas::List>();
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                    if (GlobalsV::LoadedSaveVersion >= 157) {
                        ModuleIndex = aItem::ReadSavedMicroModuleIndex(Buffer);
                    } else {
                        ModuleIndex = EC_Buf::TBufEC_GetInt32(Buffer);
                        ConfigNumber = EC_Buf::TBufEC_GetInt32(Buffer);
                        if (aConst::MicroModuleTemplates.length() - 1 + 1 < ModuleIndex || aConst::MicroModuleTemplates[ModuleIndex - 1].ConfigNumber != ConfigNumber) {
                            ModuleIndex = FindLegacyMicroModuleIndex(ConfigNumber);
                        }
                    }
                    if (ModuleIndex == 0) {
                        ModuleIndex = 1;
                    }
                    if (GlobalsV::LoadedSaveVersion >= 140) {
                        pas::new_value(Entry);
                        Entry->ModuleIndexPlusOne = ModuleIndex;
                        Entry->Count = EC_Buf::TBufEC_GetInt32(Buffer);
                        pas::list_add(ExtraSpecials, static_cast<void*>(Entry));
                    } else {
                        Entry = nullptr;
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range_2.next(ExistingIndex); ) {
                            if (pas::list_at<TExtraSpecial>(ExtraSpecials, ExistingIndex)->ModuleIndexPlusOne == ModuleIndex) {
                                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, ExistingIndex);
                                ++Entry->Count;
                                break;
                            }
                        }
                        if (Entry == nullptr) {
                            pas::new_value(Entry);
                            Entry->ModuleIndexPlusOne = ModuleIndex;
                            Entry->Count = 1;
                            pas::list_add(ExtraSpecials, static_cast<void*>(Entry));
                        }
                    }
                }
            }
        }
        DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(EC_Buf::TBufEC_GetByte(Buffer));
    }

    // Allocates a new item ID and resolves references in the current galaxy; changes LoadedSaveVersion.
    aItem::TItem* TEquipment::Clone() {
        std::uint32_t NewId = aGalaxy::Galaxy->NextItemId;
        aItem::TItem* Result = aItem::CreateItemByType(ItemType);
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        SaveToBuffer(Buffer);
        Buffer->SetPosition(0);
        GlobalsV::LoadedSaveVersion = aConst::CurrentSaveVersion;
        Result->LoadFromBuffer(Buffer, aGalaxy::Galaxy);
        Result->ResolveLoadedReferences(aGalaxy::Galaxy);
        Result->Id = NewId;
        aGalaxy::Galaxy->NextItemId = NewId + 1;
        pas::free(Buffer);
        return Result;
    }

    void TEquipment::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        PExtraSpecial Entry{};
        pas::WideString Text{};
        pas::WideString ModuleName{};
        aItem::TItem::SaveToBlock(Block);
        {
            const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(ConditionPercent), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"DyuRrdawbRiblNijtSyp"_w);
            Block->AddParam(decodeTextW, cpp_arg);
        }
        {
            const pas::WideString& boolToWideString = EC_Str::BoolToWideString(BrokenFlag);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"BorYorkNeln"_w);
            Block->AddParam(decodeTextW_2, boolToWideString);
        }
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(MicroModuleIndex);
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"BrognWulso"_w);
            Block->AddParam(decodeTextW_3, intToStr);
        }
        if (MicroModuleIndex != 0) {
            Block->AddParam(EC_Str::DecodeTextW(u"IQBaodn4ursTNgatm2e"_w), aConst::MicroModuleTemplates[MicroModuleIndex - 1].Name);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(SpecialModuleIndex);
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"SrpeeIcjigaEl4"_w);
            Block->AddParam(decodeTextW_4, intToStr_2);
        }
        if (SpecialModuleIndex != 0) {
            Block->AddParam(EC_Str::DecodeTextW(u"IaSopRefcGihajl6NtaEm3ew"_w), aConst::MicroModuleTemplates[SpecialModuleIndex - 1].Name);
        }
        if (ExtraSpecials != nullptr) {
            Text = pas::WideString();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                if (Text != u"") {
                    Text = pas::concat_wide({Text, u", "});
                }
                if (Entry->ModuleIndexPlusOne <= 0 || Entry->ModuleIndexPlusOne > aConst::MicroModuleTemplateCount) {
                    ModuleName = pas::concat_wide({u"<", EC_Str::IntToWideString(Entry->ModuleIndexPlusOne - 1), u">"});
                } else {
                    ModuleName = aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].Name;
                    if (ModuleName == u"") {
                        ModuleName = pas::concat_wide({u"[", aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].ConfigName, u"]"});
                    }
                }
                if (Entry->Count != 1) {
                    Text = pas::concat_wide({Text, EC_Str::IntToWideString(Entry->Count), u"x", ModuleName});
                } else {
                    Text = pas::concat_wide({Text, ModuleName});
                }
            }
            if (Text != u"") {
                Block->AddParam(EC_Str::DecodeTextW(u"IaEoxRtfrGahSjp6etcEi3awlhs4"_w), Text);
            }
        }
        Block->AddParam(EC_Str::DecodeTextW(u"D9o5meScewr3iwegs4"_w), aConst::DominatorSeriesNames[DominatorSeries]);
    }

    void TEquipment_LoadFromBlock(TEquipment* Self, EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        pas::WideString Text{};
        aItem::TItem_LoadFromBlock(Self, Block);
        Self->ConditionPercent = EC_Str::ExtractDecimalToSingleW(Block->GetParam(EC_Str::DecodeTextW(u"DyuRrdawbRiblNijtSyp"_w)));
        Self->BrokenFlag = SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"BorYorkNeln"_w)))) == "true";
        Self->MicroModuleIndex = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"BrognWulso"_w))));
        Self->SpecialModuleIndex = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"SrpeeIcjigaEl4"_w))));
        Text = Block->GetParam(EC_Str::DecodeTextW(u"D9o5meScewr3iwegs4"_w));
        for (I = 0; I <= 2; ++I) {
            if (Text == aConst::DominatorSeriesNames[static_cast<std::uint8_t>(I)]) {
                Self->DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(I);
            }
        }
    }

    void TEquipment::Equip() {
        EquippedFlag = 1;
        if (aGalaxy::Galaxy == nullptr || aGalaxy::Galaxy->Destroying || aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (ScriptItem != nullptr) {
            reinterpret_cast<aScript::TScriptItem*>(ScriptItem)->RunActionCode(aConst::satOnItemEquip, nullptr, nullptr, nullptr, 0);
        }
        if (pas::class_cast_if<TEquipmentWithActCode*>(this) != nullptr) {
            aScript::RunItemConfigActionCode(this, aConst::satOnItemEquip, nullptr, nullptr, nullptr, 0);
        }
    }

    void TEquipment::Unequip() {
        EquippedFlag = 0;
        if (aGalaxy::Galaxy == nullptr || aGalaxy::Galaxy->Destroying || aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (ScriptItem != nullptr) {
            reinterpret_cast<aScript::TScriptItem*>(ScriptItem)->RunActionCode(aConst::satOnItemDeEquip, nullptr, nullptr, nullptr, 0);
        }
        if (pas::class_cast_if<TEquipmentWithActCode*>(this) != nullptr) {
            aScript::RunItemConfigActionCode(this, aConst::satOnItemDeEquip, nullptr, nullptr, nullptr, 0);
        }
    }

    void TEquipment::Repair() {
        ConditionPercent = 1.0E+2;
        BrokenFlag = 0;
    }

    // Tests hull damage or supported equipment below 90 percent condition.
    std::uint8_t TEquipment::NeedsRepair() {
        static const pas::Set<0, 255> RepairableTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        if (ItemType == aConst::t_Hull) {
            std::int32_t cpp_left = pas::checked_cast<THull*>(this)->HullPoints;
            return cpp_left < pas::checked_cast<THull*>(this)->Weight;
        }
        return pas::contains(RepairableTypes, static_cast<std::uint8_t>(ItemType)) && ConditionPercent < 9.0E+1L;
    }

    // Undiscounted cost; hulls use HullPoints, other supported equipment uses ConditionPercent and BrokenFlag.
    std::int32_t TEquipment_CalculateRepairCost(TEquipment* Self) {
        std::int32_t Result{};
        double DamagePercent{};
        if (pas::class_cast_if<TCountableItem*>(Self) != nullptr) {
            return 0;
        }
        if (Self->ItemType == aConst::t_Hull) {
            Result = 0;
            {
                std::int32_t cpp_left = pas::checked_cast<THull*>(Self)->Weight;
                if (cpp_left - pas::checked_cast<THull*>(Self)->HullPoints != 0) {
                    {
                        std::int32_t cpp_left_2 = pas::checked_cast<THull*>(Self)->Weight;
                        pas::Extended cpp_right = cpp_left_2 - pas::checked_cast<THull*>(Self)->HullPoints;
                        DamagePercent = pas::real_divide(1.0E+2L, pas::checked_cast<THull*>(Self)->Weight) * cpp_right;
                    }
                    return aMyFunction::RoundAndTruncateToTens(pas::real_divide(Self->Cost / 40, 1.0E+2L) * DamagePercent + 1.0E+1L);
                }
            }
            return Result;
        } else if (pas::in_set<aConst::t_FuelTanks, aConst::t_CustomWeapon, aConst::t_Satellite, aConst::t_Satellite>(Self->ItemType)) {
            if (Self->ConditionPercent == 1.0E+2L) {
                Result = 0;
            } else {
                pas::Extended cpp_left_3 = 100 - System::Round(Self->ConditionPercent);
                Result = aMyFunction::RoundAndTruncateToTens(cpp_left_3 * pas::real_divide(Self->Cost / 7, 1.0E+2L) + Self->Cost / 20 + 1.0E+1L);
            }
            if (Self->BrokenFlag != 0) {
                return System::Round(Result * 1.3L);
            }
            return Result;
        } else if (pas::in_set<aConst::t_Artefact, aConst::t_Artefact, aConst::t_ArtefactHull, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtBio, aConst::t_ArtFastRacks>(Self->ItemType)) {
            if (Self->ConditionPercent == 1.0E+2L) {
                Result = 0;
            } else {
                pas::Extended cpp_left_4 = 100 - System::Round(Self->ConditionPercent);
                Result = aMyFunction::RoundAndTruncateToTens(cpp_left_4 * pas::real_divide(Self->Cost / 5, 1.0E+2L) + Self->Cost / 10 + 1.0E+1L);
            }
            if (Self->BrokenFlag != 0) {
                Result = System::Round(Result * 1.3L);
            }
            return Result * 2;
        } else {
            return 0;
        }
    }

    // Includes player technology restrictions as well as wear and breakage.
    pas::WideString TEquipment::GetConditionText(std::uint8_t PrefixNewLine) {
        pas::WideString Result{};
        static const pas::Set<0, 255> SupportedTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        pas::WideString Prefix{};
        if (!pas::contains(SupportedTypes, static_cast<std::uint8_t>(ItemType))) {
            return pas::WideString();
        }
        if (PrefixNewLine) {
            Prefix = u"\r\n"_w;
        } else {
            Prefix = pas::WideString();
        }
        if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aShip::TShip_CanUseEquipmentTech(aPlayer::GetPlayer(), this) ^ 1)) {
            return aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(u"Items.Equpments.CanNotBeUsed"_wref.get())}), u"<color=255,0,0>"_w);
        }
        if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aShip::TShip_CanRepairEquipmentTech(aPlayer::GetPlayer(), this) ^ 1) && BrokenFlag != 0 && static_cast<std::uint8_t>(pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_Engine)) ^ 1)) {
            return aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(u"Items.Equpments.CanNotBeUsed"_wref.get())}), u"<color=255,0,0>"_w);
        }
        if (BrokenFlag != 0) {
            if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
                Result = aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".Broken"}))}), u"<color=255,0,0>"_w);
            } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                Result = aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(u"Items.Weapon.Broken"_wref.get())}), u"<color=255,0,0>"_w);
            } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
                Result = aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(pas::concat_wide({u"Artefacts.CustomArtefacts.", ConfigBlockName, u".Broken"}))}), u"<color=255,0,0>"_w);
            } else if (pas::in_set<aConst::t_Artefact, aConst::t_Artefact, aConst::t_ArtefactHull, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtBio, aConst::t_ArtFastRacks>(ItemType)) {
                Result = aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[ItemType], u".Broken"}))}), u"<color=255,0,0>"_w);
            } else if (ItemType == aConst::t_Satellite) {
                Result = aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(u"Items.Satellite.Broken"_wref.get())}), u"<color=255,0,0>"_w);
            } else {
                Result = pas::WideString();
            }
        } else if (pas::class_cast_if<TArtefact*>(this) != nullptr) {
            // Native retains this transmitter branch despite the initial supported-type set.
            if (ItemType == aConst::t_ArtefactTransmitter && pas::checked_cast<TArtefactTransmitter*>(this)->Power < aConst::MinTransmitterPower) {
                Result = aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(u"Artefacts.ArtTransmitter.Broken"_wref.get())}), u"<color=254,217,7>"_w);
            } else {
                Result = pas::WideString();
            }
        } else if (ConditionPercent < 2.0E+1L) {
            Result = aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(u"Items.Equpments.SmallDuration"_wref.get())}), u"<color=254,217,7>"_w);
        } else if (ConditionPercent < 5.0E+1L) {
            Result = aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(u"Items.Equpments.AverageDuration"_wref.get())}), u"<color=127,127,127>"_w);
        } else {
            Result = pas::WideString();
        }
        if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aShip::TShip_CanRepairEquipmentTech(aPlayer::GetPlayer(), this) ^ 1)) {
            if (!PrefixNewLine) {
                Result = pas::WideString();
            }
            return pas::concat_wide({aMyFunction::WrapTextInColor(pas::concat_wide({Prefix, aConst::LocalizedText(u"Items.Equpments.CanNotBeRepaired"_wref.get())}), u"<color=127,127,127>"_w), Result});
        }
        return Result;
    }

    pas::WideString TEquipment::GetBrokenInBattleText() {
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            return aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".BrokenInBattle"}));
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            pas::WideString displayName = GetDisplayName();
            pas::WideString localizedText = aConst::LocalizedText(u"Items.Weapon.BrokenInBattle"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(displayName));
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
            return aConst::LocalizedText(pas::concat_wide({u"Artefacts.CustomArtefacts.", ConfigBlockName, u".BrokenInBattle"}));
        } else if (pas::in_set<aConst::t_Artefact, aConst::t_Artefact, aConst::t_ArtefactHull, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtBio, aConst::t_ArtFastRacks>(ItemType)) {
            return aConst::LocalizedText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[ItemType], u".BrokenInBattle"}));
        } else {
            return pas::WideString();
        }
    }

    pas::WideString TEquipment::GetBrokenInUseText() {
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            return aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".BrokenInUse"}));
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            pas::WideString displayName = GetDisplayName();
            pas::WideString localizedText = aConst::LocalizedText(u"Items.Weapon.BrokenInUse"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(displayName));
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
            return aConst::LocalizedText(pas::concat_wide({u"Artefacts.CustomArtefacts.", ConfigBlockName, u".BrokenInUse"}));
        } else if (pas::in_set<aConst::t_Artefact, aConst::t_Artefact, aConst::t_ArtefactHull, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtBio, aConst::t_ArtFastRacks>(ItemType)) {
            return aConst::LocalizedText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[ItemType], u".BrokenInUse"}));
        } else {
            return pas::WideString();
        }
    }

    pas::WideString TEquipment::GetBrokenByForceText() {
        pas::WideString Result{};
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            Result = aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".BrokenByForce"}));
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            Result = ([&] {
                pas::WideString displayName = GetDisplayName();
                pas::WideString localizedText = aConst::LocalizedText(u"Items.Weapon.BrokenByForce"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(displayName));
            }());
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
            Result = aConst::LocalizedText(pas::concat_wide({u"Artefacts.CustomArtefacts.", ConfigBlockName, u".BrokenByForce"}));
        } else if (pas::in_set<aConst::t_Artefact, aConst::t_Artefact, aConst::t_ArtefactHull, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtBio, aConst::t_ArtFastRacks>(ItemType)) {
            Result = aConst::LocalizedText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[ItemType], u".BrokenByForce"}));
        } else {
            return Result;
        }
        if (Result == u"") {
            return GetBrokenInUseText();
        }
        return Result;
    }

    // Returns 0 for unsupported item types.
    std::int32_t TEquipment::GetLevel() {
        switch (ItemType) {
            case aConst::t_Hull: return pas::checked_cast<THull*>(this)->TechLevel;
            case aConst::t_FuelTanks: return pas::checked_cast<TFuelTanks*>(this)->TechLevel;
            case aConst::t_Engine: return pas::checked_cast<TEngine*>(this)->TechLevel;
            case aConst::t_Radar: return pas::checked_cast<TRadar*>(this)->TechLevel;
            case aConst::t_Scaner: return pas::checked_cast<TScaner*>(this)->TechLevel;
            case aConst::t_RepairRobot: return pas::checked_cast<TRepairRobot*>(this)->TechLevel;
            case aConst::t_CargoHook: return pas::checked_cast<TCargoHook*>(this)->TechLevel;
            case aConst::t_DefGenerator: return pas::checked_cast<TDefGenerator*>(this)->TechLevel;
            default: {
                if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    return pas::checked_cast<TWeapon*>(this)->TechLevel;
                }
                return 0;
            }
        }
    }

    // A through H for levels 1 through 8; empty for unsupported levels.
    pas::WideString TEquipment::GetLevelLetter() {
        pas::WideString Result{};
        std::int32_t Level = GetLevel();
        if (Level >= 1 && Level <= 8) {
            return EquipmentLevelLetters[Level];
        }
        return Result;
    }

    pas::WideString TEquipment::GetDescriptionText() {
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator)) && OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
            return aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".KlingDescription.", pas::wide_int_to_str(GetLevel())}));
        }
        return aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".Description.", pas::wide_int_to_str(GetLevel())}));
    }

    pas::WideString TEquipment::GetDisplayName() {
        pas::WideString Result{};
        pas::WideString TypeName{};
        if (NameOverride != u"") {
            Result = NameOverride;
        } else if (SpecialModuleIndex != 0) {
            Result = ([&] {
                pas::WideString microModuleTextColorTag = aItem::GetMicroModuleTextColorTag(SpecialModuleIndex - 1);
                pas::WideString specialModuleName = GetSpecialModuleName();
                return aMyFunction::WrapTextInColor(std::move(specialModuleName), std::move(microModuleTextColorTag));
            }());
        } else if (CustomFaction != u"") {
            Result = aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".", CustomFaction, u"Name"}));
        }
        if (Result == u"") {
            if (OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                TypeName = aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".Type.", pas::wide_int_to_str(GetLevel())}));
                Result = aMyFunction::ReplaceColoredToken(aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".Name"})), u"<Type>"_w, TypeName, pas::WideString());
            } else {
                Result = aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".KlingName"}));
            }
        }
        if (HasMicroModule()) {
            return pas::concat_wide({Result, u" ", ([&] {
                pas::WideString microModuleNameColorTag = aItem::GetMicroModuleNameColorTag(MicroModuleIndex - 1);
                pas::WideString microModuleQuotedName = GetMicroModuleQuotedName();
                return aMyFunction::WrapTextInColor(std::move(microModuleQuotedName), std::move(microModuleNameColorTag));
            }())});
        }
        return Result;
    }

    pas::WideString TEquipment::GetShortName() {
        if (NameOverride != u"") {
            return NameOverride;
        } else if (OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
            return aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".ShortName"}));
        } else {
            return aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".KlingName"}));
        }
    }

    // Selects strength and stat emphasis from Id, then calls Improve. Charging and eligibility checks belong to the caller.
    void TEquipment::ImproveAtScientificBase() {
        DetailImprovement = static_cast<std::uint32_t>(Id) % 2 + 1;
        {
            std::uint32_t cpp_case = static_cast<std::uint32_t>(Id) / 3 % 6;
            if (cpp_case == 0) {
                Improve(ikMajor);
            } else if (cpp_case >= 1 && cpp_case <= 2) {
                Improve(ikMedium);
            } else {
                Improve(ikMinor);
            }
        }
    }

    std::uint8_t TEquipment::HasMicroModule() {
        return MicroModuleIndex != 0;
    }

    pas::WideString TEquipment::GetMicroModuleQuotedName() {
        if (HasMicroModule()) {
            if (EC_Str::FindTextOffsetW(aConst::MicroModuleTemplates[MicroModuleIndex - 1].Name, u"\""_wref.get(), 0) >= 0) {
                return aConst::MicroModuleTemplates[MicroModuleIndex - 1].Name;
            }
            return pas::concat_wide({u"\"", aConst::MicroModuleTemplates[MicroModuleIndex - 1].Name, u"\""});
        }
        return pas::WideString();
    }

    pas::WideString TEquipment::GetSpecialModuleName() {
        if (SpecialModuleIndex != 0) {
            return aConst::MicroModuleTemplates[SpecialModuleIndex - 1].Name;
        }
        return pas::WideString();
    }

    // Base is a no-op. Overrides change statistics and Cost without checking CanImprove or charging money; ikAny selects one of the three strengths.
    void TEquipment::Improve(TImprovementKind Kind) {
    }

    // Only ikMinor, ikMedium and ikMajor are valid: Cost times 0.3, 0.6 or 1.2, rounded then truncated to a multiple of 10, before service discounts.
    std::int32_t TEquipment::CalculateImprovementCost(TImprovementKind Kind) {
        switch (Kind) {
            case ikMinor: return aMyFunction::RoundAndTruncateToTens(Cost * 0.3L);
            case ikMedium: return aMyFunction::RoundAndTruncateToTens(Cost * 0.6L);
            case ikMajor: return aMyFunction::RoundAndTruncateToTens(Cost * 1.2L);
            default: {
                GR_Main::RaiseWideMessage(u"\u041a\u043e\u0441\u044f\u043a \u0432 \u0443\u043b\u0443\u0447\u0448\u0435\u043d\u0438\u0438"_wref.get());
                return 0;
            }
        }
    }

    // Tests expected generated statistics after accounting for installed bonuses; this is not a stored upgraded flag. The base implementation returns True.
    std::uint8_t TEquipment::HasStandardStats() {
        return true;
    }

    pas::WideString TEquipment::GetBitmapResourceName() {
        pas::WideString Result{};
        pas::WideString Path{};
        if (ConfigBlockName != u"") {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName});
        } else if (SpecialModuleIndex > 0 && aConst::MicroModuleTemplates[SpecialModuleIndex - 1].KindGraph != u"") {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType], aConst::MicroModuleTemplates[SpecialModuleIndex - 1].KindGraph});
        } else if (CustomFaction != u"") {
            Path = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType], CustomFaction});
            if (GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({Path, u"a"})) && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({Path, u"i"})) && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({Path, u"s"}))) {
                Result = Path;
            }
        }
        if (Result == u"") {
            if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType], u"Kling0"});
            }
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType], pas::wide_int_to_str(GetLevel() - 1)});
        }
        return Result;
    }

    float TEquipment::GetFragilityFactor(aGalaxyStruct::TDamageFlagSet DamageFlags) {
        std::int32_t I{};
        PExtraSpecial Entry{};
        float Factor{};
        float Result = pas::real_divide(1.0L, pas::real_max<float>(0.001f, aConst::OwnerInfo[OwnerId].EquipmentDurabilityFactor));
        if (MicroModuleIndex != 0) {
            Result = static_cast<long double>(Result) * aConst::MicroModuleTemplates[MicroModuleIndex - 1].FragilityFactor;
        }
        if (SpecialModuleIndex != 0) {
            Result = static_cast<long double>(Result) * aConst::MicroModuleTemplates[SpecialModuleIndex - 1].FragilityFactor;
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                Factor = aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].FragilityFactor;
                if (std::fabs(Factor - 1.0L) > 1.0E-6L) {
                    if (Entry->Count == 1) {
                        Result = static_cast<long double>(Result) * Factor;
                    } else {
                        Result = Math::Power(Factor, Entry->Count) * Result;
                    }
                }
            }
        }
        return Result;
    }

    void TEquipment::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
    }

    std::int32_t TEquipment::GetStatBonus(aConst::TEquipmentBonusKind BonusKind) {
        std::int32_t Result{};
        std::int32_t I{};
        std::int32_t SpecialBonus{};
        PExtraSpecial Entry{};
        if (pas::in_set<aConst::bonSkill1, aConst::bonSkill6, aConst::bonStimCapacity, aConst::bonStimCapacity>(BonusKind) && MicroModuleIndex != 0) {
            Result = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
        } else {
            Result = 0;
        }
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && pas::in_set<aConst::bonWEnergy, aConst::bonWRadius, aConst::bonMissileSpeed, aConst::bonMissileSpeed>(BonusKind)) {
            return Result;
        }
        if (SpecialModuleIndex != 0) {
            SpecialBonus = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
        } else {
            SpecialBonus = 0;
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                SpecialBonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, BonusKind * sizeof(std::int32_t))) * Entry->Count;
            }
        }
        if (SpecialBonus != 0 && MicroModuleIndex != 0 && static_cast<std::uint8_t>(pas::is_one_of<aConst::bonExtraAkrinEff, aConst::bonExtraAkrinPenalty>(BonusKind) ^ 1)) {
            if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonMass), static_cast<std::int32_t>(aConst::bonMass)) == SpecialBonus > 0) {
                SpecialBonus += System::Round(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinPenalty * sizeof(std::int32_t))) * SpecialBonus * 1.0E-4L);
            } else {
                SpecialBonus += System::Round(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinEff * sizeof(std::int32_t))) * SpecialBonus * 1.0E-4L);
            }
        }
        Result += SpecialBonus;
        return Result;
    }

    // Aggregate used by the bonus description, with SeparatedNumbers effects handled separately.
    std::int32_t TEquipment::GetDescriptionStatBonus(aConst::TEquipmentBonusKind BonusKind) {
        std::int32_t Result{};
        std::int32_t Index{};
        std::int32_t EffectPercent{};
        std::int32_t PenaltyPercent{};
        PExtraSpecial Entry{};
        if (pas::in_set<aConst::bonSkill1, aConst::bonSkill6, aConst::bonStimCapacity, aConst::bonStimCapacity>(BonusKind) && MicroModuleIndex != 0 && static_cast<std::uint8_t>(aConst::MicroModuleTemplates[MicroModuleIndex - 1].SeparatedNumbers ^ 1)) {
            Result = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
        } else {
            Result = 0;
        }
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && pas::in_set<aConst::bonWEnergy, aConst::bonWRadius, aConst::bonMissileSpeed, aConst::bonMissileSpeed>(BonusKind)) {
            return Result;
        }
        std::int32_t CombinedBonus = 0;
        std::int32_t SeparatedBonus = 0;
        std::int32_t ExtraSeparatedBonus = 0;
        if (SpecialModuleIndex != 0) {
            if (aConst::MicroModuleTemplates[SpecialModuleIndex - 1].SeparatedNumbers) {
                SeparatedBonus = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
            } else {
                CombinedBonus = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
            }
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(Index); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, Index);
                if (aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].SeparatedNumbers) {
                    ExtraSeparatedBonus = CombinedBonus + pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, BonusKind * sizeof(std::int32_t))) * Entry->Count;
                } else {
                    CombinedBonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, BonusKind * sizeof(std::int32_t))) * Entry->Count;
                }
            }
        }
        if (MicroModuleIndex != 0) {
            EffectPercent = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinEff * sizeof(std::int32_t)));
            PenaltyPercent = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinPenalty * sizeof(std::int32_t)));
        } else {
            EffectPercent = 0;
            PenaltyPercent = 0;
        }
        if ((EffectPercent != 0 || PenaltyPercent != 0) && (CombinedBonus != 0 || SeparatedBonus != 0 || ExtraSeparatedBonus != 0) && static_cast<std::uint8_t>(pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonExtraAkrinEff), static_cast<std::int32_t>(aConst::bonExtraAkrinPenalty)) ^ 1)) {
            CombinedBonus = CombinedBonus + SeparatedBonus + ExtraSeparatedBonus;
            if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonMass), static_cast<std::int32_t>(aConst::bonMass)) == CombinedBonus > 0) {
                CombinedBonus += System::Round(CombinedBonus * PenaltyPercent * 1.0E-4L);
            } else {
                CombinedBonus += System::Round(CombinedBonus * EffectPercent * 1.0E-4L);
            }
            if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonMass), static_cast<std::int32_t>(aConst::bonMass)) == SeparatedBonus > 0) {
                SeparatedBonus += System::Round(SeparatedBonus * PenaltyPercent * 1.0E-4L);
            } else {
                SeparatedBonus += System::Round(SeparatedBonus * EffectPercent * 1.0E-4L);
            }
            if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonMass), static_cast<std::int32_t>(aConst::bonMass)) == ExtraSeparatedBonus > 0) {
                ExtraSeparatedBonus += System::Round(ExtraSeparatedBonus * PenaltyPercent * 1.0E-4L);
            } else {
                ExtraSeparatedBonus += System::Round(ExtraSeparatedBonus * EffectPercent * 1.0E-4L);
            }
            CombinedBonus = CombinedBonus - SeparatedBonus - ExtraSeparatedBonus;
        }
        return Result + CombinedBonus;
    }

    pas::WideString TEquipment_GetBonusDescription(TEquipment* Self, pas::WideString ColorTag) {
        pas::WideString Result{};
        pas::WideString Description{};
        std::int32_t EffectPercent{};
        std::int32_t PenaltyPercent{};
        std::int32_t StatBonus{};
        aConst::TEquipmentBonusKind BonusKind{};
        std::int32_t Index{};
        std::int32_t ModuleIndexPlusOne{};
        // Uses the parent description, bonus multipliers and color. RET 4 removes Result; the caller removes ParentFrame. Count 0 suppresses numeric bonuses.
        auto ExpandModuleTokens = [&](pas::WideString Text, std::int32_t ModuleIndexPlusOne, std::int32_t Count) -> pas::WideString {
            pas::WideString Result{};
            std::uint8_t BonusIndex{};
            std::int32_t Value{};
            // The native helper reads the captured description; Text remains an unused managed parameter.
            Result = Description;
            for (BonusIndex = static_cast<std::uint8_t>(0); BonusIndex <= static_cast<std::uint8_t>(42); ++BonusIndex) {
                Value = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[ModuleIndexPlusOne - 1].StatBonuses, BonusIndex * sizeof(std::int32_t)));
                Value *= Count;
                if (Count != 0 && EffectPercent != 0 && static_cast<std::uint8_t>(pas::in_range(BonusIndex, 29, 30) ^ 1)) {
                    if (pas::in_range(BonusIndex, 28, 28) == Value > 0) {
                        Value += System::Round(Value * PenaltyPercent * 1.0E-4L);
                    } else {
                        Value += System::Round(Value * EffectPercent * 1.0E-4L);
                    }
                }
                if (Value > 0) {
                    pas::WideString cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(Value)}));
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"<", aConst::EquipmentBonusNames[BonusIndex], u">"});
                    aMyFunction::ReplaceTextToken(Result, std::move(cpp_arg_2), std::move(cpp_arg), ColorTag);
                } else if (Value < 0) {
                    pas::WideString intToStr = pas::wide_int_to_str(Value);
                    pas::WideString cpp_arg_3 = pas::concat_wide({u"<", aConst::EquipmentBonusNames[BonusIndex], u">"});
                    aMyFunction::ReplaceTextToken(Result, std::move(cpp_arg_3), std::move(intToStr), ColorTag);
                } else {
                    aMyFunction::ReplaceTextToken(Result, pas::concat_wide({u"<", aConst::EquipmentBonusNames[BonusIndex], u">"}), u"--"_w, ColorTag);
                }
            }
            return Result;
        };
        Result = pas::WideString();
        if (Self->MicroModuleIndex != 0) {
            EffectPercent = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Self->MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinEff * sizeof(std::int32_t)));
            PenaltyPercent = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Self->MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinPenalty * sizeof(std::int32_t)));
        } else {
            EffectPercent = 0;
            PenaltyPercent = 0;
        }
        if (Self->SpecialModuleIndex != 0) {
            Description = aConst::LocalizedColorText(pas::concat_wide({u"MicroModuls.", aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].ConfigName, u".Text"}));
            if (aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].SeparatedNumbers) {
                Description = ExpandModuleTokens(Description, Self->SpecialModuleIndex, 1);
            }
            if (pas::class_cast_if<TWeapon*>(Self) != nullptr && aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace == u"" && Self->GetSpecialModuleName() != u"") {
                Result = pas::concat_wide({u"\r\n \r\n", aConst::LocalizedText(u"Items.Weapon.WSpecial"_wref.get()), u" ", aMyFunction::WrapTextInColor(Self->GetSpecialModuleName(), u"<color=255,240,100>"_w)});
                if (Description != u"") {
                    Result = pas::concat_wide({Result, u"\r\n", aMyFunction::WrapTextInColor(Description, aItem::GetMicroModuleTextColorTag(Self->SpecialModuleIndex - 1))});
                }
            } else if (Description != u"") {
                Result = pas::concat_wide({u"\r\n \r\n", aMyFunction::WrapTextInColor(Description, aItem::GetMicroModuleTextColorTag(Self->SpecialModuleIndex - 1))});
            }
        }
        if (Self->MicroModuleIndex != 0) {
            Description = aConst::LocalizedColorText(pas::concat_wide({u"MicroModuls.", aConst::MicroModuleTemplates[Self->MicroModuleIndex - 1].ConfigName, u".ExText"}));
            if (Description != u"") {
                if (aConst::MicroModuleTemplates[Self->MicroModuleIndex - 1].SeparatedNumbers) {
                    Description = ExpandModuleTokens(Description, Self->MicroModuleIndex, 0);
                }
                Result = pas::concat_wide({Result, u"\r\n", aMyFunction::WrapTextInColor(Description, aItem::GetMicroModuleTextColorTag(Self->MicroModuleIndex - 1))});
            }
        }
        if (Self->ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->ExtraSpecials) - 1); cpp_range.next(Index); ) {
                ModuleIndexPlusOne = pas::list_at<TExtraSpecial>(Self->ExtraSpecials, Index)->ModuleIndexPlusOne;
                if (ModuleIndexPlusOne != Self->MicroModuleIndex || aConst::MicroModuleTemplates[ModuleIndexPlusOne - 1].SeparatedNumbers) {
                    Description = aConst::LocalizedColorText(pas::concat_wide({u"MicroModuls.", aConst::MicroModuleTemplates[ModuleIndexPlusOne - 1].ConfigName, u".ExText"}));
                    if (Description != u"") {
                        aMyFunction::ReplaceTextToken(Description, u"<ExCount>"_w, pas::wide_int_to_str(pas::list_at<TExtraSpecial>(Self->ExtraSpecials, Index)->Count), ColorTag);
                        if (aConst::MicroModuleTemplates[ModuleIndexPlusOne - 1].SeparatedNumbers) {
                            Description = ExpandModuleTokens(Description, ModuleIndexPlusOne, pas::list_at<TExtraSpecial>(Self->ExtraSpecials, Index)->Count);
                        }
                        Result = pas::concat_wide({Result, u"\r\n", aMyFunction::WrapTextInColor(Description, aItem::GetMicroModuleTextColorTag(ModuleIndexPlusOne - 1))});
                    }
                }
            }
        }
        if (Result != u"") {
            for (auto cpp_range_2 = pas::for_to<aConst::TEquipmentBonusKind>(aConst::bonHull, aConst::bonNull); cpp_range_2.next(BonusKind); ) {
                StatBonus = Self->GetDescriptionStatBonus(BonusKind);
                if (StatBonus > 0) {
                    pas::WideString cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)}));
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"<", aConst::EquipmentBonusNames[BonusKind], u">"});
                    aMyFunction::ReplaceTextToken(Result, std::move(cpp_arg_2), std::move(cpp_arg), ColorTag);
                } else if (StatBonus < 0) {
                    pas::WideString intToStr = pas::wide_int_to_str(StatBonus);
                    pas::WideString cpp_arg_3 = pas::concat_wide({u"<", aConst::EquipmentBonusNames[BonusKind], u">"});
                    aMyFunction::ReplaceTextToken(Result, std::move(cpp_arg_3), std::move(intToStr), ColorTag);
                } else {
                    aMyFunction::ReplaceTextToken(Result, pas::concat_wide({u"<", aConst::EquipmentBonusNames[BonusKind], u">"}), u"--"_w, ColorTag);
                }
            }
        }
        return Result;
    }

    // Requires HasStandardStats and no special module that blocks the special slot; does not check technology access.
    std::uint8_t TEquipment::CanImprove() {
        return HasStandardStats() && (SpecialModuleIndex == 0 || static_cast<std::uint8_t>(aConst::MicroModuleTemplates[SpecialModuleIndex - 1].BlocksSpecialSlot ^ 1));
    }

    void THull::Init(std::int32_t Capacity, std::uint8_t Level, std::uint8_t Owner, std::uint8_t HullType, std::int32_t Series, std::uint8_t PirateBuilt) {
        ItemType = aConst::t_Hull;
        OwnerShip = nullptr;
        Weight = Capacity;
        HullPoints = Weight;
        this->HullType = HullType;
        TechLevel = Level;
        OwnerId = Owner;
        Armor = CalculateGeneratedArmor();
        Repair();
        Cost = CalculateGeneratedCost();
        MicroModuleIndex = 0;
        HullSeries = Series;
        this->PirateBuilt = PirateBuilt;
        ApplySeriesSizeAndCost();
        CapitalShip = 0;
        ImpulseShieldsEnabled = false;
        InterceptorsEnabled = false;
        Energy = 0;
        EnergyMax = 0;
        InterceptorTarget = nullptr;
    }

    // Applies the current hull series, resets HullPoints to capacity, and bounds Cost.
    void THull::ApplySeriesSizeAndCost() {
        std::int32_t NewWeight{};
        if (HullSeries != -1) {
            NewWeight = System::Round(pas::real_divide(Weight, 1.0E+2L) * aConst::HullSeriesDefinitions[HullSeries].SizePercent);
            if (NewWeight < Weight) {
                NewWeight = std::min<std::int32_t>(Weight, std::max<std::int32_t>(250, NewWeight));
            }
            Weight = NewWeight;
            HullPoints = Weight;
            Cost = aMyFunction::RoundAndTruncateToTens(pas::real_divide(Cost, 1.0E+2L) * aConst::HullSeriesDefinitions[HullSeries].CostPercent);
            if (Cost < 0 || Cost > 100000000) {
                Cost = 100000000;
            }
        }
    }

    void THull::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddIntegerValue(HullPoints);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddAnsiChar(Armor);
        Buffer->AddAnsiChar(HullType);
        Buffer->AddIntegerValue(HullSeries);
        if (HullSeries != -1) {
            Buffer->AddDWord(aConst::HullSeriesDefinitions[HullSeries].SystemNameCRC);
        }
        Buffer->AddBoolean(PirateBuilt);
        Buffer->AddAnsiChar(CapitalShip);
        Buffer->AddBoolean(ImpulseShieldsEnabled);
        Buffer->AddBoolean(InterceptorsEnabled);
        Buffer->AddIntegerValue(Energy);
        Buffer->AddIntegerValue(EnergyMax);
        if (InterceptorsEnabled) {
            if (InterceptorTarget != nullptr) {
                Buffer->AddDWord(pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(InterceptorTarget))->Id);
            } else {
                Buffer->AddDWord(0u);
            }
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(InterceptorTargetingStrategy));
            Buffer->AddAnsiChar(InterceptorPassCountOverride);
        }
    }

    void THull::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        // Returns a zero-based series index, or -1 if absent or unresolved.
        auto ReadSavedHullSeriesIndex = [&](EC_Buf::TBufEC* Buffer) -> std::int32_t {
            std::int32_t I{};
            std::uint32_t CRC{};
            std::int32_t Result = EC_Buf::TBufEC_GetInt32(Buffer);
            if (Result != -1) {
                CRC = EC_Buf::TBufEC_GetUInt32(Buffer);
                if (Result > aConst::HullSeriesDefinitions.length() - 1 || aConst::HullSeriesDefinitions[Result].SystemNameCRC != CRC) {
                    Result = -1;
                    {
                        const std::int32_t cpp_last = aConst::HullSeriesDefinitions.length() - 1;
                        if (0 <= cpp_last) {
                            for (I = 0; I <= cpp_last; ++I) {
                                if (aConst::HullSeriesDefinitions[I].SystemNameCRC == CRC) {
                                    Result = I;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            return Result;
        };
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion >= 51) {
            HullPoints = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            HullPoints = EC_Buf::TBufEC_GetWord(Buffer);
        }
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        Armor = EC_Buf::TBufEC_GetByte(Buffer);
        std::int32_t SavedHullType = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 88) {
            HullType = SavedHullType;
        } else if (GlobalsV::LoadedSaveVersion >= 67) {
            if (SavedHullType > 6 && SavedHullType < 24) {
                SavedHullType = 6;
            }
            if (SavedHullType >= 24) {
                SavedHullType -= 17;
            }
            HullType = SavedHullType;
        } else if (SavedHullType > 26) {
            HullType = aGalaxyStruct::htSpecial;
        } else {
            if (SavedHullType > 6 && SavedHullType < 24) {
                SavedHullType = 6;
            }
            if (SavedHullType >= 24) {
                SavedHullType -= 17;
            }
            HullType = SavedHullType;
        }
        if (HullType == aGalaxyStruct::htSpecial && SpecialModuleIndex == 0) {
            HullType = aGalaxyStruct::htRanger;
        }
        if (GlobalsV::LoadedSaveVersion >= 163) {
            HullSeries = ReadSavedHullSeriesIndex(Buffer);
        } else {
            HullSeries = EC_Buf::TBufEC_GetByte(Buffer);
            if (HullSeries == 255 || aConst::HullSeriesDefinitions.length() - 1 < HullSeries) {
                HullSeries = -1;
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 50) {
            PirateBuilt = EC_Buf::TBufEC_GetBoolean(Buffer);
        } else {
            PirateBuilt = false;
        }
        if (GlobalsV::LoadedSaveVersion >= 51) {
            CapitalShip = EC_Buf::TBufEC_GetByte(Buffer);
            ImpulseShieldsEnabled = EC_Buf::TBufEC_GetBoolean(Buffer);
            InterceptorsEnabled = EC_Buf::TBufEC_GetBoolean(Buffer);
            Energy = EC_Buf::TBufEC_GetInt32(Buffer);
            EnergyMax = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            CapitalShip = 0;
            ImpulseShieldsEnabled = false;
            InterceptorsEnabled = false;
            Energy = 0;
            EnergyMax = 0;
        }
        if (GlobalsV::LoadedSaveVersion >= 52) {
            if (InterceptorsEnabled) {
                InterceptorTarget = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
                InterceptorTargetingStrategy = static_cast<TInterceptorTargetingStrategy>(EC_Buf::TBufEC_GetByte(Buffer));
                InterceptorPassCountOverride = EC_Buf::TBufEC_GetByte(Buffer);
            } else {
                InterceptorTarget = nullptr;
                InterceptorTargetingStrategy = itsManual;
                InterceptorPassCountOverride = 0;
            }
        } else if (GlobalsV::LoadedSaveVersion == 51) {
            InterceptorTarget = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            InterceptorTargetingStrategy = itsManual;
            InterceptorPassCountOverride = 0;
        } else {
            InterceptorTarget = nullptr;
            InterceptorTargetingStrategy = itsManual;
            InterceptorPassCountOverride = 0;
        }
    }

    void THull::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        if (InterceptorTarget != nullptr) {
            InterceptorTarget = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(InterceptorTarget)), false)));
        }
    }

    void THull::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(HullPoints);
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Hristophorisnotuse"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(Armor));
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"Alrumuotr"_w);
            Block->AddParam(decodeTextW_3, intToStr_3);
        }
        {
            const pas::WideString& intToStr_4 = pas::wide_int_to_str(static_cast<std::int32_t>(HullType));
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"SohtiEprTtyopwec"_w);
            Block->AddParam(decodeTextW_4, intToStr_4);
        }
        {
            const pas::WideString& intToStr_5 = pas::wide_int_to_str(HullSeries);
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"Stearoidess"_w);
            Block->AddParam(decodeTextW_5, intToStr_5);
        }
        if (HullSeries != -1) {
            const pas::WideString& seriesName = GetSeriesName();
            const pas::WideString& decodeTextW_6 = EC_Str::DecodeTextW(u"IfSoenrOilets2Noarmye"_w);
            Block->AddParam(decodeTextW_6, seriesName);
        }
        {
            const pas::WideString& boolToWideString = EC_Str::BoolToWideString(PirateBuilt);
            const pas::WideString& decodeTextW_7 = EC_Str::DecodeTextW(u"BlueivlitoBuyAPIinroaLtte"_w);
            Block->AddParam(decodeTextW_7, boolToWideString);
        }
    }

    void THull_LoadFromBlock(THull* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->HullPoints = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Hristophorisnotuse"_w))));
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->Armor = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Alrumuotr"_w))));
        Self->HullType = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"SohtiEprTtyopwec"_w))));
        Self->HullSeries = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Stearoidess"_w))));
        Self->PirateBuilt = SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"BlueivlitoBuyAPIinroaLtte"_w)))) == "true";
    }

    std::int8_t THull::CalculateGeneratedArmor() {
        return aConst::HullLevelStats[TechLevel].Armor;
    }

    // If cost generation overflows negative, repeatedly halves capacity and resets HullPoints before retrying.
    std::int32_t THull::CalculateGeneratedCost() {
        std::int32_t Value = aItem::CalculateGeneratedHullCost(Weight, TechLevel, OwnerId, HullType);
        while (Value < 0) {
            Weight = pas::abs(System::Round(pas::real_divide(Weight, 2.0L)));
            HullPoints = Weight;
            Value = aItem::CalculateGeneratedHullCost(Weight, TechLevel, OwnerId, HullType);
        }
        return Value;
    }

    void THull::Repair() {
        aItem::TEquipment::Repair();
        HullPoints = Weight;
    }

    void THull::Improve(TImprovementKind Kind) {
        std::int32_t ExtraCapacity{};
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        switch (Kind) {
            case ikMinor: Armor += aMyFunction::SeededRandomIntRange(1, 2, Id * 254571); break;
            case ikMedium: Armor += aMyFunction::SeededRandomIntRange(2, 3, Id * 254571); break;
            case ikMajor: Armor += aMyFunction::SeededRandomIntRange(3, 4, Id * 254571); break;
        }
        if (HullType == aGalaxyStruct::htTranclucator) {
            ExtraCapacity = 0;
            switch (Kind) {
                case ikMinor: aItem::CalculateHullCapacityIncrease(this, 3, 7, ExtraCapacity); break;
                case ikMedium: aItem::CalculateHullCapacityIncrease(this, 8, 12, ExtraCapacity); break;
                case ikMajor: aItem::CalculateHullCapacityIncrease(this, 13, 17, ExtraCapacity); break;
            }
            Weight += ExtraCapacity;
            HullPoints += ExtraCapacity;
        }
        Cost += CalculateImprovementCost(Kind) / 2;
    }

    std::uint8_t THull::HasStandardStats() {
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t))) == 0) {
            return CalculateGeneratedArmor() == Armor;
        }
        return CalculateGeneratedArmor() == Armor - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t)));
    }

    pas::WideString THull::GetDisplayName() {
        pas::WideString Result{};
        pas::WideString TypeName{};
        pas::WideString ShipTypeName{};
        if (NameOverride != u"") {
            Result = NameOverride;
        } else if (SpecialModuleIndex != 0) {
            Result = ([&] {
                pas::WideString microModuleTextColorTag = aItem::GetMicroModuleTextColorTag(SpecialModuleIndex - 1);
                pas::WideString specialModuleName = GetSpecialModuleName();
                return aMyFunction::WrapTextInColor(std::move(specialModuleName), std::move(microModuleTextColorTag));
            }());
            aMyFunction::ReplaceTextToken(Result, u"<Type>"_w, aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Hull.Type.", SysUtils::IntToStr(TechLevel)}))), pas::WideString());
        } else {
            TypeName = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Hull.Type.", SysUtils::IntToStr(TechLevel)})));
            switch (HullType) {
                case aGalaxyStruct::htRanger: {
                    ShipTypeName = aConst::LocalizedText(u"Items.Hull.ShipType.Ranger"_wref.get());
                    break;
                }
                case aGalaxyStruct::htTransport: {
                    ShipTypeName = aConst::LocalizedText(u"Items.Hull.ShipType.Transport"_wref.get());
                    break;
                }
                case aGalaxyStruct::htLiner: {
                    ShipTypeName = aConst::LocalizedText(u"Items.Hull.ShipType.Liner"_wref.get());
                    break;
                }
                case aGalaxyStruct::htDiplomat: {
                    ShipTypeName = aConst::LocalizedText(u"Items.Hull.ShipType.Diplomat"_wref.get());
                    break;
                }
                case aGalaxyStruct::htPirate: {
                    ShipTypeName = aConst::LocalizedText(u"Items.Hull.ShipType.Pirate"_wref.get());
                    break;
                }
                case aGalaxyStruct::htWarrior: {
                    ShipTypeName = aConst::LocalizedText(u"Items.Hull.ShipType.Warrior"_wref.get());
                    break;
                }
                case aGalaxyStruct::htKling: {
                    ShipTypeName = aConst::LocalizedText(u"Items.Hull.ShipType.Kling"_wref.get());
                    break;
                }
                default: ShipTypeName = pas::WideString(); break;
            }
            Result = aMyFunction::FormatText2(aConst::LocalizedText(u"Items.Hull.Name"_wref.get()), pas::WideString(), u"<Type>"_w, TypeName, u"<ShipType>"_w, ShipTypeName);
        }
        if (HasMicroModule()) {
            return pas::concat_wide({Result, u" ", ([&] {
                pas::WideString microModuleNameColorTag = aItem::GetMicroModuleNameColorTag(MicroModuleIndex - 1);
                pas::WideString microModuleQuotedName = GetMicroModuleQuotedName();
                return aMyFunction::WrapTextInColor(std::move(microModuleQuotedName), std::move(microModuleNameColorTag));
            }())});
        }
        return Result;
    }

    pas::WideString THull::GetSeriesName() {
        pas::WideString Result{};
        if (OwnerShip != nullptr && static_cast<aShip::TShip*>(OwnerShip)->UsesVeteranHumanRangerAppearance()) {
            pas::WideString localizedText = aConst::LocalizedText(u"HullType.HullOldfag.Name"_wref.get());
            pas::WideString localizedText_2 = aConst::LocalizedText(u"HullType.SeriesName"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedText_2), pas::WideString(), u"<Name>"_w, std::move(localizedText));
        } else if (HullSeries == -1) {
            return pas::WideString();
        } else {
            Result = aConst::LocalizedText(u"HullType.SeriesName"_wref.get());
            return aMyFunction::FormatText1(Result, pas::WideString(), u"<Name>"_w, aConst::HullSeriesDefinitions[HullSeries].Name);
        }
    }

    pas::WideString THull::GetShortName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return aConst::LocalizedText(u"Items.Hull.ShortName"_wref.get());
    }

    pas::WideString THull_GetInfoText(THull* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        pas::WideString SizeColor{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"") {
            Text = aConst::LocalizedText(u"Items.Hull.Text"_wref.get());
        }
        if (Self->OwnerShip != nullptr && static_cast<aShip::TShip*>(Self->OwnerShip)->UsesVeteranHumanRangerAppearance()) {
            Text = pas::concat_wide({aConst::LocalizedText(u"HullType.HullOldfag.Text"_wref.get()), u" ", Text});
        } else if (Self->HullSeries != -1) {
            Text = pas::concat_wide({aConst::HullSeriesDefinitions[Self->HullSeries].Text, u" ", Text});
        }
        if (Self->HullPoints <= pas::real_divide(Self->Weight, 2.0L)) {
            SizeColor = u"<color=254,217,7>"_w;
        } else {
            SizeColor = ColorTag;
        }
        aMyFunction::ReplaceTextToken(Text, u"<Size>"_w, pas::wide_int_to_str(Self->HullPoints), SizeColor);
        aMyFunction::ReplaceTextToken(Text, u"<MaxSize>"_w, pas::wide_int_to_str(Self->Weight), ColorTag);
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return Text;
    }

    void THull::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        std::int32_t BaseArmor{};
        aMyFunction::ReplaceTextToken(Text, u"<FragilityE>"_w, pas::wide_int64_to_str(System::Round(GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({{aGalaxyStruct::dkEnergy}})) * 1.0E+2L)), ColorTag);
        aMyFunction::ReplaceTextToken(Text, u"<FragilityS>"_w, pas::wide_int64_to_str(System::Round(GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({{aGalaxyStruct::dkSplinter}})) * 1.0E+2L)), ColorTag);
        aMyFunction::ReplaceTextToken(Text, u"<FragilityM>"_w, pas::wide_int64_to_str(System::Round(GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({{aGalaxyStruct::dkMissile}})) * 1.0E+2L)), ColorTag);
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t))) == 0) {
            BonusText = pas::WideString();
        } else if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t))) > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t))))})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t)))), u"<color=255,0,0>"_w);
        }
        std::int32_t StatBonus = GetStatBonus(aConst::bonHull);
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (OwnerShip != nullptr) {
            StatBonus = static_cast<aShip::TShip*>(OwnerShip)->GetTotalStatBonus(aConst::bonHull) - StatBonus;
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (MicroModuleIndex == 0) {
            BaseArmor = Armor;
        } else {
            BaseArmor = Armor - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHull * sizeof(std::int32_t)));
        }
        if (CalculateGeneratedArmor() == BaseArmor) {
            aMyFunction::ReplaceTextToken(Text, u"<HitProtect>"_w, pas::concat_wide({pas::wide_int_to_str(BaseArmor), BonusText}), ColorTag);
        } else if (CalculateGeneratedArmor() < BaseArmor) {
            aMyFunction::ReplaceTextToken(Text, u"<HitProtect>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseArmor), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<HitProtect>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseArmor), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
    }

    pas::WideString THull::GetBitmapResourceName() {
        pas::WideString Result{};
        if (ConfigBlockName != u"") {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName});
        } else if (HullType == aGalaxyStruct::htRanger) {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_", aConst::OwnerInfo[OwnerId].InternalName, u"_R_"});
        } else if (HullType == aGalaxyStruct::htWarrior) {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_", aConst::OwnerInfo[OwnerId].InternalName, u"_W_"});
        } else if (HullType == aGalaxyStruct::htPirate) {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_", aConst::OwnerInfo[OwnerId].InternalName, u"_P_"});
        } else if (HullType == aGalaxyStruct::htTransport) {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_", aConst::OwnerInfo[OwnerId].InternalName, u"_T_"});
        } else if (HullType == aGalaxyStruct::htLiner) {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_", aConst::OwnerInfo[OwnerId].InternalName, u"_L_"});
        } else if (HullType == aGalaxyStruct::htDiplomat) {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_", aConst::OwnerInfo[OwnerId].InternalName, u"_D_"});
        } else if (HullType == aGalaxyStruct::htSpecial && SpecialModuleIndex != 0) {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_", GetSpecialKindGraph(), u"_"});
        } else {
            GR_Main::RaiseWideMessage(u"THull.Image"_wref.get());
        }
        aShip::TShip* Ship = static_cast<aShip::TShip*>(OwnerShip);
        if (Ship != nullptr) {
            if (HullType == aGalaxyStruct::htSpecial && SpecialModuleIndex != 0 && GetSpecialKindGraph() == u"J" && Ship->IsFemaleHumanPilot() && Ship->TypeId == aGalaxyStruct::stRanger) {
                return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_J_alt_"});
            } else if (HullType == aGalaxyStruct::htRanger && SpecialModuleIndex == 0 && Ship->UsesVeteranHumanRangerAppearance()) {
                return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_People_ROld_"});
            } else if (Ship->TypeId == aGalaxyStruct::stPirate && Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::checked_cast<aPirate::TPirate*>(Ship)->PirateType != 0) {
                return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Hull_", aConst::OwnerInfo[OwnerId].InternalName, u"_PC_"});
            } else {
                return Result;
            }
        }
        return Result;
    }

    // Returns the special module KindGraph, or the literal 1 as fallback.
    pas::WideString THull::GetSpecialKindGraph() {
        pas::WideString Result{};
        Result = u"1"_w;
        if (SpecialModuleIndex == 0) {
            return Result;
        }
        if (aConst::MicroModuleTemplates[SpecialModuleIndex - 1].KindGraph != u"") {
            return aConst::MicroModuleTemplates[SpecialModuleIndex - 1].KindGraph;
        }
        return Result;
    }

    std::int32_t THull::GetSlotCount(aConst::TShipSlotKind Kind) {
        std::int32_t I{};
        PExtraSpecial Entry{};
        std::int32_t Result = aItem::GetBaseHullSlotCount(Kind, HullType, OwnerId, OwnerShip);
        aConst::TEquipmentBonusKind BonusKind = aConst::HullSlotBonusKinds[Kind];
        std::int32_t Maximum = aConst::DefaultHullSlotCounts[Kind];
        std::int32_t Minimum = aConst::MinimumHullSlotCounts[Kind];
        if (SpecialModuleIndex != 0) {
            Result = std::min<std::int32_t>(Maximum, std::max<std::int32_t>(Minimum, Result + pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)))));
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, BonusKind * sizeof(std::int32_t))) * Entry->Count;
            }
            Result = std::min<std::int32_t>(Maximum, std::max<std::int32_t>(Minimum, Result));
        }
        if (HullSeries != -1) {
            Result = std::min<std::int32_t>(Maximum, std::max<std::int32_t>(Minimum, Result + pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::HullSeriesDefinitions[HullSeries].SlotBonuses, Kind * sizeof(std::int32_t)))));
        }
        if (MicroModuleIndex != 0) {
            return std::min<std::int32_t>(Maximum, std::max<std::int32_t>(Minimum, Result + pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)))));
        }
        return Result;
    }

    // Round(capacity * (0.6 + 0.2 * (clamped level - 1) / 7)); excludes carried cargo and ship mass modifiers.
    std::int32_t THull::CalculateMass() {
        return System::Round(aMyFunction::RemapClamped(TechLevel, 1.0, 8.0, Weight * 0.6L, Weight * 0.8L));
    }

    // Zero flags return the average of energy, splinter and missile factors.
    float THull::GetFragilityFactor(aGalaxyStruct::TDamageFlagSet DamageFlags) {
        float Result{};
        aConst::TWeaponDamageClass DamageClass{};
        PExtraSpecial Entry{};
        std::int32_t I{};
        float Factor{};
        if (DamageFlags == pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})) {
            Result = 0.0f;
            for (auto cpp_range = pas::for_to<aConst::TWeaponDamageClass>(aConst::wdcEnergy, aConst::wdcMissile); cpp_range.next(DamageClass); ) {
                Result = static_cast<long double>(GetFragilityFactor(pas::make_set<aGalaxyStruct::TDamageFlagSet>({{static_cast<std::int32_t>(static_cast<aGalaxyStruct::TDamageKind>(aConst::WeaponDamageClasses[DamageClass].Kind))}}))) + Result;
            }
            return pas::real_divide(Result, 3.0L);
        }
        DamageClass = aConst::ClassifyWeaponDamageFlags(pas::load_unaligned<std::uint32_t>(&DamageFlags));
        Result = static_cast<long double>(aConst::HullFragilityByType[HullType]) * pas::load_unaligned<float>(pas::byte_offset(&aConst::HullLevelStats[TechLevel].Fragility, DamageClass * sizeof(float))) * aConst::HullFragilityByOwner[DamageClass][OwnerId];
        if (PirateBuilt) {
            Result = static_cast<long double>(Result) * aConst::HullFragilityByOwner[DamageClass][7];
        }
        if (MicroModuleIndex != 0) {
            Result = static_cast<long double>(Result) * pas::load_unaligned<float>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].FragilityFactorByDamageClass, DamageClass * sizeof(float)));
        }
        if (SpecialModuleIndex != 0) {
            Result = static_cast<long double>(Result) * pas::load_unaligned<float>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].FragilityFactorByDamageClass, DamageClass * sizeof(float)));
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range_2.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                Factor = pas::load_unaligned<float>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].FragilityFactorByDamageClass, DamageClass * sizeof(float)));
                if (std::fabs(Factor - 1.0L) > 1.0E-6L) {
                    if (Entry->Count == 1) {
                        Result = static_cast<long double>(Result) * Factor;
                    } else {
                        Result = Math::Power(Factor, Entry->Count) * Result;
                    }
                }
            }
        }
        if (HullSeries != -1) {
            return static_cast<long double>(Result) * pas::load_unaligned<float>(pas::byte_offset(&aConst::HullSeriesDefinitions[HullSeries].FragilityByDamageClass, DamageClass * sizeof(float)));
        }
        return Result;
    }

    // Reverses module and series size percentages with rounding; extra-special multiplicities are not used.
    std::int32_t THull::EstimateCapacityWithoutBonuses() {
        std::int32_t Result{};
        std::int32_t I{};
        // Updates the parent's capacity accumulator; caller removes ParentFrame. Nonpositive Percent leaves it unchanged.
        auto UndoSizePercent = [&](std::int32_t Percent) -> void {
            float Factor{};
            if (Percent <= 0) {
                Factor = 1.0f;
            } else {
                Factor = pas::real_divide(1.0E+2L, Percent);
            }
            Result = System::Round(static_cast<long double>(Result) * Factor);
        };
        Result = Weight;
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                UndoSizePercent(aConst::MicroModuleTemplates[pas::list_at<TExtraSpecial>(ExtraSpecials, I)->ModuleIndexPlusOne - 1].SizePercent);
            }
        }
        if (MicroModuleIndex != 0) {
            UndoSizePercent(aConst::MicroModuleTemplates[MicroModuleIndex - 1].SizePercent);
        }
        if (SpecialModuleIndex != 0) {
            UndoSizePercent(aConst::MicroModuleTemplates[SpecialModuleIndex - 1].SizePercent);
        }
        if (HullSeries != -1) {
            UndoSizePercent(aConst::HullSeriesDefinitions[HullSeries].SizePercent);
        }
        return Result;
    }

    void TFuelTanks::Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        ItemType = aConst::t_FuelTanks;
        this->Weight = Weight;
        TechLevel = Level;
        Capacity = CalculateGeneratedCapacity();
        Fuel = Capacity;
        OwnerId = Owner;
        Repair();
        Cost = CalculateGeneratedCost();
        MicroModuleIndex = 0;
    }

    void TFuelTanks::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddWideChar(Fuel);
        Buffer->AddAnsiChar(Capacity);
    }

    void TFuelTanks::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        Fuel = EC_Buf::TBufEC_GetWord(Buffer);
        Capacity = EC_Buf::TBufEC_GetByte(Buffer);
    }

    void TFuelTanks::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Fuel);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"FiuNeol"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(Capacity));
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"CraspiaNcliotay"_w);
            Block->AddParam(decodeTextW_3, intToStr_3);
        }
    }

    void TFuelTanks_LoadFromBlock(TFuelTanks* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->Fuel = static_cast<std::uint16_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"FiuNeol"_w)))));
        Self->Capacity = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"CraspiaNcliotay"_w))));
    }

    std::uint8_t TFuelTanks::CalculateGeneratedCapacity() {
        return aItem::CalculateGeneratedFuelCapacity(Weight, TechLevel);
    }

    std::int32_t TFuelTanks::CalculateGeneratedCost() {
        return aItem::CalculateGeneratedFuelTanksCost(Weight, TechLevel, OwnerId);
    }

    void TFuelTanks::Improve(TImprovementKind Kind) {
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        switch (Kind) {
            case ikMinor: {
                pas::Extended cpp_right = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.05, 0.1);
                std::int64_t cpp_left = System::Round(Capacity * cpp_right);
                Capacity = cpp_left + Capacity + 1;
                break;
            }
            case ikMedium: {
                pas::Extended cpp_right_2 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.1, 0.15);
                std::int64_t cpp_left_2 = System::Round(Capacity * cpp_right_2);
                Capacity = cpp_left_2 + Capacity + 3;
                break;
            }
            case ikMajor: {
                pas::Extended cpp_right_3 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.15, 0.2);
                std::int64_t cpp_left_3 = System::Round(Capacity * cpp_right_3);
                Capacity = cpp_left_3 + Capacity + 5;
                break;
            }
        }
        Cost += CalculateImprovementCost(Kind) / 2;
    }

    std::uint8_t TFuelTanks::HasStandardStats() {
        std::int32_t BaseCapacity{};
        std::int32_t SizePercent{};
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t))) == 0) {
            BaseCapacity = Capacity;
        } else {
            BaseCapacity = Capacity - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t)));
        }
        std::int32_t ExpectedWeight = System::Round(pas::real_divide((BaseCapacity - aConst::FuelCapacityByLevel[TechLevel]) * aConst::FuelTanksBaseSize, 2.0E+1L));
        if (MicroModuleIndex != 0) {
            SizePercent = aConst::MicroModuleTemplates[MicroModuleIndex - 1].SizePercent;
        } else {
            SizePercent = 0;
        }
        if (SizePercent > 0) {
            ExpectedWeight = System::Round(pas::real_divide(ExpectedWeight * SizePercent, 1.0E+2L));
        }
        if (SpecialModuleIndex != 0) {
            SizePercent = aConst::MicroModuleTemplates[SpecialModuleIndex - 1].SizePercent;
        } else {
            SizePercent = 0;
        }
        if (SizePercent > 0) {
            ExpectedWeight = System::Round(pas::real_divide(ExpectedWeight * SizePercent, 1.0E+2L));
        }
        return pas::abs(ExpectedWeight - Weight) <= 1;
    }

    pas::WideString TFuelTanks_GetInfoText(TFuelTanks* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"" && Self->CustomFaction != u"") {
            Text = aConst::LocalizedText(pas::concat_wide({u"Items.FuelTanks.", Self->CustomFaction, u"Text"}));
        }
        if (Text == u"") {
            if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                Text = aConst::LocalizedText(u"Items.FuelTanks.Text"_wref.get());
            } else {
                Text = aConst::LocalizedText(u"Items.FuelTanks.KlingText"_wref.get());
            }
        }
        aMyFunction::ReplaceTextToken(Text, u"<Fuel>"_w, pas::wide_int_to_str(Self->Fuel), ColorTag);
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Text});
    }

    void TFuelTanks::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        std::int32_t BaseValue{};
        std::int32_t SizePercent{};
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t))) == 0) {
            BonusText = pas::WideString();
        } else if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t))) > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t))))})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t)))), u"<color=255,0,0>"_w);
        }
        if (MicroModuleIndex == 0) {
            BaseValue = Capacity;
        } else {
            BaseValue = Capacity - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonFuel * sizeof(std::int32_t)));
        }
        std::int32_t ExpectedWeight = System::Round(pas::real_divide((BaseValue - aConst::FuelCapacityByLevel[TechLevel]) * aConst::FuelTanksBaseSize, 2.0E+1L));
        if (MicroModuleIndex != 0) {
            SizePercent = aConst::MicroModuleTemplates[MicroModuleIndex - 1].SizePercent;
        } else {
            SizePercent = 0;
        }
        if (SizePercent > 0) {
            ExpectedWeight = System::Round(pas::real_divide(ExpectedWeight * SizePercent, 1.0E+2L));
        }
        if (SpecialModuleIndex != 0) {
            SizePercent = aConst::MicroModuleTemplates[SpecialModuleIndex - 1].SizePercent;
        } else {
            SizePercent = 0;
        }
        if (SizePercent > 0) {
            ExpectedWeight = System::Round(pas::real_divide(ExpectedWeight * SizePercent, 1.0E+2L));
        }
        if (pas::abs(ExpectedWeight - Weight) <= 1) {
            aMyFunction::ReplaceTextToken(Text, u"<Capacity>"_w, pas::concat_wide({pas::wide_int_to_str(BaseValue), BonusText}), ColorTag);
        } else if (ExpectedWeight > Weight) {
            aMyFunction::ReplaceTextToken(Text, u"<Capacity>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Capacity>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
    }

    void TEngine::Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        ItemType = aConst::t_Engine;
        MicroModuleIndex = 0;
        this->Weight = Weight;
        TechLevel = Level;
        Speed = CalculateGeneratedSpeed();
        JumpRange = CalculateGeneratedJumpRange();
        OutputPercent = 100;
        OwnerId = Owner;
        Repair();
        Cost = CalculateGeneratedCost();
    }

    void TEngine::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddIntegerValue(Speed);
        Buffer->AddAnsiChar(JumpRange);
        Buffer->AddAnsiChar(OutputPercent);
    }

    void TEngine::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 161) {
            Speed = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            Speed = EC_Buf::TBufEC_GetWord(Buffer);
        }
        JumpRange = EC_Buf::TBufEC_GetByte(Buffer);
        OutputPercent = EC_Buf::TBufEC_GetByte(Buffer);
        ItemType = aConst::t_Engine;
    }

    void TEngine::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Speed);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Sapreneld"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(JumpRange));
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"JiuOmipa"_w);
            Block->AddParam(decodeTextW_3, intToStr_3);
        }
    }

    void TEngine_LoadFromBlock(TEngine* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->Speed = static_cast<std::uint16_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Sapreneld"_w)))));
        Self->JumpRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"JiuOmipa"_w))));
    }

    std::int32_t TEngine::CalculateGeneratedSpeed() {
        return aConst::EngineLevelStats[TechLevel].Speed;
    }

    std::int8_t TEngine::CalculateGeneratedJumpRange() {
        return aConst::EngineLevelStats[TechLevel].JumpRange;
    }

    std::int32_t TEngine::CalculateGeneratedCost() {
        return aItem::CalculateGeneratedEngineCost(Weight, TechLevel, OwnerId);
    }

    void TEngine::Improve(TImprovementKind Kind) {
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        if (DetailImprovement == 0) {
            if (aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->CurrentTurn * (Kind + 1) * Id) < 0.5L) {
                DetailImprovement = 1;
            } else {
                DetailImprovement = 2;
            }
        }
        if (DetailImprovement == 1) {
            switch (Kind) {
                case ikMinor: {
                    Speed += ([&] {
                        pas::Extended cpp_right = aMyFunction::SeededRandomIntRange(10, 30, Id * 254571);
                        pas::Extended cpp_right_2 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.01, 0.05);
                        return aMyFunction::RoundAndTruncateToTens(Speed * cpp_right_2 + cpp_right);
                    }());
                    JumpRange += aMyFunction::SeededRandomIntRange(1, 2, Id * 354571);
                    break;
                }
                case ikMedium: {
                    Speed += ([&] {
                        pas::Extended cpp_right_3 = aMyFunction::SeededRandomIntRange(30, 60, Id * 254571);
                        pas::Extended cpp_right_4 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.02, 0.06);
                        return aMyFunction::RoundAndTruncateToTens(Speed * cpp_right_4 + cpp_right_3);
                    }());
                    JumpRange += aMyFunction::SeededRandomIntRange(2, 4, Id * 354571);
                    break;
                }
                case ikMajor: {
                    Speed += ([&] {
                        pas::Extended cpp_right_5 = aMyFunction::SeededRandomIntRange(50, 80, Id * 254571);
                        pas::Extended cpp_right_6 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.03, 0.07);
                        return aMyFunction::RoundAndTruncateToTens(Speed * cpp_right_6 + cpp_right_5);
                    }());
                    JumpRange += aMyFunction::SeededRandomIntRange(3, 7, Id * 354571);
                    break;
                }
            }
        } else if (DetailImprovement == 2) {
            switch (Kind) {
                case ikMinor: {
                    JumpRange += aMyFunction::SeededRandomIntRange(2, 5, Id * 354571);
                    Speed += ([&] {
                        pas::Extended cpp_right_7 = aMyFunction::SeededRandomIntRange(10, 20, Id * 254571);
                        pas::Extended cpp_right_8 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.01, 0.03);
                        return aMyFunction::RoundAndTruncateToTens(Speed * cpp_right_8 + cpp_right_7);
                    }());
                    break;
                }
                case ikMedium: {
                    JumpRange += aMyFunction::SeededRandomIntRange(5, 7, Id * 354571);
                    Speed += ([&] {
                        pas::Extended cpp_right_9 = aMyFunction::SeededRandomIntRange(20, 40, Id * 254571);
                        pas::Extended cpp_right_10 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.01, 0.04);
                        return aMyFunction::RoundAndTruncateToTens(Speed * cpp_right_10 + cpp_right_9);
                    }());
                    break;
                }
                case ikMajor: {
                    JumpRange += aMyFunction::SeededRandomIntRange(7, 12, Id * 354571);
                    Speed += ([&] {
                        pas::Extended cpp_right_11 = aMyFunction::SeededRandomIntRange(30, 50, Id * 254571);
                        pas::Extended cpp_right_12 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.01, 0.05);
                        return aMyFunction::RoundAndTruncateToTens(Speed * cpp_right_12 + cpp_right_11);
                    }());
                    break;
                }
            }
        }
        Cost += CalculateImprovementCost(Kind) / 2;
        DetailImprovement = 0;
    }

    std::uint8_t TEngine::HasStandardStats() {
        std::uint8_t StandardSpeed{};
        std::uint8_t StandardJump{};
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonSpeed * sizeof(std::int32_t))) == 0) {
            StandardSpeed = CalculateGeneratedSpeed() == Speed;
        } else {
            StandardSpeed = CalculateGeneratedSpeed() == Speed - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonSpeed * sizeof(std::int32_t)));
        }
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonJump * sizeof(std::int32_t))) == 0) {
            StandardJump = CalculateGeneratedJumpRange() == JumpRange;
        } else {
            StandardJump = CalculateGeneratedJumpRange() == JumpRange - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonJump * sizeof(std::int32_t)));
        }
        return StandardSpeed && StandardJump;
    }

    pas::WideString TEngine_GetInfoText(TEngine* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"" && Self->CustomFaction != u"") {
            Text = aConst::LocalizedText(pas::concat_wide({u"Items.Engine.", Self->CustomFaction, u"Text"}));
        }
        if (Text == u"") {
            if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                Text = aConst::LocalizedText(u"Items.Engine.Text"_wref.get());
            } else {
                Text = aConst::LocalizedText(u"Items.Engine.KlingText"_wref.get());
            }
        }
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Text});
    }

    void TEngine::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        std::int32_t StatBonus{};
        if (MicroModuleIndex == 0) {
            StatBonus = 0;
        } else {
            StatBonus = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonSpeed * sizeof(std::int32_t)));
        }
        std::int32_t BaseValue = std::max<std::int32_t>(0, Speed - StatBonus);
        StatBonus = std::max<std::int32_t>(0, Speed) - BaseValue;
        if (StatBonus == 0) {
            BonusText = pas::WideString();
        } else if (StatBonus > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,0,0>"_w);
        }
        StatBonus = std::max<std::int32_t>(-std::max<std::int32_t>(0, Speed), GetStatBonus(aConst::bonSpeed));
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            StatBonus = std::max<std::int32_t>(static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonSpeed) - StatBonus, -(Speed + StatBonus));
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (CalculateGeneratedSpeed() == BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Speed>"_w, pas::concat_wide({pas::wide_int_to_str(BaseValue), BonusText}), ColorTag);
        } else if (CalculateGeneratedSpeed() < BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Speed>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Speed>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
        if (MicroModuleIndex == 0) {
            StatBonus = 0;
        } else {
            StatBonus = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonJump * sizeof(std::int32_t)));
        }
        BaseValue = std::max<std::int32_t>(0, JumpRange - StatBonus);
        StatBonus = std::max<std::int32_t>(0, static_cast<std::int32_t>(JumpRange)) - BaseValue;
        if (StatBonus == 0) {
            BonusText = pas::WideString();
        } else if (StatBonus > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,0,0>"_w);
        }
        StatBonus = std::max<std::int32_t>(-std::max<std::int32_t>(0, static_cast<std::int32_t>(JumpRange)), GetStatBonus(aConst::bonJump));
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            StatBonus = std::max<std::int32_t>(static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonJump) - StatBonus, -(JumpRange + StatBonus));
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (CalculateGeneratedJumpRange() == BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Parsec>"_w, pas::concat_wide({pas::wide_int_to_str(BaseValue), BonusText}), ColorTag);
        } else if (CalculateGeneratedJumpRange() < BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Parsec>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Parsec>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
    }

    void TRadar::Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        ItemType = aConst::t_Radar;
        this->Weight = Weight;
        TechLevel = Level;
        Range = CalculateGeneratedRange();
        OwnerId = Owner;
        Repair();
        Cost = CalculateGeneratedCost();
        MicroModuleIndex = 0;
    }

    void TRadar::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddWideChar(Range);
    }

    void TRadar::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        Range = EC_Buf::TBufEC_GetWord(Buffer);
    }

    void TRadar::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Range);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Rialdoinurs"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
    }

    void TRadar_LoadFromBlock(TRadar* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->Range = static_cast<std::uint16_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Rialdoinurs"_w)))));
    }

    std::int32_t TRadar::CalculateGeneratedRange() {
        return aConst::RadarLevelRanges[TechLevel];
    }

    std::int32_t TRadar::CalculateGeneratedCost() {
        return aItem::CalculateGeneratedRadarCost(Weight, TechLevel, OwnerId);
    }

    void TRadar::Improve(TImprovementKind Kind) {
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        switch (Kind) {
            case ikMinor: {
                Range += ([&] {
                    pas::Extended cpp_right = aMyFunction::SeededRandomIntRange(100, 200, Id * 254571);
                    pas::Extended cpp_right_2 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.05, 0.1);
                    return aMyFunction::RoundAndTruncateToTens(Range * cpp_right_2 + cpp_right);
                }());
                break;
            }
            case ikMedium: {
                Range += ([&] {
                    pas::Extended cpp_right_3 = aMyFunction::SeededRandomIntRange(300, 400, Id * 254571);
                    pas::Extended cpp_right_4 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.05, 0.1);
                    return aMyFunction::RoundAndTruncateToTens(Range * cpp_right_4 + cpp_right_3);
                }());
                break;
            }
            case ikMajor: {
                Range += ([&] {
                    pas::Extended cpp_right_5 = aMyFunction::SeededRandomIntRange(400, 500, Id * 254571);
                    pas::Extended cpp_right_6 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.05, 0.1);
                    return aMyFunction::RoundAndTruncateToTens(Range * cpp_right_6 + cpp_right_5);
                }());
                break;
            }
        }
        Cost += CalculateImprovementCost(Kind) / 2;
    }

    std::uint8_t TRadar::HasStandardStats() {
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t))) == 0) {
            return CalculateGeneratedRange() == Range;
        }
        return CalculateGeneratedRange() == Range - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t)));
    }

    pas::WideString TRadar_GetInfoText(TRadar* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"" && Self->CustomFaction != u"") {
            Text = aConst::LocalizedText(pas::concat_wide({u"Items.Radar.", Self->CustomFaction, u"Text"}));
        }
        if (Text == u"") {
            if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                Text = aConst::LocalizedText(u"Items.Radar.Text"_wref.get());
            } else {
                Text = aConst::LocalizedText(u"Items.Radar.KlingText"_wref.get());
            }
        }
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Text});
    }

    void TRadar::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        std::int32_t BaseValue{};
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t))) == 0) {
            BonusText = pas::WideString();
        } else if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t))) > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t))))})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t)))), u"<color=255,0,0>"_w);
        }
        std::int32_t StatBonus = std::max<std::int32_t>(-std::max<std::int32_t>(0, Range), GetStatBonus(aConst::bonRadar));
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            StatBonus = std::max<std::int32_t>(static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonRadar) - StatBonus, -(Range + StatBonus));
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (MicroModuleIndex == 0) {
            BaseValue = Range;
        } else {
            BaseValue = Range - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonRadar * sizeof(std::int32_t)));
        }
        if (HasStandardStats()) {
            aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({pas::wide_int_to_str(BaseValue), BonusText}), ColorTag);
        } else if (CalculateGeneratedRange() < BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
    }

    void TScaner::Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        ItemType = aConst::t_Scaner;
        this->Weight = Weight;
        TechLevel = Level;
        ScanPower = CalculateGeneratedScanPower();
        OwnerId = Owner;
        Repair();
        Cost = CalculateGeneratedCost();
        MicroModuleIndex = 0;
    }

    void TScaner::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddAnsiChar(ScanPower);
    }

    void TScaner::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        ScanPower = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion < 118 && MicroModuleIndex != 0 && aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber == 122) {
            ++ScanPower;
        }
    }

    void TScaner::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(ScanPower));
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Prouwseor"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
    }

    void TScaner_LoadFromBlock(TScaner* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->ScanPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Prouwseor"_w))));
    }

    std::int32_t TScaner::CalculateGeneratedScanPower() {
        return (aItem::DefenseDamageFactorToPercent(aItem::GetGeneratedDefenseDamageFactor(TechLevel)) & 0x0000007f) + 1;
    }

    std::int32_t TScaner::CalculateGeneratedCost() {
        return aItem::CalculateGeneratedScanerCost(Weight, TechLevel, OwnerId);
    }

    void TScaner::Improve(TImprovementKind Kind) {
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        switch (Kind) {
            case ikMinor: {
                std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(1, 3, Id * 354571);
                pas::Extended cpp_right_3 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.05, 0.1);
                std::int64_t cpp_right_2 = System::Round(ScanPower * cpp_right_3);
                ScanPower = ScanPower + cpp_right_2 + cpp_right;
                break;
            }
            case ikMedium: {
                std::int32_t cpp_right_4 = aMyFunction::SeededRandomIntRange(3, 5, Id * 354571);
                pas::Extended cpp_right_6 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.05, 0.1);
                std::int64_t cpp_right_5 = System::Round(ScanPower * cpp_right_6);
                ScanPower = ScanPower + cpp_right_5 + cpp_right_4;
                break;
            }
            case ikMajor: {
                std::int32_t cpp_right_7 = aMyFunction::SeededRandomIntRange(5, 7, Id * 354571);
                pas::Extended cpp_right_9 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.05, 0.1);
                std::int64_t cpp_right_8 = System::Round(ScanPower * cpp_right_9);
                ScanPower = ScanPower + cpp_right_8 + cpp_right_7;
                break;
            }
        }
        Cost += CalculateImprovementCost(Kind) / 2;
    }

    std::uint8_t TScaner::HasStandardStats() {
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonScan * sizeof(std::int32_t))) == 0) {
            return CalculateGeneratedScanPower() == ScanPower;
        }
        return CalculateGeneratedScanPower() == ScanPower - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonScan * sizeof(std::int32_t)));
    }

    pas::WideString TScaner_GetInfoText(TScaner* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"" && Self->CustomFaction != u"") {
            Text = aConst::LocalizedText(pas::concat_wide({u"Items.Scaner.", Self->CustomFaction, u"Text"}));
        }
        if (Text == u"") {
            if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                Text = aConst::LocalizedText(u"Items.Scaner.Text"_wref.get());
            } else {
                Text = aConst::LocalizedText(u"Items.Scaner.KlingText"_wref.get());
            }
        }
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Text});
    }

    void TScaner::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        std::int32_t StatBonus{};
        if (MicroModuleIndex == 0) {
            StatBonus = 0;
        } else {
            StatBonus = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonScan * sizeof(std::int32_t)));
        }
        std::int32_t BaseValue = std::max<std::int32_t>(0, ScanPower - StatBonus);
        StatBonus = std::max<std::int32_t>(0, static_cast<std::int32_t>(ScanPower)) - BaseValue;
        if (StatBonus == 0) {
            BonusText = pas::WideString();
        } else if (StatBonus > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,0,0>"_w);
        }
        StatBonus = std::max<std::int32_t>(-std::max<std::int32_t>(0, static_cast<std::int32_t>(ScanPower)), GetStatBonus(aConst::bonScan));
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            StatBonus = std::max<std::int32_t>(static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonScan) - StatBonus, -(ScanPower + StatBonus));
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (CalculateGeneratedScanPower() == BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, pas::concat_wide({pas::wide_int_to_str(BaseValue), BonusText}), ColorTag);
        } else if (CalculateGeneratedScanPower() < BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
    }

    void TRepairRobot::Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        ItemType = aConst::t_RepairRobot;
        this->Weight = Weight;
        TechLevel = Level;
        RepairPoints = CalculateGeneratedRepairPoints();
        OwnerId = Owner;
        Repair();
        Cost = CalculateGeneratedCost();
        MicroModuleIndex = 0;
    }

    void TRepairRobot::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddAnsiChar(RepairPoints);
    }

    void TRepairRobot::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        RepairPoints = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion < 118) {
            if (TechLevel < 5) {
                RepairPoints += 5;
            }
            if (MicroModuleIndex != 0 && pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t))) != 0) {
                switch (aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber) {
                    case 122: RepairPoints += 7; break;
                    case 21: RepairPoints += 4; break;
                    case 7: RepairPoints += 3; break;
                    case 204: RepairPoints -= 5; break;
                    case 14: RepairPoints -= 5; break;
                    case 110: RepairPoints -= 5; break;
                    case 16: RepairPoints -= 3; break;
                }
            }
        }
    }

    void TRepairRobot::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(RepairPoints));
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Raenplavikr"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
    }

    void TRepairRobot_LoadFromBlock(TRepairRobot* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->RepairPoints = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Raenplavikr"_w))));
    }

    std::uint8_t TRepairRobot::CalculateGeneratedRepairPoints() {
        return aConst::RepairRobotLevelPoints[TechLevel];
    }

    std::int32_t TRepairRobot::CalculateGeneratedCost() {
        return aItem::CalculateGeneratedRepairRobotCost(Weight, TechLevel, OwnerId);
    }

    void TRepairRobot::Improve(TImprovementKind Kind) {
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        switch (Kind) {
            case ikMinor: {
                std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(1, 4, Id * 354571);
                pas::Extended cpp_right_3 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.07, 0.12);
                std::int64_t cpp_right_2 = System::Round(RepairPoints * cpp_right_3);
                RepairPoints = RepairPoints + cpp_right_2 + cpp_right;
                break;
            }
            case ikMedium: {
                std::int32_t cpp_right_4 = aMyFunction::SeededRandomIntRange(4, 7, Id * 354571);
                pas::Extended cpp_right_6 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.07, 0.12);
                std::int64_t cpp_right_5 = System::Round(RepairPoints * cpp_right_6);
                RepairPoints = RepairPoints + cpp_right_5 + cpp_right_4;
                break;
            }
            case ikMajor: {
                std::int32_t cpp_right_7 = aMyFunction::SeededRandomIntRange(7, 10, Id * 354571);
                pas::Extended cpp_right_9 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.07, 0.12);
                std::int64_t cpp_right_8 = System::Round(RepairPoints * cpp_right_9);
                RepairPoints = RepairPoints + cpp_right_8 + cpp_right_7;
                break;
            }
        }
        Cost += CalculateImprovementCost(Kind) / 2;
    }

    std::uint8_t TRepairRobot::HasStandardStats() {
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t))) == 0) {
            return CalculateGeneratedRepairPoints() == RepairPoints;
        }
        return CalculateGeneratedRepairPoints() == RepairPoints - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t)));
    }

    pas::WideString TRepairRobot_GetInfoText(TRepairRobot* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"" && Self->CustomFaction != u"") {
            Text = aConst::LocalizedText(pas::concat_wide({u"Items.RepairRobot.", Self->CustomFaction, u"Text"}));
        }
        if (Text == u"") {
            if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                Text = aConst::LocalizedText(u"Items.RepairRobot.Text"_wref.get());
            } else {
                Text = aConst::LocalizedText(u"Items.RepairRobot.KlingText"_wref.get());
            }
        }
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Text});
    }

    void TRepairRobot::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        std::int32_t BaseValue{};
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t))) == 0) {
            BonusText = pas::WideString();
        } else if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t))) > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t))))})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t)))), u"<color=255,0,0>"_w);
        }
        std::int32_t StatBonus = std::max<std::int32_t>(-std::max<std::int32_t>(0, static_cast<std::int32_t>(RepairPoints)), GetStatBonus(aConst::bonDroid));
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            StatBonus = std::max<std::int32_t>(static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonDroid) - StatBonus, -(RepairPoints + StatBonus));
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (MicroModuleIndex == 0) {
            BaseValue = RepairPoints;
        } else {
            BaseValue = RepairPoints - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDroid * sizeof(std::int32_t)));
        }
        if (HasStandardStats()) {
            aMyFunction::ReplaceTextToken(Text, u"<RecoverHitPoints>"_w, pas::concat_wide({pas::wide_int_to_str(BaseValue), BonusText}), ColorTag);
        } else if (CalculateGeneratedRepairPoints() < BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<RecoverHitPoints>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<RecoverHitPoints>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
    }

    void TCargoHook_Create(TCargoHook* Self) {
        aItem::TEquipment_Create(Self);
    }

    void TCargoHook::Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        ItemType = aConst::t_CargoHook;
        this->Weight = Weight;
        TechLevel = Level;
        PickupPower = CalculateGeneratedPickupPower();
        Range = CalculateGeneratedRange();
        MinPullSpeed = CalculateGeneratedMinPullSpeed();
        MaxPullSpeed = CalculateGeneratedMaxPullSpeed();
        OwnerId = Owner;
        Repair();
        Cost = CalculateGeneratedCost();
        MicroModuleIndex = 0;
    }

    void TCargoHook::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddWideChar(PickupPower);
        Buffer->AddWideChar(Range);
        Buffer->AddSingle(MinPullSpeed);
        Buffer->AddSingle(MaxPullSpeed);
    }

    void TCargoHook::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        PickupPower = EC_Buf::TBufEC_GetWord(Buffer);
        Range = EC_Buf::TBufEC_GetWord(Buffer);
        MinPullSpeed = EC_Buf::TBufEC_GetSingle(Buffer);
        MaxPullSpeed = EC_Buf::TBufEC_GetSingle(Buffer);
    }

    void TCargoHook::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(PickupPower);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Prouwseor"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(Range);
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"Rialdoinurs"_w);
            Block->AddParam(decodeTextW_3, intToStr_3);
        }
        {
            const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(MinPullSpeed), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"SapperenduMaidno"_w);
            Block->AddParam(decodeTextW_4, cpp_arg);
        }
        {
            const pas::WideString& cpp_arg_2 = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(MaxPullSpeed), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"Suplexe2d3Moarxi"_w);
            Block->AddParam(decodeTextW_5, cpp_arg_2);
        }
    }

    void TCargoHook_LoadFromBlock(TCargoHook* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->PickupPower = static_cast<std::uint16_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Prouwseor"_w)))));
        Self->Range = static_cast<std::uint16_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Rialdoinurs"_w)))));
        Self->MinPullSpeed = EC_Str::ExtractDecimalToSingleW(Block->GetParam(EC_Str::DecodeTextW(u"SapperenduMaidno"_w)));
        Self->MaxPullSpeed = EC_Str::ExtractDecimalToSingleW(Block->GetParam(EC_Str::DecodeTextW(u"Suplexe2d3Moarxi"_w)));
    }

    std::int32_t TCargoHook::CalculateGeneratedPickupPower() {
        return aConst::CargoHookLevelStats[TechLevel].PickupPower;
    }

    std::int32_t TCargoHook::CalculateGeneratedRange() {
        return aConst::CargoHookLevelStats[TechLevel].Range;
    }

    float TCargoHook::CalculateGeneratedMinPullSpeed() {
        return aConst::CargoHookLevelStats[TechLevel].MinPullSpeed;
    }

    float TCargoHook::CalculateGeneratedMaxPullSpeed() {
        return aConst::CargoHookLevelStats[TechLevel].MaxPullSpeed;
    }

    std::int32_t TCargoHook::CalculateGeneratedCost() {
        return aItem::CalculateGeneratedCargoHookCost(Weight, TechLevel, OwnerId);
    }

    void TCargoHook::Improve(TImprovementKind Kind) {
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        if (DetailImprovement == 0) {
            if (aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->CurrentTurn * (Kind + 1) * Id) < 0.5L) {
                DetailImprovement = 1;
            } else {
                DetailImprovement = 2;
            }
        }
        if (DetailImprovement == 1) {
            switch (Kind) {
                case ikMinor: {
                    {
                        std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(5, 10, Id * 254571);
                        pas::Extended cpp_right_3 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.07, 0.12);
                        std::int64_t cpp_right_2 = System::Round(PickupPower * cpp_right_3);
                        PickupPower = PickupPower + cpp_right_2 + cpp_right;
                    }
                    Range += aMyFunction::SeededRandomIntRange(2, 4, Id * 354571);
                    break;
                }
                case ikMedium: {
                    {
                        std::int32_t cpp_right_4 = aMyFunction::SeededRandomIntRange(10, 15, Id * 254571);
                        pas::Extended cpp_right_6 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.07, 0.12);
                        std::int64_t cpp_right_5 = System::Round(PickupPower * cpp_right_6);
                        PickupPower = PickupPower + cpp_right_5 + cpp_right_4;
                    }
                    Range += aMyFunction::SeededRandomIntRange(5, 7, Id * 354571);
                    break;
                }
                case ikMajor: {
                    {
                        std::int32_t cpp_right_7 = aMyFunction::SeededRandomIntRange(15, 20, Id * 254571);
                        pas::Extended cpp_right_9 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.07, 0.12);
                        std::int64_t cpp_right_8 = System::Round(PickupPower * cpp_right_9);
                        PickupPower = PickupPower + cpp_right_8 + cpp_right_7;
                    }
                    Range += aMyFunction::SeededRandomIntRange(8, 10, Id * 354571);
                    break;
                }
            }
        } else if (DetailImprovement == 2) {
            switch (Kind) {
                case ikMinor: {
                    Range += aMyFunction::SeededRandomIntRange(10, 15, Id * 354571);
                    {
                        std::int32_t cpp_right_10 = aMyFunction::SeededRandomIntRange(2, 3, Id * 254571);
                        pas::Extended cpp_right_12 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.01, 0.05);
                        std::int64_t cpp_right_11 = System::Round(PickupPower * cpp_right_12);
                        PickupPower = PickupPower + cpp_right_11 + cpp_right_10;
                    }
                    break;
                }
                case ikMedium: {
                    Range += aMyFunction::SeededRandomIntRange(15, 20, Id * 354571);
                    {
                        std::int32_t cpp_right_13 = aMyFunction::SeededRandomIntRange(3, 4, Id * 254571);
                        pas::Extended cpp_right_15 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.01, 0.05);
                        std::int64_t cpp_right_14 = System::Round(PickupPower * cpp_right_15);
                        PickupPower = PickupPower + cpp_right_14 + cpp_right_13;
                    }
                    break;
                }
                case ikMajor: {
                    Range += aMyFunction::SeededRandomIntRange(20, 25, Id * 354571);
                    {
                        std::int32_t cpp_right_16 = aMyFunction::SeededRandomIntRange(4, 5, Id * 254571);
                        pas::Extended cpp_right_18 = aMyFunction::SeededRandomFloatRange(Id * 374571, 0.01, 0.05);
                        std::int64_t cpp_right_17 = System::Round(PickupPower * cpp_right_18);
                        PickupPower = PickupPower + cpp_right_17 + cpp_right_16;
                    }
                    break;
                }
            }
        }
        Cost += CalculateImprovementCost(Kind) / 2;
        DetailImprovement = 0;
    }

    std::uint8_t TCargoHook::HasStandardStats() {
        std::uint8_t StandardPower{};
        std::uint8_t StandardRange{};
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t))) == 0) {
            StandardPower = CalculateGeneratedPickupPower() == PickupPower;
        } else {
            StandardPower = CalculateGeneratedPickupPower() == PickupPower - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t)));
        }
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t))) == 0) {
            StandardRange = CalculateGeneratedRange() == Range;
        } else {
            StandardRange = CalculateGeneratedRange() == Range - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t)));
        }
        return StandardPower && StandardRange;
    }

    pas::WideString TCargoHook_GetInfoText(TCargoHook* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"" && Self->CustomFaction != u"") {
            Text = aConst::LocalizedText(pas::concat_wide({u"Items.CargoHook.", Self->CustomFaction, u"Text"}));
        }
        if (Text == u"") {
            if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                Text = aConst::LocalizedText(u"Items.CargoHook.Text"_wref.get());
            } else {
                Text = aConst::LocalizedText(u"Items.CargoHook.KlingText"_wref.get());
            }
        }
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Text});
    }

    void TCargoHook::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        std::int32_t BaseValue{};
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t))) == 0) {
            BonusText = pas::WideString();
        } else if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t))) > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t))))})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t)))), u"<color=255,0,0>"_w);
        }
        std::int32_t StatBonus = std::max<std::int32_t>(-std::max<std::int32_t>(0, PickupPower), GetStatBonus(aConst::bonHook));
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            StatBonus = std::max<std::int32_t>(static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonHook) - StatBonus, -(PickupPower + StatBonus));
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (MicroModuleIndex == 0) {
            BaseValue = PickupPower;
        } else {
            BaseValue = PickupPower - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHook * sizeof(std::int32_t)));
        }
        if (CalculateGeneratedPickupPower() == BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<PickUpSize>"_w, pas::concat_wide({pas::wide_int_to_str(BaseValue), BonusText}), ColorTag);
        } else if (CalculateGeneratedPickupPower() < BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<PickUpSize>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<PickUpSize>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t))) == 0) {
            BonusText = pas::WideString();
        } else if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t))) > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t))))})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t)))), u"<color=255,0,0>"_w);
        }
        StatBonus = std::max<std::int32_t>(-std::max<std::int32_t>(0, Range), GetStatBonus(aConst::bonHookRadius));
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            StatBonus = std::max<std::int32_t>(static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonHookRadius) - StatBonus, -(Range + StatBonus));
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (MicroModuleIndex == 0) {
            BaseValue = Range;
        } else {
            BaseValue = Range - pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t)));
        }
        if (CalculateGeneratedRange() == BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({pas::wide_int_to_str(BaseValue), BonusText}), u"<color=255,240,100>"_w);
        } else if (CalculateGeneratedRange() < BaseValue) {
            aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseValue), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
        aMyFunction::ReplaceTextToken(Text, u"<SpeedMin>"_w, pas::wide_int64_to_str(System::Round(aConst::CargoHookLevelStats[TechLevel].MinPullSpeed)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<SpeedMax>"_w, pas::wide_int64_to_str(System::Round(aConst::CargoHookLevelStats[TechLevel].MaxPullSpeed)), u"<color=255,240,100>"_w);
    }

    void TDefGenerator::Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        ItemType = aConst::t_DefGenerator;
        this->Weight = Weight;
        TechLevel = Level;
        DamageFactor = CalculateGeneratedDamageFactor();
        OwnerId = Owner;
        Repair();
        Cost = aItem::CalculateGeneratedDefGeneratorCost(Weight, Level, Owner);
        MicroModuleIndex = 0;
    }

    void TDefGenerator::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddSingle(DamageFactor);
    }

    void TDefGenerator::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        DamageFactor = EC_Buf::TBufEC_GetSingle(Buffer);
    }

    void TDefGenerator::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::float_to_str(1.0L - DamageFactor, SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Prouwseor"_w);
            Block->AddParam(decodeTextW_2, cpp_arg);
        }
    }

    void TDefGenerator_LoadFromBlock(TDefGenerator* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->DamageFactor = 1.0L - EC_Str::ExtractDecimalToSingleW(Block->GetParam(EC_Str::DecodeTextW(u"Prouwseor"_w)));
    }

    float TDefGenerator::CalculateGeneratedDamageFactor() {
        return aConst::DefGeneratorLevelFactors[TechLevel];
    }

    void TDefGenerator::Improve(TImprovementKind Kind) {
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        switch (Kind) {
            case ikMinor: {
                pas::Extended cpp_right = aMyFunction::SeededRandomFloatRange(Id * 254573, 0.01, 0.03);
                DamageFactor = DamageFactor - cpp_right;
                break;
            }
            case ikMedium: {
                pas::Extended cpp_right_2 = aMyFunction::SeededRandomFloatRange(Id * 254572, 0.02, 0.04);
                DamageFactor = DamageFactor - cpp_right_2;
                break;
            }
            case ikMajor: {
                pas::Extended cpp_right_3 = aMyFunction::SeededRandomFloatRange(Id * 254571, 0.03, 0.05);
                DamageFactor = DamageFactor - cpp_right_3;
                break;
            }
        }
        Cost += CalculateImprovementCost(Kind) / 2;
    }

    std::uint8_t TDefGenerator::HasStandardStats() {
        std::int32_t ActualPercent = System::Round(DamageFactor * 1.0E+2L);
        std::int32_t GeneratedPercent = System::Round(CalculateGeneratedDamageFactor() * 1.0E+2L);
        if (MicroModuleIndex == 0 || pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t))) == 0) {
            return ActualPercent == GeneratedPercent;
        }
        std::int32_t BonusPercent = System::Round(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t))));
        return ActualPercent + BonusPercent == GeneratedPercent;
    }

    pas::WideString TDefGenerator_GetInfoText(TDefGenerator* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"" && Self->CustomFaction != u"") {
            Text = aConst::LocalizedText(pas::concat_wide({u"Items.DefGenerator.", Self->CustomFaction, u"Text"}));
        }
        if (Text == u"") {
            if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                Text = aConst::LocalizedText(u"Items.DefGenerator.Text"_wref.get());
            } else {
                Text = aConst::LocalizedText(u"Items.DefGenerator.KlingText"_wref.get());
            }
        }
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Text});
    }

    void TDefGenerator::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        std::int32_t ModuleBonus = 0;
        if (MicroModuleIndex != 0) {
            ModuleBonus = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t)));
        }
        std::uint8_t DisplayPercent = aItem::DefenseDamageFactorToPercent(DamageFactor);
        if (ModuleBonus == 0) {
            BonusText = pas::WideString();
        } else if (ModuleBonus > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(ModuleBonus)})), u"<color=0,255,0>"_w);
        } else {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(ModuleBonus), u"<color=255,0,0>"_w);
        }
        std::int32_t StatBonus = std::max<std::int32_t>(-std::max<std::int32_t>(0, static_cast<std::int32_t>(DisplayPercent)), GetStatBonus(aConst::bonDef));
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(StatBonus)})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(StatBonus), u"<color=255,167,84>"_w)});
            }
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            StatBonus = std::max<std::int32_t>(static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonDef) - StatBonus, -(DisplayPercent + StatBonus));
        } else {
            StatBonus = 0;
        }
        if (StatBonus != 0) {
            if (StatBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(StatBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        std::int32_t ActualFactorPercent = System::Round(DamageFactor * 1.0E+2L);
        std::int32_t GeneratedFactorPercent = System::Round(CalculateGeneratedDamageFactor() * 1.0E+2L);
        if (MicroModuleIndex != 0 && pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t))) != 0) {
            ActualFactorPercent += System::Round(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonDef * sizeof(std::int32_t))));
        }
        if (HasStandardStats()) {
            aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, pas::concat_wide({pas::wide_int_to_str(DisplayPercent - ModuleBonus), BonusText}), ColorTag);
        } else if (ActualFactorPercent < GeneratedFactorPercent) {
            aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(DisplayPercent - ModuleBonus), u"<color=0,255,0>"_w), BonusText}), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(DisplayPercent - ModuleBonus), u"<color=255,0,0>"_w), BonusText}), ColorTag);
        }
    }

    void TWeapon_Destroy(TWeapon* Self) {
        aItem::TEquipment_Destroy(Self);
    }

    void TWeapon::Init(aConst::TItemType ItemType, std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        Target = nullptr;
        this->ItemType = ItemType;
        this->Weight = Weight;
        TechLevel = Level;
        Range = CalculateGeneratedRange();
        MinDamage = CalculateGeneratedMinDamage();
        MaxDamage = CalculateGeneratedMaxDamage();
        OwnerId = Owner;
        Repair();
        Cost = aItem::CalculateGeneratedWeaponCost(GetWeaponInfo(), Weight, Level, Owner);
        if (pas::in_range(GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            AmmoCapacity = CalculateGeneratedAmmoCapacity();
            Ammo = AmmoCapacity;
        }
        MicroModuleIndex = 0;
    }

    void TCustomWeapon::InitCustom(aConst::PWeaponInfo Info, std::uint8_t Equipped, std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        Target = nullptr;
        ItemType = aConst::t_CustomWeapon;
        CustomInfo = Info;
        if (Equipped) {
            Equip();
        } else {
            Unequip();
        }
        this->Weight = Weight;
        TechLevel = Level;
        Range = CalculateGeneratedRange();
        MinDamage = CalculateGeneratedMinDamage();
        MaxDamage = CalculateGeneratedMaxDamage();
        OwnerId = Owner;
        Repair();
        Cost = aItem::CalculateGeneratedWeaponCost(Info, Weight, Level, Owner);
        if (pas::in_range(GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            AmmoCapacity = CalculateGeneratedAmmoCapacity();
            Ammo = AmmoCapacity;
        }
        MicroModuleIndex = 0;
    }

    void TWeapon::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddWideChar(Range);
        Buffer->AddIntegerValue(MinDamage);
        Buffer->AddIntegerValue(MaxDamage);
        if (Target == nullptr) {
            Buffer->AddAnsiChar('\000');
        } else if (pas::class_cast_if<aShip::TShip*>(Target) != nullptr) {
            Buffer->AddAnsiChar('\001');
            Buffer->AddDWord(pas::checked_cast<aShip::TShip*>(Target)->Id);
        } else if (pas::class_cast_if<aItem::TItem*>(Target) != nullptr) {
            Buffer->AddAnsiChar('\002');
            Buffer->AddDWord(pas::checked_cast<aItem::TItem*>(Target)->Id);
        } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(Target) != nullptr) {
            Buffer->AddAnsiChar('\003');
            Buffer->AddDWord(pas::checked_cast<aAsteroid::TAsteroid*>(Target)->Id);
        } else if (pas::class_cast_if<aMissile::TMissile*>(Target) != nullptr) {
            Buffer->AddAnsiChar('\004');
            Buffer->AddDWord(pas::checked_cast<aMissile::TMissile*>(Target)->Id);
        } else {
            Buffer->AddAnsiChar('\000');
        }
        if (pas::in_range(GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            Buffer->AddDWord(Ammo);
            Buffer->AddDWord(AmmoCapacity);
        }
    }

    void TCustomWeapon::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(CustomInfo->ConfigName);
        aItem::TWeapon::SaveToBuffer(Buffer);
    }

    void TWeapon::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        Range = EC_Buf::TBufEC_GetWord(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 115) {
            MinDamage = EC_Buf::TBufEC_GetInt32(Buffer);
            MaxDamage = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            MinDamage = EC_Buf::TBufEC_GetByte(Buffer);
            MaxDamage = EC_Buf::TBufEC_GetByte(Buffer);
        }
        LoadedTargetKind = static_cast<TWeaponTargetKind>(EC_Buf::TBufEC_GetByte(Buffer));
        if (LoadedTargetKind == wtkNone) {
            Target = nullptr;
        } else {
            Target = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        }
        if (pas::in_range(GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            Ammo = EC_Buf::TBufEC_GetUInt32(Buffer);
            AmmoCapacity = EC_Buf::TBufEC_GetUInt32(Buffer);
            if (GlobalsV::LoadedSaveVersion < 118 && MicroModuleIndex != 0 && pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonWMissile * sizeof(std::int32_t))) != 0) {
                switch (aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber) {
                    case 204: MaxDamage += 10; break;
                    case 210: MaxDamage += 15; break;
                    case 216: MaxDamage += 15; break;
                    case 2: MaxDamage += 4; break;
                    case 18: MaxDamage += 4; break;
                    case 119: MaxDamage -= 2; break;
                    default: {
                        MaxDamage += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonWMissile * sizeof(std::int32_t))) / 2;
                        break;
                    }
                }
            }
        }
    }

    void TCustomWeapon::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        CustomInfo = Galaxy->RequireCustomWeaponInfo(Buffer->ReadWideString());
        aItem::TWeapon::LoadFromBuffer(Buffer, Galaxy);
    }

    void TWeapon::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(TechLevel));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Range);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Rialdoinurs"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(MinDamage);
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"MailnoDrakmoarglen"_w);
            Block->AddParam(decodeTextW_3, intToStr_3);
        }
        {
            const pas::WideString& intToStr_4 = pas::wide_int_to_str(MaxDamage);
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"MianxaDoarmuavgre"_w);
            Block->AddParam(decodeTextW_4, intToStr_4);
        }
        {
            const pas::WideString& intToStr_5 = pas::wide_int_to_str(Ammo);
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"Almamuo"_w);
            Block->AddParam(decodeTextW_5, intToStr_5);
        }
        {
            const pas::WideString& intToStr_6 = pas::wide_int_to_str(AmmoCapacity);
            const pas::WideString& decodeTextW_6 = EC_Str::DecodeTextW(u"MraixoAsmImGod"_w);
            Block->AddParam(decodeTextW_6, intToStr_6);
        }
    }

    void TCustomWeapon::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        Block->AddParam(EC_Str::DecodeTextW(u"CrulsitroimaTryspie"_w), CustomInfo->ConfigName);
        aItem::TWeapon::SaveToBlock(Block);
    }

    void TWeapon_LoadFromBlock(TWeapon* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Tre4cwh0L6eHv3ealf"_w))));
        Self->Range = static_cast<std::uint16_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Rialdoinurs"_w)))));
        Self->MinDamage = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"MailnoDrakmoarglen"_w))));
        Self->MaxDamage = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"MianxaDoarmuavgre"_w))));
        Self->Ammo = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Almamuo"_w))));
        Self->AmmoCapacity = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"MraixoAsmImGod"_w))));
    }

    void TCustomWeapon_LoadFromBlock(TCustomWeapon* Self, EC_BlockPar::TBlockParEC* Block) {
        Self->CustomInfo = aGalaxy::Galaxy->RequireCustomWeaponInfo(Block->GetParam(EC_Str::DecodeTextW(u"CrulsitroimaTryspie"_w)));
        aItem::TWeapon_LoadFromBlock(Self, Block);
    }

    void TWeapon::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aItem::TItem::ResolveLoadedReferences(Galaxy);
        if (LoadedTargetKind == wtkShip) {
            Target = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Target)), true)));
        } else if (LoadedTargetKind == wtkItem) {
            Target = pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(Galaxy->IdToItem(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Target)), true)));
        } else if (LoadedTargetKind == wtkAsteroid) {
            Target = pas::checked_cast<aAsteroid::TAsteroid*>(static_cast<pas::Object*>(Galaxy->IdToAsteroid(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Target)))));
        } else if (LoadedTargetKind == wtkMissile) {
            Target = pas::checked_cast<aMissile::TMissile*>(static_cast<pas::Object*>(Galaxy->IdToMissile(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Target)))));
        }
    }

    void TWeapon::Unequip() {
        aItem::TEquipment::Unequip();
        Target = nullptr;
    }

    std::int32_t TWeapon::CalculateGeneratedAmmoCapacity() {
        return TechLevel * 5 + 25;
    }

    std::int32_t TWeapon::CalculateGeneratedMinDamage() {
        aConst::PWeaponInfo Info = GetWeaponInfo();
        return System::Round(static_cast<long double>(Info->MinDamage) * pas::load_unaligned<float>(pas::byte_offset(&Info->DamageScaleByLevel, (TechLevel - 1) * sizeof(float))));
    }

    std::int32_t TWeapon::CalculateGeneratedMaxDamage() {
        aConst::PWeaponInfo Info = GetWeaponInfo();
        return System::Round(static_cast<long double>(Info->MaxDamage) * pas::load_unaligned<float>(pas::byte_offset(&Info->DamageScaleByLevel, (TechLevel - 1) * sizeof(float))));
    }

    std::int32_t TWeapon::CalculateGeneratedRange() {
        return System::Round(static_cast<long double>(GetWeaponInfo()->AverageRange) * aConst::WeaponRangeLevelFactors[TechLevel]);
    }

    // Generated maximum plus ordinary and special module damage bonuses; used by HasStandardStats.
    std::int32_t TWeapon::CalculateStandardMaxDamage() {
        std::int32_t Result = CalculateGeneratedMaxDamage();
        std::uint8_t BonusKind = aConst::WeaponDamageClasses[aConst::ClassifyWeaponDamageFlags(GetWeaponInfo()->DamageFlags)].BonusKind;
        if (MicroModuleIndex != 0) {
            Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
        }
        if (SpecialModuleIndex != 0) {
            Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
        }
        return Result;
    }

    // Generated range plus ordinary and special module range bonuses; used by HasStandardStats.
    std::int32_t TWeapon::CalculateStandardRange() {
        std::int32_t Result = CalculateGeneratedRange();
        if (MicroModuleIndex != 0) {
            Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t)));
        }
        if (SpecialModuleIndex != 0) {
            Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t)));
        }
        return Result;
    }

    void TWeapon::Improve(TImprovementKind Kind) {
        std::int32_t ModuleDamage{};
        if (Kind == ikAny) {
            Kind = static_cast<TImprovementKind>(aMyFunction::SeededRandomIntRange(0, 2, aGalaxy::Galaxy->CurrentTurn * Id));
        }
        aConst::PWeaponInfo Info = GetWeaponInfo();
        if (DetailImprovement == 0) {
            if (pas::in_range(Info->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                DetailImprovement = 1;
            } else if (aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->CurrentTurn * (Kind + 1) * Id) < 0.5L) {
                DetailImprovement = 1;
            } else {
                DetailImprovement = 2;
            }
        }
        std::int32_t CurrentDamage = MaxDamage;
        std::int32_t BaseDamage = Info->MaxDamage;
        if (pas::in_range(static_cast<std::uint8_t>(Info->ShotType), static_cast<std::int32_t>(aGalaxyStruct::wstMissile), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && MicroModuleIndex > 0 && static_cast<std::uint8_t>(aGalaxy::Galaxy->AreOldMissileBonusesEnabled() ^ 1)) {
            ModuleDamage = pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::WeaponDamageClasses[aConst::ClassifyWeaponDamageFlags(Info->DamageFlags)].BonusKind * sizeof(std::int32_t)));
            CurrentDamage = MathImports::Ceil(CurrentDamage - (1.0L - pas::real_divide(1.0L, GetShotCount())) * ModuleDamage);
        }
        if (DetailImprovement == 1) {
            switch (Kind) {
                case ikMinor: {
                    {
                        pas::Extended cpp_left = static_cast<long double>(CurrentDamage) * aMyFunction::SeededRandomFloatRange(Id * 276247, 0.07, 0.12);
                        std::int64_t cpp_right = System::Round(cpp_left + static_cast<long double>(BaseDamage) * aMyFunction::SeededRandomFloatRange(Id * 976247, 0.1, 0.2));
                        MaxDamage = MaxDamage + cpp_right + 1;
                    }
                    Range += aMyFunction::RoundAndTruncateToTens(([&] {
                        pas::Extended cpp_right_2 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.02, 0.05);
                        pas::Extended cpp_left_2 = Range * cpp_right_2;
                        return cpp_left_2 + ([&] {
                            pas::Extended cpp_right_3 = aMyFunction::SeededRandomFloatRange(Id * 976247, 0.02, 0.04);
                            return Info->AverageRange * cpp_right_3;
                        }());
                    }()));
                    break;
                }
                case ikMedium: {
                    {
                        pas::Extended cpp_left_3 = static_cast<long double>(CurrentDamage) * aMyFunction::SeededRandomFloatRange(Id * 276247, 0.07, 0.12);
                        std::int64_t cpp_right_4 = System::Round(cpp_left_3 + static_cast<long double>(BaseDamage) * aMyFunction::SeededRandomFloatRange(Id * 976247, 0.2, 0.3));
                        MaxDamage = MaxDamage + cpp_right_4 + 2;
                    }
                    Range += aMyFunction::RoundAndTruncateToTens(([&] {
                        pas::Extended cpp_right_5 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.02, 0.05);
                        pas::Extended cpp_left_4 = Range * cpp_right_5;
                        return cpp_left_4 + ([&] {
                            pas::Extended cpp_right_6 = aMyFunction::SeededRandomFloatRange(Id * 976247, 0.03, 0.04);
                            return Info->AverageRange * cpp_right_6;
                        }());
                    }()));
                    break;
                }
                case ikMajor: {
                    {
                        pas::Extended cpp_left_5 = static_cast<long double>(CurrentDamage) * aMyFunction::SeededRandomFloatRange(Id * 276247, 0.07, 0.12);
                        std::int64_t cpp_right_7 = System::Round(cpp_left_5 + static_cast<long double>(BaseDamage) * aMyFunction::SeededRandomFloatRange(Id * 976247, 0.3, 0.4));
                        MaxDamage = MaxDamage + cpp_right_7 + 3;
                    }
                    Range += aMyFunction::RoundAndTruncateToTens(([&] {
                        pas::Extended cpp_right_8 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.02, 0.05);
                        pas::Extended cpp_left_6 = Range * cpp_right_8;
                        return cpp_left_6 + ([&] {
                            pas::Extended cpp_right_9 = aMyFunction::SeededRandomFloatRange(Id * 976247, 0.04, 0.06);
                            return Info->AverageRange * cpp_right_9;
                        }());
                    }()));
                    break;
                }
            }
        } else if (DetailImprovement == 2) {
            switch (Kind) {
                case ikMinor: {
                    Range += aMyFunction::RoundAndTruncateToTens(([&] {
                        pas::Extended cpp_right_10 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.04, 0.07);
                        pas::Extended cpp_left_7 = Range * cpp_right_10;
                        return cpp_left_7 + ([&] {
                            pas::Extended cpp_right_11 = aMyFunction::SeededRandomFloatRange(Id * 976247, 0.03, 0.06);
                            return Info->AverageRange * cpp_right_11;
                        }());
                    }()));
                    {
                        pas::Extended cpp_left_8 = static_cast<long double>(CurrentDamage) * aMyFunction::SeededRandomFloatRange(Id * 276247, 0.03, 0.06);
                        std::int64_t cpp_right_12 = System::Round(cpp_left_8 + static_cast<long double>(BaseDamage) * aMyFunction::SeededRandomFloatRange(Id * 976247, 0.05, 0.1));
                        MaxDamage = MaxDamage + cpp_right_12 + 1;
                    }
                    break;
                }
                case ikMedium: {
                    Range += aMyFunction::RoundAndTruncateToTens(([&] {
                        pas::Extended cpp_right_13 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.04, 0.07);
                        pas::Extended cpp_left_9 = Range * cpp_right_13;
                        return cpp_left_9 + ([&] {
                            pas::Extended cpp_right_14 = aMyFunction::SeededRandomFloatRange(Id * 976247, 0.06, 0.08);
                            return Info->AverageRange * cpp_right_14;
                        }());
                    }()));
                    {
                        pas::Extended cpp_left_10 = static_cast<long double>(CurrentDamage) * aMyFunction::SeededRandomFloatRange(Id * 276247, 0.03, 0.06);
                        std::int64_t cpp_right_15 = System::Round(cpp_left_10 + static_cast<long double>(BaseDamage) * aMyFunction::SeededRandomFloatRange(Id * 976247, 0.1, 0.15));
                        MaxDamage = MaxDamage + cpp_right_15 + 2;
                    }
                    break;
                }
                case ikMajor: {
                    Range += aMyFunction::RoundAndTruncateToTens(([&] {
                        pas::Extended cpp_right_16 = aMyFunction::SeededRandomFloatRange(Id * 354571, 0.04, 0.07);
                        pas::Extended cpp_left_11 = Range * cpp_right_16;
                        return cpp_left_11 + ([&] {
                            pas::Extended cpp_right_17 = aMyFunction::SeededRandomFloatRange(Id * 976247, 0.08, 0.11);
                            return Info->AverageRange * cpp_right_17;
                        }());
                    }()));
                    {
                        pas::Extended cpp_left_12 = static_cast<long double>(CurrentDamage) * aMyFunction::SeededRandomFloatRange(Id * 276247, 0.03, 0.06);
                        std::int64_t cpp_right_18 = System::Round(cpp_left_12 + static_cast<long double>(BaseDamage) * aMyFunction::SeededRandomFloatRange(Id * 976247, 0.15, 0.2));
                        MaxDamage = MaxDamage + cpp_right_18 + 3;
                    }
                    break;
                }
            }
        } else if (DetailImprovement == 3) {
            Range += aMyFunction::RoundAndTruncateToTens(([&] {
                pas::Extended cpp_right_19 = aMyFunction::SeededRandomFloatRange(Id * 976247, 0.12, 0.18);
                pas::Extended cpp_left_13 = Range * cpp_right_19;
                return cpp_left_13 + (System::Sqrt(pas::sqr(Info->AverageRange) + 90000) - Info->AverageRange);
            }()));
        }
        Cost += CalculateImprovementCost(Kind) / 2;
        DetailImprovement = 0;
    }

    std::uint8_t TWeapon::HasStandardStats() {
        return CalculateStandardMaxDamage() == MaxDamage && CalculateStandardRange() == Range;
    }

    pas::WideString TWeapon::GetDisplayName() {
        pas::WideString Result{};
        if (NameOverride != u"") {
            Result = NameOverride;
        } else if (SpecialModuleIndex != 0 && aConst::MicroModuleTemplates[SpecialModuleIndex - 1].TextReplace != u"") {
            Result = aConst::MicroModuleTemplates[SpecialModuleIndex - 1].Name;
        } else {
            Result = GetShortName();
        }
        if (HasMicroModule()) {
            return pas::concat_wide({Result, u" ", ([&] {
                pas::WideString microModuleNameColorTag = aItem::GetMicroModuleNameColorTag(MicroModuleIndex - 1);
                pas::WideString microModuleQuotedName = GetMicroModuleQuotedName();
                return aMyFunction::WrapTextInColor(std::move(microModuleQuotedName), std::move(microModuleNameColorTag));
            }())});
        }
        return Result;
    }

    pas::WideString TWeapon::GetShortName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return aConst::LocalizedText(pas::concat_wide({u"Items.Weapon.Name.", GetConfigName()}));
    }

    pas::WideString TWeapon_GetInfoText(TWeapon* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        pas::WideString LevelText{};
        aConst::TWeaponDamageClass DamageClass{};
        Text = pas::WideString();
        if (Self->SpecialModuleIndex != 0) {
            Text = aConst::MicroModuleTemplates[Self->SpecialModuleIndex - 1].TextReplace;
        }
        if (Text == u"") {
            Text = aConst::LocalizedText(pas::concat_wide({u"Items.Weapon.Text.", Self->GetConfigName()}));
        }
        DamageClass = aConst::ClassifyWeaponDamageFlags(Self->GetWeaponInfo()->DamageFlags);
        Self->ReplaceInfoTokens(Text, ColorTag, Ship);
        LevelText = pas::concat_wide({u" (", Self->GetLevelLetter(), u")"});
        Text = pas::concat_wide({Text, u"\r\n", ([&] {
            pas::WideString cpp_arg = pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"Items.Weapon.Type", aConst::WeaponDamageClasses[DamageClass].Name})), LevelText});
            pas::WideString localizedText = aConst::LocalizedText(u"Items.Weapon.AddText"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedText), ColorTag, u"<WeaponType>"_w, std::move(cpp_arg));
        }())});
        Text = pas::concat_wide_reverse({aItem::TEquipment_GetBonusDescription(Self, ColorTag), Text});
        if (Self->ScriptItem != nullptr) {
            Text = reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Text, ColorTag);
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Text});
    }

    void TWeapon::ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) {
        pas::WideString BonusText{};
        // Native initializes/finalizes this extra string slot without reading it.
        pas::WideString UnusedNativeText{};
        std::int32_t EffectiveBonus{};
        std::int32_t I{};
        std::int32_t ExpectedRange{};
        std::int32_t CurrentRange{};
        std::int32_t BaseRange{};
        PExtraSpecial Entry{};
        std::int32_t ShipBonus{};
        std::int32_t MissileRange{};
        // Native flag displays the maximum as the minimum too.
        if ((std::bit_cast<std::uint32_t>(GetDamageFlags()) & 0x00100000) != 0) {
            aMyFunction::ReplaceTextToken(Text, u"<MinDamage>"_w, pas::wide_int_to_str(std::max<std::int32_t>(MaxDamage, MinDamage)), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<MinDamage>"_w, pas::wide_int_to_str(MinDamage), ColorTag);
        }
        std::int32_t ModuleBonus = 0;
        std::uint8_t DamageClass = static_cast<std::uint8_t>(aConst::ClassifyWeaponDamageFlags(GetWeaponInfo()->DamageFlags));
        if (MicroModuleIndex != 0) {
            ModuleBonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::WeaponDamageClasses[DamageClass].BonusKind * sizeof(std::int32_t)));
        }
        std::int32_t BaseDamage = std::max<std::int32_t>(MaxDamage, MinDamage) - ModuleBonus;
        std::int32_t ExpectedDamage = CalculateGeneratedMaxDamage();
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                BaseDamage += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, aConst::WeaponDamageClasses[DamageClass].BonusKind * sizeof(std::int32_t))) * Entry->Count;
            }
        }
        if (ModuleBonus > 0) {
            BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(ModuleBonus)})), u"<color=0,255,0>"_w);
        }
        if (ModuleBonus < 0) {
            BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(ModuleBonus), u"<color=255,0,0>"_w);
        }
        if (Ship != nullptr && EquippedFlag != 0) {
            ShipBonus = static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::WeaponDamageClasses[DamageClass].BonusKind);
        } else {
            ShipBonus = 0;
        }
        if (ShipBonus != 0) {
            if (ShipBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(ShipBonus), ")"})), u"<color=255,167,84>"_w)});
            } else {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(ShipBonus), ")"})), u"<color=255,167,84>"_w)});
            }
        }
        if (BaseDamage == ExpectedDamage) {
            aMyFunction::ReplaceTextToken(Text, u"<MaxDamage>"_w, pas::wide_int_to_str(BaseDamage), ColorTag);
        } else if (BaseDamage > ExpectedDamage) {
            aMyFunction::ReplaceTextToken(Text, u"<MaxDamage>"_w, aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseDamage), u"<color=0,255,0>"_w), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<MaxDamage>"_w, aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseDamage), u"<color=255,0,0>"_w), ColorTag);
        }
        aMyFunction::ReplaceTextToken(Text, u"<Bonus>"_w, BonusText, ColorTag);
        if (pas::in_range(GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            ModuleBonus = 0;
            if (ExtraSpecials != nullptr) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range_2.next(I); ) {
                    Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                    ModuleBonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t))) * Entry->Count;
                }
            }
            ExpectedRange = CalculateGeneratedRange() + ModuleBonus;
            CurrentRange = Range + ModuleBonus;
            if (MicroModuleIndex != 0) {
                ModuleBonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t)));
            } else {
                ModuleBonus = 0;
            }
            BaseRange = CurrentRange - ModuleBonus;
            MissileRange = GetWeaponInfo()->MissileRange;
            EffectiveBonus = std::max<std::int32_t>(CurrentRange, MissileRange) - std::max<std::int32_t>(BaseRange, MissileRange);
            if (ModuleBonus > 0) {
                BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(EffectiveBonus)})), u"<color=0,255,0>"_w);
            } else if (ModuleBonus < 0) {
                if (EffectiveBonus < 0) {
                    BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(EffectiveBonus), u"<color=255,0,0>"_w);
                } else {
                    BonusText = aMyFunction::WrapTextInColor(u"-0"_w, u"<color=255,0,0>"_w);
                }
            } else {
                BonusText = pas::WideString();
            }
            if (Ship != nullptr && EquippedFlag != 0) {
                ShipBonus = static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonWRadius);
            } else {
                ShipBonus = 0;
            }
            EffectiveBonus = std::max<std::int32_t>(CurrentRange + ShipBonus, MissileRange) - std::max<std::int32_t>(CurrentRange, MissileRange);
            if (ShipBonus > 0) {
                BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"(+", SysUtils::IntToStr(EffectiveBonus), ")"})), u"<color=255,167,84>"_w)});
            } else if (ShipBonus < 0) {
                // Native negative branch omits the opening parenthesis.
                if (EffectiveBonus < 0) {
                    BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(EffectiveBonus), ")"})), u"<color=255,167,84>"_w)});
                } else {
                    BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(u"(-0)"_w, u"<color=255,167,84>"_w)});
                }
            }
            if (BaseRange == ExpectedRange) {
                aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({pas::wide_int_to_str(std::max<std::int32_t>(BaseRange, MissileRange)), BonusText}), ColorTag);
            } else if (BaseRange > ExpectedRange) {
                aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(std::max<std::int32_t>(BaseRange, MissileRange)), u"<color=0,255,0>"_w), BonusText}), ColorTag);
            } else {
                aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(std::max<std::int32_t>(BaseRange, MissileRange)), u"<color=255,0,0>"_w), BonusText}), ColorTag);
            }
            aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(Ammo), ColorTag);
            aMyFunction::ReplaceTextToken(Text, u"<MaxCount>"_w, pas::wide_int_to_str(AmmoCapacity), ColorTag);
            aMyFunction::ReplaceTextToken(Text, u"<CntShots>"_w, pas::wide_int_to_str(GetShotCount()), ColorTag);
        } else {
            ModuleBonus = 0;
            if (ExtraSpecials != nullptr) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range_3.next(I); ) {
                    Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                    ModuleBonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t))) * Entry->Count;
                }
            }
            ExpectedRange = CalculateGeneratedRange() + ModuleBonus;
            CurrentRange = Range + ModuleBonus;
            if (MicroModuleIndex != 0) {
                ModuleBonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t)));
            } else {
                ModuleBonus = 0;
            }
            BaseRange = CurrentRange - ModuleBonus;
            if (ModuleBonus > 0) {
                BonusText = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(ModuleBonus)})), u"<color=0,255,0>"_w);
            } else if (ModuleBonus < 0) {
                BonusText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(ModuleBonus), u"<color=255,0,0>"_w);
            } else {
                BonusText = pas::WideString();
            }
            if (Ship != nullptr && EquippedFlag != 0) {
                ShipBonus = static_cast<aShip::TShip*>(Ship)->GetTotalStatBonus(aConst::bonWRadius);
            } else {
                ShipBonus = 0;
            }
            if (ShipBonus != 0) {
                if (ShipBonus > 0) {
                    BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(ShipBonus)})), u"<color=255,167,84>"_w)});
                } else {
                    BonusText = pas::concat_wide({BonusText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(ShipBonus), u"<color=255,167,84>"_w)});
                }
            }
            if (BaseRange == ExpectedRange) {
                aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({pas::wide_int_to_str(BaseRange), BonusText}), ColorTag);
            } else if (BaseRange > ExpectedRange) {
                aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseRange), u"<color=0,255,0>"_w), BonusText}), ColorTag);
            } else {
                aMyFunction::ReplaceTextToken(Text, u"<Radius>"_w, pas::concat_wide({aMyFunction::WrapTextInColor(pas::wide_int_to_str(BaseRange), u"<color=255,0,0>"_w), BonusText}), ColorTag);
            }
        }
        aMyFunction::ReplaceTextToken(Text, u"<CntAttacks>"_w, pas::wide_int_to_str(GetAttackCount()), ColorTag);
    }

    pas::WideString TWeapon::GetDescriptionText() {
        return aConst::LocalizedText(pas::concat_wide({u"Items.TWeapon.Description.", GetConfigName()}));
    }

    double TWeapon::GetShotDelayFactor() {
        std::int32_t I{};
        PExtraSpecial Entry{};
        std::int32_t SpeedPercent = GetWeaponInfo()->ShotSpeedPercent;
        if (MicroModuleIndex != 0) {
            SpeedPercent += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonShotSpeed * sizeof(std::int32_t)));
        }
        if (SpecialModuleIndex != 0) {
            SpeedPercent += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, aConst::bonShotSpeed * sizeof(std::int32_t)));
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                SpeedPercent += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, aConst::bonShotSpeed * sizeof(std::int32_t))) * Entry->Count;
            }
        }
        SpeedPercent = std::max<std::int32_t>(0, std::min<std::int32_t>(100, SpeedPercent));
        return 1.0L - SpeedPercent * 0.01L;
    }

    pas::WideString TWeapon::GetBitmapResourceName() {
        if (ConfigBlockName != u"") {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName});
        } else if (SpecialModuleIndex > 0 && aConst::MicroModuleTemplates[SpecialModuleIndex - 1].KindGraph != u"") {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType], aConst::MicroModuleTemplates[SpecialModuleIndex - 1].KindGraph});
        } else {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType]});
        }
    }

    pas::WideString TCustomWeapon::GetBitmapResourceName() {
        if (ConfigBlockName != u"") {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName});
        } else if (SpecialModuleIndex > 0 && aConst::MicroModuleTemplates[SpecialModuleIndex - 1].KindGraph != u"") {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"W", GetConfigName(), aConst::MicroModuleTemplates[SpecialModuleIndex - 1].KindGraph});
        } else {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"W", GetConfigName()});
        }
    }

    std::uint8_t TWeapon::NeedsAmmo() {
        std::uint8_t Result = false;
        if (pas::in_range(GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            if (Ammo < AmmoCapacity) {
                return true;
            }
        }
        return Result;
    }

    std::int32_t TWeapon::CalculateAmmoRefillCost() {
        std::int32_t MissingAmmo{};
        float UnitCost{};
        std::int32_t Result = 0;
        if (!pas::in_range(GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            return Result;
        }
        if (Ammo < AmmoCapacity) {
            MissingAmmo = AmmoCapacity - Ammo;
            UnitCost = aGalaxy::Galaxy->ScaleIntByTechLevel(10, 100);
            return System::Round(static_cast<long double>(MissingAmmo) * UnitCost);
        }
        return Result;
    }

    std::int32_t TWeapon::GetShotPalette() {
        if (SpecialModuleIndex == 0 || aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ShotVisual == -1) {
            return GetWeaponInfo()->DefaultPalette;
        }
        return aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ShotVisual;
    }

    // Combines the weapon template and installed ordinary, special and extra-special module flags.
    aGalaxyStruct::TDamageFlagSet TWeapon::GetDamageFlags() {
        aGalaxyStruct::TDamageFlagSet Result{};
        std::int32_t I{};
        PExtraSpecial Entry{};
        Result = pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&GetWeaponInfo()->DamageFlags);
        if (SpecialModuleIndex != 0) {
            Result = Result + pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].WeaponDamageFlags);
        }
        if (MicroModuleIndex != 0) {
            Result = Result + pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].WeaponDamageFlags);
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                Result = Result + pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].WeaponDamageFlags);
            }
        }
        return Result;
    }

    std::int32_t TWeapon::GetShotCount() {
        std::int32_t I{};
        PExtraSpecial Entry{};
        std::int32_t Result = GetWeaponInfo()->ShotCount;
        if (!pas::is_one_of<aGalaxyStruct::wstChain, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(GetWeaponInfo()->ShotType)) {
            return Result;
        }
        if (SpecialModuleIndex != 0) {
            Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, aConst::bonShots * sizeof(std::int32_t)));
        }
        if (MicroModuleIndex != 0) {
            Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonShots * sizeof(std::int32_t)));
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, aConst::bonShots * sizeof(std::int32_t))) * Entry->Count;
            }
        }
        return std::max<std::int32_t>(Result, 1);
    }

    std::int32_t TWeapon::GetAttackCount() {
        std::int32_t I{};
        PExtraSpecial Entry{};
        std::int32_t Result = GetWeaponInfo()->AttackCount;
        if (SpecialModuleIndex != 0) {
            Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses, aConst::bonAttacks * sizeof(std::int32_t)));
        }
        if (MicroModuleIndex != 0) {
            Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonAttacks * sizeof(std::int32_t)));
        }
        if (ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ExtraSpecials) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TExtraSpecial>(ExtraSpecials, I);
                Result += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses, aConst::bonAttacks * sizeof(std::int32_t))) * Entry->Count;
            }
        }
        return std::max<std::int32_t>(Result, 1);
    }

    aConst::PWeaponInfo TWeapon::GetWeaponInfo() {
        return &aConst::WeaponInfos[ItemType];
    }

    aConst::PWeaponInfo TCustomWeapon::GetWeaponInfo() {
        return CustomInfo;
    }

    pas::WideString TWeapon::GetConfigName() {
        return pas::wide_int_to_str(ItemType - aConst::t_Weapon1 + 1);
    }

    pas::WideString TCustomWeapon::GetConfigName() {
        return CustomInfo->ConfigName;
    }

    void TGoods::Init(aConst::TItemType ItemType, std::int32_t Quantity) {
        this->ItemType = ItemType;
        this->Quantity = Quantity;
        Weight = Quantity;
        Cost = aConst::GoodsMarket[this->ItemType].AveragePrice * this->Quantity;
        NaturalFlag = false;
    }

    void TGoods::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TItem::SaveToBuffer(Buffer);
        Buffer->AddIntegerValue(Quantity);
        Buffer->AddBoolean(NaturalFlag);
    }

    void TGoods::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TItem::LoadFromBuffer(Buffer, Galaxy);
        Quantity = EC_Buf::TBufEC_GetInt32(Buffer);
        NaturalFlag = EC_Buf::TBufEC_GetBoolean(Buffer);
    }

    pas::WideString TGoods::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return aConst::GoodsMarket[ItemType].DisplayName;
    }

    pas::WideString TGoods_GetInfoText(TGoods* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Text{};
        Text = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Text", SysUtils::IntToStr(Self->ItemType + 1)})));
        return pas::concat_wide({aMyFunction::WrapTextInColor(Self->GetDisplayName(), ColorTag), Text});
    }

    pas::WideString TGoods::GetDescriptionText() {
        return aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Description.", SysUtils::IntToStr(ItemType + 1)})));
    }

    pas::WideString TGoods::GetBitmapResourceName() {
        return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType]});
    }

    void TCountableItem::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddIntegerValue(StackCount);
        Buffer->AddBoolean(DropFlag);
    }

    void TCountableItem::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        StackCount = EC_Buf::TBufEC_GetInt32(Buffer);
        DropFlag = EC_Buf::TBufEC_GetBoolean(Buffer);
    }

    void TCountableItem::Init(pas::WideString ConfigName, std::int32_t Count, std::uint8_t DropFlag) {
        ItemType = aConst::t_UselessCountableItem;
        ConfigBlockName = std::move(ConfigName);
        this->DropFlag = DropFlag;
        StackCount = Count;
        Weight = GetUnitSize() * Count;
        Cost = Count;
        OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited);
        EquippedFlag = 0;
        ConditionPercent = 0.0;
        BrokenFlag = 1;
    }

    pas::WideString TCountableItem::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return aConst::LocalizedText(pas::concat_wide({u"Items.CustomCountables.", ConfigBlockName, u".Name"}));
    }

    pas::WideString TCountableItem_GetInfoText(TCountableItem* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Result{};
        Result = aConst::LocalizedText(pas::concat_wide({u"Items.CustomCountables.", Self->ConfigBlockName, u".Text"}));
        aMyFunction::ReplaceTextToken(Result, u"<N>"_w, pas::wide_int_to_str(Self->StackCount), ColorTag);
        if (Self->ScriptItem != nullptr) {
            return reinterpret_cast<aScript::TScriptItem*>(Self->ScriptItem)->FormatDataText(Result, ColorTag);
        }
        return Result;
    }

    pas::WideString TCountableItem::GetDescriptionText() {
        return aConst::LocalizedText(pas::concat_wide({u"Items.CustomCountables.", ConfigBlockName, u".Description"}));
    }

    pas::WideString TCountableItem::GetBitmapResourceName() {
        if (StackCount <= 19) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName, u"0_"});
        } else if (StackCount <= 39) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName, u"1_"});
        } else if (StackCount <= 59) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName, u"2_"});
        } else if (StackCount <= 79) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName, u"3_"});
        } else {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName, u"4_"});
        }
    }

    // Defaults to 1 when UnitSize is not configured.
    std::int32_t TCountableItem::GetUnitSize() {
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Result = 1;
        if (ItemType != aConst::t_Protoplasm) {
            Block = GR_Main::LanguageDataConfig->GetBlockByPath(pas::concat_wide({u"Items.CustomCountables.", ConfigBlockName}));
            if (Block->CountParams(u"UnitSize"_wref.get()) > 0) {
                return EC_Str::ExtractDigitsToIntW(Block->GetParam(u"UnitSize"_wref.get()));
            }
        }
        return Result;
    }

    // Allocates a new stack and removes up to Count units from Self; preserves the nodes subtype and may create a script wrapper. Self must be nonempty and Count positive.
    TCountableItem* TCountableItem::Split(std::int32_t Count) {
        TCountableItem* Result{};
        std::int32_t I{};
        aScript::TScriptItem* NewScriptItem{};
        aScript::TScriptItem* OriginalScriptItem{};
        std::int32_t SplitCount = std::min<std::int32_t>(StackCount, Count);
        if (ItemType == aConst::t_Protoplasm) {
            Result = pas::construct_call<TProtoplasm>(TEquipment_Create);
            pas::checked_cast<TProtoplasm*>(Result)->Init_2(SplitCount, 0);
        } else {
            Result = pas::construct_call<TCountableItem>(TEquipment_Create);
            Result->Init(ConfigBlockName, SplitCount, 0);
        }
        Result->Cost = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(pas::real_divide(Cost, StackCount) * SplitCount));
        Cost = std::max<std::int32_t>(1, Cost - Result->Cost);
        StackCount -= SplitCount;
        {
            std::int32_t cpp_left = GetUnitSize();
            Weight = cpp_left * StackCount;
        }
        Result->OwnerId = OwnerId;
        Result->DominatorSeries = DominatorSeries;
        Result->CustomFaction = CustomFaction;
        if (ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(ScriptItem)->Name == u"") {
            OriginalScriptItem = reinterpret_cast<aScript::TScriptItem*>(ScriptItem);
            NewScriptItem = nullptr;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OriginalScriptItem->Script->Items) - 1); cpp_range.next(I); ) {
                NewScriptItem = pas::list_at<aScript::TScriptItem>(OriginalScriptItem->Script->Items, I);
                if (NewScriptItem->Name == u"" && NewScriptItem->Item == nullptr) {
                    break;
                }
                NewScriptItem = nullptr;
            }
            if (NewScriptItem == nullptr) {
                NewScriptItem = pas::construct_call<aScript::TScriptItem>(aScript::TScriptItem_Create);
                NewScriptItem->Script = OriginalScriptItem->Script;
                pas::list_add(OriginalScriptItem->Script->Items, reinterpret_cast<void*>(NewScriptItem));
            }
            NewScriptItem->Item = Result;
            Result->ScriptItem = NewScriptItem;
            if (NewScriptItem->ActionCode != nullptr) {
                pas::free(NewScriptItem->ActionCode);
            }
            NewScriptItem->ActionCode = nullptr;
            NewScriptItem->ActionCodeInitialized = false;
            NewScriptItem->OnActionText = OriginalScriptItem->OnActionText;
            NewScriptItem->OnUseText = OriginalScriptItem->OnUseText;
            NewScriptItem->CanSell = OriginalScriptItem->CanSell;
            NewScriptItem->Data[1] = OriginalScriptItem->Data[1];
            NewScriptItem->Data[2] = OriginalScriptItem->Data[2];
            NewScriptItem->Data[3] = OriginalScriptItem->Data[3];
            NewScriptItem->TextData1 = OriginalScriptItem->TextData1;
            NewScriptItem->TextData2 = OriginalScriptItem->TextData2;
            NewScriptItem->TextData3 = OriginalScriptItem->TextData3;
        }
        return Result;
    }

    std::uint8_t TCountableItem_CanMerge(TCountableItem* Self, pas::Object* Other) {
        std::uint8_t Result = false;
        if (!(pas::class_cast_if<TCountableItem*>(Other) != nullptr)) {
            return Result;
        }
        if (Self == Other) {
            return Result;
        }
        if (reinterpret_cast<TItem*>(Other)->ItemType != Self->ItemType) {
            return Result;
        }
        TCountableItem* OtherStack = reinterpret_cast<TCountableItem*>(Other);
        if (pas::class_cast_if<TProtoplasm*>(Self) != nullptr) {
            if (OtherStack->DominatorSeries != Self->DominatorSeries) {
                return Result;
            }
        }
        if (Self->ConfigBlockName == OtherStack->ConfigBlockName) {
            return true;
        }
        return Result;
    }

    // Leaves Other unchanged.
    std::uint8_t TCountableItem_Merge(TCountableItem* Self, pas::Object* Other) {
        TCountableItem* OtherStack{};
        std::uint8_t Result = false;
        if (aItem::TCountableItem_CanMerge(Self, Other)) {
            OtherStack = reinterpret_cast<TCountableItem*>(Other);
            Self->StackCount += OtherStack->StackCount;
            {
                std::int32_t cpp_left = Self->GetUnitSize();
                Self->Weight = cpp_left * Self->StackCount;
            }
            Self->Cost += OtherStack->Cost;
            return true;
        }
        return Result;
    }

    void TProtoplasm::Init_2(std::int32_t Count, std::uint8_t DropFlag) {
        ItemType = aConst::t_Protoplasm;
        StackCount = Count;
        Weight = Count;
        Cost = Count * 10;
        OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiDominator);
        this->DropFlag = DropFlag;
        EquippedFlag = 0;
        ConditionPercent = 0.0;
        BrokenFlag = 1;
    }

    pas::WideString TProtoplasm::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return pas::concat_wide({aConst::LocalizedText(u"Items.Nod.Name"_wref.get()), u" ", aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Nod.S", SysUtils::IntToStr(DominatorSeries)})))});
    }

    pas::WideString TProtoplasm_GetInfoText(TProtoplasm* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Result{};
        Result = aConst::LocalizedText(u"Items.Nod.Text"_wref.get());
        aMyFunction::ReplaceTextToken(Result, u"<N>"_w, pas::wide_int_to_str(Self->StackCount), ColorTag);
        return Result;
    }

    pas::WideString TProtoplasm::GetDescriptionText() {
        return aConst::LocalizedText(u"Items.Nod.Description"_wref.get());
    }

    pas::WideString TProtoplasm::GetBitmapResourceName() {
        if (StackCount <= 19) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Nod0_"});
        } else if (StackCount <= 39) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Nod1_"});
        } else if (StackCount <= 59) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Nod2_"});
        } else if (StackCount <= 79) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Nod3_"});
        } else {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Nod4_"});
        }
    }

    void TEquipmentWithActCode_Create(TEquipmentWithActCode* Self) {
        aItem::TEquipment_Create(Self);
        Self->ActionCode = nullptr;
        Self->ActCodeInitialized = false;
    }

    void TEquipmentWithActCode_Destroy(TEquipmentWithActCode* Self) {
        Self->ActionCode = nullptr;
        aItem::TEquipment_Destroy(Self);
    }

    void TUselessItem_Create(TUselessItem* Self) {
        aItem::TEquipmentWithActCode_Create(Self);
        Self->DisplayAsArtefact = false;
    }

    void TUselessItem_Destroy(TUselessItem* Self) {
        if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
            aScript::RunItemConfigActionCode(Self, aConst::satOnItemDestroy, nullptr, nullptr, nullptr, 0);
        }
        aItem::TEquipmentWithActCode_Destroy(Self);
    }

    void TUselessItem::Init(pas::WideString ConfigName, aGalaxyStruct::TDominatorSeries Series, std::uint32_t Seed, std::uint8_t ForceArtefactDisplay) {
        ItemType = aConst::t_UselessItem;
        DominatorSeries = Series;
        if (ConfigName == u"Remains") {
            do {
                ConfigBlockName = static_cast<pas::WideString>(pas::concat_ansi({"Remains_", SysUtils::IntToStr(aMyFunction::SeededRandomIntRange(0, Globals::UselessItemRemainsCount - 1, Seed))}));
                if (([&] {
                    const pas::WideString& lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"UselessItems.", ConfigBlockName, u".Owner"}));
                    const pas::WideString& cpp_arg = aConst::DominatorSeriesNames[Series];
                    return pas::pos(cpp_arg, lookupLocalizedTextByKey);
                }()) > 0) {
                    break;
                }
                ++Seed;
            } while (!false);
        } else {
            ConfigBlockName = ConfigName;
        }
        if (ForceArtefactDisplay) {
            DisplayAsArtefact = true;
        } else {
            CheckIfWeDisplayAsArtefact();
        }
        if (ConfigName == u"Remains") {
            OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiDominator);
        } else {
            OwnerId = aConst::OwnerFromInternalName(GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"UselessItems.", ConfigBlockName, u".Owner"})));
        }
        Weight = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"UselessItems.", ConfigBlockName, u".Size"}))));
        Weight = std::max<std::int64_t>(static_cast<std::int64_t>(1), ([&] {
            pas::Extended cpp_left_2 = aMyFunction::SeededRandomIntRange(0, Weight, Id * 71621723);
            pas::Extended cpp_left = cpp_left_2 * aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 4.0, 8.0, 0.5, 3.0);
            return System::Round(cpp_left + Weight);
        }()));
        Cost = System::Round(([&] {
            pas::Extended cpp_left_3 = ([&] {
                pas::WideString lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"UselessItems.", ConfigBlockName, u".Cost"}));
                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                return aGalaxy::TGalaxy::ResolveMoneySizeTag(std::move(lookupLocalizedTextByKey_2), 2);
            }());
            return cpp_left_3 * aMyFunction::SeededRandomFloatRange(Id * 13567157, 0.5, 1.2);
        }()));
        {
            pas::Extended cpp_left_7 = aMyFunction::SeededRandomIntRange(150, 200, Id * 13567157);
            pas::Extended cpp_left_6 = cpp_left_7 * aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 4.0, 8.0, 1.0, 3.0);
            pas::Extended cpp_left_5 = cpp_left_6 * aMyFunction::RemapClamped(Weight, 1.0E+1, 1.0E+2, 1.0, 6.0);
            pas::Extended cpp_left_4 = cpp_left_5 * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].ArcadeRewardScale;
            Cost = aMyFunction::RoundAndTruncateToTens(cpp_left_4 + Cost);
        }
        Repair();
        CustomText = pas::WideString();
        Data[0] = 0;
        Data[1] = 0;
        Data[2] = 0;
    }

    void TUselessItem::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddWideStringZ(CustomText);
        Buffer->AddIntegerValue(Data[0]);
        Buffer->AddIntegerValue(Data[1]);
        Buffer->AddIntegerValue(Data[2]);
    }

    void TUselessItem::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion < 86) {
            ConfigBlockName = Buffer->ReadWideString();
        }
        if (GlobalsV::LoadedSaveVersion >= 76) {
            CustomText = Buffer->ReadWideString();
            Data[0] = EC_Buf::TBufEC_GetInt32(Buffer);
            Data[1] = EC_Buf::TBufEC_GetInt32(Buffer);
            Data[2] = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            CustomText = pas::WideString();
            Data[0] = 0;
            Data[1] = 0;
            Data[2] = 0;
        }
        CheckIfWeDisplayAsArtefact();
    }

    void TUselessItem::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        Block->AddParam(EC_Str::DecodeTextW(u"SoyIsaNoarmoed"_w), ConfigBlockName);
    }

    void TUselessItem_LoadFromBlock(TUselessItem* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->ConfigBlockName = Block->GetParam(EC_Str::DecodeTextW(u"SoyIsaNoarmoed"_w));
        Self->CheckIfWeDisplayAsArtefact();
    }

    pas::WideString TUselessItem::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        } else if (GR_Main::LanguageDataConfig->GetBlock(u"UselessItems"_wref.get())->CountBlocks(ConfigBlockName) <= 0) {
            return pas::WideString();
        } else {
            return aConst::LocalizedText(pas::concat_wide({u"UselessItems.", ConfigBlockName, u".Name"}));
        }
    }

    pas::WideString TUselessItem_GetInfoText(TUselessItem* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Result{};
        Result = Self->CustomText;
        if (Result == u"") {
            if (GR_Main::LanguageDataConfig->GetBlock(u"UselessItems"_wref.get())->CountBlocks(Self->ConfigBlockName) > 0) {
                Result = aConst::LocalizedText(pas::concat_wide({u"UselessItems.", Self->ConfigBlockName, u".Text"}));
            }
        }
        aMyFunction::ReplaceTextToken(Result, u"<Data1>"_w, pas::wide_int_to_str(Self->Data[0]), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<Data2>"_w, pas::wide_int_to_str(Self->Data[1]), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<Data3>"_w, pas::wide_int_to_str(Self->Data[2]), ColorTag);
        return Result;
    }

    pas::WideString TUselessItem::GetDescriptionText() {
        if (GR_Main::LanguageDataConfig->GetBlock(u"UselessItems"_wref.get())->CountBlocks(ConfigBlockName) <= 0) {
            return pas::WideString();
        }
        return aConst::LocalizedText(pas::concat_wide({u"UselessItems.", ConfigBlockName, u".Description"}));
    }

    pas::WideString TUselessItem::GetBitmapResourceName() {
        pas::WideString Result{};
        if (pas::pos(u"Remains", ConfigBlockName) > 0) {
            Result = pas::concat_wide({u"Bm.ItemsUseless.", GR_Main::GiResourceSuffix(), ConfigBlockName, u"_", pas::wide_int_to_str(static_cast<std::int32_t>(DominatorSeries)), u"_"});
        } else if (pas::pos(u"Mimic", ConfigBlockName) > 0) {
            Result = pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), EC_Str::CopyWideStringUnchecked(ConfigBlockName, 6, ConfigBlockName.length() - 5), u"_"});
        } else {
            Result = pas::concat_wide({u"Bm.ItemsUseless.", GR_Main::GiResourceSuffix(), ConfigBlockName, u"_"});
        }
        if (!GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({Result, u"s"}))) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Can not find image for useless item ", ConfigBlockName, u" changing to Usl_FishCont"})));
            return pas::concat_wide({u"Bm.ItemsUseless.", GR_Main::GiResourceSuffix(), u"Usl_FishCont_"});
        }
        return Result;
    }

    // Owner is Dominator and ConfigBlockName starts with Remains_.
    std::uint8_t TUselessItem::IsDominatorRemains() {
        return OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && pas::pos(u"Remains_", ConfigBlockName) == 1;
    }

    void TUselessItem::CheckIfWeDisplayAsArtefact() {
        std::int32_t I{};
        DisplayAsArtefact = false;
        {
            const std::int32_t cpp_last = aConst::UselessItemLootPools[3].length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (aConst::UselessItemLootPools[3][I] == ConfigBlockName) {
                        DisplayAsArtefact = true;
                        break;
                    }
                }
            }
        }
    }

    pas::WideString TUselessItem::GetOnUseCodeText() {
        pas::WideString Result{};
        EC_BlockPar::TBlockParEC* Block = GR_Main::LanguageDataConfig->GetBlock(u"UselessItems"_wref.get())->FindBlock(ConfigBlockName);
        if (Block != nullptr) {
            Block = Block->FindBlock(u"OnUseCode"_wref.get());
            if (Block != nullptr) {
                return Block->ConcatenateValues();
            }
        }
        return Result;
    }

    void* TUselessItem::GetActionCode() {
        if (ActCodeInitialized) {
            return ActionCode;
        }
        ActCodeInitialized = true;
        void* Result = nullptr;
        EC_BlockPar::TBlockParEC* Config = GR_Main::LanguageDataConfig->GetBlock(u"UselessItems"_wref.get())->FindBlock(ConfigBlockName);
        if (Config != nullptr) {
            ActionCode = aScript::GetCachedActionCode(aScript::UselessItemScriptCache, ConfigBlockName, Config);
            return ActionCode;
        }
        return Result;
    }

    void TCistern::Init(std::int32_t Fuel, std::uint8_t Capacity, std::uint8_t Owner) {
        ItemType = aConst::t_Cistern;
        this->Capacity = Capacity;
        Weight = Capacity;
        this->Fuel = std::min<std::int32_t>(static_cast<std::int32_t>(this->Capacity), Fuel);
        Cost = 10 * Capacity;
        OwnerId = Owner;
        EquippedFlag = 0;
        Repair();
    }

    void TCistern::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(Capacity);
        Buffer->AddIntegerValue(Fuel);
    }

    void TCistern::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        Capacity = EC_Buf::TBufEC_GetByte(Buffer);
        Fuel = EC_Buf::TBufEC_GetInt32(Buffer);
    }

    void TCistern::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Fuel);
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"FiuNeol"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(Capacity));
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"CraspiaNcliotay"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
    }

    void TCistern_LoadFromBlock(TCistern* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->Fuel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"FiuNeol"_w))));
        Self->Capacity = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"CraspiaNcliotay"_w))));
    }

    pas::WideString TCistern::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return aConst::LocalizedText(u"Items.Cistern.Name"_wref.get());
    }

    pas::WideString TCistern_GetInfoText(TCistern* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Result{};
        Result = aConst::LocalizedText(u"Items.Cistern.Text"_wref.get());
        aMyFunction::ReplaceTextToken(Result, u"<Fuel>"_w, pas::wide_int_to_str(Self->Fuel), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<Capacity>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Self->Capacity)), ColorTag);
        return Result;
    }

    pas::WideString TCistern::GetDescriptionText() {
        return aConst::LocalizedText(u"Items.Cistern.Description"_wref.get());
    }

    pas::WideString TCistern::GetBitmapResourceName() {
        if (ConfigBlockName != u"") {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName});
        }
        return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Cistern_"});
    }

    // Clears deployment state.
    void TSatellite::InitGenerated(std::uint8_t TypeId, std::uint8_t Owner, std::uint32_t Seed) {
        pas::WideString SpeedText{};
        ItemType = aConst::t_Satellite;
        SatelliteTypeId = TypeId;
        OwnerId = Owner;
        TargetPlanet = nullptr;
        TrajectoryIndex = 0;
        SpeedText = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Satellite.", SysUtils::IntToStr(SatelliteTypeId), ".Speed"})));
        WaterExplorationRate = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(SpeedText, 0, u","_wref.get())));
        LandExplorationRate = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(SpeedText, 1, u","_wref.get())));
        HillExplorationRate = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(SpeedText, 2, u","_wref.get())));
        float Roll = aMyFunction::NextRandomUnitFloat(Seed);
        if (Roll < 0.2L) {
            ++WaterExplorationRate;
        } else if (Roll < 0.4L) {
            ++LandExplorationRate;
        } else if (Roll < 0.6L) {
            ++HillExplorationRate;
        }
        EquippedFlag = 0;
        Weight = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Items.Satellite.", SysUtils::IntToStr(SatelliteTypeId), ".Size"})))));
        {
            pas::Extended cpp_left_2 = aMyFunction::NextRandomIntRange(0, Weight, Seed);
            pas::Extended cpp_left = cpp_left_2 * aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 4.0, 8.0, 1.0, 1.5);
            Weight = System::Round(cpp_left + Weight / 2);
        }
        Cost = System::Round(([&] {
            pas::Extended cpp_left_3 = ([&] {
                pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Items.Satellite.", SysUtils::IntToStr(SatelliteTypeId), ".Cost"})));
                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                return aGalaxy::TGalaxy::ResolveMoneySizeTag(std::move(lookupLocalizedTextByKey), 2);
            }());
            return cpp_left_3 * aMyFunction::NextRandomFloatRange(1.0, 2.0, Seed);
        }()));
        {
            pas::Extended cpp_left_7 = aMyFunction::NextRandomIntRange(150, 200, Seed);
            pas::Extended cpp_left_6 = cpp_left_7 * aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 4.0, 8.0, 1.0, 3.0);
            pas::Extended cpp_left_5 = cpp_left_6 * aMyFunction::RemapClamped(Weight, 1.0E+1, 5.0E+1, 2.0, 1.0);
            pas::Extended cpp_left_4 = pas::real_divide(cpp_left_5, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestMoneyFactor);
            Cost = aMyFunction::RoundAndTruncateToTens(cpp_left_4 + Cost);
        }
        std::uint8_t WearLevel = aConst::SizeTagToLevel(GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Items.Satellite.", SysUtils::IntToStr(SatelliteTypeId), ".Wear"}))));
        WearPerTurn = aConst::GenerateValueForSizeLevel(WearLevel, 1, 10, 30, Seed * 0xe73fe205u) * 0.1L;
        Repair();
    }

    void TSatellite::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(SatelliteTypeId);
        Buffer->AddIntegerValue(TrajectoryIndex);
        if (TargetPlanet == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(TargetPlanet))->Id);
        }
        Buffer->AddAnsiChar(WaterExplorationRate);
        Buffer->AddAnsiChar(LandExplorationRate);
        Buffer->AddAnsiChar(HillExplorationRate);
        Buffer->AddSingle(WearPerTurn);
    }

    void TSatellite::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        SatelliteTypeId = EC_Buf::TBufEC_GetByte(Buffer);
        TrajectoryIndex = EC_Buf::TBufEC_GetInt32(Buffer);
        TargetPlanet = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        WaterExplorationRate = EC_Buf::TBufEC_GetByte(Buffer);
        LandExplorationRate = EC_Buf::TBufEC_GetByte(Buffer);
        HillExplorationRate = EC_Buf::TBufEC_GetByte(Buffer);
        WearPerTurn = EC_Buf::TBufEC_GetSingle(Buffer);
    }

    void TSatellite::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(SatelliteTypeId));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"ToyIprey"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(WaterExplorationRate));
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"WuartTewrf"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(LandExplorationRate));
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"LLagnsd3"_w);
            Block->AddParam(decodeTextW_3, intToStr_3);
        }
        {
            const pas::WideString& intToStr_4 = pas::wide_int_to_str(static_cast<std::int32_t>(HillExplorationRate));
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"HbiFldle"_w);
            Block->AddParam(decodeTextW_4, intToStr_4);
        }
        {
            const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(WearPerTurn), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"WoeIamrr"_w);
            Block->AddParam(decodeTextW_5, cpp_arg);
        }
    }

    void TSatellite_LoadFromBlock(TSatellite* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->SatelliteTypeId = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"ToyIprey"_w))));
        Self->WaterExplorationRate = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"WuartTewrf"_w))));
        Self->LandExplorationRate = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"LLagnsd3"_w))));
        Self->HillExplorationRate = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"HbiFldle"_w))));
        Self->WearPerTurn = EC_Str::ExtractDecimalToSingleW(Block->GetParam(EC_Str::DecodeTextW(u"WoeIamrr"_w)));
    }

    void TSatellite::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aItem::TItem::ResolveLoadedReferences(Galaxy);
        TargetPlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(TargetPlanet)), true)));
    }

    pas::WideString TSatellite::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return pas::concat_wide({GR_Main::LookupLocalizedTextByKey(u"Items.Satellite.Name"_wref.get()), u" ", aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Satellite.", SysUtils::IntToStr(SatelliteTypeId), ".Name"}))), u"-", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 100 + 1))});
    }

    pas::WideString TSatellite_GetInfoText(TSatellite* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Result{};
        Result = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Satellite.", SysUtils::IntToStr(Self->SatelliteTypeId), ".Text"})));
        if (Self->WaterExplorationRate > 0) {
            aMyFunction::ReplaceTextToken(Result, u"<Water>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Self->WaterExplorationRate)), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Result, u"<Water>"_w, u"-"_w, pas::WideString());
        }
        if (Self->LandExplorationRate > 0) {
            aMyFunction::ReplaceTextToken(Result, u"<Land>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Self->LandExplorationRate)), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Result, u"<Land>"_w, u"-"_w, pas::WideString());
        }
        if (Self->HillExplorationRate > 0) {
            aMyFunction::ReplaceTextToken(Result, u"<Hill>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Self->HillExplorationRate)), ColorTag);
        } else {
            aMyFunction::ReplaceTextToken(Result, u"<Hill>"_w, u"-"_w, pas::WideString());
        }
        return pas::concat_wide_reverse({Self->GetConditionText(true), Result});
    }

    pas::WideString TSatellite::GetBrokenInUseText_2() {
        pas::WideString Result{};
        pas::WideString PlanetName{};
        Result = aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".BrokenInUse"}));
        if (TargetPlanet != nullptr) {
            PlanetName = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(TargetPlanet))->Name;
        } else {
            PlanetName = pas::WideString();
        }
        aMyFunction::ReplaceTextToken(Result, u"<Name>"_w, GetDisplayName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Result, u"<Planet>"_w, PlanetName, u"<color=255,240,100>"_w);
        return Result;
    }

    pas::WideString TSatellite::GetIdleInfoText() {
        pas::WideString Result{};
        pas::WideString PlanetName{};
        Result = aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[ItemType], u".IdleInfo"}));
        if (TargetPlanet != nullptr) {
            PlanetName = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(TargetPlanet))->Name;
        } else {
            PlanetName = pas::WideString();
        }
        aMyFunction::ReplaceTextToken(Result, u"<Name>"_w, GetDisplayName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Result, u"<Size>"_w, pas::wide_int_to_str(Weight), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Result, u"<Planet>"_w, PlanetName, u"<color=255,240,100>"_w);
        return Result;
    }

    pas::WideString TSatellite::GetDescriptionText() {
        return aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Satellite.", SysUtils::IntToStr(SatelliteTypeId), ".Description"})));
    }

    pas::WideString TSatellite::GetBitmapResourceName() {
        if (ConfigBlockName != u"") {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName});
        }
        return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), u"Satellite", pas::wide_int_to_str(static_cast<std::int32_t>(SatelliteTypeId)), u"_"});
    }

    void TTreasureMap::Init(void* Planet, void* Victim) {
        ItemType = aConst::t_TreasureMap;
        Weight = 1;
        Cost = 1;
        OwnerId = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Victim))->OwnerId;
        if (pas::class_cast_if<aPirate::TPirate*>(static_cast<pas::Object*>(Victim)) != nullptr) {
            SourceShipName = pas::checked_cast<aPirate::TPirate*>(static_cast<pas::Object*>(Victim))->GetFullName(u" "_wref.get());
        } else {
            SourceShipName = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Victim))->GetFullName(u" "_wref.get());
        }
        TargetPlanet = Planet;
        PreviewTablePage1 = TTreasureMap::BuildPreviewTable(1, Planet);
        PreviewTablePage2 = TTreasureMap::BuildPreviewTable(2, Planet);
        EquippedFlag = 0;
        Repair();
    }

    void TTreasureMap::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        if (TargetPlanet == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(TargetPlanet))->Id);
        }
        Buffer->AddWideStringZ(SourceShipName);
        Buffer->AddWideStringZ(PreviewTablePage1);
        Buffer->AddWideStringZ(PreviewTablePage2);
    }

    void TTreasureMap::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        TargetPlanet = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        SourceShipName = Buffer->ReadWideString();
        PreviewTablePage1 = Buffer->ReadWideString();
        PreviewTablePage2 = Buffer->ReadWideString();
    }

    void TTreasureMap::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aItem::TItem::ResolveLoadedReferences(Galaxy);
        TargetPlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(TargetPlanet)), true)));
    }

    pas::WideString TTreasureMap::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return aConst::LocalizedText(u"Items.TreasureMap.Name"_wref.get());
    }

    pas::WideString TTreasureMap_GetInfoText(TTreasureMap* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Result{};
        Result = pas::concat_wide({aConst::LocalizedText(u"Items.TreasureMap.Text"_wref.get()), u" ", aConst::LocalizedText(u"Items.TreasureMap.Hint"_wref.get())});
        aMyFunction::ReplaceTextToken(Result, u"<Planet>"_w, Self->GetTargetPlanetName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Result, u"<Ship>"_w, Self->SourceShipName, u"<color=255,240,100>"_w);
        return Result;
    }

    pas::WideString TTreasureMap::GetDescriptionText() {
        return aConst::LocalizedText(u"Items.TreasureMap.Description"_wref.get());
    }

    pas::WideString TTreasureMap::GetBitmapResourceName() {
        std::int32_t Kind{};
        if (pas::in_range(OwnerId, static_cast<std::int32_t>(aGalaxyStruct::oiMaloc), static_cast<std::int32_t>(aGalaxyStruct::oiHuman))) {
            Kind = 1;
        } else {
            Kind = 2;
        }
        return pas::concat_wide({u"Bm.ItemsUseless.", GR_Main::GiResourceSuffix(), u"TreasureMap", pas::wide_int_to_str(Kind), u"_"});
    }

    pas::WideString TTreasureMap::GetTargetPlanetName() {
        return pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(TargetPlanet))->Name;
    }

    // PageIndex is 1 or 2; Planet must be assigned.
    pas::WideString TTreasureMap::BuildPreviewTable(std::int32_t PageIndex, void* Planet) {
        pas::WideString Result{};
        std::int32_t I{};
        std::int32_t Number{};
        aPlanet::PPlanetSurfaceLootEntry Entry{};
        pas::WideString Rows{};
        pas::WideString Header{};
        pas::WideString Caption{};
        pas::WideString Rule{};
        aPlanet::TPlanet* World{};
        // Nested helper of BuildPreviewTable; caller removes the unused static link.
        auto ItemColorTag = [&](TItem* Item) -> pas::WideString {
            if (pas::class_cast_if<TGoods*>(Item) != nullptr) {
                return u"<color=127,127,127>"_w;
            } else if (pas::class_cast_if<TArtefact*>(Item) != nullptr) {
                return u"<color=255,0,0>"_w;
            } else if (pas::class_cast_if<TCistern*>(Item) != nullptr) {
                return u"<color=127,127,127>"_w;
            } else if (pas::class_cast_if<TMicroModule*>(Item) != nullptr) {
                return u"<color=255,0,255>"_w;
            } else if (pas::class_cast_if<TEquipment*>(Item) != nullptr) {
                return u"<color=254,217,7>"_w;
            } else {
                return pas::WideString();
            }
        };
        Rule = aMyFunction::WrapTextInColor(static_cast<pas::WideString>(SystemImports::StringOfChar('-', TreasureMapRuleLengths[PageIndex])), u"<color=127,127,127>"_w);
        Caption = ([&] {
            auto name = pas::borrow(pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Planet))->Name);
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormGS.PlanetInfo"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Planet>"_w, name.get());
        }());
        Header = pas::concat_wide({Header, u"\r\n", u"<td=", pas::wide_int_to_str(0), u">", u"<align=left>", Caption, u"</align>"});
        Header = pas::concat_wide({Header, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][3] / 2), u">", u"<align=center>", aMyFunction::WrapTextInColor(aConst::LocalizedText(u"Items.TreasureMap.Name"_wref.get()), u"<color=0,255,0>"_w), u"</align>"});
        Caption = ([&] {
            auto name_2 = pas::borrow(pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Planet))->CurrentStar->Name);
            pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormGS.StarInfo"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Star>"_w, name_2.get());
        }());
        Header = pas::concat_wide({Header, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][3]), u">", u"<align=right>", aMyFunction::WrapTextInColor(Caption, pas::WideString()), u"</align>"});
        Header = pas::concat_wide({Header, u"\r\n", Rule, u"\r\n"});
        Header = pas::concat_wide({Header, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][0]), u">", u"<align=right>", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormGS.ColumnNumber"_wref.get()), u"<color=255,240,100>"_w), u"</align>"});
        Header = pas::concat_wide({Header, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][1]), u">", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormGS.ColumnName"_wref.get()), u"<color=255,240,100>"_w)});
        Header = pas::concat_wide({Header, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][2]), u">", u"<align=right>", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormShip.StorageInfo.Size"_wref.get()), u"<color=255,240,100>"_w), u"</align>"});
        Header = pas::concat_wide({Header, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][3]), u">", u"<align=right>", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormShip.StorageInfo.Cost"_wref.get()), u"<color=255,240,100>"_w), u"</align>"});
        Header = pas::concat_wide({Header, u"\r\n", Rule});
        if (Planet != nullptr) {
            if (pas::class_cast_if<aPlanet::TPlanet*>(static_cast<pas::Object*>(Planet)) != nullptr) {
                Rows = pas::WideString();
                Number = 1;
                World = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Planet));
                if (World->SurfaceLootEntries != nullptr) {
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(World->SurfaceLootEntries) - 1); cpp_range.next(I); ) {
                        Entry = pas::list_at<aPlanet::TPlanetSurfaceLootEntry>(World->SurfaceLootEntries, I);
                        if (Entry->Item != nullptr) {
                            if ((static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessSurfaceLootItem(Entry->Item)) && !(pas::class_cast_if<TGoods*>(Entry->Item) != nullptr) && !(pas::class_cast_if<TCistern*>(Entry->Item) != nullptr)) {
                                Rows = pas::concat_wide({Rows, u"\r\n"});
                                Rows = pas::concat_wide({Rows, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][0]), u">", u"<align=right>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Number), pas::WideString()), u"</align>"});
                                Rows = pas::concat_wide({Rows, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][1]), u">", u"", ([&] {
                                    pas::WideString itemColorTag = ItemColorTag(Entry->Item);
                                    pas::WideString displayName = Entry->Item->GetDisplayName();
                                    return aMyFunction::WrapTextInColor(std::move(displayName), std::move(itemColorTag));
                                }()), u""});
                                Rows = pas::concat_wide({Rows, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][2]), u">", u"<align=right>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Entry->Item->Weight), u"<color=0,255,0>"_w), u"</align>"});
                                Rows = pas::concat_wide({Rows, u"<td=", pas::wide_int_to_str(TreasureMapColumnPositions[PageIndex][3]), u">", u"<align=right>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Entry->Item->Cost), u"<color=0,255,255>"_w), u"</align>"});
                                ++Number;
                            }
                        }
                    }
                    return pas::concat_wide({Header, Rows});
                }
            }
        }
        return Result;
    }

    // ModuleIndex is zero-based and must identify an existing template.
    void TMicroModule::Init(std::int32_t ModuleIndex) {
        ItemType = aConst::t_MicroModule;
        OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited);
        MicroModuleIndex = ModuleIndex + 1;
        Repair();
        Weight = 1;
        Cost = aMyFunction::RoundAndTruncateToTens(([&] {
            pas::Extended cpp_right = aMyFunction::SeededRandomFloatRange(Id * 1367, 0.5, 1.2);
            return pas::real_divide(100 * aGalaxy::Galaxy->ComputeScaledSmallMoney(2), aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority + 20) * cpp_right;
        }()));
        {
            pas::Extended cpp_left_3 = aMyFunction::SeededRandomIntRange(150, 200, Id * 13567157);
            pas::Extended cpp_left_2 = cpp_left_3 * aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 4.0, 8.0, 1.0, 3.0);
            pas::Extended cpp_left = cpp_left_2 * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestMoneyFactor;
            Cost = aMyFunction::RoundAndTruncateToTens(cpp_left + Cost);
        }
    }

    void TMicroModule::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
    }

    void TMicroModule::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion < 113) {
            EC_Buf::TBufEC_GetInt32(Buffer);
        }
        if (MicroModuleIndex == 0) {
            MicroModuleIndex = 1;
        }
    }

    pas::WideString TMicroModule::GetDisplayName() {
        pas::WideString Result{};
        pas::WideString Text{};
        if (NameOverride != u"") {
            return NameOverride;
        }
        Result = aConst::MicroModuleTemplates[MicroModuleIndex - 1].NamePrefix;
        if (Result == u"") {
            Result = aMyFunction::WrapTextInColor(GR_Main::LookupLocalizedTextOrEmpty(u"MicroModuls.Name"_wref.get()), pas::WideString());
        }
        Text = aConst::MicroModuleTemplates[MicroModuleIndex - 1].Name;
        if (EC_Str::FindTextOffsetW(Text, u"\""_wref.get(), 0) < 0) {
            Text = pas::concat_wide({u"\"", Text, u"\""});
        }
        return pas::concat_wide({Result, u" ", aMyFunction::WrapTextInColor(Text, aItem::GetMicroModuleNameColorTag(MicroModuleIndex - 1))});
    }

    pas::WideString TMicroModule::GetPlainName() {
        pas::WideString Result{};
        pas::WideString Text{};
        if (NameOverride != u"") {
            return NameOverride;
        }
        Result = aConst::MicroModuleTemplates[MicroModuleIndex - 1].NamePrefix;
        if (Result == u"") {
            Result = aMyFunction::WrapTextInColor(GR_Main::LookupLocalizedTextOrEmpty(u"MicroModuls.Name"_wref.get()), pas::WideString());
        }
        Text = aConst::MicroModuleTemplates[MicroModuleIndex - 1].Name;
        if (EC_Str::FindTextOffsetW(Text, u"\""_wref.get(), 0) < 0) {
            Text = pas::concat_wide({u"\"", Text, u"\""});
        }
        return pas::concat_wide({Result, u" ", Text});
    }

    pas::WideString TMicroModule_GetInfoText(TMicroModule* Self, pas::WideString ColorTag, void* Ship) {
        return aItem::GetMicroModuleInfoText(Self->MicroModuleIndex - 1, ColorTag);
    }

    pas::WideString TMicroModule::GetDescriptionText() {
        return pas::WideString();
    }

    pas::WideString TMicroModule::GetBitmapResourceName() {
        if (ConfigBlockName != u"") {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName});
        }
        return aItem::GetMicroModuleBitmapResourceName(MicroModuleIndex - 1);
    }

    // Node refund at the current ranger center, using priority and docked station ID. Priorities 31..69 are capped by half LowPriorityOfferCost; 70..100 by half MediumPriorityOfferCost. Minimum 5 nodes.
    std::int32_t TMicroModule::CalculateNodeExchangeValue(std::int32_t LowPriorityOfferCost, std::int32_t MediumPriorityOfferCost) {
        std::int32_t Result = 0;
        if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority >= 0 && aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority <= 30) {
            Result = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
            {
                std::int32_t round = System::Round(Result * 1.2L);
                std::int32_t round_2 = System::Round(Result * 0.8L);
                Result = aMyFunction::SeededRandomIntRange(round_2, round, Result + aPlayer::GetPlayer()->DockedTo->Id);
            }
            Result = aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(Result, 1.5L));
        }
        if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority >= 31 && aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority <= 69) {
            Result = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
            {
                std::int32_t round_3 = System::Round(Result * 1.2L);
                std::int32_t round_4 = System::Round(Result * 0.8L);
                Result = aMyFunction::SeededRandomIntRange(round_4, round_3, Result + aPlayer::GetPlayer()->DockedTo->Id);
            }
            Result = aMyFunction::RoundAndTruncateToHundreds(pas::real_min<pas::Extended>(static_cast<pas::Extended>(LowPriorityOfferCost / 2), pas::real_divide(Result, 1.5L)));
        }
        if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority >= 70 && aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority <= 100) {
            Result = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[MicroModuleIndex - 1].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
            {
                std::int32_t round_5 = System::Round(Result * 1.2L);
                std::int32_t round_6 = System::Round(Result * 0.8L);
                Result = aMyFunction::SeededRandomIntRange(round_6, round_5, Result + aPlayer::GetPlayer()->DockedTo->Id);
            }
            Result = aMyFunction::RoundAndTruncateToTens(pas::real_min<pas::Extended>(static_cast<pas::Extended>(MediumPriorityOfferCost / 2), pas::real_divide(Result, 1.5L)));
        }
        return std::max<std::int64_t>(static_cast<std::int64_t>(5), System::Round(Result * 0.3L));
    }

    // Template name wrapped in the standard yellow highlight color.
    pas::WideString TMicroModule::GetHighlightedName() {
        return aMyFunction::WrapTextInColor(aConst::MicroModuleTemplates[MicroModuleIndex - 1].Name, u"<color=255,240,100>"_w);
    }

    // Uses this micromodule item's template and checks slot blockers and equipment compatibility.
    std::uint8_t TMicroModule::CanInstallOn(aItem::TEquipment* Item) {
        std::uint8_t Result = false;
        if (Item->MicroModuleIndex != 0) {
            return Result;
        }
        if (Item->SpecialModuleIndex != 0 && aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].BlocksMicroModuleSlot) {
            return Result;
        }
        if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinEff * sizeof(std::int32_t))) != 0 && Item->SpecialModuleIndex == 0) {
            return Result;
        }
        if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinPenalty * sizeof(std::int32_t))) != 0 && Item->SpecialModuleIndex == 0) {
            return Result;
        }
        if (pas::class_cast_if<TWeapon*>(Item) != nullptr) {
            return aItem::IsBonusCompatibleWithWeapon(MicroModuleIndex - 1, reinterpret_cast<TWeapon*>(Item));
        } else if (pas::class_cast_if<THull*>(Item) != nullptr) {
            return aItem::IsBonusCompatibleWithHull(MicroModuleIndex - 1, reinterpret_cast<THull*>(Item));
        } else {
            return aItem::IsBonusCompatibleWithEquipment(MicroModuleIndex - 1, Item);
        }
    }

    void TArtefact_Create(TArtefact* Self) {
        aItem::TEquipmentWithActCode_Create(Self);
        Self->DisplayAsArtefact = true;
        Self->Repair();
    }

    void TArtefact_Destroy(TArtefact* Self) {
        if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
            aScript::RunItemConfigActionCode(Self, aConst::satOnItemDestroy, nullptr, nullptr, nullptr, 0);
        }
        aItem::TEquipmentWithActCode_Destroy(Self);
    }

    void TArtefact::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TEquipment::LoadFromBuffer(Buffer, Galaxy);
        if (!pas::in_set<aConst::t_Artefact, aConst::t_Artefact, aConst::t_ArtefactHull, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtBio, aConst::t_ArtFastRacks>(ItemType)) {
            Repair();
        }
    }

    void TArtefact::Init(std::uint8_t Owner, aConst::TItemType ItemType) {
        float MinWeightScale{};
        float MaxWeightScale{};
        float MinCostScale{};
        float MaxCostScale{};
        std::int32_t MinExtraWeight{};
        std::int32_t MaxExtraWeight{};
        std::int32_t MinCost{};
        std::int32_t MaxCost{};
        this->ItemType = ItemType;
        OwnerId = Owner;
        if (!pas::in_range(this->ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
            Weight = aConst::GetAverageItemSize(this->ItemType);
        }
        switch (ItemType) {
            case aConst::t_ArtefactHull: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 3000;
                break;
            }
            case aConst::t_ArtefactFuel: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 3000;
                break;
            }
            case aConst::t_ArtefactSpeed: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 2000;
                MaxCost = 3000;
                break;
            }
            case aConst::t_ArtefactPower: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1000;
                MaxCost = 2000;
                break;
            }
            case aConst::t_ArtefactRadar: {
                MinExtraWeight = 1;
                MaxExtraWeight = 3;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1000;
                MaxCost = 1500;
                break;
            }
            case aConst::t_ArtefactScaner: {
                MinExtraWeight = 1;
                MaxExtraWeight = 2;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 500;
                MaxCost = 1000;
                break;
            }
            case aConst::t_ArtefactDroid: {
                MinExtraWeight = 1;
                MaxExtraWeight = 4;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 3000;
                break;
            }
            case aConst::t_ArtefactNano: {
                MinExtraWeight = 1;
                MaxExtraWeight = 4;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 2.0f;
                MinCost = 2000;
                MaxCost = 4000;
                break;
            }
            case aConst::t_ArtefactHook: {
                MinExtraWeight = 1;
                MaxExtraWeight = 3;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 500;
                MaxCost = 1000;
                break;
            }
            case aConst::t_ArtefactDef: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtefactAnalyzer: {
                MinExtraWeight = 1;
                MaxExtraWeight = 2;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 2.0f;
                MinCost = 500;
                MaxCost = 1000;
                break;
            }
            case aConst::t_ArtefactMiniExpl: {
                MinExtraWeight = 1;
                MaxExtraWeight = 3;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 2.0f;
                MinCost = 1500;
                MaxCost = 3000;
                break;
            }
            case aConst::t_ArtefactAntigrav: {
                MinExtraWeight = 1;
                MaxExtraWeight = 3;
                MinWeightScale = 1.0f;
                MaxWeightScale = 3.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1000;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtefactTransmitter: {
                MinExtraWeight = 1;
                MaxExtraWeight = 2;
                MinWeightScale = 1.0f;
                MaxWeightScale = 3.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 500;
                MaxCost = 1500;
                break;
            }
            case aConst::t_ArtefactBomb: {
                MinExtraWeight = 1;
                MaxExtraWeight = 2;
                MinWeightScale = 1.0f;
                MaxWeightScale = 6.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 2.0f;
                MinCost = 1000;
                MaxCost = 2000;
                break;
            }
            case aConst::t_ArtefactTranclucator: {
                MinExtraWeight = 1;
                MaxExtraWeight = 3;
                MinWeightScale = 1.0f;
                MaxWeightScale = 4.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1000;
                MaxCost = 2000;
                break;
            }
            case aConst::t_ArtDefToEnergy: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtEnergyPulse: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtEnergyDef: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtSplinter: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtDecelerate: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtMissileDef: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtForsage: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtWeaponToSpeed: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtGiperJump: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtBlackHole: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtDefToArms1: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtDefToArms2: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtArtefactor: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtBio: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtPDTurret: {
                MinExtraWeight = 1;
                MaxExtraWeight = 9;
                MinWeightScale = 1.0f;
                MaxWeightScale = 4.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            case aConst::t_ArtFastRacks: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
            default: {
                MinExtraWeight = 1;
                MaxExtraWeight = 5;
                MinWeightScale = 1.0f;
                MaxWeightScale = 2.0f;
                MinCostScale = 1.0f;
                MaxCostScale = 3.0f;
                MinCost = 1500;
                MaxCost = 2500;
                break;
            }
        }
        if (aPlayer::GetPlayer() != nullptr) {
            Weight += ([&] {
                pas::Extended cpp_left = aMyFunction::SeededRandomIntRange(MinExtraWeight, MaxExtraWeight, Id * 317847);
                return System::Round(cpp_left * aMyFunction::RemapClamped(aPlayer::GetPlayer()->GetHull()->Weight, static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5] * 1.5L, static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[1], MinWeightScale, MaxWeightScale));
            }());
        }
        {
            pas::Extended cpp_left_2 = aMyFunction::SeededRandomIntRange(MinCost, MaxCost, Id + 135671);
            Cost = aMyFunction::RoundAndTruncateToTens(cpp_left_2 * aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 4.0, 8.0, MinCostScale, MaxCostScale));
        }
        Weight = System::Round(static_cast<long double>(Weight) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[4]].QuestTimeAndExperienceFactor);
        Cost = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(Cost) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[1]].QuestTimeAndExperienceFactor);
    }

    pas::WideString TArtefact::GetBitmapResourceName() {
        if (pas::class_cast_if<TArtefactCustom*>(this) != nullptr) {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName, u"_"});
        } else if (ConfigBlockName != u"") {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), ConfigBlockName});
        } else {
            return pas::concat_wide({u"Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ItemType], u"_"});
        }
    }

    pas::WideString TArtefact::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return aConst::LocalizedText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[ItemType], u".Name"}));
    }

    pas::WideString TArtefact_GetInfoText(TArtefact* Self, pas::WideString ColorTag, void* Ship) {
        if (Self->ConfigBlockName != u"" && GR_Main::LanguageDataConfig->GetBlock(u"Artefacts"_wref.get())->CountBlocks(Self->ConfigBlockName) > 0) {
            return pas::concat_wide({aConst::LocalizedColorText(pas::concat_wide({u"Artefacts.", Self->ConfigBlockName, u".Text"})), aItem::TEquipment_GetBonusDescription(Self, ColorTag), Self->GetConditionText(true), Self->GetBoostStatusText()});
        }
        return pas::concat_wide({aConst::LocalizedColorText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[Self->ItemType], u".Text"})), aItem::TEquipment_GetBonusDescription(Self, ColorTag), Self->GetConditionText(true), Self->GetBoostStatusText()});
    }

    pas::WideString TArtefact::GetDescriptionText() {
        return aConst::LocalizedColorText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[ItemType], u".Description"}));
    }

    // Returns empty when the OnUseCode block is absent.
    pas::WideString TArtefact::GetOnUseCodeText() {
        pas::WideString Result{};
        EC_BlockPar::TBlockParEC* Block{};
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
            Block = GR_Main::LanguageDataConfig->GetBlock(u"Artefacts"_wref.get())->GetBlock(u"CustomArtefacts"_wref.get())->GetBlock(ConfigBlockName)->FindBlock(u"OnUseCode"_wref.get());
        } else {
            Block = GR_Main::LanguageDataConfig->GetBlock(u"Artefacts"_wref.get())->GetBlock(aConst::ItemTypeNames[ItemType])->FindBlock(u"OnUseCode"_wref.get());
        }
        if (Block != nullptr) {
            return Block->ConcatenateValues();
        }
        return Result;
    }

    // Borrowed cached result, possibly nil. Marks initialization before resolving the configuration.
    void* TArtefact::GetActionCode() {
        EC_BlockPar::TBlockParEC* Config{};
        if (ActCodeInitialized) {
            return ActionCode;
        }
        void* Result = nullptr;
        ActCodeInitialized = true;
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
            Config = GR_Main::LanguageDataConfig->GetBlock(u"Artefacts"_wref.get())->GetBlock(u"CustomArtefacts"_wref.get())->GetBlock(ConfigBlockName);
        } else {
            Config = GR_Main::LanguageDataConfig->GetBlock(u"Artefacts"_wref.get())->GetBlock(aConst::ItemTypeNames[ItemType]);
        }
        if (Config != nullptr) {
            if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
                ActionCode = aScript::GetCachedActionCode(aScript::ArtefactScriptCache, ConfigBlockName, Config);
            } else {
                ActionCode = aScript::GetCachedActionCode(aScript::ArtefactKindScriptCache, aConst::ItemTypeNames[ItemType], Config);
            }
            return ActionCode;
        }
        return Result;
    }

    // Custom artefacts with SharedEffect use CountsAsItemType; otherwise returns ItemType.
    aConst::TItemType TArtefact::GetEffectiveType() {
        if (pas::is_one_of<aConst::t_Artefact, aConst::t_Artefact2>(ItemType) && reinterpret_cast<TArtefactCustom*>(this)->SharedEffect) {
            return reinterpret_cast<TArtefactCustom*>(this)->CountsAsItemType;
        }
        return ItemType;
    }

    void TArtefactTransmitter::InitTransmitter(std::uint8_t Owner) {
        Init(Owner, aConst::t_ArtefactTransmitter);
        Power = aMyFunction::RoundAndTruncateToTens(aMyFunction::SeededRandomIntRange(aConst::MinTransmitterPower, aConst::AverageTransmitterPower, Id * 317321));
    }

    void TArtefactTransmitter::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddIntegerValue(Power);
    }

    void TArtefactTransmitter::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TArtefact::LoadFromBuffer(Buffer, Galaxy);
        Power = EC_Buf::TBufEC_GetInt32(Buffer);
    }

    void TArtefactTransmitter::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Power);
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Prouwseor"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
    }

    void TArtefactTransmitter_LoadFromBlock(TArtefactTransmitter* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->Power = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Prouwseor"_w))));
    }

    pas::WideString TArtefactTransmitter_GetInfoText(TArtefactTransmitter* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Result{};
        std::int32_t DisplayPower{};
        Result = pas::concat_wide({aConst::LocalizedColorText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[Self->ItemType], u".Text"})), Self->GetConditionText(true), Self->GetBoostStatusText()});
        if (Self->Power < 0) {
            DisplayPower = 0;
        } else {
            DisplayPower = Self->Power;
        }
        aMyFunction::ReplaceTextToken(Result, u"<Power>"_w, pas::wide_int_to_str(DisplayPower), ColorTag);
        return Result;
    }

    void TArtefactTranclucator_Destroy(TArtefactTranclucator* Self) {
        if (Self->Ship != nullptr) {
            pas::free(static_cast<pas::Object*>(Self->Ship));
            Self->Ship = nullptr;
        }
        aItem::TArtefact_Destroy(Self);
    }

    // Takes ownership of ExistingShip, or creates a ship when nil.
    void TArtefactTranclucator::InitTranclucator(std::uint8_t Owner, void* OwnerShip, void* ExistingShip) {
        aTranclucator::TTranclucator* Companion{};
        std::int32_t I{};
        aItem::TItem* Item{};
        Init(Owner, aConst::t_ArtefactTranclucator);
        Ship = ExistingShip;
        if (Ship == nullptr) {
            Ship = pas::construct_call<aTranclucator::TTranclucator>(aTranclucator::TTranclucator_Create);
            Companion = pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(Ship));
            Companion->Init(pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(OwnerShip)), Owner, false);
            if (OwnerShip != nullptr) {
                pas::list_delete(Companion->CurrentStar->Ships, pas::list_indexof(Companion->CurrentStar->Ships, reinterpret_cast<void*>(Companion)));
                Companion->CurrentStar = nullptr;
            }
        }
        Companion = pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(Ship));
        if (Companion->ArtefactSize > 0) {
            Weight = Companion->ArtefactSize;
        } else {
            Companion->ArtefactSize = Weight;
        }
        ConfigBlockName = Companion->ArtefactSystemName;
        OwnerId = Companion->GetHull()->OwnerId;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Companion->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Companion->Inventory, I);
            Cost += Item->Cost;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Companion->Artefacts) - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Companion->Artefacts, I);
            Cost += Item->Cost;
        }
    }

    void TArtefactTranclucator::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(Ship))->SaveToBuffer(Buffer);
    }

    void TArtefactTranclucator::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TArtefact::LoadFromBuffer(Buffer, Galaxy);
        Ship = pas::construct_call<aTranclucator::TTranclucator>(aTranclucator::TTranclucator_Create);
        pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(Ship))->LoadFromBuffer(Buffer, Galaxy);
    }

    // Always returns nil.
    aItem::TItem* TArtefactTranclucator::Clone() {
        return nullptr;
    }

    void TArtefactTranclucator::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            EC_BlockPar::TBlockParEC* addBlockByPath = Block->AddBlockByPath(pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(static_cast<aTranclucator::TTranclucator*>(Ship)->Id))), EC_Str::DecodeTextW(u"S5heifphI4d"_w)}));
            aShip::TShip* ship = static_cast<aTranclucator::TTranclucator*>(Ship);
            ship->SaveToBlock(addBlockByPath);
        }
    }

    void TArtefactTranclucator_LoadFromBlock(TArtefactTranclucator* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        {
            EC_BlockPar::TBlockParEC* blockByPath = Block->GetBlockByPath(pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(static_cast<aTranclucator::TTranclucator*>(Self->Ship)->Id))), EC_Str::DecodeTextW(u"S5heifphI4d"_w)}));
            aShip::TShip* ship = static_cast<aTranclucator::TTranclucator*>(Self->Ship);
            ship->LoadFromBlock(blockByPath);
        }
    }

    void TArtefactTranclucator::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aItem::TItem::ResolveLoadedReferences(Galaxy);
        pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(Ship))->virtual_TShip_ResolveLoadedReferences(Galaxy);
    }

    pas::WideString TArtefactTranclucator::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        } else if (Ship != nullptr) {
            if (pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->Name.length() > 0) {
                return pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->Name;
            }
            return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[ItemType], u".Name"})), u"-", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->Id)))});
        } else {
            return aConst::LocalizedText(pas::concat_wide({u"Artefacts.", aConst::ItemTypeNames[ItemType], u".Name"}));
        }
    }

    // Uses ConfigBlockName. Reloads NoWear, CountsAs, SharedUse and SharedEffect; preserves Data and TextData.
    void TArtefactCustom::LoadConfig(std::uint8_t ApplyConfiguredWeight) {
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Name{};
        aConst::TItemType Kind{};
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(pas::concat_wide({u"Artefacts.CustomArtefacts.", ConfigBlockName}));
        if (Block->CountParams(u"NoWear"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(Block->GetParam(u"NoWear"_wref.get())) != 0) {
            ItemType = aConst::t_Artefact2;
        } else {
            ItemType = aConst::t_Artefact;
        }
        if (ApplyConfiguredWeight) {
            if (Block->CountParams(u"Size"_wref.get()) <= 0) {
                Weight = 10;
            } else {
                Weight = EC_Str::ExtractDigitsToIntW(Block->GetParam(u"Size"_wref.get()));
            }
        }
        CountsAsItemType = aConst::t_Artefact;
        SharedUse = false;
        SharedEffect = false;
        if (Block->CountParams(u"CountsAs"_wref.get()) > 0) {
            Name = Block->GetParam(u"CountsAs"_wref.get());
            for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Food, aConst::t_UselessCountableItem); cpp_range.next(Kind); ) {
                if (pas::in_set<aConst::t_Artefact, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtDefToArms1, aConst::t_ArtFastRacks>(Kind) && static_cast<std::uint8_t>(pas::in_range(Kind, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2)) ^ 1) && aConst::ItemTypeNames[Kind] == Name) {
                    CountsAsItemType = Kind;
                    break;
                }
            }
            if (CountsAsItemType != aConst::t_Artefact) {
                if (Block->CountParams(u"SharedUse"_wref.get()) > 0) {
                    SharedUse = EC_Str::ExtractDigitsToIntW(Block->GetParam(u"SharedUse"_wref.get())) != 0;
                }
                if (Block->CountParams(u"SharedEffect"_wref.get()) > 0) {
                    SharedEffect = EC_Str::ExtractDigitsToIntW(Block->GetParam(u"SharedEffect"_wref.get())) != 0;
                }
            }
        }
    }

    void TArtefactCustom::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aItem::TEquipment::SaveToBuffer(Buffer);
        Buffer->AddIntegerValue(Data[1]);
        Buffer->AddIntegerValue(Data[2]);
        Buffer->AddIntegerValue(Data[3]);
        if (TextData1 == u"") {
            Buffer->AddBoolean(false);
        } else {
            Buffer->AddBoolean(true);
            Buffer->AddWideStringZ(TextData1);
        }
        if (TextData2 == u"") {
            Buffer->AddBoolean(false);
        } else {
            Buffer->AddBoolean(true);
            Buffer->AddWideStringZ(TextData2);
        }
        if (TextData3 == u"") {
            Buffer->AddBoolean(false);
        } else {
            Buffer->AddBoolean(true);
            Buffer->AddWideStringZ(TextData3);
        }
    }

    void TArtefactCustom::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aItem::TArtefact::LoadFromBuffer(Buffer, Galaxy);
        LoadConfig(false);
        Data[1] = EC_Buf::TBufEC_GetInt32(Buffer);
        Data[2] = EC_Buf::TBufEC_GetInt32(Buffer);
        Data[3] = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 96) {
            if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                TextData1 = Buffer->ReadWideString();
            }
            if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                TextData2 = Buffer->ReadWideString();
            }
            if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                TextData3 = Buffer->ReadWideString();
            }
        }
    }

    void TArtefactCustom::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Data[1]);
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(static_cast<pas::WideString>(pas::concat_ansi({"Drastuan", SysUtils::IntToStr(1)})));
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Data[2]);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(static_cast<pas::WideString>(pas::concat_ansi({"Drastuan", SysUtils::IntToStr(2)})));
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(Data[3]);
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(static_cast<pas::WideString>(pas::concat_ansi({"Drastuan", SysUtils::IntToStr(3)})));
            Block->AddParam(decodeTextW_3, intToStr_3);
        }
    }

    void TArtefactCustom_LoadFromBlock(TArtefactCustom* Self, EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(Self, Block);
        Self->Data[1] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(static_cast<pas::WideString>(pas::concat_ansi({"Drastuan", SysUtils::IntToStr(1)}))))));
        Self->Data[2] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(static_cast<pas::WideString>(pas::concat_ansi({"Drastuan", SysUtils::IntToStr(2)}))))));
        Self->Data[3] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(static_cast<pas::WideString>(pas::concat_ansi({"Drastuan", SysUtils::IntToStr(3)}))))));
    }

    pas::WideString TArtefactCustom_GetInfoText(TArtefactCustom* Self, pas::WideString ColorTag, void* Ship) {
        pas::WideString Result{};
        Result = pas::concat_wide({aConst::LocalizedColorText(pas::concat_wide({u"Artefacts.CustomArtefacts.", Self->ConfigBlockName, u".Text"})), aItem::TEquipment_GetBonusDescription(Self, ColorTag), Self->GetConditionText(true), Self->GetBoostStatusText()});
        aMyFunction::ReplaceTextToken(Result, u"<Data1>"_w, pas::wide_int_to_str(Self->Data[1]), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<Data2>"_w, pas::wide_int_to_str(Self->Data[2]), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<Data3>"_w, pas::wide_int_to_str(Self->Data[3]), ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<TextData1>"_w, Self->TextData1, ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<TextData2>"_w, Self->TextData2, ColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<TextData3>"_w, Self->TextData3, ColorTag);
        return Result;
    }

    pas::WideString TArtefactCustom::GetDisplayName() {
        if (NameOverride != u"") {
            return NameOverride;
        }
        return aConst::LocalizedText(pas::concat_wide({u"Artefacts.CustomArtefacts.", ConfigBlockName, u".Name"}));
    }

    pas::WideString TArtefactCustom::GetDescriptionText() {
        return aConst::LocalizedColorText(pas::concat_wide({u"Artefacts.CustomArtefacts.", ConfigBlockName, u".Description"}));
    }

    // Uses the active equipment screen's ship; empty for broken artefacts or without a supported screen context.
    pas::WideString TArtefact::GetBoostStatusText() {
        pas::WideString Result{};
        std::int32_t I{};
        if (BrokenFlag != 0) {
            return Result;
        }
        aShip::TShip* Ship = nullptr;
        if (Globals::GetInnermostScreenLoop() == Globals::HangarScreen) {
            Ship = Globals::HangarScreen->SelectedShip;
        } else if (Globals::GetInnermostScreenLoop() == Globals::ScannerScreen) {
            Ship = Globals::ScannerScreen->ShipToInspect;
        } else if (Globals::GetInnermostScreenLoop() == Globals::ShipScreen) {
            Ship = fShip2::PlayerHoldShip;
        }
        if (Ship == nullptr) {
            return Result;
        }
        if (Ship->CanBoostArtefact(GetEffectiveType(), nullptr, true)) {
            if (EquippedFlag != 0) {
                return pas::concat_wide({u"\r\n \r\n", aConst::LocalizedColorText(u"Artefacts.TextArtGettingBoost"_wref.get())});
            }
            return pas::concat_wide({u"\r\n \r\n", aConst::LocalizedColorText(u"Artefacts.TextArtCanGetBoost"_wref.get())});
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Inventory) - 1); cpp_range.next(I); ) {
            if (Ship->CanBoostArtefact(GetEffectiveType(), pas::list_at<aItem::TEquipment>(Ship->Inventory, I), true)) {
                Result = pas::concat_wide({u"\r\n \r\n", aConst::LocalizedColorText(u"Artefacts.TextArtCanGetBoost"_wref.get())});
                break;
            }
        }
        return Result;
    }

    void TItem::p_destroy() {
        aItem::TItem_Destroy(this);
    }

    void TEquipment::p_destroy() {
        aItem::TEquipment_Destroy(this);
    }

    void TWeapon::p_destroy() {
        aItem::TWeapon_Destroy(this);
    }

    void TEquipmentWithActCode::p_destroy() {
        aItem::TEquipmentWithActCode_Destroy(this);
    }

    void TUselessItem::p_destroy() {
        aItem::TUselessItem_Destroy(this);
    }

    void TArtefact::p_destroy() {
        aItem::TArtefact_Destroy(this);
    }

    void TArtefactTranclucator::p_destroy() {
        aItem::TArtefactTranclucator_Destroy(this);
    }

    void TItem::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TItem_LoadFromBlock(this, Block);
    }

    pas::WideString TItem::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TItem_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TEquipment::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEquipment_LoadFromBlock(this, Block);
    }

    void THull::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::THull_LoadFromBlock(this, Block);
    }

    pas::WideString THull::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::THull_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TFuelTanks::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TFuelTanks_LoadFromBlock(this, Block);
    }

    pas::WideString TFuelTanks::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TFuelTanks_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TEngine::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TEngine_LoadFromBlock(this, Block);
    }

    pas::WideString TEngine::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TEngine_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TRadar::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TRadar_LoadFromBlock(this, Block);
    }

    pas::WideString TRadar::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TRadar_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TScaner::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TScaner_LoadFromBlock(this, Block);
    }

    pas::WideString TScaner::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TScaner_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TRepairRobot::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TRepairRobot_LoadFromBlock(this, Block);
    }

    pas::WideString TRepairRobot::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TRepairRobot_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TCargoHook::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TCargoHook_LoadFromBlock(this, Block);
    }

    pas::WideString TCargoHook::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TCargoHook_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TDefGenerator::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TDefGenerator_LoadFromBlock(this, Block);
    }

    pas::WideString TDefGenerator::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TDefGenerator_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TWeapon::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TWeapon_LoadFromBlock(this, Block);
    }

    void TCustomWeapon::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TCustomWeapon_LoadFromBlock(this, Block);
    }

    pas::WideString TWeapon::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TWeapon_GetInfoText(this, std::move(ColorTag), Ship);
    }

    pas::WideString TGoods::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TGoods_GetInfoText(this, std::move(ColorTag), Ship);
    }

    pas::WideString TCountableItem::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TCountableItem_GetInfoText(this, std::move(ColorTag), Ship);
    }

    pas::WideString TProtoplasm::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TProtoplasm_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TUselessItem::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TUselessItem_LoadFromBlock(this, Block);
    }

    pas::WideString TUselessItem::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TUselessItem_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TCistern::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TCistern_LoadFromBlock(this, Block);
    }

    pas::WideString TCistern::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TCistern_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TSatellite::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TSatellite_LoadFromBlock(this, Block);
    }

    pas::WideString TSatellite::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TSatellite_GetInfoText(this, std::move(ColorTag), Ship);
    }

    pas::WideString TTreasureMap::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TTreasureMap_GetInfoText(this, std::move(ColorTag), Ship);
    }

    pas::WideString TMicroModule::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TMicroModule_GetInfoText(this, std::move(ColorTag), Ship);
    }

    pas::WideString TArtefact::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TArtefact_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TArtefactTransmitter::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TArtefactTransmitter_LoadFromBlock(this, Block);
    }

    pas::WideString TArtefactTransmitter::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TArtefactTransmitter_GetInfoText(this, std::move(ColorTag), Ship);
    }

    void TArtefactTranclucator::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TArtefactTranclucator_LoadFromBlock(this, Block);
    }

    void TArtefactCustom::virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aItem::TArtefactCustom_LoadFromBlock(this, Block);
    }

    pas::WideString TArtefactCustom::virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) {
        return aItem::TArtefactCustom_GetInfoText(this, std::move(ColorTag), Ship);
    }

} // namespace aItem
