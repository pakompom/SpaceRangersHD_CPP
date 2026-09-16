#pragma once
#include "runtime_support.hpp"
#include "types/aGalaxyStruct.hpp"

namespace aConst {
    struct TMicroModuleInfo;

    struct TWeaponInfo;

    struct TStationEquipmentOfferQuota;

    struct THullLevelStats;

    struct TEconomyInfo;

    struct TRelationTypeInfo;

    struct TShipTypeInfo;

    struct TStatusInfo;

    struct TKlingTypeInfo;

    struct SEquipment;

    struct TGoodsInfo;

    struct TOwnerInfo;

    struct TGovermentInfo;

    struct TRewardInfo;

    struct TPrimaryDamageTypeInfo;

    struct THullTypeInfo;

    struct tInventionInfo;

    struct TIllnessInfo;

    using PWeaponInfo = TWeaponInfo*;

    enum TItemType : std::uint8_t {
        t_Food = 0,
        t_Medicine = 1,
        t_Technics = 2,
        t_Luxury = 3,
        t_Minerals = 4,
        t_Alcohol = 5,
        t_Arms = 6,
        t_Narcotics = 7,
        t_Artefact = 8,
        t_Artefact2 = 9,
        t_ArtefactHull = 10,
        t_ArtefactFuel = 11,
        t_ArtefactSpeed = 12,
        t_ArtefactPower = 13,
        t_ArtefactRadar = 14,
        t_ArtefactScaner = 15,
        t_ArtefactDroid = 16,
        t_ArtefactNano = 17,
        t_ArtefactHook = 18,
        t_ArtefactDef = 19,
        t_ArtefactAnalyzer = 20,
        t_ArtefactMiniExpl = 21,
        t_ArtefactAntigrav = 22,
        t_ArtefactTransmitter = 23,
        t_ArtefactBomb = 24,
        t_ArtefactTranclucator = 25,
        t_ArtDefToEnergy = 26,
        t_ArtEnergyPulse = 27,
        t_ArtEnergyDef = 28,
        t_ArtSplinter = 29,
        t_ArtDecelerate = 30,
        t_ArtMissileDef = 31,
        t_ArtForsage = 32,
        t_ArtWeaponToSpeed = 33,
        t_ArtGiperJump = 34,
        t_ArtBlackHole = 35,
        t_ArtDefToArms1 = 36,
        t_ArtDefToArms2 = 37,
        t_ArtArtefactor = 38,
        t_ArtBio = 39,
        t_ArtPDTurret = 40,
        t_ArtFastRacks = 41,
        t_Hull = 42,
        t_FuelTanks = 43,
        t_Engine = 44,
        t_Radar = 45,
        t_Scaner = 46,
        t_RepairRobot = 47,
        t_CargoHook = 48,
        t_DefGenerator = 49,
        t_Weapon1 = 50,
        t_Weapon2 = 51,
        t_Weapon3 = 52,
        t_Weapon4 = 53,
        t_Weapon5 = 54,
        t_Weapon6 = 55,
        t_Weapon7 = 56,
        t_Weapon8 = 57,
        t_Weapon9 = 58,
        t_Weapon10 = 59,
        t_Weapon11 = 60,
        t_Weapon12 = 61,
        t_Weapon13 = 62,
        t_Weapon14 = 63,
        t_Weapon15 = 64,
        t_Weapon16 = 65,
        t_Weapon17 = 66,
        t_Weapon18 = 67,
        t_CustomWeapon = 68,
        t_Protoplasm = 69,
        t_UselessItem = 70,
        t_MicroModule = 71,
        t_Cistern = 72,
        t_Satellite = 73,
        t_TreasureMap = 74,
        t_UselessCountableItem = 75,
    };

    #pragma pack(push, 1)
    struct TWeaponInfo {
        TItemType ItemType;
        std::uint8_t cpp_padding[3];
        pas::WideString ConfigName;
        std::uint8_t TechLevel;
        std::uint8_t InventionIndex;
        std::uint8_t cpp_padding_2[2];
        float CostFactor;
        std::int32_t MinDamage;
        std::int32_t MaxDamage;
        std::int32_t AverageSize;
        std::int32_t AverageRange;
        std::int32_t ShotSpeedPercent;
        std::int32_t MissileRange;
        std::int32_t MissileMaxSpeed;
        std::int32_t MissileMinSpeed;
        std::uint8_t MissileChanceToBeHit;
        std::uint32_t DamageFlags;
        aGalaxyStruct::TWeaponShotType ShotType;
        std::uint8_t ShotCount;
        std::uint8_t AttackCount;
        float SecondaryDamageRadius;
        float MiningFactor;
        pas::Array<float, 1, 8> DamageScaleByLevel;
        pas::WideString PrimarySE;
        pas::WideString SecondarySE;
        pas::WideString AreaSE;
        std::int32_t DefaultPalette;
        aGalaxyStruct::TWeaponAvailability Availability;
        std::uint8_t ArcadeWeaponType;
        std::uint8_t cpp_padding_3[2];
        std::uint32_t TypeHash;
    };
    #pragma pack(pop)

    enum TEquipmentBonusKind : std::uint8_t {
        bonHull = 0,
        bonFuel = 1,
        bonSpeed = 2,
        bonJump = 3,
        bonRadar = 4,
        bonScan = 5,
        bonDroid = 6,
        bonHook = 7,
        bonDef = 8,
        bonWEnergy = 9,
        bonWSplinter = 10,
        bonWMissile = 11,
        bonWRadius = 12,
        bonSlotRadar = 13,
        bonSlotScaner = 14,
        bonSlotDroid = 15,
        bonSlotHook = 16,
        bonSlotDef = 17,
        bonSlotWeapon = 18,
        bonSlotArt = 19,
        bonSlotForsage = 20,
        bonHookRadius = 21,
        bonSkill1 = 22,
        bonSkill2 = 23,
        bonSkill3 = 24,
        bonSkill4 = 25,
        bonSkill5 = 26,
        bonSkill6 = 27,
        bonMass = 28,
        bonExtraAkrinEff = 29,
        bonExtraAkrinPenalty = 30,
        bonAmmo = 31,
        bonShots = 32,
        bonMissileSpeed = 33,
        bonShotSpeed = 34,
        bonHookMaxSpeed = 35,
        bonHookMinSpeed = 36,
        bonStimCapacity = 37,
        bonZonds = 38,
        bonAttacks = 39,
        bonResistAsteroid = 40,
        bonAIValue = 41,
        bonNull = 42,
    };

    enum TShipSlotKind : std::uint8_t {
        sskFuelTanks = 0,
        sskEngine = 1,
        sskRadar = 2,
        sskScanner = 3,
        sskRepairRobot = 4,
        sskCargoHook = 5,
        sskDefGenerator = 6,
        sskWeapon = 7,
        sskArtefact = 8,
        sskAfterburner = 9,
        sskUnsupported = 10,
    };

    #pragma pack(push, 1)
    struct TRewardInfo {
        std::uint8_t AwardId;
        std::uint8_t cpp_padding[3];
        pas::WideString Name;
        pas::WideString Text;
    };
    #pragma pack(pop)

    using THullShipTypeMask = pas::Set<0, 15>;

    using TEquipmentBonuses = pas::Array<std::int32_t, 0, 42>;

    #pragma pack(push, 1)
    struct TMicroModuleInfo {
        std::uint8_t SpecialOnly;
        std::uint8_t BlocksMicroModuleSlot;
        std::uint8_t BlocksSpecialSlot;
        std::uint8_t cpp_padding[1];
        pas::WideString Name;
        pas::WideString NamePrefix;
        pas::WideString Color;
        pas::WideString TextReplace;
        TEquipmentBonuses StatBonuses;
        std::int32_t CostPercent;
        std::int32_t SizePercent;
        float FragilityFactor;
        pas::Array<float, 0, 2> FragilityFactorByDamageClass;
        std::uint8_t Priority;
        std::uint8_t AllowedHullOwnerMask;
        std::uint8_t cpp_padding_2[2];
        pas::WideString AllowedCustomHullFactions;
        pas::WideString CustomFaction;
        std::uint8_t AllowedDominatorSeriesMask;
        pas::Array<std::uint8_t, 0, 9> AllowedItemTypes;
        std::uint8_t cpp_padding_3[1];
        pas::WideString AllowedCustomWeaponTypes;
        std::uint16_t OfferStationTypes;
        std::uint8_t cpp_padding_4[2];
        pas::WideString OfferStationNames;
        std::uint8_t OnPlanets;
        std::uint8_t RacialRestriction;
        std::uint8_t SeparatedNumbers;
        std::uint8_t cpp_padding_5[1];
        std::int32_t ConfigNumber;
        pas::WideString ConfigName;
        std::uint32_t ConfigNameHash;
        pas::WideString KindGraph;
        pas::WideString MissileGraph;
        std::int32_t ShotVisual;
        std::int32_t HullGraphSizePercent;
        pas::WideString CustomTag;
        std::uint32_t WeaponDamageFlags;
    };
    #pragma pack(pop)

    using TEquipmentSizeFactorTable = pas::Array<float, 1, 5>;

    enum TWeaponDamageClass : std::uint8_t {
        wdcEnergy = 0,
        wdcSplinter = 1,
        wdcMissile = 2,
    };

    struct TPrimaryDamageTypeInfo {
        TWeaponDamageClass Kind;
        std::uint8_t BonusKind;
        std::uint8_t cpp_padding[2];
        pas::WideString Name;
    };

    #pragma pack(push, 1)
    struct THullLevelStats {
        std::uint8_t Armor;
        std::uint8_t cpp_padding[3];
        pas::Array<float, 0, 2> Fragility;
    };
    #pragma pack(pop)

    using THullLevelStatsTable = pas::Array<THullLevelStats, 1, 8>;

    #pragma pack(push, 1)
    struct THullTypeInfo {
        pas::WideString Name;
        pas::WideString Text;
        aGalaxyStruct::TOwnerMask AllowedOwners;
        THullShipTypeMask AllowedShipTypes;
        std::uint8_t cpp_padding[1];
        pas::Array<std::int32_t, 0, 10> SlotBonuses;
        std::int32_t SizePercent;
        std::int32_t CostPercent;
        float FragilityFactor;
        pas::Array<float, 0, 2> FragilityByDamageClass;
        std::uint8_t Year;
        std::uint8_t cpp_padding_2[3];
        std::int32_t ProbabilityWeight;
        std::int32_t SortKey;
        pas::WideString SystemName;
        std::uint32_t SystemNameCRC;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TOwnerInfo {
        pas::WideString InternalName;
        pas::WideString DisplayName;
        float FuelPriceFactor;
        float EquipmentDurabilityFactor;
        std::int32_t MinimumAfterburnerWear;
        std::int32_t MaximumAfterburnerWear;
        float FearThresholdScale;
        pas::WideString ColorTag;
    };
    #pragma pack(pop)

    using TWeaponRangeLevelFactors = pas::Array<float, 1, 8>;

    #pragma pack(push, 1)
    struct TGoodsInfo {
        pas::WideString InternalName;
        pas::WideString DisplayName;
        pas::WideString TradeName;
        std::int32_t BaseStock;
        std::int32_t MinPrice;
        std::int32_t AveragePrice;
        std::int32_t MaxPrice;
        float TradeExperienceFactor;
        pas::Array<float, 0, 2> EconomyFactors;
        float PirateEconomyFactor;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TGovermentInfo {
        pas::WideString InternalName;
        pas::WideString DisplayName;
        pas::Array<std::int8_t, 0, 2> RevolutionRelationDelta;
        std::uint8_t cpp_padding[1];
        pas::Array<float, 0, 4> QuestOfferProbabilities;
        pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7> GoodsFactors;
    };
    #pragma pack(pop)

    struct TRelationTypeInfo {
        pas::WideString InternalName;
        pas::WideString DisplayName;
        std::int32_t MinimumValue;
    };

    #pragma pack(push, 1)
    struct TKlingTypeInfo {
        pas::Array<pas::WideString, 0, 2> DisplayNames;
        std::int32_t MinimumHullSize;
        std::int32_t MaximumHullSize;
        std::uint8_t cpp_padding[4];
        double InitialWealthScale;
        std::uint16_t BaseNodeReserve;
        std::uint16_t KillExperience;
        std::uint16_t RankPoints;
        std::uint16_t PirateRankPoints;
        std::int32_t RankImageIndex;
        std::uint8_t cpp_padding_2[4];
        double FactionStrengthWeight;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TEconomyInfo {
        pas::WideString InternalName;
        pas::WideString DisplayName;
        pas::WideString ShortDisplayName;
        float InventionProgressScale;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TIllnessInfo {
        pas::WideString Name;
        pas::WideString Text;
        aGalaxyStruct::TOwnerMask AllowedLocationOwners;
        aGalaxyStruct::TOwnerMask AllowedOwners;
        aGalaxyStruct::TOwnerMask AllowedRatingBands;
        aGalaxyStruct::TOwnerMask AllowedRanks;
        aGalaxyStruct::TOwnerMask AllowedCareers;
        std::uint8_t EffectClass0D;
        std::uint8_t cpp_padding[2];
        double DevelopmentRate;
        double InfectionChance;
        aGalaxyStruct::TOwnerMask Locations;
        std::uint8_t Disabled;
        std::uint8_t cpp_padding_2[2];
        std::int32_t Duration;
    };
    #pragma pack(pop)

    using TRadiationHealthDefinitions = pas::Array<TIllnessInfo, 1, 1>;

    using TItemTypeSelection = pas::Set<0, 79>;

    struct TShipTypeInfo {
        pas::WideString Name;
    };

    #pragma pack(push, 1)
    struct SEquipment {
        TItemType ItemType;
        std::uint8_t cpp_padding[3];
        pas::WideString Name;
    };
    #pragma pack(pop)

    using TGoodsLegalityTable = pas::Array<pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>, 0, 7>;

    using PMicroModuleTemplate = TMicroModuleInfo*;

    using TProgramDurationTable = pas::Array<std::int32_t, 0, 11>;

    #pragma pack(push, 1)
    struct TStationEquipmentOfferQuota {
        std::int32_t Hulls;
        std::int32_t FuelTanks;
        std::int32_t Engines;
        std::int32_t Radars;
        std::int32_t Scanners;
        std::int32_t RepairRobots;
        std::int32_t CargoHooks;
        std::int32_t DefGenerators;
        std::int32_t Weapons;
    };
    #pragma pack(pop)

    using TStationEquipmentOfferQuotaTable = pas::Array<TStationEquipmentOfferQuota, 6, 13>;

    using TEquipmentInventionIndexTable = pas::Array<std::uint8_t, 42, 49>;

    using TOwnerWeaponAvailabilityTable = pas::Array<aGalaxyStruct::TWeaponAvailability, 0, 7>;

    #pragma pack(push, 1)
    struct TStatusInfo {
        pas::WideString Name;
        std::uint8_t cpp_padding[4];
        double MinimumWealthToAverageRatio;
        double MinimumWealthToBestRatio;
        double MinimumStrengthToAverageRatio;
        double MinimumStrengthToBestRatio;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct tInventionInfo {
        pas::WideString Name;
        std::uint8_t InitialLevel;
        std::uint8_t RequiredMainTechLevel;
        std::uint8_t cpp_padding[2];
    };
    #pragma pack(pop)

    inline constexpr std::int32_t satOnStep = 0;

    inline constexpr std::int32_t satOnWeaponShot = 1;

    inline constexpr std::int32_t satOnMissileShot = 2;

    inline constexpr std::int32_t satOnDealingDamage = 3;

    inline constexpr std::int32_t satOnDealingFatalDamage = 4;

    inline constexpr std::int32_t satOnDealingKamikazeDamage = 5;

    inline constexpr std::int32_t satOnTakingDamage = 6;

    inline constexpr std::int32_t satOnWeaponShot2 = 10;

    inline constexpr std::int32_t satOnMissileShot2 = 11;

    inline constexpr std::int32_t satOnGettingWeaponHit = 12;

    inline constexpr std::int32_t satOnGettingMissileHit = 13;

    inline constexpr std::int32_t satOnDroidRepair = 14;

    inline constexpr std::int32_t satOnItemPickUp = 15;

    inline constexpr std::int32_t satOnChameleonConfusion = 17;

    inline constexpr std::int32_t satOnAnotherItem = 19;

    inline constexpr std::int32_t satOnAnotherItem2 = 20;

    inline constexpr std::int32_t satOnAnotherGoods = 21;

    inline constexpr std::int32_t satOnItemHit = 22;

    inline constexpr std::int32_t satOnMissileHittingObject = 23;

    inline constexpr std::int32_t satOnEnteringForm = 24;

    inline constexpr std::int32_t satOnLeavingForm = 25;

    inline constexpr std::int32_t satOnReEnteringForm = 26;

    inline constexpr std::int32_t satOnEnteringOtherShip = 27;

    inline constexpr std::int32_t satOnReEnteringOtherShip = 29;

    inline constexpr std::int32_t satOnPlayerSkillIncrease = 30;

    inline constexpr std::int32_t satOnPlayerTalkedWithShip = 31;

    inline constexpr std::int32_t satOnShipTalkedWithPlayer = 32;

    inline constexpr std::int32_t satOnDropItem = 33;

    inline constexpr std::int32_t satOnDropItemFixed = 34;

    inline constexpr std::int32_t satOnMovingItemToStorage = 35;

    inline constexpr std::int32_t satOnReduceEqBattle = 36;

    inline constexpr std::int32_t satOnReduceEqUse = 37;

    inline constexpr std::int32_t satOnReduceEqForce = 38;

    inline constexpr std::int32_t satOnReduceEqForsage = 39;

    inline constexpr std::int32_t satOnItemDestroy = 40;

    inline constexpr std::int32_t satOnPlayerChangeHull = 41;

    inline constexpr std::int32_t satOnPlayerUseMM = 42;

    inline constexpr std::int32_t satOnPlayerBuyEq = 43;

    inline constexpr std::int32_t satOnItemEquip = 44;

    inline constexpr std::int32_t satOnItemDeEquip = 45;

    inline constexpr std::int32_t satOnTrancPacking = 46;

    inline constexpr std::int32_t satOnShipBuysGoods = 47;

    inline constexpr std::int32_t satOnShipSellsGoods = 48;

    inline constexpr std::int32_t satOnShowingItemInfo = 49;

    inline constexpr std::int32_t satOnShowingShipInfo = 50;

    inline constexpr std::int32_t satOnShowingStarInfo = 51;

    inline constexpr std::int32_t satOnNonStandartEqChange = 52;

    inline constexpr std::int32_t satOnABItemDrop = 56;

    inline constexpr std::int32_t satOnGovItemReward = 57;

    inline constexpr std::int32_t satOnCheckingUsability = 58;

    inline constexpr std::int32_t satOnCheckingUsability2 = 59;

    inline constexpr std::int32_t satOnCheckingUsabilityGoods = 60;

    inline constexpr std::int32_t satOnDeath = 61;

} // namespace aConst
