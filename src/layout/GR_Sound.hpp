#pragma once
#include "types/GR_Sound.hpp"

namespace GR_Sound {
    static_assert(sizeof(void*) != 4 || sizeof(GR_Sound::TSoundBufferControl) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBufferControl, SoundPath) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBufferControl, SoundGroup) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBufferControl, Looping) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBufferControl, Buffer) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBufferControl, Volume) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBufferControl, Pan) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(GR_Sound::TSoundBuffer) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, AutoRelease) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, Streaming) == 13);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, Started) == 14);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, DirectBuffer) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, Notify) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, StopEvent) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, ChunkEvents) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, VolumeEvent) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, BufferBytes) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, WaveFormat) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, VolumeTimer) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, Volume) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, VolumeScale) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, VolumeStep) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, FadingOut) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, SoundGroup) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, Controller) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, WriteOffset) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundBuffer, LastPlayCursor) == 100);
    static_assert(sizeof(void*) != 4 || sizeof(GR_Sound::TSoundControl) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundControl, FirstBuffer) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundControl, LastBuffer) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundControl, DirectSound) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundControl, PrimaryBuffer) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundControl, WaveFormat) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundControl, Lock) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Sound::TSoundControl, LastFadeTick) == 44);

} // namespace GR_Sound
