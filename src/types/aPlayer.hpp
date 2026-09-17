#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aRanger.hpp"

namespace Achievements {
    struct TAchievementStats;

} // namespace Achievements

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
    struct TCountableItem;

    struct TItem;

    struct TSatellite;

} // namespace aItem

namespace aMyFunction {
    struct TObjectList;

} // namespace aMyFunction

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aPlayer {
    struct TStorageEntry;

} // namespace aPlayer

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aPlayer {
    struct TPlanetBattleHistoryEntry;

    struct TEquipmentConfiguration;

    struct TJournalRecord;

    struct TStorageHeaderColumns;

    struct TProbeSummaryColumns;

    struct TStorageItemColumns;

    struct TPlayer;

    #pragma pack(push, 1)
    struct TPlanetBattleHistoryEntry {
        std::int32_t MapId;
        // Native robot statistics; individual meanings unresolved.
        pas::Array<std::int32_t, 0, 5> Statistics;
        std::int32_t ResultCode;
        std::int32_t CompletionMode;
        std::int32_t DateTurn;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TEquipmentConfiguration {
        // IDs, not pointers. Zero denotes an empty slot; the hull is not saved here.
        // Weapon slots 0..4, then up to seven other equipped items.
        pas::Array<std::int32_t, 0, 11> EquipmentIds;
        // Indexed by assigned artifact slot.
        pas::Array<std::int32_t, 0, 31> ArtefactIds;
    };
    #pragma pack(pop)

    using PStorageEntry = TStorageEntry*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPlayer : aRanger::TRanger {
        PAS_CLASS_META(TPlayer, aRanger::TRanger, "TPlayer", 3568)
        void p_destroy() override;
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        // Inherited ranger registration followed by player career/skill defaults; CharacterPreset is unused here.
        void InitializePlayerAtPlanet(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::int32_t CharacterPreset);
        // Twenty-five race/preset loadouts, stored cargo and initial planet relations. Planet is unused.
        void ApplyCharacterPreset(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::int32_t CharacterPreset);
        void virtual_TShip_NextDay() override;
        // Capped at 100000000; zero for nonpositive principal.
        std::int32_t ComputeDepositAccruedValue();
        // Increments every carried transmitter in the global player's artefact list, including unequipped ones.
        static void RechargeTransmitters();
        // Each active Bio artefact may shorten a disease and extend a stimulant by one turn.
        void ApplyBioArtefactHealthEffects();
        // Terron unresolved, late-game offer cadence and no program 4 already carried.
        std::uint8_t MayTakeSubCrack();
        // Difficulty-scaled price.
        static std::int32_t GetSubCrackCost();
        // Rounded pirate career status / 1.3, plus one percentage point.
        std::uint8_t GetPirateServiceDiscount();
        std::int32_t CountProgramRewardStocks();
        // Requires a TKling victim; records its hull capacity even when no reward is due.
        std::uint8_t TryAwardDominatorPrograms(aShip::TShip* Victim);
        // Returns the highest-scoring visible Coalition trade route; retains output arguments on failure and excludes their previous endpoints.
        std::uint8_t FindProfitableTradeRoute(std::uint8_t Nearby, std::uint32_t Seed, pas::Var<aPlanet::TPlanet*> PurchasePlanet, pas::Var<aPlanet::TPlanet*> SalePlanet, std::uint8_t& Good, aGalaxyStruct::TItemTypeMask GoodsMask);
        // Uses Self.Satellites.Count but reads the global player's list.
        std::uint8_t HasDeployedSatellites();
        // Requires Self=GetPlayer(): uses Self for the list count but fetches entries from the global player's deployed satellites.
        std::uint8_t HasSatelliteOnPlanet(aPlanet::TPlanet* Planet);
        static pas::WideString GetStorageColumnHeaderText();
        static pas::WideString GetStorageDividerText();
        pas::WideString BuildDeployedSatelliteSummary(std::int32_t& LineCount);
        // Remaining duration at the current planet using combined operational probe rates; capped at 999 per terrain.
        static std::int32_t GetSatelliteExplorationTurns(aItem::TSatellite* Satellite);
        // Native stub always returns true; Self and Item are unused. Called by treasure-map selection and planet loot reset.
        static std::uint8_t CanAccessSurfaceLootItem(aItem::TItem* Item);
        // Reports inactive deployed satellites in the entered star; records up to three target planets.
        void ReportIdleSatellites(aGalaxy::TStar* Star);
        // Native empty turn-start hook.
        static void BeginStorageTurn();
        // Native always-true permission hook; Good is passed in DL.
        static std::uint8_t CanAccessHoldGoods(std::uint8_t Good);
        // Native always-true permission hook used by storage lookup, counting and slot allocation.
        static std::uint8_t CanAccessStoredItem(aItem::TItem* Item);
        // Location=nil includes all storage locations. Goods and item types 69/75 count by weight; other matching items count individually.
        std::int32_t CountStoredItemUnits(pas::Object* Location, std::uint8_t ItemType);
        // Reassigns later accessible entries with duplicate slot indices.
        void RepairDuplicateStorageSlots(pas::Object* Location);
        // Returns a nonnegative slot local to Location.
        std::int32_t FindNextStorageSlot(pas::Object* Location);
        // Maximum accessible slot index plus one, or zero.
        std::int32_t GetStorageSlotExtent(pas::Object* Location);
        // Returns a zero-based StorageEntries index, or -1.
        std::int32_t FindStorageIndexByLocationAndSlot(pas::Object* Location, std::int32_t Slot);
        // Returns a zero-based StorageEntries index, or -1.
        std::int32_t FindStorageGoodsByLocationAndType(pas::Object* Location, std::uint8_t Good);
        // Returns a zero-based StorageEntries index, or -1.
        std::int32_t FindMergeableStorageItemByLocation(pas::Object* Location, aItem::TCountableItem* Item);
        void ShiftStorageSlotsAtOrAfter(pas::Object* Location, std::int32_t Slot);
        // Shifts later slots down only if Slot is unoccupied.
        void CloseVacantStorageSlot(pas::Object* Location, std::int32_t Slot);
        // With nil, returns whether the entire storage list is empty.
        std::uint8_t HasAccessibleStorageAt(pas::Object* Location);
        // Counts ships following Self in the current system, wrapping at 256.
        std::uint8_t CountPartnersInNormalSpace();
        // Native compares ranger TotalExperience with the global player's PlaceInRating after refreshing rankings.
        static std::uint8_t GetShipRatingComparison(aShip::TShip* Ship);
        // Returns 1..5 for relative Coalition rank, zero for non-normal ships.
        static std::uint8_t GetShipRankComparison(aShip::TShip* Ship);
        // Returns 1..5 for relative pirate rank, zero for non-normal ships.
        static std::uint8_t GetShipPirateRankComparison(aShip::TShip* Ship);
        // Returns 1..5 from strength relative to the global player.
        static std::uint8_t GetShipStrengthComparison(aShip::TShip* Ship);
        pas::WideString BuildTranclucatorStorageSummary(std::int32_t& LineCount);
        // Compares star/location IDs, type, module priority/index, weight and cost; ignores slot indices.
        static std::int32_t CompareStorageEntries(PStorageEntry Left, PStorageEntry Right);
        void SortStorageEntries();
        void RefreshStorageBubbles();
        // Publishes paginated storage summaries, including deployed probes; uses the global player's bubble list.
        void BuildStorageBubbles();
        // Returns a map ID or -1; updates shared map play-count scratch from the player's history.
        std::int32_t SelectPlanetBattleMap();
        // Index is zero-based and unchecked; the selected preset is unchanged.
        void SaveEquipmentConfiguration(std::int32_t Index);
        // Can draw from storage at the current dock. Index is unchecked; the selected preset is unchanged.
        void ApplyEquipmentConfiguration(std::int32_t Index);
        // Does not validate Index.
        std::uint8_t HasEquipmentConfiguration(std::int32_t Index);
        // Includes the carrier's hold and the player's current-location storage.
        std::int32_t GetAvailableNodeCount(aShip::TShip* Carrier);
        // Pirate career thresholds, eminent title and active license.
        std::int32_t GetMaxPiratePartners();
        // As pirate partners, with an additional threshold above career status 50.
        std::int32_t GetMaxDominionShips();
        // Dates the new record with the current turn.
        void AddJournalRecord(pas::WideString Text);
        // Native guard accepts Index=Count, leaving the list accessor to raise.
        void DeleteJournalRecord(std::int32_t Index);
        void ClearJournal();
        pas::WideString ExportJournal();
        // Ascending ID, using pairwise swaps.
        void SortNewsEntries();
        // Removes and finalizes the oldest Count-KeepCount entries; unchecked argument.
        void TrimNewsEntries(std::int32_t KeepCount);
        pas::WideString ExportNews();
        // Copies galaxy news whose IDs are absent locally.
        void MergeGalaxyNews();
        std::int32_t CalculateSpeed() override;
        // Creates a military-base proxy and removes it from the ordinary system ship list.
        void CreateRuinsProxy();
        // Nonpositive Mode uses the installed hull's CapitalShip kind.
        void EnterRuinsMode(std::int32_t Mode);
        // Returns to the saved location or star screen and requests screen closure.
        void CloseRuinsModeScreen();
        // Restores the real docking target and rebuilds its temporary shop stock.
        void ExitRuinsMode();
        void virtual_TShip_RefreshCurrentStanding() override;
        // Honors scripted targeting restrictions, chameleon logic and friendly station standing masks.
        std::uint8_t CanSelectShipTarget(aShip::TShip* Ship);
        // Checks station/Dominator restrictions and invokes the player's scan-permission item scripts.
        static std::uint8_t CanScanShip(aShip::TShip* Ship);
        std::uint8_t InPrison;
        // Player-global Script.NoTalkToShip lock.
        std::uint8_t TalkLocked;
        // Player-global Script.NoScanToShip lock.
        std::uint8_t ScanLocked;
        std::uint8_t cpp_padding[1];
        // Script.ShipStatistic kind 8.
        std::int32_t HyperspaceKillCount;
        // Script.ShipStatistic kind 7.
        std::int32_t BlackHoleKillCount;
        // Indexed by TKlingType. Arcade Keller contributes to the boss slot (zero).
        pas::Array<std::int32_t, 0, 7> DominatorKillsByType;
        pas::List* ScriptShipBindings;
        aShip::TShip* QuestTargetKillShip;
        aShip::TShip* QuestTargetDefendShip;
        aGalaxy::TStar* QuestTargetDefendStar;
        // PStorageEntry elements.
        pas::List* StorageEntries;
        // Total repayment, including interest and penalties.
        std::int32_t DebtAmount;
        std::int32_t DebtDueTurn;
        // Missed repayment deadlines, not elapsed days.
        std::int32_t DebtDefaultCount;
        std::int32_t DepositAmount;
        std::int32_t DepositStartTurn;
        std::int32_t DepositDayCount;
        // Annual percentage points.
        float DepositInterestRate;
        // A purchased five-year policy starts at 1825.
        std::int32_t MedicalPolicyTicks;
        std::int32_t PirateLicenseTicks;
        // Credited license proceeds; decays after expiry.
        std::int32_t PirateLicenseCash;
        // Folded into license proceeds and experience on the next turn.
        std::int32_t PendingPirateLicenseCash;
        // Player selection for Dominion travel.
        aGalaxy::TStar* QueuedTravelTarget;
        // Initialized to 150; last-use turns for investment/service cooldowns. Native reads and write.
        pas::Array<std::int32_t, 0, 11> StationServiceLastUseTurns;
        pas::Array<pas::WideString, 1, 24> StatusEffectSourceNames;
        // Clamped to 0..100.
        std::uint8_t DiseaseImmunity;
        std::uint8_t cpp_padding_2[3];
        // Programs awarded for destroyed Dominator hull mass.
        pas::Array<std::int32_t, 0, 11> ProgramRewardStocks;
        std::int32_t LastDominatorProgramRewardTurn;
        // Accumulated capacity; reset after a program reward.
        std::int32_t DestroyedDominatorHullMass;
        // Owned deployed TSatellite instances.
        aMyFunction::TObjectList* Satellites;
        // Includes maps marked used without playing.
        pas::DynArray<TPlanetBattleHistoryEntry> PlanetBattleHistory;
        std::int32_t PlanetBattles;
        std::int32_t LastPlanetBattleTurn;
        // Suppresses planetary-battle offers (); set by DeclineAllPlanetBattles (), editable key RejectPB.
        std::uint8_t DeclinePlanetBattleOffers;
        std::uint8_t cpp_padding_3[1];
        std::uint16_t DiseaseContractionCount;
        std::uint16_t StimulantPurchaseCount;
        // PRISON: successful return from the prison quest; native increment.
        std::uint16_t PrisonStaysCompleted;
        // ARCHEOLOGY: accumulated water/land/hill exploration gains; native add.
        std::int32_t SatelliteTilesExplored;
        // MANYFACES: accepted nationality changes (). Serialized as a word.
        std::int32_t NationalityChangeCount;
        // SIDECHANGER: switches between Coalition and pirate allegiance (). Serialized as a word.
        std::int32_t SideChangeCount;
        // Zero-based; serialized as one byte.
        std::int32_t SelectedEquipmentConfiguration;
        pas::Array<TEquipmentConfiguration, 0, 9> EquipmentConfigurations;
        // Ship references, not owned by this list.
        pas::List* PiratePartners;
        // All initially true; serialized as six flags.
        pas::Array<std::uint8_t, 0, 5> UnresolvedFlagsDA8;
        std::uint8_t cpp_padding_4[2];
        // Owned TJournalRecord entries.
        aMyFunction::TObjectList* JournalRecords;
        // Separately allocated records, not TObject instances.
        pas::List* NewsEntries;
        // After a turn, opens ruins/government dialogue for the current docking target.
        std::uint8_t PendingDockDialogue;
        std::uint8_t NoJump;
        std::uint8_t PirateClanReal;
        std::uint8_t cpp_padding_5[1];
        Achievements::TAchievementStats* AchievementStats;
        // Script.GetShipExpByType kind 1.
        std::int32_t ExperienceByDominators;
        // Kind 2.
        std::int32_t ExperienceByPirates;
        // Kind 3.
        std::int32_t ExperienceByNormals;
        // Kind 4.
        std::int32_t ExperienceByTraderCareer;
        // Zero is inactive; EnterRuinsMode copies the capital-hull kind or explicit mode here.
        std::uint8_t RuinsMode;
        std::uint8_t cpp_padding_6[3];
        // Owned station proxy; initialized through a checked TRuins cast.
        aShip::TShip* RuinsProxy;
        aShip::TShip* RuinsSavedDockedTo;
        aPlanet::TPlanet* RuinsSavedPlanet;
        pas::WideString RuinsStatusText;
        EC_BlockPar::TBlockParEC* AwardedAchievementKeys;
        // Reset by NextDay; incremented for player bomb kills and checked for BOMBER.
        std::int32_t BombKillsThisTurn;
        // Script.PlayerLogicChameleon modes, indexed by TDominatorSeries.
        pas::Array<std::uint8_t, 0, 2> ChameleonLogic;
        std::uint8_t cpp_padding_7[1];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TStorageEntry {
        pas::Object* LocationOwner;
        std::int32_t SlotIndex;
        aItem::TItem* Item;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TStorageItemColumns {
        std::int32_t Heading;
        std::int32_t Size;
        std::int32_t Cost;
    };
    #pragma pack(pop)

    using TStorageItemColumnTable = pas::Array<TStorageItemColumns, 1, 2>;

    #pragma pack(push, 1)
    struct TStorageHeaderColumns {
        std::int32_t Size;
        std::int32_t Cost;
    };
    #pragma pack(pop)

    using TStorageHeaderColumnTable = pas::Array<TStorageHeaderColumns, 1, 2>;

    #pragma pack(push, 1)
    struct TProbeSummaryColumns {
        std::int32_t Heading;
        std::int32_t Size;
        std::int32_t Exploration;
        std::int32_t Condition;
        std::int32_t Status;
    };
    #pragma pack(pop)

    using TProbeSummaryColumnTable = pas::Array<TProbeSummaryColumns, 1, 2>;

    using TStorageDividerLengthTable = pas::Array<std::int32_t, 1, 2>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TJournalRecord : EC_Struct::TObjectEx {
        PAS_CLASS_META(TJournalRecord, EC_Struct::TObjectEx, "TJournalRecord", 12)
        void p_destroy() override;
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        std::int32_t DateTurn;
        pas::WideString Text;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aPlayer
