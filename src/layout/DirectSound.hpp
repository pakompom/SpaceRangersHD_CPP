#pragma once
#include "types/DirectSound.hpp"

namespace DirectSound {
    static_assert(sizeof(void*) != 4 || sizeof(DirectSound::TSoundWaveFormat) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TSoundWaveFormat, FormatTag) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TSoundWaveFormat, Channels) == 2);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TSoundWaveFormat, SamplesPerSecond) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TSoundWaveFormat, AverageBytesPerSecond) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TSoundWaveFormat, BlockAlign) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TSoundWaveFormat, BitsPerSample) == 14);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TSoundWaveFormat, ExtraSize) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(DirectSound::TDSBufferDesc) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TDSBufferDesc, Size) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TDSBufferDesc, Flags) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TDSBufferDesc, BufferBytes) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TDSBufferDesc, Reserved) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TDSBufferDesc, WaveFormat) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TDSBufferDesc, Algorithm) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(DirectSound::TDSPositionNotify) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TDSPositionNotify, Offset) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(DirectSound::TDSPositionNotify, EventHandle) == 4);

} // namespace DirectSound
