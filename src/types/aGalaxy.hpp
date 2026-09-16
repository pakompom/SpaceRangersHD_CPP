#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GI_MessageLoop {
    struct TMessageLoopGI;

    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aGalaxy {
    struct TConstellationBoundaryRaySample;

    struct TConstellationStarLink;

    struct TJumpGateEntry;

    struct TMapLineSegment;

    struct TMovingDropItemEntry;

    struct TStarCombatEvent;

} // namespace aGalaxy

namespace aMyFunction {
    struct TObjectList;

} // namespace aMyFunction

namespace aPath {
    struct TSPath;

} // namespace aPath

namespace aVector {
    struct TPolygon2D;

} // namespace aVector

namespace aGalaxy {
    struct TInterfaceStateOverride;

    struct TInterfaceTextOverride;

    struct TInterfaceImageOverride;

    struct TInterfacePosOverride;

    struct TInterfaceSizeOverride;

    struct TStoredItem;

    struct TGalaxy;

    struct TConstellation;

    struct THole;

    struct TCustomSystemInfo;

    struct TStar;

    struct TStarDistanceEntry;

    struct TSpaceBackgroundEntry;

    struct TDominatorResearchEntry;

    using TShipPopulationCounts = pas::Array<std::int32_t, 0, 13>;

    #pragma pack(push, 1)
    struct TDominatorResearchEntry {
        float Progress;
        std::int32_t Material;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TSpaceBackgroundEntry {
        std::int32_t ImageIndex;
        std::uint8_t cpp_padding[4];
        EC_Struct::TVector3D OrbitCenter;
        EC_Struct::TVector3D Position;
        EC_Struct::TVector3D Unknown38;
        double OrbitStepDegrees;
        std::int32_t FrameIndex;
        std::uint8_t cpp_padding_2[4];
    };
    #pragma pack(pop)

    using PJumpGateEntry = TJumpGateEntry*;

    using TControlPercent = std::uint8_t;

    using TDominatorSeriesSet = pas::Set<0, 2>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGalaxy : EC_Struct::TObjectEx {
        PAS_CLASS_META(TGalaxy, EC_Struct::TObjectEx, "TGalaxy", 476)
        void p_destroy() override;
        void InitializeCampaignState();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void SaveEditableState();
        void ApplyEditableState();
        static void RunConfigOnStartHandlers();
        static void RunConfigOnLoadHandlers();
        static void RunConfigOnSaveHandlers();
        void ReapplyInterfaceOverrides();
        void BindScriptImports();
        void NextDay();
        void CompleteDay(std::uint8_t UnusedRecordFilm);
        void TransferShipsInTransit();
        void RebuildStarDistances();
        void RefreshAllShipDerivedState();
        TConstellation* IdToConstellation(std::uint32_t Id);
        TStar* IdToStar(std::uint32_t Id);
        THole* IdToHole(std::uint32_t Id);
        void* IdToPlanet(std::uint32_t Id, std::uint8_t RaiseIfMissing);
        void* IdToShip(std::uint32_t Id, std::uint8_t RaiseIfMissing);
        void* IdToItem(std::uint32_t Id, std::uint8_t RaiseIfMissing);
        void* IdToAsteroid(std::uint32_t Id);
        void* IdToMissile(std::uint32_t Id);
        std::uint8_t ContainsShipReference(void* Ship);
        std::uint8_t ContainsPlanetReference(void* Planet);
        void ClearJumpGates();
        PJumpGateEntry CreateJumpGate(std::uint8_t WithEffect);
        THole* FindHoleInStarByKind(TStar* Star, std::int32_t HoleKind);
        void ReleaseItemGraphics();
        void GenerateSpaceBackground(std::int32_t BackgroundIndex);
        void EnableDominatorSurfaces();
        void DisableDominatorSurfaces();
        void XorProtectedState(std::int32_t Seed);
        void ObfuscateProtectedState();
        void RestoreProtectedState();
        static std::uint32_t ComputeIntegrityChecksum(std::int32_t Mode);
        void PrimeIntegrityChecksum(std::int32_t StatusCode);
        void PrimeIntegrityChecksum1(std::int32_t StatusCode);
        void PrimeIntegrityChecksum2(std::int32_t StatusCode);
        void CheckIntegrityChecksum(std::int32_t ErrorCode);
        void CheckIntegrityChecksumAndSetStatus(std::int32_t StatusCode);
        void CheckIntegrityChecksum1(std::int32_t ErrorCode);
        void CheckIntegrityChecksum2(std::int32_t ErrorCode);
        static void ClearIntegrityStatus();
        static void AppendIntegritySnapshot();
        std::uint8_t HasVisibleScoreModFlags();
        std::int32_t GetCheatPoints();
        void SetCheatPoints(std::int32_t Value);
        std::int32_t FindConstellationIndexForStar(TStar* Star);
        void InitializeConstellationDistanceTiers();
        void BuildConstellationOutlineJunctions();
        std::uint8_t ShouldKeepConstellationOutlineVertex(EC_Struct::TPointF Point);
        void SimplifyConstellationOutline(std::int32_t ConstellationIndex);
        std::uint8_t BuildConstellationStarGraphs();
        void BuildConstellationPolygonsAndAdjacency(aVector::TPolygon2D* WorkingPolygon);
        void GenerateGalaxyLayout(std::uint8_t PlayerRace);
        void HideSpecialConstellation();
        std::int32_t CountVisibleConstellationsWithBoundaryPoints(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint);
        std::int32_t CountEligibleRangers();
        void RefreshRangerWealthStats();
        void RefreshRangerStrengthStats();
        void RefreshRangerRatingPlaces();
        void* FindStrongestRanger();
        void* FindWealthiestRanger();
        std::int32_t CountFactionStars(std::uint8_t Faction);
        TControlPercent GetFactionControlPercent(std::uint8_t Faction);
        static float GetDominatorSeriesControlShare(aGalaxyStruct::TDominatorSeries Series);
        std::int32_t CountStarsInBattle();
        void AssignTextQuestsToPlanets();
        static std::uint8_t HasPlayerQuestHistory(aGalaxyStruct::TQuestType QuestType, std::uint16_t QuestNumber);
        double TurnToDateTime(std::int32_t Turn);
        pas::WideString FormatTurnDate(std::int32_t Turn);
        void AddPlanetNewsWithPlayerBubble(std::uint8_t NewsType, pas::WideString Text);
        void AddPlanetNews(std::uint8_t NewsType, pas::WideString Text);
        std::int32_t CountPlanetNewsByType(std::uint8_t NewsType);
        void PrunePlanetNews();
        static void CreateDominatorSpawnProxy(TStar* Star);
        void UpdateConstellationMilitaryStats();
        std::uint8_t RefreshTechLevel();
        void CancelEnemyJumpsToStar(TStar* Star);
        TStar* SelectStarForLiberationAttack(TStar* Origin, aGalaxyStruct::TStarFaction FriendlyFaction);
        void ComputeGlobalGoodsPriceBands();
        static std::uint8_t GetGoodsPricePercent(std::uint8_t GoodsType, std::int32_t Price);
        std::int32_t ScaleGoodsPriceByGalaxyAge(std::int32_t BaseValue);
        std::int32_t ScaleGoodsStockByGalaxyAge(std::int32_t BaseValue);
        std::int32_t ScaleIntByTechLevel(std::int32_t AtLevelTwo, std::int32_t AtLevelSeven);
        float InterpolateSingleByTechLevel(float AtLevelTwo, float AtLevelSeven);
        aConst::PWeaponInfo SelectWeaponInfo(std::uint32_t Seed, aGalaxyStruct::TWeaponAvailabilityMask AvailabilityMask, std::uint8_t MaximumTechLevel, std::uint8_t MinimumTechLevel);
        static std::int32_t SelectMicroModule(std::uint8_t MinimumPriority, std::uint8_t MaximumPriority, std::uint32_t Seed, pas::Object* Context);
        static std::int32_t SelectMicroModuleForEquipment(std::uint8_t MinimumPriority, std::uint8_t MaximumPriority, std::uint32_t Seed, pas::Object* Context, void* Item);
        std::int32_t SelectHullSeries(std::uint8_t OwnerId, std::uint8_t HullType, std::uint8_t MinimumRarity, std::uint8_t MaximumRarity);
        std::uint8_t IsDominatorSeriesUnresolved(aGalaxyStruct::TDominatorSeries Series);
        std::uint8_t HasUnresolvedDominatorSeries(TDominatorSeriesSet Series);
        static void ProcessPlayerSatelliteExploration();
        std::int32_t CountExistingSatellites();
        std::int32_t ComputeScaledMiniMoney(std::uint8_t ScaleIndex);
        std::int32_t ComputeScaledSmallMoney(std::uint8_t ScaleIndex);
        std::int32_t ComputeScaledAverageMoney(std::uint8_t ScaleIndex);
        std::int32_t ComputeScaledBigMoney(std::uint8_t ScaleIndex);
        std::int32_t ComputeScaledHugeMoney(std::uint8_t ScaleIndex);
        static std::int32_t ResolveMoneySizeTag(pas::WideString Tag, std::uint8_t ScaleIndex);
        static std::int32_t GetMiniGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetSmallGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetAverageGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetBigGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetHugeGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetGoodsQuantityBySize(std::uint8_t Size, std::uint8_t GoodsType);
        std::uint8_t ClassifyGoodsQuantity(std::int32_t Quantity, std::uint8_t GoodsType);
        static std::int32_t GetMinimumGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetLowGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetAverageGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetHighGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetMaximumGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetGoodsPriceByLevel(std::uint8_t Level, std::uint8_t GoodsType);
        std::uint8_t ClassifyGoodsPrice(std::int32_t Price, std::uint8_t GoodsType);
        void ProcessStationSpawning();
        void ReplenishStationType(aGalaxyStruct::TStationType StationType);
        void ProcessDominatorResearchProgress();
        std::uint8_t IsDominatorResearchComplete(TDominatorSeriesSet Series);
        float GetDominatorResearchRate(aGalaxyStruct::TDominatorSeries Series);
        std::uint8_t GetDominatorResearchEfficiency(aGalaxyStruct::TDominatorSeries Series);
        void* FindStationByTypeAndIndex(std::int32_t Index, aGalaxyStruct::TStationType StationType);
        void TryAwardDepositPrize();
        void ProcessBankDebtAndDeposits();
        void ProcessRangerCenterNewYearEvent();
        std::uint8_t TryCreateLiberationGroup();
        std::uint8_t TryDispatchMilitaryBaseToEnemyStar();
        static void* FindMilitaryBaseInTransit();
        static std::uint8_t HasMilitaryBaseAssignedToStar(TStar* Star);
        std::uint8_t HasLiberationGroupTargetingStar(TStar* Star);
        void AssignSpecialStationService();
        static void ApplyWingmanLeadershipPenalty();
        void ProcessCoalitionDefeat();
        void ComputeRangerSpawnQuotas();
        void PruneExpiredGalaxyEvents();
        static float GetCoalitionToPirateSystemRatio();
        std::int32_t GetEffectiveDifficultyLevel();
        std::uint8_t GetDifficultyTierIndex();
        float InterpolateDifficulty(std::int32_t Level, float AtZero, float AtEight, float AtSixteen, float AtTwentyFour);
        float ScaleDifficultyExponentially(std::int32_t Level, float BaseValue, float FactorPerEightLevels);
        float GetDominatorBossHullScale();
        float GetDominatorKillExperienceScale();
        std::int32_t GetTurnsBetweenLiberationGroups();
        std::int32_t GetInitialDominatorControlPercent();
        std::int32_t GetDominatorAggressionLevel();
        std::int32_t GetDominatorSpawnLevel();
        std::int32_t GetPirateAggressionLevel();
        std::uint8_t IsChaoticRandomEnabled();
        std::uint8_t AreStationsNearStarsEnabled();
        std::uint8_t IsFullStationTargetingEnabled();
        std::uint8_t IsEquipmentKnowledgeUnrestricted();
        float GetAsteroidModifier();
        float GetStarDamageDifficultyScale();
        std::uint8_t AreSpecialShipsEnabled();
        float GetMicroModuleOfferRollThresholdPercent();
        float GetNodeDropModifier();
        float GetArcadeDropValueModifier();
        float GetDropValueModifier();
        std::int32_t GetAgriculturalPlanetWeight();
        std::int32_t GetMixedPlanetWeight();
        std::int32_t GetIndustrialPlanetWeight();
        std::uint8_t IsZeroStartingExperienceEnabled();
        std::int32_t GetExtraRangerCount();
        std::uint8_t IsArcadeBattleRoyaleEnabled();
        float GetArcadeHitpointsModifier();
        float GetArcadeDamageModifier();
        std::uint8_t AreDominatorRacialWeaponsEnabled();
        std::int32_t GetAIJunkToleranceLevel();
        std::uint8_t AreMaxRangeMissilesEnabled();
        std::uint8_t IsOldHyperspaceEnabled();
        std::uint8_t ArePirateNodesEnabled();
        std::uint8_t IsAIShoppingEnabled();
        std::uint8_t IsStationShopUpdateEnabled();
        std::uint8_t AreDuplicateArtefactsEnabled();
        std::uint8_t GetHullGrowthMod();
        std::uint8_t IsArcadeEquipmentChangeEnabled();
        std::uint8_t IsOldSpeedCalculationEnabled();
        std::uint8_t AreOldMissileBonusesEnabled();
        static void ShowLocalizedWarning(pas::WideString TextKey);
        void StoreItem(pas::WideString Name, pas::Object* Item);
        pas::Object* GetStoredItem(pas::WideString Name, std::uint8_t Remove);
        aConst::PWeaponInfo GetOrCreateCustomWeaponInfo(pas::WideString Name);
        aConst::PWeaponInfo RequireCustomWeaponInfo(pas::WideString Name);
        std::uint8_t CanRecordAchievements();
        std::uint32_t NextConstellationId;
        std::uint32_t NextStarId;
        std::uint32_t NextHoleId;
        std::uint32_t NextPlanetId;
        std::uint32_t NextSputnikId;
        std::uint32_t NextAsteroidId;
        std::uint32_t NextShipId;
        std::uint32_t NextItemId;
        std::uint32_t NextMissileId;
        std::int32_t PlayerRangerIndex;
        aMyFunction::TObjectList* Stars;
        aMyFunction::TObjectList* Holes;
        aMyFunction::TObjectList* StoredItems;
        pas::List* Planets;
        pas::List* Rangers;
        std::int32_t PirateCount;
        std::int32_t PirateClanCount;
        std::int32_t TransportCount;
        std::int32_t CurrentTurn;
        aGalaxyStruct::TGalaxyDifficultyLevels DifficultyLevels;
        std::uint32_t GenerationSeed;
        std::uint32_t RandomState;
        std::int32_t AverageRangerCapital;
        std::int32_t MaxRangerWealth;
        float AverageRangerStrength;
        float BestRangerStrength;
        pas::Object* StrongestRanger;
        pas::Object* WealthiestRanger;
        pas::Array<pas::Object*, 0, 2> EminentCareerShips;
        TShipPopulationCounts ShipTypeCounts;
        std::uint32_t NextPlanetNewsId;
        pas::List* PlanetNews;
        pas::List* CustomWeaponTypes;
        TStar* KellerTargetStar;
        std::int32_t KellerMissionState;
        float ChecksumScalarD0;
        pas::Array<TDominatorResearchEntry, 0, 2> DominatorResearch;
        float ChecksumScalarEC;
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[3];
        pas::Array<std::int32_t, 0, 2> WarDeltaWin;
        pas::Array<std::int32_t, 0, 4> RangerSpawnQuotas;
        std::int32_t TerronWeaponLockTurn;
        std::int32_t TerronGrowLockTurn;
        std::int32_t TerronLandingLockTurn;
        std::int32_t TerronToStarTurn;
        std::int32_t KellerLeaveTurn;
        std::uint32_t KellerResearchTargetStarId;
        std::uint32_t BlazerLandingPlanetId;
        std::int32_t BlazerSelfDestructTurn;
        std::int32_t TerronSeriesResolvedTurn;
        std::int32_t KellerSeriesResolvedTurn;
        std::int32_t BlazerSeriesResolvedTurn;
        std::int32_t PirateWinTurn;
        std::int32_t PirateWinType;
        std::int32_t CoalitionDefeatedTurn;
        std::uint8_t GraphDominatorSurfacesEnabled;
        std::uint8_t SpaceEffectKind;
        std::uint8_t cpp_padding_2[2];
        pas::List* Scripts;
        pas::List* LiberationGroups;
        pas::List* JumpGates;
        pas::List* ShipsInTransit;
        std::int32_t ConstellationCount;
        aMyFunction::TObjectList* Constellations;
        pas::List* ConstellationOutlineJunctions;
        pas::DynArray<TSpaceBackgroundEntry> SpaceBackgroundEntries;
        std::int32_t SaveCount;
        std::int32_t LoadCount;
        std::int32_t PendingEquipmentPurchasePrice;
        std::uint8_t IronWill;
        std::uint8_t DominatorModLevel;
        std::uint8_t TechnicModEnabled;
        std::uint8_t AmmoModEnabled;
        std::uint8_t GodModEnabled;
        std::uint8_t UltraScanModEnabled;
        std::uint8_t StasisModEnabled;
        std::uint8_t CampaignFlag183;
        pas::WideString FinalizationNameEncoded;
        aGalaxyStruct::TGalaxyCustomRules CustomRules;
        std::uint8_t cpp_padding_3[1];
        std::int32_t NextSpecialStationServiceTurn;
        aMyFunction::TObjectList* GalaxyEvents;
        aMyFunction::TObjectList* InterfaceStateOverrides;
        aMyFunction::TObjectList* InterfaceTextOverrides;
        aMyFunction::TObjectList* InterfaceImageOverrides;
        aMyFunction::TObjectList* InterfacePositionOverrides;
        aMyFunction::TObjectList* InterfaceSizeOverrides;
        pas::List* LoadedShips;
        std::uint8_t ScoreScreenDismissed;
        std::uint8_t Destroying;
        std::uint8_t cpp_padding_4[2];
        std::uint32_t GenerationMachineHash;
        std::uint8_t SpecialSimulationMode;
        std::uint8_t CheatsDisabled;
        std::uint8_t cpp_padding_5[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TStarDistanceEntry {
        std::int32_t Distance;
        TStar* Star;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStar : pas::Object {
        PAS_CLASS_META(TStar, pas::Object, "TStar", 276)
        void p_destroy() override;
        void GenerateSystemContents(std::uint8_t TerronSystem);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy);
        void ResolveLoadedReferences(TGalaxy* Galaxy);
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        void PruneWeaponTargetsAfterTurn();
        void MarkConnectedCombatEvents(pas::List* Events, pas::Object* Target, std::int32_t Group);
        std::int32_t DropMinerals(std::int32_t Quantity, EC_Struct::TPointF Position, std::uint32_t Seed);
        void ProcessPlayerAsteroidKill(std::int32_t MineralValue, EC_Struct::TPointF Position, std::uint32_t AsteroidId);
        void ClearTargetReferences(pas::Object* Target);
        void ClearShipReferences(void* Ship);
        void ClearItemReferences(void* Item);
        void ClearCombatEventWeaponReferences(void* Weapon);
        void PrepareNextDay();
        void HandleObjectLeavingStar(pas::Object* Obj);
        void AvoidShipPathCollisions();
        void RebuildShipMovementPaths();
        void OpenSpaceScene(GI_Panel::TPanelGI* MapPanel, GI_MessageLoop::TObjectGI* Minimap, GI_MessageLoop::TMessageLoopGI* Screen);
        void RefreshSpaceObjectPositions();
        void QueueSpaceImageLoads(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);
        void QueueHyperspaceShipImageLoads(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);
        pas::WideString GetBackgroundImagePath(std::int32_t& Size);
        void ProcessItemScripts(std::int32_t TurnPhase);
        void RefreshDominatorSeries();
        void RefreshDerivedStats();
        void GetControlPresence(std::uint8_t& PlayerPartyPresent, std::uint8_t& CoalitionPresent, std::uint8_t& DominatorsPresent, std::uint8_t& PiratesPresent, std::uint8_t& CustomPresent);
        void UpdateControlFaction();
        void ResetControlFaction();
        void RefreshMapDiameterAndStats();
        void RefreshMovementStepParameters();
        std::int32_t ComputeMapDiameter();
        std::int32_t CountPlanetsByOwner(std::uint8_t OwnerId);
        std::int32_t CountDistinctInhabitedPlanetOwners();
        void* FindFirstInhabitedPlanet();
        void* SelectRandomInhabitedPlanet();
        void* FindFastestResearchPlanet();
        void RefreshShipTypeCounts();
        std::int32_t CountEligibleRangersInSpace();
        std::int32_t CountShipsByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask);
        std::int32_t CountDominatorForces(aGalaxyStruct::TDominatorSeries Series, std::uint8_t ExcludeAbsoluteOrders, std::uint8_t OtherSeries, pas::Extended& Strength);
        std::int32_t CountStandardDominatorsOfLocalSeries();
        std::int32_t CountPirateForces(std::uint8_t ExcludeAbsoluteOrders, pas::Extended& Strength, std::uint8_t IncludeClanVariants, std::uint8_t IncludeIndependent);
        std::int32_t CountCustomFactionForces(std::uint8_t ExcludeAbsoluteOrders, pas::WideString& Faction, pas::Extended& Strength);
        std::int32_t CountOtherCustomFactionForces(std::uint8_t ExcludeAbsoluteOrders, pas::WideString& Faction, pas::Extended& Strength);
        std::int32_t CountPirateShips(std::uint8_t IncludeOutsideStarSpace);
        std::int32_t CountForcesByOwnerGroups(pas::Extended& Strength, std::uint8_t IncludeCoalition, std::uint8_t IncludeDominators, std::uint8_t IncludePirates, std::uint8_t IncludeCustom);
        std::uint8_t CountRatedRangersByCareerMask(aGalaxyStruct::TRangerCareerSet CareerMask);
        pas::WideString GetRangerNamesByCareerMask(aGalaxyStruct::TRangerCareerSet CareerMask);
        std::uint8_t IsConstellationVisible();
        float GetCachedFactionStrength(std::uint8_t FactionGroup);
        float SumBestRangerRelativeStrength(aGalaxyStruct::TShipTypeMask ShipTypeMask);
        void RebuildStarDistances(TGalaxy* Galaxy);
        std::uint8_t HasHostilePresenceForScriptBinding();
        TStar* FindNearestStarByFaction(aGalaxyStruct::TStarFaction Faction, std::uint8_t InBattle);
        EC_Struct::TPointF GetBoundaryPointTowardStar(TStar* Star);
        std::uint8_t HasLiberationGroupOrder();
        void TryGenerateSystemNews();
        void NextDay(std::uint8_t RecordFilm);
        std::uint32_t Id;
        std::uint32_t GenerationSeed;
        std::uint32_t RandomState;
        pas::WideString Name;
        EC_Struct::TPointF Position;
        std::uint16_t SystemRadius;
        std::uint8_t cpp_padding[2];
        std::int32_t MapDiameter;
        aMyFunction::TObjectList* Planets;
        aMyFunction::TObjectList* Asteroids;
        aMyFunction::TObjectList* Ships;
        aMyFunction::TObjectList* Items;
        pas::List* MovingDropItems;
        aMyFunction::TObjectList* Missiles;
        pas::WideString SystemProcessName;
        aGalaxyStruct::TStarStatus Status;
        float SafeRadius;
        float DamageRadius;
        std::int32_t Reserved64;
        std::int32_t Radius;
        SE_Space::TObjectSE* Graphic;
        std::int32_t DaysSincePlayerVisit;
        std::int32_t DaysSinceLastNpcShipSpawn;
        std::int32_t PlayerPresenceLevel;
        std::int32_t BackgroundImage;
        std::uint8_t Flag80;
        std::uint8_t cpp_padding_2[3];
        std::int32_t LastDominatorPresenceTurn;
        std::int32_t LastPiratePresenceTurn;
        std::int32_t LastLiberationRewardsTurn;
        std::uint8_t LiberationRewardsPending;
        std::uint8_t cpp_padding_3[3];
        pas::DynArray<TStarDistanceEntry> StarDistances;
        pas::Array<std::int32_t, 0, 13> ShipTypeCounts;
        TConstellation* Constellation;
        std::uint16_t ConstellationGraphIndex;
        std::uint8_t cpp_padding_4[2];
        std::uint8_t NoComeKling;
        std::uint8_t cpp_padding_5[3];
        pas::Object* Dominion;
        pas::WideString MapLabel;
        aMyFunction::TObjectList* CustomSystemInfos;
        std::int32_t CurrentStepIndex;
        std::int32_t SimulationStepCount;
        std::uint8_t RecordingTurnFilm;
        std::uint8_t PlayerCombatOccurred;
        std::uint8_t InterruptLongTravel;
        std::uint8_t KeepFilmRunning;
        pas::List* CombatEvents;
        pas::List* PendingFilmObjectRemovals;
        pas::List* ReferencedItems;
        aPath::TSPath* PlayerFilmPath;
        std::int32_t MovementStepCount;
        pas::Extended MovementStepScale;
        std::uint8_t cpp_padding_6[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TConstellation : EC_Struct::TObjectEx {
        PAS_CLASS_META(TConstellation, EC_Struct::TObjectEx, "TConstellation", 140)
        void p_destroy() override;
        void RestoreHiddenForm();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy);
        void ResolveLoadedReferences(TGalaxy* Galaxy);
        void ClearStarLinks();
        void ClearBoundaryRaySamples();
        void GenerateBoundaryRaySamples(std::int32_t Count);
        void SetOutlinePolygon(aVector::TPolygon2D* Polygon);
        void RebuildOutlineSegments();
        void ClearOutlineSegmentsAndBounds();
        void AddStar(TStar* Star);
        void AddAdjacentConstellation(TConstellation* Constellation);
        std::uint8_t SharesOutlineSegment(TConstellation* Constellation);
        void ResetGeneratedMapShape();
        void ClearStars();
        void ClearAdjacentConstellations();
        float GetOutlineArea();
        std::int32_t FindNextClosestStarPair(TStar*& FirstStar, TStar*& SecondStar, std::int32_t MinimumDistance);
        std::uint8_t HasStarGraphCycle();
        std::uint8_t IsStarGraphConnected();
        std::uint8_t BuildStarGraph();
        void ExpandOutlineBounds(EC_Struct::TPointF Point);
        void RefreshOutlineBounds();
        std::uint8_t ContainsPoint(EC_Struct::TPointF Point);
        std::uint8_t HasAdjacentConstellation(TConstellation* Constellation);
        std::uint8_t HasOutlineSegment(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint);
        std::uint8_t AreBothPointsOnOutline(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint);
        EC_Struct::TPointF CalculateLabelPosition();
        std::uint8_t HasOutlineVertex(EC_Struct::TPointF Point);
        std::uint8_t IsPointNearOutline(EC_Struct::TPointF Point);
        void NormalizeOutlineSegmentOrder();
        pas::WideString GetName();
        std::uint8_t HasDominatorPresence();
        std::uint8_t HasPirateClanPresence();
        std::uint8_t HasBertorOfSeries(aGalaxyStruct::TDominatorSeries Series);
        std::int32_t CountShipsByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask);
        std::uint32_t Id;
        std::uint8_t HomeDistanceTier;
        std::uint8_t Visible;
        std::uint8_t cpp_padding[2];
        EC_Struct::TPointF MapCenter;
        std::int32_t OutlineGrowthStepsRemaining;
        pas::List* Stars;
        pas::List* AdjacentConstellations;
        pas::List* OutlineSegments;
        pas::List* HiddenOutlineSegmentsBackup;
        pas::List* BoundaryRaySamples;
        WindowsSdk::TRect OutlineBounds;
        WindowsSdk::TPoint OutlineBoundsSize;
        pas::List* StarLinks;
        pas::Array<std::int32_t, 0, 13> ShipTypeCounts;
        aVector::TPolygon2D* OutlinePolygons;
        aVector::TPolygon2D* HiddenOutlinePolygonsBackup;
        std::uint16_t SerializedValue88;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PPlanetNewsEntry = aGalaxyStruct::TPlanetNews*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStoredItem : EC_Struct::TObjectEx {
        PAS_CLASS_META(TStoredItem, EC_Struct::TObjectEx, "TStoredItem", 12)
        void p_destroy() override;
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy);
        pas::WideString Name;
        pas::Object* Item;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TJumpGateEntry {
        SE_Space::TObjectSE* Gate;
        std::uint8_t UsedThisTurn;
        std::uint8_t cpp_padding[3];
        std::uint32_t GateFilmId;
        SE_Space::TObjectSE* Effect;
        std::uint32_t EffectFilmId;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct THole : EC_Struct::TObjectEx {
        PAS_CLASS_META(THole, EC_Struct::TObjectEx, "THole", 52)
        void p_destroy() override;
        void InitializeGraphic(pas::WideString GraphKey);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy);
        void ResolveLoadedReferences(TGalaxy* Galaxy);
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        std::uint32_t Id;
        TStar* Star1;
        EC_Struct::TPointF Position1;
        TStar* Star2;
        EC_Struct::TPointF Position2;
        std::int32_t CreatedTurn;
        std::int32_t HoleType;
        SE_Space::TObjectSE* Graphic;
        std::int32_t FilmObjectId;
        pas::WideString ArcadeMapName;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TMovingDropItemEntry {
        pas::Object* Payload;
        EC_Struct::TPointF Destination;
        std::int32_t SourceShipId;
        std::uint8_t InsertedIntoStar;
        std::uint8_t UseFlag;
        std::uint8_t cpp_padding[2];
    };
    #pragma pack(pop)

    using PMovingDropItemEntry = TMovingDropItemEntry*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TInterfaceStateOverride : EC_Struct::TObjectEx {
        PAS_CLASS_META(TInterfaceStateOverride, EC_Struct::TObjectEx, "TInterfaceStateOverride", 16)
        void p_destroy() override;
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, std::uint8_t State);
        void SetState(std::uint8_t State);
        std::uint8_t GetState();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void Reapply();
        pas::WideString FormName;
        pas::WideString ControlPath;
        std::uint8_t State;
        std::uint8_t OriginalState;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TInterfaceTextOverride : EC_Struct::TObjectEx {
        PAS_CLASS_META(TInterfaceTextOverride, EC_Struct::TObjectEx, "TInterfaceTextOverride", 20)
        void p_destroy() override;
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, pas::WideString Text);
        void SetText(pas::WideString Text);
        pas::WideString GetText();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void Reapply();
        pas::WideString FormName;
        pas::WideString ControlPath;
        pas::WideString Text;
        pas::WideString OriginalText;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TInterfaceImageOverride : EC_Struct::TObjectEx {
        PAS_CLASS_META(TInterfaceImageOverride, EC_Struct::TObjectEx, "TInterfaceImageOverride", 20)
        void p_destroy() override;
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, pas::WideString ImagePath);
        void SetImagePath(pas::WideString ImagePath);
        pas::WideString GetImagePath();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void Reapply();
        pas::WideString FormName;
        pas::WideString ControlPath;
        pas::WideString ImagePath;
        pas::WideString OriginalImagePath;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TInterfacePosOverride : EC_Struct::TObjectEx {
        PAS_CLASS_META(TInterfacePosOverride, EC_Struct::TObjectEx, "TInterfacePosOverride", 48)
        void p_destroy() override;
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, std::int32_t DeltaX, std::int32_t DeltaY, std::int32_t DeltaDepth);
        void SetPosition(std::int32_t DeltaX, std::int32_t DeltaY, std::int32_t DeltaDepth);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void Reapply();
        pas::WideString FormName;
        pas::WideString ControlPath;
        WindowsSdk::TPoint Position;
        std::uint8_t cpp_padding[4];
        double Depth;
        WindowsSdk::TPoint OriginalPosition;
        double OriginalDepth;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TInterfaceSizeOverride : EC_Struct::TObjectEx {
        PAS_CLASS_META(TInterfaceSizeOverride, EC_Struct::TObjectEx, "TInterfaceSizeOverride", 28)
        void p_destroy() override;
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, std::int32_t Width, std::int32_t Height);
        void SetSize(std::int32_t Width, std::int32_t Height);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void Reapply();
        pas::WideString FormName;
        pas::WideString ControlPath;
        WindowsSdk::TPoint Size;
        WindowsSdk::TPoint OriginalSize;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TDominatorSeriesMask = pas::Set<0, 7>;

    #pragma pack(push, 1)
    struct TStarCombatEvent {
        std::int32_t StepIndex;
        std::int32_t CombatGroup;
        pas::Object* Attacker;
        pas::Object* Target;
        pas::Object* Weapon;
    };
    #pragma pack(pop)

    using PStarCombatEvent = TStarCombatEvent*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCustomSystemInfo : EC_Struct::TObjectEx {
        PAS_CLASS_META(TCustomSystemInfo, EC_Struct::TObjectEx, "TCustomSystemInfo", 24)
        void p_destroy() override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        pas::WideString Name;
        pas::WideString Icon;
        pas::WideString Info;
        pas::WideString TypeTag;
        std::int32_t Distance;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TConstellationStarLink {
        EC_Struct::TPointF StartPoint;
        EC_Struct::TPointF EndPoint;
        std::int32_t StartStarIndex;
        std::int32_t EndStarIndex;
        std::uint8_t TraversalMark;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    using PConstellationStarLink = TConstellationStarLink*;

    #pragma pack(push, 1)
    struct TMapLineSegment {
        EC_Struct::TPointF StartPoint;
        EC_Struct::TPointF EndPoint;
        std::uint8_t cpp_padding[12];
    };
    #pragma pack(pop)

    using PMapLineSegment = TMapLineSegment*;

    #pragma pack(push, 1)
    struct TConstellationBoundaryRaySample {
        EC_Struct::TPointF Position;
        EC_Struct::TPointF Direction;
        float Angle;
        std::uint8_t GrowthStopped;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    using PConstellationBoundaryRaySample = TConstellationBoundaryRaySample*;

} // namespace aGalaxy
