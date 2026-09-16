#pragma once
#include "runtime_support.hpp"
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aNormalShip.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aGalaxy {
    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aItem {
    struct TItem;

    struct TWeapon;

} // namespace aItem

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aRanger {
    struct TPlayerOldQuest;

    struct TQuest;

} // namespace aRanger

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aRanger {
    struct TRanger;

    using TRangerCareerValues = pas::Array<std::uint8_t, 0, 2>;

    enum TRelationChangeMode : std::uint8_t {
        rcmCapAt = 0,
        rcmRaiseTo = 1,
        rcmIncrease = 2,
        rcmDecrease = 3,
        rcmDecreaseWithFloor20 = 4,
    };

    using TRangerProgramMask = pas::Set<0, 15>;

    using PQuest = TQuest*;

    #pragma pack(push, 1)
    struct TQuest {
        aGalaxyStruct::TQuestType QuestType;
        std::uint8_t cpp_padding[1];
        std::uint16_t QuestNumber;
        aPlanet::TPlanet* Planet;
        std::int32_t DeadlineTurn;
        std::int32_t RewardMoney;
        // Can also reference an item, as well as a planet, ship or star.
        pas::Object* ObjectiveTarget;
        std::uint8_t Successful;
        std::uint8_t cpp_padding_2[3];
        pas::WideString Description;
        pas::WideString CompletionText;
        // Protected ship lost after success.
        pas::WideString SpecialCompletionText;
    };
    #pragma pack(pop)

    enum TQuestTextKind : std::uint32_t {
        qtkOffer = 0,
        qtkCompletion = 1,
        qtkProtectedShipLost = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRanger : aNormalShip::TNormalShip {
        PAS_CLASS_META(TRanger, aNormalShip::TNormalShip, "TRanger", 1376)
        void p_destroy() override;
        // For a fresh inherited TNormalShip instance; creates loadout, career, quests and relation entries and registers it in the galaxy.
        virtual void InitializeAtPlanet(aPlanet::TPlanet* Planet, std::int32_t InitialMoney);
        // Appends Self and relation entries; requires an unregistered ranger with initialized lists.
        void RegisterInGalaxyRelations();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        // Creates the quest list and loads IDs for later resolution; rejects quest counts above 10000.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        // Resolves quest targets by quest type and LastDockedNonPlanetLocation.
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void NextDay() override;
        void NextDayLogic() override;
        // True only for the pending auto-equip branch; normal follow/weapon assignment returns false.
        std::uint8_t ProcessPendingPlayerFollowTargeting();
        // Requires HomePlanet.
        aGalaxy::TStar* GetHomeStar() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        // Returns the ranger category used by ship-greeting filters.
        std::uint8_t GetGreetingShipCategory() override;
        // Ties favor trader, then pirate.
        aGalaxyStruct::TRangerCareer GetDominantCareer() override;
        // Values occupies the low three bytes of one stack slot; result is the average of 100 minus each career-distance.
        std::uint8_t GetCareerSimilarity(TRangerCareerValues Values);
        // Selects the closest configured ShipCharacter profile; equal similarities retain the earlier profile.
        pas::WideString GetCharacterName();
        // Rounded pirate career status times StrengthInBestRanger, clamped to 0..100.
        std::uint8_t GetStrengthScaledPirateStatus() override;
        std::int32_t GetDesiredCargoFreeSpace() override;
        // Dispatches by object class and radar distance; unsupported objects yield unknown object.
        pas::WideString GetObjectInfoText(pas::Object* Instance);
        // Counts galaxy star-list ships whose PartnerShip is Self; includes docked ships.
        std::int32_t CountWingmen();
        std::uint8_t NeedsStrengthCatchup();
        // Either the absolute or relative career threshold can trigger catch-up.
        std::uint8_t NeedsWealthCatchup();
        // Only buys a full refill when its positive cost is affordable.
        void RefuelAtLocation() override;
        // Can grant money, experience, equipment, awards and simulated kills; requires the unseen-day threshold, a player and unresolved Dominators.
        void SimulateUnseenProgression();
        // Saturates at 100.
        void AddTraderCareerActivity(std::uint8_t Amount);
        // Saturates at 100.
        void AddPirateCareerActivity(std::uint8_t Amount);
        // Saturates at 100.
        void AddWarriorCareerActivity(std::uint8_t Amount);
        void ClearPendingCareerActivity();
        // Clears pending activity even when an NPC already holds a featured title. New titles require a non-excluded ranger in the upper half of the ranking and an undefeated Coalition.
        void ProcessCareerActivityAndEminentProgress();
        // Affects every galaxy ranger, including excluded entries.
        static void HalveAllRangerEminentProgress(aGalaxyStruct::TRangerCareer Career);
        std::uint8_t OrderBestQueuedTradePlanet();
        aPlanet::TPlanet* SelectBestTradePlanetFromQueue();
        // Borrowed result, nil for an empty queue.
        aPlanet::TPlanet* SelectRandomPlanetFromQueue();
        // Replaces PlanetQueue; excludes LastDockedPlanet. A scripted system encountered in distance order ends the scan.
        void BuildReachablePlanetQueue() override;
        std::uint8_t CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        // The native UnusedMode comparison has no branch effect. Chooses travel toward combat opportunities only with no cargo, a gripper and a full hull.
        void SelectIdleFreeFlightDestination(std::uint8_t UnusedMode);
        // Native distant-system branch tests ships in the current system (), rather than the candidate system.
        std::uint8_t TryOrderTravelToShipTypeLocation(std::uint8_t ShipType);
        // Can follow a partner's travel order; otherwise favors short travel to a suitable planet, station or peaceful system.
        void SelectNearestReachableDestination();
        // Excludes the last docked planet/station and can favor leaving their system.
        void SelectAlternateReachableDestination();
        void SellCargoGoods();
        void BuyProfitableGoods();
        // Skips queue index 0. Leaves BestPlanet unchanged unless a candidate improves the score; UnitCost must be nonzero.
        std::uint8_t FindBestQueuedSellPlanetProfitScore(std::uint8_t Good, aPlanet::TPlanet*& BestPlanet, double UnitCost);
        void ApplyIllegalGoodsTradeRelationsPenalty(std::int32_t TotalTradeValue);
        // Repairs eligible installed items even without sufficient money; subtracts cost only when Money is strictly greater.
        void RepairBrokenEquipmentAtLocation() override;
        std::uint8_t RelationToNonRanger(aShip::TShip* Ship) override;
        // Two female human pilots receive 100; otherwise reads the stored galaxy-indexed relation.
        std::uint8_t RelationToRanger(void* Ranger) override;
        // Applies the target's Charisma to positive changes, clamps to 0..100, and may break partnership or select an enemy.
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) override;
        // Sets EnemyShip and penalizes relations with the attacking ranger or its controlling ranger.
        void ReactToAttack(aShip::TShip* Attacker) override;
        // Updates InFear and can replace EnemyShip; special simulation mode clears fear.
        std::uint8_t RecomputeFearState() override;
        void TryOfferRansomToPursuer();
        std::uint8_t AcceptsRansomDemandFrom(aShip::TShip* Ship) override;
        // Propagates reactions among nearby ships/planets; can update player achievements. Script-bound victims suppress the relation pass.
        void ApplyAttackReputationChanges(aShip::TShip* Victim, double Severity);
        // Also adds pirate career activity and improves the main pirate planet's relation.
        void ApplyExtortionReputationPenalty(aShip::TShip* Victim);
        void TryRecruitWingman();
        void CheckForPartnershipBreakup();
        // Relation of at least 30.
        std::uint8_t TrustsAttackRequester(aShip::TShip* Ship) override;
        // Tests relation plus a relative-strength score against 120; precise dialogue role remains unresolved.
        std::uint8_t EvaluateAllyRelationAndStrength(aShip::TShip* Ship) override;
        // Returns whether imprisonment blocks this turn; may imprison, release or update standing.
        std::uint8_t ProcessPrisonAndHostileCheck();
        void ChangeGlobalRelations(pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aConst::THullShipTypeMask HullTypeMask, aGalaxyStruct::TOwnerMask OwnerMask);
        // Scope filters by ship, star or sector; nil selects all. Bulk changes skip scripted ships for which HasScriptControl is true. Masks use ShipToHullType categories and owner IDs, not TShip.TypeId.
        void ChangeShipRelations(pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aConst::THullShipTypeMask HullTypeMask, aGalaxyStruct::TOwnerMask OwnerMask);
        // Scope filters by planet, star or sector; nil selects all. Only coalition planets are affected. OwnerMask uses owner IDs as bits.
        void ChangePlanetRelations(pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aGalaxyStruct::TOwnerMask OwnerMask);
        // Averages stored relations for matching ships; empty selection returns 50. A single ship uses its virtual RelationToRanger.
        std::uint8_t GlobalRelationsShips(pas::Object* Scope, std::uint16_t HullTypeMask, std::uint8_t OwnerMask);
        // Averages Coalition planets in matching stars/sectors; empty selection returns 50. A planet Scope does not narrow this native scan.
        std::uint8_t GlobalRelationsPlanets(pas::Object* Scope, std::uint8_t OwnerMask);
        void AssignWeaponTargetsInStar() override;
        // May attempt extortion and assign weapon targets; preserves a prior enemy when no replacement qualifies.
        void SelectEnemyShipInStar() override;
        void EngageEnemyShip() override;
        void ProcessCombatDialogue() override;
        // Can reduce ship/home-planet relations and adds pirate career activity to the requester.
        void ReactToExtortionDemand(void* Ranger) override;
        // May execute payment and truce, or open the player's response dialogue; not a text-only query.
        std::uint8_t BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) override;
        // Successful requests jettison cargo and establish a truce.
        std::uint8_t BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) override;
        // Acceptance transfers OfferedAmount from OtherShip to Self and establishes a truce.
        std::uint8_t BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) override;
        // May change relations/career activity even on refusal; acceptance issues a joint attack.
        std::uint8_t BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) override;
        // Checks eligibility and formats refusal text; OtherShip must be a ranger. Success does not clear preexisting Response.
        std::uint8_t BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        // Calls the eligibility method, then sets PartnerShip/duration and transfers payment. Requires a ranger requester.
        std::uint8_t AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        static pas::WideString GetProgramName(std::uint8_t ProgramIndex);
        pas::WideString GetProgramInfoText(std::uint8_t ProgramIndex);
        // Does not mask or validate ProgramIndex.
        std::uint8_t HasProgram(std::uint8_t ProgramIndex);
        // Sums owned quantities for bits 0..11; higher bits are ignored. Native signed 32-bit additions wrap on overflow.
        std::int32_t CountProgramsInFilter(TRangerProgramMask Filter);
        // Selects an allowed ID regardless of inventory counts; deterministic system/turn seed. Empty filter returns zero after 10000 attempts.
        std::uint8_t SelectRandomProgramIdFromFilter(TRangerProgramMask Filter);
        // Favors program 5 until enough copies exist; otherwise selects among IDs 6..11.
        std::uint8_t SelectProgramReward();
        // At least one; uses galaxy seed, turn and difficulty.
        static std::int32_t GetProgramRewardCount(std::uint8_t ProgramIndex);
        float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) override;
        float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) override;
        float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) override;
        // Always true.
        std::uint8_t AcceptPickupItem(aItem::TItem* Item) override;
        std::uint8_t AcceptPickupDistance(aItem::TItem* Item, double Distance) override;
        void RefreshCurrentStanding() override;
        void ProcessQuestTimersAndOutcomes();
        // Uses player history.
        static std::int32_t CountFailedQuests(std::uint8_t OwnerId, aGalaxyStruct::TQuestTypes QuestTypes);
        void CheckQuestFailureAward(PQuest Quest, aGalaxyStruct::TQuestTypes QuestTypes);
        void TryTurnInQuests();
        void ArchiveQuest(std::int32_t Index);
        std::uint8_t TryTurnInAnyQuest(pas::WideString& ResponseText);
        std::uint8_t TryTurnInQuest(std::int32_t Index, pas::WideString& ResponseText);
        // Requires CurrentPlanet. Grants an award, program, item or module plus experience and relation effects. The hidden WideString result is cleared on entry; NPC result is empty.
        pas::WideString GrantPlanetQuestReward(std::int32_t Difficulty, std::int32_t& ExperienceAwarded);
        // Requires CurrentPlanet.
        std::uint8_t GenerateQuestOffer(TQuest& Quest, pas::WideString& ResponseText);
        pas::WideString BuildQuestText(const TQuest& Quest, TQuestTextKind Kind);
        // Outcome: 0=active, positive=completed, negative=failed.
        static void PublishQuestStatus(PQuest Quest, std::int32_t Outcome);
        void ProcessShipDestructionQuests(aShip::TShip* Ship);
        std::uint8_t HasQuestOfType(aGalaxyStruct::TQuestType QuestType);
        // Player-only; pending history can also preserve unrelated ships.
        std::uint8_t ShouldKeepShipForQuests(aShip::TShip* Ship);
        // Does nothing for NPC rangers.
        void RefreshPlayerQuestTargets();
        // One-based experience ranking.
        std::uint16_t PlaceInRating;
        // Excludes strength/title eligibility; placement sorting still assigns a position.
        std::uint8_t ExcludedFromRating;
        // TRangerCareer order.
        pas::Array<std::uint8_t, 0, 2> CareerStatus;
        // TRangerCareer order.
        pas::Array<std::uint8_t, 0, 2> EminentProgress;
        // Trader, pirate, warrior.
        pas::Array<std::uint8_t, 0, 2> PendingCareerActivity;
        // AI preference; may differ from GetDominantCareer.
        aGalaxyStruct::TRangerCareer PreferredCareer;
        // Initialized in 0..100.
        std::uint8_t Aggression;
        std::uint8_t cpp_padding[2];
        // Owned PQuest records.
        pas::List* Quests;
        std::int32_t PrisonTermRemaining;
        aShip::TShip* LastDockedNonPlanetLocation;
        std::int32_t BaseNodes;
        pas::Array<std::int32_t, 0, 11> ProgramCounts;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPlayerOldQuest {
        aPlanet::TPlanet* Planet;
        pas::WideString Description;
        std::uint8_t Successful;
        // Permanently declined offer.
        std::uint8_t Declined;
        aGalaxyStruct::TQuestType QuestType;
        std::uint8_t cpp_padding[1];
        std::uint16_t QuestNumber;
        std::uint8_t cpp_padding_2[2];
    };
    #pragma pack(pop)

    using PPlayerOldQuest = TPlayerOldQuest*;

} // namespace aRanger
