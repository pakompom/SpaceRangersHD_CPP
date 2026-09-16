#pragma once
#include "types/CheatCode.hpp"

namespace CheatCode {
    extern std::int32_t CheatCandidateIndex;

    extern std::int32_t CheatPrefixLength;

    extern std::int32_t CheatPrefixSum;

    extern CheatCode::TCheatList* CheatEntries;

    std::uint8_t IsCheatMessageBoxOpen();

    void HandleDebugKey(std::uint16_t Key);

    void AddCheatPoints(std::int32_t Points);

    void ReportCheat(std::int32_t Points, const pas::WideString& Name);

    void CheatRepair();

    void CheatKlissanmax();

    void CheatPiratemax();

    void CheatWarriormax();

    void CheatKlissancall();

    void CheatPiratecall();

    void CheatRangerpoints();

    void CheatNextrank();

    void CheatCoolweapon();

    void CheatLowcostweapon();

    void CheatBomb();

    void CheatRobotforce();

    void CheatArtefact();

    void CheatMoney();

    void CheatDrop();

    void CheatPacking();

    void CheatKlissanitem();

    void CheatWeaponstrength();

    void CheatTenbomb();

    void CheatRangersdream();

    void CheatRndbase();

    void CheatMapsector();

    void CheatHugemoney();

    void CheatPelengsurprise();

    void CheatSuperhull();

    void CheatBoom();

    void CheatHaterangers();

    void CheatPirates();

    void CheatGun();

    void CheatVertix();

    void CheatDevice();

    void CheatArts();

    void CheatModule();

    void CheatSkill();

    void CheatProgram();

    void CheatIllness();

    void CheatStimulant();

    void CheatIdeal();

    void CheatShowmap();

    void CheatMedal();

    void SetCheatDominatorLevel(std::int32_t Level);

    void CheatHorror();

    void CheatNightmare();

    void CheatHell();

    void CheatTechnic();

    void CheatAmmo();

    void CheatGod();

    void CheatHole();

    void CheatWin();

    void CheatHweapon();

    void CheatUltrascan();

    void CheatTentm();

    void CheatEncharge();

    void CheatExpa();

    void CheatMadeinchina();

    void CheatZawarudo();

    void ShowCheatFeedback(pas::WideString Text);

    void CheatMakedump();

    void CheatFitness();

    void CheatExtraone();

    void CheatSudo();

    void CheatEvents();

    void CheatSeed();

    void CheatInfos();

    void TCheatList_Destroy(TCheatList* Self);

    void UnitInitialize();

    void UnitFinalize();

} // namespace CheatCode
