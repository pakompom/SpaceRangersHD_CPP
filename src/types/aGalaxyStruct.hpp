#pragma once
#include "runtime_support.hpp"

namespace aGalaxyStruct {
    struct TGoodsTradePriceEntry;

    struct TPlanetBattleStatistics;

    struct TGalaxyCustomRules;

    struct TEngineLevelStats;

    struct TCargoHookLevelStats;

    struct TPlanetGoodsFactors;

    struct TPlanetRaceMarketInfo;

    struct TPlanetOwnerMasks;

    struct TQuestTuning;

    struct TGalaxyDifficultyTuning;

    struct TStarStatus;

    struct TPlanetNews;

    using TGalaxyDifficultyLevels = pas::Array<std::uint8_t, 0, 7>;

    // Raw saved settings. Accessors apply defaults when Enabled is false.
    // Most modifier bytes encode 0.5 + value / 16, rather than percentages.
    #pragma pack(push, 1)
    struct TGalaxyCustomRules {
        std::uint8_t Enabled;
        std::uint8_t DominatorStrength;
        std::uint8_t DominatorAggression;
        std::uint8_t DominatorSpawn;
        std::uint8_t PirateAggression;
        std::uint8_t CoalitionAggression;
        std::uint8_t AsteroidModifier;
        std::uint8_t SunDamageModifier;
        std::uint8_t ExtraInventions;
        std::uint8_t AcrynModifier;
        std::uint8_t NodeDropModifier;
        std::uint8_t ArcadeDropValueModifier;
        std::uint8_t DropValueModifier;
        // If all three economy weights are zero, their getters return one each.
        std::uint8_t AgriculturalPlanetWeight;
        std::uint8_t MixedPlanetWeight;
        std::uint8_t IndustrialPlanetWeight;
        std::uint8_t ExtraRangers;
        std::uint8_t ArcadeHitpointsModifier;
        std::uint8_t ArcadeDamageModifier;
        std::uint8_t AIJunkTolerance;
        std::uint8_t ChaoticRandom;
        std::uint8_t UnrestrictedEquipmentKnowledge;
        std::uint8_t StationsNearStars;
        std::uint8_t FullStationTargeting;
        std::uint8_t SpecialShips;
        std::uint8_t ZeroStartingExperience;
        std::uint8_t ArcadeBattleRoyale;
        std::uint8_t DominatorRacialWeapons;
        std::uint8_t StartInCenter;
        std::uint8_t MaxRangeMissiles;
        std::uint8_t OldHyperspace;
        std::uint8_t PirateNodes;
        std::uint8_t AIUseShops;
        std::uint8_t StationsUseShop;
        std::uint8_t DuplicateArtefacts;
        std::uint8_t HullGrowth;
        std::uint8_t ArcadeEquipmentChange;
        std::uint8_t OldSpeedCalculation;
        std::uint8_t OldMissileBonuses;
    };
    #pragma pack(pop)

    enum TStarFaction : std::uint8_t {
        sfCoalition = 0,
        sfDominators = 1,
        sfPirates = 2,
    };

    using TPercent = std::uint8_t;

    enum TDominatorSeries : std::uint8_t {
        dsBlazer = 0,
        dsKeller = 1,
        dsTerron = 2,
    };

    enum TQuestType : std::uint8_t {
        qtSendLetter = 0,
        qtKillShip = 1,
        qtPlanetQuest = 2,
        qtDefendSystem = 3,
        qtDefendShip = 4,
    };

    using TWeaponAvailabilityMask = pas::Set<0, 15>;

    // OwnerToSys () and RaceToSys () establish these IDs.
    // RaceId and PilotRace use the same Coalition values 0..4.
    enum TOwnerId : std::uint8_t {
        oiMaloc = 0,
        oiPeleng = 1,
        oiHuman = 2,
        oiFeyan = 3,
        oiGaal = 4,
        oiDominator = 5,
        oiUninhabited = 6,
        oiPirate = 7,
    };

    enum TStationType : std::uint8_t {
        rstRangerCenter = 6,
        rstPirateBase = 7,
        rstMilitaryBase = 8,
        rstScienceBase = 9,
        rstBusinessCenter = 10,
        rstMedicalBase = 11,
        rstDominion = 12,
        rstCustomStation = 13,
    };

    // Shared ship career category; non-ranger implementations can return a fixed career.
    enum TRangerCareer : std::uint8_t {
        rcTrader = 0,
        rcPirate = 1,
        rcWarrior = 2,
    };

    using TFactionStrengthValues = pas::Array<float, 0, 2>;

    struct TStarStatus {
        // 0..100.
        std::uint8_t ThreatLevel;
        // 0..100.
        std::uint8_t TrafficLevel;
        // Script.StarOwner.
        TStarFaction ControlFaction;
        std::uint8_t cpp_padding[1];
        pas::WideString CustomFaction;
        // Script.StarBattle.
        std::uint8_t Battle;
        // Script.StarSeries.
        TDominatorSeries DominatorSeries;
        TStarFaction PreviousControlFaction;
        std::uint8_t cpp_padding_2[1];
        // Coalition, Dominators/custom, pirates; indexed by TStarFaction.
        TFactionStrengthValues CachedFactionStrength;
        std::int32_t FactionStrengthCacheTurn;
    };

    using TShipTypeMask = pas::Set<0, 15>;

    using TRangerCareerSet = pas::Set<0, 2>;

    enum TPlanetEconomy : std::uint8_t {
        peAgricultural = 0,
        peMixed = 1,
        peIndustrial = 2,
    };

    enum TPlanetGovernment : std::uint8_t {
        pgAnarchy = 0,
        pgDictatorship = 1,
        pgMonarchy = 2,
        pgRepublic = 3,
        pgDemocracy = 4,
    };

    // Prices are from the visiting ship's perspective.
    #pragma pack(push, 1)
    struct TGoodsTradePriceEntry {
        std::int32_t Count;
        float PriceState;
        std::int32_t PurchasePrice;
        // Before the ship's Trading skill bonus.
        std::int32_t BaseSalePrice;
    };
    #pragma pack(pop)

    using TOwnerMask = pas::Set<0, 7>;

    enum TKlingType : std::uint8_t {
        ktBoss = 0,
        ktEquentor = 1,
        ktUrgant = 2,
        ktSmersh = 3,
        ktMenok = 4,
        ktShtip = 5,
        ktBertor = 6,
        ktKlig = 7,
    };

    enum TRelationLevel : std::uint8_t {
        rlHostile = 0,
        rlBad = 1,
        rlNormal = 2,
        rlGood = 3,
        rlExcellent = 4,
    };

    using TItemTypeMask = pas::Set<0, 79>;

    // Native record RTTI.
    struct TPlanetNews {
        std::uint32_t Id;
        std::int32_t Turn;
        std::uint8_t NewsType;
        std::uint8_t cpp_padding[3];
        pas::WideString Text;
    };

    using TDamageFlagSet = pas::Set<0, 19>;

    // Shared scalar configuration identifiers. Managed configuration records live in aConst.
    enum TWeaponShotType : std::uint8_t {
        wstNormal = 0,
        wstChain = 1,
        wstSplash = 2,
        wstExploder = 3,
        wstAreaDamage = 4,
        wstTorpedo = 5,
        wstMissile = 6,
        wstRocket = 7,
    };

    enum TWeaponAvailability : std::uint8_t {
        waFree = 0,
        waCoalitionOnly = 1,
        waPirateOnly = 2,
        waNotSold = 3,
        waNotSoldAndNodeRepair = 4,
        waMalocOnly = 5,
        waPelengOnly = 6,
        waPeopleOnly = 7,
        waFeiOnly = 8,
        waGaalOnly = 9,
        waSystemOnly = 10,
    };

    // The script singleton constructor proves an enum spanning three bytes.
    // Four-byte masks use bits 0..20; the exact enum upper bound within 19..23 is unresolved.
    // Bits 3..18 have semantic aliases below; their original enum spellings are not recovered.
    // Bit 19 blocks the repair droid in TShip.ApplyDamage.
    enum TDamageKind : std::uint8_t {
        dkEnergy = 0,
        dkSplinter = 1,
        dkMissile = 2,
        dkDroidBlock = 19,
    };

    using TStationStandingMask = pas::Set<0, 15>;

    using PGoodsTradePriceEntry = TGoodsTradePriceEntry*;

    enum TPilotSkill : std::uint8_t {
        psAccuracy = 0,
        psManeuverability = 1,
        psTechnical = 2,
        psTrading = 3,
        psCharisma = 4,
        psLeadership = 5,
    };

    using TQuestTypes = pas::Set<0, 4>;

    // MatrixGame's SRobotGameState / CGame.SaveResult ABI, also stored in battle history.
    #pragma pack(push, 1)
    struct TPlanetBattleStatistics {
        // Elapsed milliseconds; MatrixGame negates the winning side's time.
        std::int32_t SignedTimeMs;
        std::int32_t RobotsBuilt;
        std::int32_t RobotsDestroyed;
        std::int32_t TurretsBuilt;
        std::int32_t TurretsDestroyed;
        std::int32_t BuildingsDestroyed;
    };
    #pragma pack(pop)

    enum TShopUpdateMode : std::uint8_t {
        sumNormal = 0,
        sumDisabled = 1,
        sumEquipmentOnly = 2,
        sumGoodsOnly = 3,
    };

    using PPlanetBattleStatistics = TPlanetBattleStatistics*;

    // Preserve the full byte for native membership checks; selected series are 0..2.
    using TDominatorSeriesMask = pas::Set<0, 7>;

    // Native record RTTI.
    #pragma pack(push, 1)
    struct TEngineLevelStats {
        std::uint16_t Speed;
        std::int8_t JumpRange;
        std::uint8_t cpp_padding[1];
    };
    #pragma pack(pop)

    using TEngineLevelStatsTable = pas::Array<TEngineLevelStats, 1, 8>;

    #pragma pack(push, 1)
    struct TCargoHookLevelStats {
        std::int32_t PickupPower;
        std::int32_t Range;
        float MinPullSpeed;
        float MaxPullSpeed;
    };
    #pragma pack(pop)

    using TCargoHookLevelStatsTable = pas::Array<TCargoHookLevelStats, 1, 8>;

    using TRelationLevels = pas::Set<0, 4>;

    using TPlanetEconomies = pas::Set<0, 2>;

    using TPlanetGovernments = pas::Set<0, 4>;

    using TGreetingCountMask = pas::Set<0, 15>;

    // Native record RTTI.
    #pragma pack(push, 1)
    struct TGalaxyDifficultyTuning {
        // Also scales fuel prices.
        float GoodsEventDurationFactor;
        float QuestTimeAndExperienceFactor;
        // Player equipment degradation, indexed by DifficultyLevels[3].
        float EquipmentWearFactor;
        float InventionProgressScale;
        // Indexed by DifficultyLevels[7].
        float ArcadeRewardScale;
        float QuestMoneyFactor;
        // DifficultyLevels[1]; passed to InitializePlayerAtPlanet and ApplyCharacterPreset.
        std::int32_t StartingPlayerMoney;
        // DifficultyLevels[0]; sets the initial pirate-system selection count before placement exclusions.
        std::uint8_t InitialPirateControlPercent;
        std::uint8_t cpp_padding[3];
        float MarketPriceBandSqueeze;
        // Roll 0..maximum; zero creates a hole when other conditions allow.
        std::int32_t RandomHoleSpawnRollMaximum;
        // Before the per-series multiplier.
        float MaximumDominatorResearchRate;
        // Inclusive random upper bound per consumption event.
        std::uint8_t MaximumResearchMaterialConsumption;
        // Upper end before owned-program scaling.
        std::uint8_t MaximumQuestProgramRewardCount;
        std::uint8_t cpp_padding_2[2];
        // Indexed by DifficultyLevels[6].
        float ArcadeDamageTakenScale;
        // DifficultyLevels[0]; normalizes the system-count ratio for daily WarDeltaWin adjustments.
        float CoalitionToPirateBalanceRatio;
    };
    #pragma pack(pop)

    using TGalaxyDifficultyTuningTable = pas::Array<TGalaxyDifficultyTuning, 0, 9>;

    #pragma pack(push, 1)
    struct TPlanetGoodsFactors {
        double PriceFactor;
        double StockFactor;
    };
    #pragma pack(pop)

    using TByteMask = pas::Set<0, 7>;

    using TQuestExperienceTable = pas::Array<std::int32_t, 0, 4>;

    #pragma pack(push, 1)
    struct TQuestTuning {
        std::uint8_t RewardCapitalPercent;
        std::uint8_t cpp_padding[3];
        std::int32_t BaseDuration;
        std::int32_t BaseRewardMoney;
    };
    #pragma pack(pop)

    using TQuestTuningTable = pas::Array<TQuestTuning, 0, 4>;

    #pragma pack(push, 1)
    struct TPlanetOwnerMasks {
        // Coalition owner IDs 0..4.
        TOwnerMask Coalition;
        // Dominator owner ID 5.
        TOwnerMask Dominators;
        // Pirate Clan owner ID 7.
        TOwnerMask PirateClan;
    };
    #pragma pack(pop)

    using TOwnerRelationRow = pas::Array<std::uint8_t, 0, 7>;

    using TOwnerRelationTable = pas::Array<TOwnerRelationRow, 0, 7>;

    #pragma pack(push, 1)
    struct TPlanetRaceMarketInfo {
        // Used by TPlanet.CalculateInventionProgressRate.
        float InventionProgressScale;
        std::int32_t InitialInventionBoostCount;
        pas::Array<TPlanetGoodsFactors, 0, 7> GoodsFactors;
        // Cumulative thresholds indexed by TPlanetGovernment.
        pas::Array<std::uint8_t, 0, 4> GovernmentRollThresholds;
        std::uint8_t cpp_padding[3];
        float RevolutionChance;
        // Scales transport-to-transport relations () and partner-gift gains ().
        float FriendlyRelationScale;
        // Multiplies owner-table relations for pirates at Coalition planets.
        float PirateRelationFactor;
        // Native race factor; gameplay meaning unresolved.
        float UnknownFactor9C;
        // Upper bound before the fixed minimum relation of 30.
        std::uint8_t PirateRelationCeiling;
        std::uint8_t cpp_padding_2[7];
    };
    #pragma pack(pop)

    using TPlanetRaceMarketTable = pas::Array<TPlanetRaceMarketInfo, 0, 4>;

    using TFactionStandingMasks = pas::Array<TStationStandingMask, 0, 2>;

    // Native record RTTI.
    // Native record RTTI.
    // Nine quotas per Coalition race: types 42..49, then the shared weapon bucket 50.
    using TPlanetEquipmentOfferQuotaRow = pas::Array<std::int32_t, 0, 8>;

    using TPlanetEquipmentOfferQuotaTable = pas::Array<TPlanetEquipmentOfferQuotaRow, 0, 4>;

    using TGalaxyDifficultyIndex = std::uint8_t;

    using TGoodsTextOrder = pas::Array<std::uint8_t, 0, 7>;

    using TProgramIndex = std::uint8_t;

    // TShip.TypeId names from ShipTypeNames (initialized by the table
    // at) and the subclass initializers.
    // These are distinct from the hull-generation codes returned by ShipToHullType.
    inline constexpr std::int32_t stKling = 0;

    inline constexpr std::int32_t stRanger = 1;

    inline constexpr std::int32_t stTransport = 2;

    inline constexpr std::int32_t stPirate = 3;

    inline constexpr std::int32_t stWarrior = 4;

    inline constexpr std::int32_t stTranclucator = 5;

    // CurrentStanding categories: native GetControlPresence (),
    // ResetControlFaction (), and subclass RefreshCurrentStanding methods.
    // The companion ShipStanding reference uses CoalMilitary/Active/Passive and
    // PirateMilitary/Active/Passive for the same IDs. Keep the stored Byte ABI.
    inline constexpr std::int32_t ssDominator = 0;

    inline constexpr std::int32_t ssUnaligned = 1;

    inline constexpr std::int32_t ssCoalitionMilitary = 2;

    inline constexpr std::int32_t ssCoalitionActive = 3;

    inline constexpr std::int32_t ssCoalitionPassive = 4;

    inline constexpr std::int32_t ssNeutral = 5;

    inline constexpr std::int32_t ssPiratePassive = 6;

    inline constexpr std::int32_t ssPirateActive = 7;

    inline constexpr std::int32_t ssPirateMilitary = 8;

    inline constexpr std::int32_t ssCustom = 9;

    // GetScriptStandingOverrideMode (); its SubFaction test is preserved.
    inline constexpr std::int32_t ssmNormal = 0;

    inline constexpr std::int32_t ssmCustomFaction = 1;

    inline constexpr std::int32_t ssmFixed = 2;

    // Greeting category bits from InitializeShipGreetingDefinitions () and
    // TShip virtual slot $30. Transport subtypes and pirate allegiance have
    // separate greeting categories; these values are distinct from TypeId.
    inline constexpr std::int32_t gscTransport = 0;

    inline constexpr std::int32_t gscLiner = 1;

    inline constexpr std::int32_t gscDiplomat = 2;

    inline constexpr std::int32_t gscRanger = 3;

    inline constexpr std::int32_t gscPirate = 4;

    inline constexpr std::int32_t gscWarrior = 5;

    inline constexpr std::int32_t gscKling = 6;

    inline constexpr std::int32_t gscPirateClan = 7;

    // Hull categories from ShipToHullType (), GetDefaultHullType (),
    // and ApplySpecialMicroModule (). They are not TShip.TypeId values.
    inline constexpr std::int32_t htRanger = 0;

    inline constexpr std::int32_t htWarrior = 1;

    inline constexpr std::int32_t htPirate = 2;

    inline constexpr std::int32_t htTransport = 3;

    inline constexpr std::int32_t htLiner = 4;

    inline constexpr std::int32_t htDiplomat = 5;

    inline constexpr std::int32_t htKling = 6;

    inline constexpr std::int32_t htTranclucator = 7;

    inline constexpr std::int32_t htStation = 8;

    inline constexpr std::int32_t htSpecial = 9;

    inline constexpr std::int32_t htFlagship = 10;

    // Used by ranger inventory, Dominator effects, and script prog* identifiers.
    inline constexpr std::int32_t prgKellerCall = 0;

    inline constexpr std::int32_t prgLogicalNegation = 1;

    inline constexpr std::int32_t prgDematerial = 2;

    inline constexpr std::int32_t prgEnergotron = 3;

    inline constexpr std::int32_t prgSabCrack = 4;

    inline constexpr std::int32_t prgIntercom = 5;

    inline constexpr std::int32_t prgShipwreck = 6;

    inline constexpr std::int32_t prgWeaponBlocking = 7;

    inline constexpr std::int32_t prgInsanity = 8;

    inline constexpr std::int32_t prgShock = 9;

    inline constexpr std::int32_t prgSelfDestruction = 10;

    inline constexpr std::int32_t prgDisconnection = 11;

    // CoalitionProjectNames (), investment dispatch (),
    // and the military-base war operation () share these cooldown indices.
    inline constexpr std::int32_t cpCreateRangerCenter = 0;

    inline constexpr std::int32_t cpCreatePirateBase = 1;

    inline constexpr std::int32_t cpCreateMilitaryBase = 2;

    inline constexpr std::int32_t cpCreateScienceBase = 3;

    inline constexpr std::int32_t cpCreateBusinessCenter = 4;

    inline constexpr std::int32_t cpCreateMedicalBase = 5;

    inline constexpr std::int32_t cpRangersSubsidy = 6;

    inline constexpr std::int32_t cpPiratesSubsidy = 7;

    inline constexpr std::int32_t cpTransportSubsidy = 8;

    inline constexpr std::int32_t cpLostSubsidy = 9;

    inline constexpr std::int32_t cpWarSubsidy = 10;

    inline constexpr std::int32_t cpWarOperation = 11;

    // Conversation IDs shared by ShowPlayerDialogue (),
    // TfTalk.BuildBuiltinChoices (), and script Talk* constants.
    inline constexpr std::int32_t tkMoneyDemand = 0;

    inline constexpr std::int32_t tkGoodsDemand = 1;

    inline constexpr std::int32_t tkTruceOffer = 2;

    inline constexpr std::int32_t tkAttack = 3;

    inline constexpr std::int32_t tkPartnerBreak = 4;

    inline constexpr std::int32_t tkPartnerEnd = 5;

    inline constexpr std::int32_t tkPartnerRiot = 6;

    // Award categories from SysToReward (); SelectAward returns $FF on failure.
    inline constexpr std::int32_t atLiberation = 0;

    inline constexpr std::int32_t atAccomplishment = 1;

    inline constexpr std::int32_t atSecretMission = 2;

    inline constexpr std::int32_t atCowardice = 3;

    inline constexpr std::int32_t atPerfidy = 4;

    inline constexpr std::int32_t atPlanetBattle = 5;

    inline constexpr std::int32_t AwardNotFound = 0x000000ff;

    // Semantic aliases follow WeaponDamageFlagNames and the native
    // TShip.ApplyDamage handlers. They do not claim original enum RTTI names.
    // Keep the enum bounds and four-byte set ABI above unchanged.
    inline constexpr aGalaxyStruct::TDamageKind dkDecelerate = static_cast<aGalaxyStruct::TDamageKind>(3);

    inline constexpr aGalaxyStruct::TDamageKind dkDestruct = static_cast<aGalaxyStruct::TDamageKind>(4);

    inline constexpr aGalaxyStruct::TDamageKind dkDrain = static_cast<aGalaxyStruct::TDamageKind>(5);

    inline constexpr aGalaxyStruct::TDamageKind dkShock = static_cast<aGalaxyStruct::TDamageKind>(6);

    inline constexpr aGalaxyStruct::TDamageKind dkAcid = static_cast<aGalaxyStruct::TDamageKind>(7);

    inline constexpr aGalaxyStruct::TDamageKind dkMagnetic = static_cast<aGalaxyStruct::TDamageKind>(8);

    inline constexpr aGalaxyStruct::TDamageKind dkDecelerateA = static_cast<aGalaxyStruct::TDamageKind>(9);

    inline constexpr aGalaxyStruct::TDamageKind dkDecelerateAEx = static_cast<aGalaxyStruct::TDamageKind>(10);

    inline constexpr aGalaxyStruct::TDamageKind dkUndefendable = static_cast<aGalaxyStruct::TDamageKind>(11);

    inline constexpr aGalaxyStruct::TDamageKind dkNonLethal = static_cast<aGalaxyStruct::TDamageKind>(12);

    inline constexpr aGalaxyStruct::TDamageKind dkScanBonus = static_cast<aGalaxyStruct::TDamageKind>(13);

    inline constexpr aGalaxyStruct::TDamageKind dkBonusToDamaged = static_cast<aGalaxyStruct::TDamageKind>(14);

    inline constexpr aGalaxyStruct::TDamageKind dkMoreDrop = static_cast<aGalaxyStruct::TDamageKind>(15);

    inline constexpr aGalaxyStruct::TDamageKind dkDropCargo = static_cast<aGalaxyStruct::TDamageKind>(16);

    inline constexpr aGalaxyStruct::TDamageKind dkReduceEngine = static_cast<aGalaxyStruct::TDamageKind>(17);

    inline constexpr aGalaxyStruct::TDamageKind dkBlockWeapon = static_cast<aGalaxyStruct::TDamageKind>(18);

    // Bit 20 fixes minimum damage at the maximum (TShip.GetWeaponMinDamage).
    inline constexpr std::int32_t DamageNoDeltaMask = 1 << 20;

} // namespace aGalaxyStruct
