#pragma once
#include "types/aSaveLoad.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aSaveLoad {
    extern EC_Buf::TBufEC* MemorySnapshotBuffer;

    extern pas::CriticalSection* SaveLoadLock;

    extern aSaveLoad::TSaver* SaveWriter;

    extern std::int32_t MemorySnapshotXorSeed;

    // Detached galaxy address plus 0x17557455, modulo 2^32.
    extern std::uint32_t MemorySnapshotGalaxyToken;

    // Disk header: eight NUL-terminated UTF-16 strings: RSG, v<version>,
    // description, turn, money, pilot name, race/emblem name, EZ.
    // Each preview has a four-byte byte count. The trailing film block runs to EOF.
    // Queues the write; true does not mean the background writer has finished.
    std::uint8_t SaveGameToFile(pas::WideString FileName, pas::WideString Description);

    // Replaces the current galaxy.
    std::uint8_t LoadGameFromFile(pas::WideString FileName);

    // Requires an existing galaxy object and a decoded buffer positioned at the player-hold section.
    void LoadGameFromSaveBuffer(EC_Buf::TBufEC* Buffer);

    // Requires a live galaxy/player and no outstanding snapshot. Obfuscates and detaches the live galaxy until restoration; does not increment the persistent save count.
    void SaveGameToMemorySnapshot();

    // Consumes the snapshot, destroys the detached galaxy and rebuilds it. Preserves the persistent load count and restores UI references by object ID.
    void RestoreGameFromMemorySnapshot();

    void InitializeSaveWriter();

    // Waits for a pending write before freeing the worker.
    void FinalizeSaveWriter();

    // Owns and frees all five buffers. Writes Save.tmp before replacing the destination; shares SaveLoadLock with the loader.
    void TSaver_Execute(TSaver* Self);

} // namespace aSaveLoad
