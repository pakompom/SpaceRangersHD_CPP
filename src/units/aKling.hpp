#pragma once
#include "types/aKling.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aKling {
    // Maximum-size index followed by minimum-size index.
    extern pas::Array<pas::Array<std::int32_t, 0, 1>, 0, 7> DominatorEquipmentSizeIndices;

    extern pas::Array<std::int32_t, 1, 7> DominatorWeaponDistributionByTier;

    extern pas::Array<pas::Array<pas::Array<std::int32_t, 50, 64>, 0, 7>, 1, 4> DominatorWeaponWeights;

    // Control threshold and paired equipment/weapon generation bounds.
    extern pas::Array<pas::Array<std::int32_t, 1, 19>, 1, 7> DominatorGenerationTuning;

    extern aKling::TKling* BlazerShip;

    extern aKling::TKling* KellerShip;

    extern aKling::TKling* TerronShip;

    // PIECECREATOR target selected during new-game generation and persisted with the galaxy.
    extern std::uint32_t PieceCreatorTargetStarId;

    extern aPlanet::TPlanet* DominatorSpawnPlanet;

    // Native ANSI exception text contains UTF-8 bytes; ordinary Russian literals
    // compile to Windows-1251. Text: Клинг выпустился со скоростью 0
    extern const pas::WideString DominatorZeroSpeedError;

    void TKling_Destroy(TKling* Self);

    void TKling_ResolveLoadedReferences(TKling* Self, aGalaxy::TGalaxy* Galaxy);

    void TKling_NextDay(TKling* Self);

    void TKling_NextDayLogic(TKling* Self);

    // AI ownership check only; does not test travel range.
    std::uint8_t TKling_CanQueueReachablePlanet(TKling* Self, aPlanet::TPlanet* Planet);

    std::uint8_t TKling_RecomputeFearState(TKling* Self);

    std::uint8_t TKling_AcceptsRansomDemandFrom(TKling* Self, aShip::TShip* Ship);

    std::uint8_t TKling_TrustsAttackRequester(TKling* Self, aShip::TShip* Ship);

    std::uint8_t TKling_BuildMoneyExtortionResponse(TKling* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount);

    std::uint8_t TKling_BuildCargoExtortionResponse(TKling* Self, aShip::TShip* OtherShip, pas::WideString& Response);

    std::uint8_t TKling_BuildAttackRequestResponse(TKling* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target);

    std::uint8_t TKling_AcceptPartnershipOffer(TKling* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    std::uint8_t TKling_BuildPartnershipOfferResponse(TKling* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    void TKling_RefreshCurrentStanding(TKling* Self);

} // namespace aKling
