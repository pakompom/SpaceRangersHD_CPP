#pragma once
#include "types/aNormalShip.hpp"

namespace aNormalShip {
    static_assert(sizeof(void*) != 4 || sizeof(aNormalShip::TSystemKillCounts) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TSystemKillCounts, Normal) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TSystemKillCounts, Dominator) == 2);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TSystemKillCounts, Pirate) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TSystemKillCounts, Custom) == 6);
    static_assert(sizeof(void*) != 4 || sizeof(aNormalShip::TNormalShip) == 1296);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, LastDockedPlanet) == 1232);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, TotalShipKillCount) == 1236);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, PirateKillCount) == 1240);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, DominatorKillCount) == 1244);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, LiberatedSystemCount) == 1248);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, CivilianKillCount) == 1252);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, MilitaryKillCount) == 1256);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, RangerKillCount) == 1260);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, CurrentSystemKills) == 1264);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, PendingLiberationCeremonyPlanet) == 1272);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, PendingLiberationContribution) == 1276);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, Rank) == 1280);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, RankPoints) == 1282);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, LastPlayerExtortionTurn) == 1284);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, PirateRank) == 1288);
    static_assert(sizeof(void*) != 4 || offsetof(aNormalShip::TNormalShip, PirateRankPoints) == 1292);

} // namespace aNormalShip
