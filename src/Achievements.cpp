#include "layout/Achievements.hpp"
#include "types/System.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aMyFunction.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/aTranclucator.hpp"
#include "units/Achievements.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Buf.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/NoSteamAchievemens.hpp"
#include "units/SimpleSteamApi.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/WStringUtils.hpp"
#include "units/aGalaxy.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"

namespace Achievements {
    const Achievements::TAchievementDefinitionTable AchievementDefinitionTable = Achievements::TAchievementDefinitionTable{{
        {.Key = "NONE"_a}, {.Key = "AGENT"_a, .MaxValue = 50}, {.Key = "ARCHEOLOGY"_a, .MaxValue = 30000},
        {.Key = "BLACKHEAD"_a, .MaxValue = 200}, {.Key = "BREZHNEV"_a}, {.Key = "BUMMER"_a}, {.Key = "CHAMPION"_a},
        {.Key = "DOLGOZHID"_a}, {.Key = "HOLEMAN"_a, .MaxValue = 500}, {.Key = "ILL"_a}, {.Key = "KIBERMAN"_a},
        {.Key = "MANYFACES"_a, .MaxValue = 30}, {.Key = "MONEY"_a}, {.Key = "NARKOMAN"_a},
        {.Key = "OLDFAG"_a, .MaxValue = 5000000}, {.Key = "PEACELOVER"_a}, {.Key = "PIECECREATOR"_a},
        {.Key = "PRISON"_a, .MaxValue = 10}, {.Key = "SHIELD"_a, .MaxValue = 500}, {.Key = "SPEED"_a},
        {.Key = "SPRINTER"_a}, {.Key = "TERMINATOR"_a}, {.Key = "MASTER"_a}, {.Key = "POSTMAN"_a, .MaxValue = 10},
        {.Key = "HULL"_a}, {.Key = "PIRATE"_a, .MaxValue = 10}, {.Key = "FRY"_a, .MaxValue = 10},
        {.Key = "COALLITION"_a}, {.Key = "DEALER"_a, .MaxValue = 5000000}, {.Key = "JUMPER"_a, .MaxValue = 500},
        {.Key = "DEFENDER"_a, .MaxValue = 20}, {.Key = "NEGOCIANT"_a, .MaxValue = 15}, {.Key = "HATER"_a},
        {.Key = "CREDITOR"_a, .MaxValue = 3}, {.Key = "HOLEPEACE"_a}, {.Key = "SKILL"_a},
        {.Key = "GUARD"_a, .MaxValue = 30}, {.Key = "SCIENCE"_a}, {.Key = "IRONMAN"_a, .MaxValue = 40},
        {.Key = "BOMBER"_a}, {.Key = "ROCKET"_a, .MaxValue = 150}, {.Key = "CONTRABAND"_a, .MaxValue = 200},
        {.Key = "ASTEROID"_a, .MaxValue = 100}, {.Key = "QUEST"_a, .MaxValue = 30}, {.Key = "KELLERRESEARCH"_a},
        {.Key = "KELLERDESTROY"_a}, {.Key = "BLAZERPROGRAM"_a}, {.Key = "BLAZERPIECE"_a}, {.Key = "TERRONSTAR"_a},
        {.Key = "TERRONBATTLE"_a}, {.Key = "PIRATESYSTEMS"_a}, {.Key = "NODES"_a}, {.Key = "RATING"_a},
        {.Key = "RUINS"_a, .MaxValue = 20}, {.Key = "PIRATEWIN"_a}, {.Key = "BEST"_a}, {.Key = "COMMANDOR"_a},
        {.Key = "BARON"_a}, {.Key = "GIRLSQUEST"_a}, {.Key = "GIRLSHIRE"_a}, {.Key = "SHU"_a},
        {.Key = "SIDECHANGER"_a, .MaxValue = 15}, {.Key = "ENERGY"_a, .MaxValue = 250}, {.Key = "SCRATCHDAMAGE"_a},
        {.Key = "SPLINTER"_a, .MaxValue = 200}, {.Key = "EXPLORER"_a, .MaxValue = 100}, {.Key = "TRANCLUCATORS"_a},
        {.Key = "SUNFUEL"_a}, {.Key = "TERRORIST"_a, .MaxValue = 30}, {.Key = "COUNTERTERRORIST"_a, .MaxValue = 30},
        {.Key = "BLUEKILLS"_a, .MaxValue = 500}, {.Key = "GREENKILLS"_a, .MaxValue = 500},
        {.Key = "REDKILLS"_a, .MaxValue = 500}, {.Key = "BERTORSLAYER"_a, .MaxValue = 50}, {.Key = "MAPBUILDER"_a},
        {.Key = "HACKER"_a, .MaxValue = 100}, {.Key = "DELIVERY"_a, .MaxValue = 50}, {.Key = "INVESTOR"_a},
        {.Key = "INSURANCE"_a}, {.Key = "PRISONBAIL"_a, .MaxValue = 30}, {.Key = "ROBBER"_a, .MaxValue = 100},
        {.Key = "WARRIORKILLS"_a, .MaxValue = 100}, {.Key = "DRAIN"_a, .MaxValue = 10000},
    }};

    EC_BlockPar::TBlockParEC* AchievementDefinitions = nullptr;

    std::int32_t GetCurrentAchievementProgress(pas::WideString Key, std::int32_t StoredValue) {
        TAchievementStats* Stats{};
        std::int32_t Result = 0;
        if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
            Stats = aPlayer::GetPlayer()->AchievementStats;
            if (Key == u"ASTEROID") {
                Result = Stats->AsteroidsDestroyed;
            } else if (Key == u"FRY") {
                Result = Stats->EnemiesDestroyedByStarHeat;
            } else if (Key == u"DEFENDER") {
                Result = Stats->SystemsDefended;
            } else if (Key == u"PIRATE") {
                Result = Stats->SystemsCapturedForPirates;
            } else if (Key == u"SCIENCE") {
                Result = Stats->CompletedResearchPrograms;
            } else if (Key == u"HACKER") {
                Result = Stats->SuccessfulDominatorHacks;
            } else if (Key == u"PRISONBAIL") {
                Result = Stats->PrisonersBailedOut;
            } else if (Key == u"DRAIN") {
                Result = Stats->DrainedHullPoints;
            } else if (Key == u"BERTORSLAYER") {
                Result = aPlayer::GetPlayer()->DominatorKillsByType[aGalaxyStruct::ktBertor];
            } else if (Key == u"SIDECHANGER") {
                Result = aPlayer::GetPlayer()->SideChangeCount;
            }
            if (Result == StoredValue) {
                return 0;
            }
        }
        return Result;
    }

    std::uint8_t GetAchievementBackend() {
        if (SimpleSteamApi::SteamInitialized) {
            if (SimpleSteamApi::SteamAchievementsCount() > 0) {
                return 1;
            }
            return 2;
        }
        return 3;
    }

    std::int32_t GetAvailableAchievementCount() {
        switch (Achievements::GetAchievementBackend()) {
            case 1: return std::min<std::int32_t>(82, SimpleSteamApi::SteamAchievementsCount());
            case 3: return 82;
            case 2: return 0;
            default: return 0;
        }
    }

    SimpleSteamApi::PAchievementData CreateAchievementData() {
        SimpleSteamApi::PAchievementData Result{};
        pas::new_value(Result);
        Result->Name = WStringUtils::AllocateStartupWideString(255);
        Result->Description = WStringUtils::AllocateStartupWideString(255);
        Result->Achieved = false;
        Result->HasProgress = false;
        Result->Reserved0C = 0;
        Result->MaxValue = 0;
        Result->Value = 0;
        Result->IconPath = WStringUtils::AllocateStartupWideString(255);
        Result->Date = 0;
        return Result;
    }

    void FreeAchievementData(SimpleSteamApi::PAchievementData Data) {
        if (Data != nullptr) {
            WStringUtils::FreeStartupWideString(pas::Var<WStringUtils::PStartupWideString>(&Data->Name));
            WStringUtils::FreeStartupWideString(pas::Var<WStringUtils::PStartupWideString>(&Data->Description));
            WStringUtils::FreeStartupWideString(pas::Var<WStringUtils::PStartupWideString>(&Data->IconPath));
            pas::dispose(Data);
        }
    }

    SimpleSteamApi::PAchievementData GetAchievementData(pas::WideString Key) {
        SimpleSteamApi::PAchievementData Result{};
        Result = nullptr;
        EC_BlockPar::TBlockParEC* Block = AchievementDefinitions->FindBlock(Key);
        if (Block != nullptr) {
            Result = Achievements::CreateAchievementData();
            switch (Achievements::GetAchievementBackend()) {
                case 1: {
                    SimpleSteamApi::SteamAchievementData(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Num"_wref.get()))), Result);
                    break;
                }
                case 3: NoSteamAchievemens::GetLocalAchievementData(Key, Result); break;
            }
            WStringUtils::TruncateStartupWideString(pas::Var<WStringUtils::PStartupWideString>(&Result->Name));
            WStringUtils::TruncateStartupWideString(pas::Var<WStringUtils::PStartupWideString>(&Result->Description));
            WStringUtils::TruncateStartupWideString(pas::Var<WStringUtils::PStartupWideString>(&Result->IconPath));
        }
        return Result;
    }

    std::uint8_t TryUnlockAchievement(pas::WideString Key) {
        std::uint8_t Result = false;
        if (aGalaxy::Galaxy == nullptr) {
            return Result;
        }
        if (!aGalaxy::Galaxy->CanRecordAchievements()) {
            return Result;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->AwardedAchievementKeys->CountBlocks(Key) > 0) {
            return Result;
        }
        if (Achievements::GetAvailableAchievementCount() <= 0) {
            return Result;
        }
        EC_BlockPar::TBlockParEC* Block = AchievementDefinitions->FindBlock(Key);
        if (Block == nullptr) {
            return Result;
        }
        switch (Achievements::GetAchievementBackend()) {
            case 1: {
                Result = SimpleSteamApi::SteamUnlockAchievement(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Num"_wref.get()))));
                break;
            }
            case 3: Result = NoSteamAchievemens::UnlockLocalAchievement(Block); break;
            case 2: Result = false; break;
            default: Result = false; break;
        }
        if (Result && aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->AwardedAchievementKeys->AddChildBlock(Key);
        }
        return Result;
    }

    std::uint8_t TrySetAchievementProgress(pas::WideString Key, std::int32_t Value) {
        std::int32_t Increment{};
        std::uint8_t Result = false;
        if (aGalaxy::Galaxy == nullptr) {
            return Result;
        }
        if (!aGalaxy::Galaxy->CanRecordAchievements()) {
            return Result;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->AwardedAchievementKeys->CountBlocks(Key) > 0) {
            return Result;
        }
        if (Achievements::GetAvailableAchievementCount() <= 0) {
            return Result;
        }
        EC_BlockPar::TBlockParEC* Block = AchievementDefinitions->FindBlock(Key);
        if (Block == nullptr) {
            return Result;
        }
        SimpleSteamApi::PAchievementData Data = Achievements::GetAchievementData(Key);
        if (Value > Data->Value && Value <= Data->MaxValue) {
            Increment = Value - Data->Value;
        } else if (Value > Data->MaxValue) {
            Increment = Data->MaxValue - Data->Value;
        } else {
            Achievements::FreeAchievementData(Data);
            return Result;
        }
        switch (Achievements::GetAchievementBackend()) {
            case 1: {
                Result = SimpleSteamApi::SteamIncreaseStat(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Num"_wref.get()))), Increment);
                break;
            }
            case 3: Result = NoSteamAchievemens::IncreaseLocalAchievementProgress(Block, Increment); break;
            case 2: Result = false; break;
            default: Result = false; break;
        }
        Achievements::FreeAchievementData(Data);
        Data = Achievements::GetAchievementData(Key);
        if (aPlayer::GetPlayer() != nullptr && Data->Achieved) {
            aPlayer::GetPlayer()->AwardedAchievementKeys->AddChildBlock(Key);
        }
        Achievements::FreeAchievementData(Data);
        return Result;
    }

    std::uint8_t TryAddAchievementProgress(pas::WideString Key, std::int32_t Amount) {
        std::int32_t Increment{};
        std::uint8_t Result = false;
        if (aGalaxy::Galaxy == nullptr) {
            return Result;
        }
        if (!aGalaxy::Galaxy->CanRecordAchievements()) {
            return Result;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->AwardedAchievementKeys->CountBlocks(Key) > 0) {
            return Result;
        }
        if (Achievements::GetAvailableAchievementCount() <= 0) {
            return Result;
        }
        EC_BlockPar::TBlockParEC* Block = AchievementDefinitions->FindBlock(Key);
        if (Block == nullptr) {
            return Result;
        }
        SimpleSteamApi::PAchievementData Data = Achievements::GetAchievementData(Key);
        if (Amount + Data->Value <= Data->MaxValue) {
            Increment = Amount;
        } else {
            Increment = Data->MaxValue - Data->Value;
        }
        switch (Achievements::GetAchievementBackend()) {
            case 1: {
                Result = SimpleSteamApi::SteamIncreaseStat(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Num"_wref.get()))), Increment);
                break;
            }
            case 3: Result = NoSteamAchievemens::IncreaseLocalAchievementProgress(Block, Increment); break;
            case 2: Result = false; break;
            default: Result = false; break;
        }
        Achievements::FreeAchievementData(Data);
        Data = Achievements::GetAchievementData(Key);
        if (aPlayer::GetPlayer() != nullptr && Data->Achieved) {
            aPlayer::GetPlayer()->AwardedAchievementKeys->AddChildBlock(Key);
        }
        Achievements::FreeAchievementData(Data);
        return Result;
    }

    void InitializeAchievementDefinitions() {
        std::int32_t Index{};
        EC_BlockPar::TBlockParEC* Block{};
        AchievementDefinitions = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 82); cpp_range.next(Index); ) {
            Block = AchievementDefinitions->AddChildBlock(static_cast<pas::WideString>(AchievementDefinitionTable[Index].Key));
            Block->AddParam(u"Id"_wref.get(), static_cast<pas::WideString>(AchievementDefinitionTable[Index].Key));
            Block->AddParam(u"Num"_wref.get(), pas::wide_int_to_str(Index - 1));
            Block->AddParam(u"MaxValue"_wref.get(), pas::wide_int_to_str(AchievementDefinitionTable[Index].MaxValue));
            Block->AddParam(u"Value"_wref.get(), u"0"_wref.get());
            Block->AddParam(u"Achieved"_wref.get(), u"No"_wref.get());
            Block->AddParam(u"Date"_wref.get(), u"0"_wref.get());
        }
    }

    void TAchievementStats_Create(TAchievementStats* Self) {
        pas::object_create(Self);
        Self->AsteroidsDestroyed = 0;
        Self->EnemiesDestroyedByStarHeat = 0;
        Self->SystemsDefended = 0;
        Self->SystemsCapturedForPirates = 0;
        Self->CompletedResearchPrograms = 0;
        Self->SuccessfulDominatorHacks = 0;
        Self->PrisonersBailedOut = 0;
        Self->DrainedHullPoints = 0;
        Self->StarFuelCollected = 0u;
        Self->StarFuelTankId = 0;
        Self->UninhabitedPlanetsVisited = 0;
    }

    void TAchievementStats_Destroy(TAchievementStats* Self) {
        pas::object_destroy(Self);
    }

    void TAchievementStats::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        if (GlobalsV::LoadedSaveVersion >= 99) {
            AsteroidsDestroyed = EC_Buf::TBufEC_GetUInt32(Buffer);
            EnemiesDestroyedByStarHeat = EC_Buf::TBufEC_GetUInt32(Buffer);
            SystemsDefended = EC_Buf::TBufEC_GetUInt32(Buffer);
            SystemsCapturedForPirates = EC_Buf::TBufEC_GetUInt32(Buffer);
            CompletedResearchPrograms = EC_Buf::TBufEC_GetByte(Buffer);
            SuccessfulDominatorHacks = EC_Buf::TBufEC_GetUInt32(Buffer);
            PrisonersBailedOut = EC_Buf::TBufEC_GetUInt32(Buffer);
            DrainedHullPoints = EC_Buf::TBufEC_GetUInt32(Buffer);
            StarFuelCollected = EC_Buf::TBufEC_GetUInt32(Buffer);
            StarFuelTankId = EC_Buf::TBufEC_GetUInt32(Buffer);
            UninhabitedPlanetsVisited = EC_Buf::TBufEC_GetUInt32(Buffer);
        } else {
            AsteroidsDestroyed = EC_Buf::TBufEC_GetUInt32(Buffer);
            EC_Buf::TBufEC_GetUInt32(Buffer);
            EC_Buf::TBufEC_GetUInt32(Buffer);
            EC_Buf::TBufEC_GetUInt32(Buffer);
            EnemiesDestroyedByStarHeat = EC_Buf::TBufEC_GetUInt32(Buffer);
            EC_Buf::TBufEC_GetUInt32(Buffer);
            EC_Buf::TBufEC_GetUInt32(Buffer);
            SystemsDefended = EC_Buf::TBufEC_GetUInt32(Buffer);
            SystemsCapturedForPirates = EC_Buf::TBufEC_GetUInt32(Buffer);
            CompletedResearchPrograms = EC_Buf::TBufEC_GetByte(Buffer);
            EC_Buf::TBufEC_GetUInt32(Buffer);
            EC_Buf::TBufEC_GetUInt32(Buffer);
            EC_Buf::TBufEC_GetUInt32(Buffer);
            SuccessfulDominatorHacks = 0;
            PrisonersBailedOut = 0;
            DrainedHullPoints = 0;
            StarFuelCollected = 0u;
            StarFuelTankId = 0;
            UninhabitedPlanetsVisited = 0;
        }
    }

    void TAchievementStats::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddDWord(AsteroidsDestroyed);
        Buffer->AddDWord(EnemiesDestroyedByStarHeat);
        Buffer->AddDWord(SystemsDefended);
        Buffer->AddDWord(SystemsCapturedForPirates);
        Buffer->AddAnsiChar(CompletedResearchPrograms);
        Buffer->AddDWord(SuccessfulDominatorHacks);
        Buffer->AddDWord(PrisonersBailedOut);
        Buffer->AddDWord(DrainedHullPoints);
        Buffer->AddDWord(StarFuelCollected);
        Buffer->AddDWord(StarFuelTankId);
        Buffer->AddDWord(UninhabitedPlanetsVisited);
    }

    void TAchievementStats::CheckBomberAchievement(std::int32_t KillsThisTurn) {
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (aGalaxy::Galaxy == nullptr) {
            return;
        }
        if (KillsThisTurn >= 5) {
            Achievements::TryUnlockAchievement(u"BOMBER"_w);
        }
    }

    void TAchievementStats::CheckAllAwardsAchievement() {
        std::int32_t I{};
        std::uint8_t AwardId{};
        pas::Array<std::uint8_t, 0, 255> Present{};
        pas::fill_memory(&Present, 0, static_cast<std::uint8_t>(static_cast<std::int32_t>(sizeof(pas::Array<std::uint8_t, 0, 255>))));
        std::int32_t LastAward = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"Reward.Count"_wref.get()))) - 1;
        std::uint8_t AllPresent = true;
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->AwardIds != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->AwardIds) - 1); cpp_range.next(I); ) {
                AwardId = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aPlayer::GetPlayer()->AwardIds, I)));
                Present[AwardId] = true;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, LastAward); cpp_range_2.next(I); ) {
                AllPresent = AllPresent && Present[I];
            }
            if (AllPresent) {
                Achievements::TryUnlockAchievement(u"BREZHNEV"_w);
            }
        }
    }

    void TAchievementStats::CheckNoQuestVictoryAchievement() {
        std::int32_t I{};
        aRanger::PPlayerOldQuest Quest{};
        if (aPlayer::GetPlayer() != nullptr) {
            if (aRanger::PlayerOldQuests != nullptr) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aRanger::PlayerOldQuests) - 1); cpp_range.next(I); ) {
                    Quest = pas::list_at<aRanger::TPlayerOldQuest>(aRanger::PlayerOldQuests, I);
                    if (Quest->Successful) {
                        return;
                    }
                }
            }
        }
        Achievements::TryUnlockAchievement(u"BUMMER"_w);
    }

    void TAchievementStats::CheckChampionVictoryAchievement(std::int32_t Score) {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && Score >= 50000) {
            Achievements::TryUnlockAchievement(u"CHAMPION"_w);
        }
    }

    void TAchievementStats::CheckNoLoadVictoryAchievement() {
        if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->LoadCount == 0) {
            Achievements::TryUnlockAchievement(u"KIBERMAN"_w);
        }
    }

    void TAchievementStats::CheckNoShotsArcadeVictoryAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr) {
            Achievements::TryUnlockAchievement(u"HOLEPEACE"_w);
        }
    }

    void TAchievementStats::CheckMoneyAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer()->Money >= 10000000) {
            Achievements::TryUnlockAchievement(u"MONEY"_w);
        }
    }

    void TAchievementStats::CheckMasterAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer()->CountWingmen() >= 6) {
            Achievements::TryUnlockAchievement(u"MASTER"_w);
        }
    }

    void TAchievementStats::CheckNodesAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer()->BaseNodes >= 25000) {
            Achievements::TryUnlockAchievement(u"NODES"_w);
        }
    }

    void TAchievementStats::CheckLongGameVictoryAchievement(std::int32_t Score, std::int32_t FinishedTurn) {
        if (aPlayer::GetPlayer() != nullptr && Score >= 20000 && FinishedTurn >= 36800) {
            Achievements::TryUnlockAchievement(u"DOLGOZHID"_w);
        }
    }

    void TAchievementStats::CheckPacifistVictoryAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer()->TotalShipKillCount == 0) {
            Achievements::TryUnlockAchievement(u"PEACELOVER"_w);
        }
    }

    void TAchievementStats::CheckAllPirateSystemsAchievement() {
        std::int32_t Index{};
        aGalaxy::TStar* Star{};
        std::uint8_t AllPirateSystems = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(Index); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, Index);
            if (Star->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                AllPirateSystems = false;
                break;
            }
        }
        if (AllPirateSystems) {
            Achievements::TryUnlockAchievement(u"PIRATESYSTEMS"_w);
        }
    }

    void TAchievementStats::CheckFirstPlaceRatingAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->CurrentTurn >= 300 && aPlayer::GetPlayer()->PlaceInRating == 1) {
            Achievements::TryUnlockAchievement(u"RATING"_w);
        }
    }

    void TAchievementStats::CheckAllSkillsAchievement() {
        aShip::TPilotSkill Skill{};
        std::uint8_t Complete = true;
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr) {
            for (auto cpp_range = pas::for_to<aShip::TPilotSkill>(aShip::psAccuracy, aShip::psLeadership); cpp_range.next(Skill); ) {
                if (aPlayer::GetPlayer()->GetBaseSkillLevel(Skill) < 6) {
                    Complete = false;
                }
            }
            if (Complete) {
                Achievements::TryUnlockAchievement(u"SKILL"_w);
            }
        }
    }

    void TAchievementStats::CheckSpeedAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer()->CalculateSpeed() >= 2300) {
            Achievements::TryUnlockAchievement(u"SPEED"_w);
        }
    }

    void TAchievementStats::CheckFastVictoryAchievement() {
        std::int32_t Elapsed{};
        if (aGalaxy::Galaxy != nullptr) {
            Elapsed = aGalaxy::Galaxy->CurrentTurn - 300;
            if (pas::real_divide(Elapsed, 365.0L) < 7.0L) {
                Achievements::TryUnlockAchievement(u"SPRINTER"_w);
            }
        }
    }

    void TAchievementStats::CheckScienceAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && CompletedResearchPrograms >= 3) {
            Achievements::TryUnlockAchievement(u"SCIENCE"_w);
        }
    }

    void TAchievementStats::CheckBaronAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer()->PirateRank == 7) {
            Achievements::TryUnlockAchievement(u"BARON"_w);
        }
    }

    void TAchievementStats::CheckCommanderAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer()->Rank == 7) {
            Achievements::TryUnlockAchievement(u"COMMANDOR"_w);
        }
    }

    void TAchievementStats::CheckHaterAchievement() {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        std::int32_t Count = 0;
        if (aPlayer::GetPlayer() == nullptr || aGalaxy::Galaxy == nullptr) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(J); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                    if (Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                        ++Count;
                        if (Planet->GetRelationLevelToShip(aPlayer::GetPlayer()) > aGalaxyStruct::rlHostile) {
                            return;
                        }
                    }
                }
            }
        }
        if (Count > 0) {
            Achievements::TryUnlockAchievement(u"HATER"_w);
        }
    }

    void TAchievementStats::CheckBestEquipmentAchievement() {
        std::int32_t I{};
        if (aPlayer::GetPlayer() == nullptr || aGalaxy::Galaxy == nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->GetHull()->HasStandardStats()) {
            return;
        }
        if (aPlayer::GetPlayer()->GetFuelTanks() == nullptr || aPlayer::GetPlayer()->GetFuelTanks()->HasStandardStats()) {
            return;
        }
        if (aPlayer::GetPlayer()->GetEngine() == nullptr || aPlayer::GetPlayer()->GetEngine()->HasStandardStats()) {
            return;
        }
        if (aPlayer::GetPlayer()->GetRadar() == nullptr || aPlayer::GetPlayer()->GetRadar()->HasStandardStats()) {
            return;
        }
        if (aPlayer::GetPlayer()->GetScanner() == nullptr || aPlayer::GetPlayer()->GetScanner()->HasStandardStats()) {
            return;
        }
        if (aPlayer::GetPlayer()->GetRepairRobot() == nullptr || aPlayer::GetPlayer()->GetRepairRobot()->HasStandardStats()) {
            return;
        }
        if (aPlayer::GetPlayer()->GetCargoHook() == nullptr || aPlayer::GetPlayer()->GetCargoHook()->HasStandardStats()) {
            return;
        }
        if (aPlayer::GetPlayer()->GetDefGenerator() == nullptr || aPlayer::GetPlayer()->GetDefGenerator()->HasStandardStats()) {
            return;
        }
        if (aPlayer::GetPlayer()->CountEquippedWeapons() < 5) {
            return;
        }
        for (I = 1; I <= 5; ++I) {
            if (aPlayer::GetPlayer()->Weapons[I]->HasStandardStats()) {
                return;
            }
        }
        Achievements::TryUnlockAchievement(u"BEST"_w);
    }

    void TAchievementStats::CheckAllDiseasesAchievement() {
        std::int32_t I{};
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr) {
            for (I = 1; I <= 12; ++I) {
                if (aPlayer::GetPlayer()->CaptainHealth[I].ApplicationCount == 0) {
                    return;
                }
            }
            Achievements::TryUnlockAchievement(u"ILL"_w);
        }
    }

    void TAchievementStats::CheckAllDrugsAchievement() {
        std::int32_t I{};
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr) {
            for (I = 13; I <= 24; ++I) {
                if (aPlayer::GetPlayer()->CaptainHealth[I].ApplicationCount == 0) {
                    return;
                }
            }
            Achievements::TryUnlockAchievement(u"NARKOMAN"_w);
        }
    }

    void TAchievementStats::CheckScratchDamageAchievement(std::int32_t HitsReceived) {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && HitsReceived >= 20) {
            Achievements::TryUnlockAchievement(u"SCRATCHDAMAGE"_w);
        }
    }

    void TAchievementStats::CheckStarFuelAchievement() {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && StarFuelCollected >= 40) {
            Achievements::TryUnlockAchievement(u"SUNFUEL"_w);
        }
    }

    void TAchievementStats::CheckMapBuilderAchievement() {
        std::int32_t I{};
        aGalaxy::TConstellation* Constellation{};
        std::uint16_t Year{};
        std::uint16_t Month{};
        std::uint16_t Day{};
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr) {
            SysUtilsImports::DecodeDate(aGalaxy::GameTurnToDateTime(aGalaxy::Galaxy->CurrentTurn - 300), Year, Month, Day);
            if (Year > 3304) {
                return;
            }
            if (Year == 3304 && (Month > 1 || Day > 1)) {
                return;
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Constellations) - 1); cpp_range.next(I); ) {
                Constellation = pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, I);
                if (static_cast<std::uint8_t>(Constellation->Visible ^ 1) && Constellation->Id != 20) {
                    return;
                }
            }
            Achievements::TryUnlockAchievement(u"MAPBUILDER"_w);
        }
    }

    void TAchievementStats::CheckInvestorAchievement(std::int32_t Amount) {
        if (aPlayer::GetPlayer() != nullptr && aGalaxy::Galaxy != nullptr && Amount >= 300000) {
            Achievements::TryUnlockAchievement(u"INVESTOR"_w);
        }
    }

    void TAchievementStats::CheckTranclucatorFleetAchievement() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (aGalaxy::Galaxy == nullptr) {
            return;
        }
        std::int32_t Count = 0;
        aGalaxy::TStar* Star = aPlayer::GetPlayer()->CurrentStar;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
            if (Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr && static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == aPlayer::GetPlayer()) {
                ++Count;
            }
        }
        if (Count >= 10) {
            Achievements::TryUnlockAchievement(u"TRANCLUCATORS"_w);
        }
    }

    void TAchievementStats::p_destroy() {
        Achievements::TAchievementStats_Destroy(this);
    }

} // namespace Achievements
