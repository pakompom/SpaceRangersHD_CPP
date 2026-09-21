#pragma once
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aItem {
    // Native managed-string defaults.
    extern pas::Array<pas::WideString, 1, 8> EquipmentLevelLetters;

    extern pas::Array<pas::Array<std::int32_t, 0, 3>, 1, 2> TreasureMapColumnPositions;

    extern pas::Array<std::int32_t, 1, 2> TreasureMapRuleLengths;

    // Selects across built-in artefacts, custom artefacts and configured useless items. Pool must be nonempty; AnyAvailable is the union of the three eligibility flags.
    TEquipmentWithActCode* CreateRandomLootItem(TItemLootPool Pool, aGalaxyStruct::TOwnerId Owner, std::uint32_t Seed);

    // Returns a one-based template index, or 0 if the saved template cannot be resolved.
    std::int32_t ReadSavedMicroModuleIndex(EC_Buf::TBufEC* Buffer);

    // Applies the ordered item-type insertions for save versions before 164, 78, 131, 78 and 127; arithmetic wraps in a byte.
    aConst::TItemType MigrateSavedItemType(std::uint8_t ItemType);

    std::int32_t GetBaseHullSlotCount(aConst::TShipSlotKind Kind, std::uint8_t HullType, aGalaxyStruct::TOwnerId Owner, void* Ship);

    std::int32_t CalculateGeneratedHullCost(std::uint32_t Capacity, std::uint32_t Level, aGalaxyStruct::TOwnerId Owner, std::uint8_t HullType);

    std::int32_t CalculateGeneratedFuelCapacity(std::uint32_t Weight, std::int32_t Level);

    std::int32_t CalculateGeneratedFuelTanksCost(std::uint32_t Weight, std::int32_t Level, aGalaxyStruct::TOwnerId Owner);

    std::int32_t CalculateGeneratedEngineCost(std::uint32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);

    std::int32_t CalculateGeneratedRadarCost(std::uint32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);

    std::int32_t CalculateGeneratedScanerCost(std::uint32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);

    std::int32_t CalculateGeneratedRepairRobotCost(std::uint32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);

    std::int32_t CalculateGeneratedCargoHookCost(std::uint32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);

    std::int32_t CalculateGeneratedDefGeneratorCost(std::uint32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);

    double GetGeneratedDefenseDamageFactor(std::uint8_t Level);

    aGalaxyStruct::TPercent DefenseDamageFactorToPercent(double Factor);

    double DefensePercentToDamageFactor(std::int32_t Percent);

    // ModuleIndex is zero-based. Expands all bonus tokens in the configured description.
    pas::WideString GetMicroModuleInfoText(std::int32_t ModuleIndex, pas::WideString ColorTag);

    std::uint8_t GetMicroModulePriorityColorTier(std::int32_t ModuleIndex);

    pas::WideString GetMicroModuleNameColorTag(std::int32_t ModuleIndex);

    pas::WideString GetMicroModuleTextColorTag(std::int32_t ModuleIndex);

    pas::WideString GetMicroModuleBitmapResourceName(std::int32_t ModuleIndex);

    // Returns nil outside item types 10..41.
    TArtefact* CreateConfiguredArtefactByItemType(aConst::TItemType ItemType, aGalaxyStruct::TOwnerId Owner);

    std::int32_t CalculateGeneratedWeaponCost(aConst::PWeaponInfo Info, std::uint32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);

    // Constructs the instance without calling its Init routine.
    TItem* CreateItemByType(aConst::TItemType ItemType);

    TItem* CreateDefaultItemByType(aConst::TItemType ItemType);

    // Clamps Level to 1..8; custom weapons require CreateGeneratedWeapon.
    TEquipment* CreateGeneratedEquipment(aConst::TItemType ItemType, std::int32_t Weight, std::int32_t Level, aGalaxyStruct::TOwnerId Owner);

    TWeapon* CreateGeneratedWeapon(aConst::PWeaponInfo Info, std::int32_t Weight, std::int32_t Level, aGalaxyStruct::TOwnerId Owner);

    // Module indices are zero-based; compatibility checks also accept special bonuses.
    std::uint8_t CanInstallMicroModule(std::int32_t ModuleIndex, TEquipment* Item);

    std::uint8_t IsBonusCompatibleWithEquipment(std::int32_t ModuleIndex, TEquipment* Item);

    std::uint8_t IsBonusCompatibleWithHull(std::int32_t ModuleIndex, THull* Hull);

    std::uint8_t IsBonusCompatibleWithWeapon(std::int32_t ModuleIndex, TWeapon* Weapon);

    // Does not check compatibility or remove an existing module; -1 or nil returns False.
    std::uint8_t ApplyMicroModule(std::int32_t ModuleIndex, TEquipment* Item);

    void ApplySpecialMicroModule(std::int32_t ModuleIndex, TEquipment* Item);

    void RemoveMicroModule(TEquipment* Item);

    void RemoveSpecialMicroModule(TEquipment* Item);

    std::uint8_t CanCargoHookHandleItem(TItem* Item, void* Ship);

    pas::WideString GetItemTypeBitmapPath(aConst::TItemType ItemType);

    // Goods use their market display name; nodes use the generic node name; other types return empty.
    pas::WideString GetStackableItemTypeName(aConst::TItemType ItemType);

    // Custom countables use their configured name; all other types use GetStackableItemTypeName. Ignores per-instance name overrides.
    pas::WideString GetStackableItemName(TItem* Item);

    void TItem_Create(TItem* Self);

    void TItem_Destroy(TItem* Self);

    void TItem_LoadFromBlock(TItem* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TItem_GetInfoText(TItem* Self, pas::WideString ColorTag, void* Ship);

    // Manufacturer/faction resource key, including custom factions and Dominator series.
    pas::WideString TItem_GetOwnerConfigName(TItem* Self);

    void TEquipment_Create(TEquipment* Self);

    void TEquipment_Destroy(TEquipment* Self);

    void TEquipment_LoadFromBlock(TEquipment* Self, EC_BlockPar::TBlockParEC* Block);

    // Undiscounted cost; hulls use HullPoints, other supported equipment uses ConditionPercent and BrokenFlag.
    std::int32_t TEquipment_CalculateRepairCost(TEquipment* Self);

    pas::WideString TEquipment_GetBonusDescription(TEquipment* Self, pas::WideString ColorTag);

    void THull_LoadFromBlock(THull* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString THull_GetInfoText(THull* Self, pas::WideString ColorTag, void* Ship);

    void TFuelTanks_LoadFromBlock(TFuelTanks* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TFuelTanks_GetInfoText(TFuelTanks* Self, pas::WideString ColorTag, void* Ship);

    void TEngine_LoadFromBlock(TEngine* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TEngine_GetInfoText(TEngine* Self, pas::WideString ColorTag, void* Ship);

    void TRadar_LoadFromBlock(TRadar* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TRadar_GetInfoText(TRadar* Self, pas::WideString ColorTag, void* Ship);

    void TScaner_LoadFromBlock(TScaner* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TScaner_GetInfoText(TScaner* Self, pas::WideString ColorTag, void* Ship);

    void TRepairRobot_LoadFromBlock(TRepairRobot* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TRepairRobot_GetInfoText(TRepairRobot* Self, pas::WideString ColorTag, void* Ship);

    void TCargoHook_Create(TCargoHook* Self);

    void TCargoHook_LoadFromBlock(TCargoHook* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TCargoHook_GetInfoText(TCargoHook* Self, pas::WideString ColorTag, void* Ship);

    void TDefGenerator_LoadFromBlock(TDefGenerator* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TDefGenerator_GetInfoText(TDefGenerator* Self, pas::WideString ColorTag, void* Ship);

    void TWeapon_Destroy(TWeapon* Self);

    void TWeapon_LoadFromBlock(TWeapon* Self, EC_BlockPar::TBlockParEC* Block);

    void TCustomWeapon_LoadFromBlock(TCustomWeapon* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TWeapon_GetInfoText(TWeapon* Self, pas::WideString ColorTag, void* Ship);

    pas::WideString TGoods_GetInfoText(TGoods* Self, pas::WideString ColorTag, void* Ship);

    pas::WideString TCountableItem_GetInfoText(TCountableItem* Self, pas::WideString ColorTag, void* Ship);

    std::uint8_t TCountableItem_CanMerge(TCountableItem* Self, pas::Object* Other);

    // Leaves Other unchanged.
    std::uint8_t TCountableItem_Merge(TCountableItem* Self, pas::Object* Other);

    pas::WideString TProtoplasm_GetInfoText(TProtoplasm* Self, pas::WideString ColorTag, void* Ship);

    void TEquipmentWithActCode_Create(TEquipmentWithActCode* Self);

    void TEquipmentWithActCode_Destroy(TEquipmentWithActCode* Self);

    void TUselessItem_Create(TUselessItem* Self);

    void TUselessItem_Destroy(TUselessItem* Self);

    void TUselessItem_LoadFromBlock(TUselessItem* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TUselessItem_GetInfoText(TUselessItem* Self, pas::WideString ColorTag, void* Ship);

    void TCistern_LoadFromBlock(TCistern* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TCistern_GetInfoText(TCistern* Self, pas::WideString ColorTag, void* Ship);

    void TSatellite_LoadFromBlock(TSatellite* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TSatellite_GetInfoText(TSatellite* Self, pas::WideString ColorTag, void* Ship);

    pas::WideString TTreasureMap_GetInfoText(TTreasureMap* Self, pas::WideString ColorTag, void* Ship);

    pas::WideString TMicroModule_GetInfoText(TMicroModule* Self, pas::WideString ColorTag, void* Ship);

    void TArtefact_Create(TArtefact* Self);

    void TArtefact_Destroy(TArtefact* Self);

    pas::WideString TArtefact_GetInfoText(TArtefact* Self, pas::WideString ColorTag, void* Ship);

    void TArtefactTransmitter_LoadFromBlock(TArtefactTransmitter* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TArtefactTransmitter_GetInfoText(TArtefactTransmitter* Self, pas::WideString ColorTag, void* Ship);

    void TArtefactTranclucator_Destroy(TArtefactTranclucator* Self);

    void TArtefactTranclucator_LoadFromBlock(TArtefactTranclucator* Self, EC_BlockPar::TBlockParEC* Block);

    void TArtefactCustom_LoadFromBlock(TArtefactCustom* Self, EC_BlockPar::TBlockParEC* Block);

    pas::WideString TArtefactCustom_GetInfoText(TArtefactCustom* Self, pas::WideString ColorTag, void* Ship);

} // namespace aItem
