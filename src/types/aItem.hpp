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
        virtual void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        virtual void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        virtual void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        virtual void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        static pas::WideString GetSmallInfoText();
        std::int32_t CalculateResaleValue(std::uint8_t TradingSkill);
        std::int32_t GetConditionAdjustedCost();
        pas::WideString GetCategoryConfigName();
        virtual pas::WideString GetShortName();
        virtual pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship);
        SE_Space::TObjectSE* GetGraphObject();
        void ReleaseGraphObject();
        pas::WideString GetOwnerConfigName();
        virtual pas::WideString GetDisplayName() = 0;
        virtual pas::WideString GetDescriptionText() = 0;
        virtual pas::WideString GetBitmapResourceName() = 0;
        SE_Space::TObjectSE* GraphObject;
        std::int32_t Id;
        aConst::TItemType ItemType;
        std::uint8_t cpp_padding[3];
        EC_Struct::TPointF Position;
        std::int32_t Weight;
        std::uint8_t OwnerId;
        std::uint8_t cpp_padding_2[3];
        std::int32_t Cost;
        pas::WideString NameOverride;
        aEFilm::TEFilmObj* FilmObject;
        pas::Object* ScriptItem;
        std::int32_t DestroyFlag;
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
        virtual aItem::TItem* Clone();
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Equip();
        virtual void Unequip();
        virtual void Repair();
        std::uint8_t NeedsRepair();
        std::int32_t CalculateRepairCost();
        pas::WideString GetConditionText(std::uint8_t PrefixNewLine);
        pas::WideString GetBrokenInBattleText();
        pas::WideString GetBrokenInUseText();
        pas::WideString GetBrokenByForceText();
        std::int32_t GetLevel();
        pas::WideString GetLevelLetter();
        pas::WideString GetDescriptionText() override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetShortName() override;
        void ImproveAtScientificBase();
        std::uint8_t HasMicroModule();
        pas::WideString GetMicroModuleQuotedName();
        pas::WideString GetSpecialModuleName();
        virtual void Improve(TImprovementKind Kind);
        virtual std::int32_t CalculateImprovementCost(TImprovementKind Kind);
        virtual std::uint8_t HasStandardStats();
        pas::WideString GetBitmapResourceName() override;
        virtual float GetFragilityFactor(aGalaxyStruct::TDamageFlagSet DamageFlags);
        virtual void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship);
        std::int32_t GetStatBonus(aConst::TEquipmentBonusKind BonusKind);
        std::int32_t GetDescriptionStatBonus(aConst::TEquipmentBonusKind BonusKind);
        pas::WideString GetBonusDescription(pas::WideString ColorTag);
        std::uint8_t CanImprove();
        pas::WideString ConfigBlockName;
        pas::WideString CustomFaction;
        double ConditionPercent;
        std::uint8_t BrokenFlag;
        std::uint8_t EquippedFlag;
        std::uint8_t cpp_padding[2];
        std::uint32_t AssignedSlotData;
        std::int32_t MicroModuleIndex;
        std::int32_t SpecialModuleIndex;
        pas::List* ExtraSpecials;
        aGalaxyStruct::TDominatorSeries DominatorSeries;
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
        void Init(std::int32_t Capacity, std::uint8_t Level, std::uint8_t Owner, std::uint8_t HullType, std::int32_t Series, std::uint8_t PirateBuilt);
        void ApplySeriesSizeAndCost();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int8_t CalculateGeneratedArmor();
        std::int32_t CalculateGeneratedCost();
        void Repair() override;
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetSeriesName();
        pas::WideString GetShortName() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetBitmapResourceName() override;
        pas::WideString GetSpecialKindGraph();
        std::int32_t GetSlotCount(aConst::TShipSlotKind Kind);
        std::int32_t CalculateMass();
        float GetFragilityFactor(aGalaxyStruct::TDamageFlagSet DamageFlags) override;
        std::int32_t EstimateCapacityWithoutBonuses();
        std::int32_t HullPoints;
        std::uint8_t TechLevel;
        std::int8_t Armor;
        std::uint8_t HullType;
        std::uint8_t cpp_padding[1];
        std::int32_t HullSeries;
        void* OwnerShip;
        std::uint8_t CapitalShip;
        std::uint8_t PirateBuilt;
        std::uint8_t ImpulseShieldsEnabled;
        std::uint8_t InterceptorsEnabled;
        std::int32_t Energy;
        std::int32_t EnergyMax;
        void* InterceptorTarget;
        TInterceptorTargetingStrategy InterceptorTargetingStrategy;
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
        void Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::uint8_t CalculateGeneratedCapacity();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        void Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int32_t CalculateGeneratedSpeed();
        std::int8_t CalculateGeneratedJumpRange();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        void Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int32_t CalculateGeneratedRange();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        void Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int32_t CalculateGeneratedScanPower();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        void Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::uint8_t CalculateGeneratedRepairPoints();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        void Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::int32_t CalculateGeneratedPickupPower();
        std::int32_t CalculateGeneratedRange();
        float CalculateGeneratedMinPullSpeed();
        float CalculateGeneratedMaxPullSpeed();
        std::int32_t CalculateGeneratedCost();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        void Init(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        float CalculateGeneratedDamageFactor();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        void Init(aConst::TItemType ItemType, std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void Unequip() override;
        std::int32_t CalculateGeneratedAmmoCapacity();
        std::int32_t CalculateGeneratedMinDamage();
        std::int32_t CalculateGeneratedMaxDamage();
        std::int32_t CalculateGeneratedRange();
        std::int32_t CalculateStandardMaxDamage();
        std::int32_t CalculateStandardRange();
        void Improve(TImprovementKind Kind) override;
        std::uint8_t HasStandardStats() override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetShortName() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
        void ReplaceInfoTokens(pas::WideString& Text, pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        double GetShotDelayFactor();
        pas::WideString GetBitmapResourceName() override;
        std::uint8_t NeedsAmmo();
        std::int32_t CalculateAmmoRefillCost();
        std::int32_t GetShotPalette();
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
        pas::Object* Target;
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
        void InitGenerated(std::uint8_t TypeId, std::uint8_t Owner, std::uint32_t Seed);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        virtual void Init(std::uint8_t Owner, aConst::TItemType ItemType);
        pas::WideString GetBitmapResourceName() override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetOnUseCodeText();
        void* GetActionCode();
        aConst::TItemType GetEffectiveType();
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
        void LoadConfig(std::uint8_t ApplyConfiguredWeight);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetDescriptionText() override;
        aConst::TItemType CountsAsItemType;
        std::uint8_t SharedUse;
        std::uint8_t SharedEffect;
        std::uint8_t cpp_padding[1];
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
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        std::uint8_t IsDominatorRemains();
        void CheckIfWeDisplayAsArtefact();
        pas::WideString GetOnUseCodeText();
        void* GetActionCode();
        pas::WideString CustomText;
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
        void Init(std::int32_t ModuleIndex);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetPlainName();
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        std::int32_t CalculateNodeExchangeValue(std::int32_t LowPriorityOfferCost, std::int32_t MediumPriorityOfferCost);
        pas::WideString GetHighlightedName();
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
        void Init(std::int32_t Fuel, std::uint8_t Capacity, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString GetDisplayName() override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        std::int32_t GetUnitSize();
        TCountableItem* Split(std::int32_t Count);
        std::uint8_t CanMerge(pas::Object* Other);
        std::uint8_t Merge(pas::Object* Other);
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
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        void InitTranclucator(std::uint8_t Owner, void* OwnerShip, void* ExistingShip);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        aItem::TItem* Clone() override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        pas::WideString GetDisplayName() override;
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
        void InitTransmitter(std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
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
        pas::WideString GetInfoText(pas::WideString ColorTag, void* Ship) override;
        pas::WideString GetDescriptionText() override;
        pas::WideString GetBitmapResourceName() override;
        pas::WideString GetTargetPlanetName();
        static pas::WideString BuildPreviewTable(std::int32_t PageIndex, void* Planet);
        void* TargetPlanet;
        pas::WideString SourceShipName;
        pas::WideString PreviewTablePage1;
        pas::WideString PreviewTablePage2;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCustomWeapon : aItem::TWeapon {
        PAS_CLASS_META(TCustomWeapon, aItem::TWeapon, "TCustomWeapon", 132)
        void InitCustom(aConst::PWeaponInfo Info, std::uint8_t Equipped, std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
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

    inline constexpr std::int32_t EquipmentSlotIndexMask = 0x0000007f;

    inline constexpr std::int32_t EquipmentSecondaryFireFlag = 0x00000080;

} // namespace aItem
