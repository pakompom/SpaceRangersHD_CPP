#pragma once
#include "types/EC_Struct.hpp"
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aRuins.hpp"

namespace aGalaxy {
    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aItem {
    struct TEquipment;

    struct TWeapon;

} // namespace aItem

namespace aMyFunction {
    struct TObjectList;

} // namespace aMyFunction

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aRuins {
    extern const pas::Array<pas::Array<std::uint8_t, 0, 1>, 6, 12> StationPilotRaces;

    extern const pas::Array<aRuins::TStationHullGeneration, 6, 12> StationHullGeneration;

    extern const pas::Array<aRuins::TStationLevelRange, 6, 12> StationDefenseLevels;

    extern const pas::Array<aRuins::TStationLevelRange, 6, 12> StationRepairLevels;

    extern const pas::Array<aRuins::TStationWeaponGeneration, 6, 12> StationWeaponGeneration;

    extern const pas::Array<pas::Array<std::uint8_t, 0, 2>, 6, 12> StationWeaponTypes;

    extern const pas::Array<std::int32_t, 22, 27> StationSkillBonusWeights;

    extern const pas::Array<std::int32_t, 6, 12> StationOfferHullLevelBonus;

    extern const pas::Array<aRuins::TStationHullTypes, 6, 12> StationOfferHullTypes;

    extern const pas::Array<aRuins::TStationHullTypes, 6, 12> StationOfferRareHullTypes;

    extern const pas::Array<std::int32_t, 6, 13> StationOfferWeaponLevelBonus;

    extern const pas::Array<pas::Array<std::int32_t, 43, 49>, 6, 13> StationOfferEquipmentLevelBonus;

    void TRuins_Create(TRuins* Self);

    void TRuins_Destroy(TRuins* Self);

    // Also registers the station in Star. Type 13 inherits ranger-center defaults.
    void TRuins_Init(TRuins* Self, aGalaxyStruct::TStationType StationType, aGalaxy::TStar* Star, pas::WideString TypeNameOverride);

    void TRuins_ResolveLoadedReferences(TRuins* Self, aGalaxy::TGalaxy* Galaxy);

    void TRuins_NextDay(TRuins* Self);

    void TRuins_NextDayLogic(TRuins* Self);

    // Sponsored stations restore equipment condition without a repair transaction.
    void TRuins_RepairBrokenEquipmentAtLocation(TRuins* Self);

    // Disabled in modes 1 and 3; otherwise follows the station's weekly schedule after initial setup.
    void TRuins_RefreshShopInventory(TRuins* Self);

    // Always false.
    std::uint8_t TRuins_CanQueueReachablePlanet(TRuins* Self, aPlanet::TPlanet* Planet);

    // Targets hostile ships, incoming missiles and nearby asteroids.
    void TRuins_AssignWeaponTargetsInStar(TRuins* Self);

    // Posts the native Pirate Clan success/failure message after the station reappears.
    void TRuins_ReportAbductionOutcome(TRuins* Self);

    float TRuins_EvaluateRelocationPosition(TRuins* Self, EC_Struct::TPointF Point);

    // Returns false without changing InFear.
    std::uint8_t TRuins_RecomputeFearState(TRuins* Self);

    std::uint8_t TRuins_AcceptsRansomDemandFrom(TRuins* Self, aShip::TShip* Ship);

    std::uint8_t TRuins_TrustsAttackRequester(TRuins* Self, aShip::TShip* Ship);

    std::uint8_t TRuins_EvaluateAllyRelationAndStrength(TRuins* Self, aShip::TShip* Ship);

    std::uint8_t TRuins_BuildMoneyExtortionResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount);

    std::uint8_t TRuins_BuildCargoExtortionResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response);

    std::uint8_t TRuins_BuildAttackRequestResponse(TRuins* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target);

    std::uint8_t TRuins_AcceptPartnershipOffer(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    std::uint8_t TRuins_BuildPartnershipOfferResponse(TRuins* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount);

    float TRuins_EvaluateStatBonus(TRuins* Self, aConst::TEquipmentBonusKind BonusKind, std::int32_t Value);

    float TRuins_EvaluateWeaponDamage(TRuins* Self, aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage);

    // Script state can force standing 9 or preserve the current value; type 13 has no default assignment.
    void TRuins_RefreshCurrentStanding(TRuins* Self);

    // Accepts types 42..68; weapon types all select the weapon generator. Does not insert the result into EquipmentShop.
    aItem::TEquipment* TRuins_GenerateEquipmentOffer(TRuins* Self, pas::Object* Ship, aPlanet::TPlanet* Planet, std::uint8_t ItemType);

    // Returns a new owning list of offers. Ignores the Boolean argument.
    aMyFunction::TObjectList* TRuins_GenerateEquipmentOfferBatch(TRuins* Self, aShip::TShip* Ship, std::uint8_t UnusedForceGeneratedOffers);

    // Zero-based module index. Deterministic chain keyed by station, current turn and player RNG state; Kind 1 selects priorities 31..69, Kind 2 selects 0..20 or 10..30, others 70..100.
    std::int32_t TRuins_SelectServiceMicroModule(TRuins* Self, std::int32_t Kind, std::int32_t Index, std::uint8_t InvertRarity);

    std::uint8_t TRuins_CanDock(TRuins* Self, aShip::TShip* Ship);

} // namespace aRuins
