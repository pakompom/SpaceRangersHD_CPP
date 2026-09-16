#pragma once
#include "types/GI_GAI.hpp"

namespace GI_GAI {
    static_assert(sizeof(void*) != 4 || sizeof(GI_GAI::TgaiGI) == 380);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, MainImageCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, FirstFrameImageCache) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, AutoFrameTimer) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, ImageKindX) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, ImageKindY) == 301);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, Alpha) == 302);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, SequenceFrame) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, SequenceFrameCount) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, SequenceFrameIndexTable) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, SequenceFrameDelayTable) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, SequenceIndex) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, UsesPlaybackBuffer) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, CachedPlaybackGraphBuf) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, LastCachedFrameIndex) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, TransparentColor) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, CycleCompleteCallback) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, FrameAdvancedCallback) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, SkipImageUpdateRect) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, StopPlaybackRequested) == 361);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, StopAfterOneCycle) == 362);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, StartSoundName) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, FirstFrameOnly) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, AutoUpdateFlags) == 372);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAI::TgaiGI, HardwareMirrorHorizontal) == 376);

} // namespace GI_GAI
