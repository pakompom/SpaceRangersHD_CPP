#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aEFilm {
    struct TEFilm;

} // namespace aEFilm

namespace fFilmFile {
    struct TFilmHistoryEntry;

    struct TFilmFile;

    using PFilmHistoryEntry = TFilmHistoryEntry*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TFilmFile : EC_Struct::TObjectEx {
        PAS_CLASS_META(TFilmFile, EC_Struct::TObjectEx, "TFilmFile", 16)
        void p_destroy() override;
        void Clear();
        PFilmHistoryEntry AppendEntry();
        void RemoveEntry(PFilmHistoryEntry Entry);
        std::int32_t GetCount();
        PFilmHistoryEntry GetEntry(std::int32_t Index);
        void AddFilm(aEFilm::TEFilm* Film);
        void DeleteEntry(PFilmHistoryEntry Entry);
        void LoadFilm(PFilmHistoryEntry Entry, aEFilm::TEFilm* Film);
        void SaveEntryToBuffer(PFilmHistoryEntry Entry, EC_Buf::TBufEC* Buffer);
        void LoadEntryFromBuffer(EC_Buf::TBufEC* Buffer);
        PFilmHistoryEntry FirstEntry;
        PFilmHistoryEntry LastEntry;
        pas::CriticalSection* Lock;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TFilmHistoryEntry {
        PFilmHistoryEntry Prev;
        PFilmHistoryEntry Next;
        std::int32_t Turn;
        EC_Buf::TBufEC* Buffer;
    };
    #pragma pack(pop)

} // namespace fFilmFile
