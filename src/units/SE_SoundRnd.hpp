#pragma once
#include "types/SE_SoundRnd.hpp"

namespace SE_SoundRnd {
    extern SE_SoundRnd::TSoundRndSE* FirstRandomSound;

    extern SE_SoundRnd::TSoundRndSE* LastRandomSound;

    TSoundRndSE* CreateRandomSound();

    void FreeRandomSound(TSoundRndSE* Sound);

    void FreeAllRandomSounds();

    TSoundRndSE* FindRandomSound(pas::WideString Name, std::int32_t& GroupIndex);

    void TSoundRndSE_Create(TSoundRndSE* Self);

    void TSoundRndSE_Destroy(TSoundRndSE* Self);

} // namespace SE_SoundRnd
