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
        // Caller holds Lock. Appends a zeroed entry owned by this history.
        PFilmHistoryEntry AppendEntry();
        // Caller holds Lock. Unlinks Entry and frees its buffer and storage.
        void RemoveEntry(PFilmHistoryEntry Entry);
        std::int32_t GetCount();
        // Zero-based insertion order. Returns a borrowed entry after releasing Lock; raises for an invalid index.
        PFilmHistoryEntry GetEntry(std::int32_t Index);
        // Copies Film into a new buffer. Evicts entries with the lowest Turn until below FilmHistoryLimit, which must be positive.
        void AddFilm(aEFilm::TEFilm* Film);
        // Locks and removes an entry belonging to this history.
        void DeleteEntry(PFilmHistoryEntry Entry);
        // Replaces Film's contents but does not set Film.Turn; caller copies Entry.Turn. Rewinds the stored buffer afterward.
        void LoadFilm(PFilmHistoryEntry Entry, aEFilm::TEFilm* Film);
        // Clears Buffer, then writes Turn and the length-prefixed film payload.
        void SaveEntryToBuffer(PFilmHistoryEntry Entry, EC_Buf::TBufEC* Buffer);
        // Appends without enforcing FilmHistoryLimit. Reads from the current buffer position.
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
        // Owned serialized TEFilm, excluding Turn.
        EC_Buf::TBufEC* Buffer;
    };
    #pragma pack(pop)

} // namespace fFilmFile
