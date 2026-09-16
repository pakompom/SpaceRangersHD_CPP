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
        // Also registers the station in Star. Type 13 inherits ranger-center defaults.
        void Init(aGalaxyStruct::TStationType StationType, aGalaxy::TStar* Star, pas::WideString TypeNameOverride);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        // Requires a fresh instance. FlyToStar temporarily contains a saved star ID.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        // Includes the player's storage at this station and temporarily displayed shop stock.
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        // Applies edits to existing state; can append new shop and player-storage items.
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void NextDay() override;
        void NextDayLogic() override;
        pas::WideString GetName() override;
        pas::WideString GetFullName(const pas::WideString& Separator) override;
        // Uses the station type's alternate localized caption and colors the individual name.
        pas::WideString GetColoredFullName(const pas::WideString& ColorTag);
        std::uint8_t GetGreetingShipCategory() override;
        // Always rcTrader.
        aGalaxyStruct::TRangerCareer GetDominantCareer() override;
        aGalaxy::TStar* GetHomeStar() override;
        std::uint8_t GetStrengthScaledPirateStatus() override;
        // Always zero.
        std::int32_t GetDesiredCargoFreeSpace() override;
        // Invalidates TargetPlanet on the existing offer.
        void RegenerateSatelliteOffer();
        void RefuelAtLocation() override;
        // Sponsored stations restore equipment condition without a repair transaction.
        void RepairBrokenEquipmentAtLocation() override;
        // Sponsored stations gain only one round per weapon; other stations refill to capacity.
        void ReloadWeapons();
        // Disabled in modes 1 and 3; otherwise follows the station's weekly schedule after initial setup.
        void RefreshShopInventory();
        // Returns 10..18; advances the station RNG state.
        std::int32_t CalculateEquipmentShopTargetCount();
        // Bucket 50 includes all weapon item types 50..68.
        std::int32_t CountEquipmentShopItems(std::uint8_t ItemType);
        // Excludes hulls; inclusive cost bounds. Borrowed result, nil when absent.
        aItem::TItem* FindMostExpensiveShopItem(std::int32_t MinCost, std::int32_t MaxCost);
        // Frees at most one existing offer matching type and level; never inserts Item. May also compare size when below the target shop count.
        std::uint8_t RemoveSimilarShopItem(aItem::TEquipment* Item);
        // Returns a zero-based module index or -1; advances the station RNG.
        std::int32_t SelectEquipmentOfferSpecialMicroModule(aItem::TEquipment* Item, aPlanet::TPlanet* Planet);
        // Returns a zero-based module index or -1; advances the station RNG.
        std::int32_t SelectHullOfferSpecialMicroModule(aItem::THull* Hull, aPlanet::TPlanet* Planet);
        // Returns a zero-based module index or -1; advances the station RNG.
        std::int32_t SelectWeaponOfferSpecialMicroModule(aItem::TWeapon* Weapon, aPlanet::TPlanet* Planet);
        // Empty.
        void BuildReachablePlanetQueue() override;
        // Always false.
        std::uint8_t CanQueueReachablePlanet(aPlanet::TPlanet* Planet) override;
        // Clears EnemyShip.
        void SelectEnemyShipInStar() override;
        // Empty.
        void EngageEnemyShip() override;
        // Targets hostile ships, incoming missiles and nearby asteroids.
        void AssignWeaponTargetsInStar() override;
        std::uint8_t TryStartAbductionCycle();
        // May redirect a departing ship to this station during its abduction cycle.
        void TryAbductDepartingShip(aShip::TShip* Ship);
        // Posts the native Pirate Clan success/failure message after the station reappears.
        void ReportAbductionOutcome();
        float EvaluateLocalForceBalance(EC_Struct::TPointF Point);
        float EvaluateRelocationPosition(EC_Struct::TPointF Point);
        // Successful repositioning initiates a teleport.
        std::uint8_t TryRepositionInStar();
        // Travel starts next turn; transfers the star's Dominion ownership reference.
        std::uint8_t TryRelocateToPirateStar();
        std::uint8_t AcceptPickupItem(aItem::TItem* Item) override;
        // Consumes compatible modules from Inventory, skipping index 0; modifies EquipmentShop items.
        void ApplyInventoryMicroModulesToShopItems();
        std::uint8_t RelationToNonRanger(aShip::TShip* Ship) override;
        // Floors the stored relation at 50, except for the player when NoLanding is set.
        std::uint8_t RelationToRanger(void* Ranger) override;
        // Positive changes receive the ranger's Charisma bonus; stored relation is clamped to 0..100.
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount) override;
        void ReactToAttack(aShip::TShip* Attacker) override;
        // Returns false without changing InFear.
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
        // Script.GoodsRuinsForBuy. Restocks selected goods and sets prices near the global minimum.
        void ForceGoodsForSale(aGalaxyStruct::TItemTypeMask GoodsMask);
        // Sets base Accuracy and Maneuverability to 0..5 using galaxy technology and war state.
        void GenerateCombatSkills();
        // Immediately replaces Position with a sampled arrival point in CurrentStar.
        void RandomizePosition();
        // Search is limited to 1001 attempts.
        EC_Struct::TPointF SelectTeleportArrivalPoint(aGalaxy::TStar* Star);
        float AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) override;
        float EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) override;
        float EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) override;
        // Script state can force standing 9 or preserve the current value; type 13 has no default assignment.
        void RefreshCurrentStanding() override;
        // Type-13 fallback through the planet's hull generator; may apply a special module.
        aItem::THull* GeneratePlanetHullOffer(pas::Object* Ship, aPlanet::TPlanet* Planet);
        // Returns a newly allocated offer or nil; Planet supplies local technology.
        aItem::THull* GenerateHullOffer(pas::Object* Ship, aPlanet::TPlanet* Planet);
        // Chooses the weapon type; returns a newly allocated offer or nil.
        aItem::TWeapon* GenerateWeaponOffer(pas::Object* Ship, aPlanet::TPlanet* Planet);
        // Accepts types 42..68; weapon types all select the weapon generator. Does not insert the result into EquipmentShop.
        aItem::TEquipment* GenerateEquipmentOffer(pas::Object* Ship, aPlanet::TPlanet* Planet, std::uint8_t ItemType);
        // Returns a new owning list of offers. Ignores the Boolean argument.
        aMyFunction::TObjectList* GenerateEquipmentOfferBatch(aShip::TShip* Ship, std::uint8_t UnusedForceGeneratedOffers);
        // Does nothing in modes 1 and 2.
        void UpdateGoodsMarketState();
        // Returns the total including eligible artefacts; EquipmentCost excludes the separate artefact pass. Military discount uses the player's rank.
        std::int32_t CalculateRepairCost(aShip::TShip* Ship, std::int32_t& EquipmentCost);
        std::int32_t GetRepairCost(aShip::TShip* Ship);
        // Charges the full quote and repairs eligible items only if affordable. Caller refreshes ship statistics.
        void RepairShipEquipment(aShip::TShip* Ship);
        // Returns min(NodeReserve, 250), without a lower clamp.
        std::int32_t GetNodeSaleBatchSize();
        // Searches from CurrentStar for a standard pirate base with positive NodeReserve in a peaceful non-Dominator system. Borrowed result or nil.
        TRuins* FindPirateBaseWithNodes();
        // Zero-based module index. Deterministic chain keyed by station, current turn and player RNG state; Kind 1 selects priorities 31..69, Kind 2 selects 0..20 or 10..30, others 70..100.
        std::int32_t SelectServiceMicroModule(std::int32_t Kind, std::int32_t Index, std::uint8_t InvertRarity);
        std::uint8_t CanDock(aShip::TShip* Ship) override;
        // Clears Response on success; otherwise supplies a refusal message.
        std::uint8_t CheckDockingPermission(aShip::TShip* Ship, pas::WideString& Response) override;
        // Owns equipment offers.
        aMyFunction::TObjectList* EquipmentShop;
        pas::Array<aGalaxyStruct::TGoodsTradePriceEntry, 0, 7> ShopGoods;
        std::int32_t RelocationAge;
        aGalaxy::TStar* FlyToStar;
        std::int32_t FlyDate;
        // Owned until transferred to the buyer.
        aItem::TSatellite* SatelliteOffer;
        std::uint8_t SpecialServiceActive;
        // Script.RuinsAllowModernization.
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
