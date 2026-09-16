#pragma once
#include "types/fSaveManager.hpp"

namespace fSaveManager {
    static_assert(sizeof(void*) != 4 || sizeof(fSaveManager::TfSaveManager) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TfSaveManager, SelectedSlot) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TfSaveManager, Slots) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TfSaveManager, PreviewTimer) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TfSaveManager, PreviewSound) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TfSaveManager, Closing) == 224);
    static_assert(sizeof(void*) != 4 || sizeof(fSaveManager::TSMSlot) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TSMSlot, FileName) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TSMSlot, DisplayName) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TSMSlot, Turn) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TSMSlot, Money) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TSMSlot, PilotName) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TSMSlot, RaceName) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fSaveManager::TSMSlot, LocalWriteTime) == 24);

} // namespace fSaveManager
