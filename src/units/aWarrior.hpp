#pragma once
#include "types/aConst.hpp"
#include "types/aWarrior.hpp"

namespace aItem {
    struct TWeapon;

} // namespace aItem

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

    void TWarrior_RepairBrokenEquipmentAtLocation(TWarrior* Self);

    std::uint8_t TWarrior_RecomputeFearState(TWarrior* Self);

    std::uint8_t TWarrior_AcceptsRansomDemandFrom(TWarrior* Self, aShip::TShip* Ship);

    std::uint8_t TWarrior_TrustsAttackRequester(TWarrior* Self, aShip::TShip* Ship);

    std::uint8_t TWarrior_EvaluateAllyRelationAndStrength(TWarrior* Self, aShip::TShip* Ship);

    // Native diagnostic name: TWarrior.ArmsToTarget.
    void TWarrior_AssignWeaponTargetsInStar(TWarrior* Self);

    // Flagship branch; shares the TWarrior.ArmsToTarget diagnostic.
    void TWarrior_AssignFlagshipWeaponTargets(TWarrior* Self);

    aShip::TShip* TWarrior_FindNearestFriendlyFlagship(TWarrior* Self);

    void TWarrior_ManeuverFlagship(TWarrior* Self);

    std::uint8_t TWarrior_BuildMoneyExtortionResponse(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount);

    std::uint8_t TWarrior_BuildCargoExtortionResponse(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response);

    std::uint8_t TWarrior_BuildAttackRequestResponse(TWarrior* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target);

    std::uint8_t TWarrior_AcceptPartnershipOffer(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    std::uint8_t TWarrior_BuildPartnershipOfferResponse(TWarrior* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    float TWarrior_EvaluateStatBonus(TWarrior* Self, aConst::TEquipmentBonusKind BonusKind, std::int32_t Value);

    float TWarrior_EvaluateWeaponDamage(TWarrior* Self, aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage);

    void TWarrior_RefreshCurrentStanding(TWarrior* Self);

} // namespace aWarrior
