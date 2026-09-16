#pragma once
#include "types/VFW.hpp"

namespace VFW {
    static_assert(sizeof(void*) != 4 || sizeof(VFW::TAVIStreamInfoA) == 140);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, StreamType) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Handler) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Flags) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Caps) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Priority) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Language) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Scale) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Rate) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Start) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Length) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, InitialFrames) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, SuggestedBufferSize) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Quality) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, SampleSize) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Frame) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, EditCount) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, FormatChangeCount) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(VFW::TAVIStreamInfoA, Name) == 76);

} // namespace VFW
