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
        pas::Object* ObjectiveTarget;
        std::uint8_t Successful;
        std::uint8_t cpp_padding_2[3];
        pas::WideString Description;
        pas::WideString CompletionText;
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
        virtual void InitializeAtPlanet(aPlanet::TPlanet* Planet, std::int32_t InitialMoney);
        void RegisterInGalaxyRelations();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void NextDay() override;
        void NextDayLogic() override;
        std::uint8_t ProcessPendingPlayerFollowTargeting();
        aGalaxy::TStar* GetHomeStar() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        std::uint8_t GetGreetingShipCategory() override;
        aGalaxyStruct::TRangerCareer GetDominantCareer() override;
        std::uint8_t GetCareerSimilarity(TRangerCareerValues Values);
        pas::WideString GetCharacterName();
        std::uint8_t GetStrengthScaledPirateStatus() override;
        std::int32_t GetDesiredCargoFreeSpace() override;
        pas::WideString GetObjectInfoText(pas::Object* Instance);
        std::int32_t CountWingmen();
        std::uint8_t NeedsStrengthCatchup();
        std::uint8_t NeedsWealthCatchup();
        void RefuelAtLocation() override;
        void SimulateUnseenProgression();
        void AddTraderCareerActivity(std::uint8_t Amount);
        void AddPirateCareerActivity(std::uint8_t Amount);
        void AddWarriorCareerActivity(std::uint8_t Amount);
        void ClearPendingCareerActivity();
        void ProcessCareerActivityAndEminentProgress();
        static void HalveAllRangerEminentProgress(aGalaxyStruct::TRangerCareer Career);
        std::uint8_t OrderBestQueuedTradePlanet();
        aPlanet::TPlanet* SelectBestTradePlanetFromQueue();
        aPlanet::TPlanet* SelectRandomPlanetFromQueue();
        void BuildReachablePlanetQueue() override;
        std::uint8_t CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        void SelectIdleFreeFlightDestination(std::uint8_t UnusedMode);
        std::uint8_t TryOrderTravelToShipTypeLocation(std::uint8_t ShipType);
        void SelectNearestReachableDestination();
        void SelectAlternateReachableDestination();
        void SellCargoGoods();
        void BuyProfitableGoods();
        std::uint8_t FindBestQueuedSellPlanetProfitScore(std::uint8_t Good, aPlanet::TPlanet*& BestPlanet, double UnitCost);
        void ApplyIllegalGoodsTradeRelationsPenalty(std::int32_t TotalTradeValue);
        void RepairBrokenEquipmentAtLocation() override;
        std::uint8_t RelationToNonRanger(aShip::TShip* Ship) override;
        std::uint8_t RelationToRanger(void* Ranger) override;
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) override;
        void ReactToAttack(aShip::TShip* Attacker) override;
        std::uint8_t RecomputeFearState() override;
        void TryOfferRansomToPursuer();
        std::uint8_t AcceptsRansomDemandFrom(aShip::TShip* Ship) override;
        void ApplyAttackReputationChanges(aShip::TShip* Victim, double Severity);
        void ApplyExtortionReputationPenalty(aShip::TShip* Victim);
        void TryRecruitWingman();
        void CheckForPartnershipBreakup();
        std::uint8_t TrustsAttackRequester(aShip::TShip* Ship) override;
        std::uint8_t EvaluateAllyRelationAndStrength(aShip::TShip* Ship) override;
        std::uint8_t ProcessPrisonAndHostileCheck();
        void ChangeGlobalRelations(pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aConst::THullShipTypeMask HullTypeMask, aGalaxyStruct::TOwnerMask OwnerMask);
        void ChangeShipRelations(pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aConst::THullShipTypeMask HullTypeMask, aGalaxyStruct::TOwnerMask OwnerMask);
        void ChangePlanetRelations(pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aGalaxyStruct::TOwnerMask OwnerMask);
        std::uint8_t GlobalRelationsShips(pas::Object* Scope, std::uint16_t HullTypeMask, std::uint8_t OwnerMask);
        std::uint8_t GlobalRelationsPlanets(pas::Object* Scope, std::uint8_t OwnerMask);
        void AssignWeaponTargetsInStar() override;
        void SelectEnemyShipInStar() override;
        void EngageEnemyShip() override;
        void ProcessCombatDialogue() override;
        void ReactToExtortionDemand(void* Ranger) override;
        std::uint8_t BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) override;
        std::uint8_t BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) override;
        std::uint8_t BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) override;
        std::uint8_t BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) override;
        std::uint8_t BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        std::uint8_t AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        static pas::WideString GetProgramName(std::uint8_t ProgramIndex);
        pas::WideString GetProgramInfoText(std::uint8_t ProgramIndex);
        std::uint8_t HasProgram(std::uint8_t ProgramIndex);
        std::int32_t CountProgramsInFilter(TRangerProgramMask Filter);
        std::uint8_t SelectRandomProgramIdFromFilter(TRangerProgramMask Filter);
        std::uint8_t SelectProgramReward();
        static std::int32_t GetProgramRewardCount(std::uint8_t ProgramIndex);
        float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) override;
        float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) override;
        float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) override;
        std::uint8_t AcceptPickupItem(aItem::TItem* Item) override;
        std::uint8_t AcceptPickupDistance(aItem::TItem* Item, double Distance) override;
        void RefreshCurrentStanding() override;
        void ProcessQuestTimersAndOutcomes();
        static std::int32_t CountFailedQuests(std::uint8_t OwnerId, aGalaxyStruct::TQuestTypes QuestTypes);
        void CheckQuestFailureAward(PQuest Quest, aGalaxyStruct::TQuestTypes QuestTypes);
        void TryTurnInQuests();
        void ArchiveQuest(std::int32_t Index);
        std::uint8_t TryTurnInAnyQuest(pas::WideString& ResponseText);
        std::uint8_t TryTurnInQuest(std::int32_t Index, pas::WideString& ResponseText);
        pas::WideString GrantPlanetQuestReward(std::int32_t Difficulty, std::int32_t& ExperienceAwarded);
        std::uint8_t GenerateQuestOffer(TQuest& Quest, pas::WideString& ResponseText);
        pas::WideString BuildQuestText(const TQuest& Quest, TQuestTextKind Kind);
        static void PublishQuestStatus(PQuest Quest, std::int32_t Outcome);
        void ProcessShipDestructionQuests(aShip::TShip* Ship);
        std::uint8_t HasQuestOfType(aGalaxyStruct::TQuestType QuestType);
        std::uint8_t ShouldKeepShipForQuests(aShip::TShip* Ship);
        void RefreshPlayerQuestTargets();
        std::uint16_t PlaceInRating;
        std::uint8_t ExcludedFromRating;
        pas::Array<std::uint8_t, 0, 2> CareerStatus;
        pas::Array<std::uint8_t, 0, 2> EminentProgress;
        pas::Array<std::uint8_t, 0, 2> PendingCareerActivity;
        aGalaxyStruct::TRangerCareer PreferredCareer;
        std::uint8_t Aggression;
        std::uint8_t cpp_padding[2];
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
        std::uint8_t Declined;
        aGalaxyStruct::TQuestType QuestType;
        std::uint8_t cpp_padding[1];
        std::uint16_t QuestNumber;
        std::uint8_t cpp_padding_2[2];
    };
    #pragma pack(pop)

    using PPlayerOldQuest = TPlayerOldQuest*;

} // namespace aRanger
