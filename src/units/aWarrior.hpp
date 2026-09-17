#pragma once
#include "types/aWarrior.hpp"

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aWarrior {
    extern const pas::Array<std::int32_t, 22, 27> WarriorSkillBonusWeights;

    // EvaluateStatBonus dispatches bonus kinds 13..20 to these slot cases.
    extern const pas::Array<std::int32_t, 13, 20> WarriorSlotBonusWeights;

    void TWarrior_Destroy(TWarrior* Self);

    void TWarrior_NextDay(TWarrior* Self);

    void TWarrior_NextDayLogic(TWarrior* Self);

    // Flagship branch; its diagnostic retains TWarrior.NextDayLogic.
    void TWarrior_NextDayFlagshipLogic(TWarrior* Self);

    // AI ownership check only; does not test travel range.
    std::uint8_t TWarrior_CanQueueReachablePlanet(TWarrior* Self, aPlanet::TPlanet* Planet);

    std::uint8_t TWarrior_RecomputeFearState(TWarrior* Self);

    std::uint8_t TWarrior_AcceptsRansomDemandFrom(TWarrior* Self, aShip::TShip* Ship);

    std::uint8_t TWarrior_TrustsAttackRequester(TWarrior* Self, aShip::TShip* Ship);

    // Flagship branch; shares the TWarrior.ArmsToTarget diagnostic.
    void TWarrior_AssignFlagshipWeaponTargets(TWarrior* Self);

    std::uint8_t TWarrior_BuildMoneyExtortionResponse(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount);

    std::uint8_t TWarrior_BuildCargoExtortionResponse(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response);

    std::uint8_t TWarrior_BuildAttackRequestResponse(TWarrior* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target);

    std::uint8_t TWarrior_AcceptPartnershipOffer(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    std::uint8_t TWarrior_BuildPartnershipOfferResponse(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    void TWarrior_RefreshCurrentStanding(TWarrior* Self);

} // namespace aWarrior
