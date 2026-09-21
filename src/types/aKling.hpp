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
        // Initializes type/series and location through, then builds the generated loadout. Series occupies one four-byte stack slot.
        void InitGenerated(aGalaxyStruct::TKlingType Kind, aPlanet::TPlanet* Planet, aGalaxyStruct::TDominatorSeries Series);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void virtual_TShip_NextDay() override;
        void virtual_TShip_NextDayLogic() override;
        void MiniBossNextDayLogic();
        void BlazerNextDayLogic();
        void KellerNextDayLogic();
        void TerronNextDayLogic();
        aShip::TShip* SelectBertorLeader();
        // Requires a live enemy in the same star and KlingType=ktKlig. Existing kamikaze mode bypasses the proximity/strength test.
        std::uint8_t ShouldKamikaze();
        std::uint8_t LandOnRandomFriendlyPlanet(std::uint8_t OverrideScriptOrder);
        void BuildReachablePlanetQueue() override;
        std::uint8_t virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        std::uint8_t RetreatToReinforcedStar();
        std::uint8_t RetreatIfHullCritical();
        aGalaxy::TStar* FindKellerAttackTarget();
        static aGalaxy::TStar* FindKellerReinforcementTarget();
        void SelectKellerMission();
        void SelectKellerReinforcementMission();
        // Advances mission state 2 to 3, creates the type-4 hole and sends Keller through it with generated reinforcements.
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
        // Always rcWarrior.
        aGalaxyStruct::TRangerCareer GetDominantCareer() override;
        aGalaxyStruct::TPercent GetStrengthScaledPirateStatus() override;
        std::int32_t GetDesiredCargoFreeSpace() override;
        // Checks the stored active flag and ID; expiration is handled by the daily ship update.
        std::uint8_t IsProgramActive(std::uint8_t ProgramId);
        // Fills installed fuel tanks without charging Money.
        void RefuelAtLocation() override;
        void SetInventoryDominatorOwner();
        void ImproveStandardEquipment();
        std::uint8_t RelationToNonRanger(aShip::TShip* Ship) override;
        std::uint8_t RelationToRanger(void* Ranger) override;
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) override;
        void ReactToAttack(aShip::TShip* Attacker) override;
        std::uint8_t virtual_TShip_RecomputeFearState() override;
        std::uint8_t virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) override;
        std::uint8_t virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) override;
        std::uint8_t AcceptsAppealFrom(aShip::TShip* Ship) override;
        void AssignWeaponTargetsInStar() override;
        // Marks this series as aware of the player's camouflage and reports a matching active disguise.
        void DetectAttackingPlayer(aShip::TShip* Attacker);
        // Can mark the player's camouflage as detected by this Dominator series. Returns false for non-player ships.
        std::uint8_t IsPlayerCamouflageEffective(aShip::TShip* Ship);
        void SelectEnemyShipInStar() override;
        void EngageEnemyShip() override;
        void UpdateAfterburnerState() override;
        void ProcessCombatDialogue() override;
        void ReactToExtortionDemand(void* Ranger) override;
        std::uint8_t virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) override;
        std::uint8_t virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) override;
        std::uint8_t BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) override;
        std::uint8_t virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) override;
        std::uint8_t virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        std::uint8_t virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        void RefreshCombatSkills();
        // Bosses have a minimum calculated speed of 350.
        std::int32_t CalculateSpeed() override;
        std::uint8_t HasNearbyBertorAura();
        void virtual_TShip_RefreshCurrentStanding() override;
        // Script.ShipSubType.
        aGalaxyStruct::TKlingType KlingType;
        aGalaxyStruct::TDominatorSeries DominatorSeries;
        std::uint8_t cpp_padding[2];
        // Zero means inactive.
        std::int32_t ActiveProgramAppliedTurn;
        std::uint8_t ActiveProgramId;
        std::uint8_t AuraEffectShownThisTurn;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aKling
