#pragma once
#include "types/fFilmFile.hpp"

namespace fFilmFile {
    static_assert(sizeof(void*) != 4 || sizeof(fFilmFile::TFilmFile) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fFilmFile::TFilmFile, FirstEntry) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fFilmFile::TFilmFile, LastEntry) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fFilmFile::TFilmFile, Lock) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(fFilmFile::TFilmHistoryEntry) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fFilmFile::TFilmHistoryEntry, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fFilmFile::TFilmHistoryEntry, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fFilmFile::TFilmHistoryEntry, Turn) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fFilmFile::TFilmHistoryEntry, Buffer) == 12);

} // namespace fFilmFile
