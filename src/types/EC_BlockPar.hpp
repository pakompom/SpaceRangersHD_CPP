#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_BlockPar {
    struct TBlockParEC;

    struct TBlockParElEC;

    enum TBlockParKind : std::uint32_t {
        bpkText = 0,
        bpkString = 1,
        bpkBlock = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TBlockParEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TBlockParEC, EC_Struct::TObjectEx, "TBlockParEC", 36)
        void p_destroy() override;
        // Preserves UseSortedIndex.
        void Clear();
        void CopyFrom(TBlockParEC* Source);
        // Caller must maintain kind counts and the sorted index.
        TBlockParElEC* AddEntry();
        // Frees Entry but leaves its sorted-index entry intact.
        void DeleteEntry(TBlockParElEC* Entry);
        // Dot, slash and backslash separate components; a :number suffix selects a zero-based occurrence.
        TBlockParElEC* FindEntryByPath(const pas::WideString& Path, std::uint8_t RaiseIfMissing);
        // Returns -1 when absent.
        std::int32_t FindSortedNameRangeStartIndex(const pas::WideString& EntryName);
        // Also updates duplicate-group metadata.
        std::int32_t PrepareSortedInsertion(TBlockParElEC* Entry);
        void InsertIntoSortedIndex(TBlockParElEC* Entry);
        void RemoveFromSortedIndex(TBlockParElEC* Entry);
        // Params are string entries; blocks have separate accessors. ByPath traverses
        // subtrees, while ParamName addresses a direct child. OrMarker returns
        // '[name]' or '[path]' when missing; ordinary getters raise instead.
        pas::WideString GetParamByPath(const pas::WideString& Path);
        // Returns a marker containing Path when lookup fails, including caught exceptions.
        pas::WideString GetParamByPathOrMarker(const pas::WideString& Path);
        // Creates missing intermediate subtrees.
        std::int32_t CountParamsByPath(const pas::WideString& Path);
        TBlockParElEC* AddParam(const pas::WideString& ParamName, const pas::WideString& ParamValue);
        // Only the first match is affected; raises when absent.
        void SetParam(const pas::WideString& ParamName, const pas::WideString& ParamValue);
        void SetOrAddParam(const pas::WideString& ParamName, const pas::WideString& ParamValue);
        // Only the first match is affected; raises when absent.
        void DeleteParam(const pas::WideString& ParamName);
        // Only the first match is affected; raises when absent.
        void DeleteChildBlock(const pas::WideString& BlockName);
        pas::WideString GetParam(const pas::WideString& ParamName);
        pas::WideString GetParamOrMarker(const pas::WideString& ParamName);
        std::int32_t GetParamCount();
        std::int32_t CountParams(const pas::WideString& ParamName);
        // GetParamValue/GetParamName take zero-based string-entry indexes.
        // Kind-specific indexes use sorted order only when all entries have that kind.
        pas::WideString GetParamValue(std::int32_t Index);
        pas::WideString GetParamName(std::int32_t Index);
        // Nested insertion updates the receiver's index and block count.
        TBlockParEC* AddBlockByPath(const pas::WideString& Path);
        // Raises when Path is absent or is not a block.
        TBlockParEC* GetBlockByPath(const pas::WideString& Path);
        TBlockParEC* FindBlockByPath(const pas::WideString& Path);
        TBlockParEC* GetOrAddBlockByPath(const pas::WideString& Path);
        TBlockParEC* AddChildBlock(const pas::WideString& BlockName);
        // Raises when absent.
        TBlockParEC* GetBlock(const pas::WideString& BlockName);
        TBlockParEC* FindBlock(const pas::WideString& BlockName);
        std::int32_t GetBlockCount();
        std::int32_t CountBlocks(const pas::WideString& BlockName);
        TBlockParEC* GetBlockByIndex(std::int32_t Index);
        pas::WideString GetBlockNameByIndex(std::int32_t Index);
        std::int32_t GetEntryCount();
        // Mixed-kind indexes use the sorted array only when it covers every entry.
        TBlockParKind GetEntryKindByIndex(std::int32_t Index);
        TBlockParEC* GetEntryBlockByIndex(std::int32_t Index);
        pas::WideString GetEntryStringByIndex(std::int32_t Index);
        pas::WideString GetEntryNameByIndex(std::int32_t Index);
        // Text writers append at Dest.Position. Sorted applies only with UseSortedIndex.
        // Uses four spaces per indentation level and CRLF line endings.
        void WriteWideText(EC_Buf::TBufEC* Dest, std::int32_t Indent, std::uint8_t Sorted);
        // Uses tabs for indentation and CRLF line endings.
        void WriteAnsiText(EC_Buf::TBufEC* Dest, std::int32_t Indent, std::uint8_t Sorted);
        // Wide output starts with a UTF-16LE BOM.
        void WriteTextBuffer(EC_Buf::TBufEC* Dest, std::uint8_t AnsiText, std::uint8_t Sorted);
        // Creates or truncates FileName.
        void SaveTextFile(char16_t* FileName, std::uint8_t AnsiText, std::uint8_t Sorted);
        // Text parsers append entries; they do not clear the existing tree.
        void ParseTextBuffer(EC_Buf::TBufEC* Buf, const pas::WideString& InitialText, std::uint8_t AnsiText, std::uint8_t PreserveComments);
        // Does nothing with at most two bytes remaining; otherwise consumes a UTF-16LE BOM or parses ANSI text.
        void LoadFromTextBufferWithEncodingProbe(EC_Buf::TBufEC* Buf, std::uint8_t PreserveComments);
        void LoadFromTextFileWithEncodingProbe(char16_t* FileName, std::uint8_t PreserveComments);
        // Replaces all same-name string parameters; matches duplicate child blocks by occurrence and merges them recursively.
        void MergeFrom(TBlockParEC* Source);
        // Omits names and wraps child values in braces; text-only entries are not accepted.
        pas::WideString ConcatenateValues();
        // Replaces existing contents; trusts sorted-group metadata from the stream.
        void LoadFromDecodedBuffer(EC_Buf::TBufEC* Buf);
        // An inner checksum mismatch leaves the tree unchanged.
        void LoadFromEncryptedDatFile(const pas::WideString& FileName);
        TBlockParElEC* FirstEntry;
        TBlockParElEC* LastEntry;
        std::int32_t EntryCount;
        std::int32_t StringParamCount;
        std::int32_t ChildBlockCount;
        std::uint8_t UseSortedIndex;
        std::uint8_t cpp_padding[3];
        // Delphi dynamic array; ordered by case-sensitive name, then kind.
        pas::DynArray<TBlockParElEC*> SortedEntries;
        std::int32_t SortedEntryCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TBlockParElEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TBlockParElEC, EC_Struct::TObjectEx, "TBlockParElEC", 44)
        void p_destroy() override;
        // Frees ChildBlock; links and index metadata remain unchanged.
        void Clear();
        // Replaces the owned child; caller must update owner counts and index.
        void MakeChildBlock();
        // Deep-copies ChildBlock; links and index metadata remain unchanged.
        void CopyFrom(TBlockParElEC* Source);
        TBlockParElEC* Prev;
        TBlockParElEC* Next;
        TBlockParEC* OwnerBlock;
        TBlockParKind ItemType;
        pas::WideString Name;
        pas::WideString StringValue;
        pas::WideString Comment;
        TBlockParEC* ChildBlock;
        // Sorted entries group equal names and kinds; GroupCount is valid at the head.
        std::int32_t GroupIndex;
        std::int32_t GroupCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_BlockPar
