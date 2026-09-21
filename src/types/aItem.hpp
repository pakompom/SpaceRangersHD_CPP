#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aEFilm {
    struct TEFilmObj;

} // namespace aEFilm

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aItem {
    struct TExtraSpecial;

    struct TItem;

    struct TEquipment;

    struct THull;

    struct TFuelTanks;

    struct TEngine;

    struct TRadar;

    struct TScaner;

    struct TRepairRobot;

    struct TCargoHook;

    struct TDefGenerator;

    struct TWeapon;

    struct TCustomWeapon;

    struct TGoods;

    struct TCountableItem;

    struct TProtoplasm;

    struct TEquipmentWithActCode;

    struct TUselessItem;

    struct TCistern;

    struct TSatellite;

    struct TTreasureMap;

    struct TMicroModule;

    struct TArtefact;

    struct TArtefactTransmitter;

    struct TArtefactTranclucator;

    struct TArtefactCustom;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TItem : EC_Struct::TObjectEx {
        PAS_CLASS_META(TItem, EC_Struct::TObjectEx, "TItem", 56)
        void p_destroy() override;
        virtual void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Object-reference fields in descendants hold saved IDs until ResolveLoadedReferences; updates Galaxy.NextItemId.
        virtual void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        virtual void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        virtual void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        virtual void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        // Generic Items.SmallInfo label used outside radar range.
        static pas::WideString GetSmallInfoText();
        // Applies the trading-skill percentage to Cost minus repair cost; equipment has a minimum value of 1. Goods use Cost directly.
        std::int32_t CalculateResaleValue(std::uint8_t TradingSkill);
        // Equipment deducts repair cost, with a minimum result of 1; goods return Cost unchanged.
        std::int32_t GetConditionAdjustedCost();
        // Groups weapon types under Weapon and built-in artefacts under Artefact; otherwise returns the item-type configuration name.
        pas::WideString GetCategoryConfigName();
        virtual pas::WideString GetShortName();
        virtual pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship);
        // Lazily creates and initializes the retained scene container; returns a borrowed reference. Appearance depends on item kind, size, faction, and drop flags.
        SE_Space::TObjectSE* GetGraphObject();
        void ReleaseGraphObject();
        // Native TItem VMT slots $18, $24 and $28 point to the RTL abstract-method handler.
        virtual pas::WideString GetDisplayName() = 0;
        virtual pas::WideString GetDescriptionText() = 0;
        virtual pas::WideString GetBitmapResourceName() = 0;
        // Retained scene reference; released on destruction.
        SE_Space::TObjectSE* GraphObject;
        std::int32_t Id;
        aConst::TItemType ItemType;
        std::uint8_t cpp_padding[3];
        EC_Struct::TPointF Position;
        // Hull capacity for hull items.
        std::int32_t Weight;
        aGalaxyStruct::TOwnerId OwnerId;
        std::uint8_t cpp_padding_2[3];
        std::int32_t Cost;
        pas::WideString NameOverride;
        // Borrowed from the active film; not an integer object ID.
        aEFilm::TEFilmObj* FilmObject;
        // Borrowed TScriptItem; cleared when the wrapper is destroyed.
        pas::Object* ScriptItem;
        // Script.ItemDestroy.
        std::int32_t DestroyFlag;
        // Script.NoDropItem; not restricted to Boolean values.
        std::uint8_t NoDropFlag;
        std::uint8_t cpp_padding_3[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TImprovementKind : std::uint8_t {
        ikMinor = 0,
        ikMedium = 1,
        ikMajor = 2,
        ikAny = 3,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEquipment : aItem::TItem {
        PAS_CLASS_META(TEquipment, aItem::TItem, "TEquipment", 96)
        void p_destroy() override;
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        // Allocates a new item ID and resolves references in the current galaxy; changes LoadedSaveVersion.
        virtual aItem::TItem* Clone();
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Equip();
        virtual void Unequip();
        virtual void Repair();
        // Tests hull damage or supported equipment below 90 percent condition.
        std::uint8_t NeedsRepair();
        // Includes player technology restrictions as well as wear and breakage.
        pas::WideString GetConditionText(std::uint8_t PrefixNewLine);
        pas::WideString GetBrokenInBattleText();
        pas::WideString GetBrokenInUseText();
        pas::WideString GetBrokenByForceText();
        // Returns 0 for unsupported item types.
        std::int32_t GetLevel();
        // A through H for levels 1 through 8; empty for unsupported levels.
        pas::WideString GetLevelLetter();
        pas::WideString GetDescriptionText() override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetShortName() override;
        // Selects strength and stat emphasis from Id, then calls Improve. Charging and eligibility checks belong to the caller.
        void ImproveAtScientificBase();
        std::uint8_t HasMicroModule();
        pas::WideString GetMicroModuleQuotedName();
        pas::WideString GetSpecialModuleName();
        // Base is a no-op. Overrides change statistics and Cost without checking CanImprove or charging money; ikAny selects one of the three strengths.
        virtual void Improve(TImprovementKind Kind);
        // Only ikMinor, ikMedium and ikMajor are valid: Cost times 0.3, 0.6 or 1.2, rounded then truncated to a multiple of 10, before service discounts.
        virtual std::int32_t CalculateImprovementCost(TImprovementKind Kind);
        // Tests expected generated statistics after accounting for installed bonuses; this is not a stored upgraded flag. The base implementation returns True.
        virtual std::uint8_t HasStandardStats();
        pas::WideString GetBitmapResourceName() override;
        virtual float GetFragilityFactor(aGalaxyStruct::TDamageFlagSet DamageFlags);
        virtual void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship);
        std::int32_t GetStatBonus(aConst::TEquipmentBonusKind BonusKind);
        // Aggregate used by the bonus description, with SeparatedNumbers effects handled separately.
        std::int32_t GetDescriptionStatBonus(aConst::TEquipmentBonusKind BonusKind);
        // Requires HasStandardStats and no special module that blocks the special slot; does not check technology access.
        std::uint8_t CanImprove();
        pas::WideString ConfigBlockName;
        // Script.EqCustomFaction.
        pas::WideString CustomFaction;
        // 100 is fully maintained; hull integrity is tracked separately.
        double ConditionPercent;
        std::uint8_t BrokenFlag;
        std::uint8_t EquippedFlag;
        std::uint8_t cpp_padding[2];
        // Low 7 bits: zero-based slot index; bit 7 selects secondary fire in arcade combat.
        std::uint32_t AssignedSlotData;
        // One-based MicroModuleTemplates indexes; zero means no module.
        // For a standalone TMicroModule, MicroModuleIndex identifies the item itself.
        std::int32_t MicroModuleIndex;
        std::int32_t SpecialModuleIndex;
        // Owns PExtraSpecial entries and the list; nil when absent.
        pas::List* ExtraSpecials;
        // Script.ItemSubrace.
        aGalaxyStruct::TDominatorSeries DominatorSeries;
        // Transient selector: 0 chooses automatically; 1/2 emphasize different stats; weapons also accept 3 for range. Consumed by engine, gripper and weapon upgrades.
        std::uint8_t DetailImprovement;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TInterceptorTargetingStrategy : std::uint8_t {
        itsManual = 0,
        itsMostHullPoints = 1,
        itsFewestHullPoints = 2,
        itsGreatestStrength = 3,
        itsStrongestDefense = 4,
        itsNearest = 5,
        itsFarthest = 6,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct THull : aItem::TEquipment {
        PAS_CLASS_META(THull, aItem::TEquipment, "THull", 132)
        void Init(std::int32_t Capacity, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner, std::uint8_t HullType, std::int32_t Series, std::uint8_t PirateBuilt);
        // Applies the current hull series, resets HullPoints to capacity, and bounds Cost.
        void ApplySeriesSizeAndCost();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int8_t CalculateGeneratedArmor();
        // If cost generation overflows negative, repeatedly halves capacity and resets HullPoints before retrying.
        std::int32_t CalculateGeneratedCost();
        void Repair() override;
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetSeriesName();
        pas::WideString GetShortName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetBitmapResourceName() override;
        // Returns the special module KindGraph, or the literal 1 as fallback.
        pas::WideString GetSpecialKindGraph();
        std::int32_t GetSlotCount(aConst::TShipSlotKind Kind);
        // Round(capacity * (0.6 + 0.2 * (clamped level - 1) / 7)); excludes carried cargo and ship mass modifiers.
        std::int32_t CalculateMass();
        // Zero flags return the average of energy, splinter and missile factors.
        float GetFragilityFactor(aGalaxyStruct::TDamageFlagSet DamageFlags) override;
        // Reverses module and series size percentages with rounding; extra-special multiplicities are not used.
        std::int32_t EstimateCapacityWithoutBonuses();
        std::int32_t HullPoints;
        std::uint8_t TechLevel;
        std::int8_t Armor;
        // ht* categories in aGalaxyStruct.
        std::uint8_t HullType;
        std::uint8_t cpp_padding[1];
        // -1 means no series.
        std::int32_t HullSeries;
        // Borrowed; not serialized by the hull.
        void* OwnerShip;
        // Script.CapitalShipStats.
        std::uint8_t CapitalShip;
        std::uint8_t PirateBuilt;
        std::uint8_t ImpulseShieldsEnabled;
        std::uint8_t InterceptorsEnabled;
        std::int32_t Energy;
        std::int32_t EnergyMax;
        // Saved as an ID until ResolveLoadedReferences.
        void* InterceptorTarget;
        // Automatic target selection for the player.
        TInterceptorTargetingStrategy InterceptorTargetingStrategy;
        // Zero selects five passes.
        std::uint8_t InterceptorPassCountOverride;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TFuelTanks : aItem::TEquipment {
        PAS_CLASS_META(TFuelTanks, aItem::TEquipment, "TFuelTanks", 108)
        void Init(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::uint8_t CalculateGeneratedCapacity();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[3];
        std::int32_t Fuel;
        std::uint8_t Capacity;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEngine : aItem::TEquipment {
        PAS_CLASS_META(TEngine, aItem::TEquipment, "TEngine", 108)
        void Init(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int32_t CalculateGeneratedSpeed();
        std::int8_t CalculateGeneratedJumpRange();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[3];
        std::int32_t Speed;
        std::int8_t JumpRange;
        std::uint8_t OutputPercent;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRadar : aItem::TEquipment {
        PAS_CLASS_META(TRadar, aItem::TEquipment, "TRadar", 104)
        void Init(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int32_t CalculateGeneratedRange();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[3];
        std::int32_t Range;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScaner : aItem::TEquipment {
        PAS_CLASS_META(TScaner, aItem::TEquipment, "TScaner", 100)
        void Init(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int32_t CalculateGeneratedScanPower();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        std::uint8_t TechLevel;
        std::int8_t ScanPower;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRepairRobot : aItem::TEquipment {
        PAS_CLASS_META(TRepairRobot, aItem::TEquipment, "TRepairRobot", 100)
        void Init(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::uint8_t CalculateGeneratedRepairPoints();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        std::uint8_t TechLevel;
        std::uint8_t RepairPoints;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCargoHook : aItem::TEquipment {
        PAS_CLASS_META(TCargoHook, aItem::TEquipment, "TCargoHook", 116)
        void Init(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int32_t CalculateGeneratedPickupPower();
        std::int32_t CalculateGeneratedRange();
        float CalculateGeneratedMinPullSpeed();
        float CalculateGeneratedMaxPullSpeed();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[3];
        std::int32_t PickupPower;
        std::int32_t Range;
        float MinPullSpeed;
        float MaxPullSpeed;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TDefGenerator : aItem::TEquipment {
        PAS_CLASS_META(TDefGenerator, aItem::TEquipment, "TDefGenerator", 104)
        void Init(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        float CalculateGeneratedDamageFactor();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[3];
        float DamageFactor;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TWeaponTargetKind : std::uint8_t {
        wtkNone = 0,
        wtkShip = 1,
        wtkItem = 2,
        wtkAsteroid = 3,
        wtkMissile = 4,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TWeapon : aItem::TEquipment {
        PAS_CLASS_META(TWeapon, aItem::TEquipment, "TWeapon", 128)
        void p_destroy() override;
        void Init(aConst::TItemType ItemType, std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void Unequip() override;
        std::int32_t CalculateGeneratedAmmoCapacity();
        std::int32_t CalculateGeneratedMinDamage();
        std::int32_t CalculateGeneratedMaxDamage();
        std::int32_t CalculateGeneratedRange();
        // Generated maximum plus ordinary and special module damage bonuses; used by HasStandardStats.
        std::int32_t CalculateStandardMaxDamage();
        // Generated range plus ordinary and special module range bonuses; used by HasStandardStats.
        std::int32_t CalculateStandardRange();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetShortName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        double GetShotDelayFactor();
        pas::WideString GetBitmapResourceName() override;
        std::uint8_t NeedsAmmo();
        std::int32_t CalculateAmmoRefillCost();
        std::int32_t GetShotPalette();
        // Combines the weapon template and installed ordinary, special and extra-special module flags.
        aGalaxyStruct::TDamageFlagSet GetDamageFlags();
        std::int32_t GetShotCount();
        std::int32_t GetAttackCount();
        virtual aConst::PWeaponInfo GetWeaponInfo();
        virtual pas::WideString GetConfigName();
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[3];
        std::int32_t Range;
        std::int32_t MinDamage;
        std::int32_t MaxDamage;
        // Saved as an ID until ResolveLoadedReferences.
        pas::Object* Target;
        // Discriminator read from the save, not a live target classification.
        TWeaponTargetKind LoadedTargetKind;
        std::uint8_t cpp_padding_2[3];
        std::int32_t Ammo;
        std::int32_t AmmoCapacity;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSatellite : aItem::TEquipment {
        PAS_CLASS_META(TSatellite, aItem::TEquipment, "TSatellite", 116)
        // Clears deployment state.
        void InitGenerated(std::uint8_t TypeId, aGalaxyStruct::TOwnerId Owner, std::uint32_t Seed);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        pas::WideString GetDisplayName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetBrokenInUseText_2();
        pas::WideString GetIdleInfoText();
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        std::uint8_t SatelliteTypeId;
        std::uint8_t cpp_padding[3];
        void* TargetPlanet;
        std::int32_t TrajectoryIndex;
        float WearPerTurn;
        std::uint8_t WaterExplorationRate;
        std::uint8_t LandExplorationRate;
        std::uint8_t HillExplorationRate;
        std::uint8_t cpp_padding_2[1];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEquipmentWithActCode : aItem::TEquipment {
        PAS_CLASS_META(TEquipmentWithActCode, aItem::TEquipment, "TEquipmentWithActCode", 104)
        void p_destroy() override;
        // Borrowed.
        void* ActionCode;
        std::uint8_t ActCodeInitialized;
        std::uint8_t DisplayAsArtefact;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TArtefact : aItem::TEquipmentWithActCode {
        PAS_CLASS_META(TArtefact, aItem::TEquipmentWithActCode, "TArtefact", 104)
        void p_destroy() override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        virtual void Init(aGalaxyStruct::TOwnerId Owner, aConst::TItemType ItemType);
        pas::WideString GetBitmapResourceName() override;
        pas::WideString GetDisplayName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        // Returns empty when the OnUseCode block is absent.
        pas::WideString GetOnUseCodeText();
        // Borrowed cached result, possibly nil. Marks initialization before resolving the configuration.
        void* GetActionCode();
        // Custom artefacts with SharedEffect use CountsAsItemType; otherwise returns ItemType.
        aConst::TItemType GetEffectiveType();
        // Uses the active equipment screen's ship; empty for broken artefacts or without a supported screen context.
        pas::WideString GetBoostStatusText();
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TArtefactCustom : aItem::TArtefact {
        PAS_CLASS_META(TArtefactCustom, aItem::TArtefact, "TArtefactCustom", 132)
        // Uses ConfigBlockName. Reloads NoWear, CountsAs, SharedUse and SharedEffect; preserves Data and TextData.
        void LoadConfig(std::uint8_t ApplyConfiguredWeight);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetDescriptionText() override;
        aConst::TItemType CountsAsItemType;
        // Shares the equipped-artefact duplicate check with CountsAsItemType.
        std::uint8_t SharedUse;
        // Shares the effective artefact type with CountsAsItemType.
        std::uint8_t SharedEffect;
        std::uint8_t cpp_padding[1];
        // Description tokens <Data1> through <Data3>.
        pas::Array<std::int32_t, 1, 3> Data;
        pas::WideString TextData1;
        pas::WideString TextData2;
        pas::WideString TextData3;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TExtraSpecial {
        std::int32_t ModuleIndexPlusOne;
        std::int32_t Count;
    };
    #pragma pack(pop)

    using PExtraSpecial = TExtraSpecial*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TUselessItem : aItem::TEquipmentWithActCode {
        PAS_CLASS_META(TUselessItem, aItem::TEquipmentWithActCode, "TUselessItem", 120)
        void p_destroy() override;
        void Init(pas::WideString ConfigName, aGalaxyStruct::TDominatorSeries Series, std::uint32_t Seed, std::uint8_t ForceArtefactDisplay);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString GetDisplayName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        // Owner is Dominator and ConfigBlockName starts with Remains_.
        std::uint8_t IsDominatorRemains();
        void CheckIfWeDisplayAsArtefact();
        pas::WideString GetOnUseCodeText();
        void* GetActionCode();
        // Script.UselessItemText.
        pas::WideString CustomText;
        // Script.UselessItemData uses indexes 1..3.
        pas::Array<std::int32_t, 0, 2> Data;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMicroModule : aItem::TEquipment {
        PAS_CLASS_META(TMicroModule, aItem::TEquipment, "TMicroModule", 96)
        // ModuleIndex is zero-based and must identify an existing template.
        void Init(std::int32_t ModuleIndex);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetPlainName();
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        // Node refund at the current ranger center, using priority and docked station ID. Priorities 31..69 are capped by half LowPriorityOfferCost; 70..100 by half MediumPriorityOfferCost. Minimum 5 nodes.
        std::int32_t CalculateNodeExchangeValue(std::int32_t LowPriorityOfferCost, std::int32_t MediumPriorityOfferCost);
        // Template name wrapped in the standard yellow highlight color.
        pas::WideString GetHighlightedName();
        // Uses this micromodule item's template and checks slot blockers and equipment compatibility.
        std::uint8_t CanInstallOn(aItem::TEquipment* Item);
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCistern : aItem::TEquipment {
        PAS_CLASS_META(TCistern, aItem::TEquipment, "TCistern", 104)
        void Init(std::int32_t Fuel, std::uint8_t Capacity, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString GetDisplayName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        std::int32_t Fuel;
        std::uint8_t Capacity;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGoods : aItem::TItem {
        PAS_CLASS_META(TGoods, aItem::TItem, "TGoods", 64)
        void Init(aConst::TItemType ItemType, std::int32_t Quantity);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        pas::WideString GetDisplayName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        std::int32_t Quantity;
        std::uint8_t NaturalFlag;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCountableItem : aItem::TEquipment {
        PAS_CLASS_META(TCountableItem, aItem::TEquipment, "TCountableItem", 104)
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void Init(pas::WideString ConfigName, std::int32_t Count, std::uint8_t DropFlag);
        pas::WideString GetDisplayName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        // Defaults to 1 when UnitSize is not configured.
        std::int32_t GetUnitSize();
        // Allocates a new stack and removes up to Count units from Self; preserves the nodes subtype and may create a script wrapper. Self must be nonempty and Count positive.
        TCountableItem* Split(std::int32_t Count);
        std::int32_t StackCount;
        std::uint8_t DropFlag;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TProtoplasm : aItem::TCountableItem {
        PAS_CLASS_META(TProtoplasm, aItem::TCountableItem, "TProtoplasm", 104)
        void Init_2(std::int32_t Count, std::uint8_t DropFlag);
        pas::WideString GetDisplayName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TArtefactTranclucator : aItem::TArtefact {
        PAS_CLASS_META(TArtefactTranclucator, aItem::TArtefact, "TArtefactTranclucator", 108)
        void p_destroy() override;
        // Takes ownership of ExistingShip, or creates a ship when nil.
        void InitTranclucator(aGalaxyStruct::TOwnerId Owner, void* OwnerShip, void* ExistingShip);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        // Always returns nil.
        aItem::TItem* Clone() override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        pas::WideString GetDisplayName() override;
        // Owned while stored in the artefact; deployment transfers ownership.
        void* Ship;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TArtefactTransmitter : aItem::TArtefact {
        PAS_CLASS_META(TArtefactTransmitter, aItem::TArtefact, "TArtefactTransmitter", 108)
        void InitTransmitter(aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        std::int32_t Power;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTreasureMap : aItem::TEquipment {
        PAS_CLASS_META(TTreasureMap, aItem::TEquipment, "TTreasureMap", 112)
        void Init(void* Planet, void* Victim);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        pas::WideString GetDisplayName() override;
        pas::WideString virtual_TItem_GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        pas::WideString GetTargetPlanetName();
        // PageIndex is 1 or 2; Planet must be assigned.
        static pas::WideString BuildPreviewTable(std::int32_t PageIndex, void* Planet);
        // Saved as an ID until ResolveLoadedReferences.
        void* TargetPlanet;
        pas::WideString SourceShipName;
        pas::WideString PreviewTablePage1;
        pas::WideString PreviewTablePage2;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Native class-name spelling.
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCustomWeapon : aItem::TWeapon {
        PAS_CLASS_META(TCustomWeapon, aItem::TWeapon, "TCustomWeapon", 132)
        void InitCustom(aConst::PWeaponInfo Info, std::uint8_t Equipped, std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TItem_LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString GetBitmapResourceName() override;
        aConst::PWeaponInfo GetWeaponInfo() override;
        pas::WideString GetConfigName() override;
        aConst::PWeaponInfo CustomInfo;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TItemLootPool : std::uint8_t {
        ilpArcadeBattle = 0,
        ilpTreasure = 1,
        ilpReward = 2,
        ilpAnyAvailable = 3,
    };

    // AssignedSlotData is also copied into TabWeapon.SlotData. The arcade group
    // toggle XORs bit 7; primary/secondary fire tests it.
    inline constexpr std::int32_t EquipmentSlotIndexMask = 0x0000007f;

    inline constexpr std::int32_t EquipmentSecondaryFireFlag = 0x00000080;

} // namespace aItem
