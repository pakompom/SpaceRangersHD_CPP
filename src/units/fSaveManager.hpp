#pragma once
#include "types/fSaveManager.hpp"

namespace fSaveManager {
    extern pas::WideString AutoSaveFileName;

    extern pas::Array<pas::WideString, 1, 3> QuickSaveFileNames;

    extern pas::WideString TurnSaveFileName;

    extern std::uint32_t NewSaveNormalColor;

    extern std::uint32_t NewSaveSelectedColor;

    extern std::uint32_t SaveSlotNormalColor;

    extern std::uint32_t SaveSlotSelectedColor;

    void TfSaveManager_Create(TfSaveManager* Self);

    void TfSaveManager_Destroy(TfSaveManager* Self);

} // namespace fSaveManager
