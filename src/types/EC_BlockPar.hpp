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
        void Clear();
        void CopyFrom(TBlockParEC* Source);
        TBlockParElEC* AddEntry();
        void DeleteEntry(TBlockParElEC* Entry);
        TBlockParElEC* FindEntryByPath(const pas::WideString& Path, std::uint8_t RaiseIfMissing);
        std::int32_t FindSortedNameRangeStartIndex(const pas::WideString& EntryName);
        std::int32_t PrepareSortedInsertion(TBlockParElEC* Entry);
        void InsertIntoSortedIndex(TBlockParElEC* Entry);
        void RemoveFromSortedIndex(TBlockParElEC* Entry);
        pas::WideString GetParamByPath(const pas::WideString& Path);
        pas::WideString GetParamByPathOrMarker(const pas::WideString& Path);
        std::int32_t CountParamsByPath(const pas::WideString& Path);
        TBlockParElEC* AddParam(const pas::WideString& ParamName, const pas::WideString& ParamValue);
        void SetParam(const pas::WideString& ParamName, const pas::WideString& ParamValue);
        void SetOrAddParam(const pas::WideString& ParamName, const pas::WideString& ParamValue);
        void DeleteParam(const pas::WideString& ParamName);
        void DeleteChildBlock(const pas::WideString& BlockName);
        pas::WideString GetParam(const pas::WideString& ParamName);
        pas::WideString GetParamOrMarker(const pas::WideString& ParamName);
        std::int32_t GetParamCount();
        std::int32_t CountParams(const pas::WideString& ParamName);
        pas::WideString GetParamValue(std::int32_t Index);
        pas::WideString GetParamName(std::int32_t Index);
        TBlockParEC* AddBlockByPath(const pas::WideString& Path);
        TBlockParEC* GetBlockByPath(const pas::WideString& Path);
        TBlockParEC* FindBlockByPath(const pas::WideString& Path);
        TBlockParEC* GetOrAddBlockByPath(const pas::WideString& Path);
        TBlockParEC* AddChildBlock(const pas::WideString& BlockName);
        TBlockParEC* GetBlock(const pas::WideString& BlockName);
        TBlockParEC* FindBlock(const pas::WideString& BlockName);
        std::int32_t GetBlockCount();
        std::int32_t CountBlocks(const pas::WideString& BlockName);
        TBlockParEC* GetBlockByIndex(std::int32_t Index);
        pas::WideString GetBlockNameByIndex(std::int32_t Index);
        std::int32_t GetEntryCount();
        TBlockParKind GetEntryKindByIndex(std::int32_t Index);
        TBlockParEC* GetEntryBlockByIndex(std::int32_t Index);
        pas::WideString GetEntryStringByIndex(std::int32_t Index);
        pas::WideString GetEntryNameByIndex(std::int32_t Index);
        void WriteWideText(EC_Buf::TBufEC* Dest, std::int32_t Indent, std::uint8_t Sorted);
        void WriteAnsiText(EC_Buf::TBufEC* Dest, std::int32_t Indent, std::uint8_t Sorted);
        void WriteTextBuffer(EC_Buf::TBufEC* Dest, std::uint8_t AnsiText, std::uint8_t Sorted);
        void SaveTextFile(char16_t* FileName, std::uint8_t AnsiText, std::uint8_t Sorted);
        void ParseTextBuffer(EC_Buf::TBufEC* Buf, const pas::WideString& InitialText, std::uint8_t AnsiText, std::uint8_t PreserveComments);
        void LoadFromTextBufferWithEncodingProbe(EC_Buf::TBufEC* Buf, std::uint8_t PreserveComments);
        void LoadFromTextFileWithEncodingProbe(char16_t* FileName, std::uint8_t PreserveComments);
        void MergeFrom(TBlockParEC* Source);
        pas::WideString ConcatenateValues();
        void LoadFromDecodedBuffer(EC_Buf::TBufEC* Buf);
        void LoadFromEncryptedDatFile(const pas::WideString& FileName);
        TBlockParElEC* FirstEntry;
        TBlockParElEC* LastEntry;
        std::int32_t EntryCount;
        std::int32_t StringParamCount;
        std::int32_t ChildBlockCount;
        std::uint8_t UseSortedIndex;
        std::uint8_t cpp_padding[3];
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
        void Clear();
        void MakeChildBlock();
        void CopyFrom(TBlockParElEC* Source);
        TBlockParElEC* Prev;
        TBlockParElEC* Next;
        TBlockParEC* OwnerBlock;
        TBlockParKind ItemType;
        pas::WideString Name;
        pas::WideString StringValue;
        pas::WideString Comment;
        TBlockParEC* ChildBlock;
        std::int32_t GroupIndex;
        std::int32_t GroupCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_BlockPar
