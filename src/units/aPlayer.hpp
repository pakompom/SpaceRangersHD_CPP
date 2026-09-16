#pragma once
#include "types/aPlayer.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aPlayer {
    // Native campaign reward guard.
    extern std::int32_t ArcadeKellerDefeats;

    // Owned reward item pending transfer.
    extern pas::Object* ArcadeKellerReward;

    // The native initial DWORD decodes to nil; zero would decode to a non-null pointer.
    extern std::uint32_t EncodedPlayer;

    extern aPlayer::TStorageHeaderColumnTable StorageHeaderColumns;

    extern aPlayer::TStorageDividerLengthTable StorageDividerLengths;

    extern aPlayer::TProbeSummaryColumnTable ProbeSummaryColumns;

    extern aPlayer::TStorageDividerLengthTable TranclucatorSummaryWidths;

    extern aPlayer::TStorageItemColumnTable StorageItemColumns;

    // Updates Galaxy.PlayerRangerIndex; a nil Galaxy leaves the current player unchanged.
    void SetPlayer(TPlayer* Player, aGalaxy::TGalaxy* Galaxy);

    TPlayer* GetPlayer();

    void TJournalRecord_Create(TJournalRecord* Self);

    void TJournalRecord_Destroy(TJournalRecord* Self);

    // Native constructor initializes lists/defaults; does not register or generate the player loadout.
    void TPlayer_Create(TPlayer* Self);

    // Requires the inherited ranger registration state for final cleanup.
    void TPlayer_Destroy(TPlayer* Self);

} // namespace aPlayer
