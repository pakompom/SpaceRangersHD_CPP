#include "layout/aPlayer.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_Sound.hpp"
#include "types/SimpleSteamApi.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aTranclucator.hpp"
#include "units/Achievements.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/SystemImports.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aRuins.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fEquipmentShop.hpp"

namespace aPlayer {
    using TOwnerMasks = pas::Array<aGalaxyStruct::TOwnerMask, 0, 2>;

    // Nested in ApplyEquipmentConfiguration; caller-popped static link, player -4 and preset index -8.
    std::uint8_t SupportsItem(aItem::TItem* Item);

    // Nested in ApplyEquipmentConfiguration; caller-popped static link, player -4 and preset index -8.
    std::int32_t FindSlot(aItem::TItem* Item, TPlayer* Self, std::int32_t& Index);

    // Nested in ApplyEquipmentConfiguration; caller-popped static link, player -4 and preset index -8.
    void ReleaseStorageEntry(PStorageEntry Entry);

    // Nested in ApplyEquipmentConfiguration; caller-popped static link, player -4 and preset index -8.
    void TakeStoredItem(PStorageEntry Entry, std::int32_t Slot, TPlayer* Self);

    // Native campaign reward guard.
    std::int32_t ArcadeKellerDefeats{};

    // Owned reward item pending transfer.
    pas::Object* ArcadeKellerReward{};

    // The native initial DWORD decodes to nil; zero would decode to a non-null pointer.
    std::uint32_t EncodedPlayer = 0xb1cd15d3u;

    aPlayer::TStorageHeaderColumnTable StorageHeaderColumns = aPlayer::TStorageHeaderColumnTable{{{.Size = 300, .Cost = 380}, {.Size = 400, .Cost = 490}}};

    aPlayer::TStorageDividerLengthTable StorageDividerLengths = aPlayer::TStorageDividerLengthTable{{76, 98}};

    aPlayer::TProbeSummaryColumnTable ProbeSummaryColumns = aPlayer::TProbeSummaryColumnTable{{{.Heading = 190, .Size = 160, .Exploration = 220, .Condition = 280, .Status = 295}, {.Heading = 250, .Size = 200, .Exploration = 280, .Condition = 370, .Status = 390}}};

    aPlayer::TStorageDividerLengthTable TranclucatorSummaryWidths = aPlayer::TStorageDividerLengthTable{{180, 240}};

    aPlayer::TStorageItemColumnTable StorageItemColumns = aPlayer::TStorageItemColumnTable{{{.Heading = 190, .Size = 300, .Cost = 380}, {.Heading = 250, .Size = 400, .Cost = 490}}};

    // Updates Galaxy.PlayerRangerIndex; a nil Galaxy leaves the current player unchanged.
    void SetPlayer(TPlayer* Player, aGalaxy::TGalaxy* Galaxy) {
        if (Galaxy != nullptr) {
            EncodedPlayer = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Player)) ^ 0xb1cd15d3u;
            if (Player == nullptr) {
                Galaxy->PlayerRangerIndex = -1;
            } else if (Galaxy->Rangers == nullptr) {
                Galaxy->PlayerRangerIndex = -1;
            } else {
                Galaxy->PlayerRangerIndex = pas::list_indexof(Galaxy->Rangers, reinterpret_cast<void*>(Player));
            }
        }
    }

    TPlayer* GetPlayer() {
        return reinterpret_cast<TPlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EncodedPlayer ^ 0xb1cd15d3u)));
    }

    void TJournalRecord_Create(TJournalRecord* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->DateTurn = 0;
        Self->Text = pas::WideString();
    }

    void TJournalRecord_Destroy(TJournalRecord* Self) {
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TJournalRecord::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddIntegerValue(DateTurn);
        Buffer->AddWideStringZ(Text);
    }

    void TJournalRecord::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        DateTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        Text = Buffer->ReadWideString();
    }

    // Native constructor initializes lists/defaults; does not register or generate the player loadout.
    void TPlayer_Create(TPlayer* Self) {
        std::uint8_t ServiceIndex{};
        std::int32_t I{};
        std::int32_t J{};
        std::uint8_t RewardIndex{};
        std::uint8_t KillIndex{};
        std::uint8_t LogicIndex{};
        aNormalShip::TNormalShip_Create(Self);
        Self->StorageEntries = pas::make_object<pas::List>();
        Self->TalkLocked = false;
        Self->ScanLocked = false;
        Self->ScriptShipBindings = pas::make_object<pas::List>();
        Self->HyperspaceKillCount = 0;
        Self->BlackHoleKillCount = 0;
        for (KillIndex = static_cast<std::uint8_t>(0); KillIndex <= static_cast<std::uint8_t>(7); ++KillIndex) {
            Self->DominatorKillsByType[KillIndex] = 0;
        }
        for (LogicIndex = static_cast<std::uint8_t>(0); LogicIndex <= static_cast<std::uint8_t>(2); ++LogicIndex) {
            Self->ChameleonLogic[LogicIndex] = 0;
        }
        Self->DebtAmount = 0;
        Self->DebtDueTurn = 0;
        Self->DebtDefaultCount = 0;
        Self->DepositAmount = 0;
        Self->DepositStartTurn = 0;
        Self->DepositDayCount = 0;
        Self->DepositInterestRate = 0.0f;
        // The native constructor really uses the current turn for this duration field.
        if (aGalaxy::Galaxy != nullptr) {
            Self->MedicalPolicyTicks = aGalaxy::Galaxy->CurrentTurn;
        }
        Self->PirateLicenseTicks = 0;
        Self->PirateLicenseCash = 0;
        Self->PendingPirateLicenseCash = 0;
        for (ServiceIndex = static_cast<std::uint8_t>(0); ServiceIndex <= static_cast<std::uint8_t>(11); ++ServiceIndex) {
            Self->StationServiceLastUseTurns[ServiceIndex] = 150;
        }
        for (I = 1; I <= 24; ++I) {
            Self->StatusEffectSourceNames[I] = pas::WideString();
        }
        Self->DiseaseImmunity = 50;
        for (RewardIndex = static_cast<std::uint8_t>(0); RewardIndex <= static_cast<std::uint8_t>(11); ++RewardIndex) {
            Self->ProgramRewardStocks[RewardIndex] = 0;
        }
        Self->LastDominatorProgramRewardTurn = 0;
        Self->DestroyedDominatorHullMass = 0;
        Self->PlanetBattles = 0;
        Self->LastPlanetBattleTurn = 0;
        Self->DeclinePlanetBattleOffers = false;
        Self->Satellites = pas::make_object<aMyFunction::TObjectList>();
        Self->DiseaseContractionCount = 0;
        Self->StimulantPurchaseCount = 0;
        Self->PrisonStaysCompleted = 0;
        Self->SatelliteTilesExplored = 0;
        Self->NationalityChangeCount = 0;
        Self->SideChangeCount = 0;
        ArcadeKellerDefeats = 0;
        ArcadeKellerReward = nullptr;
        Self->SelectedEquipmentConfiguration = 0;
        for (I = 0; I <= 9; ++I) {
            for (J = 0; J <= 11; ++J) {
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&Self->EquipmentConfigurations[I].EquipmentIds, J * sizeof(std::int32_t)), 0);
            }
            for (J = 0; J <= 31; ++J) {
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&Self->EquipmentConfigurations[I].ArtefactIds, J * sizeof(std::int32_t)), 0);
            }
        }
        Self->PiratePartners = pas::make_object<pas::List>();
        for (I = 0; I <= 5; ++I) {
            Self->UnresolvedFlagsDA8[I] = true;
        }
        Self->JournalRecords = pas::make_object<aMyFunction::TObjectList>();
        Self->NewsEntries = pas::make_object<pas::List>();
        Self->PirateClanReal = false;
        Self->AchievementStats = pas::construct_call<Achievements::TAchievementStats>(Achievements::TAchievementStats_Create);
        Self->RuinsMode = 0;
        Self->RuinsProxy = nullptr;
        Self->RuinsSavedDockedTo = nullptr;
        Self->RuinsSavedPlanet = nullptr;
        Self->AwardedAchievementKeys = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        Self->QueuedTravelTarget = nullptr;
    }

    // Requires the inherited ranger registration state for final cleanup.
    void TPlayer_Destroy(TPlayer* Self) {
        std::int32_t I{};
        PStorageEntry Entry{};
        if (Self->ScriptShipBindings != nullptr) {
            while (pas::list_count(Self->ScriptShipBindings) > 0) {
                pas::list_at<aScript::TScriptShip>(Self->ScriptShipBindings, 0)->Script->UnbindShip(Self);
            }
            pas::list_clear(Self->ScriptShipBindings);
            pas::free(Self->ScriptShipBindings);
            Self->ScriptShipBindings = nullptr;
        }
        if (Self->CurrentStar != nullptr) {
            I = pas::list_indexof(Self->CurrentStar->Ships, reinterpret_cast<void*>(Self));
            if (I >= 0) {
                pas::list_delete(Self->CurrentStar->Ships, I);
            }
        }
        if (Self->StorageEntries != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->StorageEntries) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TStorageEntry>(Self->StorageEntries, I);
                if (Entry != nullptr) {
                    if (Entry->Item != nullptr) {
                        pas::free(Entry->Item);
                        Entry->Item = nullptr;
                    }
                    pas::dispose(Entry);
                    pas::list_put(Self->StorageEntries, I, nullptr);
                }
            }
            pas::list_clear(Self->StorageEntries);
            pas::free(Self->StorageEntries);
            Self->StorageEntries = nullptr;
        }
        if (Self->Satellites != nullptr) {
            pas::free(Self->Satellites);
            Self->Satellites = nullptr;
        }
        if (Self->PiratePartners != nullptr) {
            pas::free(Self->PiratePartners);
            Self->PiratePartners = nullptr;
        }
        if (Self->JournalRecords != nullptr) {
            pas::free(Self->JournalRecords);
            Self->JournalRecords = nullptr;
        }
        Self->TrimNewsEntries(0);
        pas::list_clear(Self->NewsEntries);
        pas::free(Self->NewsEntries);
        Self->NewsEntries = nullptr;
        if (Self->RuinsProxy != nullptr) {
            pas::free(Self->RuinsProxy);
            Self->RuinsProxy = nullptr;
        }
        pas::free(Self->AwardedAchievementKeys);
        Self->AwardedAchievementKeys = nullptr;
        aRanger::TRanger_Destroy(Self);
    }

    void TPlayer::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t SlotCount{};
        PStorageEntry Entry{};
        std::uint8_t ServiceIndex{};
        std::uint8_t RewardIndex{};
        aGalaxy::PPlanetNewsEntry News{};
        std::uint8_t KillIndex{};
        std::uint8_t LogicIndex{};
        aRanger::TRanger::SaveToBuffer(Buffer);
        Buffer->AddBoolean(InPrison);
        Buffer->AddBoolean(TalkLocked);
        Buffer->AddBoolean(ScanLocked);
        Buffer->AddIntegerValue(HyperspaceKillCount);
        Buffer->AddIntegerValue(BlackHoleKillCount);
        for (KillIndex = static_cast<std::uint8_t>(0); KillIndex <= static_cast<std::uint8_t>(7); ++KillIndex) {
            Buffer->AddIntegerValue(DominatorKillsByType[KillIndex]);
        }
        for (LogicIndex = static_cast<std::uint8_t>(0); LogicIndex <= static_cast<std::uint8_t>(2); ++LogicIndex) {
            Buffer->AddAnsiChar(ChameleonLogic[LogicIndex]);
        }
        Buffer->AddIntegerValue(pas::list_count(StorageEntries));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
            if (pas::class_cast_if<aPlanet::TPlanet*>(Entry->LocationOwner) != nullptr) {
                Buffer->AddAnsiChar('\000');
                Buffer->AddDWord(pas::checked_cast<aPlanet::TPlanet*>(Entry->LocationOwner)->Id);
            } else {
                Buffer->AddAnsiChar('\001');
                Buffer->AddDWord(pas::checked_cast<aShip::TShip*>(Entry->LocationOwner)->Id);
            }
            Buffer->AddIntegerValue(Entry->SlotIndex);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Entry->Item->ItemType));
            Entry->Item->SaveToBuffer(Buffer);
        }
        Buffer->AddIntegerValue(DebtAmount);
        Buffer->AddIntegerValue(DebtDueTurn);
        Buffer->AddIntegerValue(DebtDefaultCount);
        Buffer->AddIntegerValue(DepositAmount);
        Buffer->AddIntegerValue(DepositStartTurn);
        Buffer->AddIntegerValue(DepositDayCount);
        Buffer->AddSingle(DepositInterestRate);
        Buffer->AddIntegerValue(MedicalPolicyTicks);
        Buffer->AddIntegerValue(PirateLicenseTicks);
        Buffer->AddIntegerValue(PirateLicenseCash);
        Buffer->AddIntegerValue(PendingPirateLicenseCash);
        if (QueuedTravelTarget == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(QueuedTravelTarget->Id);
        }
        for (ServiceIndex = static_cast<std::uint8_t>(0); ServiceIndex <= static_cast<std::uint8_t>(11); ++ServiceIndex) {
            Buffer->AddIntegerValue(StationServiceLastUseTurns[ServiceIndex]);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 24); cpp_range_2.next(I); ) {
            Buffer->AddWideStringZ(StatusEffectSourceNames[I]);
        }
        Buffer->AddAnsiChar(DiseaseImmunity);
        for (RewardIndex = static_cast<std::uint8_t>(0); RewardIndex <= static_cast<std::uint8_t>(11); ++RewardIndex) {
            Buffer->AddIntegerValue(ProgramRewardStocks[RewardIndex]);
        }
        Buffer->AddIntegerValue(LastDominatorProgramRewardTurn);
        Buffer->AddIntegerValue(DestroyedDominatorHullMass);
        Buffer->AddIntegerValue(pas::list_count(Satellites));
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Satellites) - 1); cpp_range_3.next(I); ) {
            pas::list_at<aItem::TSatellite>(Satellites, I)->SaveToBuffer(Buffer);
        }
        Buffer->AddIntegerValue(PlanetBattleHistory.length() - 1 + 1);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, PlanetBattleHistory.length() - 1); cpp_range_4.next(I); ) {
            Buffer->AddIntegerValue(PlanetBattleHistory[I].MapId);
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&PlanetBattleHistory[I].Statistics, 0 * sizeof(std::int32_t))));
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&PlanetBattleHistory[I].Statistics, 1 * sizeof(std::int32_t))));
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&PlanetBattleHistory[I].Statistics, 2 * sizeof(std::int32_t))));
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&PlanetBattleHistory[I].Statistics, 3 * sizeof(std::int32_t))));
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&PlanetBattleHistory[I].Statistics, 4 * sizeof(std::int32_t))));
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&PlanetBattleHistory[I].Statistics, 5 * sizeof(std::int32_t))));
            Buffer->AddIntegerValue(PlanetBattleHistory[I].ResultCode);
            Buffer->AddIntegerValue(PlanetBattleHistory[I].CompletionMode);
            Buffer->AddIntegerValue(PlanetBattleHistory[I].DateTurn);
        }
        Buffer->AddIntegerValue(PlanetBattles);
        Buffer->AddIntegerValue(LastPlanetBattleTurn);
        Buffer->AddBoolean(DeclinePlanetBattleOffers);
        Buffer->AddWideChar(DiseaseContractionCount);
        Buffer->AddWideChar(StimulantPurchaseCount);
        Buffer->AddWideChar(PrisonStaysCompleted);
        Buffer->AddIntegerValue(SatelliteTilesExplored);
        Buffer->AddWideChar(NationalityChangeCount);
        Buffer->AddWideChar(SideChangeCount);
        Buffer->AddAnsiChar(SelectedEquipmentConfiguration);
        std::int32_t ConfigurationCount = 10;
        Buffer->AddAnsiChar(ConfigurationCount);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, ConfigurationCount - 1); cpp_range_5.next(I); ) {
            SlotCount = 12;
            Buffer->AddWideChar(SlotCount);
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_6.next(J); ) {
                Buffer->AddDWord(pas::load_unaligned<std::int32_t>(pas::byte_offset(&EquipmentConfigurations[I].EquipmentIds, J * sizeof(std::int32_t))));
            }
            SlotCount = 32;
            Buffer->AddWideChar(SlotCount);
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_7.next(J); ) {
                Buffer->AddDWord(pas::load_unaligned<std::int32_t>(pas::byte_offset(&EquipmentConfigurations[I].ArtefactIds, J * sizeof(std::int32_t))));
            }
        }
        Buffer->AddAnsiChar(pas::list_count(PiratePartners));
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(PiratePartners) - 1); cpp_range_8.next(I); ) {
            Buffer->AddDWord(pas::list_at<aShip::TShip>(PiratePartners, I)->Id);
        }
        std::int32_t ListCount = 6;
        Buffer->AddAnsiChar(ListCount);
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, ListCount - 1); cpp_range_9.next(I); ) {
            Buffer->AddBoolean(UnresolvedFlagsDA8[I]);
        }
        ListCount = pas::list_count(JournalRecords);
        Buffer->AddDWord(pas::list_count(JournalRecords));
        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, ListCount - 1); cpp_range_10.next(I); ) {
            pas::list_at<TJournalRecord>(JournalRecords, I)->SaveToBuffer(Buffer);
        }
        std::int32_t NewsCount = pas::list_count(NewsEntries);
        Buffer->AddWideChar(NewsCount);
        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, NewsCount - 1); cpp_range_11.next(I); ) {
            News = pas::list_at<aGalaxyStruct::TPlanetNews>(NewsEntries, I);
            Buffer->AddDWord(News->Id);
            Buffer->AddDWord(News->Turn);
            Buffer->AddAnsiChar(News->NewsType);
            Buffer->AddWideStringZ(News->Text);
        }
        Buffer->AddAnsiChar(PendingDockDialogue);
        Buffer->AddBoolean(NoJump);
        Buffer->AddBoolean(PirateClanReal);
        AchievementStats->SaveToBuffer(Buffer);
        Buffer->AddIntegerValue(ExperienceByDominators);
        Buffer->AddIntegerValue(ExperienceByPirates);
        Buffer->AddIntegerValue(ExperienceByNormals);
        Buffer->AddIntegerValue(ExperienceByTraderCareer);
        Buffer->AddAnsiChar(RuinsMode);
        if (RuinsProxy == nullptr) {
            CreateRuinsProxy();
        }
        pas::checked_cast<aRuins::TRuins*>(RuinsProxy)->SaveToBuffer(Buffer);
        if (RuinsMode > 0) {
            if (RuinsSavedDockedTo == nullptr) {
                Buffer->AddDWord(0u);
            } else {
                Buffer->AddDWord(RuinsSavedDockedTo->Id);
            }
            if (RuinsSavedPlanet == nullptr) {
                Buffer->AddDWord(0u);
            } else {
                Buffer->AddDWord(RuinsSavedPlanet->Id);
            }
        }
        Buffer->AddWideStringZ(RuinsStatusText);
        Buffer->AddIntegerValue(AwardedAchievementKeys->GetBlockCount());
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, AwardedAchievementKeys->GetBlockCount() - 1); cpp_range_12.next(I); ) {
            Buffer->AddWideStringZ(AwardedAchievementKeys->GetBlockNameByIndex(I));
        }
    }

    void TPlayer::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t SlotCount{};
        PStorageEntry Entry{};
        std::uint8_t ServiceIndex{};
        std::uint8_t RewardIndex{};
        aItem::TSatellite* Satellite{};
        TJournalRecord* Journal{};
        aGalaxy::PPlanetNewsEntry News{};
        std::int32_t AchievementIndex{};
        SimpleSteamApi::PAchievementData Data{};
        std::uint8_t KillIndex{};
        std::uint8_t LogicIndex{};
        aRanger::TRanger::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion <= 164) {
            ClearRecentlyDroppedItems();
        }
        InPrison = EC_Buf::TBufEC_GetBoolean(Buffer);
        TalkLocked = EC_Buf::TBufEC_GetBoolean(Buffer);
        ScanLocked = EC_Buf::TBufEC_GetBoolean(Buffer);
        HyperspaceKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
        BlackHoleKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 89) {
            for (KillIndex = static_cast<std::uint8_t>(0); KillIndex <= static_cast<std::uint8_t>(7); ++KillIndex) {
                DominatorKillsByType[KillIndex] = EC_Buf::TBufEC_GetInt32(Buffer);
            }
        } else if (GlobalsV::LoadedSaveVersion >= 74) {
            for (KillIndex = static_cast<std::uint8_t>(0); KillIndex <= static_cast<std::uint8_t>(5); ++KillIndex) {
                DominatorKillsByType[KillIndex] = EC_Buf::TBufEC_GetInt32(Buffer);
            }
            DominatorKillsByType[6] = 0;
            DominatorKillsByType[7] = 0;
        } else {
            for (KillIndex = static_cast<std::uint8_t>(0); KillIndex <= static_cast<std::uint8_t>(7); ++KillIndex) {
                DominatorKillsByType[KillIndex] = 0;
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 155) {
            for (LogicIndex = static_cast<std::uint8_t>(0); LogicIndex <= static_cast<std::uint8_t>(2); ++LogicIndex) {
                ChameleonLogic[LogicIndex] = EC_Buf::TBufEC_GetByte(Buffer);
            }
        }
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err"_a));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            pas::new_value(Entry);
            if (EC_Buf::TBufEC_GetByte(Buffer) == 0) {
                Entry->LocationOwner = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer) | 0x80000000u)));
            } else {
                Entry->LocationOwner = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            }
            Entry->SlotIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            Entry->Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
            pas::list_add(StorageEntries, static_cast<void*>(Entry));
            Entry->Item->LoadFromBuffer(Buffer, Galaxy);
        }
        DebtAmount = EC_Buf::TBufEC_GetInt32(Buffer);
        DebtDueTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        DebtDefaultCount = EC_Buf::TBufEC_GetInt32(Buffer);
        DepositAmount = EC_Buf::TBufEC_GetInt32(Buffer);
        DepositStartTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        DepositDayCount = EC_Buf::TBufEC_GetInt32(Buffer);
        DepositInterestRate = EC_Buf::TBufEC_GetSingle(Buffer);
        MedicalPolicyTicks = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 103) {
            PirateLicenseTicks = EC_Buf::TBufEC_GetInt32(Buffer);
            PirateLicenseCash = EC_Buf::TBufEC_GetInt32(Buffer);
            PendingPirateLicenseCash = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            PirateLicenseTicks = 0;
            PirateLicenseCash = 0;
            PendingPirateLicenseCash = 0;
        }
        if (GlobalsV::LoadedSaveVersion >= 108) {
            QueuedTravelTarget = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        } else {
            QueuedTravelTarget = nullptr;
        }
        for (ServiceIndex = static_cast<std::uint8_t>(0); ServiceIndex <= static_cast<std::uint8_t>(11); ++ServiceIndex) {
            StationServiceLastUseTurns[ServiceIndex] = EC_Buf::TBufEC_GetInt32(Buffer);
        }
        for (I = 1; I <= 24; ++I) {
            StatusEffectSourceNames[I] = Buffer->ReadWideString();
        }
        DiseaseImmunity = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion < 49) {
            ProgramRewardStocks[aGalaxyStruct::prgKellerCall] = 0;
            for (RewardIndex = static_cast<std::uint8_t>(aGalaxyStruct::prgLogicalNegation); RewardIndex <= static_cast<std::uint8_t>(11); ++RewardIndex) {
                ProgramRewardStocks[RewardIndex] = EC_Buf::TBufEC_GetInt32(Buffer);
            }
        } else {
            for (RewardIndex = static_cast<std::uint8_t>(0); RewardIndex <= static_cast<std::uint8_t>(11); ++RewardIndex) {
                ProgramRewardStocks[RewardIndex] = EC_Buf::TBufEC_GetInt32(Buffer);
            }
        }
        LastDominatorProgramRewardTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        DestroyedDominatorHullMass = EC_Buf::TBufEC_GetInt32(Buffer);
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Satellite = pas::construct_call<aItem::TSatellite>(aItem::TEquipment_Create);
            pas::list_add(Satellites, reinterpret_cast<void*>(Satellite));
            Satellite->LoadFromBuffer(Buffer, Galaxy);
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        PlanetBattleHistory.set_length(Count);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            PlanetBattleHistory[I].MapId = EC_Buf::TBufEC_GetInt32(Buffer);
            {
                std::int32_t cpp_value = EC_Buf::TBufEC_GetInt32(Buffer);
                auto cpp_target = pas::byte_offset(&PlanetBattleHistory[I].Statistics, 0 * sizeof(std::int32_t));
                pas::store_unaligned<std::int32_t>(cpp_target, cpp_value);
            }
            {
                std::int32_t cpp_value_2 = EC_Buf::TBufEC_GetInt32(Buffer);
                auto cpp_target_2 = pas::byte_offset(&PlanetBattleHistory[I].Statistics, 1 * sizeof(std::int32_t));
                pas::store_unaligned<std::int32_t>(cpp_target_2, cpp_value_2);
            }
            {
                std::int32_t cpp_value_3 = EC_Buf::TBufEC_GetInt32(Buffer);
                auto cpp_target_3 = pas::byte_offset(&PlanetBattleHistory[I].Statistics, 2 * sizeof(std::int32_t));
                pas::store_unaligned<std::int32_t>(cpp_target_3, cpp_value_3);
            }
            {
                std::int32_t cpp_value_4 = EC_Buf::TBufEC_GetInt32(Buffer);
                auto cpp_target_4 = pas::byte_offset(&PlanetBattleHistory[I].Statistics, 3 * sizeof(std::int32_t));
                pas::store_unaligned<std::int32_t>(cpp_target_4, cpp_value_4);
            }
            {
                std::int32_t cpp_value_5 = EC_Buf::TBufEC_GetInt32(Buffer);
                auto cpp_target_5 = pas::byte_offset(&PlanetBattleHistory[I].Statistics, 4 * sizeof(std::int32_t));
                pas::store_unaligned<std::int32_t>(cpp_target_5, cpp_value_5);
            }
            {
                std::int32_t cpp_value_6 = EC_Buf::TBufEC_GetInt32(Buffer);
                auto cpp_target_6 = pas::byte_offset(&PlanetBattleHistory[I].Statistics, 5 * sizeof(std::int32_t));
                pas::store_unaligned<std::int32_t>(cpp_target_6, cpp_value_6);
            }
            PlanetBattleHistory[I].ResultCode = EC_Buf::TBufEC_GetInt32(Buffer);
            PlanetBattleHistory[I].CompletionMode = EC_Buf::TBufEC_GetInt32(Buffer);
            PlanetBattleHistory[I].DateTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        }
        PlanetBattles = EC_Buf::TBufEC_GetInt32(Buffer);
        LastPlanetBattleTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 59) {
            DeclinePlanetBattleOffers = EC_Buf::TBufEC_GetBoolean(Buffer);
        } else {
            DeclinePlanetBattleOffers = false;
        }
        DiseaseContractionCount = EC_Buf::TBufEC_GetWord(Buffer);
        StimulantPurchaseCount = EC_Buf::TBufEC_GetWord(Buffer);
        PrisonStaysCompleted = EC_Buf::TBufEC_GetWord(Buffer);
        SatelliteTilesExplored = EC_Buf::TBufEC_GetInt32(Buffer);
        NationalityChangeCount = EC_Buf::TBufEC_GetWord(Buffer);
        SideChangeCount = EC_Buf::TBufEC_GetWord(Buffer);
        SelectedEquipmentConfiguration = 0;
        SelectedEquipmentConfiguration = EC_Buf::TBufEC_GetByte(Buffer);
        std::int32_t ConfigurationCount = EC_Buf::TBufEC_GetByte(Buffer);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, ConfigurationCount - 1); cpp_range_4.next(I); ) {
            SlotCount = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_5.next(J); ) {
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&EquipmentConfigurations[I].EquipmentIds, J * sizeof(std::int32_t)), static_cast<std::int32_t>(EC_Buf::TBufEC_GetUInt32(Buffer)));
            }
            SlotCount = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range_6.next(J); ) {
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&EquipmentConfigurations[I].ArtefactIds, J * sizeof(std::int32_t)), static_cast<std::int32_t>(EC_Buf::TBufEC_GetUInt32(Buffer)));
            }
        }
        std::int32_t PartnerCount = EC_Buf::TBufEC_GetByte(Buffer);
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, PartnerCount - 1); cpp_range_7.next(I); ) {
            void* uInt32 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            pas::List* piratePartners = PiratePartners;
            pas::list_add(piratePartners, uInt32);
        }
        Count = EC_Buf::TBufEC_GetByte(Buffer);
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(I); ) {
            UnresolvedFlagsDA8[I] = EC_Buf::TBufEC_GetBoolean(Buffer);
        }
        Count = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err"_a));
        }
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_9.next(I); ) {
            Journal = pas::construct_call<TJournalRecord>(TJournalRecord_Create);
            pas::list_add(JournalRecords, reinterpret_cast<void*>(Journal));
            Journal->LoadFromBuffer(Buffer);
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err"_a));
        }
        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_10.next(I); ) {
            pas::new_value(News);
            pas::list_add(NewsEntries, static_cast<void*>(News));
            News->Id = EC_Buf::TBufEC_GetUInt32(Buffer);
            News->Turn = EC_Buf::TBufEC_GetUInt32(Buffer);
            News->NewsType = EC_Buf::TBufEC_GetByte(Buffer);
            News->Text = Buffer->ReadWideString();
        }
        PendingDockDialogue = EC_Buf::TBufEC_GetByte(Buffer);
        NoJump = EC_Buf::TBufEC_GetBoolean(Buffer);
        PirateClanReal = EC_Buf::TBufEC_GetBoolean(Buffer);
        AchievementStats->LoadFromBuffer(Buffer);
        ExperienceByDominators = EC_Buf::TBufEC_GetInt32(Buffer);
        ExperienceByPirates = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 57) {
            ExperienceByNormals = EC_Buf::TBufEC_GetInt32(Buffer);
        }
        ExperienceByTraderCareer = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 51) {
            RuinsMode = EC_Buf::TBufEC_GetByte(Buffer);
            RuinsProxy = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
            pas::checked_cast<aRuins::TRuins*>(RuinsProxy)->LoadFromBuffer(Buffer, Galaxy);
        } else {
            RuinsMode = 0;
            RuinsProxy = nullptr;
        }
        if (RuinsMode > 0) {
            RuinsSavedDockedTo = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            RuinsSavedPlanet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        } else {
            RuinsSavedDockedTo = nullptr;
            RuinsSavedPlanet = nullptr;
        }
        if (GlobalsV::LoadedSaveVersion >= 114) {
            RuinsStatusText = Buffer->ReadWideString();
        } else {
            RuinsStatusText = pas::WideString();
        }
        if (GlobalsV::LoadedSaveVersion >= 120) {
            Count = EC_Buf::TBufEC_GetInt32(Buffer);
            for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_11.next(I); ) {
                const pas::WideString& readWideString = Buffer->ReadWideString();
                EC_BlockPar::TBlockParEC* awardedAchievementKeys = AwardedAchievementKeys;
                awardedAchievementKeys->AddChildBlock(readWideString);
            }
        } else if (GlobalsV::LoadedSaveVersion >= 99) {
            for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, 82); cpp_range_12.next(AchievementIndex); ) {
                if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                    AwardedAchievementKeys->AddChildBlock(static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[AchievementIndex].Key));
                }
            }
        } else if (GlobalsV::LoadedSaveVersion >= 55) {
            for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, 61); cpp_range_13.next(AchievementIndex); ) {
                if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                    AwardedAchievementKeys->AddChildBlock(static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[AchievementIndex].Key));
                }
            }
        }
        if (Achievements::GetAvailableAchievementCount() > 0) {
            for (auto cpp_range_14 = pas::for_to<std::int32_t>(1, 82); cpp_range_14.next(AchievementIndex); ) {
                Data = Achievements::GetAchievementData(static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[AchievementIndex].Key));
                if (Data != nullptr) {
                    if (Data->Achieved) {
                        if (AwardedAchievementKeys->CountBlocks(static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[AchievementIndex].Key)) <= 0) {
                            AwardedAchievementKeys->AddChildBlock(static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[AchievementIndex].Key));
                        }
                    }
                    if (!Data->Achieved) {
                        if (AwardedAchievementKeys->CountBlocks(static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[AchievementIndex].Key)) > 0) {
                            AwardedAchievementKeys->DeleteChildBlock(static_cast<pas::WideString>(Achievements::AchievementDefinitionTable[AchievementIndex].Key));
                        }
                    }
                    Achievements::FreeAchievementData(Data);
                }
            }
        }
        Globals::LastLoadedPlayerName = Name;
        RefreshPlayerQuestTargets();
    }

    void TPlayer_ResolveLoadedReferences(TPlayer* Self, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        std::uint8_t Found{};
        aItem::TItem* Item{};
        PStorageEntry Entry{};
        aRanger::TRanger_ResolveLoadedReferences(Self, Galaxy);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(Self->StorageEntries, I);
            if ((static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Entry->LocationOwner)) & 0x80000000u) != 0) {
                Entry->LocationOwner = static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Entry->LocationOwner)) & 0x7fffffff, true));
            } else {
                Entry->LocationOwner = static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Entry->LocationOwner)), true));
            }
            Entry->Item->ResolveLoadedReferences(Galaxy);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Satellites) - 1); cpp_range_2.next(I); ) {
            pas::list_at<aItem::TSatellite>(Self->Satellites, I)->ResolveLoadedReferences(Galaxy);
        }
        I = 0;
        while (pas::list_count(Self->PiratePartners) > I) {
            pas::list_put(Self->PiratePartners, I, Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Self->PiratePartners, I))), false));
            if (pas::list_get(Self->PiratePartners, I) != nullptr) {
                ++I;
            } else {
                pas::list_delete(Self->PiratePartners, I);
            }
        }
        if (Self->QueuedTravelTarget != nullptr) {
            Self->QueuedTravelTarget = Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->QueuedTravelTarget)));
        }
        if (Self->RuinsMode > 0) {
            Self->RuinsProxy->CurrentStar = Self->CurrentStar;
        }
        if (Self->RuinsSavedPlanet != nullptr) {
            Self->RuinsSavedPlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->RuinsSavedPlanet)), true)));
        }
        if (Self->RuinsSavedDockedTo != nullptr) {
            Self->RuinsSavedDockedTo = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->RuinsSavedDockedTo)), true)));
        }
        if (GlobalsV::LoadedSaveVersion < 146 && Self->CurrentPlanet == nullptr && Self->DockedTo == nullptr) {
            Found = false;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(Self->Inventory) - 1); cpp_range_3.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Self->Inventory, I);
                if (pas::class_cast_if<aItem::TEngine*>(Item) != nullptr && reinterpret_cast<aItem::TEngine*>(Item)->TechLevel <= 7) {
                    Found = true;
                    break;
                }
            }
            if (!Found) {
                if (Self->GetEngine() != nullptr) {
                    Self->UnequipItem(Self->GetEngine());
                }
                Self->CreateAndEquipEngine(aConst::EngineBaseSize, 3, Self->OwnerId);
            }
            Found = false;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(Self->Inventory) - 1); cpp_range_4.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Self->Inventory, I);
                if (pas::class_cast_if<aItem::TFuelTanks*>(Item) != nullptr && reinterpret_cast<aItem::TFuelTanks*>(Item)->TechLevel <= 7) {
                    Found = true;
                    break;
                }
            }
            if (!Found) {
                if (Self->GetFuelTanks() != nullptr) {
                    Self->UnequipItem(Self->GetFuelTanks());
                }
                Self->CreateAndEquipFuelTanks(aConst::FuelTanksBaseSize, 3, Self->OwnerId);
            }
        }
    }

    void TPlayer::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        std::uint8_t I{};
        {
            const pas::WideString& int64ToStr = pas::wide_int64_to_str(static_cast<std::int64_t>(CurrentStar->Id));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"InChukriSotoanriIndo"_w);
            Block->AddParam(decodeTextW, int64ToStr);
        }
        aNormalShip::TNormalShip::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(DebtAmount);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"D5eyb7tn"_w);
            Block->AddParam(decodeTextW_2, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(DebtDueTurn);
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"DDe3bgt5Dha6t7ej"_w);
            Block->AddParam(decodeTextW_3, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(DebtDefaultCount);
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"Dbe5bht6C7njt8"_w);
            Block->AddParam(decodeTextW_4, intToStr_3);
        }
        {
            const pas::WideString& intToStr_4 = pas::wide_int_to_str(DepositAmount);
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"D0ehp7ojsgi4td"_w);
            Block->AddParam(decodeTextW_5, intToStr_4);
        }
        {
            const pas::WideString& intToStr_5 = pas::wide_int_to_str(DepositStartTurn);
            const pas::WideString& decodeTextW_6 = EC_Str::DecodeTextW(u"Dbe5p7ojsriet4Dga6t7ek"_w);
            Block->AddParam(decodeTextW_6, intToStr_5);
        }
        {
            const pas::WideString& intToStr_6 = pas::wide_int_to_str(DepositDayCount);
            const pas::WideString& decodeTextW_7 = EC_Str::DecodeTextW(u"D0ebp5o3sfi3t5Dha7y8"_w);
            Block->AddParam(decodeTextW_7, intToStr_6);
        }
        {
            const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(DepositInterestRate), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_8 = EC_Str::DecodeTextW(u"Dpeupto5seiwtfPye6rucieon9t"_w);
            Block->AddParam(decodeTextW_8, cpp_arg);
        }
        {
            const pas::WideString& intToStr_7 = pas::wide_int_to_str(MedicalPolicyTicks);
            const pas::WideString& decodeTextW_9 = EC_Str::DecodeTextW(u"Mmejd6Ptoel4i6c7yi"_w);
            Block->AddParam(decodeTextW_9, intToStr_7);
        }
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(11)); cpp_range.next(I); ) {
            Block->AddParam(aConst::ProgramNames[I], pas::wide_int_to_str(ProgramCounts[I]));
        }
        {
            const pas::WideString& intToStr_8 = pas::wide_int_to_str(ExperienceByDominators);
            const pas::WideString& decodeTextW_10 = EC_Str::DecodeTextW(u"Emxjp7D8o5m"_w);
            Block->AddParam(decodeTextW_10, intToStr_8);
        }
        {
            const pas::WideString& intToStr_9 = pas::wide_int_to_str(ExperienceByPirates);
            const pas::WideString& decodeTextW_11 = EC_Str::DecodeTextW(u"E3xrp5P6i7r"_w);
            Block->AddParam(decodeTextW_11, intToStr_9);
        }
        {
            const pas::WideString& intToStr_10 = pas::wide_int_to_str(ExperienceByNormals);
            const pas::WideString& decodeTextW_12 = EC_Str::DecodeTextW(u"Emx8p7C4oga6"_w);
            Block->AddParam(decodeTextW_12, intToStr_10);
        }
        {
            const pas::WideString& intToStr_11 = pas::wide_int_to_str(ExperienceByTraderCareer);
            const pas::WideString& decodeTextW_13 = EC_Str::DecodeTextW(u"Ekx7peTwr3af"_w);
            Block->AddParam(decodeTextW_13, intToStr_11);
        }
    }

    void TPlayer::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        std::uint8_t I{};
        aNormalShip::TNormalShip::LoadFromBlock(Block);
        DebtAmount = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"D5eyb7tn"_w))));
        DebtDueTurn = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"DDe3bgt5Dha6t7ej"_w))));
        DebtDefaultCount = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Dbe5bht6C7njt8"_w))));
        DepositAmount = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"D0ehp7ojsgi4td"_w))));
        DepositStartTurn = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Dbe5p7ojsriet4Dga6t7ek"_w))));
        DepositDayCount = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"D0ebp5o3sfi3t5Dha7y8"_w))));
        DepositInterestRate = EC_Str::ExtractDecimalToSingleW(Block->GetParam(EC_Str::DecodeTextW(u"Dpeupto5seiwtfPye6rucieon9t"_w)));
        MedicalPolicyTicks = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Mmejd6Ptoel4i6c7yi"_w))));
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(11)); cpp_range.next(I); ) {
            ProgramCounts[I] = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(aConst::ProgramNames[I])));
        }
        ExperienceByDominators = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Emxjp7D8o5m"_w))));
        ExperienceByPirates = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"E3xrp5P6i7r"_w))));
        ExperienceByNormals = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Emx8p7C4oga6"_w))));
        ExperienceByTraderCareer = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Ekx7peTwr3af"_w))));
    }

    // Inherited ranger registration followed by player career/skill defaults; CharacterPreset is unused here.
    void TPlayer::InitializePlayerAtPlanet(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::int32_t CharacterPreset) {
        aRanger::TRanger::InitializeAtPlanet(Planet, InitialMoney);
        BaseNodes = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(BaseNodes) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].ArcadeRewardScale);
        PreferredCareer = aGalaxyStruct::rcTrader;
        CareerStatus[aGalaxyStruct::rcTrader] = 0;
        CareerStatus[aGalaxyStruct::rcWarrior] = 0;
        CareerStatus[aGalaxyStruct::rcPirate] = 0;
        EminentProgress[aGalaxyStruct::rcTrader] = 0;
        EminentProgress[aGalaxyStruct::rcPirate] = 0;
        EminentProgress[aGalaxyStruct::rcWarrior] = 0;
        BaseSkills[0] = 0;
        BaseSkills[1] = 0;
        BaseSkills[2] = 0;
        BaseSkills[3] = 0;
        BaseSkills[4] = 0;
        BaseSkills[5] = 0;
    }

    // Twenty-five race/preset loadouts, stored cargo and initial planet relations. Planet is unused.
    void TPlayer::ApplyCharacterPreset(aPlanet::TPlanet* Planet, std::int32_t InitialMoney, std::int32_t CharacterPreset) {
        std::int32_t I{};
        std::int32_t Quantity{};
        std::uint8_t Kind{};
        pas::Object* Item{};
        PStorageEntry Entry{};
        {
            const std::int32_t cpp_first = pas::list_count(Inventory) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Item = pas::list_at<pas::Object>(Inventory, I);
                    pas::list_delete(Inventory, I);
                    pas::free(Item);
                }
            }
        }
        for (Kind = static_cast<std::uint8_t>(42); Kind <= static_cast<std::uint8_t>(49); ++Kind) {
            pas::store_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<aShip::PShipEquipmentCacheView>(this)->Slots, (Kind - 42) * sizeof(aItem::TEquipment*)), nullptr);
        }
        for (I = 1; I <= 5; ++I) {
            Weapons[I] = nullptr;
        }
        WeaponCount = 0;
        switch (OwnerId * 5 + CharacterPreset) {
            case 1: {
                {
                    std::int32_t roundAndTruncateToTens = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.2, 0.3)) * InitialMoney);
                    aShip::TShip* self = this;
                    self->SetMoney(roundAndTruncateToTens);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 20, pas::constant_set<aGalaxyStruct::TOwnerMask>({{3}, {4}}));
                {
                    std::uint8_t ownerId = OwnerId;
                    std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(250, 270, RandomState);
                    aShip::TShip* self_2 = this;
                    aShip::TShip_CreateAndEquipHull(self_2, nextRandomIntRange, 2, ownerId, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[1]), 3, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                break;
            }
            case 2: {
                {
                    std::int32_t roundAndTruncateToTens_2 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.5, 0.9)) * InitialMoney);
                    aShip::TShip* self_3 = this;
                    self_3->SetMoney(roundAndTruncateToTens_2);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 1, pas::constant_set<aGalaxyStruct::TOwnerMask>({{3}, {4}}));
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmIncrease, 40, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}, {2}}));
                {
                    std::uint8_t ownerId_2 = OwnerId;
                    std::uint16_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(240, 270, RandomState);
                    aShip::TShip* self_4 = this;
                    aShip::TShip_CreateAndEquipHull(self_4, nextRandomIntRange_2, 1, ownerId_2, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[1]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[2]), 3, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon3, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon3].AverageSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                break;
            }
            case 3: {
                {
                    std::int32_t roundAndTruncateToTens_3 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 1.2, 1.4)) * InitialMoney);
                    aShip::TShip* self_5 = this;
                    self_5->SetMoney(roundAndTruncateToTens_3);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmRaiseTo, 70, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {1}, {2}, {3}, {4}}));
                {
                    std::uint8_t ownerId_3 = OwnerId;
                    std::uint16_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(290, 320, RandomState);
                    aShip::TShip* self_6 = this;
                    aShip::TShip_CreateAndEquipHull(self_6, nextRandomIntRange_3, 1, ownerId_3, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                {
                    auto& cpp_target = GetFuelTanks()->ConditionPercent;
                    cpp_target = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                {
                    auto& cpp_target_2 = GetEngine()->ConditionPercent;
                    cpp_target_2 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[1]), 1, OwnerId);
                {
                    auto& cpp_target_3 = GetRadar()->ConditionPercent;
                    cpp_target_3 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[1]), 2, OwnerId);
                {
                    auto& cpp_target_4 = GetCargoHook()->ConditionPercent;
                    cpp_target_4 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                {
                    auto& cpp_target_5 = CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId)->ConditionPercent;
                    cpp_target_5 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                break;
            }
            case 4: {
                {
                    std::int32_t roundAndTruncateToTens_4 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.9, 1.1)) * InitialMoney);
                    aShip::TShip* self_7 = this;
                    self_7->SetMoney(roundAndTruncateToTens_4);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 5, pas::constant_set<aGalaxyStruct::TOwnerMask>({{3}, {4}}));
                {
                    std::uint8_t ownerId_4 = OwnerId;
                    std::uint16_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(230, 250, RandomState);
                    aShip::TShip* self_8 = this;
                    aShip::TShip_CreateAndEquipHull(self_8, nextRandomIntRange_4, 2, ownerId_4, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon3, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon3].AverageSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                break;
            }
            case 5: {
                {
                    std::int32_t roundAndTruncateToTens_5 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 1.9, 2.1)) * InitialMoney);
                    aShip::TShip* self_9 = this;
                    self_9->SetMoney(roundAndTruncateToTens_5);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 5, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}, {3}}));
                {
                    std::uint8_t ownerId_5 = OwnerId;
                    std::uint16_t nextRandomIntRange_5 = aMyFunction::NextRandomIntRange(210, 230, RandomState);
                    aShip::TShip* self_10 = this;
                    aShip::TShip_CreateAndEquipHull(self_10, nextRandomIntRange_5, 1, ownerId_5, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[4]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 3, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                break;
            }
            case 6: {
                {
                    std::int32_t roundAndTruncateToTens_6 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 2.3, 2.5)) * InitialMoney);
                    aShip::TShip* self_11 = this;
                    self_11->SetMoney(roundAndTruncateToTens_6);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 25, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {3}}));
                {
                    std::uint8_t ownerId_6 = OwnerId;
                    std::uint16_t nextRandomIntRange_6 = aMyFunction::NextRandomIntRange(210, 230, RandomState);
                    aShip::TShip* self_12 = this;
                    aShip::TShip_CreateAndEquipHull(self_12, nextRandomIntRange_6, 1, ownerId_6, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[4]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 3, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                break;
            }
            case 7: {
                {
                    std::int32_t roundAndTruncateToTens_7 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 1.4, 2.0)) * InitialMoney);
                    aShip::TShip* self_13 = this;
                    self_13->SetMoney(roundAndTruncateToTens_7);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 30, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {2}, {3}, {4}}));
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 60, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}}));
                {
                    std::uint8_t ownerId_7 = OwnerId;
                    std::uint16_t nextRandomIntRange_7 = aMyFunction::NextRandomIntRange(230, 260, RandomState);
                    aShip::TShip* self_14 = this;
                    aShip::TShip_CreateAndEquipHull(self_14, nextRandomIntRange_7, 1, ownerId_7, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 2, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                break;
            }
            case 8: {
                {
                    std::int32_t roundAndTruncateToTens_8 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.9, 1.1)) * InitialMoney);
                    aShip::TShip* self_15 = this;
                    self_15->SetMoney(roundAndTruncateToTens_8);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmRaiseTo, 50, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}, {2}, {3}, {4}}));
                {
                    std::uint8_t ownerId_8 = OwnerId;
                    std::uint16_t nextRandomIntRange_8 = aMyFunction::NextRandomIntRange(280, 320, RandomState);
                    aShip::TShip* self_16 = this;
                    aShip::TShip_CreateAndEquipHull(self_16, nextRandomIntRange_8, 1, ownerId_8, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                {
                    auto& cpp_target_6 = GetFuelTanks()->ConditionPercent;
                    cpp_target_6 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                {
                    auto& cpp_target_7 = GetEngine()->ConditionPercent;
                    cpp_target_7 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                {
                    auto& cpp_target_8 = GetRadar()->ConditionPercent;
                    cpp_target_8 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[1]), 2, OwnerId);
                {
                    auto& cpp_target_9 = GetCargoHook()->ConditionPercent;
                    cpp_target_9 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                {
                    auto& cpp_target_10 = CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[2]), 3, OwnerId)->ConditionPercent;
                    cpp_target_10 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                break;
            }
            case 9: {
                {
                    std::int32_t roundAndTruncateToTens_9 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 1.9, 2.0)) * InitialMoney);
                    aShip::TShip* self_17 = this;
                    self_17->SetMoney(roundAndTruncateToTens_9);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 30, pas::constant_set<aGalaxyStruct::TOwnerMask>({{2}, {3}}));
                {
                    std::uint8_t ownerId_9 = OwnerId;
                    std::uint16_t nextRandomIntRange_9 = aMyFunction::NextRandomIntRange(250, 270, RandomState);
                    aShip::TShip* self_18 = this;
                    aShip::TShip_CreateAndEquipHull(self_18, nextRandomIntRange_9, 1, ownerId_9, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 2, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                pas::new_value(Entry);
                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                Entry->Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Quantity = aMyFunction::NextRandomIntRange(7, 17, RandomState);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Init(aConst::t_Luxury, Quantity);
                Entry->Item->Cost = Quantity * (aConst::GoodsMarket[3].AveragePrice / 4);
                if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                    Entry->LocationOwner = aPlayer::GetPlayer()->DockedTo;
                } else {
                    Entry->LocationOwner = aPlayer::GetPlayer()->CurrentPlanet;
                }
                Entry->SlotIndex = 0;
                break;
            }
            case 10: {
                {
                    std::int32_t roundAndTruncateToTens_10 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.9, 1.1)) * InitialMoney);
                    aShip::TShip* self_19 = this;
                    self_19->SetMoney(roundAndTruncateToTens_10);
                }
                {
                    std::uint8_t nextRandomIntRange_10 = aMyFunction::NextRandomIntRange(10, 35, RandomState);
                    aRanger::TRanger* self_20 = this;
                    aRanger::TRanger_ChangePlanetRelations(self_20, nullptr, aRanger::rcmCapAt, nextRandomIntRange_10, pas::constant_set<aGalaxyStruct::TOwnerMask>({{2}}));
                }
                {
                    std::uint8_t nextRandomIntRange_11 = aMyFunction::NextRandomIntRange(10, 35, RandomState);
                    aRanger::TRanger* self_21 = this;
                    aRanger::TRanger_ChangePlanetRelations(self_21, nullptr, aRanger::rcmCapAt, nextRandomIntRange_11, pas::constant_set<aGalaxyStruct::TOwnerMask>({{3}}));
                }
                {
                    std::uint8_t nextRandomIntRange_12 = aMyFunction::NextRandomIntRange(10, 35, RandomState);
                    aRanger::TRanger* self_22 = this;
                    aRanger::TRanger_ChangePlanetRelations(self_22, nullptr, aRanger::rcmCapAt, nextRandomIntRange_12, pas::constant_set<aGalaxyStruct::TOwnerMask>({{4}}));
                }
                {
                    std::uint8_t ownerId_10 = OwnerId;
                    std::uint16_t nextRandomIntRange_13 = aMyFunction::NextRandomIntRange(250, 270, RandomState);
                    aShip::TShip* self_23 = this;
                    aShip::TShip_CreateAndEquipHull(self_23, nextRandomIntRange_13, 1, ownerId_10, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                {
                    auto& cpp_target_11 = GetFuelTanks()->ConditionPercent;
                    cpp_target_11 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                {
                    auto& cpp_target_12 = GetEngine()->ConditionPercent;
                    cpp_target_12 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                {
                    auto& cpp_target_13 = GetRadar()->ConditionPercent;
                    cpp_target_13 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[1]), 2, OwnerId);
                {
                    auto& cpp_target_14 = GetCargoHook()->ConditionPercent;
                    cpp_target_14 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                {
                    auto& cpp_target_15 = CreateAndEquipWeapon(aConst::t_Weapon4, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon4].AverageSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId)->ConditionPercent;
                    cpp_target_15 = aMyFunction::NextRandomIntRange(60, 90, RandomState);
                }
                pas::new_value(Entry);
                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                Entry->Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Quantity = aMyFunction::NextRandomIntRange(4, 10, RandomState);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Init(aConst::t_Narcotics, Quantity);
                Entry->Item->Cost = Quantity * (aConst::GoodsMarket[7].AveragePrice / 2);
                if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                    Entry->LocationOwner = aPlayer::GetPlayer()->DockedTo;
                } else {
                    Entry->LocationOwner = aPlayer::GetPlayer()->CurrentPlanet;
                }
                Entry->SlotIndex = 0;
                break;
            }
            case 11: {
                {
                    std::int32_t roundAndTruncateToTens_11 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.3, 0.5)) * InitialMoney);
                    aShip::TShip* self_24 = this;
                    self_24->SetMoney(roundAndTruncateToTens_11);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 15, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}}));
                {
                    std::uint8_t ownerId_11 = OwnerId;
                    std::uint16_t nextRandomIntRange_14 = aMyFunction::NextRandomIntRange(210, 230, RandomState);
                    aShip::TShip* self_25 = this;
                    aShip::TShip_CreateAndEquipHull(self_25, nextRandomIntRange_14, 2, ownerId_11, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipDefGenerator(System::Round(static_cast<long double>(aConst::DefGeneratorBaseSize) * aConst::EquipmentSizeFactors[2]), 3, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                break;
            }
            case 12: {
                {
                    std::int32_t roundAndTruncateToTens_12 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 1.5, 2.0)) * InitialMoney);
                    aShip::TShip* self_26 = this;
                    self_26->SetMoney(roundAndTruncateToTens_12);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmRaiseTo, 90, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {1}}));
                {
                    std::uint8_t ownerId_12 = OwnerId;
                    std::uint16_t nextRandomIntRange_15 = aMyFunction::NextRandomIntRange(250, 270, RandomState);
                    aShip::TShip* self_27 = this;
                    aShip::TShip_CreateAndEquipHull(self_27, nextRandomIntRange_15, 2, ownerId_12, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                {
                    auto& cpp_target_16 = GetFuelTanks()->ConditionPercent;
                    cpp_target_16 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                {
                    auto& cpp_target_17 = GetEngine()->ConditionPercent;
                    cpp_target_17 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[1]), 1, OwnerId);
                {
                    auto& cpp_target_18 = GetRadar()->ConditionPercent;
                    cpp_target_18 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                {
                    auto& cpp_target_19 = GetCargoHook()->ConditionPercent;
                    cpp_target_19 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                {
                    auto& cpp_target_20 = CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId)->ConditionPercent;
                    cpp_target_20 = aMyFunction::NextRandomIntRange(20, 80, RandomState);
                }
                pas::new_value(Entry);
                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                Entry->Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Quantity = aMyFunction::NextRandomIntRange(100, 200, RandomState);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Init(aConst::t_Minerals, Quantity);
                Entry->Item->Cost = Quantity * (aConst::GoodsMarket[4].AveragePrice / 2);
                if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                    Entry->LocationOwner = aPlayer::GetPlayer()->DockedTo;
                } else {
                    Entry->LocationOwner = aPlayer::GetPlayer()->CurrentPlanet;
                }
                Entry->SlotIndex = 0;
                break;
            }
            case 13: {
                {
                    std::int32_t roundAndTruncateToTens_13 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 1.3, 1.5)) * InitialMoney);
                    aShip::TShip* self_28 = this;
                    self_28->SetMoney(roundAndTruncateToTens_13);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 25, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}}));
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmIncrease, 30, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}, {3}, {4}}));
                {
                    std::uint8_t ownerId_13 = OwnerId;
                    std::uint16_t nextRandomIntRange_16 = aMyFunction::NextRandomIntRange(280, 310, RandomState);
                    aShip::TShip* self_29 = this;
                    aShip::TShip_CreateAndEquipHull(self_29, nextRandomIntRange_16, 1, ownerId_13, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipScanner(System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                break;
            }
            case 14: {
                {
                    std::int32_t roundAndTruncateToTens_14 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 1.1, 1.3)) * InitialMoney);
                    aShip::TShip* self_30 = this;
                    self_30->SetMoney(roundAndTruncateToTens_14);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 5, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}, {4}}));
                {
                    std::uint8_t ownerId_14 = OwnerId;
                    std::uint16_t nextRandomIntRange_17 = aMyFunction::NextRandomIntRange(240, 260, RandomState);
                    aShip::TShip* self_31 = this;
                    aShip::TShip_CreateAndEquipHull(self_31, nextRandomIntRange_17, 1, ownerId_14, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[3]), 3, OwnerId);
                CreateAndEquipScanner(System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[4]), 2, OwnerId);
                break;
            }
            case 15: {
                {
                    std::int32_t roundAndTruncateToTens_15 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.2, 0.3)) * InitialMoney);
                    aShip::TShip* self_32 = this;
                    self_32->SetMoney(roundAndTruncateToTens_15);
                }
                {
                    std::uint8_t nextRandomIntRange_18 = aMyFunction::NextRandomIntRange(10, 35, RandomState);
                    aRanger::TRanger* self_33 = this;
                    aRanger::TRanger_ChangePlanetRelations(self_33, nullptr, aRanger::rcmCapAt, nextRandomIntRange_18, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}}));
                }
                {
                    std::uint8_t nextRandomIntRange_19 = aMyFunction::NextRandomIntRange(10, 35, RandomState);
                    aRanger::TRanger* self_34 = this;
                    aRanger::TRanger_ChangePlanetRelations(self_34, nullptr, aRanger::rcmCapAt, nextRandomIntRange_19, pas::constant_set<aGalaxyStruct::TOwnerMask>({{3}}));
                }
                {
                    std::uint8_t nextRandomIntRange_20 = aMyFunction::NextRandomIntRange(10, 35, RandomState);
                    aRanger::TRanger* self_35 = this;
                    aRanger::TRanger_ChangePlanetRelations(self_35, nullptr, aRanger::rcmCapAt, nextRandomIntRange_20, pas::constant_set<aGalaxyStruct::TOwnerMask>({{4}}));
                }
                {
                    std::uint8_t ownerId_15 = OwnerId;
                    std::uint16_t nextRandomIntRange_21 = aMyFunction::NextRandomIntRange(250, 270, RandomState);
                    aShip::TShip* self_36 = this;
                    aShip::TShip_CreateAndEquipHull(self_36, nextRandomIntRange_21, 1, ownerId_15, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[4]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 3, OwnerId);
                break;
            }
            case 16: {
                {
                    std::int32_t roundAndTruncateToTens_16 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.2, 0.3)) * InitialMoney);
                    aShip::TShip* self_37 = this;
                    self_37->SetMoney(roundAndTruncateToTens_16);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmRaiseTo, 70, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {1}, {2}, {3}, {4}}));
                {
                    std::uint8_t ownerId_16 = OwnerId;
                    std::uint16_t nextRandomIntRange_22 = aMyFunction::NextRandomIntRange(250, 270, RandomState);
                    aShip::TShip* self_38 = this;
                    aShip::TShip_CreateAndEquipHull(self_38, nextRandomIntRange_22, 1, ownerId_16, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 3, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon3, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon3].AverageSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                break;
            }
            case 17: {
                {
                    std::int32_t roundAndTruncateToTens_17 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.2, 0.3)) * InitialMoney);
                    aShip::TShip* self_39 = this;
                    self_39->SetMoney(roundAndTruncateToTens_17);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 70, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {1}}));
                {
                    std::uint8_t ownerId_17 = OwnerId;
                    std::uint16_t nextRandomIntRange_23 = aMyFunction::NextRandomIntRange(230, 250, RandomState);
                    aShip::TShip* self_40 = this;
                    aShip::TShip_CreateAndEquipHull(self_40, nextRandomIntRange_23, 1, ownerId_17, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon4, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon4].AverageSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                break;
            }
            case 18: {
                {
                    std::int32_t roundAndTruncateToTens_18 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.8, 1.3)) * InitialMoney);
                    aShip::TShip* self_41 = this;
                    self_41->SetMoney(roundAndTruncateToTens_18);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmRaiseTo, 70, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}, {2}, {3}, {4}}));
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 25, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}}));
                ([&] {
                    std::uint8_t ownerId_18 = OwnerId;
                    std::uint16_t nextRandomIntRange_24 = aMyFunction::NextRandomIntRange(290, 320, RandomState);
                    aShip::TShip* self_42 = this;
                    return aShip::TShip_CreateAndEquipHull(self_42, nextRandomIntRange_24, 1, ownerId_18, -1, false);
                }())->HullPoints = aMyFunction::NextRandomIntRange(50, 150, RandomState);
                {
                    auto& cpp_target_21 = CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId)->ConditionPercent;
                    cpp_target_21 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                {
                    auto& cpp_target_22 = CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId)->ConditionPercent;
                    cpp_target_22 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                {
                    auto& cpp_target_23 = CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[1]), 1, OwnerId)->ConditionPercent;
                    cpp_target_23 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                {
                    auto& cpp_target_24 = CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId)->ConditionPercent;
                    cpp_target_24 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                {
                    auto& cpp_target_25 = CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[4]), 1, OwnerId)->ConditionPercent;
                    cpp_target_25 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                break;
            }
            case 19: {
                {
                    std::int32_t roundAndTruncateToTens_19 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.2, 0.3)) * InitialMoney);
                    aShip::TShip* self_43 = this;
                    self_43->SetMoney(roundAndTruncateToTens_19);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 15, pas::constant_set<aGalaxyStruct::TOwnerMask>({{4}}));
                {
                    std::uint8_t ownerId_19 = OwnerId;
                    std::uint16_t nextRandomIntRange_25 = aMyFunction::NextRandomIntRange(270, 290, RandomState);
                    aShip::TShip* self_44 = this;
                    aShip::TShip_CreateAndEquipHull(self_44, nextRandomIntRange_25, 1, ownerId_19, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 2, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[1]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[2]), 3, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon3, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon3].AverageSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                break;
            }
            case 20: {
                {
                    std::int32_t roundAndTruncateToTens_20 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.2, 0.3)) * InitialMoney);
                    aShip::TShip* self_45 = this;
                    self_45->SetMoney(roundAndTruncateToTens_20);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 5, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {2}, {4}}));
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 90, pas::constant_set<aGalaxyStruct::TOwnerMask>({{1}}));
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 60, pas::constant_set<aGalaxyStruct::TOwnerMask>({{3}}));
                {
                    std::uint8_t ownerId_20 = OwnerId;
                    std::uint16_t nextRandomIntRange_26 = aMyFunction::NextRandomIntRange(230, 250, RandomState);
                    aShip::TShip* self_46 = this;
                    aShip::TShip_CreateAndEquipHull(self_46, nextRandomIntRange_26, 1, ownerId_20, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                pas::new_value(Entry);
                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                Entry->Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Quantity = aMyFunction::NextRandomIntRange(14, 20, RandomState);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Init(aConst::t_Narcotics, Quantity);
                Entry->Item->Cost = Quantity * (aConst::GoodsMarket[7].AveragePrice / 2);
                if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                    Entry->LocationOwner = aPlayer::GetPlayer()->DockedTo;
                } else {
                    Entry->LocationOwner = aPlayer::GetPlayer()->CurrentPlanet;
                }
                Entry->SlotIndex = 0;
                break;
            }
            case 21: {
                {
                    std::int32_t roundAndTruncateToTens_21 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.2, 0.3)) * InitialMoney);
                    aShip::TShip* self_47 = this;
                    self_47->SetMoney(roundAndTruncateToTens_21);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmRaiseTo, 70, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {1}, {2}, {3}, {4}}));
                {
                    std::uint8_t ownerId_21 = OwnerId;
                    std::uint16_t nextRandomIntRange_27 = aMyFunction::NextRandomIntRange(230, 250, RandomState);
                    aShip::TShip* self_48 = this;
                    aShip::TShip_CreateAndEquipHull(self_48, nextRandomIntRange_27, 2, ownerId_21, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 3, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon2, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon2].AverageSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                break;
            }
            case 22: {
                {
                    std::int32_t roundAndTruncateToTens_22 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 1.2, 1.3)) * InitialMoney);
                    aShip::TShip* self_49 = this;
                    self_49->SetMoney(roundAndTruncateToTens_22);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmRaiseTo, 60, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {1}, {2}, {3}, {4}}));
                {
                    std::uint8_t ownerId_22 = OwnerId;
                    std::uint16_t nextRandomIntRange_28 = aMyFunction::NextRandomIntRange(220, 230, RandomState);
                    aShip::TShip* self_50 = this;
                    aShip::TShip_CreateAndEquipHull(self_50, nextRandomIntRange_28, 1, ownerId_22, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipScanner(System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[4]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                break;
            }
            case 23: {
                {
                    std::int32_t roundAndTruncateToTens_23 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.8, 1.2)) * InitialMoney);
                    aShip::TShip* self_51 = this;
                    self_51->SetMoney(roundAndTruncateToTens_23);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 5, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}}));
                {
                    std::uint8_t ownerId_23 = OwnerId;
                    std::uint16_t nextRandomIntRange_29 = aMyFunction::NextRandomIntRange(280, 310, RandomState);
                    aShip::TShip* self_52 = this;
                    aShip::TShip_CreateAndEquipHull(self_52, nextRandomIntRange_29, 1, ownerId_23, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipScanner(System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                pas::new_value(Entry);
                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                Entry->Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Quantity = aMyFunction::NextRandomIntRange(15, 30, RandomState);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Init(aConst::t_Luxury, Quantity);
                Entry->Item->Cost = Quantity * (aConst::GoodsMarket[3].AveragePrice / 2);
                if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                    Entry->LocationOwner = aPlayer::GetPlayer()->DockedTo;
                } else {
                    Entry->LocationOwner = aPlayer::GetPlayer()->CurrentPlanet;
                }
                Entry->SlotIndex = 0;
                break;
            }
            case 24: {
                {
                    std::int32_t roundAndTruncateToTens_24 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.9, 1.2)) * InitialMoney);
                    aShip::TShip* self_53 = this;
                    self_53->SetMoney(roundAndTruncateToTens_24);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 35, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {1}, {2}, {3}}));
                {
                    std::uint8_t ownerId_24 = OwnerId;
                    std::uint16_t nextRandomIntRange_30 = aMyFunction::NextRandomIntRange(250, 260, RandomState);
                    aShip::TShip* self_54 = this;
                    aShip::TShip_CreateAndEquipHull(self_54, nextRandomIntRange_30, 1, ownerId_24, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 1, OwnerId);
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipScanner(System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                CreateAndEquipRepairRobot(System::Round(static_cast<long double>(aConst::RepairRobotBaseSize) * aConst::EquipmentSizeFactors[3]), 2, OwnerId);
                CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                pas::new_value(Entry);
                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                Entry->Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Quantity = aMyFunction::NextRandomIntRange(10, 20, RandomState);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Init(aConst::t_Alcohol, Quantity);
                Entry->Item->Cost = Quantity * (aConst::GoodsMarket[5].AveragePrice / 2);
                if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                    Entry->LocationOwner = aPlayer::GetPlayer()->DockedTo;
                } else {
                    Entry->LocationOwner = aPlayer::GetPlayer()->CurrentPlanet;
                }
                Entry->SlotIndex = 0;
                break;
            }
            case 25: {
                {
                    std::int32_t roundAndTruncateToTens_25 = aMyFunction::RoundAndTruncateToTens(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->GenerationSeed, 0.9, 1.2)) * InitialMoney);
                    aShip::TShip* self_55 = this;
                    self_55->SetMoney(roundAndTruncateToTens_25);
                }
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, aRanger::rcmCapAt, 15, pas::constant_set<aGalaxyStruct::TOwnerMask>({{0}, {1}, {3}}));
                {
                    std::uint8_t ownerId_25 = OwnerId;
                    std::uint16_t nextRandomIntRange_31 = aMyFunction::NextRandomIntRange(280, 300, RandomState);
                    aShip::TShip* self_56 = this;
                    aShip::TShip_CreateAndEquipHull(self_56, nextRandomIntRange_31, 1, ownerId_25, -1, false);
                }
                CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[3]), 1, OwnerId);
                {
                    auto& cpp_target_26 = GetFuelTanks()->ConditionPercent;
                    cpp_target_26 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 2, OwnerId);
                {
                    auto& cpp_target_27 = GetEngine()->ConditionPercent;
                    cpp_target_27 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                CreateAndEquipRadar(System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                {
                    auto& cpp_target_28 = GetRadar()->ConditionPercent;
                    cpp_target_28 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                CreateAndEquipCargoHook(System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId);
                {
                    auto& cpp_target_29 = GetCargoHook()->ConditionPercent;
                    cpp_target_29 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                {
                    auto& cpp_target_30 = CreateAndEquipWeapon(aConst::t_Weapon1, System::Round(static_cast<long double>(aConst::WeaponInfos[aConst::t_Weapon1].AverageSize) * aConst::EquipmentSizeFactors[2]), 1, OwnerId)->ConditionPercent;
                    cpp_target_30 = aMyFunction::NextRandomIntRange(10, 50, RandomState);
                }
                break;
            }
        }
        GetHull()->Weight = aMyFunction::RoundAndTruncateToTens(pas::real_divide(static_cast<long double>(GetHull()->Weight) * aConst::HullCapacityScale, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestTimeAndExperienceFactor));
        RefreshDerivedStats(true);
        while (CargoFreeSpace < pas::real_divide(15.0L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor)) {
            GetHull()->Weight += 5;
            RefreshDerivedStats(true);
        }
        switch (aGalaxy::Galaxy->DifficultyLevels[7]) {
            case 0: GetHull()->Weight += 30; break;
            case 1: GetHull()->Weight += 10; break;
            case 2: GetHull()->Weight += 5; break;
        }
        GetHull()->HullPoints = GetHull()->Weight;
        RefreshDerivedStats(true);
        RefreshGraphicSize();
        RefreshAssignedItemSlots();
        HomePlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 100);
        for (I = 1; I <= 24; ++I) {
            StatusEffectSourceNames[I] = pas::WideString();
        }
    }

    void TPlayer_NextDay(TPlayer* Self) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t LastDisease{};
        std::int32_t FirstDisease{};
        std::int32_t TargetValue{};
        std::uint32_t LocationId{};
        std::int32_t Found{};
        aShip::TShip* Ship{};
        aGalaxy::TStar* Star{};
        aItem::TEquipment* Item{};
        pas::WideString Text{};
        float ResistanceFactor{};
        aScript::TScriptShip* Binding{};
        std::uint32_t LocalSeed{};
        std::int32_t StimulantExcess{};
        std::int32_t Stage = 0;
        try {
            TPlayer::BeginStorageTurn();
            if (aGalaxy::Galaxy->CurrentTurn <= Self->LastProcessedTurn && aGalaxy::Galaxy->StasisModEnabled != 1) {
                return;
            }
            aRanger::TRanger_NextDay(Self);
            Stage = 1;
            if (Self->ScriptShipBindings != nullptr) {
                I = pas::list_count(Self->ScriptShipBindings) - 1;
                while (I >= 0) {
                    if (pas::list_count(Self->ScriptShipBindings) <= I) {
                        I = pas::list_count(Self->ScriptShipBindings) - 1;
                    } else {
                        Binding = pas::list_at<aScript::TScriptShip>(Self->ScriptShipBindings, I);
                        if (Binding->Script != nullptr) {
                            aScript::TScript_RunShipState(Binding->Script, Binding);
                        }
                        --I;
                    }
                }
            }
            Stage = 2;
            if (pas::imod(aGalaxy::Galaxy->CurrentTurn, (static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn) / 1000 % 10 + 2) == 0 && Self->DiseaseImmunity > 0) {
                --Self->DiseaseImmunity;
            }
            // Native O- code retains this unreachable lower clamp on the byte field.
            if (Self->DiseaseImmunity < 0) {
                Self->DiseaseImmunity = 0;
            }
            if (Self->DiseaseImmunity > 100) {
                Self->DiseaseImmunity = 100;
            }
            if (!Self->InNormalSpace()) {
                Self->AchievementStats->StarFuelCollected = 0u;
            }
            if (Self->MedicalPolicyTicks > 0) {
                --Self->MedicalPolicyTicks;
                aConst::LastMedicalPolicyTicks = Self->MedicalPolicyTicks;
                if (Self->MedicalPolicyTicks == 0) {
                    const pas::WideString& pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.MedPolicy.End"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10);
                    std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                    Globals::AddOrUpdatePlayerBubble(0, currentTurn, pickLocalizedTextVariant, u""_wref.get());
                }
            }
            if (Self->PendingPirateLicenseCash > 0) {
                Self->GainExperience(System::Round(Self->PendingPirateLicenseCash * Self->CareerStatus[aGalaxyStruct::rcPirate] * 0.001L), 3);
                Self->PirateLicenseCash += Self->PendingPirateLicenseCash;
                Self->PendingPirateLicenseCash = 0;
                if (Self->PirateLicenseCash > 100000000) {
                    Self->PirateLicenseCash = 100000000;
                }
            }
            if (Self->PirateLicenseTicks > 0) {
                --Self->PirateLicenseTicks;
                if (Self->PirateLicenseTicks == 0) {
                    const pas::WideString& pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.PirateLicense.End"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10);
                    std::int32_t currentTurn_2 = aGalaxy::Galaxy->CurrentTurn;
                    Globals::AddOrUpdatePlayerBubble(0, currentTurn_2, pickLocalizedTextVariant_2, u""_wref.get());
                } else if (aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::rstDominion] <= 0) {
                    Found = 0;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
                        Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                            if (pas::list_at<aShip::TShip>(Star->Ships, J)->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion)) {
                                ++Found;
                                break;
                            }
                        }
                        if (Found > 0) {
                            break;
                        }
                    }
                    if (Found == 0) {
                        Self->PirateLicenseTicks = 0;
                        {
                            const pas::WideString& pickLocalizedTextVariant_3 = aConst::PickLocalizedTextVariant(u"GalaxyNews.PirateLicense.DeadAllCB"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10);
                            std::int32_t currentTurn_3 = aGalaxy::Galaxy->CurrentTurn;
                            Globals::AddOrUpdatePlayerBubble(0, currentTurn_3, pickLocalizedTextVariant_3, u""_wref.get());
                        }
                    }
                }
            }
            if (Self->PirateLicenseCash > 0 && Self->PirateLicenseTicks <= 0) {
                if (Self->PirateLicenseCash <= 1000) {
                    Self->PirateLicenseCash = 0;
                } else if (Self->PirateLicenseCash <= 20000) {
                    Self->PirateLicenseCash -= 1000;
                } else {
                    Self->PirateLicenseCash = System::Round(Self->PirateLicenseCash * 0.95L);
                }
            }
            if (Self->Money < 0) {
                Self->SetMoney(0);
            } else if (Self->Money > 100000000) {
                Self->SetMoney(100000000);
            }
            Stage = 3;
            if (Self->InNormalSpace()) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_3.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                    if (Ship->InNormalSpace()) {
                        Ship->DaysSincePlayerSeen = 0;
                    }
                }
            }
            Stage = 4;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Inventory) - 1); cpp_range_4.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Self->Inventory, I);
                if (Item->ItemType == aConst::t_Engine && Item->EquippedFlag == 0) {
                    if (pas::checked_cast<aItem::TEngine*>(Item)->OutputPercent + 10 > 100) {
                        pas::checked_cast<aItem::TEngine*>(Item)->OutputPercent = 100;
                    } else {
                        pas::checked_cast<aItem::TEngine*>(Item)->OutputPercent += 10;
                    }
                }
            }
            Stage = 5;
            TPlayer::RechargeTransmitters();
            Self->ApplyBioArtefactHealthEffects();
            aPlayer::TPlayer_RefreshNewsAtLocation(Self);
            Stage = 6;
            if (Self->CountActiveDiseases() < 3 && (Self->CurrentPlanet != nullptr && static_cast<std::uint8_t>(Self->HasDiseaseFromCurrentPlanet() ^ 1) || Self->DockedTo != nullptr && static_cast<std::uint8_t>(Self->HasDiseaseFromCurrentDockedShip() ^ 1) || Self->InNormalSpace())) {
                FirstDisease = 1;
                LastDisease = 12;
                I = aMyFunction::NextRandomIntRange(FirstDisease, LastDisease, Self->RandomState);
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(FirstDisease, LastDisease); cpp_range_5.next(J); ) {
                    aMyFunction::IncrementWrapped(I, FirstDisease, LastDisease);
                    {
                        aConst::TIllnessInfo& cpp_with = aConst::CaptainHealthDefinitions[I];
                        if (aGalaxy::Galaxy->CurrentTurn < 300 || aConst::CaptainHealthDefinitions[I].Disabled) {
                            continue;
                        }
                        if (Self->CurrentPlanet != nullptr && static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[I].Locations, 0) ^ 1)) {
                            continue;
                        }
                        if (Self->DockedTo != nullptr && static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[I].Locations, 1) ^ 1)) {
                            continue;
                        }
                        if (Self->InNormalSpace() && static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[I].Locations, 2) ^ 1) && static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[I].Locations, 3) ^ 1)) {
                            continue;
                        }
                        if (Self->InNormalSpace() && pas::contains(aConst::CaptainHealthDefinitions[I].Locations, 3)) {
                            if (Self->EnemyShip == nullptr || static_cast<std::uint8_t>(Self->EnemyShip->IsAttackingShip(Self) ^ 1) || Self->GetHullIntegrityPercent() > 50 || I == 3 && Self->EnemyShip->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                                continue;
                            }
                        }
                        if (Self->CurrentPlanet != nullptr && static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[I].AllowedLocationOwners, Self->CurrentPlanet->OwnerId) ^ 1)) {
                            if (Self->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) || static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[I].AllowedLocationOwners, aConst::RaceToOwner(Self->CurrentPlanet->RaceId)) ^ 1)) {
                                continue;
                            }
                        }
                        if (Self->DockedTo != nullptr && static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[I].AllowedLocationOwners, aConst::RaceToOwner(Self->DockedTo->PilotRace)) ^ 1) && static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[I].AllowedLocationOwners, Self->DockedTo->OwnerId) ^ 1)) {
                            continue;
                        }
                        if (pas::contains(cpp_with.AllowedOwners, aConst::RaceToOwner(Self->PilotRace)) && pas::contains(cpp_with.AllowedRatingBands, Self->GetRangerRatingBand()) && pas::contains(cpp_with.AllowedRanks, Self->Rank) && pas::contains(cpp_with.AllowedCareers, static_cast<std::uint8_t>(Self->GetDominantCareer())) && Self->CaptainHealth[I].Progress <= 0.0L && Self->CaptainHealth[I].ExpireTurn + 365 <= aGalaxy::Galaxy->CurrentTurn) {
                            if (Self->IsHealthEffectActive(4)) {
                                ResistanceFactor = 0.1f;
                            } else {
                                ResistanceFactor = 1.0f;
                            }
                            if (Self->IsHealthEffectActive(18)) {
                                ResistanceFactor = ResistanceFactor * 5.0L;
                            }
                            ResistanceFactor = static_cast<long double>(Self->CountActiveArtefacts(aConst::t_ArtBio) + 1) * ResistanceFactor;
                            if (Self->CurrentPlanet != nullptr) {
                                LocationId = Self->CurrentPlanet->Id;
                            } else if (Self->DockedTo != nullptr) {
                                LocationId = Self->DockedTo->Id;
                            } else {
                                LocationId = Self->CurrentStar->Id;
                            }
                            LocalSeed = aGalaxy::Galaxy->GenerationSeed + static_cast<std::uint32_t>(I) + LocationId + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 3);
                            if (([&] {
                                pas::Extended cpp_left_2 = aMyFunction::NextRandomFloatRange(0.0, 1.0, LocalSeed);
                                pas::Extended cpp_left = cpp_left_2 * aMyFunction::RemapClamped(Self->DiseaseImmunity, 0.0, 1.0E+2, 5.0E+1, 3.0E+2) * ResistanceFactor;
                                return cpp_left <= cpp_with.InfectionChance * 2.0L * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor;
                            }())) {
                                Self->CaptainHealth[I].Progress = 0.1;
                                if (Self->InNormalSpace() && pas::contains(aConst::CaptainHealthDefinitions[I].Locations, 3)) {
                                    Self->CaptainHealth[I].Progress = 99.99989999999999;
                                }
                                Self->CaptainHealth[I].AppliedTurn = aGalaxy::Galaxy->CurrentTurn;
                                Self->CaptainHealth[I].ExpireTurn = ([&] {
                                    std::int64_t cpp_right = System::Round(([&] {
                                        pas::Extended cpp_left_3 = aMyFunction::RemapClamped(aMyFunction::SeededRandomUnitFloat(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed) + I + aGalaxy::Galaxy->CurrentTurn), 0.0, 1.0, 0.5, 3.0);
                                        return cpp_left_3 * aConst::CaptainHealthDefinitions[I].Duration;
                                    }()));
                                    return aGalaxy::Galaxy->CurrentTurn + cpp_right;
                                }());
                                if (Self->CurrentPlanet != nullptr) {
                                    Self->StatusEffectSourceNames[I] = Self->CurrentPlanet->GetFullName(u" "_w);
                                } else if (Self->DockedTo != nullptr) {
                                    Self->StatusEffectSourceNames[I] = Self->DockedTo->GetName();
                                } else {
                                    Self->StatusEffectSourceNames[I] = Self->CurrentStar->Name;
                                }
                            }
                        }
                    }
                }
            }
            Stage = 7;
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, 12); cpp_range_6.next(I); ) {
                if (Self->CaptainHealth[I].Progress != 0.0L) {
                    if (Self->CaptainHealth[I].Progress < 1.0E+2L) {
                        if (pas::in_range(I, 1, 3)) {
                            if (Self->CurrentPlanet != nullptr || Self->DockedTo != nullptr) {
                                Self->CaptainHealth[I].Progress = 1.0E+2;
                            }
                        } else {
                            Self->CaptainHealth[I].Progress = ([&] {
                                pas::Extended cpp_left_4 = ([&] {
                                    pas::Extended cpp_left_5 = aMyFunction::SeededRandomUnitFloat(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed) - I + aGalaxy::Galaxy->CurrentTurn);
                                    return cpp_left_5 * aConst::CaptainHealthDefinitions[I].DevelopmentRate;
                                }()) * 2.0L;
                                return cpp_left_4 + Self->CaptainHealth[I].Progress;
                            }()) + 0.01L;
                        }
                        if (Self->CaptainHealth[I].Progress >= 1.0E+2L) {
                            Self->CaptainHealth[I].Progress = 1.0E+2;
                            ++Self->CaptainHealth[I].ApplicationCount;
                            Self->CaptainHealth[I].ExpireTurn = ([&] {
                                std::int64_t cpp_right_2 = System::Round(([&] {
                                    pas::Extended cpp_left_6 = aMyFunction::RemapClamped(aMyFunction::SeededRandomUnitFloat(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed) + I + aGalaxy::Galaxy->CurrentTurn), 0.0, 1.0, 0.9, 2.0);
                                    return cpp_left_6 * aConst::CaptainHealthDefinitions[I].Duration;
                                }()));
                                return aGalaxy::Galaxy->CurrentTurn + cpp_right_2;
                            }());
                            Text = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Illness.Illness.", SysUtils::IntToStr(I - 1), ".Start"})));
                            {
                                const pas::WideString& formatText2 = ([&] {
                                    pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(-1);
                                    pas::WideString name = aConst::CaptainHealthDefinitions[I].Name;
                                    pas::WideString text = Text;
                                    return aMyFunction::FormatText2(std::move(text), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate), u"<Name>"_w, std::move(name));
                                }());
                                std::int32_t currentTurn_4 = aGalaxy::Galaxy->CurrentTurn;
                                Globals::AddOrUpdatePlayerBubble(0, currentTurn_4, formatText2, u""_wref.get());
                            }
                            static_cast<void>(Self->AchievementStats), Achievements::TAchievementStats::CheckAllDiseasesAchievement();
                            ++Self->DiseaseContractionCount;
                        }
                    } else if (Self->CaptainHealth[I].ExpireTurn <= aGalaxy::Galaxy->CurrentTurn && (static_cast<std::uint8_t>(pas::in_range(I, 1, 3) ^ 1) || static_cast<std::uint8_t>(Self->CurrentStar->RecordingTurnFilm ^ 1))) {
                        Self->CaptainHealth[I].Progress = 0.0;
                        Self->StatusEffectSourceNames[I] = pas::WideString();
                        Text = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Illness.Illness.", SysUtils::IntToStr(I - 1), ".End"})));
                        {
                            const pas::WideString& formatText2_2 = ([&] {
                                pas::WideString formatTurnDate_2 = aGalaxy::Galaxy->FormatTurnDate(-1);
                                pas::WideString name_2 = aConst::CaptainHealthDefinitions[I].Name;
                                pas::WideString text_2 = Text;
                                return aMyFunction::FormatText2(std::move(text_2), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_2), u"<Name>"_w, std::move(name_2));
                            }());
                            std::int32_t currentTurn_5 = aGalaxy::Galaxy->CurrentTurn;
                            Globals::AddOrUpdatePlayerBubble(0, currentTurn_5, formatText2_2, u""_wref.get());
                        }
                    }
                }
            }
            Stage = 8;
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(13, 24); cpp_range_7.next(I); ) {
                if (Self->CaptainHealth[I].Progress == 1.0E+2L && Self->CaptainHealth[I].ExpireTurn <= aGalaxy::Galaxy->CurrentTurn) {
                    Text = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Illness.Stimulant.", SysUtils::IntToStr(I - 12 - 1), ".End"})));
                    {
                        const pas::WideString& formatText1 = ([&] {
                            pas::WideString formatTurnDate_3 = aGalaxy::Galaxy->FormatTurnDate(-1);
                            pas::WideString text_3 = Text;
                            return aMyFunction::FormatText1(std::move(text_3), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_3));
                        }());
                        std::int32_t currentTurn_6 = aGalaxy::Galaxy->CurrentTurn;
                        Globals::AddOrUpdatePlayerBubble(0, currentTurn_6, formatText1, u""_wref.get());
                    }
                    Self->CaptainHealth[I].Progress = 0.0;
                }
            }
            Stage = 9;
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(1, 1); cpp_range_8.next(I); ) {
                if (Self->RadiationHealth[I].Progress != 0.0L && Self->RadiationHealth[I].ExpireTurn <= aGalaxy::Galaxy->CurrentTurn) {
                    Text = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Illness.ExtraIllness.", SysUtils::IntToStr(I), ".End"})));
                    {
                        const pas::WideString& formatText1_2 = ([&] {
                            pas::WideString formatTurnDate_4 = aGalaxy::Galaxy->FormatTurnDate(-1);
                            pas::WideString text_4 = Text;
                            return aMyFunction::FormatText1(std::move(text_4), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_4));
                        }());
                        std::int32_t currentTurn_7 = aGalaxy::Galaxy->CurrentTurn;
                        Globals::AddOrUpdatePlayerBubble(0, currentTurn_7, formatText1_2, u""_wref.get());
                    }
                    Self->RadiationHealth[I].Progress = 0.0;
                }
            }
            Stage = 10;
            StimulantExcess = Self->CountActiveStimulants() - aPlayer::GetPlayer()->GetTotalStatBonus(aConst::bonStimCapacity);
            // The native one-pass loop retains its dormant footer after Break.
            while (StimulantExcess >= 2) {
                I = 6;
                if (Self->CaptainHealth[I].Progress <= 0.0L) {
                    LocalSeed = aGalaxy::Galaxy->GenerationSeed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn);
                    {
                        pas::Extended cpp_right_3 = aMyFunction::NextRandomFloatRange(0.0, 1.0E+3, LocalSeed);
                        if (pas::sqr(std::max<std::int32_t>(0, StimulantExcess - Self->CountActiveArtefacts(aConst::t_ArtBio))) * 0.4L > cpp_right_3) {
                            if (pas::contains(aConst::CaptainHealthDefinitions[I].AllowedOwners, aConst::RaceToOwner(Self->PilotRace)) && pas::contains(aConst::CaptainHealthDefinitions[I].AllowedRatingBands, Self->GetRangerRatingBand()) && pas::contains(aConst::CaptainHealthDefinitions[I].AllowedRanks, Self->Rank) && pas::contains(aConst::CaptainHealthDefinitions[I].AllowedCareers, static_cast<std::uint8_t>(Self->GetDominantCareer()))) {
                                Self->CaptainHealth[I].Progress = 1.0E+2;
                                Self->CaptainHealth[I].ExpireTurn = ([&] {
                                    std::int64_t cpp_right_4 = System::Round(([&] {
                                        pas::Extended cpp_left_7 = aMyFunction::RemapClamped(aMyFunction::SeededRandomUnitFloat(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed) + I + aGalaxy::Galaxy->CurrentTurn), 0.0, 1.0, 0.5, 3.0);
                                        return cpp_left_7 * aConst::CaptainHealthDefinitions[I].Duration;
                                    }()));
                                    return aGalaxy::Galaxy->CurrentTurn + cpp_right_4;
                                }());
                                ++Self->CaptainHealth[I].ApplicationCount;
                                Text = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Illness.Illness.", SysUtils::IntToStr(I - 1), ".Start"})));
                                {
                                    const pas::WideString& formatText2_3 = ([&] {
                                        pas::WideString formatTurnDate_5 = aGalaxy::Galaxy->FormatTurnDate(-1);
                                        pas::WideString name_3 = aConst::CaptainHealthDefinitions[I].Name;
                                        pas::WideString text_5 = Text;
                                        return aMyFunction::FormatText2(std::move(text_5), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_5), u"<Name>"_w, std::move(name_3));
                                    }());
                                    std::int32_t currentTurn_8 = aGalaxy::Galaxy->CurrentTurn;
                                    Globals::AddOrUpdatePlayerBubble(0, currentTurn_8, formatText2_3, u""_wref.get());
                                }
                                ++Self->DiseaseContractionCount;
                                static_cast<void>(Self->AchievementStats), Achievements::TAchievementStats::CheckAllDiseasesAchievement();
                            }
                        }
                    }
                }
                break;
            }
            Stage = 11;
            if (Self->IsHealthEffectActive(5) && aGalaxy::Galaxy->CurrentTurn > Self->CaptainHealth[5].AppliedTurn + 15 && aGalaxy::Galaxy->CurrentTurn % 14 == 0) {
                if (aMyFunction::SeededRandomUnitFloat(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed) + 1736605 + aGalaxy::Galaxy->CurrentTurn) > 0.8L) {
                    {
                        std::int32_t computeScaledAverageMoney = aGalaxy::Galaxy->ComputeScaledAverageMoney(2);
                        std::int32_t computeScaledSmallMoney = aGalaxy::Galaxy->ComputeScaledSmallMoney(2);
                        TargetValue = aMyFunction::NextRandomIntRange(computeScaledSmallMoney, computeScaledAverageMoney, Self->RandomState);
                    }
                    Self->SetMoney(TargetValue + Self->Money);
                    GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
                    {
                        const pas::WideString& formatText2_4 = ([&] {
                            pas::WideString formatTurnDate_6 = aGalaxy::Galaxy->FormatTurnDate(-1);
                            pas::WideString intToStr = pas::wide_int_to_str(TargetValue);
                            pas::WideString pickLocalizedTextVariant_4 = aConst::PickLocalizedTextVariant(u"GalaxyNews.IllNews.IllLuatan"_wref.get(), Self->Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                            return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_4), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_6), u"<Money>"_w, std::move(intToStr));
                        }());
                        std::int32_t currentTurn_9 = aGalaxy::Galaxy->CurrentTurn;
                        Globals::AddOrUpdatePlayerBubble(0, currentTurn_9, formatText2_4, u""_wref.get());
                    }
                } else {
                    const pas::WideString& formatText1_3 = ([&] {
                        pas::WideString formatTurnDate_7 = aGalaxy::Galaxy->FormatTurnDate(-1);
                        pas::WideString pickLocalizedTextVariant_5 = aConst::PickLocalizedTextVariant(u"GalaxyNews.IllNews.IllLuatanNo"_wref.get(), Self->Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                        return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant_5), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_7));
                    }());
                    std::int32_t currentTurn_10 = aGalaxy::Galaxy->CurrentTurn;
                    Globals::AddOrUpdatePlayerBubble(0, currentTurn_10, formatText1_3, u""_wref.get());
                }
            }
            Stage = 12;
            if (Self->IsHealthEffectActive(11) && Self->InNormalSpace() && Self->HasCargoGoods() && aMyFunction::SeededRandomUnitFloat(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed) + 135432 + aGalaxy::Galaxy->CurrentTurn) > 0.8L && aGalaxy::Galaxy->CurrentTurn % 21 == 0) {
                {
                    std::int32_t computeScaledBigMoney = aGalaxy::Galaxy->ComputeScaledBigMoney(2);
                    std::int32_t computeScaledMiniMoney = aGalaxy::Galaxy->ComputeScaledMiniMoney(2);
                    TargetValue = aMyFunction::NextRandomIntRange(computeScaledMiniMoney, computeScaledBigMoney, Self->RandomState);
                }
                Self->JettisonCargoGoodsTowardTargetValue(TargetValue);
                GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
                {
                    const pas::WideString& formatText1_4 = ([&] {
                        pas::WideString formatTurnDate_8 = aGalaxy::Galaxy->FormatTurnDate(-1);
                        pas::WideString pickLocalizedTextVariant_6 = aConst::PickLocalizedTextVariant(u"GalaxyNews.IllNews.IllSeciyanka"_wref.get(), Self->Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                        return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant_6), u"<color=255,240,100>"_w, u"<Date>"_w, std::move(formatTurnDate_8));
                    }());
                    std::int32_t currentTurn_11 = aGalaxy::Galaxy->CurrentTurn;
                    Globals::AddOrUpdatePlayerBubble(0, currentTurn_11, formatText1_4, u""_wref.get());
                }
            }
            Stage = 13;
            Self->RefreshDerivedStats(true);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TPlayer.NextDay, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    // Capped at 100000000; zero for nonpositive principal.
    std::int32_t TPlayer::ComputeDepositAccruedValue() {
        pas::Extended Base{};
        pas::Extended Exponent{};
        pas::Extended LimitRatio{};
        std::int32_t Result = 0;
        if (DepositAmount > 0) {
            Base = pas::real_divide(0.01L * DepositInterestRate, 12.0L) + 1.0L;
            Exponent = pas::real_divide(DepositDayCount, 365.0L) * 12.0L;
            LimitRatio = pas::real_divide(1.0E+8L, DepositAmount);
            {
                pas::Extended cpp_left = System::Ln(Base) * Exponent;
                if (cpp_left > System::Ln(LimitRatio)) {
                    return 100000000;
                }
            }
            return System::Round(Math::Power(Base, Exponent) * DepositAmount);
        }
        return Result;
    }

    // Increments every carried transmitter in the global player's artefact list, including unequipped ones.
    void TPlayer::RechargeTransmitters() {
        aItem::TItem* Item{};
        aItem::TArtefactTransmitter* Transmitter{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Artefacts) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Artefacts, I);
            if (Item->ItemType == aConst::t_ArtefactTransmitter) {
                Transmitter = pas::checked_cast<aItem::TArtefactTransmitter*>(Item);
                ++Transmitter->Power;
            }
        }
    }

    // Each active Bio artefact may shorten a disease and extend a stimulant by one turn.
    void TPlayer::ApplyBioArtefactHealthEffects() {
        std::int32_t Selected{};
        std::int32_t I{};
        std::int32_t Count{};
        std::int32_t J{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, CountActiveArtefacts(aConst::t_ArtBio)); cpp_range.next(J); ) {
            if (HasActiveDisease() && aMyFunction::NextRandomIntRange(1, 100, RandomState) <= 20) {
                Selected = aMyFunction::NextRandomIntRange(1, CountActiveDiseases(), RandomState);
                Count = 0;
                for (I = 1; I <= 12; ++I) {
                    if (CaptainHealth[I].Progress == 1.0E+2L) {
                        ++Count;
                        if (Count == Selected) {
                            --CaptainHealth[I].ExpireTurn;
                            break;
                        }
                    }
                }
            }
            if (HasActiveStimulant() && aMyFunction::NextRandomIntRange(1, 100, RandomState) <= 50) {
                Selected = aMyFunction::NextRandomIntRange(1, CountActiveStimulants(), RandomState);
                Count = 0;
                for (I = 13; I <= 24; ++I) {
                    if (CaptainHealth[I].Progress == 1.0E+2L) {
                        ++Count;
                        if (Count == Selected) {
                            ++CaptainHealth[I].ExpireTurn;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Terron unresolved, late-game offer cadence and no program 4 already carried.
    std::uint8_t TPlayer::MayTakeSubCrack() {
        return aGalaxy::Galaxy->IsDominatorSeriesUnresolved(aGalaxyStruct::dsTerron) && aGalaxy::Galaxy->CurrentTurn > 2000 && (pas::imod(aGalaxy::Galaxy->CurrentTurn, aGalaxy::Galaxy->CurrentTurn % 11 + 20) == 0 || aGalaxy::Galaxy->CurrentTurn > 4000) && static_cast<std::uint8_t>(HasProgram(aGalaxyStruct::prgSabCrack) ^ 1);
    }

    // Difficulty-scaled price.
    std::int32_t TPlayer::GetSubCrackCost() {
        return System::Round(pas::real_divide(1.0E+5L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestMoneyFactor));
    }

    // Rounded pirate career status / 1.3, plus one percentage point.
    std::uint8_t TPlayer::GetPirateServiceDiscount() {
        return System::Round(pas::real_divide(CareerStatus[aGalaxyStruct::rcPirate], 1.3L)) + 1;
    }

    std::int32_t TPlayer::CountProgramRewardStocks() {
        std::uint8_t I{};
        std::int32_t Result = 0;
        for (I = static_cast<std::uint8_t>(0); I <= static_cast<std::uint8_t>(11); ++I) {
            Result += ProgramRewardStocks[I];
        }
        return Result;
    }

    // Requires a TKling victim; records its hull capacity even when no reward is due.
    std::uint8_t TPlayer::TryAwardDominatorPrograms(aShip::TShip* Victim) {
        std::uint8_t ProgramIndex{};
        std::int32_t Count{};
        DestroyedDominatorHullMass += Victim->GetHull()->Weight;
        if (pas::in_set<aGalaxyStruct::ktEquentor, aGalaxyStruct::ktSmersh, aGalaxyStruct::ktBertor, aGalaxyStruct::ktBertor>(pas::checked_cast<aKling::TKling*>(Victim)->KlingType) && DestroyedDominatorHullMass > static_cast<long double>(aGalaxy::Galaxy->ScaleIntByTechLevel(500, 3000)) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor && aGalaxy::Galaxy->CurrentTurn > 365.0L * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor + LastDominatorProgramRewardTurn) {
            LastDominatorProgramRewardTurn = aGalaxy::Galaxy->CurrentTurn;
            DestroyedDominatorHullMass = 0;
            ProgramIndex = SelectProgramReward();
            Count = aRanger::TRanger::GetProgramRewardCount(ProgramIndex);
            ProgramRewardStocks[ProgramIndex] += Count;
            if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                const pas::WideString& formatText2 = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(Count);
                    pas::WideString programName = aRanger::TRanger::GetProgramName(ProgramIndex);
                    pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.WB.NewProgramm"_wref.get(), Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                    return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Count>"_w, std::move(intToStr), u"<Programm>"_w, std::move(programName));
                }());
                std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                Globals::AddOrUpdatePlayerBubble(0, currentTurn, formatText2, u""_wref.get());
            }
            return true;
        }
        return false;
    }

    // Returns the highest-scoring visible Coalition trade route; retains output arguments on failure and excludes their previous endpoints.
    std::uint8_t TPlayer::FindProfitableTradeRoute(std::uint8_t Nearby, std::uint32_t Seed, pas::Var<aPlanet::TPlanet*> PurchasePlanet, pas::Var<aPlanet::TPlanet*> SalePlanet, std::uint8_t& Good, aGalaxyStruct::TItemTypeMask GoodsMask) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t JumpRange{};
        aPlanet::TPlanet* BuyPlanet{};
        aPlanet::TPlanet* SellPlanet{};
        std::uint8_t Kind{};
        if (GetEngine() != nullptr) {
            JumpRange = std::max<std::int32_t>(aGalaxy::Galaxy->ScaleIntByTechLevel(8, 20), static_cast<std::int32_t>(GetEngine()->JumpRange));
        } else {
            JumpRange = aGalaxy::Galaxy->ScaleIntByTechLevel(8, 30);
        }
        float Score = 0.0f;
        float BestScore = Score;
        aPlanet::TPlanet* BestBuyPlanet = nullptr;
        aPlanet::TPlanet* BestSellPlanet = nullptr;
        std::uint8_t BestGood = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range.next(I); ) {
            BuyPlanet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, I);
            if (!BuyPlanet->CurrentStar->IsConstellationVisible()) {
                continue;
            }
            if (!pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), BuyPlanet->OwnerId)) {
                continue;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range_2.next(J); ) {
                SellPlanet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, J);
                if (!SellPlanet->CurrentStar->IsConstellationVisible()) {
                    continue;
                }
                if (!pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), SellPlanet->OwnerId)) {
                    continue;
                }
                if (pas::load_unaligned<aPlanet::TPlanet*>(PurchasePlanet.address) == BuyPlanet || pas::load_unaligned<aPlanet::TPlanet*>(SalePlanet.address) == SellPlanet || SellPlanet == BuyPlanet) {
                    continue;
                }
                if (Nearby) {
                    if (static_cast<long double>(aMyFunction::PointDistance(CurrentStar->Position, BuyPlanet->CurrentStar->Position)) > std::min<std::int32_t>(JumpRange, 20)) {
                        continue;
                    }
                    if (static_cast<long double>(aMyFunction::PointDistance(CurrentStar->Position, SellPlanet->CurrentStar->Position)) > std::min<std::int32_t>(2 * JumpRange, 40)) {
                        continue;
                    }
                } else {
                    {
                        pas::Extended cpp_left = aMyFunction::PointDistance(CurrentStar->Position, BuyPlanet->CurrentStar->Position);
                        Score = cpp_left + aMyFunction::PointDistance(BuyPlanet->CurrentStar->Position, SellPlanet->CurrentStar->Position);
                    }
                    if (Score < 4.0E+1L || aMyFunction::PointDistance(CurrentStar->Position, BuyPlanet->CurrentStar->Position) < 2.0E+1L || aMyFunction::PointDistance(CurrentStar->Position, SellPlanet->CurrentStar->Position) < 3.0E+1L) {
                        continue;
                    }
                    if (Score < 7.0E+1L && static_cast<long double>(aMyFunction::PointDistance(CurrentStar->Position, BuyPlanet->CurrentStar->Position)) < std::min<std::int32_t>(30, JumpRange) && static_cast<long double>(aMyFunction::PointDistance(BuyPlanet->CurrentStar->Position, SellPlanet->CurrentStar->Position)) < std::min<std::int32_t>(30, JumpRange)) {
                        continue;
                    }
                }
                for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(7); ++Kind) {
                    if (pas::contains(GoodsMask, Kind)) {
                        if (aConst::GoodsLegalOnPlanet[Kind][BuyPlanet->RaceId][BuyPlanet->Government]) {
                            if (aConst::GoodsLegalOnPlanet[Kind][SellPlanet->RaceId][SellPlanet->Government]) {
                                if (BuyPlanet->RelationToShip(this) >= 20 && SellPlanet->RelationToShip(this) >= 20 && aMyFunction::SeededRandomUnitFloat(Kind * Seed * BuyPlanet->GenerationSeed + SellPlanet->GenerationSeed) >= 0.2L) {
                                    {
                                        pas::Extended cpp_left_2 = ShopGoodsSellPrice(Kind, SellPlanet);
                                        Score = pas::real_divide(cpp_left_2, ShopGoodsPurchasePrice(Kind, BuyPlanet));
                                    }
                                    if (Score >= 1.11L) {
                                        if (([&] {
                                            std::int32_t cpp_left_3 = ShopGoodsSellPrice(Kind, SellPlanet) - 5;
                                            return cpp_left_3 >= ShopGoodsPurchasePrice(Kind, BuyPlanet);
                                        }()) && aConst::GoodsMarket[Kind].BaseStock / 3 <= BuyPlanet->Goods[Kind].Count) {
                                            Score = static_cast<long double>(Score) * aMyFunction::RemapClamped(BuyPlanet->Goods[Kind].Count, aConst::GoodsMarket[Kind].BaseStock / 3, aConst::GoodsMarket[Kind].BaseStock * 1.1L, 0.7, 1.5);
                                            Score = static_cast<long double>(Score) * aMyFunction::RemapClamped(pas::real_divide(aMyFunction::PointDistance(CurrentStar->Position, BuyPlanet->CurrentStar->Position), JumpRange), 0.0, 2.0, 1.3, 1.0);
                                            Score = static_cast<long double>(Score) * aMyFunction::RemapClamped(pas::real_divide(aMyFunction::PointDistance(BuyPlanet->CurrentStar->Position, SellPlanet->CurrentStar->Position), JumpRange), 0.0, 3.0, 1.3, 1.0);
                                            Score = static_cast<long double>(Score) * aMyFunction::SeededRandomFloatRange(Kind * BuyPlanet->GenerationSeed * SellPlanet->GenerationSeed + Seed * J, 1.0, 2.1);
                                            if (Score > BestScore) {
                                                BestScore = Score;
                                                BestBuyPlanet = BuyPlanet;
                                                BestSellPlanet = SellPlanet;
                                                BestGood = Kind;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (BestScore > 0.0L) {
            pas::store_unaligned<aPlanet::TPlanet*>(PurchasePlanet.address, BestBuyPlanet);
            pas::store_unaligned<aPlanet::TPlanet*>(SalePlanet.address, BestSellPlanet);
            Good = BestGood;
            return true;
        }
        return false;
    }

    // Uses Self.Satellites.Count but reads the global player's list.
    std::uint8_t TPlayer::HasDeployedSatellites() {
        std::int32_t I{};
        aItem::TSatellite* Satellite{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Satellites) - 1); cpp_range.next(I); ) {
            Satellite = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I);
            if (Satellite->TargetPlanet != nullptr) {
                return true;
            }
        }
        return false;
    }

    // Requires Self=GetPlayer(): uses Self for the list count but fetches entries from the global player's deployed satellites.
    std::uint8_t TPlayer::HasSatelliteOnPlanet(aPlanet::TPlanet* Planet) {
        std::int32_t I{};
        aItem::TSatellite* Satellite{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Satellites) - 1); cpp_range.next(I); ) {
            Satellite = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I);
            if (Satellite->TargetPlanet == Planet) {
                return true;
            }
        }
        return false;
    }

    pas::WideString TPlayer::GetStorageColumnHeaderText() {
        pas::WideString Text{};
        Text = u"<color=255,240,100>"_w;
        Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(StorageHeaderColumns[GR_Main::GiResourceVariant()].Size), u"><align=right>", aConst::LocalizedText(u"FormShip.StorageInfo.Size"_wref.get()), u"</align>"});
        Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(StorageHeaderColumns[GR_Main::GiResourceVariant()].Cost), u"><align=right>", aConst::LocalizedText(u"FormShip.StorageInfo.Cost"_wref.get()), u"</align>"});
        Text = pas::concat_wide({Text, u"</color>"});
        return Text;
    }

    pas::WideString TPlayer::GetStorageDividerText() {
        return aMyFunction::WrapTextInColor(static_cast<pas::WideString>(SystemImports::StringOfChar('-', StorageDividerLengths[GR_Main::GiResourceVariant()])), u"<color=127,127,127>"_w);
    }

    pas::WideString TPlayer::BuildDeployedSatelliteSummary(std::int32_t& LineCount) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t HeaderCount{};
        std::int32_t Condition{};
        std::int32_t ExplorationTurns{};
        aItem::TSatellite* Satellite{};
        pas::WideString Text{};
        pas::WideString RemainingText{};
        pas::WideString SizeText{};
        pas::WideString ExplorationText{};
        pas::WideString ConditionText{};
        pas::WideString StatusText{};
        pas::WideString TempText{};
        pas::WideString Divider{};
        aPlanet::TPlanet* Planet{};
        Divider = TPlayer::GetStorageDividerText();
        LineCount = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range.next(I); ) {
            HeaderCount = 0;
            Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Satellites) - 1); cpp_range_2.next(J); ) {
                Satellite = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, J);
                if (Satellite->TargetPlanet == Planet) {
                    if (HeaderCount == 0) {
                        TempText = ([&] {
                            auto name = pas::borrow(pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Satellite->TargetPlanet))->CurrentStar->Name);
                            pas::WideString localizedText = aConst::LocalizedText(u"FormShip.StorageInfo.Star"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText), pas::WideString(), u"<Star>"_w, name.get());
                        }());
                        TempText = pas::concat_wide_reverse({aMyFunction::WrapTextInColor(pas::concat_wide({pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Satellite->TargetPlanet))->GetFullName(u" "_w), u"."}), u"<color=255,240,100>"_w), aMyFunction::WrapTextInColor(pas::concat_wide({TempText, u". "}), u"<color=255,240,100>"_w)});
                        RemainingText = pas::concat_wide({u" ", aConst::LocalizedText(u"FormShip.StorageInfo.PlanetNO"_wref.get())});
                        if (Planet->WaterTiles - Planet->WaterExplored > 0) {
                            aMyFunction::ReplaceTextToken(RemainingText, u"<Water>"_w, pas::wide_int_to_str(Planet->WaterTiles - Planet->WaterExplored), u"<color=0,128,255>"_w);
                        } else {
                            aMyFunction::ReplaceTextToken(RemainingText, u"<Water>"_w, u"-"_w, u"<color=127,127,127>"_w);
                        }
                        if (Planet->LandTiles - Planet->LandExplored > 0) {
                            aMyFunction::ReplaceTextToken(RemainingText, u"<Land>"_w, pas::wide_int_to_str(Planet->LandTiles - Planet->LandExplored), u"<color=0,255,0>"_w);
                        } else {
                            aMyFunction::ReplaceTextToken(RemainingText, u"<Land>"_w, u"-"_w, u"<color=127,127,127>"_w);
                        }
                        if (Planet->HillTiles - Planet->HillExplored > 0) {
                            aMyFunction::ReplaceTextToken(RemainingText, u"<Hill>"_w, pas::wide_int_to_str(Planet->HillTiles - Planet->HillExplored), u"<color=254,217,7>"_w);
                        } else {
                            aMyFunction::ReplaceTextToken(RemainingText, u"<Hill>"_w, u"-"_w, u"<color=127,127,127>"_w);
                        }
                        TempText = pas::concat_wide({TempText, RemainingText});
                        Text = pas::concat_wide({Text, u"\r\n", Divider, u"\r\n", u"<td=", pas::wide_int_to_str(ProbeSummaryColumns[GR_Main::GiResourceVariant()].Heading), u"><align=center>", TempText, u"</align>", u"\r\n", Divider, u"\r\n"});
                        ++HeaderCount;
                        ++LineCount;
                    }
                    SizeText = aMyFunction::WrapTextInColor(pas::wide_int_to_str(Satellite->Weight), u"<color=0,255,0>"_w);
                    TempText = pas::WideString();
                    if (Satellite->WaterExplorationRate > 0) {
                        TempText = pas::concat_wide({TempText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(static_cast<std::int32_t>(Satellite->WaterExplorationRate)), u"<color=0,128,255>"_w)});
                    } else {
                        TempText = pas::concat_wide({TempText, aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w)});
                    }
                    TempText = pas::concat_wide({TempText, u"/"});
                    if (Satellite->LandExplorationRate > 0) {
                        TempText = pas::concat_wide({TempText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(static_cast<std::int32_t>(Satellite->LandExplorationRate)), u"<color=0,255,0>"_w)});
                    } else {
                        TempText = pas::concat_wide({TempText, aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w)});
                    }
                    TempText = pas::concat_wide({TempText, u"/"});
                    if (Satellite->HillExplorationRate > 0) {
                        TempText = pas::concat_wide({TempText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(static_cast<std::int32_t>(Satellite->HillExplorationRate)), u"<color=254,217,7>"_w)});
                    } else {
                        TempText = pas::concat_wide({TempText, aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w)});
                    }
                    ExplorationText = TempText;
                    Condition = System::Trunc(Satellite->ConditionPercent);
                    if (Satellite->ConditionPercent > 0.0L) {
                        TempText = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Condition), ".", SysUtils::Int64ToStr(System::Trunc(Satellite->ConditionPercent * 1.0E+1L) % 10), "%"}));
                    } else {
                        TempText = u"0.0%"_w;
                    }
                    if (Condition > 75) {
                        TempText = aMyFunction::WrapTextInColor(TempText, u"<color=0,255,0>"_w);
                    } else if (Condition > 50) {
                        TempText = aMyFunction::WrapTextInColor(TempText, u"<color=255,240,100>"_w);
                    } else if (Condition > 25) {
                        TempText = aMyFunction::WrapTextInColor(TempText, u"<color=254,217,7>"_w);
                    } else {
                        TempText = aMyFunction::WrapTextInColor(TempText, u"<color=255,0,0>"_w);
                    }
                    ConditionText = TempText;
                    StatusText = pas::WideString();
                    ExplorationTurns = TPlayer::GetSatelliteExplorationTurns(Satellite);
                    if (ExplorationTurns == 0) {
                        StatusText = pas::concat_wide({u" ", aMyFunction::WrapTextInColor(aConst::LocalizedText(u"Items.Satellite.WorkEnd"_wref.get()), u"<color=255,0,0>"_w)});
                    }
                    Text = pas::concat_wide({Text, u"- ", Satellite->GetDisplayName()});
                    Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(ProbeSummaryColumns[GR_Main::GiResourceVariant()].Size), u"><align=right>", SizeText, u"</align>"});
                    Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(ProbeSummaryColumns[GR_Main::GiResourceVariant()].Exploration), u"><align=right>", ExplorationText, u"</align>"});
                    Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(ProbeSummaryColumns[GR_Main::GiResourceVariant()].Condition), u"><align=right>", ConditionText, u"</align>"});
                    Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(ProbeSummaryColumns[GR_Main::GiResourceVariant()].Status), u">", StatusText, u"\r\n"});
                    ++LineCount;
                }
            }
        }
        return Text;
    }

    // Remaining duration at the current planet using combined operational probe rates; capped at 999 per terrain.
    std::int32_t TPlayer::GetSatelliteExplorationTurns(aItem::TSatellite* Satellite) {
        std::int32_t I{};
        aItem::TSatellite* Probe{};
        std::int32_t Result = 0;
        if (Satellite->TargetPlanet == nullptr) {
            return Result;
        }
        if (Satellite->BrokenFlag != 0) {
            return Result;
        }
        aPlanet::TPlanet* Planet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Satellite->TargetPlanet));
        std::int32_t Water = 0;
        std::int32_t Land = 0;
        std::int32_t Hill = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Satellites) - 1); cpp_range.next(I); ) {
            Probe = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I);
            if (Satellite->TargetPlanet == Probe->TargetPlanet && Probe->BrokenFlag == 0) {
                Water = std::min<std::int32_t>(Planet->WaterTiles - Planet->WaterExplored, Water + Probe->WaterExplorationRate);
                Land = std::min<std::int32_t>(Planet->LandTiles - Planet->LandExplored, Land + Probe->LandExplorationRate);
                Hill = std::min<std::int32_t>(Planet->HillTiles - Planet->HillExplored, Hill + Probe->HillExplorationRate);
            }
        }
        if (Water > 0) {
            Water = std::min<std::int32_t>(999, MathImports::Ceil(pas::real_divide(Planet->WaterTiles - Planet->WaterExplored, Water)));
        }
        if (Land > 0) {
            Land = std::min<std::int32_t>(999, MathImports::Ceil(pas::real_divide(Planet->LandTiles - Planet->LandExplored, Land)));
        }
        if (Hill > 0) {
            Hill = std::min<std::int32_t>(999, MathImports::Ceil(pas::real_divide(Planet->HillTiles - Planet->HillExplored, Hill)));
        }
        if (Satellite->WaterExplorationRate > 0 && Water > 0) {
            Result = Water;
        }
        if (Satellite->LandExplorationRate > 0 && Land > 0) {
            Result = std::max<std::int32_t>(Result, Land);
        }
        if (Satellite->HillExplorationRate > 0 && Hill > 0) {
            return std::max<std::int32_t>(Result, Hill);
        }
        return Result;
    }

    // Native stub always returns true; Self and Item are unused. Called by treasure-map selection and planet loot reset.
    std::uint8_t TPlayer::CanAccessSurfaceLootItem(aItem::TItem* Item) {
        return true;
    }

    // Reports inactive deployed satellites in the entered star; records up to three target planets.
    void TPlayer::ReportIdleSatellites(aGalaxy::TStar* Star) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Remaining{};
        aItem::TSatellite* Satellite{};
        pas::WideString Text{};
        pas::Array<aPlanet::TPlanet*, 1, 3> Planets{};
        std::uint8_t Found = false;
        Text = pas::WideString();
        Planets[1] = nullptr;
        Planets[2] = nullptr;
        Planets[3] = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Satellites) - 1); cpp_range.next(I); ) {
            Satellite = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I);
            if (Satellite->TargetPlanet != nullptr && pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Satellite->TargetPlanet))->CurrentStar == Star) {
                Remaining = TPlayer::GetSatelliteExplorationTurns(Satellite);
                if (Remaining <= 0) {
                    for (J = 1; J <= 3; ++J) {
                        if (Planets[J] == Satellite->TargetPlanet) {
                            break;
                        }
                        if (Planets[J] == nullptr) {
                            Planets[J] = static_cast<aPlanet::TPlanet*>(Satellite->TargetPlanet);
                            break;
                        }
                    }
                    Text = pas::concat_wide({Text, u"\r\n", Satellite->GetIdleInfoText()});
                    Found = true;
                }
            }
        }
        if (Found) {
            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
            if (Planets[1] != nullptr) {
                cpp_with->Targets[0].PlanetId = Planets[1]->Id;
            }
            if (Planets[2] != nullptr) {
                cpp_with->Targets[1].PlanetId = Planets[2]->Id;
            }
            if (Planets[3] != nullptr) {
                cpp_with->Targets[2].PlanetId = Planets[3]->Id;
            }
        }
    }

    // Native empty turn-start hook.
    void TPlayer::BeginStorageTurn() {
    }

    // Native always-true permission hook; Good is passed in DL.
    std::uint8_t TPlayer::CanAccessHoldGoods(std::uint8_t Good) {
        return true;
    }

    // Native always-true permission hook used by storage lookup, counting and slot allocation.
    std::uint8_t TPlayer::CanAccessStoredItem(aItem::TItem* Item) {
        return true;
    }

    // Location=nil includes all storage locations. Goods and item types 69/75 count by weight; other matching items count individually.
    std::int32_t TPlayer::CountStoredItemUnits(pas::Object* Location, std::uint8_t ItemType) {
        std::int32_t I{};
        PStorageEntry Entry{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
            if (TPlayer::CanAccessStoredItem(Entry->Item) && (Location == nullptr || Entry->LocationOwner == Location)) {
                if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics)) || pas::in_set<69, 69, 75, 75>(ItemType)) {
                    if (static_cast<std::uint8_t>(Entry->Item->ItemType) == ItemType) {
                        Result += Entry->Item->Weight;
                    }
                } else if (static_cast<std::uint8_t>(Entry->Item->ItemType) == ItemType) {
                    ++Result;
                }
            }
        }
        return Result;
    }

    // Reassigns later accessible entries with duplicate slot indices.
    void TPlayer::RepairDuplicateStorageSlots(pas::Object* Location) {
        std::int32_t I{};
        std::int32_t J{};
        PStorageEntry Entry{};
        PStorageEntry Other{};
        std::int32_t Count = pas::list_count(StorageEntries);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
            if (Entry->LocationOwner == Location && TPlayer::CanAccessStoredItem(Entry->Item)) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, Count - 1); cpp_range_2.next(J); ) {
                    Other = pas::list_at<TStorageEntry>(StorageEntries, J);
                    if (Other->LocationOwner == Location && TPlayer::CanAccessStoredItem(Other->Item)) {
                        if (Entry->SlotIndex == Other->SlotIndex) {
                            Other->SlotIndex = FindNextStorageSlot(Location);
                        }
                    }
                }
            }
        }
    }

    // Returns a nonnegative slot local to Location.
    std::int32_t TPlayer::FindNextStorageSlot(pas::Object* Location) {
        std::int32_t I{};
        PStorageEntry Entry{};
        std::int32_t Result = 0;
        std::int32_t Count = pas::list_count(StorageEntries);
        while (true) {
            I = 0;
            while (I < Count) {
                Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
                if (Entry->LocationOwner == Location && TPlayer::CanAccessStoredItem(Entry->Item) && Entry->SlotIndex == Result) {
                    break;
                }
                ++I;
            }
            if (I >= Count) {
                break;
            }
            ++Result;
        }
        return Result;
    }

    // Maximum accessible slot index plus one, or zero.
    std::int32_t TPlayer::GetStorageSlotExtent(pas::Object* Location) {
        std::int32_t I{};
        PStorageEntry Entry{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(reinterpret_cast<pas::List*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(StorageEntries)) + 0))), I);
            if (Entry->LocationOwner == Location && TPlayer::CanAccessStoredItem(Entry->Item)) {
                Result = std::max<std::int32_t>(Result, Entry->SlotIndex + 1);
            }
        }
        return Result;
    }

    // Returns a zero-based StorageEntries index, or -1.
    std::int32_t TPlayer::FindStorageIndexByLocationAndSlot(pas::Object* Location, std::int32_t Slot) {
        std::int32_t I{};
        PStorageEntry Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
            if (Entry->LocationOwner == Location && TPlayer::CanAccessStoredItem(Entry->Item) && Entry->SlotIndex == Slot) {
                return I;
            }
        }
        return -1;
    }

    // Returns a zero-based StorageEntries index, or -1.
    std::int32_t TPlayer::FindStorageGoodsByLocationAndType(pas::Object* Location, std::uint8_t Good) {
        std::int32_t I{};
        PStorageEntry Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
            if (Entry->LocationOwner == Location && static_cast<std::uint8_t>(Entry->Item->ItemType) == Good) {
                return I;
            }
        }
        return -1;
    }

    // Returns a zero-based StorageEntries index, or -1.
    std::int32_t TPlayer::FindMergeableStorageItemByLocation(pas::Object* Location, aItem::TCountableItem* Item) {
        std::int32_t I{};
        PStorageEntry Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
            if (Entry->LocationOwner == Location && aItem::TCountableItem_CanMerge(Item, Entry->Item)) {
                return I;
            }
        }
        return -1;
    }

    void TPlayer::ShiftStorageSlotsAtOrAfter(pas::Object* Location, std::int32_t Slot) {
        std::int32_t I{};
        PStorageEntry Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
            if (Entry->LocationOwner == Location && TPlayer::CanAccessStoredItem(Entry->Item) && Entry->SlotIndex >= Slot) {
                ++Entry->SlotIndex;
            }
        }
    }

    // Shifts later slots down only if Slot is unoccupied.
    void TPlayer::CloseVacantStorageSlot(pas::Object* Location, std::int32_t Slot) {
        std::int32_t I{};
        PStorageEntry Entry{};
        if (FindStorageIndexByLocationAndSlot(Location, Slot) < 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
                if (Entry->LocationOwner == Location && TPlayer::CanAccessStoredItem(Entry->Item)) {
                    if (Entry->SlotIndex >= Slot) {
                        --Entry->SlotIndex;
                    }
                }
            }
        }
    }

    // With nil, returns whether the entire storage list is empty.
    std::uint8_t TPlayer::HasAccessibleStorageAt(pas::Object* Location) {
        std::int32_t I{};
        PStorageEntry Entry{};
        if (Location == nullptr) {
            return pas::list_count(StorageEntries) <= 0;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
            if (Entry->LocationOwner == Location && TPlayer::CanAccessStoredItem(Entry->Item)) {
                return true;
            }
        }
        return false;
    }

    // Counts ships following Self in the current system, wrapping at 256.
    std::uint8_t TPlayer::CountPartnersInNormalSpace() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        std::uint8_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (Ship->PartnerShip == this && Ship->InNormalSpace()) {
                ++Result;
            }
        }
        return Result;
    }

    // Native compares ranger TotalExperience with the global player's PlaceInRating after refreshing rankings.
    std::uint8_t TPlayer::GetShipRatingComparison(aShip::TShip* Ship) {
        aRanger::TRanger* Ranger{};
        std::uint8_t Result = 0;
        if (Ship->TypeId == aGalaxyStruct::stRanger) {
            aGalaxy::Galaxy->RefreshRangerRatingPlaces();
            Ranger = pas::checked_cast<aRanger::TRanger*>(Ship);
            {
                std::int32_t cpp_case = static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(Ranger->TotalExperience, pas::real_divide(aPlayer::GetPlayer()->PlaceInRating, 3.0L), 3 * aPlayer::GetPlayer()->PlaceInRating, 0.0, 1.0E+2)));
                if (cpp_case >= 0 && cpp_case <= 20) {
                    return 1;
                } else if (cpp_case >= 21 && cpp_case <= 40) {
                    return 2;
                } else if (cpp_case >= 41 && cpp_case <= 60) {
                    return 3;
                } else if (cpp_case >= 61 && cpp_case <= 80) {
                    return 4;
                } else if (cpp_case >= 81 && cpp_case <= 100) {
                    return 5;
                } else {
                    GR_Main::RaiseWideMessage(u"\u041e\u0448\u0438\u0431\u043a?? \u0432 \u0440\u0435\u0439\u0442\u0438\u043d\u0433\u0435 \u043a\u043e\u0440\u0430\u0431\u043b\u044f \u0432 \u0441\u0440\u0430\u0432\u043d\u0435\u043d\u0438\u0438 \u0441 \u0438\u0433\u0440\u043e\u043a\u043e\u043c"_wref.get());
                    return Result;
                }
            }
        }
        return Result;
    }

    // Returns 1..5 for relative Coalition rank, zero for non-normal ships.
    std::uint8_t TPlayer::GetShipRankComparison(aShip::TShip* Ship) {
        aNormalShip::TNormalShip* Normal{};
        std::uint8_t Result = 0;
        if (pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr) {
            Normal = pas::checked_cast<aNormalShip::TNormalShip*>(Ship);
            {
                std::int32_t cpp_case = Normal->Rank - aPlayer::GetPlayer()->Rank;
                if (cpp_case >= -7 && cpp_case <= -2) {
                    return 1;
                } else if (cpp_case == -1) {
                    return 2;
                } else if (cpp_case == 0) {
                    return 3;
                } else if (cpp_case == 1) {
                    return 4;
                } else if (cpp_case >= 2 && cpp_case <= 7) {
                    return 5;
                } else {
                    GR_Main::RaiseWideMessage(u"Error in \u0440\u0430\u043d\u043a \u043a\u043e\u0440\u0430\u0431\u043b\u044f \u0432 \u0441\u0440\u0430\u0432\u043d\u0435\u043d\u0438\u0438 \u0441 \u0438\u0433\u0440\u043e\u043a\u043e\u043c"_wref.get());
                    return Result;
                }
            }
        }
        return Result;
    }

    // Returns 1..5 for relative pirate rank, zero for non-normal ships.
    std::uint8_t TPlayer::GetShipPirateRankComparison(aShip::TShip* Ship) {
        aNormalShip::TNormalShip* Normal{};
        std::uint8_t Result = 0;
        if (pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr) {
            Normal = pas::checked_cast<aNormalShip::TNormalShip*>(Ship);
            {
                std::int32_t cpp_case = Normal->PirateRank - aPlayer::GetPlayer()->PirateRank;
                if (cpp_case >= -8 && cpp_case <= -2) {
                    return 1;
                } else if (cpp_case == -1) {
                    return 2;
                } else if (cpp_case == 0) {
                    return 3;
                } else if (cpp_case == 1) {
                    return 4;
                } else if (cpp_case >= 2 && cpp_case <= 8) {
                    return 5;
                } else {
                    GR_Main::RaiseWideMessage(u"Error in \u0440\u0430\u043d\u043a \u043a\u043e\u0440\u0430\u0431\u043b\u044f \u0432 \u0441\u0440\u0430\u0432\u043d\u0435\u043d\u0438\u0438 \u0441 \u0438\u0433\u0440\u043e\u043a\u043e\u043c"_wref.get());
                    return Result;
                }
            }
        }
        return Result;
    }

    // Returns 1..5 from strength relative to the global player.
    std::uint8_t TPlayer::GetShipStrengthComparison(aShip::TShip* Ship) {
        std::uint8_t Result = 0;
        {
            std::int32_t cpp_case = static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(Ship->Strength, pas::real_divide(aPlayer::GetPlayer()->Strength, 3.0L), aPlayer::GetPlayer()->Strength * 3.0L, 0.0, 1.0E+2)));
            if (cpp_case >= 0 && cpp_case <= 20) {
                return 1;
            } else if (cpp_case >= 21 && cpp_case <= 40) {
                return 2;
            } else if (cpp_case >= 41 && cpp_case <= 60) {
                return 3;
            } else if (cpp_case >= 61 && cpp_case <= 80) {
                return 4;
            } else if (cpp_case >= 81 && cpp_case <= 100) {
                return 5;
            } else {
                GR_Main::RaiseWideMessage(u"Error in \u0441\u0438\u043b\u0430 \u043a\u043e\u0440\u0430\u0431\u043b\u044f \u0432 \u0441\u0440\u0430\u0432\u043d\u0435\u043d\u0438\u0438 \u0441 \u0438\u0433\u0440\u043e\u043a\u043e\u043c"_wref.get());
                return Result;
            }
        }
    }

    pas::WideString TPlayer::BuildTranclucatorStorageSummary(std::int32_t& LineCount) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t HeaderCount{};
        aShip::TShip* Ship{};
        pas::WideString Text{};
        pas::WideString Heading{};
        pas::WideString Divider{};
        aGalaxy::TStar* Star{};
        Divider = TPlayer::GetStorageDividerText();
        LineCount = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            HeaderCount = 0;
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (Ship->TypeId == aGalaxyStruct::stTranclucator) {
                    if (pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == aPlayer::GetPlayer() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1)) {
                        if (HeaderCount == 0) {
                            Heading = aMyFunction::WrapTextInColor(([&] {
                                auto name = pas::borrow(Star->Name);
                                pas::WideString localizedText = aConst::LocalizedText(u"FormShip.StorageInfo.Star"_wref.get());
                                return aMyFunction::FormatText1(std::move(localizedText), pas::WideString(), u"<Star>"_w, name.get());
                            }()), u"<color=255,240,100>"_w);
                            Text = pas::concat_wide({Text, u"\r\n", Divider, u"\r\n", u"<td=", pas::wide_int_to_str(TranclucatorSummaryWidths[GR_Main::GiResourceVariant()]), u"><align=center>", Heading, u"</align>", u"\r\n", Divider, u"\r\n"});
                            ++HeaderCount;
                            ++LineCount;
                        }
                        Text = pas::concat_wide({Text, u"- ", Ship->GetName(), u"\r\n"});
                        ++LineCount;
                    }
                }
            }
        }
        return Text;
    }

    // Compares star/location IDs, type, module priority/index, weight and cost; ignores slot indices.
    std::int32_t TPlayer::CompareStorageEntries(PStorageEntry Left, PStorageEntry Right) {
        std::uint32_t LeftStarId{};
        std::uint32_t RightStarId{};
        std::int32_t LeftPriority{};
        std::int32_t RightPriority{};
        if (pas::class_cast_if<aPlanet::TPlanet*>(Left->LocationOwner) != nullptr) {
            LeftStarId = pas::checked_cast<aPlanet::TPlanet*>(Left->LocationOwner)->CurrentStar->Id;
        } else {
            LeftStarId = pas::checked_cast<aShip::TShip*>(Left->LocationOwner)->CurrentStar->Id;
        }
        if (pas::class_cast_if<aPlanet::TPlanet*>(Right->LocationOwner) != nullptr) {
            RightStarId = pas::checked_cast<aPlanet::TPlanet*>(Right->LocationOwner)->CurrentStar->Id;
        } else {
            RightStarId = pas::checked_cast<aShip::TShip*>(Right->LocationOwner)->CurrentStar->Id;
        }
        if (LeftStarId < RightStarId) {
            return -1;
        }
        if (LeftStarId > RightStarId) {
            return 1;
        }
        if (pas::class_cast_if<aPlanet::TPlanet*>(Left->LocationOwner) != nullptr && pas::class_cast_if<aShip::TShip*>(Right->LocationOwner) != nullptr) {
            return -1;
        }
        if (pas::class_cast_if<aShip::TShip*>(Left->LocationOwner) != nullptr && pas::class_cast_if<aPlanet::TPlanet*>(Right->LocationOwner) != nullptr) {
            return 1;
        }
        if (pas::class_cast_if<aPlanet::TPlanet*>(Left->LocationOwner) != nullptr) {
            {
                std::uint32_t cpp_left = pas::checked_cast<aPlanet::TPlanet*>(Left->LocationOwner)->Id;
                if (cpp_left < pas::checked_cast<aPlanet::TPlanet*>(Right->LocationOwner)->Id) {
                    return -1;
                }
            }
            {
                std::uint32_t cpp_left_2 = pas::checked_cast<aPlanet::TPlanet*>(Left->LocationOwner)->Id;
                if (cpp_left_2 > pas::checked_cast<aPlanet::TPlanet*>(Right->LocationOwner)->Id) {
                    return 1;
                }
            }
        } else {
            {
                std::uint32_t cpp_left_3 = pas::checked_cast<aShip::TShip*>(Left->LocationOwner)->Id;
                if (cpp_left_3 < static_cast<std::uint32_t>(pas::checked_cast<aShip::TShip*>(Right->LocationOwner)->Id)) {
                    return -1;
                }
            }
            {
                std::uint32_t cpp_left_4 = pas::checked_cast<aShip::TShip*>(Left->LocationOwner)->Id;
                if (cpp_left_4 > static_cast<std::uint32_t>(pas::checked_cast<aShip::TShip*>(Right->LocationOwner)->Id)) {
                    return 1;
                }
            }
        }
        if (static_cast<std::int32_t>(Left->Item->ItemType) < static_cast<std::int32_t>(Right->Item->ItemType)) {
            return -1;
        }
        if (static_cast<std::int32_t>(Left->Item->ItemType) > static_cast<std::int32_t>(Right->Item->ItemType)) {
            return 1;
        }
        if (Left->Item->ItemType == aConst::t_MicroModule && Right->Item->ItemType == aConst::t_MicroModule) {
            LeftPriority = aItem::GetMicroModulePriorityColorTier(pas::checked_cast<aItem::TMicroModule*>(Left->Item)->MicroModuleIndex - 1);
            RightPriority = aItem::GetMicroModulePriorityColorTier(pas::checked_cast<aItem::TMicroModule*>(Right->Item)->MicroModuleIndex - 1);
            if (LeftPriority > RightPriority) {
                return -1;
            }
            if (LeftPriority < RightPriority) {
                return 1;
            }
            {
                std::int32_t cpp_left_5 = pas::checked_cast<aItem::TMicroModule*>(Left->Item)->MicroModuleIndex;
                if (cpp_left_5 < pas::checked_cast<aItem::TMicroModule*>(Right->Item)->MicroModuleIndex) {
                    return -1;
                }
            }
            {
                std::int32_t cpp_left_6 = pas::checked_cast<aItem::TMicroModule*>(Left->Item)->MicroModuleIndex;
                if (cpp_left_6 > pas::checked_cast<aItem::TMicroModule*>(Right->Item)->MicroModuleIndex) {
                    return 1;
                }
            }
        }
        if (Left->Item->Weight < Right->Item->Weight) {
            return -1;
        }
        if (Left->Item->Weight > Right->Item->Weight) {
            return 1;
        }
        if (Left->Item->Cost < Right->Item->Cost) {
            return -1;
        }
        if (Left->Item->Cost > Right->Item->Cost) {
            return 1;
        }
        return 0;
    }

    void TPlayer::SortStorageEntries() {
        std::int32_t I{};
        std::int32_t J{};
        PStorageEntry Temp{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 2); cpp_range.next(I); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, pas::list_count(StorageEntries) - 1); cpp_range_2.next(J); ) {
                PStorageEntry cpp_arg = pas::list_at<TStorageEntry>(StorageEntries, J);
                PStorageEntry cpp_arg_2 = pas::list_at<TStorageEntry>(StorageEntries, I);
                if (TPlayer::CompareStorageEntries(cpp_arg_2, cpp_arg) > 0) {
                    Temp = pas::list_at<TStorageEntry>(StorageEntries, I);
                    pas::list_put(StorageEntries, I, pas::list_get(StorageEntries, J));
                    pas::list_put(StorageEntries, J, static_cast<void*>(Temp));
                }
            }
        }
    }

    void TPlayer::RefreshStorageBubbles() {
        BuildStorageBubbles();
    }

    // Publishes paginated storage summaries, including deployed probes; uses the global player's bubble list.
    void TPlayer::BuildStorageBubbles() {
        std::int32_t I{};
        std::int32_t LineCount{};
        std::int32_t AddedLines{};
        std::int32_t Page{};
        pas::WideString Text{};
        pas::WideString Heading{};
        pas::WideString ConditionText{};
        pas::WideString Divider{};
        PStorageEntry Entry{};
        pas::Object* PreviousLocation{};
        SortStorageEntries();
        Divider = TPlayer::GetStorageDividerText();
        Page = 1;
        if (pas::list_count(StorageEntries) <= 0) {
            Heading = BuildTranclucatorStorageSummary(AddedLines);
            if (HasDeployedSatellites() || Heading.length() > 0) {
                Text = pas::concat_wide({aMyFunction::WrapTextInColor(aConst::LocalizedText(u"FormShip.StorageInfo.Main"_wref.get()), u"<color=0,255,0>"_w), u"\r\n"});
                Text = pas::concat_wide_reverse({BuildDeployedSatelliteSummary(AddedLines), Text});
                Text = pas::concat_wide({Text, Heading});
                Globals::AddOrUpdatePlayerBubble(9, aGalaxy::Galaxy->CurrentTurn, Text, u"sys_storage1"_wref.get());
            } else {
                Globals::RemovePlayerBubblePages(u"sys_storage"_wref.get(), 0);
            }
        } else {
            PreviousLocation = nullptr;
            LineCount = 0;
            Text = pas::concat_wide({aMyFunction::WrapTextInColor(pas::concat_wide({aConst::LocalizedText(u"FormShip.StorageInfo.Main"_wref.get()), u"onepage", TPlayer::GetStorageColumnHeaderText()}), u"<color=0,255,0>"_w), u"\r\n"});
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
                if (PreviousLocation != Entry->LocationOwner || LineCount > 40) {
                    if (LineCount > 40) {
                        if (Page == 1) {
                            aMyFunction::ReplaceTextToken(Text, u"onepage"_w, pas::concat_wide({u" (", aConst::LocalizedText(u"FormShip.StorageInfo.Page"_wref.get()), u" ", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Page), u"<color=255,0,255>"_w), u")"}), pas::WideString());
                        }
                        Globals::AddOrUpdatePlayerBubble(9, aGalaxy::Galaxy->CurrentTurn, Text, static_cast<pas::WideString>(pas::concat_ansi({"sys_storage", SysUtils::IntToStr(Page)})));
                        ++Page;
                        Text = pas::concat_wide({aMyFunction::WrapTextInColor(pas::concat_wide({aConst::LocalizedText(u"FormShip.StorageInfo.Main"_wref.get()), u" (", aConst::LocalizedText(u"FormShip.StorageInfo.Page"_wref.get()), u" ", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Page), u"<color=255,0,255>"_w), u")"}), u"<color=0,255,0>"_w), TPlayer::GetStorageColumnHeaderText(), u"\r\n"});
                        LineCount = 0;
                    }
                    if (pas::class_cast_if<aPlanet::TPlanet*>(Entry->LocationOwner) != nullptr) {
                        Heading = ([&] {
                            auto name = pas::borrow(pas::checked_cast<aPlanet::TPlanet*>(Entry->LocationOwner)->CurrentStar->Name);
                            pas::WideString localizedText = aConst::LocalizedText(u"FormShip.StorageInfo.Star"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText), pas::WideString(), u"<Star>"_w, name.get());
                        }());
                        Heading = pas::concat_wide_reverse({aMyFunction::WrapTextInColor(pas::concat_wide({pas::checked_cast<aPlanet::TPlanet*>(Entry->LocationOwner)->GetFullName(u" "_w), u"."}), u"<color=255,240,100>"_w), aMyFunction::WrapTextInColor(pas::concat_wide({Heading, u". "}), u"<color=255,240,100>"_w)});
                        Text = pas::concat_wide({Text, Divider, u"\r\n", u"<td=", pas::wide_int_to_str(StorageItemColumns[GR_Main::GiResourceVariant()].Heading), u"><align=center>", Heading, u"</align>", u"\r\n", Divider, u"\r\n"});
                        LineCount += 3;
                    } else if (pas::class_cast_if<aShip::TShip*>(Entry->LocationOwner) != nullptr) {
                        Heading = ([&] {
                            auto name_2 = pas::borrow(pas::checked_cast<aShip::TShip*>(Entry->LocationOwner)->CurrentStar->Name);
                            pas::WideString localizedText_2 = aConst::LocalizedText(u"FormShip.StorageInfo.Star"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText_2), pas::WideString(), u"<Star>"_w, name_2.get());
                        }());
                        Heading = pas::concat_wide_reverse({aMyFunction::WrapTextInColor(pas::concat_wide({pas::checked_cast<aShip::TShip*>(Entry->LocationOwner)->GetFullName(u" "_wref.get()), u"."}), u"<color=255,240,100>"_w), aMyFunction::WrapTextInColor(pas::concat_wide({Heading, u". "}), u"<color=255,240,100>"_w)});
                        Text = pas::concat_wide({Text, Divider, u"\r\n", u"<td=", pas::wide_int_to_str(StorageItemColumns[GR_Main::GiResourceVariant()].Heading), u"><align=center>", Heading, u"</align>", u"\r\n", Divider, u"\r\n"});
                        LineCount += 3;
                    }
                }
                PreviousLocation = Entry->LocationOwner;
                if (pas::class_cast_if<aItem::TEquipment*>(Entry->Item) != nullptr) {
                    ConditionText = pas::concat_wide({u" ", pas::checked_cast<aItem::TEquipment*>(Entry->Item)->GetConditionText(false)});
                } else {
                    ConditionText = pas::WideString();
                }
                Text = pas::concat_wide({Text, u"- ", Entry->Item->GetDisplayName(), ConditionText});
                Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(StorageItemColumns[GR_Main::GiResourceVariant()].Size), u"><align=right>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Entry->Item->Weight), u"<color=0,255,0>"_w), u"</align>"});
                Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(StorageItemColumns[GR_Main::GiResourceVariant()].Cost), u"><align=right>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Entry->Item->Cost), u"<color=0,255,255>"_w), u"</align>"});
                Text = pas::concat_wide({Text, u"\r\n"});
                ++LineCount;
            }
            if (HasDeployedSatellites()) {
                // Native computes the probe text once for its line count, then again below.
                BuildDeployedSatelliteSummary(AddedLines);
                if (AddedLines + LineCount > 45) {
                    if (Page == 1) {
                        aMyFunction::ReplaceTextToken(Text, u"onepage"_w, pas::concat_wide({u" (", aConst::LocalizedText(u"FormShip.StorageInfo.Page"_wref.get()), u" ", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Page), u"<color=255,0,255>"_w), u")"}), pas::WideString());
                    }
                    Globals::AddOrUpdatePlayerBubble(9, aGalaxy::Galaxy->CurrentTurn, Text, static_cast<pas::WideString>(pas::concat_ansi({"sys_storage", SysUtils::IntToStr(Page)})));
                    ++Page;
                    Text = pas::concat_wide({aMyFunction::WrapTextInColor(pas::concat_wide({aConst::LocalizedText(u"FormShip.StorageInfo.Main"_wref.get()), u" (", aConst::LocalizedText(u"FormShip.StorageInfo.Page"_wref.get()), u" ", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Page), u"<color=255,0,255>"_w), u")"}), u"<color=0,255,0>"_w), u"\r\n"});
                }
                if (HasDeployedSatellites()) {
                    Text = pas::concat_wide_reverse({BuildDeployedSatelliteSummary(AddedLines), Text});
                }
            }
            Heading = BuildTranclucatorStorageSummary(AddedLines);
            if (Heading.length() > 0) {
                if (AddedLines + LineCount > 45) {
                    if (Page == 1) {
                        aMyFunction::ReplaceTextToken(Text, u"onepage"_w, pas::concat_wide({u" (", aConst::LocalizedText(u"FormShip.StorageInfo.Page"_wref.get()), u" ", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Page), u"<color=255,0,255>"_w), u")"}), pas::WideString());
                    }
                    Globals::AddOrUpdatePlayerBubble(9, aGalaxy::Galaxy->CurrentTurn, Text, static_cast<pas::WideString>(pas::concat_ansi({"sys_storage", SysUtils::IntToStr(Page)})));
                    ++Page;
                    Text = pas::concat_wide({aMyFunction::WrapTextInColor(pas::concat_wide({aConst::LocalizedText(u"FormShip.StorageInfo.Main"_wref.get()), u" (", aConst::LocalizedText(u"FormShip.StorageInfo.Page"_wref.get()), u" ", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Page), u"<color=255,0,255>"_w), u")"}), u"<color=0,255,0>"_w), u"\r\n"});
                }
                Text = pas::concat_wide({Text, Heading});
            }
            if (Page == 1) {
                aMyFunction::ReplaceTextToken(Text, u"onepage"_w, pas::WideString(), pas::WideString());
            }
            Globals::AddOrUpdatePlayerBubble(9, aGalaxy::Galaxy->CurrentTurn, Text, static_cast<pas::WideString>(pas::concat_ansi({"sys_storage", SysUtils::IntToStr(Page)})));
        }
        Globals::RemovePlayerBubblePages(u"sys_storage"_wref.get(), Page + 1);
        Globals::RemovePlayerBubbleByKey(u"sys_storage"_wref.get());
    }

    // Returns a map ID or -1; updates shared map play-count scratch from the player's history.
    std::int32_t TPlayer::SelectPlanetBattleMap() {
        std::int32_t I{};
        std::int32_t MapIndex{};
        std::int32_t Count{};
        pas::DynArray<std::int32_t> Candidates{};
        std::int32_t Result = -1;
        if (GR_Main::IsInstallFeatureEnabled(u"Robot"_wref.get())) {
            if (aPlayer::GetPlayer()->IsOnPlanet() && (aGalaxy::Galaxy->BlazerSeriesResolvedTurn <= 0 || aGalaxy::Galaxy->KellerSeriesResolvedTurn <= 0 || aGalaxy::Galaxy->TerronSeriesResolvedTurn <= 0)) {
                if (LastPlanetBattleTurn <= static_cast<long double>(aGalaxy::Galaxy->CurrentTurn) - aMyFunction::RemapClamped(PlanetBattleHistory.length() - 1, 0.0, Globals::RobotMapDefinitions.length() - 1, 1.3E+2, 3.6E+2)) {
                    {
                        const std::int32_t cpp_last = Globals::RobotMapDefinitions.length() - 1;
                        if (0 <= cpp_last) {
                            for (I = 0; I <= cpp_last; ++I) {
                                Globals::RobotMapDefinitions[I].PlayerPlayCount = 0;
                            }
                        }
                    }
                    {
                        const std::int32_t cpp_last_2 = PlanetBattleHistory.length() - 1;
                        if (0 <= cpp_last_2) {
                            for (I = 0; I <= cpp_last_2; ++I) {
                                MapIndex = Globals::FindRobotMapById(PlanetBattleHistory[I].MapId);
                                if (MapIndex >= 0) {
                                    ++Globals::RobotMapDefinitions[MapIndex].PlayerPlayCount;
                                }
                            }
                        }
                    }
                    Candidates.set_length(Globals::RobotMapDefinitions.length() - 1 + 1);
                    Count = 0;
                    {
                        const std::int32_t cpp_last_3 = Globals::RobotMapDefinitions.length() - 1;
                        if (0 <= cpp_last_3) {
                            for (I = 0; I <= cpp_last_3; ++I) {
                                if (Globals::RobotMapDefinitions[I].PlanetRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Globals::RobotMapDefinitions[I].PlanetRace, CurrentPlanet->RaceId) ^ 1)) {
                                    continue;
                                }
                                if (Globals::RobotMapDefinitions[I].PlayerRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Globals::RobotMapDefinitions[I].PlayerRace, PilotRace) ^ 1)) {
                                    continue;
                                }
                                if (Globals::RobotMapDefinitions[I].PlayerStatus != pas::constant_set<Globals::TRobotMapPlayerStatuses>({}) && static_cast<std::uint8_t>(pas::contains(Globals::RobotMapDefinitions[I].PlayerStatus, static_cast<std::uint8_t>(GetDominantCareer())) ^ 1)) {
                                    continue;
                                }
                                if (PlanetBattleHistory.length() - 1 == -1) {
                                    if (Globals::RobotMapDefinitions[I].MinWins != 0 || Globals::RobotMapDefinitions[I].MaxWins != 0) {
                                        continue;
                                    }
                                } else {
                                    MapIndex = PlanetBattleHistory.length() - 1 + 1;
                                    if (!((Globals::RobotMapDefinitions[I].MinWins <= MapIndex || Globals::RobotMapDefinitions[I].MinWins == 0) && (Globals::RobotMapDefinitions[I].MaxWins >= MapIndex || Globals::RobotMapDefinitions[I].MaxWins == 0))) {
                                        continue;
                                    }
                                    if (Globals::RobotMapDefinitions[I].AfterLiberation) {
                                        if (!(aGalaxy::Galaxy->CurrentTurn - aPlayer::GetPlayer()->CurrentStar->LastLiberationRewardsTurn <= 40 && aPlayer::GetPlayer()->CurrentStar->LastLiberationRewardsTurn <= aPlayer::GetPlayer()->CurrentStar->LastDominatorPresenceTurn)) {
                                            continue;
                                        }
                                    } else if (!pas::in_range(aGalaxy::Galaxy->CurrentTurn - aPlayer::GetPlayer()->CurrentStar->LastDominatorPresenceTurn, 30, 120)) {
                                        continue;
                                    }
                                }
                                if (Globals::RobotMapDefinitions[I].PlayerPlayCount < Globals::RobotMapDefinitions[I].Reiteration && static_cast<std::uint8_t>(Globals::RobotMapDefinitions[I].Terron ^ 1) && static_cast<std::uint8_t>(Globals::RobotMapDefinitions[I].Demo ^ 1) && (Count <= 0 || Globals::RobotMapDefinitions[I].PlayerPlayCount <= Globals::RobotMapDefinitions[Candidates[0]].PlayerPlayCount)) {
                                    if (Count > 0 && Globals::RobotMapDefinitions[I].PlayerPlayCount < Globals::RobotMapDefinitions[Candidates[0]].PlayerPlayCount) {
                                        Count = 0;
                                    }
                                    Candidates[Count] = I;
                                    ++Count;
                                }
                            }
                        }
                    }
                    if (Count <= 0) {
                        Candidates = nullptr;
                        return Result;
                    }
                    Result = Globals::RobotMapDefinitions[Candidates[aMyFunction::SeededRandomIntRange(0, Count - 1, CurrentPlanet->GenerationSeed)]].Id;
                    Candidates = nullptr;
                }
            }
        }
        return Result;
    }

    // Index is zero-based and unchecked; the selected preset is unchanged.
    void TPlayer::SaveEquipmentConfiguration(std::int32_t Index) {
        std::int32_t I{};
        std::int32_t NextSlot{};
        std::int32_t Slot{};
        aItem::TWeapon* Weapon{};
        aItem::TEquipment* Item{};
        aItem::TArtefact* Artefact{};
        {
            TEquipmentConfiguration& cpp_with = EquipmentConfigurations[Index];
            for (I = 0; I <= 11; ++I) {
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.EquipmentIds, I * sizeof(std::int32_t)), 0);
            }
            for (I = 0; I <= 31; ++I) {
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.ArtefactIds, I * sizeof(std::int32_t)), 0);
            }
            for (I = 1; I <= 5; ++I) {
                Weapon = Weapons[I];
                if (Weapon == nullptr || Weapon->EquippedFlag == 0) {
                    continue;
                }
                Slot = Weapon->AssignedSlotData & aItem::EquipmentSlotIndexMask;
                if (Slot >= 0 && Slot <= 4) {
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.EquipmentIds, Slot * sizeof(std::int32_t)), Weapon->Id);
                }
            }
            NextSlot = 5;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) && Item->EquippedFlag != 0 && aConst::ItemTypeToSlotKind(static_cast<std::uint8_t>(Item->ItemType)) != aConst::sskUnsupported) {
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.EquipmentIds, NextSlot * sizeof(std::int32_t)), Item->Id);
                    ++NextSlot;
                    if (NextSlot > 11) {
                        break;
                    }
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
                Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
                if (Artefact->EquippedFlag == 0) {
                    continue;
                }
                Slot = Artefact->AssignedSlotData & aItem::EquipmentSlotIndexMask;
                if (Slot >= 0 && aConst::DefaultHullSlotCounts[8] > Slot) {
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.ArtefactIds, Slot * sizeof(std::int32_t)), Artefact->Id);
                }
            }
        }
    }

    // Can draw from storage at the current dock. Index is unchecked; the selected preset is unchanged.
    void TPlayer::ApplyEquipmentConfiguration(std::int32_t Index) {
        // Nested in ApplyEquipmentConfiguration; caller-popped static link, player -4 and preset index -8.
        auto IsEmpty = [&]() -> std::uint8_t {
            std::int32_t I{};
            std::uint8_t Result = true;
            {
                TEquipmentConfiguration& cpp_with = this->EquipmentConfigurations[Index];
                for (I = 0; I <= 11; ++I) {
                    if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.EquipmentIds, I * sizeof(std::int32_t))) != 0) {
                        return false;
                    }
                }
                for (I = 0; I <= 31; ++I) {
                    if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.ArtefactIds, I * sizeof(std::int32_t))) != 0) {
                        return false;
                    }
                }
                return Result;
            }
        };
        // Nested in ApplyEquipmentConfiguration; caller-popped static link, player -4 and preset index -8.
        auto UnequipAll = [&]() -> void {
            std::int32_t I{};
            aItem::TEquipment* Item{};
            aItem::TArtefact* Artefact{};
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(this->Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(this->Inventory, I);
                if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    Item->Unequip();
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(this->Artefacts) - 1); cpp_range_2.next(I); ) {
                Artefact = pas::list_at<aItem::TArtefact>(this->Artefacts, I);
                Artefact->Unequip();
            }
            this->WeaponCount = 0;
            for (I = 1; I <= 5; ++I) {
                this->Weapons[I] = nullptr;
            }
        };
        // Nested in ApplyEquipmentConfiguration; caller-popped static link, player -4 and preset index -8.
        auto EquipCarriedItems = [&]() -> void {
            std::int32_t I{};
            std::int32_t Slot{};
            aItem::TEquipment* Equipment{};
            aItem::TArtefact* Artefact{};
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(this->Inventory) - 1); cpp_range.next(I); ) {
                Equipment = pas::list_at<aItem::TEquipment>(this->Inventory, I);
                Slot = aPlayer::FindSlot(Equipment, this, Index);
                if (Slot >= 0) {
                    Equipment->Equip();
                    Equipment->AssignedSlotData = Equipment->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | static_cast<std::uint32_t>(Slot);
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(this->Artefacts) - 1); cpp_range_2.next(I); ) {
                Artefact = pas::list_at<aItem::TArtefact>(this->Artefacts, I);
                Slot = aPlayer::FindSlot(Artefact, this, Index);
                if (Slot >= 0) {
                    Artefact->Equip();
                    Artefact->AssignedSlotData = Artefact->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | static_cast<std::uint32_t>(Slot);
                }
            }
        };
        // Nested in ApplyEquipmentConfiguration; caller-popped static link, player -4 and preset index -8.
        auto EquipStoredItems = [&]() -> void {
            std::int32_t I{};
            std::int32_t Slot{};
            PStorageEntry Entry{};
            if (this->StorageEntries != nullptr && (IsDockedToShip() || IsOnPlanet())) {
                const std::int32_t cpp_first = pas::list_count(this->StorageEntries) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        Entry = pas::list_at<TStorageEntry>(this->StorageEntries, I);
                        if (Entry != nullptr && Entry->Item != nullptr && (this->DockedTo == Entry->LocationOwner || this->CurrentPlanet == Entry->LocationOwner)) {
                            Slot = aPlayer::FindSlot(Entry->Item, this, Index);
                            if (Slot >= 0) {
                                aPlayer::TakeStoredItem(Entry, Slot, this);
                            }
                        }
                    }
                }
            }
        };
        if (!IsEmpty()) {
            UnequipAll();
            EquipCarriedItems();
            EquipStoredItems();
            RebuildEquipmentCache();
        }
    }

    std::uint8_t SupportsItem(aItem::TItem* Item) {
        std::uint8_t Result = true;
        if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TFuelTanks*>(Item) != nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TEngine*>(Item) != nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TRadar*>(Item) != nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TScaner*>(Item) != nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TRepairRobot*>(Item) != nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TCargoHook*>(Item) != nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TDefGenerator*>(Item) != nullptr) {
            return Result;
        }
        return false;
    }

    std::int32_t FindSlot(aItem::TItem* Item, TPlayer* Self, std::int32_t& Index) {
        std::int32_t I{};
        std::int32_t Result = -1;
        if (aPlayer::SupportsItem(Item)) {
            TEquipmentConfiguration& cpp_with = Self->EquipmentConfigurations[Index];
            if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                for (I = 0; I <= 31; ++I) {
                    if (pas::checked_cast<aItem::TArtefact*>(Item)->Id == pas::load_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.ArtefactIds, I * sizeof(std::int32_t)))) {
                        return I;
                    }
                }
            } else if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr) {
                for (I = 0; I <= 11; ++I) {
                    if (pas::checked_cast<aItem::TEquipment*>(Item)->Id == pas::load_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.EquipmentIds, I * sizeof(std::int32_t)))) {
                        return I;
                    }
                }
            }
            return Result;
        }
        return Result;
    }

    void ReleaseStorageEntry(PStorageEntry Entry) {
        Entry->Item = nullptr;
        pas::list_delete(aPlayer::GetPlayer()->StorageEntries, pas::list_indexof(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry)));
        pas::dispose(Entry);
    }

    void TakeStoredItem(PStorageEntry Entry, std::int32_t Slot, TPlayer* Self) {
        aItem::TArtefact* Artefact{};
        aItem::TEquipment* Equipment{};
        aItem::TItem* Item = Entry->Item;
        if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
            pas::list_add(Self->Artefacts, reinterpret_cast<void*>(Item));
            aPlayer::ReleaseStorageEntry(Entry);
            Artefact = pas::checked_cast<aItem::TArtefact*>(Item);
            Artefact->Equip();
            Artefact->AssignedSlotData = Artefact->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | static_cast<std::uint32_t>(Slot);
        } else if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr) {
            pas::list_add(Self->Inventory, reinterpret_cast<void*>(Item));
            aPlayer::ReleaseStorageEntry(Entry);
            Equipment = pas::checked_cast<aItem::TEquipment*>(Item);
            Equipment->Equip();
            if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                Equipment->AssignedSlotData = Equipment->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | static_cast<std::uint32_t>(Slot);
            }
        }
    }

    // Does not validate Index.
    std::uint8_t TPlayer::HasEquipmentConfiguration(std::int32_t Index) {
        std::int32_t I{};
        std::uint8_t Result = false;
        {
            TEquipmentConfiguration& cpp_with = EquipmentConfigurations[Index];
            for (I = 0; I <= 11; ++I) {
                if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.EquipmentIds, I * sizeof(std::int32_t))) != 0) {
                    return true;
                }
            }
            for (I = 0; I <= 31; ++I) {
                if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.ArtefactIds, I * sizeof(std::int32_t))) != 0) {
                    return true;
                }
            }
            return Result;
        }
    }

    // Includes the carrier's hold and the player's current-location storage.
    std::int32_t TPlayer::GetAvailableNodeCount(aShip::TShip* Carrier) {
        std::int32_t Result{};
        std::int32_t I{};
        PStorageEntry Entry{};
        if (Carrier != nullptr) {
            Result = Carrier->GetCarriedNodeCount();
        } else {
            Result = GetCarriedNodeCount();
        }
        if (IsOnPlanet()) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
                if (Entry->LocationOwner == CurrentPlanet && Entry->Item->ItemType == aConst::t_Protoplasm) {
                    Result += Entry->Item->Weight;
                }
            }
        }
        if (IsDockedToShip()) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(StorageEntries) - 1); cpp_range_2.next(I); ) {
                Entry = pas::list_at<TStorageEntry>(StorageEntries, I);
                if (Entry->LocationOwner == DockedTo && Entry->Item->ItemType == aConst::t_Protoplasm) {
                    Result += Entry->Item->Weight;
                }
            }
        }
        return Result;
    }

    // Uses the carrier's hold, then current-location storage; refreshes Self even when Carrier differs.
    void TPlayer_ConsumeAvailableNodes(TPlayer* Self, std::int32_t Count, aShip::TShip* Carrier) {
        std::int32_t I{};
        std::int32_t Remaining{};
        aItem::TItem* Item{};
        PStorageEntry Entry{};
        if (Count <= 0) {
            return;
        }
        if (Carrier == nullptr) {
            Carrier = Self;
        }
        {
            const std::int32_t cpp_first = pas::list_count(Carrier->Inventory) - 1;
            if (cpp_first >= 1) {
                for (I = cpp_first; I >= 1; --I) {
                    Item = pas::list_at<aItem::TItem>(Carrier->Inventory, I);
                    if (Item->ItemType == aConst::t_Protoplasm) {
                        if (Item->Weight > Count) {
                            Remaining = Item->Weight - Count;
                            pas::checked_cast<aItem::TProtoplasm*>(Item)->StackCount -= Count;
                            Item->Cost = System::Round(pas::real_divide(Item->Cost, Item->Weight) * Remaining);
                            Item->Weight = Remaining;
                            Count = 0;
                        } else {
                            Count -= Item->Weight;
                            pas::list_delete(Carrier->Inventory, I);
                            pas::free(Item);
                        }
                    }
                    if (Count == 0) {
                        break;
                    }
                }
            }
        }
        if (Self->IsOnPlanet() && Count > 0) {
            const std::int32_t cpp_first_2 = pas::list_count(Self->StorageEntries) - 1;
            if (cpp_first_2 >= 0) {
                for (I = cpp_first_2; I >= 0; --I) {
                    Entry = pas::list_at<TStorageEntry>(Self->StorageEntries, I);
                    if (Entry->LocationOwner == Self->CurrentPlanet && Entry->Item->ItemType == aConst::t_Protoplasm) {
                        if (Entry->Item->Weight > Count) {
                            Remaining = Entry->Item->Weight - Count;
                            pas::checked_cast<aItem::TProtoplasm*>(Entry->Item)->StackCount -= Count;
                            Entry->Item->Cost = System::Round(pas::real_divide(Entry->Item->Cost, Entry->Item->Weight) * Remaining);
                            Entry->Item->Weight = Remaining;
                            Count = 0;
                        } else {
                            Count -= Entry->Item->Weight;
                            pas::list_delete(Self->StorageEntries, I);
                            pas::free(Entry->Item);
                            pas::dispose(Entry);
                        }
                    }
                    if (Count == 0) {
                        break;
                    }
                }
            }
        }
        if (Self->IsDockedToShip() && Count > 0) {
            const std::int32_t cpp_first_3 = pas::list_count(Self->StorageEntries) - 1;
            if (cpp_first_3 >= 0) {
                for (I = cpp_first_3; I >= 0; --I) {
                    Entry = pas::list_at<TStorageEntry>(Self->StorageEntries, I);
                    if (Entry->LocationOwner == Self->DockedTo && Entry->Item->ItemType == aConst::t_Protoplasm) {
                        if (Entry->Item->Weight > Count) {
                            Remaining = Entry->Item->Weight - Count;
                            pas::checked_cast<aItem::TProtoplasm*>(Entry->Item)->StackCount -= Count;
                            Entry->Item->Cost = System::Round(pas::real_divide(Entry->Item->Cost, Entry->Item->Weight) * Remaining);
                            Entry->Item->Weight = Remaining;
                            Count = 0;
                        } else {
                            Count -= Entry->Item->Weight;
                            pas::list_delete(Self->StorageEntries, I);
                            pas::free(Entry->Item);
                            pas::dispose(Entry);
                        }
                    }
                    if (Count == 0) {
                        break;
                    }
                }
            }
        }
        Self->RefreshDerivedStats(true);
    }

    // Pirate career thresholds, eminent title and active license.
    std::int32_t TPlayer::GetMaxPiratePartners() {
        std::int32_t Result = 0;
        if (CareerStatus[aGalaxyStruct::rcPirate] > 60) {
            ++Result;
        }
        if (CareerStatus[aGalaxyStruct::rcPirate] > 75) {
            ++Result;
        }
        if (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate] == this) {
            ++Result;
        }
        if (PirateLicenseTicks > 0) {
            ++Result;
        }
        return Result;
    }

    // As pirate partners, with an additional threshold above career status 50.
    std::int32_t TPlayer::GetMaxDominionShips() {
        std::int32_t Result = 0;
        if (CareerStatus[aGalaxyStruct::rcPirate] > 50) {
            ++Result;
        }
        if (CareerStatus[aGalaxyStruct::rcPirate] > 60) {
            ++Result;
        }
        if (CareerStatus[aGalaxyStruct::rcPirate] > 75) {
            ++Result;
        }
        if (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate] == this) {
            ++Result;
        }
        if (PirateLicenseTicks > 0) {
            ++Result;
        }
        return Result;
    }

    // Dates the new record with the current turn.
    void TPlayer::AddJournalRecord(pas::WideString Text) {
        TJournalRecord* Entry = pas::construct_call<TJournalRecord>(TJournalRecord_Create);
        pas::list_add(JournalRecords, reinterpret_cast<void*>(Entry));
        Entry->Text = std::move(Text);
        Entry->DateTurn = aGalaxy::Galaxy->CurrentTurn;
    }

    // Native guard accepts Index=Count, leaving the list accessor to raise.
    void TPlayer::DeleteJournalRecord(std::int32_t Index) {
        if (Index >= 0 && pas::list_count(JournalRecords) >= Index) {
            pas::free(pas::list_at<TJournalRecord>(JournalRecords, Index));
            pas::list_delete(JournalRecords, Index);
        }
    }

    void TPlayer::ClearJournal() {
        while (pas::list_count(JournalRecords) > 0) {
            DeleteJournalRecord(pas::list_count(JournalRecords) - 1);
        }
    }

    pas::WideString TPlayer::ExportJournal() {
        std::int32_t I{};
        pas::AnsiString Contents{};
        pas::AnsiString FileName{};
        TJournalRecord* Entry{};
        std::uint16_t Year{};
        std::uint16_t Month{};
        std::uint16_t Day{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(JournalRecords) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TJournalRecord>(JournalRecords, I);
            Contents = static_cast<pas::AnsiString>(pas::concat_wide({static_cast<pas::WideString>(Contents), aGalaxy::Galaxy->FormatTurnDate(Entry->DateTurn), u"\r\n"}));
            Contents = static_cast<pas::AnsiString>(pas::concat_wide({static_cast<pas::WideString>(Contents), EC_Str::RemoveTextTagsW(Entry->Text), u"\r\n"}));
            Contents = pas::concat_ansi({Contents, "\r\n"});
        }
        SysUtilsImports::DecodeDate(aGalaxy::Galaxy->TurnToDateTime(-1), Year, Month, Day);
        FileName = static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Save\\Journal_", static_cast<pas::WideString>(pas::format("%d_%.2d_%.2d"_a, static_cast<std::int32_t>(Year), static_cast<std::int32_t>(Month), static_cast<std::int32_t>(Day))), u".txt"}));
        GR_Main::WriteTextFileThreadSafe(FileName, Contents);
        return static_cast<pas::WideString>(FileName);
    }

    // Ascending ID, using pairwise swaps.
    void TPlayer::SortNewsEntries() {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::PPlanetNewsEntry First{};
        aGalaxy::PPlanetNewsEntry Second{};
        void* Temp{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(NewsEntries) - 1); cpp_range.next(I); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, pas::list_count(NewsEntries) - 1); cpp_range_2.next(J); ) {
                First = pas::list_at<aGalaxyStruct::TPlanetNews>(NewsEntries, I);
                Second = pas::list_at<aGalaxyStruct::TPlanetNews>(NewsEntries, J);
                if (First->Id > Second->Id) {
                    Temp = pas::list_get(NewsEntries, I);
                    pas::list_put(NewsEntries, I, pas::list_get(NewsEntries, J));
                    pas::list_put(NewsEntries, J, Temp);
                }
            }
        }
    }

    // Removes and finalizes the oldest Count-KeepCount entries; unchecked argument.
    void TPlayer::TrimNewsEntries(std::int32_t KeepCount) {
        std::int32_t I{};
        aGalaxy::PPlanetNewsEntry Entry{};
        {
            const std::int32_t cpp_first = pas::list_count(NewsEntries) - KeepCount - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Entry = pas::list_at<aGalaxyStruct::TPlanetNews>(NewsEntries, I);
                    pas::list_delete(NewsEntries, I);
                    pas::dispose(Entry);
                }
            }
        }
    }

    pas::WideString TPlayer::ExportNews() {
        std::int32_t I{};
        pas::AnsiString Contents{};
        pas::AnsiString FileName{};
        aGalaxy::PPlanetNewsEntry Entry{};
        std::uint16_t Year{};
        std::uint16_t Month{};
        std::uint16_t Day{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(NewsEntries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<aGalaxyStruct::TPlanetNews>(NewsEntries, I);
            Contents = static_cast<pas::AnsiString>(pas::concat_wide({static_cast<pas::WideString>(Contents), aGalaxy::Galaxy->FormatTurnDate(Entry->Turn), u"\r\n"}));
            Contents = static_cast<pas::AnsiString>(pas::concat_wide({static_cast<pas::WideString>(Contents), EC_Str::RemoveTextTagsW(Entry->Text), u"\r\n"}));
            Contents = pas::concat_ansi({Contents, "\r\n"});
        }
        SysUtilsImports::DecodeDate(aGalaxy::Galaxy->TurnToDateTime(-1), Year, Month, Day);
        FileName = pas::concat_ansi({"Save\\News_", pas::format("%d_%.2d_%.2d"_a, static_cast<std::int32_t>(Year), static_cast<std::int32_t>(Month), static_cast<std::int32_t>(Day)), ".txt"});
        GR_Main::WriteTextFileThreadSafe(FileName, Contents);
        return static_cast<pas::WideString>(FileName);
    }

    // Copies galaxy news whose IDs are absent locally.
    void TPlayer::MergeGalaxyNews() {
        std::int32_t I{};
        std::int32_t J{};
        std::uint8_t Found{};
        aGalaxy::PPlanetNewsEntry Source{};
        aGalaxy::PPlanetNewsEntry Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->PlanetNews) - 1); cpp_range.next(I); ) {
            Source = pas::list_at<aGalaxyStruct::TPlanetNews>(aGalaxy::Galaxy->PlanetNews, I);
            Found = false;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(NewsEntries) - 1); cpp_range_2.next(J); ) {
                Entry = pas::list_at<aGalaxyStruct::TPlanetNews>(NewsEntries, J);
                if (Source->Id == Entry->Id) {
                    Found = true;
                    break;
                }
            }
            if (!Found) {
                pas::new_value(Entry);
                Entry->Id = Source->Id;
                Entry->Turn = Source->Turn;
                Entry->NewsType = Source->NewsType;
                Entry->Text = Source->Text;
                pas::list_add(NewsEntries, static_cast<void*>(Entry));
            }
        }
    }

    // Updates eligible docked players after turn 300 and retains the newest 100 entries.
    void TPlayer_RefreshNewsAtLocation(TPlayer* Self) {
        if (aGalaxy::Galaxy->CurrentTurn > 300 && (Self->IsOnPlanet() || Self->IsDockedToShip())) {
            if (Self->CurrentPlanet == nullptr || pas::in_set<0, 4, 7, 7>(Self->CurrentPlanet->OwnerId) && Self->CurrentPlanet->GetRelationLevelToShip(Self) > aGalaxyStruct::rlBad) {
                Self->MergeGalaxyNews();
                Self->SortNewsEntries();
                Self->TrimNewsEntries(100);
            }
        }
    }

    std::int32_t TPlayer::CalculateSpeed() {
        return aShip::TShip::CalculateSpeed();
    }

    // Creates a military-base proxy and removes it from the ordinary system ship list.
    void TPlayer::CreateRuinsProxy() {
        RuinsProxy = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
        pas::checked_cast<aRuins::TRuins*>(RuinsProxy)->Init(aGalaxyStruct::rstMilitaryBase, aPlayer::GetPlayer()->CurrentStar, pas::WideString());
        pas::list_delete(CurrentStar->Ships, pas::list_indexof(CurrentStar->Ships, reinterpret_cast<void*>(RuinsProxy)));
    }

    // Nonpositive Mode uses the installed hull's CapitalShip kind.
    void TPlayer::EnterRuinsMode(std::int32_t Mode) {
        std::int32_t SelectedMode{};
        if (InHyperspace) {
            return;
        }
        if (Mode > 0) {
            SelectedMode = Mode;
        } else {
            SelectedMode = GetHull()->CapitalShip;
        }
        if (SelectedMode == 0) {
            return;
        }
        if (RuinsMode == 0) {
            if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                fEquipmentShop::RestoreTemporaryShopStock();
            }
            RuinsSavedPlanet = CurrentPlanet;
            RuinsSavedDockedTo = DockedTo;
            if (RuinsProxy == nullptr) {
                CreateRuinsProxy();
            }
            DockedTo = RuinsProxy;
            CurrentPlanet = nullptr;
            RuinsProxy->CurrentStar = CurrentStar;
            fEquipmentShop::BuildTemporaryShopSlotGrid();
        }
        RuinsMode = SelectedMode;
        GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
        reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
    }

    // Returns to the saved location or star screen and requests screen closure.
    void TPlayer::CloseRuinsModeScreen() {
        RuinsStatusText = pas::WideString();
        if (RuinsSavedPlanet == nullptr && RuinsSavedDockedTo == nullptr) {
            GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
            if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(403);
                CurrentPlanet = nullptr;
                DockedTo = RuinsProxy;
                fEquipmentShop::RestoreTemporaryShopStock();
                aGalaxy::Galaxy->PrimeIntegrityChecksum(404);
            }
        } else {
            if (RuinsSavedDockedTo != nullptr) {
                GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
            } else if (RuinsSavedPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
            } else {
                GlobalsV::RequestedScreenId = GlobalsV::screenPlanetNO;
            }
            ExitRuinsMode();
        }
        reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
    }

    // Restores the real docking target and rebuilds its temporary shop stock.
    void TPlayer::ExitRuinsMode() {
        aGalaxy::Galaxy->CheckIntegrityChecksum(403);
        if (fEquipmentShop::TemporaryShopSlots != nullptr) {
            CurrentPlanet = nullptr;
            DockedTo = RuinsProxy;
            fEquipmentShop::RestoreTemporaryShopStock();
        }
        CurrentPlanet = RuinsSavedPlanet;
        RuinsSavedPlanet = nullptr;
        DockedTo = RuinsSavedDockedTo;
        RuinsSavedDockedTo = nullptr;
        RuinsMode = 0;
        fEquipmentShop::BuildTemporaryShopSlotGrid();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(404);
    }

    // Includes the player's current-system kill counts and main pirate planet exception.
    void TPlayer_RefreshCurrentStanding(TPlayer* Self) {
        if (aShip::TShip_IsInPrison(Self)) {
            Self->CurrentStanding = aGalaxyStruct::ssNeutral;
        } else if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            if (Self->CurrentSystemKills.Pirate > 0 || Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                Self->CurrentStanding = aGalaxyStruct::ssCoalitionActive;
            } else {
                Self->CurrentStanding = aGalaxyStruct::ssCoalitionPassive;
            }
        } else if (Self->CurrentSystemKills.Normal > 0 || Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            Self->CurrentStanding = aGalaxyStruct::ssPirateActive;
        } else {
            Self->CurrentStanding = aGalaxyStruct::ssPiratePassive;
        }
        if (Self->CurrentStanding == aGalaxyStruct::ssCoalitionActive && aPlanet::MainPiratePlanet != nullptr && aPlanet::MainPiratePlanet->CurrentStar == Self->CurrentStar) {
            Self->CurrentStanding = aGalaxyStruct::ssCoalitionMilitary;
        }
    }

    // Honors scripted targeting restrictions, chameleon logic and friendly station standing masks.
    std::uint8_t TPlayer::CanSelectShipTarget(aShip::TShip* Ship) {
        std::uint8_t Faction{};
        std::uint8_t Result = false;
        if (Ship->TargetingRestriction == 1 && Ship->EnemyShip != this && EnemyShip != Ship) {
            return Result;
        }
        if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr && ChameleonLogic[reinterpret_cast<aKling::TKling*>(Ship)->DominatorSeries] >= 2 && Ship->EnemyShip != this && EnemyShip != Ship) {
            return Result;
        }
        if (pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
            for (Faction = static_cast<std::uint8_t>(0); Faction <= static_cast<std::uint8_t>(2); ++Faction) {
                if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(pas::byte_offset(&aConst::PlanetOwnerMasks, Faction * sizeof(aGalaxyStruct::TOwnerMask))), OwnerId) && pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::NonTargetableStationStandingMasks[Faction]), Ship->CurrentStanding) && (Ship->ScriptShip == nullptr || pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(pas::byte_offset(&aConst::PlanetOwnerMasks, Faction * sizeof(aGalaxyStruct::TOwnerMask))), Ship->OwnerId))) {
                    return Result;
                }
            }
        }
        return true;
    }

    // Checks station/Dominator restrictions and invokes the player's scan-permission item scripts.
    std::uint8_t TPlayer::CanScanShip(aShip::TShip* Ship) {
        std::uint8_t Result = true;
        if (aGalaxy::Galaxy->UltraScanModEnabled == 0) {
            if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) {
                Result = false;
            } else if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr && static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1)) {
                if (aPlayer::GetPlayer()->GetScanner() == nullptr || aPlayer::GetPlayer()->GetScanner()->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || aPlayer::GetPlayer()->GetScanner()->DominatorSeries != reinterpret_cast<aKling::TKling*>(Ship)->DominatorSeries) {
                    Result = false;
                }
            }
            return aPlayer::GetPlayer()->ScriptItemsAct(0x00000012, Ship, nullptr, Result) != 0;
        }
        return Result;
    }

    void TJournalRecord::p_destroy() {
        aPlayer::TJournalRecord_Destroy(this);
    }

    void TPlayer::p_destroy() {
        aPlayer::TPlayer_Destroy(this);
    }

    void TPlayer::virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aPlayer::TPlayer_ResolveLoadedReferences(this, Galaxy);
    }

    void TPlayer::virtual_TShip_NextDay() {
        aPlayer::TPlayer_NextDay(this);
    }

    void TPlayer::virtual_TShip_RefreshCurrentStanding() {
        aPlayer::TPlayer_RefreshCurrentStanding(this);
    }

} // namespace aPlayer
