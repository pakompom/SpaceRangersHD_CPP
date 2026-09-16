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

namespace aAsteroid {
    struct TAsteroid;

} // namespace aAsteroid

namespace aEFilm {
    struct TEFilmObj;

} // namespace aEFilm

namespace aGalaxy {
    struct TGalaxy;

    struct THole;

    struct TStar;

} // namespace aGalaxy

namespace aItem {
    struct TArtefact;

    struct TArtefactTransmitter;

    struct TCargoHook;

    struct TDefGenerator;

    struct TEngine;

    struct TEquipment;

    struct TFuelTanks;

    struct THull;

    struct TItem;

    struct TRadar;

    struct TRepairRobot;

    struct TSatellite;

    struct TScaner;

    struct TWeapon;

} // namespace aItem

namespace aMyFunction {
    struct TObjectList;

} // namespace aMyFunction

namespace aPath {
    struct TSPath;

} // namespace aPath

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TCombatStatusEffect;

    struct TCustomShipInfo;

    struct TShipEquipmentCacheView;

    struct TShipStatBonusEntry;

    struct TCargoGoodsEntry;

    struct TCaptainHealthState;

    struct TShip;

    #pragma pack(push, 1)
    struct TCargoGoodsEntry {
        std::int32_t Count;
        std::int32_t TotalCost;
        std::int32_t PurchasedCount;
        std::int32_t PurchasedTotalCost;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TCaptainHealthState {
        double Progress;
        std::int32_t AppliedTurn;
        std::int32_t ExpireTurn;
        std::int32_t ApplicationCount;
        std::uint8_t cpp_padding[4];
    };
    #pragma pack(pop)

    enum TShipOrder : std::uint8_t {
        soNone = 0,
        soMove = 1,
        soLand = 2,
        soJump = 3,
        soJumpHole = 4,
        soTakeoff = 5,
        soFollowShip = 6,
        soTeleport = 7,
    };

    using TStationStandingMask = pas::Set<0, 15>;

    enum TItemDegradationKind : std::uint8_t {
        idkBattle = 0,
        idkUse = 1,
        idkForce = 2,
        idkAfterburner = 3,
    };

    enum TPilotSkill : std::uint8_t {
        psAccuracy = 0,
        psManeuverability = 1,
        psTechnical = 2,
        psTrading = 3,
        psCharisma = 4,
        psLeadership = 5,
    };

    enum TCombatStatusEffectType : std::uint8_t {
        cseShock = 0,
        cseAcid = 1,
        cseMagnetic = 2,
        cseWeaponBlock = 3,
        cseDroidBlock = 4,
        cseBWBuff = 5,
        cseBWRepairDebuff = 6,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TShip : EC_Struct::TObjectEx {
        PAS_CLASS_META(TShip, EC_Struct::TObjectEx, "TShip", 1232)
        void p_destroy() override;
        virtual void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        virtual void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        virtual void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        virtual void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        virtual void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        void SetMoney(std::int32_t Value);
        void ProcessBrokenFuelTankLeak();
        void RefreshTechKnowledgeAtLocation();
        virtual void NextDay();
        virtual void NextDayLogic();
        virtual void AssignWeaponTargetsInStar();
        static void DerivedStateCompatibilityHook();
        virtual pas::WideString GetTypeNameKey();
        pas::WideString GetLocalizedTypeName();
        pas::WideString GetFactionNameKey();
        std::uint8_t GetDefaultHullType();
        std::int32_t NextRandomInteger(std::int32_t Minimum, std::int32_t Maximum);
        pas::WideString GetSpaceInfoText();
        virtual std::int32_t GetDesiredCargoFreeSpace();
        std::uint8_t IsHullDestroyed();
        float GetTurnSeedFraction(std::int32_t TurnOffset);
        std::uint8_t HasCargoGoods();
        std::uint8_t CountCargoGoodsTypes();
        std::int32_t GetCarriedNodeCount();
        std::uint8_t HasLockedOrFollowOrder();
        std::uint8_t HasHullDamageOrBrokenEquippedItems();
        std::uint8_t CanRefuel();
        pas::WideString LookupVisibleTalkText(const pas::WideString& Path, TShip* OtherShip);
        void UpdateBestRangerRelativeRatings();
        void UpdateAverageRangerRelativeStrength();
        std::int32_t CalculateWealth();
        double CalculateAttackStrength();
        double CalculateDefenseStrength();
        double GetRepairStrengthFactor();
        double CalculateStrength();
        std::uint8_t GetFullHullRelativeStrengthPercent();
        std::int32_t GetWealthScaledAmount(std::uint8_t ScaleIndex);
        std::uint8_t HasScriptBindings();
        pas::WideString GetShipPortraitImagePath();
        pas::WideString GetCaptainPortraitResourceBase();
        static std::uint8_t HasPlayerChameleonCharges();
        std::uint8_t SelectChameleonVisualType();
        std::uint8_t IsPlayerChameleonEffectiveAgainstSelf();
        void RefreshGraphic();
        void CreateNormalGraphic();
        void CreateDominatorGraphic();
        void TransferToStar(aGalaxy::TStar* Star);
        aItem::TItem* FindCarriedItemById(std::uint32_t Id);
        void TryRelocateUnseenShip();
        void SynchronizeDockedLocation();
        std::uint8_t InNormalSpace();
        std::uint8_t IsOutsideStarSpace();
        std::uint8_t IsOnPlanet();
        std::uint8_t IsDockedToShip();
        std::uint8_t IsDocked();
        std::uint8_t HasPositiveSpeed();
        void CancelInvalidTravelOrder();
        void ClearPlanetQueue();
        aPlanet::TPlanet* SelectNearestQueuedPlanet();
        TShip* FindNearestDockableStation(TStationStandingMask StandingMask);
        aPlanet::TPlanet* FindFirstInhabitedPlanetInStar();
        aPlanet::TPlanet* NavigateToQueuedPlanet(std::uint8_t Absolute);
        std::uint8_t NavigateToEscapePlanet(std::uint8_t Absolute);
        std::uint8_t TryMirrorPartnerTravelOrders();
        void OrderRandomFreeFlightMove();
        std::uint8_t IsTargetStillPursuable(TShip* Target);
        std::uint8_t CanEscapePursuer(TShip* Pursuer);
        std::uint8_t HasLandablePlanetInStar(aGalaxy::TStar* Star);
        std::int32_t GetFullFuelBaseJumpRange();
        aGalaxy::TStar* FindNextStarTowardDestination(aGalaxy::TStar* Destination, std::uint8_t RequireFuelMargin);
        double DistanceToNearestShipByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask);
        std::int32_t EstimateOrderTravelTurns();
        std::int32_t EstimateTravelTurnsToObject(pas::Object* Target);
        std::int32_t EstimateTravelTurnsToPlanet(aPlanet::TPlanet* Planet);
        void NotifyCompanionDeath();
        std::int32_t ApplyDamage(pas::Object* Source, std::int32_t Damage, float HitRange, std::uint32_t& DamageColor, aGalaxyStruct::TDamageFlagSet DamageFlags);
        std::int32_t ApplyWeaponHit(TShip* Source, aItem::TWeapon* Weapon, float HitRange, std::uint32_t& DamageColor, pas::Var<std::uint32_t> DamageFlags, float DamageScale, std::int32_t FixedDamage);
        std::int32_t ApplyMissileHit(pas::Object* Missile, std::uint32_t& DamageColor, pas::Var<std::uint32_t> DamageFlags);
        void FireWeaponAtMissile(aItem::TWeapon* Weapon, pas::Object* Target, std::uint8_t RecordFilm);
        void FireWeaponAtShip(aItem::TWeapon* Weapon, TShip* Target, std::uint8_t RecordFilm);
        void FireWeaponAtItem(aItem::TWeapon* Weapon, aItem::TItem* Target, std::uint8_t RecordFilm);
        void FireWeaponAtAsteroid(aItem::TWeapon* Weapon, pas::Object* Target, std::uint8_t RecordFilm);
        std::int32_t ApplyInterceptorDamage(std::uint32_t& DamageColor);
        std::int32_t ApplyShockStatusDamage(std::uint32_t& DamageColor);
        std::int32_t ApplyAsteroidImpactDamage(aAsteroid::TAsteroid* Asteroid, std::uint32_t& DamageColor);
        std::int32_t ApplyExplosionDamage(TShip* SourceShip, pas::Object* ExplodingObject, std::int32_t ExtraDamage, pas::Object* Missile);
        std::int32_t ApplyStarHeatDamage();
        std::uint8_t CountEquippedWeapons();
        std::uint8_t CountMissileWeapons();
        std::uint8_t CountDirectFireWeapons();
        void ClearUnequippedWeaponTargets();
        std::uint8_t CountWeaponsByDamageFlags(aGalaxyStruct::TDamageFlagSet Flags);
        pas::WideString GetWeaponDamageSummary();
        pas::WideString GetManeuverabilitySummary();
        pas::WideString GetRepairPointsSummary();
        std::uint8_t HasScannerArtefact(TShip* UnusedTarget);
        std::int32_t GetWeaponActionRange(aItem::TWeapon* Weapon);
        std::int32_t GetWeaponSlotRange(std::int32_t SlotIndex);
        std::int32_t GetWeaponMinDamage(aItem::TWeapon* Weapon);
        std::int32_t GetWeaponMaxDamage(aItem::TWeapon* Weapon);
        std::int32_t GetMaxWeaponRange();
        void ClearWeaponTargets(pas::Object* Target);
        std::uint8_t IsAttackingShip(TShip* Target);
        double ChanceToWin(TShip* Target);
        std::uint8_t GetWinChancePercent(TShip* Target);
        void SetJointAttackTarget(TShip* Ally, TShip* Target);
        std::uint8_t IsEnemyPursuingSelf();
        std::uint8_t CanSafelyDetonateItem(aItem::TItem* Item);
        void SetStoredRangerRelationLevel(TShip* Ranger, aGalaxyStruct::TRelationLevel Level);
        aGalaxyStruct::TRelationLevel GetRelationLevelToShip(TShip* Ship);
        pas::WideString GetRelationLevelTextToShip(TShip* Ship);
        std::uint8_t TryExtortShip(TShip* Target);
        void TruceWithShip(TShip* Ship);
        void RequestAlliesAttackShip(TShip* Target);
        std::uint8_t IsInPrison();
        std::int32_t GetPrisonTermRemaining();
        void ClearPrisonTerm();
        std::int32_t GetPrisonReleaseCost();
        aGalaxyStruct::PGoodsTradePriceEntry GetLocationGoodsEntry(std::uint8_t Good);
        std::int32_t ShopGoodsPurchasePrice(std::uint8_t Good, pas::Object* Location);
        std::int32_t ShopGoodsSellPrice(std::uint8_t Good, pas::Object* Location);
        double GetAverageCargoCost(std::uint8_t Good);
        void ConsumeCargoGoods(std::uint8_t Good, std::int32_t Count);
        std::uint8_t IsCargoGoodIllegalOnCurrentPlanet(std::uint8_t Good);
        void SellGoodsToLocation(std::uint8_t Good, std::int32_t Count);
        void BuyGoodsFromLocation(std::uint8_t Good, std::int32_t Count);
        void ProcessLiberationGroupRoute();
        void LeaveLiberationGroup();
        aItem::THull* GetHull();
        aItem::TFuelTanks* GetFuelTanks();
        aItem::TEngine* GetEngine();
        aItem::TRadar* GetRadar();
        aItem::TScaner* GetScanner();
        aItem::TRepairRobot* GetRepairRobot();
        aItem::TCargoHook* GetCargoHook();
        aItem::TDefGenerator* GetDefGenerator();
        std::uint8_t CanRepairArtefactsAtLocation();
        std::uint8_t CanUseEquipmentTech(aItem::TEquipment* Item);
        std::uint8_t CanRepairEquipmentTech(aItem::TEquipment* Item);
        std::uint8_t IsEquipmentUsable(aItem::TEquipment* Item);
        void EquipItem(aItem::TEquipment* Item);
        void UnequipSlot(std::uint8_t ItemType, std::int32_t WeaponIndex);
        void UnequipItem(aItem::TEquipment* Item);
        std::int32_t CalculateMass();
        std::int32_t CalculateEquippedMass(aItem::TEquipment* ItemForModule);
        std::uint8_t GetHullIntegrityPercent();
        std::int32_t GetArmor();
        std::int32_t GetJumpDestinationDistance();
        std::int32_t GetFullRefuelCost();
        std::int32_t GetAfterburnerWear();
        std::int32_t GetFuelLimitedJumpRange();
        std::int32_t GetJumpRange();
        std::uint8_t IsMicroModuleRaciallyRestricted(std::int32_t ModuleIndex);
        std::int32_t GetEquipmentStatBonus(std::uint8_t BonusKind, aItem::TEquipment* Item);
        std::int32_t GetTotalStatBonus(std::uint8_t BonusKind);
        std::int32_t GetRadarRange();
        std::int32_t GetScannerPower();
        std::uint8_t CanResolveObjectWithScanner(pas::Object* Target);
        void ApplyRepairDroidHealing();
        float GetCargoHookMinPullSpeed();
        float GetCargoHookMaxPullSpeed();
        std::int32_t GetCargoHookRange();
        std::int32_t GetCargoHookRangeSquared();
        std::int32_t GetBaseCargoHookPower();
        double GetDefenseDamageFactor();
        std::uint8_t GetDefensePercent();
        std::int32_t GetAttackMultiplier();
        void AddAward(std::uint8_t AwardId);
        void RefreshDerivedStats(std::uint8_t UpdateRelativeRatings);
        void RefreshGraphicSize();
        void RebuildEquipmentCache();
        std::int32_t GetCargoFreeSpace();
        std::int32_t GetCarriedItemWeight();
        std::int32_t GetCargoGoodsWeight();
        std::int32_t CalculateFollowRadius();
        std::uint8_t GetFollowMode();
        std::uint8_t GetEffectiveFollowMode();
        std::uint8_t NeedsEquipmentType(aConst::TItemType ItemType);
        std::int32_t CalculateEquippedItemCostWithoutHull();
        std::int32_t CountCarriedEquipmentByType(aConst::TItemType ItemType);
        aItem::TWeapon* SelectBestUnequippedWeapon();
        std::uint8_t HasLooseNonScriptItemsOrGoods();
        void DropUnequippedItemsAndGoods();
        void AddItemToPlayerStorage(aItem::TItem* Item, pas::Object* Location, std::int32_t Slot);
        void MergeItemIntoPlayerStorage(aItem::TItem* Item, pas::Object* Location, std::int32_t Slot);
        void AddGoodsToPlayerStorage(std::uint8_t Good, std::int32_t Quantity, std::int32_t Cost, pas::Object* Location, std::int32_t Slot);
        std::uint8_t StoreLooseInventoryAt(pas::Object* Location);
        std::uint8_t RetrieveStoredItems(pas::Object* Location);
        void AutoEquipArtefacts();
        float CalculateItemEffectiveness(aItem::TItem* Item);
        float EstimateWeaponDamageAgainstTypicalDefense(aItem::TItem* Item);
        std::int32_t GetWeaponRange(aItem::TWeapon* Weapon);
        std::int32_t CalculateHullArmor(aItem::THull* Hull);
        static std::int32_t GetItemFuelTankCapacity(aItem::TItem* Item);
        std::int32_t CalculateEngineSpeed(aItem::TEngine* Engine, std::uint8_t ApplyBrokenPenalty);
        std::int32_t CalculateEngineJumpRange(aItem::TEngine* Engine);
        std::int32_t CalculateRadarRange(aItem::TRadar* Radar);
        std::int32_t CalculateScannerPower(aItem::TScaner* Scanner);
        std::int32_t CalculateRepairPoints(aItem::TRepairRobot* RepairRobot);
        std::int32_t CalculateCargoHookPower(aItem::TCargoHook* CargoHook);
        float CalculateDefGeneratorFactor(aItem::TDefGenerator* DefGenerator);
        float GetEquipmentEvaluationSynergyBonus(aItem::TEquipment* Item);
        float GetWeaponArtefactDamageFactor(aItem::TWeapon* Weapon);
        void UpdateSpeedTrackingMetrics();
        void RefreshEquipmentEvaluationMetrics();
        float EvaluateItem(aItem::TItem* Item, std::uint8_t PriceMode);
        virtual float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness);
        virtual float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value);
        virtual float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage);
        virtual float EvaluateMicroModuleGain(aItem::TEquipment* Item, std::int32_t ModuleIndex);
        void AutoApplyMicroModules();
        void AutoEquipInventory();
        void DropCargoUntilNotOverloaded();
        void DropRandomCheapItemsOnDestruction(std::int32_t Count);
        void DropRandomValuableItemsOnDestruction(std::int32_t Count);
        void DropItemsForDominatorProgram(std::int32_t Count);
        std::uint8_t DropCarriedItemAsMovingLoot(aItem::TItem* Item);
        std::uint8_t DropCarriedArtefactAsMovingLoot(aItem::TArtefact* Item);
        void DropGuaranteedDeathDropItems();
        std::uint8_t CanDropTreasureMap();
        aPlanet::TPlanet* SelectTreasureMapPlanet();
        std::uint8_t TryDropTreasureMap();
        std::uint8_t DropItemIntoStar(aItem::TItem* Item);
        void DropAllArtefactsOnDestruction();
        void DropGoodsIntoSpace(std::uint8_t Good, std::int32_t Count);
        std::uint8_t JettisonCargoGoodsTowardTargetValue(std::int32_t TargetValue);
        void DropAllCargoGoods();
        void QueueMovingItemDrop(aItem::TItem* Item, std::uint8_t UseFlag);
        aItem::TItem* SelectLeastValuableInventoryItem();
        aItem::TArtefact* SelectLeastValuableArtefact();
        std::uint8_t SelectCheapestCargoGood();
        void OptimizeInventory();
        void LiquidateInventoryItem(aItem::TItem* Item);
        void LiquidateArtefact(aItem::TArtefact* Item);
        std::uint8_t RepairHullAtLocation();
        void ReloadWeaponAmmo();
        void ApplyCombatItemDegradation(double BaseDurabilityDamage);
        void ApplyArtefactUseDegradation(double BaseDurabilityDamage);
        void ApplyAfterburnerItemDegradation();
        std::uint8_t ApplyItemDegradation(aItem::TEquipment* Item, TItemDegradationKind Kind, double DurabilityDamage);
        std::uint8_t CanGenerateMicroModuleForLoadout();
        std::uint8_t CanGenerateSpecialHullModule();
        void ImproveRandomEquipment(std::uint8_t ResolveOverload);
        void GenerateAndApplyMicroModule(aItem::TEquipment* Item, std::uint8_t ResolveOverload);
        void GenerateExtraWeapon();
        std::uint8_t NeedsMicroModule(std::int32_t ModuleIndexPlusOne);
        std::int32_t CountUnequippedDominatorEquipment();
        std::int32_t GetSatelliteLimit();
        std::uint8_t IsEssentialInventoryItem(aItem::TItem* Item);
        std::uint8_t IsOptionalUtilityEquipment(aItem::TItem* Item);
        std::uint8_t NeedsEssentialEquipment();
        void RestoreEssentialEquipment();
        void BuyEquipmentAtLocation(std::uint8_t ForceGeneratedOffers);
        aItem::THull* CreateAndEquipHull(std::uint16_t Capacity, std::uint8_t Level, std::uint8_t Owner, std::int32_t Series, std::uint8_t PirateBuilt);
        std::int32_t SelectRandomHullSeries();
        aItem::TFuelTanks* CreateAndEquipFuelTanks(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        aItem::TEngine* CreateAndEquipEngine(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        aItem::TRadar* CreateAndEquipRadar(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        aItem::TScaner* CreateAndEquipScanner(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        aItem::TRepairRobot* CreateAndEquipRepairRobot(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        aItem::TCargoHook* CreateAndEquipCargoHook(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        aItem::TDefGenerator* CreateAndEquipDefGenerator(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        aItem::TWeapon* CreateAndEquipWeapon(std::uint8_t ItemType, std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner);
        std::uint8_t ScanForCollectableItems();
        void QueueItemsWithinPickupRange();
        std::uint8_t TryCollectBestFloatingItem(std::int32_t MaximumTravelTurns);
        std::int32_t GetReservedPickupWeight();
        std::uint8_t IsItemInPickupRange(aItem::TItem* Item);
        virtual std::uint8_t AcceptPickupItem(aItem::TItem* Item);
        std::uint8_t ShouldPickUpItem(aItem::TItem* Item);
        virtual std::uint8_t AcceptPickupDistance(aItem::TItem* Item, double Distance);
        void AddPickupTarget(aItem::TItem* Item, std::uint8_t Prioritize);
        void RemovePickupTarget(aItem::TItem* Item);
        void ClearPickupTargets();
        void RemoveInvalidPickupTargets();
        void TogglePickupTargets(std::uint8_t IgnoreRange);
        std::uint8_t HasPickupTarget(aItem::TItem* Item);
        std::int32_t CountOtherShipsTargetingItem(aItem::TItem* Item);
        std::uint8_t CanReachItemBeforeOtherShips(aItem::TItem* Item);
        EC_Struct::TPointF GetPickupApproachPosition(EC_Struct::TPointF ItemPosition);
        aItem::TItem* GetCurrentPickupItem();
        void ClearRecentlyDroppedItems();
        void AddRecentlyDroppedItem(aItem::TItem* Item);
        std::uint8_t IsRecentlyDroppedItem(aItem::TItem* Item);
        void OrderNone(std::uint8_t OverrideScriptOrder);
        void OrderMove(EC_Struct::TPointF Destination, std::uint8_t Absolute);
        EC_Struct::TPointF GetJumpDeparturePoint(aGalaxy::TStar* Destination);
        EC_Struct::TPointF GetArrivalPosition(aGalaxy::TStar* DestinationStar);
        static std::int32_t CalculateJumpTravelDays(aGalaxy::TStar* Origin, aGalaxy::TStar* Destination);
        void OrderJump(aGalaxy::TStar* Star, std::uint8_t Absolute);
        void OrderJumpHole(aGalaxy::THole* Hole, std::uint8_t Absolute);
        void OrderTeleport(aGalaxy::TStar* Star, EC_Struct::TPointF Destination, std::int32_t TransitionData, std::uint8_t Absolute);
        void OrderLanding(pas::Object* Location, std::uint8_t Absolute);
        void OrderTakeoff();
        void OrderFollowShip(TShip* Ship, std::uint8_t FollowMode, std::uint8_t Absolute);
        std::int32_t GetMovementPathTurnCount();
        void PrepareTurnMovement(std::int32_t StartStepIndex, std::uint8_t RecordFilm);
        std::uint8_t ProcessMovementStep(std::int32_t StepIndex, std::uint8_t RecordFilm);
        void ClearCompletedTakeoffOrHoleOrder(std::int32_t UnusedStepIndex, std::uint8_t UnusedRecordFilm);
        std::uint8_t IsTravelCompletionPathReady();
        void RepelFollowingShips();
        void RebuildMovePath();
        void BuildFullPathTo(EC_Struct::TPointF Destination);
        void BuildPlanetLandingPath();
        void BuildOrderMovementPath(std::int32_t MaximumNodes);
        void AppendPathToWithTurnPadding(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        void AppendPathTo(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        void AppendStarAvoidingPathWithTurnPadding(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        void AppendOrbitalPath(std::int32_t MaximumNodes);
        void AppendTurningPath(EC_Struct::TPointF Destination, std::uint8_t AvoidStar, std::int32_t MaximumNodes);
        void AppendStraightPath(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        void AppendHyperspaceTransitionPath(float Direction);
        void AppendStarAvoidingPath(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        void AppendCircularDetour(EC_Struct::TPointF Destination, std::int32_t MaximumNodes, double Radius);
        void ClearMovementPath();
        std::int32_t GetSlotCount(aConst::TShipSlotKind SlotKind);
        std::int32_t GetSlotCountForItemType(std::uint8_t ItemType);
        void ReassignActiveItemSlots(aConst::TItemType ItemType);
        void RefreshAssignedItemSlots();
        aItem::TEquipment* FindEquippedItemInSlot(std::uint8_t ItemType, std::int32_t SlotIndex);
        void RefreshInactiveItemSlotAssignments();
        std::int32_t CountUnequippedItemsInSlot(std::int32_t SlotIndex);
        std::int32_t FindFreeUnequippedSlot();
        void RepairDuplicateSatelliteTrajectoryIndices();
        std::int32_t GetSatelliteTrajectoryIndexLimit();
        std::int32_t FindFreeSatelliteTrajectoryIndex();
        aItem::TSatellite* FindSatelliteByTrajectoryIndex(std::int32_t Index);
        void InsertSatelliteTrajectoryIndex(std::int32_t Index);
        void RemoveEmptySatelliteTrajectoryIndex(std::int32_t Index);
        void CompactSatelliteTrajectoryIndices();
        void AssignSatelliteIndicesFromHoldOrder();
        void ArrangeHoldSatellitesByTrajectoryIndex();
        std::uint8_t UseDominatorTransmitter(aItem::TArtefactTransmitter* Artefact);
        std::uint8_t HasMatchingArtefactOrCustomItem(aItem::TItem* Item);
        std::int32_t CountActiveArtefacts(std::uint8_t ArtefactType);
        std::uint8_t HasEquippedArtefactOfSameUseGroup(aItem::TItem* Item);
        std::uint8_t CanBoostArtefact(std::uint8_t ArtefactType, aItem::TEquipment* Item, std::uint8_t IgnoreArtefactAvailability);
        void ApplyNanoArtefactRepair();
        std::uint8_t CanContactShip(TShip* OtherShip);
        pas::WideString LookupTalkText(const pas::WideString& Path);
        std::uint8_t OpenPlayerConversation(std::uint8_t RespectChameleon);
        std::uint8_t ShowPlayerDialogue(std::uint8_t Kind, const pas::WideString& Text, std::int32_t Amount);
        void NotifyMoneyDemand(TShip* OtherShip, pas::WideString Response, std::int32_t Amount);
        void NotifyCargoDemand(TShip* OtherShip, pas::WideString Response);
        void NotifyFearCargoDrop(TShip* OtherShip);
        void NotifyTruceOffer(TShip* OtherShip, pas::WideString Response, std::int32_t Amount);
        void NotifyAttackRequest(TShip* OtherShip, pas::WideString Response, TShip* Target);
        void NotifyPartnershipOffer(TShip* OtherShip, pas::WideString Response, std::int32_t Amount);
        void NotifyPartnerBreak(TShip* Leader);
        void NotifyPartnershipExpired(TShip* Leader);
        void NotifyPartnerRebellion(TShip* Leader);
        void ShowMessageToPlayer(pas::WideString Text);
        void NotifyPiratePartnerRelationBreak(TShip* Leader);
        void NotifyPiratePartnerRatingBreak(TShip* Leader);
        void NotifyPiratePartnershipExpired(TShip* Leader);
        void NotifyPiratePartnerRebellion(TShip* Leader);
        virtual std::uint8_t UnknownVirtualC0(void* Argument);
        std::int32_t CalculatePartnershipMonths(std::int32_t Amount, TShip* OtherShip);
        pas::WideString GetGreetingText();
        void InitializeScriptStateOrders();
        void ApplyScriptStateOrders();
        void UpdateScriptStateCompletionAndPickups();
        void ScriptNextDay();
        TShip* FindScriptFollowTarget();
        void GainExperience(std::int32_t Amount, std::uint8_t SourceKind);
        void RemoveExperience(std::int32_t Amount);
        void DepositCarriedNodes();
        std::uint8_t TrainSkill(TPilotSkill Skill);
        std::uint8_t CanTrainSkill(TPilotSkill Skill);
        std::uint8_t GetBaseSkillLevel(TPilotSkill Skill);
        std::uint8_t GetEffectiveSkillLevel(TPilotSkill Skill, std::uint8_t IgnoreStatusEffects);
        std::uint8_t GetRelativeStrengthCategory();
        std::uint8_t GetHullConditionCategory();
        std::uint8_t GetRangerRatingBand();
        std::uint8_t HasActiveDisease();
        std::int32_t CountActiveDiseases();
        std::uint8_t HasPresentDisease();
        std::int32_t CountPresentDiseases();
        std::uint8_t HasActiveStimulant();
        std::int32_t CountActiveStimulants();
        std::int32_t CountPresentDiseasesAndActiveStimulants();
        std::uint8_t HasDiseaseFromCurrentPlanet();
        std::uint8_t HasDiseaseFromCurrentDockedShip();
        std::uint8_t IsHealthEffectActive(std::int32_t Index);
        void SimulateNpcHealthEffects();
        std::uint8_t HasRadiationSickness();
        virtual std::int32_t CalculateSpeed();
        std::uint8_t IsFemaleHumanPilot();
        std::uint8_t UsesVeteranHumanRangerAppearance();
        TShip* SelectInterceptorTarget();
        void LaunchInterceptors();
        void ClearIncomingInterceptors();
        std::int32_t CountActiveInterceptorTargets();
        std::int32_t GetHullEnergyRegeneration();
        std::int32_t GetInterceptorDamage();
        std::int32_t GetInterceptorEnergyCost();
        std::uint8_t GetInterceptorPassCount();
        std::uint8_t HasScriptControl();
        std::uint8_t HasNoUsableWeapons();
        std::int32_t GetOwnStatBonus(std::uint8_t BonusKind);
        void SetStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value);
        std::int32_t FindCombatStatusEffect(TCombatStatusEffectType EffectType);
        void AddCombatStatusStrength(TCombatStatusEffectType EffectType, float Strength, TShip* Source);
        void ReduceCombatStatusStrength(TCombatStatusEffectType EffectType, float Amount);
        void ClearCombatStatusEffects();
        void DecayCombatStatusEffects();
        float GetShockStatusDecay(float Strength);
        float GetAcidStatusDecay(float UnusedStrength);
        float GetMagneticStatusDecay(float Strength);
        static float GetWeaponBlockStatusDecay(float Strength);
        static float GetDroidBlockStatusDecay(float Strength);
        static float GetBWBuffStatusDecay(float Strength);
        static float GetBWRepairDebuffStatusDecay(float Strength);
        void ClearCombatStatusSourceReferences(TShip* Source);
        float GetCombatStatusStrength(TCombatStatusEffectType EffectType);
        std::int32_t GetCombatStatusSourceId(TCombatStatusEffectType EffectType);
        pas::WideString GetCombatStatusDescription(std::int32_t& Count, std::uint8_t ShowStrength);
        virtual void UpdateAfterburnerState();
        virtual void RefreshCurrentStanding();
        std::uint8_t HasScriptStateText();
        std::uint8_t HasIndependentScriptFaction();
        std::uint8_t HasNamedScriptFaction();
        std::int32_t GetScriptStandingOverrideMode();
        std::int32_t ScriptItemsAct(std::uint8_t ActionType, pas::Object* Object1, pas::Object* Object2, std::int32_t Param);
        virtual std::uint8_t CanDock(TShip* Ship);
        virtual std::uint8_t CheckDockingPermission(TShip* Ship, pas::WideString& Response);
        std::uint8_t RelationToShip(TShip* Ship);
        virtual pas::WideString GetName() = 0;
        virtual pas::WideString GetFullName(const pas::WideString& Separator) = 0;
        virtual std::uint8_t GetGreetingShipCategory() = 0;
        virtual aGalaxy::TStar* GetHomeStar() = 0;
        virtual aGalaxyStruct::TRangerCareer GetDominantCareer() = 0;
        virtual std::uint8_t GetStrengthScaledPirateStatus() = 0;
        virtual void RefuelAtLocation() = 0;
        virtual void RepairBrokenEquipmentAtLocation() = 0;
        virtual void BuildReachablePlanetQueue() = 0;
        virtual std::uint8_t CanQueueReachablePlanet(aPlanet::TPlanet* Planet) = 0;
        virtual void SelectEnemyShipInStar() = 0;
        virtual void EngageEnemyShip() = 0;
        virtual std::uint8_t RelationToRanger(void* Ranger) = 0;
        virtual void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) = 0;
        virtual void ReactToAttack(TShip* Attacker) = 0;
        virtual std::uint8_t RelationToNonRanger(TShip* Ship) = 0;
        virtual std::uint8_t RecomputeFearState() = 0;
        virtual std::uint8_t AcceptsRansomDemandFrom(TShip* Ship) = 0;
        virtual std::uint8_t TrustsAttackRequester(TShip* Ship) = 0;
        virtual std::uint8_t EvaluateAllyRelationAndStrength(TShip* Ship) = 0;
        virtual void ProcessCombatDialogue() = 0;
        virtual void ReactToExtortionDemand(void* Ranger) = 0;
        virtual std::uint8_t BuildMoneyExtortionResponse(TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) = 0;
        virtual std::uint8_t BuildCargoExtortionResponse(TShip* OtherShip, pas::WideString& Response) = 0;
        virtual std::uint8_t BuildTrucePaymentResponse(TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) = 0;
        virtual std::uint8_t BuildAttackRequestResponse(TShip* Requester, pas::WideString& Response, TShip* Target) = 0;
        virtual std::uint8_t AcceptPartnershipOffer(TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) = 0;
        virtual std::uint8_t BuildPartnershipOfferResponse(TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) = 0;
        std::int32_t Id;
        pas::WideString Name;
        pas::WideString TypeNameOverrideKey;
        std::uint8_t TypeId;
        std::uint8_t OwnerId;
        std::uint8_t cpp_padding[2];
        EC_Struct::TPointF Position;
        aPlanet::TPlanet* CurrentPlanet;
        TShip* DockedTo;
        aGalaxy::TStar* CurrentStar;
        aGalaxy::TStar* TransitOriginStar;
        aPlanet::TPlanet* HomePlanet;
        pas::Array<TCargoGoodsEntry, 0, 7> CargoGoods;
        std::int32_t Wealth;
        float WealthInBestRanger;
        float Strength;
        float StrengthInBestRanger;
        float StrengthInAverageRanger;
        std::int32_t Speed;
        std::int32_t JumpRange;
        std::uint8_t cpp_padding_2[4];
        double DefenseDamageFactor;
        std::uint8_t HasInactiveDirectEquipment;
        std::uint8_t cpp_padding_3[3];
        std::int32_t CargoFreeSpace;
        std::uint32_t Seed;
        std::uint32_t RandomState;
        std::int32_t CreationTurn;
        std::int32_t LastProcessedTurn;
        std::int32_t Money;
        std::int32_t UnknownF4;
        aItem::THull* Hull;
        aItem::TFuelTanks* FuelTanks;
        aItem::TEngine* Engine;
        aItem::TRadar* Radar;
        aItem::TScaner* Scanner;
        aItem::TRepairRobot* RepairRobot;
        aItem::TCargoHook* CargoHook;
        aItem::TDefGenerator* DefGenerator;
        pas::Array<aItem::TWeapon*, 1, 5> Weapons;
        std::uint8_t WeaponCount;
        std::uint8_t UsableWeaponCount;
        pas::Array<std::uint8_t, 0, 5> BaseSkills;
        std::uint8_t cpp_padding_4[4];
        pas::Array<TCaptainHealthState, 1, 24> CaptainHealth;
        pas::Array<TCaptainHealthState, 1, 1> RadiationHealth;
        pas::List* CustomShipInfos;
        std::int32_t TradeLossBalance;
        std::int32_t TradeExperience;
        std::int32_t ContrabandProfit;
        std::uint8_t TechKnowledge;
        std::uint8_t cpp_padding_5[3];
        std::int32_t NodeReserve;
        std::int32_t TotalExperience;
        std::int32_t FreeExperience;
        std::int32_t DaysSincePlayerSeen;
        std::uint8_t InFear;
        std::uint8_t AfterburnerActive;
        std::uint8_t cpp_padding_6[2];
        aMyFunction::TObjectList* Inventory;
        aMyFunction::TObjectList* Artefacts;
        aMyFunction::TObjectList* GuaranteedDeathDropItems;
        pas::List* StatBonuses;
        pas::List* CombatStatusEffects;
        pas::Object* ScriptShip;
        pas::Object* LiberationGroup;
        std::int32_t LiberationGroupRouteIndex;
        pas::List* PickupTargets;
        pas::List* RecentlyDroppedItemIds;
        std::uint8_t PickupPathUpdatesAllowed;
        std::uint8_t cpp_padding_7[3];
        pas::List* PlanetQueue;
        pas::List* AwardIds;
        std::int32_t AwardVisibleCount;
        pas::List* RangerRelations;
        TShip* EnemyShip;
        TShip* TruceShip;
        TShip* PartnerShip;
        std::int32_t PartnershipDaysRemaining;
        std::int32_t PortraitFaceId;
        std::uint8_t PilotRace;
        std::uint8_t cpp_padding_8[7];
        double MovementSpeed;
        double MovementTurnRate;
        double MovementDirection;
        TShipOrder Order;
        std::uint8_t cpp_padding_9[3];
        std::int32_t OrderStateData;
        pas::Object* OrderTarget;
        EC_Struct::TPointF OrderDestination;
        std::uint8_t OrderAbsolute;
        std::uint8_t cpp_padding_10[3];
        aPath::TSPath* MovementPath;
        std::uint8_t JumpDeparturePathCommitted;
        std::uint8_t AbductedByPirateClan;
        std::uint8_t cpp_padding_11[2];
        std::int32_t ConsecutiveDockedDays;
        std::uint8_t AbsoluteScriptOrder;
        std::uint8_t cpp_padding_12[3];
        SE_Space::TObjectSE* Graphic;
        pas::WideString GraphName;
        std::uint8_t GraphDominator;
        std::uint8_t InHyperspace;
        std::uint8_t cpp_padding_13[2];
        float CollisionRadius;
        std::uint8_t DestroyQueued;
        std::uint8_t ChameleonActive;
        aGalaxyStruct::TDominatorSeries ChameleonSeries;
        std::uint8_t ChameleonVisualType;
        std::int32_t ChameleonDisplayCount;
        pas::Array<std::uint8_t, 0, 2> ChameleonDetected;
        std::uint8_t cpp_padding_14[1];
        pas::Array<std::int32_t, 0, 2> ChameleonCharges;
        std::uint8_t cpp_padding_15[4];
        EC_Struct::TPointF RepulsionPosition;
        std::uint8_t cpp_padding_16[4];
        float FilmAlpha;
        float FilmAlphaStep;
        std::uint32_t EncodedMoney;
        aEFilm::TEFilmObj* FilmObject;
        std::uint8_t NoDrop;
        std::uint8_t TargetingRestriction;
        std::uint8_t NoTalk;
        std::uint8_t NoScan;
        std::uint8_t ScriptChameleon;
        std::uint8_t PlayerExtortionPactActive;
        std::uint16_t PlayerScratchHitsReceived;
        std::int32_t InterceptorPassesRemaining;
        TShip* InterceptorSourceShip;
        SE_Space::TObjectSE* InterceptorGraphic;
        aEFilm::TEFilmObj* AuxiliaryFilmObject;
        std::uint8_t CurrentStanding;
        std::uint8_t cpp_padding_17[3];
        std::int32_t SmoothedSpeed;
        std::int32_t SmoothedEnemySpeed;
        float SmoothedEquipmentEffectiveness;
        std::int32_t SmoothedWealth;
        float SmoothedMoneyFraction;
        float SmoothedFreeCapacityFraction;
        float EquipmentPriceSensitivity;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TCombatStatusEffect {
        TCombatStatusEffectType EffectType;
        std::uint8_t cpp_padding[3];
        float Strength;
        std::int32_t SourceShipId;
    };
    #pragma pack(pop)

    using PCombatStatusEffect = TCombatStatusEffect*;

    #pragma pack(push, 1)
    struct TShipStatBonusEntry {
        std::uint8_t BonusKind;
        std::uint8_t cpp_padding[3];
        std::int32_t BonusValue;
    };
    #pragma pack(pop)

    using PShipStatBonusEntry = TShipStatBonusEntry*;

    #pragma pack(push, 1)
    struct TShipEquipmentCacheView {
        pas::Array<std::uint8_t, 0, 247> Prefix;
        pas::Array<aItem::TEquipment*, 42, 49> Slots;
    };
    #pragma pack(pop)

    using PShipEquipmentCacheView = TShipEquipmentCacheView*;

    #pragma pack(push, 1)
    struct TCustomShipInfo {
        pas::WideString TypeName;
        pas::WideString Description;
        pas::Array<std::int32_t, 1, 3> Data;
        pas::WideString TextData1;
        pas::WideString TextData2;
        pas::WideString TextData3;
        void* ActionCode;
        std::uint8_t StatusEffect;
        std::uint8_t ActionCodeInitialized;
        std::uint8_t DeleteQueued;
        std::uint8_t cpp_padding[1];
    };
    #pragma pack(pop)

    using PCustomShipInfo = TCustomShipInfo*;

} // namespace aShip
