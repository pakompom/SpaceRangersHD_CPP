#pragma once
#include "types/EC_CacheSound.hpp"

namespace EC_CacheSound {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheSound::TCSoundControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheSound::TWaveFormatEx) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFormatEx, FormatTag) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFormatEx, Channels) == 2);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFormatEx, SamplesPerSecond) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFormatEx, AverageBytesPerSecond) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFormatEx, BlockAlign) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFormatEx, BitsPerSample) == 14);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFormatEx, ExtraSize) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheSound::TCSoundEC) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TCSoundEC, Format) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TCSoundEC, SampleData) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TCSoundEC, SampleDataSize) == 56);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheSound::TWaveFileHeader) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFileHeader, Channels) == 22);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFileHeader, SamplesPerSecond) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFileHeader, BlockAlign) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFileHeader, BitsPerSample) == 34);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFileHeader, DataId) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheSound::TWaveFileHeader, DataSize) == 40);

} // namespace EC_CacheSound
