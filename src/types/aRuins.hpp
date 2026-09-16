#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aGalaxy {
    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aItem {
    struct TEquipment;

    struct THull;

    struct TItem;

    struct TSatellite;

    struct TWeapon;

} // namespace aItem

namespace aMyFunction {
    struct TObjectList;

} // namespace aMyFunction

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aRuins {
    struct TStationHullGeneration;

    struct TStationLevelRange;

    struct TStationWeaponGeneration;

    struct TRuins;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRuins : aShip::TShip {
        PAS_CLASS_META(TRuins, aShip::TShip, "TRuins", 1384)
        void p_destroy() override;
        void Init(aGalaxyStruct::TStationType StationType, aGalaxy::TStar* Star, pas::WideString TypeNameOverride);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void NextDay() override;
        void NextDayLogic() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        pas::WideString GetColoredFullName(const pas::WideString& ColorTag);
        std::uint8_t GetGreetingShipCategory() override;
        aGalaxyStruct::TRangerCareer GetDominantCareer() override;
        aGalaxy::TStar* GetHomeStar() override;
        std::uint8_t GetStrengthScaledPirateStatus() override;
        std::int32_t GetDesiredCargoFreeSpace() override;
        void RegenerateSatelliteOffer();
        void RefuelAtLocation() override;
        void RepairBrokenEquipmentAtLocation() override;
        void ReloadWeapons();
        void RefreshShopInventory();
        std::int32_t CalculateEquipmentShopTargetCount();
        std::int32_t CountEquipmentShopItems(std::uint8_t ItemType);
        aItem::TItem* FindMostExpensiveShopItem(std::int32_t MinCost, std::int32_t MaxCost);
        std::uint8_t RemoveSimilarShopItem(aItem::TEquipment* Item);
        std::int32_t SelectEquipmentOfferSpecialMicroModule(aItem::TEquipment* Item, aPlanet::TPlanet* Planet);
        std::int32_t SelectHullOfferSpecialMicroModule(aItem::THull* Hull, aPlanet::TPlanet* Planet);
        std::int32_t SelectWeaponOfferSpecialMicroModule(aItem::TWeapon* Weapon, aPlanet::TPlanet* Planet);
        void BuildReachablePlanetQueue() override;
        std::uint8_t CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        void SelectEnemyShipInStar() override;
        void EngageEnemyShip() override;
        void AssignWeaponTargetsInStar() override;
        std::uint8_t TryStartAbductionCycle();
        void TryAbductDepartingShip(aShip::TShip* Ship);
        void ReportAbductionOutcome();
        float EvaluateLocalForceBalance(EC_Struct::TPointF Point);
        float EvaluateRelocationPosition(EC_Struct::TPointF Point);
        std::uint8_t TryRepositionInStar();
        std::uint8_t TryRelocateToPirateStar();
        std::uint8_t AcceptPickupItem(aItem::TItem* Item) override;
        void ApplyInventoryMicroModulesToShopItems();
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
        void ForceGoodsForSale(aGalaxyStruct::TItemTypeMask GoodsMask);
        void GenerateCombatSkills();
        void RandomizePosition();
        EC_Struct::TPointF SelectTeleportArrivalPoint(aGalaxy::TStar* Star);
        float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) override;
        float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) override;
        float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) override;
        void RefreshCurrentStanding() override;
        aItem::THull* GeneratePlanetHullOffer(pas::Object* Ship, aPlanet::TPlanet* Planet);
        aItem::THull* GenerateHullOffer(pas::Object* Ship, aPlanet::TPlanet* Planet);
        aItem::TWeapon* GenerateWeaponOffer(pas::Object* Ship, aPlanet::TPlanet* Planet);
        aItem::TEquipment* GenerateEquipmentOffer(pas::Object* Ship, aPlanet::TPlanet* Planet, std::uint8_t ItemType);
        aMyFunction::TObjectList* GenerateEquipmentOfferBatch(aShip::TShip* Ship, std::uint8_t UnusedForceGeneratedOffers);
        void UpdateGoodsMarketState();
        std::int32_t CalculateRepairCost(aShip::TShip* Ship, std::int32_t& EquipmentCost);
        std::int32_t GetRepairCost(aShip::TShip* Ship);
        void RepairShipEquipment(aShip::TShip* Ship);
        std::int32_t GetNodeSaleBatchSize();
        TRuins* FindPirateBaseWithNodes();
        std::int32_t SelectServiceMicroModule(std::int32_t Kind, std::int32_t Index, std::uint8_t InvertRarity);
        std::uint8_t CanDock(aShip::TShip* Ship) override;
        std::uint8_t CheckDockingPermission(aShip::TShip* Ship, pas::WideString& Response) override;
        aMyFunction::TObjectList* EquipmentShop;
        pas::Array<aGalaxyStruct::TGoodsTradePriceEntry, 0, 7> ShopGoods;
        std::int32_t RelocationAge;
        aGalaxy::TStar* FlyToStar;
        std::int32_t FlyDate;
        aItem::TSatellite* SatelliteOffer;
        std::uint8_t SpecialServiceActive;
        std::uint8_t ModernizationSponsor;
        std::uint8_t NoLanding;
        aGalaxyStruct::TShopUpdateMode ShopUpdateMode;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TStationHullGeneration {
        std::int32_t MinSize;
        std::int32_t MaxSize;
        std::int32_t TechSizeBonus;
        std::int32_t MinLevel;
        std::int32_t MaxLevel;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TStationLevelRange {
        std::int32_t Minimum;
        std::int32_t Maximum;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TStationWeaponGeneration {
        std::int32_t BasicLevel;
        std::int32_t IntermediateLevel;
        std::int32_t AdvancedLevel;
        std::int32_t MinimumRange;
    };
    #pragma pack(pop)

    using TStationHullTypes = pas::Set<0, 15>;

} // namespace aRuins
