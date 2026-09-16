#pragma once
#include "types/GI_GAIFile.hpp"

namespace GI_GAIFile {
    static_assert(sizeof(void*) != 4 || sizeof(GI_GAIFile::TGAIFileGI) == 424);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, ImageFile) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, Header) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, FrameDirectory) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, FrameBuffers) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, LoaderThread) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, FrameLock) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, PreloadCount) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, FrameImage) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, FrameTimer) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, ImageKindX) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, ImageKindY) == 369);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, CurrentFrame) == 372);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, SequenceFrameCount) == 376);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, SequenceFrames) == 380);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, FrameDelays) == 384);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, UsePlaybackBuffer) == 388);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, PlaybackBuffer) == 392);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, LastBufferedFrame) == 396);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, TransparentColor) == 400);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, CycleCompleteCallback) == 408);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, Stopped) == 416);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileGI, AutoUpdateFlags) == 420);
    static_assert(sizeof(void*) != 4 || sizeof(GI_GAIFile::TGAIFileThreadGI) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GI_GAIFile::TGAIFileThreadGI, Owner) == 44);

} // namespace GI_GAIFile
