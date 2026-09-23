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

    using TDifficultyLevel = std::uint8_t;

    using TGalaxyDifficultyLevels = pas::Array<TDifficultyLevel, 0, 7>;

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

    // GalaxyNews producers and CountPlanetNewsByType () share these IDs.
    // Distinct from pm* player-message presentation kinds.
    enum TGalaxyNewsKind : std::uint8_t {
        gnScript = 0,
        gnRevolutionAnarchy = 1,
        gnRevolutionDictatorship = 2,
        gnRevolutionMonarchy = 3,
        gnRevolutionRepublic = 4,
        gnRevolutionDemocracy = 5,
        gnMineralDeposit = 6,
        gnMineralShortage = 7,
        gnArmsSurplus = 8,
        gnArmsShortage = 9,
        gnTechnicsSurplus = 10,
        gnFoodSurplus = 11,
        gnFoodShortage = 12,
        gnMedicineSurplus = 13,
        gnLuxurySurplus = 14,
        gnLuxuryShortage = 15,
        gnAlcoholSurplus = 16,
        gnAlcoholShortage = 17,
        gnTransportActivity = 18,
        gnManyPirates = 19,
        gnSomePirates = 20,
        gnNoPirates = 21,
        gnManyRangers = 22,
        gnEminentRangerLocation = 23,
        gnDominatorAttack = 24,
        gnDominatorAttackRepelled = 25,
        gnLiberationGroupCreated = 26,
        gnPirateAttack = 27,
        gnPirateAttackRepelled = 28,
        gnCoalitionTakesDominatorSystem = 29,
        gnCoalitionTakesPirateSystem = 30,
        gnPiratesTakeDominatorSystem = 31,
        gnPiratesTakeCoalitionSystem = 32,
        gnDominatorsTakeCoalitionSystem = 33,
        gnDominatorsTakePirateSystem = 34,
        gnCoalitionDefeated = 35,
        gnWormholeCreated = 36,
        gnEminentWarrior = 37,
        gnEminentTrader = 38,
        gnEminentPirate = 39,
        gnImprisonment = 40,
        gnStationCreated = 41,
        gnCoalitionInvestment = 42,
        gnDominatorResearchCompleted = 43,
        gnStationSpecialShip = 44,
        gnMilitaryBaseOperation = 45,
    };

    using TWeaponAvailabilityMask = pas::Set<0, 10>;

    // Hull categories differ from TShip.TypeId; transports have three hull kinds.
    enum THullType : std::uint8_t {
        htRanger = 0,
        htWarrior = 1,
        htPirate = 2,
        htTransport = 3,
        htLiner = 4,
        htDiplomat = 5,
        htKling = 6,
        htTranclucator = 7,
        htStation = 8,
        htSpecial = 9,
        htFlagship = 10,
    };

    using TStationType = std::uint8_t;

    // Shared ship career category; non-ranger implementations can return a fixed career.
    enum TRangerCareer : std::uint8_t {
        rcTrader = 0,
        rcPirate = 1,
        rcWarrior = 2,
    };

    // ShipTypeNames and subclass initializers share this ship/station domain.
    enum TShipType : std::uint8_t {
        stKling = 0,
        stRanger = 1,
        stTransport = 2,
        stPirate = 3,
        stWarrior = 4,
        stTranclucator = 5,
        rstRangerCenter = 6,
        rstPirateBase = 7,
        rstMilitaryBase = 8,
        rstScienceBase = 9,
        rstBusinessCenter = 10,
        rstMedicalBase = 11,
        rstDominion = 12,
        rstCustomStation = 13,
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

    using TShipTypeMask = pas::Set<0, 13>;

    using TRangerCareerSet = pas::Set<0, 2>;

    // English ShipType.Dominator entries name the same seven ordinary types in every series.
    // Type zero selects the series boss: Blazer, Keller or Terron.
    enum TKlingType : std::uint8_t {
        ktBoss = 0,
        ktEquantor = 1,
        ktUrgant = 2,
        ktSmersh = 3,
        ktMenoc = 4,
        ktShtip = 5,
        ktBertor = 6,
        ktKlig = 7,
    };

    // PlanetInventionInfo () and EquipmentInventionIndices ().
    // Research-track IDs are distinct from item types and numeric technology levels.
    // Weapon tracks use English Items.Weapon.Name names; later weapons can share a track.
    enum TPlanetInvention : std::uint8_t {
        piHull = 0,
        piFuelTanks = 1,
        piEngine = 2,
        piRadar = 3,
        piScanner = 4,
        piRepairRobot = 5,
        piCargoHook = 6,
        piMainTech = 7,
        piIndustrialLaser = 8,
        piFragmentationCannon = 9,
        piFlux = 10,
        piMissileLauncher = 11,
        piTreton = 12,
        piWavePhaser = 13,
        piFlowBlaster = 14,
        piElectronicCutter = 15,
        piMultiresonator = 16,
        piAtomicVision = 17,
        piDisintegrator = 18,
        piTurbogravitron = 19,
    };

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

    enum TRelationLevel : std::uint8_t {
        rlHostile = 0,
        rlBad = 1,
        rlNormal = 2,
        rlGood = 3,
        rlExcellent = 4,
    };

    using TItemTypeMask = pas::Set<0, 79>;

    // Numeric goods-table indices correspond to t_Food..t_Narcotics in aConst.
    using TGoodsIndex = std::uint8_t;

    // Native record RTTI.
    struct TPlanetNews {
        std::uint32_t Id;
        std::int32_t Turn;
        TGalaxyNewsKind NewsType;
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

    // CurrentStanding categories: GetControlPresence (),
    // ResetControlFaction (), and subclass RefreshCurrentStanding methods.
    enum TShipStanding : std::uint8_t {
        ssDominator = 0,
        ssUnaligned = 1,
        ssCoalitionMilitary = 2,
        ssCoalitionActive = 3,
        ssCoalitionPassive = 4,
        ssNeutral = 5,
        ssPiratePassive = 6,
        ssPirateActive = 7,
        ssPirateMilitary = 8,
        ssCustom = 9,
    };

    using TShipStandings = pas::Set<0, 9>;

    using PGoodsTradePriceEntry = TGoodsTradePriceEntry*;

    // GainExperience applies separate diminishing returns to these sources.
    enum TExperienceSource : std::uint8_t {
        esUnscaled = 0,
        esDominators = 1,
        esPirates = 2,
        esNormalShips = 3,
        esTraderCareer = 4,
    };

    enum TPilotSkill : std::uint8_t {
        psAccuracy = 0,
        psManeuverability = 1,
        psTechnical = 2,
        psTrading = 3,
        psCharisma = 4,
        psLeadership = 5,
    };

    // Diseases (1..12) and stimulants (13..24); radiation is stored separately.
    enum TCaptainHealthEffect : std::uint32_t {
        heBlindness = 1,
        heChekumash = 2,
        heHolyFanaticism = 3,
        heComplexImmunocide = 4,
        heMysteriousLuatanza = 5,
        heDrugAddiction = 6,
        heWhirlwindConcussion = 7,
        hePulledMuscle = 8,
        heGrandMalosausus = 9,
        heBitterPelenosia = 10,
        heAkaSezyanka = 11,
        heNewMolizone = 12,
        heMaloqSizha = 13,
        heOneEyedKhamas = 14,
        heStardust = 15,
        heSuperTechnician = 16,
        heGaalianAlacrity = 17,
        heBloodDjogar = 18,
        heRagobamWhisper = 19,
        heShakhmandooLeader = 20,
        hePsychotropicCache = 21,
        heBusinessMark = 22,
        heDoubleplex = 23,
        heAbsoluteStatus = 24,
    };

    // GetScriptStandingOverrideMode () and its callers use a 32-bit ordinal.
    enum TScriptStandingOverrideMode : std::uint32_t {
        ssmNormal = 0,
        ssmCustomFaction = 1,
        ssmFixed = 2,
    };

    // ScriptActionTypeNames (), action masks and ship/item dispatch.
    enum TScriptActionType : std::uint8_t {
        satOnStep = 0,
        satOnWeaponShot = 1,
        satOnMissileShot = 2,
        satOnDealingDamage = 3,
        satOnDealingFatalDamage = 4,
        satOnDealingKamikazeDamage = 5,
        satOnTakingDamage = 6,
        satOnTakingDamageEn = 7,
        satOnTakingDamageSp = 8,
        satOnTakingDamageMi = 9,
        satOnWeaponShot2 = 10,
        satOnMissileShot2 = 11,
        satOnGettingWeaponHit = 12,
        satOnGettingMissileHit = 13,
        satOnDroidRepair = 14,
        satOnItemPickUp = 15,
        satOnScan = 16,
        satOnChameleonConfusion = 17,
        satOnScanPossibility = 18,
        satOnAnotherItem = 19,
        satOnAnotherItem2 = 20,
        satOnAnotherGoods = 21,
        satOnItemHit = 22,
        satOnMissileHittingObject = 23,
        satOnEnteringForm = 24,
        satOnLeavingForm = 25,
        satOnReEnteringForm = 26,
        satOnEnteringOtherShip = 27,
        satOnLeavingOtherShip = 28,
        satOnReEnteringOtherShip = 29,
        satOnPlayerSkillIncrease = 30,
        satOnPlayerTalkedWithShip = 31,
        satOnShipTalkedWithPlayer = 32,
        satOnDropItem = 33,
        satOnDropItemFixed = 34,
        satOnMovingItemToStorage = 35,
        satOnReduceEqBattle = 36,
        satOnReduceEqUse = 37,
        satOnReduceEqForce = 38,
        satOnReduceEqForsage = 39,
        satOnItemDestroy = 40,
        satOnPlayerChangeHull = 41,
        satOnPlayerUseMM = 42,
        satOnPlayerBuyEq = 43,
        satOnItemEquip = 44,
        satOnItemDeEquip = 45,
        satOnTrancPacking = 46,
        satOnShipBuysGoods = 47,
        satOnShipSellsGoods = 48,
        satOnShowingItemInfo = 49,
        satOnShowingShipInfo = 50,
        satOnShowingStarInfo = 51,
        satOnNonStandartEqChange = 52,
        satOnCustomTargetting = 53,
        satOnCustomTargettingCheck = 54,
        satOnStartAB = 55,
        satOnABItemDrop = 56,
        satOnGovItemReward = 57,
        satOnCheckingUsability = 58,
        satOnCheckingUsability2 = 59,
        satOnCheckingUsabilityGoods = 60,
        satOnDeath = 61,
    };

    // InitializeShipGreetingDefinitions () and TShip virtual slot $30.
    // Transport subtypes and pirate allegiance have distinct greeting categories.
    enum TGreetingShipCategory : std::uint8_t {
        gscTransport = 0,
        gscLiner = 1,
        gscDiplomat = 2,
        gscRanger = 3,
        gscPirate = 4,
        gscWarrior = 5,
        gscKling = 6,
        gscPirateClan = 7,
    };

    using TShipRank = std::uint8_t;

    // ranger inventory, Dominator effects and script prog* IDs.
    enum TProgramIndex : std::uint8_t {
        prgKellerCall = 0,
        prgLogicalNegation = 1,
        prgDematerial = 2,
        prgEnergotron = 3,
        prgSabCrack = 4,
        prgIntercom = 5,
        prgShipwreck = 6,
        prgWeaponBlocking = 7,
        prgInsanity = 8,
        prgShock = 9,
        prgSelfDestruction = 10,
        prgDisconnection = 11,
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

    // CoalitionProjectNames (), investment dispatch (),
    // and the military-base war operation () share these cooldown indices.
    enum TCoalitionProject : std::uint8_t {
        cpCreateRangerCenter = 0,
        cpCreatePirateBase = 1,
        cpCreateMilitaryBase = 2,
        cpCreateScienceBase = 3,
        cpCreateBusinessCenter = 4,
        cpCreateMedicalBase = 5,
        cpRangersSubsidy = 6,
        cpPiratesSubsidy = 7,
        cpTransportSubsidy = 8,
        cpLostSubsidy = 9,
        cpWarSubsidy = 10,
        cpWarOperation = 11,
    };

    enum TShopUpdateMode : std::uint8_t {
        sumNormal = 0,
        sumDisabled = 1,
        sumEquipmentOnly = 2,
        sumGoodsOnly = 3,
    };

    using PPlanetBattleStatistics = TPlanetBattleStatistics*;

    // Series filters retain the native one-byte set storage and membership checks.
    using TDominatorSeriesMask = pas::Set<0, 2>;

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

    // Native record RTTI.
    // Native record RTTI.
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

    // Award categories from SysToReward (); distinct from individual award IDs.
    enum TAwardKind : std::uint8_t {
        atLiberation = 0,
        atAccomplishment = 1,
        atSecretMission = 2,
        atCowardice = 3,
        atPerfidy = 4,
        atPlanetBattle = 5,
    };

    // ShowPlayerDialogue (), TfTalk.BuildBuiltinChoices (),
    // and the script Talk* constants share these conversation categories.
    enum TTalkKind : std::uint8_t {
        tkMoneyDemand = 0,
        tkGoodsDemand = 1,
        tkTruceOffer = 2,
        tkAttack = 3,
        tkPartnerBreak = 4,
        tkPartnerEnd = 5,
        tkPartnerRiot = 6,
    };

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

    using TFactionStandingMasks = pas::Array<TShipStandings, 0, 2>;

    using TDominatorSeriesNameTable = pas::Array<pas::WideString, 0, 2>;

    using TCaptainDisease = std::uint8_t;

    using TCaptainStimulant = std::uint8_t;

    using TGalaxyDifficultyIndex = std::uint8_t;

    using TGoodsTextOrder = pas::Array<TGoodsIndex, 0, 7>;

    // Shared simulation, economy and targeting limits recovered from their callers.
    inline constexpr std::int32_t GalaxyWarmupTurns = 300;

    inline constexpr std::int32_t TurnsPerYear = 365;

    inline constexpr std::int32_t MaxMonetaryValue = 100000000;

    inline constexpr std::int32_t BaseMovementStepsPerTurn = 200;

    inline constexpr std::int32_t FullPathNodeLimit = 999999;

    inline constexpr std::int32_t AsteroidTargetRangeSquared = 1000000;

    inline constexpr std::int32_t InterceptorTargetRangeSquared = 1000000;

    // TerronToStarTurn packs this transformation marker with a turn number.
    inline constexpr std::int32_t TerronTransformationFlag = 0x40000000;

    // RelationValueToLevel boundaries; distinct from the rl* category ordinals.
    inline constexpr std::int32_t RelationBadMin = 10;

    inline constexpr std::int32_t RelationNormalMin = 30;

    inline constexpr std::int32_t RelationGoodMin = 60;

    inline constexpr std::int32_t RelationExcellentMin = 80;

    // Common collection-count guard in saved galaxy/ship/planet records.
    inline constexpr std::int32_t MaxSavedListCount = 10000;

    // soJumpHole has finished transit and is following its emergence path.
    inline constexpr std::int32_t HoleExitOrderState = -65536;

    // The landing target and stored-item loader use opposite high-bit tags.
    inline constexpr std::uint32_t OrderTargetShipFlag = 0x80000000u;

    inline constexpr std::uint32_t StoredItemPlanetFlag = 0x80000000u;

    inline constexpr std::int32_t TaggedObjectIdMask = 0x7fffffff;

    // Configuration/adverts leave goods unspecified; greetings translate that to a skip marker.
    inline constexpr std::int32_t UnspecifiedGoods = 42;

    inline constexpr std::int32_t NoGreetingGoods = 50;

    // Higher text-quest IDs use PlanetQuestLic rather than standalone completion records.
    inline constexpr std::int32_t FirstLicensedQuestId = 10000;

    // SelectAward returns this sentinel when no individual award qualifies.
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
