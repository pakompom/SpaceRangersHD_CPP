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
        // Player purchases tracked separately for trade-profit experience.
        std::int32_t PurchasedCount;
        std::int32_t PurchasedTotalCost;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TCaptainHealthState {
        // 0: absent; 100: active.
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

    using TWeaponCount = std::uint8_t;

    using TPilotSkillLevel = std::uint8_t;

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
        // Editable subset; not a full save.
        virtual void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        // Applies edits to existing state and can append items or issue new orders. Cargo quantities change without updating cost basis.
        virtual void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        virtual void virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        // Clamps to 0..100000000.
        void SetMoney(std::int32_t Value);
        // Can cancel the player's jump when remaining fuel is insufficient.
        void ProcessBrokenFuelTankLeak();
        // Zero-byte pointer additions below retain native argument evaluation order
        // without narrowing object addresses to Integer.
        // Can unlock carried equipment use/repair and notify the player.
        void RefreshTechKnowledgeAtLocation();
        virtual void virtual_TShip_NextDay();
        virtual void virtual_TShip_NextDayLogic();
        // The base implementation clears all weapon targets.
        virtual void AssignWeaponTargetsInStar();
        // Native no-op, called after loading/editing and refreshing derived stats.
        static void DerivedStateCompatibilityHook();
        virtual pas::WideString GetTypeNameKey();
        pas::WideString GetLocalizedTypeName();
        pas::WideString GetFactionNameKey();
        std::uint8_t GetDefaultHullType();
        std::int32_t NextRandomInteger(std::int32_t Minimum, std::int32_t Maximum);
        virtual std::int32_t GetDesiredCargoFreeSpace();
        std::uint8_t IsHullDestroyed();
        // Fractional part of signed Seed divided by CurrentTurn + TurnOffset; denominator must be nonzero. Does not advance RandomState.
        float GetTurnSeedFraction(std::int32_t TurnOffset);
        std::uint8_t HasCargoGoods();
        // Counts positive cargo quantities.
        std::uint8_t CountCargoGoodsTypes();
        std::int32_t GetCarriedNodeCount();
        // True for OrderAbsolute, AbsoluteScriptOrder, or a follow-ship order.
        std::uint8_t HasLockedOrFollowOrder();
        std::uint8_t HasHullDamageOrBrokenEquippedItems();
        std::uint8_t CanRefuel();
        void UpdateBestRangerRelativeRatings();
        // Does not guard against zero AverageRangerStrength.
        void UpdateAverageRangerRelativeStrength();
        // Updates Wealth. Includes player storage and accrued deposit but does not subtract debt; capped at MaxInt.
        std::int32_t CalculateWealth();
        double CalculateAttackStrength();
        double CalculateDefenseStrength();
        double GetRepairStrengthFactor();
        // Updates Strength; calculating the player's strength also refreshes galaxy ranger strength statistics.
        double CalculateStrength();
        // Temporarily restores hull points. Leaves cached Strength and player galaxy-strength statistics at the full-hull values; byte result is not clamped.
        std::uint8_t GetFullHullRelativeStrengthPercent();
        // Uses cached Wealth and the configured scale table; no index validation.
        std::int32_t GetWealthScaledAmount(std::uint8_t ScaleIndex);
        // For the player, checks the script-binding list; for NPC ships, checks ScriptShip.
        std::uint8_t HasScriptBindings();
        // Leaves the result storage unchanged when Graphic is not a supported graphic class.
        pas::WideString GetShipPortraitImagePath();
        // Reads the player rather than Self; requires a player.
        static std::uint8_t HasPlayerChameleonCharges();
        std::uint8_t SelectChameleonVisualType();
        // Requires a player; action-17 script handlers can override the default result.
        std::uint8_t IsPlayerChameleonEffectiveAgainstSelf();
        void CreateNormalGraphic();
        void CreateDominatorGraphic();
        // Moves Self and docked ships between star lists; does not clear InHyperspace.
        void TransferToStar(aGalaxy::TStar* Star);
        // Includes artefacts, guaranteed drops, stored Tranclucator inventories and station shop stock; result is borrowed.
        aItem::TItem* FindCarriedItemById(std::uint32_t Id);
        // Copies the carrier's star and position, moving star-list membership when necessary. Does nothing without DockedTo.
        void SynchronizeDockedLocation();
        std::uint8_t InNormalSpace();
        // True without a current star, while docked/in hyperspace, or with a saved player ruins docking target.
        std::uint8_t IsOutsideStarSpace();
        std::uint8_t IsOnPlanet();
        std::uint8_t IsDockedToShip();
        std::uint8_t IsDocked();
        std::uint8_t HasPositiveSpeed();
        // Checks only ships in star space; clears the player auto-follow target when cancelling.
        void CancelInvalidTravelOrder();
        void ClearPlanetQueue();
        // Requires a non-nil queue ordered with current-star planets first; otherwise falls back to the first entry. Empty queue returns nil.
        aPlanet::TPlanet* SelectNearestQueuedPlanet();
        // Types 6..13 whose CanDock(Self) succeeds. Zero mask permits every standing; does not independently filter hyperspace/docking.
        TShip* FindNearestDockableStation(aGalaxyStruct::TStationStandingMask StandingMask);
        // Requires a nonempty planet list; returns the last planet if all are uninhabited.
        aPlanet::TPlanet* FindFirstInhabitedPlanetInStar();
        // Can build PlanetQueue and issue a landing or jump order. Result is borrowed and may be nil.
        aPlanet::TPlanet* NavigateToQueuedPlanet(std::uint8_t Absolute);
        // Uses the existing PlanetQueue; false means it is nil or empty. True does not guarantee that the order was accepted.
        std::uint8_t NavigateToEscapePlanet(std::uint8_t Absolute);
        // Requires a ranger PartnerShip. True reports handled travel, not necessarily a changed or accepted order.
        std::uint8_t TryMirrorPartnerTravelOrders();
        void OrderRandomFreeFlightMove();
        // False only when Target is jumping and its distance is at least twice Self.Speed; does not validate shared star or speed.
        std::uint8_t IsTargetStillPursuable(TShip* Target);
        // AI estimate using range, hull and travel time. Pursuer weapon range is evaluated using Self's bonuses.
        std::uint8_t CanEscapePursuer(TShip* Pursuer);
        std::uint8_t HasLandablePlanetInStar(aGalaxy::TStar* Star);
        // Minimum of raw tank Capacity and engine JumpRange; requires both items and ignores bonuses/condition.
        std::int32_t GetFullFuelBaseJumpRange();
        // Returns a borrowed intermediate star or nil. Uses raw full-fuel range and current distance caches; excludes sector 20. A direct reachable destination returns the current star.
        aGalaxy::TStar* FindNextStarTowardDestination(aGalaxy::TStar* Destination, std::uint8_t RequireFuelMargin);
        // Excludes Self but includes docked/hyperspace entries; no match returns sqrt(1000000000).
        double DistanceToNearestShipByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask);
        // Uses rounded distance / (Speed + 1) + 1; unsupported orders return zero.
        std::int32_t EstimateOrderTravelTurns();
        // Accepts planet, ship or star; a star estimates travel to the current system boundary, excluding hyperspace transit. Other classes return zero.
        std::int32_t EstimateTravelTurnsToObject(pas::Object* Target);
        // Returns -1 for zero speed, nil planet/star, or a different star.
        std::int32_t EstimateTravelTurnsToPlanet(aPlanet::TPlanet* Planet);
        // Weapon may be nil. Script handlers can change the target; a destroyed item may explode and be freed.
        void FireWeaponAtItem(aItem::TWeapon* Weapon, aItem::TItem* Target, std::uint8_t RecordFilm);
        // Ignores non-asteroid targets. Respawning preserves the target object but invalidates its previous motion and mineral reserve.
        void FireWeaponAtAsteroid(aItem::TWeapon* Weapon, pas::Object* Target, std::uint8_t RecordFilm);
        // Uses InterceptorSourceShip; absent source gives base damage 25. Returns ApplyDamage's signed result.
        std::int32_t ApplyInterceptorDamage(std::uint32_t& DamageColor);
        // Uses rounded shock strength and nonlethal flag 0x1000; returns ApplyDamage's signed result.
        std::int32_t ApplyShockStatusDamage(std::uint32_t& DamageColor);
        // Returns script-adjusted damage, which may exceed actual hull loss. DamageColor uses the current packed pixel format and is zero for nonpositive damage.
        std::int32_t ApplyAsteroidImpactDamage(aAsteroid::TAsteroid* Asteroid, std::uint32_t& DamageColor);
        // Also refuels up to five units inside the damage radius. Returns script-adjusted damage; Dominator bosses survive with at least one hull point.
        std::int32_t ApplyStarHeatDamage();
        TWeaponCount CountEquippedWeapons();
        // Counts torpedo/missile/rocket shot types; does not check usability or ammunition.
        TWeaponCount CountMissileWeapons();
        // Complement of CountMissileWeapons over cached equipped weapons.
        std::uint8_t CountDirectFireWeapons();
        void ClearUnequippedWeaponTargets();
        // Counts any intersection; does not check usability or ammunition.
        TWeaponCount CountWeaponsByDamageFlags(aGalaxyStruct::TDamageFlagSet Flags);
        pas::WideString GetManeuverabilitySummary();
        // Callers pass the target ship in EDX. This routine ignores it and only checks Self's active scanner artefact count.
        std::uint8_t HasScannerArtefact(TShip* UnusedTarget);
        // Weapon slots are numbered 1..5.
        std::int32_t GetWeaponSlotRange(std::int32_t SlotIndex);
        std::int32_t GetWeaponMinDamage(aItem::TWeapon* Weapon);
        std::int32_t GetWeaponMaxDamage(aItem::TWeapon* Weapon);
        std::int32_t GetMaxWeaponRange();
        // Nil clears every cached weapon target; otherwise clears only matches.
        void ClearWeaponTargets(pas::Object* Target);
        // Includes weapon targets, interceptor attribution and shock/acid source IDs; requires non-nil Target.
        std::uint8_t IsAttackingShip(TShip* Target);
        // Assigns both ships' enemies and eligible weapons, then issues pursuit orders where possible.
        void SetJointAttackTarget(TShip* Ally, TShip* Target);
        std::uint8_t IsEnemyPursuingSelf();
        // Rejects bomb/explosive cargo near non-hostile normal-space ships, including Self. Does not test shot range.
        std::uint8_t CanSafelyDetonateItem(aItem::TItem* Item);
        // Requires a non-nil relation list; nonempty lists require a registered ranger. Stores representative values 5, 20, 45, 70 or 90.
        void SetStoredRangerRelationLevel(TShip* Ranger, aGalaxyStruct::TRelationLevel Level);
        // Rangers and pirates only; other classes return zero.
        std::int32_t GetPrisonTermRemaining();
        void ClearPrisonTerm();
        // Zero without a positive term; otherwise at least 100, using cached Wealth.
        std::int32_t GetPrisonReleaseCost();
        // Result is borrowed; raises when no market is available.
        aGalaxyStruct::PGoodsTradePriceEntry GetLocationGoodsEntry(std::uint8_t Good);
        // Location is a planet or station; nil selects the current trade context.
        std::int32_t ShopGoodsPurchasePrice(std::uint8_t Good, pas::Object* Location);
        // Includes the Trading skill bonus. Location=nil selects the current trade context.
        std::int32_t ShopGoodsSellPrice(std::uint8_t Good, pas::Object* Location);
        // Returns 0 for an empty cargo entry.
        double GetAverageCargoCost(std::uint8_t Good);
        // Count is not clamped; remaining cost basis uses the previous average price.
        void ConsumeCargoGoods(std::uint8_t Good, std::int32_t Count);
        std::uint8_t IsCargoGoodIllegalOnCurrentPlanet(std::uint8_t Good);
        // Rejects Count above carried stock; does not reject a negative Count. Player trade losses offset later profit before trade experience is awarded.
        void SellGoodsToLocation(std::uint8_t Good, std::int32_t Count);
        // Checks stock and cash, but not free cargo space or negative Count.
        void BuyGoodsFromLocation(std::uint8_t Good, std::int32_t Count);
        // Requires an in-range route index when a group is assigned; may leave the group or issue travel/combat orders.
        void ProcessLiberationGroupRoute();
        // Requires a current liberation group.
        void LeaveLiberationGroup();
        aItem::THull* GetHull();
        aItem::TFuelTanks* GetFuelTanks();
        aItem::TEngine* GetEngine();
        aItem::TRadar* GetRadar();
        aItem::TScaner* GetScanner();
        aItem::TRepairRobot* GetRepairRobot();
        aItem::TCargoHook* GetCargoHook();
        aItem::TDefGenerator* GetDefGenerator();
        // Pirate/science bases, licensed station names and the main pirate planet; follows DockedTo recursively.
        std::uint8_t CanRepairArtefactsAtLocation();
        // Does not add Item to the inventory.
        void EquipItem(aItem::TEquipment* Item);
        // Direct types 42..49 require a populated slot; weapon types 50..68 require a valid one-based WeaponIndex. Compacts the weapon cache; does not remove/free inventory.
        void UnequipSlot(aConst::TItemType ItemType, std::int32_t WeaponIndex);
        // Only affects cached installed equipment. Requires non-nil Item.
        void UnequipItem(aItem::TEquipment* Item);
        // Uses cached CargoFreeSpace; includes cargo, artefact and status modifiers.
        std::int32_t CalculateMass();
        // Excludes cargo and unequipped items. Uses the current hull, adding ItemForModule's micromodule mass bonus when supplied.
        std::int32_t CalculateEquippedMass(aItem::TEquipment* ItemForModule);
        // Not clamped to 0..100.
        aGalaxyStruct::TPercent GetHullIntegrityPercent();
        std::int32_t GetArmor();
        // Truncated map distance for a jump order; zero otherwise.
        std::int32_t GetJumpDestinationDistance();
        // Zero without tanks. Uses current planet owner, or owner six off-planet; overfilled tanks can produce a negative cost.
        std::int32_t GetFullRefuelCost();
        // Uses engine owner and current turn; no engine gives one before artefact modifiers.
        std::int32_t GetAfterburnerWear();
        std::int32_t GetFuelLimitedJumpRange();
        // Ignores fuel; broken engines retain 60% range.
        std::int32_t GetJumpRange();
        std::int32_t GetTotalStatBonus(aConst::TEquipmentBonusKind BonusKind);
        std::int32_t GetRadarRange();
        std::int32_t GetScannerPower();
        // Ignores radar range and Dominator scanner series; non-ship targets require only a usable scanner.
        std::uint8_t CanResolveObjectWithScanner(pas::Object* Target);
        void ApplyRepairDroidHealing();
        float GetCargoHookMinPullSpeed();
        float GetCargoHookMaxPullSpeed();
        std::int32_t GetCargoHookRange();
        std::int32_t GetCargoHookRangeSquared();
        // Raw PickupPower; zero without a hook. Does not check usability.
        std::int32_t GetBaseCargoHookPower();
        // 1 means no damage reduction.
        double GetDefenseDamageFactor();
        aGalaxyStruct::TPercent GetDefensePercent();
        std::int32_t GetAttackMultiplier();
        // Rejects ID 255 and appends without deduplication; stops when list count equals 255. Extends the visible prefix only when all previous awards were visible.
        void AddAward(std::uint8_t AwardId);
        void RefreshDerivedStats(std::uint8_t UpdateRelativeRatings);
        // Requires Graphic; chooses dimensions from ship class, hull and special equipment.
        void RefreshGraphicSize();
        // Can unequip items whose slots are unavailable.
        void RebuildEquipmentCache();
        // Can be negative.
        std::int32_t GetCargoFreeSpace();
        // Excludes the hull.
        std::int32_t GetCarriedItemWeight();
        std::int32_t GetCargoGoodsWeight();
        // Requires a follow order; uses weapon ranges or the ships' collision radii.
        std::int32_t CalculateFollowRadius();
        // Raises when the current order is not follow.
        std::uint8_t GetFollowMode();
        // Requires a follow order; applies tactical and map-edge adjustments without modifying OrderStateData.
        std::uint8_t GetEffectiveFollowMode();
        std::uint8_t NeedsEquipmentType(aConst::TItemType ItemType);
        // Includes equipped artefacts.
        std::int32_t CalculateEquippedItemCostWithoutHull();
        // Skips inventory index zero; any weapon request counts all weapon types.
        std::int32_t CountCarriedEquipmentByType(aConst::TItemType ItemType);
        aItem::TWeapon* SelectBestUnequippedWeapon();
        std::uint8_t HasLooseNonScriptItemsOrGoods();
        // Preserves named script items; drops through the normal item/artefact helpers.
        void DropUnequippedItemsAndGoods();
        // Always adds to the player's storage, even when Self is an NPC. Caller must detach the item from its previous owner. Negative Slot allocates a free slot.
        void AddItemToPlayerStorage(aItem::TItem* Item, pas::Object* Location, std::int32_t Slot);
        // Transfers ownership; a successful goods/countable merge frees Item.
        void MergeItemIntoPlayerStorage(aItem::TItem* Item, pas::Object* Location, std::int32_t Slot);
        // Positive quantities only; Cost is the total cost basis, not a unit price.
        void AddGoodsToPlayerStorage(std::uint8_t Good, std::int32_t Quantity, std::int32_t Cost, pas::Object* Location, std::int32_t Slot);
        // Uses the player's storage filters. Returns true for an eligible location even if nothing was moved.
        std::uint8_t StoreLooseInventoryAt(pas::Object* Location);
        // Transfers matching player storage entries into Self; returns location eligibility, not whether items were retrieved.
        std::uint8_t RetrieveStoredItems(pas::Object* Location);
        void AutoEquipArtefacts();
        // Temporarily changes weapon installation and disables ChaoticRandom while evaluating equipment; restores them on the normal path.
        float CalculateItemEffectiveness(aItem::TItem* Item);
        // Returns one for a non-weapon; adjusts weapon damage for galaxy technology and accuracy.
        float EstimateWeaponDamageAgainstTypicalDefense(aItem::TItem* Item);
        // Zero unless Item is fuel tanks.
        static std::int32_t GetItemFuelTankCapacity(aItem::TItem* Item);
        float GetWeaponArtefactDamageFactor(aItem::TWeapon* Weapon);
        void UpdateSpeedTrackingMetrics();
        void RefreshEquipmentEvaluationMetrics();
        float EvaluateItem(aItem::TItem* Item, std::uint8_t PriceMode);
        // Price modes: 1 negated item cost, 3 resale value, 4 item cost; other modes omit the price term. Mode 0 also omits weight/fragility penalties; the supplied effectiveness is recomputed.
        virtual float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness);
        virtual float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value);
        virtual float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage);
        // Evaluates a temporary clone; restores NextItemId but leaves LoadedSaveVersion set to CurrentSaveVersion. ModuleIndex is zero-based.
        virtual float EvaluateMicroModuleGain(aItem::TEquipment* Item, std::int32_t ModuleIndex);
        // Consumes beneficial carried modules, preferring installed equipment; refreshes derived stats after each application.
        void AutoApplyMicroModules();
        void AutoEquipInventory();
        void DropRandomCheapItemsOnDestruction(std::int32_t Count);
        void DropRandomValuableItemsOnDestruction(std::int32_t Count);
        // Requires a Dominator; used by dialogue and Script.DomikProgramm. Attempts at most 100 selections.
        void DropItemsForDominatorProgram(std::int32_t Count);
        // Requires membership in Inventory; honors ship/item NoDrop and rejects the hull. True means accepted, even if a script consumes the drop.
        std::uint8_t DropCarriedItemAsMovingLoot(aItem::TItem* Item);
        // Requires membership in Artefacts; honors ship/item NoDrop.
        std::uint8_t DropCarriedArtefactAsMovingLoot(aItem::TArtefact* Item);
        void DropGuaranteedDeathDropItems();
        // Searches reachable uninhabited planets with accessible artefact/module loot; excludes maps already carried or stored by the player.
        aPlanet::TPlanet* SelectTreasureMapPlanet();
        // Player drops are placed immediately and may be consumed by scripts or stellar heat. Native artefact branch mistakenly deletes Inventory at the Artefacts index (); preserved here as observed behavior.
        std::uint8_t DropItemIntoStar(aItem::TItem* Item);
        void DropAllArtefactsOnDestruction();
        std::uint8_t JettisonCargoGoodsTowardTargetValue(std::int32_t TargetValue);
        // Considers protection, essential equipment and value per mass; can return the hull when no alternative qualifies.
        aItem::TItem* SelectLeastValuableInventoryItem();
        // Native scan starts at index one; an artefact list with one element yields nil.
        aItem::TArtefact* SelectLeastValuableArtefact();
        // Returns 255 if no cargo qualifies.
        std::uint8_t SelectCheapestCargoGood();
        // Can sell equipment, artefacts and excess cargo; outside a market, excess goods are refunded at their cost basis.
        void OptimizeInventory();
        // Removes and frees Item after crediting resale value.
        void LiquidateArtefact(aItem::TArtefact* Item);
        // Returns whether AI should remain docked; false does not guarantee full repair. Does not charge Money.
        std::uint8_t RepairHullAtLocation();
        void ReloadWeaponAmmo();
        void ApplyCombatItemDegradation(double BaseDurabilityDamage);
        void ApplyArtefactUseDegradation(double BaseDurabilityDamage);
        void ApplyAfterburnerItemDegradation();
        std::uint8_t CanGenerateMicroModuleForLoadout();
        // True for the player or when the current hull already has a special module.
        std::uint8_t CanGenerateSpecialHullModule();
        void ImproveRandomEquipment(std::uint8_t ResolveOverload);
        // Requires an installable target and no carried module of the same index.
        std::uint8_t NeedsMicroModule(std::int32_t ModuleIndexPlusOne);
        // Skips inventory index 0.
        std::int32_t CountUnequippedDominatorEquipment();
        std::int32_t GetSatelliteLimit();
        // Includes protected/script-named items and indispensable installed equipment; nil is false.
        std::uint8_t IsEssentialInventoryItem(aItem::TItem* Item);
        // Radar/scanner, or cargo hook on a ship other than a ranger or pirate. Nil is false.
        std::uint8_t IsOptionalUtilityEquipment(aItem::TItem* Item);
        std::uint8_t NeedsEssentialEquipment();
        // Can subsidize equipment purchases; does not guarantee success.
        void RestoreEssentialEquipment();
        // Normal ships use PilotRace rather than OwnerId; selects rarity 1..100 through the galaxy RNG.
        std::int32_t SelectRandomHullSeries();
        aItem::TFuelTanks* CreateAndEquipFuelTanks(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        aItem::TEngine* CreateAndEquipEngine(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        aItem::TRadar* CreateAndEquipRadar(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        aItem::TScaner* CreateAndEquipScanner(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        aItem::TRepairRobot* CreateAndEquipRepairRobot(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        aItem::TCargoHook* CreateAndEquipCargoHook(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        aItem::TDefGenerator* CreateAndEquipDefGenerator(std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        aItem::TWeapon* CreateAndEquipWeapon(aConst::TItemType ItemType, std::int32_t Weight, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner);
        std::uint8_t ScanForCollectableItems();
        void QueueItemsWithinPickupRange();
        // May queue nearby pickups and issue/cancel a move order; true means a move order remains.
        std::uint8_t TryCollectBestFloatingItem(std::int32_t MaximumTravelTurns);
        std::int32_t GetReservedPickupWeight();
        // Base implementation returns false.
        virtual std::uint8_t AcceptPickupItem(aItem::TItem* Item);
        std::uint8_t ShouldPickUpItem(aItem::TItem* Item);
        // Base implementation returns true.
        virtual std::uint8_t AcceptPickupDistance(aItem::TItem* Item, double Distance);
        // Existing targets keep their position.
        void AddPickupTarget(aItem::TItem* Item, std::uint8_t Prioritize);
        void RemovePickupTarget(aItem::TItem* Item);
        void ClearPickupTargets();
        // Frees the target list when it becomes empty.
        void RemoveInvalidPickupTargets();
        // Adds missing eligible targets; removes eligible targets only if none were added.
        void TogglePickupTargets(std::uint8_t IgnoreRange);
        std::uint8_t HasPickupTarget(aItem::TItem* Item);
        std::int32_t CountOtherShipsTargetingItem(aItem::TItem* Item);
        // Ties are allowed; requires positive speed.
        std::uint8_t CanReachItemBeforeOtherShips(aItem::TItem* Item);
        EC_Struct::TPointF GetPickupApproachPosition(EC_Struct::TPointF ItemPosition);
        aItem::TItem* GetCurrentPickupItem();
        void ClearRecentlyDroppedItems();
        void AddRecentlyDroppedItem(aItem::TItem* Item);
        std::uint8_t IsRecentlyDroppedItem(aItem::TItem* Item);
        // Except OrderJumpHole, new orders respect AbsoluteScriptOrder.
        void OrderNone(std::uint8_t OverrideScriptOrder);
        void OrderMove(EC_Struct::TPointF Destination, std::uint8_t Absolute);
        EC_Struct::TPointF GetJumpDeparturePoint(aGalaxy::TStar* Destination);
        // Point on the destination map boundary facing the current system.
        EC_Struct::TPointF GetArrivalPosition(aGalaxy::TStar* DestinationStar);
        // Minimum two days; independent of equipment and fuel.
        static std::int32_t CalculateJumpTravelDays(aGalaxy::TStar* Origin, aGalaxy::TStar* Destination);
        void OrderJump(aGalaxy::TStar* Star, std::uint8_t Absolute);
        // Does not check AbsoluteScriptOrder.
        void OrderJumpHole(aGalaxy::THole* Hole, std::uint8_t Absolute);
        void OrderTeleport(aGalaxy::TStar* Star, EC_Struct::TPointF Destination, std::int32_t TransitionData, std::uint8_t Absolute);
        // Location is a planet or dockable ship.
        void OrderLanding(pas::Object* Location, std::uint8_t Absolute);
        void OrderTakeoff();
        void OrderFollowShip(TShip* Ship, std::uint8_t FollowMode, std::uint8_t Absolute);
        // Ceiling of active path-node count times the star's MovementStepScale.
        std::int32_t GetMovementPathTurnCount();
        void PrepareTurnMovement(std::int32_t StartStepIndex, std::uint8_t RecordFilm);
        // The native result remains false.
        std::uint8_t ProcessMovementStep(std::int32_t StepIndex, std::uint8_t RecordFilm);
        void ClearCompletedTakeoffOrHoleOrder(std::int32_t UnusedStepIndex, std::uint8_t UnusedRecordFilm);
        // Tests landing, jump, hole and teleport completion conditions against the prepared path.
        std::uint8_t IsTravelCompletionPathReady();
        // Adjusts RepulsionPosition on Self and nearby following ships using their collision radii.
        void RepelFollowingShips();
        // Uses the current turn's step limit; rewrites OrderDestination to the resulting endpoint.
        void RebuildMovePath();
        // Clears the old path and uses aGroup, a 999999-node limit.
        void BuildFullPathTo(EC_Struct::TPointF Destination);
        void BuildPlanetLandingPath();
        // Clears the path, normalizes heading and handles the current order; may commit jump departure or adjust the destination.
        void BuildOrderMovementPath(std::int32_t MaximumNodes);
        // Turning then straight movement; may pad the player's visible turn to 200 nodes.
        void AppendPathToWithTurnPadding(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        void AppendPathTo(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        void AppendStarAvoidingPathWithTurnPadding(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        // Appends up to 200 nodes around the system origin, with visible-turn padding.
        void AppendOrbitalPath(std::int32_t MaximumNodes);
        void AppendTurningPath(EC_Struct::TPointF Destination, std::uint8_t AvoidStar, std::int32_t MaximumNodes);
        void AppendStraightPath(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        // Positive/negative Direction selects the outgoing/incoming transition path.
        void AppendHyperspaceTransitionPath(float Direction);
        void AppendStarAvoidingPath(EC_Struct::TPointF Destination, std::int32_t MaximumNodes);
        void AppendCircularDetour(EC_Struct::TPointF Destination, std::int32_t MaximumNodes, double Radius);
        void ClearMovementPath();
        std::int32_t GetSlotCount(aConst::TShipSlotKind SlotKind);
        std::int32_t GetSlotCountForItemType(aConst::TItemType ItemType);
        // Repairs duplicate/out-of-range equipped slots; unequips overflow and preserves slot flag bit 7.
        void ReassignActiveItemSlots(aConst::TItemType ItemType);
        void RefreshAssignedItemSlots();
        // Slot indices are zero-based.
        aItem::TEquipment* FindEquippedItemInSlot(aConst::TItemType ItemType, std::int32_t SlotIndex);
        void RefreshInactiveItemSlotAssignments();
        std::int32_t CountUnequippedItemsInSlot(std::int32_t SlotIndex);
        std::int32_t FindFreeUnequippedSlot();
        void RepairDuplicateSatelliteTrajectoryIndices();
        // One past the largest carried satellite index, or zero.
        std::int32_t GetSatelliteTrajectoryIndexLimit();
        std::int32_t FindFreeSatelliteTrajectoryIndex();
        aItem::TSatellite* FindSatelliteByTrajectoryIndex(std::int32_t Index);
        // Shifts indices at or above Index upward.
        void InsertSatelliteTrajectoryIndex(std::int32_t Index);
        // Does nothing if Index is occupied.
        void RemoveEmptySatelliteTrajectoryIndex(std::int32_t Index);
        void CompactSatelliteTrajectoryIndices();
        // Uses and refreshes global hold-view state.
        void AssignSatelliteIndicesFromHoldOrder();
        // Reorders the satellite entries in the global hold view.
        void ArrangeHoldSatellitesByTrajectoryIndex();
        // Requires minimum charge; records a galaxy event and consumes charge even if no Dominators respond.
        std::uint8_t UseDominatorTransmitter(aItem::TArtefactTransmitter* Artefact);
        // Custom items match by configuration name; generic artefacts also compare names.
        std::uint8_t HasMatchingArtefactOrCustomItem(aItem::TItem* Item);
        // Uses custom SharedEffect types and excludes broken items. Activation exceptions can count some unequipped artefacts.
        std::int32_t CountActiveArtefacts(aConst::TItemType ArtefactType);
        // Uses custom SharedUse and ConfigBlockName. Includes Item itself if equipped, and does not exclude broken items.
        std::uint8_t HasEquippedArtefactOfSameUseGroup(aItem::TItem* Item);
        // Item=nil checks cached installed equipment. A supplied item need not be equipped; eligible equipment types depend on ArtefactType.
        std::uint8_t CanBoostArtefact(aConst::TItemType ArtefactType, aItem::TEquipment* Item, std::uint8_t IgnoreArtefactAvailability);
        // Prefers installed inventory equipment; selects at most one repairable item and may clear BrokenFlag.
        void ApplyNanoArtefactRepair();
        // Uses Self's radar with a 500-unit minimum and both NoTalk flags; does not test system membership.
        std::uint8_t CanContactShip(TShip* OtherShip);
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
        // Rejects truce, extortion and protection negotiations for faction enemies. Base returns false; warrior/pirate overrides inspect OtherShip.CurrentStanding and system control.
        virtual std::uint8_t RefusesFactionNegotiation(TShip* OtherShip);
        // Payment/wealth and relation determine contract months; a player stimulant can double the result.
        std::int32_t CalculatePartnershipMonths(std::int32_t Amount, TShip* OtherShip);
        // Requires ScriptShip; clears EndState, applies state orders and refreshes completion/pickup state.
        void InitializeScriptStateOrders();
        // Requires ScriptShip; may issue travel orders and assign script-selected weapon targets.
        void ApplyScriptStateOrders();
        // Requires ScriptShip; updates EndState and queues state-requested pickups.
        void UpdateScriptStateCompletionAndPickups();
        // Requires ScriptShip; script execution can remove the binding.
        void ScriptNextDay();
        // Requires ScriptShip; matches the state's group in the current system.
        TShip* FindScriptFollowTarget();
        // Source kind 0 bypasses diminishing returns.
        void GainExperience(std::int32_t Amount, std::uint8_t SourceKind);
        // Subtracts independently from total and free experience, capped at each current balance.
        void RemoveExperience(std::int32_t Amount);
        // Deposits every carried stack and awards experience.
        void DepositCarriedNodes();
        std::uint8_t TrainSkill(aGalaxyStruct::TPilotSkill Skill);
        std::uint8_t CanTrainSkill(aGalaxyStruct::TPilotSkill Skill);
        std::uint8_t GetBaseSkillLevel(aGalaxyStruct::TPilotSkill Skill);
        // Clamps to 0..6; equipment bonuses still apply when status effects are ignored.
        TPilotSkillLevel GetEffectiveSkillLevel(aGalaxyStruct::TPilotSkill Skill, std::uint8_t IgnoreStatusEffects);
        // Returns a category from 1 to 5 using StrengthInBestRanger.
        std::uint8_t GetRelativeStrengthCategory();
        // Categories 1..5 split rounded hull percentage at 20, 50, 70 and 90.
        std::uint8_t GetHullConditionCategory();
        // Zero for non-rangers; otherwise 1..5 from the rounded experience-rank percentile. Uses all galaxy rangers, including excluded entries.
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
        // Captain effect is active only when Progress equals 100.
        std::uint8_t IsHealthEffectActive(std::int32_t Index);
        void SimulateNpcHealthEffects();
        std::uint8_t HasRadiationSickness();
        virtual std::int32_t CalculateSpeed();
        // Human portrait IDs 25..32 on normal NPC ships; excludes special simulation mode.
        std::uint8_t IsFemaleHumanPilot();
        // Native resource key RangerOldFag; deterministic ID/creation-turn selection, excluding female pilots and the player.
        std::uint8_t UsesVeteranHumanRangerAppearance();
        TShip* SelectInterceptorTarget();
        // Clears an explicit target before checking energy; successful launch installs source/pass state and a graphic on the target.
        void LaunchInterceptors();
        // Clears source/pass state and releases the interceptor graphic.
        void ClearIncomingInterceptors();
        // Counts ships referring to Self as interceptor source across all galaxy systems.
        std::int32_t CountActiveInterceptorTargets();
        std::int32_t GetHullEnergyRegeneration();
        std::int32_t GetInterceptorDamage();
        std::int32_t GetInterceptorEnergyCost();
        // Hull override or five when zero.
        std::uint8_t GetInterceptorPassCount();
        std::uint8_t HasScriptControl();
        std::uint8_t HasNoUsableWeapons();
        std::int32_t GetOwnStatBonus(aConst::TEquipmentBonusKind BonusKind);
        // Zero removes and frees the matching bonus entry; nonzero inserts or replaces it.
        void SetStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value);
        // Returns the list index, or -1.
        std::int32_t FindCombatStatusEffect(TCombatStatusEffectType EffectType);
        // Scales by hull and existing strength. Always replaces the source ID, including clearing it for nil Source; no sign validation.
        void AddCombatStatusStrength(TCombatStatusEffectType EffectType, float Strength, TShip* Source);
        // Removes only entries reduced below zero; exactly zero remains. Negative Amount increases strength.
        void ReduceCombatStatusStrength(TCombatStatusEffectType EffectType, float Amount);
        // Frees entries but keeps the list.
        void ClearCombatStatusEffects();
        // Daily decay; exactly zero remains until a subsequent reduction.
        void DecayCombatStatusEffects();
        float GetAcidStatusDecay(float UnusedStrength);
        float GetMagneticStatusDecay(float Strength);
        static float GetWeaponBlockStatusDecay(float Strength);
        static float GetDroidBlockStatusDecay(float Strength);
        static float GetBWBuffStatusDecay(float Strength);
        static float GetBWRepairDebuffStatusDecay(float Strength);
        // Requires non-nil Source; clears matching IDs without changing effect strengths.
        void ClearCombatStatusSourceReferences(TShip* Source);
        // Zero when absent.
        float GetCombatStatusStrength(TCombatStatusEffectType EffectType);
        // Zero when absent or unattributed.
        std::int32_t GetCombatStatusSourceId(TCombatStatusEffectType EffectType);
        // Includes rounded-positive shock, acid, magnetic, BW buff and custom status entries; omits transient blocking effects.
        pas::WideString GetCombatStatusDescription(std::int32_t& Count, std::uint8_t ShowStrength);
        // Native base implementation is a no-op.
        virtual void UpdateAfterburnerState();
        virtual void virtual_TShip_RefreshCurrentStanding();
        std::uint8_t HasScriptStateText();
        // Requires a nonempty faction not beginning with SubFaction. The native substring result is used as Boolean, so absence also returns true.
        std::uint8_t HasIndependentScriptFaction();
        // Requires a nonempty faction other than the exact SubFactionFixedStanding marker.
        std::uint8_t HasNamedScriptFaction();
        // 0 normal, 1 independent faction, 2 fixed standing. The SubFaction substring test accepts absence as mode one.
        std::int32_t GetScriptStandingOverrideMode();
        // Returns Param after script handlers modify it; object slots may carry event-specific integer values.
        std::int32_t ScriptItemsAct(std::uint8_t ActionType, pas::Object* Object1, pas::Object* Object2, std::int32_t Param);
        virtual std::uint8_t virtual_TShip_CanDock(TShip* Ship);
        // Base implementation clears Response and returns false.
        virtual std::uint8_t CheckDockingPermission(TShip* Ship, pas::WideString& Response);
        // Abstract entries in native TShip VMT share the RTL stub.
        // Signatures come from verified concrete overrides and their callers.
        virtual pas::WideString GetName() = 0;
        virtual pas::WideString GetFullName(const pas::WideString& Separator) = 0;
        // Category bit in ship-greeting ShipType, ToShipType and ShipBadType filters.
        virtual std::uint8_t GetGreetingShipCategory() = 0;
        virtual aGalaxy::TStar* GetHomeStar() = 0;
        virtual aGalaxyStruct::TRangerCareer GetDominantCareer() = 0;
        virtual aGalaxyStruct::TPercent GetStrengthScaledPirateStatus() = 0;
        virtual void RefuelAtLocation() = 0;
        virtual void RepairBrokenEquipmentAtLocation() = 0;
        virtual void BuildReachablePlanetQueue() = 0;
        virtual std::uint8_t virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) = 0;
        virtual void SelectEnemyShipInStar() = 0;
        virtual void EngageEnemyShip() = 0;
        virtual std::uint8_t RelationToRanger(void* Ranger) = 0;
        virtual void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) = 0;
        virtual void ReactToAttack(TShip* Attacker) = 0;
        virtual std::uint8_t RelationToNonRanger(TShip* Ship) = 0;
        virtual std::uint8_t virtual_TShip_RecomputeFearState() = 0;
        virtual std::uint8_t virtual_TShip_AcceptsRansomDemandFrom(TShip* Ship) = 0;
        virtual std::uint8_t virtual_TShip_TrustsAttackRequester(TShip* Ship) = 0;
        // Relation/strength gate for protecting a ship or sparing pickup targets; other dialogue conditions are checked by the caller.
        virtual std::uint8_t AcceptsAppealFrom(TShip* Ship) = 0;
        virtual void ProcessCombatDialogue() = 0;
        virtual void ReactToExtortionDemand(void* Ranger) = 0;
        virtual std::uint8_t virtual_TShip_BuildMoneyExtortionResponse(TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) = 0;
        virtual std::uint8_t virtual_TShip_BuildCargoExtortionResponse(TShip* OtherShip, pas::WideString& Response) = 0;
        virtual std::uint8_t BuildTrucePaymentResponse(TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) = 0;
        virtual std::uint8_t virtual_TShip_BuildAttackRequestResponse(TShip* Requester, pas::WideString& Response, TShip* Target) = 0;
        virtual std::uint8_t virtual_TShip_AcceptPartnershipOffer(TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) = 0;
        virtual std::uint8_t virtual_TShip_BuildPartnershipOfferResponse(TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) = 0;
        std::int32_t Id;
        pas::WideString Name;
        pas::WideString TypeNameOverrideKey;
        // st* ship codes and TStationType station codes are declared in aGalaxyStruct.
        std::uint8_t TypeId;
        aGalaxyStruct::TOwnerId OwnerId;
        std::uint8_t cpp_padding[2];
        EC_Struct::TPointF Position;
        aPlanet::TPlanet* CurrentPlanet;
        // Includes TRuins stations.
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
        // Cached fuel-limited range.
        std::int32_t JumpRange;
        std::uint8_t cpp_padding_2[4];
        // 1 means no damage reduction.
        double DefenseDamageFactor;
        std::uint8_t HasInactiveDirectEquipment;
        std::uint8_t cpp_padding_3[3];
        std::int32_t CargoFreeSpace;
        // Stable seed used for repeatable choices.
        std::uint32_t Seed;
        std::uint32_t RandomState;
        std::int32_t CreationTurn;
        std::int32_t LastProcessedTurn;
        std::int32_t Money;
        // Serialized after LastProcessedTurn; meaning unresolved.
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
        // 1..12: diseases; 13..24: stimulants.
        pas::Array<TCaptainHealthState, 1, 24> CaptainHealth;
        // Same native record as diseases/stimulants.
        pas::Array<TCaptainHealthState, 1, 1> RadiationHealth;
        // Owns PCustomShipInfo records; deletion may be deferred during action callbacks.
        pas::List* CustomShipInfos;
        std::int32_t TradeLossBalance;
        std::int32_t TradeExperience;
        std::int32_t ContrabandProfit;
        std::uint8_t TechKnowledge;
        std::uint8_t cpp_padding_5[3];
        std::int32_t NodeReserve;
        std::int32_t TotalExperience;
        std::int32_t FreeExperience;
        // Reset for ships in normal space in the player's current system.
        std::int32_t DaysSincePlayerSeen;
        std::uint8_t InFear;
        std::uint8_t AfterburnerActive;
        std::uint8_t cpp_padding_6[2];
        // Owns its items; index 0 holds the hull.
        aMyFunction::TObjectList* Inventory;
        aMyFunction::TObjectList* Artefacts;
        // Owned TItem entries, transferred to space on destruction.
        aMyFunction::TObjectList* GuaranteedDeathDropItems;
        // PShipStatBonusEntry elements.
        pas::List* StatBonuses;
        // Optional; owns PCombatStatusEffect entries.
        pas::List* CombatStatusEffects;
        pas::Object* ScriptShip;
        // Borrowed TGroup; native consumers use checked casts.
        pas::Object* LiberationGroup;
        std::int32_t LiberationGroupRouteIndex;
        // TItem entries.
        pas::List* PickupTargets;
        pas::List* RecentlyDroppedItemIds;
        // Suppresses pickup rerouting during final travel approaches unless a queued item lies along the path.
        std::uint8_t PickupPathUpdatesAllowed;
        std::uint8_t cpp_padding_7[3];
        // TPlanet entries.
        pas::List* PlanetQueue;
        // Integer medal IDs stored directly in list slots.
        pas::List* AwardIds;
        // Prefix displayed in the awards UI.
        std::int32_t AwardVisibleCount;
        // Optional byte-valued entries indexed by the galaxy's ranger list.
        pas::List* RangerRelations;
        TShip* EnemyShip;
        TShip* TruceShip;
        TShip* PartnerShip;
        std::int32_t PartnershipDaysRemaining;
        std::int32_t PortraitFaceId;
        aGalaxyStruct::TOwnerId PilotRace;
        std::uint8_t cpp_padding_8[7];
        double MovementSpeed;
        // Angular increment used by path construction, in degrees.
        double MovementTurnRate;
        // Heading in degrees.
        double MovementDirection;
        TShipOrder Order;
        std::uint8_t cpp_padding_9[3];
        // Order-dependent; hole travel packs countdown and endpoint side into the two words.
        std::int32_t OrderStateData;
        // Planet, ship, star or hole according to Order.
        pas::Object* OrderTarget;
        EC_Struct::TPointF OrderDestination;
        std::uint8_t OrderAbsolute;
        std::uint8_t cpp_padding_10[3];
        // Owned; nodes are recycled by the path.
        aPath::TSPath* MovementPath;
        std::uint8_t JumpDeparturePathCommitted;
        // Pirate Clan station abduction changes the arrival system and jump-gate presentation.
        std::uint8_t AbductedByPirateClan;
        std::uint8_t cpp_padding_11[2];
        // Player text quests reset this counter.
        std::int32_t ConsecutiveDockedDays;
        std::uint8_t AbsoluteScriptOrder;
        std::uint8_t cpp_padding_12[3];
        // Retained space-engine object.
        SE_Space::TObjectSE* Graphic;
        pas::WideString GraphName;
        // Alternate Dominator-style ship representation.
        std::uint8_t GraphDominator;
        std::uint8_t InHyperspace;
        std::uint8_t cpp_padding_13[2];
        // Used for follow spacing and ship repulsion.
        float CollisionRadius;
        // Script.ShipDestroy.
        std::uint8_t DestroyQueued;
        std::uint8_t ChameleonActive;
        aGalaxyStruct::TDominatorSeries ChameleonSeries;
        // Hull-dependent disguise silhouette.
        std::uint8_t ChameleonVisualType;
        // Serialized counter displayed in active chameleon info (); no gameplay update recovered.
        std::int32_t ChameleonDisplayCount;
        // TDominatorSeries order.
        pas::Array<std::uint8_t, 0, 2> ChameleonDetected;
        std::uint8_t cpp_padding_14[1];
        // TDominatorSeries order.
        pas::Array<std::int32_t, 0, 2> ChameleonCharges;
        std::uint8_t cpp_padding_15[4];
        // Predicted position adjusted when separating following ships.
        EC_Struct::TPointF RepulsionPosition;
        std::uint8_t cpp_padding_16[4];
        // May extend beyond 0..255; clamped when emitting film commands.
        float FilmAlpha;
        float FilmAlphaStep;
        // Money xor 0xA4A576AD.
        std::uint32_t EncodedMoney;
        // Borrowed from the current turn film.
        aEFilm::TEFilmObj* FilmObject;
        std::uint8_t NoDrop;
        // Script.NoTargetToShip mode, not a Boolean.
        std::uint8_t TargetingRestriction;
        std::uint8_t NoTalk;
        std::uint8_t NoScan;
        // Script-assigned appearance; separate from Dominator camouflage.
        std::uint8_t ScriptChameleon;
        // Prevents repeat player money/cargo/truce deals independently of the normal-ship turn cooldown.
        std::uint8_t PlayerExtortionPactActive;
        // One-point hits from the player; SCRATCHDAMAGE checks for twenty.
        std::uint16_t PlayerScratchHitsReceived;
        // Incoming passes against this ship.
        std::int32_t InterceptorPassesRemaining;
        TShip* InterceptorSourceShip;
        // Retained visual for incoming interceptor passes.
        SE_Space::TObjectSE* InterceptorGraphic;
        // Borrowed auxiliary graphic entry.
        aEFilm::TEFilmObj* AuxiliaryFilmObject;
        // ss* faction-combat category, exposed by Script.ShipStanding.
        std::uint8_t CurrentStanding;
        std::uint8_t cpp_padding_17[3];
        std::int32_t SmoothedSpeed;
        // Retained while EnemyShip is absent or outside this system.
        std::int32_t SmoothedEnemySpeed;
        float SmoothedEquipmentEffectiveness;
        std::int32_t SmoothedWealth;
        float SmoothedMoneyFraction;
        // Hull capacity less equipped item mass; excludes loose cargo.
        float SmoothedFreeCapacityFraction;
        // Smoothed affordability state; ranger item evaluation uses it to scale resale/cost penalties.
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
        // Zero for no source; an ID, not a pointer.
        std::int32_t SourceShipId;
    };
    #pragma pack(pop)

    using PCombatStatusEffect = TCombatStatusEffect*;

    #pragma pack(push, 1)
    struct TShipStatBonusEntry {
        aConst::TEquipmentBonusKind BonusKind;
        std::uint8_t cpp_padding[3];
        std::int32_t BonusValue;
    };
    #pragma pack(pop)

    using PShipStatBonusEntry = TShipStatBonusEntry*;

    // View of TShip.Hull through DefGenerator ($F8..$114), indexed by item kind.
    // EquipItem / UnequipSlot use Self + ItemType * 4 + $50 in the native code.
    // These are cached pointers; the owning inventory remains a separate list.
    #pragma pack(push, 1)
    struct TShipEquipmentCacheView {
        pas::Array<std::uint8_t, 0, 247> Prefix;
        pas::Array<aItem::TEquipment*, 42, 49> Slots;
    };
    #pragma pack(pop)

    using PShipEquipmentCacheView = TShipEquipmentCacheView*;

    struct TCustomShipInfo {
        pas::WideString TypeName;
        pas::WideString Description;
        pas::Array<std::int32_t, 1, 3> Data;
        pas::WideString TextData1;
        pas::WideString TextData2;
        pas::WideString TextData3;
        // Borrowed from the shared script cache.
        void* ActionCode;
        std::uint8_t StatusEffect;
        std::uint8_t ActionCodeInitialized;
        std::uint8_t DeleteQueued;
        std::uint8_t cpp_padding[1];
    };

    using PCustomShipInfo = TCustomShipInfo*;

    enum TItemDegradationKind : std::uint8_t {
        idkBattle = 0,
        idkUse = 1,
        idkForce = 2,
        idkAfterburner = 3,
    };

} // namespace aShip
