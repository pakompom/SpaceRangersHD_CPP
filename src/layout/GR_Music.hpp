#pragma once
#include "types/GR_Music.hpp"

namespace GR_Music {
    static_assert(sizeof(void*) != 4 || sizeof(GR_Music::TMusicControl) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicControl, CompletionEvent) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicControl, ControlLock) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicControl, Current) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicControl, Queued) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicControl, CurrentFileName) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicControl, CategoryOverride) == 68);
    static_assert(sizeof(void*) != 4 || sizeof(GR_Music::TMusicUnit) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, Decoder) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, BuiltinVorbis) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, RequestedFileName) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, ImmediateStop) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, Buffer) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, DecodeLock) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, DecoderLibrary) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, Stream) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, StartPlaybackEvent) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Music::TMusicUnit, CompletionEvent) == 80);

} // namespace GR_Music
