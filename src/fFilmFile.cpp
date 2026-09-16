#include "layout/fFilmFile.hpp"
#include "types/aEFilm.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GlobalsV.hpp"
#include "units/fFilmFile.hpp"

namespace fFilmFile {
    void TFilmFile_Create(TFilmFile* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Lock = pas::make_critical_section<pas::CriticalSection>();
    }

    void TFilmFile_Destroy(TFilmFile* Self) {
        Self->Clear();
        if (Self->Lock != nullptr) {
            pas::free(Self->Lock);
            Self->Lock = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TFilmFile::Clear() {
        pas::critical_enter(Lock);
        while (FirstEntry != nullptr) {
            RemoveEntry(LastEntry);
        }
        pas::critical_leave(Lock);
    }

    // Caller holds Lock. Appends a zeroed entry owned by this history.
    PFilmHistoryEntry TFilmFile::AppendEntry() {
        PFilmHistoryEntry Entry = static_cast<PFilmHistoryEntry>(EC_Mem::AllocClearEC(static_cast<std::int32_t>(sizeof(TFilmHistoryEntry))));
        if (LastEntry != nullptr) {
            LastEntry->Next = Entry;
        }
        Entry->Prev = LastEntry;
        Entry->Next = nullptr;
        LastEntry = Entry;
        if (FirstEntry == nullptr) {
            FirstEntry = Entry;
        }
        return Entry;
    }

    // Caller holds Lock. Unlinks Entry and frees its buffer and storage.
    void TFilmFile::RemoveEntry(PFilmHistoryEntry Entry) {
        if (Entry->Prev != nullptr) {
            Entry->Prev->Next = Entry->Next;
        }
        if (Entry->Next != nullptr) {
            Entry->Next->Prev = Entry->Prev;
        }
        if (LastEntry == Entry) {
            LastEntry = Entry->Prev;
        }
        if (FirstEntry == Entry) {
            FirstEntry = Entry->Next;
        }
        if (Entry->Buffer != nullptr) {
            pas::free(Entry->Buffer);
            Entry->Buffer = nullptr;
        }
        EC_Mem::FreeEC(Entry);
    }

    std::int32_t TFilmFile::GetCount() {
        pas::critical_enter(Lock);
        std::int32_t Count = 0;
        PFilmHistoryEntry Entry = FirstEntry;
        while (Entry != nullptr) {
            ++Count;
            Entry = Entry->Next;
        }
        std::int32_t Result = Count;
        pas::critical_leave(Lock);
        return Result;
    }

    // Zero-based insertion order. Returns a borrowed entry after releasing Lock; raises for an invalid index.
    PFilmHistoryEntry TFilmFile::GetEntry(std::int32_t Index) {
        PFilmHistoryEntry Result{};
        pas::critical_enter(Lock);
        PFilmHistoryEntry Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Index == 0) {
                Result = Entry;
                pas::critical_leave(Lock);
                return Result;
            }
            --Index;
            Entry = Entry->Next;
        }
        pas::critical_leave(Lock);
        pas::raise(pas::make_exception<pas::Exception>("Error in TFilmFile.InfoGet"_a));
    }

    // Copies Film into a new buffer. Evicts entries with the lowest Turn until below FilmHistoryLimit, which must be positive.
    void TFilmFile::AddFilm(aEFilm::TEFilm* Film) {
        PFilmHistoryEntry Entry{};
        PFilmHistoryEntry Oldest{};
        std::int32_t Turn{};
        pas::critical_enter(Lock);
        while (([&] {
            std::int32_t cpp_left = GetCount();
            return cpp_left >= GlobalsV::FilmHistoryLimit;
        }())) {
            Oldest = FirstEntry;
            Turn = Oldest->Turn;
            Entry = Oldest->Next;
            while (Entry != nullptr) {
                if (Entry->Turn < Turn) {
                    Turn = Entry->Turn;
                    Oldest = Entry;
                }
                Entry = Entry->Next;
            }
            DeleteEntry(Oldest);
        }
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Film->SaveToBuffer(Buffer);
        Buffer->SetPosition(0);
        if (Buffer->DataSize < 1) {
            pas::critical_leave(Lock);
            pas::raise(pas::make_exception<pas::Exception>("Error in TFilmFile.FilmAdd"_a));
        }
        Entry = AppendEntry();
        Entry->Turn = Film->Turn;
        Entry->Buffer = Buffer;
        pas::critical_leave(Lock);
    }

    // Locks and removes an entry belonging to this history.
    void TFilmFile::DeleteEntry(PFilmHistoryEntry Entry) {
        pas::critical_enter(Lock);
        RemoveEntry(Entry);
        pas::critical_leave(Lock);
    }

    // Replaces Film's contents but does not set Film.Turn; caller copies Entry.Turn. Rewinds the stored buffer afterward.
    void TFilmFile::LoadFilm(PFilmHistoryEntry Entry, aEFilm::TEFilm* Film) {
        pas::critical_enter(Lock);
        Entry->Buffer->SetPosition(0);
        Film->LoadFromBuffer(Entry->Buffer);
        Entry->Buffer->SetPosition(0);
        pas::critical_leave(Lock);
    }

    // Clears Buffer, then writes Turn and the length-prefixed film payload.
    void TFilmFile::SaveEntryToBuffer(PFilmHistoryEntry Entry, EC_Buf::TBufEC* Buffer) {
        Buffer->Clear();
        pas::critical_enter(Lock);
        Buffer->AddIntegerValue(Entry->Turn);
        Buffer->AddBuffer(Entry->Buffer);
        pas::critical_leave(Lock);
    }

    // Appends without enforcing FilmHistoryLimit. Reads from the current buffer position.
    void TFilmFile::LoadEntryFromBuffer(EC_Buf::TBufEC* Buffer) {
        pas::critical_enter(Lock);
        PFilmHistoryEntry Entry = AppendEntry();
        Entry->Turn = EC_Buf::TBufEC_GetInt32(Buffer);
        Entry->Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buffer->ReadLengthPrefixedBuffer(Entry->Buffer);
        pas::critical_leave(Lock);
    }

    void TFilmFile::p_destroy() {
        fFilmFile::TFilmFile_Destroy(this);
    }

} // namespace fFilmFile
