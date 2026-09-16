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
        pas::Array<std::int32_t, 0, 5> Statistics;
        std::int32_t ResultCode;
        std::int32_t CompletionMode;
        std::int32_t DateTurn;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TEquipmentConfiguration {
        pas::Array<std::int32_t, 0, 11> EquipmentIds;
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
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void InitializePlayerAtPlanet(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::int32_t CharacterPreset);
        void ApplyCharacterPreset(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::int32_t CharacterPreset);
        void NextDay() override;
        std::int32_t ComputeDepositAccruedValue();
        static void RechargeTransmitters();
        void ApplyBioArtefactHealthEffects();
        std::uint8_t MayTakeSubCrack();
        static std::int32_t GetSubCrackCost();
        std::uint8_t GetPirateServiceDiscount();
        std::int32_t CountProgramRewardStocks();
        std::uint8_t TryAwardDominatorPrograms(aShip::TShip* Victim);
        std::uint8_t FindProfitableTradeRoute(std::uint8_t Nearby, std::uint32_t Seed, aPlanet::TPlanet*& PurchasePlanet, aPlanet::TPlanet*& SalePlanet, std::uint8_t& Good, aGalaxyStruct::TItemTypeMask GoodsMask);
        std::uint8_t HasDeployedSatellites();
        std::uint8_t HasSatelliteOnPlanet(aPlanet::TPlanet* Planet);
        static pas::WideString GetStorageColumnHeaderText();
        static pas::WideString GetStorageDividerText();
        pas::WideString BuildDeployedSatelliteSummary(std::int32_t& LineCount);
        static std::int32_t GetSatelliteExplorationTurns(aItem::TSatellite* Satellite);
        static std::uint8_t CanAccessSurfaceLootItem(aItem::TItem* Item);
        void ReportIdleSatellites(aGalaxy::TStar* Star);
        static void BeginStorageTurn();
        static std::uint8_t CanAccessHoldGoods(std::uint8_t Good);
        static std::uint8_t CanAccessStoredItem(aItem::TItem* Item);
        std::int32_t CountStoredItemUnits(pas::Object* Location, std::uint8_t ItemType);
        void RepairDuplicateStorageSlots(pas::Object* Location);
        std::int32_t FindNextStorageSlot(pas::Object* Location);
        std::int32_t GetStorageSlotExtent(pas::Object* Location);
        std::int32_t FindStorageIndexByLocationAndSlot(pas::Object* Location, std::int32_t Slot);
        std::int32_t FindStorageGoodsByLocationAndType(pas::Object* Location, std::uint8_t Good);
        std::int32_t FindMergeableStorageItemByLocation(pas::Object* Location, aItem::TCountableItem* Item);
        void ShiftStorageSlotsAtOrAfter(pas::Object* Location, std::int32_t Slot);
        void CloseVacantStorageSlot(pas::Object* Location, std::int32_t Slot);
        std::uint8_t HasAccessibleStorageAt(pas::Object* Location);
        std::uint8_t CountPartnersInNormalSpace();
        static std::uint8_t GetShipRatingComparison(aShip::TShip* Ship);
        static std::uint8_t GetShipRankComparison(aShip::TShip* Ship);
        static std::uint8_t GetShipPirateRankComparison(aShip::TShip* Ship);
        static std::uint8_t GetShipStrengthComparison(aShip::TShip* Ship);
        pas::WideString BuildTranclucatorStorageSummary(std::int32_t& LineCount);
        static std::int32_t CompareStorageEntries(PStorageEntry Left, PStorageEntry Right);
        void SortStorageEntries();
        void RefreshStorageBubbles();
        void BuildStorageBubbles();
        std::int32_t SelectPlanetBattleMap();
        void SaveEquipmentConfiguration(std::int32_t Index);
        void ApplyEquipmentConfiguration(std::int32_t Index);
        std::uint8_t HasEquipmentConfiguration(std::int32_t Index);
        std::int32_t GetAvailableNodeCount(aShip::TShip* Carrier);
        void ConsumeAvailableNodes(std::int32_t Count, aShip::TShip* Carrier);
        std::int32_t GetMaxPiratePartners();
        std::int32_t GetMaxDominionShips();
        void AddJournalRecord(pas::WideString Text);
        void DeleteJournalRecord(std::int32_t Index);
        void ClearJournal();
        pas::WideString ExportJournal();
        void SortNewsEntries();
        void TrimNewsEntries(std::int32_t KeepCount);
        pas::WideString ExportNews();
        void MergeGalaxyNews();
        void RefreshNewsAtLocation();
        std::int32_t CalculateSpeed() override;
        void CreateRuinsProxy();
        void EnterRuinsMode(std::int32_t Mode);
        void CloseRuinsModeScreen();
        void ExitRuinsMode();
        void RefreshCurrentStanding() override;
        std::uint8_t CanSelectShipTarget(aShip::TShip* Ship);
        static std::uint8_t CanScanShip(aShip::TShip* Ship);
        std::uint8_t InPrison;
        std::uint8_t TalkLocked;
        std::uint8_t ScanLocked;
        std::uint8_t cpp_padding[1];
        std::int32_t HyperspaceKillCount;
        std::int32_t BlackHoleKillCount;
        pas::Array<std::int32_t, 0, 7> DominatorKillsByType;
        pas::List* ScriptShipBindings;
        aShip::TShip* QuestTargetKillShip;
        aShip::TShip* QuestTargetDefendShip;
        aGalaxy::TStar* QuestTargetDefendStar;
        pas::List* StorageEntries;
        std::int32_t DebtAmount;
        std::int32_t DebtDueTurn;
        std::int32_t DebtDefaultCount;
        std::int32_t DepositAmount;
        std::int32_t DepositStartTurn;
        std::int32_t DepositDayCount;
        float DepositInterestRate;
        std::int32_t MedicalPolicyTicks;
        std::int32_t PirateLicenseTicks;
        std::int32_t PirateLicenseCash;
        std::int32_t PendingPirateLicenseCash;
        aGalaxy::TStar* QueuedTravelTarget;
        pas::Array<std::int32_t, 0, 11> StationServiceLastUseTurns;
        pas::Array<pas::WideString, 1, 24> StatusEffectSourceNames;
        std::uint8_t DiseaseImmunity;
        std::uint8_t cpp_padding_2[3];
        pas::Array<std::int32_t, 0, 11> ProgramRewardStocks;
        std::int32_t LastDominatorProgramRewardTurn;
        std::int32_t DestroyedDominatorHullMass;
        aMyFunction::TObjectList* Satellites;
        pas::DynArray<TPlanetBattleHistoryEntry> PlanetBattleHistory;
        std::int32_t PlanetBattles;
        std::int32_t LastPlanetBattleTurn;
        std::uint8_t DeclinePlanetBattleOffers;
        std::uint8_t cpp_padding_3[1];
        std::uint16_t DiseaseContractionCount;
        std::uint16_t StimulantPurchaseCount;
        std::uint16_t PrisonStaysCompleted;
        std::int32_t SatelliteTilesExplored;
        std::int32_t NationalityChangeCount;
        std::int32_t SideChangeCount;
        std::int32_t SelectedEquipmentConfiguration;
        pas::Array<TEquipmentConfiguration, 0, 9> EquipmentConfigurations;
        pas::List* PiratePartners;
        pas::Array<std::uint8_t, 0, 5> UnresolvedFlagsDA8;
        std::uint8_t cpp_padding_4[2];
        aMyFunction::TObjectList* JournalRecords;
        pas::List* NewsEntries;
        std::uint8_t PendingDockDialogue;
        std::uint8_t NoJump;
        std::uint8_t PirateClanReal;
        std::uint8_t cpp_padding_5[1];
        Achievements::TAchievementStats* AchievementStats;
        std::int32_t ExperienceByDominators;
        std::int32_t ExperienceByPirates;
        std::int32_t ExperienceByNormals;
        std::int32_t ExperienceByTraderCareer;
        std::uint8_t RuinsMode;
        std::uint8_t cpp_padding_6[3];
        aShip::TShip* RuinsProxy;
        aShip::TShip* RuinsSavedDockedTo;
        aPlanet::TPlanet* RuinsSavedPlanet;
        pas::WideString RuinsStatusText;
        EC_BlockPar::TBlockParEC* AwardedAchievementKeys;
        std::int32_t BombKillsThisTurn;
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
