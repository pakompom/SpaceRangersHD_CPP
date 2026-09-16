#pragma once
#include "types/aModsInfo.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace aModsInfo {
    // Owns the TModInfo objects.
    extern pas::List* ModInfos;

    // Non-owning references into ModInfos.
    extern pas::List* SelectedModInfos;

    extern EC_BlockPar::TBlockParEC* ModIdCounts;

    // Maps declared conflict IDs to matching mod indices.
    extern EC_BlockPar::TBlockParEC* ModConflictIndex;

    // Maps required mod IDs to matching mod indices.
    extern EC_BlockPar::TBlockParEC* ModDependencyIndex;

    extern std::uint8_t ModInfosInitialized;

    std::int32_t FindOrInsertModFolder(pas::WideString Folder);

    std::uint8_t LoadModInfo(pas::WideString Folder, TModInfo* Info);

    void ScanModFolders(pas::WideString Folder, pas::WideString Prefix);

    void InitializeModInfos();

    // Frees mod objects and clears the existing containers, retaining their allocation for reload.
    void ClearModInfoState();

    void TModInfo_Create(TModInfo* Self);

    void TModInfo_Destroy(TModInfo* Self);

} // namespace aModsInfo
