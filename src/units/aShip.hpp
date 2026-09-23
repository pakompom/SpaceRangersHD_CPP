#pragma once
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aItem {
    struct TCargoHook;

    struct TDefGenerator;

    struct TEngine;

    struct TEquipment;

    struct TGoods;

    struct THull;

    struct TItem;

    struct TRadar;

    struct TRepairRobot;

    struct TScaner;

    struct TWeapon;

} // namespace aItem

namespace aShip {
    // Suppresses duplicate Pirate Clan abduction effects during turn simulation.
    extern std::int8_t SimulationContext;

    // Energy, splinter and missile hit callbacks.
    extern pas::Array<aGalaxyStruct::TScriptActionType, 0, 2> DamageScriptActionTypes;

    // Reused script-event payload for the goods leaving the ship.
    extern aItem::TGoods* TradeGoodsSold;

    // Reused payload for the purchased portion of the sale.
    extern aItem::TGoods* TradeGoodsCostBasis;

    extern pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2> DominatorShipSmallSizes;

    extern pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2> DominatorShipLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> RangerSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> RangerLargeSizes;

    extern pas::Array<pas::Array<std::int32_t, 0, 7>, 3, 5> TransportSmallSizes;

    extern pas::Array<pas::Array<std::int32_t, 0, 7>, 3, 5> TransportLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> PirateSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> PirateLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> PirateClanSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> PirateClanLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> WarriorSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> WarriorLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> BigWarriorSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> BigWarriorLargeSizes;

    extern std::int32_t TranclucatorSmallSize;

    extern std::int32_t TranclucatorLargeSize;

    extern std::int32_t SpecialHullSmallSize;

    extern std::int32_t SpecialHullLargeSize;

    extern std::int32_t StationSize;

    extern std::int32_t DefaultShipSmallSize;

    extern std::int32_t DefaultShipLargeSize;

    // bonSkill1..bonSkill6.
    extern pas::Array<std::int32_t, 22, 27> SkillBonusEvaluationWeights;

    // bonSlotRadar..bonSlotForsage.
    extern pas::Array<std::int32_t, 13, 20> SlotBonusEvaluationWeights;

    // Indexed by KlingType.
    extern pas::Array<double, 0, 7> KlingCheapDropValueFactors;

    // Indexed by KlingType.
    extern pas::Array<double, 0, 7> KlingValuableDropValueFactors;

    // TKlingType order.
    extern const pas::Array<double, 0, 7> DominatorProgramDropCostFactors;

    // Allocates an unregistered instance; caller must initialize or deserialize it.
    TShip* CreateShipByType(aGalaxyStruct::TShipType ShipType);

    // Lists contain TShip. Sum of pairwise ChanceToWin divided by Opponents.Count squared; requires nonempty Opponents when Ships is nonempty.
    float CompareShipGroupsStrength(pas::List* Ships, pas::List* Opponents);

    // Owner six skips racial scaling. Uses active galaxy turn and difficulty.
    float CalculateFuelCost(std::int32_t Amount, aGalaxyStruct::TOwnerId OwnerId);

    std::int32_t CalculateRoundedFuelCost(std::int32_t Amount, aGalaxyStruct::TOwnerId OwnerId);

    void TShip_Create(TShip* Self);

    void TShip_Destroy(TShip* Self);

    // Converts saved IDs to object references; requires all referenced objects to have been loaded.
    void TShip_ResolveLoadedReferences(TShip* Self, aGalaxy::TGalaxy* Galaxy);

    void TShip_NextDay(TShip* Self);

    void TShip_NextDayLogic(TShip* Self);

    // Requires a player; includes current order, hull, speed and relation information.
    pas::WideString TShip_GetSpaceInfoText(TShip* Self);

    // Returns empty unless the player shares CurrentStar. Substitutes OtherShip for <TalkShip>.
    pas::WideString TShip_LookupVisibleTalkText(TShip* Self, const pas::WideString& Path, TShip* OtherShip);

    // May assign PortraitFaceId lazily. Returns a resource base without the GI prefix or animation suffix.
    pas::WideString TShip_GetCaptainPortraitResourceBase(TShip* Self);

    // ScriptChameleon preserves the assigned graphic while updating GraphDominator.
    void TShip_RefreshGraphic(TShip* Self);

    // Can subsidize and queue an unseen NPC for relocation to a peaceful Coalition system; excludes protected quest/script/partner ships.
    void TShip_TryRelocateUnseenShip(TShip* Self);

    // Ranger, pirate or Tranclucator death notification; does not itself check player partnership.
    void TShip_NotifyCompanionDeath(TShip* Self);

    // Source may be nil, ship or missile. HitRange=-1 selects direct-hit rules; other values select area-hit rules. Returns adjusted damage, zero for rejection, or negative damage for an impulse-shield block; not actual hull loss. May run death handling without freeing Self.
    std::int32_t TShip_ApplyDamage(TShip* Self, pas::Object* Source, std::int32_t Damage, float HitRange, std::uint32_t& DamageColor, aGalaxyStruct::TDamageFlagSet DamageFlags);

    // Returns ApplyDamage's signed result. Positive FixedDamage bypasses the initial roll/armor stage unless weapon flag 0x800 is already set; later effects still apply.
    std::int32_t TShip_ApplyWeaponHit(TShip* Self, TShip* Source, aItem::TWeapon* Weapon, float HitRange, std::uint32_t& DamageColor, aGalaxyStruct::TDamageFlagSet& DamageFlags, float DamageScale, std::int32_t FixedDamage);

    // Requires a TMissile; returns ApplyDamage's signed result.
    std::int32_t TShip_ApplyMissileHit(TShip* Self, pas::Object* Missile, std::uint32_t& DamageColor, aGalaxyStruct::TDamageFlagSet& DamageFlags);

    // Ignores non-missile targets; can free the target and nearby missiles.
    void TShip_FireWeaponAtMissile(TShip* Self, aItem::TWeapon* Weapon, pas::Object* Target, std::uint8_t RecordFilm);

    // Can affect additional ships through chained, area or penetrating fire.
    void TShip_FireWeaponAtShip(TShip* Self, aItem::TWeapon* Weapon, TShip* Target, std::uint8_t RecordFilm);

    // ExplodingObject may be an item or ship; SourceShip and Missile may be nil. Returns script-adjusted damage, not actual hull loss; can trigger death handling without freeing Self.
    std::int32_t TShip_ApplyExplosionDamage(TShip* Self, TShip* SourceShip, pas::Object* ExplodingObject, std::int32_t ExtraDamage, pas::Object* Missile);

    pas::WideString TShip_GetWeaponDamageSummary(TShip* Self);

    pas::WideString TShip_GetRepairPointsSummary(TShip* Self);

    // Same result as GetWeaponRange in this binary.
    std::int32_t TShip_GetWeaponActionRange(TShip* Self, aItem::TWeapon* Weapon);

    // Returns a combat strength ratio, not a probability.
    double TShip_ChanceToWin(TShip* Self, TShip* Target);

    aGalaxyStruct::TPercent TShip_GetWinChancePercent(TShip* Self, TShip* Target);

    aGalaxyStruct::TRelationLevel TShip_GetRelationLevelToShip(TShip* Self, TShip* Ship);

    // Stations can display their stored ranger relation instead of the effective relation.
    pas::WideString TShip_GetRelationLevelTextToShip(TShip* Self, TShip* Ship);

    // Attempts a cargo or money demand through the target's dialogue handler; requires a player.
    std::uint8_t TShip_TryExtortShip(TShip* Self, TShip* Target);

    // Cancels reciprocal attacks and pursuers, including wingmen/Tranclucators and missiles. Only the player's TruceShip is set reciprocally.
    void TShip_TruceWithShip(TShip* Self, TShip* Ship);

    // May ask the player or nearby allied NPCs; requires a player.
    void TShip_RequestAlliesAttackShip(TShip* Self, TShip* Target);

    std::uint8_t TShip_IsInPrison(TShip* Self);

    std::uint8_t TShip_CanUseEquipmentTech(TShip* Self, aItem::TEquipment* Item);

    std::uint8_t TShip_CanRepairEquipmentTech(TShip* Self, aItem::TEquipment* Item);

    // Does not require EquippedFlag.
    std::uint8_t TShip_IsEquipmentUsable(TShip* Self, aItem::TEquipment* Item);

    // Zero-based template index; true means disallowed. Includes custom faction, Dominator series and pilot-race restrictions.
    std::uint8_t TShip_IsMicroModuleRaciallyRestricted(TShip* Self, std::int32_t ModuleIndex);

    std::int32_t TShip_GetEquipmentStatBonus(TShip* Self, aConst::TEquipmentBonusKind BonusKind, aItem::TEquipment* Item);

    std::int32_t TShip_GetWeaponRange(TShip* Self, aItem::TWeapon* Weapon);

    std::int32_t TShip_CalculateHullArmor(TShip* Self, aItem::THull* Hull);

    std::int32_t TShip_CalculateEngineSpeed(TShip* Self, aItem::TEngine* Engine, std::uint8_t ApplyBrokenPenalty);

    std::int32_t TShip_CalculateEngineJumpRange(TShip* Self, aItem::TEngine* Engine);

    std::int32_t TShip_CalculateRadarRange(TShip* Self, aItem::TRadar* Radar);

    std::int32_t TShip_CalculateScannerPower(TShip* Self, aItem::TScaner* Scanner);

    std::int32_t TShip_CalculateRepairPoints(TShip* Self, aItem::TRepairRobot* RepairRobot);

    std::int32_t TShip_CalculateCargoHookPower(TShip* Self, aItem::TCargoHook* CargoHook);

    float TShip_CalculateDefGeneratorFactor(TShip* Self, aItem::TDefGenerator* DefGenerator);

    float TShip_GetEquipmentEvaluationSynergyBonus(TShip* Self, aItem::TEquipment* Item);

    // Sells at a location or jettisons in space. May flag a long-stranded NPC for destruction when overload cannot be resolved.
    void TShip_DropCargoUntilNotOverloaded(TShip* Self);

    // Deterministic eligibility roll for an ordinary pirate without a script binding or player partnership.
    std::uint8_t TShip_CanDropTreasureMap(TShip* Self);

    std::uint8_t TShip_TryDropTreasureMap(TShip* Self);

    // Caller supplies a valid good and quantity; does not itself honor ship NoDrop.
    void TShip_DropGoodsIntoSpace(TShip* Self, std::uint8_t Good, std::int32_t Count);

    void TShip_DropAllCargoGoods(TShip* Self);

    // Caller detaches Item first. Script action 33 can suppress transfer or free Item; otherwise the moving-drop descriptor takes ownership.
    void TShip_QueueMovingItemDrop(TShip* Self, aItem::TItem* Item, std::uint8_t DeployTranclucator);

    // Normally credits resale value and frees Item; eligible NPC node stacks instead feed DepositCarriedNodes and automatic training.
    void TShip_LiquidateInventoryItem(TShip* Self, aItem::TItem* Item);

    // True only when the item becomes newly broken. Nil is accepted; script actions 36..39 can modify the damage.
    std::uint8_t TShip_ApplyItemDegradation(TShip* Self, aItem::TEquipment* Item, TItemDegradationKind Kind, double DurabilityDamage);

    // Selects up to three qualifying candidates within 51 attempts, applies the best positive gain, then auto-equips. Nil Item is accepted.
    void TShip_GenerateAndApplyMicroModule(TShip* Self, aItem::TEquipment* Item, std::uint8_t ResolveOverload);

    // Adds a generated weapon, may improve/module it, then auto-equips and optimizes inventory.
    void TShip_GenerateExtraWeapon(TShip* Self);

    void TShip_BuyEquipmentAtLocation(TShip* Self, std::uint8_t ForceGeneratedOffers);

    aItem::THull* TShip_CreateAndEquipHull(TShip* Self, std::uint16_t Capacity, std::uint8_t Level, aGalaxyStruct::TOwnerId Owner, std::int32_t Series, std::uint8_t PirateBuilt);

    // Also checks hook eligibility.
    std::uint8_t TShip_IsItemInPickupRange(TShip* Self, aItem::TItem* Item);

    // Selects among at most ten contiguous variants using ship seed and turn; substitutes ship names and HomePlanet. Missing text returns an unavailable marker.
    pas::WideString TShip_LookupTalkText(TShip* Self, const pas::WideString& Path);

    // Turn-worker/UI handshake; waits for conversation completion or shutdown. Requires an active visible-space turn.
    std::uint8_t TShip_OpenPlayerConversation(TShip* Self, std::uint8_t RespectChameleon);

    // Returns the global dialogue response, or zero when conversation cannot open. Amount only replaces the global amount when positive.
    std::uint8_t TShip_ShowPlayerDialogue(TShip* Self, aGalaxyStruct::TTalkKind Kind, const pas::WideString& Text, std::int32_t Amount);

    pas::WideString TShip_GetGreetingText(TShip* Self);

    float TShip_GetShockStatusDecay(TShip* Self, float Strength);

    void TShip_RefreshCurrentStanding(TShip* Self);

    // Base implementation always returns false.
    std::uint8_t TShip_CanDock(TShip* Self, TShip* Ship);

    aGalaxyStruct::TPercent TShip_RelationToShip(TShip* Self, TShip* Ship);

} // namespace aShip
