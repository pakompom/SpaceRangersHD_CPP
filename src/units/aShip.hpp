#pragma once
#include "types/aShip.hpp"

namespace aItem {
    struct TGoods;

} // namespace aItem

namespace aShip {
    extern std::int8_t SimulationContext;

    extern pas::Array<std::uint8_t, 0, 2> DamageScriptActionTypes;

    extern aItem::TGoods* TradeGoodsSold;

    extern aItem::TGoods* TradeGoodsCostBasis;

    extern pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2> DominatorShipSmallSizes;

    extern pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2> DominatorShipLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> RangerSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> RangerLargeSizes;

    extern pas::Array<pas::Array<std::int32_t, 0, 7>, 3, 5> TransportSmallSizes;

    extern pas::Array<pas::Array<std::int32_t, 0, 7>, 3, 5> TransportLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> PirateSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> PirateLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> PirateClanSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> PirateClanLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> WarriorSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> WarriorLargeSizes;

    extern pas::Array<std::int32_t, 0, 7> BigWarriorSmallSizes;

    extern pas::Array<std::int32_t, 0, 7> BigWarriorLargeSizes;

    extern std::int32_t TranclucatorSmallSize;

    extern std::int32_t TranclucatorLargeSize;

    extern std::int32_t SpecialHullSmallSize;

    extern std::int32_t SpecialHullLargeSize;

    extern std::int32_t StationSize;

    extern std::int32_t DefaultShipSmallSize;

    extern std::int32_t DefaultShipLargeSize;

    extern pas::Array<std::int32_t, 22, 27> SkillBonusEvaluationWeights;

    extern pas::Array<std::int32_t, 13, 20> SlotBonusEvaluationWeights;

    extern pas::Array<double, 0, 7> KlingCheapDropValueFactors;

    extern pas::Array<double, 0, 7> KlingValuableDropValueFactors;

    extern const pas::Array<double, 0, 7> DominatorProgramDropCostFactors;

    TShip* CreateShipByType(std::uint8_t ShipType);

    float CompareShipGroupsStrength(pas::List* Ships, pas::List* Opponents);

    float CalculateFuelCost(std::int32_t Amount, std::uint8_t OwnerId);

    std::int32_t CalculateRoundedFuelCost(std::int32_t Amount, std::uint8_t OwnerId);

    void ClampMissileWeaponRange(TShip* Ship, std::int32_t TemplateRange, std::int32_t& Range);

    void TShip_Create(TShip* Self);

    void TShip_Destroy(TShip* Self);

} // namespace aShip
