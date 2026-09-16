#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlayer.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace fScore {
    struct TScoreQuestResult;

    struct TfScoreUnit;

    struct TfScore;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfScore : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfScore, GI_MessageLoop::TMessageLoopGI, "TfScore", 216)
        void p_destroy() override;
        void SortAndTrimEntries();
        static void InitializeDefaultEntry(std::int32_t Index, TfScoreUnit*& Entry);
        void CreateDefaultTable();
        void RecordPlayerResult(std::uint8_t Victory);
        void RemoveSelectedEntryAndRefill();
        void ClearEntries();
        void LoadTableFromDisk();
        void ReloadTable();
        void SaveTableToDisk();
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void EntryMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void EntryMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void DeleteEntryClicked(GI_MessageLoop::TObjectGI* Sender);
        void ClearTableClicked(GI_MessageLoop::TObjectGI* Sender);
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void RefreshDetails();
        void EntryMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ExportEntryClicked(GI_MessageLoop::TObjectGI* Sender);
        void QuestHelpMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void QuestHelpMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible);
        void SelectMusic() override;
        pas::List* Entries;
        std::int32_t SelectedIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TScoreQuestResult {
        std::uint8_t Successful;
        aGalaxyStruct::TQuestType QuestType;
        std::uint16_t QuestNumber;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfScoreUnit : pas::Object {
        PAS_CLASS_META(TfScoreUnit, pas::Object, "TfScoreUnit", 124)
        void p_destroy() override;
        void CapturePlayer(std::uint8_t Victory);
        void RecalculateDifficultyPercent();
        void RecalculateTotalScore();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, std::int32_t FileVersion);
        void ExportToFile(pas::WideString FileName);
        std::uint8_t VictoryAchieved;
        std::uint8_t Disqualified;
        pas::Array<std::uint8_t, 0, 7> DifficultyLevels;
        std::uint8_t cpp_padding[2];
        std::int32_t DifficultyPercent;
        pas::WideString PlayerName;
        std::int32_t PortraitFaceId;
        std::uint8_t PilotRace;
        std::uint8_t cpp_padding_2[3];
        std::int32_t FinishedTurn;
        std::uint8_t Rank;
        std::uint8_t PirateRank;
        std::uint8_t cpp_padding_3[2];
        std::int32_t OtherShipKillCount;
        std::int32_t PirateKillCount;
        std::int32_t DominatorKillCount;
        std::int32_t LiberatedSystemCount;
        std::int32_t CivilianKillCount;
        std::int32_t MilitaryKillCount;
        std::int32_t RangerKillCount;
        std::int32_t ArcadeKillCount;
        std::int32_t AwardCount;
        pas::DynArray<std::uint8_t> AwardIds;
        std::int32_t TotalExperience;
        pas::Array<std::uint8_t, 0, 5> SkillLevels;
        std::uint8_t cpp_padding_4[2];
        std::int32_t GenerationSeed;
        EC_Buf::TBufEC* ScoreTags;
        pas::DynArray<TScoreQuestResult> QuestResults;
        std::int32_t PlanetBattles;
        pas::DynArray<aPlayer::TPlanetBattleHistoryEntry> PlanetBattleHistory;
        std::uint8_t BlazerEndingState;
        std::uint8_t KellerEndingState;
        std::uint8_t TerronEndingState;
        std::uint8_t PirateEndingState;
        std::int32_t TotalScore;
        std::uint8_t Exported;
        std::uint8_t cpp_padding_5[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fScore
