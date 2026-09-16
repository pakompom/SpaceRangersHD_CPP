#pragma once
#include "runtime_support.hpp"
#include "types/aConst.hpp"
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

namespace aTranclucator {
    struct TTranclucator;

    enum TTranclucatorCollectionKind : std::uint8_t {
        tckOther = 0,
        tckArtefact = 1,
        tckMicroModule = 2,
        tckEquipment = 3,
        tckUseless = 4,
        tckGoods = 5,
        tckCountable = 6,
    };

    enum TTranclucatorStorageKind : std::uint32_t {
        tskPlanet = 1,
        tskStation = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTranclucator : aShip::TShip {
        PAS_CLASS_META(TTranclucator, aShip::TShip, "TTranclucator", 1260)
        void p_destroy() override;
        void ClearCollectionPermissions();
        void SetCollectionPermission(TTranclucatorCollectionKind Kind, std::uint8_t Enabled);
        std::uint8_t GetCollectionPermission(TTranclucatorCollectionKind Kind);
        // Enables planets and disables stations.
        void ResetStoragePermissions();
        void SetStoragePermission(TTranclucatorStorageKind Kind, std::uint8_t Enabled);
        // Unknown kinds return false.
        std::uint8_t GetStoragePermission(TTranclucatorStorageKind Kind);
        void Init(aShip::TShip* AOwnerShip, std::uint8_t Faction, std::uint8_t BasicEquipment);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void NextDay() override;
        // Zero-byte pointer additions retain the native evaluation order of IndexOf.
        void NextDayLogic() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        std::uint8_t GetGreetingShipCategory() override;
        // Always rcWarrior.
        aGalaxyStruct::TRangerCareer GetDominantCareer() override;
        aGalaxy::TStar* GetHomeStar() override;
        std::uint8_t GetStrengthScaledPirateStatus() override;
        std::int32_t GetDesiredCargoFreeSpace() override;
        // Checks FollowOwner, owner presence, shared star and owner hyperspace state; does not require docking.
        std::uint8_t CanFollowOwnerInCurrentStar();
        // Fills installed fuel tanks without charging Money.
        void RefuelAtLocation() override;
        void RepairBrokenEquipmentAtLocation() override;
        // Moves unequipped inventory and artefacts plus all goods; refreshes Self and the destination player's storage bubbles.
        void TransferUnequippedCargo(aShip::TShip* Destination);
        // The +0 index/pointer expressions below preserve native DCC32 argument scheduling.
        void StoreUnequippedCargoAt(pas::Object* Location);
        std::uint8_t UnloadCargoForPlayerOwner();
        std::uint8_t TryLandForStorage();
        std::uint8_t ConvertToStoredArtefact();
        void UpdateFreeFlightOrder();
        void BuildReachablePlanetQueue() override;
        std::uint8_t CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        // Returns whether a move order is active; nearby pickups can be queued even when the result is false.
        std::uint8_t TryCollectPreferredFloatingLoot(std::int32_t MaxTravelDays);
        void EquipEssentialInventory();
        void AssignWeaponTargetsInStar() override;
        void SelectEnemyShipInStar() override;
        void EngageEnemyShip() override;
        std::uint8_t RelationToNonRanger(aShip::TShip* Ship) override;
        std::uint8_t RelationToRanger(void* Ranger) override;
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) override;
        void ReactToAttack(aShip::TShip* Attacker) override;
        std::uint8_t RecomputeFearState() override;
        std::uint8_t AcceptsRansomDemandFrom(aShip::TShip* Ship) override;
        std::uint8_t TrustsAttackRequester(aShip::TShip* Ship) override;
        std::uint8_t EvaluateAllyRelationAndStrength(aShip::TShip* Ship) override;
        void ProcessCombatDialogue() override;
        void ReactToExtortionDemand(void* Ranger) override;
        std::uint8_t BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) override;
        std::uint8_t BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) override;
        std::uint8_t BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) override;
        std::uint8_t BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) override;
        std::uint8_t AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        std::uint8_t BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) override;
        void RefreshCurrentStanding() override;
        float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) override;
        std::int32_t ArtefactSize;
        pas::WideString ArtefactSystemName;
        // Script.Proprietor.
        aShip::TShip* OwnerShip;
        // Script.Docking.
        std::uint8_t FollowOwner;
        std::uint8_t SeekItems;
        std::uint8_t AutoArrange;
        // Script.LandStorage; also applies when docked to a station.
        std::uint8_t StoreOnLanding;
        // Script.SeekPermitNone through SeekPermitNode.
        pas::Array<std::uint8_t, 0, 6> CollectionPermissions;
        // Planet and station storage switches.
        pas::Array<std::uint8_t, 1, 2> StoragePermissions;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aTranclucator
