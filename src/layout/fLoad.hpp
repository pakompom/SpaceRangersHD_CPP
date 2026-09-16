#pragma once
#include "types/fLoad.hpp"

namespace fLoad {
    static_assert(sizeof(void*) != 4 || sizeof(fLoad::TCacheLoader) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TCacheLoader, PendingLoads) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TCacheLoader, TotalLoadCount) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TCacheLoader, CompletedLoadCount) == 52);
    static_assert(sizeof(void*) != 4 || sizeof(fLoad::TfLoad) == 264);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, ProgressTimer) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, LoadProgress) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, DisplayedProgress) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, LoadingFinished) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, IntroSkipRequest) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, IntroTimer) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, IntroStartedAt) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, IntroConfig) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, IntroItemIndex) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, IntroVideoFrameCount) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, IntroDurationMs) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, IntroImageKind) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, LoadPanel) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fLoad::TfLoad, BackgroundStyle) == 260);

} // namespace fLoad
