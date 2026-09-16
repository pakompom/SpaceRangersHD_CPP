#include "layout/EC_BlockPar.hpp"
#include "types/WindowsImports.hpp"
#include "units/BlockParException.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"

namespace EC_BlockPar {
    std::uint8_t NextBlockPathComponent(const pas::WideString& Path, std::int32_t& Cursor, std::int32_t& PathLength, std::int32_t& Start, std::int32_t& PartLength);

    void ParseBlockPathOccurrence(const pas::WideString& Path, std::int32_t& Start, std::int32_t& PartLength, std::int32_t& Occurrence);

    std::uint8_t MatchBlockPathComponent(pas::WideString Name, const pas::WideString& Path, std::int32_t& Start, std::int32_t& PartLength);

    void WriteWideBlockEntry(TBlockParEC* Self, EC_Buf::TBufEC*& Dest, std::int32_t& Indent, std::uint8_t& Sorted, TBlockParElEC*& Entry);

    void WriteAnsiBlockEntry(TBlockParEC* Self, EC_Buf::TBufEC*& Dest, std::int32_t& Indent, std::uint8_t& Sorted, TBlockParElEC*& Entry);

    const std::uint32_t BlockDatSeedKey = 0xb1e8c689u;

    const std::uint32_t BlockDatCrcKey1 = 0x7db6c99du;

    const std::uint32_t BlockDatCrcKey2 = 0xc83fcbf3u;

    void TBlockParElEC_Create(TBlockParElEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TBlockParElEC_Destroy(TBlockParElEC* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TBlockParElEC::Clear() {
        if (ChildBlock != nullptr) {
            pas::free(ChildBlock);
            ChildBlock = nullptr;
        }
        ItemType = bpkText;
        Name = pas::WideString();
        StringValue = pas::WideString();
        Comment = pas::WideString();
    }

    void TBlockParElEC::MakeChildBlock() {
        if (ChildBlock != nullptr) {
            pas::free(ChildBlock);
            ChildBlock = nullptr;
        }
        ChildBlock = pas::construct_call<TBlockParEC>(TBlockParEC_Create);
        ItemType = bpkBlock;
        StringValue = pas::WideString();
    }

    void TBlockParElEC::CopyFrom(TBlockParElEC* Source) {
        Clear();
        ItemType = Source->ItemType;
        Name = Source->Name;
        StringValue = Source->StringValue;
        Comment = Source->Comment;
        ChildBlock = nullptr;
        if (Source->ChildBlock != nullptr) {
            ChildBlock = pas::construct_call<TBlockParEC>(TBlockParEC_Create);
            ChildBlock->CopyFrom(Source->ChildBlock);
        }
    }

    void TBlockParEC_Create(TBlockParEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->UseSortedIndex = true;
    }

    void TBlockParEC_Destroy(TBlockParEC* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TBlockParEC::Clear() {
        TBlockParElEC* Removed{};
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            Removed = Entry;
            Entry = Entry->Next;
            pas::free(Removed);
        }
        FirstEntry = nullptr;
        LastEntry = nullptr;
        EntryCount = 0;
        StringParamCount = 0;
        ChildBlockCount = 0;
        SortedEntries = nullptr;
        SortedEntryCount = 0;
    }

    void TBlockParEC::CopyFrom(TBlockParEC* Source) {
        TBlockParElEC* Added{};
        Clear();
        UseSortedIndex = Source->UseSortedIndex;
        TBlockParElEC* Entry = Source->FirstEntry;
        while (Entry != nullptr) {
            Added = AddEntry();
            Added->CopyFrom(Entry);
            if (UseSortedIndex) {
                InsertIntoSortedIndex(Added);
            }
            if (Entry->ItemType == bpkString) {
                ++StringParamCount;
            } else if (Entry->ItemType == bpkBlock) {
                ++ChildBlockCount;
            }
            Entry = Entry->Next;
        }
    }

    TBlockParElEC* TBlockParEC::AddEntry() {
        TBlockParElEC* Entry = pas::construct_call<TBlockParElEC>(TBlockParElEC_Create);
        Entry->OwnerBlock = this;
        if (LastEntry != nullptr) {
            LastEntry->Next = Entry;
        }
        Entry->Prev = LastEntry;
        Entry->Next = nullptr;
        LastEntry = Entry;
        if (FirstEntry == nullptr) {
            FirstEntry = Entry;
        }
        ++EntryCount;
        return Entry;
    }

    void TBlockParEC::DeleteEntry(TBlockParElEC* Entry) {
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
        --EntryCount;
        if (Entry->ItemType == bpkString) {
            --StringParamCount;
        } else if (Entry->ItemType == bpkBlock) {
            --ChildBlockCount;
        }
        pas::free(Entry);
    }

    TBlockParElEC* TBlockParEC::FindEntryByPath(const pas::WideString& Path, std::uint8_t RaiseIfMissing) {
        std::int32_t Start{};
        std::int32_t PartLength{};
        std::int32_t Occurrence{};
        std::int32_t Index{};
        std::int32_t Seen{};
        std::int32_t PathLength = Path.length();
        std::int32_t Cursor = 0;
        TBlockParEC* Block = this;
        TBlockParElEC* Entry = nullptr;
        while (EC_BlockPar::NextBlockPathComponent(Path, Cursor, PathLength, Start, PartLength)) {
            EC_BlockPar::ParseBlockPathOccurrence(Path, Start, PartLength, Occurrence);
            if (Block->UseSortedIndex) {
                Entry = nullptr;
                Index = Block->FindSortedNameRangeStartIndex(pas::copy(Path, Start + 1, PartLength));
                if (Index >= 0) {
                    Entry = Block->SortedEntries[Index];
                    if (Occurrence != 0) {
                        if (Occurrence < Entry->GroupCount) {
                            Entry = Block->SortedEntries[Index + Occurrence];
                        } else {
                            Entry = nullptr;
                        }
                    }
                }
            } else {
                Entry = Block->FirstEntry;
                Seen = 0;
                while (Seen <= Occurrence && Entry != nullptr) {
                    while (Entry != nullptr) {
                        if (EC_BlockPar::MatchBlockPathComponent(Entry->Name, Path, Start, PartLength)) {
                            if (Seen < Occurrence) {
                                Entry = Entry->Next;
                            }
                            break;
                        }
                        Entry = Entry->Next;
                    }
                    ++Seen;
                }
            }
            if (Entry == nullptr) {
                if (RaiseIfMissing) {
                    pas::raise(pas::construct_call<BlockParException::EBlockPar>(BlockParException::EBlockPar_Create, static_cast<pas::AnsiString>(pas::concat_wide({u"GetEl. Path=", Path})), false));
                }
                return nullptr;
            }
            if (Cursor >= PathLength) {
                break;
            }
            if (Entry->ItemType != bpkBlock) {
                if (RaiseIfMissing) {
                    pas::raise(pas::construct_call<BlockParException::EBlockPar>(BlockParException::EBlockPar_Create, static_cast<pas::AnsiString>(pas::concat_wide({u"GetEl. Path=", Path})), false));
                }
                return nullptr;
            }
            Block = Entry->ChildBlock;
        }
        if (Entry == nullptr) {
            if (RaiseIfMissing) {
                pas::raise(pas::construct_call<BlockParException::EBlockPar>(BlockParException::EBlockPar_Create, static_cast<pas::AnsiString>(pas::concat_wide({u"GetEl. Path=", Path})), false));
            }
            return nullptr;
        }
        return Entry;
    }

    std::uint8_t NextBlockPathComponent(const pas::WideString& Path, std::int32_t& Cursor, std::int32_t& PathLength, std::int32_t& Start, std::int32_t& PartLength) {
        char16_t Ch{};
        if (Cursor >= PathLength) {
            return false;
        }
        Start = Cursor;
        std::int32_t i = Start;
        while (PathLength > i) {
            Ch = Path.read(i + 1);
            if (Ch == u'.' || Ch == u'/' || Ch == u'\\') {
                break;
            }
            ++i;
        }
        PartLength = i - Start;
        Cursor = i + 1;
        return true;
    }

    void ParseBlockPathOccurrence(const pas::WideString& Path, std::int32_t& Start, std::int32_t& PartLength, std::int32_t& Occurrence) {
        char16_t Ch{};
        Occurrence = 0;
        std::int32_t i = Start;
        std::int32_t Limit = Start + PartLength;
        while (i < Limit) {
            if (Path.read(i + 1) == u':') {
                PartLength = i - Start;
                ++i;
                while (i < Limit) {
                    Ch = Path.read(i + 1);
                    if (Ch >= u'0' && Ch <= u'9') {
                        Occurrence = Occurrence * 10 + (Ch - '0');
                    }
                    ++i;
                }
                break;
            }
            ++i;
        }
    }

    std::uint8_t MatchBlockPathComponent(pas::WideString Name, const pas::WideString& Path, std::int32_t& Start, std::int32_t& PartLength) {
        if (Name.length() != PartLength) {
            return false;
        }
        return SysUtils::CompareMem(reinterpret_cast<std::uint8_t*>(Path.pchar()) + Start * static_cast<std::int32_t>(sizeof(char16_t)), Name.pchar(), PartLength * 2);
    }

    std::int32_t TBlockParEC::FindSortedNameRangeStartIndex(const pas::WideString& EntryName) {
        std::int32_t Middle{};
        std::int32_t Order{};
        TBlockParElEC* Entry{};
        if (SortedEntryCount < 1) {
            return -1;
        }
        std::int32_t Low = 0;
        std::int32_t High = SortedEntryCount - 1;
        do {
            Middle = (High - Low) / 2 + Low;
            Entry = SortedEntries[Middle];
            Order = EC_Str::CompareWideChars(EntryName.pchar(), Entry->Name.pchar());
            if (Order == 0) {
                return Middle - Entry->GroupIndex;
            }
            if (Order < 0) {
                High = Middle - 1;
            } else {
                Low = Middle + 1;
            }
        } while (!(High < Low));
        return -1;
    }

    std::int32_t TBlockParEC::PrepareSortedInsertion(TBlockParElEC* Entry) {
        std::int32_t Result{};
        std::int32_t Middle{};
        std::int32_t Order{};
        TBlockParElEC* Existing{};
        if (SortedEntryCount <= 0) {
            Result = 0;
            Entry->GroupIndex = 0;
            Entry->GroupCount = 1;
            return Result;
        }
        std::int32_t Low = 0;
        std::int32_t High = SortedEntryCount - 1;
        do {
            Middle = pas::shr(High - Low, 1) + Low;
            Existing = SortedEntries[Middle];
            Order = EC_Str::CompareWideChars(Entry->Name.pchar(), Existing->Name.pchar());
            if (Order == 0) {
                Order = static_cast<std::int32_t>(Entry->ItemType) - static_cast<std::int32_t>(Existing->ItemType);
            }
            if (Order == 0) {
                if (Existing->GroupIndex != 0) {
                    Result = Middle - Existing->GroupIndex;
                    Existing = SortedEntries[Result];
                } else {
                    Result = Middle;
                }
                Entry->GroupIndex = Existing->GroupCount;
                Result += Existing->GroupCount;
                ++Existing->GroupCount;
                return Result;
            }
            if (Order < 0) {
                High = Middle - 1;
            } else {
                Low = Middle + 1;
            }
        } while (!(High < Low));
        if (Order < 0) {
            Result = Middle;
        } else {
            Result = Middle + 1;
        }
        Entry->GroupIndex = 0;
        Entry->GroupCount = 1;
        return Result;
    }

    void TBlockParEC::InsertIntoSortedIndex(TBlockParElEC* Entry) {
        std::int32_t Index{};
        SortedEntries.set_length(SortedEntryCount + 1);
        Index = PrepareSortedInsertion(Entry);
        if (Index >= SortedEntryCount) {
            SortedEntries[SortedEntryCount] = Entry;
            ++SortedEntryCount;
            return;
        }
        Windows::MoveMemory(&SortedEntries[Index + 1], &SortedEntries[Index], (SortedEntryCount - Index) * static_cast<std::int32_t>(sizeof(TBlockParElEC*)));
        SortedEntries[Index] = Entry;
        ++SortedEntryCount;
    }

    void TBlockParEC::RemoveFromSortedIndex(TBlockParElEC* Entry) {
        std::int32_t i{};
        std::int32_t Index{};
        TBlockParElEC* Head{};
        Index = 0;
        while (Index < SortedEntryCount) {
            if (SortedEntries[Index] == Entry) {
                Head = SortedEntries[Index - Entry->GroupIndex];
                for (auto cpp_range = pas::for_to<std::int32_t>(Index + 1, Index - Entry->GroupIndex + Head->GroupCount - 1); cpp_range.next(i); ) {
                    --SortedEntries[i]->GroupIndex;
                }
                --Head->GroupCount;
                if (Entry->GroupIndex == 0) {
                    if (Head->GroupCount > 0) {
                        SortedEntries[Index + 1]->GroupCount = Entry->GroupCount;
                    }
                }
                if (Index < SortedEntryCount - 1) {
                    Windows::MoveMemory(&SortedEntries[Index], &SortedEntries[Index + 1], (SortedEntryCount - Index - 1) * static_cast<std::int32_t>(sizeof(TBlockParElEC*)));
                }
                --SortedEntryCount;
                SortedEntries.set_length(SortedEntryCount);
                return;
            }
            ++Index;
        }
    }

    pas::WideString TBlockParEC::GetParamByPath(const pas::WideString& Path) {
        TBlockParElEC* Entry = FindEntryByPath(Path, true);
        if (Entry->ItemType != bpkString) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Par_Get. Path=", Path}))));
        }
        return Entry->StringValue;
    }

    pas::WideString TBlockParEC::GetParamByPathOrMarker(const pas::WideString& Path) {
        TBlockParElEC* Entry{};
        try {
            Entry = FindEntryByPath(Path, true);
        } catch (...) {
            return pas::concat_wide({u"[", Path, u"]"});
        }
        if (Entry != nullptr && Entry->ItemType == bpkString) {
            return Entry->StringValue;
        }
        return pas::concat_wide({u"[", Path, u"]"});
    }

    std::int32_t TBlockParEC::CountParamsByPath(const pas::WideString& Path) {
        std::int32_t Count{};
        pas::WideString Part{};
        TBlockParEC* Block{};
        Count = EC_Str::CountDelimitedPartsW(Path, u"./\\"_wref.get());
        if (Count > 1) {
            Block = GetOrAddBlockByPath(EC_Str::ExtractDelimitedRangeW(Path, 0, Count - 2, u"./\\"_wref.get()));
            Part = EC_Str::ExtractDelimitedPartW(Path, Count - 1, u"./\\"_wref.get());
        } else {
            Part = Path;
            Block = this;
        }
        return Block->CountParams(Part);
    }

    TBlockParElEC* TBlockParEC::AddParam(const pas::WideString& ParamName, const pas::WideString& ParamValue) {
        TBlockParElEC* Entry = AddEntry();
        Entry->ItemType = bpkString;
        Entry->Name = ParamName;
        Entry->StringValue = ParamValue;
        if (UseSortedIndex) {
            InsertIntoSortedIndex(Entry);
        }
        ++StringParamCount;
        return Entry;
    }

    void TBlockParEC::SetParam(const pas::WideString& ParamName, const pas::WideString& ParamValue) {
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Name == ParamName && Entry->ItemType == bpkString) {
                Entry->StringValue = ParamValue;
                return;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TBlockParEC.Par_Set. name=", ParamName}))));
    }

    void TBlockParEC::SetOrAddParam(const pas::WideString& ParamName, const pas::WideString& ParamValue) {
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Name == ParamName && Entry->ItemType == bpkString) {
                Entry->StringValue = ParamValue;
                return;
            }
            Entry = Entry->Next;
        }
        AddParam(ParamName, ParamValue);
    }

    void TBlockParEC::DeleteParam(const pas::WideString& ParamName) {
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Name == ParamName && Entry->ItemType == bpkString) {
                if (UseSortedIndex) {
                    RemoveFromSortedIndex(Entry);
                }
                DeleteEntry(Entry);
                return;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TBlockParEC.Par_Delete. name=", ParamName}))));
    }

    void TBlockParEC::DeleteChildBlock(const pas::WideString& BlockName) {
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Name == BlockName && Entry->ItemType == bpkBlock) {
                if (UseSortedIndex) {
                    RemoveFromSortedIndex(Entry);
                }
                DeleteEntry(Entry);
                return;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TBlockParEC.Block_Delete. name=", BlockName}))));
    }

    pas::WideString TBlockParEC::GetParam(const pas::WideString& ParamName) {
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Name == ParamName && Entry->ItemType == bpkString) {
                return Entry->StringValue;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TBlockParEC.Par_Get. name=", ParamName}))));
    }

    pas::WideString TBlockParEC::GetParamOrMarker(const pas::WideString& ParamName) {
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Name == ParamName && Entry->ItemType == bpkString) {
                return Entry->StringValue;
            }
            Entry = Entry->Next;
        }
        return pas::concat_wide({u"[", ParamName, u"]"});
    }

    std::int32_t TBlockParEC::GetParamCount() {
        return StringParamCount;
    }

    std::int32_t TBlockParEC::CountParams(const pas::WideString& ParamName) {
        std::int32_t Result{};
        TBlockParElEC* Entry{};
        std::int32_t Index{};
        std::int32_t Limit{};
        if (UseSortedIndex) {
            Index = FindSortedNameRangeStartIndex(ParamName);
            Result = 0;
            if (Index >= 0) {
                Limit = SortedEntries[Index]->GroupCount + Index;
                while (Index < Limit) {
                    Entry = SortedEntries[Index];
                    if (Entry->ItemType == bpkString) {
                        ++Result;
                    }
                    ++Index;
                }
            }
            return Result;
        }
        Entry = FirstEntry;
        std::int32_t Count = 0;
        while (Entry != nullptr) {
            if (Entry->ItemType == bpkString && Entry->Name == ParamName) {
                ++Count;
            }
            Entry = Entry->Next;
        }
        return Count;
    }

    pas::WideString TBlockParEC::GetParamValue(std::int32_t Index) {
        if (UseSortedIndex && EntryCount == StringParamCount) {
            return SortedEntries[Index]->StringValue;
        }
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->ItemType == bpkString) {
                if (Index == 0) {
                    return Entry->StringValue;
                }
                --Index;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBlockParEC.Par_Get. no=", SysUtils::IntToStr(Index)})));
    }

    pas::WideString TBlockParEC::GetParamName(std::int32_t Index) {
        if (UseSortedIndex && EntryCount == StringParamCount) {
            return SortedEntries[Index]->Name;
        }
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->ItemType == bpkString) {
                if (Index == 0) {
                    return Entry->Name;
                }
                --Index;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBlockParEC.Par_GetName. no=", SysUtils::IntToStr(Index)})));
    }

    TBlockParEC* TBlockParEC::AddBlockByPath(const pas::WideString& Path) {
        std::int32_t Count{};
        pas::WideString Part{};
        TBlockParEC* Block{};
        Count = EC_Str::CountDelimitedPartsW(Path, u"./\\"_wref.get());
        if (Count > 1) {
            Block = GetOrAddBlockByPath(EC_Str::ExtractDelimitedRangeW(Path, 0, Count - 2, u"./\\"_wref.get()));
            Part = EC_Str::ExtractDelimitedPartW(Path, Count - 1, u"./\\"_wref.get());
        } else {
            Part = Path;
            Block = this;
        }
        TBlockParElEC* Entry = Block->AddEntry();
        Entry->MakeChildBlock();
        Entry->Name = std::move(Part);
        if (UseSortedIndex) {
            InsertIntoSortedIndex(Entry);
        }
        ++ChildBlockCount;
        return Entry->ChildBlock;
    }

    TBlockParEC* TBlockParEC::GetBlockByPath(const pas::WideString& Path) {
        TBlockParElEC* Entry = FindEntryByPath(Path, true);
        if (Entry->ItemType != bpkBlock) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TBlockParEC.BlockPath_Get. Path=", Path}))));
        }
        return Entry->ChildBlock;
    }

    TBlockParEC* TBlockParEC::FindBlockByPath(const pas::WideString& Path) {
        TBlockParElEC* Entry = FindEntryByPath(Path, false);
        if (Entry == nullptr || Entry->ItemType != bpkBlock) {
            return nullptr;
        }
        return Entry->ChildBlock;
    }

    TBlockParEC* TBlockParEC::GetOrAddBlockByPath(const pas::WideString& Path) {
        TBlockParEC* Result = FindBlockByPath(Path);
        if (Result == nullptr) {
            return AddBlockByPath(Path);
        }
        return Result;
    }

    TBlockParEC* TBlockParEC::AddChildBlock(const pas::WideString& BlockName) {
        TBlockParElEC* Entry = AddEntry();
        Entry->MakeChildBlock();
        Entry->Name = BlockName;
        if (UseSortedIndex) {
            InsertIntoSortedIndex(Entry);
        }
        ++ChildBlockCount;
        return Entry->ChildBlock;
    }

    TBlockParEC* TBlockParEC::GetBlock(const pas::WideString& BlockName) {
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Name == BlockName && Entry->ItemType == bpkBlock) {
                return Entry->ChildBlock;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TBlockParEC.Block_Get. name=", BlockName}))));
    }

    TBlockParEC* TBlockParEC::FindBlock(const pas::WideString& BlockName) {
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Name == BlockName && Entry->ItemType == bpkBlock) {
                return Entry->ChildBlock;
            }
            Entry = Entry->Next;
        }
        return nullptr;
    }

    std::int32_t TBlockParEC::GetBlockCount() {
        return ChildBlockCount;
    }

    std::int32_t TBlockParEC::CountBlocks(const pas::WideString& BlockName) {
        std::int32_t Result{};
        TBlockParElEC* Entry{};
        std::int32_t Index{};
        std::int32_t Limit{};
        if (UseSortedIndex) {
            Index = FindSortedNameRangeStartIndex(BlockName);
            Result = 0;
            if (Index >= 0) {
                Limit = SortedEntries[Index]->GroupCount + Index;
                while (Index < Limit) {
                    Entry = SortedEntries[Index];
                    if (Entry->ItemType == bpkBlock) {
                        ++Result;
                    }
                    ++Index;
                }
            }
            return Result;
        }
        Entry = FirstEntry;
        std::int32_t Count = 0;
        while (Entry != nullptr) {
            if (Entry->ItemType == bpkBlock && Entry->Name == BlockName) {
                ++Count;
            }
            Entry = Entry->Next;
        }
        return Count;
    }

    TBlockParEC* TBlockParEC::GetBlockByIndex(std::int32_t Index) {
        if (UseSortedIndex && EntryCount == ChildBlockCount) {
            return SortedEntries[Index]->ChildBlock;
        }
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->ItemType == bpkBlock) {
                if (Index == 0) {
                    return Entry->ChildBlock;
                }
                --Index;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBlockParEC.Block_Get. no=", SysUtils::IntToStr(Index)})));
    }

    pas::WideString TBlockParEC::GetBlockNameByIndex(std::int32_t Index) {
        if (UseSortedIndex && EntryCount == ChildBlockCount) {
            return SortedEntries[Index]->Name;
        }
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->ItemType == bpkBlock) {
                if (Index == 0) {
                    return Entry->Name;
                }
                --Index;
            }
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBlockParEC.Block_GetName. no=", SysUtils::IntToStr(Index)})));
    }

    std::int32_t TBlockParEC::GetEntryCount() {
        return EntryCount;
    }

    TBlockParKind TBlockParEC::GetEntryKindByIndex(std::int32_t Index) {
        if (UseSortedIndex && EntryCount == SortedEntryCount) {
            return SortedEntries[Index]->ItemType;
        }
        TBlockParElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Index == 0) {
                return Entry->ItemType;
            }
            --Index;
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBlockParEC.All_GetTip. no=", SysUtils::IntToStr(Index)})));
    }

    TBlockParEC* TBlockParEC::GetEntryBlockByIndex(std::int32_t Index) {
        TBlockParElEC* Entry{};
        if (UseSortedIndex && EntryCount == SortedEntryCount) {
            Entry = SortedEntries[Index];
            if (Entry->ItemType != bpkBlock) {
                pas::raise(pas::make_exception<pas::Exception>("TBlockParEC.All_GetBlock. Error tip."_a));
            }
            return Entry->ChildBlock;
        }
        Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Index == 0) {
                if (Entry->ItemType != bpkBlock) {
                    pas::raise(pas::make_exception<pas::Exception>("TBlockParEC.All_GetBlock. Error tip."_a));
                }
                return Entry->ChildBlock;
            }
            --Index;
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBlockParEC.All_GetBlock. no=", SysUtils::IntToStr(Index)})));
    }

    pas::WideString TBlockParEC::GetEntryStringByIndex(std::int32_t Index) {
        TBlockParElEC* Entry{};
        if (UseSortedIndex && EntryCount == SortedEntryCount) {
            Entry = SortedEntries[Index];
            if (Entry->ItemType != bpkString) {
                pas::raise(pas::make_exception<pas::Exception>("TBlockParEC.All_GetPar. Error tip."_a));
            }
            return Entry->StringValue;
        }
        Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Index == 0) {
                if (Entry->ItemType != bpkString) {
                    pas::raise(pas::make_exception<pas::Exception>("TBlockParEC.All_GetPar. Error tip."_a));
                }
                return Entry->StringValue;
            }
            --Index;
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBlockParEC.All_GetPar. no=", SysUtils::IntToStr(Index)})));
    }

    pas::WideString TBlockParEC::GetEntryNameByIndex(std::int32_t Index) {
        TBlockParElEC* Entry{};
        if (UseSortedIndex && EntryCount == SortedEntryCount) {
            Entry = SortedEntries[Index];
            if (Entry->ItemType != bpkString && Entry->ItemType != bpkBlock) {
                pas::raise(pas::make_exception<pas::Exception>("TBlockParEC.All_GetName. Error tip."_a));
            }
            return Entry->Name;
        }
        Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Index == 0) {
                if (Entry->ItemType != bpkString && Entry->ItemType != bpkBlock) {
                    pas::raise(pas::make_exception<pas::Exception>("TBlockParEC.All_GetName. Error tip."_a));
                }
                return Entry->Name;
            }
            --Index;
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBlockParEC.All_GetName. no=", SysUtils::IntToStr(Index)})));
    }

    void TBlockParEC::WriteWideText(EC_Buf::TBufEC* Dest, std::int32_t Indent, std::uint8_t Sorted) {
        TBlockParElEC* Entry{};
        std::int32_t i{};
        if (UseSortedIndex && Sorted) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, SortedEntryCount); cpp_range.next(i); ) {
                Entry = SortedEntries[i - 1];
                EC_BlockPar::WriteWideBlockEntry(this, Dest, Indent, Sorted, Entry);
            }
        } else {
            Entry = FirstEntry;
            while (Entry != nullptr) {
                EC_BlockPar::WriteWideBlockEntry(this, Dest, Indent, Sorted, Entry);
                Entry = Entry->Next;
            }
        }
    }

    void WriteWideBlockEntry(TBlockParEC* Self, EC_Buf::TBufEC*& Dest, std::int32_t& Indent, std::uint8_t& Sorted, TBlockParElEC*& Entry) {
        std::int32_t j{};
        if (Entry->ItemType == bpkText) {
            if (Entry->Comment != u"") {
                Dest->AddWideStringRaw(Entry->Comment);
            }
            Dest->AddWord(13);
            Dest->AddWord(10);
        } else if (Entry->ItemType == bpkString) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Indent * 4); cpp_range.next(j); ) {
                Dest->AddWord(' ');
            }
            Dest->AddWideStringRaw(Entry->Name);
            Dest->AddWord('=');
            Dest->AddWideStringRaw(Entry->StringValue);
            if (Entry->Comment != u"") {
                Dest->AddWideStringRaw(Entry->Comment);
            }
            Dest->AddWord(13);
            Dest->AddWord(10);
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Indent * 4); cpp_range_2.next(j); ) {
                Dest->AddWord(' ');
            }
            Dest->AddWideStringRaw(Entry->Name);
            Dest->AddWord(' ');
            if (Self->UseSortedIndex) {
                Dest->AddWord('^');
            } else {
                Dest->AddWord('~');
            }
            Dest->AddWord('{');
            Dest->AddWord(13);
            Dest->AddWord(10);
            Entry->ChildBlock->WriteWideText(Dest, Indent + 1, Sorted);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, Indent * 4); cpp_range_3.next(j); ) {
                Dest->AddWord(' ');
            }
            Dest->AddWord('}');
            if (Entry->Comment != u"") {
                Dest->AddWideStringRaw(Entry->Comment);
            }
            Dest->AddWord(13);
            Dest->AddWord(10);
        }
    }

    void TBlockParEC::WriteAnsiText(EC_Buf::TBufEC* Dest, std::int32_t Indent, std::uint8_t Sorted) {
        TBlockParElEC* Entry{};
        std::int32_t i{};
        if (UseSortedIndex && Sorted) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, SortedEntryCount); cpp_range.next(i); ) {
                Entry = SortedEntries[i - 1];
                EC_BlockPar::WriteAnsiBlockEntry(this, Dest, Indent, Sorted, Entry);
            }
        } else {
            Entry = FirstEntry;
            while (Entry != nullptr) {
                EC_BlockPar::WriteAnsiBlockEntry(this, Dest, Indent, Sorted, Entry);
                Entry = Entry->Next;
            }
        }
    }

    void WriteAnsiBlockEntry(TBlockParEC* Self, EC_Buf::TBufEC*& Dest, std::int32_t& Indent, std::uint8_t& Sorted, TBlockParElEC*& Entry) {
        std::int32_t j{};
        if (Entry->ItemType == bpkText) {
            if (Entry->Comment != u"") {
                Dest->AddAnsiStringRaw(System::WideCharToString(Entry->Comment.pchar()));
            }
            Dest->AddByte(13);
            Dest->AddByte(10);
        } else if (Entry->ItemType == bpkString) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Indent); cpp_range.next(j); ) {
                Dest->AddByte(9);
            }
            Dest->AddAnsiStringRaw(System::WideCharToString(Entry->Name.pchar()));
            Dest->AddByte('=');
            Dest->AddAnsiStringRaw(System::WideCharToString(Entry->StringValue.pchar()));
            if (Entry->Comment != u"") {
                Dest->AddAnsiStringRaw(System::WideCharToString(Entry->Comment.pchar()));
            }
            Dest->AddByte(13);
            Dest->AddByte(10);
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Indent); cpp_range_2.next(j); ) {
                Dest->AddByte(9);
            }
            Dest->AddAnsiStringRaw(System::WideCharToString(Entry->Name.pchar()));
            Dest->AddByte(' ');
            if (Self->UseSortedIndex) {
                Dest->AddByte('^');
            } else {
                Dest->AddByte('~');
            }
            Dest->AddByte('{');
            Dest->AddByte(13);
            Dest->AddByte(10);
            Entry->ChildBlock->WriteAnsiText(Dest, Indent + 1, Sorted);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, Indent); cpp_range_3.next(j); ) {
                Dest->AddByte(9);
            }
            Dest->AddByte('}');
            if (Entry->Comment != u"") {
                Dest->AddAnsiStringRaw(System::WideCharToString(Entry->Comment.pchar()));
            }
            Dest->AddByte(13);
            Dest->AddByte(10);
        }
    }

    void TBlockParEC::WriteTextBuffer(EC_Buf::TBufEC* Dest, std::uint8_t AnsiText, std::uint8_t Sorted) {
        if (!AnsiText) {
            Dest->AddWord(0x0000feff);
            WriteWideText(Dest, 0, Sorted);
        } else {
            WriteAnsiText(Dest, 0, Sorted);
        }
    }

    void TBlockParEC::SaveTextFile(char16_t* FileName, std::uint8_t AnsiText, std::uint8_t Sorted) {
        EC_File::TFileEC* FileObj = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        EC_Buf::TBufEC* Buf = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        {
            try {
                WriteTextBuffer(Buf, AnsiText, Sorted);
                FileObj->SetFileName(static_cast<pas::WideString>(FileName));
                FileObj->CreateNew();
                FileObj->WriteBuffer(Buf->Data, Buf->DataSize);
                FileObj->ReleaseHandle();
            } catch (...) {
                pas::free(FileObj);
                pas::free(Buf);
                throw;
            }
            pas::free(FileObj);
            pas::free(Buf);
        }
    }

    void TBlockParEC::ParseTextBuffer(EC_Buf::TBufEC* Buf, const pas::WideString& InitialText, std::uint8_t AnsiText, std::uint8_t PreserveComments) {
        pas::WideString Text{};
        pas::WideString Name{};
        pas::WideString IncludeFile{};
        pas::WideString Comment{};
        TBlockParEC* Child{};
        std::int32_t PartCount{};
        TBlockParElEC* Entry{};
        std::uint8_t ChildSorted{};
        Text = EC_Str::TrimWideString(InitialText);
        while (!Buf->IsAtEnd()) {
            if (Text == u"") {
                if (AnsiText) {
                    Text = EC_Str::TrimWideString(static_cast<pas::WideString>(Buf->ReadAnsiTextLine()));
                } else {
                    Text = EC_Str::TrimWideString(Buf->ReadWideTextLine());
                }
            }
            Comment = EC_Str::ExtractLineCommentW(Text);
            Text = EC_Str::TrimWideString(EC_Str::RemoveLineCommentW(Text));
            PartCount = EC_Str::CountDelimitedPartsW(Text, u"{"_wref.get());
            if (PartCount > 1) {
                Name = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Text, 0, u"{"_wref.get()));
                if (Name == u"") {
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TBlockParEC.LoadFromBuf_r. tstr=", Text}))));
                }
                ChildSorted = Name.read(Name.length()) == u'^';
                if (ChildSorted) {
                    Name.set_length(Name.length() - 1);
                    Name = EC_Str::TrimWideString(Name);
                } else {
                    ChildSorted = Name.read(Name.length()) != u'~';
                    if (!ChildSorted) {
                        Name.set_length(Name.length() - 1);
                        Name = EC_Str::TrimWideString(Name);
                    }
                }
                IncludeFile = pas::WideString();
                if (EC_Str::CountDelimitedPartsW(Name, u"="_wref.get()) == 2) {
                    IncludeFile = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Name, 1, u"="_wref.get()));
                    Name = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Name, 0, u"="_wref.get()));
                }
                Child = AddChildBlock(Name);
                Child->UseSortedIndex = ChildSorted;
                Name = EC_Str::TrimWideString(EC_Str::ExtractDelimitedRangeW(Text, 1, PartCount - 1, u"{"_wref.get()));
                Child->ParseTextBuffer(Buf, Name, AnsiText, PreserveComments);
                if (IncludeFile != u"") {
                    Child->LoadFromTextFileWithEncodingProbe(IncludeFile.pchar(), false);
                }
            } else {
                if (EC_Str::CountDelimitedPartsW(Text, u"}"_wref.get()) > 1) {
                    break;
                }
                PartCount = EC_Str::CountDelimitedPartsW(Text, u"="_wref.get());
                if (PartCount > 1) {
                    Name = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Text, 0, u"="_wref.get()));
                    IncludeFile = EC_Str::ExtractDelimitedRangeW(Text, 1, PartCount - 1, u"="_wref.get());
                    if (PreserveComments) {
                        AddParam(Name, IncludeFile)->Comment = Comment;
                    } else {
                        AddParam(Name, IncludeFile);
                    }
                } else if (PreserveComments) {
                    Entry = AddEntry();
                    Entry->ItemType = bpkText;
                    Entry->Comment = Comment;
                }
            }
            Text = pas::WideString();
        }
    }

    void TBlockParEC::LoadFromTextBufferWithEncodingProbe(EC_Buf::TBufEC* Buf, std::uint8_t PreserveComments) {
        if (Buf->DataSize - Buf->Position <= 2) {
            return;
        }
        if (EC_Buf::TBufEC_GetWord(Buf) != 0x0000feff) {
            Buf->SetPosition(Buf->Position - 2);
            ParseTextBuffer(Buf, u""_wref.get(), true, PreserveComments);
        } else {
            ParseTextBuffer(Buf, u""_wref.get(), false, PreserveComments);
        }
    }

    void TBlockParEC::LoadFromTextFileWithEncodingProbe(char16_t* FileName, std::uint8_t PreserveComments) {
        EC_Buf::TBufEC* Buf = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        {
            try {
                Buf->LoadFromWideFilePath(FileName);
                LoadFromTextBufferWithEncodingProbe(Buf, PreserveComments);
            } catch (...) {
                pas::free(Buf);
                throw;
            }
            pas::free(Buf);
        }
    }

    void TBlockParEC::MergeFrom(TBlockParEC* Source) {
        TBlockParElEC* Incoming{};
        TBlockParElEC* Removed{};
        TBlockParElEC* Cursor{};
        TBlockParElEC* Existing{};
        TBlockParEC* Child{};
        std::int32_t Occurrence{};
        std::int32_t Seen{};
        Incoming = Source->FirstEntry;
        while (Incoming != nullptr) {
            if (Incoming->ItemType != bpkText) {
                if (Incoming->ItemType == bpkString) {
                    Cursor = FirstEntry;
                    while (Cursor != nullptr) {
                        Removed = Cursor;
                        Cursor = Cursor->Next;
                        if (Removed->ItemType == Incoming->ItemType && Removed->Name == Incoming->Name) {
                            if (UseSortedIndex) {
                                RemoveFromSortedIndex(Removed);
                            }
                            DeleteEntry(Removed);
                        }
                    }
                } else if (Incoming->ItemType == bpkBlock) {
                    Occurrence = 0;
                    Cursor = Source->FirstEntry;
                    while (Cursor != Incoming) {
                        if (Cursor->ItemType == bpkBlock && Cursor->Name == Incoming->Name) {
                            ++Occurrence;
                        }
                        Cursor = Cursor->Next;
                    }
                    ++Occurrence;
                    Seen = 0;
                    Existing = FirstEntry;
                    while (Existing != nullptr) {
                        if (Existing->ItemType == bpkBlock && Existing->Name == Incoming->Name) {
                            ++Seen;
                        }
                        if (Seen == Occurrence) {
                            break;
                        }
                        Existing = Existing->Next;
                    }
                    if (Seen == Occurrence) {
                        Child = Existing->ChildBlock;
                        Child->MergeFrom(Incoming->ChildBlock);
                    } else {
                        Child = AddChildBlock(Incoming->Name);
                        Child->CopyFrom(Incoming->ChildBlock);
                    }
                }
            }
            Incoming = Incoming->Next;
        }
        Incoming = Source->FirstEntry;
        while (Incoming != nullptr) {
            if (Incoming->ItemType == bpkString) {
                AddParam(Incoming->Name, Incoming->StringValue);
            }
            Incoming = Incoming->Next;
        }
    }

    pas::WideString TBlockParEC::ConcatenateValues() {
        pas::WideString Result{};
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, GetEntryCount() - 1); cpp_range.next(i); ) {
            if (GetEntryKindByIndex(i) == bpkBlock) {
                Result = pas::concat_wide({Result, u"{", GetEntryBlockByIndex(i)->ConcatenateValues(), u"}"});
            } else {
                Result = pas::concat_wide({Result, GetEntryStringByIndex(i)});
            }
        }
        return Result;
    }

    void TBlockParEC::LoadFromDecodedBuffer(EC_Buf::TBufEC* Buf) {
        std::int32_t i{};
        TBlockParElEC* Entry{};
        Clear();
        UseSortedIndex = EC_Buf::TBufEC_GetBoolean(Buf);
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Buf);
        if (UseSortedIndex) {
            SortedEntryCount = Count;
            SortedEntries.set_length(Count);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            Entry = AddEntry();
            if (UseSortedIndex) {
                Entry->GroupIndex = EC_Buf::TBufEC_GetInt32(Buf);
                Entry->GroupCount = EC_Buf::TBufEC_GetInt32(Buf);
            }
            Entry->ItemType = static_cast<TBlockParKind>(EC_Buf::TBufEC_GetByte(Buf));
            Entry->Name = Buf->ReadWideString();
            if (Entry->ItemType == bpkString) {
                Entry->StringValue = Buf->ReadWideString();
                ++StringParamCount;
                if (UseSortedIndex) {
                    SortedEntries[i] = Entry;
                }
            } else if (Entry->ItemType == bpkBlock) {
                Entry->MakeChildBlock();
                if (UseSortedIndex) {
                    SortedEntries[i] = Entry;
                }
                ++ChildBlockCount;
                Entry->ChildBlock->LoadFromDecodedBuffer(Buf);
            }
        }
    }

    void TBlockParEC::LoadFromEncryptedDatFile(const pas::WideString& FileName) {
        EC_Buf::TBufEC* Buf{};
        std::uint32_t Crc{};
        std::int32_t Seed{};
        std::int32_t ByteCount{};
        std::uint32_t ExpectedOuter{};
        EC_File::TFileEC* FileObj = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        FileObj->SetFileName(static_cast<pas::WideString>(FileName.pchar()));
        FileObj->AcquireReadHandle(false);
        std::uint32_t Position = FileObj->GetPointer();
        FileObj->ReadBuffer(&ByteCount, static_cast<std::int32_t>(sizeof(std::int32_t)));
        FileObj->ReadBuffer(&ExpectedOuter, static_cast<std::int32_t>(sizeof(std::uint32_t)));
        ByteCount ^= BlockDatCrcKey1 ^ BlockDatCrcKey2;
        {
            std::uint32_t cpp_left_2 = FileObj->GetSize();
            std::uint32_t cpp_left = cpp_left_2 - FileObj->GetPointer();
            if (cpp_left == static_cast<std::uint32_t>(ByteCount)) {
                Buf = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                Buf->SetSize(ByteCount + 4);
                Position = FileObj->GetPointer();
                FileObj->ReadBuffer(static_cast<std::uint8_t*>(Buf->Data) + 4, Buf->DataSize - 4);
                Crc = Buf->ComputeCrc32Range(4, Buf->DataSize) ^ BlockDatCrcKey1;
                pas::store_unaligned<std::uint32_t>(static_cast<System::PCardinal>(Buf->Data), Crc);
                Crc = Buf->ComputeCrc32() ^ BlockDatCrcKey2;
                pas::free(Buf);
                if (Crc != ExpectedOuter) {
                    GR_Main::CCInterface->SetResourceChecksumFailed(true);
                }
            } else {
                GR_Main::CCInterface->SetResourceChecksumFailed(true);
            }
        }
        FileObj->SetPointer(Position, WindowsImports::FILE_BEGIN);
        {
            std::uint32_t cpp_left_3 = FileObj->GetSize();
            ByteCount = cpp_left_3 - FileObj->GetPointer();
        }
        FileObj->ReadBuffer(&Crc, static_cast<std::int32_t>(sizeof(std::uint32_t)));
        FileObj->ReadBuffer(&Seed, static_cast<std::int32_t>(sizeof(std::int32_t)));
        Seed ^= BlockDatSeedKey;
        Buf = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buf->SetSize(ByteCount - 4 - 4);
        FileObj->ReadBuffer(Buf->Data, Buf->DataSize);
        Buf->ApplyDatXorCipher(Seed);
        if (Buf->ComputeCrc32() == Crc) {
            Buf->ExpandZlibPayloadInPlace();
            Buf->SetPosition(0);
            LoadFromDecodedBuffer(Buf);
        }
        pas::free(Buf);
        pas::free(FileObj);
    }

    void TBlockParEC::p_destroy() {
        EC_BlockPar::TBlockParEC_Destroy(this);
    }

    void TBlockParElEC::p_destroy() {
        EC_BlockPar::TBlockParElEC_Destroy(this);
    }

} // namespace EC_BlockPar
