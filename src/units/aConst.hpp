#pragma once
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"

namespace aConst {
    extern std::uint32_t& IntegrityDataBegin;

    extern std::int32_t& CurrentSaveVersion;

    extern std::int32_t& MinimumLoadableSaveVersion;

    extern pas::WideString& LocalizedTextLinePrefix;

    extern std::int32_t& GalaxyStarCount;

    extern std::int32_t& GalaxySizeY;

    extern std::int32_t& GalaxySizeX;

    extern const std::uint8_t& MaximumNewGameDifficulty;

    extern aGalaxyStruct::TGalaxyDifficultyTuningTable& GalaxyDifficultyTuning;

    extern pas::Array<aConst::TRelationTypeInfo, 0, 4>& RelationInfo;

    extern pas::Array<aConst::TEconomyInfo, 0, 2>& PlanetEconomyInfo;

    extern pas::Array<aConst::TShipTypeInfo, 0, 13>& ShipTypeNames;

    extern pas::Array<std::uint8_t, 6, 13>& StationDefaultStandings;

    extern aGalaxyStruct::TFactionStandingMasks& NonTargetableStationStandingMasks;

    extern aGalaxyStruct::TFactionStandingMasks& FactionStandingMasks;

    extern pas::Array<aConst::TStatusInfo, 0, 2>& CareerTuning;

    extern pas::Array<pas::WideString, 0, 2>& TransportTypeNames;

    extern const pas::Array<aGalaxyStruct::TKlingType, 0, 7>& DominatorDisplayOrder;

    extern pas::Array<pas::WideString, 0, 7>& DominatorShipTypeNames;

    extern pas::Array<aConst::TKlingTypeInfo, 0, 7>& DominatorShipDefinitions;

    extern pas::Array<double, 0, 3>& DominatorRetreatStrengthByTier;

    extern pas::Array<pas::WideString, 0, 2>& DominatorSeriesNames;

    extern pas::Array<double, 0, 2>& DominatorResearchRateMultipliers;

    extern pas::Array<double, 0, 2>& ResearchProgramCostFactors;

    extern pas::Array<pas::WideString, 0, 61>& ScriptActionTypeNames;

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

    extern aConst::TStationEquipmentOfferQuotaTable& StationEquipmentOfferQuotas;

    extern pas::Array<pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>, 6, 13>& StationGoodsFactors;

    extern pas::Array<aConst::TGovermentInfo, 0, 4>& PlanetGovernmentMarket;

    extern aConst::TGoodsLegalityTable& GoodsLegalOnPlanet;

    extern pas::Array<pas::WideString, 0, 5>& MedalNames;

    extern pas::Array<pas::WideString, 0, 7>& CoalitionRankNames;

    extern pas::Array<std::uint16_t, 0, 7>& CoalitionRankPointThresholds;

    extern pas::Array<pas::WideString, 0, 7>& PirateRankNames;

    extern pas::Array<std::uint16_t, 0, 7>& PirateRankPointThresholds;

    extern pas::Array<pas::WideString, 0, 5>& SkillConfigNames;

    extern pas::Array<pas::Array<float, 0, 5>, 0, 4>& RaceSkillEvaluationFactors;

    extern pas::Array<pas::Array<std::uint16_t, 0, 5>, 0, 6>& PilotSkillEffects;

    extern pas::Array<std::uint16_t, 0, 6>& TechnicalSkillSatelliteLimits;

    extern pas::Array<std::uint16_t, 0, 6>& TradingSkillSalePercent;

    extern pas::Array<std::uint16_t, 0, 6>& LeadershipExperiencePercent;

    extern std::int32_t& MaxPlanetNews;

    extern pas::Array<pas::WideString, 0, 5>& SizeTagNames;

    extern pas::Array<float, 0, 5>& WealthDemandScales;

    extern pas::Array<std::int32_t, 0, 10>& MinimumHullSlotCounts;

    extern pas::Array<std::int32_t, 0, 10>& DefaultHullSlotCounts;

    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& RangerHullSlots;

    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& WarriorHullSlots;

    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& PirateHullSlots;

    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& TransportHullSlots;

    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& LinerHullSlots;

    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& DiplomatHullSlots;

    extern pas::Array<std::int32_t, 0, 10>& TranclucatorHullSlots;

    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& StationHullSlots;

    extern pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& DominatorHullSlots;

    extern pas::Array<std::int32_t, 0, 10>& HullType9Slots;

    extern pas::Array<std::int32_t, 0, 10>& HullType10Slots;

    extern pas::Array<aConst::TEquipmentBonusKind, 0, 10>& HullSlotBonusKinds;

    extern aConst::TOwnerWeaponAvailabilityTable& OwnerWeaponAvailability;

    extern pas::Array<pas::WideString, 0, 20>& WeaponDamageFlagNames;

    extern pas::Array<aConst::TPrimaryDamageTypeInfo, 0, 2>& WeaponDamageClasses;

    extern pas::Array<float, 0, 6>& CombatStatusHullFactors;

    extern pas::Array<float, 0, 6>& CombatStatusAccumulationFactors;

    extern pas::Array<pas::WideString, 0, 42>& EquipmentBonusNames;

    extern pas::Array<std::uint8_t, 0, 5>& EquipmentBonusSkills;

    extern aConst::TEquipmentSizeFactorTable& EquipmentSizeFactors;

    extern aConst::TWeaponRangeLevelFactors& WeaponRangeLevelFactors;

    extern pas::Array<aConst::tInventionInfo, 0, 19>& PlanetInventionInfo;

    extern float GoodsInflationMin;

    extern float GoodsInflationMax;

    extern float GoodsStockMin;

    extern float GoodsStockMax;

    extern std::int32_t GoodsInflationStartTurn;

    extern std::int32_t GoodsInflationEndTurn;

    extern aGalaxyStruct::TQuestTuningTable QuestTuning;

    extern pas::Array<pas::Array<std::uint16_t, 0, 5>, 0, 6> SkillTrainingCosts;

    extern std::int32_t TotalSkillTrainingCost;

    extern aGalaxyStruct::TQuestExperienceTable QuestExperience;

    extern std::int32_t HullArtefactArmor;

    extern float HullArtefactStatusDecayFactor;

    extern std::int32_t FuelArtefactBase;

    extern float SpeedArtefactFactor;

    extern std::int32_t EngineArtefactBase;

    extern std::int32_t RadarArtefactRange;

    extern std::int32_t ScannerArtefactPower;

    extern std::int32_t DroidArtefactRepair;

    extern float DroidArtefactWear;

    extern float DroidArtefactStatusDecayFactor;

    extern std::int32_t NanoArtefactRepair;

    extern float DefenseArtefactBonus;

    extern float AntigravityArtefactMassFactor;

    extern std::int32_t CargoHookArtefactPower;

    extern std::int32_t CargoHookArtefactRange;

    extern std::int32_t CargoHookArtefactSpeed;

    extern std::int32_t WeaponToSpeedArtefactBonus;

    extern float DefenseToEnergyUpperFactor;

    extern float DefenseToEnergyMinimumFactor;

    extern float DefenseToEnergyPenalty;

    extern float DefenseToWeaponPenalty;

    extern float EnergyPulseArtefactFactor;

    extern float EnergyPulseArtefactChance;

    extern float SplinterArtefactFactor;

    extern std::int32_t HyperJumpArtefactRange;

    extern float StarHeatArtefactReduction;

    extern float ExtraMissileChance;

    extern float AfterburnerArtefactWearFactor;

    extern std::int32_t HullArtefactBoostArmor;

    extern float HullArtefactBoostStatusDecay;

    extern std::int32_t FuelArtefactBoost;

    extern float SpeedArtefactBoostFactor;

    extern std::int32_t EngineArtefactBoost;

    extern std::int32_t RadarArtefactBoostRange;

    extern std::int32_t ScannerArtefactBoostPower;

    extern std::int32_t DroidArtefactBoostRepair;

    extern float DroidArtefactBoostWear;

    extern float DroidArtefactBoostStatusDecay;

    extern std::int32_t NanoArtefactBoostRepair;

    extern float DefenseArtefactBoost;

    extern float AntigravityArtefactBoostFactor;

    extern std::int32_t CargoHookArtefactBoostPower;

    extern std::int32_t CargoHookArtefactBoostRange;

    extern std::int32_t CargoHookArtefactBoostSpeed;

    extern std::int32_t WeaponToSpeedArtefactBoost;

    extern float DefenseToEnergyUpperBoost;

    extern float DefenseToEnergyMinimumBoost;

    extern float DefenseToEnergyBoostPenalty;

    extern float EnergyPulseArtefactBoostFactor;

    extern float SplinterArtefactBoostFactor;

    extern std::int32_t HyperJumpArtefactBoostRange;

    extern float StarHeatArtefactBoostReduction;

    extern float ExtraMissileBoostChance;

    extern float AfterburnerArtefactBoostWearFactor;

    extern std::int32_t MinTransmitterPower;

    extern std::int32_t AverageTransmitterPower;

    extern std::int32_t MaxTransmitterPower;

    extern std::int32_t TransmitterSameSystemPenalty;

    extern std::int32_t TransmitterAnySystemPenalty;

    extern std::int32_t TransmitterSameSystemPenaltyTurns;

    extern std::int32_t TransmitterAnySystemPenaltyTurns;

    extern std::int32_t SubportalRewardPenalty;

    extern std::int32_t SubportalRewardPenaltyTurns;

    extern std::int32_t ItemExplosionBonusDamage;

    extern std::int32_t BombMinimumDamage;

    extern std::int32_t BombMaximumDamage;

    extern std::int32_t BombDamageRadius;

    extern std::int32_t ItemExplosionRadiusSquared;

    extern std::int32_t PointDefensePassCount;

    extern std::int32_t PointDefenseBaseRange;

    extern std::int32_t PointDefenseBonusRange;

    extern float AsteroidMinDamageFactor;

    extern float AsteroidMaxDamageFactor;

    extern float AsteroidMinDamageFactorWithDefGenerator;

    extern float AsteroidMaxDamageFactorWithDefGenerator;

    extern float HullCapacityScale;

    extern std::int32_t HullBaseSize;

    extern std::int32_t FuelTanksBaseSize;

    extern std::int32_t EngineBaseSize;

    extern std::int32_t RadarBaseSize;

    extern std::int32_t ScannerBaseSize;

    extern std::int32_t RepairRobotBaseSize;

    extern std::int32_t CargoHookBaseSize;

    extern std::int32_t DefGeneratorBaseSize;

    extern float AfterburnerSpeedFactor;

    extern pas::Array<std::uint8_t, 1, 8> FuelCapacityByLevel;

    extern aGalaxyStruct::TEngineLevelStatsTable EngineLevelStats;

    extern aConst::THullLevelStatsTable HullLevelStats;

    extern pas::Array<std::uint8_t, 1, 8> RepairRobotLevelPoints;

    extern pas::Array<float, 1, 8> DefGeneratorLevelFactors;

    extern pas::Array<std::uint16_t, 1, 8> RadarLevelRanges;

    extern aGalaxyStruct::TCargoHookLevelStatsTable CargoHookLevelStats;

    extern pas::Array<pas::Array<float, 0, 7>, 0, 2> HullFragilityByOwner;

    extern pas::Array<float, 0, 10> HullFragilityByType;

    extern pas::Array<aConst::TWeaponInfo, 50, 67> WeaponInfos;

    extern aConst::TEquipmentInventionIndexTable& EquipmentInventionIndices;

    extern pas::Array<pas::WideString, 0, 11>& CoalitionProjectNames;

    extern pas::Array<std::int32_t, 0, 11>& StationServiceRepeatPeriods;

    extern pas::Array<pas::WideString, 0, 11>& ProgramNames;

    extern aConst::TProgramDurationTable& ProgramDuration;

    extern pas::Array<std::int32_t, 0, 11>& PirateProgramBatchSizes;

    extern pas::Array<std::int32_t, 0, 11>& PirateProgramBaseCosts;

    extern std::uint8_t& GoodsMarketBaseCaptured;

    extern std::uint32_t& IntegrityDataEnd;

    extern std::int32_t LastMedicalPolicyTicks;

    extern std::int32_t HullMassEvaluationStart;

    extern std::int32_t HullMassEvaluationEnd;

    extern std::int32_t WearMassMin;

    extern std::int32_t WearMassMax;

    extern pas::Array<aConst::TGoodsInfo, 0, 7> GoodsMarketBase;

    extern pas::DynArray<aConst::TMicroModuleInfo> MicroModuleTemplates;

    extern std::int32_t MicroModuleTemplateCount;

    extern pas::DynArray<aConst::THullTypeInfo> HullSeriesDefinitions;

    extern std::int32_t HullSeriesCount;

    extern pas::Array<aConst::TIllnessInfo, 1, 24> CaptainHealthDefinitions;

    extern aConst::TRadiationHealthDefinitions RadiationHealthDefinitions;

    extern pas::DynArray<std::int32_t> MicroModuleCandidateIndices;

    void IncrementWordSaturating(pas::Var<std::uint16_t> Value);

    pas::WideString OwnerToSys(std::uint8_t OwnerId);

    std::uint8_t IsKnownOwnerName(const pas::WideString& Name);

    std::uint8_t MatchesOwnerName(std::uint8_t OwnerId, const pas::WideString& Name);

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

    TWeaponDamageClass ClassifyWeaponDamageFlags(std::uint32_t Flags);

    std::uint8_t ShipToHullType(pas::Object* Ship);

    std::uint8_t RaceToOwner(std::uint8_t RaceId);

    std::uint8_t OwnerFromInternalName(const pas::WideString& Name);

    std::uint8_t OwnerToRace(std::uint8_t OwnerId);

    pas::WideString RaceToSys(std::uint8_t RaceId);

    std::uint8_t NumberToRace(std::int32_t Value);

    std::uint8_t SysToReward(const pas::WideString& Name);

    std::uint8_t SysToShipType(const pas::WideString& Name);

    std::uint32_t OwnerToFilmColor(std::int8_t OwnerId);

    std::uint32_t CustomFactionToFilmColor(pas::WideString Faction);

    std::int32_t GetCustomFactionPlanetIconNumber(pas::WideString Faction);

    std::uint8_t SizeTagToLevel(const pas::WideString& Tag);

    std::int32_t GenerateValueForSizeLevel(std::uint8_t Level, std::int32_t Minimum, std::int32_t Maximum, std::uint8_t VariationPercent, std::uint32_t Seed);

    std::int32_t GetAverageItemSize(std::uint8_t ItemType);

    std::uint8_t PickRandomItemType(TItemTypeSelection Mask);

    std::uint8_t PickRandomItemTypeFromSeed(TItemTypeSelection Mask, std::uint32_t& Seed);

    std::int32_t CountItemTypesInMask(TItemTypeSelection Mask);

    std::uint8_t GetItemTypeFromMask(TItemTypeSelection Mask, std::int32_t Index);

    std::uint8_t PickRandomEquipmentOwner(std::uint32_t RandomValue);

    pas::WideString LookupNamedColorTag(pas::WideString Name);

    pas::WideString LocalizedText(const pas::WideString& Path);

    pas::WideString LocalizedColorText(const pas::WideString& Path);

    void ExpandLocalizedTextMarkup(pas::WideString& Text);

    void ExpandLocalizedTextMarkupAndPrefixLines(pas::WideString& Text);

    pas::WideString PickLocalizedTextVariant(const pas::WideString& Path, std::int32_t SeedOffset);

    aGalaxyStruct::TRelationLevel RelationValueToLevel(std::uint8_t Value);

    pas::WideString GetFactionEmblemPath(pas::WideString Faction);

    std::int32_t FindMicroModuleTemplateByCustomTag(pas::WideString CustomTag);

} // namespace aConst
