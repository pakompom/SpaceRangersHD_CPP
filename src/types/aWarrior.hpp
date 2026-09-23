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

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aWarrior {
    struct TWarrior;

    // BuyWarrior / BuyFlagship and GetDefaultHullType distinguish these subtypes.
    enum TWarriorType : std::uint8_t {
        wtRegular = 0,
        wtFlagship = 1,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TWarrior : aNormalShip::TNormalShip {
        PAS_CLASS_META(TWarrior, aNormalShip::TNormalShip, "TWarrior", 1300)
        void p_destroy() override;
        // Sets location, money and WarriorType; registers the ship with its star and home garrison.
        void InitGenerated(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, TWarriorType Kind);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void virtual_TShip_NextDay() override;
        void virtual_TShip_NextDayLogic() override;
        std::uint8_t NavigateToHomePlanet();
        void BuildReachablePlanetQueue() override;
        std::uint8_t virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        void MoveToRandomPatrolPoint();
        void RepairBrokenEquipmentAtLocation() override;
        aGalaxy::TStar* GetHomeStar() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        pas::WideString GetTypeNameKey() override;
        aGalaxyStruct::TGreetingShipCategory GetGreetingShipCategory() override;
        // Always rcWarrior.
        aGalaxyStruct::TRangerCareer GetDominantCareer() override;
        aGalaxyStruct::TPercent GetStrengthScaledPirateStatus() override;
        std::int32_t GetDesiredCargoFreeSpace() override;
        // Fills installed fuel tanks without charging Money.
        void RefuelAtLocation() override;
        void ProcessUnseenProgression();
        std::uint8_t RelationToNonRanger(aShip::TShip* Ship) override;
        std::uint8_t RelationToRanger(void* Ranger) override;
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) override;
        void ReactToAttack(aShip::TShip* Attacker) override;
        std::uint8_t virtual_TShip_RecomputeFearState() override;
        std::uint8_t virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) override;
        std::uint8_t virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) override;
        std::uint8_t AcceptsAppealFrom(aShip::TShip* Ship) override;
        // Native diagnostic name: TWarrior.ArmsToTarget.
        void AssignWeaponTargetsInStar() override;
        aShip::TShip* FindNearestFriendlyFlagship();
        void SelectEnemyShipInStar() override;
        void EngageEnemyShip() override;
        void ManeuverFlagship();
        // Scores Coalition systems and same-race garrisons; moves the flagship between home rosters without changing its current position.
        void ReassignFlagshipHomePlanet();
        void ProcessCombatDialogue() override;
        void ReactToExtortionDemand(void* Ranger) override;
        std::uint8_t virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) override;
        std::uint8_t virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) override;
        std::uint8_t BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) override;
        std::uint8_t virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) override;
        std::uint8_t virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        std::uint8_t virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        void MoveToRandomPlanetOrbit();
        // Requires the home system and (Id + CurrentTurn) mod 100 < 25.
        std::uint8_t IsHomePatrolTurn();
        void ConsumeNodes(std::int32_t Amount);
        std::uint8_t RefusesFactionNegotiation(aShip::TShip* OtherShip) override;
        float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) override;
        float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) override;
        float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) override;
        std::uint8_t AcceptPickupItem(aItem::TItem* Item) override;
        std::uint8_t AcceptPickupDistance(aItem::TItem* Item, double Distance) override;
        void virtual_TShip_RefreshCurrentStanding() override;
        // wtRegular / wtFlagship; exposed as Script.ShipSubType.
        TWarriorType WarriorType;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aWarrior
