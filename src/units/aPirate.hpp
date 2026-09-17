#pragma once
#include "types/aPirate.hpp"

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aPirate {
    // Entries 22..27 of the dispatch table enter. These reads reload
    // the unchanged BonusKind byte at EBP-5; is the base biased by -22*4.
    extern const pas::Array<std::int32_t, 22, 27> PirateSkillBonusWeights;

    // EvaluateStatBonus dispatch bounds the indexed BonusKind to 13..20.
    extern const pas::Array<std::int32_t, 13, 20> PirateSlotBonusWeights;

    void TPirate_Destroy(TPirate* Self);

    void TPirate_NextDay(TPirate* Self);

    void TPirate_NextDayLogic(TPirate* Self);

    // AI ownership check only; does not test travel range.
    std::uint8_t TPirate_CanQueueReachablePlanet(TPirate* Self, aPlanet::TPlanet* Planet);

    std::uint8_t TPirate_RecomputeFearState(TPirate* Self);

    std::uint8_t TPirate_AcceptsRansomDemandFrom(TPirate* Self, aShip::TShip* Ship);

    std::uint8_t TPirate_TrustsAttackRequester(TPirate* Self, aShip::TShip* Ship);

    std::uint8_t TPirate_BuildMoneyExtortionResponse(TPirate* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount);

    std::uint8_t TPirate_BuildCargoExtortionResponse(TPirate* Self, aShip::TShip* OtherShip, pas::WideString& Response);

    std::uint8_t TPirate_BuildAttackRequestResponse(TPirate* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target);

    std::uint8_t TPirate_AcceptPartnershipOffer(TPirate* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    std::uint8_t TPirate_BuildPartnershipOfferResponse(TPirate* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    void TPirate_RefreshCurrentStanding(TPirate* Self);

} // namespace aPirate
