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

namespace aPirate {
    struct TPirate;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPirate : aNormalShip::TNormalShip {
        PAS_CLASS_META(TPirate, aNormalShip::TNormalShip, "TPirate", 1308)
        void p_destroy() override;
        // Sets location, money and PirateType; registers the ship with its star.
        void InitGenerated(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::uint8_t Kind);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void virtual_TShip_NextDay() override;
        void virtual_TShip_NextDayLogic() override;
        std::uint8_t NavigateToServicePlanet(std::uint8_t Absolute);
        aPlanet::TPlanet* SelectServicePlanet();
        void BuildReachablePlanetQueue() override;
        std::uint8_t virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        // The native entry stores but never reads dl; the sole caller passes zero.
        void TryJumpToNearbyBattle(std::uint8_t UnusedMode);
        std::uint8_t TryDockAtStation(aGalaxyStruct::TShipTypeMask Types);
        // Prefers the leader's route, then nearby service locations or reachable non-Dominator stars.
        void SelectNearestReachableDestination();
        void SellAllCargoGoods();
        void RepairBrokenEquipmentAtLocation() override;
        aGalaxy::TStar* GetHomeStar() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        aGalaxyStruct::TGreetingShipCategory GetGreetingShipCategory() override;
        // Always rcPirate.
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
        void TryOfferRansomToPursuer();
        std::uint8_t virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) override;
        std::uint8_t virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) override;
        std::uint8_t AcceptsAppealFrom(aShip::TShip* Ship) override;
        // True while the current turn is spent in prison.
        std::uint8_t ProcessImprisonment();
        void AssignWeaponTargetsInStar() override;
        void SelectEnemyShipInStar() override;
        void SelectIncidentalEnemy();
        void EngageEnemyShip() override;
        void ProcessCombatDialogue() override;
        void ReactToExtortionDemand(void* Ranger) override;
        std::uint8_t virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) override;
        std::uint8_t virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) override;
        std::uint8_t BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) override;
        std::uint8_t virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) override;
        std::uint8_t virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        std::uint8_t virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        void ReviewPartnership();
        void MoveToRandomPlanetOrbit();
        std::uint8_t RefusesFactionNegotiation(aShip::TShip* OtherShip) override;
        std::uint8_t TryRetreatFromSystem();
        float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) override;
        float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) override;
        float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) override;
        std::uint8_t AcceptPickupItem(aItem::TItem* Item) override;
        std::uint8_t AcceptPickupDistance(aItem::TItem* Item, double Distance) override;
        void virtual_TShip_RefreshCurrentStanding() override;
        std::uint32_t PrisonTermRemaining;
        // Zero denotes an independent pirate; nonzero values select clan variants.
        std::uint8_t PirateType;
        std::uint8_t cpp_padding[3];
        // AI pressure affecting target selection and departure decisions.
        float RaidPressure;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aPirate
