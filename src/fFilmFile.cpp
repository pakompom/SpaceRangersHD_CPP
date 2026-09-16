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

    void TFilmFile::DeleteEntry(PFilmHistoryEntry Entry) {
        pas::critical_enter(Lock);
        RemoveEntry(Entry);
        pas::critical_leave(Lock);
    }

    void TFilmFile::LoadFilm(PFilmHistoryEntry Entry, aEFilm::TEFilm* Film) {
        pas::critical_enter(Lock);
        Entry->Buffer->SetPosition(0);
        Film->LoadFromBuffer(Entry->Buffer);
        Entry->Buffer->SetPosition(0);
        pas::critical_leave(Lock);
    }

    void TFilmFile::SaveEntryToBuffer(PFilmHistoryEntry Entry, EC_Buf::TBufEC* Buffer) {
        Buffer->Clear();
        pas::critical_enter(Lock);
        Buffer->AddIntegerValue(Entry->Turn);
        Buffer->AddBuffer(Entry->Buffer);
        pas::critical_leave(Lock);
    }

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
