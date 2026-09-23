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

namespace aTransport {
    struct TTransport;

    enum TTransportType : std::uint8_t {
        ttTransport = 0,
        ttLiner = 1,
        ttDiplomat = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTransport : aNormalShip::TNormalShip {
        PAS_CLASS_META(TTransport, aNormalShip::TNormalShip, "TTransport", 1300)
        void p_destroy() override;
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void virtual_TShip_NextDay() override;
        void virtual_TShip_NextDayLogic() override;
        // May issue a movement order when repairs are needed; result is borrowed and may be nil.
        aPlanet::TPlanet* SelectRepairOrTradePlanet();
        // Requires a non-nil PlanetQueue; an empty queue returns nil. Result is borrowed.
        aPlanet::TPlanet* SelectTradePlanet();
        void BuildReachablePlanetQueue() override;
        std::uint8_t virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        // Requires CurrentPlanet. Can sell below cost; purchases can exhaust money and cargo space.
        void ProcessTrading();
        // Restores equipment condition without charging Money.
        void RepairBrokenEquipmentAtLocation() override;
        aGalaxy::TStar* GetHomeStar() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        pas::WideString GetTypeNameKey() override;
        aGalaxyStruct::TGreetingShipCategory GetGreetingShipCategory() override;
        // Always rcTrader.
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
        void AssignWeaponTargetsInStar() override;
        void SelectEnemyShipInStar() override;
        void EngageEnemyShip() override;
        void ProcessCombatDialogue() override;
        void ReactToExtortionDemand(void* Ranger) override;
        std::uint8_t virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) override;
        std::uint8_t virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) override;
        std::uint8_t BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) override;
        std::uint8_t virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) override;
        std::uint8_t virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        std::uint8_t virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) override;
        float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) override;
        float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) override;
        void virtual_TShip_RefreshCurrentStanding() override;
        // Script.ShipSubType.
        TTransportType TransportType;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aTransport
