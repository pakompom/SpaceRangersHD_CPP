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

    // VMT confirms size. SaveToBuffer/LoadFromBuffer establish the
    // stored field widths; orbit doubles are serialized through Single precision.
    // The unaccessed +1C/+3C gaps align the following Double fields.
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPlanet : EC_Struct::TObjectEx {
        PAS_CLASS_META(TPlanet, EC_Struct::TObjectEx, "TPlanet", 360)
        void p_destroy() override;
        // Fourth argument: 0 selects the Solar System, 1..3 limit inhabited planets, 10/11 select special systems. Caller inserts Self into the star's planet list.
        void InitGenerated(aGalaxy::TStar* Star, std::int32_t TotalPlanetCount, std::int32_t InhabitedCountOrSpecialMode);
        // Only sets CurrentStar, OwnerId=5 and all invention levels to 8; used by the separate Dominator spawn planet.
        void InitDominatorSpawnProxy(aGalaxy::TStar* Star);
        // Creates graphics, surface terrain, loot and initial market/research state; caller owns planet registration.
        void InitGeneratedUninhabited(aGalaxy::TStar* Star);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block);
        // Loads editable text fields, updates existing items/ships and processes creation requests. The first matching item name ends the search even when its type is disallowed or creation returns nil.
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        // Forwards Galaxy from TStar.ResolveLoadedReferences to shop, garrison and surface item resolvers; removes incompatible main-pirate-planet modules from saves older than 106.
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        // Daily strength, control and delay gates; returns nil when no ship is spawned.
        void* TrySpawnDominator();
        void NextDay();
        // Uses Self's random state for galaxy-wide attacks; disabled by pirate ending 3.
        void TryDispatchPirateAttacks();
        // Targets a pirate base in a Coalition system; disabled by pirate endings 3 and 5.
        void TrySpawnPirateBaseRaid();
        // Always initializes LastFilmPosition; optionally creates film entries for the planet and satellites.
        void InitializeFilmState(std::int32_t StepIndex, std::uint8_t RecordFilm);
        void AdvanceOrbitStep(std::int32_t StepIndex, std::uint8_t RecordFilm);
        EC_Struct::TPointF PredictPosition(std::int32_t StepsAhead);
        // Queues planet dialogue to the UI thread and waits for its event; requires normal-space player state.
        std::uint8_t RequestDialog();
        void UpdateOwnerFlags();
        void UpdateMarketState();
        void TriggerGovernmentRevolution();
        // May trigger a revolution, goods scarcity or surplus and publish planet news; honors NoRandomEvents.
        void TryTriggerEconomicEvent();
        // Native no-op. TStar.NextDay calls this after detecting a collision; the caller handles impact effects, debris and asteroid respawn.
        static void HandleAsteroidImpact(void* Asteroid);
        // Clears Choices, appends matching nonempty planet-binding titles and stores the owning TScript as each entry's data.
        void CollectScriptDialogChoices(EC_Str::TStringsEC* Choices);
        // Returns -1 when unavailable; otherwise combines a surface family in bits 24..31 with eligible animation bits.
        std::int32_t GetSurfaceAnimationMask();
        // Seed-based StyleFace selection, excluding the player's portrait.
        pas::WideString GetGovernmentPortraitGraph();
        // Main Pirate Planet returns its name without the localized planet prefix.
        pas::WideString GetFullName(pas::WideString Separator);
        EC_Struct::TPointF GetPosition();
        // ForMap suppresses the artifact treasure hint and can append a Pirate Clan warning.
        pas::WideString GetInfoText(std::uint8_t ForMap);
        pas::WideString GetGovernmentName();
        // Localized DisplayName for RaceToOwner(RaceId).
        pas::WideString GetNativeRaceName();
        // Faction/series/internal owner identifier used for resource selection.
        pas::WideString GetFactionResourceName();
        // Maps Radius 60..100 to population 100000..1000000 with clamping and rounding.
        std::int32_t CalculateBasePopulation();
        // For OwnerId=6 counts all uninhabited planets; otherwise counts non-uninhabited planets with the same RaceId. Includes Self.
        std::int32_t CountPlanetsOfSameRace();
        // First adjacent invisible constellation, excluding ID 20; borrowed result or nil.
        aGalaxy::TConstellation* FindUnchartedNeighborConstellation();
        // Searches stars in CurrentStar's distance order, then each star's planet list; no planet-distance tie break.
        TPlanet* FindNearestPlanetByOwnerMask(aGalaxyStruct::TOwnerMask OwnerMask);
        // Sorts by SurfaceTileIndex and moves overlapping markers to free cells of the 14-by-7 display grid.
        void NormalizeSurfaceLootEntries();
        std::int32_t GetTotalSurfaceTileCount();
        // Returns zero unless OwnerId=6.
        std::int32_t GetUnexploredSurfaceTileCount();
        // Takes item ownership, allocates a 12-byte entry and always returns true on completion. Requires positive surface area; resets exploration if fully explored.
        std::uint8_t AddSurfaceLootEntry(aItem::TItem* Item);
        // Requires OwnerId=6, at least 720 days without a player visit and no deployed player probe here. Clears unavailable loot flags and resets exploration if any flag changed.
        std::uint8_t TryResetSurfaceLootAfterLongAbsence();
        // Increments the current track, clears its progress and selects the next track after every increment.
        void BoostInventionLevels(std::int32_t Count);
        // Chooses among tracks permitted by ResearchLevelPercent and main technology slot 7; raises if no choice is found.
        void SelectCurrentInvention();
        // Uses the difficulty multiplier; completion requires progress strictly above 100. Levels cap at 8 and excess progress is discarded.
        void AdvanceInventionProgress();
        // Radius factor times economy and race multipliers; excludes the difficulty multiplier.
        float CalculateInventionProgressRate();
        void* BuyRanger(std::int32_t MoneyPercent);
        // Kind 0 randomizes the subtype, 3 selects transport, 4 liner, and all others diplomat. Result is owned by CurrentStar.Ships.
        void* SpawnTransport(std::uint8_t Kind, std::int32_t MoneyPercent);
        void* BuyPirate(std::int32_t MoneyPercent);
        // Creates an unowned Tranclucator docked here and inserts it into CurrentStar.Ships.
        void* SpawnTranclucator(std::uint8_t BasicEquipment);
        // Creates a TPirate for Pirate Clan ownership, otherwise a TWarrior.
        void* BuyWarrior(std::int32_t MoneyPercent);
        // Generates a warrior with WarriorType=1 and scales its budget by Coalition control.
        void* BuyFlagship(std::int32_t MoneyPercent);
        // Excludes boss type; suppresses Bertors when the constellation already has one of this series or Self is the spawn proxy.
        void* SpawnWeightedDominatorShip();
        void* SpawnDominatorShip(aGalaxyStruct::TKlingType Kind);
        // Uses group owner/type/equipment constraints; returns nil for a boss request. Temporarily changes planet ownership, star series and global technology.
        void* GenerateShipForScriptGroup(void* Group);
        std::int32_t RelationToRanger(std::int32_t RangerIndex);
        void SetRelationLevelToRanger(void* Ranger, aGalaxyStruct::TRelationLevel Level);
        void ChangeRelationToRanger(void* Ranger, std::int32_t Amount);
        std::uint8_t RelationToShip(void* Ship);
        aGalaxyStruct::TRelationLevel GetRelationLevelToShip(void* Ship);
        pas::WideString GetRelationLevelTextToShip(void* Ship);
        pas::WideString GetCivilInfoText();
        // Tests normal-space ships in CurrentStar for RelationToShip < 10.
        std::uint8_t HasHostileShipsInSystem();
        // Returns a zero-based module index or -1; advances planet RNG.
        std::int32_t SelectEquipmentOfferSpecialMicroModule(aItem::TEquipment* Item);
        // Returns a zero-based module index or -1; advances planet RNG.
        std::int32_t SelectHullOfferSpecialMicroModule(aItem::THull* Hull);
        // Returns a zero-based module index or -1; advances planet RNG.
        std::int32_t SelectWeaponOfferSpecialMicroModule(aItem::TWeapon* Weapon);
        // Weekly replacement/generation gate; disabled by sumDisabled and sumGoodsOnly.
        void RefreshEquipmentShopInventory();
        aItem::THull* GenerateHullOffer(void* Ship);
        // New item or nil; does not add it to EquipmentShop.
        aItem::TWeapon* GenerateWeaponOffer(void* Ship);
        aItem::TEquipment* GenerateEquipmentOffer(void* Ship, std::uint8_t ItemType);
        // Returns a new owning list of generated equipment, using the race quota table. Caller forwards ForceGeneratedOffers in CL; this routine saves but never reads it.
        aMyFunction::TObjectList* BuildEquipmentOfferBatch(void* Ship, std::uint8_t UnusedForceGeneratedOffers);
        // Population, economy and deterministic turn jitter adjust race quotas; clamps to 10..20.
        std::int32_t CalculateEquipmentShopTargetCount();
        // Bucket 50 includes all weapon types 50..68; other buckets require an exact type.
        std::int32_t CountEquipmentShopItemsInBucket(std::uint8_t ItemType);
        // Frees at most one other stock item of the same type and level, protecting named script items. Does not insert Item.
        std::uint8_t RemoveSimilarEquipmentShopItem(aItem::TEquipment* Item);
        void ForceGoodsScarcity(std::uint8_t StartEvent, aGalaxyStruct::TItemTypeMask GoodsMask);
        void ForceGoodsSurplus(std::uint8_t StartEvent, aGalaxyStruct::TItemTypeMask GoodsMask);
        // Counts local imprisoned rangers/pirates with a positive remaining prison term and no incompatible script state.
        std::int32_t CountBailablePrisoners();
        pas::WideString GetGovernmentBackgroundGraph();
        pas::WideString BuildNonCivilTreasureHintText();
        pas::WideString BuildGovernmentGreeting();
        std::uint32_t Id;
        std::uint32_t GenerationSeed;
        std::uint32_t RandomState;
        // Generation-only index into the 16-byte planet sprite template table.
        std::int32_t SpriteTemplateIndex;
        pas::WideString Name;
        aGalaxy::TStar* CurrentStar;
        std::uint8_t cpp_padding[4];
        // Angle at +20, radius at +28; not a Cartesian position.
        aMyFunction::TPolarPoint Orbit;
        // Binary-save passthrough. No gameplay use identified in this build; original meaning unknown.
        std::int32_t ReservedSaveValue;
        // Cleared by Create; no subsequent use identified in this build.
        std::uint32_t Reserved34;
        // Script.PlanetSize.
        std::int32_t Radius;
        std::uint8_t cpp_padding_2[4];
        // Degrees per unscaled movement step.
        double OrbitalVelocity;
        // Slot 7 is the main technology level used to gate the other tracks.
        pas::Array<std::uint8_t, 0, 19> InventionLevels;
        std::uint8_t CurrentInvention;
        std::uint8_t cpp_padding_3[3];
        float CurrentInventionPoints;
        // Selection ceiling, compared with invention level * 12.5.
        std::uint8_t ResearchLevelPercent;
        // Increment used when no eligible invention remains.
        std::uint8_t ResearchLevelStep;
        std::uint8_t cpp_padding_4[2];
        std::int32_t Population;
        aGalaxyStruct::TPlanetEconomy Economy;
        std::uint8_t cpp_padding_5[3];
        // Population-funded treasury used by ship generation and refitting.
        std::int32_t Money;
        std::uint8_t OwnerId;
        // Cached OwnerId membership in the five Coalition races.
        std::uint8_t IsCoalitionOwned;
        std::uint8_t RaceId;
        aGalaxyStruct::TPlanetGovernment Government;
        pas::Array<aGalaxyStruct::TGoodsTradePriceEntry, 0, 7> Goods;
        pas::Array<std::uint8_t, 0, 7> GoodsScarcityTicks;
        pas::Array<std::uint8_t, 0, 7> GoodsSurplusTicks;
        // // -1 when no text quest is assigned.
        std::int32_t TextQuestId;
        // Integer scores stored in pointer slots, indexed by Galaxy.Rangers.
        pas::List* RangerRelations;
        // Owned TItem stock.
        aMyFunction::TObjectList* EquipmentShop;
        // Garrison roster; Destroy clears it before freeing the list to avoid freeing ships owned elsewhere.
        aMyFunction::TObjectList* Warriors;
        std::int32_t HomeRangerCount;
        // TTransport instances based here, including liners and diplomats.
        std::int32_t HomeTransportCount;
        std::int32_t WaterTiles;
        std::int32_t WaterExplored;
        std::int32_t LandTiles;
        std::int32_t LandExplored;
        std::int32_t HillTiles;
        std::int32_t HillExplored;
        // Available probe trajectories on the exploration screen; serialized as OrbitCnt.
        std::uint8_t ProbeOrbitCount;
        // First player landing sets this; counts for EXPLORER only if OwnerId=6 then. Saved since version 99.
        std::uint8_t HasPlayerLanded;
        std::uint8_t cpp_padding_6[2];
        // Owns allocated PPlanetSurfaceLootEntry records and their items; may be nil.
        pas::List* SurfaceLootEntries;
        // Generated from the sprite template; saved as a Word.
        std::int32_t GraphicRadius;
        // Retained reference, released by Destroy.
        SE_Planet::TPlanetSE* Graphic;
        pas::WideString GraphName;
        // Owned TSputnik entries.
        aMyFunction::TObjectList* Satellites;
        WindowsSdk::TPoint LastFilmPosition;
        // Borrowed from PrimaryFilm.
        aEFilm::TEFilmObj* FilmObject;
        std::uint8_t NoLanding;
        // TShopUpdateMode value (Script.NoShopUpdate); LoadFromBuffer temporarily stores the packed flag byte here before masking to bits 0..1.
        std::uint8_t ShopUpdateMode;
        // Script.PlanetExtraFlags bit 0.
        std::uint8_t NoAutomaticShipSpawning;
        // Script.PlanetExtraFlags bit 1.
        std::uint8_t NoRandomEvents;
        // Identifies the clan home planet independently of OwnerId.
        std::uint8_t IsMainPiratePlanet;
        std::uint8_t cpp_padding_7[3];
        // Overrides the star faction in GetFactionResourceName.
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
        // Display grid column, 0..13.
        std::uint8_t GridX;
        // Display grid row, 0..6.
        std::uint8_t GridY;
        TPlanetTerrainKind TerrainKind;
        // Suppresses display/collection even after its terrain is explored (,); not a discovered flag.
        std::uint8_t Unavailable;
        // One-based ordinal within this terrain's exploration tiles.
        std::int32_t SurfaceTileIndex;
        // Owned until transferred to the player.
        aItem::TItem* Item;
    };
    #pragma pack(pop)

    using PPlanetSurfaceLootEntry = TPlanetSurfaceLootEntry*;

    // VMT; the satellite wrapper owns a retained space-object reference.
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
        // Borrowed from PrimaryFilm.
        aEFilm::TEFilmObj* FilmObject;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TDominatorSpawnWeightRow = pas::Array<std::int32_t, 0, 7>;

    using TDominatorSpawnWeightTable = pas::Array<TDominatorSpawnWeightRow, 1, 5>;

} // namespace aPlanet
