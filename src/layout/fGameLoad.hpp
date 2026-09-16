#pragma once
#include "types/fGameLoad.hpp"

namespace fGameLoad {
    static_assert(sizeof(void*) != 4 || sizeof(fGameLoad::TfGameLoad) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fGameLoad::TfGameLoad, LoadThread) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fGameLoad::TfGameLoad, ProgressTimer) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fGameLoad::TfGameLoad, AssetPreloadStarted) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fGameLoad::TfGameLoad, TargetProgress) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fGameLoad::TfGameLoad, DisplayedProgress) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fGameLoad::TfGameLoad, LoadingComplete) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fGameLoad::TfGameLoad, LoadPanel) == 232);
    static_assert(sizeof(void*) != 4 || sizeof(fGameLoad::TThreadGameLoad) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(fGameLoad::TThreadGameLoad, Succeeded) == 44);

} // namespace fGameLoad
