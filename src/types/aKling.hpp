#pragma once
#include "runtime_support.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aGalaxy {
    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aKling {
    struct TKling;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TKling : aShip::TShip {
        PAS_CLASS_META(TKling, aShip::TShip, "TKling", 1244)
        void p_destroy() override;
        void InitBlazer(aGalaxy::TStar* Star);
        void InitKeller(aGalaxy::TStar* Star);
        void InitTerron(aGalaxy::TStar* Star);
        void InitializeDominator(aGalaxyStruct::TKlingType Kind, aPlanet::TPlanet* Planet, aGalaxyStruct::TDominatorSeries Series);
        void InitGenerated(aGalaxyStruct::TKlingType Kind, aPlanet::TPlanet* Planet, aGalaxyStruct::TDominatorSeries Series);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void NextDay() override;
        void NextDayLogic() override;
        void MiniBossNextDayLogic();
        void BlazerNextDayLogic();
        void KellerNextDayLogic();
        void TerronNextDayLogic();
        aShip::TShip* SelectBertorLeader();
        std::uint8_t ShouldKamikaze();
        std::uint8_t LandOnRandomFriendlyPlanet(std::uint8_t OverrideScriptOrder);
        void BuildReachablePlanetQueue() override;
        std::uint8_t CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        std::uint8_t RetreatToReinforcedStar();
        std::uint8_t RetreatIfHullCritical();
        aGalaxy::TStar* FindKellerAttackTarget();
        static aGalaxy::TStar* FindKellerReinforcementTarget();
        void SelectKellerMission();
        void SelectKellerReinforcementMission();
        void OpenKellerMissionHole();
        std::uint8_t RelocateBertorWithinConstellation();
        std::int32_t SpawnEscortShips(aGalaxyStruct::TKlingType Kind, std::int32_t DesiredCount);
        void CoordinateSeriesInvasions(aGalaxyStruct::TDominatorSeries Series);
        void MoveToRandomPatrolPoint();
        void MoveNearKellerMissionHole();
        void RepairBrokenEquipmentAtLocation() override;
        aGalaxy::TStar* GetHomeStar() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        std::uint8_t GetGreetingShipCategory() override;
        aGalaxyStruct::TRangerCareer GetDominantCareer() override;
        std::uint8_t GetStrengthScaledPirateStatus() override;
        std::int32_t GetDesiredCargoFreeSpace() override;
        std::uint8_t IsProgramActive(std::uint8_t ProgramId);
        void RefuelAtLocation() override;
        void SetInventoryDominatorOwner();
        void ImproveStandardEquipment();
        std::uint8_t RelationToNonRanger(aShip::TShip* Ship) override;
        std::uint8_t RelationToRanger(void* Ranger) override;
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) override;
        void ReactToAttack(aShip::TShip* Attacker) override;
        std::uint8_t RecomputeFearState() override;
        std::uint8_t AcceptsRansomDemandFrom(aShip::TShip* Ship) override;
        std::uint8_t TrustsAttackRequester(aShip::TShip* Ship) override;
        std::uint8_t EvaluateAllyRelationAndStrength(aShip::TShip* Ship) override;
        void AssignWeaponTargetsInStar() override;
        void DetectAttackingPlayer(aShip::TShip* Attacker);
        std::uint8_t IsPlayerCamouflageEffective(aShip::TShip* Ship);
        void SelectEnemyShipInStar() override;
        void EngageEnemyShip() override;
        void UpdateAfterburnerState() override;
        void ProcessCombatDialogue() override;
        void ReactToExtortionDemand(void* Ranger) override;
        std::uint8_t BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) override;
        std::uint8_t BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) override;
        std::uint8_t BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) override;
        std::uint8_t BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) override;
        std::uint8_t AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        std::uint8_t BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        void RefreshCombatSkills();
        std::int32_t CalculateSpeed() override;
        std::uint8_t HasNearbyBertorAura();
        void RefreshCurrentStanding() override;
        aGalaxyStruct::TKlingType KlingType;
        aGalaxyStruct::TDominatorSeries DominatorSeries;
        std::uint8_t cpp_padding[2];
        std::int32_t ActiveProgramAppliedTurn;
        std::uint8_t ActiveProgramId;
        std::uint8_t AuraEffectShownThisTurn;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aKling
