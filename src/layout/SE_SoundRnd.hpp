#pragma once
#include "types/SE_SoundRnd.hpp"

namespace SE_SoundRnd {
    static_assert(sizeof(void*) != 4 || sizeof(SE_SoundRnd::TSoundRndUnitSE) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndUnitSE, Weight) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndUnitSE, Group) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndUnitSE, NextTimeMin) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndUnitSE, NextTimeMax) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndUnitSE, SoundNames) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndUnitSE, SoundWeights) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndUnitSE, TotalSoundWeight) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(SE_SoundRnd::TSoundRndSE) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndSE, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndSE, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndSE, Name) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndSE, Groups) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_SoundRnd::TSoundRndSE, TotalGroupWeight) == 20);

} // namespace SE_SoundRnd
