#pragma once
#include "types/VorbisFile.hpp"

namespace VorbisFile {
    static_assert(sizeof(void*) != 4 || sizeof(VorbisFile::TOggWorker) == 740);
    static_assert(sizeof(void*) != 4 || offsetof(VorbisFile::TOggWorker, VorbisState) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(VorbisFile::TOggWorker, Lock) == 728);
    static_assert(sizeof(void*) != 4 || offsetof(VorbisFile::TOggWorker, Bitstream) == 732);
    static_assert(sizeof(void*) != 4 || offsetof(VorbisFile::TOggWorker, ExternalLibrary) == 736);
    static_assert(sizeof(void*) != 4 || sizeof(VorbisFile::TVorbisCallbacks) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(VorbisFile::TVorbisCallbacks, Read) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(VorbisFile::TVorbisCallbacks, Seek) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(VorbisFile::TVorbisCallbacks, Close) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(VorbisFile::TVorbisCallbacks, Tell) == 12);

} // namespace VorbisFile
