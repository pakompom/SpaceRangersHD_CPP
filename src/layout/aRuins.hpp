#pragma once
#include "types/aRuins.hpp"

namespace aRuins {
    static_assert(sizeof(void*) != 4 || sizeof(aRuins::TRuins) == 1384);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, EquipmentShop) == 1232);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, ShopGoods) == 1236);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, RelocationAge) == 1364);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, FlyToStar) == 1368);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, FlyDate) == 1372);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, SatelliteOffer) == 1376);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, SpecialServiceActive) == 1380);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, ModernizationSponsor) == 1381);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, NoLanding) == 1382);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TRuins, ShopUpdateMode) == 1383);
    static_assert(sizeof(void*) != 4 || sizeof(aRuins::TStationHullGeneration) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationHullGeneration, MinSize) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationHullGeneration, MaxSize) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationHullGeneration, TechSizeBonus) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationHullGeneration, MinLevel) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationHullGeneration, MaxLevel) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(aRuins::TStationLevelRange) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationLevelRange, Minimum) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationLevelRange, Maximum) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(aRuins::TStationWeaponGeneration) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationWeaponGeneration, BasicLevel) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationWeaponGeneration, IntermediateLevel) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationWeaponGeneration, AdvancedLevel) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aRuins::TStationWeaponGeneration, MinimumRange) == 12);

} // namespace aRuins
