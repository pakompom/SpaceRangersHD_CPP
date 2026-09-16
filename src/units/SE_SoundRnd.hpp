#pragma once
#include "types/SE_SoundRnd.hpp"

namespace SE_SoundRnd {
    extern SE_SoundRnd::TSoundRndSE* FirstRandomSound;

    extern SE_SoundRnd::TSoundRndSE* LastRandomSound;

    TSoundRndSE* CreateRandomSound();

    // Unlinks and frees a nonnil registered sound.
    void FreeRandomSound(TSoundRndSE* Sound);

    void FreeAllRandomSounds();

    // Creates and loads an uncached name, then selects a weighted group; -1 for zero total weight.
    TSoundRndSE* FindRandomSound(pas::WideString Name, std::int32_t& GroupIndex);

    void TSoundRndSE_Create(TSoundRndSE* Self);

    void TSoundRndSE_Destroy(TSoundRndSE* Self);

} // namespace SE_SoundRnd
