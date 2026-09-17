#pragma once
#include "types/aTransport.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aTransport {
    extern pas::Array<std::int32_t, 22, 27> TransportSkillBonusWeights;

    // Native eight-entry slot table; artefact/afterburner weights are zero.
    extern pas::Array<std::int32_t, 13, 20> TransportSlotBonusWeights;

    void TTransport_Destroy(TTransport* Self);

    // Registers Self with the planet's star; requires a fresh instance.
    void TTransport_InitGenerated(TTransport* Self, aPlanet::TPlanet* Planet, std::int32_t InitialMoney, TTransportType SubType, std::uint8_t RandomizeSubType);

    void TTransport_ResolveLoadedReferences(TTransport* Self, aGalaxy::TGalaxy* Galaxy);

    void TTransport_NextDay(TTransport* Self);

    void TTransport_NextDayLogic(TTransport* Self);

    // AI ownership check only; does not test travel range.
    std::uint8_t TTransport_CanQueueReachablePlanet(TTransport* Self, aPlanet::TPlanet* Planet);

    // Updates InFear and may replace EnemyShip.
    std::uint8_t TTransport_RecomputeFearState(TTransport* Self);

    std::uint8_t TTransport_AcceptsRansomDemandFrom(TTransport* Self, aShip::TShip* Ship);

    std::uint8_t TTransport_TrustsAttackRequester(TTransport* Self, aShip::TShip* Ship);

    std::uint8_t TTransport_BuildMoneyExtortionResponse(TTransport* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount);

    std::uint8_t TTransport_BuildCargoExtortionResponse(TTransport* Self, aShip::TShip* OtherShip, pas::WideString& Response);

    std::uint8_t TTransport_BuildAttackRequestResponse(TTransport* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target);

    std::uint8_t TTransport_AcceptPartnershipOffer(TTransport* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    std::uint8_t TTransport_BuildPartnershipOfferResponse(TTransport* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    void TTransport_RefreshCurrentStanding(TTransport* Self);

} // namespace aTransport
