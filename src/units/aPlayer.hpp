#pragma once
#include "types/aPlayer.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aShip {
    struct TShip;

} // namespace aShip

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

    void TPlayer_ResolveLoadedReferences(TPlayer* Self, aGalaxy::TGalaxy* Galaxy);

    void TPlayer_NextDay(TPlayer* Self);

    // Uses the carrier's hold, then current-location storage; refreshes Self even when Carrier differs.
    void TPlayer_ConsumeAvailableNodes(TPlayer* Self, std::int32_t Count, aShip::TShip* Carrier);

    // Updates eligible docked players after turn 300 and retains the newest 100 entries.
    void TPlayer_RefreshNewsAtLocation(TPlayer* Self);

    // Includes the player's current-system kill counts and main pirate planet exception.
    void TPlayer_RefreshCurrentStanding(TPlayer* Self);

} // namespace aPlayer
