#include "layout/aConst.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aPirate.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/aTransport.hpp"
#include "types/aWarrior.hpp"
#include "units/CrcUnit.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_Main.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"

namespace aConst {
    struct CppGlobalRegion0 {
        std::uint32_t IntegrityDataBegin;
        std::int32_t CurrentSaveVersion;
        std::int32_t MinimumLoadableSaveVersion;
        pas::WideString LocalizedTextLinePrefix;
        std::int32_t GalaxyStarCount;
        std::int32_t GalaxySizeY;
        std::int32_t GalaxySizeX;
        std::uint8_t MaximumNewGameDifficulty;
        aGalaxyStruct::TGalaxyDifficultyTuningTable GalaxyDifficultyTuning;
        pas::Array<aConst::TRelationTypeInfo, 0, 4> RelationInfo;
        pas::Array<aConst::TEconomyInfo, 0, 2> PlanetEconomyInfo;
        pas::Array<aConst::TShipTypeInfo, 0, 13> ShipTypeNames;
        pas::Array<std::uint8_t, 6, 13> StationDefaultStandings;
        aGalaxyStruct::TFactionStandingMasks NonTargetableStationStandingMasks;
        aGalaxyStruct::TFactionStandingMasks FactionStandingMasks;
        pas::Array<aConst::TStatusInfo, 0, 2> CareerTuning;
        pas::Array<pas::WideString, 0, 2> TransportTypeNames;
        pas::Array<aGalaxyStruct::TKlingType, 0, 7> DominatorDisplayOrder;
        pas::Array<pas::WideString, 0, 7> DominatorShipTypeNames;
        pas::Array<aConst::TKlingTypeInfo, 0, 7> DominatorShipDefinitions;
        pas::Array<double, 0, 3> DominatorRetreatStrengthByTier;
        pas::Array<pas::WideString, 0, 2> DominatorSeriesNames;
        pas::Array<double, 0, 2> DominatorResearchRateMultipliers;
        pas::Array<double, 0, 2> ResearchProgramCostFactors;
        pas::Array<pas::WideString, 0, 61> ScriptActionTypeNames;
        aGalaxyStruct::TShipTypeMask NonNegotiatingShipTypes;
        pas::Array<aConst::SEquipment, 0, 7> EquipmentSlotLayouts;
        pas::Array<pas::WideString, 0, 75> ItemTypeNames;
        pas::Array<aConst::TGoodsInfo, 0, 7> GoodsMarket;
        aGalaxyStruct::TGoodsTextOrder GoodsTextOrder;
        pas::Array<pas::WideString, 0, 4> MissionTypeNames;
        pas::Array<aConst::TOwnerInfo, 0, 7> OwnerInfo;
        aGalaxyStruct::TPlanetOwnerMasks PlanetOwnerMasks;
        aGalaxyStruct::TOwnerRelationTable OwnerRelations;
        aGalaxyStruct::TPlanetRaceMarketTable PlanetRaceMarket;
        aGalaxyStruct::TPlanetEquipmentOfferQuotaTable PlanetEquipmentOfferQuotas;
        aConst::TStationEquipmentOfferQuotaTable StationEquipmentOfferQuotas;
        pas::Array<pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>, 6, 13> StationGoodsFactors;
        pas::Array<aConst::TGovermentInfo, 0, 4> PlanetGovernmentMarket;
        aConst::TGoodsLegalityTable GoodsLegalOnPlanet;
        pas::Array<pas::WideString, 0, 5> MedalNames;
        pas::Array<pas::WideString, 0, 7> CoalitionRankNames;
        pas::Array<std::uint16_t, 0, 7> CoalitionRankPointThresholds;
        pas::Array<pas::WideString, 0, 7> PirateRankNames;
        pas::Array<std::uint16_t, 0, 7> PirateRankPointThresholds;
        pas::Array<pas::WideString, 0, 5> SkillConfigNames;
        pas::Array<pas::Array<float, 0, 5>, 0, 4> RaceSkillEvaluationFactors;
        pas::Array<pas::Array<std::uint16_t, 0, 5>, 0, 6> PilotSkillEffects;
        pas::Array<std::uint16_t, 0, 6> TechnicalSkillSatelliteLimits;
        pas::Array<std::uint16_t, 0, 6> TradingSkillSalePercent;
        pas::Array<std::uint16_t, 0, 6> LeadershipExperiencePercent;
        std::int32_t MaxPlanetNews;
        pas::Array<pas::WideString, 0, 5> SizeTagNames;
        pas::Array<float, 0, 5> WealthDemandScales;
        pas::Array<std::int32_t, 0, 10> MinimumHullSlotCounts;
        pas::Array<std::int32_t, 0, 10> DefaultHullSlotCounts;
        pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7> RangerHullSlots;
        pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7> WarriorHullSlots;
        pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7> PirateHullSlots;
        pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7> TransportHullSlots;
        pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7> LinerHullSlots;
        pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7> DiplomatHullSlots;
        pas::Array<std::int32_t, 0, 10> TranclucatorHullSlots;
        pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7> StationHullSlots;
        pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7> DominatorHullSlots;
        pas::Array<std::int32_t, 0, 10> HullType9Slots;
        pas::Array<std::int32_t, 0, 10> HullType10Slots;
        pas::Array<aConst::TEquipmentBonusKind, 0, 10> HullSlotBonusKinds;
        aConst::TOwnerWeaponAvailabilityTable OwnerWeaponAvailability;
        pas::Array<pas::WideString, 0, 20> WeaponDamageFlagNames;
        pas::Array<aConst::TPrimaryDamageTypeInfo, 0, 2> WeaponDamageClasses;
        pas::Array<float, 0, 6> CombatStatusHullFactors;
        pas::Array<float, 0, 6> CombatStatusAccumulationFactors;
        pas::Array<pas::WideString, 0, 42> EquipmentBonusNames;
        pas::Array<aGalaxyStruct::TPilotSkill, 0, 5> EquipmentBonusSkills;
        aConst::TEquipmentSizeFactorTable EquipmentSizeFactors;
        aConst::TWeaponRangeLevelFactors WeaponRangeLevelFactors;
        pas::Array<aConst::tInventionInfo, 0, 19> PlanetInventionInfo;
        aConst::TEquipmentInventionIndexTable EquipmentInventionIndices;
        pas::Array<pas::WideString, 0, 11> CoalitionProjectNames;
        pas::Array<std::int32_t, 0, 11> StationServiceRepeatPeriods;
        pas::Array<pas::WideString, 0, 11> ProgramNames;
        aConst::TProgramDurationTable ProgramDuration;
        pas::Array<std::int32_t, 0, 11> PirateProgramBatchSizes;
        pas::Array<std::int32_t, 0, 11> PirateProgramBaseCosts;
        std::uint8_t GoodsMarketBaseCaptured;
        std::uint32_t IntegrityDataEnd;
    };

    CppGlobalRegion0 cpp_global_region_0 = CppGlobalRegion0{.IntegrityDataBegin = 0u, .CurrentSaveVersion = 167, .MinimumLoadableSaveVersion = 44, .LocalizedTextLinePrefix = u"    "_w, .GalaxyStarCount = 73, .GalaxySizeY = 100, .GalaxySizeX = 145, .MaximumNewGameDifficulty = 9, .GalaxyDifficultyTuning = aGalaxyStruct::TGalaxyDifficultyTuningTable{{
        {.GoodsEventDurationFactor = 0.7f, .QuestTimeAndExperienceFactor = 0.85f, .EquipmentWearFactor = 0.75f, .InventionProgressScale = 1.1f, .ArcadeRewardScale = 1.3f, .QuestMoneyFactor = 1.2f, .StartingPlayerMoney = 4000, .InitialPirateControlPercent = static_cast<std::uint8_t>(8), .MarketPriceBandSqueeze = -0.2f, .RandomHoleSpawnRollMaximum = 80, .MaximumDominatorResearchRate = 0.05f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(2), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(4), .ArcadeDamageTakenScale = 0.9f, .CoalitionToPirateBalanceRatio = 1.0E+1f},
        {.GoodsEventDurationFactor = 1.0f, .QuestTimeAndExperienceFactor = 1.0f, .EquipmentWearFactor = 1.0f, .InventionProgressScale = 1.0f, .ArcadeRewardScale = 1.0f, .QuestMoneyFactor = 1.0f, .StartingPlayerMoney = 1300, .InitialPirateControlPercent = static_cast<std::uint8_t>(12), .MarketPriceBandSqueeze = 0.0f, .RandomHoleSpawnRollMaximum = 100, .MaximumDominatorResearchRate = 0.04f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(4), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(3), .ArcadeDamageTakenScale = 1.0f, .CoalitionToPirateBalanceRatio = 5.0f},
        {.GoodsEventDurationFactor = 1.2f, .QuestTimeAndExperienceFactor = 1.15f, .EquipmentWearFactor = 1.3f, .InventionProgressScale = 0.9f, .ArcadeRewardScale = 0.6f, .QuestMoneyFactor = 0.7f, .StartingPlayerMoney = 800, .InitialPirateControlPercent = static_cast<std::uint8_t>(16), .MarketPriceBandSqueeze = 0.1f, .RandomHoleSpawnRollMaximum = 130, .MaximumDominatorResearchRate = 0.03f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(5), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(2), .ArcadeDamageTakenScale = 1.7f, .CoalitionToPirateBalanceRatio = 2.5f},
        {.GoodsEventDurationFactor = 1.5f, .QuestTimeAndExperienceFactor = 1.3f, .EquipmentWearFactor = 1.6f, .InventionProgressScale = 0.8f, .ArcadeRewardScale = 0.3f, .QuestMoneyFactor = 0.5f, .StartingPlayerMoney = 400, .InitialPirateControlPercent = static_cast<std::uint8_t>(20), .MarketPriceBandSqueeze = 0.15f, .RandomHoleSpawnRollMaximum = 170, .MaximumDominatorResearchRate = 0.02f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(6), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(2), .ArcadeDamageTakenScale = 2.3f, .CoalitionToPirateBalanceRatio = 1.8f},
        {.GoodsEventDurationFactor = 0.0f, .QuestTimeAndExperienceFactor = 0.0f, .EquipmentWearFactor = 0.0f, .InventionProgressScale = 0.0f, .ArcadeRewardScale = 0.0f, .QuestMoneyFactor = 0.0f, .InitialPirateControlPercent = static_cast<std::uint8_t>(0), .MarketPriceBandSqueeze = 0.0f, .MaximumDominatorResearchRate = 0.0f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(0), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(0), .ArcadeDamageTakenScale = 0.0f, .CoalitionToPirateBalanceRatio = 0.0f},
        {.GoodsEventDurationFactor = 0.0f, .QuestTimeAndExperienceFactor = 0.0f, .EquipmentWearFactor = 0.0f, .InventionProgressScale = 0.0f, .ArcadeRewardScale = 0.0f, .QuestMoneyFactor = 0.0f, .InitialPirateControlPercent = static_cast<std::uint8_t>(0), .MarketPriceBandSqueeze = 0.0f, .MaximumDominatorResearchRate = 0.0f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(0), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(0), .ArcadeDamageTakenScale = 0.0f, .CoalitionToPirateBalanceRatio = 0.0f},
        {.GoodsEventDurationFactor = 0.0f, .QuestTimeAndExperienceFactor = 0.0f, .EquipmentWearFactor = 0.0f, .InventionProgressScale = 0.0f, .ArcadeRewardScale = 0.0f, .QuestMoneyFactor = 0.0f, .InitialPirateControlPercent = static_cast<std::uint8_t>(0), .MarketPriceBandSqueeze = 0.0f, .MaximumDominatorResearchRate = 0.0f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(0), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(0), .ArcadeDamageTakenScale = 0.0f, .CoalitionToPirateBalanceRatio = 0.0f},
        {.GoodsEventDurationFactor = 0.0f, .QuestTimeAndExperienceFactor = 0.0f, .EquipmentWearFactor = 0.0f, .InventionProgressScale = 0.0f, .ArcadeRewardScale = 0.0f, .QuestMoneyFactor = 0.0f, .InitialPirateControlPercent = static_cast<std::uint8_t>(0), .MarketPriceBandSqueeze = 0.0f, .MaximumDominatorResearchRate = 0.0f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(0), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(0), .ArcadeDamageTakenScale = 0.0f, .CoalitionToPirateBalanceRatio = 0.0f},
        {.GoodsEventDurationFactor = 0.0f, .QuestTimeAndExperienceFactor = 0.0f, .EquipmentWearFactor = 0.0f, .InventionProgressScale = 0.0f, .ArcadeRewardScale = 0.0f, .QuestMoneyFactor = 0.0f, .InitialPirateControlPercent = static_cast<std::uint8_t>(0), .MarketPriceBandSqueeze = 0.0f, .MaximumDominatorResearchRate = 0.0f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(0), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(0), .ArcadeDamageTakenScale = 0.0f, .CoalitionToPirateBalanceRatio = 0.0f},
        {.GoodsEventDurationFactor = 0.0f, .QuestTimeAndExperienceFactor = 0.0f, .EquipmentWearFactor = 0.0f, .InventionProgressScale = 0.0f, .ArcadeRewardScale = 0.0f, .QuestMoneyFactor = 0.0f, .InitialPirateControlPercent = static_cast<std::uint8_t>(0), .MarketPriceBandSqueeze = 0.0f, .MaximumDominatorResearchRate = 0.0f, .MaximumResearchMaterialConsumption = static_cast<std::uint8_t>(0), .MaximumQuestProgramRewardCount = static_cast<std::uint8_t>(0), .ArcadeDamageTakenScale = 0.0f, .CoalitionToPirateBalanceRatio = 0.0f},
    }}, .RelationInfo = pas::Array<aConst::TRelationTypeInfo, 0, 4>{{{.InternalName = u"War"_w}, {.InternalName = u"Bad"_w, .MinimumValue = 10}, {.InternalName = u"Normal"_w, .MinimumValue = 30}, {.InternalName = u"Good"_w, .MinimumValue = 60}, {.InternalName = u"Best"_w, .MinimumValue = 80}}}, .PlanetEconomyInfo = pas::Array<aConst::TEconomyInfo, 0, 2>{{{.InternalName = u"Agriculture"_w, .InventionProgressScale = 0.7f}, {.InternalName = u"Mixed"_w, .InventionProgressScale = 1.0f}, {.InternalName = u"Industrial"_w, .InventionProgressScale = 1.4f}}}, .ShipTypeNames = pas::Array<aConst::TShipTypeInfo, 0, 13>{{
        {.Name = u"Kling"_w}, {.Name = u"Ranger"_w}, {.Name = u"Transport"_w}, {.Name = u"Pirate"_w},
        {.Name = u"Warrior"_w}, {.Name = u"Tranclucator"_w}, {.Name = u"RC"_w}, {.Name = u"PB"_w}, {.Name = u"WB"_w},
        {.Name = u"SB"_w}, {.Name = u"BK"_w}, {.Name = u"MC"_w}, {.Name = u"CB"_w}, {.Name = u"UB"_w},
    }}, .StationDefaultStandings = pas::Array<std::uint8_t, 6, 13>{{static_cast<std::uint8_t>(aGalaxyStruct::ssCoalitionMilitary), static_cast<std::uint8_t>(aGalaxyStruct::ssPiratePassive), static_cast<std::uint8_t>(aGalaxyStruct::ssCoalitionMilitary), static_cast<std::uint8_t>(aGalaxyStruct::ssCoalitionActive), static_cast<std::uint8_t>(aGalaxyStruct::ssCoalitionActive), static_cast<std::uint8_t>(aGalaxyStruct::ssNeutral), static_cast<std::uint8_t>(aGalaxyStruct::ssPirateMilitary), static_cast<std::uint8_t>(aGalaxyStruct::ssUnaligned)}}, .NonTargetableStationStandingMasks = aGalaxyStruct::TFactionStandingMasks{{pas::constant_set<aGalaxyStruct::TStationStandingMask>({{aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssNeutral}}), pas::constant_set<aGalaxyStruct::TStationStandingMask>({{aGalaxyStruct::ssDominator}}), pas::constant_set<aGalaxyStruct::TStationStandingMask>({{aGalaxyStruct::ssPiratePassive, aGalaxyStruct::ssPirateMilitary}})}}, .FactionStandingMasks = aGalaxyStruct::TFactionStandingMasks{{pas::constant_set<aGalaxyStruct::TStationStandingMask>({{aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssPiratePassive}}), pas::constant_set<aGalaxyStruct::TStationStandingMask>({{aGalaxyStruct::ssDominator}}), pas::constant_set<aGalaxyStruct::TStationStandingMask>({{aGalaxyStruct::ssCoalitionPassive, aGalaxyStruct::ssPirateMilitary}})}}, .CareerTuning = pas::Array<aConst::TStatusInfo, 0, 2>{{{.Name = u"Trader"_w, .MinimumWealthToAverageRatio = 1.5, .MinimumWealthToBestRatio = 0.4, .MinimumStrengthToAverageRatio = 0.9, .MinimumStrengthToBestRatio = 0.3}, {.Name = u"Pirate"_w, .MinimumWealthToAverageRatio = 0.9, .MinimumWealthToBestRatio = 0.35, .MinimumStrengthToAverageRatio = 1.1, .MinimumStrengthToBestRatio = 0.5}, {.Name = u"Warrior"_w, .MinimumWealthToAverageRatio = 0.8, .MinimumWealthToBestRatio = 0.25, .MinimumStrengthToAverageRatio = 1.2, .MinimumStrengthToBestRatio = 0.6}}}, .TransportTypeNames = pas::Array<pas::WideString, 0, 2>{{u"Transport"_w, u"Liner"_w, u"Diplomat"_w}}, .DominatorDisplayOrder = pas::Array<aGalaxyStruct::TKlingType, 0, 7>{{aGalaxyStruct::ktBoss, aGalaxyStruct::ktBertor, aGalaxyStruct::ktEquentor, aGalaxyStruct::ktUrgant, aGalaxyStruct::ktSmersh, aGalaxyStruct::ktMenok, aGalaxyStruct::ktShtip, aGalaxyStruct::ktKlig}}, .DominatorShipTypeNames = pas::Array<pas::WideString, 0, 7>{{u"K0"_w, u"K1"_w, u"K2"_w, u"K3"_w, u"K4"_w, u"K5"_w, u"K6"_w, u"K7"_w}}, .DominatorShipDefinitions = pas::Array<aConst::TKlingTypeInfo, 0, 7>{{{.DisplayNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .InitialWealthScale = 1.0E+1, .BaseNodeReserve = static_cast<std::uint16_t>(500), .KillExperience = static_cast<std::uint16_t>(5000), .RankPoints = static_cast<std::uint16_t>(250), .PirateRankPoints = static_cast<std::uint16_t>(0), .RankImageIndex = 7, .FactionStrengthWeight = 1.0E+1}, {.DisplayNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .MinimumHullSize = 900, .MaximumHullSize = 1400, .InitialWealthScale = 0.7, .BaseNodeReserve = static_cast<std::uint16_t>(100), .KillExperience = static_cast<std::uint16_t>(1000), .RankPoints = static_cast<std::uint16_t>(48), .PirateRankPoints = static_cast<std::uint16_t>(16), .RankImageIndex = 5, .FactionStrengthWeight = 5.0}, {.DisplayNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .MinimumHullSize = 700, .MaximumHullSize = 900, .InitialWealthScale = 0.6, .BaseNodeReserve = static_cast<std::uint16_t>(50), .KillExperience = static_cast<std::uint16_t>(500), .RankPoints = static_cast<std::uint16_t>(24), .PirateRankPoints = static_cast<std::uint16_t>(8), .RankImageIndex = 4, .FactionStrengthWeight = 3.5}, {.DisplayNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .MinimumHullSize = 500, .MaximumHullSize = 700, .InitialWealthScale = 0.5, .BaseNodeReserve = static_cast<std::uint16_t>(30), .KillExperience = static_cast<std::uint16_t>(300), .RankPoints = static_cast<std::uint16_t>(12), .PirateRankPoints = static_cast<std::uint16_t>(4), .RankImageIndex = 3, .FactionStrengthWeight = 2.0}, {.DisplayNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .MinimumHullSize = 350, .MaximumHullSize = 500, .InitialWealthScale = 0.3, .BaseNodeReserve = static_cast<std::uint16_t>(15), .KillExperience = static_cast<std::uint16_t>(150), .RankPoints = static_cast<std::uint16_t>(6), .PirateRankPoints = static_cast<std::uint16_t>(2), .RankImageIndex = 2, .FactionStrengthWeight = 1.0}, {.DisplayNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .MinimumHullSize = 250, .MaximumHullSize = 350, .InitialWealthScale = 0.2, .BaseNodeReserve = static_cast<std::uint16_t>(10), .KillExperience = static_cast<std::uint16_t>(100), .RankPoints = static_cast<std::uint16_t>(3), .PirateRankPoints = static_cast<std::uint16_t>(1), .RankImageIndex = 1, .FactionStrengthWeight = 1.0}, {.DisplayNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .MinimumHullSize = 1250, .MaximumHullSize = 2000, .InitialWealthScale = 2.0, .BaseNodeReserve = static_cast<std::uint16_t>(200), .KillExperience = static_cast<std::uint16_t>(2000), .RankPoints = static_cast<std::uint16_t>(60), .PirateRankPoints = static_cast<std::uint16_t>(24), .RankImageIndex = 7, .FactionStrengthWeight = 7.5}, {.DisplayNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .MinimumHullSize = 150, .MaximumHullSize = 250, .InitialWealthScale = 0.15, .BaseNodeReserve = static_cast<std::uint16_t>(5), .KillExperience = static_cast<std::uint16_t>(50), .RankPoints = static_cast<std::uint16_t>(1), .PirateRankPoints = static_cast<std::uint16_t>(1), .RankImageIndex = 1, .FactionStrengthWeight = 0.0}}}, .DominatorRetreatStrengthByTier = pas::Array<double, 0, 3>{{2.0, 2.2, 2.6, 3.0}}, .DominatorSeriesNames = pas::Array<pas::WideString, 0, 2>{{u"Blazer"_w, u"Keller"_w, u"Terron"_w}}, .DominatorResearchRateMultipliers = pas::Array<double, 0, 2>{{1.0, 1.2, 0.8}}, .ResearchProgramCostFactors = pas::Array<double, 0, 2>{{1.0, 1.4, 1.8}}, .ScriptActionTypeNames = pas::Array<pas::WideString, 0, 61>{{
        u"t_OnStep"_w, u"t_OnWeaponShot"_w, u"t_OnMissileShot"_w, u"t_OnDealingDamage"_w, u"t_OnDealingFatalDamage"_w,
        u"t_OnDealingKamikazeDamage"_w, u"t_OnTakingDamage"_w, u"t_OnTakingDamageEn"_w, u"t_OnTakingDamageSp"_w,
        u"t_OnTakingDamageMi"_w, u"t_OnWeaponShot2"_w, u"t_OnMissileShot2"_w, u"t_OnGettingWeaponHit"_w,
        u"t_OnGettingMissileHit"_w, u"t_OnDroidRepair"_w, u"t_OnItemPickUp"_w, u"t_OnScan"_w,
        u"t_OnChameleonConfusion"_w, u"t_OnScanPossibility"_w, u"t_OnAnotherItem"_w, u"t_OnAnotherItem2"_w,
        u"t_OnAnotherGoods"_w, u"t_OnItemHit"_w, u"t_OnMissileHittingObject"_w, u"t_OnEnteringForm"_w,
        u"t_OnLeavingForm"_w, u"t_OnReEnteringForm"_w, u"t_OnEnteringOtherShip"_w, u"t_OnLeavingOtherShip"_w,
        u"t_OnReEnteringOtherShip"_w, u"t_OnPlayerSkillIncrease"_w, u"t_OnPlayerTalkedWithShip"_w,
        u"t_OnShipTalkedWithPlayer"_w, u"t_OnDropItem"_w, u"t_OnDropItemFixed"_w, u"t_OnMovingItemToStorage"_w,
        u"t_OnReduceEqBattle"_w, u"t_OnReduceEqUse"_w, u"t_OnReduceEqForce"_w, u"t_OnReduceEqForsage"_w,
        u"t_OnItemDestroy"_w, u"t_OnPlayerChangeHull"_w, u"t_OnPlayerUseMM"_w, u"t_OnPlayerBuyEq"_w, u"t_OnItemEquip"_w,
        u"t_OnItemDeEquip"_w, u"t_OnTrancPacking"_w, u"t_OnShipBuysGoods"_w, u"t_OnShipSellsGoods"_w,
        u"t_OnShowingItemInfo"_w, u"t_OnShowingShipInfo"_w, u"t_OnShowingStarInfo"_w, u"t_OnNonStandartEqChange"_w,
        u"t_OnCustomTargetting"_w, u"t_OnCustomTargettingCheck"_w, u"t_OnStartAB"_w, u"t_OnABItemDrop"_w,
        u"t_OnGovItemReward"_w, u"t_OnCheckingUsability"_w, u"t_OnCheckingUsability2"_w, u"t_OnCheckingUsabilityGoods"_w,
        u"t_OnDeath"_w,
    }}, .NonNegotiatingShipTypes = pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling}, {aGalaxyStruct::stTranclucator, 13}}), .EquipmentSlotLayouts = pas::Array<aConst::SEquipment, 0, 7>{{{.ItemType = aConst::t_FuelTanks, .Name = u"FuelTanks"_w}, {.ItemType = aConst::t_Engine, .Name = u"Engine"_w}, {.ItemType = aConst::t_Radar, .Name = u"Radar"_w}, {.ItemType = aConst::t_Scaner, .Name = u"Scaner"_w}, {.ItemType = aConst::t_RepairRobot, .Name = u"RepairRobot"_w}, {.ItemType = aConst::t_CargoHook, .Name = u"CargoHook"_w}, {.ItemType = aConst::t_DefGenerator, .Name = u"DefGenerator"_w}, {.ItemType = aConst::t_Weapon1, .Name = u"Weapon"_w}}}, .ItemTypeNames = pas::Array<pas::WideString, 0, 75>{{
        u"Food"_w, u"Medicine"_w, u"Technics"_w, u"Luxury"_w, u"Minerals"_w, u"Alcohol"_w, u"Arms"_w, u"Narcotics"_w,
        u"Artefact"_w, u"Artefact2"_w, u"ArtHull"_w, u"ArtFuel"_w, u"ArtSpeed"_w, u"ArtPower"_w, u"ArtRadar"_w,
        u"ArtScaner"_w, u"ArtDroid"_w, u"ArtNano"_w, u"ArtHook"_w, u"ArtDef"_w, u"ArtAnalyzer"_w, u"ArtMiniExpl"_w,
        u"ArtAntigrav"_w, u"ArtTransmitter"_w, u"ArtBomb"_w, u"ArtTranclucator"_w, u"ArtDefToEnergy"_w,
        u"ArtEnergyPulse"_w, u"ArtEnergyDef"_w, u"ArtSplinter"_w, u"ArtDecelerate"_w, u"ArtMissileDef"_w,
        u"ArtForsage"_w, u"ArtWeaponToSpeed"_w, u"ArtGiperJump"_w, u"ArtBlackHole"_w, u"ArtDefToArms1"_w,
        u"ArtDefToArms2"_w, u"ArtArtefactor"_w, u"ArtBio"_w, u"ArtPDTurret"_w, u"ArtFastRacks"_w, u"Hull"_w,
        u"FuelTanks"_w, u"Engine"_w, u"Radar"_w, u"Scaner"_w, u"RepairRobot"_w, u"CargoHook"_w, u"DefGenerator"_w,
        u"W01"_w, u"W02"_w, u"W03"_w, u"W04"_w, u"W05"_w, u"W06"_w, u"W07"_w, u"W08"_w,
        u"W09"_w, u"W10"_w, u"W11"_w, u"W12"_w, u"W13"_w, u"W14"_w, u"W15"_w, u"W16"_w,
        u"W17"_w, u"W18"_w, u"CustomWeapon"_w, u"Protoplasm"_w, u"UselessItem"_w, u"Nod"_w, u"Cistern"_w, u"Satellite"_w,
        u"TreasureMap"_w, u"UselessCountableItem"_w,
    }}, .GoodsMarket = pas::Array<aConst::TGoodsInfo, 0, 7>{{{.InternalName = u"Food"_w, .BaseStock = 300, .MinPrice = 17, .AveragePrice = 30, .MaxPrice = 43, .TradeExperienceFactor = 1.654f, .EconomyFactors = pas::Array<float, 0, 2>{{1.15f, 1.0f, 0.85f}}, .PirateEconomyFactor = 0.9f}, {.InternalName = u"Medicine"_w, .BaseStock = 160, .MinPrice = 27, .AveragePrice = 40, .MaxPrice = 53, .TradeExperienceFactor = 2.038f, .EconomyFactors = pas::Array<float, 0, 2>{{1.1f, 1.0f, 0.9f}}, .PirateEconomyFactor = 0.7f}, {.InternalName = u"Technics"_w, .BaseStock = 100, .MinPrice = 62, .AveragePrice = 80, .MaxPrice = 98, .TradeExperienceFactor = 2.722f, .EconomyFactors = pas::Array<float, 0, 2>{{0.85f, 1.0f, 1.15f}}, .PirateEconomyFactor = 0.8f}, {.InternalName = u"Luxury"_w, .BaseStock = 60, .MinPrice = 160, .AveragePrice = 200, .MaxPrice = 240, .TradeExperienceFactor = 3.0f, .EconomyFactors = pas::Array<float, 0, 2>{{1.0f, 1.1f, 1.05f}}, .PirateEconomyFactor = 0.8f}, {.InternalName = u"Minerals"_w, .BaseStock = 250, .MinPrice = 8, .AveragePrice = 12, .MaxPrice = 16, .TradeExperienceFactor = 2.0f, .EconomyFactors = pas::Array<float, 0, 2>{{1.15f, 1.0f, 0.85f}}, .PirateEconomyFactor = 2.5f}, {.InternalName = u"Alcohol"_w, .BaseStock = 120, .MinPrice = 25, .AveragePrice = 40, .MaxPrice = 55, .TradeExperienceFactor = 1.833f, .EconomyFactors = pas::Array<float, 0, 2>{{1.2f, 1.0f, 0.8f}}, .PirateEconomyFactor = 1.3f}, {.InternalName = u"Arms"_w, .BaseStock = 70, .MinPrice = 75, .AveragePrice = 100, .MaxPrice = 125, .TradeExperienceFactor = 2.5f, .EconomyFactors = pas::Array<float, 0, 2>{{0.85f, 1.0f, 1.15f}}, .PirateEconomyFactor = 1.5f}, {.InternalName = u"Narcotics"_w, .BaseStock = 30, .MinPrice = 250, .AveragePrice = 400, .MaxPrice = 550, .TradeExperienceFactor = 1.833f, .EconomyFactors = pas::Array<float, 0, 2>{{1.1f, 1.0f, 0.9f}}, .PirateEconomyFactor = 2.0f}}}, .GoodsTextOrder = aGalaxyStruct::TGoodsTextOrder{{static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(5), static_cast<std::uint8_t>(4), static_cast<std::uint8_t>(3), static_cast<std::uint8_t>(2), static_cast<std::uint8_t>(6), static_cast<std::uint8_t>(7)}}, .MissionTypeNames = pas::Array<pas::WideString, 0, 4>{{u"SendLetter"_w, u"KillShip"_w, u"PlanetQuest"_w, u"DefSystem"_w, u"DefShip"_w}}, .OwnerInfo = pas::Array<aConst::TOwnerInfo, 0, 7>{{{.InternalName = u"Maloc"_w, .FuelPriceFactor = 0.7f, .EquipmentDurabilityFactor = 0.7f, .MinimumAfterburnerWear = 18, .MaximumAfterburnerWear = 22, .FearThresholdScale = 0.8f, .ColorTag = u"<color=255,000,000>"_w}, {.InternalName = u"Peleng"_w, .FuelPriceFactor = 0.9f, .EquipmentDurabilityFactor = 0.9f, .MinimumAfterburnerWear = 17, .MaximumAfterburnerWear = 21, .FearThresholdScale = 0.9f, .ColorTag = u"<color=000,255,000>"_w}, {.InternalName = u"People"_w, .FuelPriceFactor = 1.0f, .EquipmentDurabilityFactor = 1.0f, .MinimumAfterburnerWear = 16, .MaximumAfterburnerWear = 20, .FearThresholdScale = 1.0f, .ColorTag = u"<color=000,148,255>"_w}, {.InternalName = u"Fei"_w, .FuelPriceFactor = 1.15f, .EquipmentDurabilityFactor = 1.15f, .MinimumAfterburnerWear = 14, .MaximumAfterburnerWear = 17, .FearThresholdScale = 1.3f, .ColorTag = u"<color=255,147,241>"_w}, {.InternalName = u"Gaal"_w, .FuelPriceFactor = 1.3f, .EquipmentDurabilityFactor = 1.3f, .MinimumAfterburnerWear = 14, .MaximumAfterburnerWear = 16, .FearThresholdScale = 1.2f, .ColorTag = u"<color=237,247,062>"_w}, {.InternalName = u"Kling"_w, .FuelPriceFactor = 1.0f, .EquipmentDurabilityFactor = 1.0f, .MinimumAfterburnerWear = 10, .MaximumAfterburnerWear = 20, .FearThresholdScale = 1.0f, .ColorTag = u"<color=097,167,190>"_w}, {.InternalName = u"None"_w, .FuelPriceFactor = 1.0f, .EquipmentDurabilityFactor = 1.0f, .MinimumAfterburnerWear = 14, .MaximumAfterburnerWear = 20, .FearThresholdScale = 1.0f, .ColorTag = u"<color=255,000,255>"_w}, {.InternalName = u"PirateClan"_w, .FuelPriceFactor = 0.8f, .EquipmentDurabilityFactor = 1.0f, .MinimumAfterburnerWear = 16, .MaximumAfterburnerWear = 20, .FearThresholdScale = 0.5f, .ColorTag = u"<color=255,255,255>"_w}}}, .PlanetOwnerMasks = aGalaxyStruct::TPlanetOwnerMasks{.Coalition = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}}), .Dominators = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiDominator}}), .PirateClan = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPirate}})}, .OwnerRelations = aGalaxyStruct::TOwnerRelationTable{{aGalaxyStruct::TOwnerRelationRow{{static_cast<std::uint8_t>(100), static_cast<std::uint8_t>(80), static_cast<std::uint8_t>(70), static_cast<std::uint8_t>(40), static_cast<std::uint8_t>(60), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(30)}}, aGalaxyStruct::TOwnerRelationRow{{static_cast<std::uint8_t>(70), static_cast<std::uint8_t>(100), static_cast<std::uint8_t>(70), static_cast<std::uint8_t>(30), static_cast<std::uint8_t>(40), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(30)}}, aGalaxyStruct::TOwnerRelationRow{{static_cast<std::uint8_t>(50), static_cast<std::uint8_t>(40), static_cast<std::uint8_t>(100), static_cast<std::uint8_t>(70), static_cast<std::uint8_t>(90), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(30)}}, aGalaxyStruct::TOwnerRelationRow{{static_cast<std::uint8_t>(40), static_cast<std::uint8_t>(30), static_cast<std::uint8_t>(80), static_cast<std::uint8_t>(100), static_cast<std::uint8_t>(70), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(30)}}, aGalaxyStruct::TOwnerRelationRow{{static_cast<std::uint8_t>(70), static_cast<std::uint8_t>(70), static_cast<std::uint8_t>(80), static_cast<std::uint8_t>(90), static_cast<std::uint8_t>(100), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(30)}}, aGalaxyStruct::TOwnerRelationRow{{static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(100), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0)}}, aGalaxyStruct::TOwnerRelationRow{{static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(100), static_cast<std::uint8_t>(0)}}, aGalaxyStruct::TOwnerRelationRow{{static_cast<std::uint8_t>(50), static_cast<std::uint8_t>(50), static_cast<std::uint8_t>(50), static_cast<std::uint8_t>(50), static_cast<std::uint8_t>(50), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(100)}}}}, .PlanetRaceMarket = aGalaxyStruct::TPlanetRaceMarketTable{{{.InventionProgressScale = 0.85f, .InitialInventionBoostCount = 5, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 0.85, .StockFactor = 1.1}, {.PriceFactor = 1.1, .StockFactor = 1.1}, {.PriceFactor = 1.15, .StockFactor = 0.5}, {.PriceFactor = 0.85, .StockFactor = 0.4}, {.PriceFactor = 0.87, .StockFactor = 0.7}, {.PriceFactor = 1.1, .StockFactor = 0.1}, {.PriceFactor = 1.2, .StockFactor = 0.5}, {.PriceFactor = 0.8, .StockFactor = 0.2}}}, .GovernmentRollThresholds = pas::Array<std::uint8_t, 0, 4>{{static_cast<std::uint8_t>(10), static_cast<std::uint8_t>(30), static_cast<std::uint8_t>(50), static_cast<std::uint8_t>(90), static_cast<std::uint8_t>(95)}}, .RevolutionChance = 0.002f, .FriendlyRelationScale = 0.7f, .PirateRelationFactor = 1.1f, .UnknownFactor9C = 1.5f, .PirateRelationCeiling = static_cast<std::uint8_t>(60)}, {.InventionProgressScale = 0.95f, .InitialInventionBoostCount = 6, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 0.95, .StockFactor = 1.2}, {.PriceFactor = 1.05, .StockFactor = 1.0}, {.PriceFactor = 1.07, .StockFactor = 0.9}, {.PriceFactor = 1.15, .StockFactor = 1.2}, {.PriceFactor = 0.95, .StockFactor = 0.8}, {.PriceFactor = 1.05, .StockFactor = 1.1}, {.PriceFactor = 1.1, .StockFactor = 0.8}, {.PriceFactor = 0.95, .StockFactor = 0.6}}}, .GovernmentRollThresholds = pas::Array<std::uint8_t, 0, 4>{{static_cast<std::uint8_t>(20), static_cast<std::uint8_t>(40), static_cast<std::uint8_t>(70), static_cast<std::uint8_t>(85), static_cast<std::uint8_t>(90)}}, .RevolutionChance = 0.006f, .FriendlyRelationScale = 1.1f, .PirateRelationFactor = 1.5f, .UnknownFactor9C = 0.8f, .PirateRelationCeiling = static_cast<std::uint8_t>(80)}, {.InventionProgressScale = 1.0f, .InitialInventionBoostCount = 7, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.5}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 0.8}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}}}, .GovernmentRollThresholds = pas::Array<std::uint8_t, 0, 4>{{static_cast<std::uint8_t>(10), static_cast<std::uint8_t>(30), static_cast<std::uint8_t>(40), static_cast<std::uint8_t>(60), static_cast<std::uint8_t>(80)}}, .RevolutionChance = 0.004f, .FriendlyRelationScale = 1.0f, .PirateRelationFactor = 0.9f, .UnknownFactor9C = 1.2f, .PirateRelationCeiling = static_cast<std::uint8_t>(45)}, {.InventionProgressScale = 1.1f, .InitialInventionBoostCount = 8, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.05, .StockFactor = 0.7}, {.PriceFactor = 0.85, .StockFactor = 0.9}, {.PriceFactor = 0.87, .StockFactor = 1.4}, {.PriceFactor = 1.0, .StockFactor = 0.8}, {.PriceFactor = 1.15, .StockFactor = 0.5}, {.PriceFactor = 1.15, .StockFactor = 0.4}, {.PriceFactor = 0.9, .StockFactor = 0.4}, {.PriceFactor = 1.1, .StockFactor = 0.5}}}, .GovernmentRollThresholds = pas::Array<std::uint8_t, 0, 4>{{static_cast<std::uint8_t>(5), static_cast<std::uint8_t>(10), static_cast<std::uint8_t>(15), static_cast<std::uint8_t>(30), static_cast<std::uint8_t>(70)}}, .RevolutionChance = 0.003f, .FriendlyRelationScale = 1.1f, .PirateRelationFactor = 0.7f, .UnknownFactor9C = 1.0f, .PirateRelationCeiling = static_cast<std::uint8_t>(35)}, {.InventionProgressScale = 1.15f, .InitialInventionBoostCount = 9, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.1, .StockFactor = 0.5}, {.PriceFactor = 0.8, .StockFactor = 0.5}, {.PriceFactor = 0.8, .StockFactor = 0.8}, {.PriceFactor = 0.85, .StockFactor = 0.5}, {.PriceFactor = 1.1, .StockFactor = 0.3}, {.PriceFactor = 0.9, .StockFactor = 0.3}, {.PriceFactor = 0.84, .StockFactor = 0.1}, {.PriceFactor = 1.15, .StockFactor = 0.4}}}, .GovernmentRollThresholds = pas::Array<std::uint8_t, 0, 4>{{static_cast<std::uint8_t>(5), static_cast<std::uint8_t>(8), static_cast<std::uint8_t>(10), static_cast<std::uint8_t>(30), static_cast<std::uint8_t>(60)}}, .RevolutionChance = 0.002f, .FriendlyRelationScale = 1.3f, .PirateRelationFactor = 0.6f, .UnknownFactor9C = 0.9f, .PirateRelationCeiling = static_cast<std::uint8_t>(35)}}}, .PlanetEquipmentOfferQuotas = aGalaxyStruct::TPlanetEquipmentOfferQuotaTable{{aGalaxyStruct::TPlanetEquipmentOfferQuotaRow{{
        3, 2, 2, 2, 1, 2, 2, 2,
        6,
    }}, aGalaxyStruct::TPlanetEquipmentOfferQuotaRow{{
        4, 2, 2, 2, 1, 2, 2, 2,
        5,
    }}, aGalaxyStruct::TPlanetEquipmentOfferQuotaRow{{
        4, 2, 2, 2, 2, 1, 2, 2,
        5,
    }}, aGalaxyStruct::TPlanetEquipmentOfferQuotaRow{{
        3, 2, 2, 1, 1, 2, 2, 1,
        4,
    }}, aGalaxyStruct::TPlanetEquipmentOfferQuotaRow{{
        3, 2, 2, 2, 2, 1, 2, 2,
        4,
    }}}}, .StationEquipmentOfferQuotas = aConst::TStationEquipmentOfferQuotaTable{{{.Hulls = 4, .FuelTanks = 2, .Engines = 2, .Radars = 2, .Scanners = 2, .RepairRobots = 2, .CargoHooks = 2, .DefGenerators = 2, .Weapons = 4}, {.Hulls = 3, .FuelTanks = 2, .Engines = 2, .Radars = 2, .Scanners = 3, .RepairRobots = 2, .CargoHooks = 2, .DefGenerators = 2, .Weapons = 4}, {.Hulls = 4, .FuelTanks = 2, .Engines = 2, .Radars = 2, .Scanners = 2, .RepairRobots = 2, .CargoHooks = 2, .DefGenerators = 2, .Weapons = 6}, {.Hulls = 3, .FuelTanks = 2, .Engines = 2, .Radars = 2, .Scanners = 2, .RepairRobots = 2, .CargoHooks = 2, .DefGenerators = 2, .Weapons = 2}, {.Hulls = 5, .FuelTanks = 2, .Engines = 2, .Radars = 2, .Scanners = 2, .RepairRobots = 2, .CargoHooks = 2, .DefGenerators = 2, .Weapons = 2}, {.Hulls = 2, .FuelTanks = 1, .Engines = 2, .Radars = 1, .Scanners = 2, .RepairRobots = 1, .CargoHooks = 2, .DefGenerators = 2, .Weapons = 1}, {.Hulls = 3, .FuelTanks = 2, .Engines = 2, .Radars = 2, .Scanners = 3, .RepairRobots = 2, .CargoHooks = 2, .DefGenerators = 2, .Weapons = 4}, {.Hulls = 4, .FuelTanks = 2, .Engines = 2, .Radars = 2, .Scanners = 2, .RepairRobots = 2, .CargoHooks = 2, .DefGenerators = 2, .Weapons = 4}}}, .StationGoodsFactors = pas::Array<pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>, 6, 13>{{pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.15}, {.PriceFactor = 0.8, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 0.5, .StockFactor = 0.01}}}, pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 0.9, .StockFactor = 0.15}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.2}, {.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 0.8, .StockFactor = 0.15}, {.PriceFactor = 0.9, .StockFactor = 0.2}, {.PriceFactor = 0.9, .StockFactor = 0.3}, {.PriceFactor = 0.9, .StockFactor = 0.2}}}, pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.1, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 0.4, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 0.8, .StockFactor = 0.3}, {.PriceFactor = 0.5, .StockFactor = 0.01}}}, pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 0.8, .StockFactor = 0.3}, {.PriceFactor = 0.8, .StockFactor = 0.05}, {.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 1.1, .StockFactor = 0.05}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 0.5, .StockFactor = 0.01}}}, pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 0.9, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 0.9, .StockFactor = 0.3}, {.PriceFactor = 1.1, .StockFactor = 0.1}, {.PriceFactor = 0.8, .StockFactor = 0.1}, {.PriceFactor = 0.9, .StockFactor = 0.1}, {.PriceFactor = 1.0, .StockFactor = 0.2}, {.PriceFactor = 1.1, .StockFactor = 0.1}}}, pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 0.8, .StockFactor = 0.2}, {.PriceFactor = 0.8, .StockFactor = 0.3}, {.PriceFactor = 1.0, .StockFactor = 0.2}, {.PriceFactor = 1.0, .StockFactor = 0.05}, {.PriceFactor = 0.8, .StockFactor = 0.05}, {.PriceFactor = 0.9, .StockFactor = 0.15}, {.PriceFactor = 0.9, .StockFactor = 0.1}, {.PriceFactor = 0.7, .StockFactor = 0.2}}}, pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.1, .StockFactor = 0.05}, {.PriceFactor = 1.1, .StockFactor = 0.05}, {.PriceFactor = 0.9, .StockFactor = 0.25}, {.PriceFactor = 1.0, .StockFactor = 0.1}, {.PriceFactor = 0.8, .StockFactor = 0.3}, {.PriceFactor = 0.9, .StockFactor = 0.3}, {.PriceFactor = 0.8, .StockFactor = 0.3}, {.PriceFactor = 0.8, .StockFactor = 0.25}}}, pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 0.01}, {.PriceFactor = 1.0, .StockFactor = 0.01}, {.PriceFactor = 1.0, .StockFactor = 0.01}, {.PriceFactor = 1.0, .StockFactor = 0.01}, {.PriceFactor = 1.0, .StockFactor = 0.01}, {.PriceFactor = 1.0, .StockFactor = 0.01}, {.PriceFactor = 1.0, .StockFactor = 0.01}, {.PriceFactor = 1.0, .StockFactor = 0.01}}}}}, .PlanetGovernmentMarket = pas::Array<aConst::TGovermentInfo, 0, 4>{{{.InternalName = u"Anarchy"_w, .RevolutionRelationDelta = pas::Array<std::int8_t, 0, 2>{{static_cast<std::int8_t>(-30), static_cast<std::int8_t>(30), static_cast<std::int8_t>(0)}}, .QuestOfferProbabilities = pas::Array<float, 0, 4>{{0.2f, 0.6f, 0.8f, 0.1f, 0.3f}}, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 0.7}, {.PriceFactor = 1.0, .StockFactor = 0.7}, {.PriceFactor = 1.0, .StockFactor = 0.7}, {.PriceFactor = 1.1, .StockFactor = 1.0}, {.PriceFactor = 0.8, .StockFactor = 0.8}, {.PriceFactor = 0.8, .StockFactor = 1.0}, {.PriceFactor = 1.1, .StockFactor = 1.0}, {.PriceFactor = 0.9, .StockFactor = 1.0}}}}, {.InternalName = u"Dictatorship"_w, .RevolutionRelationDelta = pas::Array<std::int8_t, 0, 2>{{static_cast<std::int8_t>(-40), static_cast<std::int8_t>(20), static_cast<std::int8_t>(20)}}, .QuestOfferProbabilities = pas::Array<float, 0, 4>{{0.2f, 0.7f, 0.8f, 0.2f, 0.5f}}, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 0.7}, {.PriceFactor = 1.0, .StockFactor = 0.7}, {.PriceFactor = 1.0, .StockFactor = 0.7}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 0.9, .StockFactor = 1.0}, {.PriceFactor = 0.9, .StockFactor = 0.9}, {.PriceFactor = 1.0, .StockFactor = 0.9}, {.PriceFactor = 0.9, .StockFactor = 0.9}}}}, {.InternalName = u"Monarchy"_w, .RevolutionRelationDelta = pas::Array<std::int8_t, 0, 2>{{static_cast<std::int8_t>(-10), static_cast<std::int8_t>(0), static_cast<std::int8_t>(10)}}, .QuestOfferProbabilities = pas::Array<float, 0, 4>{{0.2f, 0.5f, 0.8f, 0.5f, 0.8f}}, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 0.9}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 0.8}, {.PriceFactor = 1.0, .StockFactor = 0.8}, {.PriceFactor = 1.0, .StockFactor = 0.8}}}}, {.InternalName = u"Republic"_w, .RevolutionRelationDelta = pas::Array<std::int8_t, 0, 2>{{static_cast<std::int8_t>(10), static_cast<std::int8_t>(-20), static_cast<std::int8_t>(0)}}, .QuestOfferProbabilities = pas::Array<float, 0, 4>{{0.2f, 0.3f, 0.8f, 0.7f, 0.9f}}, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.1, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 0.8}, {.PriceFactor = 1.0, .StockFactor = 0.7}, {.PriceFactor = 1.1, .StockFactor = 0.7}}}}, {.InternalName = u"Democracy"_w, .RevolutionRelationDelta = pas::Array<std::int8_t, 0, 2>{{static_cast<std::int8_t>(15), static_cast<std::int8_t>(-30), static_cast<std::int8_t>(0)}}, .QuestOfferProbabilities = pas::Array<float, 0, 4>{{0.2f, 0.3f, 0.8f, 0.7f, 0.9f}}, .GoodsFactors = pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>{{{.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 1.0}, {.PriceFactor = 1.1, .StockFactor = 1.0}, {.PriceFactor = 1.0, .StockFactor = 0.8}, {.PriceFactor = 1.0, .StockFactor = 0.5}, {.PriceFactor = 1.1, .StockFactor = 0.6}}}}}}, .GoodsLegalOnPlanet = aConst::TGoodsLegalityTable{{pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>{{pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}}}, pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>{{pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}}}, pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>{{pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}}}, pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>{{pas::Array<std::uint8_t, 0, 4>{{true, false, true, false, false}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}}}, pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>{{pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}}}, pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>{{pas::Array<std::uint8_t, 0, 4>{{true, false, false, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, false, false}}, pas::Array<std::uint8_t, 0, 4>{{true, false, false, true, true}}}}, pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>{{pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, false}}, pas::Array<std::uint8_t, 0, 4>{{true, true, false, false, false}}, pas::Array<std::uint8_t, 0, 4>{{true, true, false, false, false}}}}, pas::Array<pas::Array<std::uint8_t, 0, 4>, 0, 4>{{pas::Array<std::uint8_t, 0, 4>{{true, false, false, false, false}}, pas::Array<std::uint8_t, 0, 4>{{true, true, true, true, true}}, pas::Array<std::uint8_t, 0, 4>{{true, false, true, false, false}}, pas::Array<std::uint8_t, 0, 4>{{true, true, false, false, false}}, pas::Array<std::uint8_t, 0, 4>{{true, false, false, false, false}}}}}}, .MedalNames = pas::Array<pas::WideString, 0, 5>{{u"ForLiberationSystem"_w, u"ForAccomplishment"_w, u"ForSecretMission"_w, u"ForCowardice"_w, u"ForPerfidy"_w, u"ForPlanetBattle"_w}}, .CoalitionRankNames = pas::Array<pas::WideString, 0, 7>{{u"Rookie"_w, u"Cadet"_w, u"Pilot"_w, u"Wingman"_w, u"Leader"_w, u"Ace"_w, u"Commander"_w, u"Admiral"_w}}, .CoalitionRankPointThresholds = pas::Array<std::uint16_t, 0, 7>{{static_cast<std::uint16_t>(100), static_cast<std::uint16_t>(250), static_cast<std::uint16_t>(450), static_cast<std::uint16_t>(700), static_cast<std::uint16_t>(1000), static_cast<std::uint16_t>(1500), static_cast<std::uint16_t>(2000), static_cast<std::uint16_t>(0)}}, .PirateRankNames = pas::Array<pas::WideString, 0, 7>{{u"Noobie"_w, u"Kid"_w, u"Rader"_w, u"Skipper"_w, u"Rough"_w, u"Ataman"_w, u"Khan"_w, u"Baron"_w}}, .PirateRankPointThresholds = pas::Array<std::uint16_t, 0, 7>{{static_cast<std::uint16_t>(100), static_cast<std::uint16_t>(250), static_cast<std::uint16_t>(450), static_cast<std::uint16_t>(700), static_cast<std::uint16_t>(1000), static_cast<std::uint16_t>(1500), static_cast<std::uint16_t>(3000), static_cast<std::uint16_t>(0)}}, .SkillConfigNames = pas::Array<pas::WideString, 0, 5>{{u"sAccuracy"_w, u"sMobility"_w, u"sTechnical"_w, u"sTrader"_w, u"sCharm"_w, u"sLeadership"_w}}, .RaceSkillEvaluationFactors = pas::Array<pas::Array<float, 0, 5>, 0, 4>{{pas::Array<float, 0, 5>{{1.2f, 1.1f, 0.9f, 0.8f, 1.0f, 1.0f}}, pas::Array<float, 0, 5>{{1.0f, 1.2f, 0.8f, 1.1f, 1.0f, 0.9f}}, pas::Array<float, 0, 5>{{0.9f, 0.8f, 1.0f, 1.2f, 1.0f, 1.1f}}, pas::Array<float, 0, 5>{{1.1f, 1.0f, 1.2f, 0.8f, 0.9f, 1.0f}}, pas::Array<float, 0, 5>{{0.8f, 0.9f, 1.1f, 1.0f, 1.2f, 1.0f}}}}, .PilotSkillEffects = pas::Array<pas::Array<std::uint16_t, 0, 5>, 0, 6>{{pas::Array<std::uint16_t, 0, 5>{{static_cast<std::uint16_t>(0), static_cast<std::uint16_t>(0), static_cast<std::uint16_t>(0), static_cast<std::uint16_t>(30), static_cast<std::uint16_t>(0), static_cast<std::uint16_t>(0)}}, pas::Array<std::uint16_t, 0, 5>{{static_cast<std::uint16_t>(17), static_cast<std::uint16_t>(17), static_cast<std::uint16_t>(8), static_cast<std::uint16_t>(38), static_cast<std::uint16_t>(17), static_cast<std::uint16_t>(1)}}, pas::Array<std::uint16_t, 0, 5>{{static_cast<std::uint16_t>(33), static_cast<std::uint16_t>(33), static_cast<std::uint16_t>(17), static_cast<std::uint16_t>(47), static_cast<std::uint16_t>(33), static_cast<std::uint16_t>(2)}}, pas::Array<std::uint16_t, 0, 5>{{static_cast<std::uint16_t>(50), static_cast<std::uint16_t>(50), static_cast<std::uint16_t>(25), static_cast<std::uint16_t>(55), static_cast<std::uint16_t>(50), static_cast<std::uint16_t>(3)}}, pas::Array<std::uint16_t, 0, 5>{{static_cast<std::uint16_t>(67), static_cast<std::uint16_t>(67), static_cast<std::uint16_t>(33), static_cast<std::uint16_t>(63), static_cast<std::uint16_t>(67), static_cast<std::uint16_t>(4)}}, pas::Array<std::uint16_t, 0, 5>{{static_cast<std::uint16_t>(83), static_cast<std::uint16_t>(83), static_cast<std::uint16_t>(42), static_cast<std::uint16_t>(72), static_cast<std::uint16_t>(83), static_cast<std::uint16_t>(5)}}, pas::Array<std::uint16_t, 0, 5>{{static_cast<std::uint16_t>(100), static_cast<std::uint16_t>(100), static_cast<std::uint16_t>(50), static_cast<std::uint16_t>(80), static_cast<std::uint16_t>(100), static_cast<std::uint16_t>(6)}}}}, .TechnicalSkillSatelliteLimits = pas::Array<std::uint16_t, 0, 6>{{static_cast<std::uint16_t>(2), static_cast<std::uint16_t>(3), static_cast<std::uint16_t>(4), static_cast<std::uint16_t>(5), static_cast<std::uint16_t>(6), static_cast<std::uint16_t>(7), static_cast<std::uint16_t>(8)}}, .TradingSkillSalePercent = pas::Array<std::uint16_t, 0, 6>{{static_cast<std::uint16_t>(0), static_cast<std::uint16_t>(8), static_cast<std::uint16_t>(16), static_cast<std::uint16_t>(25), static_cast<std::uint16_t>(33), static_cast<std::uint16_t>(41), static_cast<std::uint16_t>(50)}}, .LeadershipExperiencePercent = pas::Array<std::uint16_t, 0, 6>{{static_cast<std::uint16_t>(0), static_cast<std::uint16_t>(5), static_cast<std::uint16_t>(10), static_cast<std::uint16_t>(15), static_cast<std::uint16_t>(20), static_cast<std::uint16_t>(25), static_cast<std::uint16_t>(30)}}, .MaxPlanetNews = 9, .SizeTagNames = pas::Array<pas::WideString, 0, 5>{{u"Zero"_w, u"Mini"_w, u"Small"_w, u"Average"_w, u"Big"_w, u"Huge"_w}}, .WealthDemandScales = pas::Array<float, 0, 5>{{0.0f, 0.01f, 0.0125f, 0.016666668f, 0.02f, 0.025f}}, .MinimumHullSlotCounts = pas::Array<std::int32_t, 0, 10>{{
        1, 1, 0, 0, 0, 0, 0, 1,
        0, 0, 0,
    }}, .DefaultHullSlotCounts = pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 1, 0,
    }}, .RangerHullSlots = pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>{{pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        2, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 3,
        2, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 3,
        3, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 3,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 3,
        3, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}}}, .WarriorHullSlots = pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>{{pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 0, 0, 1, 5,
        1, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 0, 1, 1, 4,
        0, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 0, 1, 4,
        1, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 0, 1, 4,
        0, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 3,
        1, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}}}, .PirateHullSlots = pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>{{pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 0, 1, 1, 4,
        2, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 0, 5,
        3, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        1, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 0, 3,
        2, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 3,
        3, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}}}, .TransportHullSlots = pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>{{pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 0, 0, 1, 0, 3,
        0, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 0, 1, 1, 1, 2,
        1, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 0, 1, 1, 0, 2,
        0, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 0, 1, 1, 1, 2,
        0, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 0, 1, 1, 1, 2,
        0, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}}}, .LinerHullSlots = pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>{{pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 0, 1, 0, 1, 4,
        0, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 0, 1, 0, 1, 4,
        0, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 0, 1, 0, 0, 4,
        0, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 0, 0, 3,
        2, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 0, 0, 0, 3,
        2, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}}}, .DiplomatHullSlots = pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>{{pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        1, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 0, 3,
        1, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 0, 1, 2,
        1, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 0, 1, 3,
        1, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 0, 1, 2,
        3, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 4,
        4, 1, 0,
    }}}}, .TranclucatorHullSlots = pas::Array<std::int32_t, 0, 10>{{
        1, 1, 0, 0, 1, 1, 1, 5,
        4, 0, 0,
    }}, .StationHullSlots = pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>{{pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}}}, .DominatorHullSlots = pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>{{pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 1, 0,
    }}}}, .HullType9Slots = pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 1, 0,
    }}, .HullType10Slots = pas::Array<std::int32_t, 0, 10>{{
        1, 1, 1, 1, 1, 1, 1, 5,
        4, 0, 0,
    }}, .HullSlotBonusKinds = pas::Array<aConst::TEquipmentBonusKind, 0, 10>{{
        aConst::bonNull, aConst::bonNull, aConst::bonSlotRadar, aConst::bonSlotScaner, aConst::bonSlotDroid,
        aConst::bonSlotHook, aConst::bonSlotDef, aConst::bonSlotWeapon, aConst::bonSlotArt, aConst::bonSlotForsage,
        aConst::bonNull,
    }}, .OwnerWeaponAvailability = aConst::TOwnerWeaponAvailabilityTable{{aGalaxyStruct::waMalocOnly, aGalaxyStruct::waPelengOnly, aGalaxyStruct::waPeopleOnly, aGalaxyStruct::waFeiOnly, aGalaxyStruct::waGaalOnly, aGalaxyStruct::waNotSoldAndNodeRepair, aGalaxyStruct::waNotSold, aGalaxyStruct::waPirateOnly}}, .WeaponDamageFlagNames = pas::Array<pas::WideString, 0, 20>{{
        u"Energy"_w, u"Splinter"_w, u"Missile"_w, u"Decelerate"_w, u"Destruct"_w, u"Drain"_w, u"Shock"_w, u"Acid"_w,
        u"Magnetic"_w, u"DecelerateA"_w, u"DecelerateAEx"_w, u"Undefendable"_w, u"NonLethal"_w, u"ScanBonus"_w,
        u"BonusToDamaged"_w, u"MoreDrop"_w, u"DropCargo"_w, u"ReduceEngine"_w, u"BlockWeapon"_w, u"BlockDroid"_w,
        u"NoDelta"_w,
    }}, .WeaponDamageClasses = pas::Array<aConst::TPrimaryDamageTypeInfo, 0, 2>{{{.Kind = aConst::wdcEnergy, .BonusKind = aConst::bonWEnergy, .Name = u"Energy"_w}, {.Kind = aConst::wdcSplinter, .BonusKind = aConst::bonWSplinter, .Name = u"Splinter"_w}, {.Kind = aConst::wdcMissile, .BonusKind = aConst::bonWMissile, .Name = u"Missile"_w}}}, .CombatStatusHullFactors = pas::Array<float, 0, 6>{{1.0f, 1.0f, 1.0f, 0.3f, 0.3f, 0.0f, 0.0f}}, .CombatStatusAccumulationFactors = pas::Array<float, 0, 6>{{0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.025f, 0.0f}}, .EquipmentBonusNames = pas::Array<pas::WideString, 0, 42>{{
        u"bonHull"_w, u"bonFuel"_w, u"bonSpeed"_w, u"bonJump"_w, u"bonRadar"_w, u"bonScan"_w, u"bonDroid"_w,
        u"bonHook"_w, u"bonDef"_w, u"bonWEnergy"_w, u"bonWSplinter"_w, u"bonWMissile"_w, u"bonWRadius"_w,
        u"bonSlotRadar"_w, u"bonSlotScaner"_w, u"bonSlotDroid"_w, u"bonSlotHook"_w, u"bonSlotDef"_w, u"bonSlotWeapon"_w,
        u"bonSlotArt"_w, u"bonSlotForsage"_w, u"bonHookRadius"_w, u"bonSkill1"_w, u"bonSkill2"_w, u"bonSkill3"_w,
        u"bonSkill4"_w, u"bonSkill5"_w, u"bonSkill6"_w, u"bonMass"_w, u"bonExtraAkrinEff"_w, u"bonExtraAkrinPenalty"_w,
        u"bonAmmo"_w, u"bonShots"_w, u"bonMissileSpeed"_w, u"bonShotSpeed"_w, u"bonHookMaxSpeed"_w, u"bonHookMinSpeed"_w,
        u"bonStimCapacity"_w, u"bonZonds"_w, u"bonAttacks"_w, u"bonResistAsteroid"_w, u"bonAIValue"_w, u"bonNull"_w,
    }}, .EquipmentBonusSkills = pas::Array<aGalaxyStruct::TPilotSkill, 0, 5>{{aGalaxyStruct::psAccuracy, aGalaxyStruct::psManeuverability, aGalaxyStruct::psTechnical, aGalaxyStruct::psTrading, aGalaxyStruct::psCharisma, aGalaxyStruct::psLeadership}}, .EquipmentSizeFactors = aConst::TEquipmentSizeFactorTable{{2.0f, 1.5f, 1.0f, 0.7f, 0.5f}}, .WeaponRangeLevelFactors = aConst::TWeaponRangeLevelFactors{{0.9f, 0.95f, 0.95f, 1.0f, 1.0f, 1.05f, 1.05f, 1.1f}}, .PlanetInventionInfo = pas::Array<aConst::tInventionInfo, 0, 19>{{
        {.Name = u"Hull level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"FuelTanks level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"Engine level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"Radar level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"Scaner level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"RepairRobot level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"CargoHook level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"Tech level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"Weapon1 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(1)},
        {.Name = u"Weapon2 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(2)},
        {.Name = u"Weapon3 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(3)},
        {.Name = u"Weapon4 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(4)},
        {.Name = u"Weapon5 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(4)},
        {.Name = u"Weapon6 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(5)},
        {.Name = u"Weapon7 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(5)},
        {.Name = u"Weapon8 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(6)},
        {.Name = u"Weapon9 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(6)},
        {.Name = u"Weapon10 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(7)},
        {.Name = u"Weapon11 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(7)},
        {.Name = u"Weapon12 level"_w, .InitialLevel = static_cast<std::uint8_t>(1), .RequiredMainTechLevel = static_cast<std::uint8_t>(8)},
    }}, .EquipmentInventionIndices = aConst::TEquipmentInventionIndexTable{{static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(2), static_cast<std::uint8_t>(3), static_cast<std::uint8_t>(4), static_cast<std::uint8_t>(5), static_cast<std::uint8_t>(6), static_cast<std::uint8_t>(7)}}, .CoalitionProjectNames = pas::Array<pas::WideString, 0, 11>{{
        u"CreateRC"_w, u"CreatePB"_w, u"CreateWB"_w, u"CreateSB"_w, u"CreateBK"_w, u"CreateMC"_w, u"RangersSubsidy"_w,
        u"PiratesSubsidy"_w, u"TransportSubsidy"_w, u"LostSubsidy"_w, u"WarSubsidy"_w, u"WarOperation"_w,
    }}, .StationServiceRepeatPeriods = pas::Array<std::int32_t, 0, 11>{{
        100, 400, 300, 200, 350, 250, 150, 220,
        40, 50, 70, 80,
    }}, .ProgramNames = pas::Array<pas::WideString, 0, 11>{{
        u"KellerCall"_w, u"LogicalNegation"_w, u"Dematerial"_w, u"Energotron"_w, u"SabCrack"_w, u"Intercom"_w,
        u"Shipwreck"_w, u"WeaponBlocking"_w, u"Insanity"_w, u"Shock"_w, u"SelfDestruction"_w, u"Disconnection"_w,
    }}, .ProgramDuration = aConst::TProgramDurationTable{{
        0, 0, 0, 0, 0, 0, 0, 10,
        23, 7, 0, 0,
    }}, .PirateProgramBatchSizes = pas::Array<std::int32_t, 0, 11>{{
        0, 0, 0, 0, 0, 5, 3, 3,
        3, 3, 1, 1,
    }}, .PirateProgramBaseCosts = pas::Array<std::int32_t, 0, 11>{{
        0, 0, 0, 0, 0, 500, 1000, 800,
        300, 200, 1200, 900,
    }}, .GoodsMarketBaseCaptured = false, .IntegrityDataEnd = 0u};

    // Four-byte zero boundary marker for the dormant native data checksum; original name unresolved.
    std::uint32_t& IntegrityDataBegin = cpp_global_region_0.IntegrityDataBegin;

    // 167 in this binary.
    std::int32_t& CurrentSaveVersion = cpp_global_region_0.CurrentSaveVersion;

    // 44 in this binary; enforced by the save-manager load action.
    std::int32_t& MinimumLoadableSaveVersion = cpp_global_region_0.MinimumLoadableSaveVersion;

    pas::WideString& LocalizedTextLinePrefix = cpp_global_region_0.LocalizedTextLinePrefix;

    // Constellations.GalaxyCountStars config; SF_GalaxyPtr('StarCnt') exposes its address.
    std::int32_t& GalaxyStarCount = cpp_global_region_0.GalaxyStarCount;

    // Constellations.GalaxySizeY config; also scales hyperspace route length.
    std::int32_t& GalaxySizeY = cpp_global_region_0.GalaxySizeY;

    // Constellations.GalaxySizeX configuration.
    std::int32_t& GalaxySizeX = cpp_global_region_0.GalaxySizeX;

    const std::uint8_t& MaximumNewGameDifficulty = cpp_global_region_0.MaximumNewGameDifficulty;

    aGalaxyStruct::TGalaxyDifficultyTuningTable& GalaxyDifficultyTuning = cpp_global_region_0.GalaxyDifficultyTuning;

    pas::Array<aConst::TRelationTypeInfo, 0, 4>& RelationInfo = cpp_global_region_0.RelationInfo;

    pas::Array<aConst::TEconomyInfo, 0, 2>& PlanetEconomyInfo = cpp_global_region_0.PlanetEconomyInfo;

    pas::Array<aConst::TShipTypeInfo, 0, 13>& ShipTypeNames = cpp_global_region_0.ShipTypeNames;

    // Standing used to gate station spawning by faction, including the custom station.
    pas::Array<std::uint8_t, 6, 13>& StationDefaultStandings = cpp_global_region_0.StationDefaultStandings;

    // Standing masks used by TPlayer.CanSelectShipTarget.
    aGalaxyStruct::TFactionStandingMasks& NonTargetableStationStandingMasks = cpp_global_region_0.NonTargetableStationStandingMasks;

    aGalaxyStruct::TFactionStandingMasks& FactionStandingMasks = cpp_global_region_0.FactionStandingMasks;

    pas::Array<aConst::TStatusInfo, 0, 2>& CareerTuning = cpp_global_region_0.CareerTuning;

    pas::Array<pas::WideString, 0, 2>& TransportTypeNames = cpp_global_region_0.TransportTypeNames;

    const pas::Array<aGalaxyStruct::TKlingType, 0, 7>& DominatorDisplayOrder = cpp_global_region_0.DominatorDisplayOrder;

    pas::Array<pas::WideString, 0, 7>& DominatorShipTypeNames = cpp_global_region_0.DominatorShipTypeNames;

    // TKlingType order; native name initializer pairs.
    pas::Array<aConst::TKlingTypeInfo, 0, 7>& DominatorShipDefinitions = cpp_global_region_0.DominatorShipDefinitions;

    // Minimum reinforced-system strength by constellation distance tier.
    pas::Array<double, 0, 3>& DominatorRetreatStrengthByTier = cpp_global_region_0.DominatorRetreatStrengthByTier;

    pas::Array<pas::WideString, 0, 2>& DominatorSeriesNames = cpp_global_region_0.DominatorSeriesNames;

    // Native Blazer, Keller, Terron research multipliers.
    pas::Array<double, 0, 2>& DominatorResearchRateMultipliers = cpp_global_region_0.DominatorResearchRateMultipliers;

    pas::Array<double, 0, 2>& ResearchProgramCostFactors = cpp_global_region_0.ResearchProgramCostFactors;

    pas::Array<pas::WideString, 0, 61>& ScriptActionTypeNames = cpp_global_region_0.ScriptActionTypeNames;

    // Excluded from ransom offers and ordinary ally requests.
    aGalaxyStruct::TShipTypeMask& NonNegotiatingShipTypes = cpp_global_region_0.NonNegotiatingShipTypes;

    const pas::Array<aConst::SEquipment, 0, 7>& EquipmentSlotLayouts = cpp_global_region_0.EquipmentSlotLayouts;

    pas::Array<pas::WideString, 0, 75>& ItemTypeNames = cpp_global_region_0.ItemTypeNames;

    pas::Array<pas::DynArray<aConst::TItemType>, 0, 3> ArtefactLootPools{};

    pas::Array<pas::DynArray<pas::WideString>, 0, 3> CustomArtefactLootPools{};

    pas::Array<pas::DynArray<pas::WideString>, 0, 3> UselessItemLootPools{};

    pas::Array<aConst::TGoodsInfo, 0, 7>& GoodsMarket = cpp_global_region_0.GoodsMarket;

    aGalaxyStruct::TGoodsTextOrder& GoodsTextOrder = cpp_global_region_0.GoodsTextOrder;

    pas::Array<pas::WideString, 0, 4>& MissionTypeNames = cpp_global_region_0.MissionTypeNames;

    pas::Array<aConst::TOwnerInfo, 0, 7>& OwnerInfo = cpp_global_region_0.OwnerInfo;

    aGalaxyStruct::TPlanetOwnerMasks& PlanetOwnerMasks = cpp_global_region_0.PlanetOwnerMasks;

    aGalaxyStruct::TOwnerRelationTable& OwnerRelations = cpp_global_region_0.OwnerRelations;

    aGalaxyStruct::TPlanetRaceMarketTable& PlanetRaceMarket = cpp_global_region_0.PlanetRaceMarket;

    aGalaxyStruct::TPlanetEquipmentOfferQuotaTable& PlanetEquipmentOfferQuotas = cpp_global_region_0.PlanetEquipmentOfferQuotas;

    // Native defaults; aRuins accesses this table through an external-unit reference. Original defining unit is inferred.
    aConst::TStationEquipmentOfferQuotaTable& StationEquipmentOfferQuotas = cpp_global_region_0.StationEquipmentOfferQuotas;

    pas::Array<pas::Array<aGalaxyStruct::TPlanetGoodsFactors, 0, 7>, 6, 13>& StationGoodsFactors = cpp_global_region_0.StationGoodsFactors;

    pas::Array<aConst::TGovermentInfo, 0, 4>& PlanetGovernmentMarket = cpp_global_region_0.PlanetGovernmentMarket;

    // Indexed by goods, native race and government; False marks prohibited goods.
    aConst::TGoodsLegalityTable& GoodsLegalOnPlanet = cpp_global_region_0.GoodsLegalOnPlanet;

    pas::Array<pas::WideString, 0, 5>& MedalNames = cpp_global_region_0.MedalNames;

    // Native initialization table.
    pas::Array<pas::WideString, 0, 7>& CoalitionRankNames = cpp_global_region_0.CoalitionRankNames;

    // Zero threshold at the maximum rank.
    pas::Array<std::uint16_t, 0, 7>& CoalitionRankPointThresholds = cpp_global_region_0.CoalitionRankPointThresholds;

    pas::Array<pas::WideString, 0, 7>& PirateRankNames = cpp_global_region_0.PirateRankNames;

    // Zero threshold at the maximum rank.
    pas::Array<std::uint16_t, 0, 7>& PirateRankPointThresholds = cpp_global_region_0.PirateRankPointThresholds;

    pas::Array<pas::WideString, 0, 5>& SkillConfigNames = cpp_global_region_0.SkillConfigNames;

    // Native race, then TPilotSkill; used by ranger bonus evaluation and character setup.
    pas::Array<pas::Array<float, 0, 5>, 0, 4>& RaceSkillEvaluationFactors = cpp_global_region_0.RaceSkillEvaluationFactors;

    // Level, then TPilotSkill; Trading supplies the item resale percentage.
    pas::Array<pas::Array<std::uint16_t, 0, 5>, 0, 6>& PilotSkillEffects = cpp_global_region_0.PilotSkillEffects;

    pas::Array<std::uint16_t, 0, 6>& TechnicalSkillSatelliteLimits = cpp_global_region_0.TechnicalSkillSatelliteLimits;

    // Fraction of the purchase/sale spread recovered by the Trading skill.
    pas::Array<std::uint16_t, 0, 6>& TradingSkillSalePercent = cpp_global_region_0.TradingSkillSalePercent;

    pas::Array<std::uint16_t, 0, 6>& LeadershipExperiencePercent = cpp_global_region_0.LeadershipExperiencePercent;

    // Loaded from GalaxyNews.MaxCntPlanetNews; caps visible-system economic events.
    std::int32_t& MaxPlanetNews = cpp_global_region_0.MaxPlanetNews;

    pas::Array<pas::WideString, 0, 5>& SizeTagNames = cpp_global_region_0.SizeTagNames;

    // Fractions of cached ship wealth used for negotiated amounts.
    pas::Array<float, 0, 5>& WealthDemandScales = cpp_global_region_0.WealthDemandScales;

    // Includes the unsupported-slot sentinel.
    pas::Array<std::int32_t, 0, 10>& MinimumHullSlotCounts = cpp_global_region_0.MinimumHullSlotCounts;

    // Artefact limit can be overridden by gameplay configuration.
    pas::Array<std::int32_t, 0, 10>& DefaultHullSlotCounts = cpp_global_region_0.DefaultHullSlotCounts;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& RangerHullSlots = cpp_global_region_0.RangerHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& WarriorHullSlots = cpp_global_region_0.WarriorHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& PirateHullSlots = cpp_global_region_0.PirateHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& TransportHullSlots = cpp_global_region_0.TransportHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& LinerHullSlots = cpp_global_region_0.LinerHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& DiplomatHullSlots = cpp_global_region_0.DiplomatHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<std::int32_t, 0, 10>& TranclucatorHullSlots = cpp_global_region_0.TranclucatorHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& StationHullSlots = cpp_global_region_0.StationHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<pas::Array<std::int32_t, 0, 10>, 0, 7>& DominatorHullSlots = cpp_global_region_0.DominatorHullSlots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<std::int32_t, 0, 10>& HullType9Slots = cpp_global_region_0.HullType9Slots;

    // Native base slot counts; final column is unsupported kind.
    pas::Array<std::int32_t, 0, 10>& HullType10Slots = cpp_global_region_0.HullType10Slots;

    pas::Array<aConst::TEquipmentBonusKind, 0, 10>& HullSlotBonusKinds = cpp_global_region_0.HullSlotBonusKinds;

    aConst::TOwnerWeaponAvailabilityTable& OwnerWeaponAvailability = cpp_global_region_0.OwnerWeaponAvailability;

    // Native managed-string descriptors.
    pas::Array<pas::WideString, 0, 20>& WeaponDamageFlagNames = cpp_global_region_0.WeaponDamageFlagNames;

    pas::Array<aConst::TPrimaryDamageTypeInfo, 0, 2>& WeaponDamageClasses = cpp_global_region_0.WeaponDamageClasses;

    // Indexed by TCombatStatusEffectType.
    pas::Array<float, 0, 6>& CombatStatusHullFactors = cpp_global_region_0.CombatStatusHullFactors;

    pas::Array<float, 0, 6>& CombatStatusAccumulationFactors = cpp_global_region_0.CombatStatusAccumulationFactors;

    pas::Array<pas::WideString, 0, 42>& EquipmentBonusNames = cpp_global_region_0.EquipmentBonusNames;

    // Maps bonSkill1..bonSkill6 to native pilot skills.
    pas::Array<aGalaxyStruct::TPilotSkill, 0, 5>& EquipmentBonusSkills = cpp_global_region_0.EquipmentBonusSkills;

    aConst::TEquipmentSizeFactorTable& EquipmentSizeFactors = cpp_global_region_0.EquipmentSizeFactors;

    // Native technology multiplier, immediately after EquipmentSizeFactors.
    aConst::TWeaponRangeLevelFactors& WeaponRangeLevelFactors = cpp_global_region_0.WeaponRangeLevelFactors;

    pas::Array<aConst::tInventionInfo, 0, 19>& PlanetInventionInfo = cpp_global_region_0.PlanetInventionInfo;

    float GoodsInflationMin{};

    float GoodsInflationMax{};

    float GoodsStockMin{};

    float GoodsStockMax{};

    std::int32_t GoodsInflationStartTurn{};

    std::int32_t GoodsInflationEndTurn{};

    aGalaxyStruct::TQuestTuningTable QuestTuning{};

    // Levels 1..6 loaded from configuration; level zero is cleared.
    pas::Array<pas::Array<std::uint16_t, 0, 5>, 0, 6> SkillTrainingCosts{};

    // Sum of all six levels of all six skills.
    std::int32_t TotalSkillTrainingCost{};

    aGalaxyStruct::TQuestExperienceTable QuestExperience{};

    // Config kArtefactHull.
    std::int32_t HullArtefactArmor{};

    // Set to 1.5 by native configuration loading.
    float HullArtefactStatusDecayFactor{};

    std::int32_t FuelArtefactBase{};

    // Config kArtefactSpeed.
    float SpeedArtefactFactor{};

    std::int32_t EngineArtefactBase{};

    // Config kArtefactRadar.
    std::int32_t RadarArtefactRange{};

    // Config kArtefactScaner.
    std::int32_t ScannerArtefactPower{};

    // Config kArtefactDroid.
    std::int32_t DroidArtefactRepair{};

    // Config kArtefactDroidWear.
    float DroidArtefactWear{};

    // Set to 1.5 by native configuration loading.
    float DroidArtefactStatusDecayFactor{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t NanoArtefactRepair{};

    // Config kArtefactDef.
    float DefenseArtefactBonus{};

    // Config kArtefactAntigrav.
    float AntigravityArtefactMassFactor{};

    // Config kArtefactHook.
    std::int32_t CargoHookArtefactPower{};

    // Config kArtefactHookRaduis.
    std::int32_t CargoHookArtefactRange{};

    // Config kArtefactHookSpeed.
    std::int32_t CargoHookArtefactSpeed{};

    // Config kArtWeaponToSpeed.
    std::int32_t WeaponToSpeedArtefactBonus{};

    // Config kArtDefToEnergyUp.
    float DefenseToEnergyUpperFactor{};

    // Config kArtDefToEnergyMin.
    float DefenseToEnergyMinimumFactor{};

    // Config kArtDefToEnergyPenalty.
    float DefenseToEnergyPenalty{};

    // Config kArtDefToArms1Penalty.
    float DefenseToWeaponPenalty{};

    // Config kArtEnergyPulse.
    float EnergyPulseArtefactFactor{};

    // Config kArtEnergyPulseChance.
    float EnergyPulseArtefactChance{};

    // Config kArtSplinter.
    float SplinterArtefactFactor{};

    // Config kArtGiperJump.
    std::int32_t HyperJumpArtefactRange{};

    // Configured star-heat reduction per active artefact.
    float StarHeatArtefactReduction{};

    float ExtraMissileChance{};

    // Config kArtForsage.
    float AfterburnerArtefactWearFactor{};

    // Config kArtefactHullEx.
    std::int32_t HullArtefactBoostArmor{};

    // Set to 0.5 by native configuration loading.
    float HullArtefactBoostStatusDecay{};

    std::int32_t FuelArtefactBoost{};

    // Config kArtefactSpeedEx.
    float SpeedArtefactBoostFactor{};

    std::int32_t EngineArtefactBoost{};

    // Config kArtefactRadarEx.
    std::int32_t RadarArtefactBoostRange{};

    // Config kArtefactScanerEx.
    std::int32_t ScannerArtefactBoostPower{};

    // Config kArtefactDroidEx.
    std::int32_t DroidArtefactBoostRepair{};

    // Config kArtefactDroidWearEx.
    float DroidArtefactBoostWear{};

    // Set to 0.5 by native configuration loading.
    float DroidArtefactBoostStatusDecay{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t NanoArtefactBoostRepair{};

    // Config kArtefactDefEx.
    float DefenseArtefactBoost{};

    // Config kArtefactAntigravEx.
    float AntigravityArtefactBoostFactor{};

    // Config kArtefactHookEx.
    std::int32_t CargoHookArtefactBoostPower{};

    // Config kArtefactHookRaduisEx.
    std::int32_t CargoHookArtefactBoostRange{};

    // Config kArtefactHookSpeedEx.
    std::int32_t CargoHookArtefactBoostSpeed{};

    // Config kArtWeaponToSpeedEx.
    std::int32_t WeaponToSpeedArtefactBoost{};

    // Config kArtDefToEnergyUpEx.
    float DefenseToEnergyUpperBoost{};

    // Config kArtDefToEnergyMinEx.
    float DefenseToEnergyMinimumBoost{};

    // Config kArtDefToEnergyPenaltyEx.
    float DefenseToEnergyBoostPenalty{};

    // Config kArtEnergyPulseEx.
    float EnergyPulseArtefactBoostFactor{};

    // Config kArtSplinterEx.
    float SplinterArtefactBoostFactor{};

    // Config kArtGiperJumpEx.
    std::int32_t HyperJumpArtefactBoostRange{};

    // Boosted additional reduction.
    float StarHeatArtefactBoostReduction{};

    // Loaded from Artefacts.NumericValues.
    float ExtraMissileBoostChance{};

    // Config kArtForsageEx.
    float AfterburnerArtefactBoostWearFactor{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t MinTransmitterPower{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t AverageTransmitterPower{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t MaxTransmitterPower{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t TransmitterSameSystemPenalty{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t TransmitterAnySystemPenalty{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t TransmitterSameSystemPenaltyTurns{};

    // Loaded from Artefacts.NumericValues.
    std::int32_t TransmitterAnySystemPenaltyTurns{};

    std::int32_t SubportalRewardPenalty{};

    std::int32_t SubportalRewardPenaltyTurns{};

    // Added when Item.DestroyFlag is two.
    std::int32_t ItemExplosionBonusDamage{};

    // Damage at the blast radius.
    std::int32_t BombMinimumDamage{};

    // Damage at the center.
    std::int32_t BombMaximumDamage{};

    std::int32_t BombDamageRadius{};

    // Square of configured BombRadius.
    std::int32_t ItemExplosionRadiusSquared{};

    std::int32_t PointDefensePassCount{};

    std::int32_t PointDefenseBaseRange{};

    std::int32_t PointDefenseBonusRange{};

    // Asteroid.kAsteroidMinDamagePercent divided by 100; fraction of hull capacity.
    float AsteroidMinDamageFactor{};

    // Asteroid.kAsteroidMaxDamagePercent divided by 100.
    float AsteroidMaxDamageFactor{};

    // Asteroid.kAsteroidMinDamagePercentDef divided by 100.
    float AsteroidMinDamageFactorWithDefGenerator{};

    // Asteroid.kAsteroidMaxDamagePercentDef divided by 100.
    float AsteroidMaxDamageFactorWithDefGenerator{};

    // Hull capacities are multiplied by this configuration value.
    float HullCapacityScale{};

    std::int32_t HullBaseSize{};

    std::int32_t FuelTanksBaseSize{};

    std::int32_t EngineBaseSize{};

    std::int32_t RadarBaseSize{};

    std::int32_t ScannerBaseSize{};

    std::int32_t RepairRobotBaseSize{};

    std::int32_t CargoHookBaseSize{};

    std::int32_t DefGeneratorBaseSize{};

    // Config native equipment configuration.
    float AfterburnerSpeedFactor{};

    // Loaded from equipment configuration.
    pas::Array<std::uint8_t, 1, 8> FuelCapacityByLevel{};

    // Loaded by the native equipment configuration initializer.
    aGalaxyStruct::TEngineLevelStatsTable EngineLevelStats{};

    // Per-technology armor and energy/splinter/missile fragility, used by THull getters.
    aConst::THullLevelStatsTable HullLevelStats{};

    // Loaded from mRepair.
    pas::Array<std::uint8_t, 1, 8> RepairRobotLevelPoints{};

    pas::Array<float, 1, 8> DefGeneratorLevelFactors{};

    // Loaded from equipment configuration.
    pas::Array<std::uint16_t, 1, 8> RadarLevelRanges{};

    aGalaxyStruct::TCargoHookLevelStatsTable CargoHookLevelStats{};

    // Damage class, then owner; loaded from mFragilityByOwner*.
    pas::Array<pas::Array<float, 0, 7>, 0, 2> HullFragilityByOwner{};

    // Loaded from mFragilityByShipType.
    pas::Array<float, 0, 10> HullFragilityByType{};

    pas::Array<aConst::TWeaponInfo, 50, 67> WeaponInfos{};

    aConst::TEquipmentInventionIndexTable& EquipmentInventionIndices = cpp_global_region_0.EquipmentInventionIndices;

    pas::Array<pas::WideString, 0, 11>& CoalitionProjectNames = cpp_global_region_0.CoalitionProjectNames;

    pas::Array<std::int32_t, 0, 11>& StationServiceRepeatPeriods = cpp_global_region_0.StationServiceRepeatPeriods;

    pas::Array<pas::WideString, 0, 11>& ProgramNames = cpp_global_region_0.ProgramNames;

    aConst::TProgramDurationTable& ProgramDuration = cpp_global_region_0.ProgramDuration;

    pas::Array<std::int32_t, 0, 11>& PirateProgramBatchSizes = cpp_global_region_0.PirateProgramBatchSizes;

    pas::Array<std::int32_t, 0, 11>& PirateProgramBaseCosts = cpp_global_region_0.PirateProgramBaseCosts;

    // Set after the one-time localized base-table copy.
    std::uint8_t& GoodsMarketBaseCaptured = cpp_global_region_0.GoodsMarketBaseCaptured;

    // Four-byte zero marker at the exclusive boundary of that native checksum span.
    std::uint32_t& IntegrityDataEnd = cpp_global_region_0.IntegrityDataEnd;

    // Updated by TPlayer.NextDay and read by TfGameEnd.OnOpen; score-related role not fully recovered.
    std::int32_t LastMedicalPolicyTicks = 0;

    // Initialized from HullBaseSize and EquipmentSizeFactors[5].
    std::int32_t HullMassEvaluationStart{};

    // Initialized from HullBaseSize and EquipmentSizeFactors[1].
    std::int32_t HullMassEvaluationEnd{};

    std::int32_t WearMassMin{};

    std::int32_t WearMassMax{};

    pas::Array<aConst::TGoodsInfo, 0, 7> GoodsMarketBase{};

    pas::DynArray<aConst::TMicroModuleInfo> MicroModuleTemplates{};

    std::int32_t MicroModuleTemplateCount{};

    // Loaded from HullType configuration, sorted by numeric suffix.
    pas::DynArray<aConst::THullTypeInfo> HullSeriesDefinitions{};

    // Native count used by CheatIdeal and hull-series configuration.
    std::int32_t HullSeriesCount{};

    // Native disease/stimulant definitions; eligibility and progression fields verified in TPlayer.NextDay.
    pas::Array<aConst::TIllnessInfo, 1, 24> CaptainHealthDefinitions{};

    // Finalized as one TIllnessInfo alongside the 24 captain effects.
    aConst::TRadiationHealthDefinitions RadiationHealthDefinitions{};

    // Shared selection scratch, sized to MicroModuleTemplateCount when templates load. Callers track the used prefix separately.
    pas::DynArray<std::int32_t> MicroModuleCandidateIndices{};

    void IncrementWordSaturating(pas::Var<std::uint16_t> Value) {
        if (pas::load_unaligned<std::uint16_t>(Value.address) < 65535) {
            pas::inc_unaligned<std::uint16_t>(Value.address, 1);
        }
    }

    pas::WideString OwnerToSys(aGalaxyStruct::TOwnerId OwnerId) {
        switch (OwnerId) {
            case aGalaxyStruct::oiMaloc: return u"Maloc"_w;
            case aGalaxyStruct::oiPeleng: return u"Peleng"_w;
            case aGalaxyStruct::oiHuman: return u"People"_w;
            case aGalaxyStruct::oiFeyan: return u"Fei"_w;
            case aGalaxyStruct::oiGaal: return u"Gaal"_w;
            case aGalaxyStruct::oiDominator: return u"Kling"_w;
            case aGalaxyStruct::oiPirate: return u"PirateClan"_w;
            default: return u"None"_w;
        }
    }

    std::uint8_t IsKnownOwnerName(const std::u16string_view& Name) {
        std::uint8_t Result = false;
        if (!Result) {
            Result = Name == u"Maloc"sv;
        }
        if (!Result) {
            Result = Name == u"Peleng"sv;
        }
        if (!Result) {
            Result = Name == u"People"sv;
        }
        if (!Result) {
            Result = Name == u"Fei"sv;
        }
        if (!Result) {
            Result = Name == u"Gaal"sv;
        }
        if (!Result) {
            Result = Name == u"Kling"sv;
        }
        if (!Result) {
            Result = Name == u"None"sv;
        }
        if (!Result) {
            return Name == u"PirateClan"sv;
        }
        return Result;
    }

    // Unrecognized names act as a wildcard.
    std::uint8_t MatchesOwnerName(aGalaxyStruct::TOwnerId OwnerId, const std::u16string_view& Name) {
        return static_cast<std::uint8_t>(aConst::IsKnownOwnerName(Name) ^ 1) || Name == pas::view(aConst::OwnerToSys(OwnerId));
    }

    // Case-sensitive substring, Any, or empty string.
    std::uint8_t MatchesCareerName(aGalaxyStruct::TRangerCareer Career, const std::u16string_view& Names) {
        return pas::pos(CareerTuning[Career].Name, Names) > 0 || Names == u"Any"sv || Names == u""sv;
    }

    void LoadArtefactConfiguration() {
        static const pas::Set<0, 255> ArtefactTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 9}}) - pas::constant_set<pas::Set<0, 255>>({{42, 79}});
        std::int32_t Index{};
        EC_BlockPar::TBlockParEC* Config{};
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString ItemName{};
        TItemType Kind{};
        std::uint8_t CanBeABDrop{};
        std::uint8_t CanBeTreasure{};
        std::uint8_t CanBeReward{};
        Config = GR_Main::LanguageDataConfig->GetBlockByPath(u"Artefacts.NumericValues"_wref.get());
        HullArtefactArmor = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactHull"sv)));
        HullArtefactStatusDecayFactor = 1.5f;
        FuelArtefactBase = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactFuel"sv)));
        SpeedArtefactFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtefactSpeed"sv));
        EngineArtefactBase = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactPower"sv)));
        RadarArtefactRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactRadar"sv)));
        ScannerArtefactPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactScaner"sv)));
        DroidArtefactRepair = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactDroid"sv)));
        DroidArtefactWear = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtefactDroidWear"sv));
        DroidArtefactStatusDecayFactor = 1.5f;
        NanoArtefactRepair = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactNano"sv)));
        DefenseArtefactBonus = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtefactDef"sv));
        AntigravityArtefactMassFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtefactAntigrav"sv));
        CargoHookArtefactPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactHook"sv)));
        CargoHookArtefactRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactHookRaduis"sv)));
        CargoHookArtefactSpeed = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactHookSpeed"sv)));
        WeaponToSpeedArtefactBonus = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtWeaponToSpeed"sv)));
        DefenseToEnergyUpperFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtDefToEnergyUp"sv));
        DefenseToEnergyMinimumFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtDefToEnergyMin"sv));
        DefenseToEnergyPenalty = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtDefToEnergyPenalty"sv));
        DefenseToWeaponPenalty = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtDefToArms1Penalty"sv));
        EnergyPulseArtefactFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtEnergyPulse"sv));
        EnergyPulseArtefactChance = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtEnergyPulseChance"sv));
        SplinterArtefactFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtSplinter"sv));
        HyperJumpArtefactRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtGiperJump"sv)));
        StarHeatArtefactReduction = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtPowerSunProtection"sv));
        ExtraMissileChance = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtFastRacksChance"sv));
        AfterburnerArtefactWearFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtForsage"sv));
        PointDefensePassCount = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kPDTurretCountShots"sv)));
        PointDefenseBaseRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kPDTurretRange"sv)));
        HullArtefactBoostArmor = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactHullEx"sv)));
        HullArtefactBoostStatusDecay = 0.5f;
        FuelArtefactBoost = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactFuelEx"sv)));
        SpeedArtefactBoostFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtefactSpeedEx"sv));
        EngineArtefactBoost = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactPowerEx"sv)));
        RadarArtefactBoostRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactRadarEx"sv)));
        ScannerArtefactBoostPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactScanerEx"sv)));
        DroidArtefactBoostRepair = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactDroidEx"sv)));
        DroidArtefactBoostWear = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtefactDroidWearEx"sv));
        DroidArtefactBoostStatusDecay = 0.5f;
        NanoArtefactBoostRepair = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactNanoEx"sv)));
        DefenseArtefactBoost = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtefactDefEx"sv));
        AntigravityArtefactBoostFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtefactAntigravEx"sv));
        CargoHookArtefactBoostPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactHookEx"sv)));
        CargoHookArtefactBoostRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactHookRaduisEx"sv)));
        CargoHookArtefactBoostSpeed = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtefactHookSpeedEx"sv)));
        WeaponToSpeedArtefactBoost = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtWeaponToSpeedEx"sv)));
        DefenseToEnergyUpperBoost = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtDefToEnergyUpEx"sv));
        DefenseToEnergyMinimumBoost = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtDefToEnergyMinEx"sv));
        DefenseToEnergyBoostPenalty = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtDefToEnergyPenaltyEx"sv));
        EnergyPulseArtefactBoostFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtEnergyPulseEx"sv));
        SplinterArtefactBoostFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtSplinterEx"sv));
        HyperJumpArtefactBoostRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kArtGiperJumpEx"sv)));
        StarHeatArtefactBoostReduction = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtPowerSunProtectionEx"sv));
        ExtraMissileBoostChance = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtFastRacksChanceEx"sv));
        AfterburnerArtefactBoostWearFactor = EC_Str::ExtractDecimalToSingleW(Config->GetParam(u"kArtForsageEx"sv));
        PointDefenseBonusRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kPDTurretRangeEx"sv)));
        MinTransmitterPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"MinTransmitterPower"sv)));
        AverageTransmitterPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"AverageTransmitterPower"sv)));
        MaxTransmitterPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"MaxTransmitterPower"sv)));
        TransmitterSameSystemPenalty = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kTransmitterPenaltySameSystem"sv)));
        TransmitterAnySystemPenalty = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kTransmitterPenaltyAnySystem"sv)));
        TransmitterSameSystemPenaltyTurns = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kTransmitterPenaltySameSystemDuration"sv)));
        TransmitterAnySystemPenaltyTurns = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kTransmitterPenaltyAnySystemDuration"sv)));
        SubportalRewardPenalty = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kSubportalPenalty"sv)));
        SubportalRewardPenaltyTurns = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"kSubportalPenaltyDuration"sv)));
        ItemExplosionBonusDamage = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"BombPower"sv)));
        BombMinimumDamage = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"BombPowerMin"sv)));
        BombMaximumDamage = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"BombPowerMax"sv)));
        BombDamageRadius = SysUtils::StrToInt(static_cast<pas::AnsiString>(Config->GetParam(u"BombRadius"sv)));
        ItemExplosionRadiusSquared = BombDamageRadius * BombDamageRadius;
        std::int32_t ABDropCount = 0;
        std::int32_t TreasureCount = 0;
        std::int32_t RewardCount = 0;
        std::int32_t AnyCount = 0;
        Config = GR_Main::LanguageDataConfig->GetBlockByPath(u"Artefacts"_wref.get());
        for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(static_cast<TItemTypeSelection>(ArtefactTypes))); cpp_range.next(Index); ) {
            Kind = static_cast<TItemType>(aConst::GetItemTypeFromMask(static_cast<TItemTypeSelection>(pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 9}}) - pas::constant_set<pas::Set<0, 255>>({{42, 79}})), Index));
            Block = Config->GetBlock(pas::view(ItemTypeNames[Kind]));
            CanBeABDrop = Block->CountParams(u"CanBeABDrop"_wref.get()) <= 0 || EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeABDrop"sv))) > 0;
            CanBeTreasure = Block->CountParams(u"CanBeTreasure"_wref.get()) <= 0 || EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeTreasure"sv))) > 0;
            CanBeReward = Block->CountParams(u"CanBeReward"_wref.get()) <= 0 || EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeReward"sv))) > 0;
            if (CanBeABDrop || CanBeTreasure || CanBeReward) {
                if (CanBeABDrop) {
                    ++ABDropCount;
                }
                if (CanBeTreasure) {
                    ++TreasureCount;
                }
                if (CanBeReward) {
                    ++RewardCount;
                }
                ++AnyCount;
            }
        }
        ArtefactLootPools[0].set_length(ABDropCount);
        ArtefactLootPools[1].set_length(TreasureCount);
        ArtefactLootPools[2].set_length(RewardCount);
        ArtefactLootPools[3].set_length(AnyCount);
        ABDropCount = 0;
        TreasureCount = 0;
        RewardCount = 0;
        AnyCount = 0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(static_cast<TItemTypeSelection>(ArtefactTypes))); cpp_range_2.next(Index); ) {
            Kind = static_cast<TItemType>(aConst::GetItemTypeFromMask(static_cast<TItemTypeSelection>(pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 9}}) - pas::constant_set<pas::Set<0, 255>>({{42, 79}})), Index));
            Block = Config->GetBlock(pas::view(ItemTypeNames[Kind]));
            CanBeABDrop = Block->CountParams(u"CanBeABDrop"_wref.get()) <= 0 || EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeABDrop"sv))) > 0;
            CanBeTreasure = Block->CountParams(u"CanBeTreasure"_wref.get()) <= 0 || EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeTreasure"sv))) > 0;
            CanBeReward = Block->CountParams(u"CanBeReward"_wref.get()) <= 0 || EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeReward"sv))) > 0;
            if (CanBeABDrop || CanBeTreasure || CanBeReward) {
                if (CanBeABDrop) {
                    ArtefactLootPools[0][ABDropCount] = Kind;
                    ++ABDropCount;
                }
                if (CanBeTreasure) {
                    ArtefactLootPools[1][TreasureCount] = Kind;
                    ++TreasureCount;
                }
                if (CanBeReward) {
                    ArtefactLootPools[2][RewardCount] = Kind;
                    ++RewardCount;
                }
                ArtefactLootPools[3][AnyCount] = Kind;
                ++AnyCount;
            }
        }
        ABDropCount = 0;
        TreasureCount = 0;
        RewardCount = 0;
        AnyCount = 0;
        Config = GR_Main::LanguageDataConfig->GetBlockByPath(u"Artefacts.CustomArtefacts"_wref.get());
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Config->GetBlockCount() - 1); cpp_range_3.next(Index); ) {
            Block = Config->GetBlockByIndex(Index);
            CanBeABDrop = Block->CountParams(u"CanBeABDrop"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeABDrop"sv))) > 0;
            CanBeTreasure = Block->CountParams(u"CanBeTreasure"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeTreasure"sv))) > 0;
            CanBeReward = Block->CountParams(u"CanBeReward"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeReward"sv))) > 0;
            if (CanBeABDrop || CanBeTreasure || CanBeReward) {
                if (CanBeABDrop) {
                    ++ABDropCount;
                }
                if (CanBeTreasure) {
                    ++TreasureCount;
                }
                if (CanBeReward) {
                    ++RewardCount;
                }
                ++AnyCount;
            }
        }
        CustomArtefactLootPools[0].set_length(ABDropCount);
        CustomArtefactLootPools[1].set_length(TreasureCount);
        CustomArtefactLootPools[2].set_length(RewardCount);
        CustomArtefactLootPools[3].set_length(AnyCount);
        ABDropCount = 0;
        TreasureCount = 0;
        RewardCount = 0;
        AnyCount = 0;
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Config->GetBlockCount() - 1); cpp_range_4.next(Index); ) {
            Block = Config->GetBlockByIndex(Index);
            CanBeABDrop = Block->CountParams(u"CanBeABDrop"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeABDrop"sv))) > 0;
            CanBeTreasure = Block->CountParams(u"CanBeTreasure"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeTreasure"sv))) > 0;
            CanBeReward = Block->CountParams(u"CanBeReward"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeReward"sv))) > 0;
            if (CanBeABDrop || CanBeTreasure || CanBeReward) {
                ItemName = Config->GetBlockNameByIndex(Index);
                if (CanBeABDrop) {
                    CustomArtefactLootPools[0][ABDropCount] = ItemName;
                    ++ABDropCount;
                }
                if (CanBeTreasure) {
                    CustomArtefactLootPools[1][TreasureCount] = ItemName;
                    ++TreasureCount;
                }
                if (CanBeReward) {
                    CustomArtefactLootPools[2][RewardCount] = ItemName;
                    ++RewardCount;
                }
                CustomArtefactLootPools[3][AnyCount] = ItemName;
                ++AnyCount;
            }
        }
        ABDropCount = 0;
        TreasureCount = 0;
        RewardCount = 0;
        AnyCount = 0;
        Config = GR_Main::LanguageDataConfig->GetBlockByPath(u"UselessItems"_wref.get());
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Config->GetBlockCount() - 1); cpp_range_5.next(Index); ) {
            Block = Config->GetBlockByIndex(Index);
            CanBeABDrop = Block->CountParams(u"CanBeABDrop"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeABDrop"sv))) > 0;
            CanBeTreasure = Block->CountParams(u"CanBeTreasure"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeTreasure"sv))) > 0;
            CanBeReward = Block->CountParams(u"CanBeReward"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeReward"sv))) > 0;
            if (CanBeABDrop || CanBeTreasure || CanBeReward) {
                if (CanBeABDrop) {
                    ++ABDropCount;
                }
                if (CanBeTreasure) {
                    ++TreasureCount;
                }
                if (CanBeReward) {
                    ++RewardCount;
                }
                ++AnyCount;
            }
        }
        UselessItemLootPools[0].set_length(ABDropCount);
        UselessItemLootPools[1].set_length(TreasureCount);
        UselessItemLootPools[2].set_length(RewardCount);
        UselessItemLootPools[3].set_length(AnyCount);
        ABDropCount = 0;
        TreasureCount = 0;
        RewardCount = 0;
        AnyCount = 0;
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Config->GetBlockCount() - 1); cpp_range_6.next(Index); ) {
            Block = Config->GetBlockByIndex(Index);
            CanBeABDrop = Block->CountParams(u"CanBeABDrop"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeABDrop"sv))) > 0;
            CanBeTreasure = Block->CountParams(u"CanBeTreasure"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeTreasure"sv))) > 0;
            CanBeReward = Block->CountParams(u"CanBeReward"_wref.get()) > 0 && EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"CanBeReward"sv))) > 0;
            if (CanBeABDrop || CanBeTreasure || CanBeReward) {
                ItemName = Config->GetBlockNameByIndex(Index);
                if (CanBeABDrop) {
                    UselessItemLootPools[0][ABDropCount] = ItemName;
                    ++ABDropCount;
                }
                if (CanBeTreasure) {
                    UselessItemLootPools[1][TreasureCount] = ItemName;
                    ++TreasureCount;
                }
                if (CanBeReward) {
                    UselessItemLootPools[2][RewardCount] = ItemName;
                    ++RewardCount;
                }
                UselessItemLootPools[3][AnyCount] = ItemName;
                ++AnyCount;
            }
        }
    }

    void LoadDamageSkillQuestMarketConfiguration() {
        std::int32_t Level{};
        std::int32_t Cost{};
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Values{};
        aGalaxyStruct::TQuestType QuestKind{};
        aGalaxyStruct::TPilotSkill Skill{};
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Asteroid"_wref.get());
        AsteroidMinDamageFactor = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"kAsteroidMinDamagePercent"sv))) * 0.01L;
        AsteroidMaxDamageFactor = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"kAsteroidMaxDamagePercent"sv))) * 0.01L;
        AsteroidMinDamageFactorWithDefGenerator = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"kAsteroidMinDamagePercentDef"sv))) * 0.01L;
        AsteroidMaxDamageFactorWithDefGenerator = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"kAsteroidMaxDamagePercentDef"sv))) * 0.01L;
        TotalSkillTrainingCost = 0;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TPilotSkill>(aGalaxyStruct::psAccuracy, aGalaxyStruct::psLeadership); cpp_range.next(Skill); ) {
            SkillTrainingCosts[0][Skill] = 0;
            Values = GR_Main::LanguageDataConfig->GetBlockByPath(pas::concat_wide({u"Skills.", SkillConfigNames[Skill]}))->GetParam(u"Points"sv);
            for (Level = 1; Level <= 6; ++Level) {
                Cost = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
                SkillTrainingCosts[Level][Skill] = Cost;
                TotalSkillTrainingCost += Cost;
            }
        }
        Values = GR_Main::LanguageDataConfig->GetBlockByPath(u"Quest"_wref.get())->GetParam(u"QuestPoints"sv);
        for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TQuestType>(aGalaxyStruct::qtSendLetter, aGalaxyStruct::qtDefendShip); cpp_range_2.next(QuestKind); ) {
            QuestExperience[QuestKind] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), QuestKind - aGalaxyStruct::qtSendLetter, u","sv)));
        }
        Values = GR_Main::LanguageDataConfig->GetBlockByPath(u"Quest"_wref.get())->GetParam(u"QuestTurns"sv);
        for (auto cpp_range_3 = pas::for_to<aGalaxyStruct::TQuestType>(aGalaxyStruct::qtSendLetter, aGalaxyStruct::qtDefendShip); cpp_range_3.next(QuestKind); ) {
            QuestTuning[QuestKind].BaseDuration = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), QuestKind - aGalaxyStruct::qtSendLetter, u","sv)));
        }
        Values = GR_Main::LanguageDataConfig->GetBlockByPath(u"Quest"_wref.get())->GetParam(u"QuestMoneyBase"sv);
        for (auto cpp_range_4 = pas::for_to<aGalaxyStruct::TQuestType>(aGalaxyStruct::qtSendLetter, aGalaxyStruct::qtDefendShip); cpp_range_4.next(QuestKind); ) {
            QuestTuning[QuestKind].BaseRewardMoney = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), QuestKind - aGalaxyStruct::qtSendLetter, u","sv)));
        }
        Values = GR_Main::LanguageDataConfig->GetBlockByPath(u"Quest"_wref.get())->GetParam(u"QuestMoneyPerc"sv);
        for (auto cpp_range_5 = pas::for_to<aGalaxyStruct::TQuestType>(aGalaxyStruct::qtSendLetter, aGalaxyStruct::qtDefendShip); cpp_range_5.next(QuestKind); ) {
            QuestTuning[QuestKind].RewardCapitalPercent = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), QuestKind - aGalaxyStruct::qtSendLetter, u","sv)));
        }
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.Goods"_wref.get());
        GoodsInflationMin = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"kInflationMin"sv));
        GoodsInflationMax = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"kInflationMax"sv));
        GoodsStockMin = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"kStockMin"sv));
        GoodsStockMax = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"kStockMax"sv));
        if (Block->CountParams(u"InflationStartTurn"_wref.get()) > 0) {
            GoodsInflationStartTurn = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"InflationStartTurn"sv)));
        } else {
            GoodsInflationStartTurn = 1000;
        }
        if (Block->CountParams(u"InflationEndTurn"_wref.get()) > 0) {
            GoodsInflationEndTurn = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"InflationEndTurn"sv)));
        } else {
            GoodsInflationEndTurn = 10000;
        }
    }

    void LoadEquipmentConfiguration() {
        std::uint8_t Level{};
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Values{};
        TWeaponDamageClass DamageKind{};
        aGalaxyStruct::TOwnerId Owner{};
        std::uint8_t HullKind{};
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.Hull"_wref.get());
        HullBaseSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
        HullCapacityScale = pas::real_divide(HullBaseSize, 5.0E+2L);
        Values = Block->GetParam(u"mAlloy"sv);
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range.next(Level); ) {
            HullLevelStats[Level].Armor = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
        }
        for (auto cpp_range_2 = pas::for_to<TWeaponDamageClass>(wdcEnergy, wdcMissile); cpp_range_2.next(DamageKind); ) {
            Values = Block->GetParam(pas::view(pas::concat_wide({u"mFragilityByLevel", WeaponDamageClasses[DamageKind].Name})));
            for (auto cpp_range_3 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_3.next(Level); ) {
                pas::store_unaligned<float>(pas::byte_offset(&HullLevelStats[Level].Fragility, DamageKind * sizeof(float)), EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
            }
            Values = Block->GetParam(pas::view(pas::concat_wide({u"mFragilityByOwner", WeaponDamageClasses[DamageKind].Name})));
            for (auto cpp_range_4 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range_4.next(Owner); ) {
                HullFragilityByOwner[DamageKind][Owner] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Values), Owner - 0, u","sv));
            }
        }
        Values = Block->GetParam(u"mFragilityByShipType"sv);
        for (auto cpp_range_5 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(10)); cpp_range_5.next(HullKind); ) {
            HullFragilityByType[HullKind] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Values), HullKind - 0, u","sv));
        }
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.FuelTanks"_wref.get());
        FuelTanksBaseSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
        Values = Block->GetParam(u"mCapacity"sv);
        for (auto cpp_range_6 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_6.next(Level); ) {
            FuelCapacityByLevel[Level] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
        }
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.Engine"_wref.get());
        EngineBaseSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
        Values = Block->GetParam(u"mSpeed"sv);
        for (auto cpp_range_7 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_7.next(Level); ) {
            EngineLevelStats[Level].Speed = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
        }
        Values = Block->GetParam(u"mJump"sv);
        for (auto cpp_range_8 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_8.next(Level); ) {
            EngineLevelStats[Level].JumpRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
        }
        AfterburnerSpeedFactor = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"ForsageCoef"sv));
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.RepairRobot"_wref.get());
        RepairRobotBaseSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
        Values = Block->GetParam(u"mRepair"sv);
        for (auto cpp_range_9 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_9.next(Level); ) {
            RepairRobotLevelPoints[Level] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
        }
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.DefGenerator"_wref.get());
        DefGeneratorBaseSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
        Values = Block->GetParam(u"mDef"sv);
        for (auto cpp_range_10 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_10.next(Level); ) {
            DefGeneratorLevelFactors[Level] = 1.0L - EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv));
        }
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.Radar"_wref.get());
        RadarBaseSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
        Values = Block->GetParam(u"mRadius"sv);
        for (auto cpp_range_11 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_11.next(Level); ) {
            RadarLevelRanges[Level] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
        }
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.Scaner"_wref.get());
        ScannerBaseSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
        Block = GR_Main::LanguageDataConfig->GetBlockByPath(u"Items.CargoHook"_wref.get());
        CargoHookBaseSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
        Values = Block->GetParam(u"mMass"sv);
        for (auto cpp_range_12 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_12.next(Level); ) {
            CargoHookLevelStats[Level].PickupPower = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
        }
        Values = Block->GetParam(u"mRadius"sv);
        for (auto cpp_range_13 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_13.next(Level); ) {
            CargoHookLevelStats[Level].Range = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv)));
        }
        Values = Block->GetParam(u"mSpeedFar"sv);
        for (auto cpp_range_14 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_14.next(Level); ) {
            CargoHookLevelStats[Level].MinPullSpeed = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv));
        }
        Values = Block->GetParam(u"mSpeedClose"sv);
        for (auto cpp_range_15 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(8)); cpp_range_15.next(Level); ) {
            CargoHookLevelStats[Level].MaxPullSpeed = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv));
        }
    }

    void LoadWeaponConfiguration() {
        static const pas::Set<0, 255> WeaponTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 49}, {68, 79}});
        std::int32_t Level{};
        std::int32_t Index{};
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Values{};
        std::uint8_t Kind{};
        std::uint8_t DamageKind{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(static_cast<TItemTypeSelection>(WeaponTypes))); cpp_range.next(Index); ) {
            Kind = aConst::GetItemTypeFromMask(static_cast<TItemTypeSelection>(WeaponTypes), Index);
            Block = GR_Main::LanguageDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"Items.Weapon.Stats.", SysUtils::IntToStr(Kind + 1 - 50)})));
            {
                TWeaponInfo& cpp_with = WeaponInfos[static_cast<TItemType>(Kind)];
                cpp_with.ItemType = static_cast<TItemType>(Kind);
                cpp_with.ConfigName = ItemTypeNames[static_cast<TItemType>(Kind)];
                cpp_with.TechLevel = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"TechLevel"sv)));
                cpp_with.CostFactor = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"kCost"sv));
                cpp_with.MinDamage = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MinDamage"sv)));
                cpp_with.MaxDamage = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MaxDamage"sv)));
                cpp_with.AverageSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageSize"sv)));
                cpp_with.AverageRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AverageRadius"sv)));
                cpp_with.ShotSpeedPercent = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Speed"sv)));
                if (Block->CountParams(u"SecondaryDamageRadius"_wref.get()) > 0) {
                    cpp_with.SecondaryDamageRadius = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"SecondaryDamageRadius"sv)));
                } else {
                    cpp_with.SecondaryDamageRadius = 0.0f;
                }
                cpp_with.MiningFactor = pas::str_to_float(static_cast<pas::AnsiString>(Block->GetParam(u"MiningFactor"sv)), SysUtils::DecimalSeparator);
                cpp_with.ArcadeWeaponType = Kind;
                cpp_with.Availability = aGalaxyStruct::waFree;
                cpp_with.DamageFlags = pas::constant_set<aGalaxyStruct::TDamageFlagSet>({});
                Values = Block->GetParam(u"DamageSet"sv);
                for (DamageKind = static_cast<std::uint8_t>(0); DamageKind <= static_cast<std::uint8_t>(20); ++DamageKind) {
                    if (static_cast<std::uint8_t>(pas::in_set<9, 10, 12, 12>(DamageKind) ^ 1) && pas::pos(WeaponDamageFlagNames[DamageKind], Values) > 0) {
                        pas::include_at(&cpp_with.DamageFlags, static_cast<aGalaxyStruct::TDamageKind>(DamageKind));
                    }
                }
                cpp_with.ShotType = aGalaxyStruct::wstNormal;
                cpp_with.ShotCount = 1;
                Values = Block->GetParam(u"ShotType"sv);
                if (!(pas::pos(u"Normal", Values) > 0)) {
                    if (pas::pos(u"Splash", Values) > 0) {
                        cpp_with.ShotType = aGalaxyStruct::wstSplash;
                    } else if (pas::pos(u"Exploder", Values) > 0) {
                        cpp_with.ShotType = aGalaxyStruct::wstExploder;
                    } else if (pas::pos(u"AreaDamage", Values) > 0) {
                        cpp_with.ShotType = aGalaxyStruct::wstAreaDamage;
                    } else if (pas::pos(u"Torpedo", Values) > 0) {
                        cpp_with.ShotType = aGalaxyStruct::wstTorpedo;
                    } else if (pas::pos(u"Missile", Values) > 0) {
                        cpp_with.ShotType = aGalaxyStruct::wstMissile;
                    } else if (pas::pos(u"Rocket", Values) > 0) {
                        cpp_with.ShotType = aGalaxyStruct::wstRocket;
                    } else if (pas::pos(u"Chain", Values) > 0) {
                        cpp_with.ShotType = aGalaxyStruct::wstChain;
                    }
                }
                if (pas::is_one_of<aGalaxyStruct::wstChain, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(cpp_with.ShotType)) {
                    cpp_with.ShotCount = EC_Str::ExtractDigitsToIntW(pas::view(Values));
                }
                cpp_with.AttackCount = 1;
                if (Block->CountParams(u"AttackCount"_wref.get()) > 0) {
                    cpp_with.AttackCount = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AttackCount"sv)));
                }
                cpp_with.MissileRange = 0;
                cpp_with.MissileMaxSpeed = 0;
                cpp_with.MissileMinSpeed = 0;
                cpp_with.MissileChanceToBeHit = 0;
                if (Block->CountParams(u"MissileRadius"_wref.get()) > 0) {
                    cpp_with.MissileRange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MissileRadius"sv)));
                }
                if (Block->CountParams(u"MissileMaxSpeed"_wref.get()) > 0) {
                    cpp_with.MissileMaxSpeed = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MissileMaxSpeed"sv)));
                }
                if (Block->CountParams(u"MissileMinSpeed"_wref.get()) > 0) {
                    cpp_with.MissileMinSpeed = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MissileMinSpeed"sv)));
                }
                if (Block->CountParams(u"MissileChanceToBeHit"_wref.get()) > 0) {
                    cpp_with.MissileChanceToBeHit = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MissileChanceToBeHit"sv)));
                }
                Values = Block->GetParam(u"mWeaponDamage"sv);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 8); cpp_range_2.next(Level); ) {
                    cpp_with.DamageScaleByLevel[Level] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Values), Level - 1, u","sv));
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(static_cast<TItemTypeSelection>(WeaponTypes))); cpp_range_3.next(Level); ) {
            Kind = aConst::GetItemTypeFromMask(static_cast<TItemTypeSelection>(WeaponTypes), Level);
            WeaponInfos[static_cast<TItemType>(Kind)].PrimarySE = static_cast<pas::WideString>(pas::concat_ansi({"Weapon.", SysUtils::IntToStr(Kind - 50)}));
            WeaponInfos[static_cast<TItemType>(Kind)].SecondarySE = u"Weapon.NoGraph"_w;
            if (pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(WeaponInfos[static_cast<TItemType>(Kind)].ShotType)) {
                WeaponInfos[static_cast<TItemType>(Kind)].AreaSE = u"Weapon.MissileHit"_w;
            } else {
                WeaponInfos[static_cast<TItemType>(Kind)].AreaSE = pas::WideString();
            }
            WeaponInfos[static_cast<TItemType>(Kind)].DefaultPalette = 0;
            WeaponInfos[static_cast<TItemType>(Kind)].TypeHash = Kind * 171;
        }
        WeaponInfos[t_Weapon9].SecondarySE = u"Weapon.Nine"_w;
        WeaponInfos[t_Weapon13].SecondarySE = u"Weapon.12"_w;
        WeaponInfos[t_Weapon14].AreaSE = u"Weapon.13"_w;
        WeaponInfos[t_Weapon1].InventionIndex = 8;
        WeaponInfos[t_Weapon2].InventionIndex = 9;
        WeaponInfos[t_Weapon3].InventionIndex = 10;
        WeaponInfos[t_Weapon4].InventionIndex = 11;
        WeaponInfos[t_Weapon5].InventionIndex = 12;
        WeaponInfos[t_Weapon6].InventionIndex = 13;
        WeaponInfos[t_Weapon7].InventionIndex = 14;
        WeaponInfos[t_Weapon8].InventionIndex = 15;
        WeaponInfos[t_Weapon9].InventionIndex = 16;
        WeaponInfos[t_Weapon10].InventionIndex = 17;
        WeaponInfos[t_Weapon11].InventionIndex = 18;
        WeaponInfos[t_Weapon12].InventionIndex = 19;
        WeaponInfos[t_Weapon13].InventionIndex = 19;
        WeaponInfos[t_Weapon14].InventionIndex = 19;
        WeaponInfos[t_Weapon15].InventionIndex = 19;
        WeaponInfos[t_Weapon16].InventionIndex = 16;
        WeaponInfos[t_Weapon17].InventionIndex = 10;
        WeaponInfos[t_Weapon18].InventionIndex = 11;
        WeaponInfos[t_Weapon13].Availability = aGalaxyStruct::waNotSoldAndNodeRepair;
        WeaponInfos[t_Weapon14].Availability = aGalaxyStruct::waNotSoldAndNodeRepair;
        WeaponInfos[t_Weapon15].Availability = aGalaxyStruct::waNotSoldAndNodeRepair;
        WeaponInfos[t_Weapon16].Availability = aGalaxyStruct::waPirateOnly;
        WeaponInfos[t_Weapon17].Availability = aGalaxyStruct::waPirateOnly;
        WeaponInfos[t_Weapon18].Availability = aGalaxyStruct::waPirateOnly;
    }

    void LoadMicroModuleConfiguration() {
        static const pas::Set<0, 255> WeaponTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 49}}) - pas::constant_set<pas::Set<0, 255>>({{68, 79}});
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Tokens{};
        std::int32_t Index{};
        std::int32_t Position{};
        std::int32_t Part{};
        pas::WideString Value{};
        pas::WideString CustomName{};
        std::uint8_t Kind{};
        std::uint8_t DamageKind{};
        TEquipmentBonusKind BonusKind{};
        TWeaponDamageClass DamageClass{};
        std::uint8_t StationKind{};
        pas::DynArray<std::int32_t> BlockIndices{};
        pas::DynArray<std::int32_t> SortKeys{};
        // Nested in LoadMicroModuleConfiguration; reads its current Block through the caller-popped static link.
        auto ReadMicroModuleParam = [&](pas::WideString ParamName) -> pas::WideString {
            pas::WideString Result{};
            std::int32_t I{};
            Result = pas::WideString();
            std::int32_t Count = Block->CountParams(ParamName);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                if (Result != u"") {
                    Result = pas::concat_wide({Result, u"\r\n"});
                }
                Result = pas::concat_wide_reverse({Block->GetParamByPath(pas::concat_wide({ParamName, u":", pas::wide_int_to_str(I)})), Result});
            }
            if (EC_Str::FindTextPosW(u"<"_wref.get(), Result) > 0) {
                Result = EC_Str::ReplaceAllWideString(Result, u"<br>"_wref.get(), u"\r\n"sv);
                return EC_Str::ReplaceAllWideString(Result, u"<ll>"_wref.get(), u"\r\n \r\n"sv);
            }
            return Result;
        };
        // Nested in LoadMicroModuleConfiguration; removes every occurrence of Token from its remaining-token string.
        auto ConsumeMicroModuleToken = [&](pas::WideString Token) -> std::uint8_t {
            std::uint8_t Result{};
            if (pas::pos(Token, Tokens) > 0) {
                Result = true;
                Tokens = EC_Str::ReplaceAllWideString(Tokens, Token, u""sv);
                return Result;
            }
            return false;
        };
        Block = GR_Main::LanguageDataConfig->GetBlock(u"MicroModuls"sv);
        MicroModuleTemplateCount = Block->GetBlockCount();
        MicroModuleTemplates.set_length(MicroModuleTemplateCount);
        MicroModuleCandidateIndices.set_length(MicroModuleTemplateCount);
        BlockIndices.set_length(MicroModuleTemplateCount);
        SortKeys.set_length(MicroModuleTemplateCount);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, MicroModuleTemplateCount - 1); cpp_range.next(Index); ) {
            BlockIndices[Index] = Index;
            SortKeys[Index] = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetBlockNameByIndex(Index)));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, MicroModuleTemplateCount - 2); cpp_range_2.next(Index); ) {
            for (auto cpp_range_3 = pas::for_downto<std::int32_t>(MicroModuleTemplateCount - 1, Index + 1); cpp_range_3.next(Position); ) {
                if (SortKeys[Position - 1] > SortKeys[Position]) {
                    Part = SortKeys[Position - 1];
                    SortKeys[Position - 1] = SortKeys[Position];
                    SortKeys[Position] = Part;
                    Part = BlockIndices[Position - 1];
                    BlockIndices[Position - 1] = BlockIndices[Position];
                    BlockIndices[Position] = Part;
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, MicroModuleTemplateCount - 1); cpp_range_4.next(Index); ) {
            Block = GR_Main::LanguageDataConfig->GetBlock(u"MicroModuls"sv);
            {
                TMicroModuleInfo& cpp_with = MicroModuleTemplates[Index];
                cpp_with.ConfigNumber = SortKeys[Index];
                cpp_with.ConfigName = Block->GetBlockNameByIndex(BlockIndices[Index]);
                cpp_with.ConfigNameHash = CrcUnit::InitCrc32();
                cpp_with.ConfigNameHash = CrcUnit::UpdateCrc32(cpp_with.ConfigNameHash, cpp_with.ConfigName.pchar(), cpp_with.ConfigName.length() * 2);
                cpp_with.ConfigNameHash = CrcUnit::FinishCrc32(cpp_with.ConfigNameHash);
                Block = Block->GetBlockByIndex(BlockIndices[Index]);
                cpp_with.SpecialOnly = EC_Str::ExtractDigitsToIntW(pas::view(ReadMicroModuleParam(u"Special"_w))) != 0;
                cpp_with.BlocksMicroModuleSlot = EC_Str::ExtractDigitsToIntW(pas::view(ReadMicroModuleParam(u"BlockMM"_w))) != 0;
                cpp_with.BlocksSpecialSlot = EC_Str::ExtractDigitsToIntW(pas::view(ReadMicroModuleParam(u"BlockImp"_w))) != 0;
                cpp_with.RacialRestriction = EC_Str::ExtractDigitsToIntW(pas::view(ReadMicroModuleParam(u"RacialRestriction"_w))) != 0;
                cpp_with.SeparatedNumbers = EC_Str::ExtractDigitsToIntW(pas::view(ReadMicroModuleParam(u"SeparatedNumbers"_w))) != 0;
                cpp_with.Name = ReadMicroModuleParam(u"Name"_w);
                cpp_with.NamePrefix = ReadMicroModuleParam(u"NamePrefix"_w);
                cpp_with.Color = ReadMicroModuleParam(u"Color"_w);
                cpp_with.TextReplace = ReadMicroModuleParam(u"TextReplace"_w);
                for (auto cpp_range_5 = pas::for_to<TEquipmentBonusKind>(static_cast<TEquipmentBonusKind>(0), static_cast<TEquipmentBonusKind>(42)); cpp_range_5.next(BonusKind); ) {
                    Value = ReadMicroModuleParam(EquipmentBonusNames[BonusKind]);
                    if (Value == u"") {
                        cpp_with.StatBonuses[BonusKind] = 0;
                    } else if (pas::is_one_of<bonExtraAkrinEff, bonExtraAkrinPenalty>(BonusKind)) {
                        cpp_with.StatBonuses[BonusKind] = System::Round(EC_Str::ExtractDecimalToSingleW(Value) * 1.0E+2L);
                    } else {
                        cpp_with.StatBonuses[BonusKind] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                    }
                }
                Value = ReadMicroModuleParam(u"Cost"_w);
                if (Value == u"") {
                    cpp_with.CostPercent = 100;
                } else {
                    cpp_with.CostPercent = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadMicroModuleParam(u"Size"_w);
                if (Value == u"") {
                    cpp_with.SizePercent = 100;
                } else {
                    cpp_with.SizePercent = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadMicroModuleParam(u"Fragility"_w);
                if (Value == u"") {
                    cpp_with.FragilityFactor = 1.0f;
                } else {
                    cpp_with.FragilityFactor = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value)) * 0.01L;
                }
                for (auto cpp_range_6 = pas::for_to<TWeaponDamageClass>(wdcEnergy, wdcMissile); cpp_range_6.next(DamageClass); ) {
                    Value = ReadMicroModuleParam(pas::concat_wide({u"Fragility", WeaponDamageClasses[DamageClass].Name}));
                    if (Value == u"") {
                        cpp_with.FragilityFactorByDamageClass[DamageClass] = cpp_with.FragilityFactor;
                    } else {
                        cpp_with.FragilityFactorByDamageClass[DamageClass] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value)) * 0.01L;
                    }
                }
                Value = ReadMicroModuleParam(u"Owner"_w);
                if (Value == u"" || Value == u"Any") {
                    if (cpp_with.SpecialOnly) {
                        cpp_with.AllowedHullOwnerMask = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}, {aGalaxyStruct::oiPirate}});
                    } else {
                        cpp_with.AllowedHullOwnerMask = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiDominator}, {aGalaxyStruct::oiPirate}});
                    }
                    cpp_with.AllowedDominatorSeriesMask = pas::constant_set<aGalaxyStruct::TDominatorSeriesMask>({{0, 2}});
                    cpp_with.AllowedCustomHullFactions = pas::WideString();
                } else {
                    cpp_with.AllowedHullOwnerMask = pas::constant_set<aGalaxyStruct::TOwnerMask>({});
                    cpp_with.AllowedDominatorSeriesMask = pas::constant_set<aGalaxyStruct::TDominatorSeriesMask>({});
                    Tokens = EC_Str::ReplaceAllWideString(Value, u" "_wref.get(), u""sv);
                    Tokens = pas::concat_wide({u"<", EC_Str::ReplaceAllWideString(Tokens, u","_wref.get(), u">,<"sv), u">"});
                    if (ConsumeMicroModuleToken(u"<Maloc>"_w)) {
                        pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiMaloc);
                    }
                    if (ConsumeMicroModuleToken(u"<Peleng>"_w)) {
                        pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiPeleng);
                    }
                    if (ConsumeMicroModuleToken(u"<People>"_w)) {
                        pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiHuman);
                    }
                    if (ConsumeMicroModuleToken(u"<Fei>"_w)) {
                        pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiFeyan);
                    }
                    if (ConsumeMicroModuleToken(u"<Gaal>"_w)) {
                        pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiGaal);
                    }
                    if (ConsumeMicroModuleToken(u"<PirateClan>"_w)) {
                        pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiPirate);
                    }
                    if (ConsumeMicroModuleToken(u"<None>"_w)) {
                        pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiUninhabited);
                    }
                    if (cpp_with.SpecialOnly) {
                        if (ConsumeMicroModuleToken(u"<Kling>"_w)) {
                            pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiDominator);
                        }
                        ConsumeMicroModuleToken(u"<NonKling>"_w);
                    } else {
                        if (!ConsumeMicroModuleToken(u"<NonKling>"_w)) {
                            pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiDominator);
                        }
                        ConsumeMicroModuleToken(u"<Kling>"_w);
                    }
                    if (ConsumeMicroModuleToken(u"<Blazer>"_w)) {
                        pas::include_at(&cpp_with.AllowedDominatorSeriesMask, aGalaxyStruct::dsBlazer);
                        if (cpp_with.SpecialOnly) {
                            pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiDominator);
                        }
                    }
                    if (ConsumeMicroModuleToken(u"<Terron>"_w)) {
                        pas::include_at(&cpp_with.AllowedDominatorSeriesMask, aGalaxyStruct::dsTerron);
                        if (cpp_with.SpecialOnly) {
                            pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiDominator);
                        }
                    }
                    if (ConsumeMicroModuleToken(u"<Keller>"_w)) {
                        pas::include_at(&cpp_with.AllowedDominatorSeriesMask, aGalaxyStruct::dsKeller);
                        if (cpp_with.SpecialOnly) {
                            pas::include_at(&cpp_with.AllowedHullOwnerMask, aGalaxyStruct::oiDominator);
                        }
                    }
                    if (cpp_with.AllowedDominatorSeriesMask == pas::constant_set<aGalaxyStruct::TDominatorSeriesMask>({})) {
                        cpp_with.AllowedDominatorSeriesMask = pas::constant_set<aGalaxyStruct::TDominatorSeriesMask>({{0, 2}});
                    }
                    cpp_with.AllowedCustomHullFactions = pas::WideString();
                    for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Tokens), u","sv) - 1); cpp_range_7.next(Part); ) {
                        CustomName = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Tokens), Part, u","sv));
                        if (CustomName != u"") {
                            if (cpp_with.AllowedCustomHullFactions != u"") {
                                cpp_with.AllowedCustomHullFactions = pas::concat_wide({cpp_with.AllowedCustomHullFactions, u",", CustomName});
                            } else {
                                cpp_with.AllowedCustomHullFactions = CustomName;
                            }
                        }
                    }
                }
                cpp_with.CustomFaction = ReadMicroModuleParam(u"CustomFaction"_w);
                Value = ReadMicroModuleParam(u"Priority"_w);
                if (Value == u"") {
                    cpp_with.Priority = 100;
                } else {
                    cpp_with.Priority = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadMicroModuleParam(u"Equipments"_w);
                if (Value == u"" || Value == u"Any") {
                    cpp_with.AllowedItemTypes = pas::constant_set<TItemTypeSelection>({{42, 68}});
                    cpp_with.AllowedCustomWeaponTypes = u"Any"_w;
                } else {
                    cpp_with.AllowedItemTypes = pas::constant_set<TItemTypeSelection>({});
                    cpp_with.AllowedCustomWeaponTypes = pas::WideString();
                    Tokens = EC_Str::ReplaceAllWideString(Value, u" "_wref.get(), u""sv);
                    Tokens = pas::concat_wide({u"<", EC_Str::ReplaceAllWideString(Tokens, u","_wref.get(), u">,<"sv), u">"});
                    if (ConsumeMicroModuleToken(u"<Hull>"_w)) {
                        pas::include_at(&cpp_with.AllowedItemTypes, t_Hull);
                    }
                    if (ConsumeMicroModuleToken(u"<FuelTank>"_w)) {
                        pas::include_at(&cpp_with.AllowedItemTypes, t_FuelTanks);
                    }
                    if (ConsumeMicroModuleToken(u"<Engine>"_w)) {
                        pas::include_at(&cpp_with.AllowedItemTypes, t_Engine);
                    }
                    if (ConsumeMicroModuleToken(u"<Radar>"_w)) {
                        pas::include_at(&cpp_with.AllowedItemTypes, t_Radar);
                    }
                    if (ConsumeMicroModuleToken(u"<Scaner>"_w)) {
                        pas::include_at(&cpp_with.AllowedItemTypes, t_Scaner);
                    }
                    if (ConsumeMicroModuleToken(u"<Droid>"_w)) {
                        pas::include_at(&cpp_with.AllowedItemTypes, t_RepairRobot);
                    }
                    if (ConsumeMicroModuleToken(u"<Hook>"_w)) {
                        pas::include_at(&cpp_with.AllowedItemTypes, t_CargoHook);
                    }
                    if (ConsumeMicroModuleToken(u"<DefGenerator>"_w)) {
                        pas::include_at(&cpp_with.AllowedItemTypes, t_DefGenerator);
                    }
                    for (auto cpp_range_8 = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(static_cast<TItemTypeSelection>(WeaponTypes))); cpp_range_8.next(Part); ) {
                        Kind = aConst::GetItemTypeFromMask(static_cast<TItemTypeSelection>(pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 49}}) - pas::constant_set<pas::Set<0, 255>>({{68, 79}})), Part);
                        if (ConsumeMicroModuleToken(pas::concat_wide({u"<", ItemTypeNames[static_cast<TItemType>(Kind)], u">"}))) {
                            pas::include_at(&cpp_with.AllowedItemTypes, Kind);
                        } else if (pas::pos(u"<WMissile>", Tokens) > 0 && pas::contains(WeaponInfos[static_cast<TItemType>(Kind)].DamageFlags, aGalaxyStruct::dkMissile)) {
                            pas::include_at(&cpp_with.AllowedItemTypes, Kind);
                        } else if (pas::pos(u"<WSplinter>", Tokens) > 0 && pas::contains(WeaponInfos[static_cast<TItemType>(Kind)].DamageFlags, aGalaxyStruct::dkSplinter)) {
                            pas::include_at(&cpp_with.AllowedItemTypes, Kind);
                        } else if (pas::pos(u"<WEnergy>", Tokens) > 0 && pas::contains(WeaponInfos[static_cast<TItemType>(Kind)].DamageFlags, aGalaxyStruct::dkEnergy)) {
                            pas::include_at(&cpp_with.AllowedItemTypes, Kind);
                        }
                    }
                    for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Tokens), u","sv) - 1); cpp_range_9.next(Part); ) {
                        CustomName = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Tokens), Part, u","sv));
                        if (CustomName != u"") {
                            if (cpp_with.AllowedCustomWeaponTypes != u"") {
                                cpp_with.AllowedCustomWeaponTypes = pas::concat_wide({cpp_with.AllowedCustomWeaponTypes, u",", CustomName});
                            } else {
                                cpp_with.AllowedCustomWeaponTypes = CustomName;
                            }
                        }
                    }
                }
                Value = ReadMicroModuleParam(u"Ruins"_w);
                cpp_with.OfferStationTypes = pas::constant_set<aGalaxyStruct::TShipTypeMask>({});
                cpp_with.OfferStationNames = EC_Str::ReplaceAllWideString(Value, u" "_wref.get(), u""sv);
                cpp_with.OfferStationNames = pas::concat_wide({u"<", EC_Str::ReplaceAllWideString(cpp_with.OfferStationNames, u","_wref.get(), u">,<"sv), u">"});
                if (Value == u"Any") {
                    cpp_with.OfferStationTypes = pas::constant_set<aGalaxyStruct::TShipTypeMask>({{6, 12}});
                } else if (Value != u"") {
                    for (StationKind = aGalaxyStruct::rstRangerCenter; StationKind <= aGalaxyStruct::rstDominion; ++StationKind) {
                        if (pas::pos(ShipTypeNames[StationKind].Name, Value) > 0) {
                            pas::include_at(&cpp_with.OfferStationTypes, StationKind);
                        }
                    }
                }
                cpp_with.OnPlanets = EC_Str::ExtractDigitsToIntW(pas::view(ReadMicroModuleParam(u"OnPlanets"_w))) != 0;
                Value = ReadMicroModuleParam(u"WeaponMods"_w);
                cpp_with.WeaponDamageFlags = pas::constant_set<aGalaxyStruct::TDamageFlagSet>({});
                if (Value != u"") {
                    for (DamageKind = static_cast<std::uint8_t>(0); DamageKind <= static_cast<std::uint8_t>(20); ++DamageKind) {
                        if (static_cast<std::uint8_t>(pas::in_range(DamageKind, static_cast<std::int32_t>(aGalaxyStruct::dkEnergy), static_cast<std::int32_t>(aGalaxyStruct::dkMissile)) ^ 1) && static_cast<std::uint8_t>(pas::in_set<9, 10, 12, 12>(DamageKind) ^ 1) && pas::pos(WeaponDamageFlagNames[DamageKind], Value) > 0) {
                            pas::include_at(&cpp_with.WeaponDamageFlags, static_cast<aGalaxyStruct::TDamageKind>(DamageKind));
                        }
                    }
                }
                cpp_with.KindGraph = ReadMicroModuleParam(u"KindGraph"_w);
                cpp_with.MissileGraph = ReadMicroModuleParam(u"MissileGraph"_w);
                Value = ReadMicroModuleParam(u"ShotVisual"_w);
                if (Value == u"") {
                    cpp_with.ShotVisual = -1;
                } else {
                    cpp_with.ShotVisual = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadMicroModuleParam(u"HullGraphSize"_w);
                if (Value == u"") {
                    cpp_with.HullGraphSizePercent = 100;
                } else {
                    cpp_with.HullGraphSizePercent = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                cpp_with.CustomTag = ReadMicroModuleParam(u"CustomTag"_w);
            }
        }
        BlockIndices = nullptr;
        SortKeys = nullptr;
    }

    void InitializeCaptainHealthDefinitions() {
        std::int32_t I{};
        pas::WideString Path{};
        pas::WideString Value{};
        CaptainHealthDefinitions[1].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[1].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[1].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[1].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[1].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[1].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[1].DevelopmentRate = 1.0E+2;
        CaptainHealthDefinitions[1].InfectionChance = 1.0;
        CaptainHealthDefinitions[1].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{3}});
        CaptainHealthDefinitions[1].Duration = 150;
        CaptainHealthDefinitions[2].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}});
        CaptainHealthDefinitions[2].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}, {aGalaxyStruct::oiHuman}, {aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[2].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[2].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{3}, {4}, {5}});
        CaptainHealthDefinitions[2].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[2].MedicalPriceSizeLevel = 4;
        CaptainHealthDefinitions[2].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[2].InfectionChance = 1.0;
        CaptainHealthDefinitions[2].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{0}});
        CaptainHealthDefinitions[2].Duration = 555;
        CaptainHealthDefinitions[3].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[3].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc}, {aGalaxyStruct::oiPeleng}, {aGalaxyStruct::oiHuman}});
        CaptainHealthDefinitions[3].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{3}, {4}, {5}});
        CaptainHealthDefinitions[3].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[3].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[3].MedicalPriceSizeLevel = 3;
        CaptainHealthDefinitions[3].DevelopmentRate = 1.0E+2;
        CaptainHealthDefinitions[3].InfectionChance = 1.0;
        CaptainHealthDefinitions[3].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{3}});
        CaptainHealthDefinitions[3].Duration = 200;
        CaptainHealthDefinitions[4].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[4].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}, {aGalaxyStruct::oiHuman}, {aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[4].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[4].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[4].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[4].MedicalPriceSizeLevel = 5;
        CaptainHealthDefinitions[4].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[4].InfectionChance = 1.0;
        CaptainHealthDefinitions[4].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{2}});
        CaptainHealthDefinitions[4].Duration = 1000;
        CaptainHealthDefinitions[5].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[5].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[5].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[5].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[5].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[5].MedicalPriceSizeLevel = 1;
        CaptainHealthDefinitions[5].DevelopmentRate = 1.0E+1;
        CaptainHealthDefinitions[5].InfectionChance = 1.0;
        CaptainHealthDefinitions[5].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}});
        CaptainHealthDefinitions[5].Duration = 170;
        CaptainHealthDefinitions[6].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[6].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[6].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[6].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[6].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[6].MedicalPriceSizeLevel = 4;
        CaptainHealthDefinitions[6].DevelopmentRate = 1.0E+2;
        CaptainHealthDefinitions[6].InfectionChance = 1.0;
        CaptainHealthDefinitions[6].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({});
        CaptainHealthDefinitions[6].Duration = 1000;
        CaptainHealthDefinitions[7].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[7].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[7].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[7].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[7].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[7].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[7].DevelopmentRate = 1.0E+2;
        CaptainHealthDefinitions[7].InfectionChance = 1.0;
        CaptainHealthDefinitions[7].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{3}});
        CaptainHealthDefinitions[7].Duration = 130;
        CaptainHealthDefinitions[8].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}, {aGalaxyStruct::oiHuman}, {aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[8].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}, {aGalaxyStruct::oiHuman}, {aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[8].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[8].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[8].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[8].MedicalPriceSizeLevel = 1;
        CaptainHealthDefinitions[8].DevelopmentRate = 1.0E+2;
        CaptainHealthDefinitions[8].InfectionChance = 1.0;
        CaptainHealthDefinitions[8].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{3}});
        CaptainHealthDefinitions[8].Duration = 100;
        CaptainHealthDefinitions[9].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc}});
        CaptainHealthDefinitions[9].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc}});
        CaptainHealthDefinitions[9].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[9].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[9].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[9].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[9].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[9].InfectionChance = 1.0;
        CaptainHealthDefinitions[9].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}});
        CaptainHealthDefinitions[9].Duration = 180;
        CaptainHealthDefinitions[10].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[10].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}});
        CaptainHealthDefinitions[10].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[10].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[10].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[10].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[10].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[10].InfectionChance = 1.0;
        CaptainHealthDefinitions[10].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}});
        CaptainHealthDefinitions[10].Duration = 122;
        CaptainHealthDefinitions[11].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[11].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiFeyan}});
        CaptainHealthDefinitions[11].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[11].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[11].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[11].MedicalPriceSizeLevel = 4;
        CaptainHealthDefinitions[11].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[11].InfectionChance = 1.0;
        CaptainHealthDefinitions[11].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}});
        CaptainHealthDefinitions[11].Duration = 164;
        CaptainHealthDefinitions[12].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[12].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[12].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[12].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[12].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[12].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[12].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[12].InfectionChance = 0.5;
        CaptainHealthDefinitions[12].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}});
        CaptainHealthDefinitions[12].Duration = 88;
        RadiationHealthDefinitions[1].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        RadiationHealthDefinitions[1].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        RadiationHealthDefinitions[1].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        RadiationHealthDefinitions[1].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        RadiationHealthDefinitions[1].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        RadiationHealthDefinitions[1].MedicalPriceSizeLevel = 4;
        RadiationHealthDefinitions[1].DevelopmentRate = 1.0E+2;
        RadiationHealthDefinitions[1].InfectionChance = 0.0;
        RadiationHealthDefinitions[1].Locations = pas::constant_set<aGalaxyStruct::TByteMask>({});
        RadiationHealthDefinitions[1].Duration = 30;
        CaptainHealthDefinitions[13].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[13].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[13].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[13].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[13].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[13].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[13].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[13].InfectionChance = 0.9;
        CaptainHealthDefinitions[13].Duration = 140;
        CaptainHealthDefinitions[14].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[14].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[14].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[14].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[14].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[14].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[14].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[14].InfectionChance = 0.9;
        CaptainHealthDefinitions[14].Duration = 130;
        CaptainHealthDefinitions[15].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[15].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[15].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[15].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[15].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[15].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[15].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[15].InfectionChance = 0.8;
        CaptainHealthDefinitions[15].Duration = 140;
        CaptainHealthDefinitions[16].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[16].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[16].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[16].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[16].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[16].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[16].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[16].InfectionChance = 0.4;
        CaptainHealthDefinitions[16].Duration = 120;
        CaptainHealthDefinitions[17].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[17].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[17].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[17].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[17].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[17].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[17].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[17].InfectionChance = 0.9;
        CaptainHealthDefinitions[17].Duration = 90;
        CaptainHealthDefinitions[18].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[18].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[18].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[18].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[18].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[18].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[18].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[18].InfectionChance = 0.8;
        CaptainHealthDefinitions[18].Duration = 300;
        CaptainHealthDefinitions[19].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[19].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[19].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[19].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[19].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[19].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[19].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[19].InfectionChance = 0.9;
        CaptainHealthDefinitions[19].Duration = 140;
        CaptainHealthDefinitions[20].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[20].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[20].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[20].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[20].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[20].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[20].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[20].InfectionChance = 0.9;
        CaptainHealthDefinitions[20].Duration = 200;
        CaptainHealthDefinitions[21].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[21].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[21].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[21].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[21].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[21].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[21].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[21].InfectionChance = 0.9;
        CaptainHealthDefinitions[21].Duration = 200;
        CaptainHealthDefinitions[22].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[22].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[22].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[22].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[22].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[22].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[22].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[22].InfectionChance = 0.25;
        CaptainHealthDefinitions[22].Duration = 150;
        CaptainHealthDefinitions[23].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[23].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[23].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[23].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[23].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[23].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[23].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[23].InfectionChance = 0.15;
        CaptainHealthDefinitions[23].Duration = 90;
        CaptainHealthDefinitions[24].AllowedLocationOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[24].AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
        CaptainHealthDefinitions[24].AllowedRatingBands = pas::constant_set<aGalaxyStruct::TByteMask>({{1}, {2}, {3}, {4}, {5}});
        CaptainHealthDefinitions[24].AllowedRanks = pas::constant_set<aGalaxyStruct::TByteMask>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}});
        CaptainHealthDefinitions[24].AllowedCareers = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}, {aGalaxyStruct::rcPirate}, {aGalaxyStruct::rcWarrior}});
        CaptainHealthDefinitions[24].MedicalPriceSizeLevel = 2;
        CaptainHealthDefinitions[24].DevelopmentRate = 1.0;
        CaptainHealthDefinitions[24].InfectionChance = 0.2;
        CaptainHealthDefinitions[24].Duration = 120;
        for (I = 1; I <= 12; ++I) {
            TIllnessInfo& cpp_with = CaptainHealthDefinitions[I];
            Path = static_cast<pas::WideString>(pas::concat_ansi({"Illness.Illness.", SysUtils::IntToStr(I - 1)}));
            cpp_with.Name = aConst::LocalizedText(pas::concat_wide({Path, u".Name"}));
            cpp_with.Text = aConst::LocalizedText(pas::concat_wide({Path, u".Text"}));
            cpp_with.Disabled = false;
            Value = aConst::LocalizedText(pas::concat_wide({Path, u".Time"}));
            if (Value != u"") {
                cpp_with.Duration = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
            }
        }
        for (I = 1; I <= 1; ++I) {
            TIllnessInfo& cpp_with_2 = RadiationHealthDefinitions[I];
            Path = static_cast<pas::WideString>(pas::concat_ansi({"Illness.ExtraIllness.", SysUtils::IntToStr(I)}));
            cpp_with_2.Name = aConst::LocalizedText(pas::concat_wide({Path, u".Name"}));
            cpp_with_2.Text = aConst::LocalizedText(pas::concat_wide({Path, u".Text"}));
            cpp_with_2.Disabled = false;
            Value = aConst::LocalizedText(pas::concat_wide({Path, u".Time"}));
            if (Value != u"") {
                cpp_with_2.Duration = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
            }
        }
        for (I = 1; I <= 12; ++I) {
            TIllnessInfo& cpp_with_3 = CaptainHealthDefinitions[12 + I];
            Path = static_cast<pas::WideString>(pas::concat_ansi({"Illness.Stimulant.", SysUtils::IntToStr(I - 1)}));
            cpp_with_3.Name = aConst::LocalizedText(pas::concat_wide({Path, u".Name"}));
            cpp_with_3.Text = aConst::LocalizedText(pas::concat_wide({Path, u".Text"}));
            cpp_with_3.Disabled = false;
            Value = aConst::LocalizedText(pas::concat_wide({Path, u".Time"}));
            if (Value != u"") {
                cpp_with_3.Duration = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
            }
        }
    }

    void LoadHullSeriesConfiguration() {
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Index{};
        std::int32_t Temp{};
        pas::WideString Value{};
        TWeaponDamageClass DamageKind{};
        pas::DynArray<std::int32_t> BlockIndices{};
        pas::DynArray<std::int32_t> SortKeys{};
        // Nested in LoadHullSeriesConfiguration; reads its current Block through the caller-popped static link.
        auto ReadHullSeriesParam = [&](pas::WideString ParamName) -> pas::WideString {
            pas::WideString Result{};
            std::int32_t I{};
            Result = pas::WideString();
            std::int32_t Count = Block->CountParams(ParamName);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                if (Result != u"") {
                    Result = pas::concat_wide({Result, u"\r\n"});
                }
                Result = pas::concat_wide_reverse({Block->GetParamByPath(pas::concat_wide({ParamName, u":", pas::wide_int_to_str(I)})), Result});
            }
            if (EC_Str::FindTextPosW(u"<"_wref.get(), Result) > 0) {
                Result = EC_Str::ReplaceAllWideString(Result, u"<br>"_wref.get(), u"\r\n"sv);
                return EC_Str::ReplaceAllWideString(Result, u"<ll>"_wref.get(), u"\r\n \r\n"sv);
            }
            return Result;
        };
        Block = GR_Main::LanguageDataConfig->GetBlock(u"HullType"sv);
        HullSeriesCount = Block->GetBlockCount();
        BlockIndices.set_length(HullSeriesCount);
        SortKeys.set_length(HullSeriesCount);
        std::int32_t Position = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, HullSeriesCount - 1); cpp_range.next(Index); ) {
            if (Block->GetBlockNameByIndex(Index) != u"HullOldfag") {
                BlockIndices[Position] = Index;
                SortKeys[Position] = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetBlockNameByIndex(Index)));
                ++Position;
            }
        }
        HullSeriesCount = Position;
        BlockIndices.set_length(HullSeriesCount);
        SortKeys.set_length(HullSeriesCount);
        HullSeriesDefinitions.set_length(HullSeriesCount);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, HullSeriesCount - 2); cpp_range_2.next(Index); ) {
            for (auto cpp_range_3 = pas::for_downto<std::int32_t>(HullSeriesCount - 1, Index + 1); cpp_range_3.next(Position); ) {
                if (SortKeys[Position - 1] > SortKeys[Position]) {
                    Temp = SortKeys[Position - 1];
                    SortKeys[Position - 1] = SortKeys[Position];
                    SortKeys[Position] = Temp;
                    Temp = BlockIndices[Position - 1];
                    BlockIndices[Position - 1] = BlockIndices[Position];
                    BlockIndices[Position] = Temp;
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, HullSeriesCount - 1); cpp_range_4.next(Index); ) {
            Block = GR_Main::LanguageDataConfig->GetBlock(u"HullType"sv);
            {
                THullTypeInfo& cpp_with = HullSeriesDefinitions[Index];
                cpp_with.SortKey = SortKeys[Index];
                cpp_with.SystemName = Block->GetBlockNameByIndex(BlockIndices[Index]);
                cpp_with.SystemNameCRC = CrcUnit::InitCrc32();
                cpp_with.SystemNameCRC = CrcUnit::UpdateCrc32(cpp_with.SystemNameCRC, cpp_with.SystemName.pchar(), cpp_with.SystemName.length() * 2);
                cpp_with.SystemNameCRC = CrcUnit::FinishCrc32(cpp_with.SystemNameCRC);
                Block = Block->GetBlockByIndex(BlockIndices[Index]);
                cpp_with.Name = ReadHullSeriesParam(u"Name"_w);
                cpp_with.Text = ReadHullSeriesParam(u"Text"_w);
                Value = ReadHullSeriesParam(u"Race"_w);
                cpp_with.AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({});
                if (Value == u"" || Value == u"Any") {
                    cpp_with.AllowedOwners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
                } else {
                    if (pas::pos(u"Maloc", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedOwners, aGalaxyStruct::oiMaloc);
                    }
                    if (pas::pos(u"Peleng", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedOwners, aGalaxyStruct::oiPeleng);
                    }
                    if (pas::pos(u"People", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedOwners, aGalaxyStruct::oiHuman);
                    }
                    if (pas::pos(u"Fei", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedOwners, aGalaxyStruct::oiFeyan);
                    }
                    if (pas::pos(u"Gaal", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedOwners, aGalaxyStruct::oiGaal);
                    }
                }
                Value = ReadHullSeriesParam(u"ShipType"_w);
                cpp_with.AllowedShipTypes = pas::constant_set<THullShipTypeMask>({});
                if (Value == u"" || Value == u"Any") {
                    cpp_with.AllowedShipTypes = pas::constant_set<THullShipTypeMask>({{aGalaxyStruct::htRanger, aGalaxyStruct::htDiplomat}});
                } else {
                    if (pas::pos(u"Transport", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedShipTypes, aGalaxyStruct::htTransport);
                    }
                    if (pas::pos(u"Liner", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedShipTypes, aGalaxyStruct::htLiner);
                    }
                    if (pas::pos(u"Diplomat", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedShipTypes, aGalaxyStruct::htDiplomat);
                    }
                    if (pas::pos(u"Ranger", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedShipTypes, aGalaxyStruct::htRanger);
                    }
                    if (pas::pos(u"Pirate", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedShipTypes, aGalaxyStruct::htPirate);
                    }
                    if (pas::pos(u"Warrior", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedShipTypes, aGalaxyStruct::htWarrior);
                    }
                    if (pas::pos(u"Flagman", Value) > 0) {
                        pas::include_at(&cpp_with.AllowedShipTypes, aGalaxyStruct::htFlagship);
                    }
                }
                cpp_with.SlotBonuses[sskFuelTanks] = 0;
                cpp_with.SlotBonuses[sskEngine] = 0;
                cpp_with.SlotBonuses[sskUnsupported] = 0;
                Value = ReadHullSeriesParam(u"Radar"_w);
                if (Value == u"") {
                    cpp_with.SlotBonuses[sskRadar] = 0;
                } else {
                    cpp_with.SlotBonuses[sskRadar] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Scaner"_w);
                if (Value == u"") {
                    cpp_with.SlotBonuses[sskScanner] = 0;
                } else {
                    cpp_with.SlotBonuses[sskScanner] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Droid"_w);
                if (Value == u"") {
                    cpp_with.SlotBonuses[sskRepairRobot] = 0;
                } else {
                    cpp_with.SlotBonuses[sskRepairRobot] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Hook"_w);
                if (Value == u"") {
                    cpp_with.SlotBonuses[sskCargoHook] = 0;
                } else {
                    cpp_with.SlotBonuses[sskCargoHook] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Def"_w);
                if (Value == u"") {
                    cpp_with.SlotBonuses[sskDefGenerator] = 0;
                } else {
                    cpp_with.SlotBonuses[sskDefGenerator] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Weapon"_w);
                if (Value == u"") {
                    cpp_with.SlotBonuses[sskWeapon] = 0;
                } else {
                    cpp_with.SlotBonuses[sskWeapon] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Artefact"_w);
                if (Value == u"") {
                    cpp_with.SlotBonuses[sskArtefact] = 0;
                } else {
                    cpp_with.SlotBonuses[sskArtefact] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Forsage"_w);
                if (Value == u"") {
                    cpp_with.SlotBonuses[sskAfterburner] = 0;
                } else {
                    cpp_with.SlotBonuses[sskAfterburner] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Size"_w);
                if (Value == u"") {
                    cpp_with.SizePercent = 100;
                } else {
                    cpp_with.SizePercent = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Cost"_w);
                if (Value == u"") {
                    cpp_with.CostPercent = 100;
                } else {
                    cpp_with.CostPercent = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Fragility"_w);
                if (Value == u"") {
                    cpp_with.FragilityFactor = 1.0f;
                } else {
                    cpp_with.FragilityFactor = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value)) * 0.01L;
                }
                for (auto cpp_range_5 = pas::for_to<TWeaponDamageClass>(wdcEnergy, wdcMissile); cpp_range_5.next(DamageKind); ) {
                    Value = ReadHullSeriesParam(pas::concat_wide({u"Fragility", WeaponDamageClasses[DamageKind].Name}));
                    if (Value == u"") {
                        cpp_with.FragilityByDamageClass[DamageKind] = cpp_with.FragilityFactor;
                    } else {
                        cpp_with.FragilityByDamageClass[DamageKind] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value)) * 0.01L;
                    }
                }
                Value = ReadHullSeriesParam(u"Year"_w);
                if (Value == u"") {
                    cpp_with.Year = 0;
                } else {
                    cpp_with.Year = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
                Value = ReadHullSeriesParam(u"Probability"_w);
                if (Value == u"") {
                    cpp_with.ProbabilityWeight = 1;
                } else {
                    cpp_with.ProbabilityWeight = SysUtils::StrToInt(static_cast<pas::AnsiString>(Value));
                }
            }
        }
    }

    void InitializeGameplayConfig() {
        std::uint8_t Level{};
        std::uint8_t GoodsIndex{};
        aGalaxyStruct::TPlanetGovernment Government{};
        aGalaxyStruct::TRelationLevel Relation{};
        std::uint8_t KlingKind{};
        std::uint8_t Series{};
        aGalaxyStruct::TOwnerId Owner{};
        aGalaxyStruct::TPlanetEconomy Economy{};
        aGalaxyStruct::TGalaxyDifficultyTuning* Difficulty{};
        // Nested in InitializeGameplayConfig; unused caller-popped static link.
        auto ExtrapolateLinearDifficulty = [&](std::uint8_t Level, float Level2, float Level3) -> float {
            std::int32_t Delta = Level - 3;
            return Level3 + (static_cast<long double>(Level3) - Level2) * Delta;
        };
        // Nested in InitializeGameplayConfig; unused caller-popped static link.
        auto ExtrapolateGeometricDifficulty = [&](std::uint8_t Level, float Level2, float Level3) -> float {
            std::int32_t Delta = Level - 3;
            return Level3 * System::Exp(System::Ln(pas::real_divide(Level3, Level2)) * Delta);
        };
        if (GR_Main::LanguageDataConfig->CountParamsByPath(u"Constellations.GalaxyCountStars"_wref.get()) > 0) {
            GalaxyStarCount = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::LanguageDataConfig->GetParamByPath(u"Constellations.GalaxyCountStars"_wref.get())));
        } else {
            GalaxyStarCount = 73;
        }
        if (GR_Main::LanguageDataConfig->CountParamsByPath(u"Constellations.GalaxySizeY"_wref.get()) > 0) {
            GalaxySizeY = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::LanguageDataConfig->GetParamByPath(u"Constellations.GalaxySizeY"_wref.get())));
        } else {
            GalaxySizeY = 100;
        }
        if (GR_Main::LanguageDataConfig->CountParamsByPath(u"Constellations.GalaxySizeX"_wref.get()) > 0) {
            GalaxySizeX = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::LanguageDataConfig->GetParamByPath(u"Constellations.GalaxySizeX"_wref.get())));
        } else {
            GalaxySizeX = 145;
        }
        if (GR_Main::LanguageDataConfig->CountParamsByPath(u"GalaxyNews.MaxCntPlanetNews"_wref.get()) > 0) {
            MaxPlanetNews = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::LanguageDataConfig->GetParamByPath(u"GalaxyNews.MaxCntPlanetNews"_wref.get())));
        } else {
            MaxPlanetNews = 9;
        }
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(4), static_cast<std::uint8_t>(9)); cpp_range.next(Level); ) {
            Difficulty = &GalaxyDifficultyTuning[Level];
            Difficulty->MaximumQuestProgramRewardCount = 1;
            Difficulty->GoodsEventDurationFactor = ExtrapolateLinearDifficulty(Level, GalaxyDifficultyTuning[2].GoodsEventDurationFactor, GalaxyDifficultyTuning[3].GoodsEventDurationFactor);
            Difficulty->QuestTimeAndExperienceFactor = ExtrapolateLinearDifficulty(Level, GalaxyDifficultyTuning[2].QuestTimeAndExperienceFactor, GalaxyDifficultyTuning[3].QuestTimeAndExperienceFactor);
            Difficulty->EquipmentWearFactor = ExtrapolateLinearDifficulty(Level, GalaxyDifficultyTuning[2].EquipmentWearFactor, GalaxyDifficultyTuning[3].EquipmentWearFactor);
            Difficulty->InitialPirateControlPercent = System::Round(ExtrapolateLinearDifficulty(Level, GalaxyDifficultyTuning[2].InitialPirateControlPercent, GalaxyDifficultyTuning[3].InitialPirateControlPercent));
            Difficulty->MarketPriceBandSqueeze = ExtrapolateLinearDifficulty(Level, GalaxyDifficultyTuning[2].MarketPriceBandSqueeze, GalaxyDifficultyTuning[3].MarketPriceBandSqueeze);
            Difficulty->RandomHoleSpawnRollMaximum = System::Round(ExtrapolateLinearDifficulty(Level, GalaxyDifficultyTuning[2].RandomHoleSpawnRollMaximum, GalaxyDifficultyTuning[3].RandomHoleSpawnRollMaximum));
            Difficulty->MaximumResearchMaterialConsumption = System::Round(ExtrapolateLinearDifficulty(Level, GalaxyDifficultyTuning[2].MaximumResearchMaterialConsumption, GalaxyDifficultyTuning[3].MaximumResearchMaterialConsumption));
            Difficulty->ArcadeDamageTakenScale = ExtrapolateLinearDifficulty(Level, GalaxyDifficultyTuning[2].ArcadeDamageTakenScale, GalaxyDifficultyTuning[3].ArcadeDamageTakenScale);
            Difficulty->InventionProgressScale = ExtrapolateGeometricDifficulty(Level, GalaxyDifficultyTuning[2].InventionProgressScale, GalaxyDifficultyTuning[3].InventionProgressScale);
            Difficulty->ArcadeRewardScale = ExtrapolateGeometricDifficulty(Level, GalaxyDifficultyTuning[2].ArcadeRewardScale, GalaxyDifficultyTuning[3].ArcadeRewardScale);
            Difficulty->QuestMoneyFactor = ExtrapolateGeometricDifficulty(Level, GalaxyDifficultyTuning[2].QuestMoneyFactor, GalaxyDifficultyTuning[3].QuestMoneyFactor);
            Difficulty->StartingPlayerMoney = System::Round(ExtrapolateGeometricDifficulty(Level, GalaxyDifficultyTuning[2].StartingPlayerMoney, GalaxyDifficultyTuning[3].StartingPlayerMoney));
            Difficulty->MaximumDominatorResearchRate = ExtrapolateGeometricDifficulty(Level, GalaxyDifficultyTuning[2].MaximumDominatorResearchRate, GalaxyDifficultyTuning[3].MaximumDominatorResearchRate);
            Difficulty->CoalitionToPirateBalanceRatio = ExtrapolateGeometricDifficulty(Level, GalaxyDifficultyTuning[2].CoalitionToPirateBalanceRatio, GalaxyDifficultyTuning[3].CoalitionToPirateBalanceRatio);
        }
        for (auto cpp_range_2 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_2.next(GoodsIndex); ) {
            GoodsMarket[GoodsIndex].DisplayName = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Name.", SysUtils::IntToStr(GoodsIndex + 1)})));
        }
        for (auto cpp_range_3 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_3.next(GoodsIndex); ) {
            GoodsMarket[GoodsIndex].TradeName = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.NameBuy.", SysUtils::IntToStr(GoodsIndex + 1)})));
        }
        for (auto cpp_range_4 = pas::for_to<aGalaxyStruct::TPlanetGovernment>(aGalaxyStruct::pgAnarchy, aGalaxyStruct::pgDemocracy); cpp_range_4.next(Government); ) {
            PlanetGovernmentMarket[Government].DisplayName = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Goverment.Type.", SysUtils::IntToStr(Government)})));
        }
        for (auto cpp_range_5 = pas::for_to<aGalaxyStruct::TRelationLevel>(aGalaxyStruct::rlHostile, aGalaxyStruct::rlExcellent); cpp_range_5.next(Relation); ) {
            RelationInfo[Relation].DisplayName = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Relations.Type.", SysUtils::IntToStr(Relation)})));
        }
        for (auto cpp_range_6 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_6.next(KlingKind); ) {
            for (auto cpp_range_7 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(2)); cpp_range_7.next(Series); ) {
                DominatorShipDefinitions[KlingKind].DisplayNames[Series] = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"ShipType.Dominator.", DominatorSeriesNames[Series], u".", pas::wide_int_to_str(static_cast<std::int32_t>(KlingKind))}));
            }
        }
        for (auto cpp_range_8 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range_8.next(Owner); ) {
            OwnerInfo[Owner].DisplayName = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"Race.Name.", OwnerInfo[Owner].InternalName}));
        }
        // Both identical localization passes are present in the native initializer.
        for (auto cpp_range_9 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range_9.next(Owner); ) {
            OwnerInfo[Owner].DisplayName = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"Race.Name.", OwnerInfo[Owner].InternalName}));
        }
        for (auto cpp_range_10 = pas::for_to<aGalaxyStruct::TPlanetEconomy>(aGalaxyStruct::peAgricultural, aGalaxyStruct::peIndustrial); cpp_range_10.next(Economy); ) {
            PlanetEconomyInfo[Economy].DisplayName = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Economy.Name.", SysUtils::IntToStr(Economy)})));
            PlanetEconomyInfo[Economy].ShortDisplayName = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Economy.ShortName.", SysUtils::IntToStr(Economy)})));
        }
        if (!GoodsMarketBaseCaptured) {
            for (auto cpp_range_11 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_11.next(GoodsIndex); ) {
                GoodsMarketBase[GoodsIndex] = GoodsMarket[GoodsIndex];
            }
            GoodsMarketBaseCaptured = true;
        }
        SE_Weapon::InitializeWeaponVisualResources();
        aConst::LoadEquipmentConfiguration();
        aConst::LoadWeaponConfiguration();
        aConst::LoadArtefactConfiguration();
        aConst::LoadDamageSkillQuestMarketConfiguration();
        aConst::LoadMicroModuleConfiguration();
        aConst::InitializeCaptainHealthDefinitions();
        aConst::LoadHullSeriesConfiguration();
        if (GR_Main::LanguageDataConfig->CountParamsByPath(u"Artefacts.NumericValues.MaxSlots"_wref.get()) > 0) {
            DefaultHullSlotCounts[sskArtefact] = std::max<std::int32_t>(4, std::min<std::int32_t>(32, EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::LanguageDataConfig->GetParamByPath(u"Artefacts.NumericValues.MaxSlots"_wref.get())))));
        } else {
            DefaultHullSlotCounts[sskArtefact] = 4;
        }
        HullMassEvaluationStart = System::Round(static_cast<long double>(HullBaseSize) * EquipmentSizeFactors[5] * 2.0L);
        HullMassEvaluationEnd = System::Round(static_cast<long double>(HullBaseSize) * EquipmentSizeFactors[1] * 2.0L);
        WearMassMin = System::Round(static_cast<long double>(HullBaseSize) * EquipmentSizeFactors[1] * 5.0L);
        WearMassMax = System::Round(static_cast<long double>(HullBaseSize) * EquipmentSizeFactors[1] * 5.0E+1L);
    }

    TShipSlotKind ItemTypeToSlotKind(TItemType ItemType) {
        switch (ItemType) {
            case t_FuelTanks: return sskFuelTanks;
            case t_Engine: return sskEngine;
            case t_Radar: return sskRadar;
            case t_Scaner: return sskScanner;
            case t_RepairRobot: return sskRepairRobot;
            case t_CargoHook: return sskCargoHook;
            case t_DefGenerator: return sskDefGenerator;
            default: {
                if (pas::in_range(ItemType, static_cast<std::int32_t>(t_Weapon1), static_cast<std::int32_t>(t_CustomWeapon))) {
                    return sskWeapon;
                } else if (pas::in_range(ItemType, static_cast<std::int32_t>(t_Artefact), static_cast<std::int32_t>(t_ArtFastRacks))) {
                    return sskArtefact;
                } else {
                    return sskUnsupported;
                }
            }
        }
    }

    // Missile bit takes precedence over splinter; otherwise energy.
    TWeaponDamageClass ClassifyWeaponDamageFlags(aGalaxyStruct::TDamageFlagSet Flags) {
        if (pas::contains(Flags, aGalaxyStruct::dkMissile)) {
            return wdcMissile;
        } else if (pas::contains(Flags, aGalaxyStruct::dkSplinter)) {
            return wdcSplinter;
        } else {
            return wdcEnergy;
        }
    }

    // Class/subtype mapping used by hull generation and legacy saves; only TObject RTTI operations precede explicit subclass casts.
    std::uint8_t ShipToHullType(pas::Object* Ship) {
        std::uint8_t Result = aGalaxyStruct::htRanger;
        if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
            return aGalaxyStruct::htRanger;
        } else if (pas::class_cast_if<aWarrior::TWarrior*>(Ship) != nullptr) {
            return aGalaxyStruct::htWarrior;
        } else if (pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr) {
            return aGalaxyStruct::htPirate;
        } else if (aTransport::TTransport* transport = pas::class_cast_if<aTransport::TTransport*>(Ship)) {
            if (transport->TransportType == aTransport::ttTransport) {
                return aGalaxyStruct::htTransport;
            } else if (transport->TransportType == aTransport::ttLiner) {
                return aGalaxyStruct::htLiner;
            } else {
                return aGalaxyStruct::htDiplomat;
            }
        } else if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) {
            return aGalaxyStruct::htKling;
        } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr) {
            return aGalaxyStruct::htTranclucator;
        } else if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) {
            return aGalaxyStruct::htStation;
        } else {
            GR_Main::RaiseWideMessage(u"ShipToSShipType"_wref.get());
            return Result;
        }
    }

    // Identity conversion for Coalition races 0..4; raises for all other values.
    aGalaxyStruct::TOwnerId RaceToOwner(aGalaxyStruct::TOwnerId RaceId) {
        switch (RaceId) {
            case aGalaxyStruct::oiMaloc: return aGalaxyStruct::oiMaloc;
            case aGalaxyStruct::oiPeleng: return aGalaxyStruct::oiPeleng;
            case aGalaxyStruct::oiHuman: return aGalaxyStruct::oiHuman;
            case aGalaxyStruct::oiFeyan: return aGalaxyStruct::oiFeyan;
            case aGalaxyStruct::oiGaal: return aGalaxyStruct::oiGaal;
            default: {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in RaceToOwner ", EC_Str::IntToWideString(RaceId)}))));
            }
        }
    }

    aGalaxyStruct::TOwnerId OwnerFromInternalName(const std::u16string_view& Name) {
        if (Name == u"Maloc"sv) {
            return aGalaxyStruct::oiMaloc;
        }
        if (Name == u"Peleng"sv) {
            return aGalaxyStruct::oiPeleng;
        }
        if (Name == u"People"sv) {
            return aGalaxyStruct::oiHuman;
        }
        if (Name == u"Fei"sv) {
            return aGalaxyStruct::oiFeyan;
        }
        if (Name == u"Gaal"sv) {
            return aGalaxyStruct::oiGaal;
        }
        if (Name == u"Kling"sv) {
            return aGalaxyStruct::oiDominator;
        }
        if (Name == u"PirateClan"sv) {
            return aGalaxyStruct::oiPirate;
        }
        return aGalaxyStruct::oiUninhabited;
    }

    // Identity conversion for Coalition owners 0..4; raises for all other values.
    aGalaxyStruct::TOwnerId OwnerToRace(aGalaxyStruct::TOwnerId OwnerId) {
        switch (OwnerId) {
            case aGalaxyStruct::oiMaloc: return aGalaxyStruct::oiMaloc;
            case aGalaxyStruct::oiPeleng: return aGalaxyStruct::oiPeleng;
            case aGalaxyStruct::oiHuman: return aGalaxyStruct::oiHuman;
            case aGalaxyStruct::oiFeyan: return aGalaxyStruct::oiFeyan;
            case aGalaxyStruct::oiGaal: return aGalaxyStruct::oiGaal;
            default: pas::raise(pas::make_exception<pas::Exception>("Error in OwnerToRace"_a));
        }
    }

    // Raises outside Coalition races 0..4.
    pas::WideString RaceToSys(aGalaxyStruct::TOwnerId RaceId) {
        switch (RaceId) {
            case aGalaxyStruct::oiMaloc: return u"Maloc"_w;
            case aGalaxyStruct::oiPeleng: return u"Peleng"_w;
            case aGalaxyStruct::oiHuman: return u"People"_w;
            case aGalaxyStruct::oiFeyan: return u"Fei"_w;
            case aGalaxyStruct::oiGaal: return u"Gaal"_w;
            default: pas::raise(pas::make_exception<pas::Exception>("Error in RaceToSys"_a));
        }
    }

    // Accepts 0..4; raises otherwise.
    aGalaxyStruct::TOwnerId NumberToRace(std::int32_t Value) {
        switch (Value) {
            case 0: return aGalaxyStruct::oiMaloc;
            case 1: return aGalaxyStruct::oiPeleng;
            case 2: return aGalaxyStruct::oiHuman;
            case 3: return aGalaxyStruct::oiFeyan;
            case 4: return aGalaxyStruct::oiGaal;
            default: pas::raise(pas::make_exception<pas::Exception>("Error in NumberToRace"_a));
        }
    }

    // Case-sensitive lookup; raises for an unknown name.
    std::uint8_t SysToReward(const std::u16string_view& Name) {
        if (Name == u"ForLiberationSystem"sv) {
            return aGalaxyStruct::atLiberation;
        } else if (Name == u"ForAccomplishment"sv) {
            return aGalaxyStruct::atAccomplishment;
        } else if (Name == u"ForSecretMission"sv) {
            return aGalaxyStruct::atSecretMission;
        } else if (Name == u"ForCowardice"sv) {
            return aGalaxyStruct::atCowardice;
        } else if (Name == u"ForPerfidy"sv) {
            return aGalaxyStruct::atPerfidy;
        } else if (Name == u"ForPlanetBattle"sv) {
            return aGalaxyStruct::atPlanetBattle;
        } else {
            GR_Main::RaiseWideMessage(u"Error in SysToReward"_wref.get());
            return aGalaxyStruct::atPerfidy;
        }
    }

    // Case-sensitive lookup among 14 ship types; raises for an unknown name.
    std::uint8_t SysToShipType(const std::u16string_view& Name) {
        std::uint8_t Kind{};
        for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(13); ++Kind) {
            if (pas::view(ShipTypeNames[Kind].Name) == Name) {
                return Kind;
            }
        }
        GR_Main::RaiseWideMessage(u"Error in SysToShipType"_wref.get());
        return 0;
    }

    // Maps owner IDs 0..5 and 7 to fixed RGB colors through CurrentPixelFormat; other values use magenta.
    std::uint32_t OwnerToFilmColor(aGalaxyStruct::TOwnerId OwnerId) {
        switch (OwnerId) {
            case aGalaxyStruct::oiMaloc: return GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
            case aGalaxyStruct::oiPeleng: return GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
            case aGalaxyStruct::oiHuman: return GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0x00000047, 0x000000ea);
            case aGalaxyStruct::oiFeyan: return GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0x00000093, 0x000000f1);
            case aGalaxyStruct::oiGaal: {
                return GR_Main::CurrentPixelFormat->PackRgbBytes(0x000000ed, 0x000000f7, 0x0000003e);
            }
            case aGalaxyStruct::oiDominator: {
                return GR_Main::CurrentPixelFormat->PackRgbBytes(0x00000061, 0x000000a7, 0x000000be);
            }
            case aGalaxyStruct::oiPirate: return GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
            default: return GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 255);
        }
    }

    std::uint32_t CustomFactionToFilmColor(pas::WideString Faction) {
        pas::WideString Text{};
        EC_BlockPar::TBlockParEC* Block = GR_Main::GameDataConfig->GetBlock(u"Race"sv)->FindBlock(u"Color"sv);
        if (Block != nullptr) {
            if (Block->CountParamsByPath(Faction) > 0) {
                Text = Block->GetParamByPathOrMarker(Faction);
                if (EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) >= 3) {
                    std::int32_t extractDigitsToIntW = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 2, u","sv)));
                    std::int32_t extractDigitsToIntW_2 = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv)));
                    std::int32_t extractDigitsToIntW_3 = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv)));
                    return GR_Main::CurrentPixelFormat->PackRgb(extractDigitsToIntW_3, extractDigitsToIntW_2, extractDigitsToIntW);
                }
            }
        }
        return aConst::OwnerToFilmColor(aGalaxyStruct::oiUninhabited);
    }

    // Race.PlanetIconNum lookup; returns -1 for an absent entry. Film owner codes offset a nonnegative result by eight.
    std::int32_t GetCustomFactionPlanetIconNumber(pas::WideString Faction) {
        EC_BlockPar::TBlockParEC* Block{};
        Block = GR_Main::GameDataConfig->GetBlock(u"Race"sv)->FindBlock(u"PlanetIconNum"sv);
        if (Block != nullptr && Block->CountParamsByPath(Faction) > 0) {
            return EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParamByPathOrMarker(Faction)));
        }
        return -1;
    }

    // Zero, Mini, Small, Average, Big, Huge map to 0..5; unknown tags map to zero.
    std::uint8_t SizeTagToLevel(const std::u16string_view& Tag) {
        if (Tag == u"Zero"sv) {
            return 0;
        } else if (Tag == u"Mini"sv) {
            return 1;
        } else if (Tag == u"Small"sv) {
            return 2;
        } else if (Tag == u"Average"sv) {
            return 3;
        } else if (Tag == u"Big"sv) {
            return 4;
        } else if (Tag == u"Huge"sv) {
            return 5;
        } else {
            return 0;
        }
    }

    // Seeded variation around a size bucket; unknown nonzero levels use the midpoint.
    std::int32_t GenerateValueForSizeLevel(std::uint8_t Level, std::int32_t Minimum, std::int32_t Maximum, std::uint8_t VariationPercent, std::uint32_t Seed) {
        std::int32_t Center{};
        std::int32_t Bound{};
        switch (Level) {
            case 0: return 0;
            case 1: Center = Minimum; break;
            case 2: Center = ((Minimum + Maximum) / 2 + Minimum) / 2; break;
            case 3: Center = (Minimum + Maximum) / 2; break;
            case 4: Center = ((Minimum + Maximum) / 2 + Maximum) / 2; break;
            case 5: Center = Maximum; break;
            default: Center = (Minimum + Maximum) / 2; break;
        }
        if (aMyFunction::SeededRandomUnitFloat(Seed + static_cast<std::uint32_t>(Center)) < 0.5L) {
            Bound = std::min<std::int64_t>(static_cast<std::int64_t>(Maximum), System::Round(pas::real_divide(Center, 1.0E+2L) * VariationPercent + Center));
            return aMyFunction::SeededRandomIntRange(Center, Bound, Seed);
        }
        Bound = std::max<std::int64_t>(static_cast<std::int64_t>(Minimum), System::Round(Center - pas::real_divide(Center, 1.0E+2L) * VariationPercent));
        return aMyFunction::SeededRandomIntRange(Bound, Center, Seed);
    }

    std::int32_t GetAverageItemSize(TItemType ItemType) {
        switch (ItemType) {
            case t_ArtefactHull: return 12;
            case t_ArtefactFuel: return 4;
            case t_ArtefactSpeed: return 12;
            case t_ArtefactPower: return 7;
            case t_ArtefactRadar: return 10;
            case t_ArtefactScaner: return 8;
            case t_ArtefactDroid: return 10;
            case t_ArtefactNano: return 3;
            case t_ArtefactHook: return 3;
            case t_ArtefactDef: return 12;
            case t_ArtefactAnalyzer: return 5;
            case t_ArtefactMiniExpl: return 10;
            case t_ArtefactAntigrav: return 20;
            case t_ArtefactTransmitter: return 3;
            case t_ArtefactBomb: return 5;
            case t_ArtefactTranclucator: return 50;
            case t_ArtDefToEnergy: return 5;
            case t_ArtEnergyPulse: return 8;
            case t_ArtEnergyDef: return 5;
            case t_ArtSplinter: return 10;
            case t_ArtDecelerate: return 5;
            case t_ArtMissileDef: return 6;
            case t_ArtForsage: return 6;
            case t_ArtWeaponToSpeed: return 7;
            case t_ArtGiperJump: return 5;
            case t_ArtBlackHole: return 3;
            case t_ArtDefToArms1: return 9;
            case t_ArtDefToArms2: return 7;
            case t_ArtArtefactor: return 3;
            case t_ArtBio: return 2;
            case t_ArtPDTurret: return 15;
            case t_ArtFastRacks: return 10;
            case t_Hull: return HullBaseSize;
            case t_FuelTanks: return FuelTanksBaseSize;
            case t_Engine: return EngineBaseSize;
            case t_Radar: return RadarBaseSize;
            case t_Scaner: return ScannerBaseSize;
            case t_RepairRobot: return RepairRobotBaseSize;
            case t_CargoHook: return CargoHookBaseSize;
            case t_DefGenerator: return DefGeneratorBaseSize;
            default: {
                if (pas::in_range(ItemType, static_cast<std::int32_t>(t_Weapon1), static_cast<std::int32_t>(t_CustomWeapon))) {
                    return WeaponInfos[ItemType].AverageSize;
                }
                pas::make_exception<pas::Exception>("Error ItemAverageSize"_a);
                return 0;
            }
        }
    }

    std::uint8_t PickRandomItemType(TItemTypeSelection Mask) {
        std::uint8_t ItemType{};
        std::int32_t Count = 0;
        for (ItemType = t_Food; ItemType <= t_UselessCountableItem; ++ItemType) {
            if (pas::contains(Mask, ItemType)) {
                ++Count;
            }
        }
        Count = aMyFunction::RandomIntRange(1, Count);
        for (ItemType = t_Food; ItemType <= t_UselessCountableItem; ++ItemType) {
            if (pas::contains(Mask, ItemType)) {
                --Count;
                if (Count == 0) {
                    break;
                }
            }
        }
        return ItemType;
    }

    // Selects a set bit among 0..75 while advancing Seed; an empty mask returns 76.
    std::uint8_t PickRandomItemTypeFromSeed(TItemTypeSelection Mask, std::uint32_t& Seed) {
        std::uint8_t ItemType{};
        std::int32_t Count = 0;
        for (ItemType = t_Food; ItemType <= t_UselessCountableItem; ++ItemType) {
            if (pas::contains(Mask, ItemType)) {
                ++Count;
            }
        }
        Count = aMyFunction::NextRandomIntRange(1, Count, Seed);
        for (ItemType = t_Food; ItemType <= t_UselessCountableItem; ++ItemType) {
            if (pas::contains(Mask, ItemType)) {
                --Count;
                if (Count == 0) {
                    break;
                }
            }
        }
        return ItemType;
    }

    // Copies the ten-byte mask, then counts bits 0..75; ignores storage bits 76..79.
    std::int32_t CountItemTypesInMask(TItemTypeSelection Mask) {
        std::uint8_t ItemType{};
        std::int32_t Count = 0;
        for (ItemType = t_Food; ItemType <= t_UselessCountableItem; ++ItemType) {
            if (pas::contains(Mask, ItemType)) {
                ++Count;
            }
        }
        return Count;
    }

    // One-based selected-bit index among types 0..75; returns zero if no index matches.
    std::uint8_t GetItemTypeFromMask(TItemTypeSelection Mask, std::int32_t Index) {
        std::uint8_t ItemType{};
        std::int32_t Count = 0;
        std::uint8_t Result = 0;
        for (ItemType = t_Food; ItemType <= t_UselessCountableItem; ++ItemType) {
            if (pas::contains(Mask, ItemType)) {
                ++Count;
            }
            if (Count == Index) {
                return ItemType;
            }
        }
        return Result;
    }

    // Only Coalition manufacturers are eligible.
    aGalaxyStruct::TOwnerId PickRandomEquipmentOwner(std::uint32_t RandomValue) {
        return static_cast<aGalaxyStruct::TOwnerId>(aMyFunction::SeededRandomIntRange(0, 4, RandomValue));
    }

    pas::WideString LookupNamedColorTag(pas::WideString Name) {
        pas::WideString Text{};
        EC_BlockPar::TBlockParEC* Block = GR_Main::GameDataConfig->GetBlock(u"Race"sv)->FindBlock(u"Color"sv);
        if (Block != nullptr) {
            if (Block->CountParamsByPath(Name) > 0) {
                Text = Block->GetParam(pas::view(Name));
                if (EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) >= 3) {
                    return pas::concat_wide({u"<color=", Text, u">"});
                }
            }
        }
        return u"<color=255,000,255>"_w;
    }

    // Repeated Path values are joined with CRLF; no separator precedes an empty accumulator.
    // Missing paths return empty and may create intermediate blocks.
    // Both lookups expand <br>, <ll> and <Player>; the player's name is highlighted.
    // LocalizedText leaves <clr>/<clrEnd> intact; LocalizedColorText expands them.
    pas::WideString LocalizedText(const pas::WideString& Path) {
        pas::WideString Result{};
        std::int32_t I{};
        Result = pas::WideString();
        std::int32_t Count = GR_Main::LanguageDataConfig->CountParamsByPath(Path);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            if (Result != u"") {
                Result = pas::concat_wide({Result, u"\r\n"});
            }
            Result = pas::concat_wide_reverse({GR_Main::LanguageDataConfig->GetParamByPath(pas::concat_wide({Path, u":", pas::wide_int_to_str(I)})), Result});
        }
        if (EC_Str::FindTextPosW(u"<"_wref.get(), Result) > 0) {
            Result = EC_Str::ReplaceAllWideString(Result, u"<br>"_wref.get(), u"\r\n"sv);
            Result = EC_Str::ReplaceAllWideString(Result, u"<ll>"_wref.get(), u"\r\n \r\n"sv);
            if (aPlayer::GetPlayer() != nullptr) {
                return EC_Str::ReplaceAllWideString(Result, u"<Player>"_wref.get(), pas::view(pas::concat_wide({u"<color=255,240,100>", aPlayer::GetPlayer()->Name, u"</color>"})));
            }
        }
        return Result;
    }

    // Also expands <clr> and <clrEnd> to yellow opening and closing color tags.
    pas::WideString LocalizedColorText(const pas::WideString& Path) {
        pas::WideString Result{};
        std::int32_t I{};
        Result = pas::WideString();
        std::int32_t Count = GR_Main::LanguageDataConfig->CountParamsByPath(Path);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            if (Result != u"") {
                Result = pas::concat_wide({Result, u"\r\n"});
            }
            Result = pas::concat_wide_reverse({GR_Main::LanguageDataConfig->GetParamByPath(pas::concat_wide({Path, u":", pas::wide_int_to_str(I)})), Result});
        }
        if (EC_Str::FindTextPosW(u"<"_wref.get(), Result) > 0) {
            Result = EC_Str::ReplaceAllWideString(Result, u"<br>"_wref.get(), u"\r\n"sv);
            Result = EC_Str::ReplaceAllWideString(Result, u"<ll>"_wref.get(), u"\r\n \r\n"sv);
            if (aPlayer::GetPlayer() != nullptr) {
                Result = EC_Str::ReplaceAllWideString(Result, u"<Player>"_wref.get(), pas::view(pas::concat_wide({u"<color=255,240,100>", aPlayer::GetPlayer()->Name, u"</color>"})));
            }
            Result = EC_Str::ReplaceAllWideString(Result, u"<clr>"_wref.get(), u"<color=255,240,100>"sv);
            return EC_Str::ReplaceAllWideString(Result, u"<clrEnd>"_wref.get(), u"</color>"sv);
        }
        return Result;
    }

    // Expands <br>, <ll>, <Player>, <clr> and <clrEnd>; leaves <Player> intact when no player exists.
    void ExpandLocalizedTextMarkup(pas::WideString& Text) {
        if (EC_Str::FindTextPosW(u"<"_wref.get(), Text) > 0) {
            Text = EC_Str::ReplaceAllWideString(Text, u"<br>"_wref.get(), u"\r\n"sv);
            Text = EC_Str::ReplaceAllWideString(Text, u"<ll>"_wref.get(), u"\r\n \r\n"sv);
            if (aPlayer::GetPlayer() != nullptr) {
                Text = EC_Str::ReplaceAllWideString(Text, u"<Player>"_wref.get(), pas::view(pas::concat_wide({u"<color=255,240,100>", aPlayer::GetPlayer()->Name, u"</color>"})));
            }
            Text = EC_Str::ReplaceAllWideString(Text, u"<clr>"_wref.get(), u"<color=255,240,100>"sv);
            Text = EC_Str::ReplaceAllWideString(Text, u"<clrEnd>"_wref.get(), u"</color>"sv);
        }
    }

    // Expands markup, trims outer whitespace, then applies LocalizedTextLinePrefix to the first line and after each CRLF.
    void ExpandLocalizedTextMarkupAndPrefixLines(pas::WideString& Text) {
        if (EC_Str::FindTextPosW(u"<"_wref.get(), Text) > 0) {
            Text = EC_Str::ReplaceAllWideString(Text, u"<br>"_wref.get(), u"\r\n"sv);
            Text = EC_Str::ReplaceAllWideString(Text, u"<ll>"_wref.get(), u"\r\n \r\n"sv);
            if (aPlayer::GetPlayer() != nullptr) {
                Text = EC_Str::ReplaceAllWideString(Text, u"<Player>"_wref.get(), pas::view(pas::concat_wide({u"<color=255,240,100>", aPlayer::GetPlayer()->Name, u"</color>"})));
            }
            Text = EC_Str::ReplaceAllWideString(Text, u"<clr>"_wref.get(), u"<color=255,240,100>"sv);
            Text = EC_Str::ReplaceAllWideString(Text, u"<clrEnd>"_wref.get(), u"</color>"sv);
        }
        Text = pas::concat_wide_reverse({EC_Str::TrimWideString(Text), LocalizedTextLinePrefix});
        Text = EC_Str::ReplaceAllWideString(Text, u"\r\n"_wref.get(), pas::view(pas::concat_wide({u"\r\n", LocalizedTextLinePrefix})));
    }

    // Collects at most ten nonempty variants: Path, then contiguous numeric suffixes
    // starting at 1 (0 when the base lookup is empty). A missing suffix prevents reaching later ones.
    // Selection uses (CurrentTurn + SeedOffset) div 10, except in chaotic mode. No variants produces an unavailable-text diagnostic.
    pas::WideString PickLocalizedTextVariant(const pas::WideString& Path, std::int32_t SeedOffset) {
        std::int32_t Count{};
        pas::Array<pas::WideString, 0, 9> Variants{};
        Count = 0;
        Variants[Count] = aConst::LocalizedColorText(Path);
        if (Variants[Count] != u"") {
            ++Count;
        }
        std::int32_t I = 1;
        do {
            Variants[Count] = aConst::LocalizedColorText(pas::concat_wide({Path, pas::wide_int_to_str(Count)}));
            if (Variants[Count] != u"") {
                ++Count;
            }
            ++I;
        } while (!(I > 9));
        if (Count == 0) {
            return pas::concat_wide({u"String: ", aMyFunction::WrapTextInColor(pas::view(Path), u"<color=255,240,100>"sv), u" is unavailable"});
        } else if (Count == 1) {
            return Variants[0];
        } else {
            Count = aMyFunction::SeededRandomIntRange(0, Count - 1, (aGalaxy::Galaxy->CurrentTurn + SeedOffset) / 10);
            return Variants[Count];
        }
    }

    // Buckets 0..100; out-of-range values map to normal.
    aGalaxyStruct::TRelationLevel RelationValueToLevel(std::uint8_t Value) {
        std::uint8_t cpp_case = Value;
        if (cpp_case >= 0 && cpp_case <= 9) {
            return aGalaxyStruct::rlHostile;
        } else if (cpp_case >= 10 && cpp_case <= 29) {
            return aGalaxyStruct::rlBad;
        } else if (cpp_case >= 30 && cpp_case <= 59) {
            return aGalaxyStruct::rlNormal;
        } else if (cpp_case >= 60 && cpp_case <= 79) {
            return aGalaxyStruct::rlGood;
        } else if (cpp_case >= 80 && cpp_case <= 100) {
            return aGalaxyStruct::rlExcellent;
        } else {
            return aGalaxyStruct::rlNormal;
        }
    }

    pas::WideString GetFactionEmblemPath(pas::WideString Faction) {
        return GR_Main::GameDataConfig->GetParamByPathOrMarker(pas::concat_wide({u"Race.Emblem.2", Faction}));
    }

    // First matching template; -1 when absent.
    std::int32_t FindMicroModuleTemplateByCustomTag(const std::u16string_view& CustomTag) {
        std::int32_t I{};
        {
            const std::int32_t cpp_last = MicroModuleTemplates.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (pas::view(MicroModuleTemplates[I].CustomTag) == CustomTag) {
                        return I;
                    }
                }
            }
        }
        return -1;
    }

} // namespace aConst
