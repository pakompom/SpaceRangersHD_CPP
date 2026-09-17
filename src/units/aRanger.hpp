#pragma once
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aRanger.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aItem {
    struct TWeapon;

} // namespace aItem

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aRanger {
    extern aShip::TShip* PendingPlayerFollowTarget;

    // Auto-equips in the follow helper and uses NPC dialogue branches for the player.
    extern std::uint8_t PlayerAutomaticControl;

    // Set by equipped-item breakage; ShouldContinuePlayerTravel checks it.
    extern std::uint8_t PlayerEquipmentBrokenThisTurn;

    // Career, then bonSkill1..bonSkill6.
    extern pas::Array<pas::Array<std::int32_t, 22, 27>, 0, 2> RangerSkillBonusEvaluationWeights;

    // Career, then bonSlotRadar..bonSlotForsage.
    extern pas::Array<pas::Array<std::int32_t, 13, 20>, 0, 2> RangerSlotBonusEvaluationWeights;

    // Owned PPlayerOldQuest records.
    extern pas::List* PlayerOldQuests;

    // Requires registered ranger/home-planet state. Removes quests and relation-column entries, adjusts the player index and refreshes galaxy ratings.
    void TRanger_Destroy(TRanger* Self);

    // Resolves quest targets by quest type and LastDockedNonPlanetLocation.
    void TRanger_ResolveLoadedReferences(TRanger* Self, aGalaxy::TGalaxy* Galaxy);

    void TRanger_NextDay(TRanger* Self);

    void TRanger_NextDayLogic(TRanger* Self);

    // True only for the pending auto-equip branch; normal follow/weapon assignment returns false.
    std::uint8_t TRanger_ProcessPendingPlayerFollowTargeting(TRanger* Self);

    // Dispatches by object class and radar distance; unsupported objects yield unknown object.
    pas::WideString TRanger_GetObjectInfoText(TRanger* Self, pas::Object* Instance);

    std::uint8_t TRanger_CanQueueReachablePlanet(TRanger* Self, aPlanet::TPlanet* Planet);

    // The native UnusedMode comparison has no branch effect. Chooses travel toward combat opportunities only with no cargo, a gripper and a full hull.
    void TRanger_SelectIdleFreeFlightDestination(TRanger* Self, std::uint8_t UnusedMode);

    // Can follow a partner's travel order; otherwise favors short travel to a suitable planet, station or peaceful system.
    void TRanger_SelectNearestReachableDestination(TRanger* Self);

    // Excludes the last docked planet/station and can favor leaving their system.
    void TRanger_SelectAlternateReachableDestination(TRanger* Self);

    // Repairs eligible installed items even without sufficient money; subtracts cost only when Money is strictly greater.
    void TRanger_RepairBrokenEquipmentAtLocation(TRanger* Self);

    // Updates InFear and can replace EnemyShip; special simulation mode clears fear.
    std::uint8_t TRanger_RecomputeFearState(TRanger* Self);

    void TRanger_TryOfferRansomToPursuer(TRanger* Self);

    std::uint8_t TRanger_AcceptsRansomDemandFrom(TRanger* Self, aShip::TShip* Ship);

    // Propagates reactions among nearby ships/planets; can update player achievements. Script-bound victims suppress the relation pass.
    void TRanger_ApplyAttackReputationChanges(TRanger* Self, aShip::TShip* Victim, double Severity);

    // Also adds pirate career activity and improves the main pirate planet's relation.
    void TRanger_ApplyExtortionReputationPenalty(TRanger* Self, aShip::TShip* Victim);

    // Relation of at least 30.
    std::uint8_t TRanger_TrustsAttackRequester(TRanger* Self, aShip::TShip* Ship);

    // Tests relation plus a relative-strength score against 120; precise dialogue role remains unresolved.
    std::uint8_t TRanger_EvaluateAllyRelationAndStrength(TRanger* Self, aShip::TShip* Ship);

    void TRanger_ChangeGlobalRelations(TRanger* Self, pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aConst::THullShipTypeMask HullTypeMask, aGalaxyStruct::TOwnerMask OwnerMask);

    // Scope filters by planet, star or sector; nil selects all. Only coalition planets are affected. OwnerMask uses owner IDs as bits.
    void TRanger_ChangePlanetRelations(TRanger* Self, pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aGalaxyStruct::TOwnerMask OwnerMask);

    void TRanger_AssignWeaponTargetsInStar(TRanger* Self);

    // May execute payment and truce, or open the player's response dialogue; not a text-only query.
    std::uint8_t TRanger_BuildMoneyExtortionResponse(TRanger* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount);

    // Successful requests jettison cargo and establish a truce.
    std::uint8_t TRanger_BuildCargoExtortionResponse(TRanger* Self, aShip::TShip* OtherShip, pas::WideString& Response);

    // May change relations/career activity even on refusal; acceptance issues a joint attack.
    std::uint8_t TRanger_BuildAttackRequestResponse(TRanger* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target);

    // Checks eligibility and formats refusal text; OtherShip must be a ranger. Success does not clear preexisting Response.
    std::uint8_t TRanger_BuildPartnershipOfferResponse(TRanger* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    // Calls the eligibility method, then sets PartnerShip/duration and transfers payment. Requires a ranger requester.
    std::uint8_t TRanger_AcceptPartnershipOffer(TRanger* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    float TRanger_EvaluateStatBonus(TRanger* Self, aConst::TEquipmentBonusKind BonusKind, std::int32_t Value);

    float TRanger_EvaluateWeaponDamage(TRanger* Self, aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage);

    void TRanger_RefreshCurrentStanding(TRanger* Self);

    void TRanger_ProcessQuestTimersAndOutcomes(TRanger* Self);

    // Requires CurrentPlanet. Grants an award, program, item or module plus experience and relation effects. The hidden WideString result is cleared on entry; NPC result is empty.
    pas::WideString TRanger_GrantPlanetQuestReward(TRanger* Self, std::int32_t Difficulty, std::int32_t& ExperienceAwarded);

    // Requires CurrentPlanet.
    std::uint8_t TRanger_GenerateQuestOffer(TRanger* Self, TQuest& Quest, pas::WideString& ResponseText);

    void TRanger_ProcessShipDestructionQuests(TRanger* Self, aShip::TShip* Ship);

} // namespace aRanger
