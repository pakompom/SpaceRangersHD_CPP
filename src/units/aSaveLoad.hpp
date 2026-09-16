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

    extern std::uint32_t MemorySnapshotGalaxyToken;

    std::uint8_t SaveGameToFile(pas::WideString FileName, pas::WideString Description);

    std::uint8_t LoadGameFromFile(pas::WideString FileName);

    void LoadGameFromSaveBuffer(EC_Buf::TBufEC* Buffer);

    void SaveGameToMemorySnapshot();

    void RestoreGameFromMemorySnapshot();

    void InitializeSaveWriter();

    void FinalizeSaveWriter();

    void TSaver_Execute(TSaver* Self);

} // namespace aSaveLoad
