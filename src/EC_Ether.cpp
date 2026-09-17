#include "layout/EC_Ether.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Ether.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"

namespace EC_Ether {
    void TEther_Create(TEther* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Lock = pas::make_critical_section<pas::CriticalSection>();
    }

    // Native destructor leaves the critical section; it does not free the lock or clear entries.
    void TEther_Destroy(TEther* Self) {
        pas::critical_leave(Self->Lock);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TEther::Clear() {
        while (First != nullptr) {
            RemoveEntry(Last);
        }
        if (SortedItems != nullptr) {
            EC_Mem::FreeEC(SortedItems);
            SortedItems = nullptr;
        }
        Count = 0;
    }

    TEtherUnit* TEther::AppendEntry() {
        TEtherUnit* Item = pas::make_object<TEtherUnit>();
        if (Last != nullptr) {
            Last->Next = Item;
        }
        Item->Prev = Last;
        Item->Next = nullptr;
        Last = Item;
        if (First == nullptr) {
            First = Item;
        }
        return Item;
    }

    // Unlinks and frees the entry without updating SortedItems or Count.
    void TEther::RemoveEntry(TEtherUnit* Item) {
        if (Item->Prev != nullptr) {
            Item->Prev->Next = Item->Next;
        }
        if (Item->Next != nullptr) {
            Item->Next->Prev = Item->Prev;
        }
        if (Last == Item) {
            Last = Item->Prev;
        }
        if (First == Item) {
            First = Item->Next;
        }
        pas::free(Item);
    }

    // Native assembly restores EAX after loading the entry, returning Self instead of the indexed value.
    TEtherUnit* TEther_GetIndexedEntry(TEther* Self, std::int32_t Index) {
        // Manual port: the native routine restores EAX, returning Self (not the entry).
        return reinterpret_cast<TEtherUnit*>(Self);
    }

    void TEther::SetIndexedEntry(std::int32_t Index, TEtherUnit* Item) {
        pas::store_unaligned<TEtherUnit*>(pas::byte_offset(SortedItems, Index * sizeof(TEtherUnit*)), Item);
    }

    std::int32_t TEther::FindInsertionIndex(const pas::WideString& Name) {
        std::int32_t Middle{};
        std::int32_t Comparison{};
        TEtherUnit* Item{};
        if (Count <= 0) {
            return 0;
        }
        std::int32_t Left = 0;
        std::int32_t Right = Count - 1;
        do {
            Middle = pas::shr(Right - Left, 1) + Left;
            Item = EC_Ether::TEther_GetIndexedEntry(this, Middle);
            Comparison = EC_Str::CompareWideChars(Name.pchar(), Item->Name.pchar());
            if (Comparison == 0) {
                return Middle;
            }
            if (Comparison < 0) {
                Right = Middle - 1;
            } else {
                Left = Middle + 1;
            }
        } while (!(Right < Left));
        if (Comparison < 0) {
            return Middle;
        }
        return Middle + 1;
    }

    void TEther::Add(const pas::WideString& Name, std::int32_t Value) {
        std::int32_t Index{};
        Enter();
        TEtherUnit* Item = AppendEntry();
        Item->Name = Name;
        Item->Value = Value;
        Index = FindInsertionIndex(Name);
        ++Count;
        SortedItems = static_cast<PEtherIndex>(EC_Mem::ReAllocREC(SortedItems, Count * static_cast<std::int32_t>(sizeof(TEtherUnit*))));
        std::int32_t MoveCount = Count - 1 - Index;
        if (MoveCount > 0) {
            // The assembly shifts the overlapping pointer range backward.
            pas::move_memory(reinterpret_cast<TEtherUnit**>(pas::byte_offset(SortedItems, Index * sizeof(TEtherUnit*))), reinterpret_cast<TEtherUnit**>(pas::byte_offset(SortedItems, (Index + 1) * sizeof(TEtherUnit*))), MoveCount * static_cast<std::int32_t>(sizeof(TEtherUnit*)));
        }
        SetIndexedEntry(Index, Item);
        Leave();
    }

    void TEther::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        TEtherUnit* Item{};
        Buffer->AddIntegerValue(Count);
        Item = First;
        while (Item != nullptr) {
            Buffer->AddWideStringZ(Item->Name);
            Buffer->AddIntegerValue(Item->Value);
            Item = Item->Next;
        }
    }

    void TEther::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        pas::WideString Name{};
        std::int32_t Index{};
        std::int32_t Value{};
        Clear();
        std::int32_t ItemCount = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ItemCount - 1); cpp_range.next(Index); ) {
            Name = Buffer->ReadWideString();
            Value = EC_Buf::TBufEC_GetInt32(Buffer);
            Add(Name, Value);
        }
    }

    void TEther::Enter() {
        pas::critical_enter(Lock);
    }

    void TEther::Leave() {
        pas::critical_leave(Lock);
    }

    void TEther::p_destroy() {
        EC_Ether::TEther_Destroy(this);
    }

} // namespace EC_Ether
