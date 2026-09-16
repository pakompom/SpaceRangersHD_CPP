#pragma once
#include "types/aModsInfo.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace aModsInfo {
    extern pas::List* ModInfos;

    extern pas::List* SelectedModInfos;

    extern EC_BlockPar::TBlockParEC* ModIdCounts;

    extern EC_BlockPar::TBlockParEC* ModConflictIndex;

    extern EC_BlockPar::TBlockParEC* ModDependencyIndex;

    extern std::uint8_t ModInfosInitialized;

    std::int32_t FindOrInsertModFolder(pas::WideString Folder);

    std::uint8_t LoadModInfo(pas::WideString Folder, TModInfo* Info);

    void ScanModFolders(pas::WideString Folder, pas::WideString Prefix);

    void InitializeModInfos();

    void ClearModInfoState();

    void TModInfo_Create(TModInfo* Self);

    void TModInfo_Destroy(TModInfo* Self);

} // namespace aModsInfo
