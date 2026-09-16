#pragma once
#include "types/aConst.hpp"
#include "types/aItem.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aItem {
    // Native managed-string defaults.
    extern pas::Array<pas::WideString, 1, 8> EquipmentLevelLetters;

    extern pas::Array<pas::Array<std::int32_t, 0, 3>, 1, 2> TreasureMapColumnPositions;

    extern pas::Array<std::int32_t, 1, 2> TreasureMapRuleLengths;

    // Nested native helpers include the caller's saved EBP explicitly in the IDA ABI.
    // Selects across built-in artefacts, custom artefacts and configured useless items. Pool must be nonempty; AnyAvailable is the union of the three eligibility flags.
    TEquipmentWithActCode* CreateRandomLootItem(TItemLootPool Pool, std::uint8_t Owner, std::uint32_t Seed);

    // Returns a one-based template index, or 0 if the saved template cannot be resolved.
    std::int32_t ReadSavedMicroModuleIndex(EC_Buf::TBufEC* Buffer);

    // Applies the ordered item-type insertions for save versions before 164, 78, 131, 78 and 127; arithmetic wraps in a byte.
    aConst::TItemType MigrateSavedItemType(std::uint8_t ItemType);

    std::int32_t GetBaseHullSlotCount(aConst::TShipSlotKind Kind, std::uint8_t HullType, std::uint8_t Owner, void* Ship);

    std::int32_t CalculateGeneratedHullCost(std::uint32_t Capacity, std::uint32_t Level, std::uint8_t Owner, std::uint8_t HullType);

    std::int32_t CalculateGeneratedFuelCapacity(std::uint32_t Weight, std::int32_t Level);

    std::int32_t CalculateGeneratedFuelTanksCost(std::uint32_t Weight, std::int32_t Level, std::uint8_t Owner);

    std::int32_t CalculateGeneratedEngineCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner);

    std::int32_t CalculateGeneratedRadarCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner);

    std::int32_t CalculateGeneratedScanerCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner);

    std::int32_t CalculateGeneratedRepairRobotCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner);

    std::int32_t CalculateGeneratedCargoHookCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner);

    std::int32_t CalculateGeneratedDefGeneratorCost(std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner);

    double GetGeneratedDefenseDamageFactor(std::uint8_t Level);

    std::uint8_t DefenseDamageFactorToPercent(double Factor);

    double DefensePercentToDamageFactor(std::int32_t Percent);

    // ModuleIndex is zero-based. Expands all bonus tokens in the configured description.
    pas::WideString GetMicroModuleInfoText(std::int32_t ModuleIndex, pas::WideString ColorTag);

    std::uint8_t GetMicroModulePriorityColorTier(std::int32_t ModuleIndex);

    pas::WideString GetMicroModuleNameColorTag(std::int32_t ModuleIndex);

    pas::WideString GetMicroModuleTextColorTag(std::int32_t ModuleIndex);

    pas::WideString GetMicroModuleBitmapResourceName(std::int32_t ModuleIndex);

    // Returns nil outside item types 10..41.
    TArtefact* CreateConfiguredArtefactByItemType(aConst::TItemType ItemType, std::uint8_t Owner);

    std::int32_t CalculateGeneratedWeaponCost(aConst::PWeaponInfo Info, std::uint32_t Weight, std::uint8_t Level, std::uint8_t Owner);

    // Constructs the instance without calling its Init routine.
    TItem* CreateItemByType(aConst::TItemType ItemType);

    TItem* CreateDefaultItemByType(aConst::TItemType ItemType);

    // Clamps Level to 1..8; custom weapons require CreateGeneratedWeapon.
    TEquipment* CreateGeneratedEquipment(aConst::TItemType ItemType, std::int32_t Weight, std::int32_t Level, std::uint8_t Owner);

    TWeapon* CreateGeneratedWeapon(aConst::PWeaponInfo Info, std::int32_t Weight, std::int32_t Level, std::uint8_t Owner);

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

    // Preserve the native evaluation order: select the percentage before clamping
    // capacity. The inline helper also retains the compiler's separate temporaries.
    void CalculateHullCapacityIncrease(THull* Hull, std::int32_t LowPercent, std::int32_t HighPercent, std::int32_t& Increase);

    void TItem_Create(TItem* Self);

    void TItem_Destroy(TItem* Self);

    void TEquipment_Create(TEquipment* Self);

    void TEquipment_Destroy(TEquipment* Self);

    void TCargoHook_Create(TCargoHook* Self);

    void TWeapon_Destroy(TWeapon* Self);

    void TEquipmentWithActCode_Create(TEquipmentWithActCode* Self);

    void TEquipmentWithActCode_Destroy(TEquipmentWithActCode* Self);

    void TUselessItem_Create(TUselessItem* Self);

    void TUselessItem_Destroy(TUselessItem* Self);

    void TArtefact_Create(TArtefact* Self);

    void TArtefact_Destroy(TArtefact* Self);

    void TArtefactTranclucator_Destroy(TArtefactTranclucator* Self);

} // namespace aItem
