#pragma once
#include "types/aConst.hpp"
#include "types/aTranclucator.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aTranclucator {
    extern pas::Array<std::int32_t, 22, 27> TranclucatorSkillBonusWeights;

    // The dispatch table confines these reads to BonusKind=13..20.
    // Each reloads the unchanged byte at EBP-5; is the base biased by -13*4.
    extern pas::Array<std::int32_t, 13, 20> TranclucatorSlotBonusWeights;

    void TTranclucator_Create(TTranclucator* Self);

    void TTranclucator_Destroy(TTranclucator* Self);

    void TTranclucator_ResolveLoadedReferences(TTranclucator* Self, aGalaxy::TGalaxy* Galaxy);

    void TTranclucator_NextDay(TTranclucator* Self);

    // Zero-byte pointer additions retain the native evaluation order of IndexOf.
    void TTranclucator_NextDayLogic(TTranclucator* Self);

    void TTranclucator_RepairBrokenEquipmentAtLocation(TTranclucator* Self);

    std::uint8_t TTranclucator_CanQueueReachablePlanet(TTranclucator* Self, aPlanet::TPlanet* Planet);

    // Returns whether a move order is active; nearby pickups can be queued even when the result is false.
    std::uint8_t TTranclucator_TryCollectPreferredFloatingLoot(TTranclucator* Self, std::int32_t MaxTravelDays);

    void TTranclucator_AssignWeaponTargetsInStar(TTranclucator* Self);

    std::uint8_t TTranclucator_RecomputeFearState(TTranclucator* Self);

    std::uint8_t TTranclucator_AcceptsRansomDemandFrom(TTranclucator* Self, aShip::TShip* Ship);

    std::uint8_t TTranclucator_TrustsAttackRequester(TTranclucator* Self, aShip::TShip* Ship);

    std::uint8_t TTranclucator_EvaluateAllyRelationAndStrength(TTranclucator* Self, aShip::TShip* Ship);

    std::uint8_t TTranclucator_BuildMoneyExtortionResponse(TTranclucator* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount);

    std::uint8_t TTranclucator_BuildCargoExtortionResponse(TTranclucator* Self, aShip::TShip* OtherShip, pas::WideString& Response);

    std::uint8_t TTranclucator_BuildAttackRequestResponse(TTranclucator* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target);

    std::uint8_t TTranclucator_AcceptPartnershipOffer(TTranclucator* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    std::uint8_t TTranclucator_BuildPartnershipOfferResponse(TTranclucator* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    void TTranclucator_RefreshCurrentStanding(TTranclucator* Self);

    float TTranclucator_EvaluateStatBonus(TTranclucator* Self, aConst::TEquipmentBonusKind BonusKind, std::int32_t Value);

} // namespace aTranclucator
