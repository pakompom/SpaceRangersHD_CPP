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
        // Preserved by fStarMap; purpose unresolved.
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
        // Increments SaveCount and restores temporary shop stock before serialization.
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Replaces the shared editable-save block, including player, holes and stars. Requires a player.
        void SaveEditableState();
        // Only runs when FinalizationNameEncoded is empty; consumes and clears the shared editable-save block. Requires a player.
        void ApplyEditableState();
        static void RunConfigOnStartHandlers();
        static void RunConfigOnLoadHandlers();
        static void RunConfigOnSaveHandlers();
        void ReapplyInterfaceOverrides();
        void BindScriptImports();
        // Processes off-screen stars; the player's star is simulated separately.
        void NextDay();
        void CompleteDay(std::uint8_t UnusedRecordFilm);
        void TransferShipsInTransit();
        // Refreshes every star's cache; called during generation and loading, not by Script.CoordX/CoordY.
        void RebuildStarDistances();
        void RefreshAllShipDerivedState();
        // Zero returns nil; an unknown nonzero ID raises.
        TConstellation* IdToConstellation(std::uint32_t Id);
        // Zero returns nil; an unknown nonzero ID raises.
        TStar* IdToStar(std::uint32_t Id);
        // Zero returns nil; an unknown nonzero ID raises.
        THole* IdToHole(std::uint32_t Id);
        // Zero always returns nil.
        void* IdToPlanet(std::uint32_t Id, std::uint8_t RaiseIfMissing);
        // Zero always returns nil. Includes docked ships and stored Tranclucators.
        void* IdToShip(std::uint32_t Id, std::uint8_t RaiseIfMissing);
        // Zero always returns nil. Includes inventories, shops, storage and moving drops.
        void* IdToItem(std::uint32_t Id, std::uint8_t RaiseIfMissing);
        // Returns nil when absent.
        void* IdToAsteroid(std::uint32_t Id);
        // Returns nil when absent.
        void* IdToMissile(std::uint32_t Id);
        std::uint8_t ContainsShipReference(void* Ship);
        std::uint8_t ContainsPlanetReference(void* Planet);
        void ClearJumpGates();
        // Registers an owned gate descriptor; returned storage is borrowed until ClearJumpGates.
        PJumpGateEntry CreateJumpGate(std::uint8_t WithEffect);
        // Accepts either endpoint star; returns nil when absent.
        THole* FindHoleInStarByKind(TStar* Star, std::int32_t HoleKind);
        // Visits loose items and ship equipment/artifacts; excludes shop stock and stored items.
        void ReleaseItemGraphics();
        // Replaces SpaceBackgroundEntries. Requires PlayerStar, a nonempty star list and differing minimum/maximum map diameters.
        void GenerateSpaceBackground(std::int32_t BackgroundIndex);
        void EnableDominatorSurfaces();
        void DisableDominatorSurfaces();
        // Includes the active text quest; applying the same seed twice restores the state.
        void XorProtectedState(std::int32_t Seed);
        // Already-obfuscated state is left unchanged.
        void ObfuscateProtectedState();
        void RestoreProtectedState();
        // An unconditional jump disables the checksum body; always returns zero in this binary.
        static std::uint32_t ComputeIntegrityChecksum(std::int32_t Mode);
        void PrimeIntegrityChecksum(std::int32_t StatusCode);
        void PrimeIntegrityChecksum1(std::int32_t StatusCode);
        void PrimeIntegrityChecksum2(std::int32_t StatusCode);
        // A matching checksum clears the integrity status.
        void CheckIntegrityChecksum(std::int32_t ErrorCode);
        // Channel zero; unlike CheckIntegrityChecksum, a matching checksum stores StatusCode rather than zero.
        void CheckIntegrityChecksumAndSetStatus(std::int32_t StatusCode);
        void CheckIntegrityChecksum1(std::int32_t ErrorCode);
        void CheckIntegrityChecksum2(std::int32_t ErrorCode);
        static void ClearIntegrityStatus();
        // An unconditional jump disables the snapshot body; no-op in this binary.
        static void AppendIntegritySnapshot();
        std::uint8_t HasVisibleScoreModFlags();
        std::int32_t GetCheatPoints();
        void SetCheatPoints(std::int32_t Value);
        // Uses polygon containment, not Star.Constellation; returns -1 when no polygon contains the star.
        std::int32_t FindConstellationIndexForStar(TStar* Star);
        // Requires a player/home planet and generated outlines. Boss sectors are forced to tier three.
        void InitializeConstellationDistanceTiers();
        void BuildConstellationOutlineJunctions();
        std::uint8_t ShouldKeepConstellationOutlineVertex(EC_Struct::TPointF Point);
        void SimplifyConstellationOutline(std::int32_t ConstellationIndex);
        // Attempts every constellation; false means at least one graph is disconnected.
        std::uint8_t BuildConstellationStarGraphs();
        // Requires at least eight constellations.
        void BuildConstellationPolygonsAndAdjacency(aVector::TPolygon2D* WorkingPolygon);
        void GenerateGalaxyLayout(std::uint8_t PlayerRace);
        // Requires constellation ID 20 and generated compatible outlines. Merges its visible outline into a neighbor and retains backups for RestoreHiddenForm.
        void HideSpecialConstellation();
        std::int32_t CountVisibleConstellationsWithBoundaryPoints(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint);
        std::int32_t CountEligibleRangers();
        void RefreshRangerWealthStats();
        void RefreshRangerStrengthStats();
        // Assigns one-based positions by descending TotalExperience, including excluded rangers; leaves the Rangers list order unchanged.
        void RefreshRangerRatingPlaces();
        // Ignores ExcludedFromRating rangers; returns nil if none has positive strength.
        void* FindStrongestRanger();
        // Ignores ExcludedFromRating rangers; returns nil if none has positive wealth.
        void* FindWealthiestRanger();
        // Excludes stars with a custom faction.
        std::int32_t CountFactionStars(std::uint8_t Faction);
        TControlPercent GetFactionControlPercent(std::uint8_t Faction);
        // Active Galaxy only. Fraction of Dominator systems in Series, multiplied by the number of unresolved series; not a percentage.
        static float GetDominatorSeriesControlShare(aGalaxyStruct::TDominatorSeries Series);
        std::int32_t CountStarsInBattle();
        // Uses quest target-owner filters; leaves planets with an existing quest unchanged.
        void AssignTextQuestsToPlanets();
        static std::uint8_t HasPlayerQuestHistory(aGalaxyStruct::TQuestType QuestType, std::uint16_t QuestNumber);
        // Delphi TDateTime; -1 selects CurrentTurn.
        double TurnToDateTime(std::int32_t Turn);
        // -1 selects CurrentTurn.
        pas::WideString FormatTurnDate(std::int32_t Turn);
        // Adds a player bubble only after turn 300; news insertion still uses duplicate-text suppression.
        void AddPlanetNewsWithPlayerBubble(std::uint8_t NewsType, pas::WideString Text);
        // Rejects empty text; identical existing text suppresses insertion regardless of NewsType.
        void AddPlanetNews(std::uint8_t NewsType, pas::WideString Text);
        std::int32_t CountPlanetNewsByType(std::uint8_t NewsType);
        // Removes entries more than 30 days old.
        void PrunePlanetNews();
        // Replaces the global spawn-planet pointer without freeing its previous value; does not register the proxy in star or galaxy planet lists.
        static void CreateDominatorSpawnProxy(TStar* Star);
        void UpdateConstellationMilitaryStats();
        // Updates TechLevel but normally returns zero. The special mode returns 8 without updating it.
        std::uint8_t RefreshTechLevel();
        // Cancels normal-space Dominator and hostile-pirate jumps from other stars, plus assigned Dominion relocations.
        void CancelEnemyJumpsToStar(TStar* Star);
        // Nil Origin omits the origin-distance penalty. Searches active Galaxy; advances Self.RandomState.
        TStar* SelectStarForLiberationAttack(TStar* Origin, aGalaxyStruct::TStarFaction FriendlyFaction);
        void ComputeGlobalGoodsPriceBands();
        // Maps the global minimum/maximum price band to 0..100 with clamping.
        static std::uint8_t GetGoodsPricePercent(std::uint8_t GoodsType, std::int32_t Price);
        std::int32_t ScaleGoodsPriceByGalaxyAge(std::int32_t BaseValue);
        std::int32_t ScaleGoodsStockByGalaxyAge(std::int32_t BaseValue);
        // Clamps TechLevel to 2..7, linearly interpolates the endpoints, then rounds.
        std::int32_t ScaleIntByTechLevel(std::int32_t AtLevelTwo, std::int32_t AtLevelSeven);
        float InterpolateSingleByTechLevel(float AtLevelTwo, float AtLevelSeven);
        // Borrowed template. Uses the closest eligible technology when the interval has no match; falls back to the first built-in template when no availability matches.
        aConst::PWeaponInfo SelectWeaponInfo(std::uint32_t Seed, aGalaxyStruct::TWeaponAvailabilityMask AvailabilityMask, std::uint8_t MaximumTechLevel, std::uint8_t MinimumTechLevel);
        // Zero-based index; Context may be a planet or ship, or nil. Relaxes the priority interval after repeated misses. No termination guarantee when every template fails the context filter.
        static std::int32_t SelectMicroModule(std::uint8_t MinimumPriority, std::uint8_t MaximumPriority, std::uint32_t Seed, pas::Object* Context);
        // Zero-based index. The final attempt-limit fallback can return an incompatible module; callers must check CanInstallMicroModule. Context rejection can bypass the attempt-limit check.
        static std::int32_t SelectMicroModuleForEquipment(std::uint8_t MinimumPriority, std::uint8_t MaximumPriority, std::uint32_t Seed, pas::Object* Context, void* Item);
        // Zero-based series index or -1; advances Self.RandomState.
        std::int32_t SelectHullSeries(std::uint8_t OwnerId, std::uint8_t HullType, std::uint8_t MinimumRarity, std::uint8_t MaximumRarity);
        // Invalid series values return false.
        std::uint8_t IsDominatorSeriesUnresolved(aGalaxyStruct::TDominatorSeries Series);
        // True if any selected series is unresolved; false for an empty set.
        std::uint8_t HasUnresolvedDominatorSeries(TDominatorSeriesSet Series);
        // Advances deployed probes' terrain exploration and wear; idle completed planets still incur reduced wear.
        static void ProcessPlayerSatelliteExploration();
        // Requires a player. Counts deployed/player-storage probes, loose and carried probes in active Galaxy, and Self.StoredItems; excludes shop stock.
        std::int32_t CountExistingSatellites();
        std::int32_t ComputeScaledMiniMoney(std::uint8_t ScaleIndex);
        std::int32_t ComputeScaledSmallMoney(std::uint8_t ScaleIndex);
        std::int32_t ComputeScaledAverageMoney(std::uint8_t ScaleIndex);
        std::int32_t ComputeScaledBigMoney(std::uint8_t ScaleIndex);
        std::int32_t ComputeScaledHugeMoney(std::uint8_t ScaleIndex);
        // Accepts Zero, Mini, Small, Average, Big and Huge; unknown tags raise. Uses active Galaxy for scaling.
        static std::int32_t ResolveMoneySizeTag(pas::WideString Tag, std::uint8_t ScaleIndex);
        static std::int32_t GetMiniGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetSmallGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetAverageGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetBigGoodsQuantity(std::uint8_t GoodsType);
        static std::int32_t GetHugeGoodsQuantity(std::uint8_t GoodsType);
        // Sizes 0..5 select zero through huge; invalid sizes raise.
        static std::int32_t GetGoodsQuantityBySize(std::uint8_t Size, std::uint8_t GoodsType);
        // Zero maps to zero; otherwise chooses levels 1..5, with ties favoring the larger level.
        std::uint8_t ClassifyGoodsQuantity(std::int32_t Quantity, std::uint8_t GoodsType);
        static std::int32_t GetMinimumGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetLowGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetAverageGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetHighGoodsPrice(std::uint8_t GoodsType);
        static std::int32_t GetMaximumGoodsPrice(std::uint8_t GoodsType);
        // Levels 1..5 select minimum through maximum; invalid levels raise.
        static std::int32_t GetGoodsPriceByLevel(std::uint8_t Level, std::uint8_t GoodsType);
        // Zero maps to zero; otherwise chooses levels 1..5, with ties favoring the larger level.
        std::uint8_t ClassifyGoodsPrice(std::int32_t Price, std::uint8_t GoodsType);
        void ProcessStationSpawning();
        void ReplenishStationType(aGalaxyStruct::TStationType StationType);
        void ProcessDominatorResearchProgress();
        // Every selected series must have at least 100 progress; the empty set returns true.
        std::uint8_t IsDominatorResearchComplete(TDominatorSeriesSet Series);
        // Percentage points per day.
        float GetDominatorResearchRate(aGalaxyStruct::TDominatorSeries Series);
        // Returns 20..100 percent.
        std::uint8_t GetDominatorResearchEfficiency(aGalaxyStruct::TDominatorSeries Series);
        // One-based index over active Galaxy star/ship order. Uses Self's cached type count as an early gate; missing entries return nil.
        void* FindStationByTypeAndIndex(std::int32_t Index, aGalaxyStruct::TStationType StationType);
        // Eligible only at positive multiples of 365 accrued deposit days.
        void TryAwardDepositPrize();
        // Debt pauses deposit accrual. Both states are cleared when no business centers remain.
        void ProcessBankDebtAndDeposits();
        // Also clears the player's deposited nodes when no ranger centers remain.
        void ProcessRangerCenterNewYearEvent();
        // Returns BuildLiberationOrders on accepted groups; false for early rejection or disbanding.
        std::uint8_t TryCreateLiberationGroup();
        std::uint8_t TryDispatchMilitaryBaseToEnemyStar();
        // Searches active Galaxy; requires an assigned destination different from the current normal-space star.
        static void* FindMilitaryBaseInTransit();
        // Searches active Galaxy; includes a base already at its assigned destination.
        static std::uint8_t HasMilitaryBaseAssignedToStar(TStar* Star);
        // Tests the fourth order target of each liberation group, unlike TStar.HasLiberationGroupOrder.
        std::uint8_t HasLiberationGroupTargetingStar(TStar* Star);
        void AssignSpecialStationService();
        static void ApplyWingmanLeadershipPenalty();
        // Can complete pirate ending five, clear eminent ranger titles and publish defeat news.
        void ProcessCoalitionDefeat();
        void ComputeRangerSpawnQuotas();
        // Retains the most recent 1825 days.
        void PruneExpiredGalaxyEvents();
        // Uses the active Galaxy, not Self; denominator is max(pirate systems - 1, 1).
        static float GetCoalitionToPirateSystemRatio();
        // With custom rules disabled, reads the active Galaxy difficulty array rather than Self.
        std::int32_t GetEffectiveDifficultyLevel();
        // Returns 0..9; tier boundaries are 6, 14, 22, and subsequent increments of eight.
        std::uint8_t GetDifficultyTierIndex();
        // A negative Level selects the effective difficulty; values above 24 extrapolate.
        float InterpolateDifficulty(std::int32_t Level, float AtZero, float AtEight, float AtSixteen, float AtTwentyFour);
        // A negative Level selects the effective difficulty.
        float ScaleDifficultyExponentially(std::int32_t Level, float BaseValue, float FactorPerEightLevels);
        float GetDominatorBossHullScale();
        float GetDominatorKillExperienceScale();
        std::int32_t GetTurnsBetweenLiberationGroups();
        std::int32_t GetInitialDominatorControlPercent();
        std::int32_t GetDominatorAggressionLevel();
        std::int32_t GetDominatorSpawnLevel();
        std::int32_t GetPirateAggressionLevel();
        // When enabled, seeded helpers ignore their supplied seed.
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
        // Adds a kind-five player notification using localized text.
        static void ShowLocalizedWarning(pas::WideString TextKey);
        // Takes ownership. Replacing a name frees the previously stored item; detach Item from its old container first.
        void StoreItem(pas::WideString Name, pas::Object* Item);
        // Returns nil when absent. Remove detaches the item and frees its named entry, transferring ownership to the caller.
        pas::Object* GetStoredItem(pas::WideString Name, std::uint8_t Remove);
        // Inserts a new custom template in the case-insensitive sorted pool.
        aConst::PWeaponInfo GetOrCreateCustomWeaponInfo(pas::WideString Name);
        // Binary search of CustomWeaponTypes; raises if absent.
        aConst::PWeaponInfo RequireCustomWeaponInfo(pas::WideString Name);
        // Checks special simulation, cheat points and all protected integrity flags.
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
        // Zero-based index into Rangers; -1 when no player is registered.
        std::int32_t PlayerRangerIndex;
        // Owns TStar entries.
        aMyFunction::TObjectList* Stars;
        // Owns THole entries.
        aMyFunction::TObjectList* Holes;
        // Owns TStoredItem entries, addressed by name by scripts.
        aMyFunction::TObjectList* StoredItems;
        // Borrowed TPlanet index; the stars own the planets.
        pas::List* Planets;
        // Borrowed TRanger roster, including the player.
        pas::List* Rangers;
        // Independent pirates; excludes Pirate Clan ships.
        std::int32_t PirateCount;
        std::int32_t PirateClanCount;
        std::int32_t TransportCount;
        std::int32_t CurrentTurn;
        aGalaxyStruct::TGalaxyDifficultyLevels DifficultyLevels;
        // Also the XOR key for stored cheat points.
        std::uint32_t GenerationSeed;
        std::uint32_t RandomState;
        std::int32_t AverageRangerCapital;
        std::int32_t MaxRangerWealth;
        float AverageRangerStrength;
        // May include independent pirates after Coalition defeat.
        float BestRangerStrength;
        // Can differ from BestRangerStrength after Coalition defeat.
        pas::Object* StrongestRanger;
        pas::Object* WealthiestRanger;
        // Trader, pirate, warrior.
        pas::Array<pas::Object*, 0, 2> EminentCareerShips;
        // Cached counts indexed by ship type, 0..13.
        TShipPopulationCounts ShipTypeCounts;
        std::uint32_t NextPlanetNewsId;
        // Owns PPlanetNewsEntry records.
        pas::List* PlanetNews;
        // Owns PWeaponInfo records.
        pas::List* CustomWeaponTypes;
        TStar* KellerTargetStar;
        std::int32_t KellerMissionState;
        // Read as Single by the dormant checksum; gameplay meaning unresolved.
        float ChecksumScalarD0;
        // TDominatorSeries order.
        pas::Array<TDominatorResearchEntry, 0, 2> DominatorResearch;
        // Read as Single by the dormant checksum; gameplay meaning unresolved.
        float ChecksumScalarEC;
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[3];
        // Script.DeltaWin faction indices.
        pas::Array<std::int32_t, 0, 2> WarDeltaWin;
        // Pilot-race indices; may be negative.
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
        // Zero is treated as unresolved by score and Ranger Center dialogue code, even if PirateWinType is nonzero.
        std::int32_t PirateWinTurn;
        // Script.PirateWin: 0 initially, endings 1..5. Ending 3 blocks clan attacks/captures, not all pirate-planet production.
        std::int32_t PirateWinType;
        std::int32_t CoalitionDefeatedTurn;
        std::uint8_t GraphDominatorSurfacesEnabled;
        // Selects space-image template kind 10 + value for captain health effect 2.
        std::uint8_t SpaceEffectKind;
        std::uint8_t cpp_padding_2[2];
        // Owns live TScript instances.
        pas::List* Scripts;
        // Owns TGroup instances.
        pas::List* LiberationGroups;
        // Owns PJumpGateEntry records and their retained graphics.
        pas::List* JumpGates;
        // Borrowed TShip entries awaiting transfer between stars.
        pas::List* ShipsInTransit;
        std::int32_t ConstellationCount;
        // Owns TConstellation entries.
        aMyFunction::TObjectList* Constellations;
        // Owned point records shared by generated sector borders.
        pas::List* ConstellationOutlineJunctions;
        pas::DynArray<TSpaceBackgroundEntry> SpaceBackgroundEntries;
        std::int32_t SaveCount;
        std::int32_t LoadCount;
        // Shop quote after any hull trade-in; included by the dormant checksum.
        std::int32_t PendingEquipmentPurchasePrice;
        std::uint8_t IronWill;
        std::uint8_t DominatorModLevel;
        std::uint8_t TechnicModEnabled;
        std::uint8_t AmmoModEnabled;
        std::uint8_t GodModEnabled;
        std::uint8_t UltraScanModEnabled;
        std::uint8_t StasisModEnabled;
        // Reset on campaign creation; other meaning unresolved.
        std::uint8_t CampaignFlag183;
        pas::WideString FinalizationNameEncoded;
        aGalaxyStruct::TGalaxyCustomRules CustomRules;
        std::uint8_t cpp_padding_3[1];
        std::int32_t NextSpecialStationServiceTurn;
        // Owns TGalaxyEvent entries.
        aMyFunction::TObjectList* GalaxyEvents;
        // Owns TInterfaceStateOverride entries.
        aMyFunction::TObjectList* InterfaceStateOverrides;
        // Owns TInterfaceTextOverride entries.
        aMyFunction::TObjectList* InterfaceTextOverrides;
        // Owns TInterfaceImageOverride entries.
        aMyFunction::TObjectList* InterfaceImageOverrides;
        // Owns TInterfacePosOverride entries.
        aMyFunction::TObjectList* InterfacePositionOverrides;
        // Owns TInterfaceSizeOverride entries.
        aMyFunction::TObjectList* InterfaceSizeOverrides;
        // Borrowed TShip queue for rebuilding caches after deserialization.
        pas::List* LoadedShips;
        // Stops the score-screen wait in TStar.NextDay; cleared after the wait.
        std::uint8_t ScoreScreenDismissed;
        // Suppresses UI and gameplay side effects during teardown.
        std::uint8_t Destroying;
        std::uint8_t cpp_padding_4[2];
        std::uint32_t GenerationMachineHash;
        // Nonzero skips CompleteDay and makes RefreshTechLevel return eight; full mode semantics unresolved.
        std::uint8_t SpecialSimulationMode;
        std::uint8_t CheatsDisabled;
        std::uint8_t cpp_padding_5[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TStarDistanceEntry {
        // Rounded Euclidean distance in parsecs, not squared.
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
        // Removes empty moving-drop descriptors; includes module-integrity checks.
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Appends owned objects; requires a fresh instance. References are resolved separately.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy);
        void ResolveLoadedReferences(TGalaxy* Galaxy);
        // Editable subset of the system; excludes the player ship.
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        // Updates existing objects and can create stations, planets, items and asteroids. Moves star-link endpoints but does not rebuild distance caches.
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        void PruneWeaponTargetsAfterTurn();
        // Recursively marks unmarked events sharing an attacker or target. Events contains PStarCombatEvent; Group must be nonzero.
        void MarkConnectedCombatEvents(pas::List* Events, pas::Object* Target, std::int32_t Group);
        // Returns the sum of the new goods' Cost, not their quantity. Nonpositive Quantity creates no drops and returns zero.
        std::int32_t DropMinerals(std::int32_t Quantity, EC_Struct::TPointF Position, std::uint32_t Seed);
        // Requires a player. Achievement progress is independent of eligibility for a planet's reward or complaint.
        void ProcessPlayerAsteroidKill(std::int32_t MineralValue, EC_Struct::TPointF Position, std::uint32_t AsteroidId);
        // Clears weapon, missile and queued attack references; accepts any target class. Nil is a no-op.
        void ClearTargetReferences(pas::Object* Target);
        // Clears attack, landing and combat-event references without removing or freeing Ship. Nil is a no-op.
        void ClearShipReferences(void* Ship);
        // Clears targets, pickups, ReferencedItems and moving-drop payload references; does not remove Item from Items or free it.
        void ClearItemReferences(void* Item);
        void ClearCombatEventWeaponReferences(void* Weapon);
        // Daily preparation without movement/combat simulation; the caller tracks whether it has already run.
        void PrepareNextDay();
        // Updates followers and targets recursively for docked ships. Does not remove the object from Ships or free it; selected pursuers may receive a jump order.
        void HandleObjectLeavingStar(pas::Object* Obj);
        // Trims or clears planned movement paths; does not apply collision damage.
        void AvoidShipPathCollisions();
        // Normal-space ships only; uses MovementStepCount.
        void RebuildShipMovementPaths();
        void OpenSpaceScene(GI_Panel::TPanelGI* MapPanel, GI_MessageLoop::TObjectGI* Minimap, GI_MessageLoop::TMessageLoopGI* Screen);
        void RefreshSpaceObjectPositions();
        void QueueSpaceImageLoads(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);
        void QueueHyperspaceShipImageLoads(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);
        // Sets Size to 2000.
        pas::WideString GetBackgroundImagePath(std::int32_t& Size);
        void ProcessItemScripts(std::int32_t TurnPhase);
        // Changes the series only when exactly one series has eligible local forces; includes the bosses.
        void RefreshDominatorSeries();
        void RefreshDerivedStats();
        // Applies campaign-ending and liberation-contribution rules; not a raw ship-presence query.
        void GetControlPresence(std::uint8_t& PlayerPartyPresent, std::uint8_t& CoalitionPresent, std::uint8_t& DominatorsPresent, std::uint8_t& PiratesPresent, std::uint8_t& CustomPresent);
        // Can transfer planet ownership and emit capture news. Pirate ending 3 prevents new Pirate Clan captures.
        void UpdateControlFaction();
        // Reevaluates ownership after clearing a custom faction, resets liberation contributions and can clear NPC prison terms. No-op without a player.
        void ResetControlFaction();
        void RefreshMapDiameterAndStats();
        // 200 steps in the player's star, 50 elsewhere; MovementStepScale is the reciprocal.
        void RefreshMovementStepParameters();
        // Uses the final planet-list entry when nonempty; otherwise SystemRadius. Does not update MapDiameter.
        std::int32_t ComputeMapDiameter();
        std::int32_t CountPlanetsByOwner(std::uint8_t OwnerId);
        // Counts owners 0..5 and 7; includes Dominators.
        std::int32_t CountDistinctInhabitedPlanetOwners();
        // If every planet is uninhabited, returns the last planet; nil only for an empty list.
        void* FindFirstInhabitedPlanet();
        // Returns nil when no inhabited planet exists; advances the star RNG.
        void* SelectRandomInhabitedPlanet();
        // Excludes uninhabited planets; first entry wins equal progress rates.
        void* FindFastestResearchPlanet();
        void RefreshShipTypeCounts();
        std::int32_t CountEligibleRangersInSpace();
        // Uses cached population counts.
        std::int32_t CountShipsByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask);
        // Excludes bosses, scripted-standing ships and Dominators with a positive ActiveProgramAppliedTurn. Strength is an Extended output.
        std::int32_t CountDominatorForces(aGalaxyStruct::TDominatorSeries Series, std::uint8_t ExcludeAbsoluteOrders, std::uint8_t OtherSeries, pas::Extended& Strength);
        // Counts normal-space types 1..5; excludes bosses and scripted standing.
        std::int32_t CountStandardDominatorsOfLocalSeries();
        // Requires pirate ownership. IncludeIndependent also includes the pirate player.
        std::int32_t CountPirateForces(std::uint8_t ExcludeAbsoluteOrders, pas::Extended& Strength, std::uint8_t IncludeClanVariants, std::uint8_t IncludeIndependent);
        std::int32_t CountCustomFactionForces(std::uint8_t ExcludeAbsoluteOrders, pas::WideString& Faction, pas::Extended& Strength);
        // Faction is empty when counted ships do not share one nonempty faction tag.
        std::int32_t CountOtherCustomFactionForces(std::uint8_t ExcludeAbsoluteOrders, pas::WideString& Faction, pas::Extended& Strength);
        // Includes the pirate player; excludes scripted standing.
        std::int32_t CountPirateShips(std::uint8_t IncludeOutsideStarSpace);
        // Includes local garrison ships absent from Ships, avoiding duplicate list entries.
        std::int32_t CountForcesByOwnerGroups(pas::Extended& Strength, std::uint8_t IncludeCoalition, std::uint8_t IncludeDominators, std::uint8_t IncludePirates, std::uint8_t IncludeCustom);
        // Includes docked and hyperspace entries in Ships; excludes ExcludedFromRating. Byte count can wrap.
        std::uint8_t CountRatedRangersByCareerMask(aGalaxyStruct::TRangerCareerSet CareerMask);
        // Unlike CountRatedRangersByCareerMask, includes ExcludedFromRating entries.
        pas::WideString GetRangerNamesByCareerMask(aGalaxyStruct::TRangerCareerSet CareerMask);
        std::uint8_t IsConstellationVisible();
        // Group 0 Coalition, 1 Dominators/custom, 2 pirates. Lazily refreshes all three once per active Galaxy.CurrentTurn.
        float GetCachedFactionStrength(std::uint8_t FactionGroup);
        // Sums StrengthInBestRanger over Ships, excluding the three bosses; no docking/hyperspace filter.
        float SumBestRangerRelativeStrength(aGalaxyStruct::TShipTypeMask ShipTypeMask);
        void RebuildStarDistances(TGalaxy* Galaxy);
        // Includes any TKling, standing eight, or scripted ship with nonempty faction not beginning with SubFaction. No docking/hyperspace filter; the substring test also accepts absence.
        std::uint8_t HasHostilePresenceForScriptBinding();
        // Starts at distance-cache index one and excludes custom factions. Requires a current distance cache.
        TStar* FindNearestStarByFaction(aGalaxyStruct::TStarFaction Faction, std::uint8_t InBattle);
        EC_Struct::TPointF GetBoundaryPointTowardStar(TStar* Star);
        // Searches every order target in active Galaxy.LiberationGroups.
        std::uint8_t HasLiberationGroupOrder();
        // Requires a visible peaceful system, no custom faction and an undefeated Coalition.
        void TryGenerateSystemNews();
        void NextDay(std::uint8_t RecordFilm);
        std::uint32_t Id;
        std::uint32_t GenerationSeed;
        std::uint32_t RandomState;
        pas::WideString Name;
        EC_Struct::TPointF Position;
        std::uint16_t SystemRadius;
        std::uint8_t cpp_padding[2];
        // Cached by RefreshMapDiameterAndStats.
        std::int32_t MapDiameter;
        // TPlanet entries.
        aMyFunction::TObjectList* Planets;
        // TAsteroid entries.
        aMyFunction::TObjectList* Asteroids;
        // TShip entries.
        aMyFunction::TObjectList* Ships;
        // TItem entries.
        aMyFunction::TObjectList* Items;
        // Owns PMovingDropItemEntry descriptors; payload ownership changes when inserted into the star.
        pas::List* MovingDropItems;
        // TMissile entries.
        aMyFunction::TObjectList* Missiles;
        pas::WideString SystemProcessName;
        // Native managed-record RTTI.
        aGalaxyStruct::TStarStatus Status;
        float SafeRadius;
        float DamageRadius;
        // Zeroed by Create; original purpose remains unresolved.
        std::int32_t Reserved64;
        std::int32_t Radius;
        // Retained reference.
        SE_Space::TObjectSE* Graphic;
        std::int32_t DaysSincePlayerVisit;
        std::int32_t DaysSinceLastNpcShipSpawn;
        std::int32_t PlayerPresenceLevel;
        // Stored as one byte in saves.
        std::int32_t BackgroundImage;
        // Reset during generation; meaning unresolved.
        std::uint8_t Flag80;
        std::uint8_t cpp_padding_2[3];
        // Updated by UpdateControlFaction while Dominators are present; recent battles can release imprisoned rangers.
        std::int32_t LastDominatorPresenceTurn;
        // Updated while Pirate Clan forces are present.
        std::int32_t LastPiratePresenceTurn;
        std::int32_t LastLiberationRewardsTurn;
        // Consumed after ProcessSystemLiberationRewards.
        std::uint8_t LiberationRewardsPending;
        std::uint8_t cpp_padding_3[3];
        // Includes Self; sorted by rounded distance. Coordinate setters do not refresh this cache.
        pas::DynArray<TStarDistanceEntry> StarDistances;
        // AI population counts; excludes most docked and hyperspace ships.
        pas::Array<std::int32_t, 0, 13> ShipTypeCounts;
        TConstellation* Constellation;
        // One-based temporary index used while building StarLinks.
        std::uint16_t ConstellationGraphIndex;
        std::uint8_t cpp_padding_4[2];
        // Script.NoComeKlingToStar.
        std::uint8_t NoComeKling;
        std::uint8_t cpp_padding_5[3];
        // Assigned Dominion; relocation transfers this reference before physical arrival.
        pas::Object* Dominion;
        pas::WideString MapLabel;
        // Owns TCustomSystemInfo entries.
        aMyFunction::TObjectList* CustomSystemInfos;
        std::int32_t CurrentStepIndex;
        // Captures MovementStepCount for this simulation.
        std::int32_t SimulationStepCount;
        std::uint8_t RecordingTurnFilm;
        std::uint8_t PlayerCombatOccurred;
        std::uint8_t InterruptLongTravel;
        std::uint8_t KeepFilmRunning;
        // Owns PStarCombatEvent records; object references are borrowed.
        pas::List* CombatEvents;
        // Space-engine object IDs.
        pas::List* PendingFilmObjectRemovals;
        // Borrowed TItem references.
        pas::List* ReferencedItems;
        // Temporary camera path; node Heading stores the step index.
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
        // Requires a fresh instance; saved object IDs remain unresolved.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy);
        void ResolveLoadedReferences(TGalaxy* Galaxy);
        void ClearStarLinks();
        void ClearBoundaryRaySamples();
        // Requires a positive Count.
        void GenerateBoundaryRaySamples(std::int32_t Count);
        // Takes ownership and frees the previous polygon chain; do not pass the current chain.
        void SetOutlinePolygon(aVector::TPolygon2D* Polygon);
        // Replaces the list without freeing its old segment records.
        void RebuildOutlineSegments();
        void ClearOutlineSegmentsAndBounds();
        // Also sets Star.Constellation; does not remove earlier membership.
        void AddStar(TStar* Star);
        // Suppresses duplicates; does not add the reciprocal relationship.
        void AddAdjacentConstellation(TConstellation* Constellation);
        // Self compares true; otherwise matches complete segment endpoints in either direction.
        std::uint8_t SharesOutlineSegment(TConstellation* Constellation);
        void ResetGeneratedMapShape();
        // Clears only the borrowed list; leaves Star.Constellation unchanged.
        void ClearStars();
        void ClearAdjacentConstellations();
        float GetOutlineArea();
        // Resumes equal-distance pairs using the input stars; zero clears both outputs and means no pair remains.
        std::int32_t FindNextClosestStarPair(TStar*& FirstStar, TStar*& SecondStar, std::int32_t MinimumDistance);
        // Changes link traversal marks; returns false without checking when Stars.Count exceeds 100.
        std::uint8_t HasStarGraphCycle();
        // Clears link traversal marks; returns false when Stars.Count exceeds 100.
        std::uint8_t IsStarGraphConnected();
        // Replaces StarLinks; result reports connectivity.
        std::uint8_t BuildStarGraph();
        void ExpandOutlineBounds(EC_Struct::TPointF Point);
        void RefreshOutlineBounds();
        std::uint8_t ContainsPoint(EC_Struct::TPointF Point);
        std::uint8_t HasAdjacentConstellation(TConstellation* Constellation);
        // Endpoint matching uses a tolerance and accepts either direction.
        std::uint8_t HasOutlineSegment(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint);
        // The points may lie on different outline segments.
        std::uint8_t AreBothPointsOnOutline(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint);
        // Requires an outline yielding interior samples; the sample mean need not lie inside a concave outline.
        EC_Struct::TPointF CalculateLabelPosition();
        std::uint8_t HasOutlineVertex(EC_Struct::TPointF Point);
        // Tests a distance of at most two map units.
        std::uint8_t IsPointNearOutline(EC_Struct::TPointF Point);
        void NormalizeOutlineSegmentOrder();
        // Localization key uses the current position in Galaxy.Constellations, not Id.
        pas::WideString GetName();
        // Uses member stars' cached population counts.
        std::uint8_t HasDominatorPresence();
        std::uint8_t HasPirateClanPresence();
        std::uint8_t HasBertorOfSeries(aGalaxyStruct::TDominatorSeries Series);
        // Uses cached population counts.
        std::int32_t CountShipsByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask);
        std::uint32_t Id;
        // 0 home/adjacent, 1..2 successive border hops, 3 farther or a boss sector.
        std::uint8_t HomeDistanceTier;
        // Script.SectorVisible.
        std::uint8_t Visible;
        std::uint8_t cpp_padding[2];
        EC_Struct::TPointF MapCenter;
        std::int32_t OutlineGrowthStepsRemaining;
        // Borrowed TStar entries.
        pas::List* Stars;
        // Borrowed TConstellation entries.
        pas::List* AdjacentConstellations;
        // Owns PMapLineSegment entries.
        pas::List* OutlineSegments;
        // Owns PMapLineSegment entries.
        pas::List* HiddenOutlineSegmentsBackup;
        // Owns PConstellationBoundaryRaySample entries.
        pas::List* BoundaryRaySamples;
        WindowsSdk::TRect OutlineBounds;
        WindowsSdk::TPoint OutlineBoundsSize;
        // Owns PConstellationStarLink entries.
        pas::List* StarLinks;
        // Sum of member stars' cached population counts.
        pas::Array<std::int32_t, 0, 13> ShipTypeCounts;
        // Owned polygon chain.
        aVector::TPolygon2D* OutlinePolygons;
        // Owned polygon chain.
        aVector::TPolygon2D* HiddenOutlinePolygonsBackup;
        // Saved value; meaning unresolved.
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
        // Requires a non-nil Item.
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Creates the owned item; overwrites a previous Item without freeing it.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy);
        pas::WideString Name;
        // Owned; clear before transferring the item elsewhere.
        pas::Object* Item;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TJumpGateEntry {
        // Retained TGateSE reference.
        SE_Space::TObjectSE* Gate;
        std::uint8_t UsedThisTurn;
        std::uint8_t cpp_padding[3];
        // Encoded TEFilmObj pointer, not a serialized object ordinal.
        std::uint32_t GateFilmId;
        // Optional retained TGateEffectSE reference.
        SE_Space::TObjectSE* Effect;
        // Encoded TEFilmObj pointer.
        std::uint32_t EffectFilmId;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct THole : EC_Struct::TObjectEx {
        PAS_CLASS_META(THole, EC_Struct::TObjectEx, "THole", 52)
        void p_destroy() override;
        // Empty GraphKey chooses a seeded Hole template. Clears ArcadeMapName; replacing an existing Graphic does not release the old reference.
        void InitializeGraphic(pas::WideString GraphKey);
        // Requires both endpoint stars and Graphic.
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Leaves endpoint IDs unresolved until ResolveLoadedReferences.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy);
        void ResolveLoadedReferences(TGalaxy* Galaxy);
        // Editable subset; writes remaining lifetime as CreatedTurn + 200 - Galaxy.CurrentTurn.
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        // Updates endpoints, positions, remaining lifetime and ArcadeMapName; preserves Id, HoleType and Graphic.
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        std::uint32_t Id;
        TStar* Star1;
        // In-system coordinates, not galaxy-map coordinates.
        EC_Struct::TPointF Position1;
        TStar* Star2;
        EC_Struct::TPointF Position2;
        std::int32_t CreatedTurn;
        // 1 ordinary, 2 Blazer, 4 Keller mission; other internal states exist.
        std::int32_t HoleType;
        // Retained reference.
        SE_Space::TObjectSE* Graphic;
        std::int32_t FilmObjectId;
        // Also accepts SkipAB and NoEntry.
        pas::WideString ArcadeMapName;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TMovingDropItemEntry {
        // TItem, or a spawned TShip during special handling; may be nil.
        pas::Object* Payload;
        EC_Struct::TPointF Destination;
        // Zero for asteroid mineral drops.
        std::int32_t SourceShipId;
        // Payload is registered in Items or, after transformation, Ships.
        std::uint8_t InsertedIntoStar;
        // Special payload handling; exact modes remain unresolved.
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
        // Captures the original value before applying the override.
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, std::uint8_t State);
        void SetState(std::uint8_t State);
        std::uint8_t GetState();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Loads both values and immediately reapplies the override.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void Reapply();
        pas::WideString FormName;
        pas::WideString ControlPath;
        // 0 inactive, 1 active; graph buttons also accept 2 disabled, 3 enabled.
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
        // Captures the original value before applying the override.
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, pas::WideString Text);
        void SetText(pas::WideString Text);
        pas::WideString GetText();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Loads both values and immediately reapplies the override.
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
        // Captures the original value before applying the override.
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, pas::WideString ImagePath);
        // Also accepts Style:name for any control. Do not change between image and style modes: they share one original-value slot.
        void SetImagePath(pas::WideString ImagePath);
        pas::WideString GetImagePath();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Loads both values and immediately reapplies the override.
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
        // Captures the original value before applying the override.
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, std::int32_t DeltaX, std::int32_t DeltaY, std::int32_t DeltaDepth);
        // Offsets are relative to the captured original position and depth, not the current control values.
        void SetPosition(std::int32_t DeltaX, std::int32_t DeltaY, std::int32_t DeltaDepth);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Loads both values and immediately reapplies the override.
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
        // Captures the original value before applying the override.
        void Initialize(pas::WideString FormName, pas::WideString ControlPath, std::int32_t Width, std::int32_t Height);
        // Nonpositive dimensions restore the corresponding original dimension.
        void SetSize(std::int32_t Width, std::int32_t Height);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Loads both values and immediately reapplies the override.
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

    // Preserve the full byte for native membership checks; selected series are 0..2.
    using TDominatorSeriesMask = pas::Set<0, 7>;

    #pragma pack(push, 1)
    struct TStarCombatEvent {
        std::int32_t StepIndex;
        // Zero until connected attacks are grouped for film timing.
        std::int32_t CombatGroup;
        pas::Object* Attacker;
        // Ship, item, asteroid or missile.
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
        // Script lookup key.
        pas::WideString TypeTag;
        // Ordering position in the system-object list.
        std::int32_t Distance;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TConstellationStarLink {
        EC_Struct::TPointF StartPoint;
        EC_Struct::TPointF EndPoint;
        // One-based ConstellationGraphIndex.
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
        // Radians.
        float Angle;
        std::uint8_t GrowthStopped;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    using PConstellationBoundaryRaySample = TConstellationBoundaryRaySample*;

} // namespace aGalaxy
