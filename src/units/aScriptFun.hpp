#pragma once
#include "runtime_support.hpp"

namespace EC_Expression {
    struct TCodeEC;

    struct TVarArrayEC;

    struct TVarEC;

} // namespace EC_Expression

namespace aScriptFun {
    void SF_GRun(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GCntRun(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GLastTurnRun(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GAllCntRun(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_IsScriptActive(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetValueFromScript(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RunFunctionFromScript(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetVariableName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetVariableType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StatusPlayer(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddPlanetNews(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddJournalRecord(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AutoBattle(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetOwner(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GiveReward(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GiveRewardByNom(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CountReward(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CountRewardByNom(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DeleteRewardByNom(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Rnd(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GameDateTxtByTurn(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Id(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_UseTranclucator(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HullDamage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Hitpoints(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Hit(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ChangeGlobalRelationsShips(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ChangeGlobalRelationsPlanets(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GlobalRelationsShips(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GlobalRelationsPlanets(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetRelationGroup(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetRelationPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetRelationPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CurTurn(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ConName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarMapLabel(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_IdToPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_IdToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_IdToItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetSetGoods(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipRank(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipRankPoints(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipNextRankPoints(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipRaiseRank(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarToCon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ConNear(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ConStars(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ConStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyStars(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarAngleBetween(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FindPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_IsPlayer(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GroupCount(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GroupIn(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CountIn(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ChangeState(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NearestGroup(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarAngle(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NewsAdd(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MsgAdd(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Ether(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomEther(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EtherDelete(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EtherIdAdd(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EtherIdDelete(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EtherState(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ConChangeRelationToRanger(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Format(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DeleteTags(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Dialog(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DText(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DAddText(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DAdd(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DChange(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DAnswer(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Player(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemExist(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemIn(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemCost(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemCount(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipPicksItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DropItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DropScriptItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DeleteEquipment(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DecayGoods(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_UpsurgeGoods(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GoodsAdd(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GoodsCount(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GoodsCost(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GoodsRuinsForBuy(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipGoods(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipGoodsIllegalOnPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GoodsDrop(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_UselessItemCreate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GoodsSellPrice(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GoodsBuyPrice(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CountTurn(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipSetBad(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GroupSetBad(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipSetPartner(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipJoin(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipOut(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AllShipOut(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInScript(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInGameEvent(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInCurScript(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInNormalSpace(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInHole(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipIsTakeoff(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCntWeapon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipWeapon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipEqInSlot(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ArtefactTypeInUse(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ArtefactTypeBoosted(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipSpeed(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EnginePower(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipJump(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipArmor(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipProtectability(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipDroidRepair(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipRadarRange(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipScanerPower(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipHookPower(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipHookRange(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipAverageDamage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipHealthFactor(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipHealthFactorStatus(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerImmunity(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipStatusEffect(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipGroup(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCanJump(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipStatistic(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerDominatorStatistic(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipMoney(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipFuel(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipFuelLow(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipStrengthInBestRanger(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipStrengthInAverageRanger(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ChanceToWin(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipFind(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RangerStatus(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RangerPlaceInRating(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RangerExcludedFromRating(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyMoney(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipDestroy(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipDestroyType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemDestroy(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RangersCapital(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GroupToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderLanding(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderJump(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GroupIs(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StateIs(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Dist(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Angle(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Dist2Star(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuyPirate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuyTransport(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Name(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShortName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FirstGiveMoney(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HaveProgramm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetProgramm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetProgramm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DomikProgramm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DomikProgrammDate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleMamaCreate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleCreate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TerronWeaponLock(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TerronGrowLock(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TerronLandingLock(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TerronToStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_KellerLeave(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_KellerNewResearch(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_KellerKill(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BlazerLanding(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BlazerSelfDestruction(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyShipId(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NearCivilPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SkipGreeting(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Sound(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Tips(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TipsState(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CT(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BlockExist(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetMainData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetGameOptions(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ResourceExist(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SFT(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CurrentMods(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RobotSupport(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarShips(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarPlanets(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarMissiles(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarAsteroids(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GroupShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipItems(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipArts(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerTranclucators(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ArtTranclucatorToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TranclucatorData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_LinkItemToScript(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ReleaseItemFromScript(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptItemData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptItemTextData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptItemToItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetShipPirateRank(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipPirateRankPoints(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipNextPirateRankPoints(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInPirateClan(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipOnSidePirateClan(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RaisePirateRank(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomWeaponType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemFullName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemSize(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemOwner(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemSubrace(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemIsInUse(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemIsInSet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerEqSet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemIsBroken(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCanUseEq(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCanRepairEq(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipTechLevelKnowledge(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_WeaponTarget(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetEquipmentStats(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetEquipmentStats(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateHull(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateEquipment(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateArt(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateCustomWeapon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateCustomArt(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomArtData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomArtTextData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateMM(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateNodes(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateCustomCountableItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateZond(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ExistingZonds(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FreeItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipJoinsClan(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddItemToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetItemFromShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetArtFromShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ArrangeItems(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddItemToPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetItemFromPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddItemToShop(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetItemFromShop(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddItemToStorage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetItemFromStorage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FindItemInStorage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PutItemInVault(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetItemFromVault(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DropItemInSystem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StopMovingItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarItems(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetItemFromStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetItems(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StorageItems(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StorageItemLocation(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShopItems(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddDialogOverride(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddDialogInject(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_InjectAnswer(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AddDialogBlock(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GotoGov(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetShipPlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetShipHomePlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetShipRuins(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetTalkShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetTalkType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TalkByAI(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptRun(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateABShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ConvertToABShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ABShipModifiers(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StartAB(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StartTextQuest(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StartRobots(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MarkRobotsMapAsUsed(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipOwner(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipPilotRace(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipSkill(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipFace(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipFreeExp(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetShipExpByType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CoordX(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CoordY(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipSetCoords(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipAngle(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ObjectType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInHyperSpace(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipStatus(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuyRanger(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuyWarrior(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuyBigWarrior(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuyDomik(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuyDomikExtremal(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuyTranclucator(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TransferShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderForsage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderNone(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderMove(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderTeleport(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderTakeOff(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderFollowShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderJumpHole(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RelationToRanger(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RelationToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarOwner(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarBattle(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarSeries(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarHoles(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarNearbyStars(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarNearbyStarsDist(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarSetGraph(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreatePlanet(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetSetGraph(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetGetGraph(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetPopulation(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetOwner(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetRace(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetGov(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetEco(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetTerrain(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetTerrainExplored(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetOrbitRadius(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetOrbitalVelocity(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetSize(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetCurInvention(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetCurInventionPoints(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetInventionLevel(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetBoostInventions(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetWarriors(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxySectors(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyTechLevel(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyDominatorResearchPercent(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyDominatorResearchMaterial(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyDiffLevels(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SectorVisible(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HullHP(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HullDamageSuspectibility(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HullType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HullSpecial(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HullSeries(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyHoles(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleCreate2(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleStar1(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleStar2(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleX1(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleY1(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleX2(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleY2(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleTurnCreate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleMap(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_HoleGraph(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarRuins(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateQuestItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipOrder(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipTurnBeforeEndOrder(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipOrderData1(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipOrderData2(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipOrderObj(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipDestination(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuildRuins(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuildCustomRuins(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RuinsChangeType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipStanding(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipSlots(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomMissileType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileOwner(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileWeaponID(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileTarget(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileMaxDamage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileMinDamage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileLive(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileSpeed(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissileAngle(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AsteroidMinerals(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AsteroidGraph(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_AsteroidRespawn(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ArrayAdd(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_ArrayDelete(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_ArrayClear(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_ArrayDim(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_ArraySort(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_ArraySortPartial(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_ArrayRandomize(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_ArrayFind(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_ArrayFindInSorted(pas::OpenArray<EC_Expression::TVarEC*> Args, EC_Expression::TCodeEC* Code);

    void SF_DistToNearestEnemySystem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarEnemyThreatLevel(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuildListOfQuestPossibleLocations(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FindItemInShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyRangers(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MakeShipEnterStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipGetBad(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipAddDropItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OrderLock(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BonusCount(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SeriesCount(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BonusPriority(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BonusIsSpecial(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BonusName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BonusNumInCfg(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SeriesNumInCfg(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BonusValue(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FindBonusByName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FindSeriesByName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FindBonusByCustomTag(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FindBonusByNameInCfg(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BonusCustomTag(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateEquipmentWithSpecial(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SpecialToEquipment(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ModuleToEquipment(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EqSpecial(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EqModule(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MayAddBonusToEq(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuildListOfMMByPriority(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BuildListOfNewShips(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetToStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Chameleon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_IsChameleon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerChameleonCharges(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerChameleonCurType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerChameleonDetected(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerLogicChameleon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SwitchToMirrorImage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EquipmentImageName(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarFonImage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ExtremalTakeOff(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ForceNextDay(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptActionsRun(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarListToPlanetList(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EndGame(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomWin(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomLose(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PirateWin(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StartVideo(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StartMusic(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoComeKlingToStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoDropToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoTargetToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoTalkToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoScanToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoJump(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoLanding(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoShopUpdate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetExtraFlags(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NoDropItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CanSellItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TruceBetweenShips(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInPrison(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipPartners(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerPirates(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipIsPartner(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipFreeSpace(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipWealth(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DomiksDefeated(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CoalitionDefeated(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipRefuel(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipRepairEq(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemInScript(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FindPlanetByAdvancement(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarListToTransitPlanetList(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyEvents(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyEventDate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyEventType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyEventData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyEventsTextData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetNews(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetNewsDate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetNewsType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetNewsText(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ControlledSystems(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DeltaWin(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipInFear(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateGoods(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetNodesFromShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetNodesFromStorage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RangerBaseNodes(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RuinsAllowModernization(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RuinsMicromoduleChain(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DomikKilledInCurSystem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipTypeN(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipSubType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipChangeStar(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_IsFilm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FilmFlags(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShowEffect(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShowStaticEffect(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipConnect(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FilmSound(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FireWeapon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_WeaponHit(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_DealDamageToShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_LaunchMissile(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SpawnMissile(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BonusText(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetPirateClan(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Blazer(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Keller(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_Terron(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PirateType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerQuestInProgress(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerQuestsCompleted(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_QuestsStatusByNom(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerPlanetaryBattlesCompleted(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerMayTakeSubCrack(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SubCrackCost(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCalcParam(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipRefit(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipImproveItems(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemImprovement(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipFreeFlight(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipKillFactionInCurSystem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CapitalShipStats(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerBridge(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerDebt(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerDebtDate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerDebtCnt(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerDeposit(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerDepositDate(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerDepositDay(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerDepositPercent(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlayerMedPolicy(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCustomShipInfosCount(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipAddCustomShipInfo(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipDeleteCustomShipInfo(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipFindCustomShipInfoByType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCustomShipInfoDescription(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCustomShipInfoData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCustomShipInfoTextData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarCustomStarInfosCount(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarAddCustomStarInfo(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarDeleteCustomStarInfo(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarFindCustomStarInfoByType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarCustomStarInfoData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemCanBeBroken(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemFragility(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemDurability(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemLevel(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ContainerFuel(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemCharge(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MissilesToRearm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_WeaponAmmunition(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_WeaponMaxAmmunition(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipSpecialBonuses(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemExtraSpecials(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemExtraSpecialsCountByType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemExtraSpecialsAddByType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemExtraSpecialsDeleteByType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ExecuteCodeFromString(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GenerateCodeStringFromBlock(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemOnUseCode(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ItemOnActCode(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CreateActCodeEvent(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CurItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CurInfo(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptItemActShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptItemActObject1(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptItemActObject2(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptItemActParam(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ScriptItemActionType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OnUseCodeTranclucator(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OnUseCodeTransmitter(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OnUseCodeBlackHole(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OnUseCodeMissileDef(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MessageBox(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MessageBoxYesNo(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CountBox(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_NumberBox(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_TextBox(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ListBox(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FormCurShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_UselessItemText(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_UselessItemData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetAchievementSHU(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetAchievementGIRLSHIRE(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetAchievementGIRLSQUEST(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetAchievementPIRATEWIN(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetAchievementCOALLITION(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetAchievementHULL(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_UICheckElement(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_InterfaceState(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_InterfaceText(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_InterfaceImage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_InterfacePos(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_InterfaceSize(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ButtonClick(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetFocus(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CurrentForm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FormShipCurItem(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_UpdateFormShip(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_FormChange(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_RunChildForm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_OpenCustomForm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CloseCustomForm(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomInterfaceState(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomInterfaceText(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomInterfaceImage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomInterfacePos(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomInterfacePosZ(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomInterfaceSize(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarMapCenterView(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarMapCurPosX(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarMapCurPosY(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarMapCustomSelectionMode(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarMapBlinkingMessage(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_CustomWeaponTypes(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_InventNewCustomWeapon(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetCustomWeaponInfo(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetCustomWeaponData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GetCustomWeaponPrimaryDamageType(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetCustomWeaponAvailability(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetCustomWeaponSE(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetCustomWeaponPrimaryData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetCustomWeaponSizeAndCost(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetCustomWeaponDamageData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetCustomWeaponShotData(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_SetCustomMissileWeaponStats(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_StarCustomFaction(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ShipCustomFaction(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_EqCustomFaction(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_PlanetCustomFaction(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ImportedFunction(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_ImportAll(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_GalaxyPtr(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_MusicControls(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void SF_BlinkingWarning(pas::OpenArray<EC_Expression::TVarEC*> av, EC_Expression::TCodeEC* code);

    void InitializeScriptBuiltinsAndConstants(EC_Expression::TVarArrayEC* Scope);

} // namespace aScriptFun
