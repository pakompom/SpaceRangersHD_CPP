#pragma once
#include "types/aRuins.hpp"

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

} // namespace aRuins
