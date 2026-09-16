#pragma once
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"

namespace aConst {
    // Four-byte zero boundary marker for the dormant native data checksum; original name unresolved.
    extern std::uint32_t& IntegrityDataBegin;

    // 167 in this binary.
    extern std::int32_t& CurrentSaveVersion;

    // 44 in this binary; enforced by the save-manager load action.
    extern std::int32_t& MinimumLoadableSaveVersion;

    extern pas::WideString& LocalizedTextLinePrefix;

    // Constellations.GalaxyCountStars config; SF_GalaxyPtr('StarCnt') exposes its address.
    extern std::int32_t& GalaxyStarCount;

    // Constellations.GalaxySizeY config; also scales hyperspace route length.
    extern std::int32_t& GalaxySizeY;

    // Constellations.GalaxySizeX configuration.
    extern std::int32_t& GalaxySizeX;

    extern const std::uint8_t& MaximumNewGameDifficulty;

    extern aGalaxyStruct::TGalaxyDifficultyTuningTable& GalaxyDifficultyTuning;

    extern pas::Array<aConst::TRelationTypeInfo, 0, 4>& RelationInfo;

    extern pas::Array<aConst::TEconomyInfo, 0, 2>& PlanetEconomyInfo;

    extern pas::Array<aConst::TShipTypeInfo, 0, 13>& ShipTypeNames;

    // Standing used to gate station spawning by faction, including the custom station.
    extern pas::Array<std::uint8_t, 6, 13>& StationDefaultStandings;

    // Standing masks used by TPlayer.CanSelectShipTarget.
    extern aGalaxyStruct::TFactionStandingMasks& NonTargetableStationStandingMasks;

    extern aGalaxyStruct::TFactionStandingMasks& FactionStandingMasks;

    extern pas::Array<aConst::TStatusInfo, 0, 2>& CareerTuning;

    extern pas::Array<pas::WideString, 0, 2>& TransportTypeNames;

    extern const pas::Array<aGalaxyStruct::TKlingType, 0, 7>& DominatorDisplayOrder;

    extern pas::Array<pas::WideString, 0, 7>& DominatorShipTypeNames;

    // TKlingType order; native name initializer pairs.
    extern pas::Array<aConst::TKlingTypeInfo, 0, 7>& DominatorShipDefinitions;

    // Minimum reinforced-system strength by constellation distance tier.
    extern pas::Array<double, 0, 3>& DominatorRetreatStrengthByTier;

    extern pas::Array<pas::WideString, 0, 2>& DominatorSeriesNames;

    // Native Blazer, Keller, Terron research multipliers.
    extern pas::Array<double, 0, 2>& DominatorResearchRateMultipliers;

    extern pas::Array<double, 0, 2>& ResearchProgramCostFactors;

    extern pas::Array<pas::WideString, 0, 61>& ScriptActionTypeNames;

    // Excluded from ransom offers and ordinary ally requests.
    extern aGalaxyStruct::TShipTypeMask& NonNegotiatingShipTypes;

    extern const pas::Array<aConst::SEquipment, 0, 7>& EquipmentSlotLayouts;

    extern pas::Array<pas::WideString, 0, 75>& ItemTypeNames;

    extern pas::Array<pas::DynArray<aConst::TItemType>, 0, 3> ArtefactLootPools;

    extern pas::Array<pas::DynArray<pas::WideString>, 0, 3> CustomArtefactLootPools;

    extern pas::Array<pas::DynArray<pas::WideString>, 0, 3> UselessItemLootPools;

    extern pas::Array<aConst::TGoodsInfo, 0, 7>& GoodsMarket;

    extern aGalaxyStruct::TGoodsTextOrder& GoodsTextOrder;

    extern pas::Array<pas::WideString, 0, 4>& MissionTypeNames;

    extern pas::Array<aConst::TOwnerInfo, 0, 7>& OwnerInfo;

    extern aGalaxyStruct::TPlanetOwnerMasks& PlanetOwnerMasks;

    extern aGalaxyStruct::TOwnerRelationTable& OwnerRelations;

    extern aGalaxyStruct::TPlanetRaceMarketTable& PlanetRaceMarket;

    extern aGalaxyStruct::TPlanetEquipmentOfferQuotaTable& PlanetEquipmentOfferQuotas;

    // Native defaults; aRuins accesses this table through an external-unit reference. Original defining unit is inferred.
    extern aConst::TStationEquipmentOfferQuotaTable& StationEquipmentOfferQuotas;

    extern pas::Array<pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>, 6, 13>& StationGoodsFactors;

    extern pas::Array<aConst::TGovermentInfo, 0, 4>& PlanetGovernmentMarket;

    // Indexed by goods, native race and government; False marks prohibited goods.
    extern aConst::TGoodsLegalityTable& GoodsLegalOnPlanet;

    extern pas::Array<pas::WideString, 0, 5>& MedalNames;

    // Native initialization table.
    extern pas::Array<pas::WideString, 0, 7>& CoalitionRankNames;

    // Zero threshold at the maximum rank.
    extern pas::Array<std::uint16_t, 0, 7>& CoalitionRankPointThresholds;

    extern pas::Array<pas::WideString, 0, 7>& PirateRankNames;

    // Zero threshold at the maximum rank.
    extern pas::Array<std::uint16_t, 0, 7>& PirateRankPointThresholds;

    extern pas::Array<pas::WideString, 0, 5>& SkillConfigNames;

    // Native race, then TPilotSkill; used by ranger bonus evaluation and character setup.
    extern pas::Array<pas::Array<float, 0, 5>, 0, 4>& RaceSkillEvaluationFactors;

    // Level, then TPilotSkill; Trading supplies the item resale percentage.
    extern pas::Array<pas::Array<std::uint16_t, 0, 5>, 0, 6>& PilotSkillEffects;

    extern pas::Array<std::uint16_t, 0, 6>& TechnicalSkillSatelliteLimits;

    // Fraction of the purchase/sale spread recovered by the Trading skill.
    extern pas::Array<std::uint16_t, 0, 6>& TradingSkillSalePercent;

    extern pas::Array<std::uint16_t, 0, 6>& LeadershipExperiencePercent;

    // Loaded from GalaxyNews.MaxCntPlanetNews; caps visible-system economic events.
    extern std::int32_t& MaxPlanetNews;

    extern pas::Array<pas::WideString, 0, 5>& SizeTagNames;

    // Fractions of cached ship wealth used for negotiated amounts.
    extern pas::Array<float, 0, 5>& WealthDemandScales;

    // Includes the unsupported-slot sentinel.
    extern pas::Array<std::int32_t, 0, 10>& MinimumHullSlotCounts;

    // Artefact limit can be overridden by gameplay configuration.
    extern pas::Array<std::int32_t, 0, 10>& DefaultHullSlotCounts;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& RangerHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& WarriorHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& PirateHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& TransportHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& LinerHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& DiplomatHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<std::int32_t, 0, 10>& TranclucatorHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& StationHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& DominatorHullSlots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<std::int32_t, 0, 10>& HullType9Slots;

    // Native base slot counts; final column is unsupported kind.
    extern pas::Array<std::int32_t, 0, 10>& HullType10Slots;

    extern pas::Array<aConst::TEquipmentBonusKind, 0, 10>& HullSlotBonusKinds;

    extern aConst::TOwnerWeaponAvailabilityTable& OwnerWeaponAvailability;

    // Native managed-string descriptors.
    extern pas::Array<pas::WideString, 0, 20>& WeaponDamageFlagNames;

    extern pas::Array<aConst::TPrimaryDamageTypeInfo, 0, 2>& WeaponDamageClasses;

    // Indexed by TCombatStatusEffectType.
    extern pas::Array<float, 0, 6>& CombatStatusHullFactors;

    extern pas::Array<float, 0, 6>& CombatStatusAccumulationFactors;

    extern pas::Array<pas::WideString, 0, 42>& EquipmentBonusNames;

    // Maps bonSkill1..bonSkill6 to native pilot skills.
    extern pas::Array<std::uint8_t, 0, 5>& EquipmentBonusSkills;

    extern aConst::TEquipmentSizeFactorTable& EquipmentSizeFactors;

    // Native technology multiplier, immediately after EquipmentSizeFactors.
    extern aConst::TWeaponRangeLevelFactors& WeaponRangeLevelFactors;

    extern pas::Array<aConst::tInventionInfo, 0, 19>& PlanetInventionInfo;

    extern float GoodsInflationMin;

    extern float GoodsInflationMax;

    extern float GoodsStockMin;

    extern float GoodsStockMax;

    extern std::int32_t GoodsInflationStartTurn;

    extern std::int32_t GoodsInflationEndTurn;

    extern aGalaxyStruct::TQuestTuningTable QuestTuning;

    // Levels 1..6 loaded from configuration; level zero is cleared.
    extern pas::Array<pas::Array<std::uint16_t, 0, 5>, 0, 6> SkillTrainingCosts;

    // Sum of all six levels of all six skills.
    extern std::int32_t TotalSkillTrainingCost;

    extern aGalaxyStruct::TQuestExperienceTable QuestExperience;

    // Config kArtefactHull.
    extern std::int32_t HullArtefactArmor;

    // Set to 1.5 by native configuration loading.
    extern float HullArtefactStatusDecayFactor;

    extern std::int32_t FuelArtefactBase;

    // Config kArtefactSpeed.
    extern float SpeedArtefactFactor;

    extern std::int32_t EngineArtefactBase;

    // Config kArtefactRadar.
    extern std::int32_t RadarArtefactRange;

    // Config kArtefactScaner.
    extern std::int32_t ScannerArtefactPower;

    // Config kArtefactDroid.
    extern std::int32_t DroidArtefactRepair;

    // Config kArtefactDroidWear.
    extern float DroidArtefactWear;

    // Set to 1.5 by native configuration loading.
    extern float DroidArtefactStatusDecayFactor;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t NanoArtefactRepair;

    // Config kArtefactDef.
    extern float DefenseArtefactBonus;

    // Config kArtefactAntigrav.
    extern float AntigravityArtefactMassFactor;

    // Config kArtefactHook.
    extern std::int32_t CargoHookArtefactPower;

    // Config kArtefactHookRaduis.
    extern std::int32_t CargoHookArtefactRange;

    // Config kArtefactHookSpeed.
    extern std::int32_t CargoHookArtefactSpeed;

    // Config kArtWeaponToSpeed.
    extern std::int32_t WeaponToSpeedArtefactBonus;

    // Config kArtDefToEnergyUp.
    extern float DefenseToEnergyUpperFactor;

    // Config kArtDefToEnergyMin.
    extern float DefenseToEnergyMinimumFactor;

    // Config kArtDefToEnergyPenalty.
    extern float DefenseToEnergyPenalty;

    // Config kArtDefToArms1Penalty.
    extern float DefenseToWeaponPenalty;

    // Config kArtEnergyPulse.
    extern float EnergyPulseArtefactFactor;

    // Config kArtEnergyPulseChance.
    extern float EnergyPulseArtefactChance;

    // Config kArtSplinter.
    extern float SplinterArtefactFactor;

    // Config kArtGiperJump.
    extern std::int32_t HyperJumpArtefactRange;

    // Configured star-heat reduction per active artefact.
    extern float StarHeatArtefactReduction;

    extern float ExtraMissileChance;

    // Config kArtForsage.
    extern float AfterburnerArtefactWearFactor;

    // Config kArtefactHullEx.
    extern std::int32_t HullArtefactBoostArmor;

    // Set to 0.5 by native configuration loading.
    extern float HullArtefactBoostStatusDecay;

    extern std::int32_t FuelArtefactBoost;

    // Config kArtefactSpeedEx.
    extern float SpeedArtefactBoostFactor;

    extern std::int32_t EngineArtefactBoost;

    // Config kArtefactRadarEx.
    extern std::int32_t RadarArtefactBoostRange;

    // Config kArtefactScanerEx.
    extern std::int32_t ScannerArtefactBoostPower;

    // Config kArtefactDroidEx.
    extern std::int32_t DroidArtefactBoostRepair;

    // Config kArtefactDroidWearEx.
    extern float DroidArtefactBoostWear;

    // Set to 0.5 by native configuration loading.
    extern float DroidArtefactBoostStatusDecay;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t NanoArtefactBoostRepair;

    // Config kArtefactDefEx.
    extern float DefenseArtefactBoost;

    // Config kArtefactAntigravEx.
    extern float AntigravityArtefactBoostFactor;

    // Config kArtefactHookEx.
    extern std::int32_t CargoHookArtefactBoostPower;

    // Config kArtefactHookRaduisEx.
    extern std::int32_t CargoHookArtefactBoostRange;

    // Config kArtefactHookSpeedEx.
    extern std::int32_t CargoHookArtefactBoostSpeed;

    // Config kArtWeaponToSpeedEx.
    extern std::int32_t WeaponToSpeedArtefactBoost;

    // Config kArtDefToEnergyUpEx.
    extern float DefenseToEnergyUpperBoost;

    // Config kArtDefToEnergyMinEx.
    extern float DefenseToEnergyMinimumBoost;

    // Config kArtDefToEnergyPenaltyEx.
    extern float DefenseToEnergyBoostPenalty;

    // Config kArtEnergyPulseEx.
    extern float EnergyPulseArtefactBoostFactor;

    // Config kArtSplinterEx.
    extern float SplinterArtefactBoostFactor;

    // Config kArtGiperJumpEx.
    extern std::int32_t HyperJumpArtefactBoostRange;

    // Boosted additional reduction.
    extern float StarHeatArtefactBoostReduction;

    // Loaded from Artefacts.NumericValues.
    extern float ExtraMissileBoostChance;

    // Config kArtForsageEx.
    extern float AfterburnerArtefactBoostWearFactor;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t MinTransmitterPower;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t AverageTransmitterPower;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t MaxTransmitterPower;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t TransmitterSameSystemPenalty;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t TransmitterAnySystemPenalty;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t TransmitterSameSystemPenaltyTurns;

    // Loaded from Artefacts.NumericValues.
    extern std::int32_t TransmitterAnySystemPenaltyTurns;

    extern std::int32_t SubportalRewardPenalty;

    extern std::int32_t SubportalRewardPenaltyTurns;

    // Added when Item.DestroyFlag is two.
    extern std::int32_t ItemExplosionBonusDamage;

    // Damage at the blast radius.
    extern std::int32_t BombMinimumDamage;

    // Damage at the center.
    extern std::int32_t BombMaximumDamage;

    extern std::int32_t BombDamageRadius;

    // Square of configured BombRadius.
    extern std::int32_t ItemExplosionRadiusSquared;

    extern std::int32_t PointDefensePassCount;

    extern std::int32_t PointDefenseBaseRange;

    extern std::int32_t PointDefenseBonusRange;

    // Asteroid.kAsteroidMinDamagePercent divided by 100; fraction of hull capacity.
    extern float AsteroidMinDamageFactor;

    // Asteroid.kAsteroidMaxDamagePercent divided by 100.
    extern float AsteroidMaxDamageFactor;

    // Asteroid.kAsteroidMinDamagePercentDef divided by 100.
    extern float AsteroidMinDamageFactorWithDefGenerator;

    // Asteroid.kAsteroidMaxDamagePercentDef divided by 100.
    extern float AsteroidMaxDamageFactorWithDefGenerator;

    // Hull capacities are multiplied by this configuration value.
    extern float HullCapacityScale;

    extern std::int32_t HullBaseSize;

    extern std::int32_t FuelTanksBaseSize;

    extern std::int32_t EngineBaseSize;

    extern std::int32_t RadarBaseSize;

    extern std::int32_t ScannerBaseSize;

    extern std::int32_t RepairRobotBaseSize;

    extern std::int32_t CargoHookBaseSize;

    extern std::int32_t DefGeneratorBaseSize;

    // Config native equipment configuration.
    extern float AfterburnerSpeedFactor;

    // Loaded from equipment configuration.
    extern pas::Array<std::uint8_t, 1, 8> FuelCapacityByLevel;

    // Loaded by the native equipment configuration initializer.
    extern aGalaxyStruct::TEngineLevelStatsTable EngineLevelStats;

    // Native per-technology armor table; remaining fields unresolved.
    extern aConst::THullLevelStatsTable HullLevelStats;

    // Loaded from mRepair.
    extern pas::Array<std::uint8_t, 1, 8> RepairRobotLevelPoints;

    extern pas::Array<float, 1, 8> DefGeneratorLevelFactors;

    // Loaded from equipment configuration.
    extern pas::Array<std::uint16_t, 1, 8> RadarLevelRanges;

    extern aGalaxyStruct::TCargoHookLevelStatsTable CargoHookLevelStats;

    // Damage class, then owner; loaded from mFragilityByOwner*.
    extern pas::Array<pas::Array<float, 0, 7>, 0, 2> HullFragilityByOwner;

    // Loaded from mFragilityByShipType.
    extern pas::Array<float, 0, 10> HullFragilityByType;

    extern pas::Array<aConst::TWeaponInfo, 50, 67> WeaponInfos;

    extern aConst::TEquipmentInventionIndexTable& EquipmentInventionIndices;

    extern pas::Array<pas::WideString, 0, 11>& CoalitionProjectNames;

    extern pas::Array<std::int32_t, 0, 11>& StationServiceRepeatPeriods;

    extern pas::Array<pas::WideString, 0, 11>& ProgramNames;

    extern aConst::TProgramDurationTable& ProgramDuration;

    extern pas::Array<std::int32_t, 0, 11>& PirateProgramBatchSizes;

    extern pas::Array<std::int32_t, 0, 11>& PirateProgramBaseCosts;

    // Set after the one-time localized base-table copy.
    extern std::uint8_t& GoodsMarketBaseCaptured;

    // Four-byte zero marker at the exclusive boundary of that native checksum span.
    extern std::uint32_t& IntegrityDataEnd;

    // Updated by TPlayer.NextDay and read by TfGameEnd.OnOpen; score-related role not fully recovered.
    extern std::int32_t LastMedicalPolicyTicks;

    // Initialized from HullBaseSize and EquipmentSizeFactors[5].
    extern std::int32_t HullMassEvaluationStart;

    // Initialized from HullBaseSize and EquipmentSizeFactors[1].
    extern std::int32_t HullMassEvaluationEnd;

    extern std::int32_t WearMassMin;

    extern std::int32_t WearMassMax;

    extern pas::Array<aConst::TGoodsInfo, 0, 7> GoodsMarketBase;

    extern pas::DynArray<aConst::TMicroModuleInfo> MicroModuleTemplates;

    extern std::int32_t MicroModuleTemplateCount;

    // Loaded from HullType configuration, sorted by numeric suffix.
    extern pas::DynArray<aConst::THullTypeInfo> HullSeriesDefinitions;

    // Native count used by CheatIdeal and hull-series configuration.
    extern std::int32_t HullSeriesCount;

    // Native disease/stimulant definitions; eligibility and progression fields verified in TPlayer.NextDay.
    extern pas::Array<aConst::TIllnessInfo, 1, 24> CaptainHealthDefinitions;

    // Finalized as one TIllnessInfo alongside the 24 captain effects.
    extern aConst::TRadiationHealthDefinitions RadiationHealthDefinitions;

    // Shared selection scratch, sized to MicroModuleTemplateCount when templates load. Callers track the used prefix separately.
    extern pas::DynArray<std::int32_t> MicroModuleCandidateIndices;

    void IncrementWordSaturating(pas::Var<std::uint16_t> Value);

    pas::WideString OwnerToSys(std::uint8_t OwnerId);

    std::uint8_t IsKnownOwnerName(const pas::WideString& Name);

    // Unrecognized names act as a wildcard.
    std::uint8_t MatchesOwnerName(std::uint8_t OwnerId, const pas::WideString& Name);

    // Case-sensitive substring, Any, or empty string.
    std::uint8_t MatchesCareerName(std::uint8_t Career, const pas::WideString& Names);

    void LoadArtefactConfiguration();

    void LoadDamageSkillQuestMarketConfiguration();

    void LoadEquipmentConfiguration();

    void LoadWeaponConfiguration();

    void LoadMicroModuleConfiguration();

    void InitializeCaptainHealthDefinitions();

    void LoadHullSeriesConfiguration();

    void InitializeGameplayConfig();

    TShipSlotKind ItemTypeToSlotKind(std::uint8_t ItemType);

    // Missile bit takes precedence over splinter; otherwise energy.
    TWeaponDamageClass ClassifyWeaponDamageFlags(std::uint32_t Flags);

    // Class/subtype mapping used by hull generation and legacy saves; only TObject RTTI operations precede explicit subclass casts.
    std::uint8_t ShipToHullType(pas::Object* Ship);

    // Identity conversion for Coalition races 0..4; raises for all other values.
    std::uint8_t RaceToOwner(std::uint8_t RaceId);

    std::uint8_t OwnerFromInternalName(const pas::WideString& Name);

    // Identity conversion for Coalition owners 0..4; raises for all other values.
    std::uint8_t OwnerToRace(std::uint8_t OwnerId);

    // Raises outside Coalition races 0..4.
    pas::WideString RaceToSys(std::uint8_t RaceId);

    // Accepts 0..4; raises otherwise.
    std::uint8_t NumberToRace(std::int32_t Value);

    // Case-sensitive lookup; raises for an unknown name.
    std::uint8_t SysToReward(const pas::WideString& Name);

    // Case-sensitive lookup among 14 ship types; raises for an unknown name.
    std::uint8_t SysToShipType(const pas::WideString& Name);

    // Maps owner IDs 0..5 and 7 to fixed RGB colors through CurrentPixelFormat; other values use magenta.
    std::uint32_t OwnerToFilmColor(std::int8_t OwnerId);

    std::uint32_t CustomFactionToFilmColor(pas::WideString Faction);

    // Race.PlanetIconNum lookup; returns -1 for an absent entry. Film owner codes offset a nonnegative result by eight.
    std::int32_t GetCustomFactionPlanetIconNumber(pas::WideString Faction);

    // Zero, Mini, Small, Average, Big, Huge map to 0..5; unknown tags map to zero.
    std::uint8_t SizeTagToLevel(const pas::WideString& Tag);

    // Seeded variation around a size bucket; unknown nonzero levels use the midpoint.
    std::int32_t GenerateValueForSizeLevel(std::uint8_t Level, std::int32_t Minimum, std::int32_t Maximum, std::uint8_t VariationPercent, std::uint32_t Seed);

    std::int32_t GetAverageItemSize(std::uint8_t ItemType);

    std::uint8_t PickRandomItemType(TItemTypeSelection Mask);

    // Selects a set bit among 0..75 while advancing Seed; an empty mask returns 76.
    std::uint8_t PickRandomItemTypeFromSeed(TItemTypeSelection Mask, std::uint32_t& Seed);

    // Copies the ten-byte mask, then counts bits 0..75; ignores storage bits 76..79.
    std::int32_t CountItemTypesInMask(TItemTypeSelection Mask);

    // One-based selected-bit index among types 0..75; returns zero if no index matches.
    std::uint8_t GetItemTypeFromMask(TItemTypeSelection Mask, std::int32_t Index);

    // Only Coalition manufacturers are eligible.
    std::uint8_t PickRandomEquipmentOwner(std::uint32_t RandomValue);

    pas::WideString LookupNamedColorTag(pas::WideString Name);

    // Repeated Path values are joined with CRLF; no separator precedes an empty accumulator.
    // Missing paths return empty and may create intermediate blocks.
    // Both lookups expand <br>, <ll> and <Player>; the player's name is highlighted.
    // LocalizedText leaves <clr>/<clrEnd> intact; LocalizedColorText expands them.
    pas::WideString LocalizedText(const pas::WideString& Path);

    // Also expands <clr> and <clrEnd> to yellow opening and closing color tags.
    pas::WideString LocalizedColorText(const pas::WideString& Path);

    // Expands <br>, <ll>, <Player>, <clr> and <clrEnd>; leaves <Player> intact when no player exists.
    void ExpandLocalizedTextMarkup(pas::WideString& Text);

    // Expands markup, trims outer whitespace, then applies LocalizedTextLinePrefix to the first line and after each CRLF.
    void ExpandLocalizedTextMarkupAndPrefixLines(pas::WideString& Text);

    // Collects at most ten nonempty variants: Path, then contiguous numeric suffixes
    // starting at 1 (0 when the base lookup is empty). A missing suffix prevents reaching later ones.
    // Selection uses (CurrentTurn + SeedOffset) div 10, except in chaotic mode. No variants produces an unavailable-text diagnostic.
    pas::WideString PickLocalizedTextVariant(const pas::WideString& Path, std::int32_t SeedOffset);

    // Buckets 0..100; out-of-range values map to normal.
    aGalaxyStruct::TRelationLevel RelationValueToLevel(std::uint8_t Value);

    pas::WideString GetFactionEmblemPath(pas::WideString Faction);

    // First matching template; -1 when absent.
    std::int32_t FindMicroModuleTemplateByCustomTag(pas::WideString CustomTag);

} // namespace aConst
