#pragma once
#include "types/aPlayer.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aPlayer {
    extern std::int32_t ArcadeKellerDefeats;

    extern pas::Object* ArcadeKellerReward;

    extern std::uint32_t EncodedPlayer;

    extern aPlayer::TStorageHeaderColumnTable StorageHeaderColumns;

    extern aPlayer::TStorageDividerLengthTable StorageDividerLengths;

    extern aPlayer::TProbeSummaryColumnTable ProbeSummaryColumns;

    extern aPlayer::TStorageDividerLengthTable TranclucatorSummaryWidths;

    extern aPlayer::TStorageItemColumnTable StorageItemColumns;

    void SetPlayer(TPlayer* Player, aGalaxy::TGalaxy* Galaxy);

    TPlayer* GetPlayer();

    void TJournalRecord_Create(TJournalRecord* Self);

    void TJournalRecord_Destroy(TJournalRecord* Self);

    void TPlayer_Create(TPlayer* Self);

    void TPlayer_Destroy(TPlayer* Self);

} // namespace aPlayer
