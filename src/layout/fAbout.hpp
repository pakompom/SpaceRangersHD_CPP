#pragma once
#include "types/fAbout.hpp"

namespace fAbout {
    static_assert(sizeof(void*) != 4 || sizeof(fAbout::TfAbout) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fAbout::TfAbout, ScrollTimer) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fAbout::TfAbout, ViewportPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fAbout::TfAbout, CreditsPanel) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fAbout::TfAbout, CreditsHeight) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fAbout::TfAbout, FirstMusicSelection) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fAbout::TfAbout, ReturnToScores) == 225);

} // namespace fAbout
