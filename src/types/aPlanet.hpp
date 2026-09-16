#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aMyFunction.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

namespace SE_Planet {
    struct TPlanetSE;

} // namespace SE_Planet

namespace SE_Sputnik {
    struct TSputnikSE;

} // namespace SE_Sputnik

namespace aEFilm {
    struct TEFilmObj;

} // namespace aEFilm

namespace aGalaxy {
    struct TConstellation;

    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aItem {
    struct TEquipment;

    struct THull;

    struct TItem;

    struct TWeapon;

} // namespace aItem

namespace aPlanet {
    struct TPlanetSurfaceLootEntry;

    struct TSputnik;

    struct TPlanet;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPlanet : EC_Struct::TObjectEx {
        PAS_CLASS_META(TPlanet, EC_Struct::TObjectEx, "TPlanet", 360)
        void p_destroy() override;
        void InitGenerated(aGalaxy::TStar* Star, std::int32_t TotalPlanetCount, std::int32_t InhabitedCountOrSpecialMode);
        void InitDominatorSpawnProxy(aGalaxy::TStar* Star);
        void InitGeneratedUninhabited(aGalaxy::TStar* Star);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        void* TrySpawnDominator();
        void NextDay();
        void TryDispatchPirateAttacks();
        void TrySpawnPirateBaseRaid();
        void InitializeFilmState(std::int32_t StepIndex, std::uint8_t RecordFilm);
        void AdvanceOrbitStep(std::int32_t StepIndex, std::uint8_t RecordFilm);
        EC_Struct::TPointF PredictPosition(std::int32_t StepsAhead);
        std::uint8_t RequestDialog();
        void UpdateOwnerFlags();
        void UpdateMarketState();
        void TriggerGovernmentRevolution();
        void TryTriggerEconomicEvent();
        static void HandleAsteroidImpact(void* Asteroid);
        void CollectScriptDialogChoices(EC_Str::TStringsEC* Choices);
        std::int32_t GetSurfaceAnimationMask();
        pas::WideString GetGovernmentPortraitGraph();
        pas::WideString GetFullName(pas::WideString Separator);
        EC_Struct::TPointF GetPosition();
        pas::WideString GetInfoText(std::uint8_t ForMap);
        pas::WideString GetGovernmentName();
        pas::WideString GetNativeRaceName();
        pas::WideString GetFactionResourceName();
        std::int32_t CalculateBasePopulation();
        std::int32_t CountPlanetsOfSameRace();
        aGalaxy::TConstellation* FindUnchartedNeighborConstellation();
        TPlanet* FindNearestPlanetByOwnerMask(aGalaxyStruct::TOwnerMask OwnerMask);
        void NormalizeSurfaceLootEntries();
        std::int32_t GetTotalSurfaceTileCount();
        std::int32_t GetUnexploredSurfaceTileCount();
        std::uint8_t AddSurfaceLootEntry(aItem::TItem* Item);
        std::uint8_t TryResetSurfaceLootAfterLongAbsence();
        void BoostInventionLevels(std::int32_t Count);
        void SelectCurrentInvention();
        void AdvanceInventionProgress();
        float CalculateInventionProgressRate();
        void* BuyRanger(std::int32_t MoneyPercent);
        void* SpawnTransport(std::uint8_t Kind, std::int32_t MoneyPercent);
        void* BuyPirate(std::int32_t MoneyPercent);
        void* SpawnTranclucator(std::uint8_t BasicEquipment);
        void* BuyWarrior(std::int32_t MoneyPercent);
        void* BuyFlagship(std::int32_t MoneyPercent);
        void* SpawnWeightedDominatorShip();
        void* SpawnDominatorShip(aGalaxyStruct::TKlingType Kind);
        void* GenerateShipForScriptGroup(void* Group);
        std::int32_t RelationToRanger(std::int32_t RangerIndex);
        void SetRelationLevelToRanger(void* Ranger, aGalaxyStruct::TRelationLevel Level);
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount);
        std::uint8_t RelationToShip(void* Ship);
        aGalaxyStruct::TRelationLevel GetRelationLevelToShip(void* Ship);
        pas::WideString GetRelationLevelTextToShip(void* Ship);
        pas::WideString GetCivilInfoText();
        std::uint8_t HasHostileShipsInSystem();
        std::int32_t SelectEquipmentOfferSpecialMicroModule(aItem::TEquipment* Item);
        std::int32_t SelectHullOfferSpecialMicroModule(aItem::THull* Hull);
        std::int32_t SelectWeaponOfferSpecialMicroModule(aItem::TWeapon* Weapon);
        void RefreshEquipmentShopInventory();
        aItem::THull* GenerateHullOffer(void* Ship);
        aItem::TWeapon* GenerateWeaponOffer(void* Ship);
        aItem::TEquipment* GenerateEquipmentOffer(void* Ship, std::uint8_t ItemType);
        aMyFunction::TObjectList* BuildEquipmentOfferBatch(void* Ship, std::uint8_t UnusedForceGeneratedOffers);
        std::int32_t CalculateEquipmentShopTargetCount();
        std::int32_t CountEquipmentShopItemsInBucket(std::uint8_t ItemType);
        std::uint8_t RemoveSimilarEquipmentShopItem(aItem::TEquipment* Item);
        void ForceGoodsScarcity(std::uint8_t StartEvent, aGalaxyStruct::TItemTypeMask GoodsMask);
        void ForceGoodsSurplus(std::uint8_t StartEvent, aGalaxyStruct::TItemTypeMask GoodsMask);
        std::int32_t CountBailablePrisoners();
        pas::WideString GetGovernmentBackgroundGraph();
        pas::WideString BuildNonCivilTreasureHintText();
        pas::WideString BuildGovernmentGreeting();
        std::uint32_t Id;
        std::uint32_t GenerationSeed;
        std::uint32_t RandomState;
        std::int32_t SpriteTemplateIndex;
        pas::WideString Name;
        aGalaxy::TStar* CurrentStar;
        std::uint8_t cpp_padding[4];
        aMyFunction::TPolarPoint Orbit;
        std::int32_t ReservedSaveValue;
        std::uint32_t Reserved34;
        std::int32_t Radius;
        std::uint8_t cpp_padding_2[4];
        double OrbitalVelocity;
        pas::Array<std::uint8_t, 0, 19> InventionLevels;
        std::uint8_t CurrentInvention;
        std::uint8_t cpp_padding_3[3];
        float CurrentInventionPoints;
        std::uint8_t ResearchLevelPercent;
        std::uint8_t ResearchLevelStep;
        std::uint8_t cpp_padding_4[2];
        std::int32_t Population;
        aGalaxyStruct::TPlanetEconomy Economy;
        std::uint8_t cpp_padding_5[3];
        std::int32_t Money;
        std::uint8_t OwnerId;
        std::uint8_t IsCoalitionOwned;
        std::uint8_t RaceId;
        aGalaxyStruct::TPlanetGovernment Government;
        pas::Array<aGalaxyStruct::TGoodsTradePriceEntry, 0, 7> Goods;
        pas::Array<std::uint8_t, 0, 7> GoodsScarcityTicks;
        pas::Array<std::uint8_t, 0, 7> GoodsSurplusTicks;
        std::int32_t TextQuestId;
        pas::List* RangerRelations;
        aMyFunction::TObjectList* EquipmentShop;
        aMyFunction::TObjectList* Warriors;
        std::int32_t HomeRangerCount;
        std::int32_t HomeTransportCount;
        std::int32_t WaterTiles;
        std::int32_t WaterExplored;
        std::int32_t LandTiles;
        std::int32_t LandExplored;
        std::int32_t HillTiles;
        std::int32_t HillExplored;
        std::uint8_t ProbeOrbitCount;
        std::uint8_t HasPlayerLanded;
        std::uint8_t cpp_padding_6[2];
        pas::List* SurfaceLootEntries;
        std::int32_t GraphicRadius;
        SE_Planet::TPlanetSE* Graphic;
        pas::WideString GraphName;
        aMyFunction::TObjectList* Satellites;
        WindowsSdk::TPoint LastFilmPosition;
        aEFilm::TEFilmObj* FilmObject;
        std::uint8_t NoLanding;
        std::uint8_t ShopUpdateMode;
        std::uint8_t NoAutomaticShipSpawning;
        std::uint8_t NoRandomEvents;
        std::uint8_t IsMainPiratePlanet;
        std::uint8_t cpp_padding_7[3];
        pas::WideString CustomFaction;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TPlanetTerrainKind : std::uint8_t {
        ptWater = 0,
        ptLand = 1,
        ptHill = 2,
    };

    #pragma pack(push, 1)
    struct TPlanetSurfaceLootEntry {
        std::uint8_t GridX;
        std::uint8_t GridY;
        TPlanetTerrainKind TerrainKind;
        std::uint8_t Unavailable;
        std::int32_t SurfaceTileIndex;
        aItem::TItem* Item;
    };
    #pragma pack(pop)

    using PPlanetSurfaceLootEntry = TPlanetSurfaceLootEntry*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSputnik : EC_Struct::TObjectEx {
        PAS_CLASS_META(TSputnik, EC_Struct::TObjectEx, "TSputnik", 16)
        void p_destroy() override;
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        std::uint32_t Id;
        SE_Sputnik::TSputnikSE* Graphic;
        aEFilm::TEFilmObj* FilmObject;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TDominatorSpawnWeightRow = pas::Array<std::int32_t, 0, 7>;

    using TDominatorSpawnWeightTable = pas::Array<TDominatorSpawnWeightRow, 1, 5>;

} // namespace aPlanet
